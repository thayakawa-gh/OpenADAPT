#ifndef ADAPT_EVALUATOR_FUNCTION_H
#define ADAPT_EVALUATOR_FUNCTION_H

#include <type_traits>
#include <OpenADAPT/Utility/TypeTraits.h>
#include <OpenADAPT/Utility/Macro.h>
#include <OpenADAPT/Common/Concepts.h>
#include <OpenADAPT/Evaluator/ConstNode.h>
#include <OpenADAPT/Evaluator/FuncNode.h>
#include <OpenADAPT/Joint/LayerInfo.h>

namespace adapt
{

namespace eval
{

namespace detail
{

/*
#define GET_RET_TYPE_OP1(OP) decltype(OP std::declval<ArgType_>())
#define GET_RET_TYPE_OP2(OP) decltype(std::declval<ArgType1_>() OP std::declval<ArgType2_>())
#define GET_RET_TYPE_FN1(FN) decltype(FN(std::declval<ArgType_>()))
#define GET_RET_TYPE_FN2(FN) decltype(FN(std::declval<ArgType1_>(), std::declval<ArgType2_>()))
#define GET_RET_TYPE_FN3(FN) decltype(FN(std::declval<ArgType1_>(), std::declval<ArgType2_>(), std::declval<ArgType3_>()))
*/

/*
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


//variadic function
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
}*/



//unary operator
/*DEFINE_FUNC1_IMPL(LogicalNot, GET_RET_TYPE_OP1(!),
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
				  ADAPT_TIE_ARGS(std::max(a1, a2)));*/

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

/*
//3 arg function
DEFINE_FUNC3_IMPL_WB(If_, GET_RET_TYPE_FN3(IfElse),
					 ADAPT_TIE_ARGS(IfElse(a1, a2, a3)),
					 ADAPT_TIE_ARGS(a1 ? buf = a2 : buf = a3));

//switch
DEFINE_VARIADIC_FUNC_IMPL_WB(Switch_, GET_RET_TYPE_VA_FN(Switch),
							 ADAPT_TIE_ARGS(Switch(a...)),
							 ADAPT_TIE_ARGS(SwitchBuf(buf, a...)));
*/

//pass through function 主にFieldNode -> FuncNodeという変換を行うもの。
//DEFINE_FUNC1_IMPL_WB(Forward, ArgType_,
//	ADAPT_TIE_ARGS(a), ADAPT_TIE_ARGS(buf = a));
/*template <class ArgType_>
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
};*/

template <class Ret>
struct Cast
{
	Cast() {}
	template <class Arg>
		requires std::convertible_to<Arg, Ret>
	Ret operator()(const Arg& a) const { return Ret(a); }
	template <class Arg>
		requires std::convertible_to<Arg, Ret>
	void operator()(Ret& buf, const Arg& a) const { buf = Ret(a); }
};

}

template <node_or_placeholder Arg>
auto operator!(Arg&& a)
{
	auto f = [](const auto& a) -> decltype(!a) { return !a; };
	return detail::MakeFunctionNode(f, std::forward<Arg>(a));
}
template <node_or_placeholder Arg>
auto operator-(Arg&& a)
{
	auto f = [](const auto& a) -> decltype(-a) { return -a; };
	return detail::MakeFunctionNode(f, std::forward<Arg>(a));
}

template <class Arg1, class Arg2>
	requires (node_or_placeholder<Arg1> || node_or_placeholder<Arg2>)
auto operator+(Arg1&& a, Arg2&& b)
{
	auto f = Overload(
		[](const auto& a, const auto& b) -> decltype(a + b) { return a + b; },
		[](auto& buf, const auto& a, const auto& b) { buf = a; buf += b; });
	return detail::MakeFunctionNode(f, std::forward<Arg1>(a), std::forward<Arg2>(b));
}
template <class Arg1, class Arg2>
	requires (node_or_placeholder<Arg1> || node_or_placeholder<Arg2>)
auto operator-(Arg1&& a, Arg2&& b)
{
	auto f = Overload(
		[](const auto& a, const auto& b) -> decltype(a - b) { return a - b; },
		[](auto& buf, const auto& a, const auto& b) { buf = a; buf -= b; });
	return detail::MakeFunctionNode(f, std::forward<Arg1>(a), std::forward<Arg2>(b));
}
template <class Arg1, class Arg2>
	requires (node_or_placeholder<Arg1> || node_or_placeholder<Arg2>)
auto operator*(Arg1&& a, Arg2&& b)
{
	auto f = Overload(
		[](const auto& a, const auto& b) -> decltype(a * b) { return a * b; },
		[](auto& buf, const auto& a, const auto& b) { buf = a; buf *= b; });
	return detail::MakeFunctionNode(f, std::forward<Arg1>(a), std::forward<Arg2>(b));
}
template <class Arg1, class Arg2>
	requires (node_or_placeholder<Arg1> || node_or_placeholder<Arg2>)
auto operator/(Arg1&& a, Arg2&& b)
{
	auto f = Overload(
		[](const auto& a, const auto& b) -> decltype(a / b) { return a / b; },
		[](auto& buf, const auto& a, const auto& b) { buf = a; buf /= b; });
	return detail::MakeFunctionNode(f, std::forward<Arg1>(a), std::forward<Arg2>(b));
}
template <class Arg1, class Arg2>
	requires (node_or_placeholder<Arg1> || node_or_placeholder<Arg2>)
auto operator%(Arg1&& a, Arg2&& b)
{
	auto f = Overload(
		[](const auto& a, const auto& b) -> decltype(a % b) { return a % b; },
		[](auto& buf, const auto& a, const auto& b) { buf = a; buf %= b; });
	return detail::MakeFunctionNode(f, std::forward<Arg1>(a), std::forward<Arg2>(b));
}

template <class Arg1, class Arg2>
	requires (node_or_placeholder<Arg1> || node_or_placeholder<Arg2>)
auto pow(Arg1&& a, Arg2&& b)
{
	auto f = [](const auto& a, const auto& b) -> decltype(std::pow(a, b)) { return std::pow(a, b); };
	return detail::MakeFunctionNode(f, std::forward<Arg1>(a), std::forward<Arg2>(b));
}
template <class Arg1, class Arg2>
	requires (node_or_placeholder<Arg1> || node_or_placeholder<Arg2>)
auto operator==(Arg1&& a, Arg2&& b)
{
	auto f = [](const auto& a, const auto& b) -> decltype(a == b) { return a == b; };
	return detail::MakeFunctionNode(f, std::forward<Arg1>(a), std::forward<Arg2>(b));
}
template <class Arg1, class Arg2>
	requires (node_or_placeholder<Arg1> || node_or_placeholder<Arg2>)
auto operator!=(Arg1&& a, Arg2&& b)
{
	auto f = [](const auto& a, const auto& b) -> decltype(a != b) { return a != b; };
	return detail::MakeFunctionNode(f, std::forward<Arg1>(a), std::forward<Arg2>(b));
}
template <class Arg1, class Arg2>
	requires (node_or_placeholder<Arg1> || node_or_placeholder<Arg2>)
auto operator<(Arg1&& a, Arg2&& b)
{
	auto f = [](const auto& a, const auto& b) -> decltype(a < b) { return a < b; };
	return detail::MakeFunctionNode(f, std::forward<Arg1>(a), std::forward<Arg2>(b));
}
template <class Arg1, class Arg2>
	requires (node_or_placeholder<Arg1> || node_or_placeholder<Arg2>)
auto operator<=(Arg1&& a, Arg2&& b)
{
	auto f = [](const auto& a, const auto& b) -> decltype(a <= b) { return a <= b; };
	return detail::MakeFunctionNode(f, std::forward<Arg1>(a), std::forward<Arg2>(b));
}
template <class Arg1, class Arg2>
	requires (node_or_placeholder<Arg1> || node_or_placeholder<Arg2>)
auto operator>(Arg1&& a, Arg2&& b)
{
	auto f = [](const auto& a, const auto& b) -> decltype(a > b) { return a > b; };
	return detail::MakeFunctionNode(f, std::forward<Arg1>(a), std::forward<Arg2>(b));
}
template <class Arg1, class Arg2>
	requires (node_or_placeholder<Arg1> || node_or_placeholder<Arg2>)
auto operator>=(Arg1&& a, Arg2&& b)
{
	auto f = [](const auto& a, const auto& b) -> decltype(a >= b) { return a >= b; };
	return detail::MakeFunctionNode(f, std::forward<Arg1>(a), std::forward<Arg2>(b));
}

struct OperatorAnd
{
	auto operator()(const auto& a, const auto& b) const -> decltype(a&& b) { return a && b; }
	template <class NodeImpl, class ...Args>
	decltype(auto) ShortCircuit(const NodeImpl& nodeimpl, Args&& ...args) const
	{
		return nodeimpl.GetArg<0>(args...) && nodeimpl.GetArg<1>(args...);
	}
};
template <class Arg1, class Arg2>
	requires (node_or_placeholder<Arg1> || node_or_placeholder<Arg2>)
auto operator&&(Arg1&& a, Arg2&& b)
{
	//auto f = [](const auto& a, const auto& b) -> decltype(a && b) { return a && b; };
	return detail::MakeFunctionNode(OperatorAnd{}, std::forward<Arg1>(a), std::forward<Arg2>(b));
}
struct OperatorOr
{
	auto operator()(const auto& a, const auto& b) const -> decltype(a || b) { return a || b; }
	template <class NodeImpl, class ...Args>
	decltype(auto) ShortCircuit(const NodeImpl& nodeimpl, Args&& ...args) const
	{
		return nodeimpl.GetArg<0>(args...) || nodeimpl.GetArg<1>(args...);
	}
};
template <class Arg1, class Arg2>
	requires (node_or_placeholder<Arg1> || node_or_placeholder<Arg2>)
auto operator||(Arg1&& a, Arg2&& b)
{
	//auto f = [](const auto& a, const auto& b) -> decltype(a || b) { return a || b; };
	return detail::MakeFunctionNode(OperatorOr{}, std::forward<Arg1>(a), std::forward<Arg2>(b));
}
//index operatorはメンバ関数として用意しなければならないので、ここでは定義できない。

template <node_or_placeholder Arg>
auto sin(Arg&& a)
{
	auto f = [](const auto& a) -> decltype(std::sin(a)) { return std::sin(a); };
	return detail::MakeFunctionNode(f, std::forward<Arg>(a));
}
template <node_or_placeholder Arg>
auto cos(Arg&& a)
{
	auto f = [](const auto& a) -> decltype(std::cos(a)) { return std::cos(a); };
	return detail::MakeFunctionNode(f, std::forward<Arg>(a));
}
template <node_or_placeholder Arg>
auto tan(Arg&& a)
{
	auto f = [](const auto& a) -> decltype(std::tan(a)) { return std::tan(a); };
	return detail::MakeFunctionNode(f, std::forward<Arg>(a));
}
template <node_or_placeholder Arg>
auto asin(Arg&& a)
{
	auto f = [](const auto& a) -> decltype(std::asin(a)) { return std::asin(a); };
	return detail::MakeFunctionNode(f, std::forward<Arg>(a));
}
template <node_or_placeholder Arg>
auto acos(Arg&& a)
{
	auto f = [](const auto& a) -> decltype(std::acos(a)) { return std::acos(a); };
	return detail::MakeFunctionNode(f, std::forward<Arg>(a));
}
template <node_or_placeholder Arg>
auto atan(Arg&& a)
{
	auto f = [](const auto& a) -> decltype(std::atan(a)) { return std::atan(a); };
	return detail::MakeFunctionNode(f, std::forward<Arg>(a));
}
template <node_or_placeholder Arg>
auto exp(Arg&& a)
{
	auto f = [](const auto& a) -> decltype(std::exp(a)) { return std::exp(a); };
	return detail::MakeFunctionNode(f, std::forward<Arg>(a));
}
template <node_or_placeholder Arg>
auto square(Arg&& a)
{
	auto f = Overload(
		[](const auto& a) -> decltype(a * a) { return a * a; },
		[](auto& buf, const auto& a) { buf = a; buf *= a; });
	return detail::MakeFunctionNode(f, std::forward<Arg>(a));
}
template <node_or_placeholder Arg>
auto sqrt(Arg&& a)
{
	auto f = [](const auto& a) -> decltype(std::sqrt(a)) { return std::sqrt(a); };
	return detail::MakeFunctionNode(f, std::forward<Arg>(a));
}
template <node_or_placeholder Arg>
auto cube(Arg&& a)
{
	auto f = Overload(
		[](const auto& a) -> decltype(a * a * a) { return a * a * a; },
		[](auto& buf, const auto& a) { buf = a; buf *= a; buf *= a; });
	return detail::MakeFunctionNode(f, std::forward<Arg>(a));
}
template <node_or_placeholder Arg>
auto cbrt(Arg&& a)
{
	auto f = [](const auto& a) -> decltype(std::cbrt(a)) { return std::cbrt(a); };
	return detail::MakeFunctionNode(f, std::forward<Arg>(a));
}
template <node_or_placeholder Arg>
auto ceil(Arg&& a)
{
	auto f = [](const auto& a) -> decltype(std::ceil(a)) { return std::ceil(a); };
	return detail::MakeFunctionNode(f, std::forward<Arg>(a));
}
template <node_or_placeholder Arg>
auto floor(Arg&& a)
{
	auto f = [](const auto& a) -> decltype(std::floor(a)) { return std::floor(a); };
	return detail::MakeFunctionNode(f, std::forward<Arg>(a));
}
template <node_or_placeholder Arg>
auto log(Arg&& a)
{
	auto f = [](const auto& a) -> decltype(std::log(a)) { return std::log(a); };
	return detail::MakeFunctionNode(f, std::forward<Arg>(a));
}
template <node_or_placeholder Arg>
auto log10(Arg&& a)
{
	auto f = [](const auto& a) -> decltype(std::log10(a)) { return std::log10(a); };
	return detail::MakeFunctionNode(f, std::forward<Arg>(a));
}
template <node_or_placeholder Arg>
auto abs(Arg&& a)
{
	auto f = [](const auto& a) -> decltype(std::abs(a)) { return std::abs(a); };
	return detail::MakeFunctionNode(f, std::forward<Arg>(a));
}
template <node_or_placeholder Arg>
auto len(Arg&& a)
{
	auto f = [](const auto& a) -> decltype(std::ranges::size(a)) { return std::ranges::size(a); };
	return detail::MakeFunctionNode(f, std::forward<Arg>(a));
}
template <node_or_placeholder Arg>
auto tostr(Arg&& a)
{
	auto f = Overload(
		[](const auto& a) -> decltype(std::to_string(a)) { return std::to_string(a); },
		[](auto& buf, const auto& a) { ToStr(a, buf); });
	return detail::MakeFunctionNode(f, std::forward<Arg>(a));
}

template <class Arg1, class Arg2>
	requires (node_or_placeholder<Arg1> || node_or_placeholder<Arg2>)
auto atan2(Arg1&& a, Arg2&& b)
{
	auto f = [](const auto& a, const auto& b) -> decltype(std::atan2(a, b)) { return std::atan2(a, b); };
	return detail::MakeFunctionNode(f, std::forward<Arg1>(a), std::forward<Arg2>(b));
}
template <class Arg1, class Arg2>
	requires (node_or_placeholder<Arg1> || node_or_placeholder<Arg2>)
auto log2(Arg1&& a, Arg2&& b)
{
	auto f = [](const auto& a, const auto& b) -> decltype(std::log2(a, b)) { return std::log2(a, b); };
	return detail::MakeFunctionNode(f, std::forward<Arg1>(a), std::forward<Arg2>(b));
}
template <class Arg1, class Arg2>
	requires (node_or_placeholder<Arg1> || node_or_placeholder<Arg2>)
auto hypot(Arg1&& a, Arg2&& b)
{
	auto f = [](const auto& a, const auto& b) -> decltype(std::hypot(a, b)) { return std::hypot(a, b); };
	return detail::MakeFunctionNode(f, std::forward<Arg1>(a), std::forward<Arg2>(b));
}
template <class Arg1, class Arg2>
	requires (node_or_placeholder<Arg1> || node_or_placeholder<Arg2>)
auto max(Arg1&& a, Arg2&& b)
{
	auto f = [](const auto& a, const auto& b) -> decltype(std::max(a, b)) { return std::max(a, b); };
	return detail::MakeFunctionNode(f, std::forward<Arg1>(a), std::forward<Arg2>(b));
}
template <class Arg1, class Arg2>
	requires (node_or_placeholder<Arg1> || node_or_placeholder<Arg2>)
auto min(Arg1&& a, Arg2&& b)
{
	auto f = [](const auto& a, const auto& b) -> decltype(std::min(a, b)) { return std::min(a, b); };
	return detail::MakeFunctionNode(f, std::forward<Arg1>(a), std::forward<Arg2>(b));
}

namespace detail
{
struct IfFunction
{
	auto operator()(const auto& a, const auto& b, const auto& c) const -> decltype(a ? b : c)
	{
		return a ? b : c;
	}
	template <class NodeImpl, class ...Args>
	decltype(auto) ShortCircuit(const NodeImpl& nodeimpl, Args&& ...args) const
	{
		return nodeimpl.template GetArg<0>(args...) ?
			nodeimpl.template GetArg<1>(args...) :
			nodeimpl.template GetArg<2>(args...);
	}
};
}
template <class Arg1, class Arg2, class Arg3>
	requires (node_or_placeholder<Arg1> || node_or_placeholder<Arg2> || node_or_placeholder<Arg3>)
auto if_(Arg1&& a, Arg2&& b, Arg3&& c)
{
	return detail::MakeFunctionNode(detail::IfFunction{},
									std::forward<Arg1>(a), std::forward<Arg2>(b), std::forward<Arg3>(c));
}

namespace detail
{
template <size_t NCase>
struct SwitchFunction
{
private:
	template <size_t I, std::integral Int, class NodeImpl, class ...Args>
	decltype(auto) ShortCircuit_rec(Int i, const NodeImpl& nodeimpl, Args&& ...args) const
	{
		if (i == I) return nodeimpl.template GetArg<I + 1>(args...);
		if constexpr (I < NCase) return ShortCircuit_rec<I + 1>(i, nodeimpl, args...);
		else throw NoElements();
	}
public:
	template <std::integral Int, class ...Args>
	auto operator()(Int i, Args&& ...args) const -> decltype(Switch(i, std::forward<Args>(args)...))
	{
		return Switch(i, std::forward<Args>(args)...);
	}
	template <class NodeImpl, class ...Args>
	decltype(auto) ShortCircuit(const NodeImpl& nodeimpl, Args&& ...args) const
	{
		return ShortCircuit_rec<0>(nodeimpl.template GetArg<0>(args...), nodeimpl, args...);
	}
};
}
//Do NOT use with 4 or more arguments including rtti nodes or placeholders,
//to avoid excessive compilation time.
template <class ...Args>
	requires (node_or_placeholder<Args> || ...) && (sizeof...(Args) > 1)
auto switch_(Args&& ...args)
{
	return detail::MakeFunctionNode(detail::SwitchFunction<sizeof...(Args) - 1>{}, std::forward<Args>(args)...);
}

template <node_or_placeholder Arg>
auto fwd(Arg&& a)
{
	auto f = [](const auto& a) { return a; };
	return detail::MakeFunctionNode(f, std::forward<Arg>(a));
}
/*
// TODO: define index operator

DEFINE_FN3(detail::If_, if_);

DEFINE_VAR_FN(detail::Switch_, switch_);

//通常は使わない。Placeholderや定数をFuncNodeに変換したい時に使う。
DEFINE_FN1(detail::Forward, fwd);
*/


template <class T, node_or_placeholder NP>
auto cast(NP&& np)
{
	return detail::MakeFunctionNode(detail::Cast<T>{}, std::forward<NP>(np));
}
template <FieldType Type, node_or_placeholder NP>
auto cast(NP&& np)
{
	using ValueType = DFieldInfo::TagTypeToValueType<Type>;
	return cast<ValueType>(std::forward<NP>(np));
}

template <node_or_placeholder NP> auto cast_i08(NP&& np) { return cast<FieldType::I08>(std::forward<NP>(np)); }
template <node_or_placeholder NP> auto cast_i16(NP&& np) { return cast<FieldType::I16>(std::forward<NP>(np)); }
template <node_or_placeholder NP> auto cast_i32(NP&& np) { return cast<FieldType::I32>(std::forward<NP>(np)); }
template <node_or_placeholder NP> auto cast_i64(NP&& np) { return cast<FieldType::I64>(std::forward<NP>(np)); }
template <node_or_placeholder NP> auto cast_f32(NP&& np) { return cast<FieldType::F32>(std::forward<NP>(np)); }
template <node_or_placeholder NP> auto cast_f64(NP&& np) { return cast<FieldType::F64>(std::forward<NP>(np)); }
template <node_or_placeholder NP> auto cast_c32(NP&& np) { return cast<FieldType::C32>(std::forward<NP>(np)); }
template <node_or_placeholder NP> auto cast_c64(NP&& np) { return cast<FieldType::C64>(std::forward<NP>(np)); }

/*
template <node_or_placeholder NP> auto cast_i08(NP&& np) { return detail::MakeFunctionNode<detail::CastI08>(std::forward<NP>(np)); }
template <node_or_placeholder NP> auto cast_i16(NP&& np) { return detail::MakeFunctionNode<detail::CastI16>(std::forward<NP>(np)); }
template <node_or_placeholder NP> auto cast_i32(NP&& np) { return detail::MakeFunctionNode<detail::CastI32>(std::forward<NP>(np)); }
template <node_or_placeholder NP> auto cast_i64(NP&& np) { return detail::MakeFunctionNode<detail::CastI64>(std::forward<NP>(np)); }
template <node_or_placeholder NP> auto cast_f32(NP&& np) { return detail::MakeFunctionNode<detail::CastF32>(std::forward<NP>(np)); }
template <node_or_placeholder NP> auto cast_f64(NP&& np) { return detail::MakeFunctionNode<detail::CastF64>(std::forward<NP>(np)); }
template <node_or_placeholder NP> auto cast_c32(NP&& np) { return detail::MakeFunctionNode<detail::CastC32>(std::forward<NP>(np)); }
template <node_or_placeholder NP> auto cast_c64(NP&& np) { return detail::MakeFunctionNode<detail::CastC64>(std::forward<NP>(np)); }
*/

}

template <class Func>
struct UserFunc
{
	template <class Func_>
		requires std::convertible_to<Func_, Func>
	UserFunc(Func_&& f) : m_func(std::forward<Func_>(f)) {}

	template <class ...NPs>
		requires (node_or_placeholder<NPs> || ...)
	auto operator()(NPs&& ...nps) const
	{
		return adapt::eval::detail::MakeFunctionNode(m_func, std::forward<NPs>(nps)...);
	}
private:
	Func m_func;
};
template<class Func> UserFunc(Func&&) -> UserFunc<std::decay_t<Func>>;

}

#endif