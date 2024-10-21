#ifndef ADAPT_EVALUATOR_FIELD_NODE_H
#define ADAPT_EVALUATOR_FIELD_NODE_H

#include <OpenADAPT/Utility/Verbose.h>
#include <OpenADAPT/Evaluator/Placeholder.h>
#include <OpenADAPT/Joint/LayerInfo.h>
#include <OpenADAPT/Joint/JointInterface.h>
#include <OpenADAPT/Common/Concepts.h>
#include <OpenADAPT/Evaluator/NodeBase.h>

namespace adapt
{

namespace eval
{

template <class Placeholder_>
struct CttiFieldNode : public detail::CttiMethods<CttiFieldNode<Placeholder_>, std::type_identity_t>
{
	using Placeholder = Placeholder_;
	using Container = Placeholder::Container;
	using Traverser = Container::Traverser;
	using ConstTraverser = Container::ConstTraverser;

	using RetType = Placeholder::RetType;

	static constexpr RankType Rank = Placeholder::Rank;
	static constexpr RankType MaxRank = Placeholder::MaxRank;

	static constexpr bool IsCtti = ctti_placeholder<Placeholder>;

	CttiFieldNode() = default;

	CttiFieldNode(const Placeholder& p)
		: m_placeholder(p)
	{}

	CttiFieldNode IncreaseDepth() const { return *this; }

	void Init(const Container&, const Bpos&) {}
	void Init(const Container&) {}
	void Init(const Traverser&, const std::vector<std::tuple<const void*, const Bpos*, bool>>&) {}
	void Init(const Traverser& t) { Init(t, {}); }
	void Init(const ConstTraverser&, const std::vector<std::tuple<const void*, const Bpos*, bool>>&) {}
	void Init(const ConstTraverser& t) { Init(t, {}); }
	void Init() const {}

	static constexpr JointLayerArray<MaxRank> GetJointLayerArray() requires IsCtti
	{
		if constexpr (MaxRank > 0) return Placeholder::GetJointLayerArray();
		else return JointLayerArray<MaxRank>{};
	}
	static constexpr LayerInfo<MaxRank> GetLayerInfo() requires IsCtti
	{
		return GetLayerInfo(LayerInfo<MaxRank>(GetJointLayerArray()));
	}
	static constexpr LayerInfo<MaxRank> GetLayerInfo(LayerInfo<MaxRank> eli) requires IsCtti
	{
		auto res = eli;
		res.template Enable<Rank>();
		res.template SetTravLayer<Rank>(Placeholder::GetInternalLayer());
		return res;
	}
	static constexpr LayerInfo<MaxRank> GetLayerInfo(LayerInfo<MaxRank> eli, DepthType) requires IsCtti
	{
		return eli;
	}
	static constexpr LayerType GetLayer() requires IsCtti
	{
		return GetLayerInfo().GetTravLayer();
	}


	JointLayerArray<MaxRank> GetJointLayerArray() const requires (!IsCtti)
	{
		if constexpr (MaxRank > 0) return m_placeholder.GetJointLayerArray();
		else return JointLayerArray<MaxRank>{};
	}
	LayerInfo<MaxRank> GetLayerInfo() const requires (!IsCtti)
	{
		return GetLayerInfo(LayerInfo<MaxRank>(GetJointLayerArray()));
	}
	LayerInfo<MaxRank> GetLayerInfo(LayerInfo<MaxRank> eli) const requires (!IsCtti)
	{
		auto res = eli;
		res.template Enable<Rank>();
		res.template SetTravLayer<Rank>(m_placeholder.GetInternalLayer());
		return res;
	}
	LayerInfo<MaxRank> GetLayerInfo(LayerInfo<MaxRank> eli, DepthType) const requires (!IsCtti)
	{
		return eli;
	}
	LayerType GetLayer() const requires (!IsCtti)
	{
		return GetLayerInfo().GetTravLayer();
	}

	//const traverserとは限らないので、戻り値はdelctype(auto)にしておく。
	RetType& Evaluate(const Traverser& t) const { return t[m_placeholder]; }
	const RetType& Evaluate(const ConstTraverser& t) const { return t[m_placeholder]; }

	const RetType& Evaluate(const Container& t) const
	{
		if constexpr (ctti_placeholder<Placeholder>)
		{
			if constexpr (Placeholder::GetInternalLayer() != -1_layer)
			{
				throw InvalidArg("Evaluate(const Container&) must be applied to top layer node or placeholder");
			}
			else
				return t.GetBranch().GetField(m_placeholder);
		}
		else//typed_placeholderの場合
		{
			assert(m_placeholder.GetInternalLayer() == -1_layer);
			return t.GetBranch().GetField(m_placeholder);
		}
	}
	decltype(auto) Evaluate(const Container& t, const Bpos& bpos) const
	{
		return t.GetBranch(bpos, m_placeholder.GetInternalLayer()).GetField(m_placeholder);
	}

	RetType& operator()(const Traverser& t) const { return Evaluate(t); }
	const RetType& operator()(const ConstTraverser& t) const { return Evaluate(t); }
	const RetType& operator()(const Container& t) const { return Evaluate(t); }
	const RetType& operator()(const Container& t, const Bpos& bpos) const { return Evaluate(t, bpos); }

protected:
	//CttiOuterFieldNodeから呼ばれる。
	//単純にouterからfieldを取得すれば良い。
	RetType& Evaluate_outer(const Traverser&, const Traverser& outer_t) const
	{
		return outer_t[m_placeholder];
	}
	const RetType& Evaluate_outer(const ConstTraverser&, const ConstTraverser& outer_t) const
	{
		return outer_t[m_placeholder];
	}
	const RetType& Evaluate_outer(const Traverser&, const Container& outer_s) const
	{
		return outer_s.GetField(m_placeholder);
	}
	const RetType& Evaluate_outer(const ConstTraverser&, const Container& outer_s, const Bpos& outer_pos) const
	{
		return outer_s.GetBranch(outer_pos, m_placeholder.GetInternalLayer()).GetField(m_placeholder);
	}

protected:

	Placeholder m_placeholder;
};

template <class Placeholder_, class Nodes, class Indices = std::make_index_sequence<Nodes::size>>
struct CttiIndexedFieldNode;
template <class Placeholder_, class ...Nodes, size_t ...Indices>
struct CttiIndexedFieldNode<Placeholder_, TypeList<Nodes...>, std::index_sequence<Indices...>>
	: public detail::CttiMethods<CttiIndexedFieldNode<Placeholder_, TypeList<Nodes...>, std::index_sequence<Indices...>>, std::type_identity_t>
{
	using Placeholder = Placeholder_;
	using Container = Placeholder::Container;
	using Traverser = Container::Traverser;
	using ConstTraverser = Container::ConstTraverser;

	using RetType = Placeholder::RetType;

	static constexpr RankType Rank = Placeholder::Rank;
	static constexpr RankType MaxRank = Placeholder::MaxRank;

	static constexpr bool IsCtti = ctti_placeholder<Placeholder>;

	CttiIndexedFieldNode() = default;
	template<class ...Nodes_>
	CttiIndexedFieldNode(const Placeholder& p, Nodes_&& ...nodes)
		: m_placeholder(p), m_nodes(std::forward<Nodes_>(nodes)...)
	{}

	auto IncreaseDepth() const&
	{
		using Nodes_ = TypeList<std::remove_cvref_t<decltype(std::get<Indices>(m_nodes).IncreaseDepth())>...>;
		using Ret = CttiIndexedFieldNode<Placeholder, Nodes_, std::index_sequence<Indices...>>;
		return Ret(m_placeholder, std::get<Indices>(m_nodes).IncreaseDepth()...);
	}
	auto IncreaseDepth()&&
	{
		using Nodes_ = TypeList<std::remove_cvref_t<decltype(std::get<Indices>(m_nodes).IncreaseDepth())>...>;
		using Ret = CttiIndexedFieldNode<Placeholder, Nodes_, std::index_sequence<Indices...>>;
		return Ret(m_placeholder, std::get<Indices>(std::move(m_nodes)).IncreaseDepth()...);
	}

private:
	template <size_t N, class Stor>
	void Init_impl(const Stor& s, const Bpos& bpos)
	{
		if constexpr (N < sizeof...(Nodes))
		{
			if constexpr (any_node<GetType_t<N, Nodes...>>) std::get<N>(m_nodes).Init(s, bpos);
			Init_impl<N + 1>(s, bpos);
		}
	}
	template <size_t N, class Stor>
	void Init_impl(const Stor& s)
	{
		if constexpr (N < sizeof...(Nodes))
		{
			if constexpr (any_node<GetType_t<N, Nodes...>>) std::get<N>(m_nodes).Init(s);
			Init_impl<N + 1>(s);
		}
	}
	template <size_t N, class Trav>
	void Init_impl(const Trav& t, const std::vector<std::tuple<const void*, const Bpos*, bool>>& outer_t)
	{
		if constexpr (N < sizeof...(Nodes))
		{
			if constexpr (any_node<GetType_t<N, Nodes...>>) std::get<N>(m_nodes).Init(t, outer_t);
			Init_impl<N + 1>(t, outer_t);
		}
	}
	template <size_t N>
	void Init_impl()
	{
		if constexpr (N < sizeof...(Nodes))
		{
			if constexpr (any_node<GetType_t<N, Nodes...>>) std::get<N>(m_nodes).Init();
			Init_impl<N + 1>();
		}
	}
public:
	void Init(const Container& s, const Bpos& bpos)
	{
		Init_impl<0>(s, bpos);
		m_init_flag = false;
	}
	void Init(const Container& s)
	{
		Init_impl<0>(s);
		m_init_flag = false;
	}
	void Init(const Traverser& t, const std::vector<std::tuple<const void*, const Bpos*, bool>>& outer_t)
	{
		Init_impl<0>(t, outer_t);
		m_init_flag = false;
	}
	void Init(const Traverser& t)
	{
		Init(t, {});
	}
	void Init(const ConstTraverser& t, const std::vector<std::tuple<const void*, const Bpos*, bool>>& outer_t)
	{
		Init_impl<0>(t, outer_t);
		m_init_flag = false;
	}
	void Init(const ConstTraverser& t)
	{
		Init(t, {});
	}
	void Init()
	{
		Init_impl<0>();
		m_init_flag = true;
	}

	static constexpr JointLayerArray<MaxRank> GetJointLayerArray() requires IsCtti
	{
		if constexpr (MaxRank > 0) return Placeholder::GetJointLayerArray();
		else return JointLayerArray<MaxRank>();
	}
	static constexpr LayerInfo<MaxRank> GetLayerInfo() requires IsCtti
	{
		return GetLayerInfo(LayerInfo<MaxRank>(GetJointLayerArray()));
	}
	static constexpr LayerInfo<MaxRank> GetLayerInfo(LayerInfo<MaxRank> eli) requires IsCtti
	{
		auto a = eli;
		a.template Enable<Rank>();
		LayerType l = Placeholder::GetInternalLayer() - (LayerType)sizeof...(Nodes);//indexによる変動の最上層。
		a.template SetTravLayer<Rank>(l);
		return std::max({ a, GetType_t<Indices, Nodes...>::GetLayerInfo(eli)... });
	}
	static constexpr LayerInfo<MaxRank> GetLayerInfo(LayerInfo<MaxRank> eli, DepthType depth) requires IsCtti
	{
		//Indexedの場合、自身がouterでなくても引数がouterの可能性がある。
		return std::max({ GetType_t<Indices, Nodes...>::GetLayerInfo(eli, depth)... });
	}
	static constexpr LayerType GetLayer() requires IsCtti
	{
		return GetLayerInfo().GetTravLayer();
	}


	JointLayerArray<MaxRank> GetJointLayerArray() const requires (!IsCtti)
	{
		if constexpr (MaxRank > 0) return m_placeholder.GetJointLayerArray();
		else return JointLayerArray<MaxRank>();
	}
	LayerInfo<MaxRank> GetLayerInfo() const requires (!IsCtti)
	{
		return GetLayerInfo(LayerInfo<MaxRank>(GetJointLayerArray()));
	}
	LayerInfo<MaxRank> GetLayerInfo(LayerInfo<MaxRank> eli) const requires (!IsCtti)
	{
		auto a = eli;
		a.template Enable<Rank>();
		LayerType l = m_placeholder.GetInternalLayer() - (LayerType)sizeof...(Nodes);//indexによる変動の最上層。
		a.template SetTravLayer<Rank>(l);
		return std::max({ a, std::get<Indices>(m_nodes).GetLayerInfo(eli)... });
	}
	LayerInfo<MaxRank> GetLayerInfo(LayerInfo<MaxRank> eli, DepthType depth) const requires (!IsCtti)
	{
		//Indexedの場合、自身がouterでなくても引数がouterの可能性がある。
		return std::max({ std::get<Indices>(m_nodes).GetLayerInfo(eli, depth)... });
	}
	LayerType GetLayer() const requires (!IsCtti)
	{
		return GetLayerInfo().GetTravLayer();
	}


	//const traverserとは限らないので、戻り値はdelctype(auto)にしておく。
	decltype(auto) Evaluate(const Traverser& t) const
	{
		return t.GetField(m_placeholder, std::get<Indices>(m_nodes).Evaluate(t)...);
	}
	decltype(auto) Evaluate(const ConstTraverser& t) const
	{
		return t.GetField(m_placeholder, std::get<Indices>(m_nodes).Evaluate(t)...);
	}
	decltype(auto) Evaluate(const Container& t) const
	{
		return t.GetBranch(std::get<Indices>(m_nodes).Evaluate(t)...).GetField(m_placeholder);
	}
	decltype(auto) Evaluate(const Container& t, const Bpos& bpos) const
	{
		//layerまでのうち、layer - sizeof...(Indices)まではbposから与え、その後はnodeから与える。
		constexpr LayerConstant<(LayerType)sizeof...(Indices)> isize;
		return t.
			GetBranch(bpos, m_placeholder.GetInternalLayer() - isize).
			GetBranch(std::get<Indices>(m_nodes).Evaluate(t)...).
			GetField(m_placeholder);
	}

	decltype(auto) operator()(const Traverser& t)
	{
		if (m_init_flag) [[unlikely]] Init(t);
		return Evaluate(t);
	}
	decltype(auto) operator()(const ConstTraverser& t)
	{
		if (m_init_flag) [[unlikely]] Init(t);
		return Evaluate(t);
	}
	decltype(auto) operator()(const Container& t)
	{
		if (m_init_flag) [[unlikely]] Init(t);
		return Evaluate(t);
	}
	decltype(auto) operator()(const Container& t, const Bpos& bpos)
	{
		if (m_init_flag) [[unlikely]] Init(t, bpos);
		return Evaluate(t, bpos);
	}

protected:
	//CttiOuterFieldNodeから呼ばれる。
	//index付きかつOuterの場合、fieldそのものはouterから取得するが、indicesは非outerから所得する必要がある。
	decltype(auto) Evaluate_outer(const Traverser& t, const Traverser& outer_t) const
	{
		return outer_t.GetField(m_placeholder, std::get<Indices>(m_nodes).Evaluate(t)...);
	}
	decltype(auto) Evaluate_outer(const ConstTraverser& t, const ConstTraverser& outer_t) const
	{
		return outer_t.GetField(m_placeholder, std::get<Indices>(m_nodes).Evaluate(t)...);
	}

	//outer field nodeが呼ばれるのはlayerfuncの内側のはずなので、
	//第一引数がContainerであるようなEvaluate_outerはなくてよいはず……。
	decltype(auto) Evaluate_outer(const Traverser& t, const Container& outer_s) const
	{
		return outer_s.GetBranch(std::get<Indices>(m_nodes).Evaluate(t)...).GetField(m_placeholder);
	}
	decltype(auto) Evaluate_outer(const Traverser& t, const Container& outer_s, const Bpos& outer_bpos) const
	{
		constexpr LayerConstant<(LayerType)sizeof...(Indices)> isize;
		return outer_s.
			GetBranch(outer_bpos, m_placeholder.GetInternalLayer() - isize).
			GetBranch(std::get<Indices>(m_nodes).Evaluate(t)...).
			GetField(m_placeholder);
	}
	decltype(auto) Evaluate_outer(const ConstTraverser& t, const Container& outer_s) const
	{
		return outer_s.GetBranch(std::get<Indices>(m_nodes).Evaluate(t)...).GetField(m_placeholder);
	}
	decltype(auto) Evaluate_outer(const ConstTraverser& t, const Container& outer_s, const Bpos& outer_bpos) const
	{
		constexpr LayerConstant<(LayerType)sizeof...(Indices)> isize;
		return outer_s.
			GetBranch(outer_bpos, m_placeholder.GetInternalLayer() - isize).
			GetBranch(std::get<Indices>(m_nodes).Evaluate(t)...).
			GetField(m_placeholder);
	}

protected:

	Placeholder m_placeholder;
	std::tuple<Nodes...> m_nodes;
	bool m_init_flag = true;
};

template <class Base_, DepthType Depth>
struct CttiOuterFieldNode : public Base_
{
	using Base = Base_;
	using Placeholder = Base::Placeholder;
	using Container = Placeholder::Container;
	using Traverser = Container::Traverser;
	using ConstTraverser = Container::ConstTraverser;

	using RetType = Placeholder::RetType;
	static constexpr RankType Rank = Placeholder::Rank;
	static constexpr RankType MaxRank = Placeholder::MaxRank;

	static constexpr bool IsCtti = ctti_placeholder<Placeholder>;

	CttiOuterFieldNode() = default;
	template <class ...Args>
	CttiOuterFieldNode(Args&& ...args)
		: Base(std::forward<Args>(args)...)
	{}

	CttiOuterFieldNode IncreaseDepth() const&
	{
		CttiOuterFieldNode res = *this;
		std::move(res).Base::IncreaseDepth();
		return std::move(res);
	}
	CttiOuterFieldNode IncreaseDepth()&&
	{
		auto res = std::move(*this).Base::IncreaseDepth();
		return CttiOuterFieldNode(std::move(res));
	}

	//outer field nodeはContainerを引数に呼ばれることはない。
	//なぜなら、これらは必ず階層関数から呼ばれ、その引数はTraverserかConstTraverserのどちらかであるため。
private:
	template <class Trav>
	void Init_impl(const Trav& t, const std::vector<std::tuple<const void*, const Bpos*, bool>>& outer_t)
	{
		if (outer_t.size() <= Depth)
		{
			std::string message =
				std::format("\"outer<{}>\" \"o{}\" must NOT be used in the depth <= {} scope of the layer function.",
							Depth, Depth, Depth);
			PrintError(message);
			throw InvalidArg(message);
		}
		std::tie(m_outer_traverser, m_outer_bpos, m_container_mode) = outer_t[Depth];
		Base::Init(t, outer_t);
		m_init_flag = false;
	}
public:
	void Init(const Traverser& trav, const std::vector<std::tuple<const void*, const Bpos*, bool>>& outer_t)
	{
		Init_impl(trav, outer_t);
	}
	void Init(const ConstTraverser& trav, const std::vector<std::tuple<const void*, const Bpos*, bool>>& outer_t)
	{
		Init_impl(trav, outer_t);
	}
	void Init()
	{
		m_outer_traverser = nullptr;
		m_outer_bpos = nullptr;
		m_container_mode = false;
		Base::Init();
		m_init_flag = true;
	}

	static constexpr LayerInfo<MaxRank> GetLayerInfo() requires IsCtti
	{
		return GetLayerInfo(LayerInfo<MaxRank>(Base::GetJointLayerArray()));
	}
	static constexpr LayerInfo<MaxRank> GetLayerInfo(LayerInfo<MaxRank> eli) requires IsCtti
	{
		return eli;
	}
	static constexpr LayerInfo<MaxRank> GetLayerInfo(LayerInfo<MaxRank> eli, DepthType depth) requires IsCtti
	{
		if (Depth != depth) return eli;
		auto res = eli;
		res.template Enable<Rank>();
		res.template SetTravLayer<Rank>(Placeholder::GetInternalLayer());
		return std::max(res, Base::GetLayerInfo());
	}
	static constexpr LayerType GetLayer() requires IsCtti
	{
		return GetLayerInfo().GetTravLayer();
	}

	//outer field nodeがContainerを引数に呼ばれることはないはず。
	decltype(auto) Evaluate(const Traverser& t) const
	{
		assert(!m_container_mode);
		//if (!m_container_mode)
		{
			const Traverser* outer_t = static_cast<const Traverser*>(m_outer_traverser);
			return this->Evaluate_outer(t, *outer_t);
		}
		/*else
		{
			const Container* s = static_cast<const Container*>(m_outer_traverser);
			if (m_outer_bpos == nullptr) return this->Evaluate_outer(t, *s);
			else
			{
				const Bpos* bpos = static_cast<const Bpos*>(m_outer_bpos);
				return this->Evaluate_outer(t, *s, *bpos);
			}
		}*/
	}
	decltype(auto) Evaluate(const ConstTraverser& t) const
	{
		if (!m_container_mode)
		{
			const ConstTraverser* outer_t = static_cast<const ConstTraverser*>(m_outer_traverser);
			return this->Evaluate_outer(t, *outer_t);
		}
		else
		{
			const Container* s = static_cast<const Container*>(m_outer_traverser);
			if (m_outer_bpos == nullptr) return std::as_const(this->Evaluate_outer(t, *s));
			else
			{
				const Bpos* bpos = static_cast<const Bpos*>(m_outer_bpos);
				return std::as_const(this->Evaluate_outer(t, *s, *bpos));
			}
		}
	}
	decltype(auto) operator()(const Traverser& t) const
	{
		if (m_init_flag) [[unlikely]] Init(t);
		const Traverser* outer_t = static_cast<const Traverser*>(m_outer_traverser);
		return this->oper_outer(t);
	}
	decltype(auto) operator()(const ConstTraverser& t) const
	{
		if (m_init_flag) [[unlikely]] Init(t);
		const ConstTraverser* outer_t = static_cast<const ConstTraverser*>(m_outer_traverser);
		return this->oper_outer(t);
	}

private:
	const void* m_outer_traverser = nullptr;
	const void* m_outer_bpos = nullptr;//最外部のouterはTreeとBposの組み合わせで与えられている可能性がある。
	bool m_container_mode = false;
	bool m_init_flag = true;
};


template <class Placeholder_>
struct RttiFieldNode : public detail::RttiMethods<RttiFieldNode<Placeholder_>, std::type_identity_t>
{
	using Placeholder = Placeholder_;
	using Container = Placeholder::Container;
	using Traverser = Container::Traverser;
	using ConstTraverser = Container::ConstTraverser;

	static constexpr RankType Rank = Placeholder::Rank;
	static constexpr RankType MaxRank = Placeholder::MaxRank;

	template <class TravOrStor, class NP, class HasBpos>
	friend struct RttiEvalProxy;

	RttiFieldNode()
	{}
	//depthはph.outer(0)のようにouter指定する場合に、その深度が与えられる。
	RttiFieldNode(const Placeholder& ph, DepthType depth = -1)
		: m_placeholder(ph), m_outer_traverser(nullptr), m_depth(depth)
	{}

	RttiFieldNode IncreaseDepth() const& { return *this; }
	RttiFieldNode IncreaseDepth()&& { return std::move(*this); }

	void Init(const Container&, const Bpos&)
	{
		assert(m_depth == -1);//この関数はouter fieldに対して呼ぶことはできない。
		m_init_flag = false;
	}
	void Init(const Container&)
	{
		assert(m_depth == -1);//この関数はouter fieldに対して呼ぶことはできない。
		m_init_flag = false;
	}
private:
	template <class Trav>
	void Init_impl(const Trav&, const std::vector<std::tuple<const void*, const Bpos*, bool>>& outer_t)
	{
		if (m_depth != -1)
		{
			assert(std::cmp_greater(outer_t.size(), m_depth));
			std::tie(m_outer_traverser, m_outer_bpos, m_container_mode) = outer_t[m_depth];
		}
		m_init_flag = false;
	}
public:
	void Init(const Traverser& t, const std::vector<std::tuple<const void*, const Bpos*, bool>>& outer_t) { Init_impl(t, outer_t); }
	void Init(const Traverser& t) { Init_impl(t, {}); }
	void Init(const ConstTraverser& t, const std::vector<std::tuple<const void*, const Bpos*, bool>>& outer_t) { Init_impl(t, outer_t); }
	void Init(const ConstTraverser& t) { Init_impl(t, {}); }
	void Init()
	{
		m_outer_traverser = nullptr;
		m_outer_bpos = nullptr;
		m_container_mode = false;
		m_init_flag = true;
	}

	JointLayerArray<MaxRank> GetJointLayerArray() const
	{
		if constexpr (MaxRank > 0) return m_placeholder.GetJointLayerArray();
		else return JointLayerArray<MaxRank>();
	}
	LayerInfo<MaxRank> GetLayerInfo() const
	{
		return GetLayerInfo(LayerInfo<MaxRank>(GetJointLayerArray()));
	}
	LayerInfo<MaxRank> GetLayerInfo(LayerInfo<MaxRank> eli) const
	{
		//depthが-1ということは、これはouter fieldではない。
		//この関数は
		if (m_depth != -1) return eli;
		auto res = eli;
		res.template Enable<Rank>();
		res.template SetTravLayer<Rank>(m_placeholder.GetInternalLayer());
		return res;
	}
	LayerInfo<MaxRank> GetLayerInfo(LayerInfo<MaxRank> eli, DepthType depth) const
	{
		if (m_depth == -1 || m_depth != depth) return eli;
		auto res = eli;
		res.template Enable<Rank>();
		res.template SetTravLayer<Rank>(this->m_placeholder.GetInternalLayer());
		return res;
	}
	LayerType GetLayer() const
	{
		return GetLayerInfo().GetTravLayer();
	}

	template <FieldType Type>
	decltype(auto) Evaluate(const Traverser& t, Number<Type>) const
	{
		if (!m_outer_traverser) [[likely]]
			return t[m_placeholder].template as_unsafe<Type>();
		else
		{
			assert(!m_container_mode);
			//Container modeのとき、Evaluateの引数はConstTraverserのはず。
			//なので、container modeかどうかの判定は必要ない。
			const Traverser* ptr = static_cast<const Traverser*>(m_outer_traverser);
			return (*ptr)[this->m_placeholder].template as_unsafe<Type>();
		}
	}
	template <FieldType Type>
	decltype(auto) Evaluate(const ConstTraverser& t, Number<Type>) const
	{
		if (!m_outer_traverser) [[likely]]
			return t[m_placeholder].template as_unsafe<Type>();
		else
		{
			if (!m_container_mode)
			{
				const ConstTraverser* ptr = static_cast<const ConstTraverser*>(m_outer_traverser);
				return (*ptr)[this->m_placeholder].template as_unsafe<Type>();
			}
			else
			{
				if constexpr (container_simplex<Container>)
				{
					const Container* ptr = static_cast<const Container*>(m_outer_traverser);
					if (m_outer_bpos == nullptr)
						return ptr->GetBranch().GetField(m_placeholder).template as_unsafe<Type>();
					else
					{
						const Bpos* bpos = static_cast<const Bpos*>(m_outer_bpos);
						return ptr->GetBranch(*bpos, m_placeholder.GetInternalLayer()).GetField(m_placeholder).template as_unsafe<Type>();
					}
				}
				else
				{
					//joined containerの場合。
					const Container* ptr = static_cast<const Container*>(m_outer_traverser);
					if (m_outer_bpos == nullptr)
						return std::as_const(ptr->GetField(m_placeholder).template as_unsafe<Type>());
					else
					{
						return std::as_const(ptr->GetField(m_placeholder, *m_outer_bpos).template as_unsafe<Type>());
					}
				}
			}
		}
	}
	template <FieldType Type>
	decltype(auto) Evaluate(const Container& t, Number<Type>) const
	{
		if (!m_outer_traverser) [[likely]]
		{
			assert(m_placeholder.GetInternalLayer() == -1_layer);
			return t.GetBranch().GetField(m_placeholder).template as_unsafe<Type>();
		}
		//Containerを引数に呼び出しているとき、outerであることはあり得ない。
		throw InvalidArg("");
	}
	template <FieldType Type>
	decltype(auto) Evaluate(const Container& t, const Bpos& bpos, Number<Type>) const
	{
		if (!m_outer_traverser) [[likely]]
			return t.GetBranch(bpos, m_placeholder.GetInternalLayer()).GetField(m_placeholder).template as_unsafe<Type>();
		//Containerを引数に呼び出しているとき、outerであることはあり得ない。
		throw InvalidArg("");
	}

	template <FieldType Type>
	decltype(auto) operator()(const Traverser& t, Number<Type> n)
	{
		if (m_init_flag) [[unlikely]] Init(t);
		return Evaluate(t, n);
	}
	template <FieldType Type>
	decltype(auto) operator()(const ConstTraverser& t, Number<Type> n)
	{
		if (m_init_flag) [[unlikely]] Init(t);
		return Evaluate(t, n);
	}
	template <FieldType Type>
	decltype(auto) operator()(const Container& t, Number<Type> n)
	{
		if (m_init_flag) [[unlikely]] Init(t);
		return Evaluate(t, n);
	}
	template <FieldType Type>
	decltype(auto) operator()(const Container& t, const Bpos& bpos, Number<Type> n)
	{
		if (m_init_flag) [[unlikely]] Init(t, bpos);
		return Evaluate(t, bpos, n);
	}

	RttiEvalProxy<Traverser, RttiFieldNode, std::false_type> Evaluate(const Traverser& t) const&
	{
		return RttiEvalProxy<Traverser, const RttiFieldNode&, std::false_type>{ t, *this };
	}
	RttiEvalProxy<ConstTraverser, RttiFieldNode, std::false_type> Evaluate(const ConstTraverser& t) const&
	{
		return RttiEvalProxy<ConstTraverser, const RttiFieldNode&, std::false_type>{ t, *this };
	}
	RttiEvalProxy<Traverser, RttiFieldNode, std::false_type> Evaluate(const Container& t) const&
	{
		return RttiEvalProxy<Traverser, const RttiFieldNode&, std::false_type>{ t, *this };
	}
	RttiEvalProxy<ConstTraverser, RttiFieldNode, std::true_type> Evaluate(const Container& t, const Bpos& bpos) const&
	{
		return RttiEvalProxy<ConstTraverser, const RttiFieldNode&, std::true_type>{ t, bpos, *this };
	}

	RttiEvalProxy<Traverser, RttiFieldNode, std::false_type> Evaluate(const Traverser& t) &&
	{
		return RttiEvalProxy<Traverser, RttiFieldNode, std::false_type>{ t, std::move(*this) };
	}
	RttiEvalProxy<ConstTraverser, RttiFieldNode, std::false_type> Evaluate(const ConstTraverser& t) &&
	{
		return RttiEvalProxy<ConstTraverser, RttiFieldNode, std::false_type>{ t, std::move(*this) };
	}
	RttiEvalProxy<Traverser, RttiFieldNode, std::false_type> Evaluate(const Container& t) &&
	{
		return RttiEvalProxy<Traverser, RttiFieldNode, std::false_type>{ t, std::move(*this) };
	}
	RttiEvalProxy<ConstTraverser, RttiFieldNode, std::true_type> Evaluate(const Container& t, const Bpos& bpos) &&
	{
		return RttiEvalProxy<ConstTraverser, RttiFieldNode, std::true_type>{ t, bpos, std::move(*this) };
	}

	RttiEvalProxy<Traverser, const RttiFieldNode&, std::false_type> operator()(const Traverser& t) &
	{
		if (m_init_flag) [[unlikely]] Init(t);
		return Evaluate(t);
	}
	RttiEvalProxy<ConstTraverser, const RttiFieldNode&, std::false_type> operator()(const ConstTraverser& t) &
	{
		if (m_init_flag) [[unlikely]] Init(t);
		return Evaluate(t);
	}
	RttiEvalProxy<Traverser, const RttiFieldNode&, std::false_type> operator()(const Container& t) &
	{
		if (m_init_flag) [[unlikely]] Init(t);
		return Evaluate(t);
	}
	RttiEvalProxy<ConstTraverser, const RttiFieldNode&, std::true_type> operator()(const Container& t, const Bpos& bpos) &
	{
		if (m_init_flag) [[unlikely]] Init(t, bpos);
		return Evaluate(t, bpos);
	}

	RttiEvalProxy<Traverser, RttiFieldNode, std::false_type> operator()(const Traverser& t) &&
	{
		if (m_init_flag) [[unlikely]] Init(t);
		return Evaluate(t);
	}
	RttiEvalProxy<ConstTraverser, RttiFieldNode, std::false_type> operator()(const ConstTraverser& t) &&
	{
		if (m_init_flag) [[unlikely]] Init(t);
		return Evaluate(t);
	}
	RttiEvalProxy<Traverser, RttiFieldNode, std::false_type> operator()(const Container& t) &&
	{
		if (m_init_flag) [[unlikely]] Init(t);
		return Evaluate(t);
	}
	RttiEvalProxy<ConstTraverser, RttiFieldNode, std::true_type> operator()(const Container& t, const Bpos& bpos) &&
	{
		if (m_init_flag) [[unlikely]] Init(t, bpos);
		return Evaluate(t, bpos);
	}

	FieldType GetType() const
	{
		return m_placeholder.GetType();
	}

protected:

	Placeholder m_placeholder;
	const void* m_outer_traverser = nullptr;
	const Bpos* m_outer_bpos = nullptr;
	DepthType m_depth = DepthType(-1);
	bool m_container_mode = false;
	bool m_init_flag = true;
};

namespace detail
{

template <class Components, placeholder Placeholder, FieldType Type, class Nodes, class IndTypes,
	class Indices = std::make_index_sequence<Nodes::size>>
struct RttiIndexedFieldNode_impl;
template <class Container, placeholder Placeholder, FieldType Type, class ...Nodes, FieldType ...IndTypes, size_t ...Indices>
struct RttiIndexedFieldNode_impl<Container, Placeholder, Type, TypeList<Nodes...>, ValueList<IndTypes...>, std::index_sequence<Indices...>>
	: public RttiFuncNode_body<Container, TypeList<Nodes...>, std::index_sequence<Indices...>,
							   TypeList<DFieldInfo::TagTypeToValueType<IndTypes>...>>
{
	using Base = RttiFuncNode_body<Container, TypeList<Nodes...>, std::index_sequence<Indices...>,
								   TypeList<DFieldInfo::TagTypeToValueType<IndTypes>...>>;
	using Traverser = Container::Traverser;
	using ConstTraverser = Container::ConstTraverser;

	using RetType = DFieldInfo::TagTypeToValueType<Type>;
	using RetTypeRef = std::conditional_t<DFieldInfo::IsTrivial(Type), RetType, const RetType&>;

	static constexpr RankType Rank = Placeholder::Rank;
	static constexpr RankType MaxRank = Placeholder::MaxRank;

	RttiIndexedFieldNode_impl()
	{}
	template <any_node ...Ns>
	RttiIndexedFieldNode_impl(const Placeholder& ph, Ns&& ...nodes)
		: Base(std::forward<Ns>(nodes)...), m_placeholder(ph)
	{}

	virtual ~RttiIndexedFieldNode_impl() = default;

	virtual RttiFuncNode_base<Container>* Clone() const override
	{
		auto* res = new RttiIndexedFieldNode_impl{};
		res->CopyFrom(*this);
		return res;
	}
	virtual void CopyFrom(const RttiFuncNode_base<Container>& that) override
	{
		Base::CopyFrom(that);
		const auto* x = dynamic_cast<const RttiIndexedFieldNode_impl*>(&that);
		if (x == nullptr)
		{
			std::string message = "The type of the RttiIndexedFieldNode_impl are not the same.";
			PrintError(message);
			throw MismatchType(message);
		}
		m_placeholder = x->m_placeholder;
	}

	virtual JointLayerArray<MaxRank> GetJointLayerArray() const override
	{
		if constexpr (MaxRank > 0) return m_placeholder.GetJointLayerArray();
		else return JointLayerArray<MaxRank>();
	}
	virtual LayerInfo<MaxRank> GetLayerInfo() const override
	{
		return GetLayerInfo(LayerInfo<MaxRank>(GetJointLayerArray()));
	}
	virtual LayerInfo<MaxRank> GetLayerInfo(LayerInfo<MaxRank> eli) const override
	{
		auto res = eli;
		res.template Enable<Rank>();
		res.template SetTravLayer<Rank>(m_placeholder.GetInternalLayer());
		LayerType nind = (LayerType)sizeof...(Nodes);//indexによる変動の最上層。
		LayerType diff = m_placeholder.GetInternalLayer() - eli.GetInternal(Rank).GetUJointLayer();

		//indexed fieldの階層上昇は単純ではない。
		//というのも、現在のindexは同じ順位の中でのみ上昇が可能で、順位をまたぐ上昇は出来ないため。
		//つまり例えば、rank位のupper joint layerが2で、このフィールドのinternal layerも2だとする。
		//このとき、indexが何個与えられようと、external layerはupper joint layerよりも上にはなれない。
		nind = std::min(diff, nind);
		if (nind > 0) res.Raise(nind);
		auto arglayer = Base::GetLayerInfo(eli);//index引数の階層。
		return std::max(res, arglayer);
	}
	virtual LayerInfo<MaxRank> GetLayerInfo(LayerInfo<MaxRank> eli, DepthType) const override
	{
		return eli;
	}
	LayerType GetLayer() const
	{
		return GetLayerInfo().GetTravLayer();
	}

	template <size_t N>
	using ArgType = GetType_t<N, Nodes...>;

	virtual RetTypeRef Evaluate(const Traverser& t, Number<Type>) const override
	{
		return t.GetField(m_placeholder, this->template GetArg<Indices>(t)...).template as_unsafe<Type>();
	}
	virtual RetTypeRef Evaluate(const ConstTraverser& t, Number<Type>) const override
	{
		return t.GetField(m_placeholder, this->template GetArg<Indices>(t)...).template as_unsafe<Type>();
	}
	virtual RetTypeRef Evaluate(const Container& s, Number<Type>) const override
	{
		return s.GetBranch((BindexType)this->template GetArg<Indices>(s)...).GetField(m_placeholder).template as_unsafe<Type>();
	}
	virtual RetTypeRef Evaluate(const Container& s, const Bpos& bpos, Number<Type>) const override
	{
		if constexpr (container_simplex<Container>)
		{
			//layerまでのうち、layer - sizeof...(Indices)まではbposから与え、その後はnodeから与える。
			constexpr LayerConstant<(LayerType)sizeof...(Indices)> isize;
			return s.
				GetBranch(bpos, m_placeholder.GetInternalLayer() - isize).
				GetBranch((BindexType)this->template GetArg<Indices>(s, bpos)...).
				GetField(m_placeholder).template as_unsafe<Type>();
		}
		else
		{
			//joined Containerの場合、simplexと同じ方法は使えない。
			return s.GetField(m_placeholder, bpos, this->template GetArg<Indices>(s, bpos)...).template as_unsafe<Type>();
		}
	}

	virtual FieldType GetType() const override { return Type; }
private:
	Placeholder m_placeholder;
};

/*template <FieldType Type, placeholder Placeholder, node ...Nodes>
auto MakeRttiIndexedFieldNode(const Placeholder& ph, Nodes&& ...indices)
{
	using Container = Placeholder::Container;
	using Impl = RttiIndexedFieldNode_impl<Container, Placeholder, Type, TypeList<Nodes...>>;
	RttiFuncNode<Container> res;
	res.Construct<Impl>(ph, std::forward<Nodes>(indices)...);
	return res;
}*/
template <FieldType PHType, placeholder Placeholder,
	FieldType ...IndTypes, any_node ...Nodes, size_t ...Indices>
auto MakeRttiIndexedFieldNode(const Placeholder& ph, ValueList<IndTypes...>,
							  std::tuple<Nodes...> t, std::index_sequence<Indices...>)
{
	using Container = Placeholder::Container;
	using Impl = RttiIndexedFieldNode_impl<Container, Placeholder, PHType, TypeList<std::decay_t<Nodes>...>, ValueList<IndTypes...>>;
	RttiFuncNode<Container> res;
	res.template Construct<Impl>(ph, std::get<Indices>(t)...);
	return res;
}
template <FieldType PHType, placeholder Placeholder,
	FieldType ...IndTypes, any_node ...Nodes>
auto MakeRttiIndexedFieldNode_switch_ind(const Placeholder& ph, ValueList<IndTypes...>,
							  std::tuple<Nodes...> t)
{
	return MakeRttiIndexedFieldNode<PHType>(ph, ValueList<IndTypes...>{}, std::move(t),
											std::make_index_sequence<sizeof...(IndTypes)>{});
}
template <FieldType PHType, placeholder Placeholder, FieldType ...IndTypes, any_node ...Nodes, any_node Head, any_node ...Body>
auto MakeRttiIndexedFieldNode_switch_ind(const Placeholder& ph, ValueList<IndTypes...>,
										 std::tuple<Nodes...> t, Head&& head, Body&& ...body)
{
	if (head.IsI08())
		return MakeRttiIndexedFieldNode_switch_ind<PHType>(
			ph, ValueList<IndTypes..., FieldType::I08>(),
			TupleAdd(std::move(t), std::forward<Head>(head)), std::forward<Body>(body)...);
	else if (head.IsI16())
		return MakeRttiIndexedFieldNode_switch_ind<PHType>(
			ph, ValueList<IndTypes..., FieldType::I16>(),
			TupleAdd(std::move(t), std::forward<Head>(head)), std::forward<Body>(body)...);
	else if (head.IsI32())
		return MakeRttiIndexedFieldNode_switch_ind<PHType>(
			ph, ValueList<IndTypes..., FieldType::I32>(),
			TupleAdd(std::move(t), std::forward<Head>(head)), std::forward<Body>(body)...);
	else if (head.IsI64())
		return MakeRttiIndexedFieldNode_switch_ind<PHType>(
			ph, ValueList<IndTypes..., FieldType::I64>(),
			TupleAdd(std::move(t), std::forward<Head>(head)), std::forward<Body>(body)...);
	throw MismatchType(std::format("Field indices must be integers, {} is assigned.", DFieldInfo::GetTagTypeString(head.GetType())));
}
template <placeholder Placeholder, any_node ...Nodes>
auto MakeRttiIndexedFieldNode_switch_ph(const Placeholder& ph, Nodes&& ...indices)
{
	auto isint = [](auto& i)
	{
		return i.IsI08() || i.IsI16() || i.IsI32() || i.IsI64();
	};
	if (!(isint(indices) && ...)) throw MismatchType("all indices of a filed must be integers.");
#define CODE(T) return MakeRttiIndexedFieldNode_switch_ind<T>(ph, ValueList<>{}, std::tuple<>{}, std::forward<Nodes>(indices)...);
	ADAPT_SWITCH_FIELD_TYPE(ph.GetType(), CODE, throw MismatchType("");)
#undef CODE
}
template <placeholder Placeholder, any_node ...Nodes>
auto MakeCttiIndexedFieldNode(const Placeholder& ph, Nodes&& ...nodes)
{
	static_assert((std::integral<typename std::remove_cvref_t<Nodes>::RetType> && ...),
		"all indices of a field must be integers.");
	using Nodes_ = TypeList<std::remove_cvref_t<Nodes>...>;
	return CttiIndexedFieldNode<Placeholder, Nodes_>(ph, std::forward<Nodes>(nodes)...);
}

template <placeholder Placeholder, class ...NPs>
auto MakeIndexedFieldNode(const Placeholder& ph, NPs&& ...indices)
{
	constexpr bool has_rtti_type = (rtti_placeholder<Placeholder> || (rtti_node_or_placeholder<NPs> || ...));

	if constexpr (has_rtti_type)
	{
		return MakeRttiIndexedFieldNode_switch_ph(ph, ConvertToNode(std::forward<NPs>(indices), std::true_type{})...);
	}
	else
	{
		return MakeCttiIndexedFieldNode(ph, ConvertToNode(std::forward<NPs>(indices), std::false_type{})...);
	}

}


}

}

}

#endif
