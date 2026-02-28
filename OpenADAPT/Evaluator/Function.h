#ifndef ADAPT_EVALUATOR_FUNCTION_H
#define ADAPT_EVALUATOR_FUNCTION_H

#include <type_traits>
#include <OpenADAPT/Utility/TypeTraits.h>
#include <OpenADAPT/Utility/Macros.h>
#include <OpenADAPT/Common/Concepts.h>
#include <OpenADAPT/Evaluator/ConstNode.h>
#include <OpenADAPT/Evaluator/FuncNode.h>
#include <OpenADAPT/Evaluator/TryJoinNode.h>
#include <OpenADAPT/Evaluator/TravInfoNode.h>
#include <OpenADAPT/Joint/LayerInfo.h>

namespace adapt
{

namespace eval
{

namespace detail
{

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

}

struct Not
{
	static constexpr ArithmeticConvLevel Level = ArithmeticConvLevel::None;
	auto operator()(const auto& a) const -> decltype(!a) { return !a; }
};
ADAPT_EXPORT
template <node_or_placeholder Arg>
auto operator!(Arg&& a)
{
	return detail::MakeFunctionNode(Not{}, std::forward<Arg>(a));
}
struct Promote
{
	static constexpr ArithmeticConvLevel Level = ArithmeticConvLevel::None;
	auto operator()(const auto& a) const -> decltype(+a) { return +a; }
};
ADAPT_EXPORT
template <node_or_placeholder Arg>
auto operator+(Arg&& a)
{
	return detail::MakeFunctionNode(Promote{}, std::forward<Arg>(a));
}
struct Negate
{
	static constexpr ArithmeticConvLevel Level = ArithmeticConvLevel::None;
	auto operator()(const auto& a) const -> decltype(-a) { return -a; }
};
ADAPT_EXPORT
template <node_or_placeholder Arg>
auto operator-(Arg&& a)
{
	return detail::MakeFunctionNode(Negate{}, std::forward<Arg>(a));
}

struct Plus
{
	static constexpr ArithmeticConvLevel Level = ArithmeticConvLevel::Compl;
	auto operator()(const auto& a, const auto& b) const -> decltype(a + b) { return a + b; }
	auto operator()(auto& buf, const auto& a, const auto& b) const { buf = a; buf += b; }
};
ADAPT_EXPORT
template <class Arg1, class Arg2>
	requires (node_or_placeholder<Arg1> || node_or_placeholder<Arg2>)
auto operator+(Arg1&& a, Arg2&& b)
{
	return detail::MakeFunctionNode(Plus{}, std::forward<Arg1>(a), std::forward<Arg2>(b));
}
struct Minus
{
	static constexpr ArithmeticConvLevel Level = ArithmeticConvLevel::Compl;
	auto operator()(const auto& a, const auto& b) const -> decltype(a - b) { return a - b; }
	auto operator()(auto& buf, const auto& a, const auto& b) const { buf = a; buf -= b; }
};
ADAPT_EXPORT
template <class Arg1, class Arg2>
	requires (node_or_placeholder<Arg1> || node_or_placeholder<Arg2>)
auto operator-(Arg1&& a, Arg2&& b)
{
	return detail::MakeFunctionNode(Minus{}, std::forward<Arg1>(a), std::forward<Arg2>(b));
}
struct Multiply
{
	static constexpr ArithmeticConvLevel Level = ArithmeticConvLevel::Compl;
	auto operator()(const auto& a, const auto& b) const -> decltype(a * b) { return a * b; }
	auto operator()(auto& buf, const auto& a, const auto& b) const { buf = a; buf *= b; }
};
ADAPT_EXPORT
template <class Arg1, class Arg2>
	requires (node_or_placeholder<Arg1> || node_or_placeholder<Arg2>)
auto operator*(Arg1&& a, Arg2&& b)
{
	return detail::MakeFunctionNode(Multiply{}, std::forward<Arg1>(a), std::forward<Arg2>(b));
}
struct Divide
{
	static constexpr ArithmeticConvLevel Level = ArithmeticConvLevel::Compl;
	auto operator()(const auto& a, const auto& b) const -> decltype(a / b) { return a / b; }
	auto operator()(auto& buf, const auto& a, const auto& b) const { buf = a; buf /= b; }
};
ADAPT_EXPORT
template <class Arg1, class Arg2>
	requires (node_or_placeholder<Arg1> || node_or_placeholder<Arg2>)
auto operator/(Arg1&& a, Arg2&& b)
{
	return detail::MakeFunctionNode(Divide{}, std::forward<Arg1>(a), std::forward<Arg2>(b));
}
struct Modulus
{
	static constexpr ArithmeticConvLevel Level = ArithmeticConvLevel::Integ;
	auto operator()(const auto& a, const auto& b) const -> decltype(a % b) { return a % b; }
	auto operator()(auto& buf, const auto& a, const auto& b) const { buf = a; buf %= b; }
};
ADAPT_EXPORT
template <class Arg1, class Arg2>
	requires (node_or_placeholder<Arg1> || node_or_placeholder<Arg2>)
auto operator%(Arg1&& a, Arg2&& b)
{
	return detail::MakeFunctionNode(Modulus{}, std::forward<Arg1>(a), std::forward<Arg2>(b));
}

struct Power
{
	static constexpr ArithmeticConvLevel Level = ArithmeticConvLevel::Compl;
	auto operator()(const auto& a, const auto& b) const -> decltype(std::pow(a, b)) { return std::pow(a, b); }
};
ADAPT_EXPORT
template <class Arg1, class Arg2>
	requires (node_or_placeholder<Arg1> || node_or_placeholder<Arg2>)
auto pow(Arg1&& a, Arg2&& b)
{
	return detail::MakeFunctionNode(Power{}, std::forward<Arg1>(a), std::forward<Arg2>(b));
}
struct Equal
{
	static constexpr ArithmeticConvLevel Level = ArithmeticConvLevel::Compl;
	auto operator()(const auto& a, const auto& b) const -> decltype(a == b) { return a == b; }
};
ADAPT_EXPORT
template <class Arg1, class Arg2>
	requires (node_or_placeholder<Arg1> || node_or_placeholder<Arg2>)
auto operator==(Arg1&& a, Arg2&& b)
{
	return detail::MakeFunctionNode(Equal{}, std::forward<Arg1>(a), std::forward<Arg2>(b));
}
struct NotEqual
{
	static constexpr ArithmeticConvLevel Level = ArithmeticConvLevel::Compl;
	auto operator()(const auto& a, const auto& b) const -> decltype(a != b) { return a != b; }
};
ADAPT_EXPORT
template <class Arg1, class Arg2>
	requires (node_or_placeholder<Arg1> || node_or_placeholder<Arg2>)
auto operator!=(Arg1&& a, Arg2&& b)
{
	return detail::MakeFunctionNode(NotEqual{}, std::forward<Arg1>(a), std::forward<Arg2>(b));
}
struct Less
{
	static constexpr ArithmeticConvLevel Level = ArithmeticConvLevel::Usual;
	auto operator()(const auto& a, const auto& b) const -> decltype(a < b)
	{
		if constexpr (std::integral<decltype(a)> && std::integral<decltype(b)>) return std::cmp_less(a, b);
		else return a < b;
	}
};
ADAPT_EXPORT
template <class Arg1, class Arg2>
	requires (node_or_placeholder<Arg1> || node_or_placeholder<Arg2>)
auto operator<(Arg1&& a, Arg2&& b)
{
	return detail::MakeFunctionNode(Less{}, std::forward<Arg1>(a), std::forward<Arg2>(b));
}
struct LessEqual
{
	static constexpr ArithmeticConvLevel Level = ArithmeticConvLevel::Usual;
	auto operator()(const auto& a, const auto& b) const -> decltype(a <= b)
	{
		if constexpr (std::integral<decltype(a)> && std::integral<decltype(b)>) return std::cmp_less_equal(a, b);
		else return a <= b;
	}
};
ADAPT_EXPORT
template <class Arg1, class Arg2>
	requires (node_or_placeholder<Arg1> || node_or_placeholder<Arg2>)
auto operator<=(Arg1&& a, Arg2&& b)
{
	return detail::MakeFunctionNode(LessEqual{}, std::forward<Arg1>(a), std::forward<Arg2>(b));
}
struct Greater
{
	static constexpr ArithmeticConvLevel Level = ArithmeticConvLevel::Usual;
	auto operator()(const auto& a, const auto& b) const -> decltype(a > b)
	{
		if constexpr (std::integral<decltype(a)> && std::integral<decltype(b)>) return std::cmp_greater(a, b);
		else return a > b;
	}
};
ADAPT_EXPORT
template <class Arg1, class Arg2>
	requires (node_or_placeholder<Arg1> || node_or_placeholder<Arg2>)
auto operator>(Arg1&& a, Arg2&& b)
{
	return detail::MakeFunctionNode(Greater{}, std::forward<Arg1>(a), std::forward<Arg2>(b));
}
struct GreaterEqual
{
	static constexpr ArithmeticConvLevel Level = ArithmeticConvLevel::Usual;
	auto operator()(const auto& a, const auto& b) const -> decltype(a >= b)
	{
		if constexpr (std::integral<decltype(a)> && std::integral<decltype(b)>) return std::cmp_greater_equal(a, b);
		else return a >= b;
	}
};
ADAPT_EXPORT
template <class Arg1, class Arg2>
	requires (node_or_placeholder<Arg1> || node_or_placeholder<Arg2>)
auto operator>=(Arg1&& a, Arg2&& b)
{
	return detail::MakeFunctionNode(GreaterEqual{}, std::forward<Arg1>(a), std::forward<Arg2>(b));
}

struct OperatorAnd
{
	static constexpr ArithmeticConvLevel Level = ArithmeticConvLevel::Bool;
	auto operator()(const auto& a, const auto& b) const -> decltype(a && b) { return a && b; }
	template <class NodeImpl, class ...Args>
	decltype(auto) ShortCircuit(const NodeImpl& nodeimpl, Args&& ...args) const
	{
		return nodeimpl.template GetArg<0>(args...) && nodeimpl.template GetArg<1>(args...);
	}
};
ADAPT_EXPORT
template <class Arg1, class Arg2>
	requires (node_or_placeholder<Arg1> || node_or_placeholder<Arg2>)
auto operator&&(Arg1&& a, Arg2&& b)
{
	return detail::MakeFunctionNode(OperatorAnd{}, std::forward<Arg1>(a), std::forward<Arg2>(b));
}
struct OperatorOr
{
	static constexpr ArithmeticConvLevel Level = ArithmeticConvLevel::Bool;
	auto operator()(const auto& a, const auto& b) const -> decltype(a || b) { return a || b; }
	template <class NodeImpl, class ...Args>
	decltype(auto) ShortCircuit(const NodeImpl& nodeimpl, Args&& ...args) const
	{
		return nodeimpl.template GetArg<0>(args...) || nodeimpl.template GetArg<1>(args...);
	}
};
ADAPT_EXPORT
template <class Arg1, class Arg2>
	requires (node_or_placeholder<Arg1> || node_or_placeholder<Arg2>)
auto operator||(Arg1&& a, Arg2&& b)
{
	return detail::MakeFunctionNode(OperatorOr{}, std::forward<Arg1>(a), std::forward<Arg2>(b));
}

struct BitwiseAnd
{
	static constexpr ArithmeticConvLevel Level = ArithmeticConvLevel::Integ;
	auto operator()(const auto& a, const auto& b) const -> decltype(a & b) { return a & b; }
};
ADAPT_EXPORT
template <class Arg1, class Arg2>
	requires (node_or_placeholder<Arg1> || node_or_placeholder<Arg2>)
auto operator&(Arg1&& a, Arg2&& b)
{
	return detail::MakeFunctionNode(BitwiseAnd{}, std::forward<Arg1>(a), std::forward<Arg2>(b));
}
struct BitwiseOr
{
	static constexpr ArithmeticConvLevel Level = ArithmeticConvLevel::Integ;
	auto operator()(const auto& a, const auto& b) const -> decltype(a | b) { return a | b; }
};
ADAPT_EXPORT
template <class Arg1, class Arg2>
	requires (node_or_placeholder<Arg1> || node_or_placeholder<Arg2>)
auto operator|(Arg1&& a, Arg2&& b)
{
	return detail::MakeFunctionNode(BitwiseOr{}, std::forward<Arg1>(a), std::forward<Arg2>(b));
}
struct BitwiseXor
{
	static constexpr ArithmeticConvLevel Level = ArithmeticConvLevel::Integ;
	auto operator()(const auto& a, const auto& b) const -> decltype(a ^ b) { return a ^ b; }
};
ADAPT_EXPORT
template <class Arg1, class Arg2>
	requires (node_or_placeholder<Arg1> || node_or_placeholder<Arg2>)
auto operator^(Arg1&& a, Arg2&& b)
{
	return detail::MakeFunctionNode(BitwiseXor{}, std::forward<Arg1>(a), std::forward<Arg2>(b));
}
struct BitwiseNot
{
	static constexpr ArithmeticConvLevel Level = ArithmeticConvLevel::Integ;
	auto operator()(const auto& a) const -> decltype(~a) { return ~a; }
};
ADAPT_EXPORT
template <node_or_placeholder Arg>
auto operator~(Arg&& a)
{
	return detail::MakeFunctionNode(BitwiseNot{}, std::forward<Arg>(a));
}
struct LeftShift
{
	static constexpr ArithmeticConvLevel Level = ArithmeticConvLevel::Promo;
	auto operator()(const auto& a, const auto& b) const -> decltype(a << b) { return a << b; }
};
ADAPT_EXPORT
template <class Arg1, class Arg2>
	requires (node_or_placeholder<Arg1> || node_or_placeholder<Arg2>)
auto operator<<(Arg1&& a, Arg2&& b)
{
	return detail::MakeFunctionNode(LeftShift{}, std::forward<Arg1>(a), std::forward<Arg2>(b));
}
struct RightShift
{
	static constexpr ArithmeticConvLevel Level = ArithmeticConvLevel::Promo;
	auto operator()(const auto& a, const auto& b) const -> decltype(a >> b) { return a >> b; }
};
ADAPT_EXPORT
template <class Arg1, class Arg2>
	requires (node_or_placeholder<Arg1> || node_or_placeholder<Arg2>)
auto operator>>(Arg1&& a, Arg2&& b)
{
	return detail::MakeFunctionNode(RightShift{}, std::forward<Arg1>(a), std::forward<Arg2>(b));
}

struct IsFinite
{
	static constexpr ArithmeticConvLevel Level = ArithmeticConvLevel::None;
	auto operator()(std::floating_point auto a) const { return std::isfinite(a); }
};
ADAPT_EXPORT
template <node_or_placeholder Arg>
auto isfinite(Arg&& a)
{
	return detail::MakeFunctionNode(IsFinite{}, std::forward<Arg>(a));
}
struct IsInf
{
	static constexpr ArithmeticConvLevel Level = ArithmeticConvLevel::None;
	auto operator()(std::floating_point auto a) const { return std::isinf(a); }
};
ADAPT_EXPORT
template <node_or_placeholder Arg>
auto isinf(Arg&& a)
{
	return detail::MakeFunctionNode(IsInf{}, std::forward<Arg>(a));
}
struct IsNan
{
	static constexpr ArithmeticConvLevel Level = ArithmeticConvLevel::None;
	auto operator()(std::floating_point auto a) const { return std::isnan(a); }
};
ADAPT_EXPORT
template <node_or_placeholder Arg>
auto isnan(Arg&& a)
{
	return detail::MakeFunctionNode(IsNan{}, std::forward<Arg>(a));
}
struct IsNormal
{
	static constexpr ArithmeticConvLevel Level = ArithmeticConvLevel::None;
	auto operator()(std::floating_point auto a) const { return std::isnormal(a); }
};
ADAPT_EXPORT
template <node_or_placeholder Arg>
auto isnormal(Arg&& a)
{
	return detail::MakeFunctionNode(IsNormal{}, std::forward<Arg>(a));
}

struct Sin
{
	static constexpr ArithmeticConvLevel Level = ArithmeticConvLevel::None;
	auto operator()(const auto& a) const -> decltype(std::sin(a)) { return std::sin(a); }
};
ADAPT_EXPORT
template <node_or_placeholder Arg>
auto sin(Arg&& a)
{
	return detail::MakeFunctionNode(Sin{}, std::forward<Arg>(a));
}
struct Cos
{
	static constexpr ArithmeticConvLevel Level = ArithmeticConvLevel::None;
	auto operator()(const auto& a) const -> decltype(std::cos(a)) { return std::cos(a); }
};
ADAPT_EXPORT
template <node_or_placeholder Arg>
auto cos(Arg&& a)
{
	return detail::MakeFunctionNode(Cos{}, std::forward<Arg>(a));
}
struct Tan
{
	static constexpr ArithmeticConvLevel Level = ArithmeticConvLevel::None;
	auto operator()(const auto& a) const -> decltype(std::tan(a)) { return std::tan(a); }
};
ADAPT_EXPORT
template <node_or_placeholder Arg>
auto tan(Arg&& a)
{
	return detail::MakeFunctionNode(Tan{}, std::forward<Arg>(a));
}
struct ASin
{
	static constexpr ArithmeticConvLevel Level = ArithmeticConvLevel::None;
	auto operator()(const auto& a) const -> decltype(std::asin(a)) { return std::asin(a); }
};
ADAPT_EXPORT
template <node_or_placeholder Arg>
auto asin(Arg&& a)
{
	return detail::MakeFunctionNode(ASin{}, std::forward<Arg>(a));
}
struct ACos
{
	static constexpr ArithmeticConvLevel Level = ArithmeticConvLevel::None;
	auto operator()(const auto& a) const -> decltype(std::acos(a)) { return std::acos(a); }
};
ADAPT_EXPORT
template <node_or_placeholder Arg>
auto acos(Arg&& a)
{
	return detail::MakeFunctionNode(ACos{}, std::forward<Arg>(a));
}
struct ATan
{
	static constexpr ArithmeticConvLevel Level = ArithmeticConvLevel::None;
	auto operator()(const auto& a) const -> decltype(std::atan(a)) { return std::atan(a); }
};
ADAPT_EXPORT
template <node_or_placeholder Arg>
auto atan(Arg&& a)
{
	return detail::MakeFunctionNode(ATan{}, std::forward<Arg>(a));
}
struct Sinh
{
	static constexpr ArithmeticConvLevel Level = ArithmeticConvLevel::None;
	auto operator()(const auto& a) const -> decltype(std::sinh(a)) { return std::sinh(a); }
};
ADAPT_EXPORT
template <node_or_placeholder Arg>
auto sinh(Arg&& a)
{
	return detail::MakeFunctionNode(Sinh{}, std::forward<Arg>(a));
}
struct Cosh
{
	static constexpr ArithmeticConvLevel Level = ArithmeticConvLevel::None;
	auto operator()(const auto& a) const -> decltype(std::cosh(a)) { return std::cosh(a); }
};
ADAPT_EXPORT
template <node_or_placeholder Arg>
auto cosh(Arg&& a)
{
	return detail::MakeFunctionNode(Cosh{}, std::forward<Arg>(a));
}
struct Tanh
{
	static constexpr ArithmeticConvLevel Level = ArithmeticConvLevel::None;
	auto operator()(const auto& a) const -> decltype(std::tanh(a)) { return std::tanh(a); }
};
ADAPT_EXPORT
template <node_or_placeholder Arg>
auto tanh(Arg&& a)
{
	return detail::MakeFunctionNode(Tanh{}, std::forward<Arg>(a));
}
struct ASinh
{
	static constexpr ArithmeticConvLevel Level = ArithmeticConvLevel::None;
	auto operator()(const auto& a) const -> decltype(std::asinh(a)) { return std::asinh(a); }
};
ADAPT_EXPORT
template <node_or_placeholder Arg>
auto asinh(Arg&& a)
{
	return detail::MakeFunctionNode(ASinh{}, std::forward<Arg>(a));
}
struct ACosh
{
	static constexpr ArithmeticConvLevel Level = ArithmeticConvLevel::None;
	auto operator()(const auto& a) const -> decltype(std::acosh(a)) { return std::acosh(a); }
};
ADAPT_EXPORT
template <node_or_placeholder Arg>
auto acosh(Arg&& a)
{
	return detail::MakeFunctionNode(ACosh{}, std::forward<Arg>(a));
}
struct ATanh
{
	static constexpr ArithmeticConvLevel Level = ArithmeticConvLevel::None;
	auto operator()(const auto& a) const -> decltype(std::atanh(a)) { return std::atanh(a); }
};
ADAPT_EXPORT
template <node_or_placeholder Arg>
auto atanh(Arg&& a)
{
	return detail::MakeFunctionNode(ATanh{}, std::forward<Arg>(a));
}

struct Exponential
{
	static constexpr ArithmeticConvLevel Level = ArithmeticConvLevel::None;
	auto operator()(const auto& a) const -> decltype(std::exp(a)) { return std::exp(a); }
};
ADAPT_EXPORT
template <node_or_placeholder Arg>
auto exp(Arg&& a)
{
	return detail::MakeFunctionNode(Exponential{}, std::forward<Arg>(a));
}
struct Exp2
{
	static constexpr ArithmeticConvLevel Level = ArithmeticConvLevel::None;
	auto operator()(const auto& a) const -> decltype(std::exp2(a)) { return std::exp2(a); }
};
ADAPT_EXPORT
template <node_or_placeholder Arg>
auto exp2(Arg&& a)
{
	return detail::MakeFunctionNode(Exp2{}, std::forward<Arg>(a));
}
struct Square
{
	static constexpr ArithmeticConvLevel Level = ArithmeticConvLevel::None;
	auto operator()(const auto& a) const -> decltype(a* a) { return a * a; }
	auto operator()(auto& buf, const auto& a) const { buf = a; buf *= a; }
};
ADAPT_EXPORT
template <node_or_placeholder Arg>
auto square(Arg&& a)
{
	return detail::MakeFunctionNode(Square{}, std::forward<Arg>(a));
}
struct Sqrt
{
	static constexpr ArithmeticConvLevel Level = ArithmeticConvLevel::None;
	auto operator()(const auto& a) const -> decltype(std::sqrt(a)) { return std::sqrt(a); }
};
ADAPT_EXPORT
template <node_or_placeholder Arg>
auto sqrt(Arg&& a)
{
	return detail::MakeFunctionNode(Sqrt{}, std::forward<Arg>(a));
}
struct Cube
{
	static constexpr ArithmeticConvLevel Level = ArithmeticConvLevel::None;
	auto operator()(const auto& a) const -> decltype(a* a* a) { return a * a * a; }
	auto operator()(auto& buf, const auto& a) const { buf = a; buf *= a; buf *= a; }
};
ADAPT_EXPORT
template <node_or_placeholder Arg>
auto cube(Arg&& a)
{
	return detail::MakeFunctionNode(Cube{}, std::forward<Arg>(a));
}
struct Cbrt
{
	static constexpr ArithmeticConvLevel Level = ArithmeticConvLevel::None;
	auto operator()(const auto& a) const -> decltype(std::cbrt(a)) { return std::cbrt(a); }
};
ADAPT_EXPORT
template <node_or_placeholder Arg>
auto cbrt(Arg&& a)
{
	return detail::MakeFunctionNode(Cbrt{}, std::forward<Arg>(a));
}
struct Log
{
	static constexpr ArithmeticConvLevel Level = ArithmeticConvLevel::Compl;
	auto operator()(const auto& a) const -> decltype(std::log(a)) { return std::log(a); }
	auto operator()(const auto& a, const auto& b) const -> decltype(std::log(a) / std::log(b)) { return std::log(a) / std::log(b); }
};
ADAPT_EXPORT
template <node_or_placeholder Arg>
auto log(Arg&& a)
{
	return detail::MakeFunctionNode(Log{}, std::forward<Arg>(a));
}
ADAPT_EXPORT
template <class Arg1, class Arg2>
	requires (node_or_placeholder<Arg1> || node_or_placeholder<Arg2>)
auto log(Arg1&& a, Arg2&& b)
{
	return detail::MakeFunctionNode(Log{}, std::forward<Arg1>(a), std::forward<Arg2>(b));
}
struct Log10
{
	static constexpr ArithmeticConvLevel Level = ArithmeticConvLevel::None;
	auto operator()(const auto& a) const -> decltype(std::log10(a)) { return std::log10(a); }
};
ADAPT_EXPORT
template <node_or_placeholder Arg>
auto log10(Arg&& a)
{
	return detail::MakeFunctionNode(Log10{}, std::forward<Arg>(a));
}
struct Log2
{
	static constexpr ArithmeticConvLevel Level = ArithmeticConvLevel::None;
	auto operator()(const auto& a) const -> decltype(std::log2(a)) { return std::log2(a); }
};
ADAPT_EXPORT
template <node_or_placeholder Arg>
auto log2(Arg&& a)
{
	return detail::MakeFunctionNode(Log2{}, std::forward<Arg>(a));
}

struct Ceil
{
	static constexpr ArithmeticConvLevel Level = ArithmeticConvLevel::None;
	auto operator()(const auto& a) const -> decltype(std::ceil(a)) { return std::ceil(a); }
};
ADAPT_EXPORT
template <node_or_placeholder Arg>
auto ceil(Arg&& a)
{
	return detail::MakeFunctionNode(Ceil{}, std::forward<Arg>(a));
}
struct Floor
{
	static constexpr ArithmeticConvLevel Level = ArithmeticConvLevel::None;
	auto operator()(const auto& a) const -> decltype(std::floor(a)) { return std::floor(a); }
};
ADAPT_EXPORT
template <node_or_placeholder Arg>
auto floor(Arg&& a)
{
	return detail::MakeFunctionNode(Floor{}, std::forward<Arg>(a));
}

struct Abs
{
	static constexpr ArithmeticConvLevel Level = ArithmeticConvLevel::None;
	auto operator()(const auto& a) const -> decltype(std::abs(a)) { return std::abs(a); }
};
ADAPT_EXPORT
template <node_or_placeholder Arg>
auto abs(Arg&& a)
{
	return detail::MakeFunctionNode(Abs{}, std::forward<Arg>(a));
}
struct Len
{
	static constexpr ArithmeticConvLevel Level = ArithmeticConvLevel::None;
	auto operator()(const auto& a) const -> decltype(std::ranges::size(a)) { return std::ranges::size(a); }
};
ADAPT_EXPORT
template <node_or_placeholder Arg>
auto len(Arg&& a)
{
	return detail::MakeFunctionNode(Len{}, std::forward<Arg>(a));
}
struct NumToStr
{
	static constexpr ArithmeticConvLevel Level = ArithmeticConvLevel::None;
	auto operator()(const auto& a) const -> decltype(std::to_string(a)) { return std::to_string(a); }
	auto operator()(auto& buf, const auto& a) const { ToStr(a, buf); }
};
ADAPT_EXPORT
template <node_or_placeholder Arg>
auto tostr(Arg&& a)
{
	return detail::MakeFunctionNode(NumToStr{}, std::forward<Arg>(a));
}
struct Substr
{
	static constexpr ArithmeticConvLevel Level = ArithmeticConvLevel::None;
	auto operator()(const auto& a, const auto& b, const auto& c) const -> decltype(a.substr((size_t)b, (size_t)c)) { return a.substr((size_t)b, (size_t)c); }
};
ADAPT_EXPORT
template <class Arg1, class Arg2, class Arg3>
	requires (node_or_placeholder<Arg1> || node_or_placeholder<Arg2> || node_or_placeholder<Arg3>)
auto substr(Arg1&& a, Arg2&& b, Arg3&& c)
{
	return detail::MakeFunctionNode(Substr{}, std::forward<Arg1>(a), std::forward<Arg2>(b), std::forward<Arg3>(c));
}


struct ATan2
{
	static constexpr ArithmeticConvLevel Level = ArithmeticConvLevel::Usual;
	auto operator()(const auto& a, const auto& b) const -> decltype(std::atan2(a, b)) { return std::atan2(a, b); }
};
ADAPT_EXPORT
template <class Arg1, class Arg2>
	requires (node_or_placeholder<Arg1> || node_or_placeholder<Arg2>)
auto atan2(Arg1&& a, Arg2&& b)
{
	return detail::MakeFunctionNode(ATan2{}, std::forward<Arg1>(a), std::forward<Arg2>(b));
}

struct Hypot
{
	static constexpr ArithmeticConvLevel Level = ArithmeticConvLevel::Usual;
	auto operator()(const auto& a, const auto& b) const -> decltype(std::hypot(a, b)) { return std::hypot(a, b); }
};
ADAPT_EXPORT
template <class Arg1, class Arg2>
	requires (node_or_placeholder<Arg1> || node_or_placeholder<Arg2>)
auto hypot(Arg1&& a, Arg2&& b)
{
	return detail::MakeFunctionNode(Hypot{}, std::forward<Arg1>(a), std::forward<Arg2>(b));
}
struct Max
{
	static constexpr ArithmeticConvLevel Level = ArithmeticConvLevel::Usual;
	//decltype(std::max(a, b))のような書き方だと、gccではエラーになる。
	template <class T>
		requires less_than_comparable<T>
	auto operator()(const T& a, const T& b) const { return std::max(a, b); }
};
ADAPT_EXPORT
template <class Arg1, class Arg2>
	requires (node_or_placeholder<Arg1> || node_or_placeholder<Arg2>)
auto max(Arg1&& a, Arg2&& b)
{
	return detail::MakeFunctionNode(Max{}, std::forward<Arg1>(a), std::forward<Arg2>(b));
}
struct Min
{
	static constexpr ArithmeticConvLevel Level = ArithmeticConvLevel::Usual;
	template <class T>
		requires less_than_comparable<T>
	auto operator()(const T& a, const T& b) const { return std::min(a, b); }
};
ADAPT_EXPORT
template <class Arg1, class Arg2>
	requires (node_or_placeholder<Arg1> || node_or_placeholder<Arg2>)
auto min(Arg1&& a, Arg2&& b)
{
	return detail::MakeFunctionNode(Min{}, std::forward<Arg1>(a), std::forward<Arg2>(b));
}

ADAPT_EXPORT
template <class A, class B, class C>
concept if_function_applicable = requires(A a, B b, C c)
{
	{ a ? b : c };
};
struct IfFunction
{
	static constexpr ArithmeticConvLevel Level = ArithmeticConvLevel::None;
	// -> decltype(a ? b : c)という形式にしてしまうと、
	// 戻り値が必ずconst&になってしまい、ダングリング参照になる。
	// よってコンセプトで呼び出しの可否を判定し、戻り値型はautoで取得する。
	template <class A, class B, class C>
		requires if_function_applicable<A, B, C>
	auto operator()(const A& a, const B& b, const C& c) const
	{
		using T1 = std::remove_cvref_t<B>;
		using T2 = std::remove_cvref_t<C>;
		if constexpr (IsComplex_v<T1> && std::is_arithmetic_v<T2>)
			return a ? b : T1(static_cast<typename T1::value_type>(c));
		else if constexpr (std::is_arithmetic_v<T1> && IsComplex_v<T2>)
			return a ? T2(static_cast<typename T2::value_type>(b)) : c;
		else
			return a ? b : c;
	}
	template <class NodeImpl, class ...Args>
	decltype(auto) ShortCircuit(const NodeImpl& n, Args&& ...args) const
	{
		//Rttiで実行するとき、complex+arithmeticの組み合わせで型変換に関する警告が出て鬱陶しいので、
		//回避するために明示的なキャストを行う。
		using T1 = std::remove_cvref_t<decltype(n.template GetArg<1>(args...))>;
		using T2 = std::remove_cvref_t<decltype(n.template GetArg<2>(args...))>;
		if constexpr (IsComplex_v<T1> && std::is_arithmetic_v<T2>)
			return n.template GetArg<0>(args...) ? n.template GetArg<1>(args...) :
												   T1(static_cast<typename T1::value_type>(n.template GetArg<2>(args...)));
		else if constexpr (std::is_arithmetic_v<T1> && IsComplex_v<T2>)
			return n.template GetArg<0>(args...) ? T2(static_cast<typename T2::value_type>(n.template GetArg<1>(args...))) :
												   n.template GetArg<2>(args...);
		else
			return n.template GetArg<0>(args...) ? n.template GetArg<1>(args...) :
												   n.template GetArg<2>(args...);
	}
};
ADAPT_EXPORT
template <class Arg1, class Arg2, class Arg3>
	requires (node_or_placeholder<Arg1> || node_or_placeholder<Arg2> || node_or_placeholder<Arg3>)
auto if_(Arg1&& a, Arg2&& b, Arg3&& c)
{
	return detail::MakeFunctionNode(IfFunction{},
									std::forward<Arg1>(a), std::forward<Arg2>(b), std::forward<Arg3>(c));
}

template <size_t NCase>
struct SwitchFunction
{
	static constexpr ArithmeticConvLevel Level = ArithmeticConvLevel::None;
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
//Do NOT use with 4 or more arguments including rtti nodes or placeholders,
//to avoid excessive compilation time.
ADAPT_EXPORT
template <class ...Args>
	requires (node_or_placeholder<Args> || ...) && (sizeof...(Args) > 1)
auto switch_(Args&& ...args)
{
	return detail::MakeFunctionNode(SwitchFunction<sizeof...(Args) - 1>{}, std::forward<Args>(args)...);
}

struct Forward
{
	static constexpr ArithmeticConvLevel Level = ArithmeticConvLevel::None;
	decltype(auto) operator()(const auto& a) const { return a; }
};
ADAPT_EXPORT
template <node_or_placeholder Arg>
auto fwd(Arg&& a)
{
	return detail::MakeFunctionNode(Forward{}, std::forward<Arg>(a));
}
/*
// TODO: define index operator

DEFINE_FN3(detail::If_, if_);

DEFINE_VAR_FN(detail::Switch_, switch_);

//通常は使わない。Placeholderや定数をFuncNodeに変換したい時に使う。
DEFINE_FN1(detail::Forward, fwd);
*/

template <class Ret>
struct Cast
{
	static constexpr ArithmeticConvLevel Level = ArithmeticConvLevel::None;
	Cast() {}
	template <class Arg>
		requires std::convertible_to<Arg, Ret>
	Ret operator()(const Arg& a) const
	{
		if constexpr (IsSame_XT_v<std::complex, Ret> && std::is_arithmetic_v<Arg>)
			return Ret((typename Ret::value_type)(a));
		else
			return Ret(a);
	}
	template <class Arg>
		requires std::convertible_to<Arg, Ret>
	void operator()(Ret& buf, const Arg& a) const { buf = Ret(a); }
};
ADAPT_EXPORT
template <class T, node_or_placeholder NP>
auto cast(NP&& np)
{
	return detail::MakeFunctionNode(Cast<T>{}, std::forward<NP>(np));
}
ADAPT_EXPORT
template <FieldType Type, node_or_placeholder NP>
auto cast(NP&& np)
{
	using ValueType = DFieldInfo::TagTypeToValueType<Type>;
	return cast<ValueType>(std::forward<NP>(np));
}

ADAPT_EXPORT
template <node_or_placeholder NP> auto cast_i08(NP&& np) { return cast<FieldType::I08>(std::forward<NP>(np)); }
ADAPT_EXPORT
template <node_or_placeholder NP> auto cast_i16(NP&& np) { return cast<FieldType::I16>(std::forward<NP>(np)); }
ADAPT_EXPORT
template <node_or_placeholder NP> auto cast_i32(NP&& np) { return cast<FieldType::I32>(std::forward<NP>(np)); }
ADAPT_EXPORT
template <node_or_placeholder NP> auto cast_i64(NP&& np) { return cast<FieldType::I64>(std::forward<NP>(np)); }
ADAPT_EXPORT
template <node_or_placeholder NP> auto cast_f32(NP&& np) { return cast<FieldType::F32>(std::forward<NP>(np)); }
ADAPT_EXPORT
template <node_or_placeholder NP> auto cast_f64(NP&& np) { return cast<FieldType::F64>(std::forward<NP>(np)); }
ADAPT_EXPORT
template <node_or_placeholder NP> auto cast_c32(NP&& np) { return cast<FieldType::C32>(std::forward<NP>(np)); }
ADAPT_EXPORT
template <node_or_placeholder NP> auto cast_c64(NP&& np) { return cast<FieldType::C64>(std::forward<NP>(np)); }

struct CastBool
{
	static constexpr ArithmeticConvLevel Level = ArithmeticConvLevel::None;
	template <std::convertible_to<bool> T>
	auto operator()(const T& a) const { return static_cast<bool>(a); }
};

ADAPT_EXPORT
template <node_or_placeholder NP> auto cast_bool(NP&& np) { return detail::MakeFunctionNode(CastBool{}, std::forward<NP>(np)); }

}

ADAPT_EXPORT
template <class Func>
struct UserFunc
{
	static constexpr ArithmeticConvLevel Level = ArithmeticConvLevel::None;
	template <class Func_>
		requires std::convertible_to<Func_, Func>
	UserFunc(Func_&& f) : m_func{ std::forward<Func_>(f) }
	{}

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