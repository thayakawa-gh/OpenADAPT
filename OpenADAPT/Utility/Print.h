#ifndef ADAPT_UTILITY_PRINT_H
#define ADAPT_UTILITY_PRINT_H

#include <iostream>

namespace adapt
{

namespace print
{

namespace detail
{

constexpr size_t GetCharLen(int del)
{
	return (char)(del >> 8) == '\0' ? 1 : (char)(del >> 16) == '\0' ? 2 : (char)(del >> 24) == '\0' ? 3 : 4;
}
template <int ...Char>
struct Character
{
	template <size_t N>
	static constexpr int GetChar() { return std::get<N>(std::make_tuple(Char...)); }
	static constexpr auto Get()
	{
		return CatArray(Get_impl<Char>()...);
	}
private:
	template <int C>
	static constexpr auto Get_impl()
	{
		return Get_impl<C>(std::make_index_sequence<GetCharLen(C)>());
	}
	template <int C, size_t ...Indices>
	static constexpr auto Get_impl(std::index_sequence<Indices...>)
	{
		return std::array<char, GetCharLen(C)>{ char(C >> (8 * (GetCharLen(C) - Indices - 1)))... };
	}
};
template <>
struct Character<>
{
	static constexpr auto Get()
	{
		return std::array<char, 0>();
	}
};
template <int ...Del> struct Delimiter : public Character<Del...> {};
template <int ...E> struct End : public Character<E...> {};

template <int B>
struct Flush
{};

template <class Type_>
struct GetFmtSpec_ { static constexpr void apply() {} };
template <class Type>
struct GetFmtSpec_<Type*> { static constexpr auto apply() { return std::array<char, 2>{ '%', 'p' }; } };

#define GET_FMT_SPEC(TYPE, ...)\
template <> constexpr auto GetFmtSpec<TYPE>() { return MakeArray(__VA_ARGS__); }
template <class Type>
constexpr auto GetFmtSpec() { return GetFmtSpec_<Type>::apply(); }
GET_FMT_SPEC(float, '%', 'f');
GET_FMT_SPEC(double, '%', 'l', 'f');
GET_FMT_SPEC(long double, '%', 'l', 'f');
GET_FMT_SPEC(char, '%', 'c');
GET_FMT_SPEC(short, '%', 'h', 'd');
GET_FMT_SPEC(int, '%', 'd');
GET_FMT_SPEC(long, '%', 'l', 'd');
GET_FMT_SPEC(long long, '%', 'l', 'l', 'd');
GET_FMT_SPEC(unsigned short, '%', 'h', 'u');
GET_FMT_SPEC(unsigned int, '%', 'u');
GET_FMT_SPEC(unsigned long, '%', 'l', 'u');
GET_FMT_SPEC(unsigned long long, '%', 'l', 'l', 'u');
GET_FMT_SPEC(const char*, '%', 's');
GET_FMT_SPEC(char*, '%', 's');
GET_FMT_SPEC(std::string, '%', 's');
#undef GET_FMT_SPEC

template <class ...Args>
constexpr auto GetOptions();
template <class Args>
struct GetOptions_impl;
template <>
struct GetOptions_impl<TypeList<>>
{
	static constexpr auto apply()
	{
		return std::make_tuple(Delimiter<' '>(), End<'\n'>(), std::false_type(), std::integral_constant<size_t, 0>());
	}
};
template <class Head, class ...Args>
struct GetOptions_impl<TypeList<Head, Args...>>
{
	static constexpr auto apply()
	{
		constexpr auto t = GetOptions_impl<TypeList<Args...>>::apply();
		return std::make_tuple(std::get<0>(t), std::get<1>(t), std::get<2>(t), std::integral_constant<size_t, std::get<3>(t) + 1>());
	}
};
template <int ...Int, class ...Args>
struct GetOptions_impl<TypeList<Delimiter<Int...>, Args...>>
{
	static constexpr auto apply()
	{
		constexpr auto t = GetOptions_impl<TypeList<Args...>>::apply();
		return std::make_tuple(Delimiter<Int...>(), std::get<1>(t), std::get<2>(t), std::get<3>(t));
	}
};
template <int ...Int, class ...Args>
struct GetOptions_impl<TypeList<End<Int...>, Args...>>
{
	static constexpr auto apply()
	{
		constexpr auto t = GetOptions_impl<TypeList<Args...>>::apply();
		return std::make_tuple(std::get<0>(t), End<Int...>(), std::get<2>(t), std::get<3>(t));
	}
};
template <bool B, class ...Args>
struct GetOptions_impl<TypeList<Flush<B>, Args...>>
{
	static constexpr auto apply()
	{
		constexpr auto t = GetOptions_impl<TypeList<Args...>>::apply();
		return std::make_tuple(std::get<0>(t), std::get<1>(t), std::bool_constant<B>(), std::get<3>(t));
	}
};
template <class ...Args>
constexpr auto GetOptions()
{
	return GetOptions_impl<TypeList<Args...>>::apply();
}

template <class F>
inline void Flush_(FILE* fp) { fflush(fp); }
template <>
inline void Flush_<std::false_type>(FILE*) {}
template <class F>
inline void Flush_(std::ostream& ost) { ost << std::flush; }
template <>
inline void Flush_<std::false_type>(std::ostream&) {}

template <class Del, class End>
constexpr auto MakeFormatStr_rec()
{
	return CatArray(End::Get(), std::array<char, 1>{ '\0' });
}
template <class Del, class End, class Head, class ...Args, std::enable_if_t<!std::is_void<decltype(GetFmtSpec<Head>())>::value, std::nullptr_t> = nullptr>
constexpr auto MakeFormatStr_rec()
{
	return CatArray(Del::Get(), GetFmtSpec<Head>(), MakeFormatStr_rec<Del, End, Args...>());
}

template <class Del, class End, class Args>
struct MakeFormatStr;
template <class Del, class End, class Head, class ...Args>
struct MakeFormatStr<Del, End, TypeList<Head, Args...>>
{
	static constexpr auto apply()
	{
		return CatArray(GetFmtSpec<Head>(), MakeFormatStr_rec<Del, End, Args...>());
	}
};

struct PrintToStream
{
	template <class Del, class End, class Fls, class Head>
	void operator()(std::ostream& ost, Del, End, Fls, Head&& head) const
	{
		ost << std::forward<Head>(head) << CatArray(End::Get(), std::array<char, 1>{ '\0' }).data();
		Flush_<Fls>(ost);
	}
	template <class Del, class End, class Fls, class Head, class ...Args, std::enable_if_t<sizeof...(Args) != 0, std::nullptr_t> = nullptr>
	void operator()(std::ostream& ost, Del d, End e, Fls f, Head&& head, Args&& ...args) const
	{
		ost << std::forward<Head>(head) << CatArray(Del::Get(), std::array<char, 1>{ '\0' }).data();
		return (*this)(ost, d, e, f, std::forward<Args>(args)...);
	}
};

template <class Type, std::enable_if_t<!std::is_same<std::decay_t<Type>, std::string>::value, std::nullptr_t> = nullptr>
Type&& ConvStringToCharPtr(Type&& v) { return std::forward<Type>(v); }
template <class Type, std::enable_if_t<std::is_same<std::decay_t<Type>, std::string>::value, std::nullptr_t> = nullptr>
const char* ConvStringToCharPtr(Type&& v) { return v.c_str(); }

}

template <int ...N>
inline constexpr detail::Delimiter<N...> delim = detail::Delimiter<N...>();
template <int ...N>
inline constexpr detail::End<N...> end = detail::End<N...>();
template <bool B>
inline constexpr detail::Flush<B> flush = detail::Flush<B>();

}

template <class ...Args>
void Print(FILE* fp, Args&& ...args)
{
	constexpr auto t = print::detail::GetOptions<std::decay_t<Args>...>();
	constexpr auto d = std::get<0>(t);
	constexpr auto e = std::get<1>(t);
	constexpr auto f = std::get<2>(t);
	constexpr size_t n = std::get<3>(t).value;
	constexpr auto fmt = print::detail::MakeFormatStr<decltype(d), decltype(e), GetFormerNTypes_t<n, std::decay_t<Args>...>>::apply();
	std::apply(&fprintf, std::tuple_cat(std::make_tuple(fp), std::make_tuple(fmt.data()), GetFormerNArgs<n>(print::detail::ConvStringToCharPtr(std::forward<Args>(args))...)));
	print::detail::Flush_<decltype(f)>(fp);
}
template <class ...Args>
void Print(char* str, Args&& ...args)
{
	constexpr auto t = print::detail::GetOptions<std::decay_t<Args>...>();
	constexpr auto d = std::get<0>(t);
	constexpr auto e = std::get<1>(t);
	constexpr auto f = std::get<2>(t);
	constexpr size_t n = std::get<3>(t).value;
	constexpr auto fmt = print::detail::MakeFormatStr<decltype(d), decltype(e), GetFormerNTypes_t<n, std::decay_t<Args>...>>::apply();
	std::apply(&sprintf, std::tuple_cat(std::make_tuple(str), std::make_tuple(fmt.data()), GetFormerNArgs<n>(print::detail::ConvStringToCharPtr(std::forward<Args>(args))...)));
}
template <class ...Args>
void Print(std::ostream& ost, Args&& ...args)
{
	constexpr auto t = print::detail::GetOptions<std::decay_t<Args>...>();
	constexpr auto d = std::get<0>(t);
	constexpr auto e = std::get<1>(t);
	constexpr auto f = std::get<2>(t).value;
	constexpr auto n = std::get<3>(t).value;
	std::apply(print::detail::PrintToStream(), std::tuple_cat(std::forward_as_tuple(ost, d, e, f), GetFormerNArgs<n>(std::forward<Args>(args)...)));
}


}


#endif