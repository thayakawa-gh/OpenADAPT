#ifndef ADAPT_EVALUATOR_NODEBASE_H
#define ADAPT_EVALUATOR_NODEBASE_H

#include <algorithm>
#include <functional>
#include <OpenADAPT/Utility/Verbose.h>
#include <OpenADAPT/Common/Concepts.h>

namespace adapt
{

namespace eval
{

namespace detail
{

//template <node Node1>
//const Node1& ConvertToNode(const Node1& node) { return node; }
template <any_node Node1>
Node1&& ConvertToNode(Node1&& node) { return std::forward<Node1>(node); }
template <typed_placeholder PH>
auto ConvertToNode(PH ph) { return eval::CttiFieldNode{ ph }; }
template <rtti_placeholder PH>
auto ConvertToNode(PH ph) { return eval::RttiFieldNode{ ph }; }

//ConvertToNodeには定数が与えられる場合があり、そのときはcttiとrttiの区別のためにstd::bool_constantを与えている。
//Node/Placeholderの場合には不要だが、同一の関数定義にするために受け取っている。
template <bool Rtti, any_node Node1>
Node1&& ConvertToNode(Node1&& node, std::bool_constant<Rtti>) { return std::forward<Node1>(node); }
template <bool Rtti, typed_placeholder PH>
auto ConvertToNode(PH ph, std::bool_constant<Rtti>) { return eval::CttiFieldNode{ ph }; }
template <bool Rtti, rtti_placeholder PH>
auto ConvertToNode(PH ph, std::bool_constant<Rtti>) { return eval::RttiFieldNode{ ph }; }

//constant nodeの場合はCttiとRttiが区別できないので、引数でどちらかを指定する。
template <neither_node_nor_placeholder Constant>
auto ConvertToNode(Constant&& c, std::bool_constant<false>) { return eval::CttiConstNode<std::decay_t<Constant>>(std::forward<Constant>(c)); }
template <neither_node_nor_placeholder Constant>
auto ConvertToNode(Constant&& c, std::bool_constant<true>) { return eval::RttiConstNode(std::forward<Constant>(c)); }

template <class Func, class ...NPs>
auto MakeFunctionNode(Func&& f, NPs&& ...nps);


#define GET_RET_TYPE_VA_FN(FN) decltype(FN(std::declval<ArgTypes_>()...))

template <class Func, class ...ArgTypes>
concept FuncApplicable = requires(Func f, const ArgTypes& ...a)
{
	{ f(a...) } -> non_void;
};
template <class Func, class RetType, class ...ArgTypes>
concept FuncWithBufApplicable = 
	FuncApplicable<Func, ArgTypes...> &&
	requires(Func f, RetType& buf, const ArgTypes& ...a)
{
	{ f(buf, a...) } -> std::same_as<void>;
};
template <class Func, class ...Args>
concept FuncShortCircuitApplicable = requires(Func f, const Args& ...args)
{
	{ f.ShortCircuit(args...) } -> non_void;
};
template <class Func, class RetType, class ...Args>
concept FuncShortCircuitWithBufApplicable = 
	FuncShortCircuitApplicable<Func, Args...> &&
	requires (Func f, RetType& buf, const Args& ...args)
{
	{ f.ShortCircuitWithBuf(buf, args...) } -> std::same_as<void>;
};
template <class Func, class RetType_, class ...ArgTypes>
	requires FuncApplicable<Func, ArgTypes...>
struct FuncDefinition
{
	template <size_t Index>
	using ArgType = GetType_t<Index, ArgTypes...>;
	using RetType = RetType_;
	//RetTypeはここでもstd::invoke_result_tで判別できないことはないが、
	//Cttiではそれでも許されるものの、
	//Rttiの場合はDFieldInfoからFieldTypeに変換しなければならず、
	//例えばFuncが参照型を返すような場合、FieldTypeの判定がうまくいかない。
	//よって、外部から与えることにする。
	
	FuncDefinition() {}
	FuncDefinition(const FuncDefinition& that) { *this = that; }
	FuncDefinition(FuncDefinition&& that) noexcept { *this = std::move(that); };

	FuncDefinition& operator=(const FuncDefinition& that) { m_func = that.m_func; return *this; }
	FuncDefinition& operator=(FuncDefinition&& that) noexcept { m_func = std::move(that.m_func); return *this; }

	template <class Func_>
		requires std::convertible_to<Func_&&, Func>
	FuncDefinition(Func_&& f) : m_func(std::forward<Func_>(f)) {}

private:
	template <class NodeImpl, size_t ...Indices, class ...Args>
	RetType Exec_nonsc(std::index_sequence<Indices...>, const NodeImpl& ni, const Args& ...args) const
	{
		return std::invoke(m_func, ni.template GetArg<Indices>(args...)...);
	}
	template <class NodeImpl, size_t ...Indices, class ...Args>
	void Exec_nonsc(std::index_sequence<Indices...>, RetType& buf, const NodeImpl& ni, const Args& ...args) const
	{
		if constexpr (FuncWithBufApplicable<Func, RetType, ArgTypes...>)
			std::invoke(m_func, buf, ni.template GetArg<Indices>(args...)...);
		else
			buf = std::invoke(m_func, ni.template GetArg<Indices>(args...)...);
	}
	template <class NodeImpl, size_t ...Indices, class ...Args>
	RetType Exec_sc(std::index_sequence<Indices...>, const NodeImpl& ni, const Args& ...args) const
	{
		return m_func.ShortCircuit(ni, args...);
	}
	template <class NodeImpl, size_t ...Indices, class ...Args>
	void Exec_sc_with_buf(std::index_sequence<Indices...>, RetType& buf, const NodeImpl& ni, const Args& ...args) const
	{
		if constexpr (FuncShortCircuitWithBufApplicable<Func, RetType, NodeImpl, Args...>)
			m_func.ShortCircuitWithBuf(buf, ni, args...);
		else
			buf = m_func.ShortCircuit(ni, args...);
	}
public:

	template <class NodeImpl, class ...Args>
	RetType Exec(const NodeImpl& ni, const Args& ...args) const
	{
		using is = std::make_index_sequence<sizeof...(ArgTypes)>;
		if constexpr (FuncShortCircuitApplicable<Func, NodeImpl, Args...>)
			return Exec_sc(is{}, ni, args...);
		else
			return Exec_nonsc(is{}, ni, args...);
	}
	template <class NodeImpl, class ...Args>
	void ExecWithBuf(RetType& buf, const NodeImpl& ni, const Args& ...args) const
	{
		using is = std::make_index_sequence<sizeof...(ArgTypes)>;
		if constexpr (FuncShortCircuitApplicable<Func, NodeImpl, Args...>)
		{
			Exec_sc_with_buf(is{}, buf, ni, args...);
		}
		else
		{
			Exec_nonsc(is{}, buf, ni, args...);
		}
	}

	Func m_func;
};

template <class ...Nodes>
struct ExtractContainer;
template <>
struct ExtractContainer<>
{};
template <class ...Nodes>
struct ExtractContainer<TypeList<Nodes...>> : public ExtractContainer<Nodes...>
{};
template <class Node, class ...Nodes>
struct ExtractContainer<Node, Nodes...> : public ExtractContainer<Nodes...>
{};
template <class Node, class ...Nodes>
	requires node_or_placeholder<Node> && (!ctti_const_node<Node>) && (!rtti_const_node<Node>)
struct ExtractContainer<Node, Nodes...>
{
	using Container = Node::Container;
};

}

//NPは、const refまたは値として与える。
//もし別所で寿命が管理されているのならNPはconst refでよいが、
//もし寿命が尽きる可能性のある一時オブジェクトならNPは値とする。
//same_as_xtに与えたいので、HasBposはboolではなくbool_constantを取る。
template <class TravOrStor, class NP, class HasBpos>
struct RttiEvalProxy : public detail::RttiMethods<RttiEvalProxy<TravOrStor, NP, HasBpos>, std::add_const_t>
{
	using Args =
		std::conditional_t<HasBpos::value,
						   std::tuple<NP, const TravOrStor&, const Bpos&>,
						   std::tuple<NP, const TravOrStor&>>;

	RttiEvalProxy(const TravOrStor& t, NP&& n) requires (!HasBpos::value)
		: m_args(n, t)
	{}
	RttiEvalProxy(const TravOrStor& s, const Bpos& bpos, NP&& n) requires HasBpos::value
		: m_args(n, s, bpos)
	{}

	//as関数は参照が返される場合があるので、decltype(auto)にしておく。
	template <FieldType Type>
	decltype(auto) as() const
	{
		//ここはNPがconst refかvalueかに関わらずconst NP&で受け取って良い。
		auto eval = []<class ...Args>(const NP& np, const Args& ...args)
		{
			return np.Evaluate(args..., Number<Type>{});
		};
		return std::apply(eval, m_args);
	}
	template <class Type>
		requires (DFieldInfo::ValueTypeToTagType<Type>() != FieldType::Emp)
	decltype(auto) as() const
	{
		return as<DFieldInfo::ValueTypeToTagType<Type>()>();
	}
	template <FieldType Type>
	decltype(auto) as_unsafe() const
	{
		return as<Type>();
	}
	template <class Type>
	decltype(auto) as_unsafe() const
	{
		return as<Type>();
	}

	/*template <FieldType Type>
		requires (DFieldInfo::IsInt(Type) || DFieldInfo::IsFlt(Type) || DFieldInfo::IsCpx(Type))
	DFieldInfo::TagTypeToValueType<Type> to() const
	{
		using RetType = DFieldInfo::TagTypeToValueType<Type>;
		return (RetType)m_np->Evaluate_to(*m_ptr, Number<Type>{});
	}
	template <FieldType Type>
		requires (!DFieldInfo::IsInt(Type) && !DFieldInfo::IsFlt(Type) && !DFieldInfo::IsCpx(Type))
	decltype(auto) to() const
	{
		return m_np->Evaluate_to(*m_ptr, Number<Type>{});
	}*/

	FieldType GetType() const { return std::get<0>(m_args).GetType(); }

private:
	Args m_args;
};

//RttiFuncNodeに事前に型情報を与えておくもの。何らかのRttiFuncNodeに大してi32()などを呼ぶと生成される。
//これ自体がTypedNodeとして機能する。
//NPは、non-const refまたは値として与える。
//もし別所で寿命が管理されているのならNPはnon-const refでよいが、
//もし寿命が尽きる可能性のある一時オブジェクトならNPは値とする。
//const refではよくない。Initする必要があるので。
template <class NP, FieldType Type>
struct RttiTypeProxy
{
	static_assert(!std::is_const_v<NP>);

	template <class NP_>
	struct ValOrPtr
	{
		ValOrPtr(NP&& np) : v(std::move(np)) {}
		NP_ v;
	};
	template <class NP_>
	struct ValOrPtr<NP_&>
	{
		ValOrPtr(NP_& np) : v(&np) {}
		NP_* v;
	};
public:
	static_assert(!std::is_const_v<NP>);
	using Decayed = std::decay_t<NP>;
	using Container = Decayed::Container;
	using Traverser = Decayed::Traverser;
	using ConstTraverser = Decayed::ConstTraverser;
	using RetType = DFieldInfo::TagTypeToValueType<Type>;

	static constexpr RankType MaxRank = Decayed::MaxRank;

	RttiTypeProxy(NP&& node)
		: m_node(std::forward<NP>(node))
	{}

	auto GetJointLayerArray() const { return GetRef().GetJointLayerArray(); }
	auto GetLayerInfo() const { return GetRef().GetLayerInfo(); }
	auto GetLayerInfo(LayerInfo<MaxRank> eli) const { return GetRef().GetLayerInfo(eli); }
	auto GetLayerInfo(LayerInfo<MaxRank> eli, DepthType depth) const { return GetRef().GetLayerInfo(eli, depth); }
	LayerType GetLayer() const { return GetRef().GetLayer(); }

	void Init(const Traverser& t, const std::vector<std::tuple<const void*, const Bpos*, bool>>& outer_t)
	{
		GetRef().Init(t, outer_t);
	}
	void Init(const Traverser& t)
	{
		Init(t, {});
	}
	void Init(const ConstTraverser& t, const std::vector<std::tuple<const void*, const Bpos*, bool>>& outer_t)
	{
		GetRef().Init(t, outer_t);
	}
	void Init(const ConstTraverser& t)
	{
		Init(t, {});
	}
	void Init(const Container& s, const Bpos& bpos)
	{
		GetRef().Init(s, bpos);
	}
	void Init(const Container& s)
	{
		GetRef().Init(s);
	}
	void Init()
	{
		GetRef().Init();
	}

	RetType Evaluate(const Traverser& t) const
		requires (std::is_trivially_copyable_v<RetType>)
	{
		return GetRef().Evaluate(t, Number<Type>{});
	}
	RetType Evaluate(const ConstTraverser& t) const
		requires (std::is_trivially_copyable_v<RetType>)
	{
		return GetRef().Evaluate(t, Number<Type>{});
	}
	const RetType& Evaluate(const Traverser& t) const
		requires (!std::is_trivially_copyable_v<RetType>)
	{
		return GetRef().Evaluate(t, Number<Type>{});
	}
	const RetType& Evaluate(const ConstTraverser& t) const
		requires (!std::is_trivially_copyable_v<RetType>)
	{
		return GetRef().Evaluate(t, Number<Type>{});
	}

	RetType Evaluate(const Container& s) const
		requires std::is_trivially_copyable_v<RetType>
	{
		return GetRef().Evaluate(s, Number<Type>{});
	}
	RetType Evaluate(const Container& s, const Bpos& bpos) const
		requires std::is_trivially_copyable_v<RetType>
	{
		return GetRef().Evaluate(s, bpos, Number<Type>{});
	}
	const RetType& Evaluate(const Container& s) const
		requires (!std::is_trivially_copyable_v<RetType>)
	{
		return GetRef().Evaluate(s, Number<Type>{});
	}
	const RetType& Evaluate(const Container& s, const Bpos& bpos) const
		requires (!std::is_trivially_copyable_v<RetType>)
	{
		return GetRef().Evaluate(s, bpos, Number<Type>{});
	}

	decltype(auto) operator()(const Traverser& t)
	{
		return GetRef()(t, Number<Type>{});
	}
	decltype(auto) operator()(const ConstTraverser& t)
	{
		return GetRef()(t, Number<Type>{});
	}
	decltype(auto) operator()(const Container& t)
	{
		return GetRef()(t, Number<Type>{});
	}
	decltype(auto) operator()(const Container& t, const Bpos& bpos)
	{
		return GetRef()(t, bpos, Number<Type>{});
	}

	static constexpr FieldType GetType() { return Type; }

private:

	Decayed& GetRef()
	{
		if constexpr (std::is_reference_v<NP>) return *m_node.v;
		else return m_node.v;
	}
	const Decayed& GetRef() const
	{
		if constexpr (std::is_reference_v<NP>) return *m_node.v;
		else return m_node.v;
	}

	ValOrPtr<NP> m_node;
};

namespace detail
{

template <class Container_>
struct RttiFuncNode_base
{
	using Container = Container_;
	using Traverser = Container::Traverser;
	using ConstTraverser = Container::ConstTraverser;

	static constexpr RankType MaxRank = Container::MaxRank;

	RttiFuncNode_base() {};
	virtual ~RttiFuncNode_base() = default;

	RttiFuncNode_base(const RttiFuncNode_base&) = delete;
	RttiFuncNode_base(RttiFuncNode_base&&) = delete;

	RttiFuncNode_base& operator=(const RttiFuncNode_base&) = delete;
	RttiFuncNode_base& operator=(RttiFuncNode_base&&) = delete;

	virtual RttiFuncNode_base<Container>* Clone() const = 0;
	virtual void CopyFrom(const RttiFuncNode_base& x) = 0;
	//virtual void MoveFrom(RttiFuncNode_base&& x) = 0;

	virtual void IncreaseDepth() = 0;

	virtual DepthType GetDepth() const { return 0; }

	virtual JointLayerArray<MaxRank> GetJointLayerArray() const = 0;
	virtual LayerInfo<MaxRank> GetLayerInfo() const = 0;
	virtual LayerInfo<MaxRank> GetLayerInfo(LayerInfo<MaxRank> eli) const = 0;
	virtual LayerInfo<MaxRank> GetLayerInfo(LayerInfo<MaxRank> eli, DepthType depth) const = 0;

	virtual void Init(const Traverser& t, const std::vector<std::tuple<const void*, const Bpos*, bool>>&) = 0;
	virtual void Init(const ConstTraverser& t, const std::vector<std::tuple<const void*, const Bpos*, bool>>&) = 0;
	void Init(const Traverser& t) { Init(t, {}); }
	void Init(const ConstTraverser& t) { Init(t, {}); }
	virtual void Init(const Container& s, const Bpos& bpos) = 0;
	virtual void Init(const Container& s) = 0;
	virtual void Init() = 0;

	virtual DFieldInfo::TagTypeToValueType<FieldType::I08>
		Evaluate(const Traverser&, Number<FieldType::I08>) const { throw Forbidden("Evaluate for I08 is not overwritten."); }
	virtual DFieldInfo::TagTypeToValueType<FieldType::I16>
		Evaluate(const Traverser&, Number<FieldType::I16>) const { throw Forbidden("Evaluate for I16 is not overwritten."); }
	virtual DFieldInfo::TagTypeToValueType<FieldType::I32>
		Evaluate(const Traverser&, Number<FieldType::I32>) const { throw Forbidden("Evaluate for I32 is not overwritten."); }
	virtual DFieldInfo::TagTypeToValueType<FieldType::I64>
		Evaluate(const Traverser&, Number<FieldType::I64>) const { throw Forbidden("Evaluate for I64 is not overwritten."); }
	virtual DFieldInfo::TagTypeToValueType<FieldType::F32>
		Evaluate(const Traverser&, Number<FieldType::F32>) const { throw Forbidden("Evaluate for F32 is not overwritten."); }
	virtual DFieldInfo::TagTypeToValueType<FieldType::F64>
		Evaluate(const Traverser&, Number<FieldType::F64>) const { throw Forbidden("Evaluate for F64 is not overwritten."); }
	virtual DFieldInfo::TagTypeToValueType<FieldType::C32>
		Evaluate(const Traverser&, Number<FieldType::C32>) const { throw Forbidden("Evaluate for C32 is not overwritten."); }
	virtual DFieldInfo::TagTypeToValueType<FieldType::C64>
		Evaluate(const Traverser&, Number<FieldType::C64>) const { throw Forbidden("Evaluate for C64 is not overwritten."); }
	virtual const DFieldInfo::TagTypeToValueType<FieldType::Str>&
		Evaluate(const Traverser&, Number<FieldType::Str>) const { throw Forbidden("Evaluate for Str is not overwritten."); }
	virtual const DFieldInfo::TagTypeToValueType<FieldType::Jbp>&
		Evaluate(const Traverser&, Number<FieldType::Jbp>) const { throw Forbidden("Evaluate for Jbp is not overwritten."); }

	virtual DFieldInfo::TagTypeToValueType<FieldType::I08>
		Evaluate(const ConstTraverser&, Number<FieldType::I08>) const { throw Forbidden("Evaluate for I08 is not overwritten."); }
	virtual DFieldInfo::TagTypeToValueType<FieldType::I16>
		Evaluate(const ConstTraverser&, Number<FieldType::I16>) const { throw Forbidden("Evaluate for I16 is not overwritten."); }
	virtual DFieldInfo::TagTypeToValueType<FieldType::I32>
		Evaluate(const ConstTraverser&, Number<FieldType::I32>) const { throw Forbidden("Evaluate for I32 is not overwritten."); }
	virtual DFieldInfo::TagTypeToValueType<FieldType::I64>
		Evaluate(const ConstTraverser&, Number<FieldType::I64>) const { throw Forbidden("Evaluate for I64 is not overwritten."); }
	virtual DFieldInfo::TagTypeToValueType<FieldType::F32>
		Evaluate(const ConstTraverser&, Number<FieldType::F32>) const { throw Forbidden("Evaluate for F32 is not overwritten."); }
	virtual DFieldInfo::TagTypeToValueType<FieldType::F64>
		Evaluate(const ConstTraverser&, Number<FieldType::F64>) const { throw Forbidden("Evaluate for F64 is not overwritten."); }
	virtual DFieldInfo::TagTypeToValueType<FieldType::C32>
		Evaluate(const ConstTraverser&, Number<FieldType::C32>) const { throw Forbidden("Evaluate for C32 is not overwritten."); }
	virtual DFieldInfo::TagTypeToValueType<FieldType::C64>
		Evaluate(const ConstTraverser&, Number<FieldType::C64>) const { throw Forbidden("Evaluate for C64 is not overwritten."); }
	virtual const DFieldInfo::TagTypeToValueType<FieldType::Str>&
		Evaluate(const ConstTraverser&, Number<FieldType::Str>) const { throw Forbidden("Evaluate for Str is not overwritten."); }
	virtual const DFieldInfo::TagTypeToValueType<FieldType::Jbp>&
		Evaluate(const ConstTraverser&, Number<FieldType::Jbp>) const { throw Forbidden("Evaluate for Jbp is not overwritten."); }

	virtual DFieldInfo::TagTypeToValueType<FieldType::I08>
		Evaluate(const Container&, Number<FieldType::I08>) const { throw Forbidden("Evaluate for I08 is not overwritten."); }
	virtual DFieldInfo::TagTypeToValueType<FieldType::I16>
		Evaluate(const Container&, Number<FieldType::I16>) const { throw Forbidden("Evaluate for I16 is not overwritten."); }
	virtual DFieldInfo::TagTypeToValueType<FieldType::I32>
		Evaluate(const Container&, Number<FieldType::I32>) const { throw Forbidden("Evaluate for I32 is not overwritten."); }
	virtual DFieldInfo::TagTypeToValueType<FieldType::I64>
		Evaluate(const Container&, Number<FieldType::I64>) const { throw Forbidden("Evaluate for I64 is not overwritten."); }
	virtual DFieldInfo::TagTypeToValueType<FieldType::F32>
		Evaluate(const Container&, Number<FieldType::F32>) const { throw Forbidden("Evaluate for F32 is not overwritten."); }
	virtual DFieldInfo::TagTypeToValueType<FieldType::F64>
		Evaluate(const Container&, Number<FieldType::F64>) const { throw Forbidden("Evaluate for F64 is not overwritten."); }
	virtual DFieldInfo::TagTypeToValueType<FieldType::C32>
		Evaluate(const Container&, Number<FieldType::C32>) const { throw Forbidden("Evaluate for C32 is not overwritten."); }
	virtual DFieldInfo::TagTypeToValueType<FieldType::C64>
		Evaluate(const Container&, Number<FieldType::C64>) const { throw Forbidden("Evaluate for C64 is not overwritten."); }
	virtual const DFieldInfo::TagTypeToValueType<FieldType::Str>&
		Evaluate(const Container&, Number<FieldType::Str>) const { throw Forbidden("Evaluate for Str is not overwritten."); }
	virtual const DFieldInfo::TagTypeToValueType<FieldType::Jbp>&
		Evaluate(const Container&, Number<FieldType::Jbp>) const { throw Forbidden("Evaluate for Jbp is not overwritten."); }

	virtual DFieldInfo::TagTypeToValueType<FieldType::I08>
		Evaluate(const Container&, const Bpos&, Number<FieldType::I08>) const { throw Forbidden("Evaluate for I08 is not overwritten."); }
	virtual DFieldInfo::TagTypeToValueType<FieldType::I16>
		Evaluate(const Container&, const Bpos&, Number<FieldType::I16>) const { throw Forbidden("Evaluate for I16 is not overwritten."); }
	virtual DFieldInfo::TagTypeToValueType<FieldType::I32>
		Evaluate(const Container&, const Bpos&, Number<FieldType::I32>) const { throw Forbidden("Evaluate for I32 is not overwritten."); }
	virtual DFieldInfo::TagTypeToValueType<FieldType::I64>
		Evaluate(const Container&, const Bpos&, Number<FieldType::I64>) const { throw Forbidden("Evaluate for I64 is not overwritten."); }
	virtual DFieldInfo::TagTypeToValueType<FieldType::F32>
		Evaluate(const Container&, const Bpos&, Number<FieldType::F32>) const { throw Forbidden("Evaluate for F32 is not overwritten."); }
	virtual DFieldInfo::TagTypeToValueType<FieldType::F64>
		Evaluate(const Container&, const Bpos&, Number<FieldType::F64>) const { throw Forbidden("Evaluate for F64 is not overwritten."); }
	virtual DFieldInfo::TagTypeToValueType<FieldType::C32>
		Evaluate(const Container&, const Bpos&, Number<FieldType::C32>) const { throw Forbidden("Evaluate for C32 is not overwritten."); }
	virtual DFieldInfo::TagTypeToValueType<FieldType::C64>
		Evaluate(const Container&, const Bpos&, Number<FieldType::C64>) const { throw Forbidden("Evaluate for C64 is not overwritten."); }
	virtual const DFieldInfo::TagTypeToValueType<FieldType::Str>&
		Evaluate(const Container&, const Bpos&, Number<FieldType::Str>) const { throw Forbidden("Evaluate for Str is not overwritten."); }
	virtual const DFieldInfo::TagTypeToValueType<FieldType::Jbp>&
		Evaluate(const Container&, const Bpos&, Number<FieldType::Jbp>) const { throw Forbidden("Evaluate for Jbp is not overwritten."); }

	virtual FieldType GetType() const = 0;
};

template <class Container, class Nodes, class Indices, class ArgTypes>
struct RttiFuncNode_body;
template <class Container_, class ...Nodes, size_t ...Indices, class ...ArgTypes>
struct RttiFuncNode_body<Container_, TypeList<Nodes...>,
						 std::index_sequence<Indices...>, TypeList<ArgTypes...>>
	: public RttiFuncNode_base<Container_>
{
	using Container = Container_;
	using Traverser = Container::Traverser;
	using ConstTraverser = Container::ConstTraverser;

	static constexpr RankType MaxRank = Container::MaxRank;

	template <size_t I>
	using Node = GetType_t<I, Nodes...>;
	template <size_t I>
	static constexpr FieldType ArgType = DFieldInfo::ValueTypeToTagType<GetType_t<I, ArgTypes...>>();

	RttiFuncNode_body() {};
	template <node_or_placeholder ...Nodes_>
	RttiFuncNode_body(Nodes_&& ...nodes)
		: m_nodes(std::forward<Nodes_>(nodes)...)
	{}

	virtual ~RttiFuncNode_body() = default;

	virtual void CopyFrom(const RttiFuncNode_base<Container>& that) override
	{
		const auto* x = dynamic_cast<const RttiFuncNode_body*>(&that);
		if (x == nullptr)
		{
			std::string message = "The type of the RttiFuncNode implementations are not the same.";
			PrintError(message);
			throw MismatchType(message);
		}
		m_nodes = x->m_nodes;
	}

	virtual void IncreaseDepth() override
	{
		DoNothing(std::get<Indices>(m_nodes).IncreaseDepth()...);
	}

private:
	template <size_t N, class ...Args>
	void Init_impl(Args&& ...args)
	{
		if constexpr (N < sizeof...(Nodes))
		{
			if constexpr (any_node<GetType_t<N, Nodes...>>)
				std::get<N>(m_nodes).Init(std::forward<Args>(args)...);
			Init_impl<N + 1>(std::forward<Args>(args)...);
		}
	}
public:
	virtual void Init(const Traverser& t, const std::vector<std::tuple<const void*, const Bpos*, bool>>& outer_t) override
	{
		Init_impl<0>(t, outer_t);
	}
	virtual void Init(const ConstTraverser& t, const std::vector<std::tuple<const void*, const Bpos*, bool>>& outer_t) override
	{
		Init_impl<0>(t, outer_t);
	}
	virtual void Init(const Container& s, const Bpos& bpos) override
	{
		Init_impl<0>(s, bpos);
	}
	virtual void Init(const Container& s) override
	{
		Init_impl<0>(s);
	}
	virtual void Init() override
	{
		Init_impl<0>();
	}

private:
	template <size_t N>
	JointLayerArray<MaxRank> GetJointLayerArray_impl() const
	{
		if constexpr (N < sizeof...(Nodes))
		{
			JointLayerArray<MaxRank> res = std::get<N>(m_nodes).GetJointLayerArray();
			if (!res.IsNotInitialized()) return res;
			return GetJointLayerArray_impl<N + 1>();
		}
		else return JointLayerArray<MaxRank>();
	}
	template <size_t ...Indices_>
	LayerInfo<MaxRank> GetLayerInfo_impl(
		LayerInfo<MaxRank> eli,
		std::index_sequence<Indices_...>) const
	{
		return std::max({ std::get<Indices_>(m_nodes).GetLayerInfo(eli)... });
	}
	template <size_t ...Indices_>
	LayerInfo<MaxRank> GetLayerInfo_impl(
		LayerInfo<MaxRank> eli, DepthType depth,
		std::index_sequence<Indices_...>) const
	{
		return std::max({ std::get<Indices_>(m_nodes).GetLayerInfo(eli, depth)... });
	}
public:

	virtual JointLayerArray<MaxRank> GetJointLayerArray() const override
	{
		return GetJointLayerArray_impl<0>();
	}
	virtual LayerInfo<MaxRank> GetLayerInfo() const override
	{
		return GetLayerInfo(LayerInfo<MaxRank>(GetJointLayerArray()));
	}
	virtual LayerInfo<MaxRank> GetLayerInfo(LayerInfo<MaxRank> eli) const override
	{
		return GetLayerInfo_impl(eli, std::make_index_sequence<sizeof...(Nodes)>());
	}
	virtual LayerInfo<MaxRank> GetLayerInfo(LayerInfo<MaxRank> eli, DepthType depth) const override
	{
		return GetLayerInfo_impl(eli, depth, std::make_index_sequence<sizeof...(Nodes)>());
	}

	template <size_t Index, class TravStor>
	decltype(auto) GetArg(const TravStor& t) const
	{
		static constexpr FieldType Type = ArgType<Index>;
		using Node = GetType_t<Index, Nodes...>;
		if constexpr (rtti_node<Node>)
			return std::get<Index>(m_nodes).Evaluate(t, Number<Type>());
		else if constexpr (Node::GetType() != Type)
		{
			//typedで本来の型とここのTypeが一致しないようなら、例外を投げる。
			return []() -> DFieldInfo::TagTypeToValueType<Type> { throw MismatchType(""); } ();
		}
		else if constexpr (DFieldInfo::IsTrivial(Type))
		{
			//typedの場合、戻り値の方はI64、F64、C64に該当しない、別のものの可能性がある。
			//その場合、適切に変換しなければならない。
			return DFieldInfo::TagTypeToValueType<Type>(std::get<Index>(m_nodes).Evaluate(t));
		}
		else
		{
			//typedでnon-trivialということは該当するのはStrかJbpである。どちらも変換する必要はない。
			return std::get<Index>(m_nodes).Evaluate(t);
		}
	}
	template <size_t Index>
	decltype(auto) GetArg(const Container& s, const Bpos& bpos) const
	{
		static constexpr FieldType Type = ArgType<Index>;
		using Node = GetType_t<Index, Nodes...>;
		if constexpr (rtti_node<Node>)
			return std::get<Index>(m_nodes).Evaluate(s, bpos, Number<Type>());
		else if constexpr (Node::GetType() != Type)
		{
			//typedで本来の型とここのTypeが一致しないようなら、例外を投げる。
			return []() -> DFieldInfo::TagTypeToValueType<Type> { throw MismatchType(""); } ();
		}
		else if constexpr (DFieldInfo::IsTrivial(Type))
		{
			//typedの場合、戻り値の方はI64、F64、C64に該当しない、別のものの可能性がある。
			//その場合、適切に変換しなければならない。
			return DFieldInfo::TagTypeToValueType<Type>(std::get<Index>(m_nodes).Evaluate(s, bpos));
		}
		else
		{
			//typedでnon-trivialということは該当するのはStrかJbpである。どちらも変換する必要はない。
			return std::get<Index>(m_nodes).Evaluate(s, bpos);
		}
	}

private:
	std::tuple<Nodes...> m_nodes;
};

}

template <class Container_>
struct RttiFuncNode : public detail::RttiMethods<RttiFuncNode<Container_>, std::type_identity_t>
{
	using Container = Container_;
	using Traverser = Container::Traverser;
	using ConstTraverser = Container::ConstTraverser;

	static constexpr RankType MaxRank = Container::MaxRank;

	template <class TravOrStor, class NP, class HasBpos>
	friend struct RttiEvalProxy;

	RttiFuncNode() = default;

	RttiFuncNode(const RttiFuncNode& x)
	{
		m_impl = std::unique_ptr<detail::RttiFuncNode_base<Container_>>(x.m_impl->Clone());
	}
	RttiFuncNode(RttiFuncNode&& x)//RttiFuncNodeの中身が一致するとは限らないので、noexceptにできない。
	{
		m_impl = std::move(x.m_impl);
	}
	RttiFuncNode& operator=(const RttiFuncNode& x)
	{
		//これが呼び出される時点で、RttiFuncNodeのConstructが既に呼ばれているとは限らない。
		if (m_impl) m_impl->CopyFrom(*x.m_impl);
		else m_impl = std::unique_ptr<detail::RttiFuncNode_base<Container_>>(x.m_impl->Clone());
		return *this;
	}
	RttiFuncNode& operator=(RttiFuncNode&& x)
	{
		m_impl = std::move(x.m_impl);
		return *this;
	}

	template <class NodeImpl, class Func, class ...Args>
	void Construct(Func&& f, Args&& ...nodes)
	{
		m_impl = std::make_unique<NodeImpl>(std::forward<Func>(f), std::forward<Args>(nodes)...);
	}

	RttiFuncNode IncreaseDepth() const&
	{
		auto res = *this;
		std::move(res).IncreaseDepth();
		return std::move(*this);
	}
	RttiFuncNode IncreaseDepth() &&
	{
		m_impl->IncreaseDepth();
		return std::move(*this);
	}

	void Init(const Traverser& t, const std::vector<std::tuple<const void*, const Bpos*, bool>>& outer_t)
	{
		m_impl->Init(t, outer_t);
		m_init_flag = false;
	}
	void Init(const ConstTraverser& t, const std::vector<std::tuple<const void*, const Bpos*, bool>>& outer_t)
	{
		m_impl->Init(t, outer_t);
		m_init_flag = false;
	}
	void Init(const Traverser& t) { Init(t, {}); }
	void Init(const ConstTraverser& t) { Init(t, {}); }
	void Init(const Container& s, const Bpos& bpos)
	{
		m_impl->Init(s, bpos);
		m_init_flag = false;
	}
	void Init(const Container& s)
	{
		m_impl->Init(s);
		m_init_flag = false;
	}
	void Init() { m_impl->Init(); m_init_flag = true; };

	JointLayerArray<MaxRank> GetJointLayerArray() const
	{
		return m_impl->GetJointLayerArray();
	}
	LayerInfo<MaxRank> GetLayerInfo() const
	{
		return m_impl->GetLayerInfo();
	}
	LayerInfo<MaxRank> GetLayerInfo(LayerInfo<MaxRank> eli) const
	{
		return m_impl->GetLayerInfo(eli);
	}
	LayerInfo<MaxRank> GetLayerInfo(LayerInfo<MaxRank> eli, DepthType depth) const
	{
		return m_impl->GetLayerInfo(eli, depth);
	}
	LayerType GetLayer() const
	{
		return m_impl->GetLayerInfo().GetTravLayer();
	}

	template <FieldType Type>
	decltype(auto) Evaluate(const Traverser& t, Number<Type> n) const
	{
		return m_impl->Evaluate(t, n);
	}
	template <FieldType Type>
	decltype(auto) Evaluate(const ConstTraverser& t, Number<Type> n) const
	{
		return m_impl->Evaluate(t, n);
	}
	template <FieldType Type>
	decltype(auto) Evaluate(const Container& s, Number<Type> n) const
	{
		assert(GetLayer() == -1);
		return m_impl->Evaluate(s, n);
	}
	template <FieldType Type>
	decltype(auto) Evaluate(const Container& s, const Bpos& bpos, Number<Type> n) const
	{
		return m_impl->Evaluate(s, bpos, n);
	}

	template <FieldType Type>
	decltype(auto) operator()(const Traverser& t, Number<Type> n) const
	{
		if (m_init_flag) [[unlikely]] Init(t);
		return Evaluate(t, n);
	}
	template <FieldType Type>
	decltype(auto) operator()(const ConstTraverser& t, Number<Type> n) const
	{
		if (m_init_flag) [[unlikely]] Init(t);
		return Evaluate(t, n);
	}
	template <FieldType Type>
	decltype(auto) operator()(const Container& s, Number<Type> n) const
	{
		if (m_init_flag) [[unlikely]] Init(s);
		return Evaluate(s, n);
	}
	template <FieldType Type>
	decltype(auto) operator()(const Container& s, const Bpos& bpos, Number<Type> n) const
	{
		if (m_init_flag) [[unlikely]] Init(s, bpos);
		return Evaluate(s, bpos, n);
	}

	RttiEvalProxy<Traverser, const RttiFuncNode&, std::false_type> Evaluate(const Traverser& t) const&
	{
		return RttiEvalProxy<Traverser, const RttiFuncNode&, std::false_type>{ t, *this };
	}
	RttiEvalProxy<ConstTraverser, const RttiFuncNode&, std::false_type> Evaluate(const ConstTraverser& t) const&
	{
		return RttiEvalProxy<ConstTraverser, const RttiFuncNode&, std::false_type>{ t, *this };
	}
	RttiEvalProxy<Container, const RttiFuncNode&, std::false_type> Evaluate(const Container& s) const&
	{
		return RttiEvalProxy<Container, const RttiFuncNode&, std::false_type>{ s, *this };
	}
	RttiEvalProxy<Container, const RttiFuncNode&, std::true_type> Evaluate(const Container& s, const Bpos& bpos) const&
	{
		return RttiEvalProxy<Container, const RttiFuncNode&, std::true_type>{ s, bpos, *this };
	}

	RttiEvalProxy<Traverser, RttiFuncNode, std::false_type> Evaluate(const Traverser& t) &&
	{
		return RttiEvalProxy<Traverser, RttiFuncNode, std::false_type>{ t, * this };
	}
	RttiEvalProxy<ConstTraverser, RttiFuncNode, std::false_type> Evaluate(const ConstTraverser& t) &&
	{
		return RttiEvalProxy<ConstTraverser, RttiFuncNode, std::false_type>{ t, * this };
	}
	RttiEvalProxy<Container, RttiFuncNode, std::false_type> Evaluate(const Container& s) &&
	{
		return RttiEvalProxy<Container, RttiFuncNode, std::false_type>{ s, * this };
	}
	RttiEvalProxy<Container, RttiFuncNode, std::true_type> Evaluate(const Container& s, const Bpos& bpos) &&
	{
		return RttiEvalProxy<Container, RttiFuncNode, std::true_type>{ s, bpos, * this };
	}

	RttiEvalProxy<Traverser, const RttiFuncNode&, std::false_type> operator()(const Traverser& t) &
	{
		if (m_init_flag) [[unlikely]] Init(t);
		return Evaluate(t);
	}
	RttiEvalProxy<ConstTraverser, const RttiFuncNode&, std::false_type> operator()(const ConstTraverser& t) &
	{
		if (m_init_flag) [[unlikely]] Init(t);
		return Evaluate(t);
	}
	RttiEvalProxy<Container, const RttiFuncNode&, std::false_type> operator()(const Container& s) &
	{
		if (m_init_flag) [[unlikely]] Init(s);
		return Evaluate(s);
	}
	RttiEvalProxy<Container, const RttiFuncNode&, std::true_type> operator()(const Container& s, const Bpos& bpos) &
	{
		if (m_init_flag) [[unlikely]] Init(s, bpos);
		return Evaluate(s, bpos);
	}

	RttiEvalProxy<Traverser, RttiFuncNode, std::false_type> operator()(const Traverser& t) &&
	{
		if (m_init_flag) [[unlikely]] Init(t);
		return Evaluate(t);
	}
	RttiEvalProxy<ConstTraverser, RttiFuncNode, std::false_type> operator()(const ConstTraverser& t) &&
	{
		if (m_init_flag) [[unlikely]] Init(t);
		return Evaluate(t);
	}
	RttiEvalProxy<Container, RttiFuncNode, std::false_type> operator()(const Container& s) &&
	{
		if (m_init_flag) [[unlikely]] Init(s);
		return Evaluate(s);
	}
	RttiEvalProxy<Container, RttiFuncNode, std::true_type> operator()(const Container& s, const Bpos& bpos) &&
	{
		if (m_init_flag) [[unlikely]] Init(s, bpos);
		return Evaluate(s, bpos);
	}

	FieldType GetType() const { return m_impl->GetType(); }

	//const ref版のasは不要。const refでproxyに渡してもInitできないため。
	template <FieldType Type>
	RttiTypeProxy<RttiFuncNode<Container>&, Type> as()&
	{
		return RttiTypeProxy<RttiFuncNode<Container>&, Type>(*this);
	}
	template <FieldType Type>
	RttiTypeProxy<RttiFuncNode<Container>, Type> as()&&
	{
		return RttiTypeProxy<RttiFuncNode<Container>, Type>(std::move(*this));
	}

private:
	std::unique_ptr<detail::RttiFuncNode_base<Container>> m_impl;
	bool m_init_flag = true;
};

}

template <any_container Container, node_or_placeholder ...NPs>
void InitAll(const Container& t, const Bpos& bpos, NPs& ...nps)
{
	DoNothing((nps.Init(t, bpos), 0)...);
}
template <any_container Container, node_or_placeholder ...NPs>
void InitAll(const Container& t, NPs& ...nps)
{
	DoNothing((nps.Init(t), 0)...);
}
template <any_traverser Trav, node_or_placeholder ...NPs>
void InitAll(const Trav& t, NPs& ...nps)
{
	DoNothing((nps.Init(t), 0)...);
}
template <node_or_placeholder ...NPs>
void InitAll(NPs& ...nps)
{
	DoNothing((nps.Init(), 0)...);
}

}


#endif
