#ifndef ADAPT_EVALUATOR_FUNC_NODE_H
#define ADAPT_EVALUATOR_FUNC_NODE_H

#include <typeindex>
#include <memory>
#include <algorithm>
#include <OpenADAPT/Utility/TypeTraits.h>
#include <OpenADAPT/Joint/LayerInfo.h>
#include <OpenADAPT/Evaluator/NodeBase.h>
#include <OpenADAPT/Traverser/ExternalTraverser.h>

namespace adapt
{

namespace eval
{


//----------CttiFuncNode----------

namespace detail
{

//trivialな場合とreferenceの場合はバッファは使わない。
template <class RetType, bool Trivial = std::is_reference_v<RetType> || std::is_trivially_copyable_v<RetType>>
struct CttiFuncNodeBuffer
{
	static constexpr bool HasBuffer = false;
};
template <class RetType>
struct CttiFuncNodeBuffer<RetType, false>
{
	static constexpr bool HasBuffer = true;
	mutable RetType m_buffer;
};

}

ADAPT_EXPORT
template <class Func, class Nodes,
	class Container = detail::ExtractContainer<Nodes>::Container,
	class Indices = std::make_index_sequence<Nodes::size>>
struct CttiFuncNode;
template <class Func, any_node ...Nodes, class Container_, size_t ...Indices>
struct CttiFuncNode<Func, TypeList<Nodes...>, Container_, std::index_sequence<Indices...>>
	: public detail::CttiFuncNodeBuffer<typename Func::RetType>,
	public detail::CttiMethods<CttiFuncNode<Func, TypeList<Nodes...>, Container_, std::index_sequence<Indices...>>, std::add_const_t>
{
	template <size_t N> using ArgType = GetType_t<N, Nodes...>;
	using RetType = Func::RetType;
	using Container = Container_;
	using Traverser = Container::Traverser;
	using ConstTraverser = Container::ConstTraverser;

	static constexpr RankType MaxRank = Container::MaxRank;
	static constexpr bool HasBuffer = detail::CttiFuncNodeBuffer<typename Func::RetType>::HasBuffer;

	//CttiFuncNodeがTypedではなくCttiであるためには、引数が全てCttiである必要がある。
	static constexpr bool IsCtti = (ctti_node_or_placeholder<Nodes> && ...);

	CttiFuncNode() {}
	template <class Func_, any_node ...Nodes_>
		requires (sizeof...(Nodes) == sizeof...(Nodes_) && sizeof...(Nodes) > 1)
	CttiFuncNode(Func_&& f, Nodes_&& ...nodes)
		: m_func(std::forward<Func_>(f)), m_nodes(std::forward<Nodes_>(nodes)...)
	{}
	template <class Func_, any_node Node_>
		requires (sizeof...(Nodes) == 1 && !std::is_same_v<std::remove_cvref_t<Node_>, CttiFuncNode>)
	CttiFuncNode(Func_&& f, Node_&& node)
		: m_func(std::forward<Func_>(f)), m_nodes(std::forward<Node_>(node))
	{}

	auto IncreaseDepth() const&
	{
		using Nodes_ = TypeList<std::remove_cvref_t<decltype(std::get<Indices>(m_nodes).IncreaseDepth())>...>;
		using Ret = CttiFuncNode<Func, Nodes_, Container, std::index_sequence<Indices...>>;
		return Ret(m_func, std::get<Indices>(m_nodes).IncreaseDepth()...);
	}
	auto IncreaseDepth()&&
	{
		using Nodes_ = TypeList<std::remove_cvref_t<decltype(std::get<Indices>(m_nodes).IncreaseDepth())>...>;
		using Ret = CttiFuncNode<Func, Nodes_, Container, std::index_sequence<Indices...>>;
		return Ret(std::move(m_func), std::get<Indices>(std::move(m_nodes)).IncreaseDepth()...);
	}

private:
	template <size_t N, class ...Args>
	void Init_impl([[maybe_unused]] Args&& ...args)
	{
		if constexpr (N < sizeof...(Nodes))
		{
			if constexpr (any_node<GetType_t<N, Nodes...>>) std::get<N>(m_nodes).Init(std::forward<Args>(args)...);
			Init_impl<N + 1>(std::forward<Args>(args)...);
		}
		else
		{
			m_init_flag = false;
		}
	}
public:
	void Init(const Traverser& t, const std::vector<std::tuple<const void*, const Bpos*, bool>>& outer_t)
	{
		Init_impl<0>(t, outer_t);
	}
	void Init(const Traverser& t)
	{
		Init(t, {});
	}
	void Init(const ConstTraverser& t, const std::vector<std::tuple<const void*, const Bpos*, bool>>& outer_t)
	{
		Init_impl<0>(t, outer_t);
	}
	void Init(const ConstTraverser& t)
	{
		Init(t, {});
	}
	void Init(const Container& s, const Bpos& bpos)
	{
		Init_impl<0>(s, bpos);
	}
	void Init(const Container& s)
	{
		Init_impl<0>(s);
	}
	void Init()
	{
		Init_impl<0>();
		m_init_flag = true;
	}

	//Cttiの場合は階層はstaticに決定できる。
private:
	//連結情報を持っているJointLayerArrayを探す。
	template <size_t N>
	static constexpr JointLayerArray<MaxRank> GetJointLayerArray_impl() requires IsCtti
	{
		if constexpr (N <= MaxRank)
		{
			JointLayerArray<MaxRank> res = GetType_t<N, Nodes...>::GetJointLayerArray();
			if (!res.IsNotInitialized()) return res;
			return GetJointLayerArray_impl<N + 1>();
		}
		else return JointLayerArray<MaxRank>{};
	}
public:
	static constexpr JointLayerArray<MaxRank> GetJointLayerArray() requires IsCtti
	{
		return GetJointLayerArray_impl<0>();
	}
	static constexpr LayerInfo<MaxRank> GetLayerInfo() requires IsCtti
	{
		return GetLayerInfo(LayerInfo<MaxRank>(GetJointLayerArray()));
	}
	static constexpr LayerInfo<MaxRank> GetLayerInfo(LayerInfo<MaxRank> eli) requires IsCtti
	{
		return std::max({ GetType_t<Indices, Nodes...>::GetLayerInfo(eli)... });
	}
	static constexpr LayerInfo<MaxRank> GetLayerInfo(LayerInfo<MaxRank> eli, DepthType depth) requires IsCtti
	{
		return std::max({ GetType_t<Indices, Nodes...>::GetLayerInfo(eli, depth)... });
	}
	static constexpr LayerType GetLayer() requires IsCtti
	{
		return GetLayerInfo().GetTravLayer();
	}


	//Typedの場合、階層はstaticに決定できず、staticメンバ関数にも出来ない。
private:
	//連結情報を持っているJointLayerArrayを探す。
	template <size_t N>
	JointLayerArray<MaxRank> GetJointLayerArray_impl() const requires (!IsCtti)
	{
		if constexpr (N < sizeof...(Nodes))
		{
			JointLayerArray<MaxRank> res = std::get<N>(m_nodes).GetJointLayerArray();
			if (!res.IsNotInitialized()) return res;
			return GetJointLayerArray_impl<N + 1>();
		}
		else return JointLayerArray<MaxRank>{};
	}
	template <size_t ...Indices_>
	LayerInfo<MaxRank> GetLayerInfo_impl(LayerInfo<MaxRank> eli, std::index_sequence<Indices_...>) const requires (!IsCtti)
	{
		return std::max({ std::get<Indices_>(m_nodes).GetLayerInfo(eli)... });
	}
	template <size_t ...Indices_>
	LayerInfo<MaxRank> GetLayerInfo_impl(LayerInfo<MaxRank> eli, DepthType depth, std::index_sequence<Indices_...>) const requires (!IsCtti)
	{
		return std::max({ std::get<Indices_>(m_nodes).GetLayerInfo(eli, depth)... });
	}
public:
	JointLayerArray<MaxRank> GetJointLayerArray() const requires (!IsCtti)
	{
		return GetJointLayerArray_impl<0>();
	}
	LayerInfo<MaxRank> GetLayerInfo() const requires (!IsCtti)
	{
		return GetLayerInfo(LayerInfo<MaxRank>(GetJointLayerArray()));
	}
	LayerInfo<MaxRank> GetLayerInfo(LayerInfo<MaxRank> eli) const requires (!IsCtti)
	{
		return GetLayerInfo_impl(eli, std::make_index_sequence<sizeof...(Nodes)>{});
	}
	LayerInfo<MaxRank> GetLayerInfo(LayerInfo<MaxRank> eli, DepthType depth) const requires (!IsCtti)
	{
		return GetLayerInfo_impl(eli, depth, std::make_index_sequence<sizeof...(Nodes)>{});
	}
	LayerType GetLayer() const requires (!IsCtti)
	{
		return GetLayerInfo().GetTravLayer();
	}

	template <size_t Index, class ...Args>
	decltype(auto) GetArg(const Args& ...args) const
	{
		return std::get<Index>(m_nodes).Evaluate(args...);
	}

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4702)
#endif
	RetType Evaluate(const Traverser& t) const
		requires (!HasBuffer)
	{
		return m_func.Exec(*this, t);
	}
	RetType Evaluate(const ConstTraverser& t) const
		requires (!HasBuffer)
	{
		return m_func.Exec(*this, t);
	}
	const RetType& Evaluate(const Traverser& t) const
		requires HasBuffer
	{
		m_func.ExecWithBuf(this->m_buffer, *this, t);
		return this->m_buffer;
	}
	const RetType& Evaluate(const ConstTraverser& t) const
		requires HasBuffer
	{
		m_func.ExecWithBuf(this->m_buffer, *this, t);
		return this->m_buffer;
	}

	RetType Evaluate(const Container& s) const
		requires (!HasBuffer)
	{
		return m_func.Exec(*this, s);
	}
	RetType Evaluate(const Container& s, const Bpos& bpos) const
		requires (!HasBuffer)
	{
		return m_func.Exec(*this, s, bpos);
	}
	const RetType& Evaluate(const Container& s) const
		requires HasBuffer
	{
		m_func.ExecWithBuf(this->m_buffer, *this, s);
		return this->m_buffer;
	}
	const RetType& Evaluate(const Container& s, const Bpos& bpos) const
		requires HasBuffer
	{
		m_func.ExecWithBuf(this->m_buffer, *this, s, bpos);
		return this->m_buffer;
	}
#ifdef _MSC_VER
#pragma warning(pop)
#endif

	decltype(auto) operator()(const Traverser& t)
	{
		if (m_init_flag) [[unlikely]] Init(t, {});
		return Evaluate(t);
	}
	decltype(auto) operator()(const ConstTraverser& t)
	{
		if (m_init_flag) [[unlikely]] Init(t, {});
		return Evaluate(t);
	}
	decltype(auto) operator()(const Container& s)
	{
		if (m_init_flag) [[unlikely]] Init(s, {});
		return Evaluate(s);
	}
	decltype(auto) operator()(const Container& s, const Bpos& bpos)
	{
		if (m_init_flag) [[unlikely]] Init(s, bpos);
		return Evaluate(s, bpos);
	}

private:
	[[no_unique_address]] Func m_func;
	std::tuple<Nodes...> m_nodes;
	bool m_init_flag = true;
};

}



//----------RttiFuncNode----------

namespace eval
{

ADAPT_EXPORT
template <class Placeholder>
struct RttiFieldNode;

namespace detail
{

template <class Func, class Container, class Nodes,
	FieldType RetType = DFieldInfo::GetSameSizeTagType<typename Func::RetType>(),
	class Indices = std::make_index_sequence<Nodes::size>>
struct RttiFuncNode_impl;

template <class Func, class Container, class ...Nodes, FieldType Type, size_t ...Indices>
	requires (DFieldInfo::IsTrivial(Type))
struct RttiFuncNode_impl<Func, Container, TypeList<Nodes...>, Type, std::index_sequence<Indices...>>
	: public RttiFuncNode_body<Container, TypeList<Nodes...>, std::index_sequence<Indices...>,
							   TypeList<typename Func::template ArgType<Indices>...>>
{
	static_assert(DFieldInfo::IsInt(Type) || DFieldInfo::IsFlt(Type) || DFieldInfo::IsCpx(Type));
	using Base = RttiFuncNode_body<Container, TypeList<Nodes...>, std::index_sequence<Indices...>,
								   TypeList<typename Func::template ArgType<Indices>...>>;
	using Traverser = Container::Traverser;
	using ConstTraverser = Container::ConstTraverser;

	RttiFuncNode_impl() {}
	template <class Func_, node_or_placeholder ...Nodes_>
	RttiFuncNode_impl(Func_&& f, Nodes_&& ...nodes)
		: Base(std::forward<Nodes_>(nodes)...), m_func(std::forward<Func_>(f)) {}

	virtual ~RttiFuncNode_impl() = default;
	virtual void CopyFrom(const RttiFuncNode_base<Container>& that) override
	{
		Base::CopyFrom(that);
		const auto* x = dynamic_cast<const RttiFuncNode_impl*>(&that);
		m_func = x->m_func;
	}
	virtual RttiFuncNode_base<Container>* Clone() const override
	{
		auto* res = new RttiFuncNode_impl{};
		res->CopyFrom(*this);
		return res;
	}
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4244)
#endif
	using RetType = DFieldInfo::TagTypeToValueType<Type>;
	RetType Evaluate_impl(const Traverser& t, Number<Type>) const
	{
		return m_func.Exec(*this, t);
	}
	RetType Evaluate_impl(const ConstTraverser& t, Number<Type>) const
	{
		return m_func.Exec(*this, t);
	}
	RetType Evaluate_impl(const Container& s, Number<Type>) const
	{
		return m_func.Exec(*this, s);
	}
	RetType Evaluate_impl(const Container& s, const Bpos& bpos, Number<Type>) const
	{
		return m_func.Exec(*this, s, bpos);
	}

	using enum FieldType;
	#define CODE(TTYPE, SYM, VTYPE)\
	virtual VTYPE Evaluate(const Traverser& t, Number<TTYPE>) const override\
	{\
		if constexpr (std::convertible_to<RetType, VTYPE>) return (VTYPE)Evaluate_impl(t, Number<Type>{});\
		else throw MismatchType("");\
	}\
	virtual VTYPE Evaluate(const ConstTraverser& t, Number<TTYPE>) const override\
	{\
		if constexpr (std::convertible_to<RetType, VTYPE>) return (VTYPE)Evaluate_impl(t, Number<Type>{});\
		else throw MismatchType("");\
	}\
	virtual VTYPE Evaluate(const Container& s, Number<TTYPE>) const override\
	{\
		if constexpr (std::convertible_to<RetType, VTYPE>) return (VTYPE)Evaluate_impl(s, Number<Type>{});\
		else throw MismatchType("");\
	}\
	virtual VTYPE Evaluate(const Container& s, const Bpos& bpos, Number<TTYPE>) const override\
	{\
		if constexpr (std::convertible_to<RetType, VTYPE>) return (VTYPE)Evaluate_impl(s, bpos, Number<Type>{});\
		else throw MismatchType("");\
	}
	ADAPT_FOR_EACH_TRIVIAL_TYPE(CODE)
	#undef CODE

#ifdef _MSC_VER
#pragma warning(pop)
#endif

	virtual FieldType GetType() const override { return Type; }

	[[no_unique_address]] Func m_func;
};

template <class Func, class Container, class ...Nodes, FieldType Type, size_t ...Indices>
	requires (!DFieldInfo::IsTrivial(Type))
struct RttiFuncNode_impl<Func, Container, TypeList<Nodes...>, Type, std::index_sequence<Indices...>>
	: public RttiFuncNode_body<Container, TypeList<Nodes...>, std::index_sequence<Indices...>,
							   TypeList<typename Func::template ArgType<Indices>...>>
{
	//m_bufをメンバとして保つ必要があるため、trivialの方と統合できない。
	static_assert(DFieldInfo::IsStr(Type) || DFieldInfo::IsJbp(Type));
	using Base = RttiFuncNode_body<Container, TypeList<Nodes...>,
								   std::index_sequence<Indices...>, TypeList<typename Func::template ArgType<Indices>...>>;
	using Traverser = Container::Traverser;
	using ConstTraverser = Container::ConstTraverser;
	
	RttiFuncNode_impl() {}
	template <class Func_, node_or_placeholder ...Nodes_>
	RttiFuncNode_impl(Func_&& f, Nodes_&& ...nodes)
		: Base(std::forward<Nodes_>(nodes)...), m_func(std::forward<Func_>(f)) {}

	virtual ~RttiFuncNode_impl() = default;
	virtual void CopyFrom(const RttiFuncNode_base<Container>& that) override
	{
		Base::CopyFrom(that);
		const auto* x = dynamic_cast<const RttiFuncNode_impl*>(&that);
		m_func = x->m_func;
	}
	virtual RttiFuncNode_base<Container>* Clone() const override
	{
		auto* res = new RttiFuncNode_impl{};
		res->CopyFrom(*this);
		res->m_func = m_func;
		return res;
	}
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4702)
#endif
	using RetType = DFieldInfo::TagTypeToValueType<Type>;
	const RetType& Evaluate_impl(const Traverser& t, Number<Type>) const
	{
		m_func.ExecWithBuf(m_buf, *this, t);
		return m_buf;
	}
	const RetType& Evaluate_impl(const ConstTraverser& t, Number<Type>) const
	{
		m_func.ExecWithBuf(m_buf, *this, t);
		return m_buf;
	}
	const RetType& Evaluate_impl(const Container& s, Number<Type>) const
	{
		m_func.ExecWithBuf(m_buf, *this, s);
		return m_buf;
	}
	const RetType& Evaluate_impl(const Container& s, const Bpos& bpos, Number<Type>) const
	{
		m_func.ExecWithBuf(m_buf, *this, s, bpos);
		return m_buf;
	}
#ifdef _MSC_VER
#pragma warning(pop)
#endif

	virtual const RetType& Evaluate(const Traverser& t, Number<Type>) const override
	{
		m_func.ExecWithBuf(m_buf, *this, t);
		return m_buf;
	}
	virtual const RetType& Evaluate(const ConstTraverser& t, Number<Type>) const override
	{
		m_func.ExecWithBuf(m_buf, *this, t);
		return m_buf;
	}
	virtual const RetType& Evaluate(const Container& s, Number<Type>) const override
	{
		m_func.ExecWithBuf(m_buf, *this, s);
		return m_buf;
	}
	virtual const RetType& Evaluate(const Container& s, const Bpos& bpos, Number<Type>) const override
	{
		m_func.ExecWithBuf(m_buf, *this, s, bpos);
		return m_buf;
	}

	using enum FieldType;
	#define CODE(TTYPE, SYM, VTYPE)\
	virtual VTYPE Evaluate(const Traverser& t, Number<TTYPE>) const override\
	{\
		if constexpr (std::convertible_to<RetType, VTYPE>) return (VTYPE)Evaluate(t, Number<Type>{});\
		else throw MismatchType("");\
	}\
	virtual VTYPE Evaluate(const ConstTraverser& t, Number<TTYPE>) const override\
	{\
		if constexpr (std::convertible_to<RetType, VTYPE>) return (VTYPE)Evaluate(t, Number<Type>{});\
		else throw MismatchType("");\
	}\
	virtual VTYPE Evaluate(const Container& s, Number<TTYPE>) const override\
	{\
		if constexpr (std::convertible_to<RetType, VTYPE>) return (VTYPE)Evaluate(s, Number<Type>{});\
		else throw MismatchType("");\
	}\
	virtual VTYPE Evaluate(const Container& s, const Bpos& bpos, Number<TTYPE>) const override\
	{\
		if constexpr (std::convertible_to<RetType, VTYPE>) return (VTYPE)Evaluate(s, bpos, Number<Type>{});\
		else throw MismatchType("");\
	}
	ADAPT_FOR_EACH_TRIVIAL_TYPE(CODE)
	#undef CODE
	virtual FieldType GetType() const override { return Type; }

	[[no_unique_address]] Func m_func;
	mutable RetType m_buf;
};

}


namespace detail
{
template <class Container, FieldType ...ArgTypes, class Func, any_node ...Nodes>
eval::RttiFuncNode<Container> MakeRttiFuncNode_construct(Func&& f, Nodes&& ...n)
{
	if constexpr (requires { f(std::declval<DFieldInfo::TagTypeToValueType<ArgTypes>>()...); })
	{
		using DecFunc = std::decay_t<Func>;
		using RetType = std::decay_t<std::invoke_result_t<DecFunc, DFieldInfo::TagTypeToValueType<ArgTypes>...>>;
		using FuncA = FuncDefinition<DecFunc, RetType, DFieldInfo::TagTypeToValueType<ArgTypes>...>;
		using NodeImpl = detail::RttiFuncNode_impl<FuncA, Container, TypeList<std::decay_t<Nodes>...>>;
		eval::RttiFuncNode<Container> res;
		res.template Construct<NodeImpl>(std::forward<Func>(f), std::forward<Nodes>(n)...);
		return res;
	}
	else
	{
		throw MismatchType("");
	}
}

template <class Container, class ...ArgTypes, class Func, any_node ...Nodes>
auto MakeRttiFuncNode_construct(int, Func&& f, Nodes&& ...n)
	-> decltype(eval::RttiFuncNode<Container>{})
{
	if constexpr (requires { f(std::declval<ArgTypes>()...); })
	{
		using DecFunc = std::decay_t<Func>;
		using RetType = std::decay_t<std::invoke_result_t<DecFunc, ArgTypes...>>;
		using FuncA = FuncDefinition<DecFunc, RetType, ArgTypes...>;
		using NodeImpl = detail::RttiFuncNode_impl<FuncA, Container, TypeList<std::decay_t<Nodes>...>>;
		eval::RttiFuncNode<Container> res;
		res.template Construct<NodeImpl>(std::forward<Func>(f), std::forward<Nodes>(n)...);
		return res;
	}
	else
	{
		throw MismatchType("");
	}
}
/*template <class Container, class ...ArgTypes, class Func, any_node ...Nodes>
auto MakeRttiFuncNode_construct(float, Func&&, Nodes&& ...)
	-> eval::RttiFuncNode<Container>
{
	throw MismatchType("");
}*/

template <class Container, class Func, FieldType ...Types, any_node ...Nodes, size_t ...Indices>
auto MakeRttiFuncNode_expand(Func&& f, ValueList<Types...>, std::tuple<Nodes...> t, std::index_sequence<Indices...>)
{
	return MakeRttiFuncNode_construct<Container, DFieldInfo::TagTypeToValueType<Types>...>(1, std::forward<Func>(f), std::get<Indices>(t)...);
}

template <class Container, class Func, FieldType ...Types, any_node ...Nodes>
auto MakeRttiFuncNode(Func&& f, ValueList<Types...> v, std::tuple<Nodes...> t)
{
	return MakeRttiFuncNode_expand<Container>(std::forward<Func>(f), v, std::move(t), std::make_index_sequence<sizeof...(Nodes)>{});
}
/*
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4702)
#endif
template <class Container, class Func, FieldType ...Types, any_node ...Nodes, any_node Head, any_node ...Body>
auto MakeRttiFuncNode(Func&& f, ValueList<Types...>, std::tuple<Nodes...> t, Head&& head, Body&& ...body)
{
	#define RECURSE(TYPE) \
	return MakeRttiFuncNode(std::forward<Func>(f), ValueList<Types..., TYPE>(),\
							TupleAdd(std::move(t), std::forward<Head>(head)),\
							std::forward<Body>(body)...);
	ADAPT_SWITCH_FIELD_TYPE(head.GetType(), RECURSE, throw MismatchType("");)
	#undef RECURSE
}
#ifdef _MSC_VER
#pragma warning(pop)
#endif
*/
template <class Container, class Func, any_node Node>
auto MakeRttiFuncNode(Func&& f, Node&& node)
{
	using enum FieldType;
	#define CODE(TTYPE, SYM, VTYPE)\
	if constexpr (requires { f(std::declval<const VTYPE&>()); }) if (node.GetType() == TTYPE)\
		return MakeRttiFuncNode_construct<Container, TTYPE>(std::forward<Func>(f), std::forward<Node>(node));
	ADAPT_FOR_EACH_TYPE(CODE)
	#undef CODE
	throw MismatchType("");
}
template <any_node Node>
auto MakeRttiFuncNode_cast_bool(Node&& node)
{
	if constexpr (rtti_field_node<Node>) return cast_bool(node);
	else if constexpr (rtti_func_node<Node>) return std::forward<Node>(node);//func_nodeなら何もする必要がない。
	else if constexpr (rtti_const_node<Node>)
	{
		if (node.GetType() == FieldType::I08) return std::forward<Node>(node);
		bool tmp = false;
		#define CODE(TTYPE, SYM, VTYPE)\
		if constexpr (std::convertible_to<VTYPE, bool>)\
			if (node.GetType() == FieldType::TTYPE) tmp = (bool)node.template as<FieldType::TTYPE>();
		ADAPT_FOR_EACH_TRIVIAL_TYPE(CODE)
		#undef CODE
		return RttiConstNode((int8_t)tmp);
	}
	else if constexpr (stat_type_node<Node>) return cast_bool(node);
	else throw MismatchType("");
}

template <FieldType Type, stat_type_node Node>
auto MakeRttiFuncNode_cast(Node&& node)
{
	assert(node.GetType() != Type);
	return cast<Type>(node);
}
template <FieldType Type, rtti_field_node Node>
auto MakeRttiFuncNode_cast(Node&& node)
{
	assert(node.GetType() != Type);
	return cast<Type>(node);
}
template <FieldType Type, rtti_func_node Node>
auto MakeRttiFuncNode_cast(Node&& node)
{
	assert(node.GetType() != Type);
	return std::forward<Node>(node);//func_nodeなら何もする必要がない。
}
template <FieldType Type, rtti_const_node Node>
RttiConstNode MakeRttiFuncNode_cast(Node&& node)
{
	assert(node.GetType() != Type);
	using enum FieldType;
	using RetType = DFieldInfo::TagTypeToValueType<Type>;
	#define CODE(TTYPE, SYM, VTYPE)\
	if constexpr (DFieldInfo::IsConvertibleTo<TTYPE, Type>())\
	{\
		if (node.GetType() == TTYPE)\
		{\
			if constexpr (IsSpecializationOf_v<std::complex, RetType> && DFieldInfo::IsArithmetic(TTYPE))\
				return RttiConstNode(RetType(typename RetType::value_type(node.template as<TTYPE>())));\
			else\
				return RttiConstNode(RetType(node.template as<TTYPE>()));\
		}\
	}
	ADAPT_FOR_EACH_TRIVIAL_TYPE(CODE)
	#undef CODE
	throw MismatchType("");
}

// 整数昇格を行った上でFuncを適用する。
template <class Container, class Func, any_node Node1, any_node Node2>
auto IntegralPromotion(Func&& f, Node1&& node1, Node2&& node2)
{
	using enum FieldType;
	FieldType type1 = node1.GetType();
	FieldType type2 = node2.GetType();
	assert(DFieldInfo::IsInt(type1) && DFieldInfo::IsInt(type2));
	if (DFieldInfo::GetSizeOf(type1) < 4 && DFieldInfo::GetSizeOf(type2) < 4)
	{
		return MakeRttiFuncNode_construct<Container, I32, I32>(std::forward<Func>(f),
															   MakeRttiFuncNode_cast<I32>(std::forward<Node1>(node1)),
															   MakeRttiFuncNode_cast<I32>(std::forward<Node2>(node2)));
	}
	if (DFieldInfo::GetSizeOf(type1) < 4)
	{
		//type1のみ昇格。
		if (type2 == I32)
			return MakeRttiFuncNode_construct<Container, I32, I32>(std::forward<Func>(f),
																   MakeRttiFuncNode_cast<I32>(std::forward<Node1>(node1)),
																   std::forward<Node2>(node2));
		else
			return MakeRttiFuncNode_construct<Container, I32, I64>(std::forward<Func>(f),
																   MakeRttiFuncNode_cast<I32>(std::forward<Node1>(node1)),
																   std::forward<Node2>(node2));
	}
	else if (DFieldInfo::GetSizeOf(type2) < 4)
	{
		//type2のみ昇格。
		if (type1 == I32)
			return MakeRttiFuncNode_construct<Container, I32, I32>(std::forward<Func>(f),
																   std::forward<Node1>(node1),
																   MakeRttiFuncNode_cast<I32>(std::forward<Node2>(node2)));
		else
			return MakeRttiFuncNode_construct<Container, I64, I32>(std::forward<Func>(f),
																   std::forward<Node1>(node1),
																   MakeRttiFuncNode_cast<I32>(std::forward<Node2>(node2)));
	}
	else
	{
		//両方とも昇格の必要なし。
		//ただしtype1、type2ともに32bitか64bitかチェックし分岐する必要はある。
		if (type1 == I32 && type2 == I32)
			return MakeRttiFuncNode_construct<Container, I32, I32>(std::forward<Func>(f),
																   std::forward<Node1>(node1),
																   std::forward<Node2>(node2));
		else if (type1 == I32 && type2 == I64)
			return MakeRttiFuncNode_construct<Container, I32, I64>(std::forward<Func>(f),
																   std::forward<Node1>(node1),
																   std::forward<Node2>(node2));
		else if (type1 == I64 && type2 == I32)
			return MakeRttiFuncNode_construct<Container, I64, I32>(std::forward<Func>(f),
																   std::forward<Node1>(node1),
																   std::forward<Node2>(node2));
		else
			return MakeRttiFuncNode_construct<Container, I64, I64>(std::forward<Func>(f),
																   std::forward<Node1>(node1),
																   std::forward<Node2>(node2));
	}
}
template <class Container, class Func, any_node Node1, any_node Node2>
auto IntegralArithmeticConversion(Func&& f, Node1&& node1, Node2&& node2)
{
	using enum FieldType;
	FieldType type1 = node1.GetType();
	FieldType type2 = node2.GetType();
	assert(DFieldInfo::IsInt(type1) && DFieldInfo::IsInt(type2));
	if (DFieldInfo::GetSizeOf(type1) < 4 && DFieldInfo::GetSizeOf(type2) < 4)
	{
		return MakeRttiFuncNode_construct<Container, I32, I32>(std::forward<Func>(f),
															   MakeRttiFuncNode_cast<I32>(std::forward<Node1>(node1)),
															   MakeRttiFuncNode_cast<I32>(std::forward<Node2>(node2)));
	}
	else if (DFieldInfo::GetSizeOf(type1) < DFieldInfo::GetSizeOf(type2))
	{
		if (type2 == I32)
			return MakeRttiFuncNode_construct<Container, I32, I32>(std::forward<Func>(f),
																   MakeRttiFuncNode_cast<I32>(std::forward<Node1>(node1)),
																   std::forward<Node2>(node2));
		else
			return MakeRttiFuncNode_construct<Container, I64, I64>(std::forward<Func>(f),
																   MakeRttiFuncNode_cast<I64>(std::forward<Node1>(node1)),
																   std::forward<Node2>(node2));
	}
	else if (DFieldInfo::GetSizeOf(type1) > DFieldInfo::GetSizeOf(type2))
	{
		if (type1 == I32)
			return MakeRttiFuncNode_construct<Container, I32, I32>(std::forward<Func>(f),
																   std::forward<Node1>(node1),
																   MakeRttiFuncNode_cast<I32>(std::forward<Node2>(node2)));
		else
			return MakeRttiFuncNode_construct<Container, I64, I64>(std::forward<Func>(f),
																   std::forward<Node1>(node1),
																   MakeRttiFuncNode_cast<I64>(std::forward<Node2>(node2)));
	}
	else
	{
		if (type1 == I32)
			return MakeRttiFuncNode_construct<Container, I32, I32>(std::forward<Func>(f),
																   std::forward<Node1>(node1),
																   std::forward<Node2>(node2));
		else
			return MakeRttiFuncNode_construct<Container, I64, I64>(std::forward<Func>(f),
																   std::forward<Node1>(node1),
																   std::forward<Node2>(node2));
	}
}
template <class Container, class Func, any_node Node1, any_node Node2>
auto UsualArithmeticConversion(Func&& f, Node1&& node1, Node2&& node2)
{
	using enum FieldType;
	FieldType type1 = node1.GetType();
	FieldType type2 = node2.GetType();
	assert(DFieldInfo::IsArithmetic(type1) && DFieldInfo::IsArithmetic(type2));
	if (DFieldInfo::IsFlt(type1) || DFieldInfo::IsFlt(type2))
	{
		//片方でも浮動小数点数型なら、両方とも浮動小数点数型に変換される。
		if (type1 == F64 && type2 == F64)
			return MakeRttiFuncNode_construct<Container, F64, F64>(std::forward<Func>(f),
																   std::forward<Node1>(node1),
																   std::forward<Node2>(node2));
		else if (type1 == F32 && type2 == F32)
			return MakeRttiFuncNode_construct<Container, F32, F32>(std::forward<Func>(f),
																   std::forward<Node1>(node1),
																   std::forward<Node2>(node2));
		else if (DFieldInfo::IsF64(type1) && !DFieldInfo::IsF64(type2))
			return MakeRttiFuncNode_construct<Container, F64, F64>(std::forward<Func>(f),
																   std::forward<Node1>(node1),
																   MakeRttiFuncNode_cast<F64>(std::forward<Node2>(node2)));
		else if (!DFieldInfo::IsF64(type1) && DFieldInfo::IsF64(type2))
			return MakeRttiFuncNode_construct<Container, F64, F64>(std::forward<Func>(f),
																   MakeRttiFuncNode_cast<F64>(std::forward<Node1>(node1)),
																   std::forward<Node2>(node2));
		else if (DFieldInfo::IsF32(type1) && !DFieldInfo::IsF32(type2))
			return MakeRttiFuncNode_construct<Container, F32, F32>(std::forward<Func>(f),
																   std::forward<Node1>(node1),
																   MakeRttiFuncNode_cast<F32>(std::forward<Node2>(node2)));
		else if (!DFieldInfo::IsF32(type1) && DFieldInfo::IsF32(type2))
			return MakeRttiFuncNode_construct<Container, F32, F32>(std::forward<Func>(f),
																   MakeRttiFuncNode_cast<F32>(std::forward<Node1>(node1)),
																   std::forward<Node2>(node2));
		else throw MismatchType("");
	}
	else
	{
		//両方とも整数型の場合は整数限定の算術変換を呼び出せば良い。
		return IntegralArithmeticConversion<Container>(std::forward<Func>(f), std::forward<Node1>(node1), std::forward<Node2>(node2));
	}
}
template <class Container, class Func, any_node Node1, any_node Node2>
auto ComplexArithmeticConversion(Func&& f, Node1&& node1, Node2&& node2)
{
	using enum FieldType;
	FieldType type1 = node1.GetType();
	FieldType type2 = node2.GetType();
	assert(DFieldInfo::IsCpxAri(type1) && DFieldInfo::IsCpxAri(type2));
	if (DFieldInfo::IsCpx(type1) || DFieldInfo::IsCpx(type2))
	{
		//片方でも複素数型なら、両方とも複素数型に変換される。
		//ただし、Int->CpxやF64->C32のような変換は禁止しており、エラーとする。
		if (DFieldInfo::IsInt(type1) || DFieldInfo::IsInt(type2)) throw MismatchType("");
		if (DFieldInfo::IsC64(type1) && !DFieldInfo::IsC64(type2))
			return MakeRttiFuncNode_construct<Container, C64, C64>(std::forward<Func>(f),
																   std::forward<Node1>(node1),
																   MakeRttiFuncNode_cast<C64>(std::forward<Node2>(node2)));
		else if (!DFieldInfo::IsC64(type1) && DFieldInfo::IsC64(type2))
			return MakeRttiFuncNode_construct<Container, C64, C64>(std::forward<Func>(f),
																   MakeRttiFuncNode_cast<C64>(std::forward<Node1>(node1)),
																   std::forward<Node2>(node2));
		else if (DFieldInfo::IsC32(type1) && !DFieldInfo::IsC32(type2))
		{
			if (DFieldInfo::IsF64(type2)) throw MismatchType("");
			return MakeRttiFuncNode_construct<Container, C32, C32>(std::forward<Func>(f),
																   std::forward<Node1>(node1),
																   MakeRttiFuncNode_cast<C32>(std::forward<Node2>(node2)));
		}
		else if (!DFieldInfo::IsC32(type1) && DFieldInfo::IsC32(type2))
		{
			if (DFieldInfo::IsF64(type1)) throw MismatchType("");
			return MakeRttiFuncNode_construct<Container, C32, C32>(std::forward<Func>(f),
																   MakeRttiFuncNode_cast<C32>(std::forward<Node1>(node1)),
																   std::forward<Node2>(node2));
		}
		else if (DFieldInfo::IsC64(type1) && DFieldInfo::IsC64(type2))
			return MakeRttiFuncNode_construct<Container, C64, C64>(std::forward<Func>(f),
																   std::forward<Node1>(node1),
																   std::forward<Node2>(node2));
		else if (DFieldInfo::IsC32(type1) && DFieldInfo::IsC32(type2))
			return MakeRttiFuncNode_construct<Container, C32, C32>(std::forward<Func>(f),
																   std::forward<Node1>(node1),
																   std::forward<Node2>(node2));
		else throw MismatchType("");
	}
	else
	{
		//両方とも非複素数型の場合は通常の算術変換を呼び出せば良い。
		return UsualArithmeticConversion<Container>(std::forward<Func>(f), std::forward<Node1>(node1), std::forward<Node2>(node2));
	}
}
template <class Container, class Func, any_node Node1, any_node Node2>
	requires (std::decay_t<Func>::Level == ArithmeticConvLevel::Bool)
auto MakeRttiFuncNode(Func&& f, Node1&& node1, Node2&& node2)
{
	using enum FieldType;
	using DecFunc = std::decay_t<Func>;
	if constexpr (requires { DecFunc::Level == ArithmeticConvLevel::Bool; })
	{
		FieldType left_type = node1.GetType();
		FieldType right_type = node2.GetType();
		//ここはI08への変換可否で判定してはいけない。
		//float/double -> int8_tは小数点以下を切り捨てるため、本来trueとなるべき状況でfalseになることがありうる。
		if (!DFieldInfo::IsConvertibleToBool(left_type) || !DFieldInfo::IsConvertibleToBool(right_type)) throw MismatchType("");
		if (left_type != I08 && right_type != I08)
		{
			auto node1_ = MakeRttiFuncNode_cast_bool(std::forward<Node1>(node1));
			auto node2_ = MakeRttiFuncNode_cast_bool(std::forward<Node2>(node2));

			using FuncA = FuncDefinition<DecFunc, std::decay_t<std::invoke_result_t<DecFunc, int8_t, int8_t>>, int8_t, int8_t>;
			using NodeImpl = detail::RttiFuncNode_impl<FuncA, Container, TypeList<std::decay_t<decltype(node1_)>, std::decay_t<decltype(node2_)>>>;
			eval::RttiFuncNode<Container> res;
			res.template Construct<NodeImpl>(std::forward<Func>(f), std::move(node1_), std::move(node2_));
			return res;
		}
		else if (left_type != I08)
		{
			auto node1_ = MakeRttiFuncNode_cast_bool(std::forward<Node1>(node1));
			using FuncA = FuncDefinition<DecFunc, std::decay_t<std::invoke_result_t<DecFunc, int8_t, int8_t>>, int8_t, int8_t>;
			using NodeImpl = detail::RttiFuncNode_impl<FuncA, Container, TypeList<std::decay_t<decltype(node1_)>, std::decay_t<Node2>>>;
			eval::RttiFuncNode<Container> res;
			res.template Construct<NodeImpl>(std::forward<Func>(f), std::move(node1_), std::forward<Node2>(node2));
			return res;
		}
		else if (right_type != I08)
		{
			auto node2_ = MakeRttiFuncNode_cast_bool(std::forward<Node2>(node2));
			using FuncA = FuncDefinition<DecFunc, std::decay_t<std::invoke_result_t<DecFunc, int8_t, int8_t>>, int8_t, int8_t>;
			using NodeImpl = detail::RttiFuncNode_impl<FuncA, Container, TypeList<std::decay_t<Node1>, std::decay_t<decltype(node2_)>>>;
			eval::RttiFuncNode<Container> res;
			res.template Construct<NodeImpl>(std::forward<Func>(f), std::forward<Node1>(node1), std::move(node2_));
			return res;
		}
		else
		{
			using FuncA = FuncDefinition<DecFunc, std::decay_t<std::invoke_result_t<DecFunc, int8_t, int8_t>>, int8_t, int8_t>;
			using NodeImpl = detail::RttiFuncNode_impl<FuncA, Container, TypeList<std::decay_t<Node1>, std::decay_t<Node2>>>;
			eval::RttiFuncNode<Container> res;
			res.template Construct<NodeImpl>(std::forward<Func>(f), std::forward<Node1>(node1), std::forward<Node2>(node2));
			return res;
		}
	}
}

ADAPT_EXPORT
template <class Func>
concept flagged_with_arithmetic_conversion = requires(Func)
{
	{ std::bool_constant<std::decay_t<Func>::Level != ArithmeticConvLevel::None>{} } -> std::same_as<std::true_type>;
};

template <class Container, class Func, any_node Node1, any_node Node2>
	requires (!flagged_with_arithmetic_conversion<Func>)
auto MakeRttiFuncNode(Func&& f, Node1&& node1, Node2&& node2)
{
	using enum FieldType;
	using DecFunc = std::decay_t<Func>;
	#define CODE(TAGTYPE1, SYM1, VTYPE1, TAGTYPE2, SYM2, VTYPE2)\
	if constexpr (requires { f(std::declval<const VTYPE1&>(), std::declval<const VTYPE2&>()); })\
	if (node1.GetType() == TAGTYPE1 && node2.GetType() == TAGTYPE2)\
	{\
		using FuncA = FuncDefinition<DecFunc, std::decay_t<std::invoke_result_t<DecFunc, VTYPE1, VTYPE2>>, VTYPE1, VTYPE2>;\
		using NodeImpl = detail::RttiFuncNode_impl<FuncA, Container, TypeList<std::decay_t<Node1>, std::decay_t<Node2>>>;\
		eval::RttiFuncNode<Container> res;\
		res.template Construct<NodeImpl>(std::forward<Func>(f), std::forward<Node1>(node1), std::forward<Node2>(node2));\
		return res;\
	}
	ADAPT_FOR_EACH_TYPE_PROD(CODE)
	#undef CODE
	throw MismatchType("");
}
template <class Container, class Func, any_node Node1, any_node Node2>
	requires (std::decay_t<Func>::Level == ArithmeticConvLevel::Promo)
auto MakeRttiFuncNode(Func&& f, Node1&& node1, Node2&& node2)
{
	// 両引数が整数の場合は整数昇格を行う。非整数が混ざる場合は何もしない。
	using enum FieldType;
	FieldType type1 = node1.GetType();
	FieldType type2 = node2.GetType();
	if (DFieldInfo::IsInt(type1) && DFieldInfo::IsInt(type2))
	{
		return IntegralPromotion<Container>(std::forward<Func>(f), std::forward<Node1>(node1), std::forward<Node2>(node2));
	}
	else
	{
		#define CODE(TAGTYPE1, SYM1, VTYPE1, TAGTYPE2, SYM2, VTYPE2)\
		if constexpr ((!DFieldInfo::IsInt(TAGTYPE1) || !DFieldInfo::IsInt(TAGTYPE2)) &&\
					  requires { f(std::declval<const VTYPE1&>(), std::declval<const VTYPE2&>()); })\
			return MakeRttiFuncNode_construct<Container, type1, type2>(std::forward<Func>(f),\
																	   std::forward<Node1>(node1),\
																	   std::forward<Node2>(node2));
		ADAPT_FOR_EACH_TYPE_PROD(CODE)
		#undef CODE
		throw MismatchType("");
	}
}
template <class Container, class Func, any_node Node1, any_node Node2>
	requires (std::decay_t<Func>::Level == ArithmeticConvLevel::Integ)
auto MakeRttiFuncNode(Func&& f, Node1&& node1, Node2&& node2)
{
	// 整数の算術変換を行う。非整数が混ざる場合は何もしない。
	// 両引数が整数の場合は整数昇格を行う。非整数が混ざる場合は何もしない。
	using enum FieldType;
	FieldType type1 = node1.GetType();
	FieldType type2 = node2.GetType();
	if (DFieldInfo::IsInt(type1) && DFieldInfo::IsInt(type2))
	{
		return IntegralArithmeticConversion<Container>(std::forward<Func>(f), std::forward<Node1>(node1), std::forward<Node2>(node2));
	}
	else
	{
		#define CODE(TAGTYPE1, SYM1, VTYPE1, TAGTYPE2, SYM2, VTYPE2)\
		if constexpr ((!DFieldInfo::IsInt(TAGTYPE1) || !DFieldInfo::IsInt(TAGTYPE2)) &&\
					  requires { f(std::declval<const VTYPE1&>(), std::declval<const VTYPE2&>()); })\
		return MakeRttiFuncNode_construct<Container, type1, type2>(std::forward<Func>(f), \
																   std::forward<Node1>(node1), \
																   std::forward<Node2>(node2));
		ADAPT_FOR_EACH_TYPE_PROD(CODE)
		#undef CODE
		throw MismatchType("");
	}
}
template <class Container, class Func, any_node Node1, any_node Node2>
	requires (std::decay_t<Func>::Level == ArithmeticConvLevel::Usual)
auto MakeRttiFuncNode(Func&& f, Node1&& node1, Node2&& node2)
{
	// 通常の算術変換を行う。非数値が混ざる場合は何もしない。
	using enum FieldType;
	FieldType type1 = node1.GetType();
	FieldType type2 = node2.GetType();
	if (DFieldInfo::IsArithmetic(type1) && DFieldInfo::IsArithmetic(type2))
	{
		return UsualArithmeticConversion<Container>(std::forward<Func>(f), std::forward<Node1>(node1), std::forward<Node2>(node2));
	}
	else
	{
		#define CODE(TAGTYPE1, SYM1, VTYPE1, TAGTYPE2, SYM2, VTYPE2)\
		if constexpr ((!DFieldInfo::IsArithmetic(TAGTYPE1) || !DFieldInfo::IsArithmetic(TAGTYPE2)) &&\
					  requires { f(std::declval<const VTYPE1&>(), std::declval<const VTYPE2&>()); })\
			return MakeRttiFuncNode_construct<Container, TAGTYPE1, TAGTYPE2>(std::forward<Func>(f), \
																			 std::forward<Node1>(node1), \
																			 std::forward<Node2>(node2)); else
		ADAPT_FOR_EACH_TYPE_PROD(CODE)
		#undef CODE
		throw MismatchType("");
	}
}
template <class Container, class Func, any_node Node1, any_node Node2>
	requires (std::decay_t<Func>::Level == ArithmeticConvLevel::Compl)
auto MakeRttiFuncNode(Func&& f, Node1&& node1, Node2&& node2)
{
	// 通常の算術変換を行う。非数値が混ざる場合は何もしない。
	using enum FieldType;
	FieldType type1 = node1.GetType();
	FieldType type2 = node2.GetType();
	if ((DFieldInfo::IsCpxAri(type1) || DFieldInfo::IsCpxAri(type2)))
	{
		return ComplexArithmeticConversion<Container>(std::forward<Func>(f), std::forward<Node1>(node1), std::forward<Node2>(node2));
	}
	else
	{
		#define CODE(TAGTYPE1, SYM1, VTYPE1, TAGTYPE2, SYM2, VTYPE2)\
		if constexpr ((!DFieldInfo::IsCpxAri(TAGTYPE1) || !DFieldInfo::IsCpxAri(TAGTYPE2)) &&\
					  requires { f(std::declval<const VTYPE1&>(), std::declval<const VTYPE2&>()); })\
			return MakeRttiFuncNode_construct<Container, TAGTYPE1, TAGTYPE2>(std::forward<Func>(f), \
																			 std::forward<Node1>(node1), \
																			 std::forward<Node2>(node2)); else
		ADAPT_FOR_EACH_TYPE_PROD(CODE)
		#undef CODE
		throw MismatchType("");
	}
}
/*template <class Container, class Func, any_node Node1, any_node Node2, any_node Node3>
auto MakeRttiFuncNode(Func&& f, Node1&& node1, Node2&& node2, Node3&& node3)
{
	#define CODE(TAGTYPE1, SYM1, VTYPE1, TAGTYPE2, SYM2, VTYPE2, TAGTYPE3, SYM3, VTYPE3)\
	if constexpr (requires { f(std::declval<const VTYPE1&>(), std::declval<const VTYPE2&>(), std::declval<const VTYPE3&>()); })\
	if (node1.GetType() == FieldType::TAGTYPE1 && node2.GetType() == FieldType::TAGTYPE2 && node3.GetType() == FieldType::TAGTYPE3)\
	{\
		using DecFunc = std::decay_t<Func>;\
		using FuncA = FuncDefinition<DecFunc, std::decay_t<std::invoke_result_t<DecFunc, VTYPE1, VTYPE2, VTYPE3>>, VTYPE1, VTYPE2, VTYPE3>;\
		using NodeImpl = detail::RttiFuncNode_impl<FuncA, Container, TypeList<std::decay_t<Node1>, std::decay_t<Node2>, std::decay_t<Node3>>>;\
		eval::RttiFuncNode<Container> res;\
		res.template Construct<NodeImpl>(std::forward<Func>(f), std::forward<Node1>(node1), std::forward<Node2>(node2), std::forward<Node3>(node3));\
		return res;\
	}
	ADAPT_FIELD_TYPE_LIST_TRIO(CODE)
	#undef CODE
	throw MismatchType("");
}*/

template <class Container, class Func, any_node Node, any_node ...Nodes>
	requires (sizeof...(Nodes) >= 2)
auto MakeRttiFuncNode(Func&& f, Node&& node, Nodes&& ...nodes)
{
	using enum FieldType;
	#define CODE(TAGTYPE1, SYM1, VTYPE1, TAGTYPE2, SYM2, VTYPE2)\
	if constexpr (requires { f(std::declval<const VTYPE1&>(), std::declval<const Former<VTYPE2, Nodes>&>()...); })\
	if (node.GetType() == FieldType::TAGTYPE1 && ((nodes.GetType() == FieldType::TAGTYPE2) && ...))\
	{\
		using DecFunc = std::decay_t<Func>;\
		using FuncA = FuncDefinition<DecFunc, std::decay_t<std::invoke_result_t<DecFunc, VTYPE1, Former<VTYPE2, Nodes>...>>, VTYPE1, Former<VTYPE2, Nodes>...>;\
		using NodeImpl = detail::RttiFuncNode_impl<FuncA, Container, TypeList<std::decay_t<Node>, std::decay_t<Nodes>...>>;\
		eval::RttiFuncNode<Container> res;\
		res.template Construct<NodeImpl>(std::forward<Func>(f), std::forward<Node>(node), std::forward<Nodes>(nodes)...);\
		return res;\
	}
	ADAPT_FOR_EACH_TYPE_PROD(CODE)
	#undef CODE
	throw MismatchType("");
}

template <class Func, class ...NPs>
auto MakeFunctionNode(Func&& f, NPs&& ...nps)
{
	constexpr bool has_rtti_type = (rtti_node_or_placeholder<NPs> || ...);
	using Container = typename ExtractContainer<std::decay_t<NPs>...>::Container;

	if constexpr (has_rtti_type)
	{
		return MakeRttiFuncNode<Container>(std::forward<Func>(f), ConvertToNode(std::forward<NPs>(nps), std::true_type{})...);
	}
	else
	{
		using RetType = std::invoke_result_t<std::decay_t<Func>, typename GetNodeType_t<std::decay_t<NPs>>::RetType...>;
		using FuncType = FuncDefinition<std::decay_t<Func>, RetType, typename GetNodeType_t<std::decay_t<NPs>>::RetType...>;
		return eval::CttiFuncNode<FuncType, TypeList<GetNodeType_t<std::decay_t<NPs>>...>>
			(std::forward<Func>(f), ConvertToNode(std::forward<NPs>(nps), std::false_type{})...);
	}
}

}

//任意のノードを強制的にRttiFuncNodeでラップする。
//ただし、既にRttiFuncNodeであれば何もせず引数をそのまま返す。
ADAPT_EXPORT
template <node_or_placeholder NPs>
auto ConvertToRttiFuncNode(NPs&& nps)
{
	using Container = typename detail::ExtractContainer<std::decay_t<NPs>>::Container;
	if constexpr (rtti_func_node<NPs>) return std::forward<NPs>(nps);
	else
	{
		auto f = [](const auto& a) { return a; };
		return detail::MakeRttiFuncNode<Container>(f, detail::ConvertToNode(std::forward<NPs>(nps), std::true_type{}));
	}
}

ADAPT_EXPORT
template <class Container>
RttiFuncNode<Container> ConvertConstNodeToRttiFuncNode(RttiConstNode&& c)
{
	return detail::MakeRttiFuncNode<Container>([](const auto& a) { return a; }, std::move(c));
}
ADAPT_EXPORT
template <class Container>
RttiFuncNode<Container> ConvertConstNodeToRttiFuncNode(const RttiConstNode& c)
{
	return detail::MakeRttiFuncNode<Container>([](const auto& a) { return a; }, c);
}


}

}

#endif
