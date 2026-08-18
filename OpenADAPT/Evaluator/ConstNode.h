#ifndef ADAPT_EVALUATOR_CONST_NODE_H
#define ADAPT_EVALUATOR_CONST_NODE_H

#include <vector>
#include <variant>
#include <OpenADAPT/Utility/Utility.h>
#include <OpenADAPT/Common/Concepts.h>
#include <OpenADAPT/Common/Bpos.h>
#include <OpenADAPT/Joint/LayerInfo.h>
#include <OpenADAPT/Evaluator/Placeholder.h>

namespace adapt
{

namespace eval
{

ADAPT_EXPORT
template <class Type>
struct CttiConstNode : public detail::CttiMethods<CttiConstNode<Type>, std::add_const_t>
{
	using RetType = Type;

	CttiConstNode()
		: m_value() {}
	CttiConstNode(const Type& t)
		: m_value(t)
	{}
	CttiConstNode(Type&& t)
		: m_value(std::move(t))
	{}

	CttiConstNode(const CttiConstNode& x) = default;
	CttiConstNode(CttiConstNode&& x) = default;

	CttiConstNode& operator=(const CttiConstNode& x) = default;
	CttiConstNode& operator=(CttiConstNode&& x) = default;

	CttiConstNode IncreaseDepth()&& { return std::move(*this); }
	CttiConstNode IncreaseDepth() const& { return *this; }

	template <class Trav>
	void Init(const Trav&, const std::vector<std::tuple<const void*, const Bpos*, bool>>&) {}
	template <class Container>
	void Init(const Container&, const Bpos&) {}
	template <class Container>
	void Init(const Container&) {}
	void Init() {}

	static constexpr Default GetJointLayerArray() { return Default{}; }
	static constexpr Default GetLayerInfo() { return Default{}; }
	template <RankType MaxRank>
	static constexpr LayerInfo<MaxRank> GetLayerInfo(LayerInfo<MaxRank> eli)
	{
		//ConstantNodeは階層を持たないので、そのまま返せばよい。
		return eli;
	}
	template <RankType MaxRank>
	static constexpr LayerInfo<MaxRank> GetLayerInfo(LayerInfo<MaxRank> eli, DepthType)
	{
		return eli;
	}
	static constexpr LayerType GetLayer()
	{
		return -1;
	}

	template <class Trav>
	const Type& Evaluate(const Trav&) const
	{
		return m_value;
	}
	template <class Container>
	const Type& Evaluate(const Container&, const Bpos&) const
	{
		return m_value;
	}

private:
	Type m_value;
};

ADAPT_EXPORT
struct RttiConstNode
{
	RttiConstNode() {}
	template <std::integral Type>
		requires (sizeof(Type) == 1)
	RttiConstNode(Type v) : m_value((int8_t)v) {}
	template <std::integral Type>
		requires (sizeof(Type) == 2)
	RttiConstNode(Type v) : m_value((int16_t)v) {}
	template <std::integral Type>
		requires (sizeof(Type) == 4)
	RttiConstNode(Type v) : m_value((int32_t)v) {}
	template <std::integral Type>
		requires (sizeof(Type) == 8)
	RttiConstNode(Type v) : m_value((int64_t)v) {}
	template <std::floating_point Type>
		requires (sizeof(Type) <= 4)
	RttiConstNode(Type v) : m_value((float)v) {}
	template <std::floating_point Type>
		requires (sizeof(Type) >= 8)
	RttiConstNode(Type v) : m_value((double)v) {}
	RttiConstNode(const std::string& v) : m_value(v) {}
	RttiConstNode(std::string&& v) : m_value(std::move(v)) {}
	RttiConstNode(const Bpos& v) : m_value(v) {}
	RttiConstNode(Bpos&& v) : m_value(std::move(v)) {}
	RttiConstNode(const JBpos& v) : m_value(v) {}
	RttiConstNode(JBpos&& v) : m_value(std::move(v)) {}

	RttiConstNode(const RttiConstNode& n) = default;
	RttiConstNode(RttiConstNode&& n) noexcept = default;

	RttiConstNode& operator=(const RttiConstNode& n) = default;
	RttiConstNode& operator=(RttiConstNode&& n) noexcept = default;

	RttiConstNode IncreaseDepth()&& { return std::move(*this); }
	RttiConstNode IncreaseDepth() const& { return *this; }

	template <class Trav>
	void Init(const Trav&, const std::vector<std::tuple<const void*, const Bpos*, bool>>&) {}
	template <class Container>
	void Init(const Container&, const Bpos&) {}
	template <class Container>
	void Init(const Container&) {}
	void Init() {}

	constexpr Default GetJointLayerArray() const
	{
		return Default{};
	}
	constexpr Default GetLayerInfo() const
	{
		return Default{};
	}
	template <RankType MaxRank>
	constexpr LayerInfo<MaxRank> GetLayerInfo(LayerInfo<MaxRank> eli) const
	{
		//ConstantNodeは階層を持たないので、そのまま返せばよい。
		return eli;
	}
	template <RankType MaxRank>
	constexpr LayerInfo<MaxRank> GetLayerInfo(LayerInfo<MaxRank> eli, DepthType) const
	{
		return eli;
	}
	constexpr LayerType GetLayer() const
	{
		return -1;
	}

	template <FieldType Type>
	const DFieldInfo::TagTypeToValueType<Type>& GetValue(Number<Type>) const
	{
#define CODE(Tag, Name, type)\
		if constexpr (Type == FieldType::Tag) return std::get<(size_t)Index##Tag>(m_value); else 
		ADAPT_FOR_EACH_TYPE(CODE)
#undef CODE
		throw MismatchType("");
	}
	template <FieldType Type>
	const DFieldInfo::TagTypeToValueType<Type>& as() const { return GetValue(Number<Type>()); }

	template <class Trav, FieldType Type>
	const DFieldInfo::TagTypeToValueType<Type>& Evaluate(const Trav&, Number<Type> n) const
	{
		return GetValue(n);
	}
	template <class Container, FieldType Type>
	const DFieldInfo::TagTypeToValueType<Type>& Evaluate(const Container&, const Bpos&, Number<Type> n) const
	{
		return GetValue(n);
	}

#define CODE(Tag, Name, Type)\
	bool Is##Tag() const { return m_value.index() == (size_t)Index##Tag; }
	ADAPT_FOR_EACH_TYPE(CODE)
#undef CODE

	FieldType GetType() const
	{
		auto get_type_visitor = []<class T>(const T&) { return DFieldInfo::ValueTypeToTagType<T>(); };
		return std::visit(get_type_visitor, m_value);
	}

private:

#define CODE(Tag, Name, Type) Index##Tag,
	enum ValueIndex : size_t
	{
		ADAPT_FOR_EACH_TYPE(CODE)
	};
#undef CODE

#define CODE(Tag, Name, Type) Type,
	using Variant = std::variant<ADAPT_FOR_EACH_TYPE(CODE) std::nullptr_t>;
#undef CODE
	Variant m_value;
};



}

}

#endif