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
	void Init_impl(Args&& ...args)
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

private:
	template <size_t N>
	static constexpr JointLayerArray<MaxRank> GetJointLayerArray_impl()
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

	static constexpr JointLayerArray<MaxRank> GetJointLayerArray()
	{
		return GetJointLayerArray_impl<0>();
	}
	static constexpr LayerInfo<MaxRank> GetLayerInfo()
	{
		return GetLayerInfo(LayerInfo<MaxRank>(GetJointLayerArray()));
	}
	static constexpr LayerInfo<MaxRank> GetLayerInfo(LayerInfo<MaxRank> eli)
	{
		return std::max({ GetType_t<Indices, Nodes...>::GetLayerInfo(eli)... });
	}
	static constexpr LayerInfo<MaxRank> GetLayerInfo(LayerInfo<MaxRank> eli, DepthType depth)
	{
		return std::max({ GetType_t<Indices, Nodes...>::GetLayerInfo(eli, depth)... });
	}
	static constexpr LayerType GetLayer()
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
		m_func.Exec(this->m_buffer, *this, t);
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
#pragma warning(disable: 4702)
#endif
	virtual DFieldInfo::TagTypeToValueType<Type>
		Evaluate(const Traverser& t, Number<Type>) const override
	{
		return m_func.Exec(*this, t);
	}
	virtual DFieldInfo::TagTypeToValueType<Type>
		Evaluate(const ConstTraverser& t, Number<Type>) const override
	{
		return m_func.Exec(*this, t);
	}
	virtual DFieldInfo::TagTypeToValueType<Type>
		Evaluate(const Container& s, Number<Type>) const override
	{
		return m_func.Exec(*this, s);
	}
	virtual DFieldInfo::TagTypeToValueType<Type>
		Evaluate(const Container& s, const Bpos& bpos, Number<Type>) const override
	{
		return m_func.Exec(*this, s, bpos);
	}
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
	virtual const DFieldInfo::TagTypeToValueType<Type>&
		Evaluate(const Traverser& t, Number<Type>) const override
	{
		m_func.ExecWithBuf(m_buf, *this, t);
		return m_buf;
	}
	virtual const DFieldInfo::TagTypeToValueType<Type>&
		Evaluate(const ConstTraverser& t, Number<Type>) const override
	{
		m_func.ExecWithBuf(m_buf, *this, t);
		return m_buf;
	}
	virtual const DFieldInfo::TagTypeToValueType<Type>&
		Evaluate(const Container& s, Number<Type>) const override
	{
		m_func.ExecWithBuf(m_buf, *this, s);
		return m_buf;
	}
	virtual const DFieldInfo::TagTypeToValueType<Type>&
		Evaluate(const Container& s, const Bpos& bpos, Number<Type>) const override
	{
		m_func.ExecWithBuf(m_buf, *this, s, bpos);
		return m_buf;
	}
#ifdef _MSC_VER
#pragma warning(pop)
#endif
	virtual FieldType GetType() const override { return Type; }

	[[no_unique_address]] Func m_func;
	mutable DFieldInfo::TagTypeToValueType<Type> m_buf;
};

}


namespace detail
{

template <class ...ArgTypes, class Func, any_node ...Nodes,
	class Container = typename ExtractContainer<std::decay_t<Nodes>...>::Container>
auto MakeRttiFuncNode_construct(int, Func&& f, Nodes&& ...n)
	-> decltype(f(std::declval<ArgTypes>()...), eval::RttiFuncNode<Container>{})
{
	using DecFunc = std::decay_t<Func>;
	using RetType = std::decay_t<std::invoke_result_t<DecFunc, ArgTypes...>>;
	using FuncA = FuncDefinition<DecFunc, RetType, ArgTypes...>;
	using NodeImpl = detail::RttiFuncNode_impl<FuncA, Container, TypeList<std::decay_t<Nodes>...>>;
	eval::RttiFuncNode<Container> res;
	res.template Construct<NodeImpl>(std::forward<Func>(f), std::forward<Nodes>(n)...);
	return res;
}
template <class ...ArgTypes, class Func, any_node ...Nodes>
auto MakeRttiFuncNode_construct(float, Func&&, Nodes&& ...)
	-> eval::RttiFuncNode<typename ExtractContainer<std::decay_t<Nodes>...>::Container>
{
	throw MismatchType("");
}

template <class Func, FieldType ...Types, any_node ...Nodes, size_t ...Indices>
auto MakeRttiFuncNode_expand(Func&& f, ValueList<Types...>, std::tuple<Nodes...> t, std::index_sequence<Indices...>)
{
	return MakeRttiFuncNode_construct<DFieldInfo::TagTypeToValueType<Types>...>(1, std::forward<Func>(f), std::get<Indices>(t)...);
}

template <class Func, FieldType ...Types, any_node ...Nodes>
auto MakeRttiFuncNode(Func&& f, ValueList<Types...> v, std::tuple<Nodes...> t)
{
	return MakeRttiFuncNode_expand(std::forward<Func>(f), v, std::move(t), std::make_index_sequence<sizeof...(Nodes)>{});
}

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4702)
#endif
template <class Func, FieldType ...Types, any_node ...Nodes, any_node Head, any_node ...Body>
auto MakeRttiFuncNode(Func&& f, ValueList<Types...>, std::tuple<Nodes...> t, Head&& head, Body&& ...body)
{
	if (head.IsI08())
		return MakeRttiFuncNode(std::forward<Func>(f), ValueList<Types..., FieldType::I08>(),
			TupleAdd(std::move(t), std::forward<Head>(head)),
			std::forward<Body>(body)...);
	else if (head.IsI16())
		return MakeRttiFuncNode(std::forward<Func>(f), ValueList<Types..., FieldType::I16>(),
			TupleAdd(std::move(t), std::forward<Head>(head)),
			std::forward<Body>(body)...);
	else if (head.IsI32())
		return MakeRttiFuncNode(std::forward<Func>(f), ValueList<Types..., FieldType::I32>(),
			TupleAdd(std::move(t), std::forward<Head>(head)),
			std::forward<Body>(body)...);
	else if (head.IsI64())
		return MakeRttiFuncNode(std::forward<Func>(f), ValueList<Types..., FieldType::I64>(),
			TupleAdd(std::move(t), std::forward<Head>(head)),
			std::forward<Body>(body)...);
	else if (head.IsF32())
		return MakeRttiFuncNode(std::forward<Func>(f), ValueList<Types..., FieldType::F32>(),
			TupleAdd(std::move(t), std::forward<Head>(head)),
			std::forward<Body>(body)...);
	else if (head.IsF64())
		return MakeRttiFuncNode(std::forward<Func>(f), ValueList<Types..., FieldType::F64>(),
			TupleAdd(std::move(t), std::forward<Head>(head)),
			std::forward<Body>(body)...);
	else if (head.IsC32())
		return MakeRttiFuncNode(std::forward<Func>(f), ValueList<Types..., FieldType::C32>(),
			TupleAdd(std::move(t), std::forward<Head>(head)),
			std::forward<Body>(body)...);
	else if (head.IsC64())
		return MakeRttiFuncNode(std::forward<Func>(f), ValueList<Types..., FieldType::C64>(),
			TupleAdd(std::move(t), std::forward<Head>(head)),
			std::forward<Body>(body)...);
	else if (head.IsStr())
		return MakeRttiFuncNode(std::forward<Func>(f), ValueList<Types..., FieldType::Str>(),
			TupleAdd(std::move(t), std::forward<Head>(head)),
			std::forward<Body>(body)...);
	else if (head.IsJbp())
		return MakeRttiFuncNode(std::forward<Func>(f), ValueList<Types..., FieldType::Jbp>(),
			TupleAdd(std::move(t), std::forward<Head>(head)),
			std::forward<Body>(body)...);
	throw MismatchType("");
}
#ifdef _MSC_VER
#pragma warning(pop)
#endif

template <class Func, class ...NPs>
auto MakeFunctionNode(Func&& f, NPs&& ...nps)
{
	/*
	1. 全てのノード構成要素がCttiNodeである場合
	Cttiならノードは単にCttiFuncNodeとする。

	2. RttiNodeまたはRttiNodeTmpを1つでも含む場合
	Rttiの場合、文字列式から認識させている場合も考慮し、RttiFuncNodeTmpへと情報を格納する。
	その後Traverserを与えての事前処理時にRttiFuncNodeへと構造を修正する。

	3. RttiとCttiが混在している場合
	RttiFuncNodeとする。

	*/
	//DCttiPlaceholderはCttiに分類される。
	constexpr bool has_rtti_type = (rtti_node_or_placeholder<NPs> || ...);

	if constexpr (has_rtti_type)
	{
		return MakeRttiFuncNode(std::forward<Func>(f), ValueList<>(), std::tuple<>(),
									  ConvertToNode(std::forward<NPs>(nps), std::true_type{})...);
	}
	else
	{
		using RetType = std::invoke_result_t<std::decay_t<Func>, typename GetNodeType<std::decay_t<NPs>>::Type::RetType...>;
		using FuncType = FuncDefinition<std::decay_t<Func>, RetType, typename GetNodeType<std::decay_t<NPs>>::Type::RetType...>;
		return eval::CttiFuncNode<FuncType, TypeList<typename GetNodeType<std::decay_t<NPs>>::Type...>>
		(std::forward<Func>(f), ConvertToNode(std::forward<NPs>(nps), std::false_type{})...);
	}
}

template <class Func, neither_node_nor_placeholder Constant,
	FieldType Type = DFieldInfo::GetSameSizeTagType<std::remove_cvref_t<Constant>>()>
	requires (Type != FieldType::Emp)
auto MakeRttiFuncNodeFromConstant(Func&& f, Constant&& c)
{
	return MakeRttiFuncNode(std::forward<Func>(f), ValueList<>(), std::tuple<>(), RttiConstNode(std::forward<Constant>(c)));
}

}

//任意のノードを強制的にRttiFuncNodeでラップする。
//ただし、既にRttiFuncNodeであれば何もしなくて良い。
template <node_or_placeholder NPs>
auto ConvertToRttiFuncNode(NPs&& nps)
{
	if constexpr (rtti_func_node<NPs>) return std::forward<NPs>(nps);
	else
	{
		auto f = [](const auto& a) { return a; };
		return detail::MakeRttiFuncNode(f, ValueList<>(), std::tuple<>(),
			ConvertToNode(std::forward<NPs>(nps), std::true_type{}));
	}
}

}

}

#endif
