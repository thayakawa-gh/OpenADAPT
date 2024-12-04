#ifndef ADAPT_RANGE_TO_H
#define ADAPT_RANGE_TO_H

#include <OpenADAPT/Range/RangeAdapter.h>
#include <OpenADAPT/Range/Evaluate.h>

namespace adapt
{

namespace detail
{

template <template <class...> class Arr, class Range>
class ToContainer
{
	template <size_t Index, class ...Values, class ...ResTypes>
	void PushBack(std::tuple<Values...> t, std::tuple<Arr<ResTypes>...>& res)
	{
		if constexpr (Index < sizeof...(Values))
		{
			std::get<Index>(res).emplace_back(std::get<Index>(t));
			return PushBack<Index + 1>(t, res);
		}
	}

	template <traversal_range Range_, stat_type_node_or_placeholder ...NPs_>
	auto Exec_complex(Range_&& range, NPs_&& ...nps)
	{
		auto erange = std::forward<Range_>(range) | Evaluate(std::forward<NPs_>(nps)...);
		std::tuple<std::vector<typename std::decay_t<NPs_>::RetType>...> res;
		for (auto&& t : erange) PushBack<0>(t, res);
		return res;
	}
	template <traversal_range Range_, stat_type_node_or_placeholder ...NPs_>
	auto Exec_complex(Range_&& range, Combine, NPs_&& ...nps)
	{
		auto erange = std::forward<Range_>(range) | Evaluate(std::forward<NPs_>(nps)...);
		using Type = std::tuple<typename std::decay_t<NPs_>::RetType...>;
		std::vector<Type> res;
		std::ranges::copy(erange, std::back_inserter(res));
		return res;
	}

	template <traversal_range Range_, stat_type_node_or_placeholder NP>
	auto Exec_simplex(Range_&& range, NP&& np)
	{
		auto erange = std::forward<Range_>(range) | Evaluate(std::forward<NP>(np));
		std::vector<std::decay_t<typename std::decay_t<NP>::RetType>> res;
		std::ranges::copy(erange, std::back_inserter(res));
		return res;
	}

public:

	template <traversal_range Range_, stat_type_node_or_placeholder NP>
	auto Exec(Range_&& range, NP&& np)
	{
		return Exec_simplex(std::forward<Range_>(range), std::forward<NP>(np));
	}
	template <traversal_range Range_, stat_type_node_or_placeholder NP>
	auto Exec(Range_&& range, Combine, NP&& np)
	{
		//simplex、つまりNP1個だけの場合、Combineの有無は関係ない。
		return Exec_simplex(std::forward<Range_>(range), std::forward<NP>(np));
	}

	template <traversal_range Range_, stat_type_node_or_placeholder ...NPs>
	auto Exec(Range_&& range, NPs&& ...nps)
	{
		return Exec_complex(std::forward<Range_>(range), std::forward<NPs>(nps)...);
	}
	template <traversal_range Range_, stat_type_node_or_placeholder ...NPs>
	auto Exec(Range_&& range, Combine c, NPs&& ...nps)
	{
		return Exec_complex(std::forward<Range_>(range), c, std::forward<NPs>(nps)...);
	}

};

template <class Range>
using ToVectorConversion = ToContainer<std::vector, Range>;

}

template <stat_type_node_or_placeholder ...NPs>
auto ToVector(Combine, NPs&& ...nps)
{
	return RangeConversion<detail::ToVectorConversion, NPs...>(std::forward<NPs>(nps)...);
}
template <stat_type_node_or_placeholder ...NPs>
auto ToVector(NPs&& ...nps)
{
	return RangeConversion<detail::ToVectorConversion, NPs...>(std::forward<NPs>(nps)...);
}

}

#endif