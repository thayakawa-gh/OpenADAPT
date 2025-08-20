#ifndef ADAPT_COMMOM_COMMON_H
#define ADAPT_COMMOM_COMMON_H

#include <OpenADAPT/Common/Definition.h>
#include <OpenADAPT/Common/Bpos.h>
#ifdef USE_ANKERL_UNORDERED_DENSE
#include <ankerl/unordered_dense.h>
#endif

namespace adapt
{

//FieldTypeの定義はDefinition.hでは行えない。
//何故なら、以下のBposとJBposの定義のためにはLayerType、RankTypeの定義が必要で、
//それらはDefinition.hで定義されているから。
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
			BindexType size = BindexType(max[i - 1] - min[i - 1] + 1);
			res[i - 1] = tmp % size + (BindexType)min[i - 1];
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