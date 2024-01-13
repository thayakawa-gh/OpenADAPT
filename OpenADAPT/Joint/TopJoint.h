#ifndef ADAPT_EVALUATOR_TOPJOINT_H
#define ADAPT_EVALUATOR_TOPJOINT_H

namespace adapt
{

namespace detail
{

template <class ExternalTraverser>
class TopJoint_impl
{

};

}

template <class Container_>
class TopJoint
{
public:

	using Container = Container_;
	using Traverser = typename Container::Traverser;
	using ConstTraverser = typename Container::ConstTraverser;
};

//class TopJointInfo {};

//auto TopJoint() { return TopJointInfo{}; }

}

#endif