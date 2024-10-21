#ifndef ADAPT_CONTAINER_JOINED_CONTAINER_H
#define ADAPT_CONTAINER_JOINED_CONTAINER_H

#include <OpenADAPT/Joint/LayerInfo.h>
#include <OpenADAPT/Container/Tree.h>
#include <OpenADAPT/Traverser/ExternalTraverser.h>

namespace adapt
{

//SJoinedContainerは連結層がコンパイル時に決定され、かつ全ての構成ストレージがStaticである場合に用いられる。
//こちらはExtractの出力がSTreeまたはSHashである点がDJoinedContainerとの最大の違い。
//また走査時の連結部計算もある程度速くはなる。
//コンパイル時にストレージ構造を完全に決定できるので、出力もstaticにできる。
//template <template <class> class Qualifier, class ...Containers>
//class SJoinedContainer {};
template <RankType MaxRank>
class SJointLayerInfo {};

template <template <class> class Qualifier, container_simplex ...IContainers>
class DJoinedRange;
template <template <class> class Qualifier, container_simplex ...IContainers>
class DJoinedRange_prompt;

namespace eval::detail
{
template <RankType Rank, class Container_>
class CttiTryJoinNode;
template <RankType Rank, joined_container Container>
CttiTryJoinNode<Rank, Container> MakeTryJoinNode(const Container& c);
}

template <RankType MaxRank>
class DJointLayerInfo
{
public:

	DJointLayerInfo(const JointLayerArray<MaxRank>& layers)
		: m_joint_layers(layers)
	{
		LayerType ext = -1_layer;
		for (RankType r = 0; r <= MaxRank; ++r)
		{
			m_ext_ujoint_layers[r] = ext;
			ext = m_joint_layers[r].second - m_joint_layers[r].first + ext;
		}
	}

	//連結後階層から結合前階層へ変換する。
	template <RankType Rank>
	LayerType ConvertExtToInt(LayerType extlayer) const
	{
		return extlayer - m_ext_ujoint_layers[Rank] + m_joint_layers[Rank].first;
	}
	//結合前階層から連結後階層へ変換する。
	template <RankType Rank>
	LayerType ConvertIntToExt(LayerType intlayer) const
	{
		return intlayer - m_joint_layers[Rank].first + m_ext_ujoint_layers[Rank];
	}

	template <RankType Rank>
	LayerType GetUJointLayer() const { return m_joint_layers[Rank].first; }
	template <RankType Rank>
	LayerType GetLJointLayer() const { return m_joint_layers[Rank].second; }
	template <RankType Rank>
	LayerType GetExtUJointLayer() const { return m_ext_ujoint_layers[Rank]; }

	const JointLayerArray<MaxRank>& GetJointLayers() const { return m_joint_layers; }

private:
	JointLayerArray<MaxRank> m_joint_layers;
	std::array<LayerType, MaxRank + 1> m_ext_ujoint_layers;
};

//DJoinedContainerは連結層が実行時に決定されるか、もしくは構成ストレージの中にDynamicなものを含む場合に用いられる。
//いずれか片方がstaticでなくなった時点でExtractの出力が
template <template <class> class Qualifier, class ...IContainers>
class DJoinedContainer
	: public detail::JointMethods<sizeof...(IContainers) - 1, DJoinedContainer<Qualifier, IContainers...>>,
	  public DJointLayerInfo<RankType(sizeof...(IContainers) - 1)>
{

	using JointLayerInfo = DJointLayerInfo<RankType(sizeof...(IContainers) - 1)>;

	static constexpr bool IsConst = std::is_const_v<Qualifier<int>>;

	//Qualifierがadd_const_tのとき、non-constなTraverserの扱いに困る。
	//実際にnon-const Traverserが使われることはないが、RttiFuncNodeなどの仮想関数がある以上インスタンス化されてしまう。
	//そうなると、Qualifierの食い違いにより一部constからnon-constへ変換できない場所が生じ、コンパイルエラーになる。
	//単純にExternalTraverserにQualifierを与えると、TraverserとConstTraverserが同じ型になってしまい、
	//同一の関数定義が2個存在するためにコンパイルエラーとなる。
	//この対処として、IsConst == trueのときはconstを付与しつつConstTraverserと異なる型とするため、
	//std::add_const_t<std::type_identity_t>>と重ねたものをExternalTraverserに与える。
	template <class T>
	using QualifierNCT = std::conditional_t<IsConst, std::add_const_t<std::type_identity_t<T>>, std::type_identity_t<T>>;

	friend class DJoinedRange<std::type_identity_t, IContainers...>;
	friend class DJoinedRange<std::add_const_t, IContainers...>;
public:
	static constexpr RankType MaxRank = sizeof...(IContainers) - 1;

	using Container = DJoinedContainer<Qualifier, IContainers...>;
	template <RankType Rank>
	using IContainer = GetType_t<Rank, IContainers...>;

	template <RankType Rank>
	using RttiPlaceholder = eval::RankedRttiPlaceholder<Rank, Container>;
	template <RankType Rank, class Type>
	using TypedPlaceholder = eval::RankedTypedPlaceholder<Rank, Type, Container>;
	template <RankType Rank, LayerType Layer, class Type>
	using CttiPlaceholder = eval::RankedCttiPlaceholder<Rank, Layer, Type, Container>;

	using Traverser = detail::ExternalTraverser<MaxRank, QualifierNCT, Container>;
	using ConstTraverser = detail::ExternalTraverser<MaxRank, std::add_const_t, Container>;
	using Sentinel = detail::ExternalSentinel<QualifierNCT, Container>;
	using ConstSentinel = detail::ExternalSentinel<std::add_const_t, Container>;
	using Range = DJoinedRange<std::type_identity_t, IContainers...>;
	using ConstRange = DJoinedRange<std::add_const_t, IContainers...>;

	using Traverser_prompt = detail::ExternalTraverser_prompt<MaxRank, QualifierNCT, Container>;
	using ConstTraverser_prompt = detail::ExternalTraverser_prompt<MaxRank, std::add_const_t, Container>;
	using Sentinel_prompt = detail::ExternalSentinel_prompt<QualifierNCT, Container>;
	using ConstSentinel_prompt = detail::ExternalSentinel_prompt<std::add_const_t, Container>;
	using Range_prompt = DJoinedRange_prompt<std::type_identity_t, IContainers...>;
	using ConstRange_prompt = DJoinedRange_prompt<std::add_const_t, IContainers...>;

	using JointInterface = detail::JointInterface_impl<Container>;

	template <class ...Args>
	struct GetBranchProxy
	{
	private:
		template <class Placeholder, size_t ...Indices>
		decltype(auto) GetField_impl(const Placeholder& ph, std::index_sequence<Indices...>) const
		{
			return m_stor->GetField(ph, std::get<Indices>(std::move(m_args))...);
		}

	public:
		template <RankType Rank>
		decltype(auto) GetField(const RttiPlaceholder<Rank>& ph) const
		{
			return GetField_impl(ph, std::make_index_sequence<sizeof...(Args)>{});
		}
		template <RankType Rank>
		decltype(auto) operator[](const RttiPlaceholder<Rank>& ph) const
		{
			return GetField_impl(ph, std::make_index_sequence<sizeof...(Args)>{});
		}
		template <RankType Rank, class Type>
		decltype(auto) GetField(const TypedPlaceholder<Rank, Type>& ph) const
		{
			return GetField_impl(ph, std::make_index_sequence<sizeof...(Args)>{});
		}
		template <RankType Rank, class Type>
		decltype(auto) operator[](const TypedPlaceholder<Rank, Type>& ph) const
		{
			return GetField_impl(ph, std::make_index_sequence<sizeof...(Args)>{});
		}
		template <RankType Rank, LayerType Layer, class Type>
		decltype(auto) GetField(const CttiPlaceholder<Rank, Layer, Type>& ph) const
		{
			return GetField_impl(ph, std::make_index_sequence<sizeof...(Args)>{});
		}
		template <RankType Rank, LayerType Layer, class Type>
		decltype(auto) operator[](const CttiPlaceholder<Rank, Layer, Type>& ph) const
		{
			return GetField_impl(ph, std::make_index_sequence<sizeof...(Args)>{});
		}
		const Container* m_stor;
		std::tuple<Args...> m_args;
	};

private:
	template <RankType N, class ...Stors_>
	void Construct(const JointLayerArray<MaxRank>& layers, const std::tuple<Stors_...>& cs)
	{
		if constexpr (N <= MaxRank)
		{
			if constexpr (N > 0)
			{
				m_find_bufs[N].Init(layers[N].first);
			}
			m_access_bufs[N].Init(std::get<N>(cs)->GetMaxLayer());
			Construct<N + 1>(layers, cs);
		}
	}
public:

	template <class ...Stors_>
	DJoinedContainer(const JointLayerArray<MaxRank>& layers, std::tuple<Stors_...> cs)
		: JointLayerInfo(layers), m_containers(cs)
	{
		Construct<0>(layers, cs);
	}

	DJoinedContainer(const DJoinedContainer&) = default;
	DJoinedContainer(DJoinedContainer&&) noexcept = default;

	DJoinedContainer& operator=(const DJoinedContainer&) = default;
	DJoinedContainer& operator=(DJoinedContainer&&) noexcept = default;

	static constexpr RankType GetMaxRank() { return MaxRank; }

	Range_prompt GetRange() const requires (!IsConst) { return Range_prompt(*this); }
	ConstRange_prompt GetRange() const requires IsConst { return ConstRange_prompt(*this); }
	Range_prompt GetRange(LayerType layer) const requires (!IsConst) { return Range_prompt(*this, layer); }
	ConstRange_prompt GetRange(LayerType layer) const requires IsConst { return ConstRange_prompt(*this, layer); }

	Range GetRange_delayed() const requires (!IsConst) { return Range(*this); }
	ConstRange GetRange_delayed() const requires IsConst { return ConstRange(*this); }
	Range GetRange_delayed(LayerType layer) const requires (!IsConst) { return Range(*this, layer); }
	ConstRange GetRange_delayed(LayerType layer) const requires IsConst { return ConstRange(*this, layer); }
	//const JointLayerArray<MaxRank>& GetJointLayers() const { return m_layers; }

	constexpr LayerType GetMaxLayer() const
	{
		return JointLayerInfo::template ConvertIntToExt<MaxRank>(std::get<MaxRank>(m_containers)->GetMaxLayer());
	}
	template <RankType Rank, StaticChar Name>
		requires s_hierarchy<IContainer<Rank>>
	static constexpr auto GetPlaceholder(StaticString<Name>)
	{
		return IContainer<Rank>::GetPlaceholder(StaticString<Name>{});
	}
	template <RankType Rank>
	RttiPlaceholder<Rank> GetPlaceholder(std::string_view name) const
	{
		auto iph = std::get<Rank>(m_containers)->GetPlaceholder(name);
		return RttiPlaceholder<Rank>(iph.GetInternalLayer(), iph.GetIndex(), iph.GetType(), iph.GetPtrOffset(), this->GetJointLayers());
	}
	template <RankType Rank, class ...Args>
		requires (Rank <= MaxRank)
	auto GetPlaceholders(const Args& ...names) const
	{
		static constexpr bool all_conv_string_view = (std::convertible_to<Args, std::string_view> && ...);
		static constexpr bool not_s_hierarchy = !s_hierarchy<IContainer<Rank>>;
		//以下の条件のどちらかを満たす場合、RttiPlaceholderなのでarrayで返す。
		//1. s_hierarchyでない。
		//2. 引数全てが、string_viewに変換可能な型。 
		if constexpr (all_conv_string_view || not_s_hierarchy)
			return std::array<RttiPlaceholder<Rank>, sizeof...(Args)>{ GetPlaceholder<Rank>(names)... };
		else
			return std::make_tuple(GetPlaceholder<Rank>(names)...);
	}

	template <placeholder PH>
	auto GetFieldName(const PH& ph) const
	{
		static constexpr RankType Rank = PH::Rank;
		return std::get<Rank>(m_containers)->GetFieldName(ph.Derank());
	}

	template <RankType Rank>
	bool TryJoin(const Bpos& bpos) const
	{
		//auto& s = this->GetContainer<Rank>();
		//auto& abuf = m_access_bufs[Rank];
		LayerType ujoint = this->template GetUJointLayer<Rank>();
		if constexpr (Rank == 0_rank) return true;
		else
		{
			if (!TryJoin<Rank - 1_rank>(bpos)) return false;
			//ujointが-1の場合、総当たり連結なので、joinする必要はない。
			if (ujoint == -1_layer) return true;
			else
			{
				auto& j = this->template GetJoint<Rank>();
				auto& res = m_find_bufs[Rank];
				return j->Find(*this, bpos, res);
			}
		}
	}
	template <RankType Rank>
	bool TryJoin(RankConstant<Rank>, const Bpos& bpos) const { return TryJoin<Rank>(bpos); }

private:
	//TargetRankの要素を検索する。
	template <RankType Rank, RankType TargetRank, class Placeholder>
	decltype(auto) Retrieve(const Placeholder& ph, const Bpos& bpos) const
	{
		auto& s = this->GetContainer<Rank>();
		auto& abuf = m_access_bufs[Rank];
		LayerType ujoint = this->template GetUJointLayer<Rank>();
		if constexpr (Rank > 0_rank)
		{
			//まずは自身より上のランクの検索を行う。
			Retrieve<Rank - 1_layer, TargetRank>(ph, bpos);
			//総当たり結合の場合など、upper joint layerは-1を取る可能性がある。
			//その場合、検索は意味がないので、この処理はujointが0以下の層の場合にのみ行う。
			if (ujoint != -1_layer)
			{
				auto& j = this->template GetJoint<Rank>();
				auto& res = m_find_bufs[Rank];
				if (!j->Find(*this, bpos, res)) throw JointError();
				//ここでresには発見された連結先Branchの位置情報が入っている。
				//ただし、ここからさらに本関数のbposから自身の位置情報を取り出し、
				//m_access_bufに与えなければならない。
				//このスコープではupper joint layerは0以下である。
				abuf.GetRow() = res.GetRow();
				for (LayerType l = 1_layer; l <= ujoint; ++l) abuf.GetTpos(l) = res.GetTpos(l);
			}
		}
		//さらに、bposから該当する位置を取り出す。
		if constexpr (Rank == TargetRank)
		{
			//TargetRankの場合、placeholderまでの位置を取り出してすぐさまGetBranchでよい。
			LayerType max = ph.GetInternalLayer();
			LayerType ext = this->template GetExtUJointLayer<Rank>() + 1_layer;
			assert(max <= this->template ConvertExtToInt<Rank>(bpos.GetLayer()));
			for (LayerType l = ujoint + 1_layer; l <= max; ++l, ++ext) abuf[l] = bpos[ext];
			return s.GetBranch(abuf, ph.GetInternalLayer())[ph.Derank()];
		}
		/*else
		{
			LayerType max = this->GetLJointLayer<Rank>();
			LayerType ext = this->GetExtUJointLayer<Rank>() + 1_layer;
			for (LayerType l = ujoint + 1_layer; l <= max; ++l, ++ext) abuf[l] = bpos[ext];
		}*/
	}
	template <RankType TargetRank, LayerType Size, class Placeholder, class Iter>
	decltype(auto) Retrieve(const Placeholder& ph, const Bpos& bpos, Iter it, Iter end) const
	{
		//現在、Traverserの仕様上、インデックス指定は最下位Containerに対して実行される。
		//望ましい動作ではないが、ここでもその仕様に合わせた実装にしておく。
		//if constexpr (TargetRank > 0_rank) Retrieve<TargetRank - 1, TargetRank>(ph, bpos);

		auto& s = this->GetContainer<TargetRank>();
		auto& abuf = m_access_bufs[TargetRank];
		LayerType ujoint = this->template GetUJointLayer<TargetRank>();
		//TargetRankより上の順位の場合は、普通にbposから検索を行う。
		if constexpr (TargetRank > 0)
		{
			//総当たり結合の場合など、upper joint layerは-1を取る可能性がある。
			//その場合、検索は意味がないので、この処理はujointが0以下の層の場合にのみ行う。
			if (ujoint != -1_layer)
			{
				auto& j = this->template GetJoint<TargetRank>();
				auto& res = m_find_bufs[TargetRank];
				if (!j->Find(*this, bpos, res)) throw JointError();
				abuf.GetRow() = res.GetRow();
				for (LayerType l = 1_layer; l <= ujoint; ++l) abuf.GetTpos(l);
			}
		}
		//bposから該当する位置を取り出す。ただしindex指定があるので、その分は除く。
		LayerType max = ph.GetInternalLayer();
		assert(ph.GetInternalLayer() <= s.GetMaxLayer());
		//TargetRank == MaxRankのときは、常にljointが-1なので、maxは更新しない。
		//if constexpr (TargetRank != MaxRank) max = std::min(this->GetLJointLayer<TargetRank>(), max);
		max -= Size;
		LayerType ext = this->template GetExtUJointLayer<TargetRank>() + 1_layer;
		for (LayerType l = ujoint + 1_layer; l <= max; ++l, ++ext) abuf[l] = bpos[ext];

		//さらに、Index指定分を与える。
		for (LayerType l = max + 1_layer; it != end; ++it, ++l) abuf[l] = (BindexType)*it;

		//TargetRankまでの検索を終えたら、その情報を用いてGetBranchを実行。
		return s.GetBranch(abuf, ph.GetInternalLayer())[ph.Derank()];
	}
public:
	//主としてIndexedFieldNodeから呼ばれるもの。ユーザーはあまり使うことはないだろう。
	template <RankType Rank, std::integral ...Indices>
	decltype(auto) GetField(const RttiPlaceholder<Rank>& ph, const Bpos& bpos, Indices ...indices) const
	{
		auto inds = { (BindexType)indices... };
		return Retrieve<Rank, sizeof...(Indices)>(ph, bpos, inds.begin(), inds.end());
	}
	template <RankType Rank>
	decltype(auto) GetField(const RttiPlaceholder<Rank>& ph, const Bpos& bpos) const
	{
		return Retrieve<Rank, Rank>(ph, bpos);
	}
	template <RankType Rank, std::integral ...Indices>
	decltype(auto) GetField(const RttiPlaceholder<Rank>& ph, Indices ...indices) const
	{
		return GetField(ph, Bpos{}, indices...);
	}
	decltype(auto) GetField(const RttiPlaceholder<0_rank>& ph) const
	{
		return std::get<0>(m_containers)->GetBranch()[ph.Derank()];
	}

	template <RankType Rank, class Type, std::integral ...Indices>
	decltype(auto) GetField(const TypedPlaceholder<Rank, Type>& ph, const Bpos& bpos, Indices ...indices) const
	{
		auto inds = { (BindexType)indices... };
		return Retrieve<Rank, sizeof...(Indices)>(ph, bpos, inds.begin(), inds.end());
	}
	template <RankType Rank, class Type>
	decltype(auto) GetField(const TypedPlaceholder<Rank, Type>& ph, const Bpos& bpos) const
	{
		return Retrieve<Rank, Rank>(ph, bpos);
	}
	template <RankType Rank, class Type, std::integral ...Indices>
	decltype(auto) GetField(const TypedPlaceholder<Rank, Type>& ph, Indices ...indices) const
	{
		return GetField(ph, Bpos{}, indices...);
	}
	template <class Type>
	decltype(auto) GetField(const TypedPlaceholder<0_rank, Type>& ph) const
	{
		return std::get<0>(m_containers)->GetBranch()[ph.Derank()];
	}

	template <RankType Rank, LayerType Layer, class Type, std::integral ...Indices>
	decltype(auto) GetField(const CttiPlaceholder<Rank, Layer, Type>& ph, const Bpos& bpos, Indices ...indices) const
	{
		auto inds = { (BindexType)indices... };
		return Retrieve<Rank, sizeof...(Indices)>(ph, bpos, inds.begin(), inds.end());
	}
	template <RankType Rank, LayerType Layer, class Type>
	decltype(auto) GetField(const CttiPlaceholder<Rank, Layer, Type>& ph, const Bpos& bpos) const
	{
		return Retrieve<Rank, Rank>(ph, bpos);
	}
	template <RankType Rank, LayerType Layer, class Type, std::integral ...Indices>
	decltype(auto) GetField(const CttiPlaceholder<Rank, Layer, Type>& ph, Indices ...indices) const
	{
		return GetField(ph, Bpos{}, indices...);
	}
	template <LayerType Layer, class Type>
	decltype(auto) GetField(const CttiPlaceholder<0_rank, Layer, Type>& ph) const
	{
		return std::get<0>(m_containers)->GetBranch()[ph.Derank()];
	}

	//JoinedContainerの場合、LayerTypeを与えても意味はない。
	GetBranchProxy<const Bpos&> GetBranch(const Bpos& bpos, LayerType = -1_layer) const
	{
		return GetBranchProxy<const Bpos&>{ this, { bpos } };
	}
	template <std::integral ...Indices>
	GetBranchProxy<BindexType, Indices...> GetBranch(BindexType index, Indices ...indices) const
	{
		return GetBranchProxy<BindexType, Indices...>{ this, { index, indices... } };
	}
	GetBranchProxy<> GetBranch() const
	{
		return GetBranchProxy<>{ this, {} };
	}

	BindexType GetSize() const
	{
		return GetContainer<0>().GetSize();
	}


	template <RankType Rank>
	auto tryjoin(RankConstant<Rank>) const
	{
		return eval::detail::MakeTryJoinNode<Rank>(*this);
	}

	template <RankType Rank>
	Qualifier<IContainer<Rank>>& GetContainer() const { return *std::get<Rank>(m_containers); }

private:

	//mutable Bpos m_current_pos;//現在の位置情報の一時保管場所。
	mutable std::array<Bpos, MaxRank + 1> m_find_bufs;//Findの結果を受け取るための一時的なバッファ。
	mutable std::array<Bpos, MaxRank + 1> m_access_bufs;//特定のブランチにアクセスするときに使う、位置情報格納用のバッファ。
	std::tuple<Qualifier<IContainers>*...> m_containers;
	//mutable std::array<std::unique_ptr<JointInterface>, MaxRank + 1> m_joints;
	//JointLayerArray<MaxRank> m_layers;
};

template <template <class> class Qualifier, container_simplex ...IContainers>
class DJoinedRange
{
public:

	static constexpr RankType MaxRank = sizeof...(IContainers) - 1;
	using Container = DJoinedContainer<Qualifier, IContainers...>;

	using Traverser = Container::Traverser;
	using ConstTraverser = Container::ConstTraverser;

	using Sentinel = Container::Sentinel;//のちのちSentinelもちゃんと作れ。
	using ConstSentinel = Container::ConstSentinel;

	static constexpr bool IsConst = std::is_const_v<Qualifier<char>>;

	DJoinedRange(const Container& s)
		: m_container(&s), m_trav_layer(-1)
	{}
	DJoinedRange(const Container& s, LayerType trav)
		: m_container(&s), m_trav_layer(trav)
	{}

	DJoinedRange(const DJoinedRange&) = default;
	DJoinedRange(DJoinedRange&&) noexcept = default;

	DJoinedRange& operator=(const DJoinedRange&) = default;
	DJoinedRange& operator=(DJoinedRange&&) noexcept = default;

	LayerType GetTravLayer() const { return m_trav_layer; }
	void SetTravLayer(LayerType layer) { m_trav_layer = layer; }
	void Assign(LayerType trav) { SetTravLayer(trav); }

	Traverser begin() const requires (!IsConst)
	{
		LayerInfo<MaxRank> l(m_container->GetJointLayers());
		l.EnableAndSetTravLayer(m_trav_layer);
		Traverser res(*m_container, l);
		auto clone_joints = [this, &res]<auto N>(auto self, Number<N>)
		{
			if constexpr (N > Container::MaxRank) return;
			else
			{
				if constexpr (N > 0) res.template SetJoint<N>(m_container->template CloneJoint<N>());
				return self(self, Number<N + (RankType)1>{});
			}
		};
		clone_joints(clone_joints, Number<(RankType)0>{});
		return res;
	}
	Sentinel end() const requires (!IsConst)
	{
		return Sentinel{};
	}

	ConstTraverser begin() const
	{
		return cbegin();
	}
	ConstSentinel end() const
	{
		return cend();
	}
	ConstTraverser cbegin() const
	{
		LayerInfo<MaxRank> l(m_container->GetJointLayers());
		l.EnableAndSetTravLayer(m_trav_layer);
		ConstTraverser res(*m_container, l);
		auto clone_joints = [this, &res]<auto N>(auto self, Number<N>)
		{
			if constexpr (N > Container::MaxRank) return;
			else
			{
				if constexpr (N > 0) res.template SetJoint<N>(m_container->template CloneJoint<N>());
				return self(self, Number<N + (RankType)1>{});
			}
		};
		clone_joints(clone_joints, Number<(RankType)0>{});
		return res;
	}
	ConstSentinel cend() const
	{
		return ConstSentinel{};
	}

	const Container& GetContainer() const { return *m_container; }

private:

	//Containerのconst有無はQualifierによって判断されるので、ここは常にconstでよい。
	const Container* m_container;
	LayerType m_trav_layer;
};

template <template <class> class Qualifier, container_simplex ...IContainers>
class DJoinedRange_prompt
{
public:

	static constexpr RankType MaxRank = sizeof...(IContainers) - 1;
	using Container = DJoinedContainer<Qualifier, IContainers...>;

	using Traverser = Container::Traverser_prompt;
	using ConstTraverser = Container::ConstTraverser_prompt;

	using Sentinel = Container::Sentinel_prompt;
	using ConstSentinel = Container::ConstSentinel_prompt;

	static constexpr bool IsConst = std::is_const_v<Qualifier<char>>;

	DJoinedRange_prompt(const Container& s)
		: m_container(&s), m_trav_layer(-1)
	{}
	DJoinedRange_prompt(const Container& s, LayerType trav)
		: m_container(&s), m_trav_layer(trav)
	{}

	DJoinedRange_prompt(const DJoinedRange_prompt&) = default;
	DJoinedRange_prompt(DJoinedRange_prompt&&) noexcept = default;

	DJoinedRange_prompt& operator=(const DJoinedRange_prompt&) = default;
	DJoinedRange_prompt& operator=(DJoinedRange_prompt&&) noexcept = default;

	LayerType GetTravLayer() const { return m_trav_layer; }
	void SetTravLayer(LayerType layer) { m_trav_layer = layer; }
	void Assign(LayerType trav) { SetTravLayer(trav); }

	Traverser begin() const requires (!IsConst)
	{
		LayerInfo<MaxRank> l(m_container->GetJointLayers());
		l.EnableAndSetTravLayer(m_trav_layer);
		Traverser res(*m_container, l);
		auto clone_joints = [this, &res]<auto N>(auto self, Number<N>)
		{
			if constexpr (N > Container::MaxRank) return;
			else
			{
				if constexpr (N > 0) res.template SetJoint<N>(m_container->template CloneJoint<N>());
				return self(self, Number<N + (RankType)1>{});
			}
		};
		clone_joints(clone_joints, Number<(RankType)0>{});
		return res;
	}
	Sentinel end() const requires (!IsConst)
	{
		return Sentinel{};
	}

	ConstTraverser begin() const
	{
		return cbegin();
	}
	ConstSentinel end() const
	{
		return cend();
	}
	ConstTraverser cbegin() const
	{
		LayerInfo<MaxRank> l(m_container->GetJointLayers());
		l.EnableAndSetTravLayer(m_trav_layer);
		ConstTraverser res(*m_container, l);
		auto clone_joints = [this, &res]<auto N>(auto self, Number<N>)
		{
			if constexpr (N > Container::MaxRank) return;
			else
			{
				if constexpr (N > 0) res.template SetJoint<N>(m_container->template CloneJoint<N>());
				return self(self, Number<N + 1>{});
			}
		};
		clone_joints(clone_joints, Number<(RankType)0>{});
		return res;
	}
	ConstSentinel cend() const
	{
		return ConstSentinel{};
	}

private:

	//Containerのconst有無はQualifierによって判断されるので、ここは常にconstでよい。
	const Container* m_container;
	LayerType m_trav_layer;
};
namespace detail
{

/*template <RankType MaxRank, class ...Joints, class ...Trees, std::pair<LayerType, LayerType> ...JLayers>
auto SJoin_ret(std::tuple<Joints...> joints, std::tuple<Trees*...> trees, LayerInfoInitializer<JLayers...>)
{
	return SJoinedTree<MaxRank, LayerInfoInitializer<JLayers...>,
		TypeList<std::decay_t<Joints>...>, TypeList<Trees...>>
		(std::move(joints), std::move(trees));
}
template <RankType Rank, class ...Joints, class ...Trees, std::pair<LayerType, LayerType> ...JLayers,
	class JointInfo, LayerType Upper, StaticTree Tree>
auto SJoin_impl(std::tuple<Joints...> joints, std::tuple<Trees*...> trees, LayerInfoInitializer<JLayers...>,
					JointInfo&& joint, LayerConstant<Upper>, Tree& tree)
{
	auto joints_ = std::tuple_cat(std::move(joints), std::forward_as_tuple(joint.Convert(tree)));
	auto trees_ = std::tuple_cat(std::move(trees), std::make_tuple(&tree));
	using JLayers_ = LayerInfoInitializer<JLayers..., std::pair<LayerType, LayerType>(Upper, tree.GetMaxLayer())>;
	return SJoin_ret<Rank>(std::move(joints_), std::move(trees_), JLayers_{});
}
template <RankType Rank, class ...Joints, class ...Trees, std::pair<LayerType, LayerType> ...JLayers,
	class JointInfo, LayerType Upper, StaticTree Tree, LayerType Lower, class ...Args>
auto SJoin_impl(std::tuple<Joints...> joints, std::tuple<Trees*...> trees, LayerInfoInitializer<JLayers...>,
				JointInfo&& joint, LayerConstant<Upper>, Tree& tree, LayerConstant<Lower>, Args&& ...args)
{
	auto joints_ = std::tuple_cat(std::move(joints), std::forward_as_tuple(joint.Convert(tree)));
	auto trees_ = std::tuple_cat(std::move(trees), std::make_tuple(&tree));
	using JLayers_ = LayerInfoInitializer<JLayers..., std::pair<LayerType, LayerType>(Upper, Lower)>;
	return SJoin_impl<Rank + 1>(std::move(joints_), std::move(trees_), JLayers_{}, std::forward<Args>(args)...);
}*/

template <template <class> class Qualifier, class Components>
struct DJoinedContainer_adapter;
template <template <class> class Qualifier, class ...Components>
struct DJoinedContainer_adapter<Qualifier, TypeList<Components...>>
{
	using Type = DJoinedContainer<Qualifier, Components...>;
};

template <class Container>
struct DJoin_DecayedContainers;
template <class ...Containers>
struct DJoin_DecayedContainers<std::tuple<Containers...>>
{
	using Type = TypeList<std::remove_cvref_t<std::remove_pointer_t<Containers>>...>;
};

template <RankType Rank, container_simplex Container>
auto DJoin_impl(LayerType upper, Container&& t)
{
	static_assert(Rank != 0);
	auto l = JointLayerArray<Rank>{};
	l[Rank] = { upper, -1 };
	auto s = std::make_tuple(&t);
	constexpr auto isconst = std::bool_constant<std::is_const_v<std::remove_reference_t<Container>>>{};
	return std::make_tuple(l, s, isconst);
}
template <RankType Rank, container_simplex Container, class ...Args>
auto DJoin_impl(LayerType upper, Container&& t, LayerType lower, Args&& ...args)
{
	static_assert(Rank != 0);
	auto [l, s, isc] = DJoin_impl<Rank + 1>(std::forward<Args>(args)...);
	constexpr bool lower_is_const = std::decay_t<decltype(isc)>::value;
	l[Rank] = { upper, lower };
	auto c = std::tuple_cat(std::make_tuple(&t), s);
	constexpr auto isconst = std::bool_constant<(lower_is_const || std::is_const_v<std::remove_reference_t<Container>>)>{};
	return std::make_tuple(l, c, isconst);
}
template <RankType Rank, container_simplex Container, class ...Args>
auto DJoin_impl(Container&& t, LayerType lower, Args&& ...args)
{
	static_assert(Rank == 0);
	auto [l, s, isc] = DJoin_impl<1>(std::forward<Args>(args)...);
	constexpr bool lower_is_const = std::decay_t<decltype(isc)>::value;
	l[Rank] = { -1, lower };
	auto c = std::tuple_cat(std::make_tuple(&t), s);
	constexpr auto isconst = std::bool_constant<(lower_is_const || std::is_const_v<std::remove_reference_t<Container>>)>{};
	using SList = DJoin_DecayedContainers<std::remove_cvref_t<decltype(c)>>::Type;
	if constexpr (isconst)
		return typename DJoinedContainer_adapter<std::add_const_t, SList>::Type(l, c);
	else
		return typename DJoinedContainer_adapter<std::type_identity_t, SList>::Type(l, c);
}

}

/*template <StaticTree Tree, LayerType Lower, class ...Args>
auto Join(Tree& t, LayerConstant<Lower> lower, Args&& ...args)
{
	return detail::SJoin_impl<1>(
		std::forward_as_tuple(TopJoint()), std::make_tuple(&t),
		detail::LayerInfoInitializer<std::pair<LayerType, LayerType>(-1, lower)>(),
		std::forward<Args>(args)...);
}*/
template <container_simplex Container, class ...Args>
auto Join(Container& t, LayerType lower, Args&& ...args)
{
	return detail::DJoin_impl<0>(t, lower, std::forward<Args>(args)...);
}

}

#endif
