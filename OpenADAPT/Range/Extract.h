#ifndef ADAPT_RANGE_EXTRACT_H
#define ADAPT_RANGE_EXTRACT_H

#include <OpenADAPT/Utility/StaticString.h>
#include <OpenADAPT/Evaluator/Placeholder.h>
#include <OpenADAPT/Evaluator/ConstNode.h>
#include <OpenADAPT/Evaluator/FieldNode.h>
#include <OpenADAPT/Evaluator/FuncNode.h>
#include <OpenADAPT/Evaluator/LayerFuncNode.h>
#include <OpenADAPT/Container/Tree.h>
#include <OpenADAPT/Traverser/ExternalTraverser.h>
#include <OpenADAPT/Range/Filter.h>
#include <OpenADAPT/Range/RangeAdapter.h>

namespace adapt
{

namespace detail
{

template <class Range>
class CttiExtractor
{
	template <LayerType Layer, class ...Cmp>
	static consteval auto GetStructure(LayerConstant<Layer>, TypeList<Cmp...>, TypeList<>)
	{
		return NamedTuple<Cmp...>{};
	}
	template <LayerType Layer, StaticChar Name, class NP, class ...Cmp, class ...NPs_>
	static consteval auto GetStructure(LayerConstant<Layer> layer, TypeList<Cmp...>, TypeList<std::tuple<StaticString<Name>, NP>, NPs_...>)
	{
		using DecayedNP = std::decay_t<NP>;
		if constexpr (DecayedNP::GetLayer() == Layer)
			return GetStructure(layer, TypeList<Cmp..., Named<Name, std::decay_t<typename DecayedNP::RetType>>>{}, TypeList<NPs_...>{});
		else
			return GetStructure(layer, TypeList<Cmp...>{}, TypeList<NPs_...>{});
	}
	template <class, class>
	struct Structure_impl;
	template <class ...NamedNPs, LayerType ...Layers>
	struct Structure_impl<TypeList<NamedNPs...>, std::integer_sequence<LayerType, Layers...>>
	{
		using Type = STree<decltype(GetStructure(LayerConstant<LayerType(Layers - 1)>{}, TypeList<>{}, TypeList<NamedNPs...>{}))...>;
	};

	template <class ...Names, class ...NPs>
	static consteval LayerType GetLayer(TypeList<std::tuple<Names, NPs>...>)
	{
		return std::max({ std::decay_t<NPs>::GetLayer()... });
	}

	template <class ...NPs>
	struct RetType
	{
		static constexpr LayerType MaxLayer = GetLayer(TypeList<NPs...>{});
		using Type = Structure_impl<TypeList<NPs...>, std::make_integer_sequence<LayerType, LayerType(MaxLayer + 2)>>::Type;
	};

	template <LayerType Layer, class STree_, class Trav, class ElementRef>
	void Exec_eval(const Trav&, ElementRef&) {}
	template <LayerType Layer, class STree_, class Trav, class ElementRef, class NamedNP, class ...NamedNPs_>
	void Exec_eval(const Trav& t, ElementRef& ref, NamedNP&& np, NamedNPs_&& ...nps)
	{
		using Decayed = std::decay_t<NamedNP>;
		using NP = std::tuple_element_t<1, Decayed>;
		if constexpr (NP::GetLayer() != Layer)
			//関係ない層のノードも混じっているので、その場合はスキップ。
			Exec_eval<Layer, STree_>(t, ref, std::forward<NamedNPs_>(nps)...);
		else
		{
			using StatName = std::tuple_element_t<0, Decayed>;
			static constexpr auto ph = STree_::GetPlaceholder(StatName{});
			ref.GetField(ph) = std::get<1>(np).Evaluate(t);
			Exec_eval<Layer, STree_>(t, ref, std::forward<NamedNPs_>(nps)...);
		}
	}
	template <LayerType Layer, LayerType MaxLayer, class STree_, class Traverser, class ElementBlock, class ElementRef, class ...NamedNPs_>
	void Exec_rec(STree_& res, Traverser& t, ElementRef upref, std::vector<ElementBlock>& bufs, NamedNPs_&& ...nps)
	{
		using ElementListRef = ElementListRef_impl<typename STree_::Hierarchy, std::type_identity_t, LayerConstant<Layer>>;
		using enum FieldType;
		static_assert(std::same_as<ElementBlock, typename ElementListRef::ElementBlock>);
		ElementListRef buf(&res, LayerConstant<Layer>{}, &bufs[Layer]);
		bool pushflag = true;

		for (bool end = true; end; end = t.MoveForward(Layer))
		{
			if (pushflag) buf.PushDefaultElement();
			pushflag = true;
			try
			{
				auto elmref = buf.Back();
				//ここ、CreateValueを個々に呼ぶことは難しいかもしれない。
				//万一SkipOverが投げられた場合、どこまで構築されたかを確認していちいちDestroyしなければならなくなる。
				//デフォルトコンストラクタを先に呼んでおくのとどちらがいいのか。
				Exec_eval<Layer, STree_>(t, elmref, std::forward<NamedNPs_>(nps)...);

				if constexpr (Layer < MaxLayer)
					Exec_rec<Layer + 1_layer, MaxLayer>(res, t, elmref, bufs, std::forward<NamedNPs_>(nps)...);
			}
			catch (JointError)
			{
				//Jointに失敗した場合などに投げられることがある。
				//既にPush済みなので、
				pushflag = false;
			}
			catch (NoElements)
			{
				pushflag = false;
			}
		}
		auto list = upref.GetLowerElements();
		list.CreateElementsFrom(std::move(buf));
	}

	template <ctti_node Node, class Trav> int Init(Node&& node, const Trav& t) { node.Init(t); return 0; }
	template <ctti_placeholder Ph, class Trav> int Init(Ph&&, const Trav&) { return 0; }
	template <class STree_, class ElementBlock, LayerType ...Layers>
	void DestructBufs(const STree_& res, std::vector<ElementBlock>& bufs, std::integer_sequence<LayerType, Layers...>)
	{
		DoNothing((bufs[Layers].Destruct(&res, LayerConstant<Layers>{}), 0)...);
	}
public:
	template <traversal_range Range_, class ...NamedNPs_>
	auto Exec(Range_&& range, NamedNPs_&& ...nps)
	{
		using RetType_ = RetType<std::decay_t<NamedNPs_>...>;
		using STree_ = RetType_::Type;
		constexpr LayerType maxlayer = RetType_::MaxLayer;
		static_assert(maxlayer >= 0_layer);

		STree_ res;
		std::vector<detail::ElementBlock_impl<typename STree_::Hierarchy, SElementBlockPolicy>> bufs(maxlayer + 1_layer);
		range.SetTravLayer(maxlayer);
		auto t = std::forward<Range_>(range).begin();
		DoNothing(Init(std::get<1>(std::forward<NamedNPs_>(nps)), t)...);
		auto upref = res.GetTopElement();
		Exec_rec<0, maxlayer>(res, t, upref, bufs, std::forward<NamedNPs_>(nps)...);
		DestructBufs(res, bufs, std::make_integer_sequence<LayerType, maxlayer + 1>{});
		return res;
	}
};

template <class Range>
class RttiExtractor
{
	using ElementBlock = detail::ElementBlock_impl<DHierarchy<DTree>, DElementBlockPolicy>;
	using Container = std::remove_cvref_t<Range>::Container;

	template <size_t N>
	void Arrange(std::vector<std::vector<std::pair<std::string, FieldType>>>& structure,
				 std::vector<std::vector<eval::RttiFuncNode<Container>>>& nodes) const
	{}
	template <size_t N, class Var, class ...Vars>
	void Arrange(std::vector<std::vector<std::pair<std::string, FieldType>>>& structure,
				 std::vector<std::vector<eval::RttiFuncNode<Container>>>& nodes, 
				 Var&& var, Vars&& ...vars)
	{
		auto&& [name, v] = std::forward<Var>(var);
		LayerType layer = v.GetLayer();
		FieldType type = v.GetType();
		while ((LayerType)structure.size() - 1 <= layer)
		{
			structure.emplace_back();
			nodes.emplace_back();
		}
		structure[layer + 1].emplace_back(name, type);
		nodes[layer + 1].emplace_back(eval::ConvertToRttiFuncNode(std::forward<decltype(v)>(v)));
		Arrange<N + 1>(structure, nodes, std::forward<Vars>(vars)...);
	}

	template <named_node_or_placeholder ...Vars>
	void Init(std::vector<std::vector<std::pair<std::string, FieldType>>>& structure,
			  std::vector<ElementBlock>& bufs,
			  std::vector<std::vector<eval::RttiFuncNode<Container>>>& nodes,
			  Vars&& ...vars)
	{
		Arrange<0>(structure, nodes, std::forward<Vars>(vars)...);
		bufs.resize(structure.size() - 1);
	}

	using ElementListRef = ElementListRef_impl<typename DTree::Hierarchy, std::type_identity_t, LayerType>;

	template <class Traverser>
	void Exec_rec(DTree& res, Traverser& t, LayerType layer, LayerType maxlayer, DTree::ElementRef upref,
				  std::vector<ElementBlock>& bufs,
				  std::vector<std::vector<eval::RttiFuncNode<Container>>>& nodes)
	{
		using enum FieldType;
		ElementListRef buf{ &res, layer, & bufs[layer] };
		const std::vector<eval::RttiFuncNode<Container>>& lnodes = nodes[layer + 1];
		const std::vector<DTree::RttiPlaceholder>& phs = res.GetPlaceholdersIn(layer);
		bool pushflag = true;
		for (bool end = true; end; end = t.MoveForward(layer))
		{
			if (pushflag) buf.PushDefaultElement();
			pushflag = true;
			//size_t ifield = 0;
			try
			{
				auto elmref = buf.Back();
				//ここ、CreateValueを個々に呼ぶことは難しいかもしれない。
				//万一SkipOverが投げられた場合、どこまで構築されたかを確認していちいちDestroyしなければならなくなる。
				//デフォルトコンストラクタを先に呼んでおくのとどちらがいいのか。
				for (auto [node, ph] : views::Zip(lnodes, phs))
				{
					auto field = elmref.GetField(ph);
					switch (ph.GetType())
					{
					case I08: field.template as_unsafe<I08>() = node.Evaluate(t, Number<I08>{}); break;
					case I16: field.template as_unsafe<I16>() = node.Evaluate(t, Number<I16>{}); break;
					case I32: field.template as_unsafe<I32>() = node.Evaluate(t, Number<I32>{}); break;
					case I64: field.template as_unsafe<I64>() = node.Evaluate(t, Number<I64>{}); break;
					case F32: field.template as_unsafe<F32>() = node.Evaluate(t, Number<F32>{}); break;
					case F64: field.template as_unsafe<F64>() = node.Evaluate(t, Number<F64>{}); break;
					case C32: field.template as_unsafe<C32>() = node.Evaluate(t, Number<C32>{}); break;
					case C64: field.template as_unsafe<C64>() = node.Evaluate(t, Number<C64>{}); break;
					case Str: field.template as_unsafe<Str>() = node.Evaluate(t, Number<Str>{}); break;
					case Jbp: field.template as_unsafe<Jbp>() = node.Evaluate(t, Number<Jbp>{}); break;
					default: throw MismatchType("");
					}
					//++ifield;
				}

				if (layer < maxlayer) Exec_rec(res, t, layer + 1, maxlayer, elmref, bufs, nodes);
			}
			catch (JointError)
			{
				//Jointに失敗した場合などに投げられることがある。
				//既にPush済みなので、
				pushflag = false;
			}
			catch (NoElements)
			{
				pushflag = false;
			}
		}
		auto list = upref.GetLowerElements();
		list.CreateElementsFrom(std::move(buf));
	}
public:

	template <traversal_range Range_, named_node_or_placeholder ...NPs_>
	DTree Exec(Range_&& range, NPs_&& ...nps)
	{
		std::vector<std::vector<std::pair<std::string, FieldType>>> structure;//m_structure[i]はi-1層のレイヤー構造。
		std::vector<ElementBlock> bufs;//m_bufs[layer]にlayer層用のバッファがある。
		//std::vector<std::vector<ElementBlock>> elements;//m_elements[layer]にはlayer層のElementBlockが一時保管されている。
		std::vector<std::vector<eval::RttiFuncNode<Container>>> nodes;//レイヤーごとに分けられたノード。m_nodes[layer + 1]がlayer層のノード。

		Init(structure, bufs, nodes, std::forward<NPs_>(nps)...);
		DTree res;
		if (!structure[0].empty()) res.SetTopLayer(structure[0]);
		for (auto it = structure.begin() + 1; it != structure.end(); ++it) res.AddLayer(*it);
		res.VerifyStructure();

		LayerType maxlayer = (LayerType)structure.size() - 2;//m_structureは-1, 0, 1, ...と順に階層情報を格納しているので、-2が正しい。
		if (maxlayer < 0_layer)
			throw InvalidLayer("Maximum layer of the arguments for Extract must be equal or larger than 0.");

		range.SetTravLayer(maxlayer);
		auto t = std::forward<Range_>(range).begin();
		for (auto& n : nodes) for (auto& nn : n) nn.Init(t);
		auto ref = res.GetTopElement();
		Exec_rec(res, t, 0, maxlayer, ref, bufs, nodes);
		for (auto[layer, b] : views::Enumerate(bufs)) b.Destruct(&res, (LayerType)layer);
		return res;
	}
};

template <size_t I, named_node_or_placeholder NP>
NP&& AddDefaultName(NP&& np) { return std::forward<NP>(np); }
template <size_t I, node_or_placeholder NP, StaticString Name = StaticString<"fld">{} + ToStr(Number<I>{}) >
auto AddDefaultName(NP&& np)
{
	return std::forward<NP>(np).named(Name);
}

template <class ...Vars>
auto Extract_impl2(Vars&& ...named_vars)
{
	constexpr bool all_stat =
		(s_named_node_or_placeholder<Vars> && ...) &&
		(ctti_node_or_placeholder<std::tuple_element_t<1, std::decay_t<Vars>>> && ...);
	if constexpr (all_stat)
		return RangeConsumer<CttiExtractor, Vars...>(std::forward<Vars>(named_vars)...);
	else
		return RangeConsumer<RttiExtractor, Vars...>(std::forward<Vars>(named_vars)...);
}
template <size_t ...Is, class ...Vars>
auto Extract_impl(std::index_sequence<Is...>, Vars&& ...named_vars)
{
	return Extract_impl2(AddDefaultName<Is>(std::forward<Vars>(named_vars))...);
}
}

template <class ...Vars>
	requires ((node_or_placeholder<Vars> || named_node_or_placeholder<Vars>) && ...)
auto Extract(Vars&& ...vars)
{
	return detail::Extract_impl(std::make_index_sequence<sizeof...(Vars)>{}, std::forward<Vars>(vars)...);
}

}

#endif
