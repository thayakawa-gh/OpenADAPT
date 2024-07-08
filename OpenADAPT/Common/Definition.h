#ifndef ADAPT_COMMON_DEFINITION_H
#define ADAPT_COMMON_DEFINITION_H

#include <cstdint>
#include <concepts>
#include <OpenADAPT/Utility/Function.h>

namespace adapt
{

using BindexType = uint32_t;
using LayerType = int16_t;
using RankType = int16_t;
using DepthType = int32_t;

inline constexpr BindexType BindexMax = std::numeric_limits<BindexType>::max();

template <LayerType Layer>
struct LayerConstant : IntegralConstant<LayerType, Layer, LayerConstant> {};
template <RankType Rank>
struct RankConstant : IntegralConstant<RankType, Rank, RankConstant> {};
template <DepthType Depth>
struct DepthConstant : IntegralConstant<DepthType, Depth, DepthConstant> {};

template <size_t Size>
struct SizeConstant : IntegralConstant<size_t, Size, SizeConstant> {};


inline constexpr size_t MultiSize = 8;

namespace detail
{
template <class Constant, auto Diff>
struct RaiseAndLower_impl;
template <std::integral Integ, auto Diff>
struct RaiseAndLower_impl<Integ, Diff>
{
	using Raised = Integ;//整数そのものが与えられた場合、型は変更しない。
	using Lowered = Integ;
};
template <std::integral Type, Type X, template <Type> class Constant, Type Diff>
struct RaiseAndLower_impl<Constant<X>, Diff>
{
	static_assert(std::derived_from<Constant<X>, IntegralConstant<Type, X, Constant>>);
	using Raised = Constant<Type(X + Diff)>;
	using Lowered = Constant<Type(X - Diff)>;
};
}
template <class Constant, auto Up>
using Raise = detail::RaiseAndLower_impl<Constant, Up>::Raised;
template <class Constant, auto Down>
using Lower = detail::RaiseAndLower_impl<Constant, Down>::Lowered;

//Traverserの挙動に関するフラグ。
class ForwardFlag {};
class BackwardFlag {};

class DelayedJoint {};
class PromptJoint {};

namespace flags
{
inline constexpr ForwardFlag forward = {};
inline constexpr BackwardFlag backward = {};

inline constexpr DelayedJoint delayed = {};
inline constexpr PromptJoint prompt = {};
}

//Extractにおいて、抽出元の全フィールドを出力するフラグ。
//現状、container_simplexに対してのみ使用可能。
struct AllFields {};
namespace flags
{
inline constexpr AllFields all_fields = {};
}


//Evaluateにおいて、戻り値にtraverserを加えるためのフラグ。
struct WithTraverser {};
namespace flags
{
inline constexpr WithTraverser with_traverser = {};
}


//ToVectorにおいて、戻り値をstd::tuple<std::vector...>からstd::vector<std::tuple<...>>に変更するためのフラグ。
struct Combine {};

namespace flags
{
inline constexpr Combine combine = {};
}


inline namespace lit
{

template <char ...N>
consteval auto operator""_layer()
{
	constexpr char a[] = { N..., '\0' };
	constexpr LayerType i = StrTo<LayerType>(a);
	return LayerConstant<i>();
}
/*template <char ...N>
constexpr auto operator""_l()
{
	constexpr char a[] = { N..., '\0' };
	constexpr LayerType i = StrTo<LayerType>(a);
	return LayerConstant<i>();
}*/

template <char ...N>
consteval auto operator""_depth()
{
	constexpr char a[] = { N..., '\0' };
	constexpr DepthType d = StrTo<DepthType>(a);
	return DepthConstant<d>();
}
/*template <char ...N>
constexpr auto operator""_d()
{
	return operator""_depth<N...>();
}*/

template <char ...N>
consteval auto operator""_rank()
{
	constexpr char a[] = { N..., '\0' };
	constexpr LayerType i = StrTo<LayerType>(a);
	return RankConstant<i>();
}
/*template <char ...N>
constexpr auto operator""_r()
{
	constexpr char a[] = { N..., '\0' };
	constexpr LayerType i = StrTo<LayerType>(a);
	return RankConstant<i>();
}*/

}

}


#endif
