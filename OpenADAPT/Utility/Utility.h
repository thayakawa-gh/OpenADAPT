#ifndef ADAPT_UTILITY_UTILITY_H
#define ADAPT_UTILITY_UTILITY_H

namespace adapt
{

struct Default
{
	template <class T>
	operator T() const { return T{}; }
};

template <auto I>
struct Number
{
	using Type = decltype(I);
	static constexpr Type value = I;
};

class EmptyClass {};

//tuple_catは要素にrvalue referenceを含む場合に使えないらしい。
//ので、代用品を用意しておく。
namespace detail
{
template <class ...T, class U, size_t ...Indices>
auto TupleAdd_impl([[maybe_unused]] std::tuple<T...> t, U&& u, std::index_sequence<Indices...>)
{
	return std::forward_as_tuple(std::get<Indices>(std::move(t))..., std::forward<U>(u));
}
}
template <class ...T, class U>
auto TupleAdd([[maybe_unused]] std::tuple<T...> t, U&& u)
{
	return detail::TupleAdd_impl(std::move(t), std::forward<U>(u), std::make_index_sequence<sizeof...(T)>());
}

// 通常、make_tupleは全てコピー、std::tieは全てlvalue ref、std::forwar_as_tupleは完全転送を行う。
// ただいずれの関数も、「lvalue refはlvalue refで」、「rvalue refはコピーして」保存しておくようなtupleを作ることができない。
// よって、代用品を用意しておく。
template <class ...Args>
std::tuple<Args...> MakeTemporaryTuple(Args&& ...args)
{
	return std::tuple<Args...>(std::forward<Args>(args)...);
}
// tupleの中身を全てコピーし、参照等はdecayする。
// Argsがrvalue referenceでもlvalue referenceでも無視してコピーする。
template <class ...Args>
std::tuple<std::decay_t<Args>...> MakeDecayedCopy(const std::tuple<Args...>& t)
{
	return std::apply([](const auto& ...args)
	{
		return std::tuple<std::decay_t<Args>...>(args...);
	}, t);
}

template<class... Ts> struct Overload : Ts... { using Ts::operator()...; };
template<class... Ts> Overload(Ts...) -> Overload<Ts...>;

}

#endif