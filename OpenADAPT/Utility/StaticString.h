#ifndef ADAPT_STATICSTRING_H
#define ADAPT_STATICSTRING_H

#include <string_view>
#include <array>
#include <utility>
#include <OpenADAPT/Utility/Utility.h>

namespace adapt
{

template <size_t N>
struct StaticChar
{
	constexpr StaticChar() { m_char[0] = '\0'; }
	constexpr StaticChar(const char(&s)[N])
	{
		for (size_t i = 0; i < N; ++i) m_char[i] = s[i];
	}
	constexpr StaticChar(std::array<char, N> s)
	{
		for (size_t i = 0; i < N; ++i) m_char[i] = s[i];
	}
	template <size_t L, size_t M>
		requires (L + M == N + 1)//L側の末尾の'\0'を除く必要がある。
	constexpr StaticChar(const char(&s)[L], const char(&t)[M])
	{
		for (size_t i = 0; i < L - 1; ++i) m_char[i] = s[i];
		for (size_t i = 0; i < M; ++i) m_char[i + L - 1] = t[i];
	}
	template <size_t M>
	constexpr bool operator==(const StaticChar<M>& that) const
	{
		if constexpr (N != M) return false;
		else
		{
			for (size_t i = 0; i < N; ++i) if (m_char[i] != that.m_char[i]) return false;
			return true;
		}
	}
	static constexpr size_t GetSize() { return N; }
	constexpr const char* GetChar() const { return m_char; }
	char m_char[N];
};

template <StaticChar Chars_>
struct StaticString
{
	static constexpr StaticChar Chars = Chars_;
	static constexpr size_t Size = Chars.GetSize();

	static constexpr const char* GetChar() { return Chars.GetChar(); }
	constexpr operator const char* () const { return GetChar(); }
	constexpr operator std::string_view() const { return std::string_view(GetChar()); }

	template <StaticChar Char>
	constexpr auto operator+(StaticString<Char> that) const
	{
		constexpr StaticChar<Size + that.Size - 1> res(Chars.m_char, that.Chars.m_char);
		return StaticString<res>{};
	}
};

namespace detail
{
template <class T>
constexpr size_t GetNumOfDigits(T num)
{
	size_t count = std::cmp_less(num, 0) ? 1 : 0;
	T i = num;
	while (i != 0)
	{
		i /= 10;
		++count;
	}
	return count;
}
template <size_t ND, class T>
constexpr std::array<char, ND + 1> ToString_impl(T num)
{
	std::array<char, ND + 1> res;
	res[ND] = '\0';
	T i = num;
	if constexpr (std::is_signed_v<T>) i = std::cmp_less(num, 0) ? -num : num;
	size_t pos = ND - 1;
	while (i != 0)
	{
		res[pos] = '0' + char(i % 10);
		--pos;
		i /= 10;
	}
	if (std::cmp_less(num, 0)) res[0] = '-';
	return res;
};
}
template <auto Num>
	requires std::integral<decltype(Num)>
consteval auto ToStr(Number<Num>)
{
	if constexpr (Num == 0)
	{
		//0のときはループに入ることが出来ないので例外的に扱う。
		return StaticString<"0">{};
	}
	else
	{
		constexpr size_t nd = detail::GetNumOfDigits(Num);
		constexpr std::array<char, nd + 1> str = detail::ToString_impl<nd>(Num);
		return StaticString<StaticChar<nd + 1>(str)>{};
	}
}

}

#endif