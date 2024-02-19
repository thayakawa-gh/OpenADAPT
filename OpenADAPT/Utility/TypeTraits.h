#ifndef ADAPT_CUF_TYPE_TRAITS_H
#define ADAPT_CUF_TYPE_TRAITS_H

#include <tuple>
#include <type_traits>
#include <utility>
#include <complex>

namespace adapt
{

//各種演算子を使えるようにしたintegral_constant。
template <class Type, Type N, template <Type> class Derived>
struct IntegralConstant : std::integral_constant<Type, N>
{
	constexpr auto operator+() const requires std::is_signed_v<Type> { return Derived<Type(+N)>(); }
	constexpr auto operator-() const requires std::is_signed_v<Type> { return Derived<Type(-N)>(); }

	template <Type X>
	constexpr auto operator+(Derived<X>) const { return Derived<Type(N + X)>{}; }
	template <Type X>
	constexpr auto operator-(Derived<X>) const { return Derived<Type(N + X)>{}; }

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

template <class ...T>
struct TypeList
{
	static constexpr size_t size = sizeof...(T);
};
template <template <class...> class ...T>
struct UnarguedList
{
	static constexpr size_t size = sizeof...(T);
};
template <auto ...V>
struct ValueList
{
	static constexpr size_t size = sizeof...(V);
};


template <auto N, class T>
struct PseudoIndexed { using Type = T; };
template <auto N, class T>
using PseudoIndexed_t = PseudoIndexed<N, T>::Type;

template <class X, class T>
struct PseudoArgued { using Type = T; };
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
{};
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

template <class Type, class ...Types>
inline constexpr size_t FindType_v = FindType<Type, Types...>::Index;

template <class Type, class ...Types>
using FirstType = Type;


template <size_t Index, auto ...Values>
constexpr auto GetValue()
{
	constexpr auto tup = std::make_tuple(Values...);
	return std::get<Index>(tup);
}


template <template <class...> class Base, class Derived>
struct IsBaseOf_XT
{
	template <class ...U>
	static constexpr std::true_type check(const Base<U...>*);
	static constexpr std::false_type check(const void*);

	static const Derived* d;
public:
	static constexpr bool value = decltype(check(d))::value;
};

template <template <class...> class Base, class Derived>
inline constexpr bool IsBaseOf_XT_v = IsBaseOf_XT<Base, Derived>::value;

template <template <auto...> class Base, class Derived>
struct IsBaseOf_XN
{
	template <auto ...U>
	static constexpr std::true_type check(const Base<U...>*);
	static constexpr std::false_type check(const void*);

	static const Derived* d;
public:
	static constexpr bool value = decltype(check(d))::value;
};

template <template <auto...> class Base, class Derived>
inline constexpr bool IsBaseOf_XN_v = IsBaseOf_XN<Base, Derived>::value;


template <template <auto, class...> class Base, class Derived>
struct IsBaseOf_NXT
{
	template <auto N, class ...U>
	static constexpr std::true_type check(const Base<N, U...>*);
	static constexpr std::false_type check(const void*);

	static const Derived* d;
public:
	static constexpr bool value = decltype(check(d))::value;
};

template <template <auto, class...> class Base, class Derived>
inline constexpr bool IsBaseOf_NXT_v = IsBaseOf_NXT<Base, Derived>::value;

template <template <class ...> class T, class U>
struct IsSame_XT : public std::false_type {};
template <template <class ...> class T, class ...X>
struct IsSame_XT<T, T<X...>> : public std::true_type {};

template <template <class ...> class T, class U>
inline constexpr bool IsSame_XT_v = IsSame_XT<T, U>::value;

template <template <auto ...> class T, class U>
struct IsSame_XN : public std::false_type {};
template <template <auto ...> class T, auto ...X>
struct IsSame_XN<T, T<X...>> : public std::true_type {};

template <template <auto ...> class T, class U>
inline constexpr bool IsSame_XN_v = IsSame_XN<T, U>::value;

template <template <class, auto> class T, class U>
struct IsSame_TN : public std::false_type {};
template <template <class, auto> class T, class X, auto Y>
struct IsSame_TN<T, T<X, Y>> : public std::true_type {};

template <template <class, auto> class T, class U>
inline constexpr bool IsSame_TN_v = IsSame_TN<T, U>::value;

template <template <auto, class> class T, class U>
struct IsSame_NT : public std::false_type {};
template <template <auto, class> class T, auto X, class Y>
struct IsSame_NT<T, T<X, Y>> : public std::true_type {};

template <template <auto, class> class T, class U>
inline constexpr bool IsSame_NT_v = IsSame_NT<T, U>::value;


template <class T>
struct DecayRRef { using Type = T; };
template <class T>
struct DecayRRef<T&&> { using Type = std::decay_t<T>; };

template <auto ...N>
struct ConstantSequence
{};

template <class T>
struct IsComplex : public std::false_type {};
template <class T>
struct IsComplex<std::complex<T>> : public std::true_type{};

template <class T>
inline constexpr bool IsComplex_v = IsComplex<T>::value;

template <class T, class U>
concept derivative_relation = std::derived_from<T, U> || std::derived_from<U, T>;

template <class T, class U>
concept similar_lvalue_reference =
	(std::is_lvalue_reference_v<T> && std::is_lvalue_reference_v<U>) &&
	(std::same_as<std::remove_cvref_t<T>, std::remove_cvref_t<U>> ||
	 derivative_relation<std::remove_cvref_t<T>, std::remove_cvref_t<U>>);

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
template <class ...T>
using CommonRef_t = CommonRef<T...>::Type;

//static constexpr auto x = ConstantSequence<1, 2, 3>();

template <class T>
concept non_void = !std::same_as<T, void>;

template <class T, template <class...> class U>
concept derived_from_xt = IsBaseOf_XT<U, T>::value;

template <class T, template <auto...> class U>
concept derived_from_xn = IsBaseOf_XN<U, T>::value;

template <class T, template <auto, class...> class U>
concept derived_from_nxt = IsBaseOf_NXT<U, T>::value;

template <class T, template <class...> class U>
concept same_as_xt = IsSame_XT<U, T>::value;
//template <class T, template <class...> class U>
//concept not_same_as_xt = !IsSame_XT<U, T>::value;

template <class T, template <auto...> class U>
concept same_as_xn = IsSame_XN<U, T>::value;
//template <class T, template <auto...> class U>
//concept not_same_as_xn = !IsSame_XN<U, T>::value;

template <class T, template <class, auto> class U>
concept same_as_tn = IsSame_TN<U, T>::value;
//template <class T, template <class, auto> class U>
//concept not_same_as_xn = !IsSame_TN<U, T>::value;

template <class T, template <auto, class> class U>
concept same_as_nt = IsSame_NT<U, T>::value;
//template <class T, template <auto...> class U>
//concept not_same_as_xn = !IsSame_XN<U, T>::value;


template <class T, class U>
concept similar_to = std::same_as<std::decay_t<T>, U>;
template <class T, template <class...> class U>
concept similar_to_xt = same_as_xt<std::decay_t<T>, U>;
template <class T, template <auto...> class U>
concept similar_to_xn = same_as_xn<std::decay_t<T>, U>;

template <class ArgType>
concept boolean_testable = requires(ArgType a)
{
	static_cast<bool>(a);
};
template <class ArgType>
concept summable = requires(ArgType a, ArgType b)
{
	a += b;
};
template <class ArgType>
concept dividable = requires(ArgType a, ArgType b)
{
	a /= b;
};
template <class ArgType>
concept greater_than_comparable = requires(ArgType a, ArgType b)
{
	{ a > b } -> boolean_testable;
};
template <class ArgType>
concept less_than_comparable = requires(ArgType a, ArgType b)
{
	{ a < b } -> boolean_testable;
};
template <class ArgType>
concept equal_comparable = requires(ArgType a, ArgType b)
{
	{ a == b } -> boolean_testable;
};

}

#endif
