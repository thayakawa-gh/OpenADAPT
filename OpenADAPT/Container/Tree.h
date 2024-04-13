#ifndef ADAPT_CONTAINER_TREE_H
#define ADAPT_CONTAINER_TREE_H

#include <OpenADAPT/Utility/NamedTuple.h>
#include <OpenADAPT/Container/ElementRef.h>
#include <OpenADAPT/Traverser/Traverser.h>

namespace adapt
{

namespace detail
{

template <class Container_, class Hierarchy_, class ElementBlockPolicy_>
class Tree_base : public Hierarchy_
{
	template <class Tree,
		template <class, template <class> class> class Iterator,
		template <class> class Qualifier>
	friend class detail::Traverser_impl;


	using ElementBlockPolicy = ElementBlockPolicy_;

public:

	static constexpr RankType MaxRank = 0_rank;

	using Container = Container_;
	using Hierarchy = Hierarchy_;

	using Traverser = Traverser_impl<Container, ElementIterator_impl, std::type_identity_t>;
	using ConstTraverser = Traverser_impl<Container, ElementIterator_impl, std::add_const_t>;

	using Sentinel = Sentinel_impl<Container, ElementIterator_impl, std::type_identity_t>;
	using ConstSentinel = Sentinel_impl<Container, ElementIterator_impl, std::add_const_t>;

	using Range = Range_impl<Container, std::type_identity_t>;
	using ConstRange = Range_impl<Container, std::add_const_t>;

	using RttiPlaceholder = Hierarchy::RttiPlaceholder;
	template <class Type>
	using TypedPlaceholder = Hierarchy::template TypedPlaceholder<Type>;
	template <LayerType Layer, class Type>
	using CttiPlaceholder = Hierarchy::template CttiPlaceholder<Layer, Type>;

	using ElementBlock = ElementBlock_impl<Hierarchy, ElementBlockPolicy>;

	template <LayerType Layer>
	using ElementRefF = ElementRef_impl<Hierarchy, std::type_identity_t, LayerConstant<Layer>>;
	template <LayerType Layer>
	using ConstElementRefF = ElementRef_impl<Hierarchy, std::add_const_t, LayerConstant<Layer>>;

	using TopElementRef = ElementRef_impl<Hierarchy, std::type_identity_t, LayerConstant<(LayerType)-1>>;
	using TopConstElementRef = ElementRef_impl<Hierarchy, std::add_const_t, LayerConstant<(LayerType)-1>>;

	using ElementRef_0 = ElementRef_impl<Hierarchy, std::type_identity_t, LayerConstant<(LayerType)0>>;
	using ConstElementRef_0 = ElementRef_impl<Hierarchy, std::add_const_t, LayerConstant<(LayerType)0>>;

	using ElementRef = ElementRef_impl<Hierarchy, std::type_identity_t, LayerType>;
	using ConstElementRef = ElementRef_impl<Hierarchy, std::add_const_t, LayerType>;


	using FieldRef = FieldRef_impl<std::type_identity_t>;
	using ConstFieldRef = FieldRef_impl<std::add_const_t>;

private:
	void Construct()
	{
		//グローバル変数の準備。
		size_t elmsize = Hierarchy::GetElementSize(-1_layer);
		m_top_element = (char*)std::malloc(elmsize + sizeof(ElementBlock));
		ElementBlock::Create_default(Hierarchy::GetPlaceholdersIn(-1_layer), m_top_element);
		ElementBlock::CreateElementBlock(m_top_element + elmsize);
	}
public:

	Tree_base()
	{
		if constexpr (s_hierarchy<Hierarchy>) Construct();
	}

	~Tree_base()
	{
		//global elementがnullptrでない場合、これらを削除する。
		if (m_top_element)
		{
			const auto& ms = Hierarchy::GetPlaceholdersIn(-1_layer);
			ElementBlock::Destroy(ms, Hierarchy::IsTotallyTrivial(-1_layer), m_top_element);
			auto e_ptr = GetBlockOneLayerDown();
			e_ptr->Destruct(this, 0_layer);
			e_ptr->~ElementBlock();
			std::free(m_top_element);
		}
	}
	Tree_base(const Tree_base&) = delete;
	Tree_base(Tree_base&& h) noexcept
		: Hierarchy(std::move(h))
	{
		m_top_element = h.m_top_element;
		h.m_top_element = nullptr;
	}

	Tree_base& operator=(const Tree_base&) = delete;
	Container& operator=(Tree_base&& h) noexcept
	{
		Hierarchy::operator=(std::move(h));
		m_top_element = h.m_top_element;
		h.m_top_element = nullptr;
		return static_cast<Container&>(*this);
	}

	void VerifyStructure()
		requires d_hierarchy<Hierarchy> || f_hierarchy<Hierarchy>
	{
		//Hierarchy::VerifyStructure();
		Construct();
	}

	template <class ...Args>
	void SetTopFields(Args&& ...args)
	{
		ElementBlock::Create(Hierarchy::GetPlaceholdersIn(-1_layer), m_top_element, std::forward<Args>(args)...);
	}

	void Reserve(BindexType size)
	{
		GetLowerElements().Reserve(size);
	}
	template <class ...Args>
	void Push(Args&& ...args)
	{
		GetLowerElements().Push(std::forward<Args>(args)...);
	}
	template <class ...Args>
	void Push_unsafe(Args&& ...args)
	{
		GetLowerElements().Push_unsafe(std::forward<Args>(args)...);
	}

	TopElementRef GetTopElement()
	{
		return { this, -1_layer, m_top_element };
	}
	TopConstElementRef GetTopElement() const
	{
		return { this, -1_layer, m_top_element };
	}
	TopElementRef GetBranch()
	{
		return GetTopElement();
	}
	TopConstElementRef GetBranch() const
	{
		return GetTopElement();
	}
	template <std::integral ...Indices>
	ElementRefF<(LayerType)sizeof...(Indices)> GetBranch(BindexType index, Indices ...indices)
	{
		return GetLowerElements().GetBranch(index, indices...);
	}
	template <std::integral ...Indices>
	ConstElementRefF<(LayerType)sizeof...(Indices)> GetBranch(BindexType index, Indices ...indices) const
	{
		return GetLowerElements().GetBranch(index, indices...);
	}

	ElementRef GetBranch(const Bpos& bpos)
	{
		return GetBranch(bpos, bpos.GetLayer());
	}
	ConstElementRef GetBranch(const Bpos& bpos) const
	{
		return GetBranch(bpos, bpos.GetLayer());
	}
	ElementRef GetBranch(const Bpos& bpos, LayerType layer)
	{
		assert(bpos.GetLayer() != -1);
		return GetTopElement().GetBranch(bpos, layer);
	}
	ConstElementRef GetBranch(const Bpos& bpos, LayerType layer) const
	{
		assert(bpos.GetLayer() != -1);
		return GetTopElement().GetBranch(bpos, layer);
	}
	template <LayerType Layer>
	auto GetBranch(const Bpos& bpos, LayerConstant<Layer> layer)
	{
		assert(bpos.GetLayer() != -1);
		return GetTopElement().GetBranch(bpos, layer);
	}
	template <LayerType Layer>
	auto GetBranch(const Bpos& bpos, LayerConstant<Layer> layer) const
	{
		assert(bpos.GetLayer() != -1);
		return GetTopElement().GetBranch(bpos, layer);
	}

	//layer層要素数を返す。
	//1層以上を与えた場合、結果はBindexTypeの最大値を超える可能性があるので、size_tで返す。
	template <LayerType Layer>
	size_t GetSize(LayerConstant<Layer> layer) const
	{
		return GetTopElement().GetSize(layer);
	}
	size_t GetSize(LayerType layer) const
	{
		return GetTopElement().GetSize(layer);
	}

	ElementRef_0 operator[](BindexType index)
	{
		return GetBranch(index);
	}
	ConstElementRef_0 operator[](BindexType index) const
	{
		return GetBranch(index);
	}

	ElementRef operator[](const Bpos& bpos)
	{
		return GetBranch(bpos);
	}
	ConstElementRef operator[](const Bpos& bpos) const
	{
		return GetBranch(bpos);
	}

	ElementRef_0 Front()
	{
		return GetLowerElements().Front();
	}
	ConstElementRef_0 Front() const
	{
		return GetLowerElements().Front();
	}
	ElementRef_0 Back()
	{
		return GetLowerElements().Back();
	}
	ConstElementRef_0 Back() const
	{
		return GetLowerElements().Back();
	}

	template <LayerType Layer, class Type>
	decltype(auto) GetField(const CttiPlaceholder<Layer, Type>& ph) { return GetTopElement()[ph]; }
	template <LayerType Layer, class Type>
	decltype(auto) operator[](const CttiPlaceholder<Layer, Type>& ph) { return GetField(ph); }
	template <LayerType Layer, class Type>
	decltype(auto) GetField(const CttiPlaceholder<Layer, Type>& ph) const { return GetTopElement()[ph]; }
	template <LayerType Layer, class Type>
	decltype(auto) operator[](const CttiPlaceholder<Layer, Type>& ph) const { return GetField(ph); }

	template <class Type>
	decltype(auto) GetField(const TypedPlaceholder<Type>& ph) { return GetTopElement()[ph]; }
	template <class Type>
	decltype(auto) operator[](const TypedPlaceholder<Type>& ph) { return GetField(ph); }
	template <class Type>
	decltype(auto) GetField(const TypedPlaceholder<Type>& ph) const { return GetTopElement()[ph]; }
	template <class Type>
	decltype(auto) operator[](const TypedPlaceholder<Type>& ph) const { return GetField(ph); }

	FieldRef GetField(const RttiPlaceholder& ph) { return GetTopElement()[ph]; }
	FieldRef operator[](const RttiPlaceholder& ph) { return GetField(ph); }
	ConstFieldRef GetField(const RttiPlaceholder& ph) const { return GetTopElement()[ph]; }
	ConstFieldRef operator[](const RttiPlaceholder& ph) const { return GetField(ph); }


	//DHTree::Traverserはiteratorに近い振る舞いをするが、
	//固定層、走査層、固定位置の指定をしなければならない。
	//そのため、それらを指定してrange化する関数を用意しておく。
	//auto range = tree.GetRange(2);//走査層に2を指定
	//auto it = range.begin();
	//auto end = range.end();
	//のように使う。
	Range GetRange() { return Range(static_cast<Container&>(*this)); }
	Range GetRange(LayerType trav) { return Range(static_cast<Container&>(*this), trav); }
	Range GetRange(const Bpos& pos, LayerType trav) { return Range(static_cast<Container&>(*this), pos, trav); }
	Range GetRange(LayerType fix, const Bpos& pos) { return Range(fix, static_cast<Container&>(*this), pos); }
	Range GetRange(LayerType fix, const Bpos& pos, LayerType trav) { return Range(fix, static_cast<Container&>(*this), pos, trav); }
	ConstRange GetRange() const { return ConstRange(static_cast<const Container&>(*this)); }
	ConstRange GetRange(LayerType trav) const { return ConstRange(static_cast<const Container&>(*this), trav); }
	ConstRange GetRange(const Bpos& pos, LayerType trav) const { return ConstRange(static_cast<const Container&>(*this), pos, trav); }
	ConstRange GetRange(LayerType fix, const Bpos& pos) const { return ConstRange(fix, static_cast<const Container&>(*this), pos); }
	ConstRange GetRange(LayerType fix, const Bpos& pos, LayerType trav) const { return ConstRange(fix, static_cast<const Container&>(*this), pos, trav); }


	auto GetLowerElements() const
	{
		//if constexpr (s_hierarchy<Hierarchy>)
			return ElementListRef_impl<Hierarchy, std::add_const_t, LayerConstant<0_layer>>(this, 0_layer, GetBlockOneLayerDown());
		//else
		//	return ElementListRef_impl<Hierarchy, std::add_const_t, LayerType>(this, 0_layer, GetBlockOneLayerDown());
	}
	auto GetLowerElements()
	{
		//if constexpr (s_hierarchy<Hierarchy>)
			return ElementListRef_impl<Hierarchy, std::type_identity_t, LayerConstant<0_layer>>(this, 0_layer, GetBlockOneLayerDown());
		//else
		//	return ElementListRef_impl<Hierarchy, std::type_identity_t, LayerType>(this, 0_layer, GetBlockOneLayerDown());
	}

private:

	ElementBlock* GetBlockOneLayerDown()
	{
		assert(this->GetMaxLayer() != -1);
		return std::launder(reinterpret_cast<ElementBlock*>(m_top_element + Hierarchy::GetElementSize(-1_layer)));
	}
	const ElementBlock* GetBlockOneLayerDown() const
	{
		assert(this->GetMaxLayer() != -1);
		return std::launder(reinterpret_cast<const ElementBlock*>(m_top_element + Hierarchy::GetElementSize(-1_layer)));
	}

	//グローバル要素を指すイテレータを作る。
	//ただしこれはTraverser内で全ての階層を統一的に扱うためのもの。
	//濫用してはいけないので、privateで定義しておく。
	detail::ElementIterator_impl<Hierarchy, std::add_const_t> GetTopIterator() const
	{
		return detail::ElementIterator_impl<Hierarchy, std::add_const_t>(this, -1, m_top_element);
	}
	detail::ElementIterator_impl<Hierarchy, std::type_identity_t> GetTopIterator()
	{
		return detail::ElementIterator_impl<Hierarchy, std::type_identity_t>(this, -1, m_top_element);
	}

	char* m_top_element = nullptr;
};

}

template <named_tuple ...LayerElements>
class STree : public detail::Tree_base<STree<LayerElements...>,
									   SHierarchy<STree<LayerElements...>, LayerElements...>,
									   detail::SElementBlockPolicy>
{
public:
	using Base = detail::Tree_base<STree<LayerElements...>,
								   SHierarchy<STree<LayerElements...>, LayerElements...>,
								   detail::SElementBlockPolicy>;
	using Base::Base;
};

class DTree : public detail::Tree_base<DTree, DHierarchy<DTree>, detail::DElementBlockPolicy>
{
public:
	using Base = detail::Tree_base<DTree, DHierarchy<DTree>, detail::DElementBlockPolicy>;
	using Base::Base;
};

template <named_tuple TopLayer, named_tuple Layer0>
class STable : public detail::Tree_base<STable<TopLayer, Layer0>,
										SHierarchy<STable<TopLayer, Layer0>, TopLayer, Layer0>,
										detail::SElementBlockPolicy>
{
public:
	using Base = detail::Tree_base<STable<TopLayer, Layer0>,
								   SHierarchy<STable<TopLayer, Layer0>, TopLayer, Layer0>,
								   detail::SElementBlockPolicy>;
	using Base::Base;
};

class DTable : public detail::Tree_base<DTable, FHierarchy<DTable, 0>, detail::DElementBlockPolicy>
{
public:
	using Base = detail::Tree_base<DTable, FHierarchy<DTable, 0>, detail::DElementBlockPolicy>;
	using Base::Base;
};

}

#endif
