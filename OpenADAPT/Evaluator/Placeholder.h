#ifndef ADAPT_EVALUATOR_PLACEHOLDER_H
#define ADAPT_EVALUATOR_PLACEHOLDER_H

#include <variant>
#include <OpenADAPT/Common/Common.h>
#include <OpenADAPT/Utility/NamedTuple.h>
#include <OpenADAPT/Joint/LayerInfo.h>
#include <OpenADAPT/Common/Concepts.h>

namespace adapt
{

ADAPT_EXPORT
struct DFieldInfo
{
	using enum FieldType;
private:
	template <FieldType Tag, std::nullptr_t = nullptr>
	struct TagTypeToValueType_impl;
#define CODE(Tag, name, type)\
	template <std::nullptr_t N> struct TagTypeToValueType_impl<Tag, N> { using Type = type; };
	ADAPT_FOR_EACH_TYPE(CODE)
#undef CODE

	template <class Type, size_t Size>
	static constexpr bool is_integral_with_size = std::is_integral_v<Type> && sizeof(Type) == Size;
public:

	template <FieldType Tag>
	using TagTypeToValueType = typename TagTypeToValueType_impl<Tag>::Type;
	template <class ValueType>
	static constexpr FieldType ValueTypeToTagType()
	{
		using enum FieldType;
#define CODE(Tag, name, type)\
		if constexpr (std::is_same_v<ValueType, type>) return Tag; else 
		ADAPT_FOR_EACH_TYPE(CODE)
#undef CODE
		return Emp;
	}

	template <class T>
	static constexpr FieldType GetSameSizeTagType()
	{
		using enum FieldType;
#define CODE(Tag, name, type)\
		if constexpr (Is##Tag<T>()) return Tag; else 
		ADAPT_FOR_EACH_TYPE(CODE)
#undef CODE
		return Emp;
	}

	//GetSameSizeTagType() != Emp、つまり
	//* 整数型なら1、2、4、8バイトである
	//* float、double、std::complex<float>、std::complex<double>のいずれかである
	//* std::stringかJBposに変換可能である
	//いずれかを満たすようなら、この関数はtrueを返す。
	template <class T>
	static constexpr bool IsConvertibleToTagType()
	{
		return GetSameSizeTagType<T>() != FieldType::Emp;
	}

	template <FieldType Type>
	static constexpr auto GetTagTypeString()
	{
		using enum FieldType;
#define CODE(Tag, name, type)\
		if constexpr (Type == Tag) return StaticString<#Tag>{};
		ADAPT_FOR_EACH_TYPE(CODE)
#undef CODE
		if constexpr (Type == Emp) return StaticString<"Emp">{};
	}
	static std::string GetTagTypeString(FieldType Type)
	{
		using enum FieldType;
#define CODE(Tag, name, type)\
		if (Type == Tag) return #Tag; else 
		ADAPT_FOR_EACH_TYPE(CODE)
#undef CODE
		if (Type == Emp) return std::string("Emp");
		else throw MismatchType("");
	}

	static constexpr size_t GetSizeOf(FieldType tag)
	{
		return size_t(((uint32_t)tag & FieldTypeFlag::SIZE_MASK) >> 16);
	}
	static constexpr size_t GetAlignOf(FieldType tag)
	{
		return size_t(((uint32_t)tag & FieldTypeFlag::ALIGN_MASK) >> 24);
	}

	template <class T>
	static constexpr bool IsTrivial() { return std::is_trivially_copyable_v<T>; }

	template <class T>
	static constexpr bool IsCpxAri() { return std::is_arithmetic_v<T> || std::is_same_v<T, std::complex<float>> || std::is_same_v<T, std::complex<double>>; }
	template <class T>
	static constexpr bool IsArithmetic() { return std::is_arithmetic_v<T>; }

	template <class T>
	static constexpr bool IsInt() { return std::is_integral_v<T>; }

	template <class T>
	static constexpr bool IsI08() { return is_integral_with_size<T, 1>; }
	template <class T>
	static constexpr bool IsI16() { return is_integral_with_size<T, 2>; }
	template <class T>
	static constexpr bool IsI32() { return is_integral_with_size<T, 4>; }
	template <class T>
	static constexpr bool IsI64() { return is_integral_with_size<T, 8>; }

	template <class T>
	static constexpr bool IsF32() { return std::is_same_v<T, float>; }
	template <class T>
	static constexpr bool IsF64() { return std::is_same_v<T, double>; }

	//template <class T>
	//static constexpr bool IsC32() { return std::is_same_v<T, std::complex<float>>; }
	//template <class T>
	//static constexpr bool IsC64() { return std::is_same_v<T, std::complex<double>>; }

	template <class T>
	static constexpr bool IsStr() { return std::is_convertible_v<T, std::string>; }
	template <class T>
	static constexpr bool IsBps() { return std::is_convertible_v<T, Bpos>; }
	template <class T>
	static constexpr bool IsJbp() { return std::is_convertible_v<T, JBpos>; }

	// std::is_trivially_copyableを満たすもの。INT、FLT、CPXが該当。
	static constexpr bool IsTrivial(FieldType type) { return (uint32_t)type & FieldTypeFlag::TRIVIAL; }

	// 数値型または複素数型。INT、FLT、CPXが該当。
	//static constexpr bool IsCpxAri(FieldType type) { return (uint32_t)type & (FieldTypeFlag::CPX | FieldTypeFlag::NUM); }
	// 数値型。INT、FLTが該当。
	static constexpr bool IsArithmetic(FieldType type) { return (uint32_t)type & FieldTypeFlag::NUM; }
	static constexpr bool IsInt(FieldType type) { return (uint32_t)type & FieldTypeFlag::INT; }
	static constexpr bool IsFlt(FieldType type) { return (uint32_t)type & FieldTypeFlag::FLT; }

#define CODE(Tag, Name, Type)\
	static constexpr bool Is##Tag(FieldType type) { return type == FieldType::Tag; }
	ADAPT_FOR_EACH_TYPE(CODE)
#undef CODE
	static constexpr bool IsEmp(FieldType type) { return type == FieldType::Emp; }

	template <FieldType From, FieldType To>
	static constexpr bool IsConvertibleTo()
	{
		using TF = TagTypeToValueType<From>;
		using TT = TagTypeToValueType<To>;
		return std::convertible_to<TF, TT>;
	}
	static constexpr bool IsConvertibleTo(FieldType from, FieldType to)
	{
		if (IsInt(from))
		{
			if (IsInt(to) || IsFlt(to)) return true;
		}
		else if (IsFlt(from))
		{
			if (IsInt(to) || IsFlt(to)) return true;
			//else if (IsCpx(to))
			//{
			//	if (IsF64(from) && IsC32(to)) return false;
			//	else return true;
			//}
		}
		//else if (IsCpx(from))
		//{
		//	if (IsCpx(to)) return true;
		//}
		else if (IsStr(from))
		{
			if (IsStr(to)) return true;
		}
		else if (IsBps(from))
		{
			if (IsJbp(to)) return true;
		}
		else if (IsJbp(from))
		{
			if (IsJbp(to)) return true;
		}
		return false;
	}
	static constexpr bool IsConvertibleToBool(FieldType from)
	{
#define CODE(TTYPE1, SYM, VTYPE1) if (from == TTYPE1) return std::convertible_to<VTYPE1, bool>;
		ADAPT_FOR_EACH_TYPE(CODE)
#undef CODE
		throw MismatchType("");
	}
};

namespace eval
{

namespace detail
{

template <class Derived, template <class> class Qualifier>
struct CttiMethods
{
	//実質的にはTypedMethodsである。
private:
	constexpr const Derived& Cast() const& { return static_cast<const Derived&>(*this); }
	constexpr Derived& Cast()& { return static_cast<Derived&>(*this); }
	constexpr Derived&& Cast_move() { return static_cast<Derived&&>(*this); }

public:

	static constexpr size_t GetSize() { return sizeof(typename Derived::RetType); }
	static constexpr size_t GetAlign() { return alignof(typename Derived::RetType); }

	static constexpr FieldType GetType() { return DFieldInfo::GetSameSizeTagType<typename Derived::RetType>(); }

	static constexpr bool IsTrivial() { return DFieldInfo::IsTrivial<typename Derived::RetType>(); }

	static constexpr bool IsArithmetic() { return DFieldInfo::IsArithmetic<typename Derived::RetType>(); }

	static constexpr bool IsInt() { return DFieldInfo::IsInt<typename Derived::RetType>(); }

#define CODE(Tag, Name, Type)\
	static constexpr bool Is##Tag() { return DFieldInfo::Is##Tag<typename Derived::RetType>(); }
	ADAPT_FOR_EACH_TYPE(CODE)
#undef CODE

	template <StaticChar Name_>
	constexpr std::tuple<StaticString<Name_>, Derived> named() const&
	{
		return std::make_tuple(StaticString<Name_>{}, Cast());
	}
	template <StaticChar Name_>
	constexpr std::tuple<StaticString<Name_>, Derived> named()&&
	{
		return std::make_tuple(StaticString<Name_>{}, Cast_move());
	}
	template <StaticChar Name_>
	constexpr std::tuple<StaticString<Name_>, Derived> named(StaticString<Name_> name) const&
	{
		return std::make_tuple(name, Cast());
	}
	template <StaticChar Name_>
	constexpr std::tuple<StaticString<Name_>, Derived> named(StaticString<Name_> name)&&
	{
		return std::make_tuple(name, Cast_move());
	}
	constexpr std::tuple<std::string, Derived> named(std::string_view name) const&
	{
		return std::tuple<std::string, Derived>(name, Cast());
	}
	constexpr std::tuple<std::string, Derived> named(std::string_view name)&&
	{
		return std::tuple<std::string, Derived>(name, Cast_move());
	}
};

template <class Derived, template <class> class Qualifier>
using TypedMethods = CttiMethods<Derived, Qualifier>;

template <class Derived, template <class> class Qualifier>
struct RttiMethods
{
	//as、toなどの値を取得する関数が定義されていれば、そこからi64()やf64()、to_i64()などを自動的に追加する。
private:
	constexpr const Derived& Cast() const& { return static_cast<const Derived&>(*this); }
	constexpr Derived& Cast()& { return static_cast<Derived&>(*this); }
	constexpr Derived&& Cast_move() { return static_cast<Derived&&>(*this); }
public:

	template <class Der2, template <class> class Qua2>
	bool operator==(const RttiMethods<Der2, Qua2>& that) const
	{
		const Derived& dthis = static_cast<const Derived&>(*this);
		const Der2& dthat = static_cast<const Der2&>(that);
		FieldType type = dthis.GetType();
		if (type != dthat.GetType()) return false;
#define CODE(T) return dthis.template as<T>() == dthat.template as<T>();
		ADAPT_SWITCH_FIELD_TYPE(type, CODE, throw MismatchType("");)
#undef CODE
	}
	template <class Der2, template <class> class Qua2>
	bool operator!=(const RttiMethods<Der2, Qua2>& that) const
	{
		return !(*this == that);
	}

	// i08(), i08_unsafe(), i08_ptr()などの関数を自動的に追加する。
#define CODE(Tag, Name, Type)\
	decltype(auto) Name() const& { return Cast().template as<FieldType::Tag>(); }\
	decltype(auto) Name()& requires (!std::is_const_v<Qualifier<char>>) { return Cast().template as<FieldType::Tag>(); }\
	decltype(auto) Name() && requires (!std::is_const_v<Qualifier<char>>) { return Cast_move().template as<FieldType::Tag>(); }\
	decltype(auto) Name##_unsafe() const& { return Cast().template as_unsafe<FieldType::Tag>(); }\
	decltype(auto) Name##_unsafe()& requires (!std::is_const_v<Qualifier<char>>) { return Cast().template as_unsafe<FieldType::Tag>(); }\
	decltype(auto) Name##_unsafe() && requires (!std::is_const_v<Qualifier<char>>) { return Cast_move().template as_unsafe<FieldType::Tag>(); }\
	decltype(auto) Name##_ptr() const { return Cast().template as_ptr<FieldType::Tag>(); }\
	decltype(auto) Name##_ptr() requires (!std::is_const_v<Qualifier<char>>) { return Cast().template as_ptr<FieldType::Tag>(); }
	ADAPT_FOR_EACH_TYPE(CODE)
#undef CODE

	FieldVariant var() const;

	// to_i08()などの関数を自動的に追加する。
	// to_はトリビアル型については変換を行うが、非トリビアル型については変換を行わず、単にstr()などをを呼び出す。
#define CODE(Tag, Name, Type)\
	Type to_##Name() const { return Cast().template to<FieldType::Tag>(); }
	ADAPT_FOR_EACH_TRIVIAL_TYPE(CODE)
#undef CODE

#define CODE(Tag, Name, Type)\
	const Type& to_##Name() const { return Name(); }
		ADAPT_FOR_EACH_NONTRIVIAL_TYPE(CODE)
#undef CODE

	constexpr size_t GetSize() const { return DFieldInfo::GetSizeOf(Cast().GetType()); }
	constexpr size_t GetAlign() const { return DFieldInfo::GetAlignOf(Cast().GetType()); }

	constexpr bool IsTrivial() const { return DFieldInfo::IsTrivial(Cast().GetType()); }
	constexpr bool IsArithmetic() const { return DFieldInfo::IsArithmetic(Cast().GetType()); }
	constexpr bool IsInt() const { return DFieldInfo::IsInt(Cast().GetType()); }
	constexpr bool IsFlt() const { return DFieldInfo::IsFlt(Cast().GetType()); }
	constexpr bool IsEmp() const { return DFieldInfo::IsEmp(Cast().GetType()); }


#define CODE(Tag, Name, Type)\
	constexpr bool Is##Tag() const { return DFieldInfo::Is##Tag(Cast().GetType()); }
	ADAPT_FOR_EACH_TYPE(CODE)
#undef CODE

	template <StaticChar Name_>
	constexpr std::tuple<StaticString<Name_>, Derived> named(StaticString<Name_> name) const&
	{
		return std::make_tuple(name, Cast());
	}
	template <StaticChar Name_>
	constexpr std::tuple<StaticString<Name_>, Derived> named(StaticString<Name_> name)&&
	{
		return std::make_tuple(name, Cast_move());
	}
	constexpr std::tuple<std::string, Derived> named(std::string_view name) const&
	{
		return std::tuple<std::string, Derived>(name, Cast());
	}
	constexpr std::tuple<std::string, Derived> named(std::string_view name)&&
	{
		return std::tuple<std::string, Derived>(name, Cast_move());
	}
};

template <placeholder Placeholder, class ...NPs>
auto MakeIndexedFieldNode(const Placeholder& ph, NPs&& ...indices);

template <class Placeholder>
struct CttiFieldMethods
{
public:

	template <DepthType Depth>
	auto outer(DepthConstant<Depth>) const
	{
		const Placeholder* self = static_cast<const Placeholder*>(this);
		return CttiOuterFieldNode<CttiFieldNode<Placeholder>, Depth>(*self);
	}
	template <DepthType Depth>
	auto o(DepthConstant<Depth> d) const { return outer(d); }
	template <class ...NP>
		requires ((node_or_placeholder<NP> || std::integral<NP>) && ...)
	auto at(NP&& ...indices) const
	{
		const Placeholder* self = static_cast<const Placeholder*>(this);
		return MakeIndexedFieldNode(*self, std::forward<NP>(indices)...);
	}
	template <class Traverser>
	decltype(auto) Evaluate(const Traverser& t) const
	{
		const Placeholder* self = static_cast<const Placeholder*>(this);
		return t[*self];
	}
	template <class Traverser>
	decltype(auto) operator()(const Traverser& t) const
	{
		const Placeholder* self = static_cast<const Placeholder*>(this);
		return t[*self];
	}
	template <class Container>
	decltype(auto) Evaluate(const Container& s, const Bpos& bpos) const
	{
		const Placeholder* self = static_cast<const Placeholder*>(this);
		return s.GetBranch(bpos, self->GetInternalLayer())[*self];
	}
};

template <class Placeholder>
struct RttiFieldMethods
{
public:

	auto outer(DepthType depth) const
	{
		const Placeholder* self = static_cast<const Placeholder*>(this);
		return RttiFieldNode<Placeholder>(*self, depth);
	}
	auto o(DepthType d) const { return outer(d); }
	auto o0() const { return outer(0); }
	auto o1() const { return outer(1); }
	auto o2() const { return outer(2); }
	auto o3() const { return outer(3); }
	auto o4() const { return outer(4); }
	auto o5() const { return outer(5); }

	template <class ...NP>
		requires ((node_or_placeholder<NP> || std::integral<NP>) && ...)
	auto at(NP&& ...indices) const
	{
		const Placeholder* self = static_cast<const Placeholder*>(this);
		return MakeIndexedFieldNode(*self, std::forward<NP>(indices)...);
	}

	template<FieldType Type>
	auto as() const
	{
		const Placeholder* self = static_cast<const Placeholder*>(this);
		if (Type != self->GetType()) throw MismatchType("");
		return self->template AddType<Type>();
	}
	template<class Type>
	auto as() const
	{
		constexpr FieldType tag = DFieldInfo::ValueTypeToTagType<Type>();
		return as<tag>();
	}

	template <any_traverser Traverser, FieldType Type>
	decltype(auto) Evaluate(const Traverser& t, Number<Type>) const
	{
		const Placeholder* self = static_cast<const Placeholder*>(this);
		return t[*self].template as<Type>();
	}
	template <any_traverser Traverser, FieldType Type>
	decltype(auto) operator()(const Traverser& t, Number<Type>) const
	{
		return Evaluate(t, Number<Type>{});
	}
	template <any_container Container, FieldType Type>
	decltype(auto) Evaluate(const Container& s, Number<Type>) const
	{
		const Placeholder* self = static_cast<const Placeholder*>(this);
		return s.GetBranch()[*self].template as<Type>();
	}
	template <any_container Container, FieldType Type>
	decltype(auto) Evaluate(const Container& s, const Bpos& bpos, Number<Type>) const
	{
		const Placeholder* self = static_cast<const Placeholder*>(this);
		return s.GetBranch(bpos, self->GetInternalLayer())[*self].template as<Type>();
	}

	template <any_traverser Traverser>
	decltype(auto) Evaluate(const Traverser& t) const
	{
		const Placeholder* self = static_cast<const Placeholder*>(this);
		return t[*self];
	}
	template <any_traverser Traverser>
	decltype(auto) operator()(const Traverser& t) const
	{
		const Placeholder* self = static_cast<const Placeholder*>(this);
		return t[*self];
	}
	template <any_container Container>
	decltype(auto) Evaluate(const Container& s) const
	{
		const Placeholder* self = static_cast<const Placeholder*>(this);
		return s.GetBranch()[*self];
	}
	template <any_container Container>
	decltype(auto) Evaluate(const Container& s, const Bpos& bpos) const
	{
		const Placeholder* self = static_cast<const Placeholder*>(this);
		return s.GetBranch(bpos, self->GetInternalLayer())[*self];
	}
};

template <RankType Rank_, class Container_, class Derived>
struct JointInfo
{
	static_assert(Rank_ == 0);
	static constexpr RankType Rank = 0;
	static constexpr RankType MaxRank = 0;
	using Container = Container_;

	constexpr bool operator==(const JointInfo&) const { return true; }

	static constexpr RankType GetRank() { return Rank; }
	static constexpr bool HasRank() { return false; }
};
template <RankType Rank_, d_joined_container Container_, class Derived>
struct JointInfo<Rank_, Container_, Derived>
{
public:
	static constexpr RankType Rank = Rank_;
	static constexpr RankType MaxRank = Container_::MaxRank;
	using Container = Container_;
	constexpr JointInfo() = default;
	constexpr JointInfo(JointLayerArray<MaxRank> j) : m_joint_layers(j) {}

	bool operator==(const JointInfo& that) const { return m_joint_layers == that.m_joint_layers; }

	static constexpr RankType GetRank() { return Rank; }
	static constexpr bool HasRank() { return true; }
	const JointLayerArray<MaxRank>& GetJointLayerArray() const { return m_joint_layers; }

private:
	JointLayerArray<MaxRank> m_joint_layers;
};
template <RankType Rank_, s_joined_container Container_, class Derived>
struct JointInfo<Rank_, Container_, Derived>
{
public:
	static constexpr RankType Rank = Rank_;
	static constexpr RankType MaxRank = Container_::MaxRank;
	using Container = Container_;
	constexpr JointInfo() = default;
	constexpr JointInfo(JointLayerArray<MaxRank>) {}

	static constexpr RankType GetRank() { return Rank; }
	static constexpr bool HasRank() { return true; }
	static constexpr JointLayerArray<MaxRank> GetJointLayerArray() { return JointLayers; }

private:
	static constexpr JointLayerArray<MaxRank> JointLayers = Container::JointLayers;
};

}

}

ADAPT_EXPORT
class FieldVariant : public eval::detail::RttiMethods<FieldVariant, std::type_identity_t>
{
	using enum FieldType;
	template <class Derived>
	struct IsBasedOnRtti
	{
	private:
		template <template <class> class Qua>
		static constexpr std::true_type check(const eval::detail::RttiMethods<Derived, Qua>*);
		static constexpr std::false_type check(const void*);

		static const Derived* d;
	public:
		static constexpr bool value = decltype(check(d))::value;
	};
public:

	FieldVariant() {}

	template <class RttiVar>
		requires IsBasedOnRtti<std::decay_t<RttiVar>>::value
	explicit FieldVariant(RttiVar&& v)
	{
		*this = std::forward<RttiVar>(v);
	}

	template <class Type>
		requires (DFieldInfo::ValueTypeToTagType<std::decay_t<Type>>() != Emp)
	explicit FieldVariant(Type&& var)
		: m_var(std::in_place_type<Type>, std::forward<Type>(var))
	{}

	FieldVariant(const FieldVariant&) = default;
	FieldVariant(FieldVariant&&) = default;

	FieldVariant& operator=(const FieldVariant&) = default;
	FieldVariant& operator=(FieldVariant&&) = default;

	template <class RttiVar>
		requires IsBasedOnRtti<std::decay_t<RttiVar>>::value
	FieldVariant& operator=(RttiVar&& v)
	{
#define CODE(T) m_var = std::forward<RttiVar>(v).template as_unsafe<T>();
		ADAPT_SWITCH_FIELD_TYPE(v.GetType(), CODE, throw MismatchType("");)
#undef CODE
		return *this;
	}
	template <class Type>
		requires (DFieldInfo::IsConvertibleToTagType<std::decay_t<Type>>())
	FieldVariant& operator=(Type&& var)
	{
		m_var = std::forward<Type>(var);
		return *this;
	}

	template <FieldType Type>
	DFieldInfo::TagTypeToValueType<Type>& as() &
	{
		return std::get<DFieldInfo::TagTypeToValueType<Type>>(m_var);
	}
	template <FieldType Type>
	const DFieldInfo::TagTypeToValueType<Type>& as() const &
	{
		return std::get<DFieldInfo::TagTypeToValueType<Type>>(m_var);
	}
	template <FieldType Type>
	DFieldInfo::TagTypeToValueType<Type>&& as() &&
	{
		return std::get<DFieldInfo::TagTypeToValueType<Type>>(m_var);
	}

	template <FieldType Type>
	DFieldInfo::TagTypeToValueType<Type>& as_unsafe()&
	{
		return as<Type>();
	}
	template <FieldType Type>
	const DFieldInfo::TagTypeToValueType<Type>& as_unsafe() const&
	{
		return as<Type>();
	}
	template <FieldType Type>
	DFieldInfo::TagTypeToValueType<Type>&& as_unsafe()&&
	{
		return std::move(*this).as<Type>();
	}

	template <FieldType Type>
	DFieldInfo::TagTypeToValueType<Type>* as_ptr() &
	{
		return std::get_if<DFieldInfo::TagTypeToValueType<Type>>(m_var);
	}
	template <FieldType Type>
	const DFieldInfo::TagTypeToValueType<Type>* as_ptr() const &
	{
		return std::get_if<DFieldInfo::TagTypeToValueType<Type>>(m_var);
	}

	// m_varはprivateなので、外部からは直接std::visitを使ってアクセスできない。
	// 代わりにVisit()を用意しておく。
	template <class Visitor>
	decltype(auto) Visit(Visitor&& v)
	{
		return std::visit(std::forward<Visitor>(v), m_var);
	}
	template <class Visitor>
	decltype(auto) Visit(Visitor&& v) const
	{
		return std::visit(std::forward<Visitor>(v), m_var);
	}

	FieldType GetType() const { return corr_table[m_var.index()]; }

	bool operator==(const FieldVariant& that) const
	{
		if (GetType() != that.GetType()) return false;
		auto visitor = [&that]<class T>(const T& v) -> bool
		{
			if constexpr (!std::same_as<T, std::nullptr_t>)
				return v == that.as<DFieldInfo::ValueTypeToTagType<T>()>();
			else throw MismatchType("");
		};
		return Visit(visitor);
	}
	template <class T>
		requires (!std::same_as<T, FieldVariant>&& DFieldInfo::IsConvertibleToTagType<T>())
	friend bool operator==(const FieldVariant& self, const T& that)
	{
		constexpr FieldType type = DFieldInfo::GetSameSizeTagType<T>();
		if (self.GetType() != type) return false;
		return self.as<type>() == that;
	}
	template <class T>
		requires (!std::same_as<T, FieldVariant>&& DFieldInfo::IsConvertibleToTagType<T>())
	friend bool operator==(const T& that, const FieldVariant& self)
	{
		return self == that;
	}
	template <class T>
		requires (!std::same_as<T, FieldVariant>&& DFieldInfo::IsConvertibleToTagType<T>())
	friend bool operator!=(const FieldVariant& self, const T& that)
	{
		return !(self == that);
	}
	template <class T>
		requires (!std::same_as<T, FieldVariant>&& DFieldInfo::IsConvertibleToTagType<T>())
	friend bool operator!=(const T& that, const FieldVariant& self)
	{
		return that != self;
	}

private:

	/*template <FieldType Type>
	static constexpr size_t FieldTypeToIndex()
	{
		for (size_t i = 0; i < sizeof(corr_table); ++i)
			if (corr_table[i] == Type) return i;
		return std::numeric_limits<size_t>::max();
	}*/

	void named() const {}//namedはおよそ使う機会がないはずなので隠蔽しておく。

#define CODE(Tag, Name, Type) Tag,
	static constexpr FieldType corr_table[] = {
		ADAPT_FOR_EACH_TYPE(CODE)
		Emp
	};
#undef CODE

#define CODE(Tag, Name, Type) Type,
	using Variant = std::variant<ADAPT_FOR_EACH_TYPE(CODE) std::nullptr_t>;
#undef CODE
	Variant m_var;
};

namespace detail
{

template <class T>
constexpr auto IsHashable(int) -> decltype(std::hash<T>{}, true) { return true; }
template <class T>
constexpr bool IsHashable(long) { return false; }
template <class T>
inline constexpr bool IsHashable_v = IsHashable<T>(1);

template <>
struct Hasher<std::tuple<FieldVariant>, 1>
{
	size_t operator()(const FieldVariant& key) const
	{
		auto calc_hash = []<class Field>(const Field& f) -> size_t
		{
			if constexpr (IsHashable_v<Field>) return std::hash<Field>{}(f);
			else throw MismatchType("");
		};
		return key.Visit(calc_hash);
	}
	size_t operator()(const std::tuple<FieldVariant>& key) const
	{
		return (*this)(std::get<0>(key));
	}
};


template <template <class> class Qualifier>
class FieldVarRef_impl : public eval::detail::RttiMethods<FieldVarRef_impl<Qualifier>, std::type_identity_t>
{
	using enum FieldType;
	template <class Derived>
	struct IsBasedOnRtti
	{
	private:
		template <template <class> class Qua>
		static constexpr std::true_type check(const eval::detail::RttiMethods<Derived, Qua>*);
		static constexpr std::false_type check(const void*);

		static const Derived* d;
	public:
		static constexpr bool value = decltype(check(d))::value;
	};

public:

	template <class RttiVar>
		requires IsBasedOnRtti<std::decay_t<RttiVar>>::value
	explicit FieldVarRef_impl(RttiVar& v)
	{
#define CODE(T) m_var = &v.template as_unsafe<T>();
		ADAPT_SWITCH_FIELD_TYPE(v.GetType(), CODE, throw MismatchType("");)
#undef CODE
	}

	template <class Type>
		requires (DFieldInfo::ValueTypeToTagType<Type>() != Emp)
	explicit FieldVarRef_impl(Type& var)
		: m_var(std::in_place_type<Qualifier<Type>*>, &var)
	{}

	template <FieldType Type>
		requires (std::is_const_v<Qualifier<char>> && Type != Emp)
	DFieldInfo::TagTypeToValueType<Type>& as()
	{
		return *std::get<FieldTypeToIndex<Type>()>(m_var);
	}
	template <FieldType Type>
		requires (Type != Emp)
	const DFieldInfo::TagTypeToValueType<Type>& as() const
	{
		return *std::get<FieldTypeToIndex<Type>()>(m_var);
	}

	template <FieldType Type>
		requires (std::is_const_v<Qualifier<char>> && Type != Emp)
	DFieldInfo::TagTypeToValueType<Type>* as_ptr()
	{
		return std::get_if<FieldTypeToIndex<Type>()>(m_var);
	}
	template <FieldType Type>
		requires (Type != Emp)
	const DFieldInfo::TagTypeToValueType<Type>* as_ptr() const
	{
		return std::get_if<FieldTypeToIndex<Type>()>(m_var);
	}

	FieldType GetType() const { return corr_table[m_var.index()]; }

private:

	template <FieldType Type>
	static constexpr size_t FieldTypeToIndex()
	{
		for (size_t i = 0; i < sizeof(corr_table); ++i)
			if (corr_table[i] == Type) return i;
		return std::numeric_limits<size_t>::max();
	}

	void named() const {}//namedはおよそ使う機会がないはずなので隠蔽しておく。

#define CODE(Tag, Name, Type) Tag,
	static constexpr FieldType corr_table[] = {
		ADAPT_FOR_EACH_TYPE(CODE)
		Emp
	};
#undef CODE

#define CODE(Tag, Name, Type) Qualifier<Type>*,
	using Variant = std::variant<ADAPT_FOR_EACH_TYPE(CODE) std::nullptr_t>;
#undef CODE
	Variant m_var;
};

}

ADAPT_EXPORT
using FieldVarRef = detail::FieldVarRef_impl<std::type_identity_t>;
ADAPT_EXPORT
using FieldVarCRef = detail::FieldVarRef_impl<std::add_const_t>;

template <class Derived, template <class> class Qualifier>
FieldVariant eval::detail::RttiMethods<Derived, Qualifier>::var() const
{
	return FieldVariant(*this);
}

namespace eval
{

ADAPT_EXPORT
template <RankType Rank_, LayerType Layer_, class Type, class Container_>
struct CttiPlaceholder_impl : public detail::CttiFieldMethods<CttiPlaceholder_impl<Rank_, Layer_, Type, Container_>>,
							  public detail::JointInfo<Rank_, Container_, CttiPlaceholder_impl<Rank_, Layer_, Type, Container_>>,
							  public detail::CttiMethods<CttiPlaceholder_impl<Rank_, Layer_, Type, Container_>, std::type_identity_t>
{
	//Containerは不完全型であることを前提に。
	using enum FieldType;
	using JointInfo = detail::JointInfo<Rank_, Container_, CttiPlaceholder_impl<Rank_, Layer_, Type, Container_>>;
public:

	static constexpr LayerType Layer = Layer_;
	using Container = Container_;
	using RetType = Type;

	constexpr CttiPlaceholder_impl()
		: m_index(0), m_ptr_offset(0)
	{}
	constexpr CttiPlaceholder_impl(uint16_t index, ptrdiff_t offs)
		: m_index(index), m_ptr_offset(offs)
	{}

	bool operator==(const CttiPlaceholder_impl& that) const
	{
		return m_index == that.m_index &&
			m_ptr_offset == that.m_ptr_offset &&
			JointInfo::operator==(that);
	}
	bool operator!=(const CttiPlaceholder_impl& that) const
	{
		return !(*this == that);
	}

	static constexpr FieldType GetType() { return DFieldInfo::GetSameSizeTagType<Type>(); }
	static constexpr LayerType GetInternalLayer() { return Layer; }
	constexpr uint16_t GetIndex() const { return m_index; }
	constexpr ptrdiff_t GetPtrOffset() const { return m_ptr_offset; }

	static constexpr LayerType GetLayer()
	{
		if constexpr (container_simplex<Container>) return GetInternalLayer();
		else
		{
			auto eli = LayerInfo<JointInfo::MaxRank>(JointInfo::GetJointLayerArray());
			eli.template Enable<Rank_>();
			eli.template SetTravLayer<Rank_>(GetInternalLayer());
			return eli.GetTravLayer();
		}
	}

	constexpr auto Derank() const requires joined_container<Container>
	{
		using IContainer = Container::template IContainer<Rank_>;
		return CttiPlaceholder_impl<0, Layer, Type, IContainer>(m_index, m_ptr_offset);
	}

private:

	uint16_t m_index;
	ptrdiff_t m_ptr_offset;
};

ADAPT_EXPORT
template <RankType Rank_, class Type_, class Container_>
struct TypedPlaceholder_impl : public detail::CttiFieldMethods<TypedPlaceholder_impl<Rank_, Type_, Container_>>,
							   public detail::JointInfo<Rank_, Container_, TypedPlaceholder_impl<Rank_, Type_, Container_>>,
							   public detail::CttiMethods<TypedPlaceholder_impl<Rank_, Type_, Container_>, std::type_identity_t>
{
	//Rtti、CttiPlaceholderの中間のような特殊なPlaceholder。
	//通常、d_containerのRttiPlaceholderに対してas<I32>()のように呼び出すことでTypedPlaceholderに変換する。

	using enum FieldType;
	using JointInfo = detail::JointInfo<Rank_, Container_, TypedPlaceholder_impl<Rank_, Type_, Container_>>;
public:
	using Container = Container_;
	using RetType = Type_;

	constexpr TypedPlaceholder_impl()
		: m_layer(-1), m_index(0), m_ptr_offset(0)
	{}
	constexpr TypedPlaceholder_impl(LayerType layer, uint16_t index, ptrdiff_t offs)
		requires (container_simplex<Container>)
		: m_layer(layer), m_index(index), m_ptr_offset(offs)
	{}
	constexpr TypedPlaceholder_impl(LayerType layer, uint16_t index, ptrdiff_t offs,
								   JointLayerArray<JointInfo::MaxRank> j)
		requires (joined_container<Container>)
		: JointInfo(j), m_layer(layer), m_index(index), m_ptr_offset(offs)
	{}

	bool operator==(const TypedPlaceholder_impl& that) const
	{
		return m_layer == that.m_layer &&
			m_index == that.m_index &&
			m_ptr_offset == that.m_ptr_offset &&
			JointInfo::operator==(that);
	}
	bool operator!=(const TypedPlaceholder_impl& that) const
	{
		return !(*this == that);
	}

	constexpr LayerType GetInternalLayer() const { return m_layer; }
	constexpr uint16_t GetIndex() const { return m_index; }
	constexpr ptrdiff_t GetPtrOffset() const { return m_ptr_offset; }

	static constexpr FieldType GetType() { return DFieldInfo::ValueTypeToTagType<RetType>(); }

	constexpr LayerType GetLayer() const
	{
		if constexpr (container_simplex<Container>) return GetInternalLayer();
		else
		{
			auto eli = LayerInfo<JointInfo::MaxRank>(this->GetJointLayerArray());
			eli.template Enable<Rank_>();
			eli.template SetTravLayer<Rank_>(GetInternalLayer());
			return eli.GetTravLayer();
		}
	}

	template <LayerType Layer>
	constexpr auto AddLayer() const
	{
		return CttiPlaceholder_impl<Rank_, Layer, RetType, Container>(m_index, m_ptr_offset);
	}

	constexpr auto Derank() const requires joined_container<Container>
	{
		using IContainer = Container::template IContainer<Rank_>;
		return TypedPlaceholder_impl<0, RetType, IContainer>(m_layer, m_index, m_ptr_offset);
	}

private:

	LayerType m_layer;//グローバル変数は階層が-1になる。
	uint16_t m_index;
	ptrdiff_t m_ptr_offset;
};

ADAPT_EXPORT
template <RankType Rank_, class Container_>
struct RttiPlaceholder_impl : public detail::RttiFieldMethods<RttiPlaceholder_impl<Rank_, Container_>>,
	public detail::RttiMethods<RttiPlaceholder_impl<Rank_, Container_>, std::type_identity_t>,
	public detail::JointInfo<Rank_, Container_, RttiPlaceholder_impl<Rank_, Container_>>
{
	//Containerは不完全型であることを前提に。
	using enum FieldType;
	using JointInfo = detail::JointInfo<Rank_, Container_, RttiPlaceholder_impl<Rank_, Container_>>;
public:
	using Container = Container_;

	constexpr RttiPlaceholder_impl()
		: m_layer(-1), m_index(0), m_type(FieldType::Emp), m_ptr_offset(0)
	{}
	constexpr RttiPlaceholder_impl(LayerType layer, uint16_t index, FieldType type, ptrdiff_t offs)
		requires (container_simplex<Container>)
	: m_layer(layer), m_index(index), m_type(type), m_ptr_offset(offs)
	{}
	constexpr RttiPlaceholder_impl(LayerType layer, uint16_t index, FieldType type, ptrdiff_t offs,
								   JointLayerArray<JointInfo::MaxRank> j)
		requires (joined_container<Container>)
		: JointInfo(j), m_layer(layer), m_index(index), m_type(type), m_ptr_offset(offs)
	{}

	bool operator==(const RttiPlaceholder_impl& that) const
	{
		return m_layer == that.m_layer &&
			m_index == that.m_index &&
			m_type == that.m_type &&
			m_ptr_offset == that.m_ptr_offset &&
			JointInfo::operator==(that);
	}
	bool operator!=(const RttiPlaceholder_impl& that) const
	{
		return !(*this == that);
	}

	constexpr LayerType GetInternalLayer() const { return m_layer; }
	constexpr uint16_t GetIndex() const { return m_index; }
	constexpr FieldType GetType() const { return m_type; }

	constexpr ptrdiff_t GetPtrOffset() const { return m_ptr_offset; }

	constexpr LayerType GetLayer() const
	{
		if constexpr (container_simplex<Container>) return GetInternalLayer();
		else
		{
			auto eli = LayerInfo<JointInfo::MaxRank>(this->GetJointLayerArray());
			eli.template Enable<Rank_>();
			eli.template SetTravLayer<Rank_>(GetInternalLayer());
			return eli.GetTravLayer();
		}
	}

	template <FieldType Type>
	constexpr auto AddType() const
	{
		if (GetType() != Type)
			throw MismatchType(std::format("The type of the placeholder is {}, not {}",
				DFieldInfo::GetTagTypeString(GetType()),
				(const char*)DFieldInfo::GetTagTypeString<Type>()));
		using ValueType = DFieldInfo::TagTypeToValueType<Type>;
		if constexpr (ranked_placeholder<RttiPlaceholder_impl>)
			return TypedPlaceholder_impl<Rank_, ValueType, Container_>(GetInternalLayer(), GetIndex(), GetPtrOffset(), this->GetJointLayerArray());
		else
			return TypedPlaceholder_impl<Rank_, ValueType, Container_>(GetInternalLayer(), GetIndex(), GetPtrOffset());
	}
	template <LayerType Layer, FieldType Type>
	constexpr auto AddCttiInfo() const
	{
		if (GetType() != Type)
			throw MismatchType(std::format("The type of the placeholder is {}, not {}",
				DFieldInfo::GetTagTypeString(GetType()),
				(const char*)DFieldInfo::GetTagTypeString<Type>()));
		if (GetInternalLayer() != Layer)
			throw MismatchType(std::format("The layer of the placeholder is {}, not {}", GetInternalLayer(), Layer));
		using ValueType = DFieldInfo::TagTypeToValueType<Type>;
		return CttiPlaceholder_impl<Rank_, Layer, ValueType, Container_>(GetIndex(), GetPtrOffset());
	}

	constexpr auto Derank() const requires joined_container<Container>
	{
		using IContainer = Container::template IContainer<Rank_>;
		return RttiPlaceholder_impl<0, IContainer>(m_layer, m_index, m_type, m_ptr_offset);
	}

private:

	LayerType m_layer;//グローバル変数は階層が-1になる。
	uint16_t m_index;
	FieldType m_type;
	ptrdiff_t m_ptr_offset;
};

/*template <FieldType Type>
struct TypedName
{
	TypedName(std::string_view name) : m_name(name) {}
	static constexpr FieldType m_type = Type;
	std::string_view m_name;
};*/

ADAPT_EXPORT
template <class Container>
using RttiPlaceholder = RttiPlaceholder_impl<0, Container>;
ADAPT_EXPORT
template <class Type, class Container>
using TypedPlaceholder = TypedPlaceholder_impl<0, Type, Container>;
ADAPT_EXPORT
template <LayerType Layer, class Type, class Container>
using CttiPlaceholder = CttiPlaceholder_impl<0, Layer, Type, Container>;

ADAPT_EXPORT
template <RankType Rank, class Container>
using RankedRttiPlaceholder = RttiPlaceholder_impl<Rank, Container>;
ADAPT_EXPORT
template <RankType Rank, class Type, class Container>
using RankedTypedPlaceholder = TypedPlaceholder_impl<Rank, Type, Container>;
ADAPT_EXPORT
template <RankType Rank, LayerType Layer, class Type, class Container>
using RankedCttiPlaceholder = CttiPlaceholder_impl<Rank, Layer, Type, Container>;

}

inline namespace lit
{

ADAPT_EXPORT
template <StaticChar Name>
consteval auto operator""_fld()
{
	return StaticString<Name>();
}

}

}

#endif
