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
	template <LayerType Layer, class ...Cmp>
	static consteval auto GetStructure(LayerConstant<Layer>, TypeList<Cmp...>, TypeList<>)
	{
		return NamedTuple<Cmp...>{};
	}
	template <LayerType Layer, StaticChar Name, class NP, class ...Cmp, class ...NPs_>
	static consteval auto GetStructure(LayerConstant<Layer> layer, TypeList<Cmp...>, TypeList<std::tuple<StaticString<Name>, NP>, NPs_...>)
	{
		using DecayedNP = std::decay_t<NP>;
		if constexpr (DecayedNP::GetLayer() == Layer)
			return GetStructure(layer, TypeList<Cmp..., Named<Name, std::decay_t<typename DecayedNP::RetType>>>{}, TypeList<NPs_...>{});
		else
			return GetStructure(layer, TypeList<Cmp...>{}, TypeList<NPs_...>{});
	}
	template <class, class>
	struct Structure_impl;
	template <class ...NamedNPs, LayerType ...Layers>
	struct Structure_impl<TypeList<NamedNPs...>, std::integer_sequence<LayerType, Layers...>>
	{
		using Type = STree<decltype(GetStructure(LayerConstant<LayerType(Layers - 1)>{}, TypeList<>{}, TypeList<NamedNPs...>{}))... > ;
	};

	template <class ...Names, class ...NPs>
	static consteval LayerType GetLayer(TypeList<std::tuple<Names, NPs>...>)
	{
		return std::max({ std::decay_t<NPs>::GetLayer()... });
	}

	template <class ...NPs>
	struct RetType
	{
		static constexpr LayerType MaxLayer = GetLayer(TypeList<NPs...>{});
		using Type = Structure_impl<TypeList<NPs...>, std::make_integer_sequence<LayerType, LayerType(MaxLayer + 2)>>::Type;
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
	template <LayerType Layer, LayerType MaxLayer, class STree_, class Traverser, class ElementBlock, class ElementRef, class ...NamedNPs_>
	void Exec_rec(STree_& res, Traverser& t, LayerConstant<Layer> layer, LayerConstant<MaxLayer> maxlayer,
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
				//ここ、CreateValueを個々に呼ぶことは難しいかもしれない。
				//万一SkipOverが投げられた場合、どこまで構築されたかを確認していちいちDestroyしなければならなくなる。
				//デフォルトコンストラクタを先に呼んでおくのとどちらがいいのか。
				Exec_eval<Layer, 0, STree_>(t, elmref, nps);

				if constexpr (Layer < MaxLayer)
					Exec_rec(res, t, layer + 1_layer, maxlayer, elmref, bufs, nps);
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

	template <LayerType MaxLayer, class STree_, class ThLocal_>
	void Exec_multi(int32_t th, STree_& res, const std::remove_cvref_t<Range>& range, LayerConstant<MaxLayer> maxlayer,
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
					Exec_eval<(LayerType)0, 0, STree_>(t, elmref, local.nodes);

					if constexpr (0_layer < MaxLayer)
						Exec_rec(res, t, 1_layer, maxlayer, elmref, local.bufs, local.nodes);
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
	template <traversal_range Range_, class ...NamedNPs_>
	auto Exec(Range_&& range, NamedNPs_&& ...nps) const
	{
		using RetType_ = RetType<std::decay_t<NamedNPs_>...>;
		using STree_ = RetType_::Type;
		using ElementBlock = detail::ElementBlock_impl<typename STree_::Hierarchy, SElementBlockPolicy>;

		constexpr auto maxlayer = LayerConstant<RetType_::MaxLayer>{};
		static_assert(maxlayer >= 0_layer);

		int32_t nth = GetNumOfThreads();
		STree_ res;
		if (nth == 1)
		{
			std::vector<ElementBlock> bufs(maxlayer + 1_layer);
			range.SetTravLayer(maxlayer);
			auto t = std::forward<Range_>(range).begin();
			DoNothing(InitNP(std::get<1>(std::forward<NamedNPs_>(nps)), t)...);
			auto upref = res.GetTopElement();
			Exec_rec(res, t, 0_layer, maxlayer, upref, bufs, std::forward_as_tuple(std::forward<NamedNPs_>(nps)...));
			DestructBufs(res, bufs, std::make_integer_sequence<LayerType, (LayerType)(maxlayer + 1_layer)>{});
		}
		else
		{
			using ThLocal_ = ThLocal<ElementBlock, std::remove_cvref_t<NamedNPs_>...>;
			ThGlobal global;
			std::vector<ThLocal_> locals;
			locals.reserve(1);
			locals.emplace_back(ThLocal_{ {}, std::vector<ElementBlock>(maxlayer + 1_layer), std::make_tuple(nps...) });
			for (int32_t i = 1; i < nth; ++i)
				locals.emplace_back(ThLocal_{ {}, std::vector<ElementBlock>(maxlayer + 1_layer), locals[0].nodes});

			range.SetTravLayer(maxlayer);
			{
				const BindexType size = (BindexType)range.GetContainer().GetSize();
				const int32_t gran = GetGranularity();
				const BindexType size_ = size / (BindexType)gran + (size % gran != 0 ? 1 : 0);
				assert(gran > 0);
				global.order.resize(size_);
				global.end = size;
			}

			std::vector<std::thread> threads;
			for (int32_t i = 0; i < nth; ++i)
			{
				threads.emplace_back(&CttiExtractor::Exec_multi<RetType_::MaxLayer, STree_, ThLocal_>, this,
									 i, std::ref(res), std::cref(range), maxlayer,
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
			structure.emplace_back();
			nodes.emplace_back();
		}
		structure[layer + 1].emplace_back(name, type);
		nodes[layer + 1].emplace_back(eval::ConvertToRttiFuncNode(std::forward<decltype(v)>(v)));
		Arrange<N + 1>(structure, nodes, std::forward<Vars>(vars)...);
	}

	template <named_node_or_placeholder ...Vars>
	void Init(std::vector<std::vector<std::pair<std::string, FieldType>>>& structure,
			  std::vector<ElementBlock>& bufs,
			  std::vector<std::vector<eval::RttiFuncNode<Container>>>& nodes,
			  Vars&& ...vars) const
	{
		Arrange<0>(structure, nodes, std::forward<Vars>(vars)...);
		bufs.resize(structure.size() - 1);
	}


	template <class Traverser>
	void Exec_rec(const DTree& res, Traverser& t, LayerType layer, LayerType maxlayer, DTree::ElementRef upref,
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
			if (pushflag) buf.PushDefaultElement();
			pushflag = true;
			try
			{
				auto elmref = buf.Back();
				//ここ、CreateValueを個々に呼ぶことは難しいかもしれない。
				//万一SkipOverが投げられた場合、どこまで構築されたかを確認していちいちDestroyしなければならなくなる。
				//デフォルトコンストラクタを先に呼んでおくのとどちらがいいのか。
				for (auto [node, ph] : views::Zip(lnodes, phs))
				{
					auto field = elmref.GetField(ph);
#define CASE(T) field.template as_unsafe<T>() = node.Evaluate(t, Number<T>{});
					ADAPT_SWITCH_FIELD_TYPE(ph.GetType(), CASE, throw MismatchType("");)
#undef CASE
				}

				if (layer < maxlayer) Exec_rec(res, t, layer + 1_layer, maxlayer, elmref, bufs, nodes);
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
	void Exec_multi(int32_t th, const DTree& res,
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
					//ここ、CreateValueを個々に呼ぶことは難しいかもしれない。
					//万一SkipOverが投げられた場合、どこまで構築されたかを確認していちいちDestroyしなければならなくなる。
					//デフォルトコンストラクタを先に呼んでおくのとどちらがいいのか。
					for (auto [node, ph] : views::Zip(lnodes, phs))
					{
						auto field = elmref.GetField(ph);
#define CASE(T) field.template as_unsafe<T>() = node.Evaluate(t, Number<T>{});
						ADAPT_SWITCH_FIELD_TYPE(ph.GetType(), CASE, throw MismatchType("");)
#undef CASE
					}
					if (0_layer < maxlayer) Exec_rec(res, t, 1_layer, maxlayer, elmref, local.bufs, local.nodes);
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

	template <traversal_range Range_, named_node_or_placeholder ...NPs_>
	DTree Exec(Range_&& range, NPs_&& ...nps)
	{
		std::vector<std::vector<std::pair<std::string, FieldType>>> structure;//m_structure[i]はi-1層のレイヤー構造。
		int32_t nth = GetNumOfThreads();
		DTree res;
		if (nth == 1)
		{
			std::vector<ElementBlock> bufs;//m_bufs[layer]にlayer層用のバッファがある。
			std::vector<std::vector<eval::RttiFuncNode<Container>>> nodes;//レイヤーごとに分けられたノード。m_nodes[layer + 1]がlayer層のノード。

			Init(structure, bufs, nodes, std::forward<NPs_>(nps)...);
			if (!structure[0].empty()) res.SetTopLayer(structure[0]);
			for (auto it = structure.begin() + 1; it != structure.end(); ++it) res.AddLayer(*it);
			res.VerifyStructure();

			LayerType maxlayer = (LayerType)structure.size() - 2;//m_structureは-1, 0, 1, ...と順に階層情報を格納しているので、-2が正しい。
			if (maxlayer < 0_layer)
				throw InvalidLayer("Maximum layer of the arguments for Extract must be equal or larger than 0.");

			range.SetTravLayer(maxlayer);
			auto t = std::forward<Range_>(range).begin();
			for (auto& n : nodes) for (auto& nn : n) nn.Init(t);
			auto ref = res.GetTopElement();
			Exec_rec(res, t, 0, maxlayer, ref, bufs, nodes);
			for (auto [layer, b] : views::Enumerate(bufs)) b.Destruct(&res, (LayerType)layer);
		}
		else
		{
			ThGlobal global;
			std::vector<ThLocal> locals(nth);

			Init(structure, locals[0].bufs, locals[0].nodes, std::forward<NPs_>(nps)...);
			for (int i = 1; i < nth; ++i)
			{
				locals[i].bufs.resize(locals[0].bufs.size());
				locals[i].nodes = locals[0].nodes;
			}
			if (structure[0].empty()) res.SetTopLayer(structure[0]);
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

			std::vector<std::thread> threads;
			for (int32_t i = 0; i < nth; ++i)
			{
				threads.emplace_back(&RttiExtractor::Exec_multi, this,
									 i, std::cref(res), std::cref(range), maxlayer,
									 std::ref(global), std::ref(locals[i]));
			}
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

template <size_t I, named_node_or_placeholder NP>
NP&& AddDefaultName(NP&& np) { return std::forward<NP>(np); }
template <size_t I, node_or_placeholder NP, StaticString Name = StaticString<"fld">{} + ToStr(Number<I>{}) >
auto AddDefaultName(NP&& np)
{
	return std::forward<NP>(np).named(Name);
}

template <class ...Vars>
auto Extract_impl2(Vars&& ...named_vars)
{
	constexpr bool all_stat =
		(s_named_node_or_placeholder<Vars> && ...) &&
		(ctti_node_or_placeholder<std::tuple_element_t<1, std::decay_t<Vars>>> && ...);
	if constexpr (all_stat)
		return RangeConsumer<CttiExtractor, Vars...>(std::forward<Vars>(named_vars)...);
	else
		return RangeConsumer<RttiExtractor, Vars...>(std::forward<Vars>(named_vars)...);
}
template <size_t ...Is, class ...Vars>
auto Extract_impl(std::index_sequence<Is...>, Vars&& ...named_vars)
{
	return Extract_impl2(AddDefaultName<Is>(std::forward<Vars>(named_vars))...);
}
}

template <class ...Vars>
	requires ((node_or_placeholder<Vars> || named_node_or_placeholder<Vars>) && ...)
auto Extract(Vars&& ...vars)
{
	return detail::Extract_impl(std::make_index_sequence<sizeof...(Vars)>{}, std::forward<Vars>(vars)...);
}

}

#endif
