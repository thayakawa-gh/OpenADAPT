#ifndef ADAPT_RANGE_MAKEHASHTABLE_H
#define ADAPT_RANGE_MAKEHASHTABLE_H

#include <OpenADAPT/Range/RangeAdapter.h>
#include <OpenADAPT/Range/Evaluate.h>

namespace adapt
{

namespace detail
{

template <class Range>
class HashingConsumer
{
	template <class ...NPs>
	struct HashtableType { using Type = Hashtable<FirstType<FieldVariant, NPs>...>; };
	template <stat_type_node_or_placeholder ...NPs>
	struct HashtableType<NPs...> { using Type = Hashtable<typename std::decay_t<NPs>::RetType...>; };

public:

	template <traversal_range Range_, node_or_placeholder ...NPs>
	auto Exec(Range_&& range, NPs&& ...nps)
	{
		using Result = HashtableType<NPs...>::Type;

		auto erange = std::forward<Range_>(range) | Evaluate(flags::with_traverser, std::forward<NPs>(nps)...);
		Result res;
		Bpos bpos(erange.GetTravLayer());
		auto emplace = [&bpos, &res]<class Trav, class ...Vs>(const Trav& t, Vs&& ...vs)
		{
			t.GetBpos(bpos);
			res.emplace(std::piecewise_construct, std::forward_as_tuple(std::forward<Vs>(vs)...), std::forward_as_tuple(bpos));
		};
		for (auto&& t : erange)
		{
			std::apply(emplace, std::forward<decltype(t)>(t));
		}
		return res;
	}

};

}

template <node_or_placeholder ...NPs>
auto MakeHashtable(NPs&& ...nps)
{
	return RangeConsumer<detail::HashingConsumer, NPs...>(std::forward<NPs>(nps)...);
}

}

#endif