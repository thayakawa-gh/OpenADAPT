#ifndef ADAPT_PLOT_PLOTCOMMAND_H
#define ADAPT_PLOT_PLOTCOMMAND_H

#include <OpenADAPT/Utility/Verbose.h>
#include <OpenADAPT/Utility/Matrix.h>
#include <OpenADAPT/Utility/Function.h>
#include <OpenADAPT/Utility/KeywordArgs.h>
#include <OpenADAPT/Utility/Ranges.h>
#include <OpenADAPT/Plot/Core.h>
#include <OpenADAPT/Plot/Axes.h>

namespace adapt
{


namespace plot_detail
{



// 1. 数値型のrange
// 2. 文字列型のrange
// 3. 単一の数値
// 4. 単一の文字列（カラム指定として認識する）
// いずれかを受け付ける。
template <class Range>
concept gnuplot_acceptable_range = ranges::arithmetic_range<Range> || ranges::string_range<Range>;
template <class Type>
concept gnuplot_acceptable_arg = gnuplot_acceptable_range<Type> || arithmetic<Type> || std::convertible_to<Type, std::string_view>;

template <ranges::arithmetic_range Range>
struct ArithmeticRange {};
template <gnuplot_acceptable_arg>
struct GnuplotAcceptableArg {};

using AnyArithmeticRange = AnyTypeKeyword<ArithmeticRange>;
using AnyAcceptableArg = AnyTypeKeyword<GnuplotAcceptableArg>;
struct BaseOption {};
struct FillOption {};
struct LineOption {};
struct PointOption {};
struct VectorOption {};
struct FilledCurveOption {};
struct ColormapOption {};
struct HistogramOption {};
struct Histogram2DOption {};

template <class T>
concept base_option = keyword_arg_tagged_with<T, BaseOption>;
template <class T>
concept point_option = keyword_arg_tagged_with<T, BaseOption, LineOption, FillOption, PointOption>;
template <class T>
concept vector_option = keyword_arg_tagged_with<T, BaseOption, LineOption, VectorOption>;
template <class T>
concept filledcurve_option = keyword_arg_tagged_with<T, BaseOption, FillOption, FilledCurveOption>;

template <class Range>
concept gnuplot_acceptable_matrix_range = (ranges::arithmetic_matrix_range<Range> || std::convertible_to<Range, std::string_view>) && !ranges::string_range<Range>;
template <gnuplot_acceptable_matrix_range Range>
struct GnuplotAcceptableMatrix {};
using AnyMatrix = AnyTypeKeyword<adapt::plot_detail::GnuplotAcceptableMatrix>;

using AnyCoordRange = AnyTypeKeyword<ArithmeticRange>;

template <class Opt>
concept colormap_option = keyword_arg_tagged_with<Opt, BaseOption, ColormapOption>;

template <class Opt>
concept histogram_option = keyword_arg_tagged_with<Opt, BaseOption, HistogramOption, LineOption, PointOption>;
template <class Opt>
concept histogram2d_option = keyword_arg_tagged_with<Opt, BaseOption, Histogram2DOption, ColormapOption>;

}

namespace plot
{

ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(x, plot_detail::AnyAcceptableArg, plot_detail::BaseOption)
ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(y, plot_detail::AnyAcceptableArg, plot_detail::BaseOption)
ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(title, std::string_view, plot_detail::BaseOption)
ADAPT_DEFINE_TAGGED_KEYWORD_OPTION(t_entries, plot_detail::BaseOption)//データプロットの場合に、データ点の数をタイトルに追加する。
ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(axis, std::string_view, plot_detail::BaseOption)//y2軸を使いたい場合などに、"x1y2"のように指定する。
ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(column, std::vector<std::string>, plot_detail::BaseOption);
ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(input, std::string_view, plot_detail::BaseOption);

ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(linetype, int, plot_detail::LineOption)
ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(linewidth, double, plot_detail::LineOption)
ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(dashtype, std::vector<int>, plot_detail::LineOption)
ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(color, std::string_view, plot_detail::LineOption)
ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(color_rgb, std::string_view, plot_detail::LineOption)
ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(variable_color, plot_detail::AnyAcceptableArg, plot_detail::LineOption)

ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(style, Style, plot_detail::PointOption)
ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(pointtype, int, plot_detail::PointOption)
ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(pointsize, double, plot_detail::PointOption)
ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(smooth, Smooth, plot_detail::PointOption)
ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(xerrorbar, plot_detail::AnyAcceptableArg, plot_detail::PointOption)
ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(yerrorbar, plot_detail::AnyAcceptableArg, plot_detail::PointOption)
ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(variable_size, plot_detail::AnyAcceptableArg, plot_detail::PointOption)

ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(xlen, plot_detail::AnyAcceptableArg, plot_detail::VectorOption)
ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(ylen, plot_detail::AnyAcceptableArg, plot_detail::VectorOption)
ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(arrowhead, int, plot_detail::VectorOption)

ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(fillpattern, int, plot_detail::FillOption)
ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(fillsolid, double, plot_detail::FillOption)
ADAPT_DEFINE_TAGGED_KEYWORD_OPTION(filltransparent, plot_detail::FillOption)
ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(fillcolor, std::string_view, plot_detail::FillOption)
ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(variable_fillcolor, plot_detail::AnyAcceptableArg, plot_detail::FillOption)
ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(bordercolor, std::string_view, plot_detail::FillOption)
ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(bordertype, int, plot_detail::FillOption)//linetype

ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(ybelow, plot_detail::AnyAcceptableArg, plot_detail::FilledCurveOption)
ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(baseline, std::string_view, plot_detail::FilledCurveOption)
ADAPT_DEFINE_TAGGED_KEYWORD_OPTION(closed, plot_detail::FilledCurveOption)
ADAPT_DEFINE_TAGGED_KEYWORD_OPTION(above, plot_detail::FilledCurveOption)
ADAPT_DEFINE_TAGGED_KEYWORD_OPTION(below, plot_detail::FilledCurveOption)

ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(xmin, double, plot_detail::HistogramOption);
ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(xmax, double, plot_detail::HistogramOption);
ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(xnbin, double, plot_detail::HistogramOption);
ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(data, plot_detail::AnyArithmeticRange, plot_detail::HistogramOption);
ADAPT_DEFINE_TAGGED_KEYWORD_OPTION(witherrorbar, plot_detail::HistogramOption);
ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(fit_gauss, ADAPT_TIE_ARGS(std::pair<double, double>), plot_detail::HistogramOption);//mean、sigmaの初期値を与える。

// タイトルなし指定の短縮版
inline constexpr auto notitle = (title = "notitle");

// 軸指定の短縮版
inline constexpr auto ax_x1y1 = (axis = "x1y1");
inline constexpr auto ax_x1y2 = (axis = "x1y2");
inline constexpr auto ax_x2y1 = (axis = "x2y1");
inline constexpr auto ax_x2y2 = (axis = "x2y2");


// 線の太さ指定の短縮版
inline constexpr auto lw_ex_thick = (linewidth = 3.0);
inline constexpr auto lw_thick = (linewidth = 2.0);
inline constexpr auto lw_med_thick = (linewidth = 1.5);
inline constexpr auto lw_medium = (linewidth = 1.0);
inline constexpr auto lw_med_fine = (linewidth = 0.7);
inline constexpr auto lw_fine = (linewidth = 0.5);
inline constexpr auto lw_ex_fine = (linewidth = 0.3);

// 色指定の短縮版
// 黒、白、灰、赤、青、緑、黄、マゼンタ、シアンが基本色。黒白以外はdark、lightの指定が可能。
inline constexpr auto c_black = (color = "black");
inline constexpr auto c_dark_gray = (color = "dark-gray");
inline constexpr auto c_gray = (color = "gray");
inline constexpr auto c_light_gray = (color = "light-gray");
inline constexpr auto c_white = (color = "white");
inline constexpr auto c_dark_red = (color = "dark-red");
inline constexpr auto c_red = (color = "red");
inline constexpr auto c_light_red = (color = "light-red");
inline constexpr auto c_dark_green = (color = "dark-green");
inline constexpr auto c_green = (color = "green");
inline constexpr auto c_light_green = (color = "light-green");
inline constexpr auto c_dark_blue = (color = "dark-blue");
inline constexpr auto c_blue = (color = "blue");
inline constexpr auto c_light_blue = (color = "light-blue");
inline constexpr auto c_dark_yellow = (color = "dark-yellow");
inline constexpr auto c_yellow = (color = "yellow");
inline constexpr auto c_light_yellow = (color_rgb = "#ffffe0");
inline constexpr auto c_dark_magenta = (color = "dark-magenta");
inline constexpr auto c_magenta = (color = "magenta");
inline constexpr auto c_light_magenta = (color = "light-magenta");
inline constexpr auto c_dark_cyan = (color = "dark-cyan");
inline constexpr auto c_cyan = (color = "cyan");
inline constexpr auto c_light_cyan = (color = "light-cyan");


// スタイル指定の短縮版
inline constexpr auto s_lines = (style = Style::lines);
inline constexpr auto s_points = (style = Style::points);
inline constexpr auto s_linespoints = (style = Style::linespoints);
inline constexpr auto s_dots = (style = Style::dots);
inline constexpr auto s_impulses = (style = Style::impulses);
inline constexpr auto s_boxes = (style = Style::boxes);
inline constexpr auto s_steps = (style = Style::steps);
inline constexpr auto s_fsteps = (style = Style::fsteps);
inline constexpr auto s_histeps = (style = Style::histeps);

// ポイントタイプ指定の短縮版
inline constexpr auto pt_dot = (pointtype = 0);//dot
inline constexpr auto pt_plus = (pointtype = 1);//+
inline constexpr auto pt_cross = (pointtype = 2);//x
inline constexpr auto pt_star = (pointtype = 3);//+ and x
inline constexpr auto pt_box = (pointtype = 4);//□
inline constexpr auto pt_fbox = (pointtype = 5);//■
inline constexpr auto pt_cir = (pointtype = 6);//○
inline constexpr auto pt_fcir = (pointtype = 7);//●
inline constexpr auto pt_tri = (pointtype = 8);//△
inline constexpr auto pt_ftri = (pointtype = 9);//▲
inline constexpr auto pt_dtri = (pointtype = 10);//▽
inline constexpr auto pt_fdtri = (pointtype = 11);//▼
inline constexpr auto pt_dia = (pointtype = 12);//◇
inline constexpr auto pt_fdia = (pointtype = 13);//♦
inline constexpr auto pt_pent = (pointtype = 14);//⬠
inline constexpr auto pt_fpent = (pointtype = 15);//⬟

// ポイントサイズ指定の短縮版
inline constexpr auto ps_ex_large = (pointsize = 3.0);
inline constexpr auto ps_large = (pointsize = 2.0);
inline constexpr auto ps_med_large = (pointsize = 1.5);
inline constexpr auto ps_medium = (pointsize = 1.0);
inline constexpr auto ps_med_small = (pointsize = 0.7);
inline constexpr auto ps_small = (pointsize = 0.5);
inline constexpr auto ps_ex_small = (pointsize = 0.3);

ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(z, plot_detail::AnyAcceptableArg, plot_detail::BaseOption)
ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(zerrorbar, plot_detail::AnyAcceptableArg, plot_detail::PointOption)
ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(zlen, plot_detail::AnyAcceptableArg, plot_detail::VectorOption)

ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(map, plot_detail::AnyMatrix, plot_detail::ColormapOption)
ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(xrange, plot_detail::AnyCoordRange, plot_detail::ColormapOption)
ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(yrange, plot_detail::AnyCoordRange, plot_detail::ColormapOption)
ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(xminmax, ADAPT_TIE_ARGS(std::pair<double, double>), plot_detail::ColormapOption)
ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(yminmax, ADAPT_TIE_ARGS(std::pair<double, double>), plot_detail::ColormapOption)

//options for contour plot
ADAPT_DEFINE_TAGGED_KEYWORD_OPTION(with_contour, plot_detail::ColormapOption)
ADAPT_DEFINE_TAGGED_KEYWORD_OPTION(without_surface, plot_detail::ColormapOption)
ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(cntrsmooth, CntrSmooth, plot_detail::ColormapOption)
ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(cntrpoints, int, plot_detail::ColormapOption)//the number of lines for cspline and bspline
ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(cntrorder, int, plot_detail::ColormapOption)//order for bspline, [2, 10]
ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(cntrlevels_auto, int, plot_detail::ColormapOption)
ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(cntrlevels_discrete, const std::vector<double>&, plot_detail::ColormapOption)
ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(cntrlevels_incremental, ADAPT_TIE_ARGS(std::tuple<double, double, double>), plot_detail::ColormapOption)
ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(cntrcolor, std::string_view, plot_detail::ColormapOption)
ADAPT_DEFINE_TAGGED_KEYWORD_OPTION(variable_cntrcolor, plot_detail::ColormapOption)
ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(cntrlinetype, int, plot_detail::ColormapOption)
ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(cntrlinewidth, double, plot_detail::ColormapOption)

}

namespace plot_detail
{

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
		if constexpr (KeywordExists(plot::input, ops...)) input = GetKeywordArg(plot::input, ops...);
		if constexpr (KeywordExists(plot::title, ops...)) title = GetKeywordArg(plot::title, ops...);
		if constexpr (KeywordExists(plot::axis, ops...)) axis = GetKeywordArg(plot::axis, ops...);
		if constexpr (KeywordExists(plot::column, ops...)) column = GetKeywordArg(plot::column, ops...);
	}

	std::string input;//データプロットなら空、関数プロットなら関数、テキストプロットならファイル名

	//Baseoption
	std::string title;
	std::string axis;

	std::vector<std::string> column;
};

template <gnuplot_acceptable_range R>
	requires (!std::convertible_to<R, std::string_view>)//char[]とかはstring_viewに変換されてしまうので除外
auto AllView(R&& r) { return std::views::all(std::forward<R>(r)); }
template <gnuplot_acceptable_matrix_range R>
	requires (!std::convertible_to<R, std::string_view>)//char[]とかはstring_viewに変換されてしまうので除外
auto AllView(R&& r) { return std::views::all(std::forward<R>(r)); }
template <arithmetic V>
auto AllView(V v) { return v; }
inline std::string_view AllView(std::string_view s) { return s; }

template <gnuplot_acceptable_arg X, gnuplot_acceptable_arg Y,
	gnuplot_acceptable_arg XE, gnuplot_acceptable_arg YE,
	gnuplot_acceptable_arg VC, gnuplot_acceptable_arg VS, gnuplot_acceptable_arg VFC>
struct PointParam : public PlotParamBase
{
	template <keyword_arg ...Ops>
	PointParam(X x_, Y y_, XE xe_, YE ye_, VC vc_, VS vs_, VFC vfc_, Ops ...ops)
		: x(x_), y(y_), xerrorbar(xe_), yerrorbar(ye_),
		variable_color(vc_), variable_size(vs_), variable_fillcolor(vfc_)
	{
		SetOptions(ops...);
	}

	template <keyword_arg ...Ops>
	void SetOptions(Ops ...ops)
	{
		SetBaseOptions(ops...);
		if constexpr (KeywordExists(plot::linetype, ops...)) linetype = GetKeywordArg(plot::linetype, ops...);
		if constexpr (KeywordExists(plot::linewidth, ops...)) linewidth = GetKeywordArg(plot::linewidth, ops...);
		if constexpr (KeywordExists(plot::dashtype, ops...)) dashtype = GetKeywordArg(plot::dashtype, ops...);
		if constexpr (KeywordExists(plot::color, ops...)) color = GetKeywordArg(plot::color, ops...);
		if constexpr (KeywordExists(plot::color_rgb, ops...)) color_rgb = GetKeywordArg(plot::color_rgb, ops...);

		if constexpr (KeywordExists(plot::fillcolor, ops...)) fillcolor = GetKeywordArg(plot::fillcolor, ops...);
		if constexpr (KeywordExists(plot::fillpattern, ops...)) pattern = GetKeywordArg(plot::fillpattern, ops...);
		if constexpr (KeywordExists(plot::fillsolid, ops...)) solid = GetKeywordArg(plot::fillsolid, ops...);
		if constexpr (KeywordExists(plot::filltransparent, ops...)) transparent = GetKeywordArg(plot::filltransparent, ops...);
		if constexpr (KeywordExists(plot::bordercolor, ops...)) bordercolor = GetKeywordArg(plot::bordercolor, ops...);
		if constexpr (KeywordExists(plot::bordertype, ops...)) bordertype = GetKeywordArg(plot::bordertype, ops...);

		if constexpr (KeywordExists(plot::style, ops...)) style = GetKeywordArg(plot::style, ops...);
		if constexpr (KeywordExists(plot::smooth, ops...)) smooth = GetKeywordArg(plot::smooth, ops...);
		if constexpr (KeywordExists(plot::pointtype, ops...)) pointtype = GetKeywordArg(plot::pointtype, ops...);
		if constexpr (KeywordExists(plot::pointsize, ops...)) pointsize = GetKeywordArg(plot::pointsize, ops...);
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

	X x;
	Y y;
	XE xerrorbar = {};
	YE yerrorbar = {};

	//LineOption
	int linetype = -2;//-2ならデフォルト
	double linewidth = -1.;//-1ならデフォルト、-2ならvariable
	std::vector<int> dashtype = {};
	std::string color = {};
	std::string color_rgb = {};
	VC variable_color = {};

	//PointOption
	Style style = Style::points;//デフォルトではPOINTS
	int pointtype = -1;//-1ならデフォルト
	double pointsize = -1.;//-1ならデフォルト、-2ならvariable
	VS variable_size = {};
	Smooth smooth = Smooth::none;

	//FillOption
	std::string fillcolor = {};
	VFC variable_fillcolor = {};
	int pattern = -1;
	double solid = -1;
	bool transparent = false;
	std::string bordercolor = {};
	int bordertype = -3;
};
template <keyword_arg ...Options>
auto MakePointParam(Options ...ops)
{
	auto x = AllView(GetKeywordArg(plot::x, std::ranges::empty_view<double>{}, ops...));
	auto y = AllView(GetKeywordArg(plot::y, std::ranges::empty_view<double>{}, ops...));
	auto xerrorbar = AllView(GetKeywordArg(plot::xerrorbar, std::ranges::empty_view<double>{}, ops...));
	auto yerrorbar = AllView(GetKeywordArg(plot::yerrorbar, std::ranges::empty_view<double>{}, ops...));
	auto variable_color = AllView(GetKeywordArg(plot::variable_color, std::ranges::empty_view<double>{}, ops...));
	auto variable_size = AllView(GetKeywordArg(plot::variable_size, std::ranges::empty_view<double>{}, ops...));
	auto variable_fillcolor = AllView(GetKeywordArg(plot::variable_fillcolor, std::ranges::empty_view<double>{}, ops...));
	return PointParam<decltype(x), decltype(y), decltype(xerrorbar), decltype(yerrorbar),
		decltype(variable_color), decltype(variable_size), decltype(variable_fillcolor)>
		(x, y, xerrorbar, yerrorbar, variable_color, variable_size, variable_fillcolor, ops...);
}

template <gnuplot_acceptable_arg X, gnuplot_acceptable_arg Y, gnuplot_acceptable_arg Z,
	gnuplot_acceptable_arg XE, gnuplot_acceptable_arg YE, gnuplot_acceptable_arg ZE,
	gnuplot_acceptable_arg VC, gnuplot_acceptable_arg VS, gnuplot_acceptable_arg VFC>
struct PointParam3D : public PointParam<X, Y, XE, YE, VC, VS, VFC>
{
	using Base = PointParam<X, Y, XE, YE, VC, VS, VFC>;
	template <keyword_arg ...Ops>
	PointParam3D(X x_, Y y_, Z z_, XE xe_, YE ye_, ZE ze_, VC vc_, VS vs_, VFC vfc_, Ops ...ops)
		: Base(x_, y_, xe_, ye_, vc_, vs_, vfc_), z(z_), zerrorbar(ze_)
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

	Z z;
	ZE zerrorbar = {};
};
template <keyword_arg ...Options>
auto MakePointParam3D(Options ...ops)
{
	auto x = AllView(GetKeywordArg(plot::x, std::ranges::empty_view<double>{}, ops...));
	auto y = AllView(GetKeywordArg(plot::y, std::ranges::empty_view<double>{}, ops...));
	auto z = AllView(GetKeywordArg(plot::z, std::ranges::empty_view<double>{}, ops...));
	auto xerrorbar = AllView(GetKeywordArg(plot::xerrorbar, std::ranges::empty_view<double>{}, ops...));
	auto yerrorbar = AllView(GetKeywordArg(plot::yerrorbar, std::ranges::empty_view<double>{}, ops...));
	auto zerrorbar = AllView(GetKeywordArg(plot::zerrorbar, std::ranges::empty_view<double>{}, ops...));
	auto variable_color = AllView(GetKeywordArg(plot::variable_color, std::ranges::empty_view<double>{}, ops...));
	auto variable_size = AllView(GetKeywordArg(plot::variable_size, std::ranges::empty_view<double>{}, ops...));
	auto variable_fillcolor = AllView(GetKeywordArg(plot::variable_fillcolor, std::ranges::empty_view<double>{}, ops...));
	return PointParam3D<decltype(x), decltype(y), decltype(z), decltype(xerrorbar), decltype(yerrorbar), decltype(zerrorbar),
		decltype(variable_color), decltype(variable_size), decltype(variable_fillcolor)>
		(x, y, z, xerrorbar, yerrorbar, zerrorbar, variable_color, variable_size, variable_fillcolor, ops...);
}

template <gnuplot_acceptable_arg X, gnuplot_acceptable_arg Y,
	gnuplot_acceptable_arg XL, gnuplot_acceptable_arg YL,
	gnuplot_acceptable_arg VC>
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
		if constexpr (KeywordExists(plot::linetype, ops...)) linetype = GetKeywordArg(plot::linetype, -2, ops...);
		if constexpr (KeywordExists(plot::linewidth, ops...)) linewidth = GetKeywordArg(plot::linewidth, -1, ops...);
		if constexpr (KeywordExists(plot::color, ops...)) color = GetKeywordArg(plot::color, "", ops...);
		if constexpr (KeywordExists(plot::color_rgb, ops...)) color_rgb = GetKeywordArg(plot::color_rgb, "", ops...);
		if constexpr (KeywordExists(plot::arrowhead, ops...)) arrowhead = GetKeywordArg(plot::arrowhead, -1, ops...);
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

	//LineOption
	int linetype = -2;//-2ならデフォルト
	double linewidth = -1;//-1ならデフォルト、-2ならvariable
	std::string color;
	std::string color_rgb;
	VC variable_color;

	//VectorOption
	X x;
	Y y;
	XL xlen;
	YL ylen;
	int arrowhead = -1;//-1ならデフォルト。
};
template <keyword_arg ...Options>
auto MakeVectorParam(Options ...ops)
{
	auto x = AllView(GetKeywordArg(plot::x, std::ranges::empty_view<double>{}, ops...));
	auto y = AllView(GetKeywordArg(plot::y, std::ranges::empty_view<double>{}, ops...));
	auto xlen = AllView(GetKeywordArg(plot::xlen, std::ranges::empty_view<double>{}, ops...));
	auto ylen = AllView(GetKeywordArg(plot::ylen, std::ranges::empty_view<double>{}, ops...));
	auto variable_color = AllView(GetKeywordArg(plot::variable_color, std::ranges::empty_view<double>{}, ops...));
	return VectorParam<decltype(x), decltype(y), decltype(xlen), decltype(ylen), decltype(variable_color)>
		(x, y, xlen, ylen, variable_color, ops...);
}

template <gnuplot_acceptable_arg X, gnuplot_acceptable_arg Y, gnuplot_acceptable_arg Z,
		  gnuplot_acceptable_arg XL, gnuplot_acceptable_arg YL, gnuplot_acceptable_arg ZL,
		  gnuplot_acceptable_arg VC>
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

	Z z;
	ZL zlen;
};
template <keyword_arg ...Options>
auto MakeVectorParam3D(Options ...ops)
{
	auto x = AllView(GetKeywordArg(plot::x, std::ranges::empty_view<double>{}, ops...));
	auto y = AllView(GetKeywordArg(plot::y, std::ranges::empty_view<double>{}, ops...));
	auto z = AllView(GetKeywordArg(plot::z, std::ranges::empty_view<double>{}, ops...));
	auto xlen = AllView(GetKeywordArg(plot::xlen, std::ranges::empty_view<double>{}, ops...));
	auto ylen = AllView(GetKeywordArg(plot::ylen, std::ranges::empty_view<double>{}, ops...));
	auto zlen = AllView(GetKeywordArg(plot::zlen, std::ranges::empty_view<double>{}, ops...));
	auto variable_color = AllView(GetKeywordArg(plot::variable_color, std::ranges::empty_view<double>{}, ops...));
	return VectorParam3D<decltype(x), decltype(y), decltype(z), decltype(xlen), decltype(ylen), decltype(zlen), decltype(variable_color)>
		(x, y, z, xlen, ylen, zlen, variable_color, ops...);
}

template <gnuplot_acceptable_arg X, gnuplot_acceptable_arg Y, gnuplot_acceptable_arg Y2,
	gnuplot_acceptable_arg VC>
struct FilledCurveParam : public PlotParamBase
{
	template <keyword_arg ...Ops>
	FilledCurveParam(X x_, Y y_, Y2 y2_, VC vc_, Ops ...ops)
		: x(x_), y(y_), ybelow(y2_), variable_fillcolor(vc_)
	{
		SetOptions(ops...);
	}

	template <keyword_arg ...Ops>
	void SetOptions(Ops ...ops)
	{
		SetBaseOptions(ops...);
		if constexpr (KeywordExists(plot::fillcolor, ops...)) fillcolor = GetKeywordArg(plot::fillcolor, ops...);
		if constexpr (KeywordExists(plot::baseline, ops...)) baseline = GetKeywordArg(plot::baseline, ops...);
		if constexpr (KeywordExists(plot::fillpattern, ops...)) fillpattern = GetKeywordArg(plot::fillpattern, ops...);
		if constexpr (KeywordExists(plot::fillsolid, ops...)) fillsolid = GetKeywordArg(plot::fillsolid, ops...);
		if constexpr (KeywordExists(plot::filltransparent, ops...)) filltransparent = GetKeywordArg(plot::filltransparent, ops...);
		if constexpr (KeywordExists(plot::bordercolor, ops...)) bordercolor = GetKeywordArg(plot::bordercolor, ops...);
		if constexpr (KeywordExists(plot::bordertype, ops...)) bordertype = GetKeywordArg(plot::bordertype, ops...);
		if constexpr (KeywordExists(plot::closed, ops...)) closed = GetKeywordArg(plot::closed, ops...);
		if constexpr (KeywordExists(plot::above, ops...)) above = GetKeywordArg(plot::above, ops...);
		if constexpr (KeywordExists(plot::below, ops...)) below = GetKeywordArg(plot::below, ops...);
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

	//FillOption
	std::string fillcolor;
	VC variable_fillcolor;
	std::string baseline;
	int fillpattern = -1;
	double fillsolid = -1.;
	bool filltransparent = false;
	std::string bordercolor;
	int bordertype = -3;//-2はnorborderを意味する。

	//FilledCurveOption
	X x;
	Y y;
	Y2 ybelow;
	bool closed = false;
	bool above = false;
	bool below = false;
};

template <keyword_arg ...Options>
auto MakeFilledCurveParam(Options ...ops)
{
	auto x = AllView(GetKeywordArg(plot::x, std::ranges::empty_view<double>{}, ops...));
	auto y = AllView(GetKeywordArg(plot::y, std::ranges::empty_view<double>{}, ops...));
	auto y2 = AllView(GetKeywordArg(plot::ybelow, std::ranges::empty_view<double>{}, ops...));
	auto variable_fillcolor = AllView(GetKeywordArg(plot::variable_fillcolor, std::ranges::empty_view<double>{}, ops...));
	return FilledCurveParam<decltype(x), decltype(y), decltype(y2), decltype(variable_fillcolor)>
		(x, y, y2, variable_fillcolor, ops...);
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
	View x;
};
struct CoordMinMax
{
	struct iterator
	{
		iterator(const CoordMinMax& p, size_t pos = 0)
			: current(pos), parent(&p) {}
		void operator++() { ++current; }
		bool operator==(const iterator& i) const { return current == i.current; }
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

template <gnuplot_acceptable_matrix_range Map, ranges::arithmetic_range XRange, ranges::arithmetic_range YRange>
struct ColormapParam : PlotParamBase
{
	template <keyword_arg ...Ops>
	ColormapParam(Map map_, XRange xrange_, YRange yrange_,
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
		if constexpr (KeywordExists(plot::with_contour, ops...)) with_contour = GetKeywordArg(plot::with_contour, ops...);
		if constexpr (KeywordExists(plot::without_surface, ops...)) without_surface = GetKeywordArg(plot::without_surface, ops...);
		if constexpr (KeywordExists(plot::cntrsmooth, ops...)) cntrsmooth = GetKeywordArg(plot::cntrsmooth, ops...);
		if constexpr (KeywordExists(plot::cntrpoints, ops...)) cntrpoints = GetKeywordArg(plot::cntrpoints, ops...);
		if constexpr (KeywordExists(plot::cntrorder, ops...)) cntrorder = GetKeywordArg(plot::cntrorder, ops...);
		if constexpr (KeywordExists(plot::cntrlevels_auto, ops...)) cntrlevels_auto = GetKeywordArg(plot::cntrlevels_auto, ops...);
		if constexpr (KeywordExists(plot::cntrlevels_discrete, ops...)) cntrlevels_discrete = GetKeywordArg(plot::cntrlevels_discrete, ops...);
		if constexpr (KeywordExists(plot::cntrlevels_incremental, ops...)) cntrlevels_incremental = GetKeywordArg(plot::cntrlevels_incremental, ops...);
		if constexpr (KeywordExists(plot::cntrcolor, ops...)) cntrcolor = GetKeywordArg(plot::cntrcolor, ops...);
		if constexpr (KeywordExists(plot::variable_cntrcolor, ops...)) variable_cntrcolor = GetKeywordArg(plot::variable_cntrcolor, ops...);
		if constexpr (KeywordExists(plot::cntrlinetype, ops...)) cntrlinetype = GetKeywordArg(plot::cntrlinetype, ops...);
		if constexpr (KeywordExists(plot::cntrlinewidth, ops...)) cntrlinewidth = GetKeywordArg(plot::cntrlinewidth, ops...);
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
	Map map;
	XRange xrange;
	YRange yrange;
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
template <colormap_option ...Options>
auto MakeColormapParam(Options ...ops)
{
	auto map = AllView(GetKeywordArg(plot::map, std::ranges::empty_view<std::ranges::empty_view<double>>{}, ops...));
	auto xrange = AllView(GetKeywordArg(plot::xrange, std::ranges::empty_view<double>{}, ops...));
	auto yrange = AllView(GetKeywordArg(plot::yrange, std::ranges::empty_view<double>{}, ops...));
	auto xminmax = GetKeywordArg(plot::xminmax, std::pair<double, double>{ 0, 0 }, ops...);
	auto yminmax = GetKeywordArg(plot::yminmax, std::pair<double, double>{ 0, 0 }, ops...);
	return ColormapParam<decltype(map), decltype(xrange), decltype(yrange)>
		(map, xrange, yrange, xminmax, yminmax, ops...);
}

template <ranges::arithmetic_range Range>
struct HistogramParam
{
	template <keyword_arg ...Ops>
	HistogramParam(Range data, double xmin, double xmax, size_t xnbin, Ops ...ops)
		: data(data), xmin(xmin), xmax(xmax), xnbin(xnbin)
	{
		SetOptions(ops...);
	}

	template <keyword_arg ...Ops>
	void SetOptions(Ops ...ops)
	{}
	Range data;
	double xmin;
	double xmax;
	size_t xnbin;
};
template <keyword_arg ...Options>
auto MakeHistogramParam(Options ...ops)
{
	auto data = AllView(GetKeywordArg(plot::data, ops...));
	auto xmin = GetKeywordArg(plot::xmin, ops...);
	auto xmax = GetKeywordArg(plot::xmax, ops...);
	auto xnbin = GetKeywordArg(plot::xnbin, ops...);
	return HistogramParam<decltype(data)>(data, xmin, xmax, xnbin, ops...);
}


template <class Range>
auto ConvertUniqueToRange(Range&& range)
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
inline void MakeDataObjectCommon(Stream& stream, std::tuple<Ranges...> ranges)
{
	auto zipped = std::apply([]<class ...R>(R&& ...r) { return views::Zip(ConvertUniqueToRange(std::forward<R>(r))...); }, ranges);
	auto&& it = zipped.begin();
	auto&& end = zipped.end();
	for (; it != end; ++it)
	{
		std::apply([&stream](auto&& ...args) { adapt::Print(stream, args...); }, *it);
	}
	if (!IsAllEnd(TypeList<Ranges...>{}, it, end, std::make_index_sequence<sizeof...(Ranges)>{}))
	{
		PrintWarning("WARNING : The numbers of datapoints are inconsistent. The trailing data points are truncated to fit the smallest data set.");
	}
}

template <class Stream, gnuplot_acceptable_matrix_range MatRange, class RangeX, class RangeY>
void MakeDataObjectCommon(Stream& stream, const MatRange& mat, const RangeX& rx, const RangeY& ry)
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
		auto [y, cy] = *ity;
		adapt::Print(stream, x, y, cx, cy, " 0\n");
		++mitx;
	}
	auto [x, cx] = *itx;
	auto ity = ry.begin();
	for (; ity != ry.end(); ++ity)
	{
		auto [y, cy] = *ity;
		adapt::Print(stream, x, y, cx, cy, " 0");
	}
	auto [y, cy] = *ity;
	adapt::Print(stream, x, y, cx, cy, " 0");
}
template <class ...Args>
inline void MakeDataObject(Canvas* g, const std::string& name, Args&& ...args)
{
	if (g->IsInMemoryDataTransferEnabled())
	{
		// make datablock
		g->Command(name + " << EOD");
		FILE* pipe = g->GetPipe();
		MakeDataObjectCommon(pipe, std::forward<Args>(args)...);
		g->Command("EOD");
	}
	else
	{
		// make file
		std::ofstream ofs(name);
		if (!ofs) throw InvalidArg("file \"" + name + "\" cannot open.");
		MakeDataObjectCommon(ofs, std::forward<Args>(args)...);
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
template <size_t I, size_t Count, size_t N>
auto ArrangeColumnOption(std::vector<std::string>&, std::string&, Canvas*,
						 [[maybe_unused]] std::array<std::string_view, N>)
{
	return std::tuple<>{};
}
template <size_t I, size_t Count, size_t N, class Range, class ...Ranges>
auto ArrangeColumnOption(std::vector<std::string>& column, std::string& labelcolumn, Canvas* canvas,
						 [[maybe_unused]] std::array<std::string_view, N> axes,
						 Range&& range, Ranges&& ...ranges)
{
	if constexpr (!PlotParamBase::IsEmptyView<Range>())
	{
		using DecayedType = std::decay_t<decltype(range)>;
		//普通のstd::vector<double>のような数値リストか、さもなくば固定値の場合。
		if constexpr (ranges::arithmetic_range<DecayedType> || arithmetic<DecayedType>)
		{
			//固定値の場合、従来は($1-$1+{})とカラム指定することで出力していたが、
			//この方法だと$1が文字列だった場合にエラーになる。
			//そこで、固定値であってもそのままデータ出力することにする。
			//固定値からrangeへの変更は別所で行われる。
			column.emplace_back(std::to_string(Count));
			return std::tuple_cat(
				std::forward_as_tuple(std::forward<Range>(range)),
				ArrangeColumnOption<I + 1, Count + 1>(column, labelcolumn, canvas, axes, std::forward<Ranges>(ranges)...));
		}
		//std::vector<std::string>のような文字列リストの場合。
		else if constexpr (ranges::string_range<DecayedType>)
		{
			std::string_view axis = axes[I];
			if (canvas->IsDateTimeEnabled(axis))
				column.emplace_back(std::to_string(Count));
			else
				labelcolumn = std::format("{}tic({})", axis, Count);
			return std::tuple_cat(
				std::forward_as_tuple(std::forward<Range>(range)),
				ArrangeColumnOption<I + 1, Count + 1>(column, labelcolumn, canvas, axes, std::forward<Ranges>(ranges)...));
		}
		//これらはrangeではないので、戻り値に加える必要はない。
		else if constexpr (std::convertible_to<DecayedType, std::string_view>)
		{
			column.emplace_back(axes[I]);
			return ArrangeColumnOption<I + 1, Count>(column, labelcolumn, canvas, axes, std::forward<Ranges>(ranges)...);
		}
	}
	else
	{
		return ArrangeColumnOption<I + 1, Count>(column, labelcolumn, canvas, axes, std::forward<Ranges>(ranges)...);
	}
}

template <class Var>
void AddColumn(Var&& var, std::string_view name, std::vector<std::string>& column)
{
	if constexpr (std::convertible_to<Var, std::string_view>) column.emplace_back(var);
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

template <class X, class Y, class XE, class YE, class VC, class VS, class VFC>
std::string MakePlotCommand(std::string_view, bool, const PointParam<X, Y, XE, YE, VC, VS, VFC>& p)
{
	std::string c;
	constexpr bool xeb_assigned = !PlotParamBase::IsEmptyView<XE>();
	constexpr bool yeb_assigned = !PlotParamBase::IsEmptyView<YE>();
	constexpr bool variablecolor_assigned = !PlotParamBase::IsEmptyView<VC>();
	constexpr bool variablesize_assigned = !PlotParamBase::IsEmptyView<VS>();
	constexpr bool variablefillcolor_assigned = !PlotParamBase::IsEmptyView<VFC>();
	if constexpr (xeb_assigned || yeb_assigned)
	{
		if (p.style == Style::lines)
		{
			if constexpr (xeb_assigned && yeb_assigned)
				c += " xyerrorlines";
			else if constexpr (xeb_assigned)
				c += " xerrorlines";
			else
				c += " yerrorlines";
			if (p.linetype != -2) c += " linetype " + std::to_string(p.linetype);
			if (p.linewidth != -1) c += " linewidth " + std::to_string(p.linewidth);
			if (!p.dashtype.empty())
			{
				c += " dashtype (" + std::to_string(p.dashtype.front());
				for (size_t i = 1; i < p.dashtype.size(); ++i) c += ", " + std::to_string(p.dashtype[i]);
				c += ")";
			}
			if (!p.color.empty()) c += std::format(" linecolor '{}'", p.color);
			else if constexpr (variablecolor_assigned) c += " linecolor palette";
		}
		else if (p.style == Style::boxes)
		{
			if constexpr (xeb_assigned && yeb_assigned)
				c += " boxxyerrorbars";
			else if constexpr (xeb_assigned)
				PrintWarning("WARNING : Box style is incompatible with xerrorbar option. It is to be ignored.");
			else
				c += " boxerrorbars";
			if (p.linetype != -2) c += " linetype " + std::to_string(p.linetype);
			if (p.linewidth != -1.) c += " linewidth " + std::to_string(p.linewidth);
			if (!p.color.empty()) c += std::format(" linecolor '{}'", p.color);
			else if (!p.color_rgb.empty()) c += std::format(" linecolor rgb '{}'", p.color_rgb);
			else if constexpr (variablecolor_assigned) c += " linecolor palette";
		}
		else
		{
			if (p.style != Style::points)
				PrintWarning("WARNING : Only \"lines\", \"boxes\" or \"points\" styles are allowed with errobars. Style option is to be ignored.");

			//c += " points";
			if constexpr (xeb_assigned && yeb_assigned)
				c += " xyerrorbars";
			else if constexpr (xeb_assigned)
				c += " xerrorbars";
			else
				c += " yerrorbars";
			if (p.pointtype != -1) c += " pointtype " + std::to_string(p.pointtype);
			if (p.pointsize != -1) c += " pointsize " + std::to_string(p.pointsize);
			else if constexpr (variablesize_assigned) c += " pointsize variable";
			if (p.linewidth != -1) c += " linewidth " + std::to_string(p.linewidth);
			if (!p.color.empty()) c += std::format(" linecolor '{}'", p.color);//pointのときも何故かlinecolorらしい。
			else if (!p.color_rgb.empty()) c += std::format(" linecolor rgb '{}'", p.color_rgb);
			else if constexpr (variablecolor_assigned) c += " palette";//しかしpalette指定の場合はlinecolorがいらない。謎。
		}
	}
	//ベクトル、エラーバー指定がない場合。
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
		case Style::lines: strstyle = " lines"; break;
		case Style::impulses: strstyle = " impulses"; break;
		case Style::steps: strstyle = " steps"; break;
		case Style::fsteps: strstyle = " fsteps"; break;
		case Style::histeps: strstyle = " histeps"; break;
		case Style::boxes: strstyle = " boxes"; break;
		default: break;
		}
		if (p.linetype != -2) others += " linetype " + std::to_string(p.linetype);
		if (p.linewidth != -1.) others += " linewidth " + std::to_string(p.linewidth);
		if (!p.dashtype.empty())
		{
			others += " dashtype (" + std::to_string(p.dashtype.front());
			for (size_t i = 1; i < p.dashtype.size(); ++i) others += ", " + std::to_string(p.dashtype[i]);
			others += ")";
		}
		if (!p.color.empty()) others += std::format(" linecolor '{}'", p.color);
		else if (!p.color_rgb.empty()) c += std::format(" linecolor rgb '{}'", p.color_rgb);
		else if constexpr (variablecolor_assigned) others += " linecolor palette";

		if (p.style == Style::boxes || p.style == Style::steps)
		{
			if (p.style == Style::steps) strstyle = " fillsteps";
			//現状、fill系オプションはboxesまたはstepsにしか使えない。
			if (!p.fillcolor.empty()) others += std::format(" fillcolor '{}'", p.fillcolor);
			else if constexpr (variablefillcolor_assigned) others += " fillcolor palette";

			{
				std::string fs;
				if (p.transparent) fs += " transparent";
				if (p.solid != -1) fs += std::format(" solid {}", p.solid);
				else if (p.pattern != -1) fs += std::format(" pattern {}", p.pattern);
				if (!fs.empty()) others += " fs" + fs;
			}
			{
				std::string bd;
				if (p.bordertype == -2) bd += " noborder";
				else if (p.bordertype != -3) bd += std::format(" {}", p.bordertype);
				if (!p.bordercolor.empty()) bd += std::format(" linecolor '{}'", p.bordercolor);
				if (!bd.empty()) others += " border" + bd;
			}
		}
		c += strstyle;
		c += others;
	}
	else if (p.style == Style::points)
	{
		c += " points";
		if (p.linetype != -2)
			PrintWarning("WARNING : \"points\" style is incompatible with linetype option. It is to be ignored.");
		if (p.linewidth != -1.)
			PrintWarning("WARNING : \"points\" style is incompatible with linewidth option. It is to be ignored.");
		if (p.pointtype != -1) c += " pointtype " + std::to_string(p.pointtype);
		if (p.pointsize != -1) c += " pointsize " + std::to_string(p.pointsize);
		else if constexpr (variablesize_assigned) c += " pointsize variable";
		if (!p.color.empty()) c += std::format(" linecolor '{}'", p.color);//pointのときも何故かlinecolorらしい。
		else if (!p.color_rgb.empty()) c += std::format(" linecolor rgb '{}'", p.color_rgb);
		else if constexpr (variablecolor_assigned) c += " palette";//しかしpalette指定の場合はlinecolorがいらない。謎。
	}
	else if (p.style == Style::linespoints)
	{
		c += " linespoints";
		if (p.linetype != -2) c += " linetype " + std::to_string(p.linetype);
		if (p.linewidth != -1) c += " linewidth " + std::to_string(p.linewidth);
		if (p.pointtype != -1) c += " pointtype " + std::to_string(p.pointtype);
		if (p.pointsize != -1) c += " pointsize " + std::to_string(p.pointsize);
		else if constexpr (variablesize_assigned) c += " pointsize variable";
		if (!p.dashtype.empty())
		{
			c += " dashtype (" + std::to_string(p.dashtype.front());
			for (size_t i = 1; i < p.dashtype.size(); ++i) c += ", " + std::to_string(p.dashtype[i]);
			c += ")";
		}
		if (!p.color.empty()) c += std::format(" linecolor '{}'", p.color);
		else if (!p.color_rgb.empty()) c += std::format(" linecolor rgb '{}'", p.color_rgb);
		else if constexpr (variablecolor_assigned) c += " linecolor palette";
	}
	else if (p.style == Style::dots)
	{
		c += " dots";
		if (p.linetype != -2)
			PrintWarning("WARNING : \"points\" style is incompatible with linetype option. It is to be ignored.");
		if (p.linewidth != -1)
			PrintWarning("WARNING : \"points\" style is incompatible with linewidth option. It is to be ignored.");
		if (p.pointtype != -1)
			PrintWarning("WARNING : \"dots\" style is incompatible with pointtype option. It is to be ignored.");
		if (p.pointsize != -1)
			PrintWarning("WARNING : \"dots\" style is incompatible with pointsize option. Use points with pointtype 7.");
		if (!p.color.empty()) c += std::format(" linecolor '{}'", p.color);
		else if (!p.color_rgb.empty()) c += std::format(" linecolor rgb '{}'", p.color_rgb);
		else if constexpr (variablecolor_assigned) c += " palette ";
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
	return c;
}
template <class X, class Y, class XL, class YL, class VC>
std::string MakePlotCommand(std::string_view, bool, const VectorParam<X, Y, XL, YL, VC>& p)
{
	std::string c;
	c += " vector ";
	if (p.arrowhead != -1)
	{
		if ((p.arrowhead & 0b11) == 0) c += " head";
		else if ((p.arrowhead & 0b11) == 1) c += " heads";
		else if ((p.arrowhead & 0b11) == 2) c += " noheads";
		if ((p.arrowhead & 0b1100) == 0) c += " filled";
		else if ((p.arrowhead & 0b1100) == 4) c += " empty";
		else if ((p.arrowhead & 0b1100) == 8) c += " nofilled";
	}
	constexpr bool variablecolor_assigned = !PlotParamBase::IsEmptyView<VC>();
	if (p.linetype != -2) c += " linetype " + std::to_string(p.linetype);
	if (p.linewidth != -1.) c += " linewidth " + std::to_string(p.linewidth);
	if (!p.color.empty()) c += " linecolor '" + p.color + "'";
	else if constexpr (variablecolor_assigned) c += " linecolor palette";
	return c;
}
template <class X, class Y, class Y2, class VC>
std::string MakePlotCommand(std::string_view, bool, const FilledCurveParam<X, Y, Y2, VC>& p)
{
	std::string c;
	c += " filledcurves";

	if (!p.closed && !p.above && !p.below && p.baseline.empty())
		//何も指定のないデフォルトの場合、x1軸との間の領域を塗りつぶす。
		c += " x1";

	if (p.closed) c += " closed";
	else if (p.above) c += " above";
	else if (p.below) c += " below";
	if (!p.baseline.empty()) c += " " + p.baseline;

	//std::string fillcolor = GetKeywordArg(plot::fillcolor, "", std::forward<Options>(opts)...);
	//bool variablecolor_assigned = KeywordExists(plot::variablecolor, std::forward<Options>(opts)...);
	constexpr bool variablecolor_assigned = !PlotParamBase::IsEmptyView<VC>();
	if (!p.fillcolor.empty()) c += " fillcolor '" + p.fillcolor + "'";
	else if constexpr (variablecolor_assigned) c += " fillcolor palette";

	{
		std::string fs;
		if (p.filltransparent) fs += " transparent";
		if (p.fillsolid != -1) fs += std::format(" solid {}", p.fillsolid);
		else if (p.fillpattern != -1) fs += std::format(" pattern {}", p.fillpattern);
		if (!fs.empty()) c += " fillstyle" + fs;
	}
	{
		std::string bd;
		if (p.bordertype == -2) bd += " noborder";
		else if (p.bordertype != -3) bd += std::format(" {}", p.bordertype);
		if (!p.bordercolor.empty()) bd += " linecolor '" + p.bordercolor + "'";
		if (!bd.empty()) c += " border" + bd;
	}
	return c;
}
template <class Map, class X, class Y>
std::string MakePlotCommand(std::string_view output_name, bool inmemory, const ColormapParam<Map, X, Y>& p)
{
	std::string c;
	c += " pm3d";
	if (p.without_surface) c += " nosurface";

	//contourが有効の場合はこれもコマンドに追加する。
	if (p.with_contour)
	{
		if (!p.IsData()) PrintWarning("WARNING : Contour option is only available for data plot mode.");
		if (p.with_contour && p.IsData())
		{
			if (inmemory)
			{
				c += std::format(", {}_cntr with line", output_name);
			}
			else
			{
				std::string str = std::format("'{}", output_name);
				str.erase(str.end() - 3, str.end());
				c += ", " + str + "cntr.txt' with line";
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
template <class Map, class X, class Y>
std::string MakeContourPlotCommand(std::string_view output_name, bool inmemory, const ColormapParam<Map, X, Y>& p)
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
								  const std::vector<std::string>& column,
								  const Param& p)
{
	std::string c;
	if (p.IsData())
	{
		//from data
		if (inmemory)
			//variable name
			c += std::format(" {}", output_name);
		else
			//filename
			c += std::format(" '{}'", output_name);

		//using
		c += " using ";
		for (const auto& clm : column) c += std::format("{}:", clm);
		c.pop_back();
	}
	else if (p.IsFile())
	{
		//from file
		c += std::format(" '{}'", output_name);

		//using
		c += " using ";
		for (const auto& clm : column) c += std::format("{}:", clm);
		c.pop_back();

	}
	else if (p.IsEquation())
	{
		//from equation
		c += std::format(" {}", output_name);
	}

	//title
	if (!p.title.empty())
	{
		if (p.title == "notitle") c += " notitle";
		else c += std::format(" title '{}'", p.title);
	}

	c += " with";
	//point、vector、filledcurveなどへ分岐
	c += MakePlotCommand(output_name, inmemory, p);

	//axis
	if (!p.axis.empty()) c += std::format(" axes {}", p.axis);

	return c;
}

}

}

#endif