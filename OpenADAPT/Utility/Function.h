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
