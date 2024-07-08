#ifndef ADAPT_RANGE_EVALUATE_H
#define ADAPT_RANGE_EVALUATE_H

#include <OpenADAPT/Container/Tree.h>
#include <OpenADAPT/Common/Concepts.h>
#include <OpenADAPT/Range/RangeAdapter.h>

namespace adapt
{

namespace detail
{

template <class Traverser_, class WithTrav, node_or_placeholder ...NPs>
class EvaluatingSentinel_impl
{};

template <class Traverser_, class WithTrav, node_or_placeholder ...NPs>
class EvaluatingTraverser : public Traverser_
{
	using TraverserBase = Traverser_;
	using EvaluatingSentinel = EvaluatingSentinel_impl<TraverserBase, WithTrav, NPs...>;

	template <node_or_placeholder NP>
	struct LayerSD_impl { using Type = LayerType; };
	template <ctti_node_or_placeholder NP>
	struct LayerSD_impl<NP> { using Type = LayerConstant<NP::GetLayer()>; };
	template <class NP>
	using LayerSD = LayerSD_impl<NP>::Type;

	template <class RetType>
	struct Result
	{
		void Set(RetType v) { value = v; }
		RetType Get() const { return value; }
		RetType value;
	};
	template <class RetType>
		requires (!std::is_trivially_copyable_v<RetType>)
	struct Result<RetType>
	{
		void Set(const RetType& v) { value = &v; }
		const RetType& Get() const { return *value; }
		const RetType* value;
	};
	template <class NP>
	struct NPResult;
	template <stat_type_node_or_placeholder NP>
	struct NPResult<NP>
	{
		using RetType = NP::RetType;

		template <class NP_>
		NPResult(NP_&& np)
			: m_np(std::forward<NP_>(np))
		{
			if constexpr (!ctti_node_or_placeholder<NP_>) m_layer = m_np.GetLayer();
		}
		template <class Trav>
		decltype(auto) Evaluate(const Trav& t)
		{
			if (m_done) return m_result.Get();
			m_result.Set(m_np.Evaluate(t));
			m_done = true;
			return m_result.Get();
		}
		void Refresh(LayerType layer) { m_done = layer > m_layer; }
		NP m_np;
		bool m_done = false;
		[[no_unique_address]] LayerSD<NP> m_layer = {};
		Result<RetType> m_result = {};
	};
	template <node_or_placeholder NP>
	struct NPResult<NP>
	{
		template <class NP_>
		NPResult(NP_&& np)
			: m_np(std::forward<NP_>(np))
		{
			m_layer = m_np.GetLayer();
		}
		template <class Trav>
		const FieldVariant& Evaluate(const Trav& t)
		{
			if (m_done) return m_result;
			m_result = m_np.Evaluate(t);
			m_done = true;
			return m_result;
		}
		void Refresh(LayerType layer) { m_done = layer > m_layer; }
		NP m_np;
		bool m_done = false;
		[[no_unique_address]] LayerSD<NP> m_layer = {};
		FieldVariant m_result = {};
	};

	using Tuple = std::tuple<decltype(std::declval<NPResult<NPs>>().Evaluate(std::declval<TraverserBase>()))...>;;
	template <class T, class Tuple>
	struct TupleAdd;
	template <class T, class ...Ts>
	struct TupleAdd<T, std::tuple<Ts...>> { using Type = std::tuple<T, Ts...>; };
public:

	using iterator_category = std::input_iterator_tag;
	using difference_type = ptrdiff_t;
	using value_type = std::conditional_t<WithTrav::value, typename TupleAdd<const EvaluatingTraverser&, Tuple>::Type, Tuple>;


	//using reference = ;
	//using pointer = RttiFilteredTraverser*;
	//NPs_はtupleにまとめられている。
	template <class NPs_>
	EvaluatingTraverser(TraverserBase&& t, NPs_&& nps)
		: TraverserBase(std::move(t)), m_nps(std::forward<NPs_>(nps))
	{
		auto init = [this]<class NP_>(NP_ & np)
		{
			if constexpr (any_node<NP_>) np.Init(*this);
			return 0;
		};
		auto init_all = [init]<class ...NPResults_>(NPResults_ & ...nps_)
		{
			DoNothing(init(nps_.m_np)...);
		};
		std::apply(init_all, m_nps);
	}

private:
	template <direction_flag Flag>
	bool Move(LayerType layer, Flag)
	{
		if (!TraverserBase::Move(layer)) return false;
		auto refresh = [layer]<class ...NPs_>(NPs_& ...nps)
		{
			DoNothing((nps.Refresh(layer), 0)...);
		};
		std::apply(refresh, m_nps);
		return true;
	}
public:
	bool MoveForward(LayerType layer)
	{
		return Move(layer, ForwardFlag{});
	}
	bool MoveBackward(LayerType layer)
	{
		return Move(layer, BackwardFlag{});
	}

private:
	template <direction_flag Flag>
	LayerType IncrDecrOperator(Flag)
	{
		LayerType moved = TraverserBase::IncrDecrOperator(Flag{});
		auto refresh = [moved]<class ...NPs_>(NPs_& ...nps)
		{
			DoNothing((nps.Refresh(moved), 0)...);
		};
		std::apply(refresh, m_nps);
		return moved;
	}
public:
	LayerType Incr()
	{
		return IncrDecrOperator(ForwardFlag{});
	}
	LayerType Decr()
	{
		return IncrDecrOperator(BackwardFlag{});
	}
	EvaluatingTraverser& operator++()
	{
		Incr();
		return *this;
	}
	EvaluatingTraverser& operator--()
	{
		Decr();
		return *this;
	}
	void operator++(int)
	{
		++(*this);
	}
	void operator--(int)
	{
		--(*this);
	}

	decltype(auto) operator*() const
	{
		auto conv = [this]<class ...NPs_>(NPs_& ...nps)
		{
			if constexpr (WithTrav::value)
				return value_type(*this, nps.Evaluate(*this)...);
			else
				return value_type(nps.Evaluate(*this)...);
		};
		return std::apply(conv, m_nps);
	}

	friend bool operator==(const EvaluatingTraverser& t, const EvaluatingSentinel&)
	{
		return t.IsEnd();
	}
	friend bool operator!=(const EvaluatingTraverser& t, const EvaluatingSentinel&)
	{
		return !t.IsEnd();
	}
	friend bool operator==(const EvaluatingSentinel&, const EvaluatingTraverser& t)
	{
		return t.IsEnd();
	}
	friend bool operator!=(const EvaluatingSentinel&, const EvaluatingTraverser& t)
	{
		return !t.IsEnd();
	}
private:
	mutable std::tuple<NPResult<NPs>...> m_nps;
};

template <class Traverser_, class WithTrav, node_or_placeholder NP>
class EvaluatingTraverser<Traverser_, WithTrav, NP> : public Traverser_
{
	using TraverserBase = Traverser_;
	using EvaluatingSentinel = EvaluatingSentinel_impl<TraverserBase, WithTrav, NP>;

	template <class> struct RetType;
	template <node_or_placeholder NP_> struct RetType<NP_>
	{
		using Type = FieldVariant;
	};
	template <stat_type_node_or_placeholder NP_>
	struct RetType<NP_>
	{
		using Ret = std::decay_t<typename NP_::RetType>;
		using Type = std::conditional_t<std::is_trivially_copyable_v<Ret>, Ret, const Ret&>;
	};

public:

	using iterator_category = std::input_iterator_tag;
	using difference_type = ptrdiff_t;
	//using reference = ;
	//using pointer = RttiFilteredTraverser*;
	using value_type = std::conditional_t<WithTrav::value,
		std::tuple<const EvaluatingTraverser&, typename RetType<NP>::Type>,
		typename RetType<NP>::Type>;

	//NPs_はtupleにまとめられている。
	template <class NP_>
	EvaluatingTraverser(TraverserBase&& t, NP_&& np)
		: TraverserBase(std::move(t)), m_np(std::get<0>(std::forward<NP_>(np)))
	{
		if constexpr (any_node<NP>) m_np.Init(*this);
	}

	EvaluatingTraverser& operator++()
	{
		TraverserBase::operator++();
		return *this;
	}
	EvaluatingTraverser& operator--()
	{
		TraverserBase::operator--();
		return *this;
	}
	void operator++(int)
	{
		++(*this);
	}
	void operator--(int)
	{
		--(*this);
	}

	value_type operator*() const
	{
		if constexpr (WithTrav::value)
			return value_type(*this, m_np.Evaluate(*this));
		else
			return value_type(m_np.Evaluate(*this));
	}

	friend bool operator==(const EvaluatingTraverser& t, const EvaluatingSentinel&)
	{
		return t.IsEnd();
	}
	friend bool operator!=(const EvaluatingTraverser& t, const EvaluatingSentinel&)
	{
		return !t.IsEnd();
	}
	friend bool operator==(const EvaluatingSentinel&, const EvaluatingTraverser& t)
	{
		return t.IsEnd();
	}
	friend bool operator!=(const EvaluatingSentinel&, const EvaluatingTraverser& t)
	{
		return !t.IsEnd();
	}

private:
	NP m_np;
};


template <class Range_, class WithTrav, node_or_placeholder ...NPs>
class EvaluatingView
	: public std::ranges::view_interface<EvaluatingView<Range_, NPs...>>
{
	using Range = std::decay_t<Range_>;
public:

	using Container = Range::Container;
	using Traverser = EvaluatingTraverser<typename Range::Traverser, WithTrav, NPs...>;
	using ConstTraverser = EvaluatingTraverser<typename Range::ConstTraverser, WithTrav, NPs...>;
	using Sentinel = EvaluatingSentinel_impl<typename Range::Traverser, WithTrav, NPs...>;
	using ConstSentinel = EvaluatingSentinel_impl<typename Range::ConstTraverser, WithTrav, NPs...>;

	static constexpr bool IsConst = Range::IsConst;

	template <class Range__, node_or_placeholder ...NPs_>
	EvaluatingView(Range__&& r, NPs_&& ...nps)
		: m_range(std::forward<Range__>(r)), m_nps(std::forward<NPs>(nps)...)
	{
		auto get_max = []<class ...X>(const X& ...x) { return std::max({ x.GetLayer()... }); };
		LayerType max = std::apply(get_max, m_nps);
		if (max > GetTravLayer()) SetTravLayer(max);
	}

	Range& base() const { return m_range; }

	LayerType GetFixedLayer() const { return m_range.GetFixedLayer(); }
	LayerType GetTravLayer() const { return m_range.GetTravLayer(); }
	void SetTravLayer(LayerType layer) { m_range.SetTravLayer(layer); }

	void Assign(LayerType fix, const Bpos& bpos, LayerType trav = -1_layer) { m_range.Assign(fix, bpos, trav); }
	void Assign(const Bpos& bpos, LayerType trav = -1_layer) { m_range.Assign(-1_layer, bpos, trav); }
	void Assign(LayerType fix, LayerType trav) { m_range.Assign(fix, trav); }
	void Assign(LayerType trav) { m_range.Assign(trav); }

	Traverser begin() const requires (!IsConst)
	{
		return Traverser(m_range.begin(), m_nps);
	}
	ConstTraverser begin() const requires IsConst
	{
		return cbegin();
	}
	ConstTraverser cbegin() const
	{
		return ConstTraverser(m_range.cbegin(), m_nps);
	}
	Sentinel end() const requires (!IsConst)
	{
		return {};
	}
	ConstSentinel end() const requires IsConst
	{
		return cend();
	}
	ConstSentinel cend() const
	{
		return {};
	}

	const Container& GetContainer() const { return m_range.GetContainer(); }

private:

	Range_ m_range;
	std::tuple<NPs...> m_nps;
};

template <class Range, node_or_placeholder ...NPs>
using EvaluatingViewWithoutTrav = EvaluatingView<Range, std::false_type, NPs...>;
template <class Range, node_or_placeholder ...NPs>
using EvaluatingViewWithTrav = EvaluatingView<Range, std::true_type, NPs...>;
}

template <node_or_placeholder ...NPs>
RangeAdapter<detail::EvaluatingViewWithoutTrav, std::decay_t<NPs>...> Evaluate(NPs&& ...nps)
{
	return RangeAdapter<detail::EvaluatingViewWithoutTrav, std::decay_t<NPs>...>(std::forward<NPs>(nps)...);
}
template <node_or_placeholder ...NPs>
RangeAdapter<detail::EvaluatingViewWithTrav, std::decay_t<NPs>...> Evaluate(WithTraverser, NPs&& ...nps)
{
	return RangeAdapter<detail::EvaluatingViewWithTrav, std::decay_t<NPs>...>(std::forward<NPs>(nps)...);
}

}

#endif
