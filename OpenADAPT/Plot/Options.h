#ifndef ADAPT_PLOT_OPTIONS_H
#define ADAPT_PLOT_OPTIONS_H

#include <string_view>
#include <vector>
#include <OpenADAPT/Utility/Macros.h>
#include <OpenADAPT/Utility/KeywordArgs.h>
#include <OpenADAPT/Utility/Ranges.h>


namespace adapt
{

ADAPT_EXPORT enum class Style : int16_t { none, lines, points, linespoints, dots, impulses, boxes, steps, fsteps, histeps, pm3d, };
ADAPT_EXPORT enum class Smooth : int16_t { none, unique, frequency, cumulative, cnormal, kdensity, csplines, acsplines, bezier, sbezier, };
ADAPT_EXPORT enum class ArrowHead : int16_t { none, head, heads, nohead, };
ADAPT_EXPORT enum class ArrowFill : int16_t { none, filled, empty, nofilled, noborder, };
ADAPT_EXPORT enum class LabelPos : int16_t { none, left, center, right, };
ADAPT_EXPORT enum class LabelOverlay : int16_t { none, front, back, };
ADAPT_EXPORT enum class Pm3dPosition : int16_t { none, bottom, surface, top, };

ADAPT_EXPORT enum class BinError : int16_t { none, poisson68, poisson95, normal68, normal95, };

ADAPT_EXPORT enum class Contour : int16_t { none, base, surface, both, };
ADAPT_EXPORT enum class CntrSmooth : int16_t { none, linear, cubicspline, bspline };

ADAPT_EXPORT enum class MPFillOrder : int16_t { rowsfirst, colsfirst };
ADAPT_EXPORT enum class MPVerticalDirection : int16_t { downwards, upwards };

namespace plot_detail
{

// 1. 数値型のrange
// 2. 文字列型のrange
// 3. 単一の数値
// いずれかを受け付ける。
// 単一の文字列の場合はカラム扱いであり、file + columnという指定と認識させる。
template <class Range>
concept acceptable_range = ranges::arithmetic_range<Range> || ranges::string_range<Range>;
template <class Type>
concept acceptable_arg = acceptable_range<Type> || arithmetic<std::decay_t<Type>> || std::convertible_to<Type, std::string_view>;
template <class Type>
concept acceptable_arg_except_string = (acceptable_range<Type> || arithmetic<std::decay_t<Type>>) && !std::convertible_to<Type, std::string_view>;

template <ranges::arithmetic_range Range>
struct ArithmeticRange {};
template <ranges::string_range Range>
struct StringRange {};
template <acceptable_arg>
struct AcceptableArg {};

using AnyArithmeticRange = AnyTypeKeyword<ArithmeticRange>;
using AnyStringRange = AnyTypeKeyword<StringRange>;
using AnyAcceptableArg = AnyTypeKeyword<AcceptableArg>;

struct BaseOption {};
struct StyleOption {};
struct FillOption {};
struct ColorOption {};
struct LineOption {};
struct PointOption {};
struct VectorOption {};
struct FilledCurveOption {};
struct LabelOption {};
struct ColormapOption {};
struct SurfaceOption {};
struct WeightOption {};
struct HistogramOption {};
struct BinscatterOption {};
struct MultiplotOption {};

template <class T>
concept base_option = keyword_arg_tagged_with<T, BaseOption>;
template <class T>
concept point_option = keyword_arg_tagged_with<T, BaseOption, StyleOption, ColorOption, LineOption, PointOption>;
template <class T>
concept vector_option = keyword_arg_tagged_with<T, BaseOption, ColorOption, LineOption, VectorOption>;
template <class T>
concept filledcurve_option = keyword_arg_tagged_with<T, BaseOption, StyleOption, ColorOption, FillOption, FilledCurveOption>;
template <class T>
concept label_option = keyword_arg_tagged_with<T, BaseOption, ColorOption, LabelOption>;

template <class Range>
concept acceptable_matrix_range = (ranges::arithmetic_matrix_range<Range> || std::convertible_to<Range, std::string_view>) && !ranges::string_range<Range>;
template <acceptable_matrix_range Range>
struct AcceptableMatrix {};
using AnyMatrix = AnyTypeKeyword<adapt::plot_detail::AcceptableMatrix>;

using AnyCoordRange = AnyTypeKeyword<ArithmeticRange>;

template <class Opt>
concept colormap_option = keyword_arg_tagged_with<Opt, BaseOption, ColormapOption>;
template <class Opt>
concept surface_option = keyword_arg_tagged_with<Opt, BaseOption, StyleOption, ColorOption, LineOption, PointOption, SurfaceOption>;

template <class Opt>
concept histogram_option = keyword_arg_tagged_with<Opt, BaseOption, WeightOption, HistogramOption, ColorOption, LineOption, PointOption, FillOption>;
template <class Opt>
concept binscatter_option = keyword_arg_tagged_with<Opt, BaseOption, WeightOption, BinscatterOption, PointOption, ColormapOption>;

template <class Opt>
concept multiplot_option = keyword_arg_tagged_with<Opt, MultiplotOption>;

template <class Param>
concept zaxis_param = requires(Param p)
{
	{ p.z };
};

}

namespace plot
{

//BaseOption
ADAPT_EXPORT ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(x, plot_detail::AnyAcceptableArg, plot_detail::BaseOption)
ADAPT_EXPORT ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(y, plot_detail::AnyAcceptableArg, plot_detail::BaseOption)
ADAPT_EXPORT ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(z, plot_detail::AnyAcceptableArg, plot_detail::BaseOption)
ADAPT_EXPORT ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(title, std::string_view, plot_detail::BaseOption)
ADAPT_EXPORT ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(title_add_entries, int64_t, plot_detail::BaseOption)//データプロットの場合に、データ点の数をタイトルに追加する。
ADAPT_EXPORT ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(axis, std::string_view, plot_detail::BaseOption)//y2軸を使いたい場合などに、"x1y2"のように指定する。
ADAPT_EXPORT ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(column, std::vector<std::string>, plot_detail::BaseOption);
ADAPT_EXPORT ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(input, std::string_view, plot_detail::BaseOption);

ADAPT_EXPORT ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(style, Style, plot_detail::StyleOption)

//LineOption
ADAPT_EXPORT ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(linetype, int, plot_detail::LineOption)
ADAPT_EXPORT ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(linewidth, double, plot_detail::LineOption)
ADAPT_EXPORT ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(dashtype, std::vector<int>, plot_detail::LineOption)

//ColorOption
ADAPT_EXPORT ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(color, std::string_view, plot_detail::ColorOption)
ADAPT_EXPORT ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(color_rgb, std::string_view, plot_detail::ColorOption)
ADAPT_EXPORT ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(variable_color, plot_detail::AnyAcceptableArg, plot_detail::ColorOption)

//PointOption
ADAPT_EXPORT ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(smooth, Smooth, plot_detail::PointOption)
ADAPT_EXPORT ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(xerrorbar, plot_detail::AnyAcceptableArg, plot_detail::PointOption)//xerrorbarの大きさ
ADAPT_EXPORT ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(yerrorbar, plot_detail::AnyAcceptableArg, plot_detail::PointOption)//yerrorbarの大きさ
ADAPT_EXPORT ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(zerrorbar, plot_detail::AnyAcceptableArg, plot_detail::PointOption)//zerrorbarの大きさ
ADAPT_EXPORT ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(xerrlow, plot_detail::AnyAcceptableArg, plot_detail::PointOption)//xerrorbarの左側（大きさではなく座標）
ADAPT_EXPORT ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(xerrhigh, plot_detail::AnyAcceptableArg, plot_detail::PointOption)//xerrorbarの右側（大きさではなく座標）
ADAPT_EXPORT ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(yerrlow, plot_detail::AnyAcceptableArg, plot_detail::PointOption)//yerrorbarの下側（大きさではなく座標）
ADAPT_EXPORT ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(yerrhigh, plot_detail::AnyAcceptableArg, plot_detail::PointOption)//yerrorbarの上側（大きさではなく座標）
ADAPT_EXPORT ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(pointtype, int, plot_detail::PointOption)
ADAPT_EXPORT ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(pointsize, double, plot_detail::PointOption)
ADAPT_EXPORT ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(variable_size, plot_detail::AnyAcceptableArg, plot_detail::PointOption)

//VectorOption
ADAPT_EXPORT ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(xlen, plot_detail::AnyAcceptableArg, plot_detail::VectorOption)
ADAPT_EXPORT ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(ylen, plot_detail::AnyAcceptableArg, plot_detail::VectorOption)
ADAPT_EXPORT ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(zlen, plot_detail::AnyAcceptableArg, plot_detail::VectorOption)
ADAPT_EXPORT ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(arrowhead, ArrowHead, plot_detail::VectorOption)//arrowheadをどこに付けるか。head:先端、heads:両端、noheads:なし
ADAPT_EXPORT ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(arrowfill, ArrowFill, plot_detail::VectorOption)//arrowheadの塗りつぶし。filled:塗りつぶし、nofilled:塗りつぶしなし

//FillOption
ADAPT_EXPORT ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(fillpattern, int, plot_detail::FillOption)
ADAPT_EXPORT ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(fillsolid, double, plot_detail::FillOption)
ADAPT_EXPORT ADAPT_DEFINE_TAGGED_KEYWORD_OPTION(filltransparent, plot_detail::FillOption)
ADAPT_EXPORT ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(bordercolor, std::string_view, plot_detail::FillOption)
ADAPT_EXPORT ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(bordertype, int, plot_detail::FillOption)
ADAPT_EXPORT ADAPT_DEFINE_TAGGED_KEYWORD_OPTION(noborder, plot_detail::FillOption)

//FilledCurveOption
ADAPT_EXPORT ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(ybelow, plot_detail::AnyAcceptableArg, plot_detail::FilledCurveOption)
ADAPT_EXPORT ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(baseline, std::string_view, plot_detail::FilledCurveOption)
ADAPT_EXPORT ADAPT_DEFINE_TAGGED_KEYWORD_OPTION(closed, plot_detail::FilledCurveOption)
ADAPT_EXPORT ADAPT_DEFINE_TAGGED_KEYWORD_OPTION(above, plot_detail::FilledCurveOption)
ADAPT_EXPORT ADAPT_DEFINE_TAGGED_KEYWORD_OPTION(below, plot_detail::FilledCurveOption)

//Weight option for histogram and binscatter
ADAPT_EXPORT ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(weight, plot_detail::AnyAcceptableArg, plot_detail::WeightOption);// データに重み付けする。

//HistogramOption
ADAPT_EXPORT ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(min, double, plot_detail::HistogramOption);
ADAPT_EXPORT ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(max, double, plot_detail::HistogramOption);
ADAPT_EXPORT ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(nbin, size_t, plot_detail::HistogramOption);
ADAPT_EXPORT ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(data, plot_detail::AnyArithmeticRange, plot_detail::HistogramOption);
ADAPT_EXPORT ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(binerror, BinError, plot_detail::HistogramOption);
ADAPT_EXPORT ADAPT_DEFINE_TAGGED_KEYWORD_OPTION(cumul, plot_detail::HistogramOption);//cumulative histogram。
ADAPT_EXPORT ADAPT_DEFINE_TAGGED_KEYWORD_OPTION(inv_cumul, plot_detail::HistogramOption);//inverse cumulative histogram。
ADAPT_EXPORT ADAPT_DEFINE_TAGGED_KEYWORD_OPTION(stack, plot_detail::HistogramOption);//積み上げヒストグラム。指定されたヒストグラムは全て積み上げて表示される。

//BinscatterOption
ADAPT_EXPORT ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(datax, plot_detail::AnyArithmeticRange, plot_detail::BinscatterOption);
ADAPT_EXPORT ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(datay, plot_detail::AnyArithmeticRange, plot_detail::BinscatterOption);
ADAPT_EXPORT ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(xmin, double, plot_detail::BinscatterOption);
ADAPT_EXPORT ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(xmax, double, plot_detail::BinscatterOption);
ADAPT_EXPORT ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(xnbin, size_t, plot_detail::BinscatterOption);
ADAPT_EXPORT ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(ymin, double, plot_detail::BinscatterOption);
ADAPT_EXPORT ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(ymax, double, plot_detail::BinscatterOption);
ADAPT_EXPORT ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(ynbin, size_t, plot_detail::BinscatterOption);
ADAPT_EXPORT ADAPT_DEFINE_TAGGED_KEYWORD_OPTION(bs_points, plot_detail::BinscatterOption);//binscatterだが、散布図の各点に密度に対応する色を付与する形で表現する。
ADAPT_EXPORT ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(bs_lower, double, plot_detail::BinscatterOption);//binscatterの下限値。これ以下の値は白色で表示される。
ADAPT_EXPORT ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(bs_upper, double, plot_detail::BinscatterOption);//binscatterの上限値。これ以上の値は白色で表示される。

//LabelOption
ADAPT_EXPORT ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(label, plot_detail::AnyAcceptableArg, plot_detail::LabelOption);
ADAPT_EXPORT ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(labelpos, LabelPos, plot_detail::LabelOption);
ADAPT_EXPORT ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(labelrotate, double, plot_detail::LabelOption);
ADAPT_EXPORT ADAPT_DEFINE_TAGGED_KEYWORD_OPTION(noenhanced, plot_detail::LabelOption);
ADAPT_EXPORT ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(labelfont, std::string_view, plot_detail::LabelOption);
ADAPT_EXPORT ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(labeloverlay, LabelOverlay, plot_detail::LabelOption);
ADAPT_EXPORT ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(labeloffset, ADAPT_TIE_ARGS(std::pair<double, double>), plot_detail::LabelOption);

//ColormapOption
ADAPT_EXPORT ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(map, plot_detail::AnyMatrix, plot_detail::ColormapOption)
ADAPT_EXPORT ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(xrange, plot_detail::AnyCoordRange, plot_detail::ColormapOption)
ADAPT_EXPORT ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(yrange, plot_detail::AnyCoordRange, plot_detail::ColormapOption)
ADAPT_EXPORT ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(xminmax, ADAPT_TIE_ARGS(std::pair<double, double>), plot_detail::ColormapOption)
ADAPT_EXPORT ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(yminmax, ADAPT_TIE_ARGS(std::pair<double, double>), plot_detail::ColormapOption)
//options for contour plot
ADAPT_EXPORT ADAPT_DEFINE_TAGGED_KEYWORD_OPTION(with_contour, plot_detail::ColormapOption)
ADAPT_EXPORT ADAPT_DEFINE_TAGGED_KEYWORD_OPTION(without_surface, plot_detail::ColormapOption)
ADAPT_EXPORT ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(cntrsmooth, CntrSmooth, plot_detail::ColormapOption)
ADAPT_EXPORT ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(cntrpoints, int, plot_detail::ColormapOption)//the number of lines for cspline and bspline
ADAPT_EXPORT ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(cntrorder, int, plot_detail::ColormapOption)//order for bspline, [2, 10]
ADAPT_EXPORT ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(cntrlevels_auto, int, plot_detail::ColormapOption)
ADAPT_EXPORT ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(cntrlevels_discrete, const std::vector<double>&, plot_detail::ColormapOption)
ADAPT_EXPORT ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(cntrlevels_incremental, ADAPT_TIE_ARGS(std::tuple<double, double, double>), plot_detail::ColormapOption)
ADAPT_EXPORT ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(cntrcolor, std::string_view, plot_detail::ColormapOption)
ADAPT_EXPORT ADAPT_DEFINE_TAGGED_KEYWORD_OPTION(variable_cntrcolor, plot_detail::ColormapOption)
ADAPT_EXPORT ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(cntrlinetype, int, plot_detail::ColormapOption)
ADAPT_EXPORT ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(cntrlinewidth, double, plot_detail::ColormapOption)

//SurfaceOption
ADAPT_EXPORT ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(pm3d_at, Pm3dPosition, plot_detail::SurfaceOption)

//MultiplotOption
ADAPT_EXPORT ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(page_title, std::string_view, plot_detail::MultiplotOption)
ADAPT_EXPORT ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(fillorder, MPFillOrder, plot_detail::MultiplotOption)
ADAPT_EXPORT ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(vertical_direction, MPVerticalDirection, plot_detail::MultiplotOption)
ADAPT_EXPORT ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(multiplot_size, ADAPT_TIE_ARGS(std::pair<double, double>), plot_detail::MultiplotOption)

// タイトルなし指定の短縮版
ADAPT_EXPORT inline constexpr auto notitle = (title = "notitle");
ADAPT_EXPORT inline constexpr auto t_entries = (title_add_entries = 1);

// スタイル指定の短縮版
ADAPT_EXPORT inline constexpr auto s_lines = (style = Style::lines);
ADAPT_EXPORT inline constexpr auto s_points = (style = Style::points);
ADAPT_EXPORT inline constexpr auto s_linespoints = (style = Style::linespoints);
ADAPT_EXPORT inline constexpr auto s_dots = (style = Style::dots);
ADAPT_EXPORT inline constexpr auto s_impulses = (style = Style::impulses);
ADAPT_EXPORT inline constexpr auto s_boxes = (style = Style::boxes);
ADAPT_EXPORT inline constexpr auto s_steps = (style = Style::steps);
ADAPT_EXPORT inline constexpr auto s_fsteps = (style = Style::fsteps);
ADAPT_EXPORT inline constexpr auto s_histeps = (style = Style::histeps);
ADAPT_EXPORT inline constexpr auto s_pm3d = (style = Style::pm3d);

// 軸指定の短縮版
ADAPT_EXPORT inline constexpr auto ax_x1y1 = (axis = "x1y1");
ADAPT_EXPORT inline constexpr auto ax_x1y2 = (axis = "x1y2");
ADAPT_EXPORT inline constexpr auto ax_x2y1 = (axis = "x2y1");
ADAPT_EXPORT inline constexpr auto ax_x2y2 = (axis = "x2y2");


// 線の太さ指定の短縮版
ADAPT_EXPORT inline constexpr auto lw_ex_thick = (linewidth = 3.0);
ADAPT_EXPORT inline constexpr auto lw_thick = (linewidth = 2.0);
ADAPT_EXPORT inline constexpr auto lw_med_thick = (linewidth = 1.5);
ADAPT_EXPORT inline constexpr auto lw_medium = (linewidth = 1.0);
ADAPT_EXPORT inline constexpr auto lw_med_fine = (linewidth = 0.7);
ADAPT_EXPORT inline constexpr auto lw_fine = (linewidth = 0.5);
ADAPT_EXPORT inline constexpr auto lw_ex_fine = (linewidth = 0.3);

// 色指定の短縮版
// グレースケール
ADAPT_EXPORT inline constexpr auto c_black = (color = "black");
ADAPT_EXPORT inline constexpr auto c_dark_gray = (color = "dark-gray");
ADAPT_EXPORT inline constexpr auto c_gray = (color = "gray");
ADAPT_EXPORT inline constexpr auto c_light_gray = (color = "light-gray");
ADAPT_EXPORT inline constexpr auto c_white = (color = "white");

// 赤、マゼンタ、青、シアン、緑、黄の基本色。
ADAPT_EXPORT inline constexpr auto c_red = (color = "red");
ADAPT_EXPORT inline constexpr auto c_magenta = (color = "magenta");
ADAPT_EXPORT inline constexpr auto c_blue = (color = "blue");
ADAPT_EXPORT inline constexpr auto c_cyan = (color = "cyan");
ADAPT_EXPORT inline constexpr auto c_green = (color = "green");
ADAPT_EXPORT inline constexpr auto c_yellow = (color = "yellow");

ADAPT_EXPORT inline constexpr auto c_light_red = (color = "light-red");
ADAPT_EXPORT inline constexpr auto c_light_magenta = (color = "light-magenta");
ADAPT_EXPORT inline constexpr auto c_light_blue = (color = "light-blue");
ADAPT_EXPORT inline constexpr auto c_light_cyan = (color = "light-cyan");
ADAPT_EXPORT inline constexpr auto c_light_green = (color = "light-green");
ADAPT_EXPORT inline constexpr auto c_light_yellow = (color_rgb = "#ffffe0");

ADAPT_EXPORT inline constexpr auto c_dark_red = (color = "dark-red");
ADAPT_EXPORT inline constexpr auto c_dark_magenta = (color = "dark-magenta");
ADAPT_EXPORT inline constexpr auto c_dark_blue = (color = "dark-blue");
ADAPT_EXPORT inline constexpr auto c_dark_cyan = (color = "dark-cyan");
ADAPT_EXPORT inline constexpr auto c_dark_green = (color = "dark-green");
ADAPT_EXPORT inline constexpr auto c_dark_yellow = (color = "dark-yellow");

// Oklch色空間に基づき概ね同じ明るさで色相を一定ずつ変化させ生成した色。
ADAPT_EXPORT inline constexpr auto c_light_rose = (color = "#ffb1c8");
ADAPT_EXPORT inline constexpr auto c_light_crimson = (color = "#ffb3b8");
ADAPT_EXPORT inline constexpr auto c_light_vermilion = (color = "#ffb5a9");
ADAPT_EXPORT inline constexpr auto c_light_persimmon = (color = "#ffb797");
ADAPT_EXPORT inline constexpr auto c_light_marigold = (color = "#ffba81");
ADAPT_EXPORT inline constexpr auto c_light_amber = (color = "#ffbd58");
ADAPT_EXPORT inline constexpr auto c_light_goldenrod = (color = "#f0c543");
ADAPT_EXPORT inline constexpr auto c_light_olive = (color = "#d9cf4b");
ADAPT_EXPORT inline constexpr auto c_light_mossgreen = (color = "#bed95f");
ADAPT_EXPORT inline constexpr auto c_light_leafgreen = (color = "#9fe079");
ADAPT_EXPORT inline constexpr auto c_light_jade = (color = "#7ce695");
ADAPT_EXPORT inline constexpr auto c_light_emerald = (color = "#52e9b2");
ADAPT_EXPORT inline constexpr auto c_light_aquamarine = (color = "#17eace");
ADAPT_EXPORT inline constexpr auto c_light_turquoise = (color = "#00e7e7");
ADAPT_EXPORT inline constexpr auto c_light_teal = (color = "#00e3fe");
ADAPT_EXPORT inline constexpr auto c_light_cerulean = (color = "#76d9ff");
ADAPT_EXPORT inline constexpr auto c_light_azure = (color = "#94d3ff");
ADAPT_EXPORT inline constexpr auto c_light_cobalt = (color = "#a7ceff");
ADAPT_EXPORT inline constexpr auto c_light_royalblue = (color = "#b6c9ff");
ADAPT_EXPORT inline constexpr auto c_light_ultramarine = (color = "#c4c4ff");
ADAPT_EXPORT inline constexpr auto c_light_amethyst = (color = "#d4bdff");
ADAPT_EXPORT inline constexpr auto c_light_orchid = (color = "#e7b3ff");
ADAPT_EXPORT inline constexpr auto c_light_fuchsia = (color = "#ffa7f7");
ADAPT_EXPORT inline constexpr auto c_light_pink = (color = "#ffaddb");

ADAPT_EXPORT inline constexpr auto c_rose = (color = "#f05290");
ADAPT_EXPORT inline constexpr auto c_crimson = (color = "#f7526d");
ADAPT_EXPORT inline constexpr auto c_vermilion = (color = "#f85743");
ADAPT_EXPORT inline constexpr auto c_persimmon = (color = "#f06400");
ADAPT_EXPORT inline constexpr auto c_marigold = (color = "#d97900");
ADAPT_EXPORT inline constexpr auto c_amber = (color = "#c78600");
ADAPT_EXPORT inline constexpr auto c_goldenrod = (color = "#b69000");
ADAPT_EXPORT inline constexpr auto c_olive = (color = "#a39900");
ADAPT_EXPORT inline constexpr auto c_mossgreen = (color = "#89a200");
ADAPT_EXPORT inline constexpr auto c_leafgreen = (color = "#5aae00");
ADAPT_EXPORT inline constexpr auto c_jade = (color = "#00b352");
ADAPT_EXPORT inline constexpr auto c_emerald = (color = "#00b07f");
ADAPT_EXPORT inline constexpr auto c_aquamarine = (color = "#00ad98");
ADAPT_EXPORT inline constexpr auto c_turquoise = (color = "#00abab");
ADAPT_EXPORT inline constexpr auto c_teal = (color = "#00a8bc");
ADAPT_EXPORT inline constexpr auto c_cerulean = (color = "#00a4d0");
ADAPT_EXPORT inline constexpr auto c_azure = (color = "#009fe9");
ADAPT_EXPORT inline constexpr auto c_cobalt = (color = "#3d95ff");
ADAPT_EXPORT inline constexpr auto c_royalblue = (color = "#6d8bff");
ADAPT_EXPORT inline constexpr auto c_ultramarine = (color = "#8d80ff");
ADAPT_EXPORT inline constexpr auto c_amethyst = (color = "#aa71fc");
ADAPT_EXPORT inline constexpr auto c_orchid = (color = "#c267e8");
ADAPT_EXPORT inline constexpr auto c_fuchsia = (color = "#d65ece");
ADAPT_EXPORT inline constexpr auto c_pink = (color = "#e557b1");

ADAPT_EXPORT inline constexpr auto c_dark_rose = (color = "#951d52");
ADAPT_EXPORT inline constexpr auto c_dark_crimson = (color = "#9a1c37");
ADAPT_EXPORT inline constexpr auto c_dark_vermilion = (color = "#9b2014");
ADAPT_EXPORT inline constexpr auto c_dark_persimmon = (color = "#8c3700");
ADAPT_EXPORT inline constexpr auto c_dark_marigold = (color = "#7f4400");
ADAPT_EXPORT inline constexpr auto c_dark_amber = (color = "#734c00");
ADAPT_EXPORT inline constexpr auto c_dark_goldenrod = (color = "#695200");
ADAPT_EXPORT inline constexpr auto c_dark_olive = (color = "#5e5800");
ADAPT_EXPORT inline constexpr auto c_dark_mossgreen = (color = "#4e5d00");
ADAPT_EXPORT inline constexpr auto c_dark_leafgreen = (color = "#316400");
ADAPT_EXPORT inline constexpr auto c_dark_jade = (color = "#00672d");
ADAPT_EXPORT inline constexpr auto c_dark_emerald = (color = "#006548");
ADAPT_EXPORT inline constexpr auto c_dark_aquamarine = (color = "#006457");
ADAPT_EXPORT inline constexpr auto c_dark_turquoise = (color = "#006262");
ADAPT_EXPORT inline constexpr auto c_dark_teal = (color = "#00616d");
ADAPT_EXPORT inline constexpr auto c_dark_cerulean = (color = "#005f79");
ADAPT_EXPORT inline constexpr auto c_dark_azure = (color = "#005b88");
ADAPT_EXPORT inline constexpr auto c_dark_cobalt = (color = "#0053a5");
ADAPT_EXPORT inline constexpr auto c_dark_royalblue = (color = "#3549ac");
ADAPT_EXPORT inline constexpr auto c_dark_ultramarine = (color = "#5040a8");
ADAPT_EXPORT inline constexpr auto c_dark_amethyst = (color = "#65389f");
ADAPT_EXPORT inline constexpr auto c_dark_orchid = (color = "#753091");
ADAPT_EXPORT inline constexpr auto c_dark_fuchsia = (color = "#83297e");
ADAPT_EXPORT inline constexpr auto c_dark_pink = (color = "#8e2269");

ADAPT_EXPORT inline constexpr auto c_muted_rose = (color = "#714754");
ADAPT_EXPORT inline constexpr auto c_muted_crimson = (color = "#73474a");
ADAPT_EXPORT inline constexpr auto c_muted_vermilion = (color = "#734841");
ADAPT_EXPORT inline constexpr auto c_muted_persimmon = (color = "#714a39");
ADAPT_EXPORT inline constexpr auto c_muted_marigold = (color = "#6e4d32");
ADAPT_EXPORT inline constexpr auto c_muted_amber = (color = "#69502e");
ADAPT_EXPORT inline constexpr auto c_muted_goldenrod = (color = "#62542c");
ADAPT_EXPORT inline constexpr auto c_muted_olive = (color = "#5a572e");
ADAPT_EXPORT inline constexpr auto c_muted_mossgreen = (color = "#515a33");
ADAPT_EXPORT inline constexpr auto c_muted_leafgreen = (color = "#475d3b");
ADAPT_EXPORT inline constexpr auto c_muted_jade = (color = "#3c5f43");
ADAPT_EXPORT inline constexpr auto c_muted_emerald = (color = "#32604d");
ADAPT_EXPORT inline constexpr auto c_muted_aquamarine = (color = "#2a6056");
ADAPT_EXPORT inline constexpr auto c_muted_turquoise = (color = "#256060");
ADAPT_EXPORT inline constexpr auto c_muted_teal = (color = "#265e68");
ADAPT_EXPORT inline constexpr auto c_muted_cerulean = (color = "#2c5c6e");
ADAPT_EXPORT inline constexpr auto c_muted_azure = (color = "#345a73");
ADAPT_EXPORT inline constexpr auto c_muted_cobalt = (color = "#3e5776");
ADAPT_EXPORT inline constexpr auto c_muted_royalblue = (color = "#485477");
ADAPT_EXPORT inline constexpr auto c_muted_ultramarine = (color = "#525176");
ADAPT_EXPORT inline constexpr auto c_muted_amethyst = (color = "#5a4e72");
ADAPT_EXPORT inline constexpr auto c_muted_orchid = (color = "#624b6c");
ADAPT_EXPORT inline constexpr auto c_muted_fuchsia = (color = "#684965");
ADAPT_EXPORT inline constexpr auto c_muted_pink = (color = "#6d485d");


// ポイントタイプ指定の短縮版
ADAPT_EXPORT inline constexpr auto pt_dot = (pointtype = 0);//dot
ADAPT_EXPORT inline constexpr auto pt_plus = (pointtype = 1);//+
ADAPT_EXPORT inline constexpr auto pt_cross = (pointtype = 2);//x
ADAPT_EXPORT inline constexpr auto pt_star = (pointtype = 3);//+ and x
ADAPT_EXPORT inline constexpr auto pt_box = (pointtype = 4);//□
ADAPT_EXPORT inline constexpr auto pt_fbox = (pointtype = 5);//■
ADAPT_EXPORT inline constexpr auto pt_cir = (pointtype = 6);//○
ADAPT_EXPORT inline constexpr auto pt_fcir = (pointtype = 7);//●
ADAPT_EXPORT inline constexpr auto pt_tri = (pointtype = 8);//△
ADAPT_EXPORT inline constexpr auto pt_ftri = (pointtype = 9);//▲
ADAPT_EXPORT inline constexpr auto pt_dtri = (pointtype = 10);//▽
ADAPT_EXPORT inline constexpr auto pt_fdtri = (pointtype = 11);//▼
ADAPT_EXPORT inline constexpr auto pt_dia = (pointtype = 12);//◇
ADAPT_EXPORT inline constexpr auto pt_fdia = (pointtype = 13);//?
ADAPT_EXPORT inline constexpr auto pt_pent = (pointtype = 14);//?
ADAPT_EXPORT inline constexpr auto pt_fpent = (pointtype = 15);//?

// ポイントサイズ指定の短縮版
ADAPT_EXPORT inline constexpr auto ps_ex_large = (pointsize = 3.0);
ADAPT_EXPORT inline constexpr auto ps_large = (pointsize = 2.0);
ADAPT_EXPORT inline constexpr auto ps_med_large = (pointsize = 1.5);
ADAPT_EXPORT inline constexpr auto ps_medium = (pointsize = 1.0);
ADAPT_EXPORT inline constexpr auto ps_med_small = (pointsize = 0.7);
ADAPT_EXPORT inline constexpr auto ps_small = (pointsize = 0.5);
ADAPT_EXPORT inline constexpr auto ps_ex_small = (pointsize = 0.3);

// アロースタイル指定の短縮版
ADAPT_EXPORT inline constexpr auto as_head = (arrowhead = ArrowHead::head);
ADAPT_EXPORT inline constexpr auto as_heads = (arrowhead = ArrowHead::heads);
ADAPT_EXPORT inline constexpr auto as_noheads = (arrowhead = ArrowHead::nohead);
ADAPT_EXPORT inline constexpr auto as_filled = (arrowfill = ArrowFill::filled);
ADAPT_EXPORT inline constexpr auto as_nofilled = (arrowfill = ArrowFill::nofilled);
ADAPT_EXPORT inline constexpr auto as_empty = (arrowfill = ArrowFill::empty);


// ヒストグラムのビンのエラーバーの短縮版
ADAPT_EXPORT inline constexpr auto he_poisson = (binerror = BinError::poisson68);
ADAPT_EXPORT inline constexpr auto he_poisson95 = (binerror = BinError::poisson95);
ADAPT_EXPORT inline constexpr auto he_normal = (binerror = BinError::normal68);
ADAPT_EXPORT inline constexpr auto he_normal95 = (binerror = BinError::normal95);
ADAPT_EXPORT inline constexpr auto he_none = (binerror = BinError::none);

// binscatterの下限無効化の短縮版
// デフォルトではビン内の点数が0だと白色で表示されるようになっているが、これを無効化する。
ADAPT_EXPORT inline constexpr auto bs_no_lowlim = (bs_lower = std::numeric_limits<double>::lowest());


// ラベルの位置指定の短縮版
ADAPT_EXPORT inline constexpr auto lp_left = (labelpos = LabelPos::left);
ADAPT_EXPORT inline constexpr auto lp_center = (labelpos = LabelPos::center);
ADAPT_EXPORT inline constexpr auto lp_right = (labelpos = LabelPos::right);

// ラベル回転指定の短縮版
ADAPT_EXPORT inline constexpr auto lr_0 = (labelrotate = 0.0);
ADAPT_EXPORT inline constexpr auto lr_45 = (labelrotate = 45.0);
ADAPT_EXPORT inline constexpr auto lr_90 = (labelrotate = 90.0);
ADAPT_EXPORT inline constexpr auto lr_135 = (labelrotate = 135.0);
ADAPT_EXPORT inline constexpr auto lr_180 = (labelrotate = 180.0);
ADAPT_EXPORT inline constexpr auto lr_m45 = (labelrotate = -45.0);
ADAPT_EXPORT inline constexpr auto lr_m90 = (labelrotate = -90.0);
ADAPT_EXPORT inline constexpr auto lr_m135 = (labelrotate = -135.0);

// ラベルオーバーレイ指定の短縮版
ADAPT_EXPORT inline constexpr auto lo_front = (labeloverlay = LabelOverlay::front);
ADAPT_EXPORT inline constexpr auto lo_back = (labeloverlay = LabelOverlay::back);

// PM3Dの位置指定の短縮版
ADAPT_EXPORT inline constexpr auto pm3d_bottom = (pm3d_at = Pm3dPosition::bottom);
ADAPT_EXPORT inline constexpr auto pm3d_surface = (pm3d_at = Pm3dPosition::surface);
ADAPT_EXPORT inline constexpr auto pm3d_top = (pm3d_at = Pm3dPosition::top);

// マルチプロットの配置順序指定の短縮版
ADAPT_EXPORT inline constexpr auto rowsfirst = (fillorder = MPFillOrder::rowsfirst);
ADAPT_EXPORT inline constexpr auto colsfirst = (fillorder = MPFillOrder::colsfirst);
ADAPT_EXPORT inline constexpr auto downwards = (vertical_direction = MPVerticalDirection::downwards);
ADAPT_EXPORT inline constexpr auto upwards = (vertical_direction = MPVerticalDirection::upwards);

}

}

#endif