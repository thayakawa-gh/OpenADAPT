#ifndef ADAPT_JOINT_CROSSJOINT_H
#define ADAPT_JOINT_CROSSJOINT_H

#include <map>
#include <memory>
#include <concepts>
#include <OpenADAPT/Common/Concepts.h>
#include <OpenADAPT/Evaluator/Function.h>
#include <OpenADAPT/Joint/JointInterface.h>

namespace adapt
{

template <RankType Rank, class Container>
struct CrossJoint : public detail::JointInterface_impl<Container>
{
	using Traverser = Container::Traverser;
	using ConstTraverser = Container::ConstTraverser;

private:
	using JointInterface = detail::JointInterface_impl<Container>;
public:

	CrossJoint() = default;

	virtual ~CrossJoint() = default;

	virtual void Init(const Traverser&, const std::vector<std::tuple<const void*, const Bpos*, bool>>&) override {}
	virtual void Init(const ConstTraverser&, const std::vector<std::tuple<const void*, const Bpos*, bool>>&) override {}
	virtual void Init(const Container&, const Bpos&) override {}
	virtual void Init() override {}

	virtual bool Find(const Traverser&, Bpos& bpos) const override { bpos.Init(-1_layer); return true; }
	virtual bool Find(const ConstTraverser&, Bpos& bpos) const override { bpos.Init(-1_layer); return true; }
	//virtual bool Find(const Container& s, Bpos& res) const { throw Terminate(""); }//return Find_impl(res, std::make_index_sequence<sizeof...(Nodes)>{}, s); }
	virtual bool Find(const Container&, const Bpos&, Bpos& res) const override { res.Init(-1_layer); return true; }

	virtual std::unique_ptr<JointInterface> Clone() const override
	{
		return std::unique_ptr<JointInterface>(new CrossJoint());
	}
};

template <RankType Rank, class Container>
std::unique_ptr<detail::JointInterface_impl<Container>> MakeCrossJoint()
{
	return std::make_unique<CrossJoint<Rank, Container>>();
}

}

#endif