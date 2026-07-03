#ifndef ADAPT_PLOT_PLOTCOMMAND_H
#define ADAPT_PLOT_PLOTCOMMAND_H

#include <variant>
#include <map>
#include <algorithm>
#include <fstream>
#include <OpenADAPT/Utility/Macros.h>
#include <OpenADAPT/Utility/Verbose.h>
#include <OpenADAPT/Utility/Matrix.h>
#include <OpenADAPT/Utility/Function.h>
#include <OpenADAPT/Utility/KeywordArgs.h>
#include <OpenADAPT/Utility/Ranges.h>
#include <OpenADAPT/Plot/Core.h>
#include <OpenADAPT/Plot/Axes.h>
#include <OpenADAPT/Optimization/LeastSquares.h>



namespace adapt
{

namespace plot_detail
{

#define ADAPT_DETAIL_GET_KEYWORD_ARG_AS_VIEW(foo, NAME) auto NAME = AllView(GetKeywordArg(plot::NAME, std::ranges::empty_view<double>{}, ops...));
#define ADAPT_DETAIL_DECLTYPE_AUTO(foo, NAME) decltype(NAME)
#define ADAPT_DETAIL_FORWARD_ARG(foo, NAME) std::forward<decltype(NAME)>(NAME)

#define ADAPT_DETAIL_MAKE_PARAM_MACRO(PARAM_NAME, ...)\
ADAPT_DETAIL_EXPAND_CONV(ADAPT_DETAIL_GET_KEYWORD_ARG_AS_VIEW, foo, , __VA_ARGS__)\
return PARAM_NAME<ADAPT_DETAIL_EXPAND_CONV_COMMA(ADAPT_DETAIL_DECLTYPE_AUTO, foo, __VA_ARGS__)>(ADAPT_DETAIL_EXPAND_CONV_COMMA(ADAPT_DETAIL_FORWARD_ARG, foo, __VA_ARGS__), ops...);

#define ADAPT_DETAIL_GET_KEYWORD_ARG_IF_EXIST(foo, NAME) if constexpr (KeywordExists(plot::NAME, ops...)) NAME = GetKeywordArg(plot::NAME, ops...)
#define ADAPT_DETAIL_SET_OPTIONS_MACRO(...) ADAPT_DETAIL_EXPAND_CONV(ADAPT_DETAIL_GET_KEYWORD_ARG_IF_EXIST, foo, ;, __VA_ARGS__);

template <acceptable_range R>
	requires (!std::convertible_to<R, std::string_view>)//char[]とかはstring_viewに変換されてしまうので除外
auto AllView(R&& r) { return std::views::all(std::forward<R>(r)); }
template <acceptable_matrix_range R>
	requires (!std::convertible_to<R, std::string_view>)//char[]とかはstring_viewに変換されてしまうので除外
auto AllView(R&& r) { return std::views::all(std::forward<R>(r)); }
template <arithmetic V>
auto AllView(V v) { return v; }
inline std::string_view AllView(std::string_view s) { return s; }

struct PlotParamBase
{
	template <class Range>
	static constexpr bool IsEmptyView()
	{
		return std::same_as<std::decay_t<Range>, std::ranges::empty_view<double>> || 
			std::same_as<std::decay_t<Range>, std::ranges::empty_view<std::ranges::empty_view<double>>>;
	}
	template <class Range>
	static constexpr bool IsString()
	{
		return std::convertible_to<Range, std::string_view>;
	}
	template <class Range>
	static constexpr bool IsArithmetic()
	{
		return arithmetic<std::decay_t<Range>>;
	}

	template <plot_detail::base_option ...Ops>
	PlotParamBase(Ops ...ops)
	{
		SetBaseOptions(ops...);
	}
	virtual ~PlotParamBase() = default;

	template <class ...Ops>
	void SetBaseOptions(Ops ...ops)
	{
		ADAPT_DETAIL_SET_OPTIONS_MACRO(input, title, title_add_entries, axis);
	}

	std::string input;//データプロットなら空、関数プロットなら関数、テキストプロットならファイル名

	//Baseoption
	std::string title;
	//t_entries指定がある場合に、データ点の数を格納し、タイトルに追加する。-2ならなし。-1なら数値格納予約。0以上なら数値格納済み。
	//ちょっとややこしいが、通常はMakeDataObjectの内部でデータ点数を数える。ヒストグラムの場合に限り、事前に指定する。
	int64_t title_add_entries = -2;
	std::string axis;
	Smooth smooth = Smooth::none;

	//std::vector<std::string> column;
};


template <acceptable_arg X, acceptable_arg Y,
	acceptable_arg XE, acceptable_arg YE,
	acceptable_arg XEL, acceptable_arg XEH, acceptable_arg YEL, acceptable_arg YEH,
	acceptable_arg VC, acceptable_arg VS>
struct PointParam : public PlotParamBase
{
	template <keyword_arg ...Ops>
	PointParam(X x_, Y y_, XE xe_, YE ye_,
			   XEL xel_, XEH xeh_, YEL yel_, YEH yeh_,
			   VC vc_, VS vs_, Ops ...ops)
		: x(std::move(x_)), y(std::move(y_)), xerrorbar(std::move(xe_)), yerrorbar(std::move(ye_)),
		xerrlow(std::move(xel_)), xerrhigh(std::move(xeh_)), yerrlow(std::move(yel_)), yerrhigh(std::move(yeh_)),
		variable_color(std::move(vc_)), variable_size(std::move(vs_))
	{
		SetOptions(ops...);
	}

	template <keyword_arg ...Ops>
	void SetOptions(Ops ...ops)
	{
		SetBaseOptions(ops...);
		ADAPT_DETAIL_SET_OPTIONS_MACRO(style, linetype, linewidth, dashtype, color, color_rgb,
									   variable_color, smooth, pointtype, pointsize);
	}

	bool IsData() const
	{
		return input.empty() && !IsEmptyView<X>() && !IsEmptyView<Y>();
	}
	bool IsFile() const
	{
		return !input.empty() && IsString<X>() && IsString<Y>();
	}
	bool IsEquation() const
	{
		return !input.empty() && IsEmptyView<X>() && IsEmptyView<Y>();
	}

	static constexpr bool HasXErrorbar() { return !IsEmptyView<XE>(); }
	static constexpr bool HasYErrorbar() { return !IsEmptyView<YE>(); }
	static constexpr bool HasXErrLow() { return !IsEmptyView<XEL>(); }
	static constexpr bool HasXErrHigh() { return !IsEmptyView<XEH>(); }
	static constexpr bool HasYErrLow() { return !IsEmptyView<YEL>(); }
	static constexpr bool HasYErrHigh() { return !IsEmptyView<YEH>(); }
	static constexpr bool HasVariableColor() { return !IsEmptyView<VC>(); }
	static constexpr bool HasVariableSize() { return !IsEmptyView<VS>(); }

	bool HasLineOption() const
	{
		return linetype != -2 || linewidth != -1 || !dashtype.empty();
	}
	bool HasPointOption() const
	{
		return style != Style::points || pointtype != -1 || pointsize != -1 || !IsEmptyView<VS>();
	}

	[[no_unique_address]] X x;
	[[no_unique_address]] Y y;
	[[no_unique_address]] XE xerrorbar = {};
	[[no_unique_address]] YE yerrorbar = {};
	[[no_unique_address]] XEL xerrlow = {};
	[[no_unique_address]] XEH xerrhigh = {};
	[[no_unique_address]] YEL yerrlow = {};
	[[no_unique_address]] YEH yerrhigh = {};

	Style style = Style::points;//デフォルトではpoints

	//LineOption
	int linetype = -2;//-2ならデフォルト
	double linewidth = -1.;//-1ならデフォルト、-2ならvariable
	std::vector<int> dashtype = {};
	std::string color = {};
	std::string color_rgb = {};
	Smooth smooth = Smooth::none;
	[[no_unique_address]] VC variable_color = {};

	//PointOption
	int pointtype = -1;//-1ならデフォルト
	double pointsize = -1.;//-1ならデフォルト、-2ならvariable
	[[no_unique_address]] VS variable_size = {};
};
template <keyword_arg ...Options>
auto MakePointParam(Options ...ops)
{
	ADAPT_DETAIL_MAKE_PARAM_MACRO(PointParam, x, y, xerrorbar, yerrorbar, xerrlow, xerrhigh, yerrlow, yerrhigh, variable_color, variable_size);
}

template <acceptable_arg_except_string X, acceptable_arg_except_string Y, point_option ...Options>
auto MakeFitPointParam(const X& x, const Y& y, Options ...ops)
{
	if constexpr (KeywordExists(plot::fit_detail, ops...))
	{
		auto get_double_span = [](const auto& v)
		{
			static_assert(ranges::arithmetic_range<decltype(v)>);

			if constexpr (std::convertible_to<decltype(v), std::span<const double>>)
				return std::make_tuple(std::span<const double>(v), std::cref(v));
			else
			{
				std::vector<double> vec;
				std::ranges::copy(v, std::back_inserter(vec));
				return std::make_tuple(std::span<const double>(vec), std::move(vec));
			}
		};
		auto&& fit_opt = GetKeywordArg(plot::fit_detail, ops...);
		std::span<const double> params = fit_opt.params;
		auto&& [x_span, x_storage] = get_double_span(x);
		auto&& [y_span, y_storage] = get_double_span(y);
		LeastSquaresResult fit_result;
		auto obj_func = [f = fit_opt.func](std::span<const double> var, std::span<const double> params)
		{
			double x = var[0];
			double y = var[1];
			double y_ = f(x, y, params);
			double residual = y - y_;
			return residual * residual;
		};
		if (GetKeywordArg(plot::fit_use_yerrorbars_as_weights, false, fit_opt.options))
		{
			if constexpr (KeywordExists(plot::yerrorbar, ops...))
			{
				// yerrorbarを重みとして使う場合、エラーの逆数に変換する必要がある。
				std::vector<double> weights;
				const auto& yerr = GetKeywordArg(plot::yerrorbar, ops...);
				if constexpr (std::ranges::sized_range<decltype(yerr)>)
					weights.reserve(yerr.size());
				double min = std::numeric_limits<double>::max();
				double max = std::numeric_limits<double>::lowest();
				for (auto e : yerr)
				{
					//エラーが0のときは重みが無限大になってしまうので、0でない最も0に近い値を最小値として記録しておく。
					if (e > 0) min = std::min(min, e);
					max = std::max(max, e);
					weights.push_back(e);
				}
				for (auto& w : weights)
				{
					if (w == 0) w = min;//エラーが0のときは最小のエラーと同じ重みにする。
					else w = 1.0 / w;
				}

				fit_result = SolveLeastSquares(obj_func, params, x_span, y_span, opts::ls_weight = weights);
			}
			else
			{
				PrintWarning("fit_use_yerrorbars_as_weights option is set but yerrorbar is not provided. Fit will be performed without weights.");
				fit_result = SolveLeastSquares(obj_func, params, x_span, y_span);
			}
		}
		else
		{
			fit_result = SolveLeastSquares(obj_func, params, x_span, y_span);
		}
		std::ranges::copy(fit_result.params, std::ranges::begin(fit_opt.params));

		std::string title = GetObjFuncTitle(fit_opt.func, fit_result.params);
		auto eq = GetObjFuncEquation(fit_opt.func, x_span, fit_result.params);
		if constexpr (std::same_as<decltype(eq), std::string>)
		{
			auto make = [&eq](auto&&... args) { return MakePointParam(plot::input = eq, args...); };
			return std::apply(make, TupleCat(fit_opt.options, std::forward_as_tuple(plot::title = title, plot::s_lines)));
		}
		else
		{
			auto&& [vx, vy] = eq;
			auto make = [&vx, &vy](auto&&... args) { return MakePointParam(plot::x = std::move(vx), plot::y = std::move(vy), args...); };
			return std::apply(make, TupleCat(fit_opt.options, std::forward_as_tuple(plot::title = title, plot::s_lines)));
		}
	}
	else
		return EmptyClass{};
}

template <acceptable_arg X, acceptable_arg Y, acceptable_arg Z,
	acceptable_arg XE, acceptable_arg YE,
	acceptable_arg XEL, acceptable_arg XEH, acceptable_arg YEL, acceptable_arg YEH,
	acceptable_arg VC, acceptable_arg VS>
struct PointParam3D : public PointParam<X, Y, XE, YE, XEL, XEH, YEL, YEH, VC, VS>
{
	using Base = PointParam<X, Y, XE, YE, XEL, XEH, YEL, YEH, VC, VS>;
	template <keyword_arg ...Ops>
	PointParam3D(X x_, Y y_, Z z_, XE xe_, YE ye_,
				 XEL xel_, XEH xeh_, YEL yel_, YEH yeh_,
				 VC vc_, VS vs_, Ops ...ops)
		: Base(x_, y_, xe_, ye_, xel_, xeh_, yel_, yeh_, vc_, vs_), z(z_)
	{
		SetOptions(ops...);
	}
	template <keyword_arg ...Ops>
	void SetOptions(Ops ...ops)
	{
		Base::SetOptions(ops...);
	}

	bool IsData() const
	{
		return Base::IsData() && !PlotParamBase::IsEmptyView<Z>();
	}
	bool IsFile() const
	{
		return Base::IsFile() && PlotParamBase::IsString<Z>();
	}
	bool IsEquation() const
	{
		return Base::IsEquation() && PlotParamBase::IsEmptyView<Z>();
	}

	[[no_unique_address]] Z z;
};
template <keyword_arg ...Options>
auto MakePointParam3D(Options ...ops)
{
	ADAPT_DETAIL_MAKE_PARAM_MACRO(PointParam3D, x, y, z, xerrorbar, yerrorbar, xerrlow, xerrhigh, yerrlow, yerrhigh, variable_color, variable_size);
}

template <acceptable_arg X, acceptable_arg Y,
	acceptable_arg XL, acceptable_arg YL,
	acceptable_arg VC>
struct VectorParam : public PlotParamBase
{
	template <keyword_arg ...Ops>
	VectorParam(X x_, Y y_, XL xlen_, YL ylen_, VC vc_, Ops ...ops)
		: x(x_), y(y_), xlen(xlen_), ylen(ylen_), variable_color(vc_)
	{
		SetOptions(ops...);
	}

	template <keyword_arg ...Ops>
	void SetOptions(Ops ...ops)
	{
		SetBaseOptions(ops...);
		ADAPT_DETAIL_SET_OPTIONS_MACRO(linetype, linewidth, color, color_rgb, arrowhead, arrowfill);
	}

	bool IsData() const
	{
		return input.empty() &&
			!PlotParamBase::IsEmptyView<X>() && !PlotParamBase::IsEmptyView<Y>() &&
			!PlotParamBase::IsEmptyView<XL>() && !PlotParamBase::IsEmptyView<YL>();
	}
	bool IsFile() const
	{
		return !input.empty() &&
			PlotParamBase::IsString<X>() && PlotParamBase::IsString<Y>() &&
			PlotParamBase::IsString<XL>() && PlotParamBase::IsString<YL>();
	}
	bool IsEquation() const
	{
		return !input.empty() &&
			PlotParamBase::IsEmptyView<X>() && PlotParamBase::IsEmptyView<Y>() &&
			PlotParamBase::IsEmptyView<XL>() && PlotParamBase::IsEmptyView<YL>();
	}
	static constexpr bool HasVariableColor() { return !IsEmptyView<VC>(); }

	[[no_unique_address]] X x;
	[[no_unique_address]] Y y;
	[[no_unique_address]] XL xlen;
	[[no_unique_address]] YL ylen;

	//LineOption
	int linetype = -2;//-2ならデフォルト
	double linewidth = -1;//-1ならデフォルト、-2ならvariable
	std::vector<int> dashtype = {};
	std::string color;
	std::string color_rgb;
	[[no_unique_address]] VC variable_color;

	//VectorOption
	ArrowHead arrowhead = ArrowHead::none;//noneならデフォルト。
	ArrowFill arrowfill = ArrowFill::none;//noneならデフォルト。
};
template <keyword_arg ...Options>
auto MakeVectorParam(Options ...ops)
{
	ADAPT_DETAIL_MAKE_PARAM_MACRO(VectorParam, x, y, xlen, ylen, variable_color);
}

template <acceptable_arg X, acceptable_arg Y, acceptable_arg Z,
		  acceptable_arg XL, acceptable_arg YL, acceptable_arg ZL,
		  acceptable_arg VC>
struct VectorParam3D : public VectorParam<X, Y, XL, YL, VC>
{
	using Base = VectorParam<X, Y, XL, YL, VC>;
	template <keyword_arg ...Ops>
	VectorParam3D(X x_, Y y_, Z z_, XL xlen_, YL ylen_, ZL zlen_, VC vc_, Ops ...ops)
		: Base(x_, y_, xlen_, ylen_, vc_), z(z_), zlen(zlen_)
	{
		SetOptions(ops...);
	}
	template <keyword_arg ...Ops>
	void SetOptions(Ops ...ops)
	{
		Base::SetOptions(ops...);
	}

	bool IsData() const
	{
		return Base::IsData() && !PlotParamBase::IsEmptyView<Z>() && !PlotParamBase::IsEmptyView<ZL>();
	}
	bool IsFile() const
	{
		return Base::IsFile() && PlotParamBase::IsString<Z>() && PlotParamBase::IsString<ZL>();
	}
	bool IsEquation() const
	{
		return Base::IsEquation() && PlotParamBase::IsEmptyView<Z>() && PlotParamBase::IsEmptyView<ZL>();
	}

	[[no_unique_address]] Z z;
	[[no_unique_address]] ZL zlen;
};
template <keyword_arg ...Options>
auto MakeVectorParam3D(Options ...ops)
{
	ADAPT_DETAIL_MAKE_PARAM_MACRO(VectorParam3D, x, y, z, xlen, ylen, zlen, variable_color);
}

template <acceptable_arg X, acceptable_arg Y, acceptable_arg Y2,
	acceptable_arg VC>
struct FilledCurveParam : public PlotParamBase
{
	template <keyword_arg ...Ops>
	FilledCurveParam(X x_, Y y_, Y2 y2_, VC vc_, Ops ...ops)
		: x(x_), y(y_), ybelow(y2_), variable_color(vc_)
	{
		SetOptions(ops...);
	}

	template <keyword_arg ...Ops>
	void SetOptions(Ops ...ops)
	{
		SetBaseOptions(ops...);
		ADAPT_DETAIL_SET_OPTIONS_MACRO(style, color, baseline, fillpattern, fillsolid, filltransparent, noborder, bordercolor, bordertype, closed, above, below);
	}

	bool IsData() const
	{
		return input.empty() && !IsEmptyView<X>() && !IsEmptyView<Y>();
	}
	bool IsFile() const
	{
		return !input.empty() && IsString<X>() && IsString<Y>();
	}
	bool IsEquation() const
	{
		return !input.empty() && IsEmptyView<X>() && IsEmptyView<Y>();
	}
	static constexpr bool HasVariablecolor() { return !IsEmptyView<VC>(); }
	static constexpr bool HasYBelow() { return !IsEmptyView<Y2>(); }

	[[no_unique_address]] X x;
	[[no_unique_address]] Y y;
	[[no_unique_address]] Y2 ybelow;

	Style style = Style::lines;
	//FillOption
	std::string color;
	[[no_unique_address]] VC variable_color;
	std::string baseline;
	int fillpattern = -1;
	double fillsolid = -1.;
	bool filltransparent = false;

	//FilledCurveOption
	bool closed = false;
	bool above = false;
	bool below = false;

	//border PlotPointsのline optionとして渡される。
	bool noborder = false;
	std::string bordercolor;
	int bordertype = -2;//-2はデフォルト。
};

template <keyword_arg ...Options>
auto MakeFilledCurveParam(Options ...ops)
{
	ADAPT_DETAIL_MAKE_PARAM_MACRO(FilledCurveParam, x, y, ybelow, variable_color);
}

template <acceptable_arg X, acceptable_arg Y, acceptable_arg L, acceptable_arg VTC>
struct LabelParam : public PlotParamBase
{
	template <class X_, class Y_, class L_, class VTC_, keyword_arg ...Ops>
	LabelParam(X_&& x_, Y_&& y_, L_&& l_, VTC_&& vtc, Ops ...ops)
		: x(std::forward<X_>(x_)), y(std::forward<Y_>(y_)), label(std::forward<L_>(l_)), variable_color(std::forward<VTC_>(vtc))
	{
		SetOptions(ops...);
	}
	template <keyword_arg ...Ops>
	void SetOptions(Ops ...ops)
	{
		SetBaseOptions(ops...);
		ADAPT_DETAIL_SET_OPTIONS_MACRO(color, color_rgb, labelpos, labelrotate, noenhanced, labelfont, labeloverlay, labeloffset);
	}
	bool IsData() const
	{
		return input.empty() && !IsEmptyView<X>() && !IsEmptyView<Y>() && !IsEmptyView<L>();
	}
	bool IsFile() const
	{
		return !input.empty() && IsString<X>() && IsString<Y>() && IsString<L>();
	}
	bool IsEquation() const
	{
		return !input.empty() && IsEmptyView<X>() && IsEmptyView<Y>() && IsEmptyView<L>();
	}
	static constexpr bool HasVariableTextcolor() { return !IsEmptyView<VTC>(); }

	[[no_unique_address]] X x;
	[[no_unique_address]] Y y;
	[[no_unique_address]] L label;
	std::string color;
	std::string color_rgb;
	[[no_unique_address]] VTC variable_color;

	LabelPos labelpos = LabelPos::none;
	double labelrotate = std::numeric_limits<double>::quiet_NaN();
	bool noenhanced = false;
	std::string labelfont;
	LabelOverlay labeloverlay = LabelOverlay::none;
	std::pair<double, double> labeloffset = { 0., 0. };
};

template <keyword_arg ...Options>
auto MakeLabelParam(Options&& ...ops)
{
	static constexpr bool has_label_format = KeywordExists(plot::labelformat, ops...);
	auto fmt_str = GetKeywordArg(plot::labelformat, std::string_view{}, ops...);
	auto fmt = [fmt_str]<class View>(View&& v)
	{
		if constexpr (has_label_format)
		{
			return std::forward<View>(v) | std::views::transform([fmt_str](auto&& a)
			{
				return std::vformat(fmt_str, std::make_format_args(a));
			});
		}
		else
			return std::forward<View>(v);
	};
	auto x = AllView(GetKeywordArg(plot::x, std::ranges::empty_view<double>{}, ops...));
	auto y = AllView(GetKeywordArg(plot::y, std::ranges::empty_view<double>{}, ops...));
	auto label = AllView(fmt(GetKeywordArg(plot::label, std::ranges::empty_view<double>{}, ops...)));
	auto variable_color = AllView(GetKeywordArg(plot::variable_color, std::ranges::empty_view<double>{}, ops...));
	return LabelParam<decltype(x), decltype(y), decltype(label), decltype(variable_color)>(
		std::forward<decltype(x)>(x), std::forward<decltype(y)>(y),
		std::forward<decltype(label)>(label),
		std::forward<decltype(variable_color)>(variable_color), ops...);
}

template <acceptable_arg X, acceptable_arg Y, acceptable_arg Z, acceptable_arg L, acceptable_arg VTC>
struct LabelParam3D : public LabelParam<X, Y, L, VTC>
{
	using Base = LabelParam<X, Y, L, VTC>;
	template <keyword_arg ...Ops>
	LabelParam3D(X x_, Y y_, Z z_, L l_, VTC vtc, Ops ...ops)
		: Base(x_, y_, l_, vtc), z(z_)
	{
		SetOptions(ops...);
	}
	template <keyword_arg ...Ops>
	void SetOptions(Ops ...ops)
	{
		Base::SetOptions(ops...);
	}
	bool IsData() const
	{
		return Base::IsData() && !PlotParamBase::IsEmptyView<Z>();
	}
	bool IsFile() const
	{
		return Base::IsFile() && PlotParamBase::IsString<Z>();
	}
	bool IsEquation() const
	{
		return Base::IsEquation() && PlotParamBase::IsEmptyView<Z>();
	}
	[[no_unique_address]] Z z;
};

template <keyword_arg ...Options>
auto MakeLabelParam3D(Options ...ops)
{
	ADAPT_DETAIL_MAKE_PARAM_MACRO(LabelParam3D, x, y, z, label, variable_color);
}

// Gnuplotでカラーマップを作成する際、
// corners2color c1を指定しているためx、y座標は矩形の左下座標をファイルに出力する必要がある。
// 一方で等高線を書くとき、矩形の中央の座標も参照する必要がある。
// そのため、常に左下と中央の2個の座標を出力するようなイテレータを用意しておく。
template <ranges::arithmetic_range Range>
struct CoordRange
{
	using It = std::ranges::iterator_t<Range>;
	using Sen = std::ranges::sentinel_t<Range>;

	struct sentinel
	{
		bool operator==(const sentinel&) const { return true; }
	};
	struct iterator
	{
		iterator(It&& it_, Sen&& end_)
			: it(std::forward<It>(it_)), end(std::forward<Sen>(end_)), min(0.), center(0.), flag(-1)
		{
			if (it == end)
				throw InvalidArg("The number of datapoints for colormap must be greater than 1.");
			double curr = *it;
			++it;
			if (it == end)
				throw InvalidArg("The number of datapoints for colormap must be greater than 1.");
			double next = *it;
			min = curr - (next - curr) / 2.;
			center = curr;
		}
		void operator++()
		{
			if (flag == 1) throw NoElements();//これが投げられたらバグ。
			if (it == end)
			{
				//xは常にひとつ先の座標を指している。
				//よって、このスコープは最後の、つまりMatrixの端のさらに一つ次の座標へ移動しようとしている。
				flag = 1;
				double prev = center;
				double w_per_2 = center - min;
				min = center + w_per_2;
				center = prev + w_per_2 * 2;
			}
			else
			{
				++it;
				flag = 0;
				double prev = center;
				double curr = *it;
				min = (curr + prev) / 2.;
				center = curr;
			}
		}
		bool operator==(const sentinel&) const { return flag == 1; }
		std::pair<double, double> operator*() const
		{
			return { min, center };
		}

	private:
		It it;
		Sen end;
		double min, center;
		int flag;//-1:first, 0:middle, 1:last
	};
	template <ranges::arithmetic_range Range_>
	CoordRange(const Range_& r)
		: x(std::views::all(r))
	{}
	iterator begin() const { return iterator(x.begin(), x.end()); }
	sentinel end() const { return sentinel{}; }

	using View = decltype(std::views::all(std::declval<Range>()));
	[[no_unique_address]] View x;
};
struct CoordMinMax
{
	struct iterator
	{
		iterator() : current(0), parent(nullptr) {}
		iterator(const CoordMinMax& p, size_t pos = 0)
			: current(pos), parent(&p) {}
		iterator(const iterator& i) = default;
		iterator& operator=(const iterator& i) = default;

		iterator& operator++() { ++current; return *this; }
		iterator operator++(int) { iterator tmp(*this); ++current; return tmp; }
		iterator& operator--() { --current; return *this; }
		iterator operator--(int) { iterator tmp(*this); --current; return tmp; }
		
		bool operator==(const iterator& i) const { return current == i.current; }
		bool operator!=(const iterator& i) const { return current != i.current; }

		std::pair<double, double> operator*() const
		{
			return std::make_pair(parent->min + current * parent->width,
								  parent->cmin + current * parent->width);
		}
	private:
		size_t current;
		const CoordMinMax* parent;
	};
	CoordMinMax(std::pair<double, double> r, size_t s)
	{
		size = s;
		width = (r.second - r.first) / double(size - 1);
		min = r.first - width / 2.;
		cmin = r.first;
	}
	iterator begin() const { return iterator(*this); }
	iterator end() const { return iterator(*this, size); }
	double min;
	double cmin;
	size_t size;
	double width;
};

template <class Map, plot_detail::colormap_option ...Ops>
auto MakeXCoordRange([[maybe_unused]] const Map& map, Ops ...ops)
{
	if constexpr (KeywordExists(plot::xrange, ops...))
	{
		auto x = GetKeywordArg(plot::xrange, ops...);
		return CoordRange(x);
	}
	else if constexpr (KeywordExists(plot::xminmax, ops...))
	{
		size_t xsize = map.size();
		auto x = GetKeywordArg(plot::xminmax, ops...);
		return CoordMinMax(x, xsize);
	}
	else
	{
		throw InvalidArg("xcoord or xrange must be specified.");
	}
}
template <class Map, plot_detail::colormap_option ...Ops>
auto MakeYCoordRange([[maybe_unused]] const Map& map, Ops ...ops)
{
	if constexpr (KeywordExists(plot::yrange, ops...))
	{
		auto y = GetKeywordArg(plot::yrange, ops...);
		return CoordRange(y);
	}
	else if constexpr (KeywordExists(plot::yminmax, ops...))
	{
		size_t ysize = map.begin()->size();
		auto y = GetKeywordArg(plot::yminmax, ops...);
		return CoordMinMax(y, ysize);
	}
	else
	{
		throw InvalidArg("xcoord or xrange must be specified.");
	}
}

template <acceptable_matrix_range Map, ranges::arithmetic_range XRange, ranges::arithmetic_range YRange>
struct HeatmapParam : PlotParamBase
{
	template <keyword_arg ...Ops>
	HeatmapParam(Map map_, XRange xrange_, YRange yrange_,
				  std::pair<double, double> xminmax_, std::pair<double, double> yminmax_,
				  Ops ...ops)
		: map(map_), xrange(xrange_), yrange(yrange_), xminmax(xminmax_), yminmax(yminmax_)
	{
		SetOptions(ops...);
	}

	template <keyword_arg ...Ops>
	void SetOptions(Ops ...ops)
	{
		SetBaseOptions(ops...);
		ADAPT_DETAIL_SET_OPTIONS_MACRO(with_contour, without_surface,
								cntrsmooth, cntrpoints, cntrorder, cntrlevels_auto, cntrlevels_discrete, cntrlevels_incremental,
								cntrcolor, variable_cntrcolor, cntrlinetype, cntrlinewidth);
	}

	bool IsXRangeAssigned() const { return !IsEmptyView<XRange>(); }
	bool IsYRangeAssigned() const { return !IsEmptyView<YRange>(); }
	bool IsXMinMaxAssigned() const { return xminmax.first != 0 || xminmax.second != 0; }
	bool IsYMinMaxAssigned() const { return yminmax.first != 0 || yminmax.second != 0; }
	bool IsXAssigned() const { return IsXRangeAssigned() || IsXMinMaxAssigned(); }
	bool IsYAssigned() const { return IsYRangeAssigned() || IsYMinMaxAssigned(); }

	bool IsData() const
	{
		return input.empty() && !IsEmptyView<Map>() && IsXAssigned() && IsYAssigned();
	}
	bool IsFile() const
	{
		return !input.empty() && IsString<Map>() && IsString<XRange>() && IsString<YRange>();
	}
	bool IsEquation() const
	{
		return !input.empty() && IsEmptyView<Map>() && !IsXAssigned() && !IsYAssigned();
	}

	//ColormapOption
	//mapはrangeよりも先に宣言しておくこと。
	//コンストラクタの初期化で必ずmapを先に評価することで、rangeの初期化にmapの値を用いることができる。
	[[no_unique_address]] Map map;
	[[no_unique_address]] XRange xrange;
	[[no_unique_address]] YRange yrange;
	std::pair<double, double> xminmax;
	std::pair<double, double> yminmax;

	bool with_contour = false;
	bool without_surface = false;
	CntrSmooth cntrsmooth = CntrSmooth::none;
	int cntrpoints = -1;
	int cntrorder = -1;
	int cntrlevels_auto = -1;
	std::vector<double> cntrlevels_discrete{};
	std::tuple<double, double, double> cntrlevels_incremental = { 0, 0, 0 };
	std::string cntrcolor;
	bool variable_cntrcolor = false;
	int cntrlinetype = -2;
	double cntrlinewidth = -1;
};
template <keyword_arg ...Options>
auto MakeHeatmapParam(Options ...ops)
{
	auto map = AllView(GetKeywordArg(plot::map, std::ranges::empty_view<std::ranges::empty_view<double>>{}, ops...));
	auto xrange = AllView(GetKeywordArg(plot::xrange, std::ranges::empty_view<double>{}, ops...));
	auto yrange = AllView(GetKeywordArg(plot::yrange, std::ranges::empty_view<double>{}, ops...));
	auto xminmax = GetKeywordArg(plot::xminmax, std::pair<double, double>{ 0, 0 }, ops...);
	auto yminmax = GetKeywordArg(plot::yminmax, std::pair<double, double>{ 0, 0 }, ops...);
	return HeatmapParam<decltype(map), decltype(xrange), decltype(yrange)>
		(map, xrange, yrange, xminmax, yminmax, ops...);
}
template <acceptable_matrix_range Map, class X, class Y, keyword_arg ...Options>
auto MakeAnnotParam(const Map& map, const X& x, const Y& y, Options ...ops)
{
	if constexpr (KeywordExists(plot::annot_detail, ops...))
	{
		std::vector<double> xvec, yvec;
		size_t size = map.size() * map.begin()->size();
		xvec.reserve(size);
		yvec.reserve(size);
		for (auto vx : x)
		{
			for (auto vy : y)
			{
				// vx、vyはstd::pair<double, double>であり、firstが左下座標、secondが中央座標なので、
				// ここでは中央座標を使う。
				xvec.push_back(vx.second);
				yvec.push_back(vy.second);
			}
		}
		auto f = []<class Map_, class X_, class Y_>(Map_&& m, X_&& x_, Y_&& y_, auto&&... args)
		{
			return MakeLabelParam(plot::label = std::forward<Map_>(m),
								  plot::x = std::forward<X_>(x_),
								  plot::y = std::forward<Y_>(y_),
								  args...);
		};

		auto annot_opt = GetKeywordArg(plot::annot_detail, ops...);
		return std::apply(
			f,
			TupleCatForward(std::forward_as_tuple(map.GetFlatRange(), std::move(xvec), std::move(yvec)),
							std::move(annot_opt.options)));
	}
	else return EmptyClass{};
}

using EmptyPointParam = PointParam<
	std::ranges::empty_view<double>,
	std::ranges::empty_view<double>,
	std::ranges::empty_view<double>,
	std::ranges::empty_view<double>,
	std::ranges::empty_view<double>,
	std::ranges::empty_view<double>,
	std::ranges::empty_view<double>,
	std::ranges::empty_view<double>,
	std::ranges::empty_view<double>,
	std::ranges::empty_view<double>>;

template <acceptable_matrix_range Z, ranges::arithmetic_range XRange, ranges::arithmetic_range YRange,
		  acceptable_matrix_range VC, acceptable_matrix_range VS>
struct SurfaceParam : public EmptyPointParam
{
private:
	static std::ranges::empty_view<double> ev() { return {}; }
public:
	using Base = EmptyPointParam;
	template <keyword_arg ...Ops>
	SurfaceParam(Z z_, XRange xrange_, YRange yrange_,
				 std::pair<double, double> xminmax_, std::pair<double, double> yminmax_,
				 VC vc_, VS vs_, Ops ...ops)
		: Base(ev(), ev(), ev(), ev(), ev(), ev(), ev(), ev(), ev(), ev()),
		z(z_), xrange(xrange_), yrange(yrange_), xminmax(xminmax_), yminmax(yminmax_),
		variable_color(vc_), variable_size(vs_)
	{
		SetOptions(ops...);
	}

	template <keyword_arg ...Ops>
	void SetOptions(Ops ...ops)
	{
		style = Style::lines;//デフォルトではlinesだが、Base::SetOptionsでstyleが変更される可能性がある。
		Base::SetOptions(ops...);
		ADAPT_DETAIL_SET_OPTIONS_MACRO(pm3d_at,
									   with_contour, without_surface,
									   cntrsmooth, cntrpoints, cntrorder, cntrlevels_auto, cntrlevels_discrete, cntrlevels_incremental,
									   cntrcolor, variable_cntrcolor, cntrlinetype, cntrlinewidth);
		if (pm3d_at != Pm3dPosition::none) style = Style::pm3d;
	}

	bool IsXRangeAssigned() const { return !IsEmptyView<XRange>(); }
	bool IsYRangeAssigned() const { return !IsEmptyView<YRange>(); }
	bool IsXMinMaxAssigned() const { return xminmax.first != 0 || xminmax.second != 0; }
	bool IsYMinMaxAssigned() const { return yminmax.first != 0 || yminmax.second != 0; }
	bool IsXAssigned() const { return IsXRangeAssigned() || IsXMinMaxAssigned(); }
	bool IsYAssigned() const { return IsYRangeAssigned() || IsYMinMaxAssigned(); }

	static constexpr bool HasVariableColor() { return !IsEmptyView<VC>(); }
	static constexpr bool HasVariableSize() { return !IsEmptyView<VS>(); }

	bool IsData() const
	{
		return input.empty() && !IsEmptyView<Z>() && IsXAssigned() && IsYAssigned();
	}
	bool IsFile() const
	{
		return !input.empty() && IsString<Z>() && IsString<XRange>() && IsString<YRange>();
	}
	bool IsEquation() const
	{
		return !input.empty() && IsEmptyView<Z>() && !IsXAssigned() && !IsYAssigned();
	}

	[[no_unique_address]] Z z;
	[[no_unique_address]] XRange xrange;
	[[no_unique_address]] YRange yrange;
	std::pair<double, double> xminmax;
	std::pair<double, double> yminmax;
	[[no_unique_address]] VC variable_color;//基底クラスの同盟メンバ変数を隠蔽している。
	[[no_unique_address]] VS variable_size;//基底クラスの同盟メンバ変数を隠蔽している。

	Pm3dPosition pm3d_at = Pm3dPosition::none;//これが指定された場合、styleは強制的にpm3dになる。

	bool with_contour = false;
	bool without_surface = false;
	CntrSmooth cntrsmooth = CntrSmooth::none;
	int cntrpoints = -1;
	int cntrorder = -1;
	int cntrlevels_auto = -1;
	std::vector<double> cntrlevels_discrete{};
	std::tuple<double, double, double> cntrlevels_incremental = { 0, 0, 0 };
	std::string cntrcolor;
	bool variable_cntrcolor = false;
	int cntrlinetype = -2;
	double cntrlinewidth = -1;
};
template <keyword_arg ...Options>
auto MakeSurfaceParam(Options ...ops)
{
	auto z = AllView(GetKeywordArg(plot::map, std::ranges::empty_view<std::ranges::empty_view<double>>{}, ops...));
	auto xrange = AllView(GetKeywordArg(plot::xrange, std::ranges::empty_view<double>{}, ops...));
	auto yrange = AllView(GetKeywordArg(plot::yrange, std::ranges::empty_view<double>{}, ops...));
	auto xminmax = GetKeywordArg(plot::xminmax, std::pair<double, double>{ 0, 0 }, ops...);
	auto yminmax = GetKeywordArg(plot::yminmax, std::pair<double, double>{ 0, 0 }, ops...);
	auto vc = GetKeywordArg(plot::variable_color, std::ranges::empty_view<std::ranges::empty_view<double>>{}, ops...);
	auto vs = GetKeywordArg(plot::variable_size, std::ranges::empty_view<std::ranges::empty_view<double>>{}, ops...);
	return SurfaceParam<decltype(z), decltype(xrange), decltype(yrange), decltype(vc), decltype(vs)>
		(z, xrange, yrange, xminmax, yminmax, vc, vs, ops...);
}

template <ranges::arithmetic_range Data, acceptable_arg Weight>
struct HistogramParam
{
	template <keyword_arg ...Ops>
	HistogramParam(Data data, double xmin, double xmax, size_t xnbin, Weight weight, Ops ...ops)
		: data(data), xmin(xmin), xmax(xmax), xnbin(xnbin), weight(weight)
	{
		SetOptions(ops...);
	}

	template <keyword_arg ...Ops>
	void SetOptions(Ops ...ops)
	{
		ADAPT_DETAIL_SET_OPTIONS_MACRO(binerror, cumul, inv_cumul, stack);
	}
	Data data;
	double xmin;
	double xmax;
	size_t xnbin;
	BinError binerror = BinError::none;
	bool cumul = false;
	bool inv_cumul = false;
	bool stack = false;
	[[no_unique_address]] Weight weight;
};
template <keyword_arg ...Options>
auto MakeHistogramParam(Options ...ops)
{
	auto data = AllView(GetKeywordArg(plot::data, ops...));
	auto xmin = GetKeywordArg(plot::min, ops...);
	auto xmax = GetKeywordArg(plot::max, ops...);
	auto xnbin = GetKeywordArg(plot::nbin, ops...);
	auto weight = AllView(GetKeywordArg(plot::weight, std::ranges::empty_view<double>{}, ops...));
	return HistogramParam<decltype(data), decltype(weight)>(data, xmin, xmax, xnbin, weight, ops...);
}

template <ranges::arithmetic_range X, ranges::arithmetic_range Y, acceptable_arg Weight>
struct BinscatterParam
{
	template <keyword_arg ...Ops>
	BinscatterParam(X x, double xmin, double xmax, size_t xnbin,
					Y y, double ymin, double ymax, size_t ynbin,
					Weight weight, Ops ...ops)
		: x(x), xmin(xmin), xmax(xmax), xnbin(xnbin), y(y), ymin(ymin), ymax(ymax), ynbin(ynbin), weight(weight)
	{
		SetOptions(ops...);
	}

	template <keyword_arg ...Ops>
	void SetOptions(Ops ...ops)
	{
		if constexpr (KeywordExists(plot::bs_points, ops...)) bs_points = GetKeywordArg(plot::bs_points, ops...);
		if constexpr (KeywordExists(plot::bs_lower, ops...)) bs_lower = GetKeywordArg(plot::bs_lower, ops...);
		if constexpr (KeywordExists(plot::bs_upper, ops...)) bs_upper = GetKeywordArg(plot::bs_upper, ops...);
	}
	X x;
	double xmin;
	double xmax;
	size_t xnbin;
	Y y;
	double ymin;
	double ymax;
	size_t ynbin;

	bool bs_points = false;
	double bs_lower = 0.0;
	double bs_upper = std::numeric_limits<double>::max();

	[[no_unique_address]] Weight weight;
};
template <keyword_arg ...Options>
auto MakeBinscatterParam(Options ...ops)
{
	auto x = AllView(GetKeywordArg(plot::datax, ops...));
	auto xmin = GetKeywordArg(plot::xmin, ops...);
	auto xmax = GetKeywordArg(plot::xmax, ops...);
	auto xnbin = GetKeywordArg(plot::xnbin, ops...);
	auto y = AllView(GetKeywordArg(plot::datay, ops...));
	auto ymin = GetKeywordArg(plot::ymin, ops...);
	auto ymax = GetKeywordArg(plot::ymax, ops...);
	auto ynbin = GetKeywordArg(plot::ynbin, ops...);
	auto weight = AllView(GetKeywordArg(plot::weight, std::ranges::empty_view<double>{}, ops...));
	return BinscatterParam<decltype(x), decltype(y), decltype(weight)>(x, xmin, xmax, xnbin, y, ymin, ymax, ynbin, weight, ops...);
}

#undef ADAPT_DETAIL_GET_KEYWORD_ARG_AS_VIEW
#undef ADAPT_DETAIL_DECLTYPE_AUTO
#undef ADAPT_DETAIL_FORWARD_ARG
#undef ADAPT_DETAIL_MAKE_PARAM_MACRO
#undef ADAPT_DETAIL_GET_KEYWORD_ARG_IF_EXIST
#undef ADAPT_DETAIL_SET_OPTIONS_MACRO


template <class Range>
decltype(auto) ConvertUniqueToRange(Range&& range)
{
	//arithmeticの場合、rangeではないのでそのままではviews::Zipに渡せない。
	//そのため、rangeに変換する。
	//C++20にはrepeat_viewがないので、iota + transformで代用する。
	if constexpr (arithmetic<std::decay_t<Range>>)
		return std::views::iota(0) | std::views::transform([range](int) { return range; });
	else
		return std::forward<Range>(range);
}
template <class Range, class It, class Sen>
bool IsEnd(const It& it, [[maybe_unused]] const Sen& end)
{
	//arithmeticの場合は無限要素のrangeになっているので、そもそもendに到達しない。
	//よって代わりに常にtrueを返す。
	if constexpr (arithmetic<std::decay_t<Range>>)
		return true;
	else
		return it == end;
}
template <class ...Ranges, class It, class Sen, size_t ...Indices>
bool IsAllEnd(TypeList<Ranges...>, const It& it, const Sen& end, std::index_sequence<Indices...>)
{
	return (IsEnd<Ranges>(it.template GetIteratorAt<Indices>(), end.template GetSentinelAt<Indices>()) && ...);
}

template <class Stream, class ...Ranges>
auto MakeDataObject(Stream& stream, std::tuple<Ranges...> ranges)
{
	auto zipped = std::apply([]<class ...R>(R&& ...r) { return views::Zip(ConvertUniqueToRange(std::forward<R>(r))...); }, std::move(ranges));
	auto&& it = zipped.begin();
	auto&& end = zipped.end();
	for (; it != end; ++it)
	{
		//print::quote<true>を指定してstd::stringに引用符を付与すると
		//xyticに文字列を使うときにスペースなどに対応できるので便利ではあるが、
		//今度はdatetimeのカラムを認識してくれなくなるらしい。
		//仕方ないので、スペースを含む文字列は自前で引用符をつけてもらうことにする。
		std::apply([&stream](auto&& ...args) { adapt::Print(stream, args...); }, *it);
	}
	if (!IsAllEnd(TypeList<Ranges...>{}, it, end, std::make_index_sequence<sizeof...(Ranges)>{}))
	{
		PrintWarning("WARNING : The numbers of datapoints are inconsistent. The trailing data points are truncated to fit the smallest data set.");
	}
}

template <class Stream, acceptable_matrix_range MatRange, class RangeX, class RangeY>
void MakeDataObject(Stream& stream, const MatRange& mat, const RangeX& rx, const RangeY& ry)
{
	auto mitx = mat.begin();
	auto itx = rx.begin();
	for (; itx != rx.end(); ++itx)
	{
		auto mity = mitx->begin();
		auto ity = ry.begin();
		auto [x, cx] = *itx;
		for (; ity != ry.end(); ++ity)
		{
			if (mity == mitx->end())
				throw InvalidArg("The number of the datapoints are inconsistent with the coordinates.");
			auto [y, cy] = *ity;
			adapt::Print(stream, x, y, cx, cy, *mity);
			++mity;
		}
		//この時点でityはry.end()に到達しているが、CoordMinMax/CoordRangeクラスのend状態は*ityで値を取得できるので問題ない。
		//auto [y, cy] = *ity;
		//adapt::Print(stream, x, y, cx, cy, " 0\n");
		adapt::Print(stream);
		++mitx;
	}
	//auto [x, cx] = *itx;
	//auto ity = ry.begin();
	//for (; ity != ry.end(); ++ity)
	//{
	//	auto [y, cy] = *ity;
	//	adapt::Print(stream, x, y, cx, cy, " 0");
	//}
	//auto [y, cy] = *ity;
	//adapt::Print(stream, x, y, cx, cy, " 0");
}
template <class Canvas_, class ...Args>
inline auto MakeDataObject(Canvas_* g, const std::string& name, Args&& ...args)
{
	if (g->IsInMemoryDataTransferEnabled())
	{
		// make datablock
		g->Command(name + " << EOD");
		FILE* pipe = g->GetPipe();
		MakeDataObject(pipe, std::forward<Args>(args)...);
		g->Command("EOD");
	}
	else
	{
		// make file
		std::ofstream ofs(name);
		if (!ofs) throw InvalidArg("file \"" + name + "\" cannot open.");
		MakeDataObject(ofs, std::forward<Args>(args)...);
	}
}

// Replace non-alphanumeric characters with '_'
inline std::string SanitizeForDataBlock(const std::string& str)
{
	std::string res = str;
	auto pos = res.begin();
	while (pos = std::find_if(pos, res.end(), [](char c) { return !isalnum(c); }), pos != res.end())
	{
		res.replace(pos, pos + 1, 1, '_');
		pos++;
	}
	return res;
}

//optionsからusingに使うrangeを取り出してstd::tupleにまとめて返す。
//また、columnとlabelcolumnに必要な情報を追加する。
template <size_t I, int Count, size_t N, class Canvas_>
auto ArrangeColumnOption(std::map<std::string, std::variant<int, std::string>>&, std::vector<std::string>&, Canvas_*,
						 const std::array<std::string, N>&,
						 [[maybe_unused]] std::array<std::string_view, N>)
{
	return std::tuple<>{};
}
template <size_t I, int Count, size_t N, class Canvas_, class Range, class ...Ranges>
auto ArrangeColumnOption(std::map<std::string, std::variant<int, std::string>>& cols,
						 std::vector<std::string>& labelcols, Canvas_* canvas,
						 const std::array<std::string, N>& names,
						 [[maybe_unused]] std::array<std::string_view, N> axes,
						 Range&& range, Ranges&& ...ranges)
{
	if constexpr (!PlotParamBase::IsEmptyView<Range>())
	{
		using DecayedType = std::decay_t<Range>;
		//普通のstd::vector<double>のような数値リストか、さもなくば固定値の場合。
		if constexpr (ranges::arithmetic_range<DecayedType>)
		{
			cols[names[I]] = Count;
			return std::tuple_cat(
				std::forward_as_tuple(std::forward<Range>(range)),
				ArrangeColumnOption<I + 1, Count + 1>(cols, labelcols, canvas, names, axes, std::forward<Ranges>(ranges)...));
		}
		else if constexpr (arithmetic<DecayedType>)
		{
			cols[names[I]] = std::format("$0*0+{}", range);
			return ArrangeColumnOption<I + 1, Count>(cols, labelcols, canvas, names, axes, std::forward<Ranges>(ranges)...);
		}
		//std::vector<std::string>のような文字列リストの場合。
		else if constexpr (ranges::string_range<DecayedType>)
		{
			std::string_view axis = axes[I];
			if (canvas->IsDateTimeEnabled(axis) || axis.empty()) cols[names[I]] = Count;
			else
			{
				//axisに有効値が入っている場合、これはxyz軸などのラベルを指定している。
				cols[names[I]] = "$0";
				labelcols.push_back(std::format("{}tic({})", axis, Count));
			}
			return std::tuple_cat(
				std::forward_as_tuple(std::forward<Range>(range)),
				ArrangeColumnOption<I + 1, Count + 1>(cols, labelcols, canvas, names, axes, std::forward<Ranges>(ranges)...));
		}
		//これらはrangeではないので、戻り値に加える必要はない。
		else if constexpr (std::convertible_to<DecayedType, std::string_view>)
		{
			cols[names[I]] = range;
			return ArrangeColumnOption<I + 1, Count>(cols, labelcols, canvas, names, axes, std::forward<Ranges>(ranges)...);
		}
	}
	else
	{
		return ArrangeColumnOption<I + 1, Count>(cols, labelcols, canvas, names, axes, std::forward<Ranges>(ranges)...);
	}
}

template <class Var>
void AddColumn(Var&& var, std::string_view name, std::map<std::string, std::variant<int, std::string>>& column)
{
	if constexpr (std::convertible_to<Var, std::string_view>)
	{
		//operator[]、insertなどにhomogeneous overloadはまだ使えない。C++23以降。
		std::string_view str = var;
		column[std::string(name.begin(), name.end())] = std::string(str.begin(), str.end());
	}
	else if constexpr (std::same_as<std::decay_t<Var>, std::ranges::empty_view<double>>);//empty_viewなら何もしない。
	else throw InvalidArg(std::format("{} for the file plot mode must be given in the form of the string column.", name));
}

inline std::pair<std::string, std::string> GetAxes2D(const PlotParamBase& p)
{
	std::string x_x2 = p.axis.find("x2") != std::string::npos ? "x2" : "x";
	std::string y_y2 = p.axis.find("y2") != std::string::npos ? "y2" : "y";
	return std::make_pair(x_x2, y_y2);
}
inline std::tuple<std::string, std::string, std::string> GetAxes3D(const PlotParamBase& p)
{
	std::string x_x2 = p.axis.find("x2") != std::string::npos ? "x2" : "x";
	std::string y_y2 = p.axis.find("y2") != std::string::npos ? "y2" : "y";
	std::string z_z2 = p.axis.find("z2") != std::string::npos ? "z2" : "z";
	return std::make_tuple(x_x2, y_y2, z_z2);
}

//こちらは単にusing a:b:cの中でa,b,cの部分を取り出すもの。intならstringに変換するが、$1のような変換は行わない。
inline std::string GetCol(const std::variant<int, std::string>& col)
{
	if (std::holds_alternative<int>(col)) return std::to_string(std::get<int>(col));
	else
	{
		const std::string& str = std::get<std::string>(col);
		if (IsIntegral(str)) return str;
		else return std::format("({})", str);
	}
}
//こちらはカラムを計算可能な形式に変換する。intなら$1のような形式に変換する。
inline std::string ConvCol(const std::variant<int, std::string>& col)
{
	if (std::holds_alternative<int>(col)) return "$" + std::to_string(std::get<int>(col));
	else
	{
		const std::string& str = std::get<std::string>(col);
		if (IsIntegral(str)) return "$" + str;
		else return str;
	}
}

template <class Param>
void MakeOutputNameCommand(bool inmemory, std::string_view output_name, const Param& p, std::string& out)
{
	if (p.IsData())
	{
		//from data
		if (inmemory)
			//variable name
			out += std::format(" {}", output_name);
		else
			//filename
			out += std::format(" '{}'", output_name);
	}
	else if (p.IsFile())
	{
		//from file
		out += std::format(" '{}'", output_name);
	}
	else if (p.IsEquation())
	{
		//from equation
		out += std::format(" {}", output_name);
	}
}

template <class Param>
void MakeTitleCommand(const Param& p, std::string& c)
{
	if (!p.title.empty())
	{
		if (p.title == "notitle") c += " notitle";
		else c += std::format(" title '{}'", p.title);
	}
}

template <bool IsLines, class Param>
void MakeErrorbarCommand(const std::map<std::string, std::variant<int, std::string>>& cols, const Param& p, std::string& c, std::string& usg)
{
	static constexpr bool xeb_assigned = Param::HasXErrorbar() || (Param::HasXErrLow() && Param::HasXErrHigh());
	static constexpr bool yeb_assigned = Param::HasYErrorbar() || (Param::HasYErrLow() && Param::HasYErrHigh());
	if constexpr (xeb_assigned && yeb_assigned)
	{
		if constexpr (IsLines) c += " with xyerrorlines";
		else c += " with xyerrorbars";
		//ややこしいが、deltaで持っているかlowhighで指定しているかで場合分けする必要がある。
		if (p.IsData() || p.IsFile())
		{
			if constexpr (Param::HasXErrorbar() && Param::HasYErrorbar()) usg += std::format(":{}:{}", GetCol(cols.at("xerrorbar")), GetCol(cols.at("yerrorbar")));
			else
			{
				//xyいずれか一方でもlowhighで指定されている場合、4カラム分の指定が必要になる。
				if constexpr (Param::HasXErrorbar()) usg += std::format(":({0}-({1})):({0}+({1}))", ConvCol(cols.at("x")), ConvCol(cols.at("xerrorbar")));
				else usg += std::format(":{}:{}", GetCol(cols.at("xerrlow")), GetCol(cols.at("xerrhigh")));
				if constexpr (Param::HasYErrorbar()) usg += std::format(":({0}-{1}):({0}+{1})", ConvCol(cols.at("y")), ConvCol(cols.at("yerrorbar")));
				else usg += std::format(":{}:{}", GetCol(cols.at("yerrlow")), GetCol(cols.at("yerrhigh")));
			}
		}
	}
	else if constexpr (xeb_assigned)
	{
		if constexpr (IsLines) c += " with xerrorlines";
		else c += " with xerrorbars";
		if (p.IsData() || p.IsFile())
		{
			if constexpr (Param::HasXErrorbar()) usg += std::format(":{}", GetCol(cols.at("xerrorbar")));
			else usg += std::format(":{}:{}", GetCol(cols.at("xerrlow")), GetCol(cols.at("xerrhigh")));
		}
	}
	else
	{
		if constexpr (IsLines) c += " with yerrorlines";
		else c += " with yerrorbars";
		if (p.IsData() || p.IsFile())
		{
			if constexpr (Param::HasYErrorbar()) usg += std::format(":{}", GetCol(cols.at("yerrorbar")));
			else usg += std::format(":{}:{}", GetCol(cols.at("yerrlow")), GetCol(cols.at("yerrhigh")));
		}
	}
}
template <bool EnableVariableSize, class Param>
void MakePointCommand(const std::map<std::string, std::variant<int, std::string>>& cols, const Param& p, std::string& c, std::string& usg)
{
	if (p.pointtype != -1) c += " pointtype " + std::to_string(p.pointtype);
	if (p.pointsize != -1) c += " pointsize " + std::to_string(p.pointsize);
	else if constexpr (EnableVariableSize && Param::HasVariableSize())
	{
		c += " pointsize variable";
		if (p.IsData() || p.IsFile()) usg += std::format(":{}", GetCol(cols.at("variable_size")));
	}
}
template <class Param>
void MakeLineCommand(const std::map<std::string, std::variant<int, std::string>>&, const Param& p, std::string& c, std::string&)
{
	if (p.linetype != -2) c += " linetype " + std::to_string(p.linetype);
	if (p.linewidth != -1) c += " linewidth " + std::to_string(p.linewidth);
	if (!p.dashtype.empty())
	{
		c += " dashtype (" + std::to_string(p.dashtype.front());
		for (size_t i = 1; i < p.dashtype.size(); ++i) c += ", " + std::to_string(p.dashtype[i]);
		c += ")";
	}
}
template <bool EnableVariableColor, class Param>
void MakeColorCommand(const std::map<std::string, std::variant<int, std::string>>& cols, const Param& p, std::string& c, std::string& usg)
{
	if (!p.color.empty()) c += std::format(" linecolor '{}'", p.color);
	else if (!p.color_rgb.empty()) c += std::format(" linecolor rgb '{}'", p.color_rgb);
	else if constexpr (EnableVariableColor && Param::HasVariableColor())
	{
		c += " linecolor palette";
		if (p.IsData() || p.IsFile()) usg += std::format(":{}", GetCol(cols.at("variable_color")));
	}
}

template <class Param>
void MakeAxisCommand(const std::vector<std::string>& labelcols, const Param& p, std::string& c, std::string& usg)
{
	for (const auto& lc : labelcols) usg += std::format(":{}", lc);
	if (!p.axis.empty()) c += std::format(" axes {}", p.axis);
}

template <class Param>
	requires derived_from_template<Param, PointParam>
std::string MakePlotCommand(std::string_view output_name, bool inmemory,
							const std::map<std::string, std::variant<int, std::string>>& cols,
							const std::vector<std::string>& labelcols,
							const Param& p)
{
	constexpr bool is_3d = zaxis_param<Param>;
	constexpr bool is_surface = derived_from_template<Param, SurfaceParam>;
	constexpr bool xeb_assigned = Param::HasXErrorbar() || (Param::HasXErrLow() && Param::HasXErrHigh());
	constexpr bool yeb_assigned = Param::HasYErrorbar() || (Param::HasYErrLow() && Param::HasYErrHigh());

	static_assert(Param::HasXErrLow() == Param::HasXErrHigh(), "xerrlow and xerrhigh must be specified together.");
	static_assert(!xeb_assigned || Param::HasXErrorbar() != (Param::HasXErrLow() && Param::HasXErrHigh()), "xerrorbar and xerrlow/xerrhigh are exclusive.");
	static_assert(Param::HasYErrLow() == Param::HasYErrHigh(), "yerrlow and yerrhigh must be specified together.");
	static_assert(!yeb_assigned || Param::HasYErrorbar() != (Param::HasYErrLow() && Param::HasYErrHigh()), "yerrorbar and yerrlow/yerrhigh are exclusive.");

	std::string c;
	std::string usg;
	std::string out;

	MakeOutputNameCommand(inmemory, output_name, p, out);
	if (p.IsData() || p.IsFile())
	{
		usg += std::format(" using {}:{}", GetCol(cols.at("x")), GetCol(cols.at("y")));
		if constexpr (is_3d)
			usg += std::format(":{}", GetCol(cols.at("z")));
	}
	MakeAxisCommand(labelcols, p, c, usg);
	MakeTitleCommand(p, c);
	if constexpr (xeb_assigned || yeb_assigned)
	{
		static_assert(!is_surface, "errorbars are not allowed with surface plots.");
		if (p.style == Style::lines || p.style == Style::linespoints)
		{
			if constexpr (Param::HasVariableSize())
				PrintWarning("WARNING : errorlines is incompatible with variable size option.");
			MakeErrorbarCommand<true>(cols, p, c, usg);
			MakePointCommand<false>(cols, p, c, usg);
			MakeLineCommand(cols, p, c, usg);
			MakeColorCommand<true>(cols, p, c, usg);
		}
		else if (p.style == Style::boxes)
		{
			//boxesではエラーバーの使い方がerrorlines、errorbarsと異なるので、独立して書き下す。
			if (p.HasPointOption())
				PrintWarning("WARNING : boxes is incompatible with point option.");
			if constexpr (xeb_assigned && !yeb_assigned)
				PrintWarning("WARNING : Box style is incompatible with only xerrorbar option.");
			else if constexpr (Param::HasXErrLow() && Param::HasXErrHigh())
				PrintWarning("WARNING : Box style is incompatible with xerrlow and xerrhigh options.");
			else
			{
				c += " with boxerrorbars";
				if (p.IsData() || p.IsFile())
				{
					if constexpr (Param::HasYErrorbar()) usg += std::format(":{}", GetCol(cols.at("yerrorbar")));
					else usg += std::format(":{}:{}", GetCol(cols.at("yerrlow")), GetCol(cols.at("yerrhigh")));
					if constexpr (Param::HasXErrorbar()) usg += std::format(":{}", GetCol(cols.at("xerrorbar")));
				}
			}
			MakeLineCommand(cols, p, c, usg);
			MakeColorCommand<true>(cols, p, c, usg);
		}
		else
		{
			if (p.style != Style::points)
				PrintWarning("WARNING : Only \"lines\", \"linespoints\", \"boxes\" or \"points\" styles are allowed with errorbars.");
			if constexpr (Param::HasVariableSize())
				PrintWarning("WARNING : errorbars is incompatible with variable size option.");

			MakeErrorbarCommand<false>(cols, p, c, usg);
			MakePointCommand<false>(cols, p, c, usg);
			MakeLineCommand(cols, p, c, usg);
			MakeColorCommand<true>(cols, p, c, usg);
			//if (p.pointtype != -1) c += " pointtype " + std::to_string(p.pointtype);
			//if (p.pointsize != -1) c += " pointsize " + std::to_string(p.pointsize);
			//if (p.linewidth != -1) c += " linewidth " + std::to_string(p.linewidth);
			//if (!p.color.empty()) c += std::format(" linecolor '{}'", p.color);//pointのときも何故かlinecolorらしい。
			//else if (!p.color_rgb.empty()) c += std::format(" linecolor rgb '{}'", p.color_rgb);
			//else if constexpr (variablecolor_assigned) c += " palette";//しかしpalette指定の場合はlinecolorがいらない。謎。
		}
	}
	//エラーバー指定がない場合。
	else if (p.style == Style::pm3d)
	{
		if constexpr (!is_surface) throw InvalidArg("pm3d style is only allowed with surface plots.");
		else
		{
			c += " with pm3d";
			if (p.pm3d_at != Pm3dPosition::none)
			{
				switch (p.pm3d_at)
				{
				case Pm3dPosition::bottom: c += " at b"; break;
				case Pm3dPosition::surface: c += " at s"; break;
				case Pm3dPosition::top: c += " at t"; break;
				default: break;
				}
			}
		}
	}
	else if (p.style == Style::lines ||
			 p.style == Style::impulses ||
			 p.style == Style::steps ||
			 p.style == Style::fsteps ||
			 p.style == Style::histeps ||
			 p.style == Style::boxes)
	{
		std::string strstyle;
		std::string others;
		switch (p.style)
		{
		case Style::lines: strstyle = " with lines"; break;
		case Style::impulses: strstyle = " with impulses"; break;
		case Style::steps: strstyle = " with steps"; break;
		case Style::fsteps: strstyle = " with fsteps"; break;
		case Style::histeps: strstyle = " with histeps"; break;
		case Style::boxes: strstyle = " with boxes"; break;
		default: break;
		}
		MakeLineCommand(cols, p, others, usg);
		MakeColorCommand<true>(cols, p, others, usg);
		/*if (p.style == Style::boxes || p.style == Style::steps)
		{
			if (p.style == Style::steps) strstyle = " with fillsteps";
			//現状、fill系オプションはboxesまたはstepsにしか使えない。
			if (!p.color.empty()) others += std::format(" fillcolor '{}'", p.color);
			else if constexpr (Param::HasVariableColor())
			{
				others += " fillcolor palette";
				usg += std::format(":{}", GetCol(cols.at("variable_color")));
			}
			{
				std::string fs;
				if (p.filltransparent) fs += " transparent";
				if (p.fillsolid != -1) fs += std::format(" solid {}", p.fillsolid);
				else if (p.fillpattern != -1) fs += std::format(" pattern {}", p.fillpattern);
				if (!fs.empty()) others += " fs" + fs;
			}
			{
				std::string bd;
				if (p.bordertype == -2) bd += " noborder";
				else if (p.bordertype != -3) bd += std::format(" {}", p.bordertype);
				if (!p.bordercolor.empty()) bd += std::format(" linecolor '{}'", p.bordercolor);
				if (!bd.empty()) others += " border" + bd;
			}
		}*/
		c += strstyle;
		c += others;
	}
	else if (p.style == Style::points)
	{
		c += " with points";
		if (p.HasLineOption())
			PrintWarning("WARNING : \"points\" style is incompatible with line options.");
		MakePointCommand<true>(cols, p, c, usg);
		MakeColorCommand<true>(cols, p, c, usg);
	}
	else if (p.style == Style::linespoints)
	{
		c += " with linespoints";
		MakePointCommand<true>(cols, p, c, usg);
		MakeLineCommand(cols, p, c, usg);
		MakeColorCommand<true>(cols, p, c, usg);
	}
	else if (p.style == Style::dots)
	{
		c += " with dots";
		if (p.HasLineOption())
			PrintWarning("WARNING : \"dots\" style is incompatible with line options.");
		if (p.HasPointOption())
			PrintWarning("WARNING : \"dots\" style is incompatible with point options.");
		MakeColorCommand<true>(cols, p, c, usg);
	}
	if (p.smooth != Smooth::none)
	{
		c += " smooth";
		switch (p.smooth)
		{
		case Smooth::unique: c += " unique"; break;
		case Smooth::frequency: c += " frequency"; break;
		case Smooth::cumulative: c += " cumulative"; break;
		case Smooth::cnormal: c += " cnormal"; break;
		case Smooth::kdensity: c += " kdensity"; break;
		case Smooth::csplines: c += " csplines"; break;
		case Smooth::acsplines: c += " acsplines"; break;
		case Smooth::bezier: c += " bezier"; break;
		case Smooth::sbezier: c += " sbezier"; break;
		default: break;
		}
	}

	if (p.IsData() || p.IsFile()) return out + usg + c;
	else return out + c;
}
template <class Param>
	requires derived_from_template<Param, VectorParam>
std::string MakePlotCommand(std::string_view output_name, bool inmemory,
							const std::map<std::string, std::variant<int, std::string>>& cols,
							const std::vector<std::string>& labelcols,
							const Param& p)
{
	constexpr bool is_3d = derived_from_template<Param, VectorParam3D>;
	std::string c;
	std::string usg;
	std::string out;

	MakeOutputNameCommand(inmemory, output_name, p, out);
	MakeAxisCommand(labelcols, p, c, usg);
	MakeTitleCommand(p, c);
	c += " with vector ";
	if (p.IsData() || p.IsFile())
	{
		if constexpr (!is_3d)
			usg += std::format(" using {}:{}:{}:{}",
							   GetCol(cols.at("x")), GetCol(cols.at("y")), GetCol(cols.at("xlen")), GetCol(cols.at("ylen")));
		else
			usg += std::format(" using {}:{}:{}:{}:{}:{}",
							   GetCol(cols.at("x")), GetCol(cols.at("y")), GetCol(cols.at("z")),
							   GetCol(cols.at("xlen")), GetCol(cols.at("ylen")), GetCol(cols.at("zlen")));
	}
	if (p.arrowhead != ArrowHead::none)
	{
		if (p.arrowhead == ArrowHead::head) c += " head";
		else if (p.arrowhead == ArrowHead::heads) c += " heads";
		else if (p.arrowhead == ArrowHead::nohead) c += " nohead";
	}
	if (p.arrowfill != ArrowFill::none)
	{
		if (p.arrowfill == ArrowFill::filled) c += " filled";
		else if (p.arrowfill == ArrowFill::empty) c += " empty";
		else if (p.arrowfill == ArrowFill::nofilled) c += " nofilled";
	}
	MakeLineCommand(cols, p, c, usg);
	MakeColorCommand<true>(cols, p, c, usg);

	if (p.IsData() || p.IsFile()) return out + usg + c;
	else return out + c;
}
template <class Param>
	requires derived_from_template<Param, FilledCurveParam>
std::string MakePlotCommand(std::string_view output_name, bool inmemory,
							const std::map<std::string, std::variant<int, std::string>>& cols,
							const std::vector<std::string>& labelcols,
							const Param& p)
{
	std::string c;
	std::string usg;
	std::string out;

	MakeOutputNameCommand(inmemory, output_name, p, out);
	MakeAxisCommand(labelcols, p, c, usg);
	MakeTitleCommand(p, c);
	if (p.style == Style::boxes) c += " with boxes";
	else if (p.style == Style::steps) c += " with fillsteps";
	else
	{
		if (p.style != Style::lines)
			PrintWarning("WARNING : Only \"lines\", \"boxes\" or \"steps\" styles are allowed for filled curves.");
		c += " with filledcurves";
	}
	if (p.IsData() || p.IsFile())
	{
		usg += std::format(" using {}:{}", GetCol(cols.at("x")), GetCol(cols.at("y")));
		if constexpr (Param::HasYBelow())
			usg += std::format(":{}", GetCol(cols.at("ybelow")));
	}
	if (!p.closed && !p.above && !p.below && p.baseline.empty())
		//何も指定のないデフォルトの場合、x1軸との間の領域を塗りつぶす。
		c += " x1";

	if (p.closed) c += " closed";
	else if (p.above) c += " above";
	else if (p.below) c += " below";
	if (!p.baseline.empty()) c += " " + p.baseline;

	//std::string fillcolor = GetKeywordArg(plot::fillcolor, "", std::forward<Options>(opts)...);
	//bool variablecolor_assigned = KeywordExists(plot::variablecolor, std::forward<Options>(opts)...);
	if (!p.color.empty()) c += " fillcolor '" + p.color + "'";
	else if constexpr (Param::HasVariablecolor())
	{
		//現時点でfilledcurvesにはvariable colorを指定する方法がないらしい。
		//色々と議論があったが、ver6.0現在シンプルな方法はなかった。
		//様々なworkaroundは提案されていたので気が向いたら作るか。
		//c += " fillcolor palette z";
		//if (p.IsData() || p.IsFile()) usg += std::format(":{}", GetCol(cols.at("variable_color")));
		PrintWarning("WARNING : variable color option is not implemented for filled curves.");
	}
	{
		std::string fs;
		if (p.filltransparent) fs += " transparent";
		if (p.fillsolid != -1) fs += std::format(" solid {}", p.fillsolid);
		else if (p.fillpattern != -1) fs += std::format(" pattern {}", p.fillpattern);
		if (!fs.empty()) c += " fillstyle" + fs;
	}
	/*{
		std::string bd;
		if (p.bordertype == -2) bd += " noborder";
		else if (p.bordertype != -3) bd += std::format(" lt {}", p.bordertype);
		if (!p.bordercolor.empty()) bd += " linecolor '" + p.bordercolor + "'";
		if (!bd.empty()) c += " border" + bd;
	}*/
	//比較的新しいバージョンのGnuplotでは、
	//filledcurvesとfillstepsは仕様上、closedモード時以外にborderを付けられない。
	//クソ仕様だが仕方がないので、ボーダーは自前でPlotPoints関数に投げることにする。
	c += " noborder";

	if (p.IsData() || p.IsFile()) return out + usg + c;
	else return out + c;
}
template <class Param>
	requires derived_from_template<Param, LabelParam>
std::string MakePlotCommand(std::string_view output_name, bool inmemory,
							const std::map<std::string, std::variant<int, std::string>>& cols,
							const std::vector<std::string>& labelcols,
							const Param& p)
{
	constexpr bool is_3d = derived_from_template<Param, LabelParam3D>;
	std::string c;
	std::string usg;
	std::string out;

	MakeOutputNameCommand(inmemory, output_name, p, out);
	MakeAxisCommand(labelcols, p, c, usg);
	MakeTitleCommand(p, c);
	c += " with labels";
	if (p.IsData() || p.IsFile())
	{
		usg += std::format(" using {}:{}", GetCol(cols.at("x")), GetCol(cols.at("y")));
		if constexpr (is_3d) usg += std::format(":{}", GetCol(cols.at("z")));
		usg += std::format(":{}", GetCol(cols.at("label")));
	}

	//gnuplotの位置指定は「ラベルに対して点が右か左か」という直感と逆になっているので、ここで反転する。
	if (p.labelpos == LabelPos::center) c += " center";
	else if (p.labelpos == LabelPos::left) c += " right";
	else if (p.labelpos == LabelPos::right) c += " left";

	if (!std::isnan(p.labelrotate)) c += std::format(" rotate by {}", p.labelrotate);

	if (p.noenhanced) c += " noenhanced";

	if (!p.labelfont.empty()) c += std::format(" font \"{}\"", p.labelfont);

	if (p.labeloverlay == LabelOverlay::front) c += " front";
	else if (p.labeloverlay == LabelOverlay::back) c += " back";

	if (!p.color.empty()) c += std::format(" textcolor '{}'", p.color);
	else if (!p.color_rgb.empty()) c += std::format(" textcolor rgb '{}'", p.color_rgb);
	else if constexpr (Param::HasVariableTextcolor())
	{
		c += " textcolor palette";
		if (p.IsData() || p.IsFile()) usg += std::format(":{}", GetCol(cols.at("variable_color")));
	}

	if (p.labeloffset != std::pair{ 0., 0. }) c += std::format(" offset {}, {}", p.labeloffset.first, p.labeloffset.second);

	if (p.IsData() || p.IsFile()) return out + usg + c;
	else return out + c;
}
template <class Map, class X, class Y>
std::string MakePlotCommand(std::string_view output_name, bool inmemory,
							const std::map<std::string, std::variant<int, std::string>>& cols,
							const std::vector<std::string>& labelcols,
							const HeatmapParam<Map, X, Y>& p)
{
	std::string c;
	std::string usg;
	std::string out;

	if (!p.without_surface)
	{
		MakeOutputNameCommand(inmemory, output_name, p, out);
		MakeAxisCommand(labelcols, p, c, usg);
		MakeTitleCommand(p, c);
		c += " with image";
		usg += std::format(" using {}:{}:{}", GetCol(cols.at("x")), GetCol(cols.at("y")), GetCol(cols.at("map")));
		if (p.IsData() || p.IsFile()) c = out + usg + c;
		else c = out + c;
	}
	else
	{
		//without_surfaceの場合、以前はnosurfaceとしていたが、
		//単にプロット自体を行わないことにする。
		if (!p.with_contour) throw InvalidArg("\"without_surface\" and \"with_contour\" are mutually exclusive.");
	}

	//contourが有効の場合はこれもコマンドに追加する。
	if (p.with_contour)
	{
		if (!p.IsData()) PrintWarning("WARNING : Contour option is only available for data plot mode.");
		if (!p.without_surface) c += ", ";
		if (p.with_contour && p.IsData())
		{
			if (inmemory)
			{
				c += std::format("{}_cntr with line", output_name);
			}
			else
			{
				std::string str = std::format("'{}", output_name);
				str.erase(str.end() - 3, str.end());
				c += str + "cntr.txt' with line";
			}
			if (p.title == "notitle") c += " notitle";
			else c += " title '" + p.title + "'";
			if (p.cntrlinetype != -2) c += std::format(" linetype {}", p.cntrlinetype);
			if (p.cntrlinewidth != -1) c += std::format(" linewidth {}", p.cntrlinewidth);
			if (p.variable_cntrcolor) c += " linecolor palette";
			else if (!p.cntrcolor.empty()) c += " linecolor '" + p.cntrcolor + "'";
		}
	}
	return c;
}
template <class Param>
std::string MakeContourPlotCommand(std::string_view output_name, bool inmemory, const Param& p)
{
	std::string c = "set contour base\n";

	if (p.cntrsmooth != CntrSmooth::none)
	{
		switch (p.cntrsmooth)
		{
		case CntrSmooth::linear: c += "set cntrparam linear\n"; break;
		case CntrSmooth::cubicspline: c += "set cntrparam cubicspline\n"; break;
		case CntrSmooth::bspline: c += "set cntrparam bspline\n"; break;
		default: break;
		}
	}
	if (p.cntrpoints != -1) c += std::format("set cntrparam points {}\n", p.cntrpoints);
	if (p.cntrorder != -1) c += std::format("set cntrparam order {}\n", p.cntrorder);

	if (p.cntrlevels_auto != -1)
	{
		c += std::format("set cntrparam levels auto {}\n", p.cntrlevels_auto);
	}
	else if (!p.cntrlevels_discrete.empty())
	{
		std::string str;
		for (auto x_ : p.cntrlevels_discrete) str += std::to_string(x_) + ", ";
		str.erase(str.end() - 2, str.end());
		c += std::format("set cntrparam levels discrete {}\n", str);
	}
	else if (p.cntrlevels_incremental != std::tuple<double, double, double>{ 0, 0, 0 })
	{
		auto [start, incr, end] = p.cntrlevels_incremental;
		c += std::format("set cntrparam levels incremental {}, {}, {}\n", start, incr, end);
	}

	c += "set pm3d implicit\n";
	c += "set contour base\n";
	c += "unset surface\n";
	if (inmemory)
	{
		c += std::format("set table {}_cntr\n", output_name);
	}
	else
	{
		std::string path(output_name.begin(), output_name.end());
		path.erase(path.end() - 3, path.end());
		path += "cntr.txt";
		c += std::format("set table '{}'\n", path);
	}
	//3:4:column[2]でplotする。
	c += std::format("splot '{}' using 3:4:5", output_name);
	if (!p.title.empty())
	{
		if (p.title == "notitle") c += " notitle\n";
		else c += std::format(" title '{}'\n", p.title);
	}
	c += "unset table\n";
	c += "set surface\n";
	c += "unset contour\n";
	c += "unset pm3d\n";
	return c;
}
template <class Param>
std::string MakePlotCommandCommon(bool inmemory,
								  std::string_view output_name,
								  const std::map<std::string, std::variant<int, std::string>>& cols,
								  const std::vector<std::string>& labelcols,
								  const Param& p)
{
	//point、vector、filledcurveなどへ分岐
	return MakePlotCommand(output_name, inmemory, cols, labelcols, p);
}

}

}

#endif