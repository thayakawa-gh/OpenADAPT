#ifndef ADAPT_EVALUATOR_TRAV_INFO_NODE_H
#define ADAPT_EVALUATOR_TRAV_INFO_NODE_H

#include <OpenADAPT/Evaluator/NodeBase.h>

namespace adapt
{

namespace eval
{

ADAPT_EXPORT
template <class LayerSD, class Container_>
class CttiPosNode : public detail::CttiMethods<CttiPosNode<LayerSD, Container_>, std::add_const_t>
{
public:

	using Container = Container_;
	using Traverser = Container::Traverser;
	using ConstTraverser = Container::ConstTraverser;

	using RetType = BindexType;

	static constexpr RankType MaxRank = Container::MaxRank;

	//index nodeのCtti判定はちょっとややこしい。
	//container_simplexの場合、LayerSDがLayerConstantであればCttiとみなして良い。
	//ただjoined_containerの場合、DJoinedContainerは原理的にCttiにできないため、
	//SJoinedContainerであることを要求する必要がある。
	static constexpr bool IsCtti = (container_simplex<Container> || s_container<Container>) && value_specialization_of<LayerSD, LayerConstant>;

	CttiPosNode() = default;
	CttiPosNode(const LayerSD& layer, const Container& c) : m_layer(layer), m_container(&c) {}

	CttiPosNode IncreaseDepth() const { return *this; }

	static constexpr JointLayerArray<MaxRank> GetJointLayerArray() requires IsCtti
	{
		if constexpr (MaxRank > 0) return Container::GetJointLayers();
		else return JointLayerArray<MaxRank>{};
	}
	JointLayerArray<MaxRank> GetJointLayerArray() const requires (!IsCtti)
	{
		if constexpr (MaxRank > 0) return m_container->GetJointLayers();
		else return JointLayerArray<MaxRank>{};
	}
	static constexpr LayerInfo<MaxRank> GetLayerInfo() requires IsCtti { return GetLayerInfo(LayerInfo<MaxRank>(GetJointLayerArray())); }
	LayerInfo<MaxRank> GetLayerInfo() const requires (!IsCtti) { return GetLayerInfo(LayerInfo<MaxRank>(GetJointLayerArray())); }
	static constexpr LayerInfo<MaxRank> GetLayerInfo(LayerInfo<MaxRank> eli) requires IsCtti
	{
		auto res = eli;
		res.EnableAndSetTravLayer(LayerSD{}, false);
		return res;
	}
	LayerInfo<MaxRank> GetLayerInfo(LayerInfo<MaxRank> eli) const requires (!IsCtti)
	{
		auto res = eli;
		res.EnableAndSetTravLayer(m_layer, false);
		return res;
	}
	static constexpr LayerInfo<MaxRank> GetLayerInfo(LayerInfo<MaxRank> eli, DepthType) { return eli; }
	static constexpr LayerType GetLayer() requires IsCtti { return GetLayerInfo().GetTravLayer(); }
	LayerType GetLayer() const requires (!IsCtti) { return GetLayerInfo().GetTravLayer(); }

	void Init(const Traverser&, const std::vector<std::tuple<const void*, const Bpos*, bool>>&) {}
	void Init(const ConstTraverser&, const std::vector<std::tuple<const void*, const Bpos*, bool>>&) {}
	void Init(const Traverser&) {}
	void Init(const ConstTraverser&) {}
	void Init(const Container&, const Bpos&) {}
	void Init(const Container&) {}
	void Init() {}

	RetType Evaluate(const Traverser& t) const { return t.GetPos(m_layer); }
	RetType Evaluate(const ConstTraverser& t) const { return t.GetPos(m_layer); }
	RetType Evaluate(const Container&) const { throw NoElements(); }
	RetType Evaluate([[maybe_unused]] const Container& c, const Bpos& bpos) const { return bpos[m_layer]; }

	RetType operator()(const Traverser& t) const { return Evaluate(t); }
	RetType operator()(const ConstTraverser& t) const { return Evaluate(t); }
	RetType operator()(const Container& t) const { return Evaluate(t); }
	RetType operator()(const Container& t, const Bpos& bpos) const { return Evaluate(t, bpos); }

private:

	[[no_unique_address]] LayerSD m_layer = {};
	const Container* m_container = nullptr;
};

ADAPT_EXPORT
template <class FromLayerSD, class ToLayerSD, class Container_>
class CttiSizeNode : public detail::CttiMethods<CttiSizeNode<FromLayerSD, ToLayerSD, Container_>, std::add_const_t>
{
public:

	using Container = Container_;
	using Traverser = Container::Traverser;
	using ConstTraverser = Container::ConstTraverser;

	using RetType = BindexType;

	static constexpr RankType MaxRank = Container::MaxRank;

	//index nodeのCtti判定はちょっとややこしい。
	//container_simplexの場合、LayerSDがLayerConstantであればCttiとみなして良い。
	//ただjoined_containerの場合、DJoinedContainerは原理的にCttiにできないため、
	//SJoinedContainerであることを要求する必要がある。
	static constexpr bool IsCtti = (container_simplex<Container> || s_container<Container>) &&
									value_specialization_of<FromLayerSD, LayerConstant> && value_specialization_of<ToLayerSD, LayerConstant>;

	CttiSizeNode() = default;
	CttiSizeNode(FromLayerSD from, ToLayerSD to, const Container& c) : m_from(from), m_to(to), m_container(&c) {}

	CttiSizeNode IncreaseDepth() const { return *this; }

	static constexpr JointLayerArray<MaxRank> GetJointLayerArray() requires IsCtti
	{
		if constexpr (MaxRank > 0) return Container::GetJointLayers();
		else return JointLayerArray<MaxRank>{};
	}
	JointLayerArray<MaxRank> GetJointLayerArray() const requires (!IsCtti)
	{
		if constexpr (MaxRank > 0) return m_container->GetJointLayers();
		else return JointLayerArray<MaxRank>{};
	}
	static constexpr LayerInfo<MaxRank> GetLayerInfo() requires IsCtti { return GetLayerInfo(LayerInfo<MaxRank>(GetJointLayerArray())); }
	LayerInfo<MaxRank> GetLayerInfo() const requires (!IsCtti) { return GetLayerInfo(LayerInfo<MaxRank>(GetJointLayerArray())); }
	static constexpr LayerInfo<MaxRank> GetLayerInfo(LayerInfo<MaxRank> eli) requires IsCtti
	{
		auto res = eli;
		res.EnableAndSetTravLayer(FromLayerSD{}, false);
		return res;
	}
	LayerInfo<MaxRank> GetLayerInfo(LayerInfo<MaxRank> eli) const requires (!IsCtti)
	{
		auto res = eli;
		res.EnableAndSetTravLayer(m_from, false);
		return res;
	}
	static constexpr LayerInfo<MaxRank> GetLayerInfo(LayerInfo<MaxRank> eli, DepthType) { return eli; }
	static constexpr LayerType GetLayer() requires IsCtti { return GetLayerInfo().GetTravLayer(); }
	LayerType GetLayer() const requires (!IsCtti) { return GetLayerInfo().GetTravLayer(); }

	void Init(const Traverser&, const std::vector<std::tuple<const void*, const Bpos*, bool>>&) {}
	void Init(const ConstTraverser&, const std::vector<std::tuple<const void*, const Bpos*, bool>>&) {}
	void Init(const Traverser&) {}
	void Init(const ConstTraverser&) {}
	void Init(const Container&, const Bpos&) {}
	void Init(const Container&) {}
	void Init() {}

	// ExternalTraverserをこの関数に対応させるのは骨が折れる。
	// というのも、fromからtoまでの範囲をすべて一時的に連結して全数を数える必要があるため。
	// それだったら階層関数のsizeでもあんまりコストに差が出てこない。
	RetType Evaluate(const Traverser& t) const { return t.GetSize(m_from, m_to); }
	RetType Evaluate(const ConstTraverser& t) const { return t.GetSize(m_from, m_to); }
	RetType Evaluate(const Container&) const { throw NoElements(); }
	RetType Evaluate(const Container& c, const Bpos& bpos) const { return c.GetBranch(bpos, m_from).GetSize(m_to); }

	RetType operator()(const Traverser& t) const { return Evaluate(t); }
	RetType operator()(const ConstTraverser& t) const { return Evaluate(t); }
	RetType operator()(const Container& t) const { return Evaluate(t); }
	RetType operator()(const Container& t, const Bpos& bpos) const { return Evaluate(t, bpos); }

private:

	[[no_unique_address]] FromLayerSD m_from = {};
	[[no_unique_address]] ToLayerSD m_to = {};
	const Container* m_container = nullptr;
};



}

}

#endif