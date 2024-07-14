#ifndef ADAPT_CONTAINER_ELEMENT_REF_H
#define ADAPT_CONTAINER_ELEMENT_REF_H

#include <type_traits>
#include <vector>
#include <format>
#include <variant>
#include <OpenADAPT/Utility/TypeTraits.h>
#include <OpenADAPT/Utility/NamedTuple.h>
#include <OpenADAPT/Utility/Exception.h>
#include <OpenADAPT/Common/Bpos.h>
#include <OpenADAPT/Evaluator/Placeholder.h>
#include <OpenADAPT/Container/ElementBlock.h>

namespace adapt
{

namespace detail
{


template <class Hierarchy, template <class> class Qualifier, class LayerSD>
class ElementListRef_impl;

template <class Hierarchy, template <class> class Qualifier, class LayerSD>
class ElementRef_impl;

template <class Hierarchy, template <class> class Qualifier>
class ElementPtr_impl;

template <class Hierarchy, template <class> class Qualifier>
class ElementIterator_impl;

template <template <class> class Qualifier>
class FieldRef_impl : public eval::detail::RttiMethods<FieldRef_impl<Qualifier>, Qualifier>
{
	static constexpr bool IsNonConst = !std::is_const_v<Qualifier<char>>;

	using Base = eval::detail::RttiMethods<FieldRef_impl<Qualifier>, Qualifier>;

public:

	FieldRef_impl(Qualifier<char>* v, FieldType type) : m_ptr(v), m_type(type) {}

	template <class Type>
		requires IsNonConst
	Type& as()
	{
		if (GetType() != DFieldInfo::ValueTypeToTagType<Type>())
			throw MismatchType(std::format("The type of this field is not {}", typeid(Type).name()));
		return *std::launder(reinterpret_cast<Type*>(m_ptr));
	}
	template <class Type>
	const Type& as() const
	{
		if (GetType() != DFieldInfo::ValueTypeToTagType<Type>())
			throw MismatchType(std::format("The type of this field is not {}", typeid(Type).name()));
		return *std::launder(reinterpret_cast<const Type*>(m_ptr));
	}
	//FieldRefは一時オブジェクトとして使うことが大半であるため、
	//rvalue ref版のas_unsafe関数を作るべきではない。
	template <class Type>
		requires IsNonConst
	Type& as_unsafe() &
	{
		assert(GetType() == DFieldInfo::ValueTypeToTagType<Type>());
		return *std::launder(reinterpret_cast<Type*>(m_ptr));
	}
	template <class Type>
	const Type& as_unsafe() const &
	{
		assert(GetType() == DFieldInfo::ValueTypeToTagType<Type>());
		return *std::launder(reinterpret_cast<const Type*>(m_ptr));
	}
	//FieldRefは一時オブジェクトとして使うことが大半であるため、
	//rvalue ref版のas_unsafe関数を作るべきではない。
	template <class Type>
		requires IsNonConst
	Type* as_ptr()
	{
		if (GetType() != DFieldInfo::ValueTypeToTagType<Type>())
			return nullptr;
		return &as_unsafe<Type>();
	}
	template <class Type>
	const Type* as_ptr() const
	{
		if (GetType() != DFieldInfo::ValueTypeToTagType<Type>())
			return nullptr;
		return &as_unsafe<Type>();
	}

	template <FieldType Type>
		requires IsNonConst
	DFieldInfo::TagTypeToValueType<Type>& as()
	{
		return as<DFieldInfo::TagTypeToValueType<Type>>();
	}
	template <FieldType Type>
	const DFieldInfo::TagTypeToValueType<Type>& as() const
	{
		return as<DFieldInfo::TagTypeToValueType<Type>>();
	}
	//FieldRefは一時オブジェクトとして使うことが大半であるため、
	//rvalue ref版のas_unsafe関数を作るべきではない。
	template <FieldType Type>
		requires IsNonConst
	DFieldInfo::TagTypeToValueType<Type>& as_unsafe()
	{
		return as_unsafe<DFieldInfo::TagTypeToValueType<Type>>();
	}
	template <FieldType Type>
	const DFieldInfo::TagTypeToValueType<Type>& as_unsafe() const
	{
		return as_unsafe<DFieldInfo::TagTypeToValueType<Type>>();
	}
	//FieldRefは一時オブジェクトとして使うことが大半であるため、
	//rvalue ref版のas_unsafe関数を作るべきではない。

	template <FieldType Type>
		requires IsNonConst
	DFieldInfo::TagTypeToValueType<Type>* as_ptr()
	{
		return as_ptr<DFieldInfo::TagTypeToValueType<Type>>();
	}
	template <FieldType Type>
	const DFieldInfo::TagTypeToValueType<Type>* as_ptr() const
	{
		return as_ptr<DFieldInfo::TagTypeToValueType<Type>>();
	}


	//asは厳密に型が一致することを要求するが、toは型をやや変換する。
	//特にINT型、FLT型、CPX型いずれかを指定した場合、それぞれ整数、浮動小数点、複素数であれば変換される。
	//ただし、INTをFLTに、STRをINTに、のような大きな型の種類を変更するような変換は行わない。
	template <std::integral Type>
	Type to() const
	{
		if (this->IsI32()) return (Type)this->i32_unsafe();
		if (this->IsI64()) return (Type)this->i64_unsafe();
		if (this->IsI08()) return (Type)this->i08_unsafe();
		if (this->IsI16()) return (Type)this->i16_unsafe();
		throw MismatchType("This field does not contain an integer.");
	}
	template <std::floating_point Type>
	Type to() const
	{
		if (this->IsF64()) return (Type)this->f64_unsafe();
		if (this->IsF32()) return (Type)this->f32_unsafe();
		throw MismatchType("This field does not contain a floating-point.");
	}
	template <class Type>
		requires IsSame_XT<std::complex, Type>::value
	Type to() const
	{
		if (this->IsC64()) return (Type)this->c64_unsafe();
		if (this->IsC32()) return (Type)this->c32_unsafe();
		throw MismatchType("This field does not contain a complex.");
	}
	template <class Type>
		requires (!std::integral<Type> && !std::floating_point<Type> && !IsSame_XT<std::complex, Type>::value)
	decltype(auto) to() const
	{
		return as<Type>();
	}

	template <FieldType Type>
	decltype(auto) to() const
	{
		using RetType = DFieldInfo::TagTypeToValueType<Type>;
		return to<RetType>();
	}

	//値をVariantに格納して返す。
	FieldVariant var() const
	{
		return FieldVariant(*this);
	}

	FieldType GetType() const { return m_type; }

private:

	Qualifier<char>* m_ptr;
	FieldType m_type;
};

template <class Range>
class CttiExtractor;
template <class Range>
class RttiExtractor;

template <class Hierarchy_, template <class> class Qualifier, class LayerSD>
class ElementListRef_impl
{
	using HierarchyRef = HierarchyWrapper<Hierarchy_>;
	using ElementBlockPolicy = std::conditional_t<s_hierarchy<Hierarchy_>, SElementBlockPolicy, DElementBlockPolicy>;

	static constexpr bool HasStaticLayer = same_as_xn<LayerSD, LayerConstant>;

	template <class Range> friend class CttiExtractor;
	template <class Range> friend class RttiExtractor;
public:

	using Hierarchy = Hierarchy_;
	using ElementBlock = detail::ElementBlock_impl<Hierarchy, ElementBlockPolicy>;
	using ElementRef = ElementRef_impl<Hierarchy, Qualifier, LayerSD>;

	using iterator = ElementIterator_impl<Hierarchy, std::type_identity_t>;
	using const_iterator = ElementIterator_impl<Hierarchy, std::add_const_t>;

	static constexpr bool IsNonConst = !std::is_const_v<Qualifier<char>>;

	ElementListRef_impl(HierarchyRef h, LayerSD layer, Qualifier<ElementBlock>* elms)
		: m_elements(elms), m_layer(layer), m_hierarchy(h)
	{}
	ElementListRef_impl(const ElementListRef_impl&) noexcept = default;
	ElementListRef_impl& operator=(const ElementListRef_impl&) noexcept = default;

	template <std::integral ...Indices>
	ElementRef_impl<Hierarchy, Qualifier, Raise<LayerSD, LayerType(sizeof...(Indices))>>
		GetBranch(BindexType index, Indices ...indices) const
	{
		using Ret = ElementRef_impl<Hierarchy, Qualifier, Raise<LayerSD, LayerType(sizeof...(Indices))>>;
		auto* elm = m_elements->GetBlock_unsafe(GetHierarchy(), GetLayer(), index);
		if (elm == nullptr) return Ret(GetHierarchy(), GetLayer() + LayerConstant<sizeof...(Indices)>{}, nullptr);
		ElementRef ref(GetHierarchy(), GetLayer(), elm);
		return ref.GetBranch(indices...);
	}

	/*ElementRef_impl<Hierarchy, Qualifier, LayerType> GetBranch(const Bpos& bpos, LayerType layer) const
	{
		auto* elm = m_elements->GetBlock(GetHierarchy(), GetLayer(), bpos[GetLayer()]);
		ElementRef ref(GetHierarchy(), GetLayer(), elm);
		return ref.GetBranch(bpos, layer);
	}*/

	bool IsNull() const { return m_elements == nullptr; }

	BindexType GetSize() const
	{
		return m_elements->GetSize(GetHierarchy(), GetLayer());
	}
	bool IsEmpty() const
	{
		return GetSize() == 0;
	}
	BindexType GetCapacity() const
	{
		return m_elements->GetCapacity(GetHierarchy(), GetLayer());
	}

	template <LayerType Layer>
		requires HasStaticLayer
	size_t GetSize(LayerConstant<Layer> layer) const
	{
		if constexpr (Layer == GetLayer()) return GetSize();
		else
		{
			size_t res = 0;
			for (auto&& e : *this) res += e.GetSize(layer);
			return res;
		}
	}
	size_t GetSize(LayerType layer) const
	{
		if (layer == GetLayer()) return GetSize();
		else
		{
			size_t res = 0;
			for (auto&& e : *this) res += e.GetSize(layer);
			return res;
		}
	}

	void Reserve(BindexType size) const
		requires IsNonConst
	{
		m_elements->Reserve(GetHierarchy(), GetLayer(), size);
	}
	void Resize(BindexType size) const
		requires IsNonConst
	{
		m_elements->Resize(GetHierarchy(), GetLayer(), size);
	}
	template <class ...Args>
		requires IsNonConst
	void Push(Args&& ...args) const
	{
		m_elements->Push(GetHierarchy(), GetLayer(), std::forward<Args>(args)...);
	}
	template <class ...Args>
		requires IsNonConst
	void Push_unsafe(Args&& ...args) const
	{
		m_elements->Push_unsafe(GetHierarchy(), GetLayer(), std::forward<Args>(args)...);
	}
	void PushDefaultElement() const
		requires IsNonConst
	{
		m_elements->PushDefaultBlock(GetHierarchy(), GetLayer());
	}
	void Pop() const
		requires IsNonConst
	{
		m_elements->Pop(GetHierarchy(), GetLayer());
	}

	template <class ...Args>
		requires IsNonConst
	void Insert(BindexType index, Args&& ...args) const
	{
		m_elements->Insert(GetHierarchy(), GetLayer(), index, std::forward<Args>(args)...);
	}
	template <class ...Args>
		requires IsNonConst
	void Insert_unsafe(BindexType index, Args&& ...args) const
	{
		m_elements->Insert_unsafe(GetHierarchy(), GetLayer(), index, std::forward<Args>(args)...);
	}
	void Erase(BindexType index, BindexType size) const
		requires IsNonConst
	{
		m_elements->Erase(GetHierarchy(), GetLayer(), index, size);
	}

	constexpr LayerType GetLayer() const { return GetLayer(); }

	//下層の最初の要素を得る。
	ElementRef Front() const
	{
		assert(!IsEmpty());
		return ElementRef(GetHierarchy(), GetLayer(), m_elements->Begin());
	}
	//下層の最後の要素を得る。
	ElementRef Back() const
	{
		assert(!IsEmpty());
		Qualifier<char>* ptr = m_elements->End() - ElementBlock::GetBlockSize(GetHierarchy(), GetLayer());
		return ElementRef(GetHierarchy(), GetLayer(), ptr);
	}

	//下層要素へのイテレータを得る。
	iterator begin() const requires IsNonConst;
	iterator end() const requires IsNonConst;
	const_iterator cbegin() const;
	const_iterator cend() const;
	const_iterator begin() const requires (!IsNonConst) { return cbegin(); }
	const_iterator end() const requires (!IsNonConst) { return cend(); }

private:
	//Extractorが使う。
	//未初期化の要素1個分を確保する。
	//下層要素分のElementBlockだけはちゃんと初期化される。
	void PseudoPush()
	{
		m_elements->PseudoPush(GetHierarchy(), GetLayer());
	}

	//Extractorが使う。ユーザーが使うべきではない。
	//自身は空のリストであるという前提で、bufの要素を全てmoveしつつ構築する。
	//下層要素も含めて移動する。
	void CreateElementsFrom(ElementListRef_impl&& buf) const
		requires IsNonConst
	{
		assert(GetSize() == 0);
		m_elements->CreateElementsFrom(GetHierarchy(), GetLayer(), std::move(*buf.m_elements));
	}
	void SwapElementBlock(ElementListRef_impl& buf) const
	{
		assert(GetSize() == 0);
		m_elements->Swap(*buf.m_elements);
	}
	//Extractorが使う。ユーザーが使うべきではない。
	//ExtractorによってReserveされた領域の要素を書き込んだ後、
	//その時点ではサイズが0のままになっているため、
	//この関数で強制的にサイズを変更する。
	//あくまでm_endの値を変更するだけで、中身を一切変更しない。
	void RewriteSize(BindexType size) const
	{
		m_elements->RewriteSize(GetHierarchy(), GetLayer(), size);
	}
	//emptyでも呼べる。
	ElementRef Back_unsafe() const
	{
		Qualifier<char>* ptr = m_elements->End() - ElementBlock::GetBlockSize(GetHierarchy(), GetLayer());
		return ElementRef(GetHierarchy(), GetLayer(), ptr);
	}

private:
	HierarchyRef GetHierarchy() const requires (!s_hierarchy<Hierarchy>) { return m_hierarchy; }
	static consteval HierarchyRef GetHierarchy() requires s_hierarchy<Hierarchy> { return {}; }
	LayerType GetLayer() const requires (!HasStaticLayer) { return m_layer; }
	static consteval auto GetLayer() requires HasStaticLayer { return LayerSD{}; }

	Qualifier<ElementBlock>* m_elements;
	[[no_unique_address]] LayerSD m_layer = {};//要素と要素の中継を担うクラスだが、階層は下層側（つまりメンバに持つm_elementsの方）に合わせられている。
	[[no_unique_address]] HierarchyRef m_hierarchy;
};

//このクラスはLayerをコンパイル時に与えるか実行時に与えるかを使い分けられる。
//例えばTraverserに与えるSTree::iteratorはlayerを実行時に与えなければ配列化出来ない。
//しかし、STree::opearor[]から続けて得るときはlayerをstaticに決定しても差し支えない。
template <class Hierarchy_, template <class> class Qualifier, class LayerSD>
class ElementRef_impl
{
	template <class Hierarchy__, template <class> class Qualifier_, class LayerSD_>
	friend class ElementRef_impl;
	friend class ElementIterator_impl<Hierarchy_, Qualifier>;
	using ElementBlockPolicy = std::conditional_t<s_hierarchy<Hierarchy_>, SElementBlockPolicy, DElementBlockPolicy>;

	template <class Range> friend class CttiExtractor;
	template <class Range> friend class RttiExtractor;
public:

	using Hierarchy = Hierarchy_;
	using ElementBlock = detail::ElementBlock_impl<Hierarchy, ElementBlockPolicy>;

	using RttiPlaceholder = Hierarchy::RttiPlaceholder;
	template <class Type>
	using TypedPlaceholder = Hierarchy::template TypedPlaceholder<Type>;
	template <LayerType Layer, class Type>
	using CttiPlaceholder = Hierarchy::template CttiPlaceholder<Layer, Type>;

	using FieldRef = FieldRef_impl<Qualifier>;

	using iterator = ElementIterator_impl<Hierarchy, std::type_identity_t>;
	using const_iterator = ElementIterator_impl<Hierarchy, std::add_const_t>;

private:
	using HierarchyRef = HierarchyWrapper<Hierarchy>;
	static constexpr bool HasStaticLayer = same_as_xn<LayerSD, LayerConstant>;
	static constexpr bool HasFixedMaxLayer = f_hierarchy<Hierarchy> || s_hierarchy<Hierarchy>;
public:

	static constexpr bool IsNonConst = !std::is_const_v<Qualifier<char>>;

	ElementRef_impl() = default;
	//SElementRef_impl(Qualifier<char>* block) : m_block(block) {}
	ElementRef_impl(HierarchyRef p, LayerSD layer, Qualifier<char>* block)
		: m_block(block), m_layer(layer), m_hierarchy(p)
	{}

	ElementRef_impl(const ElementRef_impl&) noexcept = default;
	//non-const -> constという変換を許す。
	ElementRef_impl(const ElementRef_impl<Hierarchy, std::add_const_t, LayerSD>& e) noexcept
		requires IsNonConst
		: m_block(e.m_block), m_layer(e.m_layer), m_hierarchy(e.m_hierarchy)
	{}
	//LayerConstant -> LayerTypeの変換を許す。
	template <LayerType L>
		requires std::same_as<LayerSD, LayerType>
	ElementRef_impl(const ElementRef_impl<Hierarchy, Qualifier, LayerConstant<L>>& e) noexcept
		: m_block(e.m_block), m_layer(e.m_layer), m_hierarchy(e.m_hierarchy)
	{}

	ElementRef_impl& operator=(const ElementRef_impl&) noexcept = default;
	//non-const -> constという変換を許す。
	ElementRef_impl& operator=(const ElementRef_impl<Hierarchy, std::add_const_t, LayerSD>& e) noexcept
		requires IsNonConst
	{
		m_block = e.m_block;
		m_layer = e.m_layer;
		m_hierarchy = e.m_hierarchy;
	}
	//LayerConstant -> LayerTypeの変換を許す。
	template <LayerType L>
		requires std::same_as<LayerSD, LayerType>
	ElementRef_impl& operator=(const ElementRef_impl<Hierarchy, Qualifier, LayerSD>& e) noexcept
	{
		m_block = e.m_block;
		m_layer = e.m_layer;
		m_hierarchy = e.m_hierarchy;
	}

	//----------Ctti----------
	template <LayerType Layer, class Type>
	Qualifier<Type>& GetField(const CttiPlaceholder<Layer, Type>& ph) const
	{
		if constexpr (HasStaticLayer) static_assert([]() { return Layer == LayerSD{}; }());
		else assert(Layer == m_layer);
		return *std::launder(reinterpret_cast<Qualifier<Type>*>(m_block + ph.GetPtrOffset()));
	}
	template <LayerType Layer, class Type>
	Qualifier<Type>& operator[](const CttiPlaceholder<Layer, Type>& ph) const
	{
		return GetField(ph);
	}
	template <StaticChar Name_>
		requires s_hierarchy<Hierarchy_>
	decltype(auto) operator[](StaticString<Name_>) const
	{
		return operator[](Hierarchy::template GetPlaceholder<Name_>());
	}

	//----------Typed----------
	template <class Type>
	Qualifier<Type>& GetField(const TypedPlaceholder<Type>& ph) const
	{
		assert(ph.GetInternalLayer() == m_layer);
		return *std::launder(reinterpret_cast<Qualifier<Type>*>(m_block + ph.GetPtrOffset()));
	}
	template <class Type>
	Qualifier<Type>& operator[](const TypedPlaceholder<Type>& ph) const
	{
		return GetField(ph);
	}

	//----------Rtti----------
	FieldRef GetField(const RttiPlaceholder& ph) const
	{
		assert(ph.GetInternalLayer() == m_layer);
		return FieldRef(m_block + ph.GetPtrOffset(), ph.GetType());
	}
	FieldRef operator[](const RttiPlaceholder& ph) const
	{
		return GetField(ph);
	}
	FieldRef operator[](std::string_view fieldname) const
	{
		const auto& h = GetHierarchy().value();
		return GetField(h.GetPlaceholder(fieldname));
	}

	ElementRef_impl<Hierarchy, Qualifier, LayerType> GetBranch(const Bpos& bpos, LayerType layer) const
	{
		if (layer == m_layer) return *this;
		ElementRef_impl<Hierarchy, Qualifier, LayerType> e = GetBranch(bpos[m_layer + 1_layer]);
		return e.GetBranch(bpos, layer);
	}
	template <LayerType Layer>
		requires HasStaticLayer
	ElementRef_impl<Hierarchy, Qualifier, LayerConstant<Layer>> GetBranch(const Bpos& bpos, LayerConstant<Layer> layer) const
	{
		if constexpr (Layer == LayerSD{}) return *this;
		else
		{
			auto e = GetBranch(bpos[m_layer + 1_layer]);
			return e.GetBranch(bpos, layer);
		}
	}
	ElementRef_impl<Hierarchy, Qualifier, LayerType> GetBranch(const Bpos& bpos) const
	{
		return GetBranch(bpos, bpos.GetLayer());
	}

	template <std::integral ...Indices>
	ElementRef_impl<Hierarchy, Qualifier, Raise<LayerSD, LayerType(sizeof...(Indices) + 1)>>
		GetBranch(BindexType index, Indices ...is) const
	{
		return GetLowerElements().GetBranch(index, is...);
	}
	ElementRef_impl GetBranch() const
	{
		return *this;
	}
	ElementRef_impl<Hierarchy, Qualifier, Raise<LayerSD, LayerType(1)>> operator[](BindexType index) const
	{
		return GetBranch(index);
	}

	BindexType GetSize() const
	{
		return GetLowerElements().GetSize();
	}
	bool IsEmpty() const
	{
		return GetLowerElements().IsEmpty();
	}
	BindexType GetCapacity() const
	{
		return GetLowerElements().GetCapacity();
	}
	template <LayerType Layer>
		requires HasStaticLayer
	size_t GetSize(LayerConstant<Layer> layer) const
	{
		if constexpr (s_hierarchy<Hierarchy> || f_hierarchy<Hierarchy>)
			static_assert(Layer <= Hierarchy::GetMaxLayer());
		else
			assert(layer <= GetHierarchy().value().GetMaxLayer());
		return GetLowerElements().GetSize(layer);
	}
	size_t GetSize(LayerType layer) const
	{
		assert(layer <= GetHierarchy().value().GetMaxLayer());
		return GetLowerElements().GetSize(layer);
	}

	/*template <class ...Args>
		requires IsNonConst
	void CreateFields(Args&& ...args) const
	{
		ElementBlock::Create(m_hierarchy.value().GetPlaceholdersIn(m_layer),
							 m_block, std::forward<Args>(args)...);
	}*/

	void Reserve(BindexType size) const
		requires IsNonConst
	{
		return GetLowerElements().Reserve(size);
	}
	void Resize(BindexType size) const
		requires IsNonConst
	{
		return GetLowerElements().Resize(size);
	}
	template <class ...Args>
		requires IsNonConst
	void Push(Args&& ...args)
	{
		return GetLowerElements().Push(std::forward<Args>(args)...);
	}
	template <class ...Args>
		requires IsNonConst
	void Push_unsafe(Args&& ...args)
	{
		return GetLowerElements().Push_unsafe(std::forward<Args>(args)...);
	}
	void Pop() const
		requires IsNonConst
	{
		return GetLowerElements().Pop();
	}

	template <class ...Args>
		requires IsNonConst
	void Assign(Args&& ...args) const
	{
		ElementBlockPolicy::Assign(
			ElementBlock::GetPlaceholdersIn(GetHierarchy(), GetLayer()),
			m_block, std::forward<Args>(args)...);
	}
	template <class ...Args>
		requires IsNonConst
	void Assign_unsafe(Args&& ...args) const
	{
		ElementBlockPolicy::Assign_unsafe(
			ElementBlock::GetPlaceholdersIn(GetHierarchy(), GetLayer()),
			m_block, std::forward<Args>(args)...);
	}

	//const Hierarchy* GetHierarchy() const { return m_hierarchy.value(); }
	//LayerType GetLayer() const requires (!HasStaticLayer) { return m_layer; }
	//static constexpr LayerType GetLayer() requires HasStaticLayer { return LayerSD{}; }

	//下層の最初の要素を得る。
	auto Front() const
	{
		return GetLowerElements().Front();
	}
	//下層の最後の要素を得る。
	auto Back() const
	{
		return GetLowerElements().Back();
	}

	//下層要素へのイテレータを得る。
	iterator begin() const requires IsNonConst { return GetLowerElements().begin(); }
	iterator end() const requires IsNonConst { return GetLowerElements().end(); }
	const_iterator cbegin() const { return GetLowerElements().cbegin(); }
	const_iterator cend() const { return GetLowerElements().cend(); }
	const_iterator begin() const requires (!IsNonConst) { return cbegin(); }
	const_iterator end() const requires (!IsNonConst) { return cend(); }

	//下層要素ではなく、自分自身を指すイテレータに変換する。
	iterator ConvertToIterator() const
	{
		//グローバル変数要素に対しては使えない。
		assert(m_layer >= 0);
		return iterator{ m_hierarchy, m_layer, m_block };
	}

	auto GetLowerElements() const
	{
		if constexpr (HasStaticLayer)
			return ElementListRef_impl<Hierarchy, Qualifier, LayerConstant<LayerSD{} + 1_layer>>(
				m_hierarchy, m_layer + 1_layer, GetLowerBlock());
		else
			return ElementListRef_impl<Hierarchy, Qualifier, LayerType>(
				m_hierarchy, LayerType(m_layer + 1), GetLowerBlock());
	}

	const char* GetBlock() const { return m_block; }

private:
	//Extractorが使う。ユーザーが使うべきではない。
	//自身はメモリ確保されただけの未初期化状態であるという前提で、bufの要素を全てmoveしつつ構築する。
	//bufの中身はデストラクタを呼ばれた状態になる。
	void CreateElementFrom(ElementRef_impl&& e) const
		requires IsNonConst
	{
		//s_hierarchyの場合、構造が一致するのはこの関数を呼び出せている時点で明らかである、
		//一方d_hierarchyやf_hierarchyの場合は、ポインタの一致を確認しておく必要がある。
		if constexpr (!s_hierarchy<Hierarchy>)
		{
			assert(e.GetHierarchy() == GetHierarchy());
			assert(GetLayer() == e.GetLayer());
		}
		ElementBlock::MoveConstructAndDestroyElementFrom(GetHierarchy(), GetLayer(), e.m_block, m_block);
	}
	//Extractorが使う。ユーザーが使うべきではない。
	//srcの全フィールドを自身にコピーする。
	//srcと自身の構造は基本的に一致しているが、自身の末尾には別のフィールドが付与されている場合がある。
	template <class LayerSD1, class Hierarchy__, template <class> class Qualifier_, class LayerSD2>
	void CopyFieldsTo(LayerSD1 layer, ElementRef_impl<Hierarchy__, Qualifier_, LayerSD2>& dst) const
	{
		//この関数はExtractorが呼ぶ。つまり、thisはTraverserが保持するElementIteratorから得たものであり、
		//ElementRef_impl（引数ではなく自身）のLayerSDはLayerTypeである。
		//ということは、GetLayer()でLayerConstantを得られないため、CppyFields関数をLayerConstantで呼ぶことができない。
		//これだと、STreeからこの関数を呼ぶときにElementBlock::GetPlaceholdersInを呼ぶことができなくなる。
		//それではまずいので、外からLayerをテンプレート引数で受け取る。
		//DTreeなどはLayerはLayerTypeとなるので、LayerSD1はどちらも受け取れるようにしておく。
		assert(layer == GetLayer());
		ElementBlock::CopyFields(GetHierarchy(), layer, m_block, dst.m_block);
	}
	//Extractorが使う。ユーザーが使うべきではない。
	//受け取った値を用いて、特定のフィールドをコピーコンストラクタを呼びつつ初期化する。
	template <FieldType Type, class T>
	void ConstructFieldFrom(const RttiPlaceholder& ph, const T& v) const
		requires IsNonConst
	{
		using ValueType = typename DFieldInfo::template TagTypeToValueType<Type>;
		auto* ptr = std::launder(reinterpret_cast<Qualifier<ValueType>*>(m_block + ph.GetPtrOffset()));
		new (ptr) ValueType(v);
	}


	Qualifier<ElementBlock>* GetLowerBlock() const
	{
		if constexpr (HasStaticLayer && HasFixedMaxLayer)
		{
			static_assert([]() { return LayerSD{} < Hierarchy::GetMaxLayer(); } ());
			//return std::launder(reinterpret_cast<Qualifier<ElementBlock>*>(m_block + Hierarchy::GetElementSize(LayerSD{})));
		}
		else
		{
			assert(GetHierarchy().value().GetMaxLayer() != GetLayer());
			//return std::launder(reinterpret_cast<Qualifier<ElementBlock>*>(m_block + m_hierarchy.value().GetElementSize((LayerType)m_layer)));
		}
		//if constexpr (HasStaticLayer) return GetLowerBlock_impl(m_block, m_hierarchy, LayerSD{});
		//else return GetLowerBlock_impl(m_block, m_hierarchy, (LayerType)m_layer);
		size_t elmsize = ElementBlock::GetElementSize(GetHierarchy(), GetLayer());
		return std::launder(reinterpret_cast<Qualifier<ElementBlock>*>(m_block + elmsize));
	}
	const ElementBlock* GetLowerBlockForDebug() const
	{
		size_t elmsize = ElementBlock::GetElementSize(GetHierarchy(), GetLayer());
		return (const ElementBlock*)(m_block + elmsize);
	}

	const HierarchyRef& GetHierarchy() const requires (!s_hierarchy<Hierarchy>) { return m_hierarchy; }
	static consteval HierarchyRef GetHierarchy() requires s_hierarchy<Hierarchy> { return HierarchyRef{}; }
	LayerType GetLayer() const requires (!HasStaticLayer) { return m_layer; }
	static consteval LayerSD GetLayer() requires HasStaticLayer { return {}; }

	Qualifier<char>* m_block = nullptr;
	[[no_unique_address]] LayerSD m_layer;//LayerSDはLayerTypeまたはLayerConstant。
	[[no_unique_address]] HierarchyRef m_hierarchy;
};

template <class Hierarchy_, template <class> class Qualifier>
class ElementPtr_impl
{
public:
	using Hierarchy = Hierarchy_;
	ElementPtr_impl(ElementRef_impl<Hierarchy, Qualifier, LayerType> ref) : m_ref(ref) {}
	const ElementRef_impl<Hierarchy, Qualifier, LayerType>* operator->() const { return &m_ref; }
private:
	ElementRef_impl<Hierarchy, Qualifier, LayerType> m_ref;
};

template <class Hierarchy_, template <class> class Qualifier>
class ElementIterator_impl
{
	using HierarchySD = HierarchyWrapper<Hierarchy_>;
	using FieldRef = FieldRef_impl<Qualifier>;
	using ElementBlockPolicy = std::conditional_t<s_hierarchy<Hierarchy_>, SElementBlockPolicy, DElementBlockPolicy>;

public:
	
	using Hierarchy = Hierarchy_;
	using ElementBlock = detail::ElementBlock_impl<Hierarchy, ElementBlockPolicy>;
	using ElementRef = ElementRef_impl<Hierarchy, Qualifier, LayerType>;
	using ElementPtr = ElementPtr_impl<Hierarchy, Qualifier>;

	//using iterator_category = std::random_access_iterator_tag;
	using value_type = ElementRef;
	using difference_type = ptrdiff_t;
	using pointer = ElementPtr_impl<Hierarchy, Qualifier>;
	using reference = ElementRef;

	static constexpr bool IsNonConst = !std::is_const_v<Qualifier<char>>;

	//デフォルト構築可能にするために用意しておくが、
	//この状態ではあらゆるメンバ関数を使用できない。
	ElementIterator_impl()
		: m_ref(nullptr, -1, nullptr), m_block_size(0)
	{}
	ElementIterator_impl(HierarchySD h, LayerType layer, Qualifier<char>* block)
		: m_ref(h, layer, block), m_block_size(0)
	{
		//const auto& href = m_ref.m_hierarchy.value();
		m_block_size = ElementBlock::GetBlockSize(GetHierarchy(), layer);
		//m_block_size = href.GetElementSize(layer) + (href.GetMaxLayer() != layer) * sizeof(ElementBlock);
	}

	ElementIterator_impl(const ElementIterator_impl&) = default;
	ElementIterator_impl(ElementIterator_impl&&) noexcept = default;

	ElementIterator_impl& operator=(const ElementIterator_impl&) = default;
	ElementIterator_impl& operator=(ElementIterator_impl&&) noexcept = default;

	ElementIterator_impl(const ElementIterator_impl<Hierarchy, std::type_identity_t>& e) noexcept
		requires IsNonConst
		: m_ref(e.m_ref), m_block_size(e.m_block_size)
	{}
	/*ElementIterator_impl(ElementIterator_impl<Hierarchy, std::type_identity_t>&& e) noexcept
		requires IsNonConst
		: m_ref(e.m_ref), m_block_size(e.m_block_size)
	{
		m_ref = {};
		e.m_block_size = 0;
	}*/

	ElementIterator_impl& operator=(const ElementIterator_impl<Hierarchy, std::type_identity_t>& e) noexcept
		requires IsNonConst
	{
		m_ref = e.m_ref;
		m_block_size = e.m_block_size;
		return *this;
	}
	/*ElementIterator_impl& operator=(ElementIterator_impl<Hierarchy, std::type_identity_t>&& e) noexcept
		requires IsNonConst
	{
		m_ref = e.m_ref; e.m_ref = {};
		m_block_size = e.m_block_size; e.m_block_size = 0;
		return *this;
	}*/

	ElementIterator_impl& operator++()
	{
		m_ref.m_block += m_block_size;
		return *this;
	}
	ElementIterator_impl operator++(int)
	{
		auto res = *this;
		++(*this);
		return res;
	}
	ElementIterator_impl& operator--()
	{
		m_ref.m_block -= m_block_size;
		return *this;
	}
	ElementIterator_impl operator--(int)
	{
		auto res = *this;
		--(*this);
		return res;
	}
	ElementIterator_impl& operator+=(difference_type diff)
	{
		m_ref.m_block += diff * m_block_size;
		return *this;
	}
	ElementIterator_impl& operator-=(difference_type diff)
	{
		m_ref.m_block -= diff * m_block_size;
		return *this;
	}

	ElementIterator_impl operator+(difference_type diff) const
	{
		auto res = *this;
		return res += diff;
	}
	difference_type operator-(const ElementIterator_impl& e) const
	{
		assert(m_ref.m_hierarchy == e.m_ref.m_hierarchy);
		assert(m_ref.m_layer == e.m_ref.m_layer);
		assert(m_block_size == e.m_block_size);
		return (m_ref.m_block - e.m_ref.m_block) / m_block_size;
	}
	ElementIterator_impl operator-(difference_type diff) const
	{
		auto res = *this;
		return res -= diff;
	}

	ElementRef operator[](difference_type diff) const
	{
		return *(*this + diff);
	}

	bool operator==(const ElementIterator_impl& e) const
	{
		assert(m_ref.m_hierarchy == e.m_ref.m_hierarchy);
		assert(m_ref.m_layer == e.m_ref.m_layer);
		return m_ref.m_block == e.m_ref.m_block;
	}
	bool operator!=(const ElementIterator_impl& e) const
	{
		return !(*this == e);
	}

	bool operator<(const ElementIterator_impl& e) const
	{
		assert(m_ref.m_hierarchy == e.m_ref.m_hierarchy);
		assert(m_ref.m_layer == e.m_ref.m_layer);
		return m_ref.m_block < e.m_ref.m_block;
	}
	bool operator<=(const ElementIterator_impl& e) const
	{
		assert(m_ref.m_hierarchy == e.m_ref.m_hierarchy);
		assert(m_ref.m_layer == e.m_ref.m_layer);
		return m_ref.m_block <= e.m_ref.m_block;
	}
	bool operator>(const ElementIterator_impl& e) const
	{
		assert(m_ref.m_hierarchy == e.m_ref.m_hierarchy);
		assert(m_ref.m_layer == e.m_ref.m_layer);
		return m_ref.m_block > e.m_ref.m_block;
	}
	bool operator>=(const ElementIterator_impl& e) const
	{
		assert(m_ref.m_hierarchy == e.m_ref.m_hierarchy);
		assert(m_ref.m_layer == e.m_ref.m_layer);
		return m_ref.m_block >= e.m_ref.m_block;
	}

	ElementRef operator*() const
	{
		return m_ref;
	}
	/*ElementRef operator=(const FieldRef& m) const
	{
		return m_ref;
	}*/
	ElementPtr operator->() const
	{
		return ElementPtr{ m_ref };
	}

private:

	HierarchySD GetHierarchy() const requires (!s_hierarchy<Hierarchy>) { return m_ref.GetHierarchy(); }
	static consteval HierarchySD GetHierarchy() requires s_hierarchy<Hierarchy> { return ElementRef::GetHierarchy(); }

	ElementRef m_ref;
	size_t m_block_size;
};

template <class Container, template <class> class Qualifier, class LayerSD>
ElementListRef_impl<Container, Qualifier, LayerSD>::iterator
ElementListRef_impl<Container, Qualifier, LayerSD>::begin() const
	requires IsNonConst
{
	return iterator(m_hierarchy, m_layer, m_elements->Begin());
}
template <class Container, template <class> class Qualifier, class LayerSD>
ElementListRef_impl<Container, Qualifier, LayerSD>::iterator
ElementListRef_impl<Container, Qualifier, LayerSD>::end() const
	requires IsNonConst
{
	return iterator(m_hierarchy, m_layer, m_elements->End());
}
template <class Container, template <class> class Qualifier, class LayerSD>
ElementListRef_impl<Container, Qualifier, LayerSD>::const_iterator
ElementListRef_impl<Container, Qualifier, LayerSD>::cbegin() const
{
	return const_iterator(m_hierarchy, m_layer, m_elements->CBegin());
}
template <class Container, template <class> class Qualifier, class LayerSD>
ElementListRef_impl<Container, Qualifier, LayerSD>::const_iterator
ElementListRef_impl<Container, Qualifier, LayerSD>::cend() const
{
	return const_iterator(m_hierarchy, m_layer, m_elements->CEnd());
}
}

}

#endif
