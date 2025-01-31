#ifndef ADAPT_KEYWORD_ARGS_H
#define ADAPT_KEYWORD_ARGS_H

#include <tuple>
#include <OpenADAPT/Utility/TypeTraits.h>
#include <OpenADAPT/Utility/Utility.h>
#include <OpenADAPT/Utility/Macro.h>

namespace adapt
{

namespace detail
{
template <class> struct AlwaysTrue {};
}
template <template <class> class Concept = detail::AlwaysTrue>
class AnyTypeKeyword {};

template <class Name_, class Type_, class Tag_>
struct KeywordValue
{
	using Name = Name_;
	using Type = Type_;
	using Tag = Tag_;

	constexpr KeywordValue(Type v) : m_value(std::forward<Type>(v)) {}
	constexpr Type GetValue() { return std::forward<Type>(m_value); }
	template <class T>
	constexpr bool Is() const { return std::is_same<T, Type>::value; }
private:
	Type m_value;
};

template <class Name, class Type, class Tag>
struct KeywordName;
template <class Name_, class Type_, class Tag_>
struct KeywordName
{
	using Name = Name_;
	using Type = Type_;
	using Tag = Tag_;
	using Value = KeywordValue<Name, Type, Tag>;

	constexpr KeywordName() {}
	constexpr Value operator=(Type v) const { return Value(std::forward<Type>(v)); }
};
template <class Name_, template <class> class Concept_, class Tag_>
struct KeywordName<Name_, AnyTypeKeyword<Concept_>, Tag_>
{
	using Name = Name_;
	using Tag = Tag_;

	constexpr KeywordName() {}
	template <class Type, class = Concept_<Type>>
	constexpr KeywordValue<Name, Type, Tag> operator=(Type&& v) const
	{
		return KeywordValue<Name, Type, Tag>(std::forward<Type>(v));
	}
};
template <class Name_, class Tag_>
struct KeywordName<Name_, bool, Tag_>
{
	using Name = Name_;
	using Type = bool;
	using Tag = Tag_;
	using Value = KeywordValue<Name, bool, Tag>;

	//キーワード名インスタンスのみが与えられている場合、trueとして扱う。
	static constexpr bool GetValue() { return true; }
	constexpr Value operator=(bool v) const { return Value(v); }
};

template <class Name>
concept keyword_name = derived_from_xt<std::remove_cvref_t<Name>, KeywordName>;

template <class Option>
concept keyword_value = derived_from_xt<std::remove_cvref_t<Option>, KeywordValue>;
template <class Option>
concept keyword_name_of_bool = keyword_name<Option> && std::same_as<typename std::remove_cvref_t<Option>::Type, bool>;

template <class Option>
concept keyword_arg = keyword_value<Option> || keyword_name_of_bool<Option>;

template <class Option, class ...Tags>
concept keyword_arg_tagged_with = keyword_arg<Option> && (std::same_as<typename std::remove_cvref_t<Option>::Tag, std::remove_cvref_t<Tags>> || ...);

template <class Option, class KeywordName>
concept keyword_arg_named = keyword_name<KeywordName> && keyword_arg<Option> &&
std::same_as<typename std::remove_cvref_t<KeywordName>::Name, typename std::remove_cvref_t<Option>::Name>;

namespace detail
{

template <keyword_name KeywordName>
constexpr bool KeywordExists_impl(KeywordName) { return false; }
template <keyword_name KeywordName, keyword_arg Arg, keyword_arg ...Args>
constexpr bool KeywordExists_impl(KeywordName, Arg&&, [[maybe_unused]] Args&& ...args)
{
	if constexpr (keyword_arg_named<Arg, KeywordName>) return true;
	else return KeywordExists_impl(KeywordName{}, std::forward<Args>(args)...);
}

template <keyword_name KeywordName, class Default>
constexpr decltype(auto) GetKeywordArg_impl(KeywordName, [[maybe_unused]] Default&& dflt)
{
	if constexpr (std::same_as<std::remove_cvref_t<Default>, EmptyClass>) throw std::exception("Default value does not exist.");
	else return std::forward<Default>(dflt);
}
template <keyword_name KeywordName, class Default, keyword_arg Arg, keyword_arg ...Args>
constexpr decltype(auto) GetKeywordArg_impl(KeywordName name, [[maybe_unused]] Default&& dflt, [[maybe_unused]] Arg&& arg, [[maybe_unused]] Args&& ...args)
{
	if constexpr (keyword_arg_named<Arg, KeywordName>) return static_cast<std::remove_cvref_t<Arg>::Type>(arg.GetValue());
	else return GetKeywordArg_impl(name, std::forward<Default>(dflt), std::forward<Args>(args)...);
}

}

template <keyword_name KeywordName, keyword_arg ...Args>
constexpr bool KeywordExists(KeywordName name, Args&& ...args)
{
	return detail::KeywordExists_impl(name, std::forward<Args>(args)...);
}

//該当するキーワードから値を取り出して返す。
//同じキーワードが複数与えられている場合、先のもの（左にあるもの）が優先される。
template <keyword_name KeywordName, keyword_arg ...Args>
constexpr decltype(auto) GetKeywordArg(KeywordName name, Args&& ...args)// -> typename KeywordName::Type
{
	return detail::GetKeywordArg_impl(name, EmptyClass{}, std::forward<Args>(args)...);
}
template <keyword_name KeywordName, keyword_arg ...Args>
constexpr decltype(auto) GetKeywordArg(KeywordName name, std::tuple<Args...> args)
{
	std::apply([&name](auto&& ...args) { return GetKeywordArg(name, std::forward<decltype(args)>(args)...); }, args);
}
template <keyword_name KeywordName, class Default, keyword_arg ...Args> requires (!keyword_arg<std::decay_t<Default>>)
constexpr decltype(auto) GetKeywordArg(KeywordName k, Default&& default_, Args&& ...args)// -> typename KeywordName::Type
{
	//該当するキーワードから値を取り出して返す。
	//同じキーワードが複数与えられている場合、先のもの（左にあるもの）が優先される。
	return detail::GetKeywordArg_impl(k, std::forward<Default>(default_), std::forward<Args>(args)...);
}

template <keyword_name Name, keyword_arg ...Options>
struct GetKeywordType
{
	using Type = decltype(GetKeywordArg(std::declval<Name>(), std::declval<Options>()...));
};

}

#define ADAPT_DEFINE_KEYWORD_OPTION(NAME)\
constexpr auto NAME = adapt::KeywordName<struct _##NAME, bool, void>();

#define ADAPT_DEFINE_TAGGED_KEYWORD_OPTION(NAME, TAG)\
constexpr auto NAME = adapt::KeywordName<struct _##NAME, bool, TAG>();

#define ADAPT_DEFINE_KEYWORD_OPTION_WITH_VALUE(NAME, TYPE)\
constexpr auto NAME = adapt::KeywordName<struct _##NAME, TYPE, void>();

#define ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(NAME, TYPE, TAG)\
constexpr auto NAME = adapt::KeywordName<struct _##NAME, TYPE, TAG>();

#endif