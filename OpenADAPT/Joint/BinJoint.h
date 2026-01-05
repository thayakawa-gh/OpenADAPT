#ifndef ADAPT_JOINT_BINJOINT_H
#define ADAPT_JOINT_BINJOINT_H

#include <map>
#include <memory>
#include <concepts>
#include <OpenADAPT/Common/Concepts.h>
#include <OpenADAPT/Evaluator/Function.h>
#include <OpenADAPT/Joint/JointInterface.h>

namespace adapt
{

template <RankType Rank, any_hist Hist, class Container, class Nodes>
struct BinJoint;
template <RankType Rank, any_hist Hist, class Container, node_or_placeholder ...Nodes>
struct BinJoint<Rank, Hist, Container, TypeList<Nodes...>>
	: public detail::JointInterface_impl<Container>
{
	using Traverser = Container::Traverser;
	using ConstTraverser = Container::ConstTraverser;

private:
	using JointInterface = detail::JointInterface_impl<Container>;
public:

	BinJoint() = default;
	template <any_hist Hist_, node_or_placeholder ...Nodes_>
	BinJoint(const Hist_& hist, Nodes_&& ...indices)
		: m_hist(&hist), m_bin_indices(std::forward<Nodes_>(indices)...)
	{}
	template <any_hist Hist_, node_or_placeholder ...Nodes_>
	BinJoint(const Hist_& hist, const std::tuple<Nodes_...>& indices)
		: m_hist(&hist), m_bin_indices(indices)
	{}

	virtual ~BinJoint() = default;

	template <node_or_placeholder ...NPs>
	void SetBinIndices(NPs&& ...indices)
	{
		m_bin_indices = std::make_tuple(eval::detail::ConvertToNode(std::forward<NPs>(indices))...);
	}

private:
	template <size_t I, class ...Args>
	void Init_impl([[maybe_unused]] const Args& ...args)
	{
		if constexpr (any_node<GetType_t<I, Nodes...>>)
			std::get<I>(m_bin_indices).Init(args...);
		if constexpr (I < sizeof...(Nodes) - 1)
			Init_impl<I + 1>(args...);
	}
public:
	virtual void Init(const Traverser& t, const std::vector<std::tuple<const void*, const Bpos*, bool>>& outer_t) override { Init_impl<0>(t, outer_t); }
	virtual void Init(const ConstTraverser& t, const std::vector<std::tuple<const void*, const Bpos*, bool>>& outer_t) override { Init_impl<0>(t, outer_t); }
	virtual void Init(const Container& s, const Bpos& bpos) override { Init_impl<0>(s, bpos); }
	virtual void Init() override { Init_impl<0>(); }
private:
	template <size_t I, class ...Args>
	BinBaseType Eval(const Args& ...args) const
	{
		auto& index = std::get<I>(m_bin_indices);
		if constexpr (stat_type_node_or_placeholder<decltype(index)>)
			return (BinBaseType)index.Evaluate(args...);
		else
		{
			using enum FieldType;
			switch (index.GetType())
			{
			case I08: return (BinBaseType)index.Evaluate(args..., Number<I08>{});
			case I16: return (BinBaseType)index.Evaluate(args..., Number<I16>{});
			case I32: return (BinBaseType)index.Evaluate(args..., Number<I32>{});
			case I64: return (BinBaseType)index.Evaluate(args..., Number<I64>{});
			default: throw MismatchType("In BinJoint, all keys must be integer type or integer node.");
			}
		}
	}
	template <class ...Args> requires (sizeof...(Args) > 1)
		static decltype(auto) MakeKey(Args&& ...args) { return std::tuple<Args...>(std::forward<Args>(args)...); }
	template <class Arg>
	static decltype(auto) MakeKey(Arg&& arg) { return std::forward<Arg>(arg); }
	template <size_t ...Indices, class ...Args>
	bool Find_impl(Bpos& res, std::index_sequence<Indices...>, const Args& ...args) const
	{
		auto bin = Bin<sizeof...(Indices)>{ Eval<Indices>(args...)... };
		if (!m_hist->IsInside(bin)) return false;
		BindexType row = m_hist->BinToIndex(bin);
		res.Assign(row);
		return true;
	}
public:

	virtual bool Find(const Traverser& t, Bpos& bpos) const override { return Find_impl(bpos, std::make_index_sequence<sizeof...(Nodes)>{}, t); }
	virtual bool Find(const ConstTraverser& t, Bpos& bpos) const override { return Find_impl(bpos, std::make_index_sequence<sizeof...(Nodes)>{}, t); }
	virtual bool Find(const Container& s, const Bpos& bpos, Bpos& res) const override { return Find_impl(res, std::make_index_sequence<sizeof...(Nodes)>{}, s, bpos); }

	virtual std::unique_ptr<JointInterface> Clone() const override
	{
		return std::unique_ptr<JointInterface>(new BinJoint(*m_hist, m_bin_indices));
	}

private:
	const Hist* m_hist = nullptr;
	std::tuple<Nodes...> m_bin_indices;
};


namespace detail
{

template <RankType Rank, class Container, any_hist Hist, node_or_placeholder ...Keys>
auto MakeBinJoint(const Hist& hist, Keys&& ...keys)
{
	if (!(keys.IsInt() && ...))
		throw MismatchType("In BinJoint, all keys must be integer type.");
	using Joint = BinJoint<Rank, std::remove_cvref_t<Hist>, Container, TypeList<std::remove_cvref_t<Keys>...>>;
	std::unique_ptr<JointInterface_impl<Container>> j =
		std::make_unique<Joint>(hist, std::forward<Keys>(keys)...);
	return std::move(j);
}

}

}

#endif