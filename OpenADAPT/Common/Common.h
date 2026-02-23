#ifndef ADAPT_COMMOM_COMMON_H
#define ADAPT_COMMOM_COMMON_H

#include <complex>
#include <OpenADAPT/Common/Macros.h>
#include <OpenADAPT/Common/Definition.h>
#include <OpenADAPT/Common/Bpos.h>
#ifdef USE_ANKERL_UNORDERED_DENSE
#include <ankerl/unordered_dense.h>
#else
#include <unordered_map>
#endif

namespace adapt
{

//FieldTypeの定義はDefinition.hでは行えない。
//何故なら、以下のBposとJBposの定義のためにはLayerType、RankTypeの定義が必要で、
//それらはDefinition.hで定義されているから。
ADAPT_EXPORT
struct FieldTypeFlag
{
	static constexpr uint32_t INT = 0b0000000000000001;
	static constexpr uint32_t FLT = 0b0000000000000010;
	static constexpr uint32_t CPX = 0b0000000000000100;
	static constexpr uint32_t STR = 0b0000000000001000;
	static constexpr uint32_t YMD = 0b0000000000100000;
	static constexpr uint32_t JBP = 0b0100000000000000;
	//static constexpr uint32_t ANY = 0b1000000000000000;

	static constexpr uint32_t NUM = FLT | INT;

	static constexpr uint32_t SIZE_MASK = 0x00ff0000;
	static constexpr uint32_t ALIGN_MASK = 0xff000000;

	static constexpr uint32_t TRIVIAL = INT | FLT | CPX;
};

ADAPT_EXPORT
enum class FieldType : uint32_t
{
	Emp = 0,
	I08 = FieldTypeFlag::INT | (sizeof(int8_t) << 16) | (alignof(int8_t) << 24),
	I16 = FieldTypeFlag::INT | (sizeof(int16_t) << 16) | (alignof(int16_t) << 24),
	I32 = FieldTypeFlag::INT | (sizeof(int32_t) << 16) | (alignof(int32_t) << 24),
	I64 = FieldTypeFlag::INT | (sizeof(int64_t) << 16) | (alignof(int64_t) << 24),
	F32 = FieldTypeFlag::FLT | (sizeof(float) << 16) | (alignof(float) << 24),
	F64 = FieldTypeFlag::FLT | (sizeof(double) << 16) | (alignof(double) << 24),
	C32 = FieldTypeFlag::CPX | (sizeof(std::complex<float>) << 16) | (alignof(std::complex<float>) << 24),
	C64 = FieldTypeFlag::CPX | (sizeof(std::complex<double>) << 16) | (alignof(std::complex<double>) << 24),
	Str = FieldTypeFlag::STR | (sizeof(std::string) << 16) | (alignof(std::string) << 24),
	Jbp = FieldTypeFlag::JBP | (sizeof(JBpos) << 16) | (alignof(JBpos) << 24),
};

#define ADAPT_SWITCH_FIELD_TYPE(FIELD_TYPE, CODE, DEFAULT) \
	switch (FIELD_TYPE) \
	{ \
	case FieldType::I08: CODE(FieldType::I08) break; \
	case FieldType::I16: CODE(FieldType::I16) break; \
	case FieldType::I32: CODE(FieldType::I32) break; \
	case FieldType::I64: CODE(FieldType::I64) break; \
	case FieldType::F32: CODE(FieldType::F32) break; \
	case FieldType::F64: CODE(FieldType::F64) break; \
	case FieldType::C32: CODE(FieldType::C32) break; \
	case FieldType::C64: CODE(FieldType::C64) break; \
	case FieldType::Str: CODE(FieldType::Str) break; \
	case FieldType::Jbp: CODE(FieldType::Jbp) break; \
	default: DEFAULT \
	}

#define ADAPT_FIELD_TYPE_LIST_SOLO(CODE) \
	ADAPT_EXPAND_VARS(CODE, (I08, i08, int8_t)) \
	ADAPT_EXPAND_VARS(CODE, (I16, i16, int16_t)) \
	ADAPT_EXPAND_VARS(CODE, (I32, i32, int32_t)) \
	ADAPT_EXPAND_VARS(CODE, (I64, i64, int64_t)) \
	ADAPT_EXPAND_VARS(CODE, (F32, f32, float)) \
	ADAPT_EXPAND_VARS(CODE, (F64, f64, double)) \
	ADAPT_EXPAND_VARS(CODE, (C32, c32, std::complex<float>)) \
	ADAPT_EXPAND_VARS(CODE, (C64, c64, std::complex<double>)) \
	ADAPT_EXPAND_VARS(CODE, (Str, str, std::string)) \
	ADAPT_EXPAND_VARS(CODE, (Jbp, jbp, JBpos))

#define ADAPT_FIELD_TYPE_LIST_FN(CODE, ...) \
	ADAPT_EXPAND_VARS(CODE, (I08, i08, int8_t, __VA_ARGS__)) \
	ADAPT_EXPAND_VARS(CODE, (I16, i16, int16_t, __VA_ARGS__)) \
	ADAPT_EXPAND_VARS(CODE, (I32, i32, int32_t, __VA_ARGS__)) \
	ADAPT_EXPAND_VARS(CODE, (I64, i64, int64_t, __VA_ARGS__)) \
	ADAPT_EXPAND_VARS(CODE, (F32, f32, float, __VA_ARGS__)) \
	ADAPT_EXPAND_VARS(CODE, (F64, f64, double, __VA_ARGS__)) \
	ADAPT_EXPAND_VARS(CODE, (C32, c32, std::complex<float>, __VA_ARGS__)) \
	ADAPT_EXPAND_VARS(CODE, (C64, c64, std::complex<double>, __VA_ARGS__)) \
	ADAPT_EXPAND_VARS(CODE, (Str, str, std::string, __VA_ARGS__)) \
	ADAPT_EXPAND_VARS(CODE, (Jbp, jbp, JBpos, __VA_ARGS__))

#define ADAPT_FIELD_TYPE_LIST_DUO(CODE, ...)\
	ADAPT_FIELD_TYPE_LIST_FN(CODE, I08, i08, int8_t, __VA_ARGS__) \
	ADAPT_FIELD_TYPE_LIST_FN(CODE, I16, i16, int16_t, __VA_ARGS__) \
	ADAPT_FIELD_TYPE_LIST_FN(CODE, I32, i32, int32_t, __VA_ARGS__) \
	ADAPT_FIELD_TYPE_LIST_FN(CODE, I64, i64, int64_t, __VA_ARGS__) \
	ADAPT_FIELD_TYPE_LIST_FN(CODE, F32, f32, float, __VA_ARGS__) \
	ADAPT_FIELD_TYPE_LIST_FN(CODE, F64, f64, double, __VA_ARGS__) \
	ADAPT_FIELD_TYPE_LIST_FN(CODE, C32, c32, std::complex<float>, __VA_ARGS__) \
	ADAPT_FIELD_TYPE_LIST_FN(CODE, C64, c64, std::complex<double>, __VA_ARGS__) \
	ADAPT_FIELD_TYPE_LIST_FN(CODE, Str, str, std::string, __VA_ARGS__) \
	ADAPT_FIELD_TYPE_LIST_FN(CODE, Jbp, jbp, JBpos, __VA_ARGS__)

#define ADAPT_INT_TYPE_LIST_SOLO(CODE, ...) \
	ADAPT_EXPAND_VARS(CODE, (I08, i08, int8_t, __VA_ARGS__)) \
	ADAPT_EXPAND_VARS(CODE, (I16, i16, int16_t, __VA_ARGS__)) \
	ADAPT_EXPAND_VARS(CODE, (I32, i32, int32_t, __VA_ARGS__)) \
	ADAPT_EXPAND_VARS(CODE, (I64, i64, int64_t, __VA_ARGS__)) \

#define ADAPT_INT_TYPE_LIST_DUO(CODE, ...)\
	ADAPT_INT_TYPE_LIST_SOLO(CODE, I08, i08, int8_t, __VA_ARGS__) \
	ADAPT_INT_TYPE_LIST_SOLO(CODE, I16, i16, int16_t, __VA_ARGS__) \
	ADAPT_INT_TYPE_LIST_SOLO(CODE, I32, i32, int32_t, __VA_ARGS__) \
	ADAPT_INT_TYPE_LIST_SOLO(CODE, I64, i64, int64_t, __VA_ARGS__)

#define ADAPT_INT_TYPE_LIST_TRIO(CODE, ...)\
	ADAPT_INT_TYPE_LIST_DUO(CODE, I08, i08, int8_t, __VA_ARGS__) \
	ADAPT_INT_TYPE_LIST_DUO(CODE, I16, i16, int16_t, __VA_ARGS__) \
	ADAPT_INT_TYPE_LIST_DUO(CODE, I32, i32, int32_t, __VA_ARGS__) \
	ADAPT_INT_TYPE_LIST_DUO(CODE, I64, i64, int64_t, __VA_ARGS__)

#define ADAPT_TRIVIAL_TYPE_LIST_SOLO(CODE, ...) \
	ADAPT_EXPAND_VARS(CODE, (I08, i08, int8_t, __VA_ARGS__)) \
	ADAPT_EXPAND_VARS(CODE, (I16, i16, int16_t, __VA_ARGS__)) \
	ADAPT_EXPAND_VARS(CODE, (I32, i32, int32_t, __VA_ARGS__)) \
	ADAPT_EXPAND_VARS(CODE, (I64, i64, int64_t, __VA_ARGS__)) \
	ADAPT_EXPAND_VARS(CODE, (F32, f32, float, __VA_ARGS__)) \
	ADAPT_EXPAND_VARS(CODE, (F64, f64, double, __VA_ARGS__)) \
	ADAPT_EXPAND_VARS(CODE, (C32, c32, std::complex<float>, __VA_ARGS__)) \
	ADAPT_EXPAND_VARS(CODE, (C64, c64, std::complex<double>, __VA_ARGS__))

// 関数オブジェクト群には、Rttiモードでのコンパイルコストおよびバイナリファイルサイズ削減のために
// 各ラムダ関数生成を補助するための属性を与えている。
// ArithmeticConvLevel: 算術変換のレベル。0は算術変換なしで整数昇格のみ、1は整数のみの算術変換あり、2は整数+浮動小数点の算術変換あり、3は整数+浮動小数点+複素数の算術変換あり。
// Level-1 Bool:  論理演算子。両辺をI08へと変換する。
// Level0  None:  一切の変換を行わない。
// Level1  Promo: 算術変換はせず整数昇格のみ行う。シフト演算子が該当。
// Level2  Integ: 整数の算術変換を行う。剰余、ビット演算子が該当。
// Level3  Usual: 整数と浮動小数点の算術変換を行う。比較演算子が該当。
// Level4  Compl: 整数と浮動小数点と複素数の算術変換を行う。加減乗除が該当。
// Level指定なしはLevel0に同じ。
// これらのレベルに応じて、MakeRttiFuncNodeでは引数に与えられたnode_or_placeholderの型変換を暗黙的に行う。

// 注意点として、Level4-Complにおいては整数-複素数間の変換は行われず、そもそも呼び出し不能と判断される。
// std::complexは浮動小数点からの暗黙変換は行えるが、整数型からは行えないためである。
// さらに、C32 + F64のような、複素数型のビット数が浮動小数点側より小さい場合も変換は行われず、呼び出し不能と判断される。

enum class ArithmeticConvLevel : int32_t
{
	Bool = -1,
	None = 0,
	Promo = 1,
	Integ = 2,
	Usual = 3,
	Compl = 4
};

ADAPT_EXPORT
class FieldVariant;

namespace detail
{
template <class Key, size_t Size = std::tuple_size_v<Key>>
struct Hasher;
template <class Key>
struct Hasher<std::tuple<Key>, 1>
{
	using is_transparent = void;
#ifdef USE_ANKERL_UNORDERED_DENSE
	using is_avalanching = void;
#endif
	template <class T>
		requires std::convertible_to<T, Key>
	size_t operator()(const T& key) const noexcept
	{
		return std::hash<Key>{}(key);
	}
	template <class T>
		requires std::convertible_to<T, Key>
	size_t operator()(const std::tuple<T>& key) const noexcept
	{
		return (*this)(std::get<0>(key));
	}
};
template <class ...Keys, size_t Size>
struct Hasher<std::tuple<Keys...>, Size>
{
	using is_transparent = void;
#ifdef USE_ANKERL_UNORDERED_DENSE
	using is_avalanching = void;
#endif
private:
	template <size_t I, class ...Keys_>
	static size_t Combine(const std::tuple<Keys_...>& v) noexcept
	{
		if constexpr (sizeof...(Keys_) == I) return 0;
		else
		{
			size_t seed = Combine<I + 1>(v);
			using Key = GetType_t<I, Keys...>;
			return seed ^ (Hasher<std::tuple<Key>>{}(std::get<I>(v)) + 0x9e3779b9 + (seed << 6) + (seed >> 2));
		}
	}
public:
	size_t operator()(const std::tuple<Keys...>& keys) const noexcept
	{
		return Combine<0>(keys);
	}
};
}

#ifdef USE_ANKERL_UNORDERED_DENSE
template <class ...Types>
using Hashtable = ankerl::unordered_dense::map<std::tuple<Types...>, Bpos, detail::Hasher<std::tuple<Types...>>>;
#else
template <class ...Types>
using Hashtable = std::unordered_map<std::tuple<Types...>, Bpos, detail::Hasher<std::tuple<Types...>>>;
#endif

namespace detail
{

// インデックス指定でアクセスできるものをすべて許容する。
template <class Arr, class T>
concept raw_array_like = requires(Arr t, size_t i)
{
	{ t[i] } -> similar_to<T>;
};

template <size_t Dim>
struct HistIsEmpty
{
	template <raw_array_like<BinBaseType> Int32Array>
	bool operator()(const Int32Array& min, const Int32Array& max) const
	{
		for (size_t i = 0; i < Dim; ++i)
		{
			if (min[i] != std::numeric_limits<BinBaseType>::max() ||
				max[i] != std::numeric_limits<BinBaseType>::min())
			{
				return false;
			}
		}
		return true;
	}
};
template <size_t Dim>
struct BinToIndex
{
	template <raw_array_like<BinBaseType> Int32Array>
	BindexType operator()(const Bin<Dim>& bin, const Int32Array& min, const Int32Array& max) const
	{
#ifndef NDEBUG
		for (size_t i = 0; i < Dim; ++i) assert(bin[i] >= min[i] && bin[i] <= max[i]);
#endif
		BindexType row = BindexType(bin[0] - min[0]);
		for (size_t i = 1; i < Dim; ++i)
		{
			BindexType size = BindexType(max[i] - min[i] + 1);
			BindexType x = BindexType(bin[i] - min[i]);
			row = row * size + x;
		}
		return row;
	}
};
template <size_t Dim>
struct IndexToBin
{
	template <raw_array_like<int32_t> Int32Array>
	Bin<Dim> operator()(BindexType index, const Int32Array& min, const Int32Array& max) const
	{
		Bin<Dim> res{};
		BindexType tmp = index;
		for (size_t i = Dim; i > 0; --i)
		{
			int32_t size = max[i - 1] - min[i - 1] + 1;
			res[i - 1] = (int32_t)(tmp % size + min[i - 1]);
			tmp /= size;
		}
		return res;

	}
};
template <size_t Dim>
struct IsInsideRange
{
	template <raw_array_like<int32_t> Int32Array>
	bool operator()(const Bin<Dim>& bin, const Int32Array& min, const Int32Array& max) const
	{
		for (size_t i = 0; i < Dim; ++i)
			if (bin[i] < min[i] || bin[i] > max[i]) return false;
		return true;
	}
};

}

}

#endif