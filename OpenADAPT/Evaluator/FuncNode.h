#ifndef ADAPT_EVALUATOR_FUNC_NODE_H
#define ADAPT_EVALUATOR_FUNC_NODE_H

#include <typeindex>
#include <memory>
#include <algorithm>
#include <OpenADAPT/Utility/TypeTraits.h>
#include <OpenADAPT/Joint/LayerInfo.h>
#include <OpenADAPT/Evaluator/Function.h>
#include <OpenADAPT/Evaluator/NodeBase.h>
#include <OpenADAPT/Traverser/ExternalTraverser.h>

namespace adapt
{

namespace eval
{


//----------CttiFuncNode----------

namespace detail
{

template <class RetType, bool Trivial = std::is_trivially_copyable_v<RetType>>
struct CttiFuncNodeBuffer
{};
template <class RetType>
struct CttiFuncNodeBuffer<RetType, false>
{
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

	CttiFuncNode() = default;
	template <any_node ...Nodes_>
		requires (sizeof...(Nodes) == sizeof...(Nodes_)) && (sizeof...(Nodes) > 1)
	CttiFuncNode(Nodes_&& ...nodes)
		: m_nodes(std::forward<Nodes_>(nodes)...)
	{}
	template <any_node Node_>
		requires (!std::is_same_v<std::remove_cvref_t<Node_>, CttiFuncNode>)//コピー、ムーブコンストラクタと曖昧にならないように。
	CttiFuncNode(Node_&& node)
		: m_nodes(std::forward<Node_>(node))
	{}

	auto IncreaseDepth() const&
	{
		using Nodes_ = TypeList<std::remove_cvref_t<decltype(std::get<Indices>(m_nodes).IncreaseDepth())>...>;
		using Ret = CttiFuncNode<Func, Nodes_, Container, std::index_sequence<Indices...>>;
		return Ret(std::get<Indices>(m_nodes).IncreaseDepth()...);
	}
	auto IncreaseDepth()&&
	{
		using Nodes_ = TypeList<std::remove_cvref_t<decltype(std::get<Indices>(m_nodes).IncreaseDepth())>...>;
		using Ret = CttiFuncNode<Func, Nodes_, Container, std::index_sequence<Indices...>>;
		return Ret(std::get<Indices>(std::move(m_nodes)).IncreaseDepth()...);
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

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4702)
#endif
	RetType Evaluate(const Traverser& t) const
		requires std::is_trivially_copyable_v<RetType>
	{
		return Func::Exec(std::get<Indices>(m_nodes).Evaluate(t)...);
	}
	RetType Evaluate(const ConstTraverser& t) const
		requires std::is_trivially_copyable_v<RetType>
	{
		return Func::Exec(std::get<Indices>(m_nodes).Evaluate(t)...);
	}
	const RetType& Evaluate(const Traverser& t) const
		requires (!std::is_trivially_copyable_v<RetType>)
	{
		Func::Exec(std::get<Indices>(m_nodes).Evaluate(t)..., this->m_buffer);
		return this->m_buffer;
	}
	const RetType& Evaluate(const ConstTraverser& t) const
		requires (!std::is_trivially_copyable_v<RetType>)
	{
		Func::Exec(std::get<Indices>(m_nodes).Evaluate(t)..., this->m_buffer);
		return this->m_buffer;
	}

	RetType Evaluate(const Container& s) const
		requires std::is_trivially_copyable_v<RetType>
	{
		return Func::Exec(std::get<Indices>(m_nodes).Evaluate(s)...);
	}
	RetType Evaluate(const Container& s, const Bpos& bpos) const
		requires std::is_trivially_copyable_v<RetType>
	{
		return Func::Exec(std::get<Indices>(m_nodes).Evaluate(s, bpos)...);
	}
	const RetType& Evaluate(const Container& s) const
		requires (!std::is_trivially_copyable_v<RetType>)
	{
		Func::Exec(std::get<Indices>(m_nodes).Evaluate(s)..., this->m_buffer);
		return this->m_buffer;
	}
	const RetType& Evaluate(const Container& s, const Bpos& bpos) const
		requires (!std::is_trivially_copyable_v<RetType>)
	{
		Func::Exec(std::get<Indices>(m_nodes).Evaluate(s, bpos)..., this->m_buffer);
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
	: public RttiFuncNode_body<Container, TypeList<Nodes...>, std::index_sequence<Indices...>>
{
	static_assert(DFieldInfo::IsInt(Type) || DFieldInfo::IsFlt(Type) || DFieldInfo::IsCpx(Type));
	using Base = RttiFuncNode_body<Container, TypeList<Nodes...>, std::index_sequence<Indices...>>;
	using Traverser = Container::Traverser;
	using ConstTraverser = Container::ConstTraverser;
	template <size_t Index>
	using ArgType = Func::template ArgType<Index>;
	using Base::Base;

	virtual ~RttiFuncNode_impl() = default;
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
		return Func::Exec(this->template GetArg<Indices, DFieldInfo::ValueTypeToTagType<ArgType<Indices>>()>(t)...);
	}
	virtual DFieldInfo::TagTypeToValueType<Type>
		Evaluate(const ConstTraverser& t, Number<Type>) const override
	{
		return Func::Exec(this->template GetArg<Indices, DFieldInfo::ValueTypeToTagType<ArgType<Indices>>()>(t)...);
	}
	virtual DFieldInfo::TagTypeToValueType<Type>
		Evaluate(const Container& s, Number<Type>) const override
	{
		return Func::Exec(this->template GetArg<Indices, DFieldInfo::ValueTypeToTagType<ArgType<Indices>>()>(s)...);
	}
	virtual DFieldInfo::TagTypeToValueType<Type>
		Evaluate(const Container& s, const Bpos& bpos, Number<Type>) const override
	{
		return Func::Exec(this->template GetArg<Indices, DFieldInfo::ValueTypeToTagType<ArgType<Indices>>()>(s, bpos)...);
	}
#ifdef _MSC_VER
#pragma warning(pop)
#endif
	virtual FieldType GetType() const override { return Type; }
};

template <class Func, class Container, class ...Nodes, FieldType Type, size_t ...Indices>
	requires (!DFieldInfo::IsTrivial(Type))
struct RttiFuncNode_impl<Func, Container, TypeList<Nodes...>, Type, std::index_sequence<Indices...>>
	: public RttiFuncNode_body<Container, TypeList<Nodes...>, std::index_sequence<Indices...>>
{
	//m_bufをメンバとして保つ必要があるため、trivialの方と統合できない。
	static_assert(DFieldInfo::IsStr(Type) || DFieldInfo::IsJbp(Type));
	using Base = RttiFuncNode_body<Container, TypeList<Nodes...>, std::index_sequence<Indices...>>;
	using Traverser = Container::Traverser;
	using ConstTraverser = Container::ConstTraverser;
	template <size_t Index>
	using ArgType = Func::template ArgType<Index>;
	using Base::Base;

	virtual ~RttiFuncNode_impl() = default;
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
	virtual const DFieldInfo::TagTypeToValueType<Type>&
		Evaluate(const Traverser& t, Number<Type>) const override
	{
		Func::Exec(this->template GetArg<Indices, DFieldInfo::ValueTypeToTagType<ArgType<Indices>>()>(t)..., m_buf);
		return m_buf;
	}
	virtual const DFieldInfo::TagTypeToValueType<Type>&
		Evaluate(const ConstTraverser& t, Number<Type>) const override
	{
		Func::Exec(this->template GetArg<Indices, DFieldInfo::ValueTypeToTagType<ArgType<Indices>>()>(t)..., m_buf);
		return m_buf;
	}
	virtual const DFieldInfo::TagTypeToValueType<Type>&
		Evaluate(const Container& s, Number<Type>) const override
	{
		Func::Exec(this->template GetArg<Indices, DFieldInfo::ValueTypeToTagType<ArgType<Indices>>()>(s)..., m_buf);
		return m_buf;
	}
	virtual const DFieldInfo::TagTypeToValueType<Type>&
		Evaluate(const Container& s, const Bpos& bpos, Number<Type>) const override
	{
		Func::Exec(this->template GetArg<Indices, DFieldInfo::ValueTypeToTagType<ArgType<Indices>>()>(s, bpos)..., m_buf);
		return m_buf;
	}
#ifdef _MSC_VER
#pragma warning(pop)
#endif
	virtual FieldType GetType() const override { return Type; }
	mutable DFieldInfo::TagTypeToValueType<Type> m_buf;
};

}


namespace detail
{

template <template <class...> class Func, class ...ArgTypes, any_node ...Nodes,
	class FuncA = Func<ArgTypes...>, class Container = typename ExtractContainer<std::decay_t<Nodes>...>::Container>
auto MakeRttiFuncNode_construct(int, Nodes&& ...n)
	-> eval::RttiFuncNode<Container>
{
	//FuncはArgTypes全てに対して適用可能なものしか定義されていない。
	//例えばPlus<int64_t, std::string>はint64_t + stringが不可能なので未定義。
	//もしFunc<ArgTypes...>が未定義であれば、FuncAの推定に失敗するので、この関数は呼ばれず、下の例外を投げるほうが呼ばれる。
	using NodeImpl = detail::RttiFuncNode_impl<FuncA, Container, TypeList<std::decay_t<Nodes>...>>;
	eval::RttiFuncNode<Container> res;
	res.template Construct<NodeImpl>(std::forward<Nodes>(n)...);
	return res;
}
template <template <class...> class Func, class ...ArgTypes, any_node ...Nodes>
auto MakeRttiFuncNode_construct(float, Nodes&& ...)
	-> eval::RttiFuncNode<typename ExtractContainer<std::decay_t<Nodes>...>::Container>
{
	throw MismatchType("");
}

template <template <class...> class Func, FieldType ...Types>
struct MakeRttiFuncNode_convert
{
	template <any_node ...Nodes>
	auto operator()(Nodes&& ...n) const
	{
		return MakeRttiFuncNode_construct<Func, DFieldInfo::TagTypeToValueType<Types>...>(1, std::forward<Nodes>(n)...);
	}
};

template <template <class...> class Func, FieldType ...Types, any_node ...Nodes>
auto MakeRttiFuncNode(ValueList<Types...>, std::tuple<Nodes...> t)
{
	return std::apply(MakeRttiFuncNode_convert<Func, Types...>(), std::move(t));
}

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4702)
#endif
template <template <class...> class Func, FieldType ...Types, any_node ...Nodes, any_node Head, any_node ...Body>
auto MakeRttiFuncNode(ValueList<Types...>, std::tuple<Nodes...> t, Head&& head, Body&& ...body)
{
	if (head.IsI08())
		return MakeRttiFuncNode<Func>(ValueList<Types..., FieldType::I08>(),
									  TupleAdd(std::move(t), std::forward<Head>(head)),
									  std::forward<Body>(body)...);
	else if (head.IsI16())
		return MakeRttiFuncNode<Func>(ValueList<Types..., FieldType::I16>(),
									  TupleAdd(std::move(t), std::forward<Head>(head)),
									  std::forward<Body>(body)...);
	else if (head.IsI32())
		return MakeRttiFuncNode<Func>(ValueList<Types..., FieldType::I32>(),
									  TupleAdd(std::move(t), std::forward<Head>(head)),
									  std::forward<Body>(body)...);
	else if (head.IsI64())
		return MakeRttiFuncNode<Func>(ValueList<Types..., FieldType::I64>(),
									  TupleAdd(std::move(t), std::forward<Head>(head)),
									  std::forward<Body>(body)...);
	else if (head.IsF32())
		return MakeRttiFuncNode<Func>(ValueList<Types..., FieldType::F32>(),
									  TupleAdd(std::move(t), std::forward<Head>(head)),
									  std::forward<Body>(body)...);
	else if (head.IsF64())
		return MakeRttiFuncNode<Func>(ValueList<Types..., FieldType::F64>(),
									  TupleAdd(std::move(t), std::forward<Head>(head)),
									  std::forward<Body>(body)...);
	else if (head.IsC32())
		return MakeRttiFuncNode<Func>(ValueList<Types..., FieldType::C32>(),
									  TupleAdd(std::move(t), std::forward<Head>(head)),
									  std::forward<Body>(body)...);
	else if (head.IsC64())
		return MakeRttiFuncNode<Func>(ValueList<Types..., FieldType::C64>(),
									  TupleAdd(std::move(t), std::forward<Head>(head)),
									  std::forward<Body>(body)...);
	else if (head.IsStr())
		return MakeRttiFuncNode<Func>(ValueList<Types..., FieldType::Str>(),
									  TupleAdd(std::move(t), std::forward<Head>(head)),
									  std::forward<Body>(body)...);
	else if (head.IsJbp())
		return MakeRttiFuncNode<Func>(ValueList<Types..., FieldType::Jbp>(),
									  TupleAdd(std::move(t), std::forward<Head>(head)),
									  std::forward<Body>(body)...);
	throw MismatchType("");
}
#ifdef _MSC_VER
#pragma warning(pop)
#endif

template <template <class...> class Func, class ...NPs>
auto MakeFunctionNode(NPs&& ...nps)
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
		return MakeRttiFuncNode<Func>(ValueList<>(), std::tuple<>(),
									  ConvertToNode(std::forward<NPs>(nps), std::true_type{})...);
	}
	else
	{
		using FuncType = Func<typename GetNodeType<std::decay_t<NPs>>::Type::RetType...>;
		return eval::CttiFuncNode<FuncType, TypeList<typename GetNodeType<std::decay_t<NPs>>::Type...>>
		{ ConvertToNode(std::forward<NPs>(nps), std::false_type{})... };
	}
}

template <template <class...> class Func, neither_node_nor_placeholder Constant,
	FieldType Type = DFieldInfo::GetSameSizeTagType<std::remove_cvref_t<Constant>>()>
	requires (Type != FieldType::Emp)
auto MakeRttiFuncNodeFromConstant(Constant&& c)
{
	return MakeRttiFuncNode<Func>(ValueList<>(), std::tuple<>(), RttiConstNode(std::forward<Constant>(c)));
}

}

//任意のノードを強制的にRttiFuncNodeでラップする。
//ただし、既にRttiFuncNodeであれば何もしなくて良い。
template <node_or_placeholder NPs>
auto ConvertToRttiFuncNode(NPs&& nps)
{
	if constexpr (rtti_func_node<NPs>) return std::forward<NPs>(nps);
	else return MakeRttiFuncNode<detail::Forward>(ValueList<>(), std::tuple<>(),
		ConvertToNode(std::forward<NPs>(nps), std::true_type{}));
}

}

}

#endif
