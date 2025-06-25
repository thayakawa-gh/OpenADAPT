#ifndef ADAPT_JOINT_JOINTINTERFACE_H
#define ADAPT_JOINT_JOINTINTERFACE_H

#include <vector>
#include <memory>
#include <OpenADAPT/Common/Bpos.h>
#include <OpenADAPT/Common/Concepts.h>

namespace adapt
{

namespace detail
{

//Jointを仮想関数で呼び分けたいときのためのインターフェイス。
template <class Container_>
class JointInterface_impl
{
public:

	using Container = Container_;
	using Traverser = Container::Traverser;
	using ConstTraverser = Container::ConstTraverser;

	JointInterface_impl() = default;
	virtual ~JointInterface_impl() = default;

	virtual void Init(const Traverser& t, const std::vector<std::tuple<const void*, const Bpos*, bool>>& outer_t) = 0;
	virtual void Init(const ConstTraverser& t, const std::vector<std::tuple<const void*, const Bpos*, bool>>& outer_t) = 0;
	virtual void Init(const Container& s, const Bpos& bpos) = 0;
	virtual void Init() = 0;
	virtual bool Find(const Traverser& t, Bpos& bpos) const = 0;
	virtual bool Find(const ConstTraverser& t, Bpos& bpos) const = 0;
	//virtual bool Find(const Container& s, Bpos& res) const = 0;
	virtual bool Find(const Container& s, const Bpos& bpos, Bpos& res) const = 0;

	virtual std::unique_ptr<JointInterface_impl> Clone() const = 0;
};


}

}

#endif