#ifndef ADAPT_EVALUATOR_MAKE_RTTI_FUNC_NODE_H
#define ADAPT_EVALUATOR_MAKE_RTTI_FUNC_NODE_H

#include <OpenADAPT/Evaluator/Function.h>

namespace adapt
{

namespace eval
{

namespace detail
{

template <class Container, class Func, any_node ...Nodes>
	requires std::same_as<std::decay_t<Func>, eval::Not>
eval::RttiFuncNode<Container> MakeRttiFuncNode(int, Func&& f, ValueList<>, std::tuple<>, Nodes&& ...nodes)
{
	using enum FieldType;
	if constexpr (sizeof...(Nodes) == 1)
	{
		const auto& [ arg1 ] = std::forward_as_tuple(std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
	}
	throw MismatchType("");
}
template <class Container, class Func, any_node ...Nodes>
	requires std::same_as<std::decay_t<Func>, eval::Negate>
eval::RttiFuncNode<Container> MakeRttiFuncNode(int, Func&& f, ValueList<>, std::tuple<>, Nodes&& ...nodes)
{
	using enum FieldType;
	if constexpr (sizeof...(Nodes) == 1)
	{
		const auto& [ arg1 ] = std::forward_as_tuple(std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == C32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<C32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == C64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<C64>>
				(1, f, std::forward<Nodes>(nodes)...);
	}
	throw MismatchType("");
}
template <class Container, class Func, any_node ...Nodes>
	requires std::same_as<std::decay_t<Func>, eval::Plus>
eval::RttiFuncNode<Container> MakeRttiFuncNode(int, Func&& f, ValueList<>, std::tuple<>, Nodes&& ...nodes)
{
	using enum FieldType;
	if constexpr (sizeof...(Nodes) == 2)
	{
		const auto& [ arg1, arg2 ] = std::forward_as_tuple(std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == C32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<C32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == C64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<C64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == C32 && arg2.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == C32 && arg2.GetType() == C32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == C64 && arg2.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == C64 && arg2.GetType() == C64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == Str && arg2.GetType() == Str)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>>
				(1, f, std::forward<Nodes>(nodes)...);
	}
	throw MismatchType("");
}
template <class Container, class Func, any_node ...Nodes>
	requires std::same_as<std::decay_t<Func>, eval::Minus>
eval::RttiFuncNode<Container> MakeRttiFuncNode(int, Func&& f, ValueList<>, std::tuple<>, Nodes&& ...nodes)
{
	using enum FieldType;
	if constexpr (sizeof...(Nodes) == 2)
	{
		const auto& [ arg1, arg2 ] = std::forward_as_tuple(std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == C32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<C32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == C64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<C64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == C32 && arg2.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == C32 && arg2.GetType() == C32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == C64 && arg2.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == C64 && arg2.GetType() == C64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>>
				(1, f, std::forward<Nodes>(nodes)...);
	}
	throw MismatchType("");
}
template <class Container, class Func, any_node ...Nodes>
	requires std::same_as<std::decay_t<Func>, eval::Multiply>
eval::RttiFuncNode<Container> MakeRttiFuncNode(int, Func&& f, ValueList<>, std::tuple<>, Nodes&& ...nodes)
{
	using enum FieldType;
	if constexpr (sizeof...(Nodes) == 2)
	{
		const auto& [ arg1, arg2 ] = std::forward_as_tuple(std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == C32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<C32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == C64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<C64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == C32 && arg2.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == C32 && arg2.GetType() == C32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == C64 && arg2.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == C64 && arg2.GetType() == C64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>>
				(1, f, std::forward<Nodes>(nodes)...);
	}
	throw MismatchType("");
}
template <class Container, class Func, any_node ...Nodes>
	requires std::same_as<std::decay_t<Func>, eval::Divide>
eval::RttiFuncNode<Container> MakeRttiFuncNode(int, Func&& f, ValueList<>, std::tuple<>, Nodes&& ...nodes)
{
	using enum FieldType;
	if constexpr (sizeof...(Nodes) == 2)
	{
		const auto& [ arg1, arg2 ] = std::forward_as_tuple(std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == C32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<C32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == C64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<C64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == C32 && arg2.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == C32 && arg2.GetType() == C32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == C64 && arg2.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == C64 && arg2.GetType() == C64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>>
				(1, f, std::forward<Nodes>(nodes)...);
	}
	throw MismatchType("");
}
template <class Container, class Func, any_node ...Nodes>
	requires std::same_as<std::decay_t<Func>, eval::Modulus>
eval::RttiFuncNode<Container> MakeRttiFuncNode(int, Func&& f, ValueList<>, std::tuple<>, Nodes&& ...nodes)
{
	using enum FieldType;
	if constexpr (sizeof...(Nodes) == 2)
	{
		const auto& [ arg1, arg2 ] = std::forward_as_tuple(std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
	}
	throw MismatchType("");
}
template <class Container, class Func, any_node ...Nodes>
	requires std::same_as<std::decay_t<Func>, eval::Power>
eval::RttiFuncNode<Container> MakeRttiFuncNode(int, Func&& f, ValueList<>, std::tuple<>, Nodes&& ...nodes)
{
	using enum FieldType;
	if constexpr (sizeof...(Nodes) == 2)
	{
		const auto& [ arg1, arg2 ] = std::forward_as_tuple(std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == C32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<C32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == C64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<C64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == C32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<C32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == C64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<C64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == C32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<C32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == C64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<C64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == C32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<C32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == C64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<C64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == C32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<C32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == C64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<C64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == C32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<C32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == C64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<C64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == C32 && arg2.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == C32 && arg2.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == C32 && arg2.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == C32 && arg2.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == C32 && arg2.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == C32 && arg2.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == C32 && arg2.GetType() == C32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == C32 && arg2.GetType() == C64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == C64 && arg2.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == C64 && arg2.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == C64 && arg2.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == C64 && arg2.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == C64 && arg2.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == C64 && arg2.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == C64 && arg2.GetType() == C32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == C64 && arg2.GetType() == C64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>>
				(1, f, std::forward<Nodes>(nodes)...);
	}
	throw MismatchType("");
}
template <class Container, class Func, any_node ...Nodes>
	requires std::same_as<std::decay_t<Func>, eval::Equal>
eval::RttiFuncNode<Container> MakeRttiFuncNode(int, Func&& f, ValueList<>, std::tuple<>, Nodes&& ...nodes)
{
	using enum FieldType;
	if constexpr (sizeof...(Nodes) == 2)
	{
		const auto& [ arg1, arg2 ] = std::forward_as_tuple(std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == C32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<C32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == C64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<C64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == C32 && arg2.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == C32 && arg2.GetType() == C32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == C64 && arg2.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == C64 && arg2.GetType() == C64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == Str && arg2.GetType() == Str)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>>
				(1, f, std::forward<Nodes>(nodes)...);
	}
	throw MismatchType("");
}
template <class Container, class Func, any_node ...Nodes>
	requires std::same_as<std::decay_t<Func>, eval::NotEqual>
eval::RttiFuncNode<Container> MakeRttiFuncNode(int, Func&& f, ValueList<>, std::tuple<>, Nodes&& ...nodes)
{
	using enum FieldType;
	if constexpr (sizeof...(Nodes) == 2)
	{
		const auto& [ arg1, arg2 ] = std::forward_as_tuple(std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == C32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<C32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == C64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<C64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == C32 && arg2.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == C32 && arg2.GetType() == C32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == C64 && arg2.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == C64 && arg2.GetType() == C64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == Str && arg2.GetType() == Str)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>>
				(1, f, std::forward<Nodes>(nodes)...);
	}
	throw MismatchType("");
}
template <class Container, class Func, any_node ...Nodes>
	requires std::same_as<std::decay_t<Func>, eval::Less>
eval::RttiFuncNode<Container> MakeRttiFuncNode(int, Func&& f, ValueList<>, std::tuple<>, Nodes&& ...nodes)
{
	using enum FieldType;
	if constexpr (sizeof...(Nodes) == 2)
	{
		const auto& [ arg1, arg2 ] = std::forward_as_tuple(std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == Str && arg2.GetType() == Str)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>>
				(1, f, std::forward<Nodes>(nodes)...);
	}
	throw MismatchType("");
}
template <class Container, class Func, any_node ...Nodes>
	requires std::same_as<std::decay_t<Func>, eval::LessEqual>
eval::RttiFuncNode<Container> MakeRttiFuncNode(int, Func&& f, ValueList<>, std::tuple<>, Nodes&& ...nodes)
{
	using enum FieldType;
	if constexpr (sizeof...(Nodes) == 2)
	{
		const auto& [ arg1, arg2 ] = std::forward_as_tuple(std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == Str && arg2.GetType() == Str)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>>
				(1, f, std::forward<Nodes>(nodes)...);
	}
	throw MismatchType("");
}
template <class Container, class Func, any_node ...Nodes>
	requires std::same_as<std::decay_t<Func>, eval::Greater>
eval::RttiFuncNode<Container> MakeRttiFuncNode(int, Func&& f, ValueList<>, std::tuple<>, Nodes&& ...nodes)
{
	using enum FieldType;
	if constexpr (sizeof...(Nodes) == 2)
	{
		const auto& [ arg1, arg2 ] = std::forward_as_tuple(std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == Str && arg2.GetType() == Str)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>>
				(1, f, std::forward<Nodes>(nodes)...);
	}
	throw MismatchType("");
}
template <class Container, class Func, any_node ...Nodes>
	requires std::same_as<std::decay_t<Func>, eval::GreaterEqual>
eval::RttiFuncNode<Container> MakeRttiFuncNode(int, Func&& f, ValueList<>, std::tuple<>, Nodes&& ...nodes)
{
	using enum FieldType;
	if constexpr (sizeof...(Nodes) == 2)
	{
		const auto& [ arg1, arg2 ] = std::forward_as_tuple(std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == Str && arg2.GetType() == Str)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>>
				(1, f, std::forward<Nodes>(nodes)...);
	}
	throw MismatchType("");
}
template <class Container, class Func, any_node ...Nodes>
	requires std::same_as<std::decay_t<Func>, eval::OperatorAnd>
eval::RttiFuncNode<Container> MakeRttiFuncNode(int, Func&& f, ValueList<>, std::tuple<>, Nodes&& ...nodes)
{
	using enum FieldType;
	if constexpr (sizeof...(Nodes) == 2)
	{
		const auto& [ arg1, arg2 ] = std::forward_as_tuple(std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
	}
	throw MismatchType("");
}
template <class Container, class Func, any_node ...Nodes>
	requires std::same_as<std::decay_t<Func>, eval::OperatorOr>
eval::RttiFuncNode<Container> MakeRttiFuncNode(int, Func&& f, ValueList<>, std::tuple<>, Nodes&& ...nodes)
{
	using enum FieldType;
	if constexpr (sizeof...(Nodes) == 2)
	{
		const auto& [ arg1, arg2 ] = std::forward_as_tuple(std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
	}
	throw MismatchType("");
}
template <class Container, class Func, any_node ...Nodes>
	requires std::same_as<std::decay_t<Func>, eval::Sin>
eval::RttiFuncNode<Container> MakeRttiFuncNode(int, Func&& f, ValueList<>, std::tuple<>, Nodes&& ...nodes)
{
	using enum FieldType;
	if constexpr (sizeof...(Nodes) == 1)
	{
		const auto& [ arg1 ] = std::forward_as_tuple(std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == C32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<C32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == C64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<C64>>
				(1, f, std::forward<Nodes>(nodes)...);
	}
	throw MismatchType("");
}
template <class Container, class Func, any_node ...Nodes>
	requires std::same_as<std::decay_t<Func>, eval::Cos>
eval::RttiFuncNode<Container> MakeRttiFuncNode(int, Func&& f, ValueList<>, std::tuple<>, Nodes&& ...nodes)
{
	using enum FieldType;
	if constexpr (sizeof...(Nodes) == 1)
	{
		const auto& [ arg1 ] = std::forward_as_tuple(std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == C32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<C32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == C64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<C64>>
				(1, f, std::forward<Nodes>(nodes)...);
	}
	throw MismatchType("");
}
template <class Container, class Func, any_node ...Nodes>
	requires std::same_as<std::decay_t<Func>, eval::Tan>
eval::RttiFuncNode<Container> MakeRttiFuncNode(int, Func&& f, ValueList<>, std::tuple<>, Nodes&& ...nodes)
{
	using enum FieldType;
	if constexpr (sizeof...(Nodes) == 1)
	{
		const auto& [ arg1 ] = std::forward_as_tuple(std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == C32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<C32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == C64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<C64>>
				(1, f, std::forward<Nodes>(nodes)...);
	}
	throw MismatchType("");
}
template <class Container, class Func, any_node ...Nodes>
	requires std::same_as<std::decay_t<Func>, eval::ASin>
eval::RttiFuncNode<Container> MakeRttiFuncNode(int, Func&& f, ValueList<>, std::tuple<>, Nodes&& ...nodes)
{
	using enum FieldType;
	if constexpr (sizeof...(Nodes) == 1)
	{
		const auto& [ arg1 ] = std::forward_as_tuple(std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == C32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<C32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == C64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<C64>>
				(1, f, std::forward<Nodes>(nodes)...);
	}
	throw MismatchType("");
}
template <class Container, class Func, any_node ...Nodes>
	requires std::same_as<std::decay_t<Func>, eval::ACos>
eval::RttiFuncNode<Container> MakeRttiFuncNode(int, Func&& f, ValueList<>, std::tuple<>, Nodes&& ...nodes)
{
	using enum FieldType;
	if constexpr (sizeof...(Nodes) == 1)
	{
		const auto& [ arg1 ] = std::forward_as_tuple(std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == C32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<C32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == C64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<C64>>
				(1, f, std::forward<Nodes>(nodes)...);
	}
	throw MismatchType("");
}
template <class Container, class Func, any_node ...Nodes>
	requires std::same_as<std::decay_t<Func>, eval::ATan>
eval::RttiFuncNode<Container> MakeRttiFuncNode(int, Func&& f, ValueList<>, std::tuple<>, Nodes&& ...nodes)
{
	using enum FieldType;
	if constexpr (sizeof...(Nodes) == 1)
	{
		const auto& [ arg1 ] = std::forward_as_tuple(std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == C32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<C32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == C64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<C64>>
				(1, f, std::forward<Nodes>(nodes)...);
	}
	throw MismatchType("");
}
template <class Container, class Func, any_node ...Nodes>
	requires std::same_as<std::decay_t<Func>, eval::Exponential>
eval::RttiFuncNode<Container> MakeRttiFuncNode(int, Func&& f, ValueList<>, std::tuple<>, Nodes&& ...nodes)
{
	using enum FieldType;
	if constexpr (sizeof...(Nodes) == 1)
	{
		const auto& [ arg1 ] = std::forward_as_tuple(std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == C32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<C32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == C64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<C64>>
				(1, f, std::forward<Nodes>(nodes)...);
	}
	throw MismatchType("");
}
template <class Container, class Func, any_node ...Nodes>
	requires std::same_as<std::decay_t<Func>, eval::Square>
eval::RttiFuncNode<Container> MakeRttiFuncNode(int, Func&& f, ValueList<>, std::tuple<>, Nodes&& ...nodes)
{
	using enum FieldType;
	if constexpr (sizeof...(Nodes) == 1)
	{
		const auto& [ arg1 ] = std::forward_as_tuple(std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == C32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<C32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == C64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<C64>>
				(1, f, std::forward<Nodes>(nodes)...);
	}
	throw MismatchType("");
}
template <class Container, class Func, any_node ...Nodes>
	requires std::same_as<std::decay_t<Func>, eval::Sqrt>
eval::RttiFuncNode<Container> MakeRttiFuncNode(int, Func&& f, ValueList<>, std::tuple<>, Nodes&& ...nodes)
{
	using enum FieldType;
	if constexpr (sizeof...(Nodes) == 1)
	{
		const auto& [ arg1 ] = std::forward_as_tuple(std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == C32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<C32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == C64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<C64>>
				(1, f, std::forward<Nodes>(nodes)...);
	}
	throw MismatchType("");
}
template <class Container, class Func, any_node ...Nodes>
	requires std::same_as<std::decay_t<Func>, eval::Cube>
eval::RttiFuncNode<Container> MakeRttiFuncNode(int, Func&& f, ValueList<>, std::tuple<>, Nodes&& ...nodes)
{
	using enum FieldType;
	if constexpr (sizeof...(Nodes) == 1)
	{
		const auto& [ arg1 ] = std::forward_as_tuple(std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == C32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<C32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == C64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<C64>>
				(1, f, std::forward<Nodes>(nodes)...);
	}
	throw MismatchType("");
}
template <class Container, class Func, any_node ...Nodes>
	requires std::same_as<std::decay_t<Func>, eval::Cbrt>
eval::RttiFuncNode<Container> MakeRttiFuncNode(int, Func&& f, ValueList<>, std::tuple<>, Nodes&& ...nodes)
{
	using enum FieldType;
	if constexpr (sizeof...(Nodes) == 1)
	{
		const auto& [ arg1 ] = std::forward_as_tuple(std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
	}
	throw MismatchType("");
}
template <class Container, class Func, any_node ...Nodes>
	requires std::same_as<std::decay_t<Func>, eval::Ceil>
eval::RttiFuncNode<Container> MakeRttiFuncNode(int, Func&& f, ValueList<>, std::tuple<>, Nodes&& ...nodes)
{
	using enum FieldType;
	if constexpr (sizeof...(Nodes) == 1)
	{
		const auto& [ arg1 ] = std::forward_as_tuple(std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
	}
	throw MismatchType("");
}
template <class Container, class Func, any_node ...Nodes>
	requires std::same_as<std::decay_t<Func>, eval::Floor>
eval::RttiFuncNode<Container> MakeRttiFuncNode(int, Func&& f, ValueList<>, std::tuple<>, Nodes&& ...nodes)
{
	using enum FieldType;
	if constexpr (sizeof...(Nodes) == 1)
	{
		const auto& [ arg1 ] = std::forward_as_tuple(std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
	}
	throw MismatchType("");
}
template <class Container, class Func, any_node ...Nodes>
	requires std::same_as<std::decay_t<Func>, eval::Round>
eval::RttiFuncNode<Container> MakeRttiFuncNode(int, Func&& f, ValueList<>, std::tuple<>, Nodes&& ...nodes)
{
	using enum FieldType;
	if constexpr (sizeof...(Nodes) == 1)
	{
		const auto& [ arg1 ] = std::forward_as_tuple(std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
	}
	throw MismatchType("");
}
template <class Container, class Func, any_node ...Nodes>
	requires std::same_as<std::decay_t<Func>, eval::Log>
eval::RttiFuncNode<Container> MakeRttiFuncNode(int, Func&& f, ValueList<>, std::tuple<>, Nodes&& ...nodes)
{
	using enum FieldType;
	if constexpr (sizeof...(Nodes) == 1)
	{
		const auto& [ arg1 ] = std::forward_as_tuple(std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == C32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<C32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == C64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<C64>>
				(1, f, std::forward<Nodes>(nodes)...);
	}
	throw MismatchType("");
}
template <class Container, class Func, any_node ...Nodes>
	requires std::same_as<std::decay_t<Func>, eval::Log10>
eval::RttiFuncNode<Container> MakeRttiFuncNode(int, Func&& f, ValueList<>, std::tuple<>, Nodes&& ...nodes)
{
	using enum FieldType;
	if constexpr (sizeof...(Nodes) == 1)
	{
		const auto& [ arg1 ] = std::forward_as_tuple(std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == C32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<C32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == C64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<C64>>
				(1, f, std::forward<Nodes>(nodes)...);
	}
	throw MismatchType("");
}
template <class Container, class Func, any_node ...Nodes>
	requires std::same_as<std::decay_t<Func>, eval::Abs>
eval::RttiFuncNode<Container> MakeRttiFuncNode(int, Func&& f, ValueList<>, std::tuple<>, Nodes&& ...nodes)
{
	using enum FieldType;
	if constexpr (sizeof...(Nodes) == 1)
	{
		const auto& [ arg1 ] = std::forward_as_tuple(std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == C32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<C32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == C64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<C64>>
				(1, f, std::forward<Nodes>(nodes)...);
	}
	throw MismatchType("");
}
template <class Container, class Func, any_node ...Nodes>
	requires std::same_as<std::decay_t<Func>, eval::Len>
eval::RttiFuncNode<Container> MakeRttiFuncNode(int, Func&& f, ValueList<>, std::tuple<>, Nodes&& ...nodes)
{
	using enum FieldType;
	if constexpr (sizeof...(Nodes) == 1)
	{
		const auto& [ arg1 ] = std::forward_as_tuple(std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == Str)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<Str>>
				(1, f, std::forward<Nodes>(nodes)...);
	}
	throw MismatchType("");
}
template <class Container, class Func, any_node ...Nodes>
	requires std::same_as<std::decay_t<Func>, eval::NumToStr>
eval::RttiFuncNode<Container> MakeRttiFuncNode(int, Func&& f, ValueList<>, std::tuple<>, Nodes&& ...nodes)
{
	using enum FieldType;
	if constexpr (sizeof...(Nodes) == 1)
	{
		const auto& [ arg1 ] = std::forward_as_tuple(std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
	}
	throw MismatchType("");
}
template <class Container, class Func, any_node ...Nodes>
	requires std::same_as<std::decay_t<Func>, eval::ATan2>
eval::RttiFuncNode<Container> MakeRttiFuncNode(int, Func&& f, ValueList<>, std::tuple<>, Nodes&& ...nodes)
{
	using enum FieldType;
	if constexpr (sizeof...(Nodes) == 2)
	{
		const auto& [ arg1, arg2 ] = std::forward_as_tuple(std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
	}
	throw MismatchType("");
}
template <class Container, class Func, any_node ...Nodes>
	requires std::same_as<std::decay_t<Func>, eval::Log2>
eval::RttiFuncNode<Container> MakeRttiFuncNode(int, Func&& f, ValueList<>, std::tuple<>, Nodes&& ...nodes)
{
	using enum FieldType;
	if constexpr (sizeof...(Nodes) == 2)
	{
		const auto& [ arg1, arg2 ] = std::forward_as_tuple(std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == C64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<C64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == C64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<C64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == C64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<C64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == C64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<C64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == C32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<C32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == C64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<C64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == C32 && arg2.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == C32 && arg2.GetType() == C32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == C64 && arg2.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == C64 && arg2.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == C64 && arg2.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == C64 && arg2.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == C64 && arg2.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == C64 && arg2.GetType() == C64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>>
				(1, f, std::forward<Nodes>(nodes)...);
	}
	throw MismatchType("");
}
template <class Container, class Func, any_node ...Nodes>
	requires std::same_as<std::decay_t<Func>, eval::Hypot>
eval::RttiFuncNode<Container> MakeRttiFuncNode(int, Func&& f, ValueList<>, std::tuple<>, Nodes&& ...nodes)
{
	using enum FieldType;
	if constexpr (sizeof...(Nodes) == 2)
	{
		const auto& [ arg1, arg2 ] = std::forward_as_tuple(std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
	}
	if constexpr (sizeof...(Nodes) == 3)
	{
		const auto& [ arg1, arg2, arg3 ] = std::forward_as_tuple(std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I08 && arg3.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I08 && arg3.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I08 && arg3.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I08 && arg3.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I08 && arg3.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I08 && arg3.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I16 && arg3.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I16 && arg3.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I16 && arg3.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I16 && arg3.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I16 && arg3.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I16 && arg3.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I32 && arg3.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I32 && arg3.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I32 && arg3.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I32 && arg3.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I32 && arg3.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I32 && arg3.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I64 && arg3.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I64 && arg3.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I64 && arg3.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I64 && arg3.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I64 && arg3.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I64 && arg3.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == F32 && arg3.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == F32 && arg3.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == F32 && arg3.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == F32 && arg3.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == F32 && arg3.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == F32 && arg3.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == F64 && arg3.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == F64 && arg3.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == F64 && arg3.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == F64 && arg3.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == F64 && arg3.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == F64 && arg3.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I08 && arg3.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I08 && arg3.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I08 && arg3.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I08 && arg3.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I08 && arg3.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I08 && arg3.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I16 && arg3.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I16 && arg3.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I16 && arg3.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I16 && arg3.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I16 && arg3.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I16 && arg3.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I32 && arg3.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I32 && arg3.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I32 && arg3.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I32 && arg3.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I32 && arg3.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I32 && arg3.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I64 && arg3.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I64 && arg3.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I64 && arg3.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I64 && arg3.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I64 && arg3.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I64 && arg3.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == F32 && arg3.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == F32 && arg3.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == F32 && arg3.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == F32 && arg3.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == F32 && arg3.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == F32 && arg3.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == F64 && arg3.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == F64 && arg3.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == F64 && arg3.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == F64 && arg3.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == F64 && arg3.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == F64 && arg3.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I08 && arg3.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I08 && arg3.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I08 && arg3.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I08 && arg3.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I08 && arg3.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I08 && arg3.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I16 && arg3.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I16 && arg3.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I16 && arg3.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I16 && arg3.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I16 && arg3.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I16 && arg3.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I32 && arg3.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I32 && arg3.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I32 && arg3.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I32 && arg3.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I32 && arg3.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I32 && arg3.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I64 && arg3.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I64 && arg3.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I64 && arg3.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I64 && arg3.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I64 && arg3.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I64 && arg3.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == F32 && arg3.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == F32 && arg3.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == F32 && arg3.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == F32 && arg3.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == F32 && arg3.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == F32 && arg3.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == F64 && arg3.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == F64 && arg3.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == F64 && arg3.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == F64 && arg3.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == F64 && arg3.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == F64 && arg3.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I08 && arg3.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I08 && arg3.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I08 && arg3.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I08 && arg3.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I08 && arg3.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I08 && arg3.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I16 && arg3.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I16 && arg3.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I16 && arg3.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I16 && arg3.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I16 && arg3.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I16 && arg3.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I32 && arg3.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I32 && arg3.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I32 && arg3.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I32 && arg3.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I32 && arg3.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I32 && arg3.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I64 && arg3.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I64 && arg3.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I64 && arg3.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I64 && arg3.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I64 && arg3.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I64 && arg3.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == F32 && arg3.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == F32 && arg3.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == F32 && arg3.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == F32 && arg3.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == F32 && arg3.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == F32 && arg3.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == F64 && arg3.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == F64 && arg3.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == F64 && arg3.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == F64 && arg3.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == F64 && arg3.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == F64 && arg3.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == I08 && arg3.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == I08 && arg3.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == I08 && arg3.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == I08 && arg3.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == I08 && arg3.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == I08 && arg3.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == I16 && arg3.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == I16 && arg3.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == I16 && arg3.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == I16 && arg3.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == I16 && arg3.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == I16 && arg3.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == I32 && arg3.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == I32 && arg3.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == I32 && arg3.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == I32 && arg3.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == I32 && arg3.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == I32 && arg3.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == I64 && arg3.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == I64 && arg3.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == I64 && arg3.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == I64 && arg3.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == I64 && arg3.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == I64 && arg3.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == F32 && arg3.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == F32 && arg3.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == F32 && arg3.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == F32 && arg3.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == F32 && arg3.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == F32 && arg3.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == F64 && arg3.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == F64 && arg3.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == F64 && arg3.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == F64 && arg3.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == F64 && arg3.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == F64 && arg3.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == I08 && arg3.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == I08 && arg3.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == I08 && arg3.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == I08 && arg3.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == I08 && arg3.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == I08 && arg3.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == I16 && arg3.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == I16 && arg3.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == I16 && arg3.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == I16 && arg3.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == I16 && arg3.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == I16 && arg3.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == I32 && arg3.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == I32 && arg3.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == I32 && arg3.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == I32 && arg3.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == I32 && arg3.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == I32 && arg3.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == I64 && arg3.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == I64 && arg3.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == I64 && arg3.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == I64 && arg3.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == I64 && arg3.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == I64 && arg3.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == F32 && arg3.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == F32 && arg3.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == F32 && arg3.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == F32 && arg3.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == F32 && arg3.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == F32 && arg3.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == F64 && arg3.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == F64 && arg3.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == F64 && arg3.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == F64 && arg3.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == F64 && arg3.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == F64 && arg3.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
	}
	throw MismatchType("");
}
template <class Container, class Func, any_node ...Nodes>
	requires std::same_as<std::decay_t<Func>, eval::Max>
eval::RttiFuncNode<Container> MakeRttiFuncNode(int, Func&& f, ValueList<>, std::tuple<>, Nodes&& ...nodes)
{
	using enum FieldType;
	if constexpr (sizeof...(Nodes) == 2)
	{
		const auto& [ arg1, arg2 ] = std::forward_as_tuple(std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == Str && arg2.GetType() == Str)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>>
				(1, f, std::forward<Nodes>(nodes)...);
	}
	if constexpr (sizeof...(Nodes) == 3)
	{
		const auto& [ arg1, arg2, arg3 ] = std::forward_as_tuple(std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I08 && arg3.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I16 && arg3.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I32 && arg3.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I64 && arg3.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == F32 && arg3.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == F64 && arg3.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == Str && arg2.GetType() == Str && arg3.GetType() == Str)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>>
				(1, f, std::forward<Nodes>(nodes)...);
	}
	if constexpr (sizeof...(Nodes) == 4)
	{
		const auto& [ arg1, arg2, arg3, arg4 ] = std::forward_as_tuple(std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I08 && arg3.GetType() == I08 && arg4.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I16 && arg3.GetType() == I16 && arg4.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I32 && arg3.GetType() == I32 && arg4.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I64 && arg3.GetType() == I64 && arg4.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == F32 && arg3.GetType() == F32 && arg4.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == F64 && arg3.GetType() == F64 && arg4.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == Str && arg2.GetType() == Str && arg3.GetType() == Str && arg4.GetType() == Str)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>>
				(1, f, std::forward<Nodes>(nodes)...);
	}
	if constexpr (sizeof...(Nodes) == 5)
	{
		const auto& [ arg1, arg2, arg3, arg4, arg5 ] = std::forward_as_tuple(std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I08 && arg3.GetType() == I08 && arg4.GetType() == I08 && arg5.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I16 && arg3.GetType() == I16 && arg4.GetType() == I16 && arg5.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I32 && arg3.GetType() == I32 && arg4.GetType() == I32 && arg5.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I64 && arg3.GetType() == I64 && arg4.GetType() == I64 && arg5.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == F32 && arg3.GetType() == F32 && arg4.GetType() == F32 && arg5.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == F64 && arg3.GetType() == F64 && arg4.GetType() == F64 && arg5.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == Str && arg2.GetType() == Str && arg3.GetType() == Str && arg4.GetType() == Str && arg5.GetType() == Str)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>>
				(1, f, std::forward<Nodes>(nodes)...);
	}
	if constexpr (sizeof...(Nodes) == 6)
	{
		const auto& [ arg1, arg2, arg3, arg4, arg5, arg6 ] = std::forward_as_tuple(std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I08 && arg3.GetType() == I08 && arg4.GetType() == I08 && arg5.GetType() == I08 && arg6.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I16 && arg3.GetType() == I16 && arg4.GetType() == I16 && arg5.GetType() == I16 && arg6.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I32 && arg3.GetType() == I32 && arg4.GetType() == I32 && arg5.GetType() == I32 && arg6.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I64 && arg3.GetType() == I64 && arg4.GetType() == I64 && arg5.GetType() == I64 && arg6.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == F32 && arg3.GetType() == F32 && arg4.GetType() == F32 && arg5.GetType() == F32 && arg6.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == F64 && arg3.GetType() == F64 && arg4.GetType() == F64 && arg5.GetType() == F64 && arg6.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == Str && arg2.GetType() == Str && arg3.GetType() == Str && arg4.GetType() == Str && arg5.GetType() == Str && arg6.GetType() == Str)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>>
				(1, f, std::forward<Nodes>(nodes)...);
	}
	if constexpr (sizeof...(Nodes) == 7)
	{
		const auto& [ arg1, arg2, arg3, arg4, arg5, arg6, arg7 ] = std::forward_as_tuple(std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I08 && arg3.GetType() == I08 && arg4.GetType() == I08 && arg5.GetType() == I08 && arg6.GetType() == I08 && arg7.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I16 && arg3.GetType() == I16 && arg4.GetType() == I16 && arg5.GetType() == I16 && arg6.GetType() == I16 && arg7.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I32 && arg3.GetType() == I32 && arg4.GetType() == I32 && arg5.GetType() == I32 && arg6.GetType() == I32 && arg7.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I64 && arg3.GetType() == I64 && arg4.GetType() == I64 && arg5.GetType() == I64 && arg6.GetType() == I64 && arg7.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == F32 && arg3.GetType() == F32 && arg4.GetType() == F32 && arg5.GetType() == F32 && arg6.GetType() == F32 && arg7.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == F64 && arg3.GetType() == F64 && arg4.GetType() == F64 && arg5.GetType() == F64 && arg6.GetType() == F64 && arg7.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == Str && arg2.GetType() == Str && arg3.GetType() == Str && arg4.GetType() == Str && arg5.GetType() == Str && arg6.GetType() == Str && arg7.GetType() == Str)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>>
				(1, f, std::forward<Nodes>(nodes)...);
	}
	if constexpr (sizeof...(Nodes) == 8)
	{
		const auto& [ arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8 ] = std::forward_as_tuple(std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I08 && arg3.GetType() == I08 && arg4.GetType() == I08 && arg5.GetType() == I08 && arg6.GetType() == I08 && arg7.GetType() == I08 && arg8.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I16 && arg3.GetType() == I16 && arg4.GetType() == I16 && arg5.GetType() == I16 && arg6.GetType() == I16 && arg7.GetType() == I16 && arg8.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I32 && arg3.GetType() == I32 && arg4.GetType() == I32 && arg5.GetType() == I32 && arg6.GetType() == I32 && arg7.GetType() == I32 && arg8.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I64 && arg3.GetType() == I64 && arg4.GetType() == I64 && arg5.GetType() == I64 && arg6.GetType() == I64 && arg7.GetType() == I64 && arg8.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == F32 && arg3.GetType() == F32 && arg4.GetType() == F32 && arg5.GetType() == F32 && arg6.GetType() == F32 && arg7.GetType() == F32 && arg8.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == F64 && arg3.GetType() == F64 && arg4.GetType() == F64 && arg5.GetType() == F64 && arg6.GetType() == F64 && arg7.GetType() == F64 && arg8.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == Str && arg2.GetType() == Str && arg3.GetType() == Str && arg4.GetType() == Str && arg5.GetType() == Str && arg6.GetType() == Str && arg7.GetType() == Str && arg8.GetType() == Str)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>>
				(1, f, std::forward<Nodes>(nodes)...);
	}
	if constexpr (sizeof...(Nodes) == 9)
	{
		const auto& [ arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9 ] = std::forward_as_tuple(std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I08 && arg3.GetType() == I08 && arg4.GetType() == I08 && arg5.GetType() == I08 && arg6.GetType() == I08 && arg7.GetType() == I08 && arg8.GetType() == I08 && arg9.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I16 && arg3.GetType() == I16 && arg4.GetType() == I16 && arg5.GetType() == I16 && arg6.GetType() == I16 && arg7.GetType() == I16 && arg8.GetType() == I16 && arg9.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I32 && arg3.GetType() == I32 && arg4.GetType() == I32 && arg5.GetType() == I32 && arg6.GetType() == I32 && arg7.GetType() == I32 && arg8.GetType() == I32 && arg9.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I64 && arg3.GetType() == I64 && arg4.GetType() == I64 && arg5.GetType() == I64 && arg6.GetType() == I64 && arg7.GetType() == I64 && arg8.GetType() == I64 && arg9.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == F32 && arg3.GetType() == F32 && arg4.GetType() == F32 && arg5.GetType() == F32 && arg6.GetType() == F32 && arg7.GetType() == F32 && arg8.GetType() == F32 && arg9.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == F64 && arg3.GetType() == F64 && arg4.GetType() == F64 && arg5.GetType() == F64 && arg6.GetType() == F64 && arg7.GetType() == F64 && arg8.GetType() == F64 && arg9.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == Str && arg2.GetType() == Str && arg3.GetType() == Str && arg4.GetType() == Str && arg5.GetType() == Str && arg6.GetType() == Str && arg7.GetType() == Str && arg8.GetType() == Str && arg9.GetType() == Str)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>>
				(1, f, std::forward<Nodes>(nodes)...);
	}
	if constexpr (sizeof...(Nodes) == 10)
	{
		const auto& [ arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10 ] = std::forward_as_tuple(std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I08 && arg3.GetType() == I08 && arg4.GetType() == I08 && arg5.GetType() == I08 && arg6.GetType() == I08 && arg7.GetType() == I08 && arg8.GetType() == I08 && arg9.GetType() == I08 && arg10.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I16 && arg3.GetType() == I16 && arg4.GetType() == I16 && arg5.GetType() == I16 && arg6.GetType() == I16 && arg7.GetType() == I16 && arg8.GetType() == I16 && arg9.GetType() == I16 && arg10.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I32 && arg3.GetType() == I32 && arg4.GetType() == I32 && arg5.GetType() == I32 && arg6.GetType() == I32 && arg7.GetType() == I32 && arg8.GetType() == I32 && arg9.GetType() == I32 && arg10.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I64 && arg3.GetType() == I64 && arg4.GetType() == I64 && arg5.GetType() == I64 && arg6.GetType() == I64 && arg7.GetType() == I64 && arg8.GetType() == I64 && arg9.GetType() == I64 && arg10.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == F32 && arg3.GetType() == F32 && arg4.GetType() == F32 && arg5.GetType() == F32 && arg6.GetType() == F32 && arg7.GetType() == F32 && arg8.GetType() == F32 && arg9.GetType() == F32 && arg10.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == F64 && arg3.GetType() == F64 && arg4.GetType() == F64 && arg5.GetType() == F64 && arg6.GetType() == F64 && arg7.GetType() == F64 && arg8.GetType() == F64 && arg9.GetType() == F64 && arg10.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == Str && arg2.GetType() == Str && arg3.GetType() == Str && arg4.GetType() == Str && arg5.GetType() == Str && arg6.GetType() == Str && arg7.GetType() == Str && arg8.GetType() == Str && arg9.GetType() == Str && arg10.GetType() == Str)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>>
				(1, f, std::forward<Nodes>(nodes)...);
	}
	throw MismatchType("");
}
template <class Container, class Func, any_node ...Nodes>
	requires std::same_as<std::decay_t<Func>, eval::Min>
eval::RttiFuncNode<Container> MakeRttiFuncNode(int, Func&& f, ValueList<>, std::tuple<>, Nodes&& ...nodes)
{
	using enum FieldType;
	if constexpr (sizeof...(Nodes) == 2)
	{
		const auto& [ arg1, arg2 ] = std::forward_as_tuple(std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == Str && arg2.GetType() == Str)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>>
				(1, f, std::forward<Nodes>(nodes)...);
	}
	if constexpr (sizeof...(Nodes) == 3)
	{
		const auto& [ arg1, arg2, arg3 ] = std::forward_as_tuple(std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I08 && arg3.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I16 && arg3.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I32 && arg3.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I64 && arg3.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == F32 && arg3.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == F64 && arg3.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == Str && arg2.GetType() == Str && arg3.GetType() == Str)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>>
				(1, f, std::forward<Nodes>(nodes)...);
	}
	if constexpr (sizeof...(Nodes) == 4)
	{
		const auto& [ arg1, arg2, arg3, arg4 ] = std::forward_as_tuple(std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I08 && arg3.GetType() == I08 && arg4.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I16 && arg3.GetType() == I16 && arg4.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I32 && arg3.GetType() == I32 && arg4.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I64 && arg3.GetType() == I64 && arg4.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == F32 && arg3.GetType() == F32 && arg4.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == F64 && arg3.GetType() == F64 && arg4.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == Str && arg2.GetType() == Str && arg3.GetType() == Str && arg4.GetType() == Str)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>>
				(1, f, std::forward<Nodes>(nodes)...);
	}
	if constexpr (sizeof...(Nodes) == 5)
	{
		const auto& [ arg1, arg2, arg3, arg4, arg5 ] = std::forward_as_tuple(std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I08 && arg3.GetType() == I08 && arg4.GetType() == I08 && arg5.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I16 && arg3.GetType() == I16 && arg4.GetType() == I16 && arg5.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I32 && arg3.GetType() == I32 && arg4.GetType() == I32 && arg5.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I64 && arg3.GetType() == I64 && arg4.GetType() == I64 && arg5.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == F32 && arg3.GetType() == F32 && arg4.GetType() == F32 && arg5.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == F64 && arg3.GetType() == F64 && arg4.GetType() == F64 && arg5.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == Str && arg2.GetType() == Str && arg3.GetType() == Str && arg4.GetType() == Str && arg5.GetType() == Str)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>>
				(1, f, std::forward<Nodes>(nodes)...);
	}
	if constexpr (sizeof...(Nodes) == 6)
	{
		const auto& [ arg1, arg2, arg3, arg4, arg5, arg6 ] = std::forward_as_tuple(std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I08 && arg3.GetType() == I08 && arg4.GetType() == I08 && arg5.GetType() == I08 && arg6.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I16 && arg3.GetType() == I16 && arg4.GetType() == I16 && arg5.GetType() == I16 && arg6.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I32 && arg3.GetType() == I32 && arg4.GetType() == I32 && arg5.GetType() == I32 && arg6.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I64 && arg3.GetType() == I64 && arg4.GetType() == I64 && arg5.GetType() == I64 && arg6.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == F32 && arg3.GetType() == F32 && arg4.GetType() == F32 && arg5.GetType() == F32 && arg6.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == F64 && arg3.GetType() == F64 && arg4.GetType() == F64 && arg5.GetType() == F64 && arg6.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == Str && arg2.GetType() == Str && arg3.GetType() == Str && arg4.GetType() == Str && arg5.GetType() == Str && arg6.GetType() == Str)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>>
				(1, f, std::forward<Nodes>(nodes)...);
	}
	if constexpr (sizeof...(Nodes) == 7)
	{
		const auto& [ arg1, arg2, arg3, arg4, arg5, arg6, arg7 ] = std::forward_as_tuple(std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I08 && arg3.GetType() == I08 && arg4.GetType() == I08 && arg5.GetType() == I08 && arg6.GetType() == I08 && arg7.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I16 && arg3.GetType() == I16 && arg4.GetType() == I16 && arg5.GetType() == I16 && arg6.GetType() == I16 && arg7.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I32 && arg3.GetType() == I32 && arg4.GetType() == I32 && arg5.GetType() == I32 && arg6.GetType() == I32 && arg7.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I64 && arg3.GetType() == I64 && arg4.GetType() == I64 && arg5.GetType() == I64 && arg6.GetType() == I64 && arg7.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == F32 && arg3.GetType() == F32 && arg4.GetType() == F32 && arg5.GetType() == F32 && arg6.GetType() == F32 && arg7.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == F64 && arg3.GetType() == F64 && arg4.GetType() == F64 && arg5.GetType() == F64 && arg6.GetType() == F64 && arg7.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == Str && arg2.GetType() == Str && arg3.GetType() == Str && arg4.GetType() == Str && arg5.GetType() == Str && arg6.GetType() == Str && arg7.GetType() == Str)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>>
				(1, f, std::forward<Nodes>(nodes)...);
	}
	if constexpr (sizeof...(Nodes) == 8)
	{
		const auto& [ arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8 ] = std::forward_as_tuple(std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I08 && arg3.GetType() == I08 && arg4.GetType() == I08 && arg5.GetType() == I08 && arg6.GetType() == I08 && arg7.GetType() == I08 && arg8.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I16 && arg3.GetType() == I16 && arg4.GetType() == I16 && arg5.GetType() == I16 && arg6.GetType() == I16 && arg7.GetType() == I16 && arg8.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I32 && arg3.GetType() == I32 && arg4.GetType() == I32 && arg5.GetType() == I32 && arg6.GetType() == I32 && arg7.GetType() == I32 && arg8.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I64 && arg3.GetType() == I64 && arg4.GetType() == I64 && arg5.GetType() == I64 && arg6.GetType() == I64 && arg7.GetType() == I64 && arg8.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == F32 && arg3.GetType() == F32 && arg4.GetType() == F32 && arg5.GetType() == F32 && arg6.GetType() == F32 && arg7.GetType() == F32 && arg8.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == F64 && arg3.GetType() == F64 && arg4.GetType() == F64 && arg5.GetType() == F64 && arg6.GetType() == F64 && arg7.GetType() == F64 && arg8.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == Str && arg2.GetType() == Str && arg3.GetType() == Str && arg4.GetType() == Str && arg5.GetType() == Str && arg6.GetType() == Str && arg7.GetType() == Str && arg8.GetType() == Str)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>>
				(1, f, std::forward<Nodes>(nodes)...);
	}
	if constexpr (sizeof...(Nodes) == 9)
	{
		const auto& [ arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9 ] = std::forward_as_tuple(std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I08 && arg3.GetType() == I08 && arg4.GetType() == I08 && arg5.GetType() == I08 && arg6.GetType() == I08 && arg7.GetType() == I08 && arg8.GetType() == I08 && arg9.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I16 && arg3.GetType() == I16 && arg4.GetType() == I16 && arg5.GetType() == I16 && arg6.GetType() == I16 && arg7.GetType() == I16 && arg8.GetType() == I16 && arg9.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I32 && arg3.GetType() == I32 && arg4.GetType() == I32 && arg5.GetType() == I32 && arg6.GetType() == I32 && arg7.GetType() == I32 && arg8.GetType() == I32 && arg9.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I64 && arg3.GetType() == I64 && arg4.GetType() == I64 && arg5.GetType() == I64 && arg6.GetType() == I64 && arg7.GetType() == I64 && arg8.GetType() == I64 && arg9.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == F32 && arg3.GetType() == F32 && arg4.GetType() == F32 && arg5.GetType() == F32 && arg6.GetType() == F32 && arg7.GetType() == F32 && arg8.GetType() == F32 && arg9.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == F64 && arg3.GetType() == F64 && arg4.GetType() == F64 && arg5.GetType() == F64 && arg6.GetType() == F64 && arg7.GetType() == F64 && arg8.GetType() == F64 && arg9.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == Str && arg2.GetType() == Str && arg3.GetType() == Str && arg4.GetType() == Str && arg5.GetType() == Str && arg6.GetType() == Str && arg7.GetType() == Str && arg8.GetType() == Str && arg9.GetType() == Str)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>>
				(1, f, std::forward<Nodes>(nodes)...);
	}
	if constexpr (sizeof...(Nodes) == 10)
	{
		const auto& [ arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10 ] = std::forward_as_tuple(std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I08 && arg3.GetType() == I08 && arg4.GetType() == I08 && arg5.GetType() == I08 && arg6.GetType() == I08 && arg7.GetType() == I08 && arg8.GetType() == I08 && arg9.GetType() == I08 && arg10.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I16 && arg3.GetType() == I16 && arg4.GetType() == I16 && arg5.GetType() == I16 && arg6.GetType() == I16 && arg7.GetType() == I16 && arg8.GetType() == I16 && arg9.GetType() == I16 && arg10.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I32 && arg3.GetType() == I32 && arg4.GetType() == I32 && arg5.GetType() == I32 && arg6.GetType() == I32 && arg7.GetType() == I32 && arg8.GetType() == I32 && arg9.GetType() == I32 && arg10.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I64 && arg3.GetType() == I64 && arg4.GetType() == I64 && arg5.GetType() == I64 && arg6.GetType() == I64 && arg7.GetType() == I64 && arg8.GetType() == I64 && arg9.GetType() == I64 && arg10.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == F32 && arg3.GetType() == F32 && arg4.GetType() == F32 && arg5.GetType() == F32 && arg6.GetType() == F32 && arg7.GetType() == F32 && arg8.GetType() == F32 && arg9.GetType() == F32 && arg10.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == F64 && arg3.GetType() == F64 && arg4.GetType() == F64 && arg5.GetType() == F64 && arg6.GetType() == F64 && arg7.GetType() == F64 && arg8.GetType() == F64 && arg9.GetType() == F64 && arg10.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == Str && arg2.GetType() == Str && arg3.GetType() == Str && arg4.GetType() == Str && arg5.GetType() == Str && arg6.GetType() == Str && arg7.GetType() == Str && arg8.GetType() == Str && arg9.GetType() == Str && arg10.GetType() == Str)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>>
				(1, f, std::forward<Nodes>(nodes)...);
	}
	throw MismatchType("");
}
template <class Container, class Func, any_node ...Nodes>
	requires std::same_as<std::decay_t<Func>, eval::IfFunction>
eval::RttiFuncNode<Container> MakeRttiFuncNode(int, Func&& f, ValueList<>, std::tuple<>, Nodes&& ...nodes)
{
	using enum FieldType;
	if constexpr (sizeof...(Nodes) == 3)
	{
		const auto& [ arg1, arg2, arg3 ] = std::forward_as_tuple(std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I08 && arg3.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I16 && arg3.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I32 && arg3.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I64 && arg3.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == F32 && arg3.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == F64 && arg3.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == C32 && arg3.GetType() == C32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == C64 && arg3.GetType() == C64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == Str && arg3.GetType() == Str)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == Jbp && arg3.GetType() == Jbp)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I08 && arg3.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I16 && arg3.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I32 && arg3.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I64 && arg3.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == F32 && arg3.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == F64 && arg3.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == C32 && arg3.GetType() == C32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == C64 && arg3.GetType() == C64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == Str && arg3.GetType() == Str)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == Jbp && arg3.GetType() == Jbp)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I08 && arg3.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I16 && arg3.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I32 && arg3.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I64 && arg3.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == F32 && arg3.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == F64 && arg3.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == C32 && arg3.GetType() == C32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == C64 && arg3.GetType() == C64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == Str && arg3.GetType() == Str)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == Jbp && arg3.GetType() == Jbp)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I08 && arg3.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I16 && arg3.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I32 && arg3.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I64 && arg3.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == F32 && arg3.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == F64 && arg3.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == C32 && arg3.GetType() == C32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == C64 && arg3.GetType() == C64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == Str && arg3.GetType() == Str)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == Jbp && arg3.GetType() == Jbp)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == I08 && arg3.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == I16 && arg3.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == I32 && arg3.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == I64 && arg3.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == F32 && arg3.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == F64 && arg3.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == C32 && arg3.GetType() == C32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == C64 && arg3.GetType() == C64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == Str && arg3.GetType() == Str)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F32 && arg2.GetType() == Jbp && arg3.GetType() == Jbp)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == I08 && arg3.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == I16 && arg3.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == I32 && arg3.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == I64 && arg3.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == F32 && arg3.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == F64 && arg3.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == C32 && arg3.GetType() == C32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == C64 && arg3.GetType() == C64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == Str && arg3.GetType() == Str)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == F64 && arg2.GetType() == Jbp && arg3.GetType() == Jbp)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>>
				(1, f, std::forward<Nodes>(nodes)...);
	}
	throw MismatchType("");
}
template <class Container, class Func, any_node ...Nodes>
	requires std::same_as<std::decay_t<Func>, eval::SwitchFunction>
eval::RttiFuncNode<Container> MakeRttiFuncNode(int, Func&& f, ValueList<>, std::tuple<>, Nodes&& ...nodes)
{
	using enum FieldType;
	if constexpr (sizeof...(Nodes) == 2)
	{
		const auto& [ arg1, arg2 ] = std::forward_as_tuple(std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == C32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<C32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == C64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<C64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == Str)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<Str>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == Jbp)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<Jbp>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == C32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<C32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == C64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<C64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == Str)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<Str>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == Jbp)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<Jbp>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == C32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<C32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == C64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<C64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == Str)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<Str>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == Jbp)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<Jbp>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == C32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<C32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == C64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<C64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == Str)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<Str>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == Jbp)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<Jbp>>
				(1, f, std::forward<Nodes>(nodes)...);
	}
	if constexpr (sizeof...(Nodes) == 3)
	{
		const auto& [ arg1, arg2, arg3 ] = std::forward_as_tuple(std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I08 && arg3.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I16 && arg3.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I32 && arg3.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I64 && arg3.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == F32 && arg3.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == F64 && arg3.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == C32 && arg3.GetType() == C32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == C64 && arg3.GetType() == C64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == Str && arg3.GetType() == Str)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == Jbp && arg3.GetType() == Jbp)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I08 && arg3.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I16 && arg3.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I32 && arg3.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I64 && arg3.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == F32 && arg3.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == F64 && arg3.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == C32 && arg3.GetType() == C32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == C64 && arg3.GetType() == C64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == Str && arg3.GetType() == Str)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == Jbp && arg3.GetType() == Jbp)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I08 && arg3.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I16 && arg3.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I32 && arg3.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I64 && arg3.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == F32 && arg3.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == F64 && arg3.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == C32 && arg3.GetType() == C32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == C64 && arg3.GetType() == C64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == Str && arg3.GetType() == Str)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == Jbp && arg3.GetType() == Jbp)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I08 && arg3.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I16 && arg3.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I32 && arg3.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I64 && arg3.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == F32 && arg3.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == F64 && arg3.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == C32 && arg3.GetType() == C32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == C64 && arg3.GetType() == C64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == Str && arg3.GetType() == Str)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == Jbp && arg3.GetType() == Jbp)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>>
				(1, f, std::forward<Nodes>(nodes)...);
	}
	if constexpr (sizeof...(Nodes) == 4)
	{
		const auto& [ arg1, arg2, arg3, arg4 ] = std::forward_as_tuple(std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I08 && arg3.GetType() == I08 && arg4.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I16 && arg3.GetType() == I16 && arg4.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I32 && arg3.GetType() == I32 && arg4.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I64 && arg3.GetType() == I64 && arg4.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == F32 && arg3.GetType() == F32 && arg4.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == F64 && arg3.GetType() == F64 && arg4.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == C32 && arg3.GetType() == C32 && arg4.GetType() == C32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == C64 && arg3.GetType() == C64 && arg4.GetType() == C64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == Str && arg3.GetType() == Str && arg4.GetType() == Str)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == Jbp && arg3.GetType() == Jbp && arg4.GetType() == Jbp)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I08 && arg3.GetType() == I08 && arg4.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I16 && arg3.GetType() == I16 && arg4.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I32 && arg3.GetType() == I32 && arg4.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I64 && arg3.GetType() == I64 && arg4.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == F32 && arg3.GetType() == F32 && arg4.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == F64 && arg3.GetType() == F64 && arg4.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == C32 && arg3.GetType() == C32 && arg4.GetType() == C32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == C64 && arg3.GetType() == C64 && arg4.GetType() == C64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == Str && arg3.GetType() == Str && arg4.GetType() == Str)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == Jbp && arg3.GetType() == Jbp && arg4.GetType() == Jbp)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I08 && arg3.GetType() == I08 && arg4.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I16 && arg3.GetType() == I16 && arg4.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I32 && arg3.GetType() == I32 && arg4.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I64 && arg3.GetType() == I64 && arg4.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == F32 && arg3.GetType() == F32 && arg4.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == F64 && arg3.GetType() == F64 && arg4.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == C32 && arg3.GetType() == C32 && arg4.GetType() == C32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == C64 && arg3.GetType() == C64 && arg4.GetType() == C64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == Str && arg3.GetType() == Str && arg4.GetType() == Str)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == Jbp && arg3.GetType() == Jbp && arg4.GetType() == Jbp)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I08 && arg3.GetType() == I08 && arg4.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I16 && arg3.GetType() == I16 && arg4.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I32 && arg3.GetType() == I32 && arg4.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I64 && arg3.GetType() == I64 && arg4.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == F32 && arg3.GetType() == F32 && arg4.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == F64 && arg3.GetType() == F64 && arg4.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == C32 && arg3.GetType() == C32 && arg4.GetType() == C32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == C64 && arg3.GetType() == C64 && arg4.GetType() == C64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == Str && arg3.GetType() == Str && arg4.GetType() == Str)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == Jbp && arg3.GetType() == Jbp && arg4.GetType() == Jbp)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>>
				(1, f, std::forward<Nodes>(nodes)...);
	}
	if constexpr (sizeof...(Nodes) == 5)
	{
		const auto& [ arg1, arg2, arg3, arg4, arg5 ] = std::forward_as_tuple(std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I08 && arg3.GetType() == I08 && arg4.GetType() == I08 && arg5.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I16 && arg3.GetType() == I16 && arg4.GetType() == I16 && arg5.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I32 && arg3.GetType() == I32 && arg4.GetType() == I32 && arg5.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I64 && arg3.GetType() == I64 && arg4.GetType() == I64 && arg5.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == F32 && arg3.GetType() == F32 && arg4.GetType() == F32 && arg5.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == F64 && arg3.GetType() == F64 && arg4.GetType() == F64 && arg5.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == C32 && arg3.GetType() == C32 && arg4.GetType() == C32 && arg5.GetType() == C32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == C64 && arg3.GetType() == C64 && arg4.GetType() == C64 && arg5.GetType() == C64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == Str && arg3.GetType() == Str && arg4.GetType() == Str && arg5.GetType() == Str)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == Jbp && arg3.GetType() == Jbp && arg4.GetType() == Jbp && arg5.GetType() == Jbp)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I08 && arg3.GetType() == I08 && arg4.GetType() == I08 && arg5.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I16 && arg3.GetType() == I16 && arg4.GetType() == I16 && arg5.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I32 && arg3.GetType() == I32 && arg4.GetType() == I32 && arg5.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I64 && arg3.GetType() == I64 && arg4.GetType() == I64 && arg5.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == F32 && arg3.GetType() == F32 && arg4.GetType() == F32 && arg5.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == F64 && arg3.GetType() == F64 && arg4.GetType() == F64 && arg5.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == C32 && arg3.GetType() == C32 && arg4.GetType() == C32 && arg5.GetType() == C32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == C64 && arg3.GetType() == C64 && arg4.GetType() == C64 && arg5.GetType() == C64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == Str && arg3.GetType() == Str && arg4.GetType() == Str && arg5.GetType() == Str)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == Jbp && arg3.GetType() == Jbp && arg4.GetType() == Jbp && arg5.GetType() == Jbp)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I08 && arg3.GetType() == I08 && arg4.GetType() == I08 && arg5.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I16 && arg3.GetType() == I16 && arg4.GetType() == I16 && arg5.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I32 && arg3.GetType() == I32 && arg4.GetType() == I32 && arg5.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I64 && arg3.GetType() == I64 && arg4.GetType() == I64 && arg5.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == F32 && arg3.GetType() == F32 && arg4.GetType() == F32 && arg5.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == F64 && arg3.GetType() == F64 && arg4.GetType() == F64 && arg5.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == C32 && arg3.GetType() == C32 && arg4.GetType() == C32 && arg5.GetType() == C32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == C64 && arg3.GetType() == C64 && arg4.GetType() == C64 && arg5.GetType() == C64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == Str && arg3.GetType() == Str && arg4.GetType() == Str && arg5.GetType() == Str)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == Jbp && arg3.GetType() == Jbp && arg4.GetType() == Jbp && arg5.GetType() == Jbp)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I08 && arg3.GetType() == I08 && arg4.GetType() == I08 && arg5.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I16 && arg3.GetType() == I16 && arg4.GetType() == I16 && arg5.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I32 && arg3.GetType() == I32 && arg4.GetType() == I32 && arg5.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I64 && arg3.GetType() == I64 && arg4.GetType() == I64 && arg5.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == F32 && arg3.GetType() == F32 && arg4.GetType() == F32 && arg5.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == F64 && arg3.GetType() == F64 && arg4.GetType() == F64 && arg5.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == C32 && arg3.GetType() == C32 && arg4.GetType() == C32 && arg5.GetType() == C32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == C64 && arg3.GetType() == C64 && arg4.GetType() == C64 && arg5.GetType() == C64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == Str && arg3.GetType() == Str && arg4.GetType() == Str && arg5.GetType() == Str)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == Jbp && arg3.GetType() == Jbp && arg4.GetType() == Jbp && arg5.GetType() == Jbp)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>>
				(1, f, std::forward<Nodes>(nodes)...);
	}
	if constexpr (sizeof...(Nodes) == 6)
	{
		const auto& [ arg1, arg2, arg3, arg4, arg5, arg6 ] = std::forward_as_tuple(std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I08 && arg3.GetType() == I08 && arg4.GetType() == I08 && arg5.GetType() == I08 && arg6.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I16 && arg3.GetType() == I16 && arg4.GetType() == I16 && arg5.GetType() == I16 && arg6.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I32 && arg3.GetType() == I32 && arg4.GetType() == I32 && arg5.GetType() == I32 && arg6.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I64 && arg3.GetType() == I64 && arg4.GetType() == I64 && arg5.GetType() == I64 && arg6.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == F32 && arg3.GetType() == F32 && arg4.GetType() == F32 && arg5.GetType() == F32 && arg6.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == F64 && arg3.GetType() == F64 && arg4.GetType() == F64 && arg5.GetType() == F64 && arg6.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == C32 && arg3.GetType() == C32 && arg4.GetType() == C32 && arg5.GetType() == C32 && arg6.GetType() == C32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == C64 && arg3.GetType() == C64 && arg4.GetType() == C64 && arg5.GetType() == C64 && arg6.GetType() == C64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == Str && arg3.GetType() == Str && arg4.GetType() == Str && arg5.GetType() == Str && arg6.GetType() == Str)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == Jbp && arg3.GetType() == Jbp && arg4.GetType() == Jbp && arg5.GetType() == Jbp && arg6.GetType() == Jbp)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I08 && arg3.GetType() == I08 && arg4.GetType() == I08 && arg5.GetType() == I08 && arg6.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I16 && arg3.GetType() == I16 && arg4.GetType() == I16 && arg5.GetType() == I16 && arg6.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I32 && arg3.GetType() == I32 && arg4.GetType() == I32 && arg5.GetType() == I32 && arg6.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I64 && arg3.GetType() == I64 && arg4.GetType() == I64 && arg5.GetType() == I64 && arg6.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == F32 && arg3.GetType() == F32 && arg4.GetType() == F32 && arg5.GetType() == F32 && arg6.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == F64 && arg3.GetType() == F64 && arg4.GetType() == F64 && arg5.GetType() == F64 && arg6.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == C32 && arg3.GetType() == C32 && arg4.GetType() == C32 && arg5.GetType() == C32 && arg6.GetType() == C32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == C64 && arg3.GetType() == C64 && arg4.GetType() == C64 && arg5.GetType() == C64 && arg6.GetType() == C64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == Str && arg3.GetType() == Str && arg4.GetType() == Str && arg5.GetType() == Str && arg6.GetType() == Str)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == Jbp && arg3.GetType() == Jbp && arg4.GetType() == Jbp && arg5.GetType() == Jbp && arg6.GetType() == Jbp)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I08 && arg3.GetType() == I08 && arg4.GetType() == I08 && arg5.GetType() == I08 && arg6.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I16 && arg3.GetType() == I16 && arg4.GetType() == I16 && arg5.GetType() == I16 && arg6.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I32 && arg3.GetType() == I32 && arg4.GetType() == I32 && arg5.GetType() == I32 && arg6.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I64 && arg3.GetType() == I64 && arg4.GetType() == I64 && arg5.GetType() == I64 && arg6.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == F32 && arg3.GetType() == F32 && arg4.GetType() == F32 && arg5.GetType() == F32 && arg6.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == F64 && arg3.GetType() == F64 && arg4.GetType() == F64 && arg5.GetType() == F64 && arg6.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == C32 && arg3.GetType() == C32 && arg4.GetType() == C32 && arg5.GetType() == C32 && arg6.GetType() == C32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == C64 && arg3.GetType() == C64 && arg4.GetType() == C64 && arg5.GetType() == C64 && arg6.GetType() == C64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == Str && arg3.GetType() == Str && arg4.GetType() == Str && arg5.GetType() == Str && arg6.GetType() == Str)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == Jbp && arg3.GetType() == Jbp && arg4.GetType() == Jbp && arg5.GetType() == Jbp && arg6.GetType() == Jbp)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I08 && arg3.GetType() == I08 && arg4.GetType() == I08 && arg5.GetType() == I08 && arg6.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I16 && arg3.GetType() == I16 && arg4.GetType() == I16 && arg5.GetType() == I16 && arg6.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I32 && arg3.GetType() == I32 && arg4.GetType() == I32 && arg5.GetType() == I32 && arg6.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I64 && arg3.GetType() == I64 && arg4.GetType() == I64 && arg5.GetType() == I64 && arg6.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == F32 && arg3.GetType() == F32 && arg4.GetType() == F32 && arg5.GetType() == F32 && arg6.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == F64 && arg3.GetType() == F64 && arg4.GetType() == F64 && arg5.GetType() == F64 && arg6.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == C32 && arg3.GetType() == C32 && arg4.GetType() == C32 && arg5.GetType() == C32 && arg6.GetType() == C32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == C64 && arg3.GetType() == C64 && arg4.GetType() == C64 && arg5.GetType() == C64 && arg6.GetType() == C64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == Str && arg3.GetType() == Str && arg4.GetType() == Str && arg5.GetType() == Str && arg6.GetType() == Str)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == Jbp && arg3.GetType() == Jbp && arg4.GetType() == Jbp && arg5.GetType() == Jbp && arg6.GetType() == Jbp)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>>
				(1, f, std::forward<Nodes>(nodes)...);
	}
	if constexpr (sizeof...(Nodes) == 7)
	{
		const auto& [ arg1, arg2, arg3, arg4, arg5, arg6, arg7 ] = std::forward_as_tuple(std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I08 && arg3.GetType() == I08 && arg4.GetType() == I08 && arg5.GetType() == I08 && arg6.GetType() == I08 && arg7.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I16 && arg3.GetType() == I16 && arg4.GetType() == I16 && arg5.GetType() == I16 && arg6.GetType() == I16 && arg7.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I32 && arg3.GetType() == I32 && arg4.GetType() == I32 && arg5.GetType() == I32 && arg6.GetType() == I32 && arg7.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I64 && arg3.GetType() == I64 && arg4.GetType() == I64 && arg5.GetType() == I64 && arg6.GetType() == I64 && arg7.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == F32 && arg3.GetType() == F32 && arg4.GetType() == F32 && arg5.GetType() == F32 && arg6.GetType() == F32 && arg7.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == F64 && arg3.GetType() == F64 && arg4.GetType() == F64 && arg5.GetType() == F64 && arg6.GetType() == F64 && arg7.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == C32 && arg3.GetType() == C32 && arg4.GetType() == C32 && arg5.GetType() == C32 && arg6.GetType() == C32 && arg7.GetType() == C32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == C64 && arg3.GetType() == C64 && arg4.GetType() == C64 && arg5.GetType() == C64 && arg6.GetType() == C64 && arg7.GetType() == C64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == Str && arg3.GetType() == Str && arg4.GetType() == Str && arg5.GetType() == Str && arg6.GetType() == Str && arg7.GetType() == Str)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == Jbp && arg3.GetType() == Jbp && arg4.GetType() == Jbp && arg5.GetType() == Jbp && arg6.GetType() == Jbp && arg7.GetType() == Jbp)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I08 && arg3.GetType() == I08 && arg4.GetType() == I08 && arg5.GetType() == I08 && arg6.GetType() == I08 && arg7.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I16 && arg3.GetType() == I16 && arg4.GetType() == I16 && arg5.GetType() == I16 && arg6.GetType() == I16 && arg7.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I32 && arg3.GetType() == I32 && arg4.GetType() == I32 && arg5.GetType() == I32 && arg6.GetType() == I32 && arg7.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I64 && arg3.GetType() == I64 && arg4.GetType() == I64 && arg5.GetType() == I64 && arg6.GetType() == I64 && arg7.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == F32 && arg3.GetType() == F32 && arg4.GetType() == F32 && arg5.GetType() == F32 && arg6.GetType() == F32 && arg7.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == F64 && arg3.GetType() == F64 && arg4.GetType() == F64 && arg5.GetType() == F64 && arg6.GetType() == F64 && arg7.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == C32 && arg3.GetType() == C32 && arg4.GetType() == C32 && arg5.GetType() == C32 && arg6.GetType() == C32 && arg7.GetType() == C32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == C64 && arg3.GetType() == C64 && arg4.GetType() == C64 && arg5.GetType() == C64 && arg6.GetType() == C64 && arg7.GetType() == C64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == Str && arg3.GetType() == Str && arg4.GetType() == Str && arg5.GetType() == Str && arg6.GetType() == Str && arg7.GetType() == Str)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == Jbp && arg3.GetType() == Jbp && arg4.GetType() == Jbp && arg5.GetType() == Jbp && arg6.GetType() == Jbp && arg7.GetType() == Jbp)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I08 && arg3.GetType() == I08 && arg4.GetType() == I08 && arg5.GetType() == I08 && arg6.GetType() == I08 && arg7.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I16 && arg3.GetType() == I16 && arg4.GetType() == I16 && arg5.GetType() == I16 && arg6.GetType() == I16 && arg7.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I32 && arg3.GetType() == I32 && arg4.GetType() == I32 && arg5.GetType() == I32 && arg6.GetType() == I32 && arg7.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I64 && arg3.GetType() == I64 && arg4.GetType() == I64 && arg5.GetType() == I64 && arg6.GetType() == I64 && arg7.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == F32 && arg3.GetType() == F32 && arg4.GetType() == F32 && arg5.GetType() == F32 && arg6.GetType() == F32 && arg7.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == F64 && arg3.GetType() == F64 && arg4.GetType() == F64 && arg5.GetType() == F64 && arg6.GetType() == F64 && arg7.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == C32 && arg3.GetType() == C32 && arg4.GetType() == C32 && arg5.GetType() == C32 && arg6.GetType() == C32 && arg7.GetType() == C32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == C64 && arg3.GetType() == C64 && arg4.GetType() == C64 && arg5.GetType() == C64 && arg6.GetType() == C64 && arg7.GetType() == C64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == Str && arg3.GetType() == Str && arg4.GetType() == Str && arg5.GetType() == Str && arg6.GetType() == Str && arg7.GetType() == Str)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == Jbp && arg3.GetType() == Jbp && arg4.GetType() == Jbp && arg5.GetType() == Jbp && arg6.GetType() == Jbp && arg7.GetType() == Jbp)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I08 && arg3.GetType() == I08 && arg4.GetType() == I08 && arg5.GetType() == I08 && arg6.GetType() == I08 && arg7.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I16 && arg3.GetType() == I16 && arg4.GetType() == I16 && arg5.GetType() == I16 && arg6.GetType() == I16 && arg7.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I32 && arg3.GetType() == I32 && arg4.GetType() == I32 && arg5.GetType() == I32 && arg6.GetType() == I32 && arg7.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I64 && arg3.GetType() == I64 && arg4.GetType() == I64 && arg5.GetType() == I64 && arg6.GetType() == I64 && arg7.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == F32 && arg3.GetType() == F32 && arg4.GetType() == F32 && arg5.GetType() == F32 && arg6.GetType() == F32 && arg7.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == F64 && arg3.GetType() == F64 && arg4.GetType() == F64 && arg5.GetType() == F64 && arg6.GetType() == F64 && arg7.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == C32 && arg3.GetType() == C32 && arg4.GetType() == C32 && arg5.GetType() == C32 && arg6.GetType() == C32 && arg7.GetType() == C32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == C64 && arg3.GetType() == C64 && arg4.GetType() == C64 && arg5.GetType() == C64 && arg6.GetType() == C64 && arg7.GetType() == C64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == Str && arg3.GetType() == Str && arg4.GetType() == Str && arg5.GetType() == Str && arg6.GetType() == Str && arg7.GetType() == Str)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == Jbp && arg3.GetType() == Jbp && arg4.GetType() == Jbp && arg5.GetType() == Jbp && arg6.GetType() == Jbp && arg7.GetType() == Jbp)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>>
				(1, f, std::forward<Nodes>(nodes)...);
	}
	if constexpr (sizeof...(Nodes) == 8)
	{
		const auto& [ arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8 ] = std::forward_as_tuple(std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I08 && arg3.GetType() == I08 && arg4.GetType() == I08 && arg5.GetType() == I08 && arg6.GetType() == I08 && arg7.GetType() == I08 && arg8.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I16 && arg3.GetType() == I16 && arg4.GetType() == I16 && arg5.GetType() == I16 && arg6.GetType() == I16 && arg7.GetType() == I16 && arg8.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I32 && arg3.GetType() == I32 && arg4.GetType() == I32 && arg5.GetType() == I32 && arg6.GetType() == I32 && arg7.GetType() == I32 && arg8.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I64 && arg3.GetType() == I64 && arg4.GetType() == I64 && arg5.GetType() == I64 && arg6.GetType() == I64 && arg7.GetType() == I64 && arg8.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == F32 && arg3.GetType() == F32 && arg4.GetType() == F32 && arg5.GetType() == F32 && arg6.GetType() == F32 && arg7.GetType() == F32 && arg8.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == F64 && arg3.GetType() == F64 && arg4.GetType() == F64 && arg5.GetType() == F64 && arg6.GetType() == F64 && arg7.GetType() == F64 && arg8.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == C32 && arg3.GetType() == C32 && arg4.GetType() == C32 && arg5.GetType() == C32 && arg6.GetType() == C32 && arg7.GetType() == C32 && arg8.GetType() == C32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == C64 && arg3.GetType() == C64 && arg4.GetType() == C64 && arg5.GetType() == C64 && arg6.GetType() == C64 && arg7.GetType() == C64 && arg8.GetType() == C64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == Str && arg3.GetType() == Str && arg4.GetType() == Str && arg5.GetType() == Str && arg6.GetType() == Str && arg7.GetType() == Str && arg8.GetType() == Str)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == Jbp && arg3.GetType() == Jbp && arg4.GetType() == Jbp && arg5.GetType() == Jbp && arg6.GetType() == Jbp && arg7.GetType() == Jbp && arg8.GetType() == Jbp)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I08 && arg3.GetType() == I08 && arg4.GetType() == I08 && arg5.GetType() == I08 && arg6.GetType() == I08 && arg7.GetType() == I08 && arg8.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I16 && arg3.GetType() == I16 && arg4.GetType() == I16 && arg5.GetType() == I16 && arg6.GetType() == I16 && arg7.GetType() == I16 && arg8.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I32 && arg3.GetType() == I32 && arg4.GetType() == I32 && arg5.GetType() == I32 && arg6.GetType() == I32 && arg7.GetType() == I32 && arg8.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I64 && arg3.GetType() == I64 && arg4.GetType() == I64 && arg5.GetType() == I64 && arg6.GetType() == I64 && arg7.GetType() == I64 && arg8.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == F32 && arg3.GetType() == F32 && arg4.GetType() == F32 && arg5.GetType() == F32 && arg6.GetType() == F32 && arg7.GetType() == F32 && arg8.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == F64 && arg3.GetType() == F64 && arg4.GetType() == F64 && arg5.GetType() == F64 && arg6.GetType() == F64 && arg7.GetType() == F64 && arg8.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == C32 && arg3.GetType() == C32 && arg4.GetType() == C32 && arg5.GetType() == C32 && arg6.GetType() == C32 && arg7.GetType() == C32 && arg8.GetType() == C32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == C64 && arg3.GetType() == C64 && arg4.GetType() == C64 && arg5.GetType() == C64 && arg6.GetType() == C64 && arg7.GetType() == C64 && arg8.GetType() == C64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == Str && arg3.GetType() == Str && arg4.GetType() == Str && arg5.GetType() == Str && arg6.GetType() == Str && arg7.GetType() == Str && arg8.GetType() == Str)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == Jbp && arg3.GetType() == Jbp && arg4.GetType() == Jbp && arg5.GetType() == Jbp && arg6.GetType() == Jbp && arg7.GetType() == Jbp && arg8.GetType() == Jbp)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I08 && arg3.GetType() == I08 && arg4.GetType() == I08 && arg5.GetType() == I08 && arg6.GetType() == I08 && arg7.GetType() == I08 && arg8.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I16 && arg3.GetType() == I16 && arg4.GetType() == I16 && arg5.GetType() == I16 && arg6.GetType() == I16 && arg7.GetType() == I16 && arg8.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I32 && arg3.GetType() == I32 && arg4.GetType() == I32 && arg5.GetType() == I32 && arg6.GetType() == I32 && arg7.GetType() == I32 && arg8.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I64 && arg3.GetType() == I64 && arg4.GetType() == I64 && arg5.GetType() == I64 && arg6.GetType() == I64 && arg7.GetType() == I64 && arg8.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == F32 && arg3.GetType() == F32 && arg4.GetType() == F32 && arg5.GetType() == F32 && arg6.GetType() == F32 && arg7.GetType() == F32 && arg8.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == F64 && arg3.GetType() == F64 && arg4.GetType() == F64 && arg5.GetType() == F64 && arg6.GetType() == F64 && arg7.GetType() == F64 && arg8.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == C32 && arg3.GetType() == C32 && arg4.GetType() == C32 && arg5.GetType() == C32 && arg6.GetType() == C32 && arg7.GetType() == C32 && arg8.GetType() == C32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == C64 && arg3.GetType() == C64 && arg4.GetType() == C64 && arg5.GetType() == C64 && arg6.GetType() == C64 && arg7.GetType() == C64 && arg8.GetType() == C64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == Str && arg3.GetType() == Str && arg4.GetType() == Str && arg5.GetType() == Str && arg6.GetType() == Str && arg7.GetType() == Str && arg8.GetType() == Str)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == Jbp && arg3.GetType() == Jbp && arg4.GetType() == Jbp && arg5.GetType() == Jbp && arg6.GetType() == Jbp && arg7.GetType() == Jbp && arg8.GetType() == Jbp)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I08 && arg3.GetType() == I08 && arg4.GetType() == I08 && arg5.GetType() == I08 && arg6.GetType() == I08 && arg7.GetType() == I08 && arg8.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I16 && arg3.GetType() == I16 && arg4.GetType() == I16 && arg5.GetType() == I16 && arg6.GetType() == I16 && arg7.GetType() == I16 && arg8.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I32 && arg3.GetType() == I32 && arg4.GetType() == I32 && arg5.GetType() == I32 && arg6.GetType() == I32 && arg7.GetType() == I32 && arg8.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I64 && arg3.GetType() == I64 && arg4.GetType() == I64 && arg5.GetType() == I64 && arg6.GetType() == I64 && arg7.GetType() == I64 && arg8.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == F32 && arg3.GetType() == F32 && arg4.GetType() == F32 && arg5.GetType() == F32 && arg6.GetType() == F32 && arg7.GetType() == F32 && arg8.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == F64 && arg3.GetType() == F64 && arg4.GetType() == F64 && arg5.GetType() == F64 && arg6.GetType() == F64 && arg7.GetType() == F64 && arg8.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == C32 && arg3.GetType() == C32 && arg4.GetType() == C32 && arg5.GetType() == C32 && arg6.GetType() == C32 && arg7.GetType() == C32 && arg8.GetType() == C32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == C64 && arg3.GetType() == C64 && arg4.GetType() == C64 && arg5.GetType() == C64 && arg6.GetType() == C64 && arg7.GetType() == C64 && arg8.GetType() == C64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == Str && arg3.GetType() == Str && arg4.GetType() == Str && arg5.GetType() == Str && arg6.GetType() == Str && arg7.GetType() == Str && arg8.GetType() == Str)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == Jbp && arg3.GetType() == Jbp && arg4.GetType() == Jbp && arg5.GetType() == Jbp && arg6.GetType() == Jbp && arg7.GetType() == Jbp && arg8.GetType() == Jbp)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>>
				(1, f, std::forward<Nodes>(nodes)...);
	}
	if constexpr (sizeof...(Nodes) == 9)
	{
		const auto& [ arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9 ] = std::forward_as_tuple(std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I08 && arg3.GetType() == I08 && arg4.GetType() == I08 && arg5.GetType() == I08 && arg6.GetType() == I08 && arg7.GetType() == I08 && arg8.GetType() == I08 && arg9.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I16 && arg3.GetType() == I16 && arg4.GetType() == I16 && arg5.GetType() == I16 && arg6.GetType() == I16 && arg7.GetType() == I16 && arg8.GetType() == I16 && arg9.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I32 && arg3.GetType() == I32 && arg4.GetType() == I32 && arg5.GetType() == I32 && arg6.GetType() == I32 && arg7.GetType() == I32 && arg8.GetType() == I32 && arg9.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I64 && arg3.GetType() == I64 && arg4.GetType() == I64 && arg5.GetType() == I64 && arg6.GetType() == I64 && arg7.GetType() == I64 && arg8.GetType() == I64 && arg9.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == F32 && arg3.GetType() == F32 && arg4.GetType() == F32 && arg5.GetType() == F32 && arg6.GetType() == F32 && arg7.GetType() == F32 && arg8.GetType() == F32 && arg9.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == F64 && arg3.GetType() == F64 && arg4.GetType() == F64 && arg5.GetType() == F64 && arg6.GetType() == F64 && arg7.GetType() == F64 && arg8.GetType() == F64 && arg9.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == C32 && arg3.GetType() == C32 && arg4.GetType() == C32 && arg5.GetType() == C32 && arg6.GetType() == C32 && arg7.GetType() == C32 && arg8.GetType() == C32 && arg9.GetType() == C32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == C64 && arg3.GetType() == C64 && arg4.GetType() == C64 && arg5.GetType() == C64 && arg6.GetType() == C64 && arg7.GetType() == C64 && arg8.GetType() == C64 && arg9.GetType() == C64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == Str && arg3.GetType() == Str && arg4.GetType() == Str && arg5.GetType() == Str && arg6.GetType() == Str && arg7.GetType() == Str && arg8.GetType() == Str && arg9.GetType() == Str)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == Jbp && arg3.GetType() == Jbp && arg4.GetType() == Jbp && arg5.GetType() == Jbp && arg6.GetType() == Jbp && arg7.GetType() == Jbp && arg8.GetType() == Jbp && arg9.GetType() == Jbp)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I08 && arg3.GetType() == I08 && arg4.GetType() == I08 && arg5.GetType() == I08 && arg6.GetType() == I08 && arg7.GetType() == I08 && arg8.GetType() == I08 && arg9.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I16 && arg3.GetType() == I16 && arg4.GetType() == I16 && arg5.GetType() == I16 && arg6.GetType() == I16 && arg7.GetType() == I16 && arg8.GetType() == I16 && arg9.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I32 && arg3.GetType() == I32 && arg4.GetType() == I32 && arg5.GetType() == I32 && arg6.GetType() == I32 && arg7.GetType() == I32 && arg8.GetType() == I32 && arg9.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I64 && arg3.GetType() == I64 && arg4.GetType() == I64 && arg5.GetType() == I64 && arg6.GetType() == I64 && arg7.GetType() == I64 && arg8.GetType() == I64 && arg9.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == F32 && arg3.GetType() == F32 && arg4.GetType() == F32 && arg5.GetType() == F32 && arg6.GetType() == F32 && arg7.GetType() == F32 && arg8.GetType() == F32 && arg9.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == F64 && arg3.GetType() == F64 && arg4.GetType() == F64 && arg5.GetType() == F64 && arg6.GetType() == F64 && arg7.GetType() == F64 && arg8.GetType() == F64 && arg9.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == C32 && arg3.GetType() == C32 && arg4.GetType() == C32 && arg5.GetType() == C32 && arg6.GetType() == C32 && arg7.GetType() == C32 && arg8.GetType() == C32 && arg9.GetType() == C32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == C64 && arg3.GetType() == C64 && arg4.GetType() == C64 && arg5.GetType() == C64 && arg6.GetType() == C64 && arg7.GetType() == C64 && arg8.GetType() == C64 && arg9.GetType() == C64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == Str && arg3.GetType() == Str && arg4.GetType() == Str && arg5.GetType() == Str && arg6.GetType() == Str && arg7.GetType() == Str && arg8.GetType() == Str && arg9.GetType() == Str)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == Jbp && arg3.GetType() == Jbp && arg4.GetType() == Jbp && arg5.GetType() == Jbp && arg6.GetType() == Jbp && arg7.GetType() == Jbp && arg8.GetType() == Jbp && arg9.GetType() == Jbp)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I08 && arg3.GetType() == I08 && arg4.GetType() == I08 && arg5.GetType() == I08 && arg6.GetType() == I08 && arg7.GetType() == I08 && arg8.GetType() == I08 && arg9.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I16 && arg3.GetType() == I16 && arg4.GetType() == I16 && arg5.GetType() == I16 && arg6.GetType() == I16 && arg7.GetType() == I16 && arg8.GetType() == I16 && arg9.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I32 && arg3.GetType() == I32 && arg4.GetType() == I32 && arg5.GetType() == I32 && arg6.GetType() == I32 && arg7.GetType() == I32 && arg8.GetType() == I32 && arg9.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I64 && arg3.GetType() == I64 && arg4.GetType() == I64 && arg5.GetType() == I64 && arg6.GetType() == I64 && arg7.GetType() == I64 && arg8.GetType() == I64 && arg9.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == F32 && arg3.GetType() == F32 && arg4.GetType() == F32 && arg5.GetType() == F32 && arg6.GetType() == F32 && arg7.GetType() == F32 && arg8.GetType() == F32 && arg9.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == F64 && arg3.GetType() == F64 && arg4.GetType() == F64 && arg5.GetType() == F64 && arg6.GetType() == F64 && arg7.GetType() == F64 && arg8.GetType() == F64 && arg9.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == C32 && arg3.GetType() == C32 && arg4.GetType() == C32 && arg5.GetType() == C32 && arg6.GetType() == C32 && arg7.GetType() == C32 && arg8.GetType() == C32 && arg9.GetType() == C32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == C64 && arg3.GetType() == C64 && arg4.GetType() == C64 && arg5.GetType() == C64 && arg6.GetType() == C64 && arg7.GetType() == C64 && arg8.GetType() == C64 && arg9.GetType() == C64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == Str && arg3.GetType() == Str && arg4.GetType() == Str && arg5.GetType() == Str && arg6.GetType() == Str && arg7.GetType() == Str && arg8.GetType() == Str && arg9.GetType() == Str)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == Jbp && arg3.GetType() == Jbp && arg4.GetType() == Jbp && arg5.GetType() == Jbp && arg6.GetType() == Jbp && arg7.GetType() == Jbp && arg8.GetType() == Jbp && arg9.GetType() == Jbp)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I08 && arg3.GetType() == I08 && arg4.GetType() == I08 && arg5.GetType() == I08 && arg6.GetType() == I08 && arg7.GetType() == I08 && arg8.GetType() == I08 && arg9.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I16 && arg3.GetType() == I16 && arg4.GetType() == I16 && arg5.GetType() == I16 && arg6.GetType() == I16 && arg7.GetType() == I16 && arg8.GetType() == I16 && arg9.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I32 && arg3.GetType() == I32 && arg4.GetType() == I32 && arg5.GetType() == I32 && arg6.GetType() == I32 && arg7.GetType() == I32 && arg8.GetType() == I32 && arg9.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I64 && arg3.GetType() == I64 && arg4.GetType() == I64 && arg5.GetType() == I64 && arg6.GetType() == I64 && arg7.GetType() == I64 && arg8.GetType() == I64 && arg9.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == F32 && arg3.GetType() == F32 && arg4.GetType() == F32 && arg5.GetType() == F32 && arg6.GetType() == F32 && arg7.GetType() == F32 && arg8.GetType() == F32 && arg9.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == F64 && arg3.GetType() == F64 && arg4.GetType() == F64 && arg5.GetType() == F64 && arg6.GetType() == F64 && arg7.GetType() == F64 && arg8.GetType() == F64 && arg9.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == C32 && arg3.GetType() == C32 && arg4.GetType() == C32 && arg5.GetType() == C32 && arg6.GetType() == C32 && arg7.GetType() == C32 && arg8.GetType() == C32 && arg9.GetType() == C32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == C64 && arg3.GetType() == C64 && arg4.GetType() == C64 && arg5.GetType() == C64 && arg6.GetType() == C64 && arg7.GetType() == C64 && arg8.GetType() == C64 && arg9.GetType() == C64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == Str && arg3.GetType() == Str && arg4.GetType() == Str && arg5.GetType() == Str && arg6.GetType() == Str && arg7.GetType() == Str && arg8.GetType() == Str && arg9.GetType() == Str)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == Jbp && arg3.GetType() == Jbp && arg4.GetType() == Jbp && arg5.GetType() == Jbp && arg6.GetType() == Jbp && arg7.GetType() == Jbp && arg8.GetType() == Jbp && arg9.GetType() == Jbp)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>>
				(1, f, std::forward<Nodes>(nodes)...);
	}
	if constexpr (sizeof...(Nodes) == 10)
	{
		const auto& [ arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10 ] = std::forward_as_tuple(std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I08 && arg3.GetType() == I08 && arg4.GetType() == I08 && arg5.GetType() == I08 && arg6.GetType() == I08 && arg7.GetType() == I08 && arg8.GetType() == I08 && arg9.GetType() == I08 && arg10.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I16 && arg3.GetType() == I16 && arg4.GetType() == I16 && arg5.GetType() == I16 && arg6.GetType() == I16 && arg7.GetType() == I16 && arg8.GetType() == I16 && arg9.GetType() == I16 && arg10.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I32 && arg3.GetType() == I32 && arg4.GetType() == I32 && arg5.GetType() == I32 && arg6.GetType() == I32 && arg7.GetType() == I32 && arg8.GetType() == I32 && arg9.GetType() == I32 && arg10.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == I64 && arg3.GetType() == I64 && arg4.GetType() == I64 && arg5.GetType() == I64 && arg6.GetType() == I64 && arg7.GetType() == I64 && arg8.GetType() == I64 && arg9.GetType() == I64 && arg10.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == F32 && arg3.GetType() == F32 && arg4.GetType() == F32 && arg5.GetType() == F32 && arg6.GetType() == F32 && arg7.GetType() == F32 && arg8.GetType() == F32 && arg9.GetType() == F32 && arg10.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == F64 && arg3.GetType() == F64 && arg4.GetType() == F64 && arg5.GetType() == F64 && arg6.GetType() == F64 && arg7.GetType() == F64 && arg8.GetType() == F64 && arg9.GetType() == F64 && arg10.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == C32 && arg3.GetType() == C32 && arg4.GetType() == C32 && arg5.GetType() == C32 && arg6.GetType() == C32 && arg7.GetType() == C32 && arg8.GetType() == C32 && arg9.GetType() == C32 && arg10.GetType() == C32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == C64 && arg3.GetType() == C64 && arg4.GetType() == C64 && arg5.GetType() == C64 && arg6.GetType() == C64 && arg7.GetType() == C64 && arg8.GetType() == C64 && arg9.GetType() == C64 && arg10.GetType() == C64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == Str && arg3.GetType() == Str && arg4.GetType() == Str && arg5.GetType() == Str && arg6.GetType() == Str && arg7.GetType() == Str && arg8.GetType() == Str && arg9.GetType() == Str && arg10.GetType() == Str)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I08 && arg2.GetType() == Jbp && arg3.GetType() == Jbp && arg4.GetType() == Jbp && arg5.GetType() == Jbp && arg6.GetType() == Jbp && arg7.GetType() == Jbp && arg8.GetType() == Jbp && arg9.GetType() == Jbp && arg10.GetType() == Jbp)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I08 && arg3.GetType() == I08 && arg4.GetType() == I08 && arg5.GetType() == I08 && arg6.GetType() == I08 && arg7.GetType() == I08 && arg8.GetType() == I08 && arg9.GetType() == I08 && arg10.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I16 && arg3.GetType() == I16 && arg4.GetType() == I16 && arg5.GetType() == I16 && arg6.GetType() == I16 && arg7.GetType() == I16 && arg8.GetType() == I16 && arg9.GetType() == I16 && arg10.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I32 && arg3.GetType() == I32 && arg4.GetType() == I32 && arg5.GetType() == I32 && arg6.GetType() == I32 && arg7.GetType() == I32 && arg8.GetType() == I32 && arg9.GetType() == I32 && arg10.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == I64 && arg3.GetType() == I64 && arg4.GetType() == I64 && arg5.GetType() == I64 && arg6.GetType() == I64 && arg7.GetType() == I64 && arg8.GetType() == I64 && arg9.GetType() == I64 && arg10.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == F32 && arg3.GetType() == F32 && arg4.GetType() == F32 && arg5.GetType() == F32 && arg6.GetType() == F32 && arg7.GetType() == F32 && arg8.GetType() == F32 && arg9.GetType() == F32 && arg10.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == F64 && arg3.GetType() == F64 && arg4.GetType() == F64 && arg5.GetType() == F64 && arg6.GetType() == F64 && arg7.GetType() == F64 && arg8.GetType() == F64 && arg9.GetType() == F64 && arg10.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == C32 && arg3.GetType() == C32 && arg4.GetType() == C32 && arg5.GetType() == C32 && arg6.GetType() == C32 && arg7.GetType() == C32 && arg8.GetType() == C32 && arg9.GetType() == C32 && arg10.GetType() == C32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == C64 && arg3.GetType() == C64 && arg4.GetType() == C64 && arg5.GetType() == C64 && arg6.GetType() == C64 && arg7.GetType() == C64 && arg8.GetType() == C64 && arg9.GetType() == C64 && arg10.GetType() == C64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == Str && arg3.GetType() == Str && arg4.GetType() == Str && arg5.GetType() == Str && arg6.GetType() == Str && arg7.GetType() == Str && arg8.GetType() == Str && arg9.GetType() == Str && arg10.GetType() == Str)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I16 && arg2.GetType() == Jbp && arg3.GetType() == Jbp && arg4.GetType() == Jbp && arg5.GetType() == Jbp && arg6.GetType() == Jbp && arg7.GetType() == Jbp && arg8.GetType() == Jbp && arg9.GetType() == Jbp && arg10.GetType() == Jbp)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I08 && arg3.GetType() == I08 && arg4.GetType() == I08 && arg5.GetType() == I08 && arg6.GetType() == I08 && arg7.GetType() == I08 && arg8.GetType() == I08 && arg9.GetType() == I08 && arg10.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I16 && arg3.GetType() == I16 && arg4.GetType() == I16 && arg5.GetType() == I16 && arg6.GetType() == I16 && arg7.GetType() == I16 && arg8.GetType() == I16 && arg9.GetType() == I16 && arg10.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I32 && arg3.GetType() == I32 && arg4.GetType() == I32 && arg5.GetType() == I32 && arg6.GetType() == I32 && arg7.GetType() == I32 && arg8.GetType() == I32 && arg9.GetType() == I32 && arg10.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == I64 && arg3.GetType() == I64 && arg4.GetType() == I64 && arg5.GetType() == I64 && arg6.GetType() == I64 && arg7.GetType() == I64 && arg8.GetType() == I64 && arg9.GetType() == I64 && arg10.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == F32 && arg3.GetType() == F32 && arg4.GetType() == F32 && arg5.GetType() == F32 && arg6.GetType() == F32 && arg7.GetType() == F32 && arg8.GetType() == F32 && arg9.GetType() == F32 && arg10.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == F64 && arg3.GetType() == F64 && arg4.GetType() == F64 && arg5.GetType() == F64 && arg6.GetType() == F64 && arg7.GetType() == F64 && arg8.GetType() == F64 && arg9.GetType() == F64 && arg10.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == C32 && arg3.GetType() == C32 && arg4.GetType() == C32 && arg5.GetType() == C32 && arg6.GetType() == C32 && arg7.GetType() == C32 && arg8.GetType() == C32 && arg9.GetType() == C32 && arg10.GetType() == C32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == C64 && arg3.GetType() == C64 && arg4.GetType() == C64 && arg5.GetType() == C64 && arg6.GetType() == C64 && arg7.GetType() == C64 && arg8.GetType() == C64 && arg9.GetType() == C64 && arg10.GetType() == C64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == Str && arg3.GetType() == Str && arg4.GetType() == Str && arg5.GetType() == Str && arg6.GetType() == Str && arg7.GetType() == Str && arg8.GetType() == Str && arg9.GetType() == Str && arg10.GetType() == Str)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I32 && arg2.GetType() == Jbp && arg3.GetType() == Jbp && arg4.GetType() == Jbp && arg5.GetType() == Jbp && arg6.GetType() == Jbp && arg7.GetType() == Jbp && arg8.GetType() == Jbp && arg9.GetType() == Jbp && arg10.GetType() == Jbp)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I08 && arg3.GetType() == I08 && arg4.GetType() == I08 && arg5.GetType() == I08 && arg6.GetType() == I08 && arg7.GetType() == I08 && arg8.GetType() == I08 && arg9.GetType() == I08 && arg10.GetType() == I08)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>, DFieldInfo::TagTypeToValueType<I08>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I16 && arg3.GetType() == I16 && arg4.GetType() == I16 && arg5.GetType() == I16 && arg6.GetType() == I16 && arg7.GetType() == I16 && arg8.GetType() == I16 && arg9.GetType() == I16 && arg10.GetType() == I16)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>, DFieldInfo::TagTypeToValueType<I16>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I32 && arg3.GetType() == I32 && arg4.GetType() == I32 && arg5.GetType() == I32 && arg6.GetType() == I32 && arg7.GetType() == I32 && arg8.GetType() == I32 && arg9.GetType() == I32 && arg10.GetType() == I32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>, DFieldInfo::TagTypeToValueType<I32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == I64 && arg3.GetType() == I64 && arg4.GetType() == I64 && arg5.GetType() == I64 && arg6.GetType() == I64 && arg7.GetType() == I64 && arg8.GetType() == I64 && arg9.GetType() == I64 && arg10.GetType() == I64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<I64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == F32 && arg3.GetType() == F32 && arg4.GetType() == F32 && arg5.GetType() == F32 && arg6.GetType() == F32 && arg7.GetType() == F32 && arg8.GetType() == F32 && arg9.GetType() == F32 && arg10.GetType() == F32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>, DFieldInfo::TagTypeToValueType<F32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == F64 && arg3.GetType() == F64 && arg4.GetType() == F64 && arg5.GetType() == F64 && arg6.GetType() == F64 && arg7.GetType() == F64 && arg8.GetType() == F64 && arg9.GetType() == F64 && arg10.GetType() == F64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>, DFieldInfo::TagTypeToValueType<F64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == C32 && arg3.GetType() == C32 && arg4.GetType() == C32 && arg5.GetType() == C32 && arg6.GetType() == C32 && arg7.GetType() == C32 && arg8.GetType() == C32 && arg9.GetType() == C32 && arg10.GetType() == C32)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>, DFieldInfo::TagTypeToValueType<C32>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == C64 && arg3.GetType() == C64 && arg4.GetType() == C64 && arg5.GetType() == C64 && arg6.GetType() == C64 && arg7.GetType() == C64 && arg8.GetType() == C64 && arg9.GetType() == C64 && arg10.GetType() == C64)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>, DFieldInfo::TagTypeToValueType<C64>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == Str && arg3.GetType() == Str && arg4.GetType() == Str && arg5.GetType() == Str && arg6.GetType() == Str && arg7.GetType() == Str && arg8.GetType() == Str && arg9.GetType() == Str && arg10.GetType() == Str)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>, DFieldInfo::TagTypeToValueType<Str>>
				(1, f, std::forward<Nodes>(nodes)...);
		if (arg1.GetType() == I64 && arg2.GetType() == Jbp && arg3.GetType() == Jbp && arg4.GetType() == Jbp && arg5.GetType() == Jbp && arg6.GetType() == Jbp && arg7.GetType() == Jbp && arg8.GetType() == Jbp && arg9.GetType() == Jbp && arg10.GetType() == Jbp)
			return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<I64>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>, DFieldInfo::TagTypeToValueType<Jbp>>
				(1, f, std::forward<Nodes>(nodes)...);
	}
	throw MismatchType("");
}

}

}

}

#endif
