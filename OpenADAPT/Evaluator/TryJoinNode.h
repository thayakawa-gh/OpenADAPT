#ifndef ADAPT_EVALUATOR_TRYJOINNODE_H
#define ADAPT_EVALUATOR_TRYJOINNODE_H

#include <OpenADAPT/Evaluator/NodeBase.h>

namespace adapt
{

namespace eval
{

namespace detail
{

//
template <RankType Rank, class Container_>
class CttiTryJoinNode : public CttiMethods<CttiTryJoinNode<Rank, Container_>, std::add_const_t>
{
public:

	using RetType = bool;
	using Container = Container_;
	using Traverser = Container::Traverser;
	using ConstTraverser = Container::ConstTraverser;

	static constexpr RankType MaxRank = Container::MaxRank;

	static constexpr bool IsCtti = (s_container<Container>);

	CttiTryJoinNode() : m_container(nullptr) {}
	CttiTryJoinNode(const Container& c) : m_container(&c) {}

	CttiTryJoinNode IncreaseDepth() const { return std::move(*this); }

	static constexpr JointLayerArray<MaxRank> GetJointLayerArray() requires IsCtti { return Container::GetJointLayers(); }

	JointLayerArray<MaxRank> GetJointLayerArray() const requires (!IsCtti) { return m_container->GetJointLayers(); }

	//CttiとTypedとを分けるのは引数のないGetLayerInfoだけで十分。それ以外は受け取ったeliが連結情報を持っている。
	static constexpr LayerInfo<MaxRank> GetLayerInfo() requires IsCtti { return GetLayerInfo(LayerInfo<MaxRank>(GetJointLayerArray())); }
	LayerInfo<MaxRank> GetLayerInfo() const requires (!IsCtti) { return GetLayerInfo(LayerInfo<MaxRank>(GetJointLayerArray())); }
	static constexpr LayerInfo<MaxRank> GetLayerInfo(LayerInfo<MaxRank> eli)
	{
		//TryJoinは階層を持たないが、少なくともRankまでの連結を行う関係でRankのfixed層とみなされる。
		auto res = eli;
		res.template Enable<Rank>();
		res.template SetTravLayer<Rank>(eli.GetInternal(Rank).GetFixedLayer());
		return res;
	}
	static constexpr LayerInfo<MaxRank> GetLayerInfo(LayerInfo<MaxRank> eli, DepthType) { return eli; }
	static constexpr LayerType GetLayer() requires IsCtti { return GetLayerInfo().GetTravLayer(); }
	LayerType GetLayer() const requires (!IsCtti) { return GetLayerInfo().GetTravLayer(); }


	void Init(const Traverser&, const std::vector<std::tuple<const void*, const Bpos*, bool>>&) {}
	void Init(const Traverser&) {}
	void Init(const ConstTraverser&, const std::vector<std::tuple<const void*, const Bpos*, bool>>&) {}
	void Init(const ConstTraverser&) {}
	void Init(const Container&, const Bpos&) {}
	void Init(const Container&) {}
	void Init() {}

	bool Evaluate(const Traverser& t) const
	{
		return t.TryJoin<Rank>();
	}
	bool Evaluate(const ConstTraverser& t) const
	{
		return t.TryJoin<Rank>();
	}

	bool Evaluate(const Container&) const
	{
		//-1層へのアクセスのときにJoinはできない。
		throw JointError();
	}
	bool Evaluate(const Container& c, const Bpos& bpos) const
	{
		return c.TryJoin<Rank>(bpos);
	}

	bool operator()(const Traverser& t) const { return Evaluate(t); }
	bool operator()(const ConstTraverser& t) const { return Evaluate(t); }
	bool operator()(const Container& t) const { return Evaluate(t); }
	bool operator()(const Container& t, const Bpos& bpos) const { return Evaluate(t, bpos); }

private:

	const Container* m_container;
};

template <RankType Rank, joined_container Container>
CttiTryJoinNode<Rank, Container> MakeTryJoinNode(const Container& c)
{
	return CttiTryJoinNode<Rank, Container>(c);
}

}

}

}


#endif