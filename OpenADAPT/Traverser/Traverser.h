#ifndef ADAPT_TRAVERSER_TRAVERSER_H
#define ADAPT_TRAVERSER_TRAVERSER_H

#include <ranges>
#include <OpenADAPT/Utility/Exception.h>
#include <OpenADAPT/Utility/Ranges.h>
#include <OpenADAPT/Common/Bpos.h>
#include <OpenADAPT/Evaluator/Placeholder.h>
#include <OpenADAPT/Container/ElementRef.h>

namespace adapt
{

namespace detail
{

template <class Tree,
	template <class, template <class> class> class Iterator,
	template <class> class Qualifier>
class Sentinel_impl
{
};

template <class Container_,
	template <class, template <class> class> class Iterator,
	template <class> class Qualifier>
class Traverser_impl
{
	//階層構造コンテナをイテレータのように走査するためのもの。
	//各層のイテレータを束ねた構造となっている。
	//std::input_iteratorのコンセプトを満たすが、実際にはstd::bidirectional_iteratorに近い動作が可能。

	//固定層と走査層の概念がある。
	//1. 固定層(m_fixed_layer)以上の位置情報は固定され、それより下の層のみを走査する。
	//2. 走査するのは走査層(m_trav_layer)基準である。固定層<layer<=走査層の範囲で空の要素がある場合、その要素はスキップされる。

	//末尾の状態について少し仕様を纏めておく。
	//end()によって得られる末尾は、走査範囲、つまりm_fixed_layerまでの位置情報が固定された状態での最終要素の一つ次を表す。
	//m_fixed_layer + 1層のイテレータはm_fixed_layer + 1層末尾(mIterator[m_fixed_layer + 1]->end())を指す。
	//m_fixed_layer + 2層以下では、各イテレータの位置は不定であり、直前に呼び出した関数によって変わる。
	//したがって、末尾状態はm_fixed_layer + 1層のイテレータが末尾要素を指しているかで判定することができる。
	//m_fixed_layer + 2以下は、MoveForward、Backwardなどとの兼ね合いで
	//末尾と見做すことのできる要素が存在しないため、このような仕様になっている。

	//なので、実のところ、このイテレータの末尾判定は末尾位置を指すTraverserを必要としない。
	//すなわちSentinelは一切情報を持たない空クラスでもよい。

public:

	using iterator_category = std::input_iterator_tag;
	using difference_type = ptrdiff_t;
	using reference = Traverser_impl&;
	using pointer = Traverser_impl*;
	using value_type = Traverser_impl;

	using Container = Container_;
	using Hierarchy = Container::Hierarchy;
	using FieldRef = FieldRef_impl<Qualifier>;
	using ElementIterator = Iterator<Hierarchy, Qualifier>;
	using ElementRef = ElementRef_impl<Hierarchy, Qualifier, LayerType>;
	using ElementBlock = ElementRef::ElementBlock;
	using Sentinel = Sentinel_impl<Container, Iterator, Qualifier>;

	using RttiPlaceholder = Hierarchy::RttiPlaceholder;
	template <class Type>
	using TypedPlaceholder = Hierarchy::template TypedPlaceholder<Type>;
	template <LayerType Layer, class Type>
	using CttiPlaceholder = Hierarchy::template CttiPlaceholder<Layer, Type>;
private:


	template <class Flag>
	constexpr bool IsFlag() { return std::is_same_v<Flag, ForwardFlag> || std::is_same_v<Flag, BackwardFlag>; }

public:
	Traverser_impl()
		: m_fixed_layer((LayerType)-1), m_trav_layer((LayerType)-1), m_container(nullptr)
	{}

	Traverser_impl(LayerType fixed, Qualifier<Container>& tree, LayerType trav, ForwardFlag = {})
		: Traverser_impl()
	{
		Init(fixed, tree, trav, ForwardFlag{});
	}
	Traverser_impl(Qualifier<Container>& c, LayerType trav, ForwardFlag = {})
		: Traverser_impl((LayerType)-1, c, trav, ForwardFlag{})
	{}
	Traverser_impl(LayerType fixed, Qualifier<Container>& c, ForwardFlag = {})
		: Traverser_impl(fixed, c, c.GetMaxLayer(), ForwardFlag{})
	{}
	Traverser_impl(Qualifier<Container>& c, ForwardFlag = {})
		: Traverser_impl((LayerType)-1, c, c.GetMaxLayer(), ForwardFlag{})
	{}
	Traverser_impl(LayerType fixed, Qualifier<Container>& c, LayerType trav, BackwardFlag)
		: Traverser_impl()
	{
		Init(fixed, c, trav, BackwardFlag{});
	}
	Traverser_impl(Qualifier<Container>& c, LayerType trav, BackwardFlag)
		: Traverser_impl((LayerType)-1, c, trav, BackwardFlag{})
	{}
	Traverser_impl(LayerType fixed, Qualifier<Container>& c, BackwardFlag)
		: Traverser_impl(fixed, c, c.GetMaxLayer(), BackwardFlag{})
	{}
	Traverser_impl(Qualifier<Container>& c, BackwardFlag)
		: Traverser_impl((LayerType)-1, c, c.GetMaxLayer(), BackwardFlag{})
	{}

	Traverser_impl(LayerType fixed, Qualifier<Container>& c, const Bpos& pos, LayerType trav, ForwardFlag = {})
		: Traverser_impl()
	{
		Init(fixed, c, pos, trav, ForwardFlag{});
	}
	Traverser_impl(Qualifier<Container>& c, const Bpos& pos, LayerType trav, ForwardFlag = {})
		: Traverser_impl((LayerType)-1, c, pos, trav, ForwardFlag{})
	{}
	Traverser_impl(LayerType fixed, Qualifier<Container>& c, const Bpos& pos, ForwardFlag = {})
		: Traverser_impl(fixed, c, pos, c.GetMaxLayer(), ForwardFlag{})
	{}
	Traverser_impl(Qualifier<Container>& c, const Bpos& pos, ForwardFlag = {})
		: Traverser_impl((LayerType)-1, c, pos, c.GetMaxLayer(), ForwardFlag{})
	{}

	Traverser_impl(LayerType fixed, Qualifier<Container>& c, const Bpos& pos, LayerType trav, BackwardFlag)
		: Traverser_impl()
	{
		Init(fixed, c, pos, trav, BackwardFlag{});
	}
	Traverser_impl(Qualifier<Container>& c, const Bpos& pos, LayerType trav, BackwardFlag)
		: Traverser_impl(-1, c, pos, trav, BackwardFlag{})
	{}
	Traverser_impl(LayerType fixed, Qualifier<Container>& c, const Bpos& pos, BackwardFlag)
		: Traverser_impl(fixed, c, pos, c.GetMaxLayer(), BackwardFlag{})
	{}
	Traverser_impl(Qualifier<Container>& c, const Bpos& pos, BackwardFlag)
		: Traverser_impl((LayerType)-1, c, pos, c.GetMaxLayer(), BackwardFlag{})
	{}

	Traverser_impl(const Traverser_impl& x) = delete;
	//{
	//	*this = x;
	//}
	Traverser_impl(Traverser_impl&& x) noexcept
	{
		*this = std::move(x);
	}

	Traverser_impl& operator=(const Traverser_impl& x) = delete;
	/*{
		m_fixed_layer = x.m_fixed_layer;
		m_trav_layer = x.m_trav_layer;
		m_container = x.m_container;
		m_iterators = x.m_iterators;
		return *this;
	}*/
	Traverser_impl& operator=(Traverser_impl&& x) noexcept
	{
		m_fixed_layer = x.m_fixed_layer; x.m_fixed_layer = (LayerType)-1;
		m_trav_layer = x.m_trav_layer; x.m_trav_layer = (LayerType)-1;
		m_container = x.m_container; x.m_container = nullptr;
		m_iterators = std::move(x.m_iterators);
		return *this;
	}

	Traverser_impl Clone() const
	{
		Traverser_impl res;
		res.CopyFrom(*this);
		return res;
	}
	void CopyFrom(const Traverser_impl& x)
	{
		m_fixed_layer = x.m_fixed_layer;
		m_trav_layer = x.m_trav_layer;
		m_container = x.m_container;
		m_iterators = x.m_iterators;
	}

private:
	//layer - 1層移動後、layer層以下の情報を更新する。
	//空要素を持つ階層があった場合、そのひとつ上の階層を返す。
	//空要素がなく更新に成功した場合、-2が返る。
	//通常のRenewはlayer層のmIteratorが修正されていることを前提に、
	//layer + 1以下のmIteratorを修正する。
	//もし空要素が見つかった場合、空だった層以下のmIteratorは不定値となる。
	//したがって、空要素が見つかったとしてもその後確実にmIteratorを修正できる場合にのみ使うこと。
	template <direction_flag Flag>
	LayerType Renew(LayerType layer, Flag)
	{
		constexpr bool IsForward = std::is_same_v<Flag, ForwardFlag>;
		for (LayerType l = layer; l <= m_trav_layer; ++l)
		{
			auto& uit = m_iterators[l];
			auto& it = m_iterators[l + 1];
			if constexpr (IsForward)
			{
				auto x = uit->begin();
				if (x != uit->end()) { it = x; continue; }
			}
			else
			{
				auto x = uit->end();
				if (x != uit->begin()) { it = x - 1; continue; }
			}
			return l - 1;
		}
		return (LayerType)-2;
	}
	//renew_from - 1層までが初期化されているものとし、
	//renew_from - 1層以上は移動させず、renew_from以下を先頭または末尾に設定する。
	//設定に成功すればtrueを返す。
	//もしl層が空であった場合、l-1<=renew_fromであるならl-1層を移動させ再度更新を試みるが、
	//l-1>renew_from出会った場合、更新失敗としてfalseを返す。
	template <direction_flag Flag>
	bool Assign_renew(LayerType renew_from, Flag)
	{
		LayerType x = renew_from;
		while (true)
		{
			x = Renew(x, Flag{});
			if (x == (LayerType)-2) return true;
			//retry_from + 1層が空だったので移動する。
			for (LayerType l = x; l >= renew_from; --l)
			{
				assert(l > m_fixed_layer && l <= m_trav_layer);
				auto& uit = m_iterators[l];
				auto& it = m_iterators[l + 1];
				if constexpr (std::is_same_v<Flag, ForwardFlag>)
				{
					if ((++it) == uit->end())
					{
						//l層が空だったため、空要素を詰めておく。
						//it = m_tree->GetEmptyElement(l - 1).begin();
						continue;
					}
				}
				else
				{
					if (it == uit->begin()) continue;
					--it;
				}
				x = l;
				goto renew;
			}
			return false;
		renew:;
		}
	}
public:


private:
	//Initは固定層、走査層、Containerなどを指定し初期化する。階層の変化や参照するTreeの変更を伴う。
	template <direction_flag Flag>
	void Init_impl(LayerType fixed, Qualifier<Container>& c, LayerType trav, Flag)
	{
		static_assert(std::is_same_v<Flag, ForwardFlag> || std::is_same_v<Flag, BackwardFlag>);
		m_fixed_layer = fixed;
		m_container = &c;
		m_trav_layer = trav;
		assert(m_fixed_layer <= m_trav_layer);
		assert(m_container != nullptr);
		m_iterators.resize(m_trav_layer + 2);//globalの分も含む。
		m_iterators[0] = c.GetTopIterator();

		if (!Assign_renew(0, Flag{})) throw NoElements();
	}
	//Bposを与えるInitの場合、固定層まではBposの位置に設定する。
	//ただし、fixed<=pos.GetLayer()でなければならない。
	//固定層以下は先頭または末尾（endの一つ前）に設定する。
	//また固定層までの位置の要素が存在しない場合、そもそも固定自体が不可能であるため、
	//例外NoElementsが投げられる。
	template <direction_flag Flag>
	void Init_impl(LayerType fixed, Qualifier<Container>& c, const Bpos& pos, LayerType trav, Flag)
	{
		static_assert(std::is_same_v<Flag, ForwardFlag> || std::is_same_v<Flag, BackwardFlag>);
		m_fixed_layer = fixed;
		m_container = &c;
		m_trav_layer = trav;
		assert(m_fixed_layer <= m_trav_layer);
		assert(m_container != nullptr);
		assert(m_fixed_layer <= pos.GetLayer());
		m_iterators.resize(m_trav_layer + 2);//globalの分も含む。
		m_iterators[0] = c.GetTopIterator();

		for (LayerType l = (LayerType)0; l <= pos.GetLayer(); ++l)
		{
			auto& uit = m_iterators[l];
			auto& it = m_iterators[l + 1];
			BindexType i = pos[l];
			if (uit->GetSize() <= i) throw NoElements();
			it = uit->begin() + i;
		}
		if (!Assign_renew(m_fixed_layer + 1, Flag{})) throw NoElements();
	}

public:

	void Init(LayerType fixed, Qualifier<Container>& c, LayerType trav, ForwardFlag = {})
	{
		return Init_impl(fixed, c, trav, ForwardFlag{});
	}
	void Init(LayerType fixed, Qualifier<Container>& c, LayerType trav, BackwardFlag)
	{
		return Init_impl(fixed, c, trav, BackwardFlag{});
	}
	void Init(LayerType fixed, Qualifier<Container>& c, const Bpos& pos, LayerType trav, ForwardFlag = {})
	{
		return Init_impl(fixed, c, pos, trav, ForwardFlag{});
	}
	void Init(LayerType fixed, Qualifier<Container>& c, const Bpos& pos, LayerType trav, BackwardFlag)
	{
		return Init_impl(fixed, c, pos, trav, BackwardFlag{});
	}
	/*template <class Flag>
	void SetTravLayer(LayerType trav, Flag = ForwardFlag{})
	{
		Init(m_fixed_layer, *m_container, trav, Flag{});
	}*/

private:
	template <size_t N, std::integral ...Indices>
	bool Assign_rec(const ElementIterator& uit, BindexType i, Indices ...is)
	{
		if (i >= uit->size()) return false;
		auto it = uit->begin() + i;
		if (!Assign_rec<N + 1>(it + i, is...)) return false;
		m_iterators[N + 1] = it;
		return true;
	}
	template <size_t N>
	bool Assign_rec(const ElementIterator&)
	{
		return true;
	}
public:
	//指定された位置に設定する。
	//もしその位置が存在しない（空、範囲外など）場合、falseが返る。
	//与えたindexの数と層数が一致していなければならない。
	template <std::integral ...Indices>
	bool Assign(BindexType i, Indices ...is)
	{
		assert(sizeof...(Indices) == m_trav_layer);
		auto& it = m_iterators[0];
		return Assign_rec<0>(it, i, is...);
	}
	//thisのm_trav_layerまで全てbposの値で初期化する。
	//bpos.GetLayer() >= this->m_trav_layerが条件。
	//その位置の要素が存在しない場合falseが返るが、その際にTraverserの位置情報は破壊される。
	//そのため、falseが返った場合はTraverserはそのままでは使えないので、
	//AssignなりInitなりできちんと初期化しなければならない。
	bool Assign(const Bpos& bpos)
	{
		LayerType layer = bpos.GetLayer();
		assert(layer >= m_trav_layer);
		for (LayerType l = (LayerType)0; l <= m_trav_layer; ++l)
		{
			auto& uit = m_iterators[l];
			auto& it = m_iterators[l + 1];
			BindexType i = bpos[l];
			if (i >= uit->size()) return false;
			it = uit->begin() + i;
		}
		return true;
	}
	//thisのm_trav_layerまで全てthatの値で初期化する。
	//that.m_trav_layer >= this->m_trav_layerが条件。
	bool Assign(const Traverser_impl& that)
	{
		assert(m_trav_layer <= that.m_trav_layer);
		assert(m_container == that.m_container);
		for (LayerType l = 0; l <= m_trav_layer; ++l)
		{
			//treeの一致を要求している以上、
			//原理的にイテレータが範囲外となるはずがない。
			//まるごとコピーしてよいはず。
			m_iterators[l + 1] = that.m_iterators[l + 1];
		}
		return true;
	}

private:
	template <LayerType N, std::integral ...Indices, direction_flag Flag>
	bool AssignPartially_rec(const ElementIterator& uit, BindexType i, Indices ...is, Flag)
	{
		if (N > m_trav_layer) return true;
		if (i >= uit->size()) return false;
		auto it = uit->begin() + i;
		if (!AssignPartially_rec<N + 1>(it, is..., Flag{})) return false;
		m_iterators[N + 1] = it;
		return true;
	}
	template <LayerType N, direction_flag Flag>
	bool AssignPartially_rec(const ElementIterator&, Flag)
	{
		if (N > m_trav_layer) return true;
		return Assign_renew(N, Flag{});
	}
public:
	//指定位置に"部分的に"設定する。
	//与えたindexの数と走査層+1が一致する場合は通常のAssignと同じ結果となる。
	//一致しない場合、
	//1. indexの数<走査層 + 1なら、0層から順にindexの位置が当てられ、足りない場所は空でない最初の要素に設定される。
	//2. indexの数>走査層 + 1なら、0層から順にindexの位置が当てられ、超過したindexは無視される。
	template <std::integral ...Indices, direction_flag Flag>
	bool AssignPartially(BindexType i, Indices ...is, Flag)
	{
		auto& uit = m_iterators[0];
		return AssignPartially_rec<0>(uit, i, is..., Flag{});
	}

	//layerまでをbpos位置に設定し、それ以上は先頭または末尾に設定する。
	//GetTravLayer() >= layer && bpos.GetLayer() >= layerでなければならない。
	template <direction_flag Flag>
	bool AssignPartially(const Bpos& bpos, LayerType layer, Flag)
	{
		assert(GetTravLayer() >= layer);
		assert(bpos.GetLayer() >= layer);
		for (LayerType l = (LayerType)0; l <= layer; ++l)
		{
			auto& uit = m_iterators[l];
			auto& it = m_iterators[l + 1];
			BindexType i = bpos[l];
			if (i >= uit->GetSize()) return false;
			it = uit->begin() + i;
		}
		if (layer == m_trav_layer) return true;
		return Assign_renew(layer + 1, Flag{});
	}
	bool AssignPartially(const Bpos& b, LayerType layer)
	{
		return AssignPartially(b, layer, ForwardFlag{});
	}
	//bpos.GetLayer()とtraverse layerの小さい方までをbposの位置に設定し、
	//それ以上は（もしあれば）先頭または末尾に設定する。
	template <direction_flag Flag>
	bool AssignPartially(const Bpos& bpos, Flag)
	{
		assert(bpos.GetLayer() >= m_fixed_layer);
		return AssignPartially(bpos, m_fixed_layer, Flag{});
	}
	bool AssignPartially(const Bpos& b)
	{
		return AssignPartially(b, ForwardFlag{});
	}

	//layerまでをthatの位置に設定し、それ以上は先頭または末尾に設定する。
	//GetTravLayer() >= layer && that.GetTravlayer() >= layerでなければならない。
	template <direction_flag Flag>
	bool AssignPartially(const Traverser_impl& that, LayerType layer, Flag)
	{
		assert(GetTravLayer() >= layer);
		assert(that.GetTravLayer() >= layer);
		assert(m_container == that.m_container);
		for (LayerType l = (LayerType)0; l <= layer; ++l)
		{
			//treeの一致を要求している以上、
			//原理的にイテレータが範囲外となるはずがない。
			//まるごとコピーしてよいはず。
			m_iterators[l + 1] = that.m_iterators[l + 1];
		}
		if (layer == m_trav_layer) return true;
		return Assign_renew(layer + 1, Flag{});
	}
	bool AssignPartially(const Traverser_impl& that, LayerType layer)
	{
		return AssignPartially(that, layer, ForwardFlag{});
	}
	//thisのm_fixed_layerまでをthatの位置に設定し、
	//それ以上は（もしあれば）先頭または末尾に設定する。
	template <direction_flag Flag>
	bool AssignPartially(const Traverser_impl& that, Flag)
	{
		assert(that.GetTravLayer() >= m_fixed_layer);
		return AssignPartially(that, m_fixed_layer, Flag{});
	}
	bool AssignPartially(const Traverser_impl& that)
	{
		return AssignPartially(that, ForwardFlag{});
	}

protected:
	//主としてExternalTraverserがBposから部分的にAssignする際に利用する。
	//0～fixedまでは既に与えられているものとして変更せず無視し、fixed + 1～travまでをAssignする。
	//このとき、bposの位置はbpos[from]～bpos[from + trav - fixed - 1]を参照する。bposは連結後階層で与えられるため。
	//fixedとupper jointの階層は元来的に異なる可能性があるが、そのような状況でこの関数が呼ばれることはない。
	bool AssignByExtBpos(const Bpos& bpos, LayerType bpos_from)
	{
		assert(bpos_from + m_trav_layer - m_fixed_layer - 1 <= bpos.GetLayer());
		for (LayerType l = m_fixed_layer + 1; l <= m_trav_layer; ++l)
		{
			auto& uit = m_iterators[l];//l-1層のイテレータ。
			auto& it = m_iterators[l + 1];//l層のイテレータ。
			BindexType i = bpos[bpos_from + l - m_fixed_layer - 1];//lは初期値がfixed + 1なので、差っ引く。
			if (i >= uit->GetSize()) return false;
			it = uit->begin() + i;
		}
		return true;
	}
	//主としてExternalTraverserがBposから部分的にAssignPartiallyする際に利用する。
	//AssignPartiallyByExtBposは、AssignByExtBposとは異なり、init_fromからfixedまでを初期化する。
	//基本的にはExternalTraverserのAssignPartially(bpos, ...)から呼び出すもので、
	//init_fromはujoint + 1を与える。
	bool AssignPartiallyByExtBpos(const Bpos& bpos, LayerType bpos_from, LayerType init_from)
	{
		assert(bpos_from + m_fixed_layer - init_from - 1 <= bpos.GetLayer());
		assert(init_from >= 0_layer);
		for (LayerType l = init_from; l <= m_fixed_layer; ++l)
		{
			auto& uit = m_iterators[l];//l-1層のイテレータ。
			auto& it = m_iterators[l + 1];//l層のイテレータ。
			BindexType i = bpos[bpos_from + l - init_from];//lは初期値がinit_fromなので、差っ引く。
			if (i >= uit->GetSize()) return false;
			it = uit->begin() + i;
		}
		if (m_fixed_layer == m_trav_layer) return true;
		return Assign_renew(m_fixed_layer + 1, ForwardFlag{});
	}

public:

	//強制的にbeginへ移動させる。fixedまでは固定されているので移動せず、fixed + 1 ~ travをbeginと同等にする。
	bool MoveToBegin()
	{
		return Assign_renew(m_fixed_layer + 1, ForwardFlag{});
	}
	//強制的にendへ移動させる。fixedまでは固定されているので移動せず、fixed + 1をendと同等にする。
	bool MoveToEnd()
	{
		if (m_fixed_layer != m_trav_layer)
			m_iterators[m_fixed_layer + 2] = m_iterators[m_fixed_layer + 1]->end();
		return true;
	}

private:
	template <direction_flag Flag>
	bool Move_impl2(LayerType layer, Flag)
	{
		assert(layer > m_fixed_layer);
		assert(layer >= 0_layer);
		constexpr bool IsForward = std::is_same_v<Flag, ForwardFlag>;
		LayerType move = layer;
		while (true)
		{
			auto& uit = m_iterators[move];//layer-1層。
			auto& it = m_iterators[move + 1];//layer層。
			if constexpr (IsForward)
			{
				auto next = it + 1;
				if (next == uit->end()) return false;
				it = next;
			}
			else
			{
				if (it == uit->begin()) return false;
				--it;
			}
			if (move == m_trav_layer) return true;
			else
			{
				LayerType failure = Renew(move + 1_layer, Flag{});
				//failureが-2のとき、下層の更新に成功したということ。
				if (failure == -2_layer) return true;
				//failureが-2でない場合、これは空要素が見つかった層のさらにひとつ上の層を意味する。
				//つまり、failure層の移動を行う必要がある。
				assert(move <= failure);//failureは移動した階層と同じかそれより上層のはずである。
				move = failure;
			}
		}
	}
public:
	template <direction_flag Flag>
	bool Move(LayerType layer, Flag)
	{
		return Move_impl2(layer, Flag{});
	}
	//layer層を前進させる。他の層はlayer層移動後の最初の要素を指すようになる。
	//移動に成功したらtrueを、layer層が最終要素(endの一つ手前)だった場合は移動せずfalseを返す。
	//IsEnd()==trueの状態のときに呼んではいけない。
	bool MoveForward(LayerType layer)
	{
		return Move_impl2(layer, ForwardFlag{});
	}
	//layer層を後退させる。他の層はlayer層移動後の末尾の要素を指すようになる。
	//移動に成功したらtrueを、layer層が最終要素(endの一つ手前)だった場合は移動せずfalseを返す。
	//IsEnd()==trueのときに呼ぶ場合、layerは必ずm_fixed_layer+1でなければならない。
	bool MoveBackward(LayerType layer)
	{
		return Move_impl2(layer, BackwardFlag{});
	}

	//trav layerを移動する。
	//移動に失敗した場合は上層の移動も試み、trav layerが有効要素になるまで繰り返す。
	//戻り値は移動した最も上の層。例えばtravが3で1～3層の移動を何度も繰り返したとき、1が返る。
	//ただし末尾に到達した場合、fixed layerが返る。
	template <direction_flag Flag>
	LayerType IncrDecrOperator(Flag)
	{
		constexpr bool IsForward = std::is_same_v<Flag, ForwardFlag>;
		//backwardかつend状態のときはfixed + 1層が末尾であるため、
		//そこから移動する必要がある。
		LayerType retry_from = (!IsForward && IsEnd()) ? m_fixed_layer + 1 : m_trav_layer;
		LayerType moved = (LayerType)-1;
		LayerType res = m_trav_layer;
		while (true)
		{
			for (LayerType l = retry_from; l > m_fixed_layer; --l)
			{
				assert(l > m_fixed_layer && l <= m_trav_layer);
				auto& uit = m_iterators[l];
				auto& it = m_iterators[l + 1];
				if constexpr (IsForward)
				{
					if ((++it) == uit->end())
					{
						//m_fixed_layer + 1、つまり走査する最上層が末尾に達した場合、
						//empty iteratorではなく単にその層のendにしておけばよい。これがIsEnd関数の条件である。
						//よって、empty iteratorを取得するのはlがm_fixed_layer + 1以外の時。
						//if (l != m_fixed_layer + 1) it = m_tree->GetEmptyElement(l).ConvertToIterator();
						continue;
					}
				}
				else
				{
					//beginより前に戻る必要はない。
					if (it == uit->begin()) continue;
					--it;
				}
				moved = l;
				goto renew;
			}
			//fixed layer + 1が末尾に達した。
			//もしくは、Backwardの場合、fixed + 1以下が全て最初の要素になっている。
			//この場合これ以上移動はできないので、fixed layerを返す。
			return m_fixed_layer;
		renew:;
			//moved層を移動させたので、moved + 1層以下の位置情報を更新する。
			//ただしmoved==m_trav_layerなら不要である。
			if (moved == m_trav_layer) return std::min(res, moved);
			retry_from = Renew(moved + (LayerType)1, Flag{});
			res = std::min(res, moved);
			//retry_fromは空要素の見つかった層のひとつ上の層。
			//retry_fromが-2のとき、更新に成功したという意味である。
			if (retry_from == (LayerType)-2) return res;
			assert(retry_from >= moved);
			//更新に失敗した場合、retry_from層をもう一度移動させる。
		}
		throw Forbidden("Thrown in Traverser_impl::InclDeclOperator. THIS IS A BUG.");
	}

	Traverser_impl& operator++()
	{
		IncrDecrOperator(ForwardFlag{});
		return *this;
	}
	Traverser_impl& operator--()
	{
		IncrDecrOperator(BackwardFlag{});
		return *this;
	}

	//Traverserはnon-copyableなので、後置インクリメント、デクリメントでは値を返せない。
	void operator++(int)
	{
		++(*this);
	}
	void operator--(int)
	{
		--(*this);
	}

	//実質的な効果はoperator++などと同じ。ただし戻り値は実際に移動した階層である。
	//例えば、有効な要素が見つかるまで0～2層を繰り返し移動したとき、戻り値は0となる。
	LayerType Incr() { return IncrDecrOperator(ForwardFlag{}); }
	LayerType Decr() { return IncrDecrOperator(BackwardFlag{}); }

	//全ての位置が一致することを要求する。
	bool operator==(const Traverser_impl& t) const
	{
		//各層イテレータの比較はポインタで行われるので、
		//Bposのindexを比較していた昔とは違い、原理的には最下層の比較だけで十分だが、
		//一応デバッグ時は確認する。
		assert(m_trav_layer == t.m_trav_layer);
		bool res = m_iterators[m_trav_layer + 1] == t.m_iterators[m_trav_layer + 1];
#ifndef NDEBUG
		bool verify = true;
		for (auto [sit, oit] : views::Zip(m_iterators, t.m_iterators)) if (sit != oit) verify = false;
		assert(res == verify);
#endif
		return res;
	}
	bool operator!=(const Traverser_impl& t) const
	{
		return !(*this == t);
	}
	//すべての位置が一致することを要求する。
	bool operator==(const Bpos& bpos) const
	{
		assert(m_trav_layer == bpos.GetLayer());
		assert(bpos.GetLayer() >= 0);
		if (GetPos(0) != bpos.GetRow()) return false;
		for (LayerType l = 1; l <= m_trav_layer; ++l) if (GetPos(l) != bpos.GetTpos(l)) return false;
		return true;
	}

	bool Match(const Traverser_impl& o) const
	{
		return *this == o;
	}
	bool Match(const Bpos& bpos) const
	{
		return *this == bpos;
	}
	//thisのm_fixed_layerまでを引数と比較する。
	//よって、引数の方のm_trav_layer>=this.m_fixed_layerでなければならない。
	bool MatchPartially(const Traverser_impl& o) const
	{
		assert(m_fixed_layer <= o.m_trav_layer);
		return MatchPartially(o, m_fixed_layer);
	}
	//max層までを引数と比較する。
	//自身とoのどちらか一方でもm_trav_layer < maxである場合、falseが返る。
	bool MatchPartially(const Traverser_impl& o, LayerType max) const
	{
		assert(m_trav_layer >= max && o.m_trav_layer >= max);
		//if (m_trav_layer < max || o.m_trav_layer < max) throw InvalidArg("");
		auto sit = m_iterators.begin();
		auto oit = o.m_iterators.begin();
		for (LayerType l = 0; l <= max; ++l, ++sit, ++oit)
		{
			if (sit != oit) return false;
		}
		return true;
	}
	//thisのm_trav_layerまでを引数と比較する。
	//よって、引数の方のlayer>=this.m_trav_layerでなければならない。
	bool MatchPartially(const Bpos& bpos) const
	{
		assert(m_trav_layer <= bpos.GetLayer());
		return MatchPartially(bpos, m_trav_layer);
	}
	bool MatchPartially(const Bpos& bpos, LayerType max) const
	{
		for (LayerType l = 0; l <= max; ++l) if (GetPos(l) != bpos[l]) return false;
		return true;
	}

	bool IsEnd() const
	{
		//m_fixed_layer + 1層が末尾要素を指している場合、endの状態を示している。
		//ただし、m_fixed_layer + 2層以下についてはどこを指しているかは指定しない。
		//また、m_fixed_layer == m_trav_layerになる（連結状態などで発生しうる）では常にtrueとなる。
		//operator--などによって後ろに戻る場合、
		return (m_fixed_layer == m_trav_layer) || (m_iterators[m_fixed_layer + 2] == m_iterators[m_fixed_layer + 1]->end());
	}

	const Traverser_impl& operator*() const { return *this; }
	const Traverser_impl* operator->() const { return this; }

	BindexType GetPos(LayerType layer) const
	{
		return (BindexType)(m_iterators[layer + 1] - m_iterators[layer]->begin());
	}
	//std::min(m_trav_layer, bpos.GetLayer())までを自身のposで初期化する。
	//もしbpos.GetLayer() > m_trav_layerの場合、余剰分は0で初期化する。
	void GetBpos(Bpos& bpos) const
	{
		assert(bpos.GetLayer() >= 0_layer);
		LayerType x = bpos.GetLayer();
		if (x < m_trav_layer)
		{
			bpos.Init(m_trav_layer);
			for (LayerType l = (LayerType)0; l <= m_trav_layer; ++l) bpos[l] = GetPos(l);
		}
		else
		{
			for (LayerType l = (LayerType)0; l <= m_trav_layer; ++l) bpos[l] = GetPos(l);
			for (LayerType l = m_trav_layer + 1; l <= x; ++l) bpos[l] = 0;
		}
	}

private:
	template <bool SizeCheck, std::integral Index, std::integral ...Indices>
	FieldRef GetField_rec(const RttiPlaceholder& m, const ElementIterator& it, Index i, Indices ...is) const
	{
		if constexpr (SizeCheck)
			if (i < 0 || std::cmp_greater_equal(i, it->GetSize())) throw NoElements();
		return GetField_rec<SizeCheck>(m, it->begin() + i, is...);
	}
	template <bool SizeCheck, class Iter>
	FieldRef GetField_rec(const RttiPlaceholder& m, const Iter& it) const
	{
		return (*it)[m];
	}
public:
	FieldRef operator[](const RttiPlaceholder& m) const
	{
		return GetField(m);
	}
	FieldRef GetField(const RttiPlaceholder& m) const
	{
		return (*this->GetIterator(m.GetInternalLayer()))[m];
	}
	template <std::integral ...Indices>
	FieldRef GetField(const RttiPlaceholder& m, Indices ...is) const
	{
		static_assert(sizeof...(Indices) <= std::numeric_limits<LayerType>::max());//常識的に考えて要らんけど、一応。
		assert((LayerType)sizeof...(Indices) <= this->GetTravLayer());
		assert((LayerType)sizeof...(Indices) <= m.GetInternalLayer());
		auto& it = this->GetIterator(m.GetInternalLayer() - sizeof...(Indices));
		return GetField_rec<true>(m, it, is...);
	}

private:
	template <bool SizeCheck, class Type, std::integral Index, class ...Indices>
	Qualifier<Type>& GetField_rec(const TypedPlaceholder<Type>& m, const ElementIterator& it,
								  Index i, Indices ...is) const
	{
		if constexpr (SizeCheck)
			if (i < 0 || std::cmp_greater_equal(i, it->GetSize())) throw NoElements();
		return GetField_rec<SizeCheck>(m, it->begin() + i, is...);
	}
	template <bool SizeCheck, class Type, class Iter>
	decltype(auto) GetField_rec(const TypedPlaceholder<Type>& m, const Iter& it) const
	{
		return (*it)[m];
	}
public:
	template <class Type>
	Qualifier<Type>& operator[](const TypedPlaceholder<Type>& m) const
	{
		return GetField(m);
	}
	template <class Type>
	Qualifier<Type>& GetField(const TypedPlaceholder<Type>& m) const
	{
		return (*this->GetIterator(m.GetInternalLayer()))[m];
	}
	template <class Type, std::integral ...Indices>
	Qualifier<Type>& GetField(const TypedPlaceholder<Type>& m, Indices ...is) const
	{
		static_assert(sizeof...(Indices) <= std::numeric_limits<LayerType>::max());//常識的に考えて要らんけど、一応。
		assert((LayerType)sizeof...(Indices) <= this->GetTravLayer());
		assert((LayerType)sizeof...(Indices) <= m.GetInternalLayer());
		auto& it = this->GetIterator(m.GetInternalLayer() - sizeof...(Indices));
		return GetField_rec<true>(m, it, is...);
	}

private:
	template <bool SizeCheck, LayerType Layer, class Type, std::integral Index, class ...Indices>
	Qualifier<Type>& GetField_rec(const CttiPlaceholder<Layer, Type>& m, const ElementIterator& it,
								  Index i, Indices ...is) const
	{
		if constexpr (SizeCheck)
			if (i < 0 || std::cmp_greater_equal(i, it->GetSize())) throw NoElements();
		return GetField_rec<SizeCheck>(m, it->begin() + i, is...);
	}
	template <bool SizeCheck, LayerType Layer, class Type, class Iter>
	decltype(auto) GetField_rec(const CttiPlaceholder<Layer, Type>& m, const Iter& it) const
	{
		return (*it)[m];
	}
public:
	template <LayerType Layer, class Type>
	Qualifier<Type>& operator[](const CttiPlaceholder<Layer, Type>& m) const
	{
		return GetField(m);
	}
	template <LayerType Layer, class Type>
	Qualifier<Type>& GetField(const CttiPlaceholder<Layer, Type>& m) const
	{
		return (*this->GetIterator(m.GetInternalLayer()))[m];
	}
	template <LayerType Layer, class Type, std::integral ...Indices>
	Qualifier<Type>& GetField(const CttiPlaceholder<Layer, Type>& m, Indices ...is) const
	{
		static_assert(sizeof...(Indices) <= std::numeric_limits<LayerType>::max());//常識的に考えて要らんけど、一応。
		assert((LayerType)sizeof...(Indices) <= this->GetTravLayer());
		assert((LayerType)sizeof...(Indices) <= m.GetInternalLayer());
		auto& it = this->GetIterator(m.GetInternalLayer() - sizeof...(Indices));
		return GetField_rec<true>(m, it, is...);
	}


	friend bool operator==(const Traverser_impl& self, const Sentinel&)
	{
		return self.IsEnd();
	}
	friend bool operator!=(const Traverser_impl& self, const Sentinel&)
	{
		return !self.IsEnd();
	}
	friend bool operator==(const Sentinel&, const Traverser_impl& self)
	{
		return self.IsEnd();
	}
	friend bool operator!=(const Sentinel&, const Traverser_impl& self)
	{
		return !self.IsEnd();
	}

	const ElementIterator& GetIterator(LayerType layer) const
	{
		assert(layer <= m_trav_layer);
		return m_iterators[layer + 1];
	}
	LayerType GetFixedLayer() const
	{
		return m_fixed_layer;
	}
	LayerType GetTravLayer() const
	{
		return m_trav_layer;
	}

	Qualifier<Container>& GetContainer() const { return *m_container; }
	Qualifier<Container>* GetContainerPtr() const { return m_container; }

private:

	LayerType m_fixed_layer;//固定層。この層以上の位置は変更されない。
	LayerType m_trav_layer;//走査層。この層より下層は無視され、走査されない。
	Qualifier<Container>* m_container;
	std::vector<ElementIterator> m_iterators;//m_iterators[0]は-1層の要素を指すイテレータ、m_iterators[N+1]がN層のイテレータ。
};


//HierarcicalTreeのrangeは固定層、走査層、固定位置の情報の指定が必要であるため、
//Treeそれ自体はrangesとして成立しない。
template <class Container_, template <class> class Qualifier>
class Range_impl : public std::ranges::view_interface<Range_impl<Container_, Qualifier>>
{
public:
	using Container = Container_;
	using Traverser = Container::Traverser;
	using ConstTraverser = Container::ConstTraverser;

	using Sentinel = Container::Sentinel;
	using ConstSentinel = Container::ConstSentinel;

	static constexpr bool IsConst = std::is_const_v<Qualifier<int>>;

	Range_impl(LayerType fix, Qualifier<Container>& tree, LayerType trav)
		: m_fixed_layer(fix), m_trav_layer(trav), m_tree(&tree)
	{}
	Range_impl(Qualifier<Container>& tree, LayerType trav = -1_layer)
		: Range_impl((LayerType)-1, tree, trav)
	{}
	Range_impl(LayerType fix, Qualifier<Container>& tree)
		: Range_impl(fix, tree, -1_layer)
	{}

	Range_impl(LayerType fix, Qualifier<Container>& tree, const Bpos& pos, LayerType trav)
		: m_fixed_layer(fix), m_trav_layer(trav), m_bpos(pos), m_tree(&tree)
	{}
	Range_impl(Qualifier<Container>& tree, const Bpos& pos, LayerType trav)
		: Range_impl((LayerType)-1, tree, pos, trav)
	{}
	Range_impl(LayerType fix, Qualifier<Container>& tree, const Bpos& pos)
		: Range_impl(fix, tree, pos, -1_layer)
	{}
	Range_impl(Qualifier<Container>& tree, const Bpos& pos)
		: Range_impl((LayerType)-1, tree, pos, -1_layer)
	{}

	Range_impl(const Range_impl&) = default;
	Range_impl(Range_impl&&) noexcept = default;

	Range_impl& operator=(const Range_impl&) = default;
	Range_impl& operator=(Range_impl&&) noexcept = default;

	LayerType GetFixedLayer() const { return m_fixed_layer; }
	LayerType GetTravLayer() const { return m_trav_layer; }
	void SetFixedLayer(LayerType layer) { m_fixed_layer = layer; }
	void SetTravLayer(LayerType layer) { m_trav_layer = layer; }

	void Assign(LayerType trav)
	{
		m_trav_layer = trav;
		m_bpos.reset();
	}
	void Assign(LayerType fix, LayerType trav)
	{
		m_fixed_layer = fix;
		m_trav_layer = trav;
		m_bpos.reset();
	}
	void Assign(LayerType fix, const Bpos& bpos, LayerType trav = -1_layer)
	{
		m_fixed_layer = fix;
		m_trav_layer = trav;
		m_bpos = bpos;
	}
	void Assign(const Bpos& bpos, LayerType trav = -1_layer)
	{
		Assign(-1_layer, bpos, trav);
	}

	Traverser begin() const requires (!IsConst)
	{
		if (m_trav_layer == -1) throw NotInitialized("");
		if (m_bpos.has_value()) return Traverser(m_fixed_layer, *m_tree, *m_bpos, m_trav_layer, ForwardFlag{});
		else return Traverser(m_fixed_layer, *m_tree, m_trav_layer, ForwardFlag{});
	}
	ConstTraverser begin() const requires IsConst
	{
		return cbegin();
	}
	ConstTraverser cbegin() const
	{
		if (m_trav_layer == -1) throw NotInitialized("");
		if (m_bpos.has_value()) return ConstTraverser(m_fixed_layer, *m_tree, *m_bpos, m_trav_layer, ForwardFlag{});
		else return ConstTraverser(m_fixed_layer, *m_tree, m_trav_layer, ForwardFlag{});
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

	const Container& GetContainer() const { return *m_tree; }

private:
	LayerType m_fixed_layer;
	LayerType m_trav_layer;
	std::optional<Bpos> m_bpos;
	Qualifier<Container>* m_tree;
};


template <class Container_,
	template <class, template <class> class> class Iterator,
	template <class> class Qualifier>
	requires (Container_::MaxLayer == 0)
class Traverser_impl<Container_, Iterator, Qualifier>
{
	//階層構造のないテーブル専用のTraverser。
	//Treeは構造的柔軟性は高いが、どうしても走査のコストが大きいため、
	//複雑な構造が必要ない状況のためにテーブル走査機能を用意しておく。
	//走査層指定が原理的に存在し得ないため、TravLayerは0で固定される。
	//ただしFixedLayerは-1か0のどちらかを取りうる。

	//階層数固定であれば複数層に拡張することも原理的にはできるが、
	//そこまでする価値はないだろう。

public:

	using iterator_category = std::input_iterator_tag;
	using difference_type = ptrdiff_t;
	using reference = Traverser_impl;
	using pointer = Traverser_impl*;
	using value_type = Traverser_impl;

	using Container = Container_;
	using Hierarchy = Container::Hierarchy;
	using FieldRef = FieldRef_impl<Qualifier>;
	using ElementIterator = Iterator<Hierarchy, Qualifier>;
	using ElementRef = ElementRef_impl<Hierarchy, Qualifier, LayerType>;
	using ElementBlock = ElementRef::ElementBlock;
	using Sentinel = Sentinel_impl<Container, Iterator, Qualifier>;

	using RttiPlaceholder = Hierarchy::RttiPlaceholder;
	template <class Type>
	using TypedPlaceholder = Hierarchy::template TypedPlaceholder<Type>;
	template <LayerType Layer, class Type>
	using CttiPlaceholder = Hierarchy::template CttiPlaceholder<Layer, Type>;

	Traverser_impl() {}
	Traverser_impl(LayerType fixed, Qualifier<Container>& c, LayerType trav, ForwardFlag = {})
	{
		Init(fixed, c, trav, ForwardFlag{});
	}
	Traverser_impl(Qualifier<Container>& c, LayerType trav, ForwardFlag = {})
		: Traverser_impl(-1_layer, c, trav, ForwardFlag{})
	{}
	Traverser_impl(LayerType fixed, Qualifier<Container>& c, ForwardFlag = {})
		: Traverser_impl(fixed, c, 0_layer, ForwardFlag{})
	{}
	Traverser_impl(Qualifier<Container>& c, ForwardFlag = {})
		: Traverser_impl(-1_layer, c, 0_layer, ForwardFlag{})
	{}

	Traverser_impl(LayerType fixed, Qualifier<Container>& c, LayerType trav, BackwardFlag)
		: Traverser_impl()
	{
		Init(fixed, c, trav, BackwardFlag{});
	}
	Traverser_impl(Qualifier<Container>& c, LayerType trav, BackwardFlag)
		: Traverser_impl((LayerType)-1, c, trav, BackwardFlag{})
	{}
	Traverser_impl(LayerType fixed, Qualifier<Container>& c, BackwardFlag)
		: Traverser_impl(fixed, c, 0_layer, BackwardFlag{})
	{}
	Traverser_impl(Qualifier<Container>& c, BackwardFlag)
		: Traverser_impl((LayerType)-1, c, 0_layer, BackwardFlag{})
	{}

	Traverser_impl(LayerType fixed, Qualifier<Container>& c, const Bpos& pos, LayerType trav, ForwardFlag = {})
		: Traverser_impl()
	{
		Init(fixed, c, pos, trav, ForwardFlag{});
	}
	Traverser_impl(Qualifier<Container>& c, const Bpos& pos, LayerType trav, ForwardFlag = {})
		: Traverser_impl((LayerType)-1, c, pos, trav, ForwardFlag{})
	{}
	Traverser_impl(LayerType fixed, Qualifier<Container>& c, const Bpos& pos, ForwardFlag = {})
		: Traverser_impl(fixed, c, pos, 0_layer, ForwardFlag{})
	{}
	Traverser_impl(Qualifier<Container>& c, const Bpos& pos, ForwardFlag = {})
		: Traverser_impl((LayerType)-1, c, pos, 0_layer, ForwardFlag{})
	{}

	Traverser_impl(LayerType fixed, Qualifier<Container>& c, const Bpos& pos, LayerType trav, BackwardFlag)
		: Traverser_impl()
	{
		Init(fixed, c, pos, trav, BackwardFlag{});
	}
	Traverser_impl(Qualifier<Container>& c, const Bpos& pos, LayerType trav, BackwardFlag)
		: Traverser_impl(-1, c, pos, trav, BackwardFlag{})
	{}
	Traverser_impl(LayerType fixed, Qualifier<Container>& c, const Bpos& pos, BackwardFlag)
		: Traverser_impl(fixed, c, pos, 0_layer, BackwardFlag{})
	{}
	Traverser_impl(Qualifier<Container>& c, const Bpos& pos, BackwardFlag)
		: Traverser_impl((LayerType)-1, c, pos, 0_layer, BackwardFlag{})
	{}

	//TableTraverserは動的メモリ確保を行わないのでコピー可能にしてもよいのだが、仕様統一のために不可にしておく。
	Traverser_impl(const Traverser_impl& x) = delete;
	Traverser_impl(Traverser_impl&& x) noexcept
	{
		*this = std::move(x);
	}
	Traverser_impl& operator=(const Traverser_impl& x) = delete;
	Traverser_impl& operator=(Traverser_impl&& x) noexcept
	{
		m_fixed_layer = x.m_fixed_layer;
		m_container = x.m_container; x.m_container = nullptr;
		m_iterators = std::move(x.m_iterators);
		return *this;
	}

	Traverser_impl Clone() const
	{
		Traverser_impl res;
		res.CopyFrom(*this);
		return res;
	}
	void CopyFrom(const Traverser_impl& x)
	{
		m_fixed_layer = x.m_fixed_layer;
		m_container = x.m_container;
		m_iterators = x.m_iterators;
	}

private:
	template <direction_flag Flag>
	void Init_impl(LayerType fixed, Qualifier<Container>& c, [[maybe_unused]] LayerType trav, Flag)
	{
		assert(trav == 0_layer);
		m_fixed_layer = fixed;
		m_container = &c;
		m_iterators[0] = c.GetTopIterator();
		if constexpr (std::same_as<ForwardFlag, Flag>)
			m_iterators[1] = m_iterators[0]->begin();
		else
		{
			auto it = m_iterators[0]->end();
			if (it != m_iterators[0]->begin()) m_iterators[1] = it - 1;
		}
	}

	//位置をposから与えるので、Flagは無意味。
	template <direction_flag Flag>
	void Init_impl(LayerType fixed, Qualifier<Container>& c, const Bpos& pos, [[maybe_unused]] LayerType trav, Flag)
	{
		assert(trav == 0_layer);
		assert(pos.GetLayer() == 0_layer);
		m_fixed_layer = fixed;
		m_container = &c;
		m_iterators[0] = c.GetTopIterator();
		assert(pos.GetRow() < m_iterators[0]->GetSize());
		m_iterators[1] = m_iterators[0]->begin() + pos.GetRow();
	}
public:
	void Init(LayerType fixed, Qualifier<Container>& c, LayerType trav, ForwardFlag = {})
	{
		return Init_impl(fixed, c, trav, ForwardFlag{});
	}
	void Init(LayerType fixed, Qualifier<Container>& c, LayerType trav, BackwardFlag)
	{
		return Init_impl(fixed, c, trav, BackwardFlag{});
	}
	void Init(LayerType fixed, Qualifier<Container>& c, const Bpos& pos, LayerType trav, ForwardFlag = {})
	{
		return Init_impl(fixed, c, pos, trav, ForwardFlag{});
	}
	void Init(LayerType fixed, Qualifier<Container>& c, const Bpos& pos, LayerType trav, BackwardFlag)
	{
		return Init_impl(fixed, c, pos, trav, BackwardFlag{});
	}

	bool Assign(BindexType i)
	{
		auto& it = m_iterators[0];
		if (i >= it->GetSize()) return false;
		m_iterators[1] = it->begin() + i;
		return true;
	}
	bool Assign(const Traverser_impl& that)
	{
		CopyFrom(that);
		return true;
	}

	template <direction_flag Flag>
	bool AssignPartially(BindexType i, Flag)
	{
		return Assign(i);
	}
	bool AssignPartially(BindexType i)
	{
		return Assign(i);
	}
	//本来期待される挙動は、layer層までをbposから与え、それより下層は初期位置とする、というもの。
	//ただしTableであるため、
	template <direction_flag Flag>
	bool AssignPartially(const Bpos& bpos, LayerType layer, Flag)
	{
		assert(layer <= 0_layer);
		assert(bpos.GetLayer() >= layer);
		if (layer == -1_layer)
		{
			if constexpr (std::same_as<Flag, ForwardFlag>) MoveToBegin();
			else MoveToEnd();
			return true;
		}
		else return Assign(bpos.GetRow());
	}
	bool AssignPartially(const Bpos& b, LayerType layer)
	{
		return AssignPartially(b, layer, ForwardFlag{});
	}
	template <direction_flag Flag>
	bool AssignPartially(const Bpos& bpos, Flag)
	{
		return AssignPartially(bpos, 0_layer, Flag{});
	}
	bool AssignPartially(const Bpos& b)
	{
		return AssignPartially(b, ForwardFlag{});
	}

	template <direction_flag Flag>
	bool AssignPartially(const Traverser_impl& that, LayerType layer, Flag)
	{
		assert(layer <= 0_layer);
		assert(that.GetTravLayer() >= layer);
		assert(m_container == that.m_container);
		if (layer == -1_layer)
		{
			if constexpr (std::same_as<Flag, ForwardFlag>) MoveToBegin();
			else MoveToEnd();
			return true;
		}
		return Assign(that);
	}
	bool AssignPartially(const Traverser_impl& that, LayerType layer)
	{
		return AssignPartially(that, layer, ForwardFlag{});
	}
	template <direction_flag Flag>
	bool AssignPartially(const Traverser_impl& that, Flag)
	{
		return AssignPartially(that, 0_layer, Flag{});
	}
	bool AssignPartially(const Traverser_impl& that)
	{
		return AssignPartially(that, ForwardFlag{});
	}

protected:
	//ExternalTraverserがBposから部分的にAssignするときに利用する。
	//fixed layerまでは既に与えられているとして、fixed+1 ～ 0層のみをAssignする。
	//ということは、fixed==0層の場合は何もしない。
	bool AssignByExtBpos(const Bpos& bpos, LayerType bpos_from)
	{
		assert(bpos_from - m_fixed_layer - 1 <= bpos.GetLayer());
		if (m_fixed_layer == -1_layer)
		{
			auto& it = m_iterators[0];
			BindexType i = bpos[bpos_from];
			if (i >= it->GetSize()) return false;
			m_iterators[1] = it->begin() + i;
		}
		return true;
	}
	//ExternalTraverserがBposから部分的にAssignPartiallyする際に利用する。
	//init_from(>=0)からfixedまでをbposで初期化する。
	//ということは事実上、init_from == 0、fixed == 0のときにしか効果がない。
	bool AssignPartiallyByExtBpos(const Bpos& bpos, LayerType bpos_from, LayerType init_from)
	{
		assert(bpos_from + m_fixed_layer - init_from - 1 <= bpos.GetLayer());
		assert(init_from >= 0_layer);
		if (init_from == 0_layer && m_fixed_layer == 0_layer)
		{
			auto& it = m_iterators[0];
			BindexType i = bpos[bpos_from];
			if (i >= it->GetSize()) return false;
			m_iterators[1] = it->begin() + i;
		}
		return true;
	}
public:

	bool MoveToBegin()
	{
		m_iterators[1] = m_iterators[0]->begin();
		return true;
	}
	bool MoveToEnd()
	{
		m_iterators[1] = m_iterators[0]->end();
		return true;
	}

	template <direction_flag Flag>
	bool Move([[maybe_unused]] LayerType layer, Flag)
	{
		assert(m_fixed_layer == -1_layer);
		assert(layer == 0_layer);
		auto& uit = m_iterators[0];
		auto& it = m_iterators[1];
		if constexpr (std::same_as<Flag, ForwardFlag>)
		{
			auto next = it + 1;
			if (next == uit->end()) return false;
			it = next;
		}
		else
		{
			if (it == uit->begin()) return false;
			--it;
		}
		return true;
	}
	bool MoveForward(LayerType layer) { return Move(layer, ForwardFlag{}); }
	bool MoveBackward(LayerType layer) { return Move(layer, BackwardFlag{}); }

	//0層を移動する。
	//戻り値は移動した最も上の層。TableTraverserの場合は常に0となる。
	//ただし末尾に到達した場合、fixed layerが返る。
	template <direction_flag Flag>
	LayerType IncrDecrOperator(Flag)
	{
		if (m_fixed_layer == -1_layer)
		{
			auto& it = m_iterators[1];
			++it;
			bool x = it == m_iterators[0]->end();
			return LayerType(0 - (int)x);//末尾に到達した場合は-1を返さなければならない。
		}
		return 0_layer;
	}
	Traverser_impl& operator++()
	{
		assert(m_iterators[1] != m_iterators[0]->end());
		if (m_fixed_layer == -1_layer) ++m_iterators[1];
		return *this;
	}
	Traverser_impl& operator--()
	{
		assert(m_iterators[1] != m_iterators[0]->begin());
		if (m_fixed_layer == -1_layer) --m_iterators[1];
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

	LayerType Incr() { return IncrDecrOperator(ForwardFlag{}); }
	LayerType Decr() { return IncrDecrOperator(BackwardFlag{}); }

	bool operator==(const Traverser_impl& that) const
	{
		assert(m_container == that.m_container);
		return m_iterators[1] == that.m_iterators[1];
	}
	bool operator!=(const Traverser_impl& that) const
	{
		return !(*this == that);
	}
	bool operator==(const Bpos& bpos) const
	{
		assert(bpos.GetLayer() == 0_layer);
		return GetPos(0_layer) == bpos.GetRow();
	}

	bool Match(const Traverser_impl& o) const
	{
		return *this == o;
	}
	bool Match(const Bpos& bpos) const
	{
		return *this == bpos;
	}

	//thisのm_fixed_layerまでを引数と比較する。
	bool MatchPartially(const Traverser_impl& that) const
	{
		assert(m_fixed_layer <= that.m_fixed_layer);
		return MatchPartially(that, m_fixed_layer);
	}
	//max層までを引数と比較する。
	//自身とoのどちらか一方でもm_trav_layer < maxである場合、falseが返る。
	bool MatchPartially(const Traverser_impl& o, LayerType max) const
	{
		assert(0_layer >= max);
		//max層までとは言いつつ、max==-1であれば比較する必要はないし、
		//max==0なら0層のみを比較すれば良い。
		return (max == -1_layer) || m_iterators[1] == o.m_iterators[1];
	}
	bool MatchPartially(const Bpos& bpos) const
	{
		assert(0_layer <= bpos.GetLayer());
		return MatchPartially(bpos, 0_layer);
	}
	bool MatchPartially(const Bpos& bpos, LayerType max) const
	{
		return (max == -1_layer) || GetPos(0) == bpos.GetRow();
	}

	bool IsEnd() const
	{
		return m_fixed_layer == 0_layer || (m_iterators[1] == m_iterators[0]->end());
	}

	const Traverser_impl& operator*() const { return *this; }
	const Traverser_impl* operator->() const { return this; }

	BindexType GetPos([[maybe_unused]] LayerType layer) const
	{
		assert(layer == 0_layer);
		return (BindexType)(m_iterators[1] - m_iterators[0]->begin());
	}
	//0層のみを自身のposで初期化する。
	//bpos.GetLayer() >= 1_layerの場合、1以上の層は0とする。
	void GetBpos(Bpos& bpos) const
	{
		assert(bpos.GetLayer() >= 0_layer);
		bpos.GetRow() = GetPos(0_layer);
		LayerType max = bpos.GetLayer();
		for (LayerType l = 1_layer; l <= max; ++l) bpos.GetTpos(l) = 0;
	}

	FieldRef operator[](const RttiPlaceholder& m) const
	{
		return GetField(m);
	}
	FieldRef GetField(const RttiPlaceholder& m) const
	{
		assert(m.GetInternalLayer() <= 0_layer);
		return (*this->GetIterator(m.GetInternalLayer()))[m];
	}
	FieldRef GetField(const RttiPlaceholder& m, BindexType i) const
	{
		assert(m.GetInternalLayer() == 0_layer);
		return (*(m_iterators[0]->begin() + i))[m];
	}

	template <class Type>
	Qualifier<Type>& operator[](const TypedPlaceholder<Type>& m) const
	{
		return GetField(m);
	}
	template <class Type>
	Qualifier<Type>& GetField(const TypedPlaceholder<Type>& m) const
	{
		assert(m.GetInternalLayer() <= 0_layer);
		return (*this->GetIterator(m.GetInternalLayer()))[m];
	}
	template <class Type>
	Qualifier<Type>& GetField(const TypedPlaceholder<Type>& m, BindexType i) const
	{
		assert(m.GetInternalLayer() == 0_layer);
		return (*(m_iterators[0]->begin() + i))[m];
	}

	template <LayerType Layer, class Type>
	Qualifier<Type>& operator[](const CttiPlaceholder<Layer, Type>& m) const
	{
		return GetField(m);
	}
	template <LayerType Layer, class Type>
	Qualifier<Type>& GetField(const CttiPlaceholder<Layer, Type>& m) const
	{
		static_assert(m.GetInternalLayer() <= 0_layer);
		return (*this->GetIterator(m.GetInternalLayer()))[m];
	}
	template <LayerType Layer, class Type>
	Qualifier<Type>& GetField(const CttiPlaceholder<Layer, Type>& m, BindexType i) const
	{
		static_assert(Layer == 0_layer);
		return (*(m_iterators[0]->begin() + i))[m];
	}

	friend bool operator==(const Traverser_impl& self, const Sentinel&)
	{
		return self.IsEnd();
	}
	friend bool operator!=(const Traverser_impl& self, const Sentinel&)
	{
		return !self.IsEnd();
	}
	friend bool operator==(const Sentinel&, const Traverser_impl& self)
	{
		return self.IsEnd();
	}
	friend bool operator!=(const Sentinel&, const Traverser_impl& self)
	{
		return !self.IsEnd();
	}

	const ElementIterator& GetIterator(LayerType layer) const
	{
		assert(layer <= 0);
		return m_iterators[layer + 1];
	}

	LayerType GetFixedLayer() const
	{
		return m_fixed_layer;
	}
	LayerType GetTravLayer() const
	{
		return 0_layer;
	}

	Qualifier<Container>& GetContainer() const { return *m_container; }
	Qualifier<Container>* GetContainerPtr() const { return m_container; }

private:

	LayerType m_fixed_layer = -1_layer;
	Qualifier<Container>* m_container = nullptr;
	std::array<ElementIterator, 2> m_iterators;//[0]が-1層、[1]が0層。
};

template <class Container_, template <class> class Qualifier>
	requires (Container_::MaxLayer == 0)
class Range_impl<Container_, Qualifier> : public std::ranges::view_interface<Range_impl<Container_, Qualifier>>
{
public:
	using Container = Container_;
	using Traverser = Container::Traverser;
	using ConstTraverser = Container::ConstTraverser;

	using Sentinel = Container::Sentinel;
	using ConstSentinel = Container::ConstSentinel;

	static constexpr bool IsConst = std::is_const_v<Qualifier<int>>;

	Range_impl(LayerType fix, Qualifier<Container>& table, LayerType trav)
		: m_fixed_layer(fix), m_table(&table)
	{
		if (trav != 0_layer) throw InvalidLayer("trav layer must be 0 for TableTraverser.");
	}
	Range_impl(Qualifier<Container>& table, LayerType trav = 0_layer)
		: Range_impl((LayerType)-1, table, trav)
	{}
	Range_impl(LayerType fix, Qualifier<Container>& table)
		: Range_impl(fix, table, 0_layer)
	{}

	Range_impl(LayerType fix, Qualifier<Container>& table, const Bpos& pos, LayerType trav)
		: m_fixed_layer(fix), m_bpos(pos), m_table(&table)
	{
		if (trav != 0_layer) throw InvalidLayer("trav layer must be 0 for TableTraverser.");
		if (m_bpos->GetLayer() != 0_layer) throw InvalidLayer("Bpos layer must be 0 for TableTraverser.");
	}
	Range_impl(Qualifier<Container>& table, const Bpos& pos, LayerType trav)
		: Range_impl((LayerType)-1, table, pos, trav)
	{}
	Range_impl(LayerType fix, Qualifier<Container>& table, const Bpos& pos)
		: Range_impl(fix, table, pos, -1_layer)
	{}
	Range_impl(Qualifier<Container>& table, const Bpos& pos)
		: Range_impl((LayerType)-1, table, pos, 0_layer)
	{}

	Range_impl(const Range_impl&) = default;
	Range_impl(Range_impl&&) noexcept = default;

	Range_impl& operator=(const Range_impl&) = default;
	Range_impl& operator=(Range_impl&&) noexcept = default;

	LayerType GetFixedLayer() const { return m_fixed_layer; }
	LayerType GetTravLayer() const { return 0_layer; }
	void SetFixedLayer(LayerType layer) { m_fixed_layer = layer; }
	void SetTravLayer([[maybe_unused]] LayerType layer) { assert(layer == 0_layer); }

	void Assign(LayerType trav)
	{
		if (trav != 0_layer) throw InvalidLayer("trav layer must be 0 for TableTraverser.");
		m_bpos.reset();
	}
	void Assign(LayerType fix, LayerType trav)
	{
		if (trav != 0_layer) throw InvalidLayer("trav layer must be 0 for TableTraverser.");
		m_fixed_layer = fix;
		m_bpos.reset();
	}
	void Assign(LayerType fix, const Bpos& bpos, LayerType trav = 0_layer)
	{
		if (trav != 0_layer) throw InvalidLayer("trav layer must be 0 for TableTraverser.");
		m_fixed_layer = fix;
		m_bpos = bpos;
	}
	void Assign(const Bpos& bpos, LayerType trav = 0_layer)
	{
		Assign(-1_layer, bpos, trav);
	}

	Traverser begin() const requires (!IsConst)
	{
		if (m_bpos.has_value()) return Traverser(m_fixed_layer, *m_table, *m_bpos, ForwardFlag{});
		else return Traverser(m_fixed_layer, *m_table, 0_layer, ForwardFlag{});
	}
	ConstTraverser begin() const requires IsConst
	{
		return cbegin();
	}
	ConstTraverser cbegin() const
	{
		if (m_bpos.has_value()) return ConstTraverser(m_fixed_layer, *m_table, *m_bpos, 0_layer, ForwardFlag{});
		else return ConstTraverser(m_fixed_layer, *m_table, 0_layer, ForwardFlag{});
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

	const Container& GetContainer() const { return *m_table; }

private:
	std::optional<Bpos> m_bpos;
	LayerType m_fixed_layer;
	Qualifier<Container>* m_table;
};

}

}

#endif
