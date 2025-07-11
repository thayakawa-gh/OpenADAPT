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
	//null初期化したい状況があるのでポインタで受け取る。
	constexpr HierarchyWrapper(const Hierarchy*) {}
	static consteval Hierarchy value() { return Hierarchy{}; }
	constexpr bool operator==(const HierarchyWrapper&) const { return true; }
};
template <class Container>
struct HierarchyWrapper<DHierarchy<Container>>
{
	using Hierarchy = DHierarchy<Container>;
	constexpr HierarchyWrapper() {};
	//null初期化したい状況があるのでポインタで受け取る。
	constexpr HierarchyWrapper(const Hierarchy* h) : m_hierarchy(h) {}
	constexpr const Hierarchy& value() const { return *m_hierarchy; }
	constexpr bool operator==(const HierarchyWrapper& h) const { return m_hierarchy == h.m_hierarchy; }
	const Hierarchy* m_hierarchy = nullptr;
};
template <class Container, LayerType MaxLayer>
struct HierarchyWrapper<FHierarchy<Container, MaxLayer>>
{
	using Hierarchy = FHierarchy<Container, MaxLayer>;
	constexpr HierarchyWrapper() {};
	//null初期化したい状況があるのでポインタで受け取る。
	constexpr HierarchyWrapper(const Hierarchy* h) : m_hierarchy(h) {}
	constexpr const Hierarchy& value() const { return *m_hierarchy; }
	constexpr bool operator==(const HierarchyWrapper& h) const { return m_hierarchy == h.m_hierarchy; }
	const Hierarchy* m_hierarchy = nullptr;
};

template <class Hierarchy, class Policy_>
class ElementBlock_impl
{
public:

	//#define CONSTEXPR_ASSERT static_assert(std::is_same_v<LayerSD, LayerType> || std::is_constant_evaluated());

		//Hierarchyはstaticかどうかをここで判断できるが、
		//Layerに関してはSTreeなどでもRttiPlaceholderを使うことがあるので、
		//各関数にテンプレートで持たせたほうが良い。
	using HierarchySD = HierarchyWrapper<Hierarchy>;
	using Policy = Policy_;

	/*
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
	}*/

	void Swap(ElementBlock_impl& e) noexcept
	{
		std::swap(m_blocks, e.m_blocks);
		std::swap(m_end, e.m_end);
		std::swap(m_cap_end, e.m_cap_end);
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
		if ((size_t)index >= GetSize(h, layer)) return nullptr;
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
		assert((size_t)index < GetSize(h, layer));
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
		assert(newcap >= GetSize(h, layer));
		//if (newcap < GetCapacity(h, layer)) return;
		auto [ismaxlayer, elmsize, blocksize] = GetBlockTraits(h, layer);

		size_t size = GetSize(h, layer);
		size_t end = blocksize * size;

		const auto& ms = GetPlaceholdersIn(h, layer);
		char* oldptr = m_blocks;
		//もしm_blocksがnullptrであった場合、oldptrもnullptrとなるが、
		//そのときはend==0となっているのでループに入ることはなく、動作に問題は起きない。
		m_blocks = (char*)std::malloc((size_t)newcap * blocksize);


		auto is_totally_trivial = h.value().IsTotallyTrivial(layer);
		if (is_totally_trivial)
		{
			std::memcpy(m_blocks, oldptr, end);
		}
		else
		{
			for (size_t diff = 0; diff != end; diff += blocksize)
			{
				char* from = oldptr + diff;
				char* to = m_blocks + diff;
				Policy::MoveConstructAndDestroy(ms, is_totally_trivial, from, to);
				//もし最下層でない場合、各ブロック末尾には下層ElementBlockオブジェクトが存在する。
				//これも移動させなければならない。
				if (!ismaxlayer) MoveConstructElementBlock(from + elmsize, to + elmsize);
			}
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
		auto is_totally_trivial = h.value().IsTotallyTrivial(layer);
		if (newsize > oldsize)
		{
			//新たにコンストラクタを呼ばなければならない場合。
			Reserve(h, layer, newsize);//まず領域を確保する。不要な場合は即時returnされるので無条件に呼べば良い。
			auto [ismaxlayer, elmsize, blocksize] = GetBlockTraits(h, layer);
			BindexType newcap = GetCapacity(h, layer);
			//Reserveでoldsize-1までは既に移動されているので、
			//oldsizeからnewsize-1までの要素を新たにコンストラクタで構築する。
			auto* ptr = m_blocks + oldsize * blocksize;
			const auto* end = m_blocks + newsize * blocksize;
			for (; ptr != end; ptr += blocksize)
			{
				Policy::Create_default(phs, ptr);
				if (!ismaxlayer) CreateElementBlock(ptr + elmsize);
			}
			m_end = m_blocks + newsize * blocksize;
			m_cap_end = m_blocks + newcap * blocksize;
		}
		else if (newsize < oldsize)
		{
			auto [ismaxlayer, elmsize, blocksize] = GetBlockTraits(h, layer);
			//こちらはサイズを縮小するが、capacityは変更しない。
			//よって、newsizeからoldsize-1までの要素をデストラクタで破棄する。
			auto* ptr = m_blocks + newsize * blocksize;
			const auto* end = m_blocks + oldsize * blocksize;
			for (; ptr != end; ptr += blocksize)
			{
				Policy::Destroy(phs, is_totally_trivial, ptr);
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

	template <class LayerSD, class ...Args>
	void Push(HierarchySD h, LayerSD layer, Args&& ...args)
	{
		//return Push_impl<false>(h, layer, std::forward<Args>(args)...);
		return Insert(h, layer, GetSize(h, layer), std::forward<Args>(args)...);
	}
	template <class LayerSD, class ...Args>
	void Push_unsafe(HierarchySD h, LayerSD layer, Args&& ...args)
	{
		//return Push_impl<true>(h, layer, std::forward<Args>(args)...);
		return Insert_unsafe(h, layer, GetSize(h, layer), std::forward<Args>(args)...);
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
	void PseudoPush(HierarchySD h, LayerSD layer)
	{
		BindexType size = GetSize(h, layer);
		if (size == GetCapacity(h, layer)) Reserve(h, layer, size * 2 + 1);
		auto [ismaxlayer, elmsize, blocksize] = GetBlockTraits(h, layer);
		char* pos = m_blocks + size * blocksize + elmsize;
		if (!ismaxlayer) CreateElementBlock(pos);
		m_end += blocksize;
	}
	template <class LayerSD>
	void Pop(HierarchySD h, LayerSD layer)
	{
		assert(GetSize(h, layer) > 0);
		auto [ismaxlayer, elmsize, blocksize] = GetBlockTraits(h, layer);
		const auto& ms = GetPlaceholdersIn(h, layer);
		auto is_totally_trivial = h.value().IsTotallyTrivial(layer);
		auto* ptr = m_end - blocksize;
		Policy::Destroy(ms, is_totally_trivial, ptr);
		if (!ismaxlayer) DestroyElementBlock(h, layer, ptr + elmsize);
		m_end -= blocksize;
	}

private:
	template <bool UnsafeFlag, class LayerSD, class ...Args>
	void Insert_impl(HierarchySD h, LayerSD layer, BindexType index, Args&& ...args)
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
		assert(index <= GetSize(h, layer));
		//もしキャパシティが不足する場合は再確保する。
		BindexType size = GetSize(h, layer);
		if (size == GetCapacity(h, layer)) Reserve(h, layer, size * 2 + 1);
		auto [ismaxlayer, elmsize, blocksize] = GetBlockTraits(h, layer);
		const auto& ms = GetPlaceholdersIn(h, layer);
		auto is_totally_trivial = h.value().IsTotallyTrivial(layer);
		if (index != size)
		{
			//index != sizeの場合、末尾挿入ではないので、
			//index~size-1番目の要素を後ろにずらす。
			//注意しなければならないのは、最後の要素だけは未初期化領域へ移動するため、
			//Policy::MoveではなくMoveConstructを呼ばなければならない点。
			char* from = m_blocks + size * blocksize - blocksize;//size == 0の場合に非負整数型の計算が狂うので、(size - 1)にしてはいけない。
			char* to = m_blocks + size * blocksize;
			const char* end = m_blocks + index * blocksize - blocksize;//index - 1番目。(index - 1)にするなよ。
			Policy::MoveConstruct(ms, is_totally_trivial, from, to);
			if (!ismaxlayer) MoveElementBlock(from + elmsize, to + elmsize);
			from -= blocksize;
			to -= blocksize;
			for (; from != end; from -= blocksize, to -= blocksize)
			{
				Policy::Move(ms, is_totally_trivial, from, to);
				//もし最下層でない場合、各ブロック末尾には下層ElementBlockオブジェクトが存在する。
				//これも移動させなければならない。
				if (!ismaxlayer) MoveElementBlock(from + elmsize, to + elmsize);
			}
			//ここで末尾挿入でない場合、挿入するメモリ領域は初期化済みなので、
			//コンストラクタなしのAssign関数を呼ぶ。
			//また下層のElementBlockも初期化済みなので、何もしなくて良い。
			char* pos = m_blocks + index * blocksize;
			if constexpr (UnsafeFlag) Policy::Assign_static(ms, pos, std::forward<Args>(args)...);
			else Policy::Assign(ms, pos, std::forward<Args>(args)...);
		}
		else
		{
			char* pos = m_blocks + index * blocksize;
			//ここで末尾挿入の場合、挿入するメモリ領域は未初期化であるため、Create関数でよい。
			if constexpr (UnsafeFlag) Policy::Create_static(ms, pos, std::forward<Args>(args)...);
			else Policy::Create(ms, pos, std::forward<Args>(args)...);
			if (!ismaxlayer) CreateElementBlock(pos + elmsize);
		}
		m_end += blocksize;
	}
public:
	//indexの位置に要素を挿入する。
	template <class LayerSD, class ...Args>
	void Insert(HierarchySD h, LayerSD layer, BindexType index, Args&& ...args)
	{
		return Insert_impl<false>(h, layer, index, std::forward<Args>(args)...);
	}
	template <class LayerSD, class ...Args>
	void Insert_unsafe(HierarchySD h, LayerSD layer, BindexType index, Args&& ...args)
	{
		return Insert_impl<true>(h, layer, index, std::forward<Args>(args)...);
	}

private:
	template <class LayerSD, class CharPtr, bool MoveFlag, bool EraseFlag>
		requires(std::same_as<CharPtr, char*> || std::same_as<CharPtr, const char*>)
	void Insert_impl(HierarchySD h, LayerSD layer, BindexType index, CharPtr srcfrom, CharPtr srcend, std::bool_constant<MoveFlag>, std::bool_constant<EraseFlag>)
	{
		static_assert(!(!MoveFlag && EraseFlag));// moveしないのにeraseするのはおかしい。
		assert(GetSize(h, layer) < std::numeric_limits<BindexType>::max());
		assert(index <= GetSize(h, layer));
		//もしキャパシティが不足する場合は再確保する。
		BindexType size = GetSize(h, layer);
		BindexType inssize = (BindexType)((srcend - srcfrom) / GetBlockSize(h, layer));

		if (size + inssize > GetCapacity(h, layer)) Reserve(h, layer, size + inssize);
		auto [ismaxlayer, elmsize, blocksize] = GetBlockTraits(h, layer);
		const auto& ms = GetPlaceholdersIn(h, layer);
		auto is_totally_trivial = h.value().IsTotallyTrivial(layer);

		{
			//index~size-1番目の要素を後ろにずらす。
			//このとき、size - inssize ~ size - 1番目の要素は未初期化領域へ移動するため、
			//Policy::MoveではなくMoveConstructを呼ばなければならない。
			char* from = m_blocks + size * blocksize - blocksize;//(size - 1)にしてはいけない。size == 0のときにバグる。
			char* to = m_blocks + (size + inssize) * blocksize - blocksize;

			const char* end1 = nullptr;
			//1. index + inssize >= size: 挿入の結果、挿入される要素はもともとの初期化済み領域よりも外に出る。
			//したがって、index ~ size - 1の要素はすべて未初期化領域への移動となる。
			if (index + inssize >= size) end1 = m_blocks + index * blocksize - blocksize;
			//2. index + inssize < size: 初期化済み領域への移動が発生する。
			else end1 = m_blocks + (size - inssize - 1) * blocksize;//size - inssize - 1番目。未初期化領域への移動範囲。
			const char* end2 = m_blocks + index * blocksize - blocksize;//index - 1番目。初期化済み領域への移動範囲。

			for (; from != end1; from -= blocksize, to -= blocksize)
			{
				Policy::MoveConstruct(ms, is_totally_trivial, from, to);
				if (!ismaxlayer) MoveConstructElementBlock(from + elmsize, to + elmsize);
			}
			for (; from != end2; from -= blocksize, to -= blocksize)
			{
				Policy::Move(ms, is_totally_trivial, from, to);
				//もし最下層でない場合、各ブロック末尾には下層ElementBlockオブジェクトが存在する。
				//これも移動させなければならない。
				if (!ismaxlayer) MoveElementBlock(from + elmsize, to + elmsize);
			}
		}
		{
			//srcからの挿入を行う。
			//こちらも上と同様に、index ~ size - 1はMoveConstructによって初期化された領域なのでCopy/Moveでよい。
			//size ~ index + inssize - 1は未初期化領域なので、CopyConstruct/MoveConstructを呼ぶ。
			CharPtr from = srcfrom;
			char* to = m_blocks + index * blocksize;
			char* end1 = m_blocks + size * blocksize;
			//もしindex + inssize < sizeの場合、挿入する要素はすべて初期化済み領域内に収まる。
			if (index + inssize < size) end1 = m_blocks + (index + inssize) * blocksize;
			char* end2 = m_blocks + (index + inssize) * blocksize;
			for (; to != end1; from += blocksize, to += blocksize)
			{
				if constexpr (MoveFlag)
				{
					if constexpr (EraseFlag) Policy::MoveAndDestroy(ms, is_totally_trivial, from, to);
					else Policy::Move(ms, is_totally_trivial, from, to);
					if (!ismaxlayer) MoveElementBlock(from + elmsize, to + elmsize);
				}
				else
				{
					Policy::Copy(ms, is_totally_trivial, from, to);
					if (!ismaxlayer) CopyConstructElementBlock(h, layer + 1_layer, from + elmsize, to + elmsize);
				}
			}
			for (; to != end2; from += blocksize, to += blocksize)
			{
				if constexpr (MoveFlag)
				{
					if constexpr (EraseFlag) Policy::MoveConstructAndDestroy(ms, is_totally_trivial, from, to);
					else Policy::MoveConstruct(ms, is_totally_trivial, from, to);
					if (!ismaxlayer) MoveConstructElementBlock(from + elmsize, to + elmsize);
				}
				else
				{
					Policy::CopyConstruct(ms, is_totally_trivial, from, to);
					if (!ismaxlayer) CopyConstructElementBlock(h, layer + 1_layer, from + elmsize, to + elmsize);
				}
			}
		}
		m_end += blocksize * inssize;
	}
public:
	//from-endまでの要素をindexにコピー/ムーブ挿入する。
	//このとき、from-endの要素は全く異なるtree/tableであることを前提とし、
	//階層構造が完全一致することを要求する。
	//さらに、srcfrom ~ srcendまでの要素はデストラクタが呼ばれた未初期化状態となる。
	template <class LayerSD>
	void InsertBlock(HierarchySD h, LayerSD layer, BindexType index, const char* srcfrom, const char* srcend)
	{
		Insert_impl(h, layer, index, srcfrom, srcend, std::false_type{}, std::false_type{});
	}
	template <class LayerSD>
	void MoveInsertBlock(HierarchySD h, LayerSD layer, BindexType index, char* srcfrom, char* srcend)
	{
		Insert_impl(h, layer, index, srcfrom, srcend, std::true_type{}, std::false_type{});
	}
	template <class LayerSD>
	void MoveAndEraseInsertBlock(HierarchySD h, LayerSD layer, BindexType index, char* srcfrom, char* srcend)
	{
		Insert_impl(h, layer, index, srcfrom, srcend, std::true_type{}, std::true_type{});
	}

	//index~index+size-1番目の要素の削除。
	template <class LayerSD>
	void Erase(HierarchySD h, LayerSD layer, BindexType index, BindexType size)
	{
		//assert(!IsEmptyBlock());
		auto [ismaxlayer, elmsize, blocksize] = GetBlockTraits(h, layer);
		const auto& ms = GetPlaceholdersIn(h, layer);
		auto is_totally_trivial = h.value().IsTotallyTrivial(layer);

		//BindexType oldsize = GetSize(h, layer);
		char* from = m_blocks + (index + size) * blocksize;
		char* to = m_blocks + index * blocksize;
		for (; from != m_end; from += blocksize, to += blocksize)
		{
			Policy::Move(ms, is_totally_trivial, from, to);
			//もし最下層でない場合、各ブロック末尾には下層ElementBlockオブジェクトが存在する。
			//これも移動させなければならない。
			if (!ismaxlayer) MoveElementBlock(from + elmsize, to + elmsize);
		}
		for (; to != m_end; to += blocksize)
		{
			Policy::Destroy(ms, is_totally_trivial, to);
			if (!ismaxlayer) DestroyElementBlock(h, layer, to + elmsize);
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
		auto is_totally_trivial = h.value().IsTotallyTrivial(layer);
		const auto& ms = GetPlaceholdersIn(h, layer);
		for (ptrdiff_t diff = 0; diff != end; diff += blocksize)
		{
			char* ptr = m_blocks + diff;
			Policy::Destroy(ms, is_totally_trivial, ptr);
			if (!ismaxlayer) DestroyElementBlock(h, layer, ptr + elmsize);
		}
		std::free(m_blocks);
		Init();
	}
	void Init()
	{
		m_blocks = nullptr;
		m_end = nullptr;
		m_cap_end = nullptr;
	}
	//自身はメモリ確保されただけの未初期化状態と想定し、from_の全要素を自身に移動する。下層要素も移動する。
	//fromのフィールドはデフォルト初期化された状態に、下層はnullptrになる。
	template <class LayerSD>
	void CreateElementsFrom(HierarchySD h, LayerSD layer, ElementBlock_impl&& from_)
	{
		assert(GetCapacity(h, layer) == 0);
		auto [ismaxlayer, elmsize, blocksize] = GetBlockTraits(h, layer);
		BindexType size = from_.GetSize(h, layer);
		const auto& phs = GetPlaceholdersIn(h, layer);
		auto is_totally_trivial = h.value().IsTotallyTrivial(layer);
		Reserve(h, layer, size);
		char* from = from_.m_blocks;
		char* to = m_blocks;
		if (is_totally_trivial)
		{
			std::memcpy(to, from, (size_t)size * blocksize);
			m_end = to + size * blocksize;
		}
		else
		{
			for (; from != from_.m_end; from += blocksize, to += blocksize)
			{
				Policy::MoveConstruct(phs, is_totally_trivial, from, to);
				if (!ismaxlayer) MoveConstructElementBlock(from + elmsize, to + elmsize);
			}
			//こちらはループによってtoが末尾要素をさしているので、toを代入すればよい。
			m_end = to;
		}
		from_.m_end = from_.m_blocks;
	}
	template <class LayerSD>
	void RewriteSize(HierarchySD h, LayerSD layer, BindexType size)
	{
		assert(GetCapacity(h, layer) >= size);
		m_end = m_blocks + size * GetBlockSize(h, layer);
	}

	//----------static methods----------
	/*
	template <class Placeholders, class Bool>
	static void Destroy(const Placeholders& phs, Bool b, char* ptr)
	{
		Policy::Destroy(phs, b, ptr);
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
	}*/

	static void CreateElementBlock(char* ptr)
	{
		new (ptr) ElementBlock_impl{};
	}
	//fromのElementBlockをtoへとコピーする。
	//toはメモリ確保されただけの未初期化状態と想定している。
	template <class LayerSD>
	static void CopyConstructFields(HierarchySD h, LayerSD layer, const char* from, char* to)
	{
		//auto [ismaxlayer, elmsize, blocksize] = GetBlockTraits(h, layer);
		const auto& phs = GetPlaceholdersIn(h, layer);
		auto is_totally_trivial = h.value().IsTotallyTrivial(layer);
		Policy::CopyConstruct(phs, is_totally_trivial, from, to);
	}
	//fromのElementBlockをtoへとコピーする。
	//toは初期化済み、コンストラクタが全要素全フィールドに対して呼ばれていると想定している。
	template <class LayerSD>
	static void CopyFields(HierarchySD h, LayerSD layer, const char* from, char* to)
	{
		//auto [ismaxlayer, elmsize, blocksize] = GetBlockTraits(h, layer);
		const auto& phs = GetPlaceholdersIn(h, layer);
		auto is_totally_trivial = h.value().IsTotallyTrivial(layer);
		Policy::Copy(phs, is_totally_trivial, from, to);
	}

	//from_からthisへと全要素をコピーする。下層要素も再帰的に。
	template <class LayerSD>
	static void CopyElementBlock(HierarchySD h, LayerSD layer, const char* from, char* to)
	{
		const ElementBlock_impl* e_from = std::launder(reinterpret_cast<ElementBlock_impl*>(from));
		ElementBlock_impl* e_to = std::launder(reinterpret_cast<ElementBlock_impl*>(to));
		e_to->Insert(h, layer, 0, e_from->m_blocks, e_from->m_end);
	}

	//fromのElementBlockをtoへと移動する。fromは全てnullptrの状態になる。
	//toはElementBlock_implとして初期化済みと想定している。
	//ElementBlock_implはtrivialなので、MoveConstructElementBlockと統合していい気がするが。
	static void MoveElementBlock(char* from, char* to)
	{
		ElementBlock_impl* e_from = std::launder(reinterpret_cast<ElementBlock_impl*>(from));
		ElementBlock_impl* e_to = std::launder(reinterpret_cast<ElementBlock_impl*>(to));
		*e_to = *e_from;
		e_from->Init();
	}
	template <class Hier, LayerType L>
		requires s_hierarchy<Hier> || f_hierarchy<Hier>
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
		}
	}
	static void DestroyElementBlock(HierarchySD h, LayerType layer, char* ptr)
	{
		//ptrの先にElementBlockがあるものとして、それを削除する。
		auto* e_ptr = std::launder(reinterpret_cast<ElementBlock_impl*>(ptr));
		e_ptr->Destruct(h, layer + 1_layer);
	}

	template <class Hier, LayerType L>
		requires s_hierarchy<Hier> || f_hierarchy<Hier>
	static void CopyConstructElementBlock(HierarchyWrapper<Hier> h, LayerConstant<L> layer, const char* from, char* to)
	{
		if constexpr (Hier::GetMaxLayer() < L) return;
		else
		{
			const ElementBlock_impl* e_from = std::launder(reinterpret_cast<const ElementBlock_impl*>(from));
			ElementBlock_impl* e_to = new (to) ElementBlock_impl();
			e_to->InsertBlock(h, layer, 0, e_from->m_blocks, e_from->m_end);
		}
	}
	static void CopyConstructElementBlock(HierarchySD h, LayerType layer, const char* from, char* to)
	{
		const ElementBlock_impl* e_from = std::launder(reinterpret_cast<const ElementBlock_impl*>(from));
		ElementBlock_impl* e_to = new (to) ElementBlock_impl();
		e_to->InsertBlock(h, layer, 0, e_from->m_blocks, e_from->m_end);
	}
	//fromのElementBlockをtoへと移動する。
	//toはメモリを確保しただけの未初期化領域と想定している。
	static void MoveConstructElementBlock(char* from, char* to)
	{
		ElementBlock_impl* e_from = std::launder(reinterpret_cast<ElementBlock_impl*>(from));
		new (to) ElementBlock_impl(*e_from);
		e_from->Init();
	}

	//メモリ確保されただけの未初期化領域toにfromの要素を移動し、fromのデストラクタを呼ぶ。
	template <class LayerSD>
	static void MoveConstructAndDestroyElementFrom(HierarchySD h, LayerSD layer, char* from, char* to)
	{
		auto [ismaxlayer, elmsize, blocksize] = GetBlockTraits(h, layer);
		const auto& phs = GetPlaceholdersIn(h, layer);
		auto is_totally_trivial = h.value().IsTotallyTrivial(layer);
		Policy::MoveConstructAndDestroy(phs, is_totally_trivial, from, to);
		if (!ismaxlayer) MoveConstructElementBlock(from + elmsize, to + elmsize);
	}


	template <class Hier, LayerType L>
		requires s_hierarchy<Hier>
	static ptrdiff_t GetPtrDiff(HierarchyWrapper<Hier>, LayerConstant<L>, BindexType index)
	{
		constexpr ptrdiff_t blocksize = GetBlockSize(HierarchyWrapper<Hier>{}, LayerConstant<L>{});
		return (ptrdiff_t)index * blocksize;
	}
	template <class Hier, LayerType L>
		requires f_hierarchy<Hier>
	static ptrdiff_t GetPtrDiff(HierarchyWrapper<Hier> h, LayerConstant<L>, BindexType index)
	{
		ptrdiff_t blocksize = GetBlockSize(h, LayerConstant<L>{});
		return (ptrdiff_t)index * blocksize;
	}
	static ptrdiff_t GetPtrDiff(HierarchySD h, LayerType layer, BindexType index)
	{
		ptrdiff_t blocksize = GetBlockSize(h, layer);
		return (ptrdiff_t)index * blocksize;
	}

	template <s_hierarchy Hier, LayerType L>
	static consteval auto GetBlockTraits(HierarchyWrapper<Hier>, LayerConstant<L>)
	{
		constexpr auto ismaxlayer = std::bool_constant<(L == Hier::GetMaxLayer())>{};
		constexpr auto elmsize = SizeConstant<Hier::GetElementSize(LayerConstant<L>{})>{};
		constexpr auto blocksize = SizeConstant<(elmsize + (!ismaxlayer) * sizeof(ElementBlock_impl))>{};
		return std::make_tuple(ismaxlayer, elmsize, blocksize);
	}
	template <f_hierarchy Hier, LayerType L>
	static constexpr auto GetBlockTraits(HierarchyWrapper<Hier> h, LayerConstant<L>)
	{
		constexpr auto ismaxlayer = std::bool_constant<(L == Hier::GetMaxLayer())>{};
		size_t elmsize = h.value().GetElementSize(LayerConstant<L>{});
		size_t blocksize = elmsize + (!ismaxlayer) * sizeof(ElementBlock_impl);
		return std::make_tuple(ismaxlayer, elmsize, blocksize);
	}
	template <class Hier>
	static constexpr std::tuple<bool, size_t, size_t> GetBlockTraits(HierarchyWrapper<Hier> h, LayerType layer)
	{
		if constexpr (s_hierarchy<Hier>)
		{
			bool ismaxlayer = layer == Hier::GetMaxLayer();
			size_t elmsize = Hier::GetElementSize(layer);
			size_t blocksize = elmsize + (!ismaxlayer) * sizeof(ElementBlock_impl);
			return std::make_tuple(ismaxlayer, elmsize, blocksize);
		}
		else
		{
			bool ismaxlayer = layer == h.value().GetMaxLayer();
			size_t elmsize = h.value().GetElementSize(layer);
			size_t blocksize = elmsize + (!ismaxlayer) * sizeof(ElementBlock_impl);
			return std::make_tuple(ismaxlayer, elmsize, blocksize);
		}
	}

	template <class Hier, LayerType L>
		requires s_hierarchy<Hier> || f_hierarchy<Hier>
	static consteval bool IsMaxLayer(HierarchyWrapper<Hier>, LayerConstant<L>)
	{
		return Hierarchy::GetMaxLayer() == L;
	}
	static constexpr bool IsMaxLayer(HierarchySD h, LayerType layer)
	{
		if constexpr (s_hierarchy<Hierarchy> || f_hierarchy<Hierarchy>)
			return Hierarchy::GetMaxLayer() == layer;
		else
			return h.value().GetMaxLayer() == layer;
	}

	template <s_hierarchy Hier, LayerType L>
	static consteval size_t GetBlockSize(HierarchyWrapper<Hier>, LayerConstant<L>)
	{
		return GetBlockSize(HierarchyWrapper<Hier>{}, L);
	}
	template <f_hierarchy Hier, LayerType L>
	static constexpr size_t GetBlockSize(HierarchyWrapper<Hier> h, LayerConstant<L>)
	{
		constexpr bool ismaxlayer = IsMaxLayer(HierarchyWrapper<Hier>{}, LayerConstant<L>{});
		constexpr size_t blocksize = (!ismaxlayer) * sizeof(ElementBlock_impl);
		return GetElementSize(h, LayerConstant<L>{}) + blocksize;
	}
	static constexpr size_t GetBlockSize(HierarchySD h, LayerType layer)
	{
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
		static_assert(!s_hierarchy<Hierarchy>);
		return h.value().GetPlaceholdersIn(layer);
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
