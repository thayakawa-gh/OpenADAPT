#ifndef ADAPT_FUNCTION_H
#define ADAPT_FUNCTION_H

#include <cstdlib>
#include <concepts>
#include <string_view>
#include <array>
#include <OpenADAPT/Utility/Exception.h>
#include <OpenADAPT/Utility/TypeTraits.h>

namespace adapt
{

inline void* AlignedAlloc(size_t size, size_t align)
{
#ifdef _MSC_VER
	return _aligned_malloc(size, align);
#else
	return std::aligned_alloc(size, align);
#endif
}
inline void FreeAligned(void* ptr)
{
#ifdef _MSC_VER
	return _aligned_free(ptr);
#else
	return std::free(ptr);
#endif
}

inline constexpr bool IsDigit(char c) { return c <= '9' && c >= '0'; }

//+-数字以外の文字が含まれてはいけない。空白も駄目。
template <std::integral T>
constexpr T StrTo(std::string_view str, T base = 10)
{
	using namespace std::literals;
	T res = 0;
	bool minus = false;
	auto it = str.begin();
	if (*it == '+') minus = false, ++it;
	else if (*it == '-') minus = true, ++it;
	for (; it != str.end(); ++it)
	{
		if (!IsDigit(*it)) throw InvalidArg("");
		T num = *it - '0';
		if (num >= base) throw InvalidArg("");
		res = res * base + num;
	}
	return minus ? -res : res;
}

template <class Num>
	requires std::is_arithmetic_v<Num>
void ToStr(Num v, std::string& res)
{
	auto func = [&res](const char* fmt, auto v)
	{
		size_t len = snprintf(nullptr, 0, fmt, v);
		res.resize(len);
		snprintf(&res[0], len + 1, fmt, v);
	};
	if constexpr (std::is_same_v<Num, unsigned int>) func("%u", v);
	else if constexpr (std::is_same_v<Num, long>) func("%ld", v);
	else if constexpr (std::is_same_v<Num, unsigned long>) func("%lu", v);
	else if constexpr (std::is_same_v<Num, long long>) func("%lld", v);
	else if constexpr (std::is_same_v<Num, unsigned long long>) func("%llu", v);
	else if constexpr (std::is_same_v<Num, float>) func("%f", v);
	else if constexpr (std::is_same_v<Num, double>) func("%f", v);
	else if constexpr (std::is_same_v<Num, long double>) func("%Lf", v);
	else if constexpr (std::is_same_v<Num, bool>) { v ? res = "true" : res = "false"; }
	//上に該当しない整数は全てintまたはunsigned intと見做す。
	else if constexpr (std::is_signed_v<Num>) func("%d", (int)v);
	else if constexpr (std::is_unsigned_v<Num>) func("%d", (unsigned int)v);
}

inline std::string ReplaceStr(std::string_view str, std::string_view from, std::string_view to)
{
	std::string res(str);
	std::string::size_type pos = 0;
	while (pos = res.find(from, pos), pos != std::string::npos)
	{
		res.replace(pos, from.length(), to);
		pos += to.length();
	}
	return res;
}

namespace detail
{

template <class T>
struct GetArraySize_impl;
template <class Type, size_t N>
struct GetArraySize_impl<std::array<Type, N>> { static constexpr size_t Size = N; };

template <class Array>
constexpr size_t GetArraySize() { return GetArraySize_impl<std::decay_t<Array>>::Size; }

template <bool Rval>
struct ForwardArrayElm_impl
{
	template <class Type>
	static constexpr std::remove_reference_t<Type>& apply(Type&& t) { return static_cast<std::remove_reference_t<Type>&>(t); }
};
template <>
struct ForwardArrayElm_impl<true>
{
	template <class Type>
	static constexpr std::remove_reference_t<Type>&& apply(Type&& t) { return static_cast<std::remove_reference_t<Type>&&>(t); }
};

template <bool Rval, class Type>
constexpr decltype(auto) ForwardArrayElm(Type&& v) { return ForwardArrayElm_impl<Rval>::apply(std::forward<Type>(v)); }

template <any_array Array1, size_t ...Indices1, class Array2, size_t ...Indices2>
constexpr auto CatArray_impl(Array1&& a, std::index_sequence<Indices1...>,
							 Array2&& b, std::index_sequence<Indices2...>)
{
	using Type = typename std::decay_t<Array1>::value_type;
	static_assert(std::is_same<Type, typename std::decay_t<Array2>::value_type>::value, "");
	constexpr size_t N1 = detail::GetArraySize<Array1>();
	constexpr size_t N2 = detail::GetArraySize<Array2>();
	constexpr bool L1 = std::is_lvalue_reference<Array1>::value;
	constexpr bool L2 = std::is_lvalue_reference<Array2>::value;
	return std::array<Type, N1 + N2>{ ForwardArrayElm<!L1>(a[Indices1])..., ForwardArrayElm<!L2>(b[Indices2])... };
}
}

template <any_array Array>
constexpr Array CatArray(Array&& a)
{
	return a;
}
template <any_array Array1, any_array Array2>
constexpr auto CatArray(Array1&& a, Array2&& b)
{
	return detail::CatArray_impl(std::forward<Array1>(a), std::make_index_sequence<detail::GetArraySize<Array1>()>(),
								 std::forward<Array2>(b), std::make_index_sequence<detail::GetArraySize<Array2>()>());
}

namespace detail
{
template <any_array Array1, any_array Array2>
constexpr auto CatArray_rec(Array1&& a, Array2&& b)
{
	return CatArray(std::forward<Array1>(a), std::forward<Array2>(b));
}
template <any_array Array1, any_array Array2, any_array Array3, any_array ...Arrays>
constexpr auto CatArray_rec(Array1&& a, Array2&& b, Array3&& c, Arrays&& ...as)
{
	return CatArray_rec(CatArray(std::forward<Array1>(a), std::forward<Array2>(b)), std::forward<Array3>(c), std::forward<Arrays>(as)...);
}
}
template <any_array ...Array>
constexpr auto CatArray(Array&& ...a)
{
	return detail::CatArray_rec(std::forward<Array>(a)...);
}
template <class ...Args>
constexpr auto MakeArray(Args&& ...args) ->
std::array<
	typename std::decay_t<
	typename std::common_type_t<Args...>>,
	sizeof...(Args)>
{
	return std::array<
		typename std::decay<
		typename std::common_type<Args...>::type>::type,
		sizeof...(Args)>{ std::forward<Args>(args)... };
}

namespace detail
{
template <size_t N>
struct GetFormerNArgs_impl
{
	template <class Head, class ...Args>
	static constexpr auto apply(Head&& head, Args&& ...args)
	{
		return std::tuple_cat(std::forward_as_tuple(std::forward<Head>(head)), GetFormerNArgs_impl<N - 1>::apply(std::forward<Args>(args)...));
	}
};
template <>
struct GetFormerNArgs_impl<0>
{
	template <class ...Args>
	static constexpr auto apply(Args&& ...)
	{
		return std::tuple<>();
	}
};
}
template <size_t Size, class ...Args>
static constexpr auto GetFormerNArgs(Args&& ...args)
{
	return detail::GetFormerNArgs_impl<Size>::apply(std::forward<Args>(args)...);
}
/*
namespace detail
{
template <class T, std::size_t N>
class ReferenceArray
{
public:

	template <class ...Args,
		std::enable_if_t<(sizeof...(Args) != 1), std::nullptr_t> = nullptr>
	ReferenceArray(Args&& ...args) : m_ptr_array{ &std::forward<Args>(args)... } {}
	template <class Arg,
		std::enable_if_t<!std::is_same_v<std::decay_t<Arg>, ReferenceArray>, std::nullptr_t> = nullptr>
	ReferenceArray(Arg&& arg) : m_ptr_array{ &std::forward<Arg>(arg) } {}

	ReferenceArray(const ReferenceArray& arg) = default;

	class Iterator
	{
	public:
		using difference_type = ptrdiff_t;
		using value_type = T;
		using pointer = T*;
		using reference = T&;
		using iterator_category = std::forward_iterator_tag;

		Iterator(typename std::array<std::add_pointer_t<T>, N>::iterator i) : m_iter(i) {}

		Iterator& operator++()
		{
			++m_iter;
			return *this;
		}
		T& operator*() const noexcept { return *(*m_iter); }
		bool operator==(const Iterator& it2) const { return m_iter == it2.m_iter; }
		bool operator!=(const Iterator& it2) const { return !(*this == it2); }

	private:
		typename std::array<std::add_pointer_t<T>, N>::iterator m_iter;
	};
	class ConstIterator
	{
	public:
		using difference_type = ptrdiff_t;
		using value_type = T;
		using pointer = const T*;
		using reference = const T&;
		using iterator_category = std::forward_iterator_tag;

		ConstIterator(typename std::array<std::add_pointer_t<T>, N>::const_iterator i) : m_iter(i) {}

		ConstIterator& operator++()
		{
			++m_iter;
			return *this;
		}
		const T& operator*() const noexcept { return *(*m_iter); }
		bool operator==(const ConstIterator& it2) const { return m_iter == it2.m_iter; }
		bool operator!=(const ConstIterator& it2) const { return !(*this == it2); }

	private:
		typename std::array<std::add_pointer_t<T>, N>::const_iterator m_iter;
	};

	constexpr std::size_t size() const { return N; }
	Iterator begin() { return Iterator(m_ptr_array.begin()); }
	ConstIterator begin() const { return ConstIterator(m_ptr_array.begin()); }
	Iterator end() { return Iterator(m_ptr_array.end()); }
	ConstIterator end() const { return ConstIterator(m_ptr_array.end()); }

private:

	std::array<std::add_pointer_t<T>, N> m_ptr_array;
};
}
template <class ...Args>
//	requires similar_lvalue_reference<Args...>
detail::ReferenceArray<CommonRef_t<Args...>, sizeof...(Args)> HoldRefArray(Args& ...args)
{
	return detail::ReferenceArray<CommonRef_t<Args...>, sizeof...(Args)>{ args... };
}
inline detail::ReferenceArray<std::nullptr_t, 0> HoldRefArray()
{
	return detail::ReferenceArray<std::nullptr_t, 0>();
}
*/

//rvalueは値に、lvalueは参照のままでtupleにする。
template <class ...Args>
auto MakeDecayedTuple(Args&& ...args)
{
	return std::tuple<typename DecayRRef<Args>::Type...>(std::forward<Args>(args)...);
}

template <class ...Args>
void DoNothing(Args&& ...) {}

}

#endif
