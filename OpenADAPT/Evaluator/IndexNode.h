#ifndef ADAPT_EVALUATOR_INDEX_NODE_H
#define ADAPT_EVALUATOR_INDEX_NODE_H

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

namespace detail
{

template <RankType Rank_, LayerType Layer_, class Container_>
class CttiIndexNode
{
public:
	using Container = Container_;
	using Traverser = Container::Traverser;
	using ConstTraverser = Container::ConstTraverser;

	using RetType = BindexType;

	static constexpr RankType Rank = Rank_;
	static constexpr RankType MaxRank = Container::MaxRank;

	static constexpr LayerType Layer = Layer_;

	CttiIndexNode IncreaseDepth() const { return *this; }

	template <class Trav>
	void Init(const Trav& t, const std::vector<std::tuple<const void*, const Bpos*, bool>>&)
	{}
	template <class Trav>
	void Init(const Trav& t)
	{
		Init(t, {});
	}

	static constexpr JointLayerArray<MaxRank> GetJointLayerArray()
	{
		if constexpr (MaxRank > 0) return Container::GetJointLayers();
		else return JointLayerArray<MaxRank>{};
	}
	static constexpr LayerInfo<MaxRank> GetLayerInfo()
	{
		return GetLayerInfo(LayerInfo<MaxRank>(GetJointLayerArray()));
	}
	static constexpr LayerInfo<MaxRank> GetLayerInfo(LayerInfo<MaxRank> eli)
	{
		auto res = eli;
		res.Enable<Rank>();
		res.SetTravLayer<Rank>(Layer);
		return res;
	}
	static constexpr LayerInfo<MaxRank> GetLayerInfo(LayerInfo<MaxRank> eli, DepthType)
	{
		return eli;
	}
	static constexpr LayerType GetLayer()
	{
		return GetLayerInfo().GetTravLayer();
	}

	BindexType Evaluate(const Traverser& t) const { return t.GetPos(Layer); }
	BindexType Evaluate(const ConstTraverser& t) const { return t.GetPos(Layer); }

	/*BindexType Evaluate(const Container&, const Bpos& bpos) const
	{
		return bpos[Layer];
	}
	BindexType Evaluate(const Container&) const
	{
		static_assert(Layer == -1_layer);
		return 0;
	}*/

}


}

}

}
