#ifndef ADAPT_EVALUATOR_CONST_NODE_H
#define ADAPT_EVALUATOR_CONST_NODE_H

#include <vector>
#include <variant>
#include <OpenADAPT/Utility/Utility.h>
#include <OpenADAPT/Common/Concepts.h>
#include <OpenADAPT/Joint/LayerInfo.h>
#include <OpenADAPT/Evaluator/Placeholder.h>

namespace adapt
{

namespace eval
{


template <class Type>
struct CttiConstNode : public detail::CttiMethods<CttiConstNode<Type>, std::add_const_t>
{
	using RetType = Type;

	CttiConstNode() {}
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
	template <class T>
		requires std::floating_point<T> && (sizeof(T) <= 4)
	RttiConstNode(std::complex<T> v) : m_value(std::complex<float>(v)) {}
	template <class T>
		requires std::floating_point<T> && (sizeof(T) >= 8)
	RttiConstNode(std::complex<T> v) : m_value(std::complex<double>(v)) {}
	RttiConstNode(const std::string& v) : m_value(v) {}
	RttiConstNode(std::string&& v) : m_value(std::move(v)) {}
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
	const DFieldInfo::TagTypeToValueType<Type>& Evaluate_impl(Number<Type>) const
	{
		if constexpr (Type == FieldType::I08) return std::get<0>(m_value);
		else if constexpr (Type == FieldType::I16) return std::get<1>(m_value);
		else if constexpr (Type == FieldType::I32) return std::get<2>(m_value);
		else if constexpr (Type == FieldType::I64) return std::get<3>(m_value);
		else if constexpr (Type == FieldType::F32) return std::get<4>(m_value);
		else if constexpr (Type == FieldType::F64) return std::get<5>(m_value);
		else if constexpr (Type == FieldType::C32) return std::get<6>(m_value);
		else if constexpr (Type == FieldType::C64) return std::get<7>(m_value);
		else if constexpr (Type == FieldType::Str) return std::get<8>(m_value);
		else if constexpr (Type == FieldType::Jbp) return std::get<9>(m_value);
		else throw MismatchType("");
	}

	template <class Trav, FieldType Type>
	const DFieldInfo::TagTypeToValueType<Type>& Evaluate(const Trav&, Number<Type> n) const
	{
		return Evaluate_impl(n);
	}
	template <class Container, FieldType Type>
	const DFieldInfo::TagTypeToValueType<Type>& Evaluate(const Container&, const Bpos&, Number<Type> n) const
	{
		return Evaluate_impl(n);
	}

	bool IsI08() const { return m_value.index() == 0; }
	bool IsI16() const { return m_value.index() == 1; }
	bool IsI32() const { return m_value.index() == 2; }
	bool IsI64() const { return m_value.index() == 3; }
	bool IsF32() const { return m_value.index() == 4; }
	bool IsF64() const { return m_value.index() == 5; }
	bool IsC32() const { return m_value.index() == 6; }
	bool IsC64() const { return m_value.index() == 7; }
	bool IsStr() const { return m_value.index() == 8; }
	bool IsJbp() const { return m_value.index() == 9; }

	FieldType GetType() const
	{
		auto get_type_visitor = []<class T>(const T&) { return DFieldInfo::ValueTypeToTagType<T>(); };
		return std::visit(get_type_visitor, m_value);
	}

private:
	std::variant<int8_t, int16_t, int32_t, int64_t,
		float, double,
		std::complex<float>, std::complex<double>,
		std::string, JBpos> m_value;
};



}

}

#endif