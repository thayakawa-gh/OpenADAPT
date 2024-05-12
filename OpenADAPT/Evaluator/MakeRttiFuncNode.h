#ifndef ADAPT_EVALUATOR_MAKE_FUNC_NODE_H
#define ADAPT_EVALUATOR_MAKE_FUNC_NODE_H

#include <OpenADAPT/Evaluator/FuncNode.h>
#include <OpenADAPT/Evaluator/MakeRttiFuncNode_gen.h>

namespace adapt
{

namespace eval
{

namespace detail
{

template <class Container, class ...ArgTypes, class Func, any_node ...Nodes>
auto MakeRttiFuncNode_construct(int, Func&& f, Nodes&& ...n)
-> decltype(f(std::declval<ArgTypes>()...), eval::RttiFuncNode<Container>{})
{
	using DecFunc = std::decay_t<Func>;
	using RetType = std::decay_t<std::invoke_result_t<DecFunc, ArgTypes...>>;
	using FuncA = FuncDefinition<DecFunc, RetType, ArgTypes...>;
	using NodeImpl = detail::RttiFuncNode_impl<FuncA, Container, TypeList<std::decay_t<Nodes>...>>;
	eval::RttiFuncNode<Container> res;
	res.template Construct<NodeImpl>(std::forward<Func>(f), std::forward<Nodes>(n)...);
	return res;
}
template <class Container, class ...ArgTypes, class Func, any_node ...Nodes>
eval::RttiFuncNode<Container> MakeRttiFuncNode_construct(float, Func&&, Nodes&& ...)
{
	throw MismatchType("");
}

template <class Container, class Func, FieldType ...Types, any_node ...Nodes, size_t ...Indices>
eval::RttiFuncNode<Container> MakeRttiFuncNode_expand(Func&& f, ValueList<Types...>, std::tuple<Nodes...> t, std::index_sequence<Indices...>)
{
	return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<Types>...>(1, std::forward<Func>(f), std::get<Indices>(t)...);
}

template <class Container, class Func, FieldType ...Types, any_node ...Nodes>
eval::RttiFuncNode<Container> MakeRttiFuncNode(long, Func&& f, ValueList<Types...> v, std::tuple<Nodes...> t)
{
	return MakeRttiFuncNode_expand<Container>(std::forward<Func>(f), v, std::move(t), std::make_index_sequence<sizeof...(Nodes)>{});
}

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4702)
#endif
template <class Container, class Func, FieldType ...Types, any_node ...Nodes, any_node Head, any_node ...Body>
eval::RttiFuncNode<Container> MakeRttiFuncNode(long l, Func&& f, ValueList<Types...>, std::tuple<Nodes...> t, Head&& head, Body&& ...body)
{
#define MAKE_RTTI_FUNC_NODE(TYPE)\
	return MakeRttiFuncNode<Container>(l, std::forward<Func>(f), ValueList<Types..., TYPE>(),\
									   TupleAdd(std::move(t), std::forward<Head>(head)), std::forward<Body>(body)...);
	ADAPT_SWITCH_FIELD_TYPE(head.GetType(), MAKE_RTTI_FUNC_NODE, throw MismatchType(""););
#undef MAKE_RTTI_FUNC_NODE
}
#ifdef _MSC_VER
#pragma warning(pop)
#endif


/*template <class Func, neither_node_nor_placeholder Constant,
	FieldType Type = DFieldInfo::GetSameSizeTagType<std::remove_cvref_t<Constant>>()>
	requires (Type != FieldType::Emp)
auto MakeRttiFuncNodeFromConstant(Func&& f, Constant&& c)
{
	return MakeRttiFuncNode(std::forward<Func>(f), ValueList<>(), std::tuple<>(), RttiConstNode(std::forward<Constant>(c)));
}*/

}

}

}

#endif