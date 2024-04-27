#ifndef ADAPT_INTERNALTRAVERSER_H
#define ADAPT_INTERNALTRAVERSER_H

#include <OpenADAPT/Joint/LayerInfo.h>
#include <OpenADAPT/Joint/JointInterface.h>

namespace adapt
{

namespace detail
{

template <template <class> class Qualifier, class Base>
class InternalTraverser : public Base
{
	//単体のTraverserは固定層と走査層を持つが、
	//その2つはあくまで階層関数などで変化したものであって、
	//上下位との連結層の情報ではない。
	//よって、それらをInternalTraverserに持たせておく。

	using Container = Base::Container;

	//static constexpr bool IsDelayedJointMode = std::is_same_v<JointMode, DelayedJoint>;
public:

	InternalTraverser()
		: m_state(InternalLayerInfo::DISABLED), m_ext_ujoint_layer(-1),
		m_ujoint_layer(-1), m_ljoint_layer(-1), m_is_joined(false)
	{}
	InternalTraverser(const InternalTraverser&) = delete;
	InternalTraverser(InternalTraverser&&) noexcept = default;

	InternalTraverser& operator=(const InternalTraverser&) = delete;
	InternalTraverser& operator=(InternalTraverser&&) noexcept = default;

	void CopyFron(const InternalTraverser& that)
	{
		m_state = that.m_state;
		m_ext_ujoint_layer = that.m_ext_ujoint_layer;
		m_ujoint_layer = that.m_ujoint_layer;
		m_ljoint_layer = that.m_ljoint_layer;
		m_is_joined = that.m_is_joined;
		Base::CopyFrom(that);
	}

	void Init(LayerType ext_upper, const InternalLayerInfo& l, Qualifier<Container>& c)
	{
		Base::Init(l.GetFixedLayer(), c, l.GetTravLayer());
		m_ext_ujoint_layer = ext_upper;
		m_state = l.GetState();
		m_ujoint_layer = l.GetUJointLayer();
		m_ljoint_layer = l.GetLJointLayer();
		m_is_joined = false;
	}

	LayerType GetExtUpperJLayer() const { return m_ext_ujoint_layer; }
	LayerType GetUJointLayer() const { return m_ujoint_layer; }
	LayerType GetLJointLayer() const { return m_ljoint_layer; }

	//連結後階層から結合前階層へ変換する。
	LayerType ConvertExtToInt(LayerType extlayer) const { return extlayer - m_ext_ujoint_layer + m_ujoint_layer; }
	//結合前階層から連結後階層へ変換する。
	LayerType ConvertIntToExt(LayerType intlayer) const { return intlayer - m_ujoint_layer + m_ext_ujoint_layer; }

	//layer指定しない場合、自動的にm_ujoint_layerまで割り当てる。
	template <RankType Rank, direction_flag Flag, joint_mode JointMode>
	bool AssignPartially(const InternalTraverser& i, Flag, JointMode)
	{
		if constexpr (Rank == 0) return Base::AssignPartially(i, Flag{});
		else
		{
			if constexpr (std::is_same_v<JointMode, DelayedJoint>) assert(!IsJoined(JointMode{}));
			return (m_is_joined = Base::AssignPartially(i, Flag{}));
		}
	}
	//layer指定がある場合、そのlayerまで割り当てる。
	template <RankType Rank, direction_flag Flag, joint_mode JointMode>
	bool AssignPartially(const InternalTraverser& i, LayerType layer, Flag, JointMode)
	{
		if constexpr (Rank == 0) return Base::AssignPartially(i, layer, Flag{});
		else
		{
			if constexpr (std::is_same_v<JointMode, DelayedJoint>) assert(!IsJoined(JointMode{}));
			return (m_is_joined = Base::AssignPartially(i, layer, Flag{}));
		}
	}
	//layer指定しない場合、自動的にm_fixed_layerまで割り当てる。
	template <RankType Rank, direction_flag Flag, joint_mode JointMode>
	bool AssignPartially(const Bpos& bpos, Flag, JointMode)
	{
		if constexpr (Rank == 0) return Base::AssignPartially(bpos, Flag{});
		else
		{
			if constexpr (std::is_same_v<JointMode, DelayedJoint>) assert(!IsJoined(JointMode{}));
			return (m_is_joined = Base::AssignPartially(bpos, Flag{}));
		}
	}
	//layer指定がある場合、そのlayerまで割り当てる。
	template <RankType Rank, direction_flag Flag, joint_mode JointMode>
	bool AssignPartially(const Bpos& bpos, LayerType layer, Flag, JointMode)
	{
		if constexpr (Rank == 0) return Base::AssignPartially(bpos, layer, Flag{});
		else
		{
			if constexpr (std::is_same_v<JointMode, DelayedJoint>) assert(!IsJoined(JointMode{}));
			return (m_is_joined = Base::AssignPartially(bpos, layer, Flag{}));
		}
	}

	template <RankType Rank, direction_flag Flag, joint_mode JointMode>
	bool AssignPartiallyByExtBpos(const Bpos& bpos, Flag, JointMode)
	{
		if constexpr (Rank == 0)
			return Base::AssignPartiallyByExtBpos(bpos, m_ext_ujoint_layer + 1, m_ujoint_layer + 1_layer);
		else
		{
			//bposを与える場合は他とは違いjoin済みでなければならない。
			if constexpr (std::is_same_v<JointMode, DelayedJoint>) assert(IsJoined(JointMode{}));
			return Base::AssignPartiallyByExtBpos(bpos, m_ext_ujoint_layer + 1, m_ujoint_layer + 1_layer);
		}
	}

	template <RankType Rank>
	bool AssignRow(BindexType row)
	{
		static_assert(Rank == 0_rank);
		return m_is_joined = Base::AssignRow(row);
	}
	template <RankType Rank, joint_mode JointMode>
	bool Assign(const InternalTraverser& ithat, JointMode)
	{
		if constexpr (Rank == 0) return Base::Assign(ithat);
		else
		{
			if constexpr (std::is_same_v<JointMode, DelayedJoint>) assert(!IsJoined(JointMode{}));
			return (m_is_joined = Base::Assign(ithat));
		}
	}
	template <RankType Rank, joint_mode JointMode>
	bool Assign(const Bpos& bpos, JointMode)
	{
		if constexpr (Rank == 0) return Base::Assign(bpos);
		else
		{
			if constexpr (std::is_same_v<JointMode, DelayedJoint>) assert(!IsJoined(JointMode{}));
			return (m_is_joined = Base::Assign(bpos));
		}
	}

	//通常のAssignに近い動作だが、
	//こちらはExternal Layerで見たbposが与えられている。
	template <RankType Rank, joint_mode JointMode>
	bool AssignByExtBpos(const Bpos& bpos, JointMode)
	{
		assert(IsFixed());
		if constexpr (Rank == 0) return Base::AssignByExtBpos(bpos, m_ext_ujoint_layer + 1);
		else
		{
			//bposを与える場合は他とは違いjoin済みでなければならない。
			if constexpr (std::is_same_v<JointMode, DelayedJoint>) assert(IsJoined(JointMode{}));
			//bposはm_ext_ujoint_layer + 1からを使う。
			return Base::AssignByExtBpos(bpos, m_ext_ujoint_layer + 1);
		}
	}

	void Release(DelayedJoint) { m_is_joined = false; }

	bool IsJoined(DelayedJoint) const { return m_is_joined; }

	bool IsPseudoTop() const { return m_state == InternalLayerInfo::PSEUDOTOP; }
	bool IsIgnored() const { return m_state == InternalLayerInfo::IGNORED; }
	bool IsEnabled() const { return m_state == InternalLayerInfo::ENABLED; }
	bool IsDisabled() const { return m_state == InternalLayerInfo::DISABLED; }
	bool IsFixed() const { return m_state == InternalLayerInfo::FIXED; }

	template <class RankedPlaceholder>
	decltype(auto) operator[](const RankedPlaceholder& ph) const
	{
		return Base::operator[](ph.Derank());
	}
	template <class RankedPlaceholder, std::integral ...Indices>
	decltype(auto) GetField(const RankedPlaceholder& ph, Indices ...is) const
	{
		return Base::GetField(ph.Derank(), is...);
	}


private:
	InternalLayerInfo::State m_state;
	LayerType m_ext_ujoint_layer;//連結後階層で見たm_ujoint_layer。
	LayerType m_ujoint_layer;
	LayerType m_ljoint_layer;
	bool m_is_joined;//PromptJointでは使われない。
};

}

}

#endif