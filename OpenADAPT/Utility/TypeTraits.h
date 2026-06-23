#ifndef ADAPT_CUF_TYPE_TRAITS_H
#define ADAPT_CUF_TYPE_TRAITS_H

#include <tuple>
#include <type_traits>
#include <utility>
#include <complex>
#include <format>
#include <OpenADAPT/Utility/Common.h>

namespace adapt
{

//各種演算子を使えるようにしたintegral_constant。
ADAPT_EXPORT
template <class Type, Type N, template <Type> class Derived>
struct IntegralConstant : std::integral_constant<Type, N>
{
	constexpr auto operator+() const requires std::is_signed_v<Type> { return Derived<Type(+N)>(); }
	constexpr auto operator-() const requires std::is_signed_v<Type> { return Derived<Type(-N)>(); }

	template <Type X>
	constexpr auto operator+(Derived<X>) const { return Derived<Type(N + X)>{}; }
	template <Type X>
	constexpr auto operator-(Derived<X>) const { return Derived<Type(N - X)>{}; }

	constexpr Type operator+(Type x) const { return Type(N + x); }
	constexpr Type operator-(Type x) const { return Type(N - x); }

	template <Type X>
	constexpr auto operator==(Derived<X>) const { return std::bool_constant<(N == X)>{}; }
	template <Type X>
	constexpr auto operator!=(Derived<X>) const { return std::bool_constant<(N != X)>{}; }
	template <Type X>
	constexpr auto operator>(Derived<X>) const { return std::bool_constant<(N > X)>{}; }
	template <Type X>
	constexpr auto operator<(Derived<X>) const { return std::bool_constant<(N < X)>{}; }
	template <Type X>
	constexpr auto operator>=(Derived<X>) const { return std::bool_constant<(N >= X)>{}; }
	template <Type X>
	constexpr auto operator<=(Derived<X>) const { return std::bool_constant<(N <= X)>{}; }
};

template <class Type, Type N, template <Type> class Derived>
constexpr Type operator+(Type x, IntegralConstant<Type, N, Derived>) { return Type(x + N); }
template <class Type, Type N, template <Type> class Derived>
constexpr Type operator-(Type x, IntegralConstant<Type, N, Derived>) { return Type(x - N); }

ADAPT_EXPORT
template <class ...T>
struct TypeList
{
	static constexpr size_t size = sizeof...(T);
};
ADAPT_EXPORT
template <class ...T>
struct CatTypeList;
template <class ...T1>
struct CatTypeList<TypeList<T1...>>
{
	using Type = TypeList<T1...>;
};
template <class ...T1, class ...T2>
struct CatTypeList<TypeList<T1...>, TypeList<T2...>>
{
	using Type = TypeList<T1..., T2...>;
};
template <class T1, class T2, class T3, class ...Ts>
struct CatTypeList<T1, T2, T3, Ts...>
	: public CatTypeList<typename CatTypeList<T1, T2>::Type, T3, Ts...>
{
};
ADAPT_EXPORT
template <class ...T>
using CatTypeList_t = typename CatTypeList<T...>::Type;


ADAPT_EXPORT
template <template <class...> class ...T>
struct UnarguedList
{
	static constexpr size_t size = sizeof...(T);
};
ADAPT_EXPORT
template <auto ...V>
struct ValueList
{
	static constexpr size_t size = sizeof...(V);
};


ADAPT_EXPORT
template <auto N, class T>
struct PseudoIndexed { using Type = T; };
ADAPT_EXPORT
template <auto N, class T>
using PseudoIndexed_t = PseudoIndexed<N, T>::Type;

ADAPT_EXPORT
template <class X, class T>
struct PseudoArgued { using Type = T; };
ADAPT_EXPORT
template <class X, class T>
using PseudoArgued_t = PseudoArgued<X, T>::Type;

namespace detail
{
template <size_t Index, class Type>
struct GetType_impl_s
{
	template <class Type_>
	friend Type_ GetTypeResult(const GetType_impl_s<Index, Type_>&, std::integral_constant<size_t, Index>);
};
template <class, class>
struct GetType_impl;
template <size_t ...Indices, class ...Types>
struct GetType_impl<std::index_sequence<Indices...>, TypeList<Types...>>
	: public GetType_impl_s<Indices, Types>...
{};
}

ADAPT_EXPORT
template <size_t N, class ...Types>
	requires (N < sizeof...(Types))
struct GetType
{
	using Type = decltype(GetTypeResult(detail::GetType_impl<
										std::make_index_sequence<sizeof...(Types)>,
										TypeList<Types...>>(),
										std::integral_constant<size_t, N>()));
};
template <size_t N, class ...Types>
struct GetType<N, TypeList<Types...>>
	: public GetType<N, Types...>
{
};
ADAPT_EXPORT
template <size_t N, class ...Args>
using GetType_t = typename GetType<N, Args...>::Type;

namespace detail
{
template <size_t Index, class Type>
struct FindType_impl_s
{
	friend std::integral_constant<size_t, Index> FindResult(const FindType_impl_s<Index, Type>&, std::type_identity_t<Type>) {}
};
template <class, class>
struct FindType_impl;
template <size_t ...Indices, class ...Types>
struct FindType_impl<std::index_sequence<Indices...>, TypeList<Types...>>
	: public FindType_impl_s<Indices, Types>...
{};
}
ADAPT_EXPORT
template <class Type, class ...Types>
struct FindType
{
private:
	using Impl = detail::FindType_impl<std::make_index_sequence<sizeof...(Types)>, TypeList<Types...>>;
	struct Exist
	{
		template <class T = Type>
		static constexpr auto Check(int) -> decltype(FindResult(Impl(), std::type_identity_t<T>()), bool()) { return true; }
		static constexpr auto Check(...) -> bool { return false; }
		template <class T = Type>
		static constexpr auto GetIndex(int) -> decltype(FindResult(Impl(), std::type_identity_t<T>()), size_t())
		{
			using IC = decltype(FindResult(Impl(), std::type_identity_t<Type>()));
			return IC::value;
		}
		static constexpr auto GetIndex(...) -> size_t { return std::numeric_limits<size_t>::max(); }
		static constexpr bool value = Check(0);
		static constexpr size_t Index = GetIndex(0);
	};
public:

	static constexpr size_t Index = Exist::Index;
	static constexpr bool value = Exist::value;
};

ADAPT_EXPORT
template <class Type, class ...Types>
inline constexpr size_t FindType_v = FindType<Type, Types...>::Index;

ADAPT_EXPORT
template <class Type, class ...Types>
using FirstType = Type;

ADAPT_EXPORT
template <size_t Index, class ...Types>
struct GetFormerNTypes;
template <>
struct GetFormerNTypes<0>
{
	using Type = TypeList<>;
};
template <class Head, class ...Types>
struct GetFormerNTypes<0, Head, Types...>
{
	using Type = TypeList<>;
};
template <size_t Index, class Head, class ...Types>
struct GetFormerNTypes<Index, Head, Types...>
{
	using Type = CatTypeList_t<TypeList<Head>, typename GetFormerNTypes<Index - 1, Types...>::Type>;
};
ADAPT_EXPORT
template <size_t Index, class ...Types>
using GetFormerNTypes_t = typename GetFormerNTypes<Index, Types...>::Type;

ADAPT_EXPORT
template <class A, class B>
using Former = A;
ADAPT_EXPORT
template <class A, class B>
using Latter = B;

ADAPT_EXPORT
template <size_t Index, auto ...Values>
constexpr auto GetValue()
{
	constexpr auto tup = std::make_tuple(Values...);
	return std::get<Index>(tup);
}

ADAPT_EXPORT
template <template <class...> class Base, class Derived>
struct IsBaseOfTemplate
{
	template <class ...U>
	static constexpr std::true_type check(const Base<U...>*);
	static constexpr std::false_type check(const void*);

	static const Derived* d;
public:
	static constexpr bool value = decltype(check(d))::value;
};

ADAPT_EXPORT
template <template <class...> class Base, class Derived>
inline constexpr bool IsBaseOfTemplate_v = IsBaseOfTemplate<Base, Derived>::value;

ADAPT_EXPORT
template <template <auto...> class Base, class Derived>
struct IsBaseOfValueTemplate
{
	template <auto ...U>
	static constexpr std::true_type check(const Base<U...>*);
	static constexpr std::false_type check(const void*);

	static const Derived* d;
public:
	static constexpr bool value = decltype(check(d))::value;
};

ADAPT_EXPORT
template <template <auto...> class Base, class Derived>
inline constexpr bool IsBaseOfValueTemplate_v = IsBaseOfValueTemplate<Base, Derived>::value;

ADAPT_EXPORT
template <template <class ...> class T, class U>
struct IsSpecializationOf : public std::false_type {};
template <template <class ...> class T, class ...X>
struct IsSpecializationOf<T, T<X...>> : public std::true_type {};

ADAPT_EXPORT
template <template <class ...> class T, class U>
inline constexpr bool IsSpecializationOf_v = IsSpecializationOf<T, U>::value;

ADAPT_EXPORT
template <template <auto ...> class T, class U>
struct IsValueSpecializationOf : public std::false_type {};
template <template <auto ...> class T, auto ...X>
struct IsValueSpecializationOf<T, T<X...>> : public std::true_type {};

ADAPT_EXPORT
template <template <auto ...> class T, class U>
inline constexpr bool IsValueSpecializationOf_v = IsValueSpecializationOf<T, U>::value;

ADAPT_EXPORT
template <template <auto, class...> class Base, class Derived>
struct IsSpecializationOfValueType
{
	template <auto N, class ...U>
	static constexpr std::true_type check(const Base<N, U...>*);
	static constexpr std::false_type check(const void*);

	static const Derived* d;
public:
	static constexpr bool value = decltype(check(d))::value;
};

ADAPT_EXPORT
template <template <auto, class...> class Base, class Derived>
inline constexpr bool IsSpecializationOfValueType_v = IsSpecializationOfValueType<Base, Derived>::value;

ADAPT_EXPORT
template <template <class, auto> class T, class U>
struct IsSpecializationOfTypeValue : public std::false_type {};
template <template <class, auto> class T, class X, auto Y>
struct IsSpecializationOfTypeValue<T, T<X, Y>> : public std::true_type {};

ADAPT_EXPORT
template <template <class, auto> class T, class U>
inline constexpr bool IsSpecializationOfTypeValue_v = IsSpecializationOfTypeValue<T, U>::value;


ADAPT_EXPORT
template <class T>
struct DecayRRef { using Type = T; };
template <class T>
struct DecayRRef<T&&> { using Type = std::decay_t<T>; };

ADAPT_EXPORT
template <auto ...N>
struct ConstantSequence
{
};

ADAPT_EXPORT
template <class T>
struct IsComplex : public std::false_type {};
template <class T>
struct IsComplex<std::complex<T>> : public std::true_type {};

ADAPT_EXPORT
template <class T>
inline constexpr bool IsComplex_v = IsComplex<T>::value;

ADAPT_EXPORT
template <class T, class U>
concept derivative_relation = std::derived_from<T, U> || std::derived_from<U, T>;

ADAPT_EXPORT
template <class T, class U>
concept similar_lvalue_reference =
(std::is_lvalue_reference_v<T> && std::is_lvalue_reference_v<U>) &&
(std::same_as<std::remove_cvref_t<T>, std::remove_cvref_t<U>> ||
 derivative_relation<std::remove_cvref_t<T>, std::remove_cvref_t<U>>);

ADAPT_EXPORT
template <class ...Ts>
struct CommonRef { using Type = void; };
template <class T>
struct CommonRef<T&> { using Type = T&; };
template <class T, class U>
	requires similar_lvalue_reference<T&, U&>
struct CommonRef<T&, U&> : std::true_type
{
private:
	static constexpr bool is_const = std::is_const_v<T> || std::is_const_v<U>;
	using CT = std::common_type_t<T, U>;
public:
	using Type = std::conditional_t<is_const, const CT&, CT&>;
};

//<int&, const int&>のような同じ型
//<Base&, Derived&>のような派生関係
//いずれかのlvalue referenceであるときに、その共通型を取り出す。
//const有無の違いを許す。constが混ざっていたら共通型もconst。
ADAPT_EXPORT
template <class ...T>
using CommonRef_t = CommonRef<T...>::Type;

ADAPT_EXPORT
template <class T>
concept non_void = !std::same_as<T, void>;

ADAPT_EXPORT
template <class T, template <class...> class U>
concept derived_from_template = IsBaseOfTemplate<U, T>::value;

ADAPT_EXPORT
template <class T, template <auto...> class U>
concept derived_from_value_template = IsBaseOfValueTemplate<U, T>::value;

ADAPT_EXPORT
template <class T, template <auto, class...> class U>
concept derived_from_value_type_template = IsSpecializationOfValueType<U, T>::value;

ADAPT_EXPORT
template <class T, template <class...> class U>
concept specialization_of = IsSpecializationOf<U, T>::value;

ADAPT_EXPORT
template <class T, template <auto...> class U>
concept value_specialization_of = IsValueSpecializationOf<U, T>::value;

ADAPT_EXPORT
template <class T, template <class, auto> class U>
concept same_as_type_value_template = IsSpecializationOfTypeValue<U, T>::value;

ADAPT_EXPORT
template <class T, template <auto, class> class U>
concept same_as_value_type_template = IsSpecializationOfValueType<U, T>::value;
//template <class T, template <auto...> class U>
//concept not_value_specialization_of = !IsValueSpecializationOf<U, T>::value;


ADAPT_EXPORT
template <class T, class U>
concept similar_to = std::same_as<std::decay_t<T>, U>;
ADAPT_EXPORT
template <class T, template <class...> class U>
concept similar_to_template = specialization_of<std::decay_t<T>, U>;
ADAPT_EXPORT
template <class T, template <auto...> class U>
concept similar_to_value_template = value_specialization_of<std::decay_t<T>, U>;


ADAPT_EXPORT
template <class T>
struct IsTuple : public std::false_type {};
template <class ...T>
struct IsTuple<std::tuple<T...>> : public std::true_type {};
ADAPT_EXPORT
template <class T>
concept any_tuple = IsTuple<std::remove_cvref_t<T>>::value;

ADAPT_EXPORT
template <class T>
struct IsArray : public std::false_type {};
template <class T, size_t N>
struct IsArray<std::array<T, N>> : public std::true_type {};
ADAPT_EXPORT
template <class T>
concept any_array = IsArray<std::remove_cvref_t<T>>::value;

ADAPT_EXPORT
template <class ArgType>
concept boolean_testable = requires(ArgType a)
{
	static_cast<bool>(a);
};
ADAPT_EXPORT
template <class ArgType>
concept summable = requires(ArgType a, ArgType b)
{
	a += b;
};
ADAPT_EXPORT
template <class ArgType>
concept dividable = requires(ArgType a, ArgType b)
{
	a /= b;
};
ADAPT_EXPORT
template <class ArgType>
concept greater_than_comparable = requires(ArgType a, ArgType b)
{
	{ a > b } -> boolean_testable;
};
ADAPT_EXPORT
template <class T, class U>
concept greater_than_comparable_with = requires(T a, U b)
{
	{ a > b } -> boolean_testable;
};
ADAPT_EXPORT
template <class ArgType>
concept less_than_comparable = requires(ArgType a, ArgType b)
{
	{ a < b } -> boolean_testable;
};
ADAPT_EXPORT
template <class T, class U>
concept less_than_comparable_with = requires(T a, U b)
{
	{ a < b } -> boolean_testable;
};
ADAPT_EXPORT
template <class ArgType>
concept equal_comparable = requires(ArgType a, ArgType b)
{
	{ a == b } -> boolean_testable;
};
ADAPT_EXPORT
template <class T, class U>
concept equal_comparable_with = requires(T a, U b)
{
	{ a == b } -> boolean_testable;
};

ADAPT_EXPORT
template <class T>
concept arithmetic = std::is_arithmetic_v<T>;

// C++23を使えないときのための、std::formattableの簡易的代用。非常に雑だが取りあえず動く。
ADAPT_EXPORT
template <class T, class Out, class Char>
concept formattable = requires(T v, std::formatter<std::remove_cvref_t<T>, Char> f, std::basic_format_context<Out, Char>&fc)
{
	{ f.format(v, fc) };
};

}

#endif
