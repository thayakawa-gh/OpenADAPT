#ifndef ADAPT_CONTAINER_ELEMENTBLOCK_H
#define ADAPT_CONTAINER_ELEMENTBLOCK_H

#include <OpenADAPT/Utility/Utility.h>
#include <OpenADAPT/Common/Definition.h>
#include <OpenADAPT/Container/Hierarchy.h>
#include <OpenADAPT/Container/ElementBlockPolicy.h>

namespace adapt
{

namespace detail
{

template <class Hierarchy>
struct HierarchyWrapper;
template <class Container, class ...LayerElements>
struct HierarchyWrapper<SHierarchy<Container, LayerElements...>>
{
	using Hierarchy = SHierarchy<Container, LayerElements...>;
	constexpr HierarchyWrapper() {};
	constexpr HierarchyWrapper(const Hierarchy*) {}
	static consteval Hierarchy value() { return Hierarchy{}; }
	constexpr bool operator==(const HierarchyWrapper&) const { return true; }
};
template <class Container>
struct HierarchyWrapper<DHierarchy<Container>>
{
	using Hierarchy = DHierarchy<Container>;
	HierarchyWrapper() {};
	HierarchyWrapper(const Hierarchy* h) : m_hierarchy(h) {}
	const Hierarchy& value() const { return *m_hierarchy; }
	bool operator==(const HierarchyWrapper& h) const { return m_hierarchy == h.m_hierarchy; }
	const Hierarchy* m_hierarchy = nullptr;
};

template <class Hierarchy, class Policy>
class ElementBlock_impl
{
public:

//#define CONSTEXPR_ASSERT static_assert(std::is_same_v<LayerSD, LayerType> || std::is_constant_evaluated());

	//Hierarchyはstaticかどうかをここで判断できるが、
	//Layerに関してはSTreeなどでもRttiPlaceholderを使うことがあるので、
	//各関数にテンプレートで持たせたほうが良い。
	using HierarchySD = HierarchyWrapper<Hierarchy>;

	ElementBlock_impl() = default;
	~ElementBlock_impl()
	{
		assert(m_blocks == nullptr);
	}

	ElementBlock_impl(const ElementBlock_impl&) = delete;
	ElementBlock_impl(ElementBlock_impl&& e) noexcept
	{
		operator=(std::move(e));
	}

	ElementBlock_impl& operator=(const ElementBlock_impl&) = delete;
	ElementBlock_impl& operator=(ElementBlock_impl&& e) noexcept
	{
		m_blocks = e.m_blocks; e.m_blocks = nullptr;
		m_end = e.m_end; e.m_end = nullptr;
		m_cap_end = e.m_cap_end; e.m_cap_end = nullptr;
		return *this;
	}

	template <class LayerSD>
	BindexType GetSize(HierarchySD h, LayerSD layer) const
	{
		return (BindexType)((m_end - m_blocks) / GetBlockSize(h, layer));
	}
	template <class LayerSD>
	BindexType GetCapacity(HierarchySD h, LayerSD layer) const
	{
		return (BindexType)((m_cap_end - m_blocks) / GetBlockSize(h, layer));
	}

	template <class LayerSD>
	char* GetBlock(HierarchySD h, LayerSD layer, BindexType index)
	{
		ptrdiff_t ptrpos = GetPtrDiff(h, layer, index);
		if ((size_t)index >= GetSize(h, layer) * std::get<2>(GetBlockTraits(h, layer))) return nullptr;
		return m_blocks + ptrpos;
	}
	template <class LayerSD>
	const char* GetBlock(HierarchySD h, LayerSD layer, BindexType index) const
	{
		ptrdiff_t ptrpos = GetPtrDiff(h, layer, index);
		if ((size_t)index >= GetSize(h, layer)) return nullptr;
		return m_blocks + ptrpos;
	}
	template <class LayerSD>
	char* GetBlock_unsafe(HierarchySD h, LayerSD layer, BindexType index)
	{
		ptrdiff_t ptrpos = GetPtrDiff(h, layer, index);
		assert((size_t)index < GetSize(h, layer) * std::get<2>(GetBlockTraits(h, layer)));
		return m_blocks + ptrpos;
	}
	template <class LayerSD>
	const char* GetBlock_unsafe(HierarchySD h, LayerSD layer, BindexType index) const
	{
		ptrdiff_t ptrpos = GetPtrDiff(h, layer, index);
		assert((size_t)index < GetSize(h, layer));
		return m_blocks + ptrpos;
	}

	char* Begin() const
	{
		return m_blocks;
	}
	const char* CBegin() const
	{
		return m_blocks;
	}
	char* End() const
	{
		return m_end;
	}
	const char* CEnd() const
	{
		return m_end;
	}

	//capacityの拡大。縮小はしない。
	template <class LayerSD>
	void Reserve(HierarchySD h, LayerSD layer, BindexType newcap)
	{
		if (newcap < GetCapacity(h, layer)) return;
		auto [ismaxlayer, elmsize, blocksize] = GetBlockTraits(h, layer);

		size_t size = GetSize(h, layer);
		ptrdiff_t end = blocksize * (ptrdiff_t)size;

		const auto& ms = GetPlaceholdersIn(h, layer);
		char* oldptr = m_blocks;
		m_blocks = (char*)std::malloc((ptrdiff_t)newcap * blocksize);
		for (ptrdiff_t diff = 0; diff != end; diff += blocksize)
		{
			char* from = oldptr + diff;
			char* to = m_blocks + diff;
			Policy::MoveAndDestroy(ms, from, to);
			//もし最下層でない場合、各ブロック末尾には下層ElementBlockオブジェクトが存在する。
			//これも移動させなければならない。
			if (!ismaxlayer) MoveAndDestroyElementBlock(from + elmsize, to + elmsize);
		}
		std::free(oldptr);
		m_end = m_blocks + size * blocksize;
		m_cap_end = m_blocks + (size_t)newcap * blocksize;
	}
	//拡大する場合はcapacityとsizeの変更。縮小の場合はsizeのみを変更。
	//こちらは確保した分のコンストラクタも呼ぶし、縮小分はデストラクタを呼ぶ。
	template <class LayerSD>
	void Resize(HierarchySD h, LayerSD layer, BindexType newsize)
	{
		BindexType oldsize = GetSize(h, layer);
		const auto& phs = GetPlaceholdersIn(h, layer);
		if (newsize > oldsize)
		{
			//新たにコンストラクタを呼ばなければならない場合。
			Reserve(h, layer, newsize);//まず領域を確保する。不要な場合は即時returnされるので無条件に呼べば良い。
			auto [ismaxlayer, elmsize, blocksize] = GetBlockTraits(h, layer);
			BindexType newcap = GetCapacity(h, layer);
			for (ptrdiff_t diff = oldsize; diff < newsize; ++diff)
			{
				char* ptr = m_blocks + diff;
				Create_default<Hierarchy>(phs, ptr);
			}
			m_end = m_blocks + newsize * blocksize;
			m_cap_end = m_blocks + newcap * blocksize;
		}
		else if (newsize < oldsize)
		{
			//BindexType popsize = oldsize - newsize;
			auto [ismaxlayer, elmsize, blocksize] = GetBlockTraits(h, layer);
			auto& ms = h.value().GetFieldInfosIn(layer);
			for (BindexType i = newsize; i < oldsize; ++i)
			{
				auto* ptr = m_blocks + i * blocksize;
				Destroy(ms, ptr);
				if (!ismaxlayer) DestroyElementBlock(h, layer, ptr + elmsize);
			}
			m_end = m_blocks + newsize * blocksize;
		}
		else return;
	}
	//capacityがsizeに一致するよう縮小する。
	template <class LayerSD>
	void Shrink(HierarchySD /*h*/, LayerSD /*layer*/)
	{
		// TODO: ElementBlock_impl::Shrinkの実装
		//もし要素数が0になるのなら、mContainer、mEnd、mCapacityは全てnullptrにすること。
		//Empty状態と区別するため。
		throw Forbidden("");
	}
private:
	template <bool UnsafeFlag, class LayerSD, class ...Args>
	void Push_impl(HierarchySD h, LayerSD layer, Args&& ...args)
	{
		[[maybe_unused]] constexpr auto get_size = [](const auto& arr)
		{
			using Type = std::remove_cvref_t<decltype(arr)>;
			if constexpr (same_as_xt<Type, std::vector>)
				return arr.size();
			else if constexpr (same_as_xt<Type, std::tuple>)
				return std::tuple_size_v<Type>;
		};
		assert(GetSize(h, layer) < std::numeric_limits<BindexType>::max());
		assert(get_size(GetPlaceholdersIn(h, layer)) == sizeof...(Args));
		//もしキャパシティが不足する場合は再確保する。
		BindexType size = GetSize(h, layer);
		if (size == GetCapacity(h, layer)) Reserve(h, layer, size * 2 + 1);
		auto [ismaxlayer, elmsize, blocksize] = GetBlockTraits(h, layer);
		const auto& ms = GetPlaceholdersIn(h, layer);
		char* pos = m_blocks + size * blocksize;
		if constexpr (UnsafeFlag) Create_static(ms, pos, std::forward<Args>(args)...);
		else Create(ms, pos, std::forward<Args>(args)...);
		if (!ismaxlayer) CreateElementBlock(pos + elmsize);
		m_end += blocksize;
	}
public:
	template <class LayerSD, class ...Args>
	void Push(HierarchySD h, LayerSD layer, Args&& ...args)
	{
		return Push_impl<false>(h, layer, std::forward<Args>(args)...);
	}
	template <class LayerSD, class ...Args>
	void Push_unsafe(HierarchySD h, LayerSD layer, Args&& ...args)
	{
		return Push_impl<true>(h, layer, std::forward<Args>(args)...);
	}
	//必要であれば空の領域を確保し、各フィールドをデフォルトコンストラクタから構築する。
	//下層のElementBlockも用意される。
	template <class LayerSD>
	void PushDefaultBlock(HierarchySD h, LayerSD layer)
	{
		//assert(!IsEmptyBlock());
		assert(GetSize(h, layer) < std::numeric_limits<BindexType>::max());
		BindexType size = GetSize(h, layer);
		if (size == GetCapacity(h, layer)) Reserve(h, layer, size * 2 + 1);
		auto [ismaxlayer, elmsize, blocksize] = GetBlockTraits(h, layer);
		const auto& ms = GetPlaceholdersIn(h, layer);
		char* pos = m_blocks + size * blocksize;
		Policy::Create_default(ms, pos);
		if (!ismaxlayer) CreateElementBlock(pos + elmsize);
		m_end += blocksize;
	}
	template <class LayerSD>
	void Pop(HierarchySD h, LayerSD layer)
	{
		assert(GetSize(h, layer) > 0);
		auto [ismaxlayer, elmsize, blocksize] = GetBlockTraits(h, layer);
		const auto& ms = GetPlaceholdersIn(h, layer);
		auto* ptr = m_end - blocksize;
		Destroy(ms, ptr);
		if (!ismaxlayer) DestroyElementBlock(h, layer, ptr + elmsize);
		m_end -= blocksize;
	}
	//index~index+size-1番目の要素の削除。
	template <class LayerSD>
	void Erase(HierarchySD h, LayerSD layer, BindexType index, BindexType size)
	{
		//assert(!IsEmptyBlock());
		auto [ismaxlayer, elmsize, blocksize] = GetBlockTraits(h, layer);
		const auto& ms = GetPlaceholdersIn(h, layer);

		//BindexType oldsize = GetSize(h, layer);
		char* from = m_blocks + (index + size) * blocksize;
		char* to = m_blocks + index * blocksize;
		for (; from != m_end; from += blocksize, to += blocksize)
		{
			Move(ms, from, to);
			//もし最下層でない場合、各ブロック末尾には下層ElementBlockオブジェクトが存在する。
			//これも移動させなければならない。
			if (!ismaxlayer) MoveElementBlock(from + elmsize, to + elmsize);
		}
		for (; to != m_end; to += blocksize)
		{
			Destroy(ms, to);
			if (!ismaxlayer) DestroyElementBlock(h, layer + 1, to + elmsize);
		}
		m_end -= size * blocksize;
	}
	//デストラクタ相当の働きをする。
	//デストラクタ自体は引数を取れないので、デストラクタを呼ぶ前にこちらで全削除する。
	template <class LayerSD>
	void Destruct(HierarchySD h, LayerSD layer)
	{
		//assert(!IsEmptyBlock());
		auto [ismaxlayer, elmsize, blocksize] = GetBlockTraits(h, layer);
		ptrdiff_t end = blocksize * (ptrdiff_t)GetSize(h, layer);
		const auto& ms = GetPlaceholdersIn(h, layer);
		for (ptrdiff_t diff = 0; diff != end; diff += blocksize)
		{
			char* ptr = m_blocks + diff;
			Destroy(ms, ptr);
			if (!ismaxlayer) DestroyElementBlock(h, layer, ptr + elmsize);
		}
		std::free(m_blocks);
		m_blocks = nullptr;
	}
	//from_の全要素を自身に移動し、from_の全フィールドに対してデストラクタを呼ぶ。
	//下層要素も移動する。
	template <class LayerSD>
	void MoveElementsFrom(HierarchySD h, LayerSD layer, ElementBlock_impl&& from_)
	{
		auto [ismaxlayer, elmsize, blocksize] = GetBlockTraits(h, layer);
		BindexType size = from_.GetSize(h, layer);
		const auto& phs = GetPlaceholdersIn(h, layer);
		Reserve(h, layer, size);
		char* from = from_.m_blocks;
		char* to = m_blocks;
		for (; from != from_.m_end; from += blocksize, to += blocksize)
		{
			Policy::MoveAndDestroy(phs, from, to);
			if (!ismaxlayer) MoveAndDestroyElementBlock(from + elmsize, to + elmsize);
		}
		m_end = to;
		from_.m_end = from_.m_blocks;
	}


	//----------static methods----------

	template <class Placeholders>
	static void Destroy(const Placeholders& phs, char* ptr)
	{
		Policy::Destroy(phs, ptr);
	}

	template <class Placeholders, class ...Args>
	static void Create(const Placeholders& phs, char* ptr, Args&& ...args)
	{
		Policy::Create(phs, ptr, std::forward<Args>(args)...);
	}
	template <class Placeholders, class ...Args>
	static void Create_static(const Placeholders& phs, char* ptr, Args&& ...args)
	{
		Policy::Create_static(phs, ptr, std::forward<Args>(args)...);
	}
	template <class Placeholders, class ...Args>
	static void Create_default(const Placeholders& phs, char* ptr)
	{
		Policy::Create_default(phs, ptr);
	}

	static void CreateElementBlock(char* ptr)
	{
		new (ptr) ElementBlock_impl();
	}
	//fromのElementBlockをtoへと移動する。fromの中の各要素は初期状態となる。
	//toは初期化済み、コンストラクタが全要素全フィールドに対して呼ばれていると想定している。
	static void MoveElementBlock(char* from, char* to)
	{
		ElementBlock_impl* e_from = std::launder(reinterpret_cast<ElementBlock_impl*>(from));
		ElementBlock_impl* e_to = std::launder(reinterpret_cast<ElementBlock_impl*>(to));
		*e_to = std::move(*e_from);
	}
	template <s_hierarchy Hier, LayerType L>
	static void DestroyElementBlock(HierarchyWrapper<Hier> h, LayerConstant<L>, char* ptr)
	{
		//ptrの先にElementBlockがあるものとして、それを削除する。
		//maxlayerのとき、DestroyElementBlockをそのままSHierarchyに対して呼ぶとコンパイルエラーになる。
		//GetElementSizeをmaxlayerよりも下層に対して呼んでしまい、SHierarchy::ElementSizesが範囲外になるため。
		if constexpr (Hier::GetMaxLayer() == L) return;
		else
		{
			auto* e_ptr = std::launder(reinterpret_cast<ElementBlock_impl*>(ptr));
			e_ptr->Destruct(h, LayerConstant<LayerType(L + 1)>{});
			e_ptr->~ElementBlock_impl();
		}
	}
	static void DestroyElementBlock(HierarchySD h, LayerType layer, char* ptr)
	{
		//ptrの先にElementBlockがあるものとして、それを削除する。
		auto* e_ptr = std::launder(reinterpret_cast<ElementBlock_impl*>(ptr));
		e_ptr->Destruct(h, (LayerType)(layer + 1));
		e_ptr->~ElementBlock_impl();
	}

	//fromのElementBlockをtoへと移動し、fromに対してデストラクタを呼ぶ。
	//toはメモリを確保しただけの未初期化領域と想定している。
	static void MoveAndDestroyElementBlock(char* from, char* to)
	{
		ElementBlock_impl* e_from = std::launder(reinterpret_cast<ElementBlock_impl*>(from));
		new (to) ElementBlock_impl(std::move(*e_from));
		e_from->~ElementBlock_impl();
	}


	template <s_hierarchy Hier, LayerType L>
	static ptrdiff_t GetPtrDiff(HierarchyWrapper<Hier>, LayerConstant<L>, BindexType index)
	{
		constexpr ptrdiff_t blocksize = GetBlockSize(HierarchyWrapper<Hier>{}, LayerConstant<L>{});
		return (ptrdiff_t)index * blocksize;
	}
	static ptrdiff_t GetPtrDiff(HierarchySD h, LayerType layer, BindexType index)
	{
		ptrdiff_t blocksize = GetBlockSize(h, layer);
		return (ptrdiff_t)index * blocksize;
	}

	template <s_hierarchy Hier, LayerType L>
	static consteval std::tuple<bool, size_t, size_t> GetBlockTraits(HierarchyWrapper<Hier>, LayerConstant<L>)
	{
		constexpr bool ismaxlayer = L == Hier::GetMaxLayer();
		constexpr size_t elmsize = Hier::GetElementSize(LayerConstant<L>{});
		constexpr size_t blocksize = elmsize + (!ismaxlayer) * sizeof(ElementBlock_impl);
		return std::make_tuple(ismaxlayer, elmsize, blocksize);
	}
	static constexpr std::tuple<bool, size_t, size_t> GetBlockTraits(HierarchySD h, LayerType layer)
	{
		bool ismaxlayer = layer == h.value().GetMaxLayer();
		size_t elmsize = h.value().GetElementSize(layer);
		size_t blocksize = elmsize + (!ismaxlayer) * sizeof(ElementBlock_impl);
		return std::make_tuple(ismaxlayer, elmsize, blocksize);
	}

	template <s_hierarchy Hier, LayerType L>
	static consteval bool IsMaxLayer(HierarchyWrapper<Hier>, LayerConstant<L>)
	{
		return IsMaxLayer(HierarchySD{}, L);
	}
	static constexpr bool IsMaxLayer(HierarchySD h, LayerType layer)
	{
		if constexpr (s_hierarchy<Hierarchy>) return Hierarchy::GetMaxLayer() == layer;
		else return h.value().GetMaxLayer() == layer;
	}

	template <s_hierarchy Hier, LayerType L>
	static consteval size_t GetBlockSize(HierarchyWrapper<Hier>, LayerConstant<L>)
	{
		return GetBlockSize(HierarchyWrapper<Hier>{}, L);
		//return Hier::GetElementSize(LayerConstant<L>{}) + (L != Hier::GetMaxLayer()) * sizeof(ElementBlock_impl);
	}
	static constexpr size_t GetBlockSize(HierarchySD h, LayerType layer)
	{
		//if constexpr (s_hierarchy<Hierarchy>)
		//	return GetElementSize(h, layer) + (!IsMaxLayer(h, layer)) * sizeof(ElementBlock_impl);
		//else
			return GetElementSize(h, layer) + (!IsMaxLayer(h, layer)) * sizeof(ElementBlock_impl);
	}

	template <s_hierarchy Hier, LayerType L>
	static consteval size_t GetElementSize(HierarchyWrapper<Hier>, LayerConstant<L>)
	{
		//return GetBlockSize(h, L);
		return GetElementSize(HierarchyWrapper<Hier>{}, L);
	}
	static constexpr size_t GetElementSize(HierarchySD h, LayerType layer)
	{
		if constexpr (s_hierarchy<Hierarchy>) return Hierarchy::GetElementSize(layer);
		else return h.value().GetElementSize(layer);
	}

	template <s_hierarchy Hier, LayerType L>
	static consteval decltype(auto) GetPlaceholdersIn(HierarchyWrapper<Hier>, LayerConstant<L>)
	{
		return Hier::GetPlaceholdersIn(LayerConstant<L>{});
	}
	static constexpr decltype(auto) GetPlaceholdersIn(HierarchySD h, LayerType layer)
	{
		if constexpr (s_hierarchy<Hierarchy>) return Hierarchy::GetPlaceholdersIn(layer);
		else return h.value().GetPlaceholdersIn(layer);
	}

private:

	//----------rtti methods----------


private:
	char* m_blocks = nullptr;
	char* m_end = nullptr;
	char* m_cap_end = nullptr;
};

template <class Container>
using SElementBlock = ElementBlock_impl<Container, SElementBlockPolicy>;
template <class Container>
using DElementBlock = ElementBlock_impl<Container, DElementBlockPolicy>;

}

}

#endif
