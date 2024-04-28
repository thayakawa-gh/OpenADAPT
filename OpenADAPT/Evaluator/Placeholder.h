#ifndef ADAPT_EVALUATOR_PLACEHOLDER_H
#define ADAPT_EVALUATOR_PLACEHOLDER_H

#include <variant>
#include <OpenADAPT/Utility/NamedTuple.h>
#include <OpenADAPT/Joint/LayerInfo.h>
#include <OpenADAPT/Common/Concepts.h>

namespace adapt
{

struct DFieldInfo
{
	using enum FieldType;
private:
	template <FieldType Tag, std::nullptr_t = nullptr>
	struct TagTypeToValueType_impl;
	template <std::nullptr_t N> struct TagTypeToValueType_impl<I08, N> { using Type = int8_t; };
	template <std::nullptr_t N> struct TagTypeToValueType_impl<I16, N> { using Type = int16_t; };
	template <std::nullptr_t N> struct TagTypeToValueType_impl<I32, N> { using Type = int32_t; };
	template <std::nullptr_t N> struct TagTypeToValueType_impl<I64, N> { using Type = int64_t; };
	template <std::nullptr_t N> struct TagTypeToValueType_impl<F32, N> { using Type = float; };
	template <std::nullptr_t N> struct TagTypeToValueType_impl<F64, N> { using Type = double; };
	template <std::nullptr_t N> struct TagTypeToValueType_impl<C32, N> { using Type = std::complex<float>; };
	template <std::nullptr_t N> struct TagTypeToValueType_impl<C64, N> { using Type = std::complex<double>; };
	template <std::nullptr_t N> struct TagTypeToValueType_impl<Str, N> { using Type = std::string; };
	template <std::nullptr_t N> struct TagTypeToValueType_impl<Jbp, N> { using Type = JBpos; };

	template <class Type, size_t Size>
	inline static constexpr bool is_integral_with_size = std::is_integral_v<Type> && sizeof(Type) == Size;
public:

	template <FieldType Tag>
	using TagTypeToValueType = typename TagTypeToValueType_impl<Tag>::Type;
	template <class ValueType>
	static constexpr FieldType ValueTypeToTagType()
	{
		if constexpr (std::is_same_v<ValueType, int8_t>) return I08;
		else if constexpr (std::is_same_v<ValueType, int16_t>) return I16;
		else if constexpr (std::is_same_v<ValueType, int32_t>) return I32;
		else if constexpr (std::is_same_v<ValueType, int64_t>) return I64;
		else if constexpr (std::is_same_v<ValueType, float>) return F32;
		else if constexpr (std::is_same_v<ValueType, double>) return F64;
		else if constexpr (std::is_same_v<ValueType, std::complex<float>>) return C32;
		else if constexpr (std::is_same_v<ValueType, std::complex<double>>) return C64;
		else if constexpr (std::is_same_v<ValueType, std::string>) return Str;
		else if constexpr (std::is_same_v<ValueType, JBpos>) return Jbp;
		else return Emp;
	}

	template <class T>
	static constexpr FieldType GetSameSizeTagType()
	{
		if constexpr (IsI08<T>()) return I08;
		else if constexpr (IsI16<T>()) return I16;
		else if constexpr (IsI32<T>()) return I32;
		else if constexpr (IsI64<T>()) return I64;
		else if constexpr (IsF32<T>()) return F32;
		else if constexpr (IsF64<T>()) return F64;
		else if constexpr (IsC32<T>()) return C32;
		else if constexpr (IsC64<T>()) return C64;
		else if constexpr (IsStr<T>()) return Str;
		else if constexpr (IsJbp<T>()) return Jbp;
		else return Emp;
	}

	//GetSameSizeTagType() != Emp、つまり
	//* 整数型なら1、2、4、8バイトである
	//* float、double、std::complex<float>、std::complex<double>のいずれかである
	//* std::stringかJBposに変換可能である
	//いずれかを満たすようなら、この関数はtrueを返す。
	template <class T>
	static constexpr bool IsConvertibleToTagType()
	{
		return GetSameSizeTagType<T>() != Emp;
	}

	template <FieldType Type>
	static constexpr auto GetTagTypeString()
	{
		if constexpr (Type == I08) return StaticString<"I08">{};
		else if constexpr (Type == I16) return StaticString<"I16">{};
		else if constexpr (Type == I32) return StaticString<"I32">{};
		else if constexpr (Type == I64) return StaticString<"I64">{};
		else if constexpr (Type == F32) return StaticString<"F32">{};
		else if constexpr (Type == F64) return StaticString<"F64">{};
		else if constexpr (Type == C32) return StaticString<"C32">{};
		else if constexpr (Type == C64) return StaticString<"C64">{};
		else if constexpr (Type == Str) return StaticString<"Str">{};
		else if constexpr (Type == Jbp) return StaticString<"Jbp">{};
		else if constexpr (Type == Emp) return StaticString<"Emp">{};
	}
	static std::string GetTagTypeString(FieldType Type)
	{
		if (Type == I08) return std::string("I08");
		else if (Type == I16) return std::string("I16");
		else if (Type == I32) return std::string("I32");
		else if (Type == I64) return std::string("I64");
		else if (Type == F32) return std::string("F32");
		else if (Type == F64) return std::string("F64");
		else if (Type == C32) return std::string("C32");
		else if (Type == C64) return std::string("C64");
		else if (Type == Str) return std::string("Str");
		else if (Type == Jbp) return std::string("Jbp");
		else if (Type == Emp) return std::string("Emp");
		throw MismatchType("");
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
	static constexpr bool IsNum() { return std::is_arithmetic_v<T>; }

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

	template <class T>
	static constexpr bool IsC32() { return std::is_same_v<T, std::complex<float>>; }
	template <class T>
	static constexpr bool IsC64() { return std::is_same_v<T, std::complex<double>>; }

	template <class T>
	static constexpr bool IsStr() { return std::is_convertible_v<T, std::string>; }
	template <class T>
	static constexpr bool IsJbp() { return std::is_convertible_v<T, JBpos>; }


	static constexpr bool IsTrivial(FieldType type) { return (uint32_t)type & FieldTypeFlag::TRIVIAL; }

	static constexpr bool IsNum(FieldType type) { return (uint32_t)type & FieldTypeFlag::NUM; }

	static constexpr bool IsInt(FieldType type) { return (uint32_t)type & FieldTypeFlag::INT; }
	static constexpr bool IsI08(FieldType type) { return type == I08; }
	static constexpr bool IsI16(FieldType type) { return type == I16; }
	static constexpr bool IsI32(FieldType type) { return type == I32; }
	static constexpr bool IsI64(FieldType type) { return type == I64; }

	static constexpr bool IsFlt(FieldType type) { return (uint32_t)type & FieldTypeFlag::FLT; }
	static constexpr bool IsF32(FieldType type) { return type == F32; }
	static constexpr bool IsF64(FieldType type) { return type == F64; }

	static constexpr bool IsCpx(FieldType type) { return (uint32_t)type & FieldTypeFlag::CPX; }
	static constexpr bool IsC32(FieldType type) { return type == C32; }
	static constexpr bool IsC64(FieldType type) { return type == C64; }

	static constexpr bool IsStr(FieldType type) { return type == Str; }
	static constexpr bool IsJbp(FieldType type) { return type == Jbp; }

	static constexpr bool IsEmp(FieldType type) { return type == Emp; }
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

	static constexpr bool IsNum() { return DFieldInfo::IsNum<typename Derived::RetType>(); }

	static constexpr bool IsInt() { return DFieldInfo::IsInt<typename Derived::RetType>(); }

	static constexpr bool IsI08() { return DFieldInfo::IsI08<typename Derived::RetType>(); }
	static constexpr bool IsI16() { return DFieldInfo::IsI16<typename Derived::RetType>(); }
	static constexpr bool IsI32() { return DFieldInfo::IsI32<typename Derived::RetType>(); }
	static constexpr bool IsI64() { return DFieldInfo::IsI64<typename Derived::RetType>(); }

	static constexpr bool IsF32() { return DFieldInfo::IsF32<typename Derived::RetType>(); }
	static constexpr bool IsF64() { return DFieldInfo::IsF64<typename Derived::RetType>(); }

	static constexpr bool IsC32() { return DFieldInfo::IsC32<typename Derived::RetType>(); }
	static constexpr bool IsC64() { return DFieldInfo::IsC64<typename Derived::RetType>(); }

	static constexpr bool IsStr() { return DFieldInfo::IsStr<typename Derived::RetType>(); }
	static constexpr bool IsJbp() { return DFieldInfo::IsJbp<typename Derived::RetType>(); }

	//static constexpr bool IsEmp() const { return false; }

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

	decltype(auto) i08() const& { return Cast().template as<FieldType::I08>(); }
	decltype(auto) i16() const& { return Cast().template as<FieldType::I16>(); }
	decltype(auto) i32() const& { return Cast().template as<FieldType::I32>(); }
	decltype(auto) i64() const& { return Cast().template as<FieldType::I64>(); }
	decltype(auto) f32() const& { return Cast().template as<FieldType::F32>(); }
	decltype(auto) f64() const& { return Cast().template as<FieldType::F64>(); }
	decltype(auto) c32() const& { return Cast().template as<FieldType::C32>(); }
	decltype(auto) c64() const& { return Cast().template as<FieldType::C64>(); }
	decltype(auto) str() const& { return Cast().template as<FieldType::Str>(); }
	decltype(auto) jbp() const& { return Cast().template as<FieldType::Jbp>(); }

	decltype(auto) i08()& requires (!std::is_const_v<Qualifier<char>>) { return Cast().template as<FieldType::I08>(); }
	decltype(auto) i16()& requires (!std::is_const_v<Qualifier<char>>) { return Cast().template as<FieldType::I16>(); }
	decltype(auto) i32()& requires (!std::is_const_v<Qualifier<char>>) { return Cast().template as<FieldType::I32>(); }
	decltype(auto) i64()& requires (!std::is_const_v<Qualifier<char>>) { return Cast().template as<FieldType::I64>(); }
	decltype(auto) f32()& requires (!std::is_const_v<Qualifier<char>>) { return Cast().template as<FieldType::F32>(); }
	decltype(auto) f64()& requires (!std::is_const_v<Qualifier<char>>) { return Cast().template as<FieldType::F64>(); }
	decltype(auto) c32()& requires (!std::is_const_v<Qualifier<char>>) { return Cast().template as<FieldType::C32>(); }
	decltype(auto) c64()& requires (!std::is_const_v<Qualifier<char>>) { return Cast().template as<FieldType::C64>(); }
	decltype(auto) str()& requires (!std::is_const_v<Qualifier<char>>) { return Cast().template as<FieldType::Str>(); }
	decltype(auto) jbp()& requires (!std::is_const_v<Qualifier<char>>) { return Cast().template as<FieldType::Jbp>(); }

	decltype(auto) i08() && requires (!std::is_const_v<Qualifier<char>>) { return Cast_move().template as<FieldType::I08>(); }
	decltype(auto) i16() && requires (!std::is_const_v<Qualifier<char>>) { return Cast_move().template as<FieldType::I16>(); }
	decltype(auto) i32() && requires (!std::is_const_v<Qualifier<char>>) { return Cast_move().template as<FieldType::I32>(); }
	decltype(auto) i64() && requires (!std::is_const_v<Qualifier<char>>) { return Cast_move().template as<FieldType::I64>(); }
	decltype(auto) f32() && requires (!std::is_const_v<Qualifier<char>>) { return Cast_move().template as<FieldType::F32>(); }
	decltype(auto) f64() && requires (!std::is_const_v<Qualifier<char>>) { return Cast_move().template as<FieldType::F64>(); }
	decltype(auto) c32() && requires (!std::is_const_v<Qualifier<char>>) { return Cast_move().template as<FieldType::C32>(); }
	decltype(auto) c64() && requires (!std::is_const_v<Qualifier<char>>) { return Cast_move().template as<FieldType::C64>(); }
	decltype(auto) str() && requires (!std::is_const_v<Qualifier<char>>) { return Cast_move().template as<FieldType::Str>(); }
	decltype(auto) jbp() && requires (!std::is_const_v<Qualifier<char>>) { return Cast_move().template as<FieldType::Jbp>(); }

	decltype(auto) i08_unsafe() const& { return Cast().template as_unsafe<FieldType::I08>(); }
	decltype(auto) i16_unsafe() const& { return Cast().template as_unsafe<FieldType::I16>(); }
	decltype(auto) i32_unsafe() const& { return Cast().template as_unsafe<FieldType::I32>(); }
	decltype(auto) i64_unsafe() const& { return Cast().template as_unsafe<FieldType::I64>(); }
	decltype(auto) f32_unsafe() const& { return Cast().template as_unsafe<FieldType::F32>(); }
	decltype(auto) f64_unsafe() const& { return Cast().template as_unsafe<FieldType::F64>(); }
	decltype(auto) c32_unsafe() const& { return Cast().template as_unsafe<FieldType::C32>(); }
	decltype(auto) c64_unsafe() const& { return Cast().template as_unsafe<FieldType::C64>(); }
	decltype(auto) str_unsafe() const& { return Cast().template as_unsafe<FieldType::Str>(); }
	decltype(auto) jbp_unsafe() const& { return Cast().template as_unsafe<FieldType::Jbp>(); }

	decltype(auto) i08_unsafe()& requires (!std::is_const_v<Qualifier<char>>) { return Cast().template as_unsafe<FieldType::I08>(); }
	decltype(auto) i16_unsafe()& requires (!std::is_const_v<Qualifier<char>>) { return Cast().template as_unsafe<FieldType::I16>(); }
	decltype(auto) i32_unsafe()& requires (!std::is_const_v<Qualifier<char>>) { return Cast().template as_unsafe<FieldType::I32>(); }
	decltype(auto) i64_unsafe()& requires (!std::is_const_v<Qualifier<char>>) { return Cast().template as_unsafe<FieldType::I64>(); }
	decltype(auto) f32_unsafe()& requires (!std::is_const_v<Qualifier<char>>) { return Cast().template as_unsafe<FieldType::F32>(); }
	decltype(auto) f64_unsafe()& requires (!std::is_const_v<Qualifier<char>>) { return Cast().template as_unsafe<FieldType::F64>(); }
	decltype(auto) c32_unsafe()& requires (!std::is_const_v<Qualifier<char>>) { return Cast().template as_unsafe<FieldType::C32>(); }
	decltype(auto) c64_unsafe()& requires (!std::is_const_v<Qualifier<char>>) { return Cast().template as_unsafe<FieldType::C64>(); }
	decltype(auto) str_unsafe()& requires (!std::is_const_v<Qualifier<char>>) { return Cast().template as_unsafe<FieldType::Str>(); }
	decltype(auto) jbp_unsafe()& requires (!std::is_const_v<Qualifier<char>>) { return Cast().template as_unsafe<FieldType::Jbp>(); }

	decltype(auto) i08_unsafe() && requires (!std::is_const_v<Qualifier<char>>) { return Cast_move().template as_unsafe<FieldType::I08>(); }
	decltype(auto) i16_unsafe() && requires (!std::is_const_v<Qualifier<char>>) { return Cast_move().template as_unsafe<FieldType::I16>(); }
	decltype(auto) i32_unsafe() && requires (!std::is_const_v<Qualifier<char>>) { return Cast_move().template as_unsafe<FieldType::I32>(); }
	decltype(auto) i64_unsafe() && requires (!std::is_const_v<Qualifier<char>>) { return Cast_move().template as_unsafe<FieldType::I64>(); }
	decltype(auto) f32_unsafe() && requires (!std::is_const_v<Qualifier<char>>) { return Cast_move().template as_unsafe<FieldType::F32>(); }
	decltype(auto) f64_unsafe() && requires (!std::is_const_v<Qualifier<char>>) { return Cast_move().template as_unsafe<FieldType::F64>(); }
	decltype(auto) c32_unsafe() && requires (!std::is_const_v<Qualifier<char>>) { return Cast_move().template as_unsafe<FieldType::C32>(); }
	decltype(auto) c64_unsafe() && requires (!std::is_const_v<Qualifier<char>>) { return Cast_move().template as_unsafe<FieldType::C64>(); }
	decltype(auto) str_unsafe() && requires (!std::is_const_v<Qualifier<char>>) { return Cast_move().template as_unsafe<FieldType::Str>(); }
	decltype(auto) jbp_unsafe() && requires (!std::is_const_v<Qualifier<char>>) { return Cast_move().template as_unsafe<FieldType::Jbp>(); }

	decltype(auto) i08_ptr() const { return Cast().template as_ptr<FieldType::I08>(); }
	decltype(auto) i16_ptr() const { return Cast().template as_ptr<FieldType::I16>(); }
	decltype(auto) i32_ptr() const { return Cast().template as_ptr<FieldType::I32>(); }
	decltype(auto) i64_ptr() const { return Cast().template as_ptr<FieldType::I64>(); }
	decltype(auto) f32_ptr() const { return Cast().template as_ptr<FieldType::F32>(); }
	decltype(auto) f64_ptr() const { return Cast().template as_ptr<FieldType::F64>(); }
	decltype(auto) c32_ptr() const { return Cast().template as_ptr<FieldType::C32>(); }
	decltype(auto) c64_ptr() const { return Cast().template as_ptr<FieldType::C64>(); }
	decltype(auto) str_ptr() const { return Cast().template as_ptr<FieldType::Str>(); }
	decltype(auto) jbp_ptr() const { return Cast().template as_ptr<FieldType::Jbp>(); }

	decltype(auto) i08_ptr() requires (!std::is_const_v<Qualifier<char>>) { return Cast().template as_ptr<FieldType::I08>(); }
	decltype(auto) i16_ptr() requires (!std::is_const_v<Qualifier<char>>) { return Cast().template as_ptr<FieldType::I16>(); }
	decltype(auto) i32_ptr() requires (!std::is_const_v<Qualifier<char>>) { return Cast().template as_ptr<FieldType::I32>(); }
	decltype(auto) i64_ptr() requires (!std::is_const_v<Qualifier<char>>) { return Cast().template as_ptr<FieldType::I64>(); }
	decltype(auto) f32_ptr() requires (!std::is_const_v<Qualifier<char>>) { return Cast().template as_ptr<FieldType::F32>(); }
	decltype(auto) f64_ptr() requires (!std::is_const_v<Qualifier<char>>) { return Cast().template as_ptr<FieldType::F64>(); }
	decltype(auto) c32_ptr() requires (!std::is_const_v<Qualifier<char>>) { return Cast().template as_ptr<FieldType::C32>(); }
	decltype(auto) c64_ptr() requires (!std::is_const_v<Qualifier<char>>) { return Cast().template as_ptr<FieldType::C64>(); }
	decltype(auto) str_ptr() requires (!std::is_const_v<Qualifier<char>>) { return Cast().template as_ptr<FieldType::Str>(); }
	decltype(auto) jbp_ptr() requires (!std::is_const_v<Qualifier<char>>) { return Cast().template as_ptr<FieldType::Jbp>(); }

	FieldVariant var() const;

	int8_t to_i08() const { return Cast().template to<FieldType::I08>(); }
	int16_t to_i16() const { return Cast().template to<FieldType::I16>(); }
	int32_t to_i32() const { return Cast().template to<FieldType::I32>(); }
	int64_t to_i64() const { return Cast().template to<FieldType::I64>(); }
	float to_f32() const { return Cast().template to<FieldType::F32>(); }
	double to_f64() const { return Cast().template to<FieldType::F64>(); }
	std::complex<float> to_c32() const { return Cast().template to<FieldType::C32>(); }
	std::complex<double> to_c64() const { return Cast().template to<FieldType::C64>(); }
	const std::string& to_str() const { return str(); }
	const JBpos& to_jbp() const { return jbp(); }

	constexpr size_t GetSize() const { return DFieldInfo::GetSizeOf(Cast().GetType()); }
	constexpr size_t GetAlign() const { return DFieldInfo::GetAlignOf(Cast().GetType()); }

	constexpr bool IsTrivial() const { return DFieldInfo::IsTrivial(Cast().GetType()); }

	constexpr bool IsNum() const { return DFieldInfo::IsNum(Cast().GetType()); }

	constexpr bool IsInt() const { return DFieldInfo::IsInt(Cast().GetType()); }
	constexpr bool IsI08() const { return DFieldInfo::IsI08(Cast().GetType()); }
	constexpr bool IsI16() const { return DFieldInfo::IsI16(Cast().GetType()); }
	constexpr bool IsI32() const { return DFieldInfo::IsI32(Cast().GetType()); }
	constexpr bool IsI64() const { return DFieldInfo::IsI64(Cast().GetType()); }

	constexpr bool IsFlt() const { return DFieldInfo::IsFlt(Cast().GetType()); }
	constexpr bool IsF32() const { return DFieldInfo::IsF32(Cast().GetType()); }
	constexpr bool IsF64() const { return DFieldInfo::IsF64(Cast().GetType()); }

	constexpr bool IsCpx() const { return DFieldInfo::IsCpx(Cast().GetType()); }
	constexpr bool IsC32() const { return DFieldInfo::IsC32(Cast().GetType()); }
	constexpr bool IsC64() const { return DFieldInfo::IsC64(Cast().GetType()); }

	constexpr bool IsStr() const { return DFieldInfo::IsStr(Cast().GetType()); }
	constexpr bool IsJbp() const { return DFieldInfo::IsJbp(Cast().GetType()); }

	constexpr bool IsEmp() const { return DFieldInfo::IsEmp(Cast().GetType()); }

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
		return std::get<FieldTypeToIndex<Type>()>(m_var);
	}
	template <FieldType Type>
	const DFieldInfo::TagTypeToValueType<Type>& as() const &
	{
		return std::get<FieldTypeToIndex<Type>()>(m_var);
	}
	template <FieldType Type>
	DFieldInfo::TagTypeToValueType<Type>&& as() &&
	{
		return std::get<FieldTypeToIndex<Type>()>(m_var);
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
		return std::get_if<FieldTypeToIndex<Type>()>(m_var);
	}
	template <FieldType Type>
	const DFieldInfo::TagTypeToValueType<Type>* as_ptr() const &
	{
		return std::get_if<FieldTypeToIndex<Type>()>(m_var);
	}

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
		auto visitor = [&that]<class T>(const T & v)
		{
			if constexpr (!std::same_as<T, JBpos>)
				return v == that.as<DFieldInfo::ValueTypeToTagType<T>()>();
			else
				return v.MatchPerfectly(that.as<DFieldInfo::Jbp>());
		};
		return Visit(visitor);
	}
	template <class T>
		requires (!std::same_as<T, FieldVariant>&& DFieldInfo::IsConvertibleToTagType<T>())
	friend bool operator==(const FieldVariant& self, const T& that)
	{
		constexpr FieldType type = DFieldInfo::GetSameSizeTagType<T>();
		if (self.GetType() != type) return false;
		if constexpr (!std::same_as<T, JBpos>)
			return self.as<type>() == that;
		else
			return self.as<FieldType::Jbp>().MatchPerfectly(that);
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

	template <FieldType Type>
	static constexpr size_t FieldTypeToIndex()
	{
		for (size_t i = 0; i < sizeof(corr_table); ++i)
			if (corr_table[i] == Type) return i;
		return std::numeric_limits<size_t>::max();
	}

	void named() const {}//namedはおよそ使う機会がないはずなので隠蔽しておく。

	static constexpr FieldType corr_table[] = {I08, I16, I32, I64, F32, F64, C32, C64, Str, Jbp, Emp};
	std::variant<int8_t, int16_t, int32_t, int64_t, float, double, std::complex<float>, std::complex<double>, std::string, JBpos> m_var;
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

	static constexpr FieldType corr_table[] = { I08, I16, I32, I64, F32, F64, C32, C64, Str, Jbp, Emp };
	std::variant<Qualifier<int8_t>*, Qualifier<int16_t>*, Qualifier<int32_t>*, Qualifier<int64_t>*,
		Qualifier<float>*, Qualifier<double>*, Qualifier<std::complex<float>>*, Qualifier<std::complex<double>>*,
		Qualifier<std::string>*, Qualifier<JBpos>*> m_var;
};

}

using FieldVarRef = detail::FieldVarRef_impl<std::type_identity_t>;
using FieldVarCRef = detail::FieldVarRef_impl<std::add_const_t>;

template <class Derived, template <class> class Qualifier>
FieldVariant eval::detail::RttiMethods<Derived, Qualifier>::var() const
{
	return FieldVariant(*this);
}

namespace eval
{

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

template <class Container>
using RttiPlaceholder = RttiPlaceholder_impl<0, Container>;
template <class Type, class Container>
using TypedPlaceholder = TypedPlaceholder_impl<0, Type, Container>;
template <LayerType Layer, class Type, class Container>
using CttiPlaceholder = CttiPlaceholder_impl<0, Layer, Type, Container>;

template <RankType Rank, class Container>
using RankedRttiPlaceholder = RttiPlaceholder_impl<Rank, Container>;
template <RankType Rank, class Type, class Container>
using RankedTypedPlaceholder = TypedPlaceholder_impl<Rank, Type, Container>;
template <RankType Rank, LayerType Layer, class Type, class Container>
using RankedCttiPlaceholder = CttiPlaceholder_impl<Rank, Layer, Type, Container>;

}

inline namespace lit
{

template <StaticChar Name>
consteval auto operator""_fld()
{
	return StaticString<Name>();
}

}

}

#endif
