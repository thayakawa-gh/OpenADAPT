#ifndef ADAPT_EVALUATOR_FUNCTION_H
#define ADAPT_EVALUATOR_FUNCTION_H

#include <type_traits>
#include <OpenADAPT/Utility/TypeTraits.h>
#include <OpenADAPT/Utility/Macro.h>
#include <OpenADAPT/Common/Concepts.h>
#include <OpenADAPT/Evaluator/ConstNode.h>
#include <OpenADAPT/Joint/LayerInfo.h>

namespace adapt
{

//前方宣言
namespace eval
{

template <class Combination>
struct RttiFuncNode;

namespace detail
{

//template <node Node1>
//const Node1& ConvertToNode(const Node1& node) { return node; }
template <any_node Node1>
Node1&& ConvertToNode(Node1&& node) { return std::forward<Node1>(node); }
template <ctti_placeholder PH>
auto ConvertToNode(PH ph) { return eval::CttiFieldNode{ ph }; }
template <rtti_placeholder PH>
auto ConvertToNode(PH ph) { return eval::RttiFieldNode{ ph }; }

template <bool Rtti, any_node Node1>
Node1&& ConvertToNode(Node1&& node, std::bool_constant<Rtti>) { return std::forward<Node1>(node); }
template <bool Rtti, ctti_placeholder PH>
auto ConvertToNode(PH ph, std::bool_constant<Rtti>) { return eval::CttiFieldNode{ ph }; }
template <bool Rtti, rtti_placeholder PH>
auto ConvertToNode(PH ph, std::bool_constant<Rtti>) { return eval::RttiFieldNode{ ph }; }

//constant nodeの場合はCttiとRttiが区別できないので、引数でどちらかを指定する。
template <neither_node_nor_placeholder Constant>
auto ConvertToNode(Constant&& c, std::bool_constant<false>) { return eval::CttiConstNode<std::decay_t<Constant>>(std::forward<Constant>(c)); }
template <neither_node_nor_placeholder Constant>
auto ConvertToNode(Constant&& c, std::bool_constant<true>) { return eval::RttiConstNode(std::forward<Constant>(c)); }

template <template <class...> class Func, class ...NPs>
auto MakeFunctionNode(NPs&& ...nps);

#define GET_RET_TYPE_OP1(OP) decltype(OP std::declval<ArgType_>())
#define GET_RET_TYPE_OP2(OP) decltype(std::declval<ArgType1_>() OP std::declval<ArgType2_>())
#define GET_RET_TYPE_FN1(FN) decltype(FN(std::declval<ArgType_>()))
#define GET_RET_TYPE_FN2(FN) decltype(FN(std::declval<ArgType1_>(), std::declval<ArgType2_>()))
#define GET_RET_TYPE_FN3(FN) decltype(FN(std::declval<ArgType1_>(), std::declval<ArgType2_>(), std::declval<ArgType3_>()))

#define GET_RET_TYPE_VA_FN(FN) decltype(FN(std::declval<ArgTypes_>()...))

//1 arg function
#define DEFINE_FUNC1_IMPL(NAME, DEFAULT_RET, PROCESS)\
template <class ArgType_>\
concept NAME##Applicable = requires(const ArgType_& a)\
{\
	PROCESS;\
};\
template <class ArgType_>\
requires NAME##Applicable<ArgType_>\
struct NAME\
{\
	template <size_t Index, std::nullptr_t = nullptr> struct ArgType_impl;\
	template <std::nullptr_t X> struct ArgType_impl<0, X> { using Type = ArgType_; };\
	template <size_t Index> using ArgType = ArgType_impl<Index>::Type;\
	using RetType = DEFAULT_RET; \
	static RetType Exec(const ArgType_& a) { return PROCESS; }\
};
#define DEFINE_FUNC1_IMPL_WB(NAME, DEFAULT_RET, PROCESS, PROCESS_BUF)\
template <class ArgType_>\
concept NAME##Applicable = requires(const ArgType_& a)\
{\
	PROCESS;\
};\
template <class ArgType_>\
requires NAME##Applicable<ArgType_>\
struct NAME\
{\
	template <size_t Index, std::nullptr_t = nullptr> struct ArgType_impl;\
	template <std::nullptr_t X> struct ArgType_impl<0, X> { using Type = ArgType_; };\
	template <size_t Index> using ArgType = ArgType_impl<Index>::Type;\
	using RetType = DEFAULT_RET; \
	static RetType Exec(const ArgType_& a) { return PROCESS; }\
	static void Exec(const ArgType_& a, RetType& buf) { PROCESS_BUF; }\
};
/*#define SPECIALIZE_FUNC1_IMPL_WB(NAME, ARG_TYPE, RET_TYPE, PROCESS, PROCESS_BUF)\
template <>\
struct NAME<ARG_TYPE>\
{\
	template <size_t Index, std::nullptr_t = nullptr> struct ArgType_impl;\
	template <std::nullptr_t X> struct ArgType_impl<0, X> { using Type = ARG_TYPE; };\
	template <size_t Index> using ArgType = ArgType_impl<Index>::Type;\
	using RetType = RET_TYPE;\
	static RetType Exec(const ARG_TYPE& a) { return PROCESS; }\
	static void Exec(const ARG_TYPE& a, RetType& buf) { PROCESS_BUF; }\
};*/


//2 arg function
#define DEFINE_FUNC2_IMPL(NAME, DEFAULT_RET, PROCESS)\
template <class ArgType1_, class ArgType2_>\
concept NAME##Applicable = requires(const ArgType1_& a1, const ArgType2_& a2)\
{\
	PROCESS;\
};\
template <class ArgType1_, class ArgType2_>\
requires NAME##Applicable<ArgType1_, ArgType2_>\
struct NAME\
{\
	template <size_t Index, std::nullptr_t = nullptr> struct ArgType_impl;\
	template <std::nullptr_t X> struct ArgType_impl<0, X> { using Type = ArgType1_; };\
	template <std::nullptr_t X> struct ArgType_impl<1, X> { using Type = ArgType2_; };\
	template <size_t Index> using ArgType = ArgType_impl<Index>::Type;\
	using RetType = DEFAULT_RET;\
	static RetType Exec(const ArgType1_& a1, const ArgType2_& a2) { return PROCESS; }\
};
#define DEFINE_FUNC2_IMPL_WB(NAME, DEFAULT_RET, PROCESS, PROCESS_BUF)\
template <class ArgType1_, class ArgType2_>\
concept NAME##Applicable = requires(const ArgType1_& a1, const ArgType2_& a2)\
{\
	PROCESS;\
};\
template <class ArgType1_, class ArgType2_>\
requires NAME##Applicable<ArgType1_, ArgType2_>\
struct NAME\
{\
	template <size_t Index, std::nullptr_t = nullptr> struct ArgType_impl;\
	template <std::nullptr_t X> struct ArgType_impl<0, X> { using Type = ArgType1_; };\
	template <std::nullptr_t X> struct ArgType_impl<1, X> { using Type = ArgType2_; };\
	template <size_t Index> using ArgType = ArgType_impl<Index>::Type;\
	using RetType = DEFAULT_RET;\
	static RetType Exec(const ArgType1_& a1, const ArgType2_& a2) { return PROCESS; }\
	static void Exec(const ArgType1_& a1, const ArgType2_& a2, RetType& buf) { PROCESS_BUF; }\
};
/*#define SPECIALIZE_FUNC2_IMPL_WB(NAME, ARG_TYPE1, ARG_TYPE2, RET_TYPE, PROCESS, PROCESS_BUF)\
template <>\
struct NAME<ARG_TYPE1, ARG_TYPE2>\
{\
	template <size_t Index, std::nullptr_t = nullptr> struct ArgType_impl;\
	template <std::nullptr_t X> struct ArgType_impl<0, X> { using Type = ARG_TYPE1; };\
	template <std::nullptr_t X> struct ArgType_impl<1, X> { using Type = ARG_TYPE2; };\
	template <size_t Index> using ArgType = ArgType_impl<Index>::Type;\
	using RetType = RET_TYPE;\
	static RetType Exec(const ARG_TYPE1& a1, const ARG_TYPE2& a2) { return PROCESS; }\
	static void Exec(const ARG_TYPE1& a1, const ARG_TYPE2& a2, RetType& buf) { PROCESS_BUF; }\
};*/

//3 arg function
#define DEFINE_FUNC3_IMPL(NAME, DEFAULT_RET, PROCESS)\
template <class ArgType1_, class ArgType2_, class ArgType3_>\
concept NAME##Applicable = requires(const ArgType1_& a1, const ArgType2_& a2, const ArgType3_& a3)\
{\
	PROCESS;\
};\
template <class ArgType1_, class ArgType2_, class ArgType3_>\
requires NAME##Applicable<ArgType1_, ArgType2_, ArgType3_>\
struct NAME\
{\
	template <size_t Index, std::nullptr_t = nullptr> struct ArgType_impl;\
	template <std::nullptr_t X> struct ArgType_impl<0, X> { using Type = ArgType1_; };\
	template <std::nullptr_t X> struct ArgType_impl<1, X> { using Type = ArgType2_; };\
	template <std::nullptr_t X> struct ArgType_impl<2, X> { using Type = ArgType3_; };\
	template <size_t Index> using ArgType = ArgType_impl<Index>::Type;\
	using RetType = DEFAULT_RET;\
	static RetType Exec(const ArgType1_& a1, const ArgType2_& a2, const ArgType3_& a3) { return PROCESS; }\
};
#define DEFINE_FUNC3_IMPL_WB(NAME, DEFAULT_RET, PROCESS, PROCESS_BUF)\
template <class ArgType1_, class ArgType2_, class ArgType3_>\
concept NAME##Applicable = requires(const ArgType1_& a1, const ArgType2_& a2, const ArgType3_& a3)\
{\
	PROCESS;\
};\
template <class ArgType1_, class ArgType2_, class ArgType3_>\
requires NAME##Applicable<ArgType1_, ArgType2_, ArgType3_>\
struct NAME\
{\
	template <size_t Index, std::nullptr_t = nullptr> struct ArgType_impl;\
	template <std::nullptr_t X> struct ArgType_impl<0, X> { using Type = ArgType1_; };\
	template <std::nullptr_t X> struct ArgType_impl<1, X> { using Type = ArgType2_; };\
	template <std::nullptr_t X> struct ArgType_impl<2, X> { using Type = ArgType3_; };\
	template <size_t Index> using ArgType = ArgType_impl<Index>::Type;\
	using RetType = DEFAULT_RET;\
	static RetType Exec(const ArgType1_& a1, const ArgType2_& a2, const ArgType3_& a3) { return PROCESS; }\
	static void Exec(const ArgType1_& a1, const ArgType2_& a2, const ArgType3_& a3, RetType& buf) { PROCESS_BUF; }\
};
/*#define SPECIALIZE_FUNC3_IMPL_WB(NAME, ARG_TYPE1, ARG_TYPE2, ARG_TYPE3, RET_TYPE, PROCESS, PROCESS_BUF)\
template <>\
struct NAME<ARG_TYPE1, ARG_TYPE2, ARG_TYPE3>\
{\
	template <size_t Index, std::nullptr_t = nullptr> struct ArgType_impl;\
	template <std::nullptr_t X> struct ArgType_impl<0, X> { using Type = ARG_TYPE1; };\
	template <std::nullptr_t X> struct ArgType_impl<1, X> { using Type = ARG_TYPE2; };\
	template <std::nullptr_t X> struct ArgType_impl<1, X> { using Type = ARG_TYPE3; };\
	template <size_t Index> using ArgType = ArgType_impl<Index>::Type;\
	using RetType = RET_TYPE;\
	static RetType Exec(const ARG_TYPE1& a1, const ARG_TYPE2& a2, const ARG_TYPE3& a3) { return PROCESS; }\
	static void Exec(const ARG_TYPE1& a1, const ARG_TYPE2& a2, const ARG_TYPE3& a3, RetType& buf) { PROCESS_BUF; }\
};*/


//3 arg function
#define DEFINE_VARIADIC_FUNC_IMPL(NAME, DEFAULT_RET, PROCESS)\
template <class ...ArgTypes_>\
concept NAME##Applicable = requires(const ArgTypes_& ...a)\
{\
	PROCESS;\
};\
template <class ...ArgTypes_>\
requires NAME##Applicable<ArgTypes_...>\
struct NAME\
{\
	template <size_t Index> using ArgType = GetType_t<Index, ArgTypes_...>;\
	using RetType = DEFAULT_RET;\
	static RetType Exec(const ArgTypes_& ...a) { return PROCESS; }\
};
#define DEFINE_VARIADIC_FUNC_IMPL_WB(NAME, DEFAULT_RET, PROCESS, PROCESS_BUF)\
template <class ...ArgTypes_>\
concept NAME##Applicable = requires(const ArgTypes_& ...a)\
{\
	PROCESS;\
};\
template <class ...ArgTypes_>\
requires NAME##Applicable<ArgTypes_...>\
struct NAME\
{\
	template <size_t Index> using ArgType = GetType_t<Index, ArgTypes_...>;\
	using RetType = DEFAULT_RET;\
	static RetType Exec(const ArgTypes_& ...a) { return PROCESS; }\
	static void Exec(const ArgTypes_& ...a, RetType& buf) { PROCESS_BUF; }\
};

#define DEFINE_FN1(NAME, NAME_ID)\
template <node_or_placeholder NP>\
auto NAME_ID(NP&& np)\
{\
	return detail::MakeFunctionNode<NAME>(std::forward<NP>(np));\
}

#define DEFINE_FN2(NAME, NAME_ID)\
template <neither_node_nor_placeholder Constant, node_or_placeholder NP>\
auto NAME_ID(Constant&& c, NP&& np)\
{\
	return detail::MakeFunctionNode<NAME>\
		(std::forward<Constant>(c), std::forward<NP>(np));\
}\
template <node_or_placeholder NP, neither_node_nor_placeholder Constant>\
auto NAME_ID(NP&& np, Constant&& c)\
{\
	return detail::MakeFunctionNode<NAME>\
		(std::forward<NP>(np), std::forward<Constant>(c));\
}\
template <node_or_placeholder NP1, node_or_placeholder NP2>\
auto NAME_ID(NP1&& np1, NP2&& np2)\
{\
	return detail::MakeFunctionNode<NAME>\
		(std::forward<NP1>(np1), std::forward<NP2>(np2));\
}

#define DEFINE_OP1(NAME, SIGN) DEFINE_FN1(NAME, operator SIGN)
#define DEFINE_OP2(NAME, SIGN) DEFINE_FN2(NAME, operator SIGN)

#define DEFINE_FN3(NAME, NAME_ID)\
template <class NP1, class NP2, class NP3>\
	requires node_or_placeholder<NP1> || node_or_placeholder<NP2> || node_or_placeholder<NP3>\
auto NAME_ID(NP1&& np1, NP2&& np2, NP3&& np3)\
{\
	return detail::MakeFunctionNode<NAME>\
		(std::forward<NP1>(np1), std::forward<NP2>(np2), std::forward<NP3>(np3));\
}

#define DEFINE_VAR_FN(NAME, NAME_ID)\
template <class ...NPs>\
	requires (node_or_placeholder<NPs> || ...)\
auto NAME_ID(NPs&& ...nps)\
{\
	return detail::MakeFunctionNode<NAME>(std::forward<NPs>(nps)...);\
}

//unary operator
DEFINE_FUNC1_IMPL(LogicalNot, GET_RET_TYPE_OP1(!),
				  ADAPT_TIE_ARGS(!a));
DEFINE_FUNC1_IMPL(Negate, GET_RET_TYPE_OP1(-),
				  ADAPT_TIE_ARGS(-a));
 
//binary operator
DEFINE_FUNC2_IMPL_WB(Plus, GET_RET_TYPE_OP2(+),
					 ADAPT_TIE_ARGS(a1 + a2),
					 ADAPT_TIE_ARGS(buf = a1; buf += a2));
DEFINE_FUNC2_IMPL_WB(Minus, GET_RET_TYPE_OP2(-),
					 ADAPT_TIE_ARGS(a1 - a2),
					 ADAPT_TIE_ARGS(buf = a1; buf -= a2));
DEFINE_FUNC2_IMPL_WB(Multiply, GET_RET_TYPE_OP2(*),
					 ADAPT_TIE_ARGS(a1 * a2),
					 ADAPT_TIE_ARGS(buf = a1; buf *= a2));
DEFINE_FUNC2_IMPL_WB(Divide, GET_RET_TYPE_OP2(/ ),
					 ADAPT_TIE_ARGS(a1 / a2),
					 ADAPT_TIE_ARGS(buf = a1; buf /= a2));
DEFINE_FUNC2_IMPL_WB(Modulus, GET_RET_TYPE_OP2(%),
					 ADAPT_TIE_ARGS(a1 % a2),
					 ADAPT_TIE_ARGS(buf = a1; buf %= a2));

DEFINE_FUNC2_IMPL(Power, GET_RET_TYPE_FN2(pow),
				  ADAPT_TIE_ARGS(pow(a1, a2)));
DEFINE_FUNC2_IMPL(Equal, GET_RET_TYPE_OP2(== ),
				  ADAPT_TIE_ARGS(a1 == a2));
DEFINE_FUNC2_IMPL(NotEqual, GET_RET_TYPE_OP2(!= ),
				  ADAPT_TIE_ARGS(a1 != a2));
DEFINE_FUNC2_IMPL(Less, GET_RET_TYPE_OP2(< ),
				  ADAPT_TIE_ARGS(a1 < a2));
DEFINE_FUNC2_IMPL(LessEqual, GET_RET_TYPE_OP2(<= ),
				  ADAPT_TIE_ARGS(a1 <= a2));
DEFINE_FUNC2_IMPL(Greater, GET_RET_TYPE_OP2(> ),
				  ADAPT_TIE_ARGS(a1 > a2));
DEFINE_FUNC2_IMPL(GreaterEqual, GET_RET_TYPE_OP2(>= ),
				  ADAPT_TIE_ARGS(a1 >= a2));
DEFINE_FUNC2_IMPL(LocgicalAnd, GET_RET_TYPE_OP2(&&),
				  ADAPT_TIE_ARGS(a1 && a2));
DEFINE_FUNC2_IMPL(LogicalOr, GET_RET_TYPE_OP2(|| ),
				  ADAPT_TIE_ARGS(a1 || a2));

//index operator
DEFINE_FUNC2_IMPL(IndexOp, ADAPT_TIE_ARGS(decltype(std::declval<ArgType1_>()[std::declval<ArgType2_>()])),
				  ADAPT_TIE_ARGS(a1[a2]));

//1 arg function
DEFINE_FUNC1_IMPL(Sin, GET_RET_TYPE_FN1(sin),
				  ADAPT_TIE_ARGS(sin(a)));
DEFINE_FUNC1_IMPL(Cos, GET_RET_TYPE_FN1(cos),
				  ADAPT_TIE_ARGS(cos(a)));
DEFINE_FUNC1_IMPL(Tan, GET_RET_TYPE_FN1(tan),
				  ADAPT_TIE_ARGS(tan(a)));

DEFINE_FUNC1_IMPL(ASin, GET_RET_TYPE_FN1(asin),
				  ADAPT_TIE_ARGS(asin(a)));
DEFINE_FUNC1_IMPL(ACos, GET_RET_TYPE_FN1(acos),
				  ADAPT_TIE_ARGS(acos(a)));
DEFINE_FUNC1_IMPL(ATan, GET_RET_TYPE_FN1(atan),
				  ADAPT_TIE_ARGS(atan(a)));
DEFINE_FUNC1_IMPL(Exponential, GET_RET_TYPE_FN1(exp),
				  ADAPT_TIE_ARGS(exp(a)));
DEFINE_FUNC1_IMPL_WB(Square,
					 ADAPT_TIE_ARGS(decltype(std::declval<ArgType_>()* std::declval<ArgType_>())),
					 ADAPT_TIE_ARGS(a * a),
					 ADAPT_TIE_ARGS(buf = a; buf *= a));
DEFINE_FUNC1_IMPL(Sqrt, GET_RET_TYPE_FN1(sqrt),
				  ADAPT_TIE_ARGS(sqrt(a)));
DEFINE_FUNC1_IMPL_WB(Cube, GET_RET_TYPE_FN1(sqrt),
					 ADAPT_TIE_ARGS(a * a * a),
					 ADAPT_TIE_ARGS(buf = a; buf *= a; buf *= a));
DEFINE_FUNC1_IMPL(Cbrt, GET_RET_TYPE_FN1(cbrt),
				  ADAPT_TIE_ARGS(cbrt(a)));
DEFINE_FUNC1_IMPL(Ceil, GET_RET_TYPE_FN1(ceil),
				  ADAPT_TIE_ARGS(ceil(a)));
DEFINE_FUNC1_IMPL(Floor, GET_RET_TYPE_FN1(floor),
				  ADAPT_TIE_ARGS(floor(a)));

DEFINE_FUNC1_IMPL(Log, GET_RET_TYPE_FN1(log),
				  ADAPT_TIE_ARGS(log(a)));
DEFINE_FUNC1_IMPL(Log10, GET_RET_TYPE_FN1(log10),
				  ADAPT_TIE_ARGS(log10(a)));
DEFINE_FUNC1_IMPL(Abs, GET_RET_TYPE_FN1(abs),
				  ADAPT_TIE_ARGS(abs(a)));
DEFINE_FUNC1_IMPL(Len, ADAPT_TIE_ARGS(decltype(std::declval<ArgType_>().size())),
				  ADAPT_TIE_ARGS(a.size()));

//
DEFINE_FUNC1_IMPL_WB(NumToStr, std::string,
					 ADAPT_TIE_ARGS(std::to_string(a)),
					 ADAPT_TIE_ARGS(ToStr(a, buf)));

//2 arg function
DEFINE_FUNC2_IMPL(ATan2, GET_RET_TYPE_FN2(atan2),
				  ADAPT_TIE_ARGS(atan2(a1, a2)));
DEFINE_FUNC2_IMPL(Log2, ADAPT_TIE_ARGS(decltype(log(std::declval<ArgType1_>()) / log(std::declval<ArgType2_>()))),
				  ADAPT_TIE_ARGS(log(a1) / log(a2)));
DEFINE_FUNC2_IMPL(Min, GET_RET_TYPE_FN2(std::min),
				  ADAPT_TIE_ARGS(std::min(a1, a2)));
DEFINE_FUNC2_IMPL(Max, GET_RET_TYPE_FN2(std::max),
				  ADAPT_TIE_ARGS(std::max(a1, a2)));

template <class T1, class T2>
decltype(auto) IfElse(bool a, T1&& b, T2&& c) { return a ? b : c; }

template <size_t I, class Res, std::integral Int, class T, class ...U>
Res Switch(Int i, T&& t, U&& ...u)
{
	if (i == I) return std::forward<T>(t);
	if constexpr (sizeof...(U) > 0) return Switch<I + 1, Res>(i, std::forward<U>(u)...);
	else throw NoElements();
}
template <std::integral Int, class ...T>
	requires (std::is_trivial_v<std::decay_t<T>> &&...)
auto Switch(Int i, T&& ...t)
{
	using Res = std::common_type_t<T...>;
	return Switch<0, Res>(i, std::forward<T>(t)...);
}

template <size_t I, std::integral Int, class Res, class T, class ...U>
void SwitchBuf(Res& res, Int i, T&& t, U&& ...u)
{
	if (i == I) res = std::forward<T>(t);
	else
	{
		if constexpr (sizeof...(U) > 0) return SwitchBuf<I + 1>(res, i, std::forward<U>(u)...);
		else throw NoElements();
	}
}
template <std::integral Int, class Res, class ...T>
void SwitchBuf(Res& res, Int i, T&& ...t)
{
	SwitchBuf<0>(res, i, std::forward<T>(t)...);
}

//3 arg function
DEFINE_FUNC3_IMPL_WB(If_, GET_RET_TYPE_FN3(IfElse),
					 ADAPT_TIE_ARGS(IfElse(a1, a2, a3)),
					 ADAPT_TIE_ARGS(a1 ? buf = a2 : buf = a3));

//switch
DEFINE_VARIADIC_FUNC_IMPL_WB(Switch_, GET_RET_TYPE_VA_FN(Switch),
							 ADAPT_TIE_ARGS(Switch(a...)),
							 ADAPT_TIE_ARGS(SwitchBuf(buf, a...)));

//pass through function 主にFieldNode -> FuncNodeという変換を行うもの。
//DEFINE_FUNC1_IMPL_WB(Forward, ArgType_,
//	ADAPT_TIE_ARGS(a), ADAPT_TIE_ARGS(buf = a));
template <class ArgType_>
struct Forward
{
	template <size_t Index, std::nullptr_t = nullptr>
	struct ArgType_impl;
	template <std::nullptr_t X>
	struct ArgType_impl<0, X>
	{
		using Type = ArgType_;
	};
	template <size_t Index>
	using ArgType = ArgType_impl<Index>::Type;
	using RetType = ArgType_;
	static RetType Exec(const ArgType_& a)
	{
		return a;
	}
	static void Exec(const ArgType_& a, RetType& buf)
	{
		buf = a;
	}
};

template <class ArgType_, class RetType_>
	requires std::convertible_to<ArgType_, RetType_>
struct Cast
{
	template <size_t Index, std::nullptr_t = nullptr> struct ArgType_impl;
	template <std::nullptr_t X> struct ArgType_impl<0, X> { using Type = ArgType_; };
	template <size_t Index> using ArgType = ArgType_impl<Index>::Type;
	using RetType = RetType_;
	static RetType Exec(const ArgType_& a) { return RetType(a); }
	static void Exec(const ArgType_& a, RetType& buf) { buf = RetType(a); }
};
template <class RetType_>
struct CastWrapper
{
	template <class ArgType_>
	using Func = Cast<ArgType_, RetType_>;
};
//CastWrapperを使うとVisual Studio上でcast関数を使ったときに戻り値の型がerror-typeになって、
//エディタが赤波線だらけになってしまう。
//頻出する以下のものだけでも回避する。
template <class ArgType_>
using CastI08 = Cast<ArgType_, int8_t>;
template <class ArgType_>
using CastI16 = Cast<ArgType_, int16_t>;
template <class ArgType_>
using CastI32 = Cast<ArgType_, int32_t>;
template <class ArgType_>
using CastI64 = Cast<ArgType_, int64_t>;
template <class ArgType_>
using CastF32 = Cast<ArgType_, float>;
template <class ArgType_>
using CastF64 = Cast<ArgType_, double>;
template <class ArgType_>
using CastC32 = Cast<ArgType_, std::complex<float>>;
template <class ArgType_>
using CastC64 = Cast<ArgType_, std::complex<double>>;

}

DEFINE_OP1(detail::LogicalNot, !)
DEFINE_OP1(detail::Negate, -)

DEFINE_OP2(detail::Plus, +)
DEFINE_OP2(detail::Minus, -)
DEFINE_OP2(detail::Multiply, *)
DEFINE_OP2(detail::Divide, / )
DEFINE_OP2(detail::Modulus, %)

DEFINE_FN2(detail::Power, pow);
DEFINE_OP2(detail::Equal, == );
DEFINE_OP2(detail::NotEqual, != );
DEFINE_OP2(detail::Less, < );
DEFINE_OP2(detail::LessEqual, <= );
DEFINE_OP2(detail::Greater, > );
DEFINE_OP2(detail::GreaterEqual, >= );
DEFINE_OP2(detail::LocgicalAnd, &&);
DEFINE_OP2(detail::LogicalOr, || );

// TODO: define index operator

DEFINE_FN1(detail::Sin, sin);
DEFINE_FN1(detail::Cos, cos);
DEFINE_FN1(detail::Tan, tan);
DEFINE_FN1(detail::ASin, asin);
DEFINE_FN1(detail::ACos, acos);
DEFINE_FN1(detail::ATan, atan);
DEFINE_FN1(detail::Exponential, exp);
DEFINE_FN1(detail::Square, square);
DEFINE_FN1(detail::Sqrt, sqrt);
DEFINE_FN1(detail::Cube, cube);
DEFINE_FN1(detail::Cbrt, cbrt);
DEFINE_FN1(detail::Ceil, ceil);
DEFINE_FN1(detail::Floor, floor);
DEFINE_FN1(detail::Log, log);
DEFINE_FN1(detail::Log10, log10);
DEFINE_FN1(detail::Abs, abs);
DEFINE_FN1(detail::Len, len);
DEFINE_FN1(detail::NumToStr, tostr);

DEFINE_FN2(detail::ATan2, atan2);
DEFINE_FN2(detail::Log2, log2);
DEFINE_FN2(detail::Min, min);
DEFINE_FN2(detail::Max, max);

DEFINE_FN3(detail::If_, if_);

DEFINE_VAR_FN(detail::Switch_, switch_);

//通常は使わない。Placeholderや定数をFuncNodeに変換したい時に使う。
DEFINE_FN1(detail::Forward, fwd);

template <class T, node_or_placeholder NP>
auto cast(NP&& np)
{
	//using NP_ = std::remove_cvref_t<NP>;
	using CastWrapper = detail::CastWrapper<T>;
	return detail::MakeFunctionNode<CastWrapper::template Func>(std::forward<NP>(np));
}
template <FieldType Type, node_or_placeholder NP>
auto cast(NP&& np)
{
	using ValueType = DFieldInfo::TagTypeToValueType<Type>;
	return cast<ValueType>(std::forward<NP>(np));
}
/*
template <node_or_placeholder NP> auto cast_i08(NP&& np) { return cast<FieldType::I08>(std::forward<NP>(np)); }
template <node_or_placeholder NP> auto cast_i16(NP&& np) { return cast<FieldType::I16>(std::forward<NP>(np)); }
template <node_or_placeholder NP> auto cast_i32(NP&& np) { return cast<FieldType::I32>(std::forward<NP>(np)); }
template <node_or_placeholder NP> auto cast_i64(NP&& np) { return cast<FieldType::I64>(std::forward<NP>(np)); }
template <node_or_placeholder NP> auto cast_f32(NP&& np) { return cast<FieldType::F32>(std::forward<NP>(np)); }
template <node_or_placeholder NP> auto cast_f64(NP&& np) { return cast<FieldType::F64>(std::forward<NP>(np)); }
template <node_or_placeholder NP> auto cast_c32(NP&& np) { return cast<FieldType::C32>(std::forward<NP>(np)); }
template <node_or_placeholder NP> auto cast_c64(NP&& np) { return cast<FieldType::C64>(std::forward<NP>(np)); }
*/
template <node_or_placeholder NP> auto cast_i08(NP&& np) { return detail::MakeFunctionNode<detail::CastI08>(std::forward<NP>(np)); }
template <node_or_placeholder NP> auto cast_i16(NP&& np) { return detail::MakeFunctionNode<detail::CastI16>(std::forward<NP>(np)); }
template <node_or_placeholder NP> auto cast_i32(NP&& np) { return detail::MakeFunctionNode<detail::CastI32>(std::forward<NP>(np)); }
template <node_or_placeholder NP> auto cast_i64(NP&& np) { return detail::MakeFunctionNode<detail::CastI64>(std::forward<NP>(np)); }
template <node_or_placeholder NP> auto cast_f32(NP&& np) { return detail::MakeFunctionNode<detail::CastF32>(std::forward<NP>(np)); }
template <node_or_placeholder NP> auto cast_f64(NP&& np) { return detail::MakeFunctionNode<detail::CastF64>(std::forward<NP>(np)); }
template <node_or_placeholder NP> auto cast_c32(NP&& np) { return detail::MakeFunctionNode<detail::CastC32>(std::forward<NP>(np)); }
template <node_or_placeholder NP> auto cast_c64(NP&& np) { return detail::MakeFunctionNode<detail::CastC64>(std::forward<NP>(np)); }

}

}

#endif