#ifndef ADAPT_RANGES_H
#define ADAPT_RANGES_H

#include <ranges>
#include <iterator>
#include <optional>
#include <OpenADAPT/Utility/Common.h>
#include <OpenADAPT/Utility/TypeTraits.h>
#include <OpenADAPT/Utility/Function.h>

namespace adapt
{

namespace detail
{

template <class TrueBound>
struct RepeatBound;
template <>
struct RepeatBound<void>
{
	using difference_type = ptrdiff_t;
	RepeatBound() = default;
	RepeatBound(const RepeatBound&) = default;
	RepeatBound(RepeatBound&&) = default;
	RepeatBound& operator=(const RepeatBound&) = default;
	RepeatBound& operator=(RepeatBound&&) = default;
	void operator++() {}
	bool operator==(const RepeatBound&) const { return false; }
};
template <>
struct RepeatBound<size_t>
{
	using difference_type = ptrdiff_t;
	explicit RepeatBound() : m_count(0) {}
	explicit RepeatBound(size_t count) : m_count(count) {}
	RepeatBound(const RepeatBound&) = default;
	RepeatBound(RepeatBound&&) = default;
	RepeatBound& operator=(const RepeatBound&) = default;
	RepeatBound& operator=(RepeatBound&&) = default;
	void operator++() { ++m_count; }
	bool operator==(const RepeatBound& that) const { return m_count == that.m_count; }
	bool operator!=(const RepeatBound& that) const { return !(*this == that); }
	size_t GetCount() const { return m_count; }
private:
	size_t m_count;
};

}

ADAPT_EXPORT
template <class Type, class Bound>
struct RepeatIterator
{
	using iterator_category = std::input_iterator_tag;
	using difference_type = std::ptrdiff_t;
	using value_type = Type;
	using reference = const Type&;

	RepeatIterator() = default;
	explicit RepeatIterator(const Type& value) : m_value(value) {}
	explicit RepeatIterator(Type&& value) : m_value(std::move(value)) {}
	RepeatIterator(const RepeatIterator&) = default;
	RepeatIterator(RepeatIterator&&) = default;
	RepeatIterator& operator=(const RepeatIterator&) = default;
	RepeatIterator& operator=(RepeatIterator&&) = default;

	RepeatIterator& operator++() { ++m_count; return *this; }
	RepeatIterator operator++(int) { ++m_count; return *this; }
	const Type& operator*() const noexcept { return m_value; }
	friend bool operator==(const RepeatIterator& self, const detail::RepeatBound<Bound>& that) { return self.m_count == that; }
	friend bool operator==(const detail::RepeatBound<Bound>& that, const RepeatIterator& self) { return self.m_count == that; }
	friend bool operator!=(const RepeatIterator& self, const detail::RepeatBound<Bound>& that) { return !(self == that); }
	friend bool operator!=(const detail::RepeatBound<Bound>& that, const RepeatIterator& self) { return !(self == that); }

private:
	Type m_value;
	[[no_unique_address]] detail::RepeatBound<Bound> m_count;
};

ADAPT_EXPORT
template <class Type, class Bound>
struct RepeatView
	: public std::ranges::view_interface<RepeatView<Type, Bound>>
{
	using iterator = RepeatIterator<Type, Bound>;
	using sentinel = detail::RepeatBound<Bound>;
	RepeatView() = default;
	explicit RepeatView(const Type& value) : m_value(value) {}
	explicit RepeatView(const Type& value, size_t bound) : m_value(value), m_bound(bound) {}
	iterator begin() const& { return iterator(m_value); }
	iterator begin() && { return iterator(std::move(m_value)); }
	sentinel end() const { return m_bound; }
private:
	Type m_value;
	[[no_unique_address]] detail::RepeatBound<Bound> m_bound;
};

namespace views
{

ADAPT_EXPORT
template <class Type>
RepeatView<Type, void> Repeat(Type&& v)
{
	return RepeatView<std::decay_t<Type>, void>(std::forward<Type>(v));
}
ADAPT_EXPORT
template <class Type>
RepeatView<Type, size_t> Repeat(Type&& v, size_t bound)
{
	return RepeatView<std::decay_t<Type>, size_t>(std::forward<Type>(v), bound);
}

}

namespace detail
{

template <class Iterators, class IndexSequence>
class ZippedIterator_impl;
template <class Iterators, class IndexSequence>
class ZippedSentinel_impl;

template <class ...Iterators, std::size_t ...Indices>
class ZippedIterator_impl<std::tuple<Iterators...>, std::index_sequence<Indices...>>
{
public:

	using iterator_category = std::input_iterator_tag;
	using difference_type = typename std::tuple_element_t<0, std::tuple<std::iter_difference_t<Iterators>...>>;
	using value_type = std::tuple<std::iter_value_t<Iterators>...>;
	using reference = std::tuple<std::iter_reference_t<Iterators>...>;

	ZippedIterator_impl() = default;
	template <class ...Iters>
	ZippedIterator_impl(Iters&& ...cs)
		: mIterators(std::forward<Iters>(cs)...)
	{}
	ZippedIterator_impl(const ZippedIterator_impl&) = default;
	ZippedIterator_impl(ZippedIterator_impl&&) = default;
	ZippedIterator_impl& operator=(const ZippedIterator_impl&) = default;
	ZippedIterator_impl& operator=(ZippedIterator_impl&&) = default;

	ZippedIterator_impl& operator++()
	{
		[[maybe_unused]] int d[] = { (++std::get<Indices>(mIterators), 0)... };
		return *this;
	}
	ZippedIterator_impl operator++(int)
	{
		ZippedIterator_impl res = *this;
		[[maybe_unused]] int d[] = { (++std::get<Indices>(mIterators), 0)... };
		return res;
	}
	auto operator*() const noexcept
	{
		//std::ranges::viewは場合によってはoperator*によって一時オブジェクトを返してくる場合がある。
		//その場合、ここをstd::forward_as_tupleにするとrvalue referenceのダングリングが発生する。
		//よって、各iteratorのoperator*戻り値がrvalueである場合は参照を取り払ってtupleにするため、MakeDecayedTupleを使う。
		return MakeDecayedTuple(*std::get<Indices>(mIterators)...);
	}
	bool operator==(const ZippedIterator_impl& it) const
	{
		return (... && (std::get<Indices>(mIterators) == std::get<Indices>(it.mIterators)));
	}
	bool operator!=(const ZippedIterator_impl& it) const
	{
		return !(*this == it);
	}

	template <size_t Index>
	decltype(auto) GetIteratorAt() const { return std::get<Index>(mIterators); }

private:
	std::tuple<Iterators...> mIterators;
};

template <class ...Sentinels, std::size_t ...Indices>
class ZippedSentinel_impl<std::tuple<Sentinels...>, std::index_sequence<Indices...>>
{
	//using ZippedIterator_ = ZippedIterator_impl<std::tuple<Sentinels...>, std::index_sequence<Indices...>>;
public:

	ZippedSentinel_impl() = default;
	template <class ...Ss>
		requires (std::convertible_to<Ss, Sentinels> && ...)
	ZippedSentinel_impl(Ss&& ...ss)
		: mIterators(std::forward<Ss>(ss)...)
	{}
	ZippedSentinel_impl(const ZippedSentinel_impl&) = default;
	ZippedSentinel_impl(ZippedSentinel_impl&&) = default;
	ZippedSentinel_impl& operator=(const ZippedSentinel_impl&) = default;
	ZippedSentinel_impl& operator=(ZippedSentinel_impl&&) = default;
	template <class ZippedIterator>
	friend bool operator==(const ZippedIterator& it, const ZippedSentinel_impl& end)
	{
		//一つでも最終要素を超えれば終端とみなす。
		return (... || (it.template GetIteratorAt<Indices>() == std::get<Indices>(end.mIterators)));
	}
	template <class ZippedIterator>
	friend bool operator==(const ZippedSentinel_impl& end, const ZippedIterator& it)
	{
		return it == end;
	}
	template <class ZippedIterator>
	friend bool operator!=(const ZippedIterator& it, const ZippedSentinel_impl& end)
	{
		return !(it == end);
	}
	template <class ZippedIterator>
	friend bool operator!=(const ZippedSentinel_impl& end, const ZippedIterator& it)
	{
		return it != end;
	}

	template <size_t Index>
	decltype(auto) GetSentinelAt() const { return std::get<Index>(mIterators); }

private:
	std::tuple<Sentinels...> mIterators;
};

}

ADAPT_EXPORT
template <class ...Iterators>
using ZippedIterator = detail::ZippedIterator_impl<std::tuple<Iterators...>, std::make_index_sequence<sizeof...(Iterators)>>;
ADAPT_EXPORT
template <class ...Sentinels>
using ZippedSentinel = detail::ZippedSentinel_impl<std::tuple<Sentinels...>, std::make_index_sequence<sizeof...(Sentinels)>>;


ADAPT_EXPORT
template <class ...Iterators>
class ZippedIteratorWithIndex
	: public ZippedIterator<Iterators...>
{
public:

	using value_type = std::tuple<size_t, typename std::iterator_traits<Iterators>::value_type...>;

	ZippedIteratorWithIndex() = default;
	template <class ...Iters>
	ZippedIteratorWithIndex(Iters&& ...cs)
		: ZippedIterator<Iterators...>(std::forward<Iters>(cs)...), mIndex(0)
	{}

	ZippedIteratorWithIndex& operator++()
	{
		++mIndex;
		ZippedIterator<Iterators...>::operator++();
		return *this;
	}
	ZippedIteratorWithIndex operator++(int)
	{
		ZippedIteratorWithIndex res = *this;
		++mIndex;
		ZippedIterator<Iterators...>::operator++();
		return res;
	}
	auto operator*() const noexcept
	{
		return std::tuple_cat(std::make_tuple(mIndex), ZippedIterator<Iterators...>::operator*());
	}

private:
	std::size_t mIndex;
};

//sentinelの方にindexの情報は必要ないので、ZippedSentinelをそのまま使う。
ADAPT_EXPORT
template <class ...Iterators>
using ZippedSentinelWithIndex = ZippedSentinel<Iterators...>;

namespace detail
{

template <class ...Iterators>
auto MakeZippedIterator(Iterators&& ...it)
{
	return ZippedIterator<std::remove_cvref_t<Iterators>...>(std::forward<Iterators>(it)...);
}
template <class ...Iterators>
auto MakeZippedSentinel(Iterators&& ...it)
{
	return ZippedSentinel<std::remove_cvref_t<Iterators>...>(std::forward<Iterators>(it)...);
}
template <class ...Iterators>
auto MakeZippedIteratorWithIndex(Iterators&& ...it)
{
	return ZippedIteratorWithIndex<std::remove_cvref_t<Iterators>...>(std::forward<Iterators>(it)...);
}
template <class ...Iterators>
auto MakeZippedSentinelWithIndex(Iterators&& ...it)
{
	return ZippedSentinelWithIndex<std::remove_cvref_t<Iterators>...>(std::forward<Iterators>(it)...);
}


template <class Containers, class IndexSequence>
class Zip_impl;
template <std::ranges::input_range ...Containers, std::size_t ...Indices>
class Zip_impl<std::tuple<Containers...>, std::index_sequence<Indices...>>
	: public std::ranges::view_interface<Zip_impl<std::tuple<Containers...>, std::index_sequence<Indices...>>>
{
	//std::ranges::viewの要件を満たすためには、このクラスがデフォルト初期化可能でなければならない。
	//しかしZippedRangeは場合によって一時オブジェクトを持たなければならないことがあり、
	//デフォルト初期化不可能な一時オブジェクトを与えられるとコンパイルができなくなるし、
	//参照を与えられた場合もポインタに変換しなければならない。
	//諸々の違いを吸収するために、Holder、Observer、std::optionalを使い分ける。
	template <class C>
	struct Holder
	{
		Holder() {}
		Holder(C&& c) : c(std::move(c)) {}
		C c;
		C& operator*() { return c; }
		const C& operator*() const { return c; }
	};
	template <class C>
	struct Observer
	{
		std::add_pointer_t<C> c;
		Observer() : c(nullptr) {}
		Observer(C& c) : c(&c) {}
		Observer(const Observer<C>& o) = default;
		C& operator*() { return *c; }
		const C& operator*() const { return *c; }
	};

	template <class C, bool IsLValueReference, bool IsDefaultInitializable>
	struct PNO;
	template <class C, bool IsDefaultInitializable>
	struct PNO<C, true, IsDefaultInitializable> { using Type = Observer<C>; };
	template <class C>
	struct PNO<C, false, true> { using Type = Holder<C>; };
	template <class C>
	struct PNO<C, false, false> { using Type = std::optional<C>; };

	template <class C>
	using PNO_t = typename PNO<C, std::is_lvalue_reference_v<C>, std::default_initializable<C>>::Type;

public:

	Zip_impl() = default;
	template <class ...C, std::enable_if_t<(sizeof...(C) > 0), std::nullptr_t> = nullptr>
	Zip_impl(C&& ...c)
		: mContainers{ std::forward<C>(c)... } {}

	auto begin() const { return MakeZippedIterator(std::ranges::begin(*std::get<Indices>(mContainers))...); }
	auto end() const { return MakeZippedSentinel(std::ranges::end(*std::get<Indices>(mContainers))...); }

protected:

	std::tuple<PNO_t<Containers>...> mContainers;
};

}

ADAPT_EXPORT
template <std::ranges::input_range ...Containers>
class ZipView : public detail::Zip_impl<std::tuple<Containers...>, std::make_index_sequence<sizeof...(Containers)>>
{
	using Base = detail::Zip_impl<std::tuple<Containers...>, std::make_index_sequence<sizeof...(Containers)>>;
public:
	using Base::Base;
};


namespace views
{

ADAPT_EXPORT
template <std::ranges::input_range ...Range>
ZipView<Range...> Zip(Range&& ...ranges)
{
	return ZipView<Range...>(std::forward<Range>(ranges)...);
}

}

namespace detail
{

template <class Iterators, class IndexSequence>
class Enumerate_impl;
template <class ...Containers, std::size_t ...Indices>
class Enumerate_impl<std::tuple<Containers...>, std::index_sequence<Indices...>>
	: public std::ranges::view_interface<Enumerate_impl<std::tuple<Containers...>, std::index_sequence<Indices...>>>,
	public ZipView<Containers...>
{
public:

	using ZipView<Containers...>::ZipView;
	using Base = ZipView<Containers...>;

	auto begin() const
	{
		return MakeZippedIteratorWithIndex(std::ranges::begin(*std::get<Indices>(this->mContainers))...);
	}
	auto end() const
	{
		return MakeZippedSentinelWithIndex(std::ranges::end(*std::get<Indices>(this->mContainers))...);
	}

};

}

namespace views
{

ADAPT_EXPORT
template <std::ranges::input_range ...Containers>
class Enumerate : public detail::Enumerate_impl<std::tuple<Containers...>, std::make_index_sequence<sizeof...(Containers)>>
{
	using Base = detail::Enumerate_impl<std::tuple<Containers...>, std::make_index_sequence<sizeof...(Containers)>>;
public:
	using Base::Base;
};

template <std::ranges::input_range ...C> Enumerate(C&& ...) -> Enumerate<C...>;

}

namespace ranges
{

ADAPT_EXPORT
template <class Range>
concept arithmetic_range = std::ranges::input_range<Range> && arithmetic<std::ranges::range_value_t<Range>>;
ADAPT_EXPORT
template <class Range>
concept string_range = std::ranges::input_range<Range> && std::convertible_to<std::ranges::range_value_t<Range>, std::string>;
ADAPT_EXPORT
template <class Range>
concept range_of_range = std::ranges::input_range<Range> && std::ranges::input_range<std::ranges::range_value_t<Range>>;
ADAPT_EXPORT
template <class Range>
concept arithmetic_matrix_range = ranges::range_of_range<Range> && std::ranges::sized_range<Range> &&
ranges::arithmetic_range<std::ranges::range_value_t<Range>> && std::ranges::sized_range<std::ranges::range_value_t<Range>>;

}

}

#endif