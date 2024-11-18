#ifndef ADAPT_UTILITY_GENERICS_H
#define ADAPT_UTILITY_GENERICS_H

//#ifdef _MSC_VER
//#pragma warning(push)
//#pragma warning(disable: 4702)
//#endif

#include <cassert>
#include <cstddef>
#include <type_traits>
#include <utility>
#include <tuple>
#include <typeindex>
#include <any>
#include <OpenADAPT/Utility/TypeTraits.h>
#include <OpenADAPT/Utility/Any.h>

namespace adapt
{

namespace detail
{

template <class Refs, class Visitors>
class Generics_impl;

template <class ...Refs, class ...Visitors>
class Generics_impl<std::tuple<Refs...>, std::tuple<Visitors...>>
{
	template <class Type, class Any_>
	static decltype(auto) GetFromAny(Any_&& any)
	{
		if constexpr (std::is_same_v<std::decay_t<Any_>, std::any>)
		{
			if constexpr (std::is_rvalue_reference_v<Any_&&>)
				return std::any_cast<Type&&>(std::forward<Any_>(any));
			else if constexpr (std::is_const_v<std::remove_reference_t<Any_&&>>)
				return std::any_cast<const Type&>(std::forward<Any_>(any));
			else
				return std::any_cast<Type&>(std::forward<Any_>(any));
		}
		else
			return std::forward<Any_>(any).template Get_unsafe<Type>();
	}

	//再帰継承にするよりもstd::tuple<VisitBase<Visitors>...>のようにtupleに押し込んだほうがいい、という考え方もできるが、
	//それだとVisitorsのコピー時に全要素に対して仮想関数を呼ぶ必要があり、オーバーヘッドが大きくなる。
	//再帰継承なら仮想関数の呼び出しは一度で済む。
	template <class Visitors_, size_t Index>
	class VisitBase_impl;
	template <class Visitor, size_t Index>
	class VisitBase_impl<TypeList<Visitor>, Index>
	{
	public:
		using ArgTypes = typename Visitor::ArgTypes;
		using RetType = typename Visitor::RetType;
		virtual RetType Visit(std::integral_constant<size_t, Index>, ArgTypes args, const std::tuple<Refs...>& refs) const = 0;
		virtual RetType Visit(std::integral_constant<size_t, Index>, ArgTypes args, std::tuple<Refs...>& refs) = 0;
	};
	template <class Visitor, class ...Visitors_, size_t Index>
	class VisitBase_impl<TypeList<Visitor, Visitors_...>, Index>
		: public VisitBase_impl<TypeList<Visitors_...>, Index + 1>
	{
	public:
		using Base = VisitBase_impl<TypeList<Visitors_...>, Index + 1>;
		using Base::Visit;
		using ArgTypes = typename Visitor::ArgTypes;
		using RetType = typename Visitor::RetType;
		virtual RetType Visit(std::integral_constant<size_t, Index>, ArgTypes args, const std::tuple<Refs...>& refs) const = 0;
		virtual RetType Visit(std::integral_constant<size_t, Index>, ArgTypes args, std::tuple<Refs...>& refs) = 0;
	};

	class VisitBase : public VisitBase_impl<TypeList<Visitors...>, 0>
	{
	public:
		using VisitBase_impl<TypeList<Visitors...>, 0>::Visit;
		virtual void CopyTo(void*) const = 0;
		virtual void MoveTo(void*) = 0;
	};

	template <class Types, class TIndices, class Visitors_, size_t VIndex>
	class VisitDerivative_impl;
	template <class ...Types, size_t ...TIndices, class Visitor, size_t VIndex>
	class VisitDerivative_impl<TypeList<Types...>, std::index_sequence<TIndices...>, TypeList<Visitor>, VIndex>
		: public VisitBase
	{
	public:
		using VisitBase::Visit;
		using ArgTypes = typename Visitor::ArgTypes;
		using RetType = typename Visitor::RetType;
		virtual RetType Visit(std::integral_constant<size_t, VIndex>, ArgTypes args, const std::tuple<Refs...>& refs) const override
		{
			return std::apply(Visitor(), std::tuple_cat(args, std::forward_as_tuple(GetFromAny<Types>(std::get<TIndices>(refs))...)));
		}
		virtual RetType Visit(std::integral_constant<size_t, VIndex>, ArgTypes args, std::tuple<Refs...>& refs) override
		{
			return std::apply(Visitor(), std::tuple_cat(args, std::forward_as_tuple(GetFromAny<Types>(std::get<TIndices>(refs))...)));
		}
	};
	template <class ...Types, size_t ...TIndices, class Visitor, class ...Visitors_, size_t VIndex>
	class VisitDerivative_impl<TypeList<Types...>, std::index_sequence<TIndices...>, TypeList<Visitor, Visitors_...>, VIndex>
		: public VisitDerivative_impl<TypeList<Types...>, std::index_sequence<TIndices...>, TypeList<Visitors_...>, VIndex + 1>
	{
	public:
		using Base = VisitDerivative_impl<TypeList<Types...>, std::index_sequence<TIndices...>, TypeList<Visitors_...>, VIndex + 1>;
		using Base::Visit;
		using ArgTypes = typename Visitor::ArgTypes;
		using RetType = typename Visitor::RetType;
		virtual RetType Visit(std::integral_constant<size_t, VIndex>, ArgTypes args, const std::tuple<Refs...>& refs) const override
		{
			return std::apply(Visitor(), std::tuple_cat(args, std::forward_as_tuple(GetFromAny<Types>(std::get<TIndices>(refs))...)));
		}
		virtual RetType Visit(std::integral_constant<size_t, VIndex>, ArgTypes args, std::tuple<Refs...>& refs) override
		{
			return std::apply(Visitor(), std::tuple_cat(args, std::forward_as_tuple(GetFromAny<Types>(std::get<TIndices>(refs))...)));
		}
	};

	template <class ...Types>
	class VisitDerivative
		: public VisitDerivative_impl<TypeList<Types...>, std::make_index_sequence<sizeof...(Types)>, TypeList<Visitors...>, 0>
	{
	public:
		virtual void CopyTo(void* base) const override
		{
			new (base) VisitDerivative(*this);
		}
		virtual void MoveTo(void* base) override
		{
			new (base) VisitDerivative(std::move(*this));
		}
	};

	template <class Ref, class Type_>
	struct Adaptor { using Type = std::remove_cvref_t<Type_>; };
	template <class Type_>
	struct Adaptor<AnyURef, Type_> { using Type = Type_; };
	template <class Refs_, class Types, class ATypes>
	struct MakeVisitDerivative;
	template <class ...Refs_, class ...ATypes>
	struct MakeVisitDerivative<TypeList<Refs_...>, TypeList<>, TypeList<ATypes...>>
	{
		using Type = VisitDerivative<ATypes...>;
	};
	template <class Ref_, class ...Refs_, class Type, class ...Types, class ...ATypes>
	struct MakeVisitDerivative<TypeList<Ref_, Refs_...>, TypeList<Type, Types...>, TypeList<ATypes...>>
		: MakeVisitDerivative<TypeList<Refs_...>, TypeList<Types...>, TypeList<ATypes..., typename Adaptor<Ref_, Type>::Type>>
	{};

	template <size_t N, std::enable_if_t<(N == 0), std::nullptr_t> = nullptr>
	static constexpr auto MakeNullRefTuple()
	{
		return std::tuple<>();
	}
	template <size_t N, class Ref_, class ...Refs_, std::enable_if_t<(N == 0), std::nullptr_t> = nullptr>
	static constexpr auto MakeNullRefTuple()
	{
		return std::tuple_cat(std::make_tuple(EmptyClass()), MakeNullRefTuple<0, Refs_...>());
	}
	template <size_t N, class Ref_, class ...Refs_, std::enable_if_t<(N != 0), std::nullptr_t> = nullptr>
	static constexpr auto MakeNullRefTuple()
	{
		return MakeNullRefTuple<N - 1, Refs_...>();
	}

public:

	//Refsが1個のみの場合、argsがtupleか否かに関わらず問答無用でstd::tupleにパックされたままmRefsの<0>番目に格納される。
	//結果、mRefsの中身はTypesの<0>番目ではなく、std::tuple<Types...>となってしまう。
	//これを回避するため、引数が1個のときだけは処理を分岐させる。
	template <class ...Types, std::enable_if_t<(sizeof...(Refs) > 1 && sizeof...(Types) >= 1), std::nullptr_t> = nullptr>
	Generics_impl(std::tuple<Types...> args)
		: m_refs(std::tuple_cat(std::move(args), MakeNullRefTuple<sizeof...(Types), Refs...>()))
	{
		using Visit_ = typename MakeVisitDerivative<TypeList<Refs...>, TypeList<Types...>, TypeList<>>::Type;
		static_assert(sizeof(Visit_) <= sizeof(m_visitors), "the size of storage is insufficient.");
		Visit_* p = reinterpret_cast<Visit_*>(&m_visitors);
		new (p) Visit_();
	}
	template <class Type, size_t RefSize = sizeof...(Refs), std::enable_if_t<(RefSize == 1), std::nullptr_t> = nullptr>
	Generics_impl(std::tuple<Type> arg)
		: m_refs(std::get<0>(arg))
	{
		using Visit_ = typename MakeVisitDerivative<TypeList<Refs...>, TypeList<Type>, TypeList<>>::Type;
		static_assert(sizeof(Visit_) <= sizeof(m_visitors), "the size of storage is insufficient.");
		Visit_* p = reinterpret_cast<Visit_*>(&m_visitors);
		new (p) Visit_();
	}

	Generics_impl(const Generics_impl& g)
		: m_refs(g.m_refs)
	{
		const VisitBase* vis = reinterpret_cast<const VisitBase*>(&g.m_visitors);
		vis->CopyTo(&m_visitors);
	}
	//mRefsというメンバ名ではあるが、実はAnyやShareableAnyも対応しているので、
	//ムーブコンストラクタが意味を持つこともある。
	Generics_impl(Generics_impl&& g) noexcept
		: m_refs(std::move(g.m_refs))
	{
		VisitBase* vis = reinterpret_cast<VisitBase*>(&g.m_visitors);
		vis->MoveTo(&m_visitors);
	}
	Generics_impl& operator=(const Generics_impl& g)
	{
		m_refs = g.m_refs;
		const VisitBase* vis = reinterpret_cast<const VisitBase*>(&g.m_visitors);
		vis->CopyTo(&m_visitors);
		return *this;
	}
	Generics_impl& operator=(Generics_impl&& g) noexcept
	{
		m_refs = std::move(g.m_refs);
		VisitBase* vis = reinterpret_cast<VisitBase*>(&g.m_visitors);
		vis->MoveTo(&m_visitors);
		return *this;
	}

	template <size_t Index>
	decltype(auto) GetAny() const { return std::get<Index>(m_refs); }
	template <size_t Index>
	decltype(auto) GetAny() { return std::get<Index>(m_refs); }
	template <size_t Index, class Type>
	decltype(auto) Get() const { return GetFromAny<Type>(std::get<Index>(m_refs)); }
	template <size_t Index, class Type>
	decltype(auto) Get() { return GetFromAny<Type>(std::get<Index>(m_refs)); }

	template <size_t Index, class ...Args>
	decltype(auto) Visit(Args&& ...args) const
	{
		using Visitor = typename GetType<Index, Visitors...>::Type;
		using ArgTypes = typename Visitor::ArgTypes;
		const VisitBase* vis = std::launder(reinterpret_cast<const VisitBase*>(&m_visitors));
		return vis->Visit(std::integral_constant<size_t, Index>(), ArgTypes(std::forward<Args>(args)...), m_refs);
	}
	template <size_t Index, class ...Args>
	decltype(auto) Visit(Args&& ...args)
	{
		using Visitor = typename GetType<Index, Visitors...>::Type;
		using ArgTypes = typename Visitor::ArgTypes;
		VisitBase* vis = std::launder(reinterpret_cast<VisitBase*>(&m_visitors));
		return vis->Visit(std::integral_constant<size_t, Index>(), ArgTypes(std::forward<Args>(args)...), m_refs);
	}
	/*template <class Visitor, class ...Args>
	decltype(auto) Visit(Args&& ...args) const
	{
		constexpr size_t Index = FindType<Visitor, Visitors...>::Index;
		return Visit<Index>(std::forward<Args>(args)...);
	}*/

private:

	std::tuple<Refs...> m_refs;
	std::aligned_storage_t<sizeof...(Visitors) * 8> m_visitors;
};

}

template <class Ref, class Visitor>
class Generics : public Generics<std::tuple<Ref>, std::tuple<Visitor>>
{
	using Base = Generics<std::tuple<Ref>, std::tuple<Visitor>>;
	using Base::Base;
};
template <class Ref, class ...Visitors>
class Generics<Ref, std::tuple<Visitors...>> : public Generics<std::tuple<Ref>, std::tuple<Visitors...>>
{
	using Base = Generics<std::tuple<Ref>, std::tuple<Visitors...>>;
	using Base::Base;
};
template <class ...Refs, class Visitor>
class Generics<std::tuple<Refs...>, Visitor> : public Generics<std::tuple<Refs...>, std::tuple<Visitor>>
{
	using Base = Generics<std::tuple<Refs...>, std::tuple<Visitor>>;
	using Base::Base;
};
template <class ...Refs, class ...Visitors>
class Generics<std::tuple<Refs...>, std::tuple<Visitors...>>
	: public detail::Generics_impl<std::tuple<Refs...>, std::tuple<Visitors...>>
{
public:
	using Base = detail::Generics_impl<std::tuple<Refs...>, std::tuple<Visitors...>>;

	template <class ...Types, std::enable_if_t<(sizeof...(Types) == sizeof...(Refs)), std::nullptr_t> = nullptr>
	Generics(std::tuple<Types...> v)
		: Base(std::move(v))
	{}
	template <class ...Types, std::enable_if_t<(sizeof...(Types) == sizeof...(Refs) && sizeof...(Types) > 1), std::nullptr_t> = nullptr>
	Generics(Types&& ...args)
		: Base(std::forward_as_tuple(std::forward<Types>(args)...))
	{}
	template <class Type, std::enable_if_t<(sizeof...(Refs) == 1 &&
											!IsBaseOf_XT<std::tuple, std::remove_cvref_t<Type>>::value &&
											!std::same_as<std::remove_cvref_t<Type>, Generics>), std::nullptr_t> = nullptr>
		Generics(Type&& arg)
		: Base(std::forward_as_tuple(std::forward<Type>(arg)))
	{}

};
template <class Ref, size_t MaxNumOfArgs = 16>
class Variadic
{
	template <size_t N, class ...Refs>
	struct Expand : public Expand<N - 1, Refs..., Ref>
	{};
	template <class ...Refs>
	struct Expand<0, Refs...>
	{
		using Type = std::tuple<Refs...>;
	};
public:
	using Type = typename Expand<MaxNumOfArgs>::Type;
};

template <class Ref, size_t MaxNumOfArgs, class Visitor>
class Generics<Variadic<Ref, MaxNumOfArgs>, Visitor>
	: public Generics<Variadic<Ref, MaxNumOfArgs>, std::tuple<Visitor>>
{
	using Base = Generics<Variadic<Ref, MaxNumOfArgs>, std::tuple<Visitor>>;
	using Base::Base;
};
template <class Ref, size_t MaxNumOfArgs, class ...Visitors>
class Generics<Variadic<Ref, MaxNumOfArgs>, std::tuple<Visitors...>>
	: public detail::Generics_impl<typename Variadic<Ref, MaxNumOfArgs>::Type, std::tuple<Visitors...>>
{
	using Base = detail::Generics_impl<typename Variadic<Ref, MaxNumOfArgs>::Type, std::tuple<Visitors...>>;
public:
	using Base::Base;


};

}

//#ifdef _MSC_VER
//#pragma warning(pop)
//#endif

#endif
