#ifndef ADAPT_RANGE_EXTRACT_H
#define ADAPT_RANGE_EXTRACT_H

#include <numeric>
#include <mutex>
#include <thread>
#include <OpenADAPT/Common/Global.h>
#include <OpenADAPT/Utility/StaticString.h>
#include <OpenADAPT/Evaluator/Placeholder.h>
#include <OpenADAPT/Evaluator/ConstNode.h>
#include <OpenADAPT/Evaluator/FieldNode.h>
#include <OpenADAPT/Evaluator/FuncNode.h>
#include <OpenADAPT/Evaluator/LayerFuncNode.h>
#include <OpenADAPT/Container/Tree.h>
#include <OpenADAPT/Traverser/ExternalTraverser.h>
#include <OpenADAPT/Range/Filter.h>
#include <OpenADAPT/Range/RangeAdapter.h>

namespace adapt
{

namespace detail
{

template <class Range>
class CttiExtractor
{
	using Container = typename std::remove_cvref_t<Range>::Container;

	template <LayerType Layer, class ...Cmp>
	static consteval auto GetStructure_np(LayerConstant<Layer>, TypeList<Cmp...>, TypeList<>)
	{
		return NamedTuple<Cmp...>{};
	}
	template <LayerType Layer, StaticChar Name, class NP, class ...Cmp, class ...NPs_>
	static consteval auto GetStructure_np(LayerConstant<Layer> layer, TypeList<Cmp...>, TypeList<std::tuple<StaticString<Name>, NP>, NPs_...>)
	{
		using DecayedNP = std::decay_t<NP>;
		if constexpr (DecayedNP::GetLayer() == Layer)
			return GetStructure_np(layer, TypeList<Cmp..., Named<Name, std::decay_t<typename DecayedNP::RetType>>>{}, TypeList<NPs_...>{});
		else
			return GetStructure_np(layer, TypeList<Cmp...>{}, TypeList<NPs_...>{});
	}
	template <class ...Names, class ...CttiPHs>
	static consteval auto ConvertToNamedType(std::tuple<Names...>, std::tuple<CttiPHs...>)
	{
		return TypeList<Named<Names::Chars, typename CttiPHs::RetType>...>{};
	}
	template <bool AllFieldsFlag, LayerType Layer, class ...NamedNPs>
	static consteval auto GetStructure(LayerConstant<Layer>, TypeList<>, TypeList<NamedNPs...>)
	{
		constexpr auto layer_const = LayerConstant<Layer>{};
		if constexpr (AllFieldsFlag)
		{
			constexpr auto name_types = ConvertToNamedType(Container::GetFieldNamesIn(layer_const), Container::GetPlaceholdersIn(layer_const));
			return GetStructure_np(layer_const, name_types, TypeList<NamedNPs...>{});
		}
		else
		{
			return GetStructure_np(layer_const, TypeList<>{}, TypeList<NamedNPs...>{});
		}
	}
	template <bool, class, class>
	struct Structure_impl;
	template <bool AllFieldsFlag, class ...NamedNPs, LayerType ...Layers>
	struct Structure_impl<AllFieldsFlag, TypeList<NamedNPs...>, std::integer_sequence<LayerType, Layers...>>
	{
		using Type = STree<decltype(GetStructure<AllFieldsFlag>(LayerConstant<LayerType(Layers - 1)>{}, TypeList<>{}, TypeList<NamedNPs...>{}))... > ;
	};

	template <bool AllFieldsFlag, class ...Names, class ...NPs>
	static consteval LayerType GetLayer(TypeList<std::tuple<Names, NPs>...>)
	{
		//AllFieldsFlagがtrueの場合、Containerはs_containerであることがExtract_impl2関数内で保証されている。
		if constexpr (AllFieldsFlag)
			return Container::MaxLayer;
		else
			return std::max({ std::decay_t<NPs>::GetLayer()... });
	}

	template <bool AllFieldsFlag, class ...NPs>
	struct RetType
	{
		static constexpr LayerType MaxLayer = GetLayer<AllFieldsFlag>(TypeList<NPs...>{});
		using Type = Structure_impl<AllFieldsFlag, TypeList<NPs...>, std::make_integer_sequence<LayerType, LayerType(MaxLayer + 2)>>::Type;
	};

	template <LayerType Layer, size_t Index, class STree_, class Trav, class ElementRef, class ...NamedNPs_>
	void Exec_eval(const Trav& t, ElementRef& ref, std::tuple<NamedNPs_...> nps) const
	{
		if constexpr (Index < sizeof...(NamedNPs_))
		{
			using Decayed = std::decay_t<GetType_t<Index, NamedNPs_...>>;
			using NP = std::tuple_element_t<1, Decayed>;//0は名前、1がノードである。
			if constexpr (NP::GetLayer() != Layer)
				//関係ない層のノードも混じっているので、その場合はスキップ。
				Exec_eval<Layer, Index + 1, STree_>(t, ref, nps);
			else
			{
				using StatName = std::tuple_element_t<0, Decayed>;
				static constexpr auto ph = STree_::GetPlaceholder(StatName{});
				const auto& np = std::get<Index>(nps);//nはnamedなので、NPとは型が異なる。
				//npはstd::tuple<name, node>のような形になっているので、std::get<1>で取得する必要がある。
				ref.GetField(ph) = std::get<1>(np).Evaluate(t);
				Exec_eval<Layer, Index + 1, STree_>(t, ref, nps);
			}
		}
	}
	template <bool AllFieldsFlag, class STree_, class STreeSrc, class ...NamedNPs_>
	void Exec_top(std::bool_constant<AllFieldsFlag>, STree_& res, const STreeSrc& src, std::tuple<NamedNPs_...> nps) const
	{
		auto elmref = res.GetTopElement();
		if constexpr (AllFieldsFlag)
		{
			//もしAllFieldsFlagがtrueの場合、bufには元のコンテナの全フィールドを代入する必要がある。
			auto srcref = src.GetTopElement();
			srcref.CopyFieldsTo(-1_layer, elmref);
		}
		Exec_eval<-1_layer, 0, STree_>(src, elmref, nps);
	}
	template <bool AllFieldsFlag, LayerType Layer, LayerType MaxLayer, class STree_, class Traverser, class ElementBlock, class ElementRef, class ...NamedNPs_>
	void Exec_rec(std::bool_constant<AllFieldsFlag> b, STree_& res, Traverser& t, LayerConstant<Layer> layer, LayerConstant<MaxLayer> maxlayer,
				  ElementRef upref, std::vector<ElementBlock>& bufs, std::tuple<NamedNPs_...> nps) const
	{
		using enum FieldType;
		using ElementListRef = ElementListRef_impl<typename STree_::Hierarchy, std::type_identity_t, LayerConstant<Layer>>;
		static_assert(std::same_as<ElementBlock, typename ElementListRef::ElementBlock>);

		ElementListRef buf(&res, LayerConstant<Layer>{}, & bufs[Layer]);
		bool pushflag = true;

		for (bool end = true; end; end = t.MoveForward(Layer))
		{
			if (pushflag) buf.PushDefaultElement();
			pushflag = true;
			try
			{
				auto elmref = buf.Back();
				if constexpr (AllFieldsFlag)
				{
					//もしAllFieldsFlagがtrueの場合、bufには元のコンテナの全フィールドを代入する必要がある。
					auto srcref = *t.GetIterator(Layer);
					srcref.CopyFieldsTo(LayerConstant<Layer>{}, elmref);
				}
				//ここ、CreateValueを個々に呼ぶことは難しいかもしれない。
				//万一SkipOverが投げられた場合、どこまで構築されたかを確認していちいちDestroyしなければならなくなる。
				//デフォルトコンストラクタを先に呼んでおくのとどちらがいいのか。
				Exec_eval<Layer, 0, STree_>(t, elmref, nps);

				if constexpr (Layer < MaxLayer)
					Exec_rec(b, res, t, layer + 1_layer, maxlayer, elmref, bufs, nps);
			}
			catch (JointError)
			{
				//Jointに失敗した場合などに投げられることがある。
				//既にPush済みなので、
				pushflag = false;
			}
			catch (NoElements)
			{
				pushflag = false;
			}
		}
		auto list = upref.GetLowerElements();
		if constexpr (sized_traversal_range<Range> && Layer == 0_layer)
			list.SwapElementBlock(buf);
		else
			list.CreateElementsFrom(std::move(buf));
	}

	struct Order
	{
		int32_t th = 0;
		BindexType count = 0;
	};
	struct ThGlobal
	{
		std::mutex mutex;
		BindexType next = 0;
		BindexType end = 0;
		std::vector<Order> order;
	};
	template <class ElementBlock, class ...NamedNPs>
	struct ThLocal
	{
		BindexType count = 0;
		std::vector<ElementBlock> bufs;
		std::tuple<NamedNPs...> nodes;
	};

	template <bool AllFieldsFlag, LayerType MaxLayer, class STree_, class ThLocal_>
	void Exec_multi(std::bool_constant<AllFieldsFlag> b, int32_t th, STree_& res, const std::remove_cvref_t<Range>& range, LayerConstant<MaxLayer> maxlayer,
					ThGlobal& global, ThLocal_& local) const
	{
		using enum FieldType;
		using ElementListRef = ElementListRef_impl<typename STree_::Hierarchy, std::type_identity_t, LayerConstant<(LayerType)0>>;

		ElementListRef buf(&res, 0_layer, &local.bufs[0_layer]);
		bool pushflag = true;
		const int32_t gran = GetGranularity();
		auto t = range.begin();
		auto init_all = [&t](auto& ...nps)
		{
			DoNothing(InitNP(std::get<1>(nps), t)...);
		};
		std::apply(init_all, local.nodes);
		BindexType row = 0;
		BindexType end = 0;
		auto assignnext = [](auto& t, BindexType row, BindexType end)
		{
			for (; row < end; ++row) if (t.AssignRow(row)) return true;
			return false;
		};

		while (true)
		{
			BindexType* count = nullptr;
			{
				std::lock_guard lock(global.mutex);
				if (global.next == global.end) break;
				row = global.next;//次に読むべきrow。
				BindexType x = row / gran + BindexType(row % gran != 0);//orderのインデックス。
				end = std::min(row + gran, global.end);//末尾+1の位置。

				//xの位置の処理を本スレッドが担当することになったので、
				//その情報を記録しておく。
				global.order[x].th = th;
				count = &global.order[x].count;
				global.next = end;
			}
			if (!assignnext(t, row, end)) continue;
			for (bool moved = true; t.GetRow() < end && moved; moved = t.MoveForward(0_layer))
			{
				if (pushflag) buf.PushDefaultElement();
				pushflag = true;
				try
				{
					auto elmref = buf.Back();
					//ここ、CreateValueを個々に呼ぶことは難しいかもしれない。
					//万一SkipOverが投げられた場合、どこまで構築されたかを確認していちいちDestroyしなければならなくなる。
					//デフォルトコンストラクタを先に呼んでおくのとどちらがいいのか。
					if constexpr (AllFieldsFlag)
					{
						//もしAllFieldsFlagがtrueの場合、bufには元のコンテナの全フィールドを代入する必要がある。
						auto srcref = *t.GetIterator(0_layer);
						srcref.CopyFieldsTo(0_layer, elmref);
					}
					Exec_eval<(LayerType)0, 0, STree_>(t, elmref, local.nodes);

					if constexpr (0_layer < MaxLayer)
						Exec_rec(b, res, t, 1_layer, maxlayer, elmref, local.bufs, local.nodes);
					++(*count);
				}
				catch (JointError)
				{
					//Jointに失敗した場合などに投げられることがある。
					//既にPush済みなので、
					pushflag = false;
				}
				catch (NoElements)
				{
					pushflag = false;
				}
			}
		}
	}
	template <LayerType Layer, LayerType MaxLayer, class STree_, class ThLocal_>
	void Detruct(LayerConstant<Layer> layer, LayerConstant<MaxLayer> maxlayer, STree_& res, ThLocal_& local) const
	{
		if constexpr (Layer <= MaxLayer)
		{
			local.bufs[Layer].Destruct(&res, layer);
			Detruct(layer + 1_layer, maxlayer, res, local);
		}
	}
	template <LayerType MaxLayer, class STree_, class ThLocal_>
	void MoveData(int32_t my_th, LayerConstant<MaxLayer> maxlayer, STree_& res, ThGlobal& global, ThLocal_& local) const
	{
		//BindexType index = 0;//各スレッドが保存しているbufの中で、現在移動しなければならない要素の位置。
		BindexType total = 0;//全スレッドのbufの移動先となるresの中で、移動先となる要素の位置。
		auto list = res.GetLowerElements();//Layerの型が異なるので、ElementListRefではなくautoにしておく。
		using ElementListRef = ElementListRef_impl<typename STree_::Hierarchy, std::type_identity_t, LayerConstant<(LayerType)0>>;
		ElementListRef buf{ &res, 0_layer, &local.bufs[0_layer] };
		[[maybe_unused]] BindexType cap = list.GetCapacity();
		BindexType ifrom = 0;
		for (auto [th, count] : global.order)
		{
			//自身のスレッド番号と一致しない場合は、他のスレッドの担当分である。
			//この場合は単にスキップすれば良い。
			if (th == my_th)
			{
				assert(cap >= total + count);
				//ElementListRefに対してbeginを使うとiteratorを介してしまい、layerが必ずLayerTypeになってしまう。
				//イテレータを使うことが出来ない以上、LayerConstantにするためにはGetBranchを使う必要がある。
				//auto to = list.begin() + total;
				BindexType end = count + total;
				for (BindexType i = total; i < end; ++i)
				{
					list.GetBranch(i).CreateElementFrom(buf.GetBranch(ifrom));
					++ifrom;
				}
			}
			total += count;
		}
		DestructBufs(res, local.bufs, std::make_integer_sequence<LayerType, (LayerType)(maxlayer + 1_layer)>{});
	}

	template <ctti_node Node, class Trav> static int InitNP(Node&& node, const Trav& t) { node.Init(t); return 0; }
	template <ctti_placeholder Ph, class Trav> static int InitNP(Ph&&, const Trav&) { return 0; }
	template <class STree_, class ElementBlock, LayerType ...Layers>
	void DestructBufs(const STree_& res, std::vector<ElementBlock>& bufs, std::integer_sequence<LayerType, Layers...>) const
	{
		DoNothing((bufs[Layers].Destruct(&res, LayerConstant<Layers>{}), 0)...);
	}
public:
	template <traversal_range Range_, bool AllFieldsFlag, class ...NamedNPs_>
	auto Exec(Range_&& range, std::bool_constant<AllFieldsFlag> b, NamedNPs_&& ...nps) const
	{
		static_assert(container_simplex<typename std::decay_t<Range_>::Container> || !AllFieldsFlag,
			"all_fields flag is not allowed for joined containers.");
		using RetType_ = RetType<AllFieldsFlag, std::decay_t<NamedNPs_>...>;
		using STree_ = RetType_::Type;
		using ElementBlock = detail::ElementBlock_impl<typename STree_::Hierarchy, SElementBlockPolicy>;
		using ElementListRef = ElementListRef_impl<typename STree_::Hierarchy, std::type_identity_t, LayerConstant<(LayerType)0>>;

		constexpr auto maxlayer = LayerConstant<RetType_::MaxLayer>{};
		static_assert(maxlayer >= 0_layer);

		int32_t nth = GetNumOfThreads();
		STree_ res;
		Exec_top(b, res, range.GetContainer(), std::forward_as_tuple(std::forward<NamedNPs_>(nps)...));
		if (nth == 1)
		{
			std::vector<ElementBlock> bufs(maxlayer + 1_layer);
			if constexpr (sized_traversal_range<Range_>)
			{
				//sizedの場合、0層要素数は固定値で得られる。
				//まあ極めて稀に要素取得失敗などで変動する可能性はあるが、まずありえないので、
				//ここでバッファを予め確保しておく。
				ElementListRef buf{ &res, 0_layer, &bufs[0_layer] };
				buf.Reserve((BindexType)range.GetSize(0_layer));
			}
			range.SetTravLayer(maxlayer);
			auto t = std::forward<Range_>(range).begin();
			DoNothing(InitNP(std::get<1>(std::forward<NamedNPs_>(nps)), t)...);
			auto upref = res.GetTopElement();
			Exec_rec(b, res, t, 0_layer, maxlayer, upref, bufs, std::forward_as_tuple(std::forward<NamedNPs_>(nps)...));
			DestructBufs(res, bufs, std::make_integer_sequence<LayerType, (LayerType)(maxlayer + 1_layer)>{});
		}
		else
		{
			using ThLocal_ = ThLocal<ElementBlock, std::remove_cvref_t<NamedNPs_>...>;
			ThGlobal global;
			std::vector<ThLocal_> locals;
			locals.reserve(nth);
			locals.emplace_back(ThLocal_{ {}, std::vector<ElementBlock>(maxlayer + 1_layer), std::make_tuple(nps...) });
			for (int32_t i = 1; i < nth; ++i)
			{
				locals.emplace_back(ThLocal_{ {}, std::vector<ElementBlock>(maxlayer + 1_layer), locals[0].nodes });
			}
			range.SetTravLayer(maxlayer);
			{
				const BindexType size = (BindexType)range.GetContainer().GetSize();
				const int32_t gran = GetGranularity();
				const BindexType size_ = size / (BindexType)gran + (size % gran != 0 ? 1 : 0);
				assert(gran > 0);
				global.order.resize(size_);
				global.end = size;
			}
			//multi threadの場合、予めバッファをReserveしても効果がないか、下手すると悪化傾向にあったので、やめる。

			std::vector<std::thread> threads;
			for (int32_t i = 0; i < nth; ++i)
			{
				threads.emplace_back(&CttiExtractor::Exec_multi<AllFieldsFlag, RetType_::MaxLayer, STree_, ThLocal_>, this,
									 b, i, std::ref(res), std::cref(range), maxlayer,
									 std::ref(global), std::ref(locals[i]));
			}
			for (auto& th : threads) th.join();
			BindexType total = std::accumulate(global.order.begin(), global.order.end(), 0, [](BindexType sum, const Order& o) { return sum + o.count; });
			res.Reserve(total);
			res.GetLowerElements().RewriteSize(total);//デフォルトコンストラクタを呼ぶことなく、強制的にサイズを変更する。
			for (int32_t i = 0; i < nth; ++i)
				threads[i] = std::thread(&CttiExtractor::MoveData<RetType_::MaxLayer, STree_, ThLocal_>, this,
										 i, maxlayer, std::ref(res), std::ref(global), std::ref(locals[i]));
			for (auto& th : threads) th.join();
		}
		return res;
	}
};

template <class Range>
class RttiExtractor
{
	using ElementBlock = detail::ElementBlock_impl<DHierarchy<DTree>, DElementBlockPolicy>;
	using ElementListRef = ElementListRef_impl<typename DTree::Hierarchy, std::type_identity_t, LayerType>;
	using Container = std::remove_cvref_t<Range>::Container;

	template <size_t N>
	void Arrange(std::vector<std::vector<std::pair<std::string, FieldType>>>& /*structure*/,
				 std::vector<std::vector<eval::RttiFuncNode<Container>>>& /*nodes*/) const
	{}
	template <size_t N, class Var, class ...Vars>
	void Arrange(std::vector<std::vector<std::pair<std::string, FieldType>>>& structure,
				 std::vector<std::vector<eval::RttiFuncNode<Container>>>& nodes, 
				 Var&& var, Vars&& ...vars) const
	{
		auto&& [name, v] = std::forward<Var>(var);
		LayerType layer = v.GetLayer();
		FieldType type = v.GetType();
		while ((LayerType)structure.size() - 1 <= layer)
		{
			//もし現在のstructureの最下層よりも下層のノードが見つかった場合、
			//そこまでstructureとnodesの最下層を引き下げる。
			structure.emplace_back();
			nodes.emplace_back();
		}
		structure[layer + 1].emplace_back(name, type);
		nodes[layer + 1].emplace_back(eval::ConvertToRttiFuncNode(std::forward<decltype(v)>(v)));
		Arrange<N + 1>(structure, nodes, std::forward<Vars>(vars)...);
	}

	template <bool AllFieldsFlag, class TreeSrc, named_node_or_placeholder ...Vars>
	void Init(std::bool_constant<AllFieldsFlag>, const TreeSrc& src,
			  std::vector<std::vector<std::pair<std::string, FieldType>>>& structure,
			  std::vector<ElementBlock>& bufs,
			  std::vector<std::vector<eval::RttiFuncNode<Container>>>& nodes,
			  Vars&& ...vars) const
	{
		//structureは現在からの状態。ここにnodesから構造を判定し追加していく。
		//ただし、AllFieldsFlagがtrueの場合は、srcのフィールドも全てコピーする。
		if constexpr (AllFieldsFlag)
		{
			LayerType maxlayer = src.GetMaxLayer();
			structure.resize(maxlayer + 2_layer);
			nodes.resize(maxlayer + 2_layer);//Exec関数内でmaxlayerまでアクセスする可能性があるので、事実上使わないが確保はしておく。
			for (LayerType layer = -1_layer; layer <= maxlayer; ++layer)
			{
				const auto& names = src.GetFieldNamesIn(layer);
				const auto& phs = src.GetPlaceholdersIn(layer);
				for (auto [name, ph] : views::Zip(names, phs))
				{
					structure[layer + 1].emplace_back(name, ph.GetType());
				}
			}
		}
		Arrange<0>(structure, nodes, std::forward<Vars>(vars)...);
		bufs.resize(structure.size() - 1);
	}

	template <bool AllFieldsFlag, class ElementRefRes, class Traverser>
	void Exec_eval(std::bool_constant<AllFieldsFlag>, LayerType layer,
				   ElementRefRes elmref, const Traverser& t,
				   const std::vector<DTree::RttiPlaceholder>& phs,
				   const std::vector<eval::RttiFuncNode<Container>>& nodes) const
	{
		auto itph = phs.begin();
		auto itnode = nodes.begin();
		if constexpr (AllFieldsFlag)
		{
			auto get_srcref = [&t, layer]()
			{
				//tはContainerの場合とTraverserの場合がある。
				//Containerの場合、layerは必ず-1である。
				//なので、srcrefを得るにはGetTopElement()でよい。
				//一方Traverserの場合は、layerが0以上である。
				//その場合はGetIteratorを使う必要がある。
				if constexpr (any_traverser<Traverser>)
					return *t.GetIterator(layer);
				else
					return t.GetTopElement();
			};
			auto srcref = get_srcref();
			srcref.CopyFieldsTo(layer, elmref);
			itph = phs.end() - nodes.size();
		}
		for (; itnode != nodes.end(); ++itph, ++itnode)
		{
			auto field = elmref.GetField(*itph);
#define CASE(T) field.template as_unsafe<T>() = itnode->Evaluate(t, Number<T>{});
			ADAPT_SWITCH_FIELD_TYPE(itph->GetType(), CASE, throw MismatchType("");)
#undef CASE
		}
	}
	template <bool AllFieldsFlag, bool IsTopLayer, class Traverser>
	void Exec_rec(std::bool_constant<AllFieldsFlag> b, std::bool_constant<IsTopLayer>, const DTree& res, Traverser& t,
				  LayerType layer, LayerType maxlayer, DTree::ElementRef upref,
				  std::vector<ElementBlock>& bufs,
				  std::vector<std::vector<eval::RttiFuncNode<Container>>>& nodes) const
	{
		using enum FieldType;
		ElementListRef buf{ &res, layer, & bufs[layer] };
		const std::vector<eval::RttiFuncNode<Container>>& lnodes = nodes[layer + 1];
		const std::vector<DTree::RttiPlaceholder>& phs = res.GetPlaceholdersIn(layer);
		bool pushflag = true;
		for (bool end = true; end; end = t.MoveForward(layer))
		{
			//PushDefaultElementをやめてExec_evalで計算しつつコンストラクタを呼ぶ方法も試したが、
			//結果的に遅くなってしまったので、PushDefaultElementのほうが良さそうである。
			if (pushflag) buf.PushDefaultElement();
			pushflag = true;
			try
			{
				auto elmref = buf.Back();
				Exec_eval(b, layer, buf.Back(), t, phs, lnodes);

				if (layer < maxlayer) Exec_rec(b, std::false_type{}, res, t, layer + 1_layer, maxlayer, elmref, bufs, nodes);
			}
			catch (JointError)
			{
				//Jointに失敗した場合などに投げられることがある。
				//既にPush済みなので、
				pushflag = false;
			}
			catch (NoElements)
			{
				pushflag = false;
			}
		}
		auto list = upref.GetLowerElements();
		if constexpr (sized_traversal_range<Range> && IsTopLayer)
			list.SwapElementBlock(buf);
		else
			list.CreateElementsFrom(std::move(buf));
	}

	struct Order
	{
		int32_t th = 0;
		BindexType count = 0;
	};
	struct ThGlobal
	{
		std::mutex mutex;
		BindexType next = 0;
		BindexType end = 0;
		std::vector<Order> order;
	};
	struct ThLocal
	{
		BindexType count = 0;
		//ElementBlock buf0;
		std::vector<ElementBlock> bufs;
		std::vector<std::vector<eval::RttiFuncNode<Container>>> nodes;
	};
	template <bool AllFieldsFlag>
	void Exec_multi(std::bool_constant<AllFieldsFlag> b, int32_t th, const DTree& res,
					const std::remove_cvref_t<Range>& range, LayerType maxlayer,
					ThGlobal& global, ThLocal& local) const
	{
		using enum FieldType;
		ElementListRef buf{ &res, 0_layer, &local.bufs[0_layer] };
		const std::vector<eval::RttiFuncNode<Container>>& lnodes = local.nodes[1_layer];
		const std::vector<DTree::RttiPlaceholder>& phs = res.GetPlaceholdersIn(0_layer);
		bool pushflag = true;
		const int32_t gran = GetGranularity();
		auto t = range.begin();
		for (auto& n : local.nodes) for (auto& nn : n) nn.Init(t);
		BindexType row = 0;
		BindexType end = 0;
		auto assignnext = [](auto& t, BindexType row, BindexType end)
		{
			for (; row < end; ++row) if (t.AssignRow(row)) return true;
			return false;
		};
		while (true)
		{
			BindexType* count = nullptr;
			{
				std::lock_guard lock(global.mutex);
				if (global.next == global.end) break;
				row = global.next;//次に読むべきrow。
				BindexType x = row / gran + BindexType(row % gran != 0);//orderのインデックス。
				end = std::min(row + gran, global.end);//末尾+1の位置。

				//xの位置の処理を本スレッドが担当することになったので、
				//その情報を記録しておく。
				global.order[x].th = th;
				count = &global.order[x].count;
				global.next = end;
			}
			if (!assignnext(t, row, end)) continue;
			for (bool moved = true; t.GetRow() < end && moved; moved = t.MoveForward(0_layer))
			{
				if (pushflag) buf.PushDefaultElement();
				pushflag = true;
				try
				{
					auto elmref = buf.Back();
					Exec_eval(b, 0_layer, buf.Back(), t, phs, lnodes);
					if (0_layer < maxlayer) Exec_rec(b, std::false_type{}, res, t, 1_layer, maxlayer, elmref, local.bufs, local.nodes);
					++(*count);
				}
				catch (JointError)
				{
					//Jointに失敗した場合などに投げられることがある。
					//既にPush済みなので、
					pushflag = false;
				}
				catch (NoElements)
				{
					pushflag = false;
				}
			}
		}
	}

	void MoveData(int32_t my_th, DTree& res, ThGlobal& global, ThLocal& local) const
	{
		//BindexType index = 0;//各スレッドが保存しているbufの中で、現在移動しなければならない要素の位置。
		BindexType total = 0;//全スレッドのbufの移動先となるresの中で、移動先となる要素の位置。
		auto list = res.GetLowerElements();//Layerの型が異なるので、ElementListRefではなくautoにしておく。
		ElementListRef buf{ &res, 0_layer, &local.bufs[0_layer] };
		[[maybe_unused]] BindexType cap = list.GetCapacity();
		auto from = buf.begin();
		for (auto [th, count] : global.order)
		{
			//自身のスレッド番号と一致しない場合は、他のスレッドの担当分である。
			//この場合は単にスキップすれば良い。
			if (th == my_th)
			{
				assert(cap >= total + count);
				auto to = list.begin() + total;
				for (BindexType i = 0; i < count; ++i)
				{
					to->CreateElementFrom(*from);
					++to;
					++from;
				}
			}
			total += count;
		}
		LayerType maxlayer = (LayerType)local.bufs.size() - 1;
		for (LayerType l = 0_layer; l <= maxlayer; ++l) local.bufs[l].Destruct(&res, l);
	}

public:

	template <traversal_range Range_, bool AllFieldsFlag, named_node_or_placeholder ...NPs_>
	DTree Exec(Range_&& range, std::bool_constant<AllFieldsFlag> b, NPs_&& ...nps)
	{
		static_assert(container_simplex<typename std::decay_t<Range_>::Container> || !AllFieldsFlag,
			"all_fields flag is not allowed for joined containers.");
		std::vector<std::vector<std::pair<std::string, FieldType>>> structure;//m_structure[i]はi-1層のレイヤー構造。
		int32_t nth = GetNumOfThreads();
		DTree res;
		const auto& src = range.GetContainer();
		if (nth == 1)
		{
			std::vector<ElementBlock> bufs;//m_bufs[layer]にlayer層用のバッファがある。
			std::vector<std::vector<eval::RttiFuncNode<Container>>> nodes;//レイヤーごとに分けられたノード。m_nodes[layer + 1]がlayer層のノード。

			Init(b, src, structure, bufs, nodes, std::forward<NPs_>(nps)...);
			if (!structure[0].empty()) res.SetTopLayer(structure[0]);
			for (auto it = structure.begin() + 1; it != structure.end(); ++it) res.AddLayer(*it);
			res.VerifyStructure();

			LayerType maxlayer = (LayerType)structure.size() - 2;//m_structureは-1, 0, 1, ...と順に階層情報を格納しているので、-2が正しい。
			if (maxlayer < 0_layer)
				throw InvalidLayer("Maximum layer of the arguments for Extract must be equal or larger than 0.");

			if constexpr (sized_traversal_range<Range_>)
			{
				//sizedの場合、0層要素数は固定値で得られる。
				//まあ極めて稀に要素取得失敗などで変動する可能性はあるが、まずありえないので、
				//ここでバッファを予め確保しておく。
				ElementListRef buf{ &res, 0_layer, &bufs[0_layer] };
				buf.Reserve((BindexType)range.GetSize(0_layer));
			}
			range.SetTravLayer(maxlayer);
			auto t = std::forward<Range_>(range).begin();
			for (auto& n : nodes) for (auto& nn : n) nn.Init(t);
			auto ref = res.GetTopElement();
			Exec_rec(b, std::true_type{}, res, t, 0_layer, maxlayer, ref, bufs, nodes);
			Exec_eval(b, -1_layer, ref, src, res.GetPlaceholdersIn(-1_layer), nodes[0]);
			for (auto [layer, buf] : views::Enumerate(bufs)) buf.Destruct(&res, (LayerType)layer);
		}
		else
		{
			ThGlobal global;
			std::vector<ThLocal> locals(nth);

			Init(b, src, structure, locals[0].bufs, locals[0].nodes, std::forward<NPs_>(nps)...);
			for (int i = 1; i < nth; ++i)
			{
				locals[i].bufs.resize(locals[0].bufs.size());
				locals[i].nodes = locals[0].nodes;
			}
			if (!structure[0].empty()) res.SetTopLayer(structure[0]);
			for (auto it = structure.begin() + 1; it != structure.end(); ++it) res.AddLayer(*it);
			res.VerifyStructure();

			LayerType maxlayer = (LayerType)structure.size() - 2;//m_structureは-1, 0, 1, ...と順に階層情報を格納しているので、-2が正しい。
			if (maxlayer < 0_layer)
				throw InvalidLayer("Maximum layer of the arguments for Extract must be equal or larger than 0.");

			range.SetTravLayer(maxlayer);
			{
				const BindexType size = (BindexType)range.GetContainer().GetSize();
				const int32_t gran = GetGranularity();
				const BindexType size_ = size / (BindexType)gran + (size % gran != 0 ? 1 : 0);
				assert(gran > 0);
				global.order.resize(size_);
				global.end = size;
			}
			//multi threadの場合、予めバッファをReserveしても効果がないか、下手すると悪化傾向にあったので、やめる。

			std::vector<std::thread> threads;
			for (int32_t i = 0; i < nth; ++i)
			{
				threads.emplace_back(&RttiExtractor::Exec_multi<AllFieldsFlag>, this,
									 b, i, std::cref(res), std::cref(range), maxlayer,
									 std::ref(global), std::ref(locals[i]));
			}
			Exec_eval(b, -1_layer, res.GetTopElement(), src, res.GetPlaceholdersIn(-1_layer), locals[0].nodes[0]);
			for (auto& th : threads) th.join();
			BindexType total = std::accumulate(global.order.begin(), global.order.end(), 0, [](BindexType sum, const Order& o) { return sum + o.count; });
			res.Reserve(total);
			res.GetLowerElements().RewriteSize(total);//デフォルトコンストラクタを呼ぶことなく、強制的にサイズを変更する。
			for (int32_t i = 0; i < nth; ++i)
				threads[i] = std::thread(&RttiExtractor::MoveData, this, i, std::ref(res), std::ref(global), std::ref(locals[i]));
			for (auto& th : threads) th.join();
		}
		return res;
	}

};

template <class Range>
class ExtractorRelay
{
	//all_fieldsのオプションを有効にしているとき、他に一切nodeやplaceholderを与えていないと
	//CttiとRttiのどちらを使うべきかが判定できない。
	//Extractorクラスからメンバ関数等も一切排除してしまった今、RttiとCttiの実装を別クラスに分ける意味はそれほどないのだが、
	//
};


template <size_t I, named_node_or_placeholder NP>
NP&& AddDefaultName(NP&& np) { return std::forward<NP>(np); }
template <size_t I, node_or_placeholder NP, StaticString Name = StaticString<"fld">{} + ToStr(Number<I>{}) >
auto AddDefaultName(NP&& np)
{
	return std::forward<NP>(np).named(Name);
}

template <bool AllFieldsFlag, class ...Vars>
auto Extract_impl2(std::bool_constant<AllFieldsFlag> b, Vars&& ...named_vars)
{
	using Container = eval::detail::ExtractContainer<std::remove_cvref_t<decltype(std::get<1>(named_vars))>...>::Container;
	constexpr bool all_stat =
		(s_named_node_or_placeholder<Vars> && ...) &&
		(ctti_node_or_placeholder<std::tuple_element_t<1, std::decay_t<Vars>>> && ...) &&
		(!AllFieldsFlag || s_container<Container>);//全フィールド出力の場合、コンテナがstaticでないとstaticに構造決定できない。
	if constexpr (all_stat)
		return RangeConsumer<CttiExtractor, std::bool_constant<AllFieldsFlag>, Vars...>(b, std::forward<Vars>(named_vars)...);
	else
		return RangeConsumer<RttiExtractor, std::bool_constant<AllFieldsFlag>, Vars...>(b, std::forward<Vars>(named_vars)...);
}
template <bool AllFieldsFlag, size_t ...Is, class ...Vars>
auto Extract_impl(std::bool_constant<AllFieldsFlag> b, std::index_sequence<Is...>, Vars&& ...named_vars)
{
	return Extract_impl2(b, AddDefaultName<Is>(std::forward<Vars>(named_vars))...);
}
}

template <class ...Vars>
	requires ((node_or_placeholder<Vars> || named_node_or_placeholder<Vars>) && ...)
auto Extract(Vars&& ...vars)
{
	return detail::Extract_impl(std::false_type{}, std::make_index_sequence<sizeof...(Vars)>{}, std::forward<Vars>(vars)...);
}
template <class ...Vars>
	requires ((node_or_placeholder<Vars> || named_node_or_placeholder<Vars>) && ...)
auto Extract(AllFields, Vars&& ...vars)
{
	return detail::Extract_impl(std::true_type{}, std::make_index_sequence<sizeof...(Vars)>{}, std::forward<Vars>(vars)...);
}

}

#endif
