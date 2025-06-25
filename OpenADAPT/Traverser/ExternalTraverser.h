#ifndef ADAPT_EXTERNALTRAVERSER_H
#define ADAPT_EXTERNALTRAVERSER_H

#include <tuple>
#include <optional>
#include <OpenADAPT/Utility/TypeTraits.h>
#include <OpenADAPT/Common/Bpos.h>
#include <OpenADAPT/Traverser/Traverser.h>
#include <OpenADAPT/Traverser/InternalTraverser.h>
#include <OpenADAPT/Joint/JointInterface.h>
#include <OpenADAPT/Joint/JointMethods.h>

namespace adapt
{

namespace detail
{

template <template <class> class Modifier, class Container>
class ExternalSentinel
{};

template <RankType MaxRank_, template <class> class Qualifier, class Container, size_t ...Indices>
class ExternalTraverser<MaxRank_, Qualifier, Container, std::index_sequence<Indices...>>
	: public JointMethods<MaxRank_, Container>
{
	static constexpr RankType MaxRank = MaxRank_;
	using Sentinel = ExternalSentinel<Qualifier, Container>;
	static constexpr bool IsConst = std::is_const_v<Qualifier<char>>;
	template <RankType Rank>
	using IContainer = typename Container::template IContainer<Rank>;
	template <RankType Rank>
	using ITraverser = std::conditional_t<IsConst, typename IContainer<Rank>::ConstTraverser, typename IContainer<Rank>::Traverser>;
	using JointInterface = JointInterface_impl<Container>;

	template <RankType N, class ...IContainers_>
	void Construct(const Container& s, const LayerInfo<MaxRank>& eli, LayerType ext_upper)
	{
		if constexpr (N <= MaxRank)
		{
			auto& i = std::get<N>(m_internals);
			auto& ili = eli.GetInternal(N);
			i.Init(ext_upper, ili, s.template GetContainer<N>());
			if constexpr (N > 0)
			{
				//N==0ならjoinが発生しないので、バッファもいらない。
				auto& b = m_bpos_bufs[N];
				b.Init(i.GetUJointLayer());
			}
			ext_upper += ili.IsIgnored() ? 0 : (ili.GetLJointLayer() - ili.GetUJointLayer());
			Construct<N + 1>(s, eli, ext_upper);
		}
	}
	template <RankType N>
	void Construct(const Container& s, LayerType trav, const JointLayerArray<MaxRank>& l)
	{
		auto& is = s.template GetContainer<N>();
		if (trav < is.ConvertIntToExt(is.GetTravLayer()))
		{
			LayerInfo<MaxRank> eli(l);
			eli.template Enable<N>();
			eli.template SetTravLayer<N>(trav);
			Construct<0>(s, eli, (LayerType)-1);
		}
		else
		{
			Construct<N + 1>(s, trav, l);
		}
	}

public:

	using iterator_category = std::input_iterator_tag;
	using difference_type = ptrdiff_t;
	using reference = ExternalTraverser&;
	using pointer = ExternalTraverser*;
	using value_type = ExternalTraverser;

	//Fixされている、PseudoTopがあるなど、特殊な走査階層設定をしたい場合。
	ExternalTraverser()
		: m_container(nullptr)
	{}
	ExternalTraverser(const Container& s, const LayerInfo<MaxRank>& eli)
		: m_container(&s)
	{
		Construct<0>(s, eli, (LayerType)-1);
		JointMethods<MaxRank_, Container>::CopyFrom(s);
	}

	//最上位から走査順位までがEnabledなような普通の初期化をするためのもの。
	ExternalTraverser(const Container& s, LayerType trav)
		: m_container(&s)
	{
		constexpr JointLayerArray<MaxRank> l = s.GetJointLayers();
		Construct<0>(s, trav, l);
	}

	ExternalTraverser(const ExternalTraverser&) = delete;
	ExternalTraverser(ExternalTraverser&&) noexcept = default;

	ExternalTraverser& operator=(const ExternalTraverser&) = default;
	ExternalTraverser& operator=(ExternalTraverser&&) noexcept = default;

	void CopyFrom(const ExternalTraverser& that)
	{
		JointMethods<MaxRank, Container>::CopyFrom(that);
		m_container = that.m_container;
		m_bpos_bufs = that.m_bpos_bufs;
		DoNothing((std::get<Indices>(m_internals).CopyFrom(std::get<Indices>(that.m_internals)), 0)...);
	}

	template <RankType Rank>
	Qualifier<IContainer<Rank>>& GetIContainer() const { return std::get<Rank>(m_internals).GetContainer(); }
	const Container& GetContainer() const { return *m_container; }

private:
	template <RankType Rank, direction_flag Flag, joint_mode JointMode>
	bool Assign_impl(const ExternalTraverser& that, Flag, JointMode)
	{
		if constexpr (Rank <= MaxRank)
		{
			auto& i = std::get<Rank>(m_internals);
			auto& that_i = std::get<Rank>(that.m_internals);
			assert(!that_i.IsDisabled());
			//disabledまで到達したということは、有効順位は全て初期化成功したということ。
			//trueを返せば良い。
			if (i.IsDisabled()) return true;
			if (!i.template AssignPartially<Rank>(that, Flag{}, JointMode{})) return false;
			return Assign_impl<Rank + 1>(that, Flag{});
		}
		else
		{
			return true;
		}
	}
protected:
	//走査層まで全てをthatの位置で初期化する。
	template <direction_flag Flag, joint_mode JointMode>
	bool Assign(const ExternalTraverser& that, Flag, JointMode)
	{
		return Assign_impl<0>(that, Flag{}, JointMode{});
	}
public:
	//走査層まで全てをthatの位置で初期化する。
	template <direction_flag Flag>
	bool Assign(const ExternalTraverser& that, Flag)
	{
		return Assign_impl<0>(that, Flag{}, DelayedJoint{});
	}
	//走査層まで全てをthatの位置で初期化する。
	bool Assign(const ExternalTraverser& that)
	{
		return Assign_impl<0>(that, ForwardFlag{}, DelayedJoint{});
	}

private:
	template <RankType Rank, direction_flag Flag, joint_mode JointMode>
	bool AssignPartially_rec(const ExternalTraverser& that, Flag, JointMode)
	{
		if constexpr (Rank > MaxRank) return true;
		else
		{
			auto& i = std::get<Rank>(m_internals);
			auto& ithat = std::get<Rank>(that.m_internals);
			assert(!i.IsDisabled());//disabledの場合、少なくともここより上位のいずれかは有効なはずで、そこでreturnされていなければならない。
			if constexpr (std::is_same_v<JointMode, DelayedJoint>)
				if (!ithat.IsJoined(JointMode{}))
					that.Join<Rank>(Flag{}, JointMode{});

			//fixedの場合、ここは完全固定されているRankである。Travまで全層を初期化すれば良い。
			if (i.IsFixed())
			{
				if (!i.template Assign<Rank>(ithat, JointMode{})) return false;
			}
			//PseudoTopまたはEnabledのとき、このRankは走査対象である。
			//特に、ここでこの分岐に入るとすれば、最上位のActiveなTraverser。
			//この場合、fixed layerまでをAssignPartiallyで割り当て、下位はDelayedならRelease、PromptならJoinする。
			//本来AssignPartiallyが呼ばれるのはLayerFuncからなので、既に走査は終えておりRelease済みのはずだが、
			//場合によっては走査が打ち切られていることもあるだろうし、念のため。
			else if (i.IsEnabled() || i.IsPseudoTop())
			{
				//Assignに失敗した場合はfalse。
				if (!i.template AssignPartially<Rank>(ithat, Flag{}, JointMode{})) return false;
				//DelayedJointの場合は、下位はアクセスがあった時に改めてJoinする。今はReleaseだけでよい。
				if constexpr (std::is_same_v<JointMode, DelayedJoint>) { Release<Rank + 1>(); return true; }
				//Promptの場合、下位まで全てJoinする必要がある。
				else return JoinDownward<Rank + 1>(Flag{}, JointMode{});
			}
			//FixedのAssignに成功した場合、およびIGNOREDの場合のみ、ここに到達する。
			return AssignPartially_rec<Rank + 1>(that, Flag{}, JointMode{});
		}
	}
	template <RankType Rank, direction_flag Flag, joint_mode JointMode>
	bool AssignPartially_rec(const Bpos& bpos, Flag, JointMode)
	{
		if constexpr (Rank > MaxRank) return true;
		else
		{
			auto& i = std::get<Rank>(m_internals);
			if (i.IsDisabled()) return true;
			//bposからAssignする場合、通常通りのJoinをまず行う。
			//Joinに失敗したのならfalseを返す。
			if constexpr (Rank > 0_rank)
				if (!i.IsJoined(JointMode{}))
					if (!Join<Rank>(Flag{}, JointMode{})) return false;

			//Fixedの場合、ここは完全固定されているRankである。Travまで全層を初期化すれば良い。
			if (i.IsFixed())
			{
				if (!i.template AssignByExtBpos<Rank>(bpos, JointMode{})) return false;
				//Assignがうまく行ったのなら、そのまま下位も割り当てる。
				return AssignPartially_rec<Rank + 1>(bpos, Flag{}, JointMode{});
			}
			//PseudoTopまたはEnabledのとき、このRankは走査対象である。
			//ujointまではJoinで与えられているので、ujoint + 1からfixedまでを初期化する。
			else if (i.IsEnabled() || i.IsPseudoTop())
			{
				if constexpr (std::is_same_v<JointMode, DelayedJoint>)
				{
					Release<Rank + 1>();
					if (!i.template AssignPartiallyByExtBpos<Rank>(bpos, Flag{}, JointMode{})) return false;
					return AssignPartially_rec<Rank + 1>(bpos, Flag{}, JointMode{});
				}
				else return JoinDownward<Rank + 1>(Flag{}, JointMode{});
			}
			//IGNOREDを含む場合、この関数を呼んではならない。
			throw Forbidden("");
		}
	}
protected:
	//自身のFixedRankのfixed_layerまでをthatで割り当てる。
	template <direction_flag Flag, joint_mode JointMode>
	bool AssignPartially(const ExternalTraverser& that, Flag, JointMode)
	{
		return AssignPartially_rec<0>(that, Flag{}, JointMode{});
	}
	template <direction_flag Flag, joint_mode JointMode>
	bool AssignPartially(const Bpos& bpos, Flag, JointMode)
	{
		return AssignPartially_rec<0>(bpos, Flag{}, JointMode{});
	}
public:
	template <direction_flag Flag>
	bool AssignPartially(const ExternalTraverser& that, Flag) { return AssignPartially(that, Flag{}, DelayedJoint{}); }
	bool AssignPartially(const ExternalTraverser& that) { return AssignPartially(that, ForwardFlag{}, DelayedJoint{}); }

	bool AssignPartially(const Bpos& bpos) { return AssignPartially(bpos, ForwardFlag{}, DelayedJoint{}); }

	//第0位のTraverserの0層のみを引数の値に割り当てる。
	//それより下の階層は最初の要素。
	template <direction_flag Flag, joint_mode JointMode>
	bool AssignRow(BindexType row, Flag, JointMode)
	{
		auto& i = std::get<0>(m_internals);
		assert(i.IsEnabled());
		if (!i.template AssignRow<0>(row)) return false;
		if constexpr (std::same_as<JointMode, DelayedJoint>)
		{
			Release<1>();
			return true;
		}
		else
		{
			return JoinDownward<1>(Flag{}, JointMode{});
		}
	}
	template <direction_flag Flag>
	bool AssignRow(BindexType row, Flag) { return AssignRow(row, Flag{}, DelayedJoint{}); }
	bool AssignRow(BindexType row) { return AssignRow(row, ForwardFlag{}, DelayedJoint{}); }

private:
	template <RankType Rank, joint_mode JointMode>
	bool MoveToBegin_impl(JointMode)
	{
		if constexpr (Rank <= MaxRank)
		{
			auto& i = std::get<Rank>(m_internals);
			assert(!i.IsDisabled());
			if (i.IsFixed()) return MoveToBegin_impl<Rank + 1>(JointMode{});
			i.MoveToBegin();
			if constexpr (std::same_as<JointMode, DelayedJoint>)
			{
				Release<Rank + 1>();
				return true;
			}
			else
			{//promptの場合、Joinに失敗する可能性を想定しなければならない。
				for (; !i.IsEnd(); ++i)
				{
					if (!JoinDownward<Rank + 1>(JointMode{})) continue;
					return true;
				}
				return false;
			}
		}
		else throw Forbidden("");
	}
	template <RankType Rank>
	bool MoveToEnd_impl()
	{
		if constexpr (Rank <= MaxRank)
		{
			auto& i = std::get<Rank>(m_internals);
			assert(!i.IsDisabled());
			if (i.IsFixed()) return MoveToEnd_impl<Rank + 1>();
			return i.MoveToEnd();
		}
		else throw Forbidden("");
	}
public:
	template <joint_mode JointMode>
	bool MoveToBegin(JointMode) { return MoveToBegin_impl<0>(JointMode{}); }
	bool MoveToBegin() { return MoveToBegin(DelayedJoint{}); }
	template <joint_mode JointMode>
	bool MoveToEnd(JointMode) { return MoveToEnd_impl<0>(); }
	bool MoveToEnd() { return MoveToEnd(DelayedJoint{}); }


private:
	template <RankType Rank>
	struct Join_f
	{
		template <class Flag>
		bool operator()(Flag)
		{
			//Rank - 1のTraverserの移動が可能であると判定された場合に、PromptJointModeであれば呼ばれる。
			//RankのTraverserのJoinが可能かどうかをさらに判定する。
			if constexpr (Rank <= MaxRank)
			{
				auto& j = mExternal.template GetJoint<Rank>();
				auto& i = std::get<Rank>(mExternal.m_internals);
				if (i.IsDisabled()) return true;
				if (!j->Find(*this)) return false;
				if (!i.Assign(j->GetBpos(), Flag{})) return false;
				return Join_f<Rank + 1>()(Flag{});
			}
			else
			{
				return true;
			}
		}
		ExternalTraverser& mExternal;
	};

	template <RankType Rank, direction_flag Flag, joint_mode JointMode>
	bool Move_impl(LayerType layer, Flag, JointMode)
	{
		if constexpr (Rank >= 0)
		{
			auto& i = std::get<Rank>(m_internals);
			assert(i.IsEnabled());
			if (i.GetExtUpperJLayer() >= layer) return Move_impl<Rank - 1>(layer, Flag{}, JointMode{});
			//ImmediateJointの場合、Move可能かどうかを判定するためにはJoinまで実行しなければならない。
			//ただしMove可能かどうかは動的メモリ確保を避けたい場合再帰的にしか判定できないので、
			//再帰のためのMove_recを渡す。
			LayerType ilayer = i.ConvertExtToInt(layer);
			if constexpr (std::is_same_v<JointMode, PromptJoint>)
			{
				//Promptなら移動した時点で完了。
				//InternalのMoveとそれに与えるJoin_fが全て判定するので、この関数中でループさせる必要はない。
				//return i.Move(ilayer, Flag{}, Join_f<Rank + 1>{ *this });
				while (i.Move(ilayer, Flag{}))
				{
					if (JoinDownward<Rank + 1>(Flag{}, PromptJoint{})) return true;
				}
				return false;
			}
			else
			{
				//Rank == 0のときはIsJoinedが常にfalseなので、この条件分岐は行わない。
				if constexpr (Rank > 0)
					if (!i.IsJoined(JointMode{})) return false;
				bool res = i.Move(ilayer, Flag{});
				//Delayedの場合、移動成功と判定されたとしても、Rank + 1以下はまだJoinされていない。
				//移動に成功した場合は、下位のリリースを行う。
				if (res) Release<Rank + 1>();
				return res;
			}
		}
		else
		{
			//Rank == -1まで到達してしまった場合は明らかにエラーである。
			throw Forbidden("");
		}
	}
public:

	//Moveは正規連結でDisabledなInternalが存在しないことを前提に効率化している。
	//よって、local traverserから呼ぶことはできない。operator++を使うこと。
	template <direction_flag Flag, joint_mode JointMode>
	bool Move(LayerType layer, Flag, JointMode)
	{
		//Moveの呼び出しは普通に考えて下位ほど多くなるはずなので、
		//下位から順に判定する。
		return Move_impl<MaxRank>(layer, Flag{}, JointMode{});
	}
	template <direction_flag Flag>
	bool Move(LayerType layer, Flag) { return Move(layer, Flag{}, DelayedJoint{}); }

	template <joint_mode JointMode>
	bool MoveForward(LayerType layer, JointMode) { return Move(layer, ForwardFlag{}, JointMode{}); }
	template <joint_mode JointMode>
	bool MoveBackward(LayerType layer, JointMode) { return Move(layer, BackwardFlag{}, JointMode{}); }

	bool MoveForward(LayerType layer) { return Move(layer, ForwardFlag{}, DelayedJoint{}); }
	bool MoveBackward(LayerType layer) { return Move(layer, BackwardFlag{}, DelayedJoint{}); }

private:
	template <RankType Rank, direction_flag Flag>
	LayerType IncrDecrOperator(Flag, PromptJoint)
	{
		constexpr bool IsForward = std::is_same_v<Flag, ForwardFlag>;
		if constexpr (Rank >= 0)
		{
			auto& i = std::get<Rank>(m_internals);
			if (i.IsDisabled()) return IncrDecrOperator<Rank - 1>(Flag{}, PromptJoint{});
			//Promptの場合、この時点でDisabledでないTraverserがJoinされていないことはありえない。
			//よって、素直にoperator++を呼び、Joinできるか確認する。
			//assert(i.IsJoined(PromptJoint{}));
			if constexpr (IsForward)
			{
				LayerType moved = i.IncrDecrOperator(Flag{});
				while (!i.IsEnd())
				{
					//次の要素に移動できた場合は、
					//Immediateの場合、Joinできるか確認する。
					if (JoinDownward<Rank + 1>(Flag{}, PromptJoint{})) return i.ConvertIntToExt(moved);
					return i.IncrDecrOperator(Flag{});
				}
			}
			else
			{
				/*while (!i.IsBegin())
				{
					--i;
					if (!JoinDownward<Rank + 1>(Flag{}, PromptJoint{})) continue;
					return true;
					return i.ConvertIntToExt(moved);
				}*/
				LayerType fixed = i.GetFixedLayer();
				LayerType moved = fixed;
				while ((moved = i.Decr()) != fixed)
				{
					if (!JoinDownward<Rank + 1>(Flag{}, PromptJoint{})) continue;
					return i.ConvertIntToExt(moved);
				}
			}
			//もし走査を終えてしまった場合、上位に投げる。
			return IncrDecrOperator<Rank - 1>(Flag{}, PromptJoint{});
		}
		else
		{
			return -1_layer;
		}
	}
	template <RankType Rank, direction_flag Flag>
	LayerType IncrDecrOperator(Flag, DelayedJoint)
	{
		constexpr bool IsForward = std::is_same_v<Flag, ForwardFlag>;
		if constexpr (Rank >= 0)
		{
			auto& i = std::get<Rank>(m_internals);
			if (i.IsDisabled()) return IncrDecrOperator<Rank - 1>(Flag{}, DelayedJoint{});
			if (i.IsFixed())
			{
				//fixedということは、下位の走査を終えてしまったということ。
				//この場合は全ての順位のTraverserをReleaseする。
				Release<0>();
				return i.ConvertIntToExt(i.GetLJointLayer());
			}
			//Delayedの場合、この順位は連結されていないことがある。
			//このときはRank位は存在しないものと扱い、上位に投げる。
			//ただしRank == 0のときは常にfalseなので、その場合は無視。
			if constexpr (Rank != 0)
				if (!i.IsJoined(DelayedJoint{}))
					return IncrDecrOperator<Rank - 1>(Flag{}, DelayedJoint{});
			LayerType moved = i.IncrDecrOperator(Flag{});
			//この順位の走査を終えてしまった場合、上位の移動を行う。
			//走査を終えたかどうかの判定条件がForwardとBackwardで異なる。
			//Forwardの場合は、IsEndで末尾に達したかどうかを見るのが確実。
			if constexpr (IsForward)
			{
				if (i.IsEnd())
					return IncrDecrOperator<Rank - 1>(Flag{}, DelayedJoint{});
			}
			//Backwardの場合は、IsBegin相当の関数はちょっと作れない。代わりにmovedの値を見る。
			//movedの値は、もしiの移動を終えてしまった場合、fixedに一致するはずなので、
			//その場合は上位に投げる。
			else
			{
				if (moved == i.GetFixedLayer())
					return IncrDecrOperator<Rank - 1>(Flag{}, DelayedJoint{});
			}
			Release<Rank + 1>();
			return i.ConvertIntToExt(moved);
		}
		else
		{
			Release<0>();
			return -1_layer;
		}
	}
public:
	template <direction_flag Flag, joint_mode JointMode>
	LayerType IncrDecrOperator(Flag, JointMode)
	{
		if constexpr (std::same_as<Flag, BackwardFlag>)
		{
			//backwardの場合、IsEnd==trueのときは0位がEnd状態なので、
			//まずこれを一つ前に戻す必要がある。
			if (IsEnd()) return IncrDecrOperator<0>(BackwardFlag{}, JointMode{});
			else return IncrDecrOperator<MaxRank>(BackwardFlag{}, JointMode{});
		}
		else return IncrDecrOperator<MaxRank>(Flag{}, JointMode{});
	}
	template <direction_flag Flag>
	LayerType IncrDecrOperator(Flag) { return IncrDecrOperator(Flag{}, DelayedJoint{}); }

	template <joint_mode JointMode>
	LayerType Incr(JointMode) { return IncrDecrOperator(ForwardFlag{}, JointMode{}); }
	template <joint_mode JointMode>
	LayerType Decr(JointMode) { return IncrDecrOperator(BackwardFlag{}, JointMode{}); }

	LayerType Incr() { return Incr(DelayedJoint{}); }
	LayerType Decr() { return Decr(DelayedJoint{}); }
	ExternalTraverser& operator++() { Incr(); return *this; }
	ExternalTraverser& operator--() { Decr(); return *this; }
	void operator++(int) { ++(*this); }//本来operator++(int)の戻り地はExternalTraverserにすべきだが、適用前のExternalTraverserなど返しようがない。
	void operator--(int) { --(*this); }

private:
	template <RankType Rank>
	bool Equal_impl(const ExternalTraverser& that) const
	{
		if constexpr (Rank <= MaxRank)
		{
			auto& ithis = std::get<Rank>(m_internals);
			auto& ithat = std::get<Rank>(that.m_internals);
			assert(ithis.GetFixedLayer() == ithat.GetFixedLayer());
			assert(ithis.GetTravLayer() == ithat.GetTravLayer());
			assert(ithis.GetExtUpperJLayer() == ithat.GetExtUpperJLayer());
			assert(ithis.GetUJointLayer() == ithat.GetUJointLayer());
			assert(ithis.GetLJointLayer() == ithat.GetLJointLayer());
			if (ithis != ithat) return false;
			return Equal_impl<Rank + 1>(that);
		}
		else
			return true;
	}
public:
	//すべての位置が一致することを要求する。
	bool operator==(const ExternalTraverser& that) const
	{
		return Equal_impl<0>(that);
	}
	bool operator!=(const ExternalTraverser& that) const
	{
		return !operator==(that);
	}

private:
	template <RankType Rank>
	bool IsEnd_rec() const
	{
		if constexpr (Rank <= MaxRank)
		{
			auto& i = std::get<Rank>(m_internals);
			if (i.IsFixed()) return IsEnd_rec<Rank + 1>();
			assert(i.IsEnabled() || i.IsPseudoTop());
			return i.IsEnd();
		}
		else throw Forbidden("");
	}
public:
	bool IsEnd() const { return IsEnd_rec<0>(); }
	friend bool operator==(const ExternalTraverser& self, const Sentinel&) { return self.IsEnd(); }
	friend bool operator==(const Sentinel&, const ExternalTraverser& self) { return self.IsEnd(); }
	friend bool operator!=(const ExternalTraverser& self, const Sentinel& t) { return !(self == t); }
	friend bool operator!=(const Sentinel& t, const ExternalTraverser& self) { return !(t == self); }

private:
	template <RankType Rank, direction_flag Flag, joint_mode JointMode>
	bool MatchPartially_rec(const JBpos& jbp, RankType rank, LayerType intlayer, Flag, JointMode) const
	{
		if constexpr (Rank > MaxRank) throw Forbidden("");
		else
		{
			auto& i = std::get<Rank>(m_internals);
			if constexpr (std::is_same_v<JointMode, DelayedJoint> && Rank > 0)
			{
				if (!i.IsJoined(DelayedJoint{}))
				{
					bool b = Join<Rank>(Flag{}, DelayedJoint{});
					if (!b) throw JointError();
				}
			}
			if (Rank == rank)
			{
				return i.MatchPartially(jbp[Rank], intlayer);
			}
			else
			{
				if (!i.Match(jbp[Rank])) return false;
				return MatchPartially_rec<Rank + 1>(jbp, rank, intlayer, Flag{}, DelayedJoint{});
			}
		}
	}
public:
	template <direction_flag Flag, joint_mode JointMode>
	bool MatchPartially(const JBpos& jbp, RankType rank, LayerType intlayer, Flag, JointMode) const
	{
		return MatchPartially_rec<0>(jbp, rank, intlayer, Flag{}, JointMode{});
	}
	template <direction_flag Flag>
	bool MatchPartially(const JBpos& jbp, RankType rank, LayerType intlayer, Flag) const
	{
		return MatchPartially(jbp, rank, intlayer, Flag{}, DelayedJoint{});
	}
	bool MatchPartially(const JBpos& jbp, RankType rank, LayerType intlayer) const
	{
		return MatchPartially(jbp, rank, intlayer, ForwardFlag{}, DelayedJoint{});
	}

	template <RankType Rank, direction_flag Flag>
	bool TryJoin(Flag) const
	{
		auto& i = std::get<Rank>(m_internals);
		if (i.IsJoined(DelayedJoint{})) return true;
		return JoinRecursively<Rank>(Flag{}, DelayedJoint{});
	}
	template <RankType Rank, direction_flag Flag>
	bool TryJoin(RankConstant<Rank>, Flag) const
	{
		auto& i = std::get<Rank>(m_internals);
		if (i.IsJoined(DelayedJoint{})) return true;
		return JoinRecursively<Rank>(Flag{}, DelayedJoint{});
	}
	template <RankType Rank>
	bool TryJoin() const { return TryJoin<Rank>(ForwardFlag{}); }
	template <RankType Rank>
	bool TryJoin(RankConstant<Rank>) const { return TryJoin<Rank>(ForwardFlag{}); }

	const ExternalTraverser& operator*() const { return *this; }
	const ExternalTraverser* operator->() const { return this; }

protected:
	template <class Placeholder, direction_flag Flag, joint_mode JointMode>
	decltype(auto) GetField(const Placeholder& ph, Flag, JointMode) const
	{
		constexpr RankType Rank = Placeholder::Rank;
		auto& i = std::get<Rank>(m_internals);
		if constexpr (std::is_same_v<JointMode, DelayedJoint> && Rank > 0)
		{
			//PromptJointなら既に連結済みのはずである。
			if (!i.IsJoined(DelayedJoint{}) && !JoinRecursively<Rank>(Flag{}, DelayedJoint{}))
				throw JointError();
		}
		return i[ph];
	}
public:
	template <class Placeholder, direction_flag Flag>
	decltype(auto) GetField(const Placeholder& ph, Flag) const { return GetField(ph, Flag{}, DelayedJoint{}); }
	template <class Placeholder>
	decltype(auto) GetField(const Placeholder& ph) const { return GetField(ph, ForwardFlag{}, DelayedJoint{}); }

	template <class Placeholder>
	decltype(auto) operator[](const Placeholder& ph) const { return GetField(ph, ForwardFlag{}, DelayedJoint{}); }

protected:
	template <class Placeholder, direction_flag Flag, joint_mode JointMode, std::integral ...Inds>
	decltype(auto) GetField(const Placeholder& ph, Flag, JointMode, Inds ...is) const
	{
		constexpr RankType Rank = Placeholder::Rank;
		auto& i = std::get<Rank>(m_internals);
		if constexpr (std::is_same_v<JointMode, DelayedJoint> && Rank > 0)
		{
			//PromptJointなら既に連結済みのはずである。
			if (!i.IsJoined(DelayedJoint{}) && !JoinRecursively<Rank>(Flag{}, DelayedJoint{})) throw JointError();
		}
		return i.GetField(ph, is...);
	}
public:
	template <class Placeholder, direction_flag Flag, std::integral ...Inds>
	decltype(auto) GetField(const Placeholder& ph, Flag, Inds ...is) const { return GetField(ph, Flag{}, DelayedJoint{}, is...); }
	template <class Placeholder, std::integral ...Inds>
	decltype(auto) GetField(const Placeholder& ph, Inds ...is) const { return GetField(ph, ForwardFlag{}, DelayedJoint{}, is...); }

protected:
	template <RankType Rank, direction_flag Flag, joint_mode JointMode>
	BindexType GetPos_rec(LayerType extlayer, Flag, JointMode) const
	{
		if constexpr (Rank > MaxRank) throw NoElements();
		else
		{
			auto& i = std::get<Rank>(m_internals);
			if (i.IsDisabled()) throw NoElements();
			if constexpr (std::is_same_v<JointMode, DelayedJoint> && Rank > 0)
			{
				if (!i.IsJoined(DelayedJoint{}))
				{
					bool b = Join<Rank>(Flag{}, DelayedJoint{});
					if (!b) throw JointError();
				}
			}
			LayerType intlayer = extlayer;
			intlayer = i.ConvertExtToInt(intlayer);
			if (i.GetTravLayer() >= intlayer) return i.GetPos(intlayer);
			return GetPos_rec<Rank + 1>(extlayer, Flag{}, DelayedJoint{});
		}
	}
	template <RankType Rank, direction_flag Flag, joint_mode JointMode>
	void GetBpos_rec(Bpos& bpos, LayerType from, LayerType max, Flag, JointMode) const
	{
		if constexpr (Rank > MaxRank) throw NoElements();
		else
		{
			auto& i = std::get<Rank>(m_internals);
			if (i.IsDisabled()) throw NoElements();
			if constexpr (std::is_same_v<JointMode, DelayedJoint> && Rank > 0)
			{
				if (!i.IsJoined(DelayedJoint{}))
				{
					bool b = Join<Rank>(Flag{}, DelayedJoint{});
					if (!b) throw JointError();
				}
			}
			LayerType l = from;
			LayerType exttrav = i.ConvertIntToExt(i.GetTravLayer());
			while (l <= exttrav && l <= max)
			{
				LayerType intlayer = i.ConvertExtToInt(l);
				bpos[l] = i.GetPos(intlayer);
				++l;
			}
			if (l > max) return;
			GetBpos_rec<Rank + 1>(bpos, l, max, Flag{}, DelayedJoint{});
		}
	}
	template <RankType Rank, direction_flag Flag, joint_mode JointMode>
	void GetJBpos_rec(JBpos& jbp, Flag, JointMode) const
	{
		if constexpr (Rank > MaxRank) return;
		else
		{
			auto& i = std::get<Rank>(m_internals);
			if (i.IsDisabled())
			{
				//assert(jbp[Rank].GetLayer() == LayerType(-1));
				jbp[Rank].Init(-1);
				return;
			}
			if constexpr (std::is_same_v<JointMode, DelayedJoint> && Rank > 0)
			{
				if (!i.IsJoined(JointMode{})) Join<Rank>(Flag{}, JointMode{});
			}
			i.GetBpos(jbp[Rank]);
			GetJBpos_rec<Rank + 1>(jbp, Flag{}, DelayedJoint{});
		}
	}

public:

	BindexType GetRow() const { return std::get<0>(m_internals).GetRow(); }
	template <direction_flag Flag>
	BindexType GetPos(LayerType layer, Flag) const { return GetPos_rec<(RankType)0>(layer, Flag{}, DelayedJoint{}); }
	BindexType GetPos(LayerType layer) const { return GetPos(layer, ForwardFlag{}); }

	//std::min(trav layer, bpos.GetLayer())までを自身のposで初期化する。
	//もしbpos.GetLayer() > trav layerの場合、余剰分は0で初期化する。
	template <direction_flag Flag>
	void GetBpos(Bpos& bpos, Flag) const { return GetBpos_rec<0_layer>(bpos, 0_layer, std::min(bpos.GetLayer(), GetTravLayer()), Flag{}, DelayedJoint{}); }
	void GetBpos(Bpos& bpos) const { return GetBpos(bpos, ForwardFlag{}); }

	template <direction_flag Flag>
	void GetJBpos(JBpos& jbp, Flag) const { GetJBpos_rec<(RankType)0>(jbp, Flag{}, DelayedJoint{}); }
	void GetJBpos(JBpos& jbp) const { GetJBpos(jbp, ForwardFlag{}); }
	//void ResetJBpos(JBpos& jbp) const { jbp.Init(MaxRank); ResetJBpos_rec<(RankType)0>(jbp); }

protected:
	//Rank-1まではJoin済みであるとして、Rank位のみを連結する。
	template <RankType Rank, direction_flag Flag, joint_mode JointMode>
	bool Join(Flag, JointMode) const
	{
		if constexpr (Rank > 0)
		{
			auto& j = this->template GetJoint<Rank>();
			auto& i = std::get<Rank>(m_internals);
			//disableされている場合、そもそもこの順位にアクセスを試みている事自体がおかしい。
			//if (i.IsDisabled()) throw Terminate(std::format("rank %d is disabled", Rank));
			assert(!i.IsDisabled());
			//固定順位の場合はJoinではなくouterからのAssignで割り当てられるため、
			//traverser引数の場合はJoinは呼ばれないが、
			//Container+bposで呼び出す場合は結局assignpartially内部でjoinを呼ぶので、
			//IsFixed==trueであってもこの関数が呼ばれる可能性はある。
			//assert(!i.IsFixed());
			auto& buf = std::get<Rank>(m_bpos_bufs);
			//Findして見つからなければ失敗。
			if (!j->Find(*this, buf)) return false;
			//AssignPartiallyは通常、fixed layerまでを自動的に割り当てるが、
			//Internal Traverserの場合、fixed != ujointである場合があるため、階層は明示する必要がある。
			//Assignできない（例えば下層要素が空）なら失敗。
			return i.template AssignPartially<Rank>(buf, i.GetUJointLayer(), Flag{}, JointMode{});
		}
		else
		{
			return true;
		}
	}
public:
	template <RankType Rank, direction_flag Flag>
	bool Join(Flag) const { return Join(Flag{}, DelayedJoint{}); }
	template <RankType Rank>
	bool Join() const { return Join(ForwardFlag{}, DelayedJoint{}); }

protected:
	//1位からRank位までを順にJoinする。
	//Join済みのときに呼んでも構わない。
	//Delayedのときに使う。
	template <RankType Rank, direction_flag Flag>
	bool JoinRecursively(Flag, DelayedJoint) const
	{
		if constexpr (Rank > 0)
		{
			//上位のJoinに失敗するのならこの順位もJoin不可能である。
			if (!JoinRecursively<Rank - 1>(Flag{}, DelayedJoint{})) return false;
			auto& i = std::get<Rank>(m_internals);
			assert(!i.IsDisabled());
			if (!i.IsJoined(DelayedJoint{})) return Join<Rank>(Flag{}, DelayedJoint{});
			else return true;
		}
		else
		{
			return true;
		}
	}
public:
	template <RankType Rank, direction_flag Flag>
	bool JoinRecursively(Flag) const { return JoinRecursively<Rank>(Flag{}, DelayedJoint{}); }
	template <RankType Rank>
	bool JoinRecursively() const { return JoinRecursively<Rank>(ForwardFlag{}); }

private:
	template <RankType Rank, direction_flag Flag>
	bool JoinDownward(Flag, PromptJoint)
	{
		//Promptの場合にのみ、上位の移動やAssignに成功した際に呼ばれる。
		//Joinを試み、もし成功したのなら更に下位のJoinを試みる。
		//自身のJoinに失敗した場合は上位へとfalseを返し、上位の移動を促す。
		//Rank + 1のJoinが失敗した場合は次へ移動する。
		//自身が末尾に達した場合も同様に上位の移動処理へ回す。
		static_assert(Rank != 0);
		constexpr bool IsForward = std::is_same_v<Flag, ForwardFlag>;
		if constexpr (Rank <= MaxRank)
		{
			//auto& j = this->GetJoint<Rank>();
			auto& i = std::get<Rank>(m_internals);
			if (i.IsDisabled()) return true;
			if (!Join<Rank>(Flag{}, PromptJoint{})) return false;
			if constexpr (IsForward)
			{
				for (; !i.IsEnd(); ++i)
				{
					if (!JoinDownward<Rank + 1>(Flag{}, PromptJoint{})) continue;
					return true;
				}
			}
			else
			{
				/*IsBeginは判定が非常に難しく高コストなのでやめる。
				while (!i.IsBegin())
				{
					--i;
					if (!JoinDownward<Rank + 1>(Flag{})) continue;
					return true;
				}*/
				LayerType fixed = i.GetFixedLayer();
				do//fixedが返ってくるということは、begin()にいる。
				{
					if (!JoinDownward<Rank + 1>(Flag{}, PromptJoint{})) continue;
					return true;
				} while (i.Decr() != fixed);
			}
			//ここでは通常falseを返す（つまりJoinに失敗した）べきなのだが、
			//例外として、このRankがtrav == fixedの場合がありうる。
			//この場合、Joinには成功しているが必ずEnd状態なので、
			//trueを返さなければならない。
			return i.GetTravLayer() == i.GetFixedLayer();
		}
		else
		{
			return true;
		}
	}

	//Rank位以下のJointを切り離す。
	template <RankType Rank>
	void Release() const
	{
		if constexpr (Rank <= MaxRank)
		{
			if constexpr (Rank > 0)
			{
				auto& i = std::get<Rank>(m_internals);
				if (i.IsDisabled()) return;
				i.Release(DelayedJoint{});
			}
			return Release<Rank + 1>();
		}
		else return;
	}


	template <RankType Rank>
	LayerType GetFixedLayer_rec() const
	{
		if constexpr (Rank <= MaxRank)
		{
			const auto& i = std::get<Rank>(m_internals);
			assert(!i.IsIgnored());//ignoredを含むようなものはrank付き階層関数中でしか存在し得ない。よって、ここでは不適切。
			assert(!i.IsPseudoTop());//同上。PseudoTopはIgnoredの下位にあるはずなので、ここでassertに引っかかるとしたらバグである。
			assert(!i.IsDisabled());//
			if (i.IsEnabled())
			{
				return i.ConvertIntToExt(i.GetFixedLayer());
			}
			else
			{
				assert(i.IsFixed());
				//fixedだとすれば完全固定されているので、ここは無視していいはず。
				return GetFixedLayer_rec<Rank + 1_rank>();
			}
		}
		else throw Forbidden("");
	}
	template <RankType Rank>
	LayerType GetTravLayer_rec() const
	{
		if constexpr (Rank >= 0)
		{
			const auto& i = std::get<Rank>(m_internals);
			assert(!i.IsIgnored());//ignoredを含むようなものはrank付き階層関数中でしか存在し得ない。よって、ここでは不適切。
			assert(!i.IsPseudoTop());//同上。PseudoTopはIgnoredの下位にあるはずなので、ここでassertに引っかかるとしたらバグである。
			assert(!i.IsFixed());//fixedよりも前にenabledがあるはず。
			if (i.IsEnabled())
			{
				return i.ConvertIntToExt(i.GetTravLayer());
			}
			else
			{
				assert(i.IsDisabled());
				//disabledの場合はより上位のenabledを探しに行く。
				return GetTravLayer_rec<Rank - 1_rank>();
			}
		}
		else throw Forbidden("");
	}

public:
	LayerType GetFixedLayer() const
	{
		return GetFixedLayer_rec<(RankType)0>();
	}
	LayerType GetTravLayer() const
	{
		return GetTravLayer_rec<MaxRank>();
	}

private:

	//std::tuple<std::unique_ptr<PseudoArgued_t<Container, JointInterface>>...> m_joints;
	const Container* m_container;
	mutable std::array<Bpos, MaxRank + 1> m_bpos_bufs;//Findの結果を受け取るためだけの一時的なバッファ。
	mutable std::tuple<InternalTraverser<Qualifier, ITraverser<Indices>>...> m_internals;
};


template <template <class> class Qualifier, class Container>
class ExternalSentinel_prompt : public ExternalSentinel<Qualifier, Container>
{};
template <RankType MaxRank_, template <class> class Qualifier, class Container, class IS = std::make_index_sequence<MaxRank_ + 1>>
class ExternalTraverser_prompt : public ExternalTraverser<MaxRank_, Qualifier, Container, IS>
{
public:
	//PromptJointで動作するExternalTraverser。
	//このクラスは、DelayedJointを基本とするExternalTraverserを継承する必要がある。
	//何故なら、Evaluatorの各種NodeはDelayedのExternalTraverserを引数に取っており、
	//ExternalTraverser_promptはDelayedのExternalTraverserに変換可能でなければならないため。
	using Base = ExternalTraverser<MaxRank_, Qualifier, Container, IS>;
	using Base::Base;
	using Sentinel = ExternalSentinel_prompt<Qualifier, Container>;

	template <direction_flag Flag>
	bool Assign(const ExternalTraverser_prompt& that, Flag) { return Base::Assign(that, Flag{}, PromptJoint{}); }
	bool Assign(const ExternalTraverser_prompt& that) { return Base::Assign(that, ForwardFlag{}, PromptJoint{}); }

	template <direction_flag Flag>
	bool AssignPartially(const ExternalTraverser_prompt& that, Flag) { return Base::AssignPartially(that, Flag{}, PromptJoint{}); }
	bool AssignPartially(const ExternalTraverser_prompt& that) { return Base::AssignPartially(that, ForwardFlag{}, PromptJoint{}); }

	bool MoveForward(LayerType layer) { return Base::Move(layer, ForwardFlag{}, PromptJoint{}); }
	bool MoveBackward(LayerType layer) { return Base::Move(layer, BackwardFlag{}, PromptJoint{}); }

	ExternalTraverser_prompt& operator++() { Base::Incr(PromptJoint{}); return *this; }
	ExternalTraverser_prompt& operator--() { Base::Decr(PromptJoint{}); return *this; }
	void operator++(int) { ++(*this); }//本来operator++(int)の戻り地はExternalTraverserにすべきだが、適用前のExternalTraverserなど返しようがない。
	void operator--(int) { --(*this); }

	using Base::operator==;
	using Base::operator!=;
	friend bool operator==(const ExternalTraverser_prompt& self, const Sentinel&) { return self.IsEnd(); }
	friend bool operator==(const Sentinel&, const ExternalTraverser_prompt& self) { return self.IsEnd(); }
	friend bool operator!=(const ExternalTraverser_prompt& self, const Sentinel& t) { return !(self == t); }
	friend bool operator!=(const Sentinel& t, const ExternalTraverser_prompt& self) { return !(t == self); }

	template <class Placeholder, direction_flag Flag>
	decltype(auto) GetField(const Placeholder& ph, Flag) const { return Base::GetField(ph, Flag{}, PromptJoint{}); }
	template <class Placeholder>
	decltype(auto) GetField(const Placeholder& ph) const { return Base::GetField(ph, ForwardFlag{}, PromptJoint{}); }

	//template <class Placeholder, direction_flag Flag>
	//decltype(auto) operator[](const Placeholder& ph, Flag) const { return Base::GetField(ph, Flag{}, PromptJoint{}); }
	template <class Placeholder>
	decltype(auto) operator[](const Placeholder& ph) const { return Base::GetField(ph, ForwardFlag{}, PromptJoint{}); }

	template <class Placeholder, direction_flag Flag, std::integral ...Indices>
	decltype(auto) GetField(const Placeholder& ph, Flag, Indices ...is) const { return Base::GetField(ph, Flag{}, PromptJoint{}, is...); }
	template <class Placeholder, std::integral ...Indices>
	decltype(auto) GetField(const Placeholder& ph, Indices ...is) const { return Base::GetField(ph, ForwardFlag{}, PromptJoint{}, is...); }

	template <direction_flag Flag>
	BindexType GetPos(LayerType layer, Flag) const { return Base::template GetPos_rec<(RankType)0>(layer, Flag{}, PromptJoint{}); }
	BindexType GetPos(LayerType layer) const { return Base::template GetPos_rec<(RankType)0>(layer, ForwardFlag{}, PromptJoint{}); }
	template <direction_flag Flag>
	void GetJBpos(JBpos& jbp, Flag) const { Base::template GetJBpos_rec<(RankType)0>(jbp, Flag{}, PromptJoint{}); }
	void GetJBpos(JBpos& jbp) const { Base::template GetJBpos_rec<(RankType)0>(jbp, ForwardFlag{}, PromptJoint{}); }

};

}

}

#endif