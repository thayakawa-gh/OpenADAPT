#ifndef ADAPT_EVALUATOR_JOINTBASE_H
#define ADAPT_EVALUATOR_JOINTBASE_H

#include <vector>
#include <memory>
#include <OpenADAPT/Common/Bpos.h>

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

	virtual void Init(const Traverser& t, const std::vector<const void*>& outer_t) = 0;
	virtual void Init(const ConstTraverser& t, const std::vector<const void*>& outer_t) = 0;
	virtual void Init(const Container& s, const Bpos& bpos) = 0;
	virtual void Init() = 0;
	virtual bool Find(const Traverser& t, Bpos& bpos) const = 0;
	virtual bool Find(const ConstTraverser& t, Bpos& bpos) const = 0;
	//virtual bool Find(const Container& s, Bpos& res) const = 0;
	virtual bool Find(const Container& s, const Bpos& bpos, Bpos& res) const = 0;

	virtual std::unique_ptr<JointInterface_impl> Clone() const = 0;
};

template <RankType MaxRank_, class Container>
class JointMethods
{
public:
	static constexpr RankType MaxRank = MaxRank_;
	//using Traverser = detail::ExternalTraverser<MaxRank, std::type_identity_t, Container>;
	//using ConstTraverser = detail::ExternalTraverser<MaxRank, std::add_const_t, Container>;
	using JointInterface = JointInterface_impl<Container>;

	void CopyFrom(const JointMethods& that)
	{
		for (auto [r, j, t] : views::Enumerate(m_joints, that.m_joints))
		{
			if (r == 0) continue;//rank 0の場合、jointは常にnullptrである。
			j = t->Clone();
		}
	}

	template <RankType Rank, class ...Types, node_or_placeholder ...Keys>
		requires (Rank > 0) && ((std::decay_t<Keys>::Rank < Rank) && ...)
	void SetKeyJoint(const Hashtable<Types...>& hash, Keys&& ...keys)
	{
		auto j = MakeKeyJoint<Rank, Container>
			(TypeList<Types...>{}, hash, std::forward<Keys>(keys)...);
		m_joints[Rank] = std::move(j);
	}
	template <RankType Rank, class ...Types, node_or_placeholder ...Keys>
		requires (Rank > 0) && ((std::decay_t<Keys>::Rank < Rank) && ...)
	void SetKeyJoint(Hashtable<Types...>&& hash, Keys&& ...keys)
	{
		auto j = MakeKeyJoint<Rank, Container>
			(TypeList<Types...>{}, std::move(hash), std::forward<Keys>(keys)...);
		m_joints[Rank] = std::move(j);
	}
	template <RankType Rank, node_or_placeholder ...KeysLeft, node_or_placeholder ...KeysRight>
		requires (((std::decay_t<KeysLeft>::Rank < Rank) && ...) && !(ranked_placeholder<KeysRight> || ...))
	void SetKeyJoint(std::tuple<KeysLeft...> left, std::tuple<KeysRight...> right)
	{
		const auto& self = static_cast<const Container&>(*this);
		const auto& cr = self.template GetContainer<Rank>();
		auto hash = std::apply(
			[&cr]<class ...Keys>(Keys&& ...keys) { return cr | MakeHashtable(std::forward<Keys>(keys)...); },
			std::move(right));
		std::apply([this, &hash]<class ...Keys>(Keys&& ...keys) mutable
		{
			//this->を消すとclangが何故か"キャプチャされたthisが使われていない"と警告を出す。バグだと思うがうるさいので明示する。
			this->SetKeyJoint<Rank>(std::move(hash), std::forward<Keys>(keys)...);
		},
			std::move(left));
	}
	template <RankType Rank, node_or_placeholder KeyLeft, node_or_placeholder KeyRight>
		requires (std::decay_t<KeyLeft>::Rank < Rank && !ranked_placeholder<KeyRight>)
	void SetKeyJoint(KeyLeft&& left, KeyRight&& right)
	{
		SetKeyJoint<Rank>(std::forward_as_tuple(std::forward<KeyLeft>(left)), std::forward_as_tuple(std::forward<KeyRight>(right)));
	}


	template <RankType Rank>
	void SetJoint(std::unique_ptr<JointInterface>&& joint) { static_assert(Rank != 0); SetJoint(Rank, std::move(joint)); }
	void SetJoint(RankType rank, std::unique_ptr<JointInterface>&& joint) { assert(rank != 0); m_joints[rank] = std::move(joint); }
	template <RankType Rank>
	std::unique_ptr<JointInterface> CloneJoint() const
	{
		static_assert(Rank != 0);
		return CloneJoint(Rank);
	}
	std::unique_ptr<JointInterface> CloneJoint(RankType rank) const
	{
		assert(rank != 0);
		const auto& j = m_joints[rank];
		if (!j) throw NotInitialized(std::format("Rank<{}> joint is not initialized.", rank));
		return j->Clone();
	}

protected:

	template <RankType Rank>
	std::unique_ptr<JointInterface>& GetJoint() { return m_joints[Rank]; }
	template <RankType Rank>
	const std::unique_ptr<JointInterface>& GetJoint() const { return m_joints[Rank]; }

private:
	std::array<std::unique_ptr<JointInterface>, MaxRank + 1> m_joints;
};

}

}

#endif