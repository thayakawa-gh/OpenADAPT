#ifndef ADAPT_COMMON_DEFINITION_H
#define ADAPT_COMMON_DEFINITION_H

#include <cstdint>
#include <concepts>
#include <OpenADAPT/Utility/Function.h>

namespace adapt
{

ADAPT_EXPORT
#ifdef ADAPT_USE_32BIT_BINDEX
using BindexType = uint32_t;
#else
using BindexType = int64_t;
#endif
ADAPT_EXPORT
using LayerType = int16_t;
ADAPT_EXPORT
using RankType = int16_t;
ADAPT_EXPORT
using DepthType = int32_t;
ADAPT_EXPORT
using BinBaseType = int32_t;
ADAPT_EXPORT
template <size_t Dim>
using Bin = std::array<BinBaseType, Dim>;
ADAPT_EXPORT
using Bin1D = Bin<1>;
ADAPT_EXPORT
using Bin2D = Bin<2>;
ADAPT_EXPORT
using Bin3D = Bin<3>;
ADAPT_EXPORT
using Bin4D = Bin<4>;

ADAPT_EXPORT
inline constexpr BindexType BindexMax = std::numeric_limits<BindexType>::max();

ADAPT_EXPORT
template <LayerType Layer>
struct LayerConstant : IntegralConstant<LayerType, Layer, LayerConstant> {};
ADAPT_EXPORT
template <RankType Rank>
struct RankConstant : IntegralConstant<RankType, Rank, RankConstant> {};
ADAPT_EXPORT
template <DepthType Depth>
struct DepthConstant : IntegralConstant<DepthType, Depth, DepthConstant> {};

ADAPT_EXPORT
template <size_t Size>
struct SizeConstant : IntegralConstant<size_t, Size, SizeConstant> {};


ADAPT_EXPORT
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
ADAPT_EXPORT
template <class Constant, auto Up>
using Raise = detail::RaiseAndLower_impl<Constant, Up>::Raised;
ADAPT_EXPORT
template <class Constant, auto Down>
using Lower = detail::RaiseAndLower_impl<Constant, Down>::Lowered;

//Traverserの挙動に関するオプション。
// 移動方向
ADAPT_EXPORT
class ForwardMovement {};
ADAPT_EXPORT
class BackwardMovement {};
// Jointの即時/遅延
ADAPT_EXPORT
class DelayedJoint {};
ADAPT_EXPORT
class PromptJoint {};

namespace opts
{
ADAPT_EXPORT
inline constexpr ForwardMovement forward = {};
ADAPT_EXPORT
inline constexpr BackwardMovement backward = {};

ADAPT_EXPORT
inline constexpr DelayedJoint delayed = {};
ADAPT_EXPORT
inline constexpr PromptJoint prompt = {};
}

//Extractにおいて、抽出元の全フィールドを出力するフラグ。
//現状、container_simplexに対してのみ使用可能。
ADAPT_EXPORT
struct AllFields {};
namespace opts
{
ADAPT_EXPORT
inline constexpr AllFields all_fields = {};
}


//Evaluateにおいて、戻り値にtraverserを加えるためのフラグ。
ADAPT_EXPORT
struct WithTraverser {};
namespace opts
{
ADAPT_EXPORT
inline constexpr WithTraverser with_traverser = {};
}


//ToVectorにおいて、戻り値をstd::tuple<std::vector...>からstd::vector<std::tuple<...>>に変更するためのフラグ。
ADAPT_EXPORT
struct Combine {};

namespace opts
{
ADAPT_EXPORT
inline constexpr Combine combine = {};
}


inline namespace lit
{

ADAPT_EXPORT
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

ADAPT_EXPORT
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

ADAPT_EXPORT
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
