#ifndef ADAPT_UTILITY_H
#define ADAPT_UTILITY_H

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

template<class... Ts> struct Overload : Ts... { using Ts::operator()...; };
template<class... Ts> Overload(Ts...) -> Overload<Ts...>;

}

#endif