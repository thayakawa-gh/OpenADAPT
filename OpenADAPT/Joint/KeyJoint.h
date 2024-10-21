#ifndef ADAPT_EVALUATOR_KEYJOINT_H
#define ADAPT_EVALUATOR_KEYJOINT_H

#include <map>
#include <memory>
#include <concepts>
#include <OpenADAPT/Common/Concepts.h>
#include <OpenADAPT/Evaluator/Function.h>
#include <OpenADAPT/Joint/JointInterface.h>

namespace adapt
{

template <RankType Rank, class Container, class Types, class Nodes>
struct KeyJoint;
template <RankType Rank, class Container, class ...Types, node_or_placeholder ...Nodes>
struct KeyJoint<Rank, Container, TypeList<Types...>, TypeList<Nodes...>>
	: public detail::JointInterface_impl<Container>
{
	//Containerは場合によっては不完全型の可能性がある。
	//というのも、Jointをstaticに与える場合、
	//KeyJointが実体化されないことにはContainerの方も実体化できないため。
	//dynamicの場合は完全型になりうるが、一応不完全型を想定して書く。

	using Traverser = Container::Traverser;
	using ConstTraverser = Container::ConstTraverser;

private:
	using JointInterface = detail::JointInterface_impl<Container>;
public:

	KeyJoint() = default;
	template <class Hash_, node_or_placeholder ...Nodes_>
	KeyJoint(Hash_&& hash, Nodes_&& ...keys)
		: m_keys(std::forward<Nodes_>(keys)...), m_hash(std::make_shared<Hashtable<Types...>>(std::forward<Hash_>(hash)))
	{}
	template <node_or_placeholder ...Nodes_>
	KeyJoint(const Hashtable<Types...>&hash, const std::tuple<Nodes_...>&keys)
		: m_keys(keys), m_hash(std::make_shared<Hashtable<Types...>>(hash))
	{}
	template <node_or_placeholder ...Nodes_>
	KeyJoint(const std::shared_ptr<Hashtable<Types...>>& hash, const std::tuple<Nodes_...>&keys)
		: m_keys(keys), m_hash(hash)
	{}

	virtual ~KeyJoint() = default;

	template <node_or_placeholder ...NPs>
	void SetKey(NPs&& ...keys)
	{
		m_keys = std::make_tuple(eval::detail::ConvertToNode(std::forward<NPs>(keys))...);
	}
	template <class Hash_>
		requires std::same_as<Hashtable<Types...>, std::remove_cvref_t<Hash_>>
	void SetHashtable(Hash_&& hash)
	{
		m_hash = std::make_shared<Hashtable<Types...>>(std::forward<Hash_>(hash));
	}

private:
	template <size_t I, class ...Args>
	void Init_impl([[maybe_unused]] const Args& ...args)
	{
		if constexpr (any_node<GetType_t<I, Nodes...>>)
			std::get<I>(m_keys).Init(args...);
		if constexpr (I < sizeof...(Nodes) - 1)
			Init_impl<I + 1>(args...);
	}
public:
	virtual void Init(const Traverser& t, const std::vector<std::tuple<const void*, const Bpos*, bool>>& outer_t) override { Init_impl<0>(t, outer_t); }
	virtual void Init(const ConstTraverser& t, const std::vector<std::tuple<const void*, const Bpos*, bool>>& outer_t) override { Init_impl<0>(t, outer_t); }
	virtual void Init(const Container& s, const Bpos& bpos) override { Init_impl<0>(s, bpos); }
	virtual void Init() override { Init_impl<0>(); }
private:
	template <size_t I, class Type, class ...Args>
	decltype(auto) Eval(const Args& ...args) const
	{
		auto& key = std::get<I>(m_keys);
		if constexpr (std::same_as<Type, FieldVariant>)
			return key.Evaluate(args...).var();
		else
		{
			if constexpr (rtti_node_or_placeholder<decltype(key)>)
				return key.Evaluate(args...).template as<Type>();
			else
				return key.Evaluate(args...);
		}
	}
	template <class ...Args> requires (sizeof...(Args) > 1)
	static decltype(auto) MakeKey(Args&& ...args) { return std::tuple<Args...>(std::forward<Args>(args)...); }
	template <class Arg>
	static decltype(auto) MakeKey(Arg&& arg) { return std::forward<Arg>(arg); }
	template <size_t ...Indices, class ...Args>
	bool Find_impl(Bpos& bpos, std::index_sequence<Indices...>, const Args& ...args) const
	{
		//auto key = MakeKey(std::get<Indices>(m_keys).Evaluate(args...).template to<Types>()...);
		auto key = MakeKey(Eval<Indices, Types>(args...)...);
		auto it = m_hash->find(key);
		if (it == m_hash->end()) return false;
		assert(it->second.GetLayer() >= bpos.GetLayer());
		bpos.Assign(it->second);
		return true;
	}
public:

	virtual bool Find(const Traverser& t, Bpos& bpos) const override { return Find_impl(bpos, std::make_index_sequence<sizeof...(Nodes)>{}, t); }
	virtual bool Find(const ConstTraverser& t, Bpos& bpos) const override { return Find_impl(bpos, std::make_index_sequence<sizeof...(Nodes)>{}, t); }
	//virtual bool Find(const Container& s, Bpos& res) const { throw Terminate(""); }//return Find_impl(res, std::make_index_sequence<sizeof...(Nodes)>{}, s); }
	virtual bool Find(const Container& s, const Bpos& bpos, Bpos& res) const override { return Find_impl(res, std::make_index_sequence<sizeof...(Nodes)>{}, s, bpos); }

	virtual std::unique_ptr<JointInterface> Clone() const override
	{
		return std::unique_ptr<JointInterface>(new KeyJoint(m_hash, m_keys));
	}

private:
	std::tuple<Nodes...> m_keys;
	std::shared_ptr<Hashtable<Types...>> m_hash;
};


namespace detail
{

template <size_t I, class Type, class ...Types, class Hash, class Key, class ...Keys>
constexpr bool CheckKeyTypeMatching(TypeList<Type, Types...>, const Hash& hash, const Key& key, const Keys& ...keys)
{
	if constexpr (DFieldInfo::ValueTypeToTagType<Type>() != FieldType::Emp)
	{
		if (DFieldInfo::ValueTypeToTagType<Type>() != key.GetType()) return false;
	}
	else
	{
		const auto& firstkey = hash.begin()->first;
		if (std::get<I>(firstkey).GetType() != key.GetType()) return false;
	}
	if constexpr (sizeof...(Types) > 0) return CheckKeyTypeMatching<I + 1>(TypeList<Types...>{}, hash, keys...);
	else return true;
}

template <RankType Rank, class Container, class ...Types,
	class Hash, rtti_node_or_placeholder ...Keys>
auto MakeKeyJoint(TypeList<Types...>, Hash&& hash, Keys&& ...keys)
{
	//rtti用
	if (!CheckKeyTypeMatching<0>(TypeList<Types...>{}, hash, keys...))
		throw MismatchType("");
	using Joint = KeyJoint<Rank, Container, TypeList<Types...>, TypeList<std::remove_cvref_t<Keys>...>>;
	std::unique_ptr<JointInterface_impl<Container>> j =
		std::make_unique<Joint>(std::forward<Hash>(hash), std::forward<Keys>(keys)...);
	return std::move(j);
}

}

}

#endif