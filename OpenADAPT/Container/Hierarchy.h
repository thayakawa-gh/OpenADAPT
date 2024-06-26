#ifndef ADAPT_CONTAINER_HIERARCHY_H
#define ADAPT_CONTAINER_HIERARCHY_H

#include <cassert>
#include <vector>
#include <map>
#include <string>
#include <cstdint>
#include <iostream>
#include <OpenADAPT/Utility/Ranges.h>
#include <OpenADAPT/Utility/NamedTuple.h>
#include <OpenADAPT/Utility/Exception.h>
#include <OpenADAPT/Common/Concepts.h>
#include <OpenADAPT/Common/Bpos.h>
#include <OpenADAPT/Evaluator/Placeholder.h>

namespace adapt
{

namespace detail
{
template <class Container, LayerType Layer, uint16_t Index, ptrdiff_t EndPtr>
consteval auto GetPlaceholderList_field(NamedTuple<>)
{
	return std::tuple<>{};
}
template <class Container, LayerType Layer, uint16_t Index, ptrdiff_t EndPtr, StaticChar Name, class Type, StaticChar ...Names, class ...Types>
consteval auto GetPlaceholderList_field(NamedTuple<Named<Name, Type>, Named<Names, Types>...>)
{
	using CttiPlaceholder = eval::CttiPlaceholder<Layer, Type, Container>;
	constexpr size_t size = sizeof(Type);
	constexpr size_t align = alignof(Type);
	constexpr ptrdiff_t mod = EndPtr % align;
	constexpr ptrdiff_t offset = (mod == 0) ? EndPtr : (EndPtr + align - mod);
	constexpr auto follower = GetPlaceholderList_field<Container, Layer, LayerType(Index + 1), offset + size>(NamedTuple<Named<Names, Types>...>{});
	constexpr auto f = CttiPlaceholder(Index, offset);
	return std::tuple_cat(std::make_tuple(f), follower);
}

template <class Container, named_tuple ...Elements, LayerType ...Indices>
consteval auto GetPlaceholderList_layer(TypeList<Elements...>, std::integer_sequence<LayerType, Indices...>)
{
	return std::make_tuple(GetPlaceholderList_field<Container, LayerType(Indices - 1), 0, 0>(Elements{})...);
}
template <class Container, named_tuple ...Elements>
consteval auto GetPlaceholderList(TypeList<Elements...>)
{
	return GetPlaceholderList_layer<Container>(TypeList<Elements...>{}, std::make_integer_sequence<LayerType, sizeof...(Elements)>{});
}

}


template <class Container_, named_tuple ...LayerElements>
class SHierarchy
{
public:
	using Container = Container_;
	using RttiPlaceholder = eval::RttiPlaceholder<Container>;
	template <class Type>
	using TypedPlaceholder = eval::TypedPlaceholder<Type, Container>;
	template <LayerType Layer_, class Type_>
	using CttiPlaceholder = eval::CttiPlaceholder<Layer_, Type_, Container>;

	template <LayerType Layer>
	using Element = GetType_t<Layer + 1, LayerElements...>;

	static constexpr LayerType MaxLayer = (LayerType)sizeof...(LayerElements) - 2_layer;
	static constexpr LayerType GetMaxLayer() { return MaxLayer; }

	//std::tuple<std::tuple<CttiPlaceholder...>, std::tuple<CttiPlaceholder...>>;と、階層ごとのCttiのリストになっている。
	using PlaceholderListType = decltype(detail::GetPlaceholderList<Container>(TypeList<LayerElements...>{}));
	static constexpr PlaceholderListType PlaceholderList = detail::GetPlaceholderList<Container>(TypeList<LayerElements...>{});

private:
	template <LayerType Layer_plus1>
	static constexpr std::array<size_t, MaxLayer + 2_layer> GetElementSizes()
	{
		if constexpr (Layer_plus1 == MaxLayer + 2_layer)
		{
			return std::array<size_t, MaxLayer + 2_layer>{};
		}
		else
		{
			std::array<size_t, MaxLayer + 2_layer> res = GetElementSizes<Layer_plus1 + 1_layer>();
			auto phs = std::get<Layer_plus1>(PlaceholderList);
			constexpr size_t phssize = std::tuple_size_v<decltype(phs)>;
			if constexpr (phssize == 0)
			{
				res[Layer_plus1] = 0;
			}
			else
			{
				auto lastph = std::get<phssize - 1>(phs);
				size_t size = lastph.GetPtrOffset() + sizeof(typename decltype(lastph)::RetType);
				constexpr size_t align = alignof(void*);
				if (ptrdiff_t mod = size % align; mod != 0) size += (align - mod);
				res[Layer_plus1] = size;
			}
			return res;
		}
	}
	template <LayerType Layer, class ...Types>
	static consteval bool JudgeTotallyTrivial_layer(const std::tuple<CttiPlaceholder<Layer, Types>...>&)
	{
		return (std::is_trivially_copyable_v<Types> && ...);
	}
	static consteval bool JudgeTotallyTrivial_layer(const std::tuple<>&)
	{
		return true;
	}
	template <LayerType Layer_plus1>
	static consteval std::array<bool, MaxLayer + 2_layer> JudgeTotallyTrivial()
	{
		if constexpr (Layer_plus1 == MaxLayer + 2_layer)
		{
			return std::array<bool, MaxLayer + 2_layer>{};
		}
		else
		{
			std::array<bool, MaxLayer + 2_layer> res = JudgeTotallyTrivial<Layer_plus1 + 1_layer>();
			constexpr auto phs = std::get<Layer_plus1>(PlaceholderList);
			constexpr bool is_totally_trivial = JudgeTotallyTrivial_layer(phs);
			res[Layer_plus1] = is_totally_trivial;
			return res;
		}
	}

	static constexpr std::array<size_t, MaxLayer + 2_layer> ElementSizes = GetElementSizes<0>();
	static constexpr std::array<bool, MaxLayer + 2_layer> TotallyTrivial = JudgeTotallyTrivial<0>();
public:
	//ある層の要素の大きさ。下層のElementBlockは除く。
	static constexpr size_t GetElementSize(LayerType layer) { return ElementSizes[layer + 1_layer]; }
	template <LayerType Layer>
	static consteval size_t GetElementSize(LayerConstant<Layer>) { return ElementSizes[Layer + 1_layer]; }

	static constexpr bool IsTotallyTrivial(LayerType layer) { return TotallyTrivial[layer + 1_layer]; }
	template <LayerType Layer>
	static consteval auto IsTotallyTrivial(LayerConstant<Layer>) { return std::bool_constant<TotallyTrivial[Layer + 1_layer]>{}; }

private:
	template <LayerType Layer, size_t Index>
	static consteval auto GetPlaceholder_impl()
	{
		auto phs = std::get<Layer + 1_layer>(PlaceholderList);
		return std::get<Index>(phs);
	}
	template <StaticChar Name_, LayerType Layer>
	static consteval auto GetPlaceholder_rec()
	{
		if constexpr (Layer <= MaxLayer)
		{
			constexpr size_t index = Element<Layer>::FindName(Name_);
			if constexpr (index == std::numeric_limits<size_t>::max()) return GetPlaceholder_rec<Name_, Layer + 1>();
			else return GetPlaceholder_impl<Layer, index>();
		}
		else
		{
			static_assert(Layer <= MaxLayer, "no fields have been found.");
			//throw NoElements(std::format("no fields with name {}", Name.m_name));
		}
	}
	template <size_t Index, LayerType Layer>
	static RttiPlaceholder GetPlaceholder_rec(std::string_view name)
	{
		if constexpr (Layer <= MaxLayer)
		{
			using Element_ = Element<Layer>;
			if constexpr (Index < TupleSize_v<Element_>)
			{
				constexpr TupleElementName_t<Index, Element_> this_name;
				if (std::string_view(this_name) == name)
				{
					constexpr auto cttiph = GetPlaceholder_impl<Layer, Index>();
					if (DFieldInfo::ValueTypeToTagType<typename decltype(cttiph)::RetType>() == FieldType::Emp)
						throw MismatchType(std::format("RttiPlaceholder cannot be created from a field with type {}.", typeid(cttiph).name()));
					return RttiPlaceholder(cttiph.GetInternalLayer(), cttiph.GetIndex(), cttiph.GetType(), cttiph.GetPtrOffset());
				}
				else return GetPlaceholder_rec<Index + 1, Layer>(name);
			}
			else return GetPlaceholder_rec<0, Layer + 1_layer>(name);
		}
		else
		{
			throw InvalidArg(std::format("no fields with name {}", name));
		}
	}
public:
	template <StaticChar Name_>
	static constexpr auto GetPlaceholder()
	{
		return GetPlaceholder_rec<Name_, -1_layer>();
	}
	template <StaticChar Name_>
	static constexpr auto GetPlaceholder(StaticString<Name_>)
	{
		return GetPlaceholder<Name_>();
	}
	static RttiPlaceholder GetPlaceholder(std::string_view name)
	{
		return GetPlaceholder_rec<0, -1_layer>(name);
	}
	template <class ...Args>
		requires (IsBaseOf_XN<StaticString, std::decay_t<Args>>::value && ...)
	static constexpr auto GetPlaceholders(Args&& ...args)
	{
		return std::make_tuple(GetPlaceholder(std::forward<Args>(args))...);
	}

	template <StaticChar ...Names>
	static constexpr auto GetPlaceholders()
	{
		return std::make_tuple(GetPlaceholder<Names>()...);
	}
	template <class ...Args>
		requires ((!IsSame_XN_v<StaticString, std::decay_t<Args>>&& std::convertible_to<Args, std::string_view>) && ...)
	static std::array<RttiPlaceholder, sizeof...(Args)> GetPlaceholders(Args&& ...args)
	{
		return { GetPlaceholder(args)... };
	}

	template <LayerType Layer>
	static consteval auto GetPlaceholdersIn(LayerConstant<Layer>)
	{
		return std::get<Layer + 1_layer>(PlaceholderList);
	}

private:
	template <StaticChar ...Names, class ...Types>
	static constexpr auto GetFieldNamesIn_impl(NamedTuple<Named<Names, Types>...>)
	{
		return std::make_tuple(StaticString<Names>{}...);
	}
	template <LayerType Layer>
	static constexpr auto GetFieldNamesIn_rec()
	{
		if constexpr (Layer <= MaxLayer)
		{
			using Element_ = Element<Layer>;
			constexpr auto names = std::tuple_cat(GetFieldNamesIn_impl(Element_{}), GetFieldNamesIn_rec<Layer + 1>());
		}
		else
		{
			return std::tuple<>{};
		}
	}
public:
	static constexpr auto GetFieldNames()
	{
		return GetFieldNamesIn_rec<-1_layer>();
	}
	template <LayerType Layer>
	static consteval auto GetFieldNamesIn(LayerConstant<Layer>)
	{
		using Element_ = Element<Layer>;
		return GetFieldNamesIn_impl(Element_{});
	}

private:
	template <LayerType Layer, size_t Index, class PHs, class Names>
	static void ShowHierarchy_index(std::ostream& o, PHs phs, Names names)
	{
		if constexpr (Index < std::tuple_size_v<PHs>)
		{
			auto ph = std::get<Index>(phs);
			auto name = std::get<Index>(names);
			o << std::format(" {{ \"{}\", {} }}", name.GetChar(), typeid(typename decltype(ph)::RetType).name());
			ShowHierarchy_index<Layer, Index + 1>(o, phs, names);
		}
	}
	template <LayerType Layer>
	static void ShowHierarchy_layer(std::ostream& o)
	{
		o << std::format("Layer[{:>2}] {{", Layer);
		auto fields = GetPlaceholdersIn(LayerConstant<Layer>{});
		auto names = GetFieldNamesIn(LayerConstant<Layer>{});
		ShowHierarchy_index<Layer, 0>(o, fields, names);
		o << " }\n";
		if constexpr (Layer < MaxLayer)
			ShowHierarchy_layer<Layer + 1_layer>(o);
	}
public:
	static void ShowHierarchy(std::ostream& o = std::cout)
	{
		ShowHierarchy_layer<-1_layer>(o);
	}
};

/*template <class Container_, named_tuple ...LayerElements>
class SHierarchy
{
public:

	using Container = Container_;
	using RttiPlaceholder = eval::RttiPlaceholder<Container>;
	template <class Type>
	using TypedPlaceholder = eval::TypedPlaceholder<Type, Container>;
	template <LayerType Layer_, class Type_>
	using CttiPlaceholder = eval::CttiPlaceholder<Layer_, Type_, Container>;

	template <LayerType Layer>
	using Element = GetType_t<Layer + 1, LayerElements...>;

	static constexpr LayerType MaxLayer = (LayerType)sizeof...(LayerElements) - 2_layer;

	static constexpr LayerType GetMaxLayer() { return MaxLayer; }

private:
	//Layer_層Index_番目のフィールドの先頭アドレスへのオフセット。
	template <LayerType Layer_, uint16_t Index_>
	static constexpr ptrdiff_t GetPtrOffset()
	{
		if constexpr (Index_ == 0) return 0;
		else
		{
			ptrdiff_t offs = GetPtrOffset<Layer_, Index_ - 1>();
			offs += sizeof(TupleElement_t<Index_ - 1, Element<Layer_>>);
			size_t align = alignof(TupleElement_t<Index_, Element<Layer_>>);
			if (ptrdiff_t mod = offs % align; mod != 0) offs += (align - mod);
			return offs;
		}
	}
	template <LayerType Layer_>
	static constexpr size_t GetElementSize_impl()
	{
		constexpr uint16_t Index = TupleSize_v<Element<Layer_>>;
		if constexpr (Index == 0) return 0;//サイズ0の場合、要素がないということなので、単に0を返せば良い。
		else
		{
			ptrdiff_t offs = GetPtrOffset<Layer_, Index - 1>();
			offs += sizeof(TupleElement_t<Index - 1, Element<Layer_>>);
			constexpr size_t align = alignof(void*);
			if (ptrdiff_t mod = offs % align; mod != 0) offs += (align - mod);
			return offs;
		}
	}
	template <size_t ...Indices>
	static constexpr std::array<size_t, MaxLayer + 2_layer> GetElementSizes(std::index_sequence<Indices...>)
	{
		return std::array<size_t, MaxLayer + 2_layer>{ GetElementSize_impl<(LayerType)Indices - 1_layer>()... };
	}
	static constexpr std::array<size_t, MaxLayer + 2_layer> GetElementSizes()
	{
		return GetElementSizes(std::make_index_sequence<MaxLayer + 2>{});
	}

	static constexpr std::array<size_t, MaxLayer + 2_layer> ElementSizes = GetElementSizes();

public:
	//ある層の要素の大きさ。下層のElementBlockは除く。
	static constexpr size_t GetElementSize(LayerType layer) { return ElementSizes[layer + (LayerType)1]; }
	template <LayerType Layer>
	static consteval size_t GetElementSize(LayerConstant<Layer>) { return ElementSizes[Layer + (LayerType)1]; }

private:
	template <LayerType Layer, size_t Index>
	static consteval auto GetPlaceholder_impl()
	{
		using Element_ = Element<Layer>;
		using Type = TupleElement_t<Index, Element_>;
		return CttiPlaceholder<Layer, Type>(Index, GetPtrOffset<Layer, Index>());
	}
	template <StaticChar Name_, LayerType Layer>
	static consteval auto GetPlaceholder_rec()
	{
		if constexpr (Layer <= MaxLayer)
		{
			constexpr size_t index = Element<Layer>::FindName(Name_);
			if constexpr (index == std::numeric_limits<size_t>::max()) return GetPlaceholder_rec<Name_, Layer + 1>();
			else return GetPlaceholder_impl<Layer, index>();
		}
		else
		{
			static_assert(Layer <= MaxLayer, "no fields have been found.");
			//throw NoElements(std::format("no fields with name {}", Name.m_name));
		}
	}
	template <size_t Index, LayerType Layer>
	static RttiPlaceholder GetPlaceholder_rec(std::string_view name)
	{
		if constexpr (Layer <= MaxLayer)
		{
			using Element_ = Element<Layer>;
			if constexpr (Index < TupleSize_v<Element_>)
			{
				constexpr TupleElementName_t<Index, Element_> this_name;
				if (std::string_view(this_name) == name)
				{
					constexpr auto cttiph = GetPlaceholder_impl<Layer, Index>();
					if (DFieldInfo::ValueTypeToTagType<typename decltype(cttiph)::RetType>() == FieldType::Emp)
						throw MismatchType(std::format("RttiPlaceholder cannot be created from a field with type {}.", typeid(cttiph).name()));
					return RttiPlaceholder(cttiph.GetInternalLayer(), cttiph.GetIndex(), cttiph.GetType(), cttiph.GetPtrOffset());
				}
				else return GetPlaceholder_rec<Index + 1, Layer>(name);
			}
			else return GetPlaceholder_rec<0, Layer + 1_layer>(name);
		}
		else
		{
			throw InvalidArg(std::format("no fields with name {}", name));
		}
	}
public:
	template <StaticChar Name_>
	static constexpr auto GetPlaceholder()
	{
		return GetPlaceholder_rec<Name_, -1_layer>();
	}
	template <StaticChar Name_>
	static constexpr auto GetPlaceholder(StaticString<Name_>)
	{
		return GetPlaceholder<Name_>();
	}
	static RttiPlaceholder GetPlaceholder(std::string_view name)
	{
		return GetPlaceholder_rec<0, -1_layer>(name);
	}

	template <class ...Args>
		requires (IsBaseOf_XN<StaticString, std::decay_t<Args>>::value && ...)
	static constexpr auto GetPlaceholders(Args&& ...args)
	{
		return std::make_tuple(GetPlaceholder(std::forward<Args>(args))...);
	}
	template <StaticChar ...Names>
	static constexpr auto GetPlaceholders()
	{
		return std::make_tuple(GetPlaceholder<Names>()...);
	}
	template <class ...Args>
		requires ((!IsSame_XN_v<StaticString, std::decay_t<Args>> && std::convertible_to<Args, std::string_view>) && ...)
	static std::array<RttiPlaceholder, sizeof...(Args)> GetPlaceholders(Args&& ...args)
	{
		return { GetPlaceholder(args)... };
	}

private:
	template <size_t Index, StaticChar ...Names>
	static constexpr auto GetPlaceholdersIn_rec()
	{

	}
	template <LayerType Layer, size_t ...Indices>
	static constexpr auto GetPlaceholdersIn_impl(LayerConstant<Layer>, std::index_sequence<Indices...>)
	{
		return std::make_tuple(GetPlaceholder_impl<Layer, Indices>()...);
	}
public:
	template <LayerType Layer>
	static consteval auto GetPlaceholdersIn(LayerConstant<Layer>)
	{
		return GetPlaceholdersIn_impl(LayerConstant<Layer>{}, std::make_index_sequence<TupleSize_v<Element<Layer>>>{});
	}

private:
	template <StaticChar ...Names, class ...Types>
	static constexpr auto GetFieldNamesIn_impl(NamedTuple<Named<Names, Types>...>)
	{
		return std::make_tuple(StaticString<Names>{}...);
	}
	template <LayerType Layer>
	static constexpr auto GetFieldNamesIn_rec()
	{
		if constexpr (Layer <= MaxLayer)
		{
			using Element_ = Element<Layer>;
			constexpr auto names = std::tuple_cat(GetFieldNamesIn_impl(Element_{}), GetFieldNamesIn_rec<Layer + 1>());
		}
		else
		{
			return std::tuple<>{};
		}
	}
public:
	static constexpr auto GetFieldNames()
	{
		return GetFieldNamesIn_rec<-1_layer>();
	}
	template <LayerType Layer>
	static consteval auto GetFieldNamesIn(LayerConstant<Layer>)
	{
		using Element_ = Element<Layer>;
		return GetFieldNamesIn_impl(Element_{});
	}
};*/

template <class Container>
class DHierarchy
{
public:

	using RttiPlaceholder = eval::RttiPlaceholder<Container>;
	template <class Type>
	using TypedPlaceholder = eval::TypedPlaceholder<Type, Container>;
	template <LayerType Layer, class Type>
	using CttiPlaceholder = eval::CttiPlaceholder<Layer, Type, Container>;

	DHierarchy()
		: m_element_sizes(1, 0), m_totally_trivial(1, true), m_field_infos_by_layer(1)
	{}

	void SetTopLayer(const std::vector<std::pair<std::string, FieldType>>& mems)
	{
		intptr_t offs = 0;
		LayerType layer = -1;
		auto& mbl = m_field_infos_by_layer[0];
		mbl.resize(mems.size());
		bool totally_trivial = true;
		for (auto[i, mem, x]: views::Enumerate(mems, mbl))
		{
			FieldType tag = mem.second;
			totally_trivial = totally_trivial && DFieldInfo::IsTrivial(tag);
			ptrdiff_t size = (ptrdiff_t)DFieldInfo::GetSizeOf(tag);
			ptrdiff_t align = (ptrdiff_t)DFieldInfo::GetAlignOf(tag);
			if (ptrdiff_t add = offs % align; add != 0) offs += (align - add);
			x = RttiPlaceholder(layer, (uint16_t)i, tag, offs);
			m_field_map.insert(std::make_pair(mem.first, &x));
			offs += size;
		}
		constexpr size_t align = alignof(void*);
		if (auto a = offs % align; a != 0) offs += (align - a);
		m_element_sizes[0] = offs;
		m_totally_trivial[0] = totally_trivial;
	}
	void AddLayer(std::vector<std::pair<std::string, FieldType>> mems)
	{
		intptr_t offs = 0;
		assert(m_field_infos_by_layer.size() < std::numeric_limits<int16_t>::max());
		LayerType layer = (LayerType)m_field_infos_by_layer.size() - 1;
		m_field_infos_by_layer.emplace_back();
		auto& mbl = m_field_infos_by_layer.back();
		mbl.resize(mems.size());
		bool totally_trivial = true;
		for (auto[i, mem, x] : views::Enumerate(mems, mbl))
		{
			FieldType tag = mem.second;
			totally_trivial = totally_trivial && DFieldInfo::IsTrivial(tag);
			ptrdiff_t size = (ptrdiff_t)DFieldInfo::GetSizeOf(tag);
			ptrdiff_t align = (ptrdiff_t)DFieldInfo::GetAlignOf(tag);
			if (ptrdiff_t add = offs % align; add != 0) offs += (align - add);
			x = RttiPlaceholder(layer, (uint16_t)i, tag, offs);
			m_field_map.insert(std::make_pair(mem.first, &x));
			offs += size;
		}
		constexpr size_t align = alignof(void*);
		if (auto a = offs % align; a != 0) offs += (align - a);
		m_element_sizes.push_back(offs);
		m_totally_trivial.push_back(totally_trivial);
		++m_max_layer;
	}

	[[deprecated("no need to call this function in the current version")]]
	void VerifyStructure()
	{
		/*for (auto [name, mem] : views::Zip(m_field_names, m_field_infos))
		{
			auto res = m_field_map.insert(std::make_pair(name, mem));
			if (!res.second)
			{
				m_field_map.clear();
				throw InvalidArg(std::format("The field with name \"{}\" already exists.", name));
			}
		}
		m_max_layer = (LayerType)(m_field_infos_by_layer.size() - 2);*/
	}
	bool IsValid() const
	{
		return !m_field_map.empty();
	}

	void ShowHierarchy(std::ostream& o = std::cout) const
	{
		LayerType layer = -1;
		for (const auto& fields : m_field_infos_by_layer)
		{
			o << std::format("Layer[{:>2}] {{", layer);
			for (const auto& field : fields)
			{
				const std::string& name = GetFieldName(field);
				o << std::format(" {{ \"{}\", {} }}", name, DFieldInfo::GetTagTypeString(field.GetType()));
			}
			o << " }\n";
			++layer;
		}
	}

	template <LayerType Layer, class Type>
	CttiPlaceholder<Layer, Type> GetPlaceholder(std::string_view name) const
	{
		return GetPlaceholder<Layer, DFieldInfo::ValueTypeToTagType<Type>()>(name);
	}
	template <LayerType Layer, FieldType Type>
	CttiPlaceholder<Layer, DFieldInfo::TagTypeToValueType<Type>> GetPlaceholder(std::string_view name) const
	{
		const RttiPlaceholder& rtti = GetPlaceholder(name);
		return rtti.template AddCttiInfo<Layer, Type>();
	}

	template <class Type>
		requires (DFieldInfo::ValueTypeToTagType<Type>() != FieldType::Emp)
	TypedPlaceholder<Type> GetPlaceholder(std::string_view name) const
	{
		return GetPlaceholder<DFieldInfo::ValueTypeToTagType<Type>()>(name);
	}
	template <FieldType Type>
	TypedPlaceholder<DFieldInfo::TagTypeToValueType<Type>> GetPlaceholder(std::string_view name) const
	{
		const RttiPlaceholder& rtti = GetPlaceholder(name);
		return rtti.template AddType<Type>();
	}

	/*const RttiPlaceholder& GetPlaceholder(uint16_t index) const
	{
		return *m_field_infos[index];
	}*/
	const RttiPlaceholder& GetPlaceholder(LayerType layer, uint16_t arr_index) const
	{
		return m_field_infos_by_layer[layer + (LayerType)1][arr_index];
	}
	const RttiPlaceholder& GetPlaceholder(std::string_view name) const
	{
		//string_viewにしてもどうせfindでstringに変換されるので無意味。
		//->いや、heterogeneous overloadがある。
		auto it = m_field_map.find(name);
		if (it == m_field_map.end()) throw InvalidArg(std::format("no fields with name \"{}\"", name));
		return *it->second;
	}
	template <StaticChar Name_>
	RttiPlaceholder GetPlaceholder(StaticString<Name_> name) const
	{
		return GetPlaceholder(name.GetChar());
	}

	template <class ...NAMES>
		requires (std::convertible_to<NAMES, std::string_view> && ...)
	std::array<RttiPlaceholder, sizeof...(NAMES)> GetPlaceholders(const NAMES& ...names) const
	{
		return { GetPlaceholder(names)... };
	}

	const std::vector<RttiPlaceholder>& GetPlaceholdersIn(LayerType layer) const
	{
		return m_field_infos_by_layer[layer + (LayerType)1];
	}

	LayerType GetMaxLayer() const
	{
		return m_max_layer;
	}
	const std::string& GetFieldName(const RttiPlaceholder& m) const
	{
		for (auto it = m_field_map.begin(); it != m_field_map.end(); ++it)
		{
			//indexはPlaceholderごと固有の値なので、これが一致すれば良い。
			if (m.GetIndex() == it->second->GetIndex() && m.GetLayer() == it->second->GetLayer()) return it->first;
		}
		throw InvalidArg("The field does not exist.");
	}
	size_t GetElementSize(LayerType layer) const
	{
		assert(layer <= m_max_layer);
		return m_element_sizes[layer + (LayerType)1];
	}
	bool IsTotallyTrivial(LayerType layer) const
	{
		assert(layer <= m_max_layer);
		return (bool)m_totally_trivial[layer + (LayerType)1];
	}

private:

	LayerType m_max_layer = -1;//layerは-1を持つことがあるので、符号付きにする必要がある。
	std::vector<size_t> m_element_sizes;//レイヤーごとの要素サイズ。
	std::vector<char> m_totally_trivial;//あるレイヤーのフィールドが全てトリビアルかどうか。
	std::vector<std::vector<RttiPlaceholder>> m_field_infos_by_layer;//0番目要素は-1層のグローバル変数。
	std::map<std::string, const RttiPlaceholder*, std::ranges::less> m_field_map;
};

//型は実行時だが、階層の最大値だけが与えられている構造。
//DTable, DHashなどに使う。STableとSHashはSHierarchyにすべきだろう。
template <class Container_, LayerType MaxLayer_>
class FHierarchy
{
public:
	static constexpr LayerType MaxLayer = MaxLayer_;
	using Container = Container_;

	//LayeredPlaceholderを作るメリットはあるのだろうか？
	using RttiPlaceholder = eval::RttiPlaceholder<Container>;
	template <class Type>
	using TypedPlaceholder = eval::TypedPlaceholder<Type, Container>;
	template <LayerType Layer, class Type>
	using CttiPlaceholder = eval::CttiPlaceholder<Layer, Type, Container>;

	static constexpr LayerType GetMaxLayer() { return MaxLayer; }

	FHierarchy() : m_element_sizes{ 0, 0 } {};

	void SetTopLayer(const std::vector<std::pair<std::string, FieldType>>& cols)
	{
		SetLayer(-1_layer, cols);
	}
	void SetLayer(LayerType layer, const std::vector<std::pair<std::string, FieldType>>& cols)
	{
		intptr_t offs = 0;
		assert(layer <= MaxLayer);
		auto& mbl = m_field_infos_by_layer[layer + 1_layer];
		mbl.resize(cols.size());
		bool totally_trivial = true;
		for (auto[i, mem, x] : views::Enumerate(cols, mbl))
		{
			FieldType tag = mem.second;
			totally_trivial = totally_trivial && DFieldInfo::IsTrivial(tag);
			ptrdiff_t size = (ptrdiff_t)DFieldInfo::GetSizeOf(tag);
			ptrdiff_t align = (ptrdiff_t)DFieldInfo::GetAlignOf(tag);
			if (ptrdiff_t add = offs % align; add != 0) offs += (align - add);
			x = RttiPlaceholder(layer, (uint16_t)i, tag, offs);
			m_field_map.insert(std::make_pair(mem.first, &x));
			offs += size;
		}
		constexpr size_t align = alignof(void*);
		if (auto a = offs % align; a != 0) offs += (align - a);
		m_element_sizes[layer + 1_layer] = offs;
		m_totally_trivial[layer + 1_layer] = totally_trivial;
	}

	[[deprecated("no need to call this function in the current version")]]
	void VerifyStructure()
	{
		/*for (auto [name, mem] : views::Zip(m_field_names, m_field_infos))
		{
			auto res = m_field_map.insert(std::make_pair(name, mem));
			if (!res.second)
			{
				m_field_map.clear();
				throw InvalidArg(std::format("The field with name \"{}\" already exists.", name));
			}
		}*/
	}
	bool IsValid() const
	{
		return !m_field_map.empty();
	}
	void ShowHierarchy(std::ostream& o = std::cout) const
	{
		LayerType layer = -1;
		for (const auto& fields : m_field_infos_by_layer)
		{
			o << std::format("Layer[{:>2}] {{", layer);
			for (const auto& field : fields)
			{
				const std::string& name = GetFieldName(field);
				o << std::format(" {{ \"{}\", {} }}", name, DFieldInfo::GetTagTypeString(field.GetType()));
			}
			o << " }\n";
			++layer;
		}
	}

	template <LayerType Layer, class Type>
	CttiPlaceholder<Layer, Type> GetPlaceholder(std::string_view name) const
	{
		return GetPlaceholder<Layer, DFieldInfo::ValueTypeToTagType<Type>()>(name);
	}
	template <LayerType Layer, FieldType Type>
	CttiPlaceholder<Layer, DFieldInfo::TagTypeToValueType<Type>> GetPlaceholder(std::string_view name) const
	{
		const RttiPlaceholder& rtti = GetPlaceholder(name);
		return rtti.template AddCttiInfo<Layer, Type>();
	}

	template <class Type>
		requires (DFieldInfo::ValueTypeToTagType<Type>() != FieldType::Emp)
	TypedPlaceholder<Type> GetPlaceholder(std::string_view name) const
	{
		return GetPlaceholder<DFieldInfo::ValueTypeToTagType<Type>()>(name);
	}
	template <FieldType Type>
	TypedPlaceholder<DFieldInfo::TagTypeToValueType<Type>> GetPlaceholder(std::string_view name) const
	{
		const RttiPlaceholder& rtti = GetPlaceholder(name);
		return rtti.template AddType<Type>();
	}

	const RttiPlaceholder& GetPlaceholder(LayerType layer, uint16_t arr_index) const
	{
		return m_field_infos_by_layer[layer + 1_layer][arr_index];
	}
	const RttiPlaceholder& GetPlaceholder(std::string_view name) const
	{
		//string_viewにしてもどうせfindでstringに変換されるので無意味。
		//->いや、heterogeneous overloadがある。
		auto it = m_field_map.find(name);
		if (it == m_field_map.end()) throw InvalidArg(std::format("no fields with name \"{}\"", name));
		return *it->second;
	}
	template <StaticChar Name_>
	RttiPlaceholder GetPlaceholder(StaticString<Name_> name) const
	{
		return GetPlaceholder(name.GetChar());
	}

	const std::vector<RttiPlaceholder>& GetPlaceholdersIn(LayerType layer) const
	{
		return m_field_infos_by_layer[layer + 1_layer];
	}

	template <class ...NAMES>
		requires (std::convertible_to<NAMES, std::string_view> && ...)
	std::array<RttiPlaceholder, sizeof...(NAMES)> GetPlaceholders(const NAMES& ...names) const
	{
		return { GetPlaceholder(names)... };
	}

	const std::string& GetFieldName(const RttiPlaceholder& m) const
	{
		for (auto [name, ptr] : m_field_map)
		{
			//layer + indexが一致すれば同一のPlaceholderであると同定できる。
			if (m.GetIndex() == ptr->GetIndex() && m.GetLayer() == ptr->GetLayer()) return name;
		}
		throw InvalidArg("The field does not exist.");
	}
	size_t GetElementSize(LayerType layer) const
	{
		return m_element_sizes[layer + 1_layer];
	}
	bool IsTotallyTrivial(LayerType layer) const
	{
		assert(layer <= MaxLayer);
		return (bool)m_totally_trivial[layer + (LayerType)1];
	}

private:

	std::array<size_t, MaxLayer + 2_layer> m_element_sizes;
	std::array<char, MaxLayer + 2_layer> m_totally_trivial;//あるレイヤーのフィールドが全てトリビアルかどうか。
	std::array<std::vector<RttiPlaceholder>, MaxLayer + 2_layer> m_field_infos_by_layer;
	std::map<std::string, const RttiPlaceholder*, std::ranges::less> m_field_map;
};

}

#endif
