#ifndef ADAPT_RANGE_FILTER_H
#define ADAPT_RANGE_FILTER_H

#include <ranges>
#include <tuple>
#include <OpenADAPT/Container/Tree.h>
#include <OpenADAPT/Evaluator/FuncNode.h>
#include <OpenADAPT/Range/RangeAdapter.h>

namespace adapt
{

namespace detail
{

template <class Traverser, class Container, class ...NPs>
class CttiFilteredSentinel
{};

template <class Traverser_, class Container_, class ...NPs>
class CttiFilteredTraverser : public Traverser_
{
	using TraverserBase = Traverser_;
	static constexpr LayerType MaxLayer = std::max({ NPs::GetLayer()... });

	//m_traverserの最初の位置が条件を満たしているとは限らないので、
	//条件を満たす最初の位置まで移動する。
	void MoveToFirst()
	{
		LayerType move = CheckConditions<(LayerType)-1>(-1_layer);
		if (move == -2_layer) return;
		IncrDecrOperator(ForwardMovement{}, move);
	}
public:

	//NPs_はtupleにまとめられている。
	template <class NPs_>
	CttiFilteredTraverser(TraverserBase&& t, NPs_ nps)
		: TraverserBase(std::move(t)), m_nps(std::forward<NPs_>(nps))
	{
		m_fixed_layer = TraverserBase::GetFixedLayer();
		m_trav_layer = TraverserBase::GetTravLayer();
		auto init = [this]<class NP_>([[maybe_unused]] NP_ & np)
		{
			if constexpr (any_node<NP_>) np.Init(*this);
			return 0;
		};
		auto init_all = [init]<class ...NPs__>(NPs__ & ...nps_)
		{
			DoNothing(init(nps_)...);
		};
		std::apply(init_all, m_nps);
		MoveToFirst();
	}

private:

	template <LayerType Layer, size_t Index>
	LayerType CheckConditions_rec(LayerType moved) const
	{
		if constexpr (Index >= sizeof...(NPs)) return CheckConditions<LayerType(Layer + 1)>(moved);
		else
		{
			if constexpr (GetType_t<Index, NPs...>::GetLayer() == Layer)
			{
				auto& np = std::get<Index>(m_nps);
				if (!np.Evaluate(*this)) return Layer;
			}
			return CheckConditions_rec<Layer, Index + 1>(moved);
		}
	}
	template <LayerType Layer>
	LayerType CheckConditions(LayerType moved) const
	{
		if constexpr (Layer <= MaxLayer)
		{
			if (Layer < moved) return CheckConditions<Layer + 1>(moved);
			return CheckConditions_rec<Layer, 0>(moved);
		}
		else return -2_layer;
	}

public:

	bool AssignRow(BindexType row)
	{
		if (!TraverserBase::AssignRow(row)) return false;
		LayerType move = CheckConditions<(LayerType)-1>(0_layer);
		if (move == 0_layer || move == -1_layer) return false;
		if (move != -2_layer)
		{
			//条件を満たさなかったので、rowは固定したまま1層以下に限定して満足する要素まで移動する。
			return Move_impl(1_layer, move, ForwardMovement{});
		}
		return true;
	}

private:
	//move層を移動する。
	//移動に失敗した場合はmove - 1層を移動するが、fix - 1層以上は固定し、移動しない。
	template <direction_flag Flag>
	bool Move_impl(LayerType fix, LayerType move, Flag)
	{
		assert(fix >= m_fixed_layer);
		assert(fix <= move);
		while (true)
		{
			if (!TraverserBase::Move(move, Flag{}))
			{
				//move層の移動に失敗したので、move-1層を移動する。
				//ただしfix>moveとなることは禁止されているので、その場合はreturn false。
				if (fix >= move) return false;
				--move;
				continue;
			}

			//move層の移動に成功した場合、次はmove層以下の条件が満足されているかを確認する。
			move = CheckConditions<(LayerType)-1>(move);
			if (move == -2_layer) return true;
			//fixより上層の移動は許さない。
			if (move < fix) return false;
		}
	}
public:
	template <direction_flag Flag>
	bool Move(LayerType layer, Flag)
	{
		return Move_impl(layer, layer, Flag{});
	}

	//layer層を移動する。
	bool MoveForward(LayerType layer)
	{
		return Move(layer, ForwardMovement{});
	}
	bool MoveBackward(LayerType layer)
	{
		return Move(layer, BackwardMovement{});
	}

private:
	//move層から移動する。
	template <direction_flag Flag>
	LayerType IncrDecrOperator(Flag, LayerType move)
	{
		LayerType res = move;
		while (true)
		{
			if (!TraverserBase::Move(move, Flag{}))
			{
				//move層の移動に失敗したので、ひとつ上の層の移動を試みる。
				//ただし、fixed + 1層の移動まで終えてしまった場合は
				//traverserをendに移動させ終了する。
				//Backwardの場合は移動に失敗した時点で最初の要素を指しているはずなので、MoveToBeginは不要。
				--move;
				if (move == m_fixed_layer)
				{
					if constexpr (std::same_as<Flag, ForwardMovement>) TraverserBase::MoveToEnd();
					return m_fixed_layer;
				}
				continue;
			}
			res = std::min(move, res);
			move = CheckConditions<(LayerType)-1>(move);
			if (move == -2_layer) return res;
		}
		throw Forbidden("Thrown from FilteredTraverser::IncrDecrOperator. THIS IS A BUG.");
	}
public:

	template <direction_flag Flag>
	LayerType IncrDecrOperator(Flag)
	{
		return IncrDecrOperator(Flag{}, m_trav_layer);
	}
	LayerType Incr() { return IncrDecrOperator(ForwardMovement{}); }
	LayerType Decr() { return IncrDecrOperator(BackwardMovement{}); }
	CttiFilteredTraverser& operator++()
	{
		Incr();
		return *this;
	}
	CttiFilteredTraverser& operator--()
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

	const CttiFilteredTraverser& operator*() const { return *this; }
	const CttiFilteredTraverser* operator->() const { return this; }

private:
	LayerType m_fixed_layer = -1_layer;
	LayerType m_trav_layer = -1_layer;
	std::tuple<NPs...> m_nps;
};

template <class Traverser, class Container, class ...NPs>
bool operator==(const CttiFilteredTraverser<Traverser, Container, NPs...>& t, const CttiFilteredSentinel<Traverser, Container, NPs...>&)
{
	return t.IsEnd();
}
template <class Traverser, class Container, class ...NPs>
bool operator!=(const CttiFilteredTraverser<Traverser, Container, NPs...>& t, const CttiFilteredSentinel<Traverser, Container, NPs...>&)
{
	return !t.IsEnd();
}
template <class Traverser, class Container, class ...NPs>
bool operator==(const CttiFilteredSentinel<Traverser, Container, NPs...>&, const CttiFilteredTraverser<Traverser, Container, NPs...>& t)
{
	return t.IsEnd();
}
template <class Traverser, class Container, class ...NPs>
bool operator!=(const CttiFilteredSentinel<Traverser, Container, NPs...>&, const CttiFilteredTraverser<Traverser, Container, NPs...>& t)
{
	return !t.IsEnd();
}


template <class Range_, node_or_placeholder ...NPs>
class CttiFilterView
	: public std::ranges::view_interface<CttiFilterView<Range_, NPs...>>
{
	using Range = std::decay_t<Range_>;
public:

	using Container = Range::Container;
	using Traverser = CttiFilteredTraverser<typename Range::Traverser, Container, NPs...>;
	using ConstTraverser = CttiFilteredTraverser<typename Range::ConstTraverser, Container, NPs...>;
	using Sentinel = CttiFilteredSentinel<typename Range::Traverser, Container, NPs...>;
	using ConstSentinel = CttiFilteredSentinel<typename Range::ConstTraverser, Container, NPs...>;
	static constexpr LayerType MaxLayer = std::max({ NPs::GetLayer()... });

	static constexpr bool IsConst = Range::IsConst;

	template <class Range__, node_or_placeholder ...NPs_>
	CttiFilterView(Range__&& r, NPs_&&... nps)
		: m_range(std::forward<Range__>(r)), m_conds(std::forward<NPs_>(nps)...)
	{
		if (MaxLayer > GetTravLayer()) SetTravLayer(MaxLayer);
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
		return Traverser(m_range.begin(), m_conds);
	}
	ConstTraverser begin() const requires IsConst
	{
		return cbegin();
	}
	ConstTraverser cbegin() const
	{
		return ConstTraverser(m_range.cbegin(), m_conds);
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
	std::tuple<NPs...> m_conds;
};


template <class Traverser, class Container>
class RttiFilteredSentinel
{};

template <class Traverser_, class Container_>
class RttiFilteredTraverser : public Traverser_
{
public:

	using Container = Container_;
	using TraverserBase = Traverser_;

	using Sentinel = RttiFilteredSentinel<TraverserBase, Container>;

	using iterator_category = std::input_iterator_tag;
	using difference_type = ptrdiff_t;
	using reference = RttiFilteredTraverser&;
	using pointer = RttiFilteredTraverser*;
	using value_type = RttiFilteredTraverser;

private:
	void Reserve(std::vector<size_t>& count, LayerType max,
				 std::vector<eval::RttiFuncNode<Container>>::const_iterator it,
				 std::vector<eval::RttiFuncNode<Container>>::const_iterator end)
	{
		if (it != end)
		{
			LayerType layer = it->GetLayer();
			size_t index = count[layer + 1_layer]++;
			max = std::max(layer, max);
			Reserve(count, max, it + 1, end);
			m_nodes[layer + 1_layer][index] = *it;
			m_nodes[layer + 1_layer][index].Init(*this);
		}
		else
		{
			m_nodes.resize(max + 2);
			for (auto [n, c] : views::Zip(m_nodes, count)) n.resize(c);
		}
	}

	//m_traverserの最初の位置が条件を満たしているとは限らないので、
	//条件を満たす最初の位置まで移動する。
	void MoveToFirst()
	{
		LayerType move = CheckConditions(-1_layer);
		if (move == -2_layer) return;
		IncrDecrOperator(ForwardMovement{}, move);
	}
public:

	RttiFilteredTraverser(TraverserBase&& t, const std::vector<eval::RttiFuncNode<Container>>& nps)
		: TraverserBase(std::move(t))
	{
		m_trav_layer = this->GetTravLayer();
		std::vector<size_t> count(m_trav_layer + 2, 0);
		Reserve(count, -1_layer, nps.begin(), nps.end());
		m_fixed_layer = TraverserBase::GetFixedLayer();
		MoveToFirst();
	}

private:
	//layer層以下の条件を全て判定する。
	//戻り値は、全条件を満足した場合は-2。そうでない場合は、条件を満足しなかった最上層。
	LayerType CheckConditions(LayerType moved) const
	{
		LayerType l = moved;
		//m_nodes.begin()+moveがmove層の条件。
		if (std::cmp_greater_equal(moved + 1, m_nodes.size())) return -2_layer;
		for (auto conds = m_nodes.begin() + (moved + 1); conds != m_nodes.end(); ++conds, ++l)
		{
			for (auto cit = conds->begin(); cit != conds->end(); ++cit)
			{
				//nodeは1バイト整数、すなわち比較演算子などの戻り値であると想定する。
				try
				{
					//条件を満たさなかった場合、l層をもう一度移動する。
					if (!cit->Evaluate(*this, Number<FieldType::I08>{})) return l;
				}
				catch (const JointError&) { return l; }
				catch (const NoElements&) { return l; }
			}
		}
		return -2_layer;
	}

public:

	bool AssignRow(BindexType row)
	{
		if (!TraverserBase::AssignRow(row)) return false;
		LayerType move = CheckConditions(0_layer);
		if (move == 0_layer || move == -1_layer) return false;//-層または0層の条件を満たさない場合、これ以上移動できないのでfalseを返す。
		if (move != -2_layer)
		{
			//条件を満たさなかったので、rowは固定したまま1層以下に限定して満足する要素まで移動する。
			return Move_impl(1_layer, move, ForwardMovement{});
		}
		return true;
	}

private:
	//move層を移動する。
	//移動に失敗した場合は--moveして上の層を移動する。
	//ただしfix - 1層以上は固定し、移動しない。
	template <direction_flag Flag>
	bool Move_impl(LayerType fix, LayerType move, Flag)
	{
		assert(fix >= m_fixed_layer);
		assert(fix <= move);
		while (true)
		{
			if (!TraverserBase::Move(move, Flag{}))
			{
				//move層の移動に失敗したので、move-1層を移動する。
				//ただしfix>moveとなることは禁止されているので、その場合はreturn false。
				if (fix >= move) return false;
				--move;
				continue;
			}

			//move層の移動に成功した場合、次はmove層以下の条件が満足されているかを確認する。
			move = CheckConditions(move);
			if (move == -2_layer) return true;
			//fixより上層の移動は許さない。
			if (move < fix) return false;
		}
	}
public:

	template <direction_flag Flag>
	bool Move(LayerType layer, Flag)
	{
		return Move_impl(layer, layer, Flag{});
	}

	//layer層を移動する。
	bool MoveForward(LayerType layer)
	{
		return Move(layer, ForwardMovement{});
	}
	bool MoveBackward(LayerType layer)
	{
		return Move(layer, BackwardMovement{});
	}

private:
	//move層から移動する。
	template <direction_flag Flag>
	LayerType IncrDecrOperator(Flag, LayerType move)
	{
		LayerType res = move;
		while (true)
		{
			if (!TraverserBase::Move(move, Flag{}))
			{
				//move層の移動に失敗したので、ひとつ上の層の移動を試みる。
				//ただし、fixed + 1層の移動まで終えてしまった場合は
				//traverserをendに移動させ終了する。
				//Backwardの場合は移動に失敗した時点で最初の要素を指しているはずなので、MoveToBeginは不要。
				--move;
				if (move == m_fixed_layer)
				{
					if constexpr (std::same_as<Flag, ForwardMovement>) TraverserBase::MoveToEnd();
					return m_fixed_layer;
				}
				continue;
			}
			res = std::min(move, res);
			move = CheckConditions(move);
			if (move == -2_layer) return res;
		}
		throw Forbidden("Thrown from FilteredTraverser::IncrDecrOperator. THIS IS A BUG.");
	}
public:

	template <direction_flag Flag>
	LayerType IncrDecrOperator(Flag)
	{
		return IncrDecrOperator(Flag{}, m_trav_layer);
	}
	LayerType Incr() { return IncrDecrOperator(ForwardMovement{}); }
	LayerType Decr() { return IncrDecrOperator(BackwardMovement{}); }
	RttiFilteredTraverser& operator++()
	{
		Incr();
		return *this;
	}
	RttiFilteredTraverser& operator--()
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

	const RttiFilteredTraverser& operator*() const { return *this; }
	const RttiFilteredTraverser* operator->() const { return this; }

private:

	//fix, travの値はメンバとして持っておく。
	//JoinedTraverserはこれらの値の取得にいちいち計算が必要なため。
	LayerType m_fixed_layer = -1_layer;
	LayerType m_trav_layer = -1_layer;
	//レイヤーごとに分けられたノード。m_nodes[layer + 1]がlayer層のノードリスト。
	std::vector<std::vector<eval::RttiFuncNode<Container>>> m_nodes;
};

template <class Traverser, class Container>
bool operator==(const RttiFilteredTraverser<Traverser, Container>& t, const RttiFilteredSentinel<Traverser, Container>&)
{
	return t.IsEnd();
}
template <class Traverser, class Container>
bool operator!=(const RttiFilteredTraverser<Traverser, Container>& t, const RttiFilteredSentinel<Traverser, Container>&)
{
	return !t.IsEnd();
}
template <class Traverser, class Container>
bool operator==(const RttiFilteredSentinel<Traverser, Container>&, const RttiFilteredTraverser<Traverser, Container>& t)
{
	return t.IsEnd();
}
template <class Traverser, class Container>
bool operator!=(const RttiFilteredSentinel<Traverser, Container>&, const RttiFilteredTraverser<Traverser, Container>& t)
{
	return !t.IsEnd();
}

//cttiでない条件が一つでも含まれる場合、こちらになる。
template <class Range_, node_or_placeholder ...NPs>
class RttiFilterView
	: public std::ranges::view_interface<RttiFilterView<Range_, NPs...>>
{
	using Range = std::decay_t<Range_>;
public:

	using Container = Range::Container;
	using Traverser = RttiFilteredTraverser<typename Range::Traverser, Container>;
	using ConstTraverser = RttiFilteredTraverser<typename Range::ConstTraverser, Container>;
	using Sentinel = RttiFilteredSentinel<typename Range::Traverser, Container>;
	using ConstSentinel = RttiFilteredSentinel<typename Range::ConstTraverser, Container>;

	static constexpr bool IsConst = Range::IsConst;

	template <class Range__, node_or_placeholder ...NPs_>
	RttiFilterView(Range__&& r, NPs_&&... nps)
		: m_range(std::forward<Range__>(r)), m_conds({ ConvertToRttiFuncNode(std::forward<NPs_>(nps))... })
	{
		LayerType max = std::ranges::max(std::views::all(m_conds) | std::views::transform([](const auto& c) { return c.GetLayer(); }));
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
		return Traverser(m_range.begin(), m_conds);
	}
	ConstTraverser begin() const requires IsConst
	{
		return cbegin();
	}
	ConstTraverser cbegin() const
	{
		return ConstTraverser(m_range.cbegin(), m_conds);
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
	std::vector<eval::RttiFuncNode<Container>> m_conds;
};

}

template <ctti_node_or_placeholder ...NPs>
RangeReceiver<detail::CttiFilterView, std::decay_t<NPs>...> Filter(NPs&& ...nps)
{
	return RangeReceiver<detail::CttiFilterView, std::decay_t<NPs>...>(std::forward<NPs>(nps)...);
}

template <node_or_placeholder ...NPs>
	requires (!(ctti_node_or_placeholder<NPs> && ...))
RangeReceiver<detail::RttiFilterView, std::decay_t<NPs>...> Filter(NPs&& ...nps)
{
	return RangeReceiver<detail::RttiFilterView, std::decay_t<NPs>...>(std::forward<NPs>(nps)...);
}

}

#endif
