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
	auto operator()(const auto& a) const -> decltype(!a) { return !a; }
};
template <node_or_placeholder Arg>
auto operator!(Arg&& a)
{
	return detail::MakeFunctionNode(Not{}, std::forward<Arg>(a));
}
struct Negate
{
	auto operator()(const auto& a) const -> decltype(-a) { return -a; }
};
template <node_or_placeholder Arg>
auto operator-(Arg&& a)
{
	return detail::MakeFunctionNode(Negate{}, std::forward<Arg>(a));
}

struct Plus
{
	auto operator()(const auto& a, const auto& b) const -> decltype(a + b) { return a + b; }
	auto operator()(auto& buf, const auto& a, const auto& b) const { buf = a; buf += b; }
};
template <class Arg1, class Arg2>
	requires (node_or_placeholder<Arg1> || node_or_placeholder<Arg2>)
auto operator+(Arg1&& a, Arg2&& b)
{
	return detail::MakeFunctionNode(Plus{}, std::forward<Arg1>(a), std::forward<Arg2>(b));
}
struct Minus
{
	auto operator()(const auto& a, const auto& b) const -> decltype(a - b) { return a - b; }
	auto operator()(auto& buf, const auto& a, const auto& b) const { buf = a; buf -= b; }
};
template <class Arg1, class Arg2>
	requires (node_or_placeholder<Arg1> || node_or_placeholder<Arg2>)
auto operator-(Arg1&& a, Arg2&& b)
{
	return detail::MakeFunctionNode(Minus{}, std::forward<Arg1>(a), std::forward<Arg2>(b));
}
struct Multiply
{
	auto operator()(const auto& a, const auto& b) const -> decltype(a* b) { return a * b; }
	auto operator()(auto& buf, const auto& a, const auto& b) const { buf = a; buf *= b; }
};
template <class Arg1, class Arg2>
	requires (node_or_placeholder<Arg1> || node_or_placeholder<Arg2>)
auto operator*(Arg1&& a, Arg2&& b)
{
	return detail::MakeFunctionNode(Multiply{}, std::forward<Arg1>(a), std::forward<Arg2>(b));
}
struct Divide
{
	auto operator()(const auto& a, const auto& b) const -> decltype(a / b) { return a / b; }
	auto operator()(auto& buf, const auto& a, const auto& b) const { buf = a; buf /= b; }
};
template <class Arg1, class Arg2>
	requires (node_or_placeholder<Arg1> || node_or_placeholder<Arg2>)
auto operator/(Arg1&& a, Arg2&& b)
{
	return detail::MakeFunctionNode(Divide{}, std::forward<Arg1>(a), std::forward<Arg2>(b));
}
struct Modulus
{
	auto operator()(const auto& a, const auto& b) const -> decltype(a% b) { return a % b; }
	auto operator()(auto& buf, const auto& a, const auto& b) const { buf = a; buf %= b; }
};
template <class Arg1, class Arg2>
	requires (node_or_placeholder<Arg1> || node_or_placeholder<Arg2>)
auto operator%(Arg1&& a, Arg2&& b)
{
	return detail::MakeFunctionNode(Modulus{}, std::forward<Arg1>(a), std::forward<Arg2>(b));
}

struct Power
{
	auto operator()(const auto& a, const auto& b) const -> decltype(std::pow(a, b)) { return std::pow(a, b); }
};
template <class Arg1, class Arg2>
	requires (node_or_placeholder<Arg1> || node_or_placeholder<Arg2>)
auto pow(Arg1&& a, Arg2&& b)
{
	return detail::MakeFunctionNode(Power{}, std::forward<Arg1>(a), std::forward<Arg2>(b));
}
struct Equal
{
	auto operator()(const auto& a, const auto& b) const -> decltype(a == b) { return a == b; }
};
template <class Arg1, class Arg2>
	requires (node_or_placeholder<Arg1> || node_or_placeholder<Arg2>)
auto operator==(Arg1&& a, Arg2&& b)
{
	return detail::MakeFunctionNode(Equal{}, std::forward<Arg1>(a), std::forward<Arg2>(b));
}
struct NotEqual
{
	auto operator()(const auto& a, const auto& b) const -> decltype(a != b) { return a != b; }
};
template <class Arg1, class Arg2>
	requires (node_or_placeholder<Arg1> || node_or_placeholder<Arg2>)
auto operator!=(Arg1&& a, Arg2&& b)
{
	return detail::MakeFunctionNode(NotEqual{}, std::forward<Arg1>(a), std::forward<Arg2>(b));
}
struct Less
{
	auto operator()(const auto& a, const auto& b) const -> decltype(a < b) { return a < b; }
};
template <class Arg1, class Arg2>
	requires (node_or_placeholder<Arg1> || node_or_placeholder<Arg2>)
auto operator<(Arg1&& a, Arg2&& b)
{
	return detail::MakeFunctionNode(Less{}, std::forward<Arg1>(a), std::forward<Arg2>(b));
}
struct LessEqual
{
	auto operator()(const auto& a, const auto& b) const -> decltype(a <= b) { return a <= b; }
};
template <class Arg1, class Arg2>
	requires (node_or_placeholder<Arg1> || node_or_placeholder<Arg2>)
auto operator<=(Arg1&& a, Arg2&& b)
{
	return detail::MakeFunctionNode(LessEqual{}, std::forward<Arg1>(a), std::forward<Arg2>(b));
}
struct Greater
{
	auto operator()(const auto& a, const auto& b) const -> decltype(a > b) { return a > b; }
};
template <class Arg1, class Arg2>
	requires (node_or_placeholder<Arg1> || node_or_placeholder<Arg2>)
auto operator>(Arg1&& a, Arg2&& b)
{
	return detail::MakeFunctionNode(Greater{}, std::forward<Arg1>(a), std::forward<Arg2>(b));
}
struct GreaterEqual
{
	auto operator()(const auto& a, const auto& b) const -> decltype(a >= b) { return a >= b; }
};
template <class Arg1, class Arg2>
	requires (node_or_placeholder<Arg1> || node_or_placeholder<Arg2>)
auto operator>=(Arg1&& a, Arg2&& b)
{
	return detail::MakeFunctionNode(GreaterEqual{}, std::forward<Arg1>(a), std::forward<Arg2>(b));
}

struct OperatorAnd
{
	auto operator()(const auto& a, const auto& b) const -> decltype(a&& b) { return a && b; }
	template <class NodeImpl, class ...Args>
	decltype(auto) ShortCircuit(const NodeImpl& nodeimpl, Args&& ...args) const
	{
		return nodeimpl.template GetArg<0>(args...) && nodeimpl.template GetArg<1>(args...);
	}
};
template <class Arg1, class Arg2>
	requires (node_or_placeholder<Arg1> || node_or_placeholder<Arg2>)
auto operator&&(Arg1&& a, Arg2&& b)
{
	return detail::MakeFunctionNode(OperatorAnd{}, std::forward<Arg1>(a), std::forward<Arg2>(b));
}
struct OperatorOr
{
	auto operator()(const auto& a, const auto& b) const -> decltype(a || b) { return a || b; }
	template <class NodeImpl, class ...Args>
	decltype(auto) ShortCircuit(const NodeImpl& nodeimpl, Args&& ...args) const
	{
		return nodeimpl.template GetArg<0>(args...) || nodeimpl.template GetArg<1>(args...);
	}
};
template <class Arg1, class Arg2>
	requires (node_or_placeholder<Arg1> || node_or_placeholder<Arg2>)
auto operator||(Arg1&& a, Arg2&& b)
{
	return detail::MakeFunctionNode(OperatorOr{}, std::forward<Arg1>(a), std::forward<Arg2>(b));
}

struct IsFinite
{
	auto operator()(const auto& a) const -> decltype(std::isfinite(a)) { return std::isfinite(a); }
};
template <node_or_placeholder Arg>
auto isfinite(Arg&& a)
{
	return detail::MakeFunctionNode(IsFinite{}, std::forward<Arg>(a));
}
struct IsInf
{
	auto operator()(const auto& a) const -> decltype(std::isinf(a)) { return std::isinf(a); }
};
template <node_or_placeholder Arg>
auto isinf(Arg&& a)
{
	return detail::MakeFunctionNode(IsInf{}, std::forward<Arg>(a));
}
struct IsNan
{
	auto operator()(const auto& a) const -> decltype(std::isnan(a)) { return std::isnan(a); }
};
template <node_or_placeholder Arg>
auto isnan(Arg&& a)
{
	return detail::MakeFunctionNode(IsNan{}, std::forward<Arg>(a));
}
struct IsNormal
{
	auto operator()(const auto& a) const -> decltype(std::isnormal(a)) { return std::isnormal(a); }
};
template <node_or_placeholder Arg>
auto isnormal(Arg&& a)
{
	return detail::MakeFunctionNode(IsNormal{}, std::forward<Arg>(a));
}

struct Sin
{
	auto operator()(const auto& a) const -> decltype(std::sin(a)) { return std::sin(a); }
};
template <node_or_placeholder Arg>
auto sin(Arg&& a)
{
	return detail::MakeFunctionNode(Sin{}, std::forward<Arg>(a));
}
struct Cos
{
	auto operator()(const auto& a) const -> decltype(std::cos(a)) { return std::cos(a); }
};
template <node_or_placeholder Arg>
auto cos(Arg&& a)
{
	return detail::MakeFunctionNode(Cos{}, std::forward<Arg>(a));
}
struct Tan
{
	auto operator()(const auto& a) const -> decltype(std::tan(a)) { return std::tan(a); }
};
template <node_or_placeholder Arg>
auto tan(Arg&& a)
{
	return detail::MakeFunctionNode(Tan{}, std::forward<Arg>(a));
}
struct ASin
{
	auto operator()(const auto& a) const -> decltype(std::asin(a)) { return std::asin(a); }
};
template <node_or_placeholder Arg>
auto asin(Arg&& a)
{
	return detail::MakeFunctionNode(ASin{}, std::forward<Arg>(a));
}
struct ACos
{
	auto operator()(const auto& a) const -> decltype(std::acos(a)) { return std::acos(a); }
};
template <node_or_placeholder Arg>
auto acos(Arg&& a)
{
	return detail::MakeFunctionNode(ACos{}, std::forward<Arg>(a));
}
struct ATan
{
	auto operator()(const auto& a) const -> decltype(std::atan(a)) { return std::atan(a); }
};
template <node_or_placeholder Arg>
auto atan(Arg&& a)
{
	return detail::MakeFunctionNode(ATan{}, std::forward<Arg>(a));
}
struct Sinh
{
	auto operator()(const auto& a) const -> decltype(std::sinh(a)) { return std::sinh(a); }
};
template <node_or_placeholder Arg>
auto sinh(Arg&& a)
{
	return detail::MakeFunctionNode(Sinh{}, std::forward<Arg>(a));
}
struct Cosh
{
	auto operator()(const auto& a) const -> decltype(std::cosh(a)) { return std::cosh(a); }
};
template <node_or_placeholder Arg>
auto cosh(Arg&& a)
{
	return detail::MakeFunctionNode(Cosh{}, std::forward<Arg>(a));
}
struct Tanh
{
	auto operator()(const auto& a) const -> decltype(std::tanh(a)) { return std::tanh(a); }
};
template <node_or_placeholder Arg>
auto tanh(Arg&& a)
{
	return detail::MakeFunctionNode(Tanh{}, std::forward<Arg>(a));
}
struct ASinh
{
	auto operator()(const auto& a) const -> decltype(std::asinh(a)) { return std::asinh(a); }
};
template <node_or_placeholder Arg>
auto asinh(Arg&& a)
{
	return detail::MakeFunctionNode(ASinh{}, std::forward<Arg>(a));
}
struct ACosh
{
	auto operator()(const auto& a) const -> decltype(std::acosh(a)) { return std::acosh(a); }
};
template <node_or_placeholder Arg>
auto acosh(Arg&& a)
{
	return detail::MakeFunctionNode(ACosh{}, std::forward<Arg>(a));
}
struct ATanh
{
	auto operator()(const auto& a) const -> decltype(std::atanh(a)) { return std::atanh(a); }
};
template <node_or_placeholder Arg>
auto atanh(Arg&& a)
{
	return detail::MakeFunctionNode(ATanh{}, std::forward<Arg>(a));
}

struct Exponential
{
	auto operator()(const auto& a) const -> decltype(std::exp(a)) { return std::exp(a); }
};
template <node_or_placeholder Arg>
auto exp(Arg&& a)
{
	return detail::MakeFunctionNode(Exponential{}, std::forward<Arg>(a));
}
struct Exp2
{
	auto operator()(const auto& a) const -> decltype(std::exp2(a)) { return std::exp2(a); }
};
template <node_or_placeholder Arg>
auto exp2(Arg&& a)
{
	return detail::MakeFunctionNode(Exp2{}, std::forward<Arg>(a));
}
struct Square
{
	auto operator()(const auto& a) const -> decltype(a* a) { return a * a; }
	auto operator()(auto& buf, const auto& a) const { buf = a; buf *= a; }
};
template <node_or_placeholder Arg>
auto square(Arg&& a)
{
	return detail::MakeFunctionNode(Square{}, std::forward<Arg>(a));
}
struct Sqrt
{
	auto operator()(const auto& a) const -> decltype(std::sqrt(a)) { return std::sqrt(a); }
};
template <node_or_placeholder Arg>
auto sqrt(Arg&& a)
{
	return detail::MakeFunctionNode(Sqrt{}, std::forward<Arg>(a));
}
struct Cube
{
	auto operator()(const auto& a) const -> decltype(a* a* a) { return a * a * a; }
	auto operator()(auto& buf, const auto& a) const { buf = a; buf *= a; buf *= a; }
};
template <node_or_placeholder Arg>
auto cube(Arg&& a)
{
	return detail::MakeFunctionNode(Cube{}, std::forward<Arg>(a));
}
struct Cbrt
{
	auto operator()(const auto& a) const -> decltype(std::cbrt(a)) { return std::cbrt(a); }
};
template <node_or_placeholder Arg>
auto cbrt(Arg&& a)
{
	return detail::MakeFunctionNode(Cbrt{}, std::forward<Arg>(a));
}
struct Log
{
	auto operator()(const auto& a) const -> decltype(std::log(a)) { return std::log(a); }
	auto operator()(const auto& a, const auto& b) const -> decltype(std::log(a) / std::log(b)) { return std::log(a) / std::log(b); }
};
template <node_or_placeholder Arg>
auto log(Arg&& a)
{
	return detail::MakeFunctionNode(Log{}, std::forward<Arg>(a));
}
template <class Arg1, class Arg2>
	requires (node_or_placeholder<Arg1> || node_or_placeholder<Arg2>)
auto log(Arg1&& a, Arg2&& b)
{
	return detail::MakeFunctionNode(Log{}, std::forward<Arg1>(a), std::forward<Arg2>(b));
}
struct Log10
{
	auto operator()(const auto& a) const -> decltype(std::log10(a)) { return std::log10(a); }
};
template <node_or_placeholder Arg>
auto log10(Arg&& a)
{
	return detail::MakeFunctionNode(Log10{}, std::forward<Arg>(a));
}
struct Log2
{
	auto operator()(const auto& a) const -> decltype(std::log2(a)) { return std::log2(a); }
};
template <node_or_placeholder Arg>
auto log2(Arg&& a)
{
	return detail::MakeFunctionNode(Log2{}, std::forward<Arg>(a));
}

struct Ceil
{
	auto operator()(const auto& a) const -> decltype(std::ceil(a)) { return std::ceil(a); }
};
template <node_or_placeholder Arg>
auto ceil(Arg&& a)
{
	return detail::MakeFunctionNode(Ceil{}, std::forward<Arg>(a));
}
struct Floor
{
	auto operator()(const auto& a) const -> decltype(std::floor(a)) { return std::floor(a); }
};
template <node_or_placeholder Arg>
auto floor(Arg&& a)
{
	return detail::MakeFunctionNode(Floor{}, std::forward<Arg>(a));
}

struct Abs
{
	auto operator()(const auto& a) const -> decltype(std::abs(a)) { return std::abs(a); }
};
template <node_or_placeholder Arg>
auto abs(Arg&& a)
{
	return detail::MakeFunctionNode(Abs{}, std::forward<Arg>(a));
}
struct Len
{
	auto operator()(const auto& a) const -> decltype(std::ranges::size(a)) { return std::ranges::size(a); }
};
template <node_or_placeholder Arg>
auto len(Arg&& a)
{
	return detail::MakeFunctionNode(Len{}, std::forward<Arg>(a));
}
struct NumToStr
{
	auto operator()(const auto& a) const -> decltype(std::to_string(a)) { return std::to_string(a); }
	auto operator()(auto& buf, const auto& a) const { ToStr(a, buf); }
};
template <node_or_placeholder Arg>
auto tostr(Arg&& a)
{
	return detail::MakeFunctionNode(NumToStr{}, std::forward<Arg>(a));
}

struct ATan2
{
	auto operator()(const auto& a, const auto& b) const -> decltype(std::atan2(a, b)) { return std::atan2(a, b); }
};
template <class Arg1, class Arg2>
	requires (node_or_placeholder<Arg1> || node_or_placeholder<Arg2>)
auto atan2(Arg1&& a, Arg2&& b)
{
	return detail::MakeFunctionNode(ATan2{}, std::forward<Arg1>(a), std::forward<Arg2>(b));
}

struct Hypot
{
	auto operator()(const auto& a, const auto& b) const -> decltype(std::hypot(a, b)) { return std::hypot(a, b); }
};
template <class Arg1, class Arg2>
	requires (node_or_placeholder<Arg1> || node_or_placeholder<Arg2>)
auto hypot(Arg1&& a, Arg2&& b)
{
	return detail::MakeFunctionNode(Hypot{}, std::forward<Arg1>(a), std::forward<Arg2>(b));
}
struct Max
{
	//decltype(std::max(a, b))のような書き方だと、gccではエラーになる。
	template <class T>
		requires less_than_comparable<T>
	auto operator()(const T& a, const T& b) const { return std::max(a, b); }
};
template <class Arg1, class Arg2>
	requires (node_or_placeholder<Arg1> || node_or_placeholder<Arg2>)
auto max(Arg1&& a, Arg2&& b)
{
	return detail::MakeFunctionNode(Max{}, std::forward<Arg1>(a), std::forward<Arg2>(b));
}
struct Min
{
	template <class T>
		requires less_than_comparable<T>
	auto operator()(const T& a, const T& b) const { return std::min(a, b); }
};
template <class Arg1, class Arg2>
	requires (node_or_placeholder<Arg1> || node_or_placeholder<Arg2>)
auto min(Arg1&& a, Arg2&& b)
{
	return detail::MakeFunctionNode(Min{}, std::forward<Arg1>(a), std::forward<Arg2>(b));
}

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
template <class ...Args>
	requires (node_or_placeholder<Args> || ...) && (sizeof...(Args) > 1)
auto switch_(Args&& ...args)
{
	return detail::MakeFunctionNode(SwitchFunction<sizeof...(Args) - 1>{}, std::forward<Args>(args)...);
}

struct Forward
{
	decltype(auto) operator()(const auto& a) const { return a; }
};
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
	Cast() {}
	template <class Arg>
		requires std::convertible_to<Arg, Ret>
	Ret operator()(const Arg& a) const { return Ret(a); }
	template <class Arg>
		requires std::convertible_to<Arg, Ret>
	void operator()(Ret& buf, const Arg& a) const { buf = Ret(a); }
};
template <class T, node_or_placeholder NP>
auto cast(NP&& np)
{
	return detail::MakeFunctionNode(Cast<T>{}, std::forward<NP>(np));
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