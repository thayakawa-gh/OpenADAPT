#ifndef ADAPT_NAMEDTUPLE_H
#define ADAPT_NAMEDTUPLE_H

#include <type_traits>
#include <tuple>
#include <limits>
#include <string_view>
#include <OpenADAPT/Utility/StaticString.h>
#include <OpenADAPT/Utility/TypeTraits.h>

namespace adapt
{

template <StaticChar N, class T>
struct Named
{
	static constexpr StaticChar m_name = N;
	using Type = T;
	template <class T_>
		requires std::convertible_to<T_&&, T>
	Named(T_&& v) : m_value(std::forward<T_>(v)) {}
	T m_value;
};

template <class ...T>
class NamedTuple;
template <StaticChar ...Names, class ...T>
class NamedTuple<Named<Names, T>...> : public std::tuple<T...>
{
	//static constexpr std::tuple<detail::Name<Names.GetSize()>...> m_names = { Names... };
public:
	using Base = std::tuple<T...>;
	template <size_t N, size_t I = 0>
	static constexpr size_t FindName([[maybe_unused]] StaticChar<N> n)
	{
		if constexpr (I < sizeof...(T))
		{
			if (std::get<I>(std::make_tuple(Names...)) == n) return I;
			return FindName<N, I + 1>(n);
		}
		else return std::numeric_limits<size_t>::max();
	}
	using std::tuple<T...>::tuple;
};

template <class T>
struct IsNamedTuple : public std::false_type {};
template <class ...T>
struct IsNamedTuple<NamedTuple<T...>> : public std::true_type {};

template <class T>
concept named_tuple = IsNamedTuple<T>::value;

template <StaticChar Name_, class ...T>
decltype(auto) Get(NamedTuple<T...>& t) { return std::get<NamedTuple<T...>::FindName(Name_)>(t); }
template <StaticChar Name_, class ...T>
decltype(auto) Get(const NamedTuple<T...>& t) { return std::get<NamedTuple<T...>::FindName(Name_)>(t); }
template <StaticChar Name_, class ...T>
decltype(auto) Get(NamedTuple<T...>&& t) { return std::get<NamedTuple<T...>::FindName(Name_)>(std::move(t)); }

template <size_t N, class T>
struct TupleElement;
template <size_t N, StaticChar ...Names, class ...T>
struct TupleElement<N, NamedTuple<Named<Names, T>...>> : public std::tuple_element<N, std::tuple<T...>> {};
template <size_t N, class ...T>
struct TupleElement<N, std::tuple<T...>> : public std::tuple_element<N, std::tuple<T...>> {};
template <size_t N, class T>
using TupleElement_t = TupleElement<N, T>::type;

template <class T>
struct TupleSize;
template <StaticChar ...Names, class ...T>
struct TupleSize<NamedTuple<Named<Names, T>...>> : public std::tuple_size<std::tuple<T...>> {};
template <class ...T>
struct TupleSize<std::tuple<T...>> : public std::tuple_size<typename std::tuple<T...>> {};
template <class T>
inline constexpr size_t TupleSize_v = TupleSize<T>::value;

namespace detail
{
template <size_t Index, StaticChar Name>
struct TupleElementName_impl_s
{
	static constexpr StaticString<Name> GetName(Number<Index>) { return StaticString<Name>{}; }
};
template <size_t Index, class Indices, class Names>
struct TupleElementName_impl;
template <size_t Index, size_t ...Indices, StaticChar ...Names>
struct TupleElementName_impl<Index, std::index_sequence<Indices...>, ValueList<Names...>>
	: public TupleElementName_impl_s<Indices, Names>...
{
	using TupleElementName_impl_s<Indices, Names>::GetName...;
	using Type = decltype(GetName(Number<Index>{}));
};
}
template <size_t Index, class T>
struct TupleElementName;
template <size_t Index, StaticChar ...Names, class ...Ts>
struct TupleElementName<Index, NamedTuple<Named<Names, Ts>...>>
{
	static constexpr size_t Size = sizeof...(Ts);
	using Type = detail::TupleElementName_impl<Index, std::make_index_sequence<Size>, ValueList<Names...>>::Type;
};
template <size_t Index, class T>
using TupleElementName_t = TupleElementName<Index, T>::Type;

template <class ...Tuples>
struct NamedTupleCat;
template <>
struct NamedTupleCat<>
{
	using Type = NamedTuple<>;
};
template <named_tuple Tuple>
struct NamedTupleCat<Tuple>
{
	using Type = Tuple;
};
template <StaticChar ...Names1, class ...Ts1, StaticChar ...Names2, class ...Ts2>
struct NamedTupleCat<NamedTuple<Named<Names1, Ts1>...>, NamedTuple<Named<Names2, Ts2>...>>
{
	using Type = NamedTuple<Named<Names1, Ts1>..., Named<Names2, Ts2>...>;
};
template <named_tuple TupleHead, named_tuple ...Tuples>
	requires (sizeof...(Tuples) >= 2)
struct NamedTupleCat<TupleHead, Tuples...>
{
	using Type = NamedTupleCat<TupleHead, typename NamedTupleCat<Tuples...>::Type>::Type;
};
template <named_tuple ...Tuples>
using NamedTupleCat_t = typename NamedTupleCat<Tuples...>::Type;

}

#endif
