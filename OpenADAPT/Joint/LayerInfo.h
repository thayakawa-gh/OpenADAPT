#ifndef ADAPT_JOINT_LAYERINFO_H
#define ADAPT_JOINT_LAYERINFO_H

#include <type_traits>
#include <OpenADAPT/Utility/Exception.h>
#include <OpenADAPT/Utility/Ranges.h>
#include <OpenADAPT/Common/Bpos.h>
#include <OpenADAPT/Common/Concepts.h>

namespace adapt
{

class InternalLayerInfo
{
public:

	//Stateについて。
	//例えば、最上位からRank位までが操作対象となっているとき、次のようにRank位まではENABLED、それ以下はDISABLEDとなる。
	//0:ENABLED ... Rank-1:ENABLED, Rank:ENABLED, Rank+1:DISABLED, Rank+2:DISABLED ...
	//例えば、階層関数によってRank位がFixされ、走査順位がRank+1であるとき、次のようにRank-1位までがFIXED、Rank位以降の走査位までがENABLEDである。
	//0:FIXED ... Rank-1:FIXED, Rank:ENABLED, Rank+1:ENABLED, Rank+2:DISABLED ...
	//例えば、順位指定階層関数によってRankが疑似最上位であるとき、次のようになる。
	//0:IGNORED ... Rank-1:IGNORED, Rank:PSEUDOTOP, Rank+1:ENABLED, Rank+2:DISABLED ...
	enum class State
	{
		DISABLED,  //初期状態。使用されないTraverser。Set関数によってENABLEDに変更される。
		IGNORED,   //DISABLEDよりも強い無効化。rank付き階層関数によってのみこの状態が作られる。IGNORED状態は解除できない。
		ENABLED,   //有効なTRAVERSER。
		PSEUDOTOP, //特殊な有効状態。上位はIGNOREDされ、自身が事実上の最上位として扱われる。
		FIXED,     //無効状態であるが、これはFixによって上位のTraverserが固定されている状態を意味する。
	};
	using enum State;

	constexpr InternalLayerInfo()
		: m_ujoint_layer(-1), m_ljoint_layer(-1), m_fixed_layer(-1), m_trav_layer(-1), m_state(DISABLED)
	{}
	constexpr InternalLayerInfo(std::pair<LayerType, LayerType> j)
	{
		*this = j;
	}
	constexpr InternalLayerInfo(LayerType u, LayerType l)
	{
		*this = std::make_pair(u, l);
	}

	constexpr InternalLayerInfo& operator=(std::pair<LayerType, LayerType> j)
	{
		if (j.first < -1) throw InvalidArg("upper joint layer must be equal or greater than -1.");
		if (j.second < -1) throw InvalidArg("lower joint layer must be equal or greater than 0 except the lowest container.");
		if (j.second != -1 && j.first > j.second) throw InvalidArg("lower joint layer of non-lowest container ");
		m_ujoint_layer = j.first;
		m_ljoint_layer = j.second;
		m_fixed_layer = j.first;
		m_trav_layer = j.first;
		m_state = DISABLED;
		return *this;
	}
	constexpr InternalLayerInfo(const InternalLayerInfo& l) = default;

	constexpr bool operator==(const InternalLayerInfo& l) const = default;
	constexpr bool operator!=(const InternalLayerInfo& l) const = default;


	constexpr LayerType GetUJointLayer() const { return m_ujoint_layer; }
	constexpr LayerType GetLJointLayer() const { return m_ljoint_layer; }
	constexpr LayerType GetFixedLayer() const { return m_fixed_layer; }
	constexpr LayerType GetTravLayer() const { return m_trav_layer; }

	constexpr bool IsNotInitialied() const
	{
		return m_ujoint_layer == -1 &&
			m_ljoint_layer == -1 &&
			m_fixed_layer == -1 &&
			m_trav_layer == -1 &&
			m_state == DISABLED;
	}

	//この順位のTraverserが有効かどうかを調べる。
	constexpr bool IsActive() const { return (m_state == ENABLED) || (m_state == PSEUDOTOP); }

	constexpr bool IsPseudoTop() const { return m_state == PSEUDOTOP; }
	constexpr bool IsIgnored() const { return m_state == IGNORED; }
	constexpr bool IsEnabled() const { return m_state == ENABLED; }
	constexpr bool IsDisabled() const { return m_state == DISABLED; }
	constexpr bool IsFixed() const { return m_state == FIXED; }

	constexpr void Ignore()
	{
		//既にIgnored状態のものを再度Ignoreするのはおかしい。エラーとする。
		assert(!IsIgnored());
		//Ignoreするよりも前に有効化されるのもおかしい。
		assert(IsDisabled());
		assert(!IsFixed());
		m_state = IGNORED;
		m_fixed_layer = m_ujoint_layer;
		m_trav_layer = m_ljoint_layer;
	}
	constexpr void Enable()
	{
		assert(!IsIgnored());
		assert(IsDisabled());
		m_state = ENABLED;
	}
	/*constexpr void Disable()
	{
		//IGNOREDをさらにDisableする状況があるはずがない。
		assert(!IsIgnored());
		//PseudoTopをさらにDisableするはずもない。
		assert(!IsPseudoTop());
		assert(!(mFixedLayer == -1 && mFixedLayer < mUpperJLayer));
		//分岐状態をDisableしてはいけない。
		assert(mTravLayer <= mLowerJLayer);
		mState = DISABLED;
	}*/
	//constexpr bool IsEmpty() const { return mState == DISABLED && mFixedLayer == mUpperJLayer && mTravLayer == -1; }

	//分岐状態か否かを判定する。
	//分岐状態のとき、本順位よりも下位のTraverserは無視され、
	//本Traverserは下位連結層より下層まで走査される。
	//ただし、最下位のContainerだけは常にm_ljoint_layerが-1であるため、これを無視する。
	constexpr bool IsBranching() const { return m_ljoint_layer != -1 && m_trav_layer > m_ljoint_layer; }

	constexpr State GetState() const { return m_state; }

	constexpr void SetTravLayer(LayerType layer)
	{
		assert(IsActive());
		m_trav_layer = layer;
	}

	constexpr LayerType GetDiff() const
	{
		if (!IsActive()) return 0;
		if (IsPseudoTop()) return m_trav_layer;
		LayerType diff = m_trav_layer - m_ujoint_layer;
		return diff;
	}

	//階層関数の上昇値を与えることで、走査層を上昇させる。
	//戻り値はこの順位だけでは足りなかった上昇分。上位Traverserの上昇に持ち越される。
	//もし-1が返るならこの順位は無効である。
	//-2が返る時、PseudoTopであるにも関わらず上昇が不足している。
	constexpr LayerType Raise(LayerType up)
	{
		assert(!IsIgnored());
		if (!IsActive()) return -1;
		LayerType diff = m_trav_layer - m_fixed_layer;
		if (diff < up)
		{
			//固定層まで上昇させてなお上昇分が不足するのなら、
			//このTraverserは無効にする。
			//ただしpseudo topの場合に上昇分が不足することはありえない。
			assert(!IsPseudoTop());
			m_state = DISABLED;
			m_trav_layer = m_fixed_layer;
			return up - diff;
		}
		else
		{
			//このTraverserの上昇で上昇値を消費しきることができるので、
			//不足なしとして0を返す。
			m_trav_layer -= up;
			return 0;
		}
	}

	//OVERTOP、IGNORED状態のLayerInfoも最終的には正規連結での階層へと換算する必要がある。
	//NormalizeはPSEUDOTOPまたはIGNORED状態から正規連結状態に修正する。
	//分岐状態の場合、Normalizeしてはいけない。falseが返る。
	constexpr void Normalize()
	{
		if (IsPseudoTop())
		{
			m_fixed_layer = m_ujoint_layer;
			m_trav_layer = m_ujoint_layer;
			m_state = ENABLED;
		}
		else if (IsIgnored())
		{
			m_state = ENABLED;
		}
		else
		{
			assert(!IsBranching());
		}
	}

	//固定層を変更する。階層関数などで固定層の変更が必要な場合に使う。
	//引数は走査層と固定層との差。もしこの順位で足りない場合は上位へと持ち越す。
	//-2が返る場合はエラー。
	//-1のときはこの順位は無効になっている。
	//0が返る場合は過不足なく固定が完了した。
	constexpr LayerType Fix(LayerType layer)
	{
		assert(!IsIgnored());
		if (!IsActive()) return -1;
		//fixする時点では、固定層はPseudoTopなら0に、そうでないなら上位連結層に等しいはず。
		assert(m_fixed_layer == ((IsPseudoTop() ? -1 : m_ujoint_layer)));
		LayerType diff = m_trav_layer - m_fixed_layer;
		if (diff < layer)
		{
			//PseudoTopの場合、この階層で上昇分を使い切らなければならない。
			//つまり、diff < layerで上昇分に不足する時点でエラーである。
			assert(!IsPseudoTop());
			m_fixed_layer = m_ujoint_layer;
			return layer - diff;
		}
		else
		{
			m_fixed_layer = m_trav_layer - layer;
			return 0;
		}
	}
	//こちらはこの順位のTraverser全体をFixする。つまり一切動かせなくする。
	constexpr void Fix()
	{
		//IGNOREDをさらにFixする状況があるはずがない。
		assert(!IsIgnored());
		//PseudoTopをさらにFixするはずもない。
		assert(!IsPseudoTop());
		assert(!(m_fixed_layer == -1 && m_fixed_layer < m_ujoint_layer));
		//分岐状態をFixしてはいけない。
		assert(!IsBranching());
		m_state = FIXED;
	}

	//この順位をPseudoTopとする。
	constexpr void SetPseudoTop()
	{
		m_state = PSEUDOTOP;
		m_fixed_layer = -1;
		m_trav_layer = 0;
	}

private:
	LayerType m_ujoint_layer;//上位との連結層。
	LayerType m_ljoint_layer;//下位との連結層。mUpperJLayer==mLowerJLayerの場合、このTraverserは走査されない。
	LayerType m_fixed_layer;//固定層。通常はmUpperJLayerに等しいが、階層関数でFixされている場合に値が変化する。m_ujoint_layerよりも上のとき、PseudoTopである。
	LayerType m_trav_layer;//走査層。走査するのは必ずしもmLowerJLayerである必要はない。場合によってはmLowerより下になる。
	State m_state;
};

template <RankType MaxRank>
struct JointLayerArray : public std::array<std::pair<LayerType, LayerType>, MaxRank + 1>
{
	using Base = std::array<std::pair<LayerType, LayerType>, MaxRank + 1>;
private:
	template <size_t N, class ...Ints>
	constexpr void Construct(LayerType u, LayerType l, Ints ...is)
	{
		(*this)[N] = { u, l };
		if constexpr (N < MaxRank) Construct<N + 1>(is...);
	}
public:
	template <class ...Ints>
		requires (std::convertible_to<Ints, LayerType> && ...) && (sizeof...(Ints) == (MaxRank + 1) * 2)
	constexpr JointLayerArray(Ints ...is)
	{
		Construct<0>((LayerType)is...);
	}
	constexpr JointLayerArray()
	{
		for (auto& [u, l] : *this) u = l = -1;
	}

	constexpr bool IsNotInitialized() const
	{
		bool res = true;
		for (auto [u, l] : *this)
			res = (res && u == -1 && l == -1);
		return res;
	}
};

template <RankType MaxRank>
class LayerInfo
{
public:

	//LayerInfoはconstexprとして処理する時があり、
	//そのときの初期化用配列を非型テンプレートで保持する場合がある。
	//std::arrayはprivateメンバを持ち非型テンプレートに渡すことが出来ないため、
	//ELIInitArray型として受け取る。
	constexpr LayerInfo() = default;
	constexpr LayerInfo(JointLayerArray<MaxRank> j)
	{
		Init(j);
	}
	constexpr LayerInfo(const LayerInfo&) = default;
	constexpr LayerInfo& operator=(const LayerInfo&) = default;

	constexpr void Init(JointLayerArray<MaxRank> j)
	{
		for (RankType r = 0; r <= MaxRank; ++r)
			m_internals[r] = j[r];
	}

	//この関数はデフォルトコンストラクタが呼ばれたままInitされていない状態を示す。
	//つまり、upper、lower joint layerなどが全て-1となっている。
	constexpr bool IsNotInitialized() const
	{
		bool res = true;
		for (RankType r = 0; r <= MaxRank; ++r)
			res = (res && m_internals[r].IsNotInitialied());
		return res;
	}

	//順位Rまでを有効化する。
	//各順位はデフォルトで無効となっているので、SetTravLayerの前にこれを呼んでおく必要がある。
	//もしPseudoTop化したい場合は、Enableを呼ぶ前にSetPseudoTopを呼んでおくこと。
	template <RankType R>
	constexpr void Enable()
	{
		for (RankType r = 0; r <= R; ++r)
		{
			auto& i = m_internals[r];
			//もしIgnoreされているのなら有効にしてはいけない。
			//ただし、PseudoTop以下はきちんと有効化しなければならない。
			if (i.IsIgnored())
			{
				//万一最下位がIgnoredだったりする場合は明らかにエラーである。
				assert(r != R);
				continue;
			}
			i.Enable();
		}
	}
	//走査したい順位と、そのInternalTraverser内の連結前の階層を与える。
	//予めEnableを呼んで有効化しておくこと。
	template <RankType RankAct>
	constexpr void SetTravLayer(LayerType intlayer)
	{
		for (RankType r = 0; r < RankAct; ++r)
		{
			auto& i = m_internals[r];
			if (i.IsIgnored()) continue;
			i.SetTravLayer(i.GetLJointLayer());
		}
		auto& i = m_internals[RankAct];
		i.SetTravLayer(intlayer);
	}

	//走査する連結後の階層を与える。
	//このとき、例えばlayerがある順位のupper_joint_layerに相当する場合、
	//階層関数などでは無効の場合もあるが、この関数では無条件に有効化する。
	constexpr void EnableAndSetTravLayer(LayerType extlayer)
	{
		LayerType exttop = -1;
		for (RankType r = 0; r < MaxRank; ++r)
		{
			auto& i = m_internals[r];
			//ignoredのときこれを呼んではならない。
			if (i.IsIgnored()) throw Forbidden("");
			assert(i.IsDisabled());
			LayerType diff = i.GetLJointLayer() - i.GetUJointLayer();
			if (exttop + diff <= extlayer)
			{
				i.Enable();
				i.SetTravLayer(i.GetLJointLayer());
				exttop += diff;
			}
			else
			{
				i.Enable();
				LayerType intlayer = extlayer - exttop + i.GetUJointLayer();
				i.SetTravLayer(intlayer);
				return;
			}
		}
		//最下位まで回ってきてしまった場合、例外的にループ外で処理する。
		//ljoint_layerが-1であったり、ここでextlayerを消費し切る必要があったりと処理が異なるので。
		auto& i = m_internals[MaxRank];
		i.Enable();
		LayerType intlayer = extlayer - exttop + i.GetUJointLayer();
		i.SetTravLayer(intlayer);
	}


	//連結後の走査階層を返す。
	//もしPseudoTopな順位がある場合-2を返す。
	//もし分岐している場合は-3を返す。
	constexpr LayerType GetTravLayer() const
	{
		bool active = false;
		LayerType res = -1;
		for (RankType r = 0; r <= MaxRank; ++r)
		{
			auto& i = m_internals[r];
			if (i.IsIgnored()) return -2_layer;
			if (i.IsBranching()) return -3_layer;
			if (i.IsDisabled())
			{
				//無効な階層が見つかったときの振る舞いは2通り。
				//もし有効な順位が見つかっていない場合は、探索を継続する。
				//既に見つかっている場合、ひとつ上の順位に走査層があり、resにその値が収まっている。
				//よってresを返せばよい。
				if (active) return res;
				continue;
			}
			//PseudoTopの場合、必要なのはUpperJLayerではなくFixedLayerとの差である。
			assert(!i.IsPseudoTop());// res += (i.GetTravLayer() - i.GetFixedLayer());
			
			res += i.GetDiff();
			active = true;
		}
		return res;
	}

	//有効な最下位Traverserの順位を返す。
	//全て無効の場合、-1が返る。
	constexpr RankType GetActiveRank() const
	{
		for (RankType r = MaxRank; r >= 0; --r)
		{
			if (m_internals[r].IsActive()) return r;
		}
		return RankType(-1);
	}
	constexpr RankType GetFixedRank() const
	{
		for (RankType r = MaxRank; r >= 0; --r)
		{
			if (m_internals[r].IsFixed()) return r;
		}
		return RankType(-1);
	}

	constexpr void Raise(LayerType up)
	{
		for (RankType r = MaxRank; r >= 0; --r)
		{
			auto& i = m_internals[r];
			//ignoredにも関わらず上昇分が残っていてはいけない。後述の-2判定があれば十分なはずだが、一応。
			assert(!(i.IsIgnored() && up != 0));
			LayerType l = i.Raise(up);
			//戻り値が0の場合、全ての上昇分を使い切ったので、完了とする。
			if (l == 0) return;
			//戻り値が-1の場合、その順位は無効になっている。
			if (l != -1) up = l;
		}
	}

	//階層関数などでend判定をするために固定層を修正する。
	//引数には現在の走査層と固定する層との差を与える。
	constexpr void Fix(LayerType diff)
	{
		assert(diff >= -1);
		LayerType res = -1;//直前の結果。固定層までの残りを意味する。
		for (RankType r = MaxRank; r >= 0; --r)
		{
			auto& i = m_internals[r];
			if (res == -1)
			{
				//-1のとき、これよりも下の順位は全て無効だったということである。
				//ということは引数diffはまだ一切消費していないので、そのままdiffでFixする。
				res = i.Fix(diff);
			}
			else if (res == 0)
			{
				//res==0なら、これよりも下の順位までで固定が十分だったということである。
				//よってこの順位は無効とする。ただしIgnoredの場合はそのままにする。
				if (!i.IsIgnored()) i.Fix();
			}
			else
			{
				//有効値が入っているのなら、それは固定層までの残り。
				//xを更新する。
				res = i.Fix(res);
			}
		}
		assert(res == 0);
	}

	//順位R-1以上をIgnored状態にし、R位をPseudoTopとする。
	template <RankType R>
	constexpr void SetPseudoTop()
	{
		for (RankType r = 0; r < R; ++r)
		{
			m_internals[r].Ignore();
		}
		m_internals[R].SetPseudoTop();
	}

	//PseudoTop状態から正規化する。もともと正規連結状態なら何もしない。
	//分岐している場合に正規化することはできない。その場合falseが返る。
	constexpr void Normalize()
	{
		for (RankType r = 0; r <= MaxRank; ++r)
		{
			auto& i = m_internals[r];
			assert(!i.IsBranching());
			i.Normalize();
		}
	}

	constexpr bool IsBranching()
	{
		for (RankType r = 0; r <= MaxRank; ++r)
		{
			if (m_internals[r].IsBranching()) return true;
		}
		return false;
	}

	//自身と引数がどちらも正規連結状態であることを事前にチェックしておくこと。
	//正規連結での階層が一致したとしても、深さが同一であるとは限らない。
	//例えば、Rank1のLowerJLayerまでを走査している場合と、Rank2のUpperJLayerまでの場合。
	//双方の階層は数字上は同じだが、前者はRank2はDisabledであり、後者はEnabledである。
	//したがって、後者のほうが深い。
	constexpr bool operator<(const LayerInfo& e) const
	{
		//assert(e.IsEnabled() && IsEnabled());
		for (RankType r = MaxRank; r >= 0; --r)
		{
			auto& ithis = m_internals[r];
			auto& ithat = e.m_internals[r];
			//どちらもdisabledなら上位へ。
			if (!ithis.IsEnabled() && !ithat.IsEnabled()) continue;
			//一方のみがenabledなら、そちらのほうが大きい。
			if (ithis.IsEnabled() != ithat.IsEnabled()) return ithat.IsEnabled();
			//両方がenabledなら、trav layerを比較する。
			//ここが最も下位のenabledなので、上位を比較する必要はない。
			return ithis.GetTravLayer() < ithat.GetTravLayer();
		}
		return false;
	}
	constexpr bool operator>(const LayerInfo& e) const
	{
		//assert(e.IsEnabled() && IsEnabled());
		for (RankType r = MaxRank; r >= 0; --r)
		{
			auto& ithis = m_internals[r];
			auto& ithat = e.m_internals[r];
			//どちらもdisabledなら上位へ。
			if (!ithis.IsEnabled() && !ithat.IsEnabled()) continue;
			//一方のみがenabledなら、そちらのほうが大きい。
			if (ithis.IsEnabled() != ithat.IsEnabled()) return ithis.IsEnabled();
			//両方がenabledなら、trav layerを比較する。
			//ここが最も下位のenabledなので、上位を比較する必要はない。
			return ithis.GetTravLayer() > ithat.GetTravLayer();
		}
		return false;
	}
	constexpr bool operator<=(const LayerInfo& e) const
	{
		return !(*this > e);
	}
	constexpr bool operator>=(const LayerInfo& e) const
	{
		return !(*this < e);
	}
	constexpr bool operator==(const LayerInfo& e) const
	{
		return m_internals == e.m_internals;
	}
	constexpr bool operator!=(const LayerInfo& e) const
	{
		return !(*this == e);
	}
	constexpr bool operator==(const JointLayerArray<MaxRank>& a) const
	{
		bool res = true;
		for (auto[i, j] : views::Zip(m_internals, a))
		{
			res = res && (i.GetUJointLayer() == j.first);
			res = res && (i.GetLJointLayer() == j.second);
		}
		return res;
	}
	constexpr bool operator!=(const JointLayerArray<MaxRank>& a) const
	{
		return !(*this == a);
	}

	//連結後階層から結合前階層へ変換する。
	template <RankType Rank>
	constexpr LayerType ConvertExtToInt(LayerType extlayer) const
	{
		auto& i = m_internals[Rank];
		return extlayer - i.GetExtUJointLayer() + i.GetUJointLayer();
	}
	//結合前階層から連結後階層へ変換する。
	template <RankType Rank>
	constexpr LayerType ConvertIntToExt(LayerType intlayer) const
	{
		auto& i = m_internals[Rank];
		return intlayer - i.GetUJointLayer() + i.GetExtUJointLayer();
	}

	constexpr const InternalLayerInfo& GetInternal(RankType r) const { return m_internals[r]; }

private:

	std::array<InternalLayerInfo, MaxRank + 1> m_internals;
};

}

#endif