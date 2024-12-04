#ifndef ADAPT_RANGE_RANGEADAPTER_H
#define ADAPT_RANGE_RANGEADAPTER_H

#include <OpenADAPT/Container/Tree.h>

namespace adapt
{

namespace detail
{

template <class Container, class ...Args>
auto GetRange_select_jointmode(Container&& s, Args&& ...args)
{
	if constexpr (joined_container<Container>)
		return s.GetRange_delayed(std::forward<Args>(args)...);
	else
		return s.GetRange(std::forward<Args>(args)...);
}

}

template <template <class...> class View, class ...Args>
class RangeReceiver
{
public:
	template <class ...Args_>
		requires (std::convertible_to<Args_&&, Args> && ...)
	RangeReceiver(Args_&& ...args)
		: m_args(std::forward<Args_>(args)...)
	{}

private:
	template <class BaseView, size_t ...Indices>
	View<BaseView, Args...> Create_impl(BaseView&& bw, std::index_sequence<Indices...>)
	{
		return View<BaseView, Args...>(std::forward<BaseView>(bw), std::get<Indices>(m_args)...);
	}
	template <class BaseView, size_t ...Indices>
	View<BaseView, Args...> Create_impl_rvref(BaseView&& bw, std::index_sequence<Indices...>)
	{
		return View<BaseView, Args...>(std::forward<BaseView>(bw), std::get<Indices>(std::move(m_args))...);
	}
public:

	template <class BaseView>
	View<BaseView, Args...> Create(BaseView&& bw)&
	{
		return Create_impl(std::forward<BaseView>(bw), std::make_index_sequence<sizeof...(Args)>{});
	}
	template <class BaseView>
	View<BaseView, Args...> Create(BaseView&& bw)&&
	{
		return Create_impl_rvref(std::forward<BaseView>(bw), std::make_index_sequence<sizeof...(Args)>{});
	}

	template <any_container Container>
	friend auto operator|(Container&& s, RangeReceiver<View, Args...>&& r)
	{
		return std::move(r).Create(detail::GetRange_select_jointmode(s));
	}
	template <any_container Container>
	friend auto operator|(Container&& s, const RangeReceiver<View, Args...>& r)
	{
		return r.Create(detail::GetRange_select_jointmode(s));
	}
	template <traversal_range Range>
	friend auto operator|(Range&& s, RangeReceiver<View, Args...>&& r)
	{
		return std::move(r).Create(std::forward<Range>(s));
	}
	template <traversal_range Range>
	friend auto operator|(Range&& s, const RangeReceiver<View, Args...>& r)
	{
		return r.Create(std::forward<Range>(s));
	}

private:

	std::tuple<Args...> m_args;
};


template <template <class> class Consumer, class ...Args>
class RangeConversion
{
public:
	template <class ...Args_>
		requires (std::convertible_to<Args_&&, Args> && ...)
	RangeConversion(Args_&& ...args) : m_args(std::forward<Args_>(args)...) {}

private:
	template <class Range, size_t ...Indices>
	auto Exec_impl(Range&& bw, std::index_sequence<Indices...>)&
	{
		Consumer<std::decay_t<Range>> c;
		return c.Exec(std::forward<Range>(bw), std::get<Indices>(m_args)...);
	}
	template <class Range, size_t ...Indices>
	auto Exec_impl(Range&& bw, std::index_sequence<Indices...>)&&
	{
		Consumer<std::decay_t<Range>> c;
		return c.Exec(std::forward<Range>(bw), std::get<Indices>(std::move(m_args))...);
	}
public:
	template <traversal_range Range>
	auto Exec(Range&& bw)
	{
		return Exec_impl(std::forward<Range>(bw), std::make_index_sequence<sizeof...(Args)>{});
	}

	template <any_container Container>
	friend auto operator|(Container&& s, RangeConversion<Consumer, Args...>&& r)
	{
		return std::move(r).Exec(detail::GetRange_select_jointmode(s));
	}
	template <any_container Container>
	friend auto operator|(Container&& s, const RangeConversion<Consumer, Args...>& r)
	{
		return r.Exec(detail::GetRange_select_jointmode(s));
	}
	template <traversal_range Range>
	friend auto operator|(Range&& s, RangeConversion<Consumer, Args...>&& r)
	{
		return std::move(r).Exec(std::forward<Range>(s));
	}
	template <traversal_range Range>
	friend auto operator|(Range&& s, const RangeConversion<Consumer, Args...>& r)
	{
		return r.Exec(std::forward<Range>(s));
	}

private:
	std::tuple<Args...> m_args;
};

/*template <any_container Container, template <class...> class View, class ...Args>
auto operator|(Container&& s, RangeConversion<View, Args...>&& r)
{
	return std::move(r).Exec(detail::GetRange_select_jointmode(s));
}
template <any_container Container, template <class...> class View, class ...Args>
auto operator|(Container&& s, const RangeConversion<View, Args...>& r)
{
	return r.Exec(detail::GetRange_select_jointmode(s));
}
template <traversal_range Range, template <class...> class View, class ...Args>
auto operator|(Range&& s, RangeConversion<View, Args...>&& r)
{
	return std::move(r).Exec(std::forward<Range>(s));
}
template <traversal_range Range, template <class...> class View, class ...Args>
auto operator|(Range&& s, const RangeConversion<View, Args...>& r)
{
	return r.Exec(std::forward<Range>(s));
}*/

template <class LayerFixed, class Bpos, class LayerTrav>
struct GetRangeArgs
{
	LayerFixed m_fixed_layer;
	Bpos m_bpos;
	LayerTrav m_trav_layer;
};

template <any_container Container, class LayerFixed, class Bpos_, class LayerTrav>
auto operator|(Container&& s, const GetRangeArgs<LayerFixed, Bpos_, LayerTrav>& r)
{
	//if (r.m_bpos.has_value()) return detail::GetRange_select_jointmode(s, r.m_fixed_layer, r.m_bpos.value(), r.m_trav_layer);
	//else return detail::GetRange_select_jointmode(s, r.m_fixed_layer, r.m_trav_layer);
	if constexpr (std::same_as<LayerFixed, std::nullptr_t>)
	{
		if constexpr (std::same_as<Bpos_, std::nullptr_t>)
		{
			if constexpr (std::same_as<LayerTrav, std::nullptr_t>)
				return detail::GetRange_select_jointmode(s);
			else
				return detail::GetRange_select_jointmode(s, r.m_trav_layer);
		}
		else
		{
			if constexpr (std::same_as<LayerTrav, std::nullptr_t>)
				return detail::GetRange_select_jointmode(s, r.m_bpos);
			else
				return detail::GetRange_select_jointmode(s, r.m_bpos, r.m_trav_layer);
		}
	}
	else
	{
		if constexpr (std::same_as<Bpos_, std::nullptr_t>)
		{
			if constexpr (std::same_as<LayerTrav, std::nullptr_t>)
				return detail::GetRange_select_jointmode(s, r.m_fixed_layer);
			else
				return detail::GetRange_select_jointmode(s, r.m_fixed_layer, r.m_trav_layer);
		}
		else
		{
			if constexpr (std::same_as<LayerTrav, std::nullptr_t>)
				return detail::GetRange_select_jointmode(s, r.m_fixed_layer, r.m_bpos);
			else
				return detail::GetRange_select_jointmode(s, r.m_fixed_layer, r.m_bpos, r.m_trav_layer);
		}
	}
}
template <traversal_range Range, class LayerFixed, class Bpos_, class LayerTrav>
Range operator|(Range&& s, const GetRangeArgs<LayerFixed, Bpos_, LayerTrav>& r)
{
	//if (r.m_bpos.has_value()) s.Assign(r.m_fixed_layer, r.m_bpos.value(), r.m_trav_layer);
	//else s.Assign(r.m_fixed_layer, r.m_trav_layer);
	if constexpr (std::same_as<LayerFixed, std::nullptr_t>)
	{
		if constexpr (std::same_as<Bpos_, std::nullptr_t>)
		{
			if constexpr (std::same_as<LayerTrav, std::nullptr_t>);
			else s.Assign(r.m_trav_layer);
		}
		else
		{
			if constexpr (std::same_as<LayerTrav, std::nullptr_t>) s.Assign(r.m_bpos);
			else s.Assign(r.m_bpos, r.m_trav_layer);
		}
	}
	else
	{
		if constexpr (std::same_as<Bpos_, std::nullptr_t>)
		{
			if constexpr (std::same_as<LayerTrav, std::nullptr_t>) s.Assign(r.m_fixed_layer, s.GetTravLayer());
			else s.Assign(r.m_fixed_layer, r.m_trav_layer);
		}
		else
		{
			if constexpr (std::same_as<LayerTrav, std::nullptr_t>) s.Assign(r.m_fixed_layer, r.m_bpos);
			else s.Assign(r.m_fixed_layer, r.m_bpos, r.m_trav_layer);
		}
	}
	return std::forward<Range>(s);
}

inline GetRangeArgs<LayerType, const Bpos&, LayerType> GetRange(LayerType fix, const Bpos& bpos, LayerType trav)
{
	return { fix, bpos, trav };
}
inline GetRangeArgs<LayerType, const Bpos&, std::nullptr_t> GetRange(LayerType fix, const Bpos& bpos)
{
	return { fix, bpos, nullptr };
}
inline GetRangeArgs<std::nullptr_t, const Bpos&, LayerType> GetRange(const Bpos& bpos, LayerType trav)
{
	return { nullptr, bpos, trav };
}
inline GetRangeArgs<LayerType, std::nullptr_t, LayerType> GetRange(LayerType fix, LayerType trav)
{
	return { fix, nullptr, trav };
}
inline GetRangeArgs<std::nullptr_t, std::nullptr_t, LayerType> GetRange(LayerType trav)
{
	return { nullptr, nullptr, trav };
}

}

#endif
