#ifndef ADAPT_EVALUATOR_LAYERFUNCNODE_H
#define ADAPT_EVALUATOR_LAYERFUNCNODE_H

#include <vector>
#include <string>
#include <functional>
#include <ranges>
#include <OpenADAPT/Utility/Verbose.h>
#include <OpenADAPT/Common/Bpos.h>
#include <OpenADAPT/Evaluator/FieldNode.h>
#include <OpenADAPT/Evaluator/FuncNode.h>
#include <OpenADAPT/Joint/LayerInfo.h>

namespace adapt
{

namespace eval
{

namespace detail
{
template <class Trav>
void DeleteTrav(void* ptr)
{
	auto* t = static_cast<Trav*>(ptr);
	delete t;
}
struct SpMemFuncs
{
	template <class Trav>
	static void* New()
	{
		return new Trav{};
	}
	template <class Trav>
	static void Copy(const void* ptr, void* dst)
	{
		auto* t = static_cast<const Trav*>(ptr);
		auto* d = static_cast<Trav*>(dst);
		//*d = *t;
		d->CopyFrom(*t);

	}
	template <class Trav>
	static void Delete(void* ptr)
	{
		auto* t = static_cast<Trav*>(ptr);
		delete t;
	}

	using NewFunc = void* (*)();
	using CopyFunc = void(*)(const void*, void*);
	using DeleteFunc = void(*)(void*);

	NewFunc m_new = nullptr;
	CopyFunc m_copy = nullptr;
	DeleteFunc m_delete = nullptr;
};

template <class DerivedNode, class Func, class Node, class Depth, class Up, class Cond = EmptyClass>
struct LayerFuncBase
{
	using Container = Node::Container;
	using Traverser = Container::Traverser;
	using ConstTraverser = Container::ConstTraverser;
	using RetType = Func::RetType;
	static constexpr bool HasCondition = !std::is_same_v<Cond, EmptyClass>;
	static constexpr bool IsRtti = rtti_node<Node> || (HasCondition && rtti_node<Cond>);
	static constexpr bool IsCtti = ctti_node<Node> && (!HasCondition || ctti_node<Cond>);
	static constexpr RankType MaxRank = Container::MaxRank;
	static constexpr bool IsJoinedContainer = MaxRank > 0;

	LayerFuncBase()
	{}

	template <class Node_>
		requires std::is_same_v<std::remove_cvref_t<Node_>, Node>
	LayerFuncBase(Node_&& node, Up up)
		: m_node(std::forward<Node_>(node)), m_up(up)
	{
		//m_traverserはここで確保しておく必要がある。
		//Init関数はEvaluateから呼び出されるためconstである必要があるが、
		//その場合、Init関数中でm_traverserを確保できない。
		//ただし、ここで予め確保しておけば、m_traverserの指し示す先に対して
		//constのままでInitを呼ぶことはできる。
		//m_traverser = std::make_unique<Traverser>();
		//->Rttiの方はここで確保するのが非常に難しいので、やっぱりmutableにして対処する。
	}
	template <class Node_, class Cond_>
		requires std::is_same_v<std::remove_cvref_t<Node_>, Node>
	LayerFuncBase(Node_&& node, Cond_&& cond, Up up)
		: m_cond(std::forward<Cond_>(cond)), m_node(std::forward<Node_>(node)), m_up(up)
	{}
	~LayerFuncBase()
	{
		if (m_traverser) (*m_sp_func->m_delete)(m_traverser);
	}

	LayerFuncBase(const LayerFuncBase& n)
		: LayerFuncBase()
	{
		*this = n;
	}
	LayerFuncBase(LayerFuncBase&& n) noexcept
		: LayerFuncBase()
	{
		*this = std::move(n);
	}
	LayerFuncBase& operator=(const LayerFuncBase& x)
	{
		//if (x.m_traverser != nullptr)
		//	PrintWarning("The copy constructor of the CttiLayerFuncNode has been called, although the node had already been initialized.");
		m_cond = x.m_cond;
		m_node = x.m_node;
		m_func = x.m_func;
		if (x.m_traverser)
		{
			//この時点でx.m_traverserがnullptrでないことは普通はしないと思うが、一応許す。
			if (!m_traverser) m_traverser = (*x.m_sp_func->m_new)();
			(*x.m_sp_func->m_copy)(x.m_traverser, m_traverser);
		}
		m_depth = x.m_depth;
		m_up = x.m_up;
		m_has_outer = x.m_has_outer;
		m_sp_func = x.m_sp_func;
		return *this;
	}
	LayerFuncBase& operator=(LayerFuncBase&& x) noexcept
	{
		m_cond = std::move(x.m_cond);
		m_node = std::move(x.m_node);
		m_func = std::move(x.m_func);
		if (m_traverser) (*m_sp_func->m_delete)(m_traverser);
		m_traverser = x.m_traverser; x.m_traverser = nullptr;
		m_depth = x.m_depth; x.m_depth = {};
		m_up = x.m_up; x.m_up = {};
		m_sp_func = x.m_sp_func; x.m_sp_func = nullptr;
		return *this;
	}

private:
	void InitISFunc()
	{
		//is系関数の場合、非常に多重計算が発生しやすい。
		//例えばisgreatest(math+english+japanese)と英国数で最高点を取ったときを選び出そうとすると、
		//単純に走らせると、前期後期中間期末、計4回も、ループによる最大値の取得と一致確認をする必要がある。
		//このような無駄なループ計算を防ぐ必要がある。

		//もし自身の深度以上のouterを持つ場合、直前の計算と一致する可能性はほぼない。
		//ので、その場合はループ防止機能自体を無効にする。
		//Cttiの場合はOuterInfoがtrue_typeまたはfalse_typeになっておりコンパイル時に分かるが、
		//Rttiの場合は実行時にしかわからないのでここで判定する。
		if constexpr (!IsCtti)
		{
			bool has_outer = false;
			LayerInfo<MaxRank> dflt(m_node.GetJointLayerArray());
			for (DepthType d = 0; d <= m_depth; ++d)
				if (dflt != m_node.GetLayerInfo(m_node.GetJointLayerArray(), d)) has_outer = true;
			if constexpr (HasCondition)
			{
				//条件がある場合は、条件の深度も考慮する。
				if (!has_outer)
				{
					for (DepthType d = 0; d <= m_depth; ++d)
						if (dflt != m_cond.GetLayerInfo(m_cond.GetJointLayerArray(), d)) has_outer = true;
				}
			}
			m_has_outer = has_outer;
		}
		if (!m_has_outer)
		{
			auto trav = m_node.GetLayerInfo();
			auto& cand = m_func.GetCand();
			if constexpr (IsJoinedContainer)
			{
				//t.ResetJBpos(cand, trav);
				cand.Init(MaxRank);
				for (RankType r = 0; r <= MaxRank; ++r)
				{
					const auto& ili = trav.GetInternal(r);
					if (ili.IsDisabled()) break;
					cand[r].Init(ili.GetTravLayer(), std::numeric_limits<BindexType>::max());
				}
				RankType tr = trav.GetActiveRank();
				LayerType tl = trav.GetInternal(tr).GetTravLayer();
				trav.Fix(m_up);
				RankType fr = trav.GetActiveRank();
				LayerType fl = trav.GetInternal(fr).GetFixedLayer();
				m_func.SetRankLayer(fr, fl, tr, tl);
			}
			else
			{
				//t.ResetBpos(cand, trav.GetTravLayer());
				cand.Init(trav.GetTravLayer(), std::numeric_limits<BindexType>::max());
				LayerType tl = trav.GetInternal(RankType(0)).GetTravLayer();
				trav.Fix(m_up);
				LayerType fl = trav.GetInternal(RankType(0)).GetFixedLayer();
				m_func.SetRankLayer(fl, tl);
			}
		}
	}

	//outer_tを伴うInitは、Containerを引数に呼ばれることはない。
	template <any_traverser Trav>
	void Init_impl(const Trav& t, const std::vector<std::tuple<const void*, const Bpos*, bool>>& outer_t)
	{
		if (m_traverser) (*m_sp_func->m_delete)(m_traverser);
		m_sp_func = &SpMemFuncs_value<Trav>;
		Trav* ptr = nullptr;
		if constexpr (IsJoinedContainer)
		{
			auto lnode = m_node.GetLayerInfo();
			auto fix = lnode;
			fix.Fix(m_up);
			ptr = new Trav(t->GetContainer(), fix);
		}
		else
		{
			LayerType trav = m_node.GetLayerInfo().GetTravLayer();
			ptr = new Trav(trav - m_up, t->GetContainer(), trav);
		}
		m_traverser = ptr;

		{
			//m_depthはIncreaseDepthによって増加させることにする。
			//if constexpr (!IsSame_XN<DepthConstant, Depth>::value)
			//else m_depth = (DepthType)outer_t.size();
			std::vector<std::tuple<const void*, const Bpos*, bool>> o;
			o.reserve(outer_t.size() + 1);
			o.insert(o.end(), outer_t.begin(), outer_t.end());
			o.push_back(std::tuple<const void*, const Bpos*, bool>{ &t, nullptr, false });
			m_node.Init(*ptr, o);
			if constexpr (HasCondition)
			{
				m_cond.Init(*ptr, o);
				if (m_node.GetLayerInfo() < m_cond.GetLayerInfo())
					throw InvalidArg("The condition layer is lower than the node layer.");
			}
		}

		if constexpr (!Func::IsRaisingFunc) InitISFunc();
	}
	void Init_impl(const Container& s, const Bpos* bpos)
	{
		if (m_traverser) (*m_sp_func->m_delete)(m_traverser);
		m_sp_func = &SpMemFuncs_value<ConstTraverser>;
		ConstTraverser* ptr = nullptr;
		if constexpr (IsJoinedContainer)
		{
			auto lnode = m_node.GetLayerInfo();
			auto fix = lnode;
			fix.Fix(m_up);
			ptr = new ConstTraverser(s, fix);
		}
		else
		{
			LayerType trav = m_node.GetLayerInfo().GetTravLayer();
			ptr = new ConstTraverser(trav - m_up, s, trav);
		}
		m_traverser = ptr;

		{
			std::vector<std::tuple<const void*, const Bpos*, bool>> o;
			o.reserve(1);
			o.push_back(std::tuple<const void*, const Bpos*, bool>{ &s, bpos, true });
			m_node.Init(*ptr, o);
			if constexpr (HasCondition)
			{
				m_cond.Init(*ptr, o);
				if (m_node.GetLayerInfo() < m_cond.GetLayerInfo())
					throw InvalidArg("The condition layer is lower than the node layer.");
			}
		}


		if constexpr (!Func::IsRaisingFunc) InitISFunc();
	}
public:
	void Init(const Traverser& t, const std::vector<std::tuple<const void*, const Bpos*, bool>>& outer_t)
	{
		Init_impl(t, outer_t);
	}
	void Init(const Traverser& t)
	{
		Init_impl(t, {});
	}
	void Init(const ConstTraverser& t, const std::vector<std::tuple<const void*, const Bpos*, bool>>& outer_t)
	{
		Init_impl(t, outer_t);
	}
	void Init(const ConstTraverser& t)
	{
		Init_impl(t, {});
	}
	void Init(const Container& s, const Bpos& bpos)
	{
		Init_impl(s, &bpos);
	}
	void Init(const Container& s)
	{
		Init_impl(s, nullptr);
	}
	void Init()
	{
		if (m_traverser) (*m_sp_func->m_delete)(m_traverser);
		m_traverser = nullptr;
		m_sp_func = nullptr;
		m_has_outer = {};
		m_node.Init();
		if constexpr (HasCondition) m_cond.Init();
		m_func = {};
	}

	static constexpr DepthType GetDepth() requires IsCtti { return Depth{}; }
	DepthType GetDepth() const requires (!IsCtti) { return m_depth; }

	static constexpr JointLayerArray<MaxRank> GetJointLayerArray() requires IsCtti { return Node::GetJointLayerArray(); }
	JointLayerArray<MaxRank> GetJointLayerArray() const requires (!IsCtti) { return m_node.GetJointLayerArray(); }
	static constexpr LayerInfo<MaxRank> GetLayerInfo() requires IsCtti { return GetLayerInfo(GetJointLayerArray()); }
	LayerInfo<MaxRank> GetLayerInfo() const requires (!IsCtti) { return GetLayerInfo(GetJointLayerArray()); }
	static constexpr LayerInfo<MaxRank> GetLayerInfo(LayerInfo<MaxRank> eli) requires IsCtti
	{
		auto no_outer = Node::GetLayerInfo(eli);
		//inner scopeの中から、depthの一致するfieldの情報を取り出す。
		auto outer = Node::GetLayerInfo(eli, GetDepth());
		if constexpr (HasCondition) outer = std::max(outer, Cond::GetLayerInfo(eli, GetDepth()));
		//上昇関数の上昇分。
		if constexpr (Func::IsRaisingFunc) no_outer.Raise(Up{});
		//これと通常のlayerを比較し、大きい方を返す。
		return std::max(no_outer, outer);
	}
	LayerInfo<MaxRank> GetLayerInfo(LayerInfo<MaxRank> eli) const requires (!IsCtti)
	{
		auto no_outer = m_node.GetLayerInfo(eli);
		//inner scopeの中から、depthの一致するfieldの情報を取り出す。
		auto outer = m_node.GetLayerInfo(eli, GetDepth());
		if constexpr (HasCondition) outer = std::max(outer, m_cond.GetLayerInfo(eli, GetDepth()));
		//上昇関数の上昇分。
		if constexpr (Func::IsRaisingFunc) no_outer.Raise(m_up);
		//これと通常のlayerを比較し、大きい方を返す。
		return std::max(no_outer, outer);
	}
	static constexpr LayerInfo<MaxRank> GetLayerInfo(LayerInfo<MaxRank> eli, DepthType depth) requires IsCtti
	{
		auto outer = Node::GetLayerInfo(eli, depth);
		if constexpr (HasCondition) outer = std::max(outer, Cond::GetLayerInfo(eli, depth));
		return outer;
	}
	LayerInfo<MaxRank> GetLayerInfo(LayerInfo<MaxRank> eli, DepthType depth) const requires (!IsCtti)
	{
		auto outer = m_node.GetLayerInfo(eli, depth);
		if constexpr (HasCondition) outer = std::max(outer, m_cond.GetLayerInfo(eli, depth));
		return outer;
	}

	static constexpr LayerType GetLayer() requires IsCtti { return GetLayerInfo().GetTravLayer(); }
	LayerType GetLayer() const requires (!IsCtti) { return GetLayerInfo().GetTravLayer(); }

protected:
	template <class TravOrStor, class Bpos_>
	const RetType& Evaluate_impl(const TravOrStor& t, const Bpos_& bpos) const
	{
		using ConstTraverser = Container::ConstTraverser;
		using Trav = std::conditional_t<any_traverser<TravOrStor>, TravOrStor, ConstTraverser>;
		Trav* trav = static_cast<Trav*>(m_traverser);

		if constexpr (!Func::IsRaisingFunc)
		{
			//is系関数の場合、重複計算を避けるためにtraverserの一致を確認する。
			//m_has_outerがtrueの場合はouterを持っているため、一致する可能性はほぼないものと見なす。
			if (!m_has_outer)
			{
				if constexpr (any_traverser<TravOrStor>)
				{
					bool match = m_func.MatchFixed(t);
					if (match) return m_func.MatchTrav(t);
				}
				else if constexpr (!std::same_as<Bpos_, std::nullptr_t>)
				{
					bool match = m_func.MatchFixed(t, bpos);
					if (match) return m_func.MatchTrav(t, bpos);
				}
				else
				{
					//なお、Container + nullptrの場合、is系関数であることがありえないことに加え、
					//MatchFixedがコンパイルエラーになる。
					//明らかに呼び出してはいけない関数を呼び出しているので、InvalidArgを投げる。
					throw InvalidArg("is-function must NOT be called with only a single container argument.");
				}
			}
		}

		m_func.Init();
		bool b = true;
		if constexpr (any_traverser<TravOrStor>) b = trav->AssignPartially(t);
		else if constexpr (!std::same_as<Bpos_, std::nullptr_t>) b = trav->AssignPartially(bpos);
		else trav->MoveToBegin();

		//ここでbがfalseということはありえない、と言いたいが、絶対ではない気がする。
		//例えば、Rank==0までが有効なouterによって、ujoint==fixedであるようなRank==1のTraverserを走査するとき。
		//Rank==0へのアクセス時点では問題なくても、上のAssignPartiallyでRank==1にJoinして初めて失敗する可能性はある。
		//この場合、この階層関数そのものの処理をキャンセルする。
		if (!b) throw NoElements();

		[[maybe_unused]] auto eval_cond = []([[maybe_unused]] const Cond& cond, [[maybe_unused]] const Trav* trav)
		{
			if constexpr (HasCondition)
			{
				if constexpr (rtti_node<Cond>) return cond.Evaluate(*trav, Number<DFieldInfo::I08>{});
				else return cond.Evaluate(*trav);
			}
		};
		auto eval_node = [](const Node& node, const Trav* trav)
		{
			if constexpr (rtti_node<Node>) return node.Evaluate(*trav, Number<DFieldInfo::GetSameSizeTagType<typename Func::ArgType>()>{});
			else return node.Evaluate(*trav);
		};

			//条件指定がある場合、最初の要素が条件を満たすとは限らないので、
			//条件を満たす要素が見つかるまでループする必要がある。
		for (; !trav->IsEnd(); ++(*trav))
		{
			try
			{
				if constexpr (HasCondition)
					if (!eval_cond(m_cond, trav)) continue;
				m_func.First(eval_node(m_node, trav), *trav);
				++(*trav);
				break;
			}
			catch (const NoElements&) {}
			catch (const JointError&) {}
		}
		for (; !trav->IsEnd(); ++(*trav))
		{
			try
			{
				if constexpr (HasCondition) if (!eval_cond(m_cond, trav)) continue;
				m_func.Exec(eval_node(m_node, trav), *trav);
			}
			catch (const NoElements&) {}
			catch (const JointError&) {}
		}
		if constexpr (Func::IsRaisingFunc) return m_func.GetResult();
		else
		{
			if constexpr (any_traverser<TravOrStor>) return m_func.MatchTrav(t);
			else if constexpr (!std::same_as<Bpos_, std::nullptr_t>) return m_func.MatchTrav(t, bpos);
			else throw InvalidArg("is-function must NOT be called with only a single container argument.");
		}
	}
public:

protected:
	template <class Dep>
	static constexpr auto FindOuter(Dep d)
	{
		//Typedの場合、ノードがouterを持つかどうかはコンパイル時には分からない。
		//例えばrttiをプロキシでtypedに見せかけている場合、完全に判定不能になる。
		//とはいえ、outer有無がstaticに決定されなかったとしても致命的なオーバーヘッドになるわけではないので、
		//typedの場合は動的に決定する。
		if constexpr (!IsCtti) return false;
		else
		{
			static_assert(IsSame_XN_v<DepthConstant, Dep>);
			if constexpr (d > Depth{}) return std::false_type{};
			else
			{
				//もしNodeがouterを持っている場合、初期化済みのLayerInfoが帰ってくるはずである。
				//outerを持たないのなら、返されるLayerInfoは未初期化の空状態。
				constexpr bool t = Node::GetLayerInfo(Node::GetJointLayerArray(), d).IsNotInitialized();
				if constexpr (!t) return std::true_type{};
				else if constexpr (HasCondition)
				{
					constexpr bool tt = Cond::GetLayerInfo(Cond::GetJointLayerArray(), d).IsNotInitialized();
					if constexpr (!tt) return std::true_type{};
					else return FindOuter(d + 1_depth);
				}
				else return FindOuter(d + 1_depth);
			}
		}
	}

	[[no_unique_address]] Cond m_cond;//Condは何らかのNodeもしくはEmptyClass。EmptyClassのときは条件未指定なので考慮しなくてよい。
	Node m_node;
	mutable Func m_func;//Funcはメンバ変数として持っておく必要がある。例えばFuncの結果がstd::stringのようなnon-trivialな型であった場合、バッファの代わりになる。

	void* m_traverser = nullptr;//自身が管理する、innerを走査するためのtraverser。

	//Depthは自身の深度、Upは走査層と固定層の差だが、Rttiの場合はDepthTypeとLayerTypeに、Cttiの場合はDepthConstantとLayerConstantになる。
	//Cttiの場合は戻り値層をstaticに決定する必要があるので、このような仕様になっている。
	[[no_unique_address]] Depth m_depth = {};//このdepthは、自身の外側で見た深度。自身が管理するm_traverserの深度より一つ上。自身がfunc1(func2(...))におけるfunc2だとすれば、深度は1。
	[[no_unique_address]] Up m_up = {};//上昇値ではない。ので、Is系でも0にはならない。
	//Matching<> m_matching;

	using OuterInfo = decltype(FindOuter(Depth{}));//rttiの場合はbool、cttiでouterを持たない場合はfalse_type、outerを保つ場合はtrue_type。
	[[no_unique_address]] OuterInfo m_has_outer = {};
	template <class Trav>
	inline static constexpr detail::SpMemFuncs SpMemFuncs_value = { &detail::SpMemFuncs::New<Trav>, &detail::SpMemFuncs::Copy<Trav>, &detail::SpMemFuncs::Delete<Trav>};
	const detail::SpMemFuncs* m_sp_func = nullptr;
};

}

//--------Ctti--------

template <class Func_, class Node_, RankType FixRank, DepthType Depth, LayerType Up, class Cond_>
struct CttiLayerFuncNode<Func_, Node_, FixRank, DepthConstant<Depth>, LayerConstant<Up>, Cond_>
	: public detail::LayerFuncBase<CttiLayerFuncNode<Func_, Node_, FixRank, DepthConstant<Depth>, LayerConstant<Up>, Cond_>,
								   Func_, Node_, DepthConstant<Depth>, LayerConstant<Up>, Cond_>,
	  public detail::CttiMethods<CttiLayerFuncNode<Func_, Node_, FixRank, DepthConstant<Depth>, LayerConstant<Up>, Cond_>, std::add_const_t>
{
	using Func = Func_;
	using Node = Node_;
	using Base = detail::LayerFuncBase<CttiLayerFuncNode<Func_, Node_, FixRank, DepthConstant<Depth>, LayerConstant<Up>, Cond_>,
									   Func_, Node_, DepthConstant<Depth>, LayerConstant<Up>, Cond_>;
	using RetType = typename Func::RetType;
	using Container = typename Node::Container;
	using Traverser = Container::Traverser;
	using ConstTraverser = Container::ConstTraverser;

	static constexpr RankType MaxRank = Container::MaxRank;

	using Base::Base;

	auto IncreaseDepth() const&
	{
		auto node = this->m_node.IncreaseDepth();
		using Ret = CttiLayerFuncNode<Func_, decltype(node), FixRank, DepthConstant<Depth + 1>, LayerConstant<Up>>;
		return Ret(node, {});
	}
	auto IncreaseDepth() &&
	{
		auto n = std::move(*this).m_node.IncreaseDepth();
		using Ret = CttiLayerFuncNode<Func_, decltype(n), FixRank, DepthConstant<Depth + 1>, LayerConstant<Up>>;
		return Ret(std::move(n), {});
	}

	const RetType& Evaluate(const Container& t) const
	{
		return Base::Evaluate_impl(t, nullptr);
	}
	const RetType& Evaluate(const Container& s, const Bpos& bpos) const
	{
		return Base::Evaluate_impl(s, bpos);
	}
	const RetType& Evaluate(const Traverser& t) const
	{
		return Base::Evaluate_impl(t, nullptr);
	}
	const RetType& Evaluate(const ConstTraverser& t) const
	{
		return Base::Evaluate_impl(t, nullptr);
	}

	const RetType& operator()(const Container& t)
	{
		if (!Base::m_traverser) [[unlikely]]
			Base::Init(t);
		return Base::Evaluate_impl(t, nullptr);
	}
	const RetType& operator()(const Traverser& t)
	{
		if (!Base::m_traverser) [[unlikely]]
			Base::Init(t);
		return Base::Evaluate_impl(t, nullptr);
	}
	const RetType& operator()(const ConstTraverser& t)
	{
		if (!Base::m_traverser) [[unlikely]]
			Base::Init(t);
		return Base::Evaluate_impl(t, nullptr);
	}
	const RetType& operator()(const Container& s, const Bpos& bpos)
	{
		if (!Base::m_traverser) [[unlikely]]
			Base::Init(s, bpos);
		return Base::Evaluate_impl(s, bpos);
	}

};


namespace detail
{

//--------Rtti--------

template <class Func, class Container, class Node, class Cond = EmptyClass,
		  FieldType Type = DFieldInfo::GetSameSizeTagType<typename Func::RetType>(),
		  class Indices = std::make_index_sequence<Container::MaxRank + 1>>
struct RttiLayerFuncNode_impl;
template <class Func_, class Container_, class Node_, class Cond_, FieldType Type, size_t ...Indices>
struct RttiLayerFuncNode_impl<Func_, Container_, Node_, Cond_, Type, std::index_sequence<Indices...>>
	: public RttiFuncNode_base<Container_>,
	  public detail::LayerFuncBase<RttiLayerFuncNode_impl<Func_, Container_, Node_, Cond_, Type, std::index_sequence<Indices...>>, Func_, Node_, DepthType, LayerType, Cond_>
{
	using Func = Func_;
	using Node = Node_;
	using Base = detail::LayerFuncBase<RttiLayerFuncNode_impl<Func_, Container_, Node_, Cond_, Type, std::index_sequence<Indices...>>, Func_, Node_, DepthType, LayerType, Cond_>;
	using RetType = DFieldInfo::TagTypeToValueType<Type>;
	using Container = Container_;
	using Traverser = Container::Traverser;
	using ConstTraverser = Container::ConstTraverser;

	static constexpr RankType MaxRank = Container::MaxRank;

	using ValueType = DFieldInfo::TagTypeToValueType<Type>;

	//LayerFuncの戻り値は常に参照型ではあるが、
	//仮想関数RttiFuncNode_base::Evaluateをオーバーライドするときは
	//trivially_copyableのみ値型に変換しなければならない。
	using RetTypeRef = std::conditional_t<std::is_trivially_copyable_v<ValueType>, ValueType, const ValueType&>;

	using Base::Base;

	virtual ~RttiLayerFuncNode_impl() = default;

	virtual RttiFuncNode_base<Container>* Clone() const override
	{
		auto* res = new RttiLayerFuncNode_impl{};
		res->CopyFrom(*this);
		return res;
	}
	virtual void CopyFrom(const RttiFuncNode_base<Container>& that) override
	{
		const auto* x = dynamic_cast<const RttiLayerFuncNode_impl*>(&that);
		if (x == nullptr)
		{
			std::string message = "The type of the RttiFuncNode implementations are not the same.";
			PrintError(message);
			throw MismatchType(message);
		}
		Base::operator=(*x);
	}
	/*virtual void MoveFrom(RttiFuncNode_base<Container>&& that) override
	{
		auto* x = dynamic_cast<RttiLayerFuncNode_impl*>(&that);
		if (x == nullptr)
		{
			std::string message = "The type of the RttiFuncNode implementations are not the same.";
			PrintError(message);
			throw MismatchType(message);
		}
		Base::operator=(std::move(*x));
	}*/

	virtual void IncreaseDepth() override
	{
		this->m_node = std::move(this->m_node).IncreaseDepth();
		++this->m_depth;
	}

	virtual void Init(const Traverser& t, const std::vector<std::tuple<const void*, const Bpos*, bool>>& outer_t) override { Base::Init(t, outer_t); }
	virtual void Init(const ConstTraverser& t, const std::vector<std::tuple<const void*, const Bpos*, bool>>& outer_t) override { Base::Init(t, outer_t); }
	virtual void Init(const Container& s, const Bpos& bpos) override { Base::Init(s, bpos); }
	virtual void Init(const Container& s) override { Base::Init(s); }
	virtual void Init() override { Base::Init(); }

	//ここで返すdepthは引数ではなく、自分自身の深度。
	virtual DepthType GetDepth() const override
	{
		return Base::GetDepth();
	}

	virtual JointLayerArray<MaxRank> GetJointLayerArray() const override
	{
		return Base::GetJointLayerArray();
	}

	//与えられた深度に一致するplaceholderの最下層を、全ての深度の階層関数を通じて求める。
	virtual LayerInfo<MaxRank>
		GetLayerInfo() const override
	{
		return Base::GetLayerInfo();
	}
	virtual LayerInfo<MaxRank>
		GetLayerInfo(LayerInfo<MaxRank> eli) const override
	{
		return Base::GetLayerInfo(eli);
	}
	virtual LayerInfo<MaxRank>
		GetLayerInfo(LayerInfo<MaxRank> eli, DepthType depth) const override
	{
		return Base::GetLayerInfo(eli, depth);
	}
	//与えられた深度に一致するplaceholderの最下層を、全ての深度の階層関数を通じて求める。
	/*virtual LayerType GetInternalLayer() const override
	{
		return Base::GetInternalLayer();
	}
	virtual LayerType GetInternalLayer(DepthType depth) const override
	{
		return Base::GetInternalLayer(depth);
	}*/
	
	virtual RetTypeRef Evaluate(const Traverser& t, Number<Type>) const override
	{
		return Base::Evaluate_impl(t, nullptr);
	}
	virtual RetTypeRef Evaluate(const ConstTraverser& t, Number<Type>) const override
	{
		return Base::Evaluate_impl(t, nullptr);
	}
	virtual RetTypeRef Evaluate(const Container& s, Number<Type>) const override
	{
		return Base::Evaluate_impl(s, nullptr);
	}
	virtual RetTypeRef Evaluate(const Container& s, const Bpos& bpos, Number<Type>) const override
	{
		return Base::Evaluate_impl(s, bpos);
	}
	virtual FieldType GetType() const override
	{
		return DFieldInfo::GetSameSizeTagType<typename Func::RetType>();
	}
	/*template <class Trav>
	RetTypeRef operator()(const Trav& t)
	{
		if (!Base::m_traverser) [[unlikely]]
			Base::Init(t);
		return Base::Evaluate_impl(t);
	}*/
};

}


namespace detail
{

template <class ArgType_, class>
struct LayerFuncSize
{
	using ArgType = ArgType_;
	using RetType = int64_t;
	static constexpr bool IsRaisingFunc = true;
	LayerFuncSize() : m_result(0) {}
	void Init() { m_result = 0; }
	template <class Trav>
	void First(const ArgType&, const Trav&) { m_result = 1; }
	template <class Trav>
	void Exec(const ArgType&, const Trav&) { ++m_result; }
	const int64_t& GetResult() const { return m_result; }
private:
	int64_t m_result;
};

template <boolean_testable ArgType_, class>
struct LayerFuncExist
{
	using ArgType = ArgType_;
	using RetType = bool;
	static constexpr bool IsRaisingFunc = true;
	LayerFuncExist() : m_result(false) {}
	void Init() { m_result = false; }
	template <class Trav>
	void First(const ArgType& v, const Trav&) { m_result = (v); }
	template <class Trav>
	void Exec(const ArgType& v, const Trav&) { m_result = m_result || v; }
	const bool& GetResult() const { return m_result; }
private:
	bool m_result;
};
template <boolean_testable ArgType_, class>
struct LayerFuncCount
{
	using ArgType = ArgType_;
	using RetType = int64_t;
	static constexpr bool IsRaisingFunc = true;
	LayerFuncCount() : m_result(0) {}
	void Init() { m_result = 0; }
	template <class Trav>
	void First(const ArgType& v, const Trav&) { m_result = (int64_t)(v != 0); }
	template <class Trav>
	void Exec(const ArgType& v, const Trav&) { m_result += (int64_t)(v != 0); }
	const int64_t& GetResult() const { return m_result; }
private:
	int64_t m_result;
};
template <summable ArgType_, class>
struct LayerFuncSum
{
	//sumがStrなどに対して適用される可能性を考えると、
	//この実装はあまり良くないかもしれない。
	using ArgType = ArgType_;
	using RetType = decltype(std::declval<ArgType>() + std::declval<ArgType>());
	static constexpr bool IsRaisingFunc = true;
	LayerFuncSum() : m_result() {}
	void Init() { m_result = {}; }
	template <class Trav>
	void First(const ArgType& v, const Trav&) { m_result = v; }
	template <class Trav>
	void Exec(const ArgType& v, const Trav&) { m_result += v; }
	const RetType& GetResult() const { return m_result; }
private:
	RetType m_result;
};
template <class ArgType_, class>
	requires summable<ArgType_> && dividable<ArgType_>
struct LayerFuncMean
{
	using ArgType = ArgType_;
	using RetType = decltype((std::declval<ArgType>() + std::declval<ArgType>()) / std::declval<ArgType>());
	static constexpr bool IsRaisingFunc = true;
	LayerFuncMean() : m_result(), m_count(0) {}
	void Init() { m_result = {}; m_count = 0; }
	template <class Trav>
	void First(const ArgType& v, const Trav&) { m_result = v; ++m_count; }
	template <class Trav>
	void Exec(const ArgType& v, const Trav&) { m_result += v; ++m_count; }
	const RetType& GetResult()
	{
		if (m_count == 0) throw NoElements();
		//除算を行うとき、std::complexに対してはm_countの暗黙的な浮動小数点型への変換が行われるが、
		//このときmsvcはC4244を出して鬱陶しいので、分岐して回避する。
		if constexpr (IsSame_XT<std::complex, RetType>::value) m_result /= (typename RetType::value_type)m_count;
		else m_result /= RetType(m_count);
		return m_result;
	}
private:
	RetType m_result;
	int64_t m_count;
};
template <class ArgType_, class>
	requires std::integral<ArgType_> || std::floating_point<ArgType_>
struct LayerFuncDev
{
	using ArgType = ArgType_;
	using RetType = double;
	static constexpr bool IsRaisingFunc = true;
	LayerFuncDev() : m_result(), m_mean(), m_count(0) {}
	void Init() { m_result = {}; m_mean = {}; m_count = 0; }
	template <class Trav>
	void First(const ArgType& v, const Trav&) { m_result = (RetType)v * (RetType)v; m_mean += (RetType)v; ++m_count; }
	template <class Trav>
	void Exec(const ArgType& v, const Trav&) { m_result += (RetType)v * (RetType)v; m_mean += (RetType)v; ++m_count; }
	const RetType& GetResult()
	{
		if (m_count == 0) throw NoElements();
		RetType mean = (m_mean / RetType(m_count));
		RetType sqmean = (m_result /= RetType(m_count));
		RetType x = sqmean - mean * mean;
		if (x <= 0.) m_result = 0;
		else m_result = std::sqrt(x);
		return m_result;
	}
private:
	RetType m_result;
	RetType m_mean;
	int64_t m_count;
};
template <less_than_comparable ArgType_, class>
struct LayerFuncGreatest
{
	using ArgType = ArgType_;
	using RetType = ArgType;
	static constexpr bool IsRaisingFunc = true;
	LayerFuncGreatest() : m_result(), m_count(0) {}
	void Init() { m_result = {}; m_count = 0; }
	template <class Trav>
	void First(const ArgType& v, const Trav&) { m_result = v; ++m_count; }
	template <class Trav>
	void Exec(const ArgType& v, const Trav&) { m_result = std::max(v, m_result); ++m_count; }
	const RetType& GetResult() const
	{
		if (m_count == 0) throw NoElements{};
		return m_result;
	}
private:
	RetType m_result;
	int64_t m_count;
};
template <less_than_comparable ArgType_, class>
struct LayerFuncLeast
{
	using ArgType = ArgType_;
	using RetType = ArgType;
	static constexpr bool IsRaisingFunc = true;
	LayerFuncLeast() : m_result(), m_count(0) {}
	void Init() { m_result = {}; m_count = 0; }
	template <class Trav>
	void First(const ArgType& v, const Trav&) { m_result = v; ++m_count; }
	template <class Trav>
	void Exec(const ArgType& v, const Trav&) { m_result = std::min(v, m_result); ++m_count; }
	const RetType& GetResult() const
	{
		if (m_count == 0) throw NoElements{};
		return m_result;
	}
private:
	RetType m_result;
	int64_t m_count;
};
template <class ArgType_, class>
struct LayerFuncFirst
{
	using ArgType = ArgType_;
	using RetType = ArgType_;
	static constexpr bool IsRaisingFunc = true;
	LayerFuncFirst() : m_result(), m_found(false) {}
	void Init() { m_result = {}; m_found = false; }
	template <class Trav>
	void First(const ArgType& v, const Trav&) { m_result = v; m_found = true; }
	template <class Trav>
	void Exec(const ArgType&, const Trav&) {}
	const RetType& GetResult() const
	{
		if (!m_found) throw NoElements{};
		return m_result;
	}
private:
	RetType m_result;
	bool m_found;
};
template <class ArgType_, class>
struct LayerFuncLast
{
	using ArgType = ArgType_;
	using RetType = ArgType_;
	static constexpr bool IsRaisingFunc = true;
	LayerFuncLast() : m_result(), m_found(false) {}
	void Init() { m_result = {}; m_found = false; }
	template <class Trav>
	void First(const ArgType& v, const Trav&) { m_result = v; m_found = true; }
	template <class Trav>
	void Exec(const ArgType& v, const Trav&) { m_result = v; m_found = true; }
	const RetType& GetResult() const
	{
		if (!m_found) throw NoElements{};
		return m_result;
	}
private:
	RetType m_result;
	bool m_found;
};
template <boolean_testable ArgType_, class>
struct LayerFuncIndex
{
	using ArgType = ArgType_;
	using RetType = int64_t;
	static constexpr bool IsRaisingFunc = true;
	LayerFuncIndex() : m_result(-1), m_count(0) {}
	void Init() { m_result = -1; m_count = 0; }
	template <class Trav>
	void First(const ArgType& v, const Trav&) { if (v) m_result = m_count; ++m_count; }
	template <class Trav>
	void Exec(const ArgType& v, const Trav&) { if (m_result == -1 && v) m_result = m_count; ++m_count; }
	const RetType& GetResult() const
	{
		return m_result;
	}
private:
	RetType m_result;
	int64_t m_count;
};
template <boolean_testable ArgType_, class>
struct LayerFuncLastIndex
{
	using ArgType = ArgType_;
	using RetType = int64_t;
	static constexpr bool IsRaisingFunc = true;
	LayerFuncLastIndex() : m_result(-1), m_count(0) {}
	void Init() { m_result = -1; m_count = 0; }
	template <class Trav>
	void First(const ArgType& v, const Trav&) { if (v) m_result = m_count; ++m_count; }
	template <class Trav>
	void Exec(const ArgType& v, const Trav&) { if (v) m_result = m_count; ++m_count; }
	const RetType& GetResult() const
	{
		return m_result;
	}
private:
	RetType m_result;
	int64_t m_count;
};

template <class Container>
struct IsFuncBase
{
	static constexpr bool IsRaisingFunc = false;
	static constexpr bool IsJoinedContainer = (Container::MaxRank > 0);
	using Bpos_ = std::conditional_t<IsJoinedContainer, JBpos, Bpos>;
	void SetRankLayer(RankType fixedrank, LayerType fixedlayer, RankType travrank, LayerType travlayer)
	{
		m_matching.m_fixed_rank = fixedrank;
		m_matching.m_fixed_layer = fixedlayer;
		m_matching.m_trav_rank = travrank;
		m_matching.m_trav_layer = travlayer;
	}
	void SetRankLayer(LayerType fixedlayer, LayerType travlayer)
	{
		m_matching.m_fixed_layer = fixedlayer;
		m_matching.m_trav_layer = travlayer;
	}
	void Init() { m_executed = false; }
	template <class Trav>
	void GetBpos(const Trav& t)
	{
		if constexpr (IsJoinedContainer) t.GetJBpos(m_cand);
		else t.GetBpos(m_cand);
	}
	void SetExecuted() { m_executed = true; }
	bool IsExecuted() const { return m_executed; }
	Bpos_& GetCand() { return m_cand; }

	//戻り値はそのままEvaluateの戻り値になるため、const referenceにしておく必要がある。
	template <any_traverser Trav>
	const bool& MatchFixed(const Trav& t)
	{
		//IsExecuted() == trueを要求する。
		//というのも、m_candは初期値ではlayer==0かつindex最大値になっているので一致するはずはないが、
		//m_fixed_layer==-1の場合に限り、m_candの値にかかわらず一致扱いになってしまうため。
		if constexpr (IsJoinedContainer) m_result = IsExecuted() && t.MatchPartially(m_cand, m_matching.m_fixed_rank, m_matching.m_fixed_layer);
		else m_result = IsExecuted() && t.MatchPartially(m_cand, m_matching.m_fixed_layer);
		return m_result;
	}
	const bool& MatchFixed(const Container& s, const Bpos& bpos)
	{
		//IsExecuted() == trueを要求する。
		//というのも、m_candは初期値ではlayer==0かつindex最大値になっているので一致するはずはないが、
		//m_fixed_layer==-1の場合に限り、m_candの値にかかわらず一致扱いになってしまうため。
		if constexpr (IsJoinedContainer) m_result = IsExecuted() && m_cand.MatchPartially(bpos, s.GetJointLayers(), m_matching.m_fixed_rank, m_matching.m_fixed_layer);
		else m_result = IsExecuted() && m_cand.MatchPartially(bpos, m_matching.m_fixed_layer);
		return m_result;
	}
	template <any_traverser Trav>
	const bool& MatchTrav(const Trav& t)
	{
		if constexpr (IsJoinedContainer) m_result = t.MatchPartially(m_cand, m_matching.m_trav_rank, m_matching.m_trav_layer);
		else m_result = t.MatchPartially(m_cand, m_matching.m_trav_layer);
		return m_result;
	}
	const bool& MatchTrav(const Container& s, const Bpos& bpos)
	{
		if constexpr (IsJoinedContainer) m_result = m_cand.MatchPartially(bpos, s.GetJointLayers(), m_matching.m_trav_rank, m_matching.m_trav_layer);
		else m_result = m_cand.MatchPartially(bpos, m_matching.m_trav_layer);
		return m_result;
	}
private:
	template <class IsJoined = std::bool_constant<IsJoinedContainer>>
	struct Matching
	{
		LayerType m_fixed_layer = -1;
		LayerType m_trav_layer = -1;
	};
	template <class IsJoined>
		requires std::is_same_v<IsJoined, std::true_type>
	struct Matching<IsJoined>
	{
		RankType m_fixed_rank = -1;
		LayerType m_fixed_layer = -1;
		RankType m_trav_rank = -1;
		LayerType m_trav_layer = -1;
	};
	Matching<> m_matching;
	bool m_executed = false;
	bool m_result = false;
	Bpos_ m_cand;
};
template <class ArgType_, class Container>
struct LayerFuncIsFirst : public IsFuncBase<Container>
{
	using Base = IsFuncBase<Container>;
	using ArgType = ArgType_;
	using RetType = bool;
	void Init() { Base::Init(); }
	template <class Trav>
	void First(const ArgType&, const Trav& t) { Base::GetBpos(t); Base::SetExecuted(); }
	template <class Trav>
	void Exec(const ArgType&, const Trav&) {}
};
template <class ArgType_, class Container>
struct LayerFuncIsLast : public IsFuncBase<Container>
{
	using Base = IsFuncBase<Container>;
	using ArgType = ArgType_;
	using RetType = bool;
	void Init() { Base::Init(); }
	template <class Trav>
	void First(const ArgType&, const Trav& t) { Base::GetBpos(t); Base::SetExecuted(); }
	template <class Trav>
	void Exec(const ArgType&, const Trav& t) { Base::GetBpos(t); }
};
template <less_than_comparable  ArgType_, class Container>
struct LayerFuncIsGreatest : public IsFuncBase<Container>
{
	using Base = IsFuncBase<Container>;
	using ArgType = ArgType_;
	using RetType = bool;
	void Init() { Base::Init(); m_greatest = {}; }
	template <class Trav>
	void First(const ArgType& v, const Trav& t) { m_greatest = v; Base::GetBpos(t); Base::SetExecuted(); }
	template <class Trav>
	void Exec(const ArgType& v, const Trav& t) { if (v > m_greatest) { m_greatest = v; Base::GetBpos(t); } }
private:
	ArgType m_greatest = {};
};
template <less_than_comparable  ArgType_, class Container>
struct LayerFuncIsLeast : public IsFuncBase<Container>
{
	using Base = IsFuncBase<Container>;
	using ArgType = ArgType_;
	using RetType = bool;
	void Init() { Base::Init();  m_least = {}; }
	template <class Trav>
	void First(const ArgType& v, const Trav& t) { m_least = v; Base::GetBpos(t); Base::SetExecuted(); }
	template <class Trav>
	void Exec(const ArgType& v, const Trav& t) { if (v < m_least) { m_least = v; Base::GetBpos(t); } }
private:
	ArgType m_least = {};
};


template <template <class, class> class Func, FieldType Type, any_node Node,
	class Container = typename std::decay_t<Node>::Container,
	class FuncA = Func<DFieldInfo::TagTypeToValueType<Type>, Container>>
auto MakeRttiLayerFuncNode_impl(int, Node&& node, LayerType up)
	-> RttiFuncNode<Container>
{
	using NodeImpl = detail::RttiLayerFuncNode_impl<FuncA, Container, std::decay_t<Node>>;
	RttiFuncNode<Container> res;
	res.template Construct<NodeImpl>(std::forward<Node>(node).IncreaseDepth(), up);
	return res;
}

//テンプレート引数Funcのを引数型Typeが満たさない場合、FuncAの推定に失敗しSFINAEによってこちらが呼ばれる。
template <template <class, class> class Func, FieldType Type, any_node Node>
auto MakeRttiLayerFuncNode_impl(long, Node&&, LayerType)
	-> RttiFuncNode<typename std::decay_t<Node>::Container>
{
	throw MismatchType(DFieldInfo::GetTagTypeString<Type>().GetChar());
}

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4702)
#endif
template <template <class, class> class Func, any_node Node>
auto MakeRttiLayerFuncNode(Node&& node, LayerType up)
{
	if (node.IsI08())
		return MakeRttiLayerFuncNode_impl<Func, FieldType::I08>(1, std::forward<Node>(node), up);
	else if (node.IsI16())
		return MakeRttiLayerFuncNode_impl<Func, FieldType::I16>(1, std::forward<Node>(node), up);
	else if (node.IsI32())
		return MakeRttiLayerFuncNode_impl<Func, FieldType::I32>(1, std::forward<Node>(node), up);
	else if (node.IsI64())
		return MakeRttiLayerFuncNode_impl<Func, FieldType::I64>(1, std::forward<Node>(node), up);
	else if (node.IsF32())
		return MakeRttiLayerFuncNode_impl<Func, FieldType::F32>(1, std::forward<Node>(node), up);
	else if (node.IsF64())
		return MakeRttiLayerFuncNode_impl<Func, FieldType::F64>(1, std::forward<Node>(node), up);
	else if (node.IsC32())
		return MakeRttiLayerFuncNode_impl<Func, FieldType::C32>(1, std::forward<Node>(node), up);
	else if (node.IsC64())
		return MakeRttiLayerFuncNode_impl<Func, FieldType::C64>(1, std::forward<Node>(node), up);
	else if (node.IsStr())
		return MakeRttiLayerFuncNode_impl<Func, FieldType::Str>(1, std::forward<Node>(node), up);
	else if (node.IsJbp())
		return MakeRttiLayerFuncNode_impl<Func, FieldType::Jbp>(1, std::forward<Node>(node), up);
	throw MismatchType(DFieldInfo::GetTagTypeString<FieldType::Emp>().GetChar());
}
#ifdef _MSC_VER
#pragma warning(pop)
#endif

template <template <class, class> class Func, FieldType Type, any_node Node, any_node Cond,
	class Container = typename std::decay_t<Node>::Container,
	class FuncA = Func<DFieldInfo::TagTypeToValueType<Type>, Container>>
auto MakeRttiLayerFuncNode_impl(int, Node&& node, Cond&& cond, LayerType up)
	-> RttiFuncNode<Container>
{
	using NodeImpl = detail::RttiLayerFuncNode_impl<FuncA, Container, std::decay_t<Node>, std::decay_t<Cond>>;
	RttiFuncNode<Container> res;
	res.template Construct<NodeImpl>(std::forward<Node>(node).IncreaseDepth(), std::forward<Cond>(cond).IncreaseDepth(), up);
	return res;
}

//テンプレート引数Funcのを引数型Typeが満たさない場合、FuncAの推定に失敗しSFINAEによってこちらが呼ばれる。
template <template <class, class> class Func, FieldType Type, any_node Node, any_node Cond>
auto MakeRttiLayerFuncNode_impl(long, Node&&, Cond&&, LayerType)
-> RttiFuncNode<typename std::decay_t<Node>::Container>
{
	throw MismatchType(DFieldInfo::GetTagTypeString<Type>().GetChar());
}

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4702)
#endif
template <template <class, class> class Func, any_node Node, any_node Cond>
auto MakeRttiLayerFuncNode(Node&& node, Cond&& cond, LayerType up)
{
	if (!cond.IsI08()) throw MismatchType(DFieldInfo::GetTagTypeString(cond.GetType()));
	if (node.IsI08())
		return MakeRttiLayerFuncNode_impl<Func, FieldType::I08>(1, std::forward<Node>(node), std::forward<Cond>(cond), up);
	else if (node.IsI16())
		return MakeRttiLayerFuncNode_impl<Func, FieldType::I16>(1, std::forward<Node>(node), std::forward<Cond>(cond), up);
	else if (node.IsI32())
		return MakeRttiLayerFuncNode_impl<Func, FieldType::I32>(1, std::forward<Node>(node), std::forward<Cond>(cond), up);
	else if (node.IsI64())
		return MakeRttiLayerFuncNode_impl<Func, FieldType::I64>(1, std::forward<Node>(node), std::forward<Cond>(cond), up);
	else if (node.IsF32())
		return MakeRttiLayerFuncNode_impl<Func, FieldType::F32>(1, std::forward<Node>(node), std::forward<Cond>(cond), up);
	else if (node.IsF64())
		return MakeRttiLayerFuncNode_impl<Func, FieldType::F64>(1, std::forward<Node>(node), std::forward<Cond>(cond), up);
	else if (node.IsC32())
		return MakeRttiLayerFuncNode_impl<Func, FieldType::C32>(1, std::forward<Node>(node), std::forward<Cond>(cond), up);
	else if (node.IsC64())
		return MakeRttiLayerFuncNode_impl<Func, FieldType::C64>(1, std::forward<Node>(node), std::forward<Cond>(cond), up);
	else if (node.IsStr())
		return MakeRttiLayerFuncNode_impl<Func, FieldType::Str>(1, std::forward<Node>(node), std::forward<Cond>(cond), up);
	else if (node.IsJbp())
		return MakeRttiLayerFuncNode_impl<Func, FieldType::Jbp>(1, std::forward<Node>(node), std::forward<Cond>(cond), up);
	throw MismatchType(DFieldInfo::GetTagTypeString<FieldType::Emp>().GetChar());
}
#ifdef _MSC_VER
#pragma warning(pop)
#endif
}

#define DEFINE_LAYER_FUNCTION(NAME, NAME_ID, UP)\
template <node_or_placeholder NP>\
auto NAME_ID(NP&& np)\
{\
	constexpr bool is_rtti_type = (rtti_node_or_placeholder<NP>);\
	if constexpr (is_rtti_type)\
	{\
		try\
		{\
			return detail::MakeRttiLayerFuncNode<NAME>(detail::ConvertToNode(std::forward<NP>(np), std::true_type{}), UP);\
		}\
		catch (const MismatchType& e)\
		{\
			std::string message = std::format("No matching " #NAME_ID " functions for argument type {}.", e.GetMessage());\
			throw MismatchType(message);\
		}\
	}\
	else\
	{\
		auto n = detail::ConvertToNode(std::forward<NP>(np), std::false_type{}).IncreaseDepth();\
		using Node = decltype(n);\
		using ArgType = typename Node::RetType;\
		return CttiLayerFuncNode<NAME<ArgType, typename Node::Container>, Node, 0, DepthConstant<0>, LayerConstant<UP>>(std::move(n), LayerConstant<UP>{});\
	}\
}
#define DEFINE_LAYER_FUNCTION_10(NAME, NAME_ID)\
DEFINE_LAYER_FUNCTION(NAME, NAME_ID, 1)\
DEFINE_LAYER_FUNCTION(NAME, NAME_ID##1, 1)\
DEFINE_LAYER_FUNCTION(NAME, NAME_ID##2, 2)\
DEFINE_LAYER_FUNCTION(NAME, NAME_ID##3, 3)\
DEFINE_LAYER_FUNCTION(NAME, NAME_ID##4, 4)\
DEFINE_LAYER_FUNCTION(NAME, NAME_ID##5, 5)\
DEFINE_LAYER_FUNCTION(NAME, NAME_ID##6, 6)\
DEFINE_LAYER_FUNCTION(NAME, NAME_ID##7, 7)\
DEFINE_LAYER_FUNCTION(NAME, NAME_ID##8, 8)\
DEFINE_LAYER_FUNCTION(NAME, NAME_ID##9, 9)\
DEFINE_LAYER_FUNCTION(NAME, NAME_ID##10, 10)

#define DEFINE_LAYER_FUNCTION_IF(NAME, NAME_ID, UP)\
template <node_or_placeholder NP, node_or_placeholder Cond>\
auto NAME_ID(NP&& np, Cond&& cond)\
{\
	constexpr bool is_rtti_type = (rtti_node_or_placeholder<NP>) || (rtti_node_or_placeholder<Cond>);\
	if constexpr (is_rtti_type)\
	{\
		try\
		{\
			return detail::MakeRttiLayerFuncNode<NAME>(detail::ConvertToNode(std::forward<NP>(np), std::true_type{}),\
													   detail::ConvertToNode(std::forward<Cond>(cond), std::true_type{}), UP);\
		}\
		catch (const MismatchType& e)\
		{\
			std::string message = std::format("No matching " #NAME_ID " functions for argument type {}.", e.GetMessage());\
			throw MismatchType(message);\
		}\
	}\
	else\
	{\
		auto n = detail::ConvertToNode(std::forward<NP>(np), std::false_type{}).IncreaseDepth();\
		auto c = detail::ConvertToNode(std::forward<Cond>(cond), std::false_type{}).IncreaseDepth();\
		using Node = decltype(n);\
		using CondNode = decltype(c);\
		using ArgType = typename Node::RetType;\
		return CttiLayerFuncNode<NAME<ArgType, typename Node::Container>, Node, 0, DepthConstant<0>, LayerConstant<UP>, CondNode>(std::move(n), std::move(c), LayerConstant<UP>{});\
	}\
}
#define DEFINE_LAYER_FUNCTION_IF_10(NAME, NAME_ID)\
DEFINE_LAYER_FUNCTION_IF(NAME, NAME_ID, 1)\
DEFINE_LAYER_FUNCTION_IF(NAME, NAME_ID##1, 1)\
DEFINE_LAYER_FUNCTION_IF(NAME, NAME_ID##2, 2)\
DEFINE_LAYER_FUNCTION_IF(NAME, NAME_ID##3, 3)\
DEFINE_LAYER_FUNCTION_IF(NAME, NAME_ID##4, 4)\
DEFINE_LAYER_FUNCTION_IF(NAME, NAME_ID##5, 5)\
DEFINE_LAYER_FUNCTION_IF(NAME, NAME_ID##6, 6)\
DEFINE_LAYER_FUNCTION_IF(NAME, NAME_ID##7, 7)\
DEFINE_LAYER_FUNCTION_IF(NAME, NAME_ID##8, 8)\
DEFINE_LAYER_FUNCTION_IF(NAME, NAME_ID##9, 9)\
DEFINE_LAYER_FUNCTION_IF(NAME, NAME_ID##10, 10)


//上昇関数
//size
//引数を取得できた要素の数。引数の値は完全に無視する。戻り値型はint64_t/I64。
//The number of elements that can obtain the arguments. The value of the argument is completely ignored.
//The return type is int64_t.
DEFINE_LAYER_FUNCTION_10(detail::LayerFuncSize, size)
//exist
//引数が真であるような要素が存在するか否か。戻り値型はbool/I08。
//Whether there is an element that the argument is true.
//The return type is bool/I08.
DEFINE_LAYER_FUNCTION_10(detail::LayerFuncExist, exist)
//count
//引数が真であるような要素の数。戻り値型はint64_t/I64。
//The number of elements that the argument is true.
//The return type is int64_t.
DEFINE_LAYER_FUNCTION_10(detail::LayerFuncCount, count)
//sum
//引数の合計値。戻り値型は引数型と同じ。+=演算が可能な型に対してのみ呼び出せる。
//要素が一つもない場合は値初期化によるデフォルト値が返る。
//The sum of the arguments. The return type is the same as the argument type.
//This can only be called for types that can be added with +=.
//If there are no elements, the default value is returned.
DEFINE_LAYER_FUNCTION_10(detail::LayerFuncSum, sum)
DEFINE_LAYER_FUNCTION_IF_10(detail::LayerFuncSum, sum_if)
//mean
//引数の平均値。戻り値型は引数型と同じ。+=および/=演算が可能な型に対してのみ呼び出せる。
//要素が一つもない場合は値取得失敗となる。
//The average value of the arguments. The return type is the same as the argument type.
//This can only be called for types that can be added with += and divided by /=.
//If there are no elements, the value acquisition fails.
DEFINE_LAYER_FUNCTION_10(detail::LayerFuncMean, mean)
DEFINE_LAYER_FUNCTION_IF_10(detail::LayerFuncMean, mean_if)
//dev
//引数の標準偏差。戻り値型はdouble。整数または浮動小数点に対して飲み呼び出せる。
//要素が一つもない場合は値取得失敗となる。
//The standard deviation of the arguments. The return type is double.
//This can be called for integers or floating points.
//If there are no elements, the value acquisition fails.
DEFINE_LAYER_FUNCTION_10(detail::LayerFuncDev, dev)
DEFINE_LAYER_FUNCTION_IF_10(detail::LayerFuncDev, dev_if)
//greatest
//引数のうち最大値を返す。戻り値型は引数型と同じ。<演算が可能な型に対してのみ呼び出せる。
//要素が一つもない場合は値取得失敗となる。
//The maximum value of the arguments. The return type is the same as the argument type.
//This can only be called for types that can be compared with <.
//If there are no elements, the value acquisition fails.
DEFINE_LAYER_FUNCTION_10(detail::LayerFuncGreatest, greatest)
DEFINE_LAYER_FUNCTION_IF_10(detail::LayerFuncGreatest, greatest_if)
//least
//引数のうち最小値を返す。戻り値型は引数型と同じ。<演算が可能な型に対してのみ呼び出せる。
//要素が一つもない場合は値取得失敗となる。
//The minimum value of the arguments. The return type is the same as the argument type.
//This can only be called for types that can be compared with <.
//If there are no elements, the value acquisition fails.
DEFINE_LAYER_FUNCTION_10(detail::LayerFuncLeast, least)
DEFINE_LAYER_FUNCTION_IF_10(detail::LayerFuncLeast, least_if)
//first
//引数のうち取得できた最初の値を返す。
//要素が一つもない場合は値取得失敗となる。
//The first value that can be obtained from the arguments.
//If there are no elements, the value acquisition fails.
DEFINE_LAYER_FUNCTION_10(detail::LayerFuncFirst, first)
DEFINE_LAYER_FUNCTION_IF_10(detail::LayerFuncFirst, first_if)
//last
//引数のうち取得できた最後の値を返す。
//要素が一つもない場合は値取得失敗となる。
//The last value that can be obtained from the arguments.
//If there are no elements, the value acquisition fails.
DEFINE_LAYER_FUNCTION_10(detail::LayerFuncLast, last)
DEFINE_LAYER_FUNCTION_IF_10(detail::LayerFuncLast, last_if)
//index
//引数が真であるような最初の要素が、走査対象要素の中で何番目かを返す。
//要素が一つもない場合は-1を返す。
//The index of the first element that is true in the argument.
//If there are no elements, -1 is returned.
DEFINE_LAYER_FUNCTION_10(detail::LayerFuncIndex, index)
//lastindex
//引数が真であるような最後の要素が、走査対象要素の中で何番目かを返す。
//要素が一つもない場合は-1を返す。
//The index of the last element that is true in the argument.
//If there are no elements, -1 is returned.
DEFINE_LAYER_FUNCTION_10(detail::LayerFuncLastIndex, lastindex)

//is系関数
//isfirst
//現在の計算対象が、引数の値を取得できた最初の要素を指している時、trueを返す。
//Returns true when the current calculation target points to the first element that can get the value of the argument.
DEFINE_LAYER_FUNCTION_10(detail::LayerFuncIsFirst, isfirst)
DEFINE_LAYER_FUNCTION_IF_10(detail::LayerFuncIsFirst, isfirst_if)
//islast
//現在の計算対象が、引数の値を取得できた最後の要素を指している時、trueを返す。
//Returns true when the current calculation target points to the last element that can get the value of the argument.
DEFINE_LAYER_FUNCTION_10(detail::LayerFuncIsLast, islast)
DEFINE_LAYER_FUNCTION_IF_10(detail::LayerFuncIsLast, islast_if)
//islast
//現在の計算対象が、走査対象のうち引数が最大となるような要素を指している時、trueを返す。
//Returns true when the current calculation target points to an element that is the maximum of the arguments among the traversal targets.
DEFINE_LAYER_FUNCTION_10(detail::LayerFuncIsGreatest, isgreatest)
DEFINE_LAYER_FUNCTION_IF_10(detail::LayerFuncIsGreatest, isgreatest_if)
//islast
//現在の計算対象が、走査対象のうち引数が最小となるような要素を指している時、trueを返す。
//Returns true when the current calculation target points to an element that is the minimum of the arguments among the traversal targets.
DEFINE_LAYER_FUNCTION_10(detail::LayerFuncIsLeast, isleast)
DEFINE_LAYER_FUNCTION_IF_10(detail::LayerFuncIsLeast, isleast_if)

}

}

#endif
