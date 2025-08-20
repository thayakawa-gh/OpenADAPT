#ifndef ADAPT_COMMON_CONCEPTS_H
#define ADAPT_COMMON_CONCEPTS_H

#include <concepts>
#include <unordered_map>
#include <OpenADAPT/Utility/NamedTuple.h>
#include <OpenADAPT/Common/Definition.h>
#include <OpenADAPT/Common/Bpos.h>
#include <OpenADAPT/Common/Common.h>
#ifdef USE_ANKERL_UNORDERED_DENSE
#include <ankerl/unordered_dense.h>
#endif

namespace adapt
{

template <class T>
concept direction_flag = std::is_same_v<ForwardMovement, T> || std::is_same_v<BackwardMovement, T>;
template <class JointMode>
concept joint_mode = std::is_same_v<PromptJoint, JointMode> || std::is_same_v<DelayedJoint, JointMode>;

namespace eval
{

//定数値ノード
template <class Type>
struct CttiConstNode;
struct RttiConstNode;
template <class TravOrStor, class NP, class HasBpos>
struct RttiEvalProxy;

//コンパイル時型情報を持つフィールドノード。SPlaceholderまたはDCttiPlaceholderから作られる。
template <class Placeholder>
struct CttiFieldNode;
template <class Placeholder, DepthType>
struct CttiOuterFieldNode;
template <class Placeholder_, class Nodes, class Indices>
struct CttiIndexedFieldNode;

//実行時型情報のみを持つフィールドノード。DPlaceholderかSRttiPlaceholderにより作られる。
template <class Placeholder>
struct RttiFieldNode;
//
template <class Func, class Nodes, class Components, class Indices>
struct CttiFuncNode;

template <class Components>
struct RttiFuncNode;

template <class Func_, class Node_, RankType FixRank, class Depth, class Up, class Cond = EmptyClass>
struct CttiLayerFuncNode;

}


template <class T>
struct IsRttiConstNode : public std::false_type {};
template <>
struct IsRttiConstNode<eval::RttiConstNode> : public std::true_type {};

template <class T>
struct IsRttiFieldNode : public std::false_type {};
template <class Placeholder>
struct IsRttiFieldNode<eval::RttiFieldNode<Placeholder>> : public std::true_type {};

template <class T>
struct IsRttiFuncNode : public std::false_type {};
template <class Container>
struct IsRttiFuncNode<eval::RttiFuncNode<Container>> : public std::true_type {};

//RttiLayerFuncNodeやRttiIndexedFieldNodeはRttiFuncNodeに統合されている。
template <class T>
struct IsRttiNode : public std::disjunction<IsRttiConstNode<T>, IsRttiFieldNode<T>, IsRttiFuncNode<T>> {};

namespace eval::detail
{
template <class Placeholder>
struct CttiFieldMethods;
template <class Placeholder>
struct RttiFieldMethods;
}

template <class T>
concept anything_typed = requires(std::remove_cvref_t<T> t)
{
	{ t.GetType() } -> std::same_as<FieldType>;
};
template <class T>
concept statistically_typed = anything_typed<T> && requires
{
	typename std::remove_cvref_t<T>::RetType;
	Number<std::remove_cvref_t<T>::GetType()>{};
};
template <class T>
concept dynamically_typed = anything_typed<T> && !statistically_typed<T>;

template <class T>
concept anything_layered = requires(std::remove_cvref_t<T> t)
{
	{ t.GetLayer() } -> std::same_as<LayerType>;
};
template <class T>
concept statistically_layered = anything_layered<T> && requires
{
	LayerConstant<std::remove_cvref_t<T>::GetLayer()>{};
};
template <class T>
concept dynamically_layered = anything_layered<T> && !statistically_layered<T>;

template <class T>
concept placeholder = anything_typed<T> && anything_layered<T> &&
	requires(std::remove_cvref_t<T> v)
{
	typename std::remove_cvref_t<T>::Container;
	{ v.GetIndex() } -> similar_to<uint16_t>;
	{ v.GetPtrOffset() } -> similar_to<ptrdiff_t>;
};
template <class T>
concept rtti_placeholder =
	placeholder<T> && dynamically_typed<T> && dynamically_layered<T> &&
	derived_from_xt<std::remove_cvref_t<T>, eval::detail::RttiFieldMethods>&&
	requires(std::remove_cvref_t<T> v)
{
	{ v.GetInternalLayer() } -> std::same_as<LayerType>;
};
template <class T>
concept typed_placeholder = placeholder<T> &&
	statistically_typed<T> && dynamically_layered<T> &&
	derived_from_xt<std::remove_cvref_t<T>, eval::detail::CttiFieldMethods>;
template <class T>
concept ctti_placeholder = placeholder<T> &&
	statistically_typed<T> && statistically_layered<T> &&
	derived_from_xt<std::remove_cvref_t<T>, eval::detail::CttiFieldMethods>;

template <class T>
concept ranked_placeholder = placeholder<T> && std::remove_cvref_t<T>::MaxRank != 0;

template <class T>
concept stat_type_placeholder = typed_placeholder<T> || ctti_placeholder<T>;

// FieldRefやEvalProxyのように、as<Int>()などで値を取得できるもの。
template <class T>
concept evaluation_proxy = anything_typed<T> && requires(std::remove_cvref_t<T> t)
{
	{ t.template as<FieldType::I32>() };
};

namespace detail
{
template <class T>
struct IsCttiIndexedOuterFieldNode : public std::false_type {};
template <class Placeholder, DepthType Depth, class Nodes, class Indices>
struct IsCttiIndexedOuterFieldNode<eval::CttiOuterFieldNode<eval::CttiIndexedFieldNode<Placeholder, Nodes, Indices>, Depth>> : public std::true_type {};
template <class T>
struct IsCttiLayerFuncNode : public std::false_type {};
template <class Func, class Nodes, RankType FixRank, class Depth, class Up>
struct IsCttiLayerFuncNode<eval::CttiLayerFuncNode<Func, Nodes, FixRank, Depth, Up>> : public std::true_type {};
}

//const nodeはany_nodeのような汎用的な判定が難しいので、
//別に用意する。
template <class T>
concept rtti_const_node = std::same_as<std::remove_cvref_t<T>, eval::RttiConstNode>;
template <class T>
concept ctti_const_node = same_as_xt<std::remove_cvref_t<T>, eval::CttiConstNode>;
template <class T>
concept const_node = rtti_const_node<T> || ctti_const_node<T>;
template <class T>
concept any_node =
	const_node<T> || (anything_typed<T> && anything_layered<T> &&
	requires(std::remove_cvref_t<T> t, LayerType layer,
			 typename std::remove_cvref_t<T>::Traverser trav,
			 typename std::remove_cvref_t<T>::ConstTraverser ctrav,
			 typename std::remove_cvref_t<T>::Container c)
{
	t.Init();
	//t.Evaluate(c.GetRange(layer).cbegin());
	t.Evaluate(trav);
	t.Evaluate(ctrav);
});

template <class T>
concept rtti_node =
	rtti_const_node<T> || (dynamically_typed<T> && dynamically_layered<T> && any_node<T>);

template <class T>
concept rtti_field_node = rtti_node<T> && same_as_xt<std::remove_cvref_t<T>, eval::RttiFieldNode>;
template <class T>
concept rtti_func_node = rtti_node<T> && same_as_xt<std::remove_cvref_t<T>, eval::RttiFuncNode>;

template <class T>
concept typed_node =
	any_node<T> && statistically_typed<T> && dynamically_layered<T>;

template <class T>
concept ctti_node =
	ctti_const_node<T> || (any_node<T> && statistically_typed<T> && statistically_layered<T>);

template <class T>
concept stat_type_node = typed_node<T> || ctti_node<T>;

/*template <class T>
concept ctti_field_node = same_as_xt<std::remove_cvref_t<T>, eval::CttiFieldNode>;
template <class T>
concept ctti_indexed_field_node = same_as_xt<std::remove_cvref_t<T>, eval::CttiIndexedFieldNode>;
template <class T>
concept ctti_outer_field_node = same_as_tn<std::remove_cvref_t<T>, eval::CttiOuterFieldNode>;
template <class T>
concept ctti_indexed_outer_field_node = detail::IsCttiIndexedOuterFieldNode<std::remove_cvref_t<T>>::value;
template <class T>
concept ctti_func_node = same_as_xt<std::remove_cvref_t<T>, eval::CttiFuncNode>;
template <class T>
concept ctti_layer_func_node = detail::IsCttiLayerFuncNode<std::remove_cvref_t<T>>::value;*/

//template <class T>
//concept any_node = rtti_node<T> || ctti_node<T>;

template <class T>
concept node_or_placeholder = placeholder<T> || any_node<T>;

template <class T>
concept neither_node_nor_placeholder = !placeholder<T> && !any_node<T>;

template <class T>
concept ctti_node_or_placeholder = ctti_placeholder<T> || ctti_node<T>;
template <class T>
concept rtti_node_or_placeholder = rtti_placeholder<T> || rtti_node<T>;
template <class T>
concept typed_node_or_placeholder = typed_placeholder<T> || typed_node<T>;

template <class T>
concept stat_type_node_or_placeholder = typed_node_or_placeholder<T> || ctti_node_or_placeholder<T>;

namespace detail
{

/*template <class T>
struct IsNamedNodeOrPlaceholder : std::false_type {};
template <StaticChar Name_, node_or_placeholder NP>
struct IsNamedNodeOrPlaceholder<std::tuple<StaticString<Name_>, NP>> : std::true_type {};
template <node_or_placeholder NP>
struct IsNamedNodeOrPlaceholder<std::tuple<std::string, NP>> : std::true_type {};
*/
template <class T>
struct IsSNamedNodeOrPlaceholder : std::false_type {};
template <StaticChar Name_, node_or_placeholder NP>
struct IsSNamedNodeOrPlaceholder<std::tuple<StaticString<Name_>, NP>> : std::true_type {};
template <class T>
struct IsDNamedNodeOrPlaceholder : std::false_type {};
template <node_or_placeholder NP>
struct IsDNamedNodeOrPlaceholder<std::tuple<std::string, NP>> : std::true_type {};

template <class T>
struct IsCttiNamedNodeOrPlaceholder : std::false_type {};
template <class Name, ctti_node_or_placeholder T>
struct IsCttiNamedNodeOrPlaceholder<std::tuple<Name, T>> : std::true_type {};
}

template <class T>
concept s_named_node_or_placeholder = detail::IsSNamedNodeOrPlaceholder<std::decay_t<T>>::value;
template <class T>
concept d_named_node_or_placeholder = detail::IsDNamedNodeOrPlaceholder<std::decay_t<T>>::value;

template <class T>
concept named_node_or_placeholder = s_named_node_or_placeholder<T> || d_named_node_or_placeholder<T>;

template <class T>
concept s_ctti_named_node_or_placeholder = s_named_node_or_placeholder<T> && detail::IsCttiNamedNodeOrPlaceholder<std::decay_t<T>>::value;

template <class T>
struct GetNodeType { using Type = eval::CttiConstNode<T>; };
template <stat_type_placeholder PH>
struct GetNodeType<PH> { using Type = eval::CttiFieldNode<PH>; };
template <rtti_placeholder PH>
struct GetNodeType<PH> { using Type = eval::RttiFieldNode<PH>; };
template <any_node N>
struct GetNodeType<N> { using Type = N; };
template <class T>
using GetNodeType_t = typename GetNodeType<T>::Type;

template <class T>
concept any_traverser =
	std::input_iterator<std::remove_cvref_t<T>> &&
	requires(std::remove_cvref_t<T> t, LayerType layer, BindexType row)
{
	{ t.GetFixedLayer() } -> std::same_as<LayerType>;
	{ t.GetTravLayer() } -> std::same_as<LayerType>;
	{ t.AssignRow(row) } -> std::same_as<bool>;
	{ t.Move(layer, ForwardMovement{}) } -> std::same_as<bool>;
	{ t.Move(layer, BackwardMovement{}) } -> std::same_as<bool>;
	{ t.MoveForward(layer) } -> std::same_as<bool>;
	{ t.MoveBackward(layer) } -> std::same_as<bool>;
	{ t.MoveToBegin() };
	{ t.MoveToEnd() };
	{ t.IncrDecrOperator(ForwardMovement{}) } -> std::same_as<LayerType>;
	{ t.IncrDecrOperator(BackwardMovement{}) } -> std::same_as<LayerType>;
	{ t.Incr() } -> std::same_as<LayerType>;
	{ t.Decr() } -> std::same_as<LayerType>;
};

template <class T>
concept traversal_range =
std::ranges::input_range<T> &&
	requires (T r, LayerType layer)
{
	typename std::remove_cvref_t<T>::Traverser;
	typename std::remove_cvref_t<T>::ConstTraverser;
	typename std::remove_cvref_t<T>::Sentinel;
	typename std::remove_cvref_t<T>::ConstSentinel;
	{ r.SetTravLayer(layer) } -> std::same_as<void>;
};
template <class T>
concept view_from_traversal_range =
std::ranges::viewable_range<T> && requires (T t)
{
	{ *t.begin() } -> any_traverser;
};

template <class T>
concept sized_traversal_range = traversal_range<T> &&
	requires (T r, LayerType layer)
{
	{ r.GetSize(layer) } -> std::same_as<size_t>;
};


namespace detail
{

template <class Hierarchy, template <class> class Qualifier, class LayerSD>
class ElementListRef_impl;

template <class Hierarchy, template <class> class Qualifier, class LayerSD>
class ElementRef_impl;

template <class Hierarchy, template <class> class Qualifier>
class ElementPtr_impl;

template <class Hierarchy, template <class> class Qualifier>
class ElementIterator_impl;

template <class ListRef>
struct IsElementListRef : std::false_type {};
template <class Hierarchy, template <class> class Qualifier, class LayerSD>
struct IsElementListRef<ElementListRef_impl<Hierarchy, Qualifier, LayerSD>> : std::true_type {};

template <class Ref>
struct IsElementRef : std::false_type {};
template <class Hierarchy, template <class> class Qualifier, class LayerSD>
struct IsElementRef<ElementRef_impl<Hierarchy, Qualifier, LayerSD>> : std::true_type {};

template <class Iter>
struct IsElementIterator : std::false_type {};
template <class Hierarchy, template <class> class Qualifier>
struct IsElementIterator<detail::ElementIterator_impl<Hierarchy, Qualifier>> : std::true_type {};

}

template <class ListRef>
concept element_list_ref = detail::IsElementListRef<ListRef>::value;

template <class Ref>
concept element_ref = detail::IsElementRef<Ref>::value;

template <class Iter>
concept element_iterator = detail::IsElementIterator<Iter>::value;


template <class T>
concept any_hierarchy = requires(std::remove_cvref_t<T> h, LayerType layer, const std::string & name)
{
	//typename std::remove_cvref_t<T>::RttiPlaceholder;
	{ h.GetMaxLayer() } -> std::convertible_to<LayerType>;
	{ h.GetElementSize(0_layer) } -> std::convertible_to<size_t>;
	{ h.GetPlaceholder(name) } -> rtti_placeholder;
	h.GetPlaceholdersIn(0_layer);
};
template <class T>
concept s_hierarchy = any_hierarchy<T> && requires(std::remove_cvref_t<T> h)
{
	//typename std::remove_cvref_t<T>::template CttiPlaceholder<0_layer, int32_t>;
	LayerConstant<std::remove_cvref_t<T>::GetMaxLayer()>{};
	Number<std::remove_cvref_t<T>::GetElementSize(0_layer)>{};
	//{ std::remove_cvref_t<T>::template GetPlaceholder(std::get<0>(std::remove_cvref_t<T>::GetFieldNames())) } -> ctti_placeholder;
	std::remove_cvref_t<T>::GetPlaceholdersIn(0_layer);
};
template <class T>
concept f_hierarchy = any_hierarchy<T> && !s_hierarchy<T> && requires(std::remove_cvref_t<T> h)
{
	LayerConstant<std::remove_cvref_t<T>::GetMaxLayer()>{};
};
template <class T>
concept d_hierarchy = any_hierarchy<T> && !s_hierarchy<T> && !f_hierarchy<T> &&
	requires(std::remove_cvref_t<T> h, LayerType layer, const std::string & name)
{
	//typename std::remove_cvref_t<T>::RttiPlaceholder;
	{ h.GetMaxLayer() } -> similar_to<LayerType>;
	{ h.GetElementSize(layer) } -> similar_to<size_t>;
	{ h.GetPlaceholder(name) } -> rtti_placeholder;
	h.GetPlaceholdersIn(layer);
};

//joined containerはhierarchyではないので、ここではany_hierarchyを要求しない。
template <class T>
concept any_container = requires(std::remove_cvref_t<T> t, LayerType layer)
{
	typename std::remove_cvref_t<T>::Traverser;
	typename std::remove_cvref_t<T>::ConstTraverser;
	typename std::remove_cvref_t<T>::Range;
	typename std::remove_cvref_t<T>::ConstRange;
	{ t.GetRange(layer) } -> traversal_range;
};

template <class T>
concept any_table = any_container<T> && (std::remove_cvref_t<T>::MaxLayer == 0_layer);
template <class T>
concept s_table = s_hierarchy<T> && any_table<T>;
template <class T>
concept d_table = f_hierarchy<T> && any_table<T>;

// DimはDHistの場合は実行時に決まるのでここでは分からない。
// 仕方ないのでBin<1>と固定値にするが、SHistまで一緒くたにBin<1>で判定するのはちょっと意味不明すぎる。
// どうにかならんか。
// いずれかのDimが適格となるような[]やResizeが存在する、というような判定はできないのか。
template <class T>
concept any_hist = any_container<T> && requires(std::remove_cvref_t<T> t, Bin<1> bin)
{
	{ t[bin] } -> element_ref;
	{ t.Resize(bin, bin) };
};
template <class T>
concept s_hist = s_hierarchy<T> && any_hist<T>;
template <class T>
concept d_hist = f_hierarchy<T> && any_hist<T>;

template <class T>
concept any_tree = any_container<T> && !any_table<T> && !any_hist<T>;
template <class T>
concept s_tree = s_hierarchy<T> && any_tree<T>;
template <class T>
concept d_tree = d_hierarchy<T> && any_tree<T>;


//hierarchyであるものはjoined_containerではない。
template <class T>
concept container_simplex = any_container<T> && any_hierarchy<T>;

namespace detail
{
template <RankType MaxRank_, class Container>
class JointMethods;
}

template <class T>
concept joined_container = any_container<T> && derived_from_nxt<std::remove_cvref_t<T>, detail::JointMethods>;

template <template <class> class Modifier, class ...Containers>
class DJoinedContainer;
template <class T>
struct IsDJoinedContainer : public std::false_type {};
template <template <class> class Qualifier, class ...Containers>
struct IsDJoinedContainer<DJoinedContainer<Qualifier, Containers...>> : public std::true_type {};

template <class T>
struct IsSJoinedContainer : public std::false_type {};
//template <template <class> class Qualifier, class ...Containers>
//struct IsSJoinedContainer<SJoinedContainer<Qualifier, Containers...>> : public std::true_type {};

//以下のコンセプトをjoined_containerから引き継ぐ形にすると
//どういうわけか明らかに満足するクラスを渡しても満足しなくなる。
//たとえばd_joined_contaier = joined_container<T>;とするだけでも狂う。
//それどころか、any_containerなどのコンセプトも巻き添えを食って機能しなくなる。
//msvc、gcc、clangいずれも同様だったのでコンパイラのバグではないと思うが、原因が分からない。
template <class T>
concept d_joined_container = IsDJoinedContainer<std::remove_cvref_t<T>>::value;
template <class T>
concept s_joined_container = IsSJoinedContainer<std::remove_cvref_t<T>>::value;

//階層構造をdynamicにしか決定できないもの。
template <class T>
concept d_container = d_tree<T> || d_table<T> || d_joined_container<T>;
//階層構造をstaticに決定できるもの。
template <class T>
concept s_container = s_tree<T> || s_table<T> || s_joined_container<T>;

//template <class T>
//concept joined_container = s_joined_container<T> || d_joined_container<T>;

//template <class T>
//concept any_container = container_simplex<T> || joined_container<T>;


namespace detail
{
template <RankType MaxRank, template <class> class Qualifier, class Container, class IS = std::make_index_sequence<MaxRank + 1>>
class ExternalTraverser;
//template <RankType MaxRank, template <class> class Qualifier, class Container, class IS = std::make_index_sequence<MaxRank + 1>>
//class ExternalTraverser_prompt;
}

}

#endif
