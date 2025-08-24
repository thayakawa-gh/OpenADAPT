#ifndef ADAPT_RANGE_HISH_H
#define ADAPT_RANGE_HISH_H

#include <mutex>
#include <thread>
#include <tuple>
#include <OpenADAPT/Utility/TypeTraits.h>
#include <OpenADAPT/Container/Tree.h>
#include <OpenADAPT/Container/Hist.h>
#include <OpenADAPT/Range/Filter.h>
#include <OpenADAPT/Range/Extract.h>

namespace adapt
{

template <class Ax>
	requires (node_or_placeholder<Ax> || named_node_or_placeholder<Ax>)
struct AxisArgs
{
	template <class Ax_>
		requires (node_or_placeholder<Ax_> || named_node_or_placeholder<Ax_>)
	AxisArgs(Ax_&& ax, double wbin_, double cbin_ = 0.0)
		: axis(std::forward<Ax_>(ax)), wbin(wbin_), cbin(cbin_) {}
	Ax axis;
	double wbin;
	double cbin = 0.;
};
template <class Ax_>
	requires (node_or_placeholder<Ax_> || named_node_or_placeholder<Ax_>)
AxisArgs(Ax_&&, double, double = 0.0) -> AxisArgs<Ax_>;

namespace detail
{

template <class Range>
class ToHistogram
{
	template <bool AllStat, class Axes, class Vars>
	struct HistResult;
	template <class ...Axes, StaticChar ...AxNames, class ...Vars, StaticChar ...VarNames>
	struct HistResult<true, std::tuple<AxisArgs<std::tuple<StaticString<AxNames>, Axes>>...>,
							std::tuple<std::tuple<StaticString<VarNames>, Vars>...>>
	{
		using Layer = NamedTuple<Named<AxNames, double>..., Named<VarNames, typename std::decay_t<Vars>::RetType>...>;
		using Res = SHist<sizeof...(Axes), Layer>;
	};
	template <class ...Axes, class ...AxNames, class ...Vars, class ...VarNames>
	struct HistResult<false, std::tuple<AxisArgs<std::tuple<AxNames, Axes>>...>,
							 std::tuple<std::tuple<VarNames, Vars>...>>
	{
		using Res = DHist;
	};

	template <size_t N, class ...Axes>
	void ConvAxisAttr(std::vector<AxisAttr>& attr, const std::tuple<Axes...>& axes) const
	{
		if constexpr (N < sizeof...(Axes))
		{
			const auto& ax = std::get<N>(axes);
			attr[N] = AxisAttr{ std::string(std::get<0>(ax.axis)), ax.wbin, ax.cbin };
			ConvAxisAttr<N + 1>(attr, axes);
		}
	}
	template <size_t N, class ...Vars>
	void ConvExtra(std::vector<std::pair<std::string, FieldType>>& extr, const std::tuple<Vars...>& vars) const
	{
		if constexpr (N < sizeof...(Vars))
		{
			const auto& var = std::get<N>(vars);
			extr[N] = { std::string(std::get<0>(var)), std::get<1>(var).GetType() };
			ConvExtra<N + 1>(extr, vars);
		}
	}
	template <any_hist Hist, class ...Axes, class ...Vars>
	void SetAxesAttr(Hist& hist, const std::tuple<Axes...>& axes, const std::tuple<Vars...>& vars) const
	{
		constexpr size_t dim = sizeof...(Axes);
		if constexpr (d_hist<Hist>)
		{
			std::vector<AxisAttr> attr(dim);
			ConvAxisAttr<0>(attr, axes);
			std::vector<std::pair<std::string, FieldType>> extr(sizeof...(Vars));
			ConvExtra<0>(extr, vars);
			hist.SetAxesAttr(attr, extr);
		}
		else
		{
			std::vector<AxisAttr> attr(dim);
			ConvAxisAttr<0>(attr, axes);
			hist.SetAxesAttr(attr);
		}
	}

	template <any_traverser Trav, class ...Axes, class ...Vars>
	void InitAxesVars(const Trav& t, std::tuple<Axes...>& axes, std::tuple<Vars...>& vars) const
	{
		auto initax = [&t]<class ...Ax>(Ax&& ...ax) { InitAll(t, std::get<1>(ax.axis)...); };
		auto initvar = [&t]<class ...Var>(Var&& ...var) { InitAll(t, std::get<1>(var)...); };
		std::apply(initax, axes);
		std::apply(initvar, vars);
	}
	template <class ...Axes, class ...Vars>
	LayerType GetMaxLayer(std::tuple<Axes...>& axes, std::tuple<Vars...>& vars) const
	{
		auto get_max_layer_ax = []<class ...Ax>(Ax&& ...ax) { return std::max({ std::get<1>(ax.axis).GetLayer()... }); };
		auto get_max_layer_var = []<class ...Var>(Var&& ...var) { return std::max({ std::get<1>(var).GetLayer()... }); };
		LayerType max_layer_ax = std::apply(get_max_layer_ax, axes);
		LayerType max_layer_var = std::apply(get_max_layer_var, vars);
		return std::max(max_layer_ax, max_layer_var);
	}

	template <bool AllStat, any_hist Hist, class PHs, any_traverser Trav,
			  similar_to_xt<std::tuple> AxesTuple, similar_to_xt<std::tuple> VarsTuple,
			  size_t ...AxIs, size_t ...VarIs>
	void SetAxesAndVars(Hist& hist, [[maybe_unused]] const PHs& phs, Trav& t, const AxesTuple& axes, const VarsTuple& vars,
						std::index_sequence<AxIs...>, std::index_sequence<VarIs...>) const
	{
		if constexpr (AllStat)
		{
			try
			{
				hist.Append(
					std::forward_as_tuple(std::get<1>(std::get<AxIs>(axes).axis).Evaluate(t)...),
					std::forward_as_tuple(std::get<1>(std::get<VarIs>(vars)).Evaluate(t)...));
			}
			catch (JointError) {}
			catch (NoElements) {}
		}
		else
		{
			constexpr size_t dim = sizeof...(AxIs);
			typename Hist::ElementRef_0 binref;
			typename Hist::ElementRef_1 back;
			auto eval = [&t](auto&& np, [[maybe_unused]] auto type)
			{
				if constexpr (typed_node_or_placeholder<decltype(np)> || ctti_node_or_placeholder<decltype(np)>)
				{
					return np.Evaluate(t);
				}
				else
					return np.Evaluate(t, type);
			};
			auto set_var = [&back, &t](const auto& ph, auto&& np) -> int32_t
			{
				// !AllStatなので出力はDHistなのだが、
				// npの方はCttiやTypedの可能性があるので、分岐する必要がある。
				// この分岐はevalではできない。なぜなら、
				// switch文中でcase I08: back[ph].as_unsafe<I08>() = ...;のように展開されたとき、
				// evalの戻り値がstd::stringだったりするとエラーになるため。
				if constexpr (typed_node_or_placeholder<decltype(np)> || ctti_node_or_placeholder<decltype(np)>)
				{
					constexpr FieldType type = np.GetType();
					back[ph].template as_unsafe<type>() = np.Evaluate(t);
				}
				else
				{
#define CASE(T) back[ph].template as_unsafe<T>() = np.Evaluate(t, Number<T>{});
					ADAPT_SWITCH_FIELD_TYPE(np.GetType(), CASE, throw MismatchType("");)
#undef CASE
				}
					return 0;
			};
			try
			{
				binref = hist.AppendDefaultElement(eval(std::get<1>(std::get<AxIs>(axes).axis), Number<FieldType::F64>{})...);
				back = binref.Back();
			}
			// これらの例外処理の時点ではAppendが不成立なので、それらをPopする必要はない。
			catch (JointError) { return; }
			catch (NoElements) { return; }
			try
			{
				(set_var(phs[VarIs + dim], std::get<1>(std::get<VarIs>(vars))), ...);
			}
			// こちらでは上のAppendが成立してしまっており余計な要素が追加されているので、
			// 例外が飛んだ場合はそれを削除しておく必要がある。
			catch (JointError) { binref.Pop(); }
			catch (NoElements) { binref.Pop(); }
		}
	}

	struct ThGlobal
	{
		std::mutex mtx;
		BindexType next = 0;
		BindexType end = 0;
	};

	template <bool AllStat, traversal_range Range_, any_hist Hist, class ...Axes, class ...Vars, size_t ...AxIs, size_t ...VarIs>
	void Exec_multi_impl(std::bool_constant<AllStat>, Range_& range, Hist& res, ThGlobal& g, std::tuple<Axes...> axes, std::tuple<Vars...> vars,
						 std::index_sequence<AxIs...> ax_is, std::index_sequence<VarIs...> var_is) const
	{
		LayerType max = GetMaxLayer(axes, vars);
		range.SetTravLayer(max);
		auto t = range.begin();
		auto end = range.end();
		InitAxesVars(t, axes, vars);
		const auto& ph = res.GetPlaceholdersIn(1_layer);
		auto assignnext = [](auto& t, BindexType row, BindexType end)
		{
			for (; row < end; ++row) if (t.AssignRow(row)) return true;
			return false;
		};
		while (true)
		{
			BindexType row = 0, row_end = 0;
			{
				int32_t gran = GetGranularity();
				std::lock_guard lock(g.mtx);
				if (g.next >= g.end) return; // 全ての要素を処理した。
				row = g.next;
				row_end = std::min(row + gran, g.end);
				t.AssignRow(row);
				g.next += gran;
			}
			if (!assignnext(t, row, row_end)) continue; // 次の要素がなかった。
			for (LayerType moved = 0_layer; t != end && (moved != 0_layer || t.GetRow() < row_end); moved = t.Incr())
			{
				SetAxesAndVars<AllStat>(res, ph, t, axes, vars, ax_is, var_is);
			}
		}
	}
	template <bool AllStat, traversal_range Range_, any_hist Hist, class AxesTuple, class VarsTuple>
	void Exec_multi(std::bool_constant<AllStat>, Range_& range, Hist& res, ThGlobal& g, AxesTuple axes, VarsTuple vars) const
	{
		constexpr size_t dim = std::tuple_size_v<std::remove_cvref_t<AxesTuple>>;
		constexpr size_t extr = std::tuple_size_v<std::remove_cvref_t<VarsTuple>>;
		Exec_multi_impl<AllStat>(std::bool_constant<AllStat>{}, range, res, g, std::move(axes), std::move(vars),
								 std::make_index_sequence<dim>{}, std::make_index_sequence<extr>{});
	}

public:

	template <traversal_range Range_, bool AllStat, similar_to_xt<std::tuple> AxesTuple, similar_to_xt<std::tuple> VarsTuple>
	auto Exec(Range_&& range, std::bool_constant<AllStat> all_stat, AxesTuple&& axes, VarsTuple&& vars) const
	{
		constexpr size_t dim = std::tuple_size_v<std::remove_cvref_t<AxesTuple>>;
		constexpr size_t extr = std::tuple_size_v<std::remove_cvref_t<VarsTuple>>;
		using HistRes = HistResult<AllStat, std::remove_cvref_t<AxesTuple>, std::remove_cvref_t<VarsTuple>>::Res;
		HistRes res;
		SetAxesAttr(res, axes, vars);

		const int32_t nth = GetNumOfThreads();
		if (nth == 1)
		{
			// 単一スレッドで実行する場合。
			const auto& ph = res.GetPlaceholdersIn(1_layer);
			LayerType max = GetMaxLayer(axes, vars);
			range.SetTravLayer(max);
			auto t = range.begin();
			auto end = range.end();
			InitAxesVars(t, axes, vars);
			for (; t != end; ++t)
			{
				// 各軸の値を取得して、ヒストグラムに追加する。
				SetAxesAndVars<AllStat>(res, ph, t, axes, vars, std::make_index_sequence<dim>{}, std::make_index_sequence<extr>{});
			}
		}
		else
		{
			std::vector<HistRes> bufs(nth);
			std::vector<std::thread> threads;
			threads.reserve(nth);
			ThGlobal g;
			g.end = (BindexType)range.GetContainer().GetSize();
			for (int32_t i = 0; i < nth; ++i)
			{
				auto axes_dup = MakeDecayedCopy(axes);
				auto vars_dup = MakeDecayedCopy(vars);
				SetAxesAttr(bufs[i], axes, vars);
				threads.emplace_back(&ToHistogram::Exec_multi<AllStat, Range_, HistRes, decltype(axes_dup), decltype(vars_dup)>,
									 this, all_stat, std::ref(range), std::ref(bufs[i]), std::ref(g), std::move(axes_dup), std::move(vars_dup));
			}
			for (auto& th : threads) th.join();
			// 全てのスレッドが終了したら、結果を統合する。
			Bin<dim> min, max;
			bufs[0].GetMin(min);
			bufs[0].GetMax(max);
			for (int32_t th = 1; th < nth; ++th)
			{
				Bin<dim> min2, max2;
				bufs[th].GetMin(min2);
				bufs[th].GetMax(max2);
				for (size_t d = 0; d < dim; ++d)
				{
					if (min[d] > min2[d]) min[d] = min2[d];
					if (max[d] < max2[d]) max[d] = max2[d];
				}
			}
			res.Resize(min, max);
			BindexType size = res.GetSize();
			std::vector<std::pair<typename HistRes::ElementRef_0, bool>> refs(nth);
			for (BindexType row = 0; row < size; ++row)
			{
				Bin<dim> bin = IndexToBin<dim>{}(row, min, max);
				BindexType total = 0;
				for (int32_t th = 0; th < nth; ++th)
				{
					auto& buf = bufs[th];
					if (buf.IsInside(bin) && !buf[bin].IsEmpty())
					{
						auto r = buf[bin];
						total += r.GetSize();
						refs[th] = { r, true };
					}
					else refs[th] = { {}, false };
				}
				if (total == 0) continue; // このbinには何も入っていない。
				auto binref = res[bin];
				binref.Reserve(total);
				for (int32_t th = 0; th < nth; ++th)
				{
					if (refs[th].second)
						binref.MoveConcat(refs[th].first);
				}
			}
		}
		return res;
	}
};

template <size_t I, named_node_or_placeholder NP>
AxisArgs<NP>&& AddDefaultName(AxisArgs<NP>&& ax) { return std::move(ax); }
template <size_t I, named_node_or_placeholder NP>
const AxisArgs<NP>& AddDefaultName(const AxisArgs<NP>& ax) { return ax; }

template <size_t I, node_or_placeholder NP, StaticString Name = StaticString<"axis">{} + ToStr(Number<I>{})>
auto AddDefaultName(const AxisArgs<NP>& ax) { return AxisArgs{ ax.axis.named(Name), ax.wbin, ax.cbin }; }
template <size_t I, node_or_placeholder NP, StaticString Name = StaticString<"axis">{} + ToStr(Number<I>{})>
auto AddDefaultName(AxisArgs<NP>&& ax) { return AxisArgs{ std::move(ax.axis).named(Name), ax.wbin, ax.cbin }; }

template <similar_to_xt<std::tuple> TupleAxes, similar_to_xt<std::tuple> TupleVars, size_t ...Is, size_t ...Js>
auto Hist_impl2(TupleAxes&& axes, TupleVars&& vars, std::index_sequence<Is...>, std::index_sequence<Js...>)
{
	// 全てのaxes、varsがstatistically_namedかつCttiであれば、出力はSHistになる。
	// Histの場合は階層構造が固定されているため、原理的にはCttiでなくTypedでもSHistにできるのだが、
	// Extractと仕様を揃えたほうがいいような気がする。
	using DAxes = std::remove_cvref_t<TupleAxes>;
	using DVars = std::remove_cvref_t<TupleVars>;
	constexpr bool all_stat =
		(s_ctti_named_node_or_placeholder<decltype(std::remove_cvref_t<std::tuple_element_t<Is, DAxes>>::axis)> && ...) &&
		(s_ctti_named_node_or_placeholder<std::tuple_element_t<Js, DVars>> && ...);
	return RangeConversion<ToHistogram, std::bool_constant<all_stat>, TupleAxes, TupleVars>
		(std::bool_constant<all_stat>{}, axes, vars);
}

template <similar_to_xt<std::tuple> TupleAxes, similar_to_xt<std::tuple> TupleVars, size_t ...Is, size_t ...Js>
auto Hist_impl(TupleAxes&& axes, TupleVars&& vars, std::index_sequence<Is...> is, std::index_sequence<Js...> js)
{
	// Hist_impl2に送るときは、std::tupleのrvalue refを外して値に変換しておく。
	// 受け取った引数が一時変数だった場合に、寿命が尽きてしまうのを避けるために。
	return Hist_impl2(
		MakeTemporaryTuple(AddDefaultName<Is>(std::get<Is>(std::forward<TupleAxes>(axes)))...),
		MakeTemporaryTuple(AddDefaultName<Js>(std::get<Js>(std::forward<TupleVars>(vars)))...), is, js);
}

}

template <similar_to_xt<std::tuple> TupleAxes, similar_to_xt<std::tuple> TupleVars>
auto Hist(TupleAxes&& axes, TupleVars&& vars)
{
	return detail::Hist_impl(std::forward<TupleAxes>(axes), std::forward<TupleVars>(vars),
							 std::make_index_sequence<std::tuple_size_v<std::remove_cvref_t<TupleAxes>>>{},
							 std::make_index_sequence<std::tuple_size_v<std::remove_cvref_t<TupleVars>>>{});
}
template <similar_to_xt<AxisArgs> ...Axes>
auto Hist(Axes&& ...axes)
{
	return Hist(std::forward_as_tuple(std::forward<Axes>(axes)...), std::make_tuple());
}

namespace detail
{
template <similar_to_xt<std::tuple> AxesTuple, named_or_anon_node_or_placeholder NP, named_or_anon_node_or_placeholder ...Args>
auto Hist_impl(AxesTuple&& axes, NP&& np, double wbin, Args&& ...args)
{
	// AxisArgsの寿命が尽きるとaxes2の中身が空になってしまうので、ここはforward_as_tupleは使わずmake_tuple。
	auto axes2 = std::tuple_cat(std::forward<AxesTuple>(axes), std::make_tuple(AxisArgs(std::forward<NP>(np), wbin)));
	// argsの残りはextra fieldsなので、Histにわたす。
	return Hist(std::move(axes2), std::forward_as_tuple(std::forward<Args>(args)...));
}
template <similar_to_xt<std::tuple> AxesTuple, named_or_anon_node_or_placeholder NP, named_or_anon_node_or_placeholder ...Args>
auto Hist_impl(AxesTuple&& axes, NP&& np, double wbin, double cbin, Args&& ...args)
{
	// AxisArgsの寿命が尽きるとaxes2の中身が空になってしまうので、ここはforward_as_tupleは使わずmake_tuple。
	auto axes2 = std::tuple_cat(std::forward<AxesTuple>(axes), std::make_tuple(AxisArgs(std::forward<NP>(np), wbin, cbin)));
	// argsの残りはextra fieldsなので、Histにわたす。
	return Hist(std::move(axes2), std::forward_as_tuple(std::forward<Args>(args)...));
}
template <similar_to_xt<std::tuple> AxesTuple, named_or_anon_node_or_placeholder NP, named_or_anon_node_or_placeholder Next, class ...Args>
	requires (std::convertible_to<Args, double> || ...)
auto Hist_impl(AxesTuple&& axes, NP&& np, double wbin, Next&& next, Args&& ...args)
{
	// AxisArgsの寿命が尽きるとaxes2の中身が空になってしまうので、ここはforward_as_tupleは使わずmake_tuple。
	auto axes2 = std::tuple_cat(std::forward<AxesTuple>(axes), std::make_tuple(AxisArgs(std::forward<NP>(np), wbin)));
	// argsの中にはまだdouble類似型が残っている、つまりaxesに渡すべきものが残っている。
	return Hist_impl(std::move(axes2), std::forward<Next>(next), std::forward<Args>(args)...);
}
template <similar_to_xt<std::tuple> AxesTuple, named_or_anon_node_or_placeholder NP, named_or_anon_node_or_placeholder Next, class ...Args>
	requires (std::convertible_to<Args, double> || ...)
auto Hist_impl(AxesTuple&& axes, NP&& np, double wbin, double cbin, Next&& next, Args&& ...args)
{
	// AxisArgsの寿命が尽きるとaxes2の中身が空になってしまうので、ここはforward_as_tupleは使わずmake_tuple。
	auto axes2 = std::tuple_cat(std::forward<AxesTuple>(axes), std::make_tuple(AxisArgs(std::forward<NP>(np), wbin, cbin)));
	// argsの中にはまだdouble類似型が残っている、つまりaxesに渡すべきものが残っている。
	return Hist_impl(std::move(axes2), std::forward<Next>(next), std::forward<Args>(args)...);
}
}
template <named_or_anon_node_or_placeholder NP, class ...Args>
	requires ((named_or_anon_node_or_placeholder<Args> || std::convertible_to<Args, double>) && ...)
auto Hist(NP&& np, double wbin, Args&& ...args)
{
	return detail::Hist_impl(std::make_tuple(), std::forward<NP>(np), wbin, std::forward<Args>(args)...);
}

}

#endif