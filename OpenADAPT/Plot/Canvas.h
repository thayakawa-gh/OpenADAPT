#ifndef ADAPT_PLOT_CANVAS_H
#define ADAPT_PLOT_CANVAS_H

#include <OpenADAPT/Utility/Matrix.h>
#include <OpenADAPT/Utility/Function.h>
#include <OpenADAPT/Utility/KeywordArgs.h>
#include <OpenADAPT/Plot/Core.h>
#include <OpenADAPT/Plot/ArrayData.h>

namespace adapt
{

namespace detail
{

using RangeReceiver = ArrayData::RangeReceiver;

template <class OutputFunc>
inline void MakeDataObjectCommon(OutputFunc output_func, std::vector<ArrayData::Range>& its)
{
	while (true)
	{
		size_t eod_count = 0;
		for (auto& it : its)
		{
			it.Output(output_func);
			++it;
			eod_count += it.End() ? 1 : 0;//いずれか1つでも終端に達したら打ち切る。
		}
		output_func("\n", print::end<>);
		if (eod_count)
		{
			if (eod_count != its.size())
				std::cout <<
					"WARNING : The numbers of data points are inconsistent."
					" The trailing data points are truncated to fit the smallest data set." << std::endl;
			break;
		}
	}
}

template <class OutputFunc, class RangeX, class RangeY>
inline void MakeDataObjectCommon(OutputFunc output_func, MatrixData::MatRef map, RangeX rx, RangeY ry)
{
	//uint32_t xsize = map.GetSize(0);
	//uint32_t ysize = map.GetSize(1);
	auto [xsize, ysize] = map.GetSize();
	typename RangeY::iterator ity(ry);
	//xsize、ysizeはxcoord.size()-1、ycoord.size()-1にそれぞれ等しいはず。
	for (uint32_t iy = 0; iy < ysize && !ity.End(); ++iy, ++ity)
	{
		auto [y, cy] = *ity;
		typename RangeX::iterator itx(rx);
		for (uint32_t ix = 0; ix < xsize; ++ix, ++itx)
		{
			if (itx.End())
			{
				std::cout <<
					"WARNING : The numbers of datapoints are inconsistent."
					" The trailing data points are truncated to fit the smallest data set." << std::endl;
			}
			auto [x, cx] = *itx;
			map.Out(output_func, x, y, cx, cy, ix, iy);
		}
		auto [x, cx] = *itx;
		output_func(x, y, cx, cy, " 0\n");
	}
	typename RangeX::iterator itx(rx);
	auto [y, cy] = *ity;
	for (uint32_t ix = 0; ix < xsize && !itx.End(); ++ix, ++itx)
	{
		auto [x, cx] = *itx;
		output_func(x, y, cx, cy, " 0");
	}
	auto [x, cx] = *itx;
	output_func(x, y, cx, cy, " 0");
}
template <class ...Args>
inline void MakeDataObject(Canvas* g, const std::string& name, Args&& ...args)
{
	if (g->IsInMemoryDataTransferEnabled()) 
	{
		// make datablock
		g->Command(name + " << EOD");
		ToFPtr output_func{ g->GetPipe() };
		MakeDataObjectCommon(output_func, std::forward<Args>(args)...);
		g->Command("EOD");
	}
	else
	{
		// make file
		std::ofstream ofs(name);
		if (!ofs) throw InvalidArg("file \"" + name + "\" cannot open.");
		ToOStr output_func{ ofs };
		MakeDataObjectCommon(output_func, std::forward<Args>(args)...);
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

template <class PointParam>
std::string PointPlotCommand(const PointParam& p)
{
	std::string c;
	if (p.m_x_errorbar || p.m_y_errorbar)
	{
		if (p.m_style == Style::lines)
		{
			if (p.m_x_errorbar && p.m_y_errorbar)
				c += " xyerrorlines";
			else if (p.m_x_errorbar)
				c += " xerrorlines";
			else
				c += " yerrorlines";
			if (p.m_line_type != -2) c += " linetype " + std::to_string(p.m_line_type);
			if (p.m_line_width != -1) c += " linewidth " + std::to_string(p.m_line_width);
			if (!p.m_dash_type.empty())
			{
				c += " dashtype (" + std::to_string(p.m_dash_type.front());
				for (size_t i = 1; i < p.m_dash_type.size(); ++i) c += ", " + std::to_string(p.m_dash_type[i]);
				c += ")";
			}
			if (!p.m_color.empty()) c += " linecolor '" + p.m_color + "'";
			else if (p.m_variable_color) c += " linecolor palette";
		}
		else if (p.m_style == Style::boxes)
		{
			//c += " boxes";
			if (p.m_x_errorbar && p.m_y_errorbar)
				c += " boxxyerrorbars";
			else if (p.m_x_errorbar)
				std::cerr << "WARNING : Box style is incompatible with xerrorbar option. It is to be ignored." << std::endl;
			else
				c += " boxerrorbars";
			if (p.m_line_type != -2) c += " linetype " + std::to_string(p.m_line_type);
			if (p.m_line_width != -1) c += " linewidth " + std::to_string(p.m_line_width);
			if (!p.m_color.empty()) c += " linecolor '" + p.m_color + "'";
			else if (p.m_variable_color) c += " linecolor palette";
		}
		else
		{
			if (p.m_style != Style::points)
				std::cerr << "WARNING : Only \"lines\", \"boxes\" or \"points\" styles are allowed with errobars. Style option is to be ignored." << std::endl;

			//c += " points";
			if (p.m_x_errorbar && p.m_y_errorbar)
				c += " xyerrorbars";
			else if (p.m_x_errorbar)
				c += " xerrorbars";
			else if (p.m_y_errorbar)
				c += " yerrorbars";
			if (p.m_point_type != -1) c += " pointtype " + std::to_string(p.m_point_type);
			if (p.m_point_size != -1) c += " pointsize " + std::to_string(p.m_point_size);
			if (p.m_line_width != -1) c += " linewidth " + std::to_string(p.m_line_width);
			if (!p.m_color.empty()) c += " linecolor '" + p.m_color + "'";//pointのときも何故かlinecolorらしい。
			else if (p.m_variable_color) c += " palette";//しかしpalette指定の場合はlinecolorがいらない。謎。
		}
	}
	//ベクトル、エラーバー指定がない場合。
	else if (p.m_style == Style::lines ||
			 p.m_style == Style::impulses ||
			 p.m_style == Style::steps ||
			 p.m_style == Style::fsteps ||
			 p.m_style == Style::histeps ||
			 p.m_style == Style::boxes)
	{
		std::string style;
		std::string others;
		switch (p.m_style)
		{
		case Style::lines: style = " lines"; break;
		case Style::impulses: style = " impulses"; break;
		case Style::steps: style = " steps"; break;
		case Style::fsteps: style = " fsteps"; break;
		case Style::histeps: style = " histeps"; break;
		case Style::boxes: style = " boxes"; break;
		default: break;
		}
		if (p.m_line_type != -2) others += " linetype " + std::to_string(p.m_line_type);
		if (p.m_line_width != -1) others += " linewidth " + std::to_string(p.m_line_width);
		if (!p.m_dash_type.empty())
		{
			others += " dashtype (" + std::to_string(p.m_dash_type.front());
			for (size_t i = 1; i < p.m_dash_type.size(); ++i) others += ", " + std::to_string(p.m_dash_type[i]);
			others += ")";
		}
		if (!p.m_color.empty()) others += " linecolor '" + p.m_color + "'";
		else if (p.m_variable_color) others += " linecolor palette";

		if (p.m_style == Style::boxes || p.m_style == Style::steps)
		{
			if (p.m_style == Style::steps) style = " fillsteps";
			//現状、fill系オプションはboxesまたはstepsにしか使えない。
			if (!p.m_fill_color.empty()) others += " fillcolor '" + p.m_fill_color + "'";
			else if (p.m_variable_fill_color) others += " fillcolor palette";
			{
				std::string fs;
				if (p.m_transparent) fs += " transparent";
				if (p.m_solid != -1) fs += std::format(" solid {}", p.m_solid);
				else if (p.m_pattern != -1) fs += std::format(" pattern {}", p.m_pattern);
				if (!fs.empty()) others += " fs" + fs;
			}
			{
				std::string bd;
				if (p.m_border_type == -2) bd += " noborder";
				else if (p.m_border_type != -3) bd += std::format(" {}", p.m_border_type);
				if (!p.m_border_color.empty()) bd += " linecolor '" + p.m_border_color + "'";
				if (!bd.empty()) others += " border" + bd;
			}
		}
		c += style;
		c += others;
	}
	else if (p.m_style == Style::points)
	{
		c += " points";
		if (p.m_line_type != -2)
			std::cerr << "WARNING : \"points\" style is incompatible with linetype option. It is to be ignored." << std::endl;
		if (p.m_line_width != -1)
			std::cerr << "WARNING : \"points\" style is incompatible with linewidth option. It is to be ignored." << std::endl;
		if (p.m_point_type != -1) c += " pointtype " + std::to_string(p.m_point_type);
		if (p.m_point_size != -1) c += " pointsize " + std::to_string(p.m_point_size);
		if (!p.m_color.empty()) c += " linecolor '" + p.m_color + "'";//pointのときも何故かlinecolorらしい。
		else if (p.m_variable_color) c += " palette";//しかしpalette指定の場合はlinecolorがいらない。謎。
	}
	else if (p.m_style == Style::linespoints)
	{
		c += " linespoints";
		if (p.m_line_type != -2) c += " linetype " + std::to_string(p.m_line_type);
		if (p.m_line_width != -1) c += " linewidth " + std::to_string(p.m_line_width);
		if (p.m_point_type != -1) c += " pointtype " + std::to_string(p.m_point_type);
		if (p.m_point_size != -1) c += " pointsize " + std::to_string(p.m_point_size);
		if (!p.m_dash_type.empty())
		{
			c += " dashtype (" + std::to_string(p.m_dash_type.front());
			for (size_t i = 1; i < p.m_dash_type.size(); ++i) c += ", " + std::to_string(p.m_dash_type[i]);
			c += ")";
		}
		if (!p.m_color.empty()) c += " linecolor '" + p.m_color + "'";
		else if (p.m_variable_color) c += " linecolor palette";
	}
	else if (p.m_style == Style::dots)
	{
		c += " dots";
		if (p.m_line_type != -2)
			std::cerr << "WARNING : \"points\" style is incompatible with linetype option. It is to be ignored." << std::endl;
		if (p.m_line_width != -1)
			std::cerr << "WARNING : \"points\" style is incompatible with linewidth option. It is to be ignored." << std::endl;
		if (p.m_point_type != -1)
			std::cerr << "WARNING : \"dots\" style is incompatible with pointtype option. It is to be ignored." << std::endl;
		if (p.m_point_size != -1)
			std::cerr << "WARNING : \"dots\" style is incompatible with pointsize option. Use points with pointtype 7." << std::endl;
		if (!p.m_color.empty()) c += " linecolor '" + p.m_color + "'";
		else if (p.m_variable_color) c += " palette ";
	}
	if (p.m_smooth != Smooth::none)
	{
		c += " smooth";
		switch (p.m_smooth)
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
template<class VectorParam>
std::string VectorPlotCommand(const VectorParam& v)
{
	std::string c;
	c += " vector ";
	if (v.m_arrow_head != -1)
	{
		if ((v.m_arrow_head & 0b11) == 0) c += " head";
		else if ((v.m_arrow_head & 0b11) == 1) c += " heads";
		else if ((v.m_arrow_head & 0b11) == 2) c += " noheads";
		if ((v.m_arrow_head & 0b1100) == 0) c += " filled";
		else if ((v.m_arrow_head & 0b1100) == 4) c += " empty";
		else if ((v.m_arrow_head & 0b1100) == 8) c += " nofilled";
	}
	if (v.m_line_type != -2) c += " linetype " + std::to_string(v.m_line_type);
	if (v.m_line_width != -1) c += " linewidth " + std::to_string(v.m_line_width);
	if (!v.m_color.empty()) c += " linecolor '" + v.m_color + "'";
	else if (v.m_variable_color) c += " linecolor palette";
	return c;
}
template <class FilledCurveParam>
std::string FilledCurveplotCommand(const FilledCurveParam& f)
{
	std::string c;
	c += " filledcurves";

	if (!f.m_closed && !f.m_above && !f.m_below &&
		f.m_base_line.empty())
	{
		//何も指定のないデフォルトの場合、x1軸との間の領域を塗りつぶす。
		c += " x1";
	}

	if (f.m_closed) c += " closed";
	else if (f.m_above) c += " above";
	else if (f.m_below) c += " below";
	if (!f.m_base_line.empty()) c += " " + f.m_base_line;

	if (!f.m_fill_color.empty()) c += " fillcolor '" + f.m_fill_color + "'";
	else if (f.m_variable_color) c += " fillcolor palette";
	{
		std::string fs;
		if (f.m_transparent) fs += " transparent";
		if (f.m_solid != -1) fs += std::format(" solid {}", f.m_solid);
		else if (f.m_pattern != -1) fs += std::format(" pattern {}", f.m_pattern);
		if (!fs.empty()) c += " fillstyle" + fs;
	}
	{
		std::string bd;
		if (f.m_border_type == -2) bd += " noborder";
		else if (f.m_border_type != -3) bd += std::format(" {}", f.m_border_type);
		if (!f.m_border_color.empty()) bd += " linecolor '" + f.m_border_color + "'";
		if (!bd.empty()) c += " border" + bd;
	}
	return c;
}
template <class GraphParam>
std::string ColormapPlotCommand(const GraphParam&)
{
	return std::string();
}

#define DEF_GPMAXIS(AXIS, axis)\
template <class GPM>\
class Axis##AXIS : public GPM\
{\
public:\
	using GPM::GPM;\
	void Set##AXIS##Label(const std::string& label);\
	void Set##AXIS##Range(double min, double max);\
	void Set##AXIS##RangeMin(double min);\
	void Set##AXIS##RangeMax(double max);\
	void SetLog##AXIS(double base = 0);\
	void SetFormat##AXIS(const std::string& fmt);\
	void Set##AXIS##DataTime(const std::string& fmt = std::string());\
	template <class ...Args>\
	void Set##AXIS##Tics(Args&& ...args);\
	void Set##AXIS##TicsRotate(double ang);\
};\
template <class GPM>\
inline void Axis##AXIS<GPM>::Set##AXIS##Label(const std::string& label) { this->SetLabel(axis, label); }\
template <class GPM>\
inline void Axis##AXIS<GPM>::Set##AXIS##Range(double min, double max) { this->SetRange(axis, min, max); }\
template <class GPM>\
inline void Axis##AXIS<GPM>::Set##AXIS##RangeMin(double min) { this->SetRangeMin(axis, min); }\
template <class GPM>\
inline void Axis##AXIS<GPM>::Set##AXIS##RangeMax(double max) { this->SetRangeMax(axis, max); }\
template <class GPM>\
inline void Axis##AXIS<GPM>::SetLog##AXIS(double base) { this->SetLog(axis, base); }\
template <class GPM>\
inline void Axis##AXIS<GPM>::SetFormat##AXIS(const std::string& fmt) { this->SetFormat(axis, fmt); }\
template <class GPM>\
inline void Axis##AXIS<GPM>::Set##AXIS##DataTime(const std::string& fmt) { this->SetDataTime(axis, fmt); }\
template <class GPM>\
template <class ...Args>\
inline void Axis##AXIS<GPM>::Set##AXIS##Tics(Args&& ...args) { this->SetTics(axis, std::forward<Args>(args)...); }\
template <class GPM>\
inline void Axis##AXIS<GPM>::Set##AXIS##TicsRotate(double ang) { this->SetTicsRotate(axis, ang); }

DEF_GPMAXIS(X, "x")
DEF_GPMAXIS(X2, "x2")
DEF_GPMAXIS(Y, "y")
DEF_GPMAXIS(Y2, "y2")
DEF_GPMAXIS(Z, "z")
DEF_GPMAXIS(Z2, "z2")
DEF_GPMAXIS(CB, "cb")

#undef DEF_GPMAXIS

template <class GPM>
struct Axis2D : public AxisX<AxisX2<AxisY<AxisY2<AxisCB<GPM>>>>>
{
	using Base = AxisX<AxisX2<AxisY<AxisY2<AxisCB<GPM>>>>>;
	using Base::Base;
};
template <class GPM>
struct Axis3D : public AxisZ<Axis2D<GPM>>
{
	using Base = AxisZ<Axis2D<GPM>>;
	using Base::Base;
};

}

namespace plot
{

struct BaseOption {};
struct FillOption : BaseOption {};
struct LineOption : BaseOption {};
struct PointOption : LineOption, FillOption {};
struct VectorOption : LineOption {};
struct FilledCurveOption : FillOption {};

ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(x, detail::ArrayData, BaseOption)
ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(y, detail::ArrayData, BaseOption)
ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(title, std::string_view, BaseOption)
ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(axis, std::string_view, BaseOption)//y2軸を使いたい場合などに、"x1y2"のように指定する。

ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(linetype, int, LineOption)
ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(linewidth, double, LineOption)
ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(dashtype, std::vector<int>, LineOption)
ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(color, std::string_view, LineOption)
ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(variable_color, detail::ArrayData, LineOption)

ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(style, Style, PointOption)
ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(pointtype, int, PointOption)
ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(pointsize, double, PointOption)
ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(smooth, Smooth, PointOption)
ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(xerrorbar, detail::ArrayData, PointOption)
ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(yerrorbar, detail::ArrayData, PointOption)
ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(variable_size, detail::ArrayData, PointOption)

ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(xlen, detail::ArrayData, VectorOption)
ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(ylen, detail::ArrayData, VectorOption)
ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(arrowhead, int, VectorOption)

ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(fillpattern, int, FillOption)
ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(fillsolid, double, FillOption)
ADAPT_DEFINE_TAGGED_KEYWORD_OPTION(filltransparent, FillOption)
ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(fillcolor, std::string_view, FillOption)
ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(variable_fillcolor, std::string_view, FillOption)
ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(bordercolor, std::string_view, FillOption)
ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(bordertype, int, FillOption)//linetype

ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(ybelow, detail::ArrayData, FilledCurveOption)
ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(baseline, std::string_view, FilledCurveOption)
ADAPT_DEFINE_TAGGED_KEYWORD_OPTION(closed, FilledCurveOption)
ADAPT_DEFINE_TAGGED_KEYWORD_OPTION(above, FilledCurveOption)
ADAPT_DEFINE_TAGGED_KEYWORD_OPTION(below, FilledCurveOption)


// タイトルなし指定の短縮版
inline constexpr auto no_title = (title = "notitle");

// 軸指定の短縮版
inline constexpr auto ax_x1y1 = (axis = "x1y1");
inline constexpr auto ax_x1y2 = (axis = "x1y2");
inline constexpr auto ax_x2y1 = (axis = "x2y1");
inline constexpr auto ax_x2y2 = (axis = "x2y2");


// 線の太さ指定の短縮版
inline constexpr auto lw_ex_thick  = (linewidth = 3.0);
inline constexpr auto lw_thick     = (linewidth = 2.0);
inline constexpr auto lw_med_thick = (linewidth = 1.5);
inline constexpr auto lw_medium    = (linewidth = 1.0);
inline constexpr auto lw_med_fine  = (linewidth = 0.7);
inline constexpr auto lw_fine      = (linewidth = 0.5);
inline constexpr auto lw_ex_fine   = (linewidth = 0.3);

// 色指定の短縮版
// 黒、白、灰、赤、青、緑、黄、マゼンタ、シアンが基本色。黒白以外はdark、lightの指定が可能。
inline constexpr auto c_black         = (color = "black");
inline constexpr auto c_dark_gray     = (color = "dark-gray");
inline constexpr auto c_gray          = (color = "gray");
inline constexpr auto c_light_gray    = (color = "light-gray");
inline constexpr auto c_white         = (color = "white");
inline constexpr auto c_dark_red      = (color = "dark-red");
inline constexpr auto c_red           = (color = "red");
inline constexpr auto c_light_red     = (color = "light-red");
inline constexpr auto c_dark_green    = (color = "dark-green");
inline constexpr auto c_green         = (color = "green");
inline constexpr auto c_light_green   = (color = "light-green");
inline constexpr auto c_dark_blue     = (color = "dark-blue");
inline constexpr auto c_blue          = (color = "blue");
inline constexpr auto c_light_blue    = (color = "light-blue");
inline constexpr auto c_dark_yellow   = (color = "dark-yellow");
inline constexpr auto c_yellow        = (color = "yellow");
inline constexpr auto c_light_yellow  = (color = "rgb #ffffe0");
inline constexpr auto c_dark_magenta  = (color = "dark-magenta");
inline constexpr auto c_magenta       = (color = "magenta");
inline constexpr auto c_light_magenta = (color = "light-magenta");
inline constexpr auto c_dark_cyan     = (color = "dark-cyan");
inline constexpr auto c_cyan          = (color = "cyan");
inline constexpr auto c_light_cyan    = (color = "light-cyan");


// スタイル指定の短縮版
inline constexpr auto s_lines       = (style = Style::lines);
inline constexpr auto s_points      = (style = Style::points);
inline constexpr auto s_linespoints = (style = Style::linespoints);
inline constexpr auto s_dots        = (style = Style::dots);
inline constexpr auto s_impulses    = (style = Style::impulses);
inline constexpr auto s_boxes       = (style = Style::boxes);
inline constexpr auto s_steps       = (style = Style::steps);
inline constexpr auto s_fsteps      = (style = Style::fsteps);
inline constexpr auto s_histeps     = (style = Style::histeps);

// ポイントタイプ指定の短縮版
inline constexpr auto pt_dot   = (pointtype = 0);//dot
inline constexpr auto pt_plus  = (pointtype = 1);//+
inline constexpr auto pt_cross = (pointtype = 2);//x
inline constexpr auto pt_star  = (pointtype = 3);//+ and x
inline constexpr auto pt_box   = (pointtype = 4);//□
inline constexpr auto pt_fbox  = (pointtype = 5);//■
inline constexpr auto pt_cir   = (pointtype = 6);//○
inline constexpr auto pt_fcir  = (pointtype = 7);//●
inline constexpr auto pt_tri   = (pointtype = 8);//△
inline constexpr auto pt_ftri  = (pointtype = 9);//▲
inline constexpr auto pt_dtri  = (pointtype = 10);//▽
inline constexpr auto pt_fdtri = (pointtype = 11);//▼
inline constexpr auto pt_dia   = (pointtype = 12);//◇
inline constexpr auto pt_fdia  = (pointtype = 13);//♦
inline constexpr auto pt_pent  = (pointtype = 14);//⬠
inline constexpr auto pt_fpent = (pointtype = 15);//⬟

// ポイントサイズ指定の短縮版
inline constexpr auto ps_ex_large  = (pointsize = 3.0);
inline constexpr auto ps_large     = (pointsize = 2.0);
inline constexpr auto ps_med_large = (pointsize = 1.5);
inline constexpr auto ps_medium    = (pointsize = 1.0);
inline constexpr auto ps_med_small = (pointsize = 0.7);
inline constexpr auto ps_small     = (pointsize = 0.5);
inline constexpr auto ps_ex_small  = (pointsize = 0.3);

}

namespace detail
{

struct PointParam
{
	PointParam()
		: m_line_type(-2), m_line_width(-1),
		m_pattern(-1), m_solid(-1), m_transparent(false), m_border_type(-3),
		m_style(Style::none), m_point_type(-1), m_point_size(-1), m_smooth(Smooth::none)
	{}

	template <class ...Ops>
	void SetOptions(Ops ...ops)
	{
		m_line_type = GetKeywordArg(plot::linetype, -2, ops...);
		m_line_width = GetKeywordArg(plot::linewidth, -1, ops...);
		m_dash_type = GetKeywordArg(plot::dashtype, std::vector<int>{}, ops...);
		m_color = GetKeywordArg(plot::color, "", ops...);

		m_fill_color = GetKeywordArg(plot::fillcolor, "", ops...);
		if (KeywordExists(plot::variable_fillcolor, ops...)) m_variable_fill_color = GetKeywordArg(plot::variable_fillcolor, ops...);
		m_pattern = GetKeywordArg(plot::fillpattern, -1, ops...);
		m_solid = GetKeywordArg(plot::fillsolid, -1, ops...);
		m_transparent = KeywordExists(plot::filltransparent, ops...);
		m_border_color = GetKeywordArg(plot::bordercolor, "", ops...);
		m_border_type = GetKeywordArg(plot::bordertype, -3, ops...);

		if (KeywordExists(plot::xerrorbar, ops...)) m_x_errorbar = GetKeywordArg(plot::xerrorbar, ops...);
		if (KeywordExists(plot::yerrorbar, ops...)) m_y_errorbar = GetKeywordArg(plot::yerrorbar, ops...);
		if (KeywordExists(plot::variable_color, ops...)) m_variable_color = GetKeywordArg(plot::variable_color, ops...);
		if (KeywordExists(plot::variable_size, ops...)) m_variable_size = GetKeywordArg(plot::variable_size, ops...);
		m_style = GetKeywordArg(plot::style, Style::points, ops...);
		m_smooth = GetKeywordArg(plot::smooth, Smooth::none, ops...);
		m_point_type = GetKeywordArg(plot::pointtype, -1, ops...);
		m_point_size = GetKeywordArg(plot::pointsize, -1, ops...);
	}

	//LineOption
	int m_line_type;//-2ならデフォルト
	double m_line_width;//-1ならデフォルト、-2ならvariable
	std::vector<int> m_dash_type;
	std::string m_color;
	detail::ArrayData m_variable_color;

	//FillOption
	std::string m_fill_color;
	detail::ArrayData m_variable_fill_color;
	int m_pattern;
	double m_solid;
	bool m_transparent;
	std::string m_border_color;
	int m_border_type;//-2はnorborderを意味する。

	//PointOption
	Style m_style;//デフォルトではPOINTS
	int m_point_type;//-1ならデフォルト
	double m_point_size;//-1ならデフォルト、-2ならvariable
	Smooth m_smooth;
	detail::ArrayData m_x;
	detail::ArrayData m_y;
	detail::ArrayData m_x_errorbar;
	detail::ArrayData m_y_errorbar;
	detail::ArrayData m_variable_size;
};
struct VectorParam
{
	VectorParam()
		: m_line_type(-2), m_line_width(-1), m_arrow_head(0)
	{}

	template <class ...Ops>
	void SetOptions(Ops ...ops)
	{
		m_line_type = GetKeywordArg(plot::linetype, -2, ops...);
		m_line_width = GetKeywordArg(plot::linewidth, -1, ops...);
		m_color = GetKeywordArg(plot::color, "", ops...);
		if (KeywordExists(plot::variable_color, ops...)) m_variable_color = GetKeywordArg(plot::variable_color, ops...);
	}

	//LineOption
	int m_line_type;//-2ならデフォルト
	double m_line_width;//-1ならデフォルト、-2ならvariable
	std::string m_color;
	detail::ArrayData m_variable_color;

	//VectorOption
	detail::ArrayData m_x;
	detail::ArrayData m_y;
	detail::ArrayData m_x_len;
	detail::ArrayData m_y_len;
	int m_arrow_head;//-1ならデフォルト。
};
struct FilledCurveParam
{
	FilledCurveParam()
		: m_pattern(-1), m_solid(-1), m_transparent(false), m_border_type(-3),
		m_closed(false), m_above(false), m_below(false)
	{}

	template <class ...Ops>
	void SetOptions(Ops ...ops)
	{
		m_closed = GetKeywordArg(plot::closed, false, ops...);
		m_above = GetKeywordArg(plot::above, false, ops...);
		m_below = GetKeywordArg(plot::below, false, ops...);
		m_fill_color = GetKeywordArg(plot::fillcolor, "", ops...);
		if (KeywordExists(plot::variable_fillcolor, ops...)) m_variable_color = GetKeywordArg(plot::variable_fillcolor, ops...);
		m_pattern = GetKeywordArg(plot::fillpattern, -1, ops...);
		m_solid = GetKeywordArg(plot::fillsolid, -1., ops...);
		m_transparent = KeywordExists(plot::filltransparent, ops...);
		m_border_color = GetKeywordArg(plot::bordercolor, "", ops...);
		m_border_type = GetKeywordArg(plot::bordertype, -3, ops...);
	}

	//FillOption
	std::string m_fill_color;
	detail::ArrayData m_variable_color;
	std::string m_base_line;
	int m_pattern;
	double m_solid;
	bool m_transparent;
	std::string m_border_color;
	int m_border_type;//-2はnorborderを意味する。

	//FilledCurveOption
	detail::ArrayData m_x;
	detail::ArrayData m_y;
	detail::ArrayData mY2;
	bool m_closed;
	bool m_above;
	bool m_below;
};

template <class ...Styles>
struct GraphParamBase : public std::variant<Styles...>
{
	GraphParamBase()
		: m_type(EQUATION)
	{}
	virtual ~GraphParamBase() = default;

	template <class ...Ops>
	void SetBaseOptions(Ops ...ops)
	{
		m_title = GetKeywordArg(plot::title, "", ops...);
		m_axis = GetKeywordArg(plot::axis, "", ops...);
	}

	enum Type : char { EQUATION, FILE, DATA, };
	char m_type;
	std::string m_graph;

	//Baseoption
	std::string m_title;
	std::string m_axis;

	std::vector<std::string> m_column;
};
struct GraphParam2D : public GraphParamBase<PointParam, VectorParam, FilledCurveParam>
{
	void AssignPoint() { this->emplace<PointParam>(); }
	void AssignVector() { this->emplace<VectorParam>(); }
	void AssignFilledCurve() { this->emplace<FilledCurveParam>(); }

	bool IsPoint() const { return this->index() == 0; }
	bool IsVector() const { return this->index() == 1; }
	bool IsFilledCurve() const { return this->index() == 2; }

	PointParam& GetPointParam() { return std::get<PointParam>(*this); }
	const PointParam& GetPointParam() const { return std::get<PointParam>(*this); }
	VectorParam& GetVectorParam() { return std::get<VectorParam>(*this); }
	const VectorParam& GetVectorParam() const { return std::get<VectorParam>(*this); }
	FilledCurveParam& GetFilledCurveParam() { return std::get<FilledCurveParam>(*this); }
	const FilledCurveParam& GetFilledCurveParam() const { return std::get<FilledCurveParam>(*this); }
};


template <class GraphParam>
struct PlotBuffer2D
{
	PlotBuffer2D(Canvas* g);
	PlotBuffer2D(const PlotBuffer2D&) = delete;
	PlotBuffer2D(PlotBuffer2D&& p) noexcept;
	PlotBuffer2D& operator=(const PlotBuffer2D&) = delete;
	PlotBuffer2D& operator=(PlotBuffer2D&& p) noexcept;
	virtual ~PlotBuffer2D();

	void Flush();

	template <class ...Options> requires all_keyword_args_tagged_with<plot::PointOption, Options...>
	PlotBuffer2D PlotPoints(RangeReceiver x, RangeReceiver y, Options ...ops);
	template <class ...Options> requires all_keyword_args_tagged_with<plot::PointOption, Options...>
	PlotBuffer2D PlotPoints(std::string_view filename, std::string_view xcol, std::string_view ycol, Options ...ops);
	template <class ...Options> requires all_keyword_args_tagged_with<plot::PointOption, Options...>
	PlotBuffer2D PlotPoints(std::string_view equation, Options ...ops);

	template <class ...Options> requires all_keyword_args_tagged_with<plot::PointOption, Options...>
	PlotBuffer2D PlotLines(RangeReceiver x, RangeReceiver y, Options ...ops);
	template <class ...Options> requires all_keyword_args_tagged_with<plot::PointOption, Options...>
	PlotBuffer2D PlotLines(std::string_view filename, std::string_view xcol, std::string_view ycol, Options ...ops);
	template <class ...Options> requires all_keyword_args_tagged_with<plot::PointOption, Options...>
	PlotBuffer2D PlotLines(std::string_view equation, Options ...ops);

	template <class ...Options> requires all_keyword_args_tagged_with<plot::VectorOption, Options...>
	PlotBuffer2D PlotVectors(RangeReceiver xfrom, RangeReceiver yfrom,
								RangeReceiver xlen, RangeReceiver ylen,
								Options ...ops);
	template <class ...Options> requires all_keyword_args_tagged_with<plot::VectorOption, Options...>
	PlotBuffer2D PlotVectors(std::string_view filename,
								std::string_view xbegin, std::string_view xlen,
								std::string_view ybegin, std::string_view ylen,
								Options ...ops);

	template <class ...Options> requires all_keyword_args_tagged_with<plot::FilledCurveOption, Options...>
	PlotBuffer2D PlotFilledCurves(RangeReceiver x, RangeReceiver y, Options ...ops);
	template <class ...Options> requires all_keyword_args_tagged_with<plot::FilledCurveOption, Options...>
	PlotBuffer2D PlotFilledCurves(std::string_view filename, std::string_view x, std::string_view y, Options ...ops);
	template <class ...Options> requires all_keyword_args_tagged_with<plot::FilledCurveOption, Options...>
	PlotBuffer2D PlotFilledCurves(RangeReceiver x, RangeReceiver y, RangeReceiver y2,
									 Options ...ops);
	template <class ...Options> requires all_keyword_args_tagged_with<plot::FilledCurveOption, Options...>
	PlotBuffer2D PlotFilledCurves(std::string_view filename, std::string_view x, std::string_view y, std::string_view y2,
									 Options ...ops);

protected:

	PlotBuffer2D Plot(GraphParam& i);

	static std::string PlotCommand(const GraphParam& i, const bool IsInMemoryDataTransferEnabled);
	static std::string InitCommand();

	std::vector<GraphParam> m_param;
	Canvas* m_canvas;
};

template <class GraphParam, template <class> class Buffer>
class Canvas2D : public Axis2D<Canvas>
{
public:

	using _Buffer = Buffer<GraphParam>;
	using detail::Axis2D<Canvas>::Axis2D;

	friend class MultiPlotter;

	template <class ...Options> requires all_keyword_args_tagged_with<plot::PointOption, Options...>
	_Buffer PlotPoints(RangeReceiver x, RangeReceiver y, Options ...ops);
	template <class ...Options> requires all_keyword_args_tagged_with<plot::PointOption, Options...>
	_Buffer PlotPoints(std::string_view filename, std::string_view xcol, std::string_view ycol, Options ...ops);
	template <class ...Options> requires all_keyword_args_tagged_with<plot::PointOption, Options...>
	_Buffer PlotPoints(std::string_view equation, Options ...ops);
	
	template <class ...Options> requires all_keyword_args_tagged_with<plot::PointOption, Options...>
	_Buffer PlotLines(RangeReceiver x, RangeReceiver y, Options ...ops);
	template <class ...Options> requires all_keyword_args_tagged_with<plot::PointOption, Options...>
	_Buffer PlotLines(std::string_view filename, std::string_view xcol, std::string_view ycol, Options ...ops);
	template <class ...Options> requires all_keyword_args_tagged_with<plot::PointOption, Options...>
	_Buffer PlotLines(std::string_view equation, Options ...ops);

	template <class ...Options> requires all_keyword_args_tagged_with<plot::VectorOption, Options...>
	_Buffer PlotVectors(RangeReceiver xfrom, RangeReceiver yfrom,
						RangeReceiver xlen, RangeReceiver ylen,
						Options ...ops);
	template <class ...Options> requires all_keyword_args_tagged_with<plot::VectorOption, Options...>
	_Buffer PlotVectors(std::string_view filename,
						std::string_view xbegin, std::string_view xlen,
						std::string_view ybegin, std::string_view ylen,
						Options ...ops);

	template <class ...Options> requires all_keyword_args_tagged_with<plot::FilledCurveOption, Options...>
	_Buffer PlotFilledCurves(RangeReceiver x, RangeReceiver y, Options ...ops);
	template <class ...Options> requires all_keyword_args_tagged_with<plot::FilledCurveOption, Options...>
	_Buffer PlotFilledCurves(std::string_view filename, std::string_view x, std::string_view y, Options ...ops);
	template <class ...Options> requires all_keyword_args_tagged_with<plot::FilledCurveOption, Options...>
	_Buffer PlotFilledCurves(RangeReceiver x, RangeReceiver y, RangeReceiver y2,
							 Options ...ops);
	template <class ...Options> requires all_keyword_args_tagged_with<plot::FilledCurveOption, Options...>
	_Buffer PlotFilledCurves(std::string_view filename, std::string_view x, std::string_view y, std::string_view y2,
							 Options ...ops);

	_Buffer GetBuffer();

};

template <class GraphParam>
inline PlotBuffer2D<GraphParam>::PlotBuffer2D(Canvas* g)
	: m_canvas(g) {}
template <class GraphParam>
inline PlotBuffer2D<GraphParam>::PlotBuffer2D(PlotBuffer2D&& p) noexcept
	: m_param(std::move(p.m_param)), m_canvas(p.m_canvas)
{
	p.m_canvas = nullptr;
}
template <class GraphParam>
PlotBuffer2D<GraphParam>& PlotBuffer2D<GraphParam>::operator=(PlotBuffer2D<GraphParam>&& p) noexcept
{
	m_canvas = p.m_canvas; p.m_canvas = nullptr;
	m_param = std::move(p.m_param);
	return *this;
}
template <class GraphParam>
inline PlotBuffer2D<GraphParam>::~PlotBuffer2D()
{
	//mCanvasがnullptrでないときはこのPlotBufferが最終処理を担当する。
	if (m_canvas != nullptr) Flush();
}
template <class GraphParam>
inline void PlotBuffer2D<GraphParam>::Flush()
{
	if (m_canvas == nullptr) throw NotInitialized("Buffer is empty");
	std::string c = "plot";
	for (auto& i : m_param)
	{
		c += PlotCommand(i, m_canvas->IsInMemoryDataTransferEnabled()) + ", ";
	}
	c.erase(c.end() - 2, c.end());
	m_canvas->Command(c);
	m_canvas->Command(InitCommand());
}
template <class GraphParam>
inline PlotBuffer2D<GraphParam> PlotBuffer2D<GraphParam>::Plot(GraphParam& i)
{
	if (i.m_type == GraphParam::DATA)
	{
		if (m_canvas->IsInMemoryDataTransferEnabled())
		{
			i.m_graph = "$" + SanitizeForDataBlock(m_canvas->GetOutput()) + "_" + std::to_string(m_param.size()); // datablock name
		}
		else
		{
			i.m_graph = m_canvas->GetOutput() + ".tmp" + std::to_string(m_param.size()) + ".txt";
		}
		auto GET_ARRAY = [this](ArrayData& X, std::string_view ax,
							std::vector<ArrayData::Range>& it, std::vector<std::string>& column, std::string& labelcolumn)
		{
			switch (X.GetType())
			{
			case ArrayData::NUM_RANGE:
				it.emplace_back(X.GetRange());
				column.emplace_back(std::to_string(it.size()));
				break;
			case ArrayData::STR_RANGE:
				it.emplace_back(X.GetRange());
				if (m_canvas->IsDateTimeEnabled(ax))
					column.emplace_back(std::to_string(it.size()));
				else
					labelcolumn = std::format("{}tic({})", ax, it.size());
				break;
			case ArrayData::COLUMN:
				column.emplace_back(X.GetColumn());
				break;
			case ArrayData::UNIQUE:
				column.emplace_back(std::format("($1-$1+{})", X.GetUnique()));
				break;
			}
		};

		std::vector<ArrayData::Range> it;
		std::vector<std::string> column;
		std::string labelcolumn;

		auto find_axis = [&i](const char* a) { return i.m_axis.find(a) != std::string::npos; };
		std::string x_x2 = find_axis("x2") ? m_canvas->Command("set x2tics"), "x2" : "x";
		std::string y_y2 = find_axis("y2") ? m_canvas->Command("set y2tics"), "y2" : "y";
		//ファイルを作成する。
		if (i.IsPoint())
		{
			auto& p = i.GetPointParam();
			if (!p.m_x) throw InvalidArg("x coordinate list is not given.");
			if (!p.m_y) throw InvalidArg("y coordinate list is not given.");

			GET_ARRAY(p.m_x, x_x2, it, column, labelcolumn);
			GET_ARRAY(p.m_y, y_y2, it, column, labelcolumn);

			if (p.m_x_errorbar) GET_ARRAY(p.m_x_errorbar, x_x2, it, column, labelcolumn);
			if (p.m_y_errorbar) GET_ARRAY(p.m_y_errorbar, y_y2, it, column, labelcolumn);
			if (p.m_variable_color) GET_ARRAY(p.m_variable_color, "cb", it, column, labelcolumn);
			if (p.m_variable_size) GET_ARRAY(p.m_variable_size, "variable_size", it, column, labelcolumn);
		}
		else if (i.IsVector())
		{
			auto& v = i.GetVectorParam();
			if (!v.m_x) throw InvalidArg("x coordinate list is not given.");
			GET_ARRAY(v.m_x, x_x2, it, column, labelcolumn);
			if (!v.m_y) throw InvalidArg("y coordinate list is not given.");
			GET_ARRAY(v.m_y, y_y2, it, column, labelcolumn);
			if (!v.m_x_len) throw InvalidArg("xlen list is not given.");
			GET_ARRAY(v.m_x_len, x_x2, it, column, labelcolumn);
			if (!v.m_y_len) throw InvalidArg("ylen list is not given.");
			GET_ARRAY(v.m_y_len, y_y2, it, column, labelcolumn);

			if (v.m_variable_color) GET_ARRAY(v.m_variable_color, "variable_color", it, column, labelcolumn);
		}
		else if (i.IsFilledCurve())
		{
			auto& f = i.GetFilledCurveParam();
			if (!f.m_x) throw InvalidArg("x coordinate list is not given.");
			GET_ARRAY(f.m_x, x_x2, it, column, labelcolumn);
			if (!f.m_y) throw InvalidArg("y coordinate list is not given.");
			GET_ARRAY(f.m_y, y_y2, it, column, labelcolumn);

			if (f.mY2) GET_ARRAY(f.mY2, y_y2, it, column, labelcolumn);
			if (f.m_variable_color) GET_ARRAY(f.m_variable_color, "variable_fillcolor", it, column, labelcolumn);
		}
		MakeDataObject(m_canvas, i.m_graph, it);
		if (!labelcolumn.empty()) column.emplace_back(std::move(labelcolumn));
		i.m_column = std::move(column);
	}
	else if (i.m_type == GraphParam::FILE)
	{
		auto ADD_COLUMN = [](const ArrayData& a, const std::string& name, std::vector<std::string>& c)
		{
			if (a.GetType() == ArrayData::COLUMN) c.emplace_back(a.GetColumn());
			else if (a.GetType() == ArrayData::UNIQUE) c.emplace_back("($1-$1+" + std::to_string(a.GetUnique()) + ")");
			else throw InvalidArg(name + "list in the file plot mode must be given in the form of the string column or unique value.");
		};
		//ファイルからプロットする場合、x軸ラベルを文字列にするのがちょっと難しい。
		//普通ならusing 1:2で済むところ、x軸を文字列ラベルにするとusing 2:xticlabels(1)のようにしなければならない。
		//しかしそのためには、ファイルをコメントを除く数行読んでみて、数値か文字列かを判定する必要が生じる。
		//ややこしくなるので今のところ非対応。
		//一応、ユーザー側でPlotPointsの引数に"2", "xticlabels(1)"と与えることで動作はするが、その場合エラーバーなどは使えなくなる。
		std::vector<std::string> column;
		if (i.IsPoint())
		{
			auto& p = i.GetPointParam();
			if (!p.m_x) throw InvalidArg("x coordinate list is not given.");
			ADD_COLUMN(p.m_x, "x", column);
			if (!p.m_y) throw InvalidArg("y coordinate list is not given.");
			ADD_COLUMN(p.m_y, "y", column);
			if (p.m_x_errorbar) ADD_COLUMN(p.m_x_errorbar, "xerrorbar", column);
			if (p.m_y_errorbar) ADD_COLUMN(p.m_y_errorbar, "yerrorbar", column);
			if (p.m_variable_color) ADD_COLUMN(p.m_variable_color, "variable_color", column);
			if (p.m_variable_size) ADD_COLUMN(p.m_variable_size, "variable_size", column);
		}
		else if (i.IsVector())
		{
			auto& v = i.GetVectorParam();
			if (!v.m_x) throw InvalidArg("x coordinate list is not given.");
			ADD_COLUMN(v.m_x, "x", column);
			if (!v.m_y) throw InvalidArg("y coordinate list is not given.");
			ADD_COLUMN(v.m_y, "y", column);
			if (!v.m_x_len) throw InvalidArg("xlen list is not given.");
			ADD_COLUMN(v.m_x_len, "xlen", column);
			if (!v.m_y_len) throw InvalidArg("ylen list is not given.");
			ADD_COLUMN(v.m_y_len, "ylen", column);
			if (v.m_variable_color) ADD_COLUMN(v.m_variable_color, "variable_color", column);
		}
		else if (i.IsFilledCurve())
		{
			auto& f = i.GetFilledCurveParam();
			if (!f.m_x) throw InvalidArg("x coordinate list is not given.");
			ADD_COLUMN(f.m_x, "x", column);
			if (!f.m_y) throw InvalidArg("y coordinate list is not given.");
			ADD_COLUMN(f.m_y, "y", column);

			if (f.mY2) ADD_COLUMN(f.mY2, "y2", column);
			if (f.m_variable_color) ADD_COLUMN(f.m_variable_color, "variable_fillcolor", column);
		}
		i.m_column = std::move(column);
	}
	m_param.emplace_back(std::move(i));
	return std::move(*this);
}
template <class GraphParam>
template <class ...Options> requires all_keyword_args_tagged_with<plot::PointOption, Options...>
inline PlotBuffer2D<GraphParam> PlotBuffer2D<GraphParam>::
PlotPoints(RangeReceiver x, RangeReceiver y, Options ...ops)
{
	GraphParam i;
	i.AssignPoint();
	i.m_type = GraphParam::DATA;
	i.SetBaseOptions(ops...);

	//point
	auto& p = i.GetPointParam();
	p.m_x = x;
	p.m_y = y;
	p.SetOptions(ops...);

	return Plot(i);
}
template <class GraphParam>
template <class ...Options> requires all_keyword_args_tagged_with<plot::PointOption, Options...>
inline PlotBuffer2D<GraphParam> PlotBuffer2D<GraphParam>::
PlotPoints(std::string_view filename, std::string_view xcol, std::string_view ycol, Options ...ops)
{
	GraphParam i;
	i.AssignPoint();
	i.m_graph = filename;
	i.m_type = GraphParam::FILE;
	i.SetBaseOptions(ops...);

	//point
	auto& p = i.GetPointParam();
	p.m_x = xcol;
	p.m_y = ycol;
	p.SetOptions(ops...);

	return Plot(i);
}
template <class GraphParam>
template <class ...Options> requires all_keyword_args_tagged_with<plot::PointOption, Options...>
inline PlotBuffer2D<GraphParam> PlotBuffer2D<GraphParam>::
PlotPoints(std::string_view equation, Options ...ops)
{
	GraphParam i;
	i.AssignPoint();
	i.m_graph = equation;
	i.m_type = GraphParam::EQUATION;
	i.SetBaseOptions(ops...);

	//point
	auto& p = i.GetPointParam();
	p.SetOptions(ops...);

	return Plot(i);
}
template <class GraphParam>
template <class ...Options> requires all_keyword_args_tagged_with<plot::PointOption, Options...>
inline PlotBuffer2D<GraphParam> PlotBuffer2D<GraphParam>::
PlotLines(RangeReceiver x, RangeReceiver y, Options ...ops)
{
	return PlotPoints(x, y, plot::style = Style::lines, ops...);
}
template <class GraphParam>
template <class ...Options> requires all_keyword_args_tagged_with<plot::PointOption, Options...>
inline PlotBuffer2D<GraphParam> PlotBuffer2D<GraphParam>::
PlotLines(std::string_view filename, std::string_view xcol, std::string_view ycol, Options ...ops)
{
	return PlotPoints(filename, xcol, ycol, plot::style = Style::lines, ops...);
}
template <class GraphParam>
template <class ...Options> requires all_keyword_args_tagged_with<plot::PointOption, Options...>
inline PlotBuffer2D<GraphParam> PlotBuffer2D<GraphParam>::
PlotLines(std::string_view equation, Options ...ops)
{
	return PlotPoints(equation, plot::style = Style::lines, ops...);
}

template <class GraphParam>
template <class ...Options> requires all_keyword_args_tagged_with<plot::VectorOption, Options...>
inline PlotBuffer2D<GraphParam> PlotBuffer2D<GraphParam>::
PlotVectors(RangeReceiver xfrom, RangeReceiver yfrom,
			RangeReceiver xlen, RangeReceiver ylen,
			Options ...ops)
{
	GraphParam i;
	i.AssignVector();
	i.m_type = GraphParam::DATA;
	i.SetBaseOptions(ops...);

	auto& v = i.GetVectorParam();
	v.m_x = xfrom;
	v.m_y = yfrom;
	v.m_x_len = xlen;
	v.m_y_len = ylen;
	v.SetOptions(ops...);
	return Plot(i);
}

template <class GraphParam>
template <class ...Options> requires all_keyword_args_tagged_with<plot::VectorOption, Options...>
inline PlotBuffer2D<GraphParam> PlotBuffer2D<GraphParam>::
PlotVectors(std::string_view filename,
			std::string_view xfrom, std::string_view xlen,
			std::string_view yfrom, std::string_view ylen,
			Options ...ops)
{
	GraphParam i;
	i.AssignVector();
	i.m_type = GraphParam::FILE;
	i.m_graph = filename;
	i.SetBaseOptions(ops...);

	auto& v = i.GetVectorParam();
	v.m_x = xfrom;
	v.m_y = yfrom;
	v.m_x_len = xlen;
	v.m_y_len = ylen;
	v.SetOptions(ops...);
	return Plot(i);
}

template <class GraphParam>
template <class ...Options> requires all_keyword_args_tagged_with<plot::FilledCurveOption, Options...>
inline PlotBuffer2D<GraphParam> PlotBuffer2D<GraphParam>::
PlotFilledCurves(RangeReceiver x, RangeReceiver y, Options ...ops)
{
	GraphParam p;
	p.AssignFilledCurve();
	p.m_type = GraphParam::DATA;
	p.SetBaseOptions(ops...);

	auto& f = p.GetFilledCurveParam();
	f.m_x = x;
	f.m_y = y;
	f.SetOptions(ops...);
	return Plot(p);
}
template <class GraphParam>
template <class ...Options> requires all_keyword_args_tagged_with<plot::FilledCurveOption, Options...>
inline PlotBuffer2D<GraphParam> PlotBuffer2D<GraphParam>::
PlotFilledCurves(std::string_view filename, std::string_view x, std::string_view y, Options ...ops)
{
	GraphParam p;
	p.AssignFilledCurve();
	p.m_type = GraphParam::FILE;
	p.m_graph = filename;
	p.SetBaseOptions(ops...);

	auto& f = p.GetFilledCurveParam();
	f.m_x = x;
	f.m_y = y;
	f.SetOptions(ops...);
	return Plot(p);
}
template <class GraphParam>
template <class ...Options> requires all_keyword_args_tagged_with<plot::FilledCurveOption, Options...>
inline PlotBuffer2D<GraphParam> PlotBuffer2D<GraphParam>::
PlotFilledCurves(RangeReceiver x, RangeReceiver y, RangeReceiver y2,
				 Options ...ops)
{
	GraphParam p;
	p.AssignFilledCurve();
	p.m_type = GraphParam::DATA;
	p.SetBaseOptions(ops...);

	auto& f = p.GetFilledCurveParam();
	f.m_x = x;
	f.m_y = y;
	f.mY2 = y2;
	f.SetOptions(ops...);
	return Plot(p);
}
template <class GraphParam>
template <class ...Options> requires all_keyword_args_tagged_with<plot::FilledCurveOption, Options...>
inline PlotBuffer2D<GraphParam> PlotBuffer2D<GraphParam>::
PlotFilledCurves(std::string_view filename, std::string_view x, std::string_view y, std::string_view y2, Options ...ops)
{
	GraphParam p;
	p.AssignFilledCurve();
	p.m_type = GraphParam::FILE;
	p.m_graph = filename;
	p.SetBaseOptions(ops...);

	auto& f = p.GetFilledCurveParam();
	f.m_x = x;
	f.m_y = y;
	f.mY2 = y2;
	f.SetOptions(ops...);
	return Plot(p);
}

template <class GraphParam>
inline std::string PlotBuffer2D<GraphParam>::
PlotCommand(const GraphParam& p, bool IsInMemoryDataTransferEnabled)
{
	//filename or equation
	std::string c;
	switch (p.m_type) 
	{
	case GraphParam::EQUATION:
		//equation
		c += " " + p.m_graph;
		break;
	case GraphParam::FILE:
		//filename
		c += " '" + p.m_graph + "'";

		//using
		c += " using ";
		for (size_t i = 0; i < p.m_column.size(); ++i)
			c += p.m_column[i] + ":";
		c.pop_back();
		break;
	case GraphParam::DATA:
		if (IsInMemoryDataTransferEnabled) 
		{
			//variable name
			c += " " + p.m_graph;
		}
		else {
			//filename
			c += " '" + p.m_graph + "'";
		}

		//using
		c += " using ";
		for (size_t i = 0; i < p.m_column.size(); ++i)
			c += p.m_column[i] + ":";
		c.pop_back();
		break;
	}

	//title
	if (!p.m_title.empty())
	{
		if (p.m_title == "notitle") c += " notitle";
		else c += " title '" + p.m_title + "'";
	}

	c += " with";
	//ベクトル指定がある場合。
	if (p.IsVector())
	{
		c += VectorPlotCommand(p.GetVectorParam());
	}
	//点指定がある場合。
	else if (p.IsPoint())
	{
		c += PointPlotCommand(p.GetPointParam());
	}
	else if (p.IsFilledCurve())
	{
		c += FilledCurveplotCommand(p.GetFilledCurveParam());
	}

	//axis
	if (!p.m_axis.empty()) c += " axes " + p.m_axis;

	return c;
}
template <class GraphParam>
inline std::string PlotBuffer2D<GraphParam>::InitCommand()
{
	std::string c;
	c += "set autoscale\n";
	c += "unset logscale\n";
	c += "unset title\n";
	c += "unset grid\n";
	c += "set size noratio\n";
	return c;
}

template <class GraphParam, template <class> class Buffer>
template <class ...Options> requires all_keyword_args_tagged_with<plot::PointOption, Options...>
inline Buffer<GraphParam> Canvas2D<GraphParam, Buffer>::
PlotPoints(RangeReceiver x, RangeReceiver y, Options ...ops)
{
	_Buffer r(this);
	return r.PlotPoints(x, y, ops...);
}
template <class GraphParam, template <class> class Buffer>
template <class ...Options> requires all_keyword_args_tagged_with<plot::PointOption, Options...>
inline Buffer<GraphParam> Canvas2D<GraphParam, Buffer>::
PlotPoints(std::string_view filename, std::string_view xcol, std::string_view ycol, Options ...ops)
{
	_Buffer r(this);
	return r.PlotPoints(filename, xcol, ycol, ops...);
}
template <class GraphParam, template <class> class Buffer>
template <class ...Options> requires all_keyword_args_tagged_with<plot::PointOption, Options...>
inline Buffer<GraphParam> Canvas2D<GraphParam, Buffer>::
PlotPoints(std::string_view equation, Options ...ops)
{
	_Buffer r(this);
	return r.PlotPoints(equation, ops...);
}

template <class GraphParam, template <class> class Buffer>
template <class ...Options> requires all_keyword_args_tagged_with<plot::PointOption, Options...>
inline Buffer<GraphParam> Canvas2D<GraphParam, Buffer>::
PlotLines(RangeReceiver x, RangeReceiver y, Options ...ops)
{
	_Buffer r(this);
	return r.PlotLines(x, y, ops...);
}
template <class GraphParam, template <class> class Buffer>
template <class ...Options> requires all_keyword_args_tagged_with<plot::PointOption, Options...>
inline Buffer<GraphParam> Canvas2D<GraphParam, Buffer>::
PlotLines(std::string_view filename, std::string_view xcol, std::string_view ycol, Options ...ops)
{
	_Buffer r(this);
	return r.PlotLines(filename, xcol, ycol, ops...);
}
template <class GraphParam, template <class> class Buffer>
template <class ...Options> requires all_keyword_args_tagged_with<plot::PointOption, Options...>
inline Buffer<GraphParam> Canvas2D<GraphParam, Buffer>::
PlotLines(std::string_view equation, Options ...ops)
{
	_Buffer r(this);
	return r.PlotLines(equation, ops...);
}

template <class GraphParam, template <class> class Buffer>
template <class ...Options> requires all_keyword_args_tagged_with<plot::VectorOption, Options...>
inline Buffer<GraphParam> Canvas2D<GraphParam, Buffer>::
PlotVectors(RangeReceiver xfrom, RangeReceiver yfrom,
			RangeReceiver xlen, RangeReceiver ylen,
			Options ...ops)
{
	_Buffer r(this);
	return r.PlotVectors(xfrom, yfrom, xlen, ylen, ops...);
}
template <class GraphParam, template <class> class Buffer>
template <class ...Options> requires all_keyword_args_tagged_with<plot::VectorOption, Options...>
inline Buffer<GraphParam> Canvas2D<GraphParam, Buffer>::
PlotVectors(std::string_view filename,
			std::string_view xfrom, std::string_view yfrom,
			std::string_view xlen, std::string_view ylen,
			Options ...ops)
{
	_Buffer r(this);
	return r.PlotVectors(filename, xfrom, yfrom, xlen, ylen, ops...);
}
template <class GraphParam, template <class> class Buffer>
template <class ...Options> requires all_keyword_args_tagged_with<plot::FilledCurveOption, Options...>
inline Buffer<GraphParam> Canvas2D<GraphParam, Buffer>::
PlotFilledCurves(RangeReceiver x, RangeReceiver y, Options ...ops)
{
	_Buffer r(this);
	return r.PlotFilledCurves(x, y, ops...);
}
template <class GraphParam, template <class> class Buffer>
template <class ...Options> requires all_keyword_args_tagged_with<plot::FilledCurveOption, Options...>
inline Buffer<GraphParam> Canvas2D<GraphParam, Buffer>::
PlotFilledCurves(std::string_view filename, std::string_view x, std::string_view y, Options ...ops)
{
	_Buffer r(this);
	return r.PlotFilledCurves(filename, x, y, ops...);
}
template <class GraphParam, template <class> class Buffer>
template <class ...Options> requires all_keyword_args_tagged_with<plot::FilledCurveOption, Options...>
inline Buffer<GraphParam> Canvas2D<GraphParam, Buffer>::
PlotFilledCurves(RangeReceiver x, RangeReceiver y, RangeReceiver y2,
				 Options ...ops)
{
	_Buffer r(this);
	return r.PlotFilledCurves(x, y, y2, ops...);
}
template <class GraphParam, template <class> class Buffer>
template <class ...Options> requires all_keyword_args_tagged_with<plot::FilledCurveOption, Options...>
inline Buffer<GraphParam> Canvas2D<GraphParam, Buffer>::
PlotFilledCurves(std::string_view filename, std::string_view x, std::string_view y, std::string_view y2, Options ...ops)
{
	_Buffer r(this);
	return r.PlotFilledCurves(filename, x, y, y2, ops...);
}
template <class GraphParam, template <class> class Buffer>
inline Buffer<GraphParam> Canvas2D<GraphParam, Buffer>::GetBuffer()
{
	return _Buffer(this);
}

}

enum class Contour { none, base, surface, both, };
enum class CntrSmooth { none, linear, cubicspline, bspline };

namespace plot
{

struct Base3DOption : public BaseOption {};
struct Point3DOption : public PointOption, public Base3DOption {};
struct Vector3DOption : public VectorOption, public Base3DOption {};
struct ColormapOption : public BaseOption {};

ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(z, detail::ArrayData, Base3DOption)
ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(zlen, detail::ArrayData, Vector3DOption)

ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(map, const detail::MatrixData&, ColormapOption)
ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(xcoord, const detail::ArrayData&, ColormapOption)
ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(ycoord, const detail::ArrayData&, ColormapOption)
ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(xrange, ADAPT_TIE_ARGS(std::pair<double, double>), ColormapOption)
ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(yrange, ADAPT_TIE_ARGS(std::pair<double, double>), ColormapOption)

//options for contour plot
ADAPT_DEFINE_TAGGED_KEYWORD_OPTION(with_contour, ColormapOption)
ADAPT_DEFINE_TAGGED_KEYWORD_OPTION(without_surface, ColormapOption)
ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(cntrsmooth, CntrSmooth, ColormapOption)
ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(cntrpoints, int, ColormapOption)//the number of lines for cspline and bspline
ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(cntrorder, int, ColormapOption)//order for bspline, [2, 10]
ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(cntrlevels_auto, int, ColormapOption)
ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(cntrlevels_discrete, const std::vector<double>&, ColormapOption)
ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(cntrlevels_incremental, ADAPT_TIE_ARGS(std::tuple<double, double, double>), ColormapOption)
ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(cntrcolor, std::string_view, ColormapOption)
ADAPT_DEFINE_TAGGED_KEYWORD_OPTION(variable_cntrcolor, ColormapOption)
ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(cntrlinetype, int, ColormapOption)
ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(cntrlinewidth, double, ColormapOption)

}

namespace detail
{

struct PointParamCM : public PointParam
{
	template <class ...Ops>
	void SetOptions(Ops ...ops)
	{
		PointParam::SetOptions(ops...);
	}
	detail::ArrayData m_z;
};
struct VectorParamCM : public VectorParam
{
	template <class ...Ops>
	void SetOptions(Ops ...ops)
	{
		VectorParam::SetOptions(ops...);
	}
	detail::ArrayData m_z;
	detail::ArrayData m_z_len;
};
struct FilledCurveParamCM : public FilledCurveParam
{
	template <class ...Ops>
	void SetOptions(Ops ...ops)
	{
		FilledCurveParam::SetOptions(ops...);
	}
	detail::ArrayData m_z;
};
struct ColormapParam
{
	ColormapParam()
		: m_x_range(DBL_MAX, -DBL_MAX), m_y_range(DBL_MAX, -DBL_MAX),
		m_with_contour(false), m_without_surface(false), m_cntr_smooth(CntrSmooth::none),
		m_cntr_points(-1), m_cntr_order(-1), m_cntr_levels_auto(-1), m_cntr_levels_incremental(0, 0, 0),
		m_variable_cntr_color(false), m_cntr_line_type(-2), m_cntr_line_width(-1.)
	{}

	template <class ...Ops>
	void SetOptions(Ops ...ops)
	{
		m_with_contour = KeywordExists(plot::with_contour, ops...);
		m_without_surface = KeywordExists(plot::without_surface, ops...);
		m_cntr_smooth = GetKeywordArg(plot::cntrsmooth, CntrSmooth::none, ops...);
		m_cntr_points = GetKeywordArg(plot::cntrpoints, -1, ops...);
		m_cntr_order = GetKeywordArg(plot::cntrorder, -1, ops...);
		m_cntr_levels_auto = GetKeywordArg(plot::cntrlevels_auto, -1, ops...);
		m_cntr_levels_discrete = GetKeywordArg(plot::cntrlevels_discrete, std::vector<double>{}, ops...);
		m_cntr_levels_incremental = GetKeywordArg(plot::cntrlevels_incremental, std::tuple<double, double, double>(0., 0., 0.), ops...);
		m_cntr_color = GetKeywordArg(plot::cntrcolor, "", ops...);
		m_variable_cntr_color = KeywordExists(plot::variable_cntrcolor, ops...);
		m_cntr_line_type = GetKeywordArg(plot::cntrlinetype, -2, ops...);
		m_cntr_line_width = GetKeywordArg(plot::cntrlinewidth, -1., ops...);
	}
	//ColormapOption
	ArrayData m_x_coord;
	ArrayData m_y_coord;
	std::pair<double, double> m_x_range;
	std::pair<double, double> m_y_range;
	MatrixData m_z_map;

	bool m_with_contour;
	bool m_without_surface;
	CntrSmooth m_cntr_smooth;
	int m_cntr_points;
	int m_cntr_order;
	int m_cntr_levels_auto;
	std::vector<double> m_cntr_levels_discrete;
	std::tuple<double, double, double> m_cntr_levels_incremental;
	std::string m_cntr_color;
	bool m_variable_cntr_color;
	int m_cntr_line_type;
	double m_cntr_line_width;
};

template <class PointParam, class VectorParam, class FilledCurveParam, class ColormapParam>
struct GraphParamBaseCM
	: public GraphParamBase<PointParam, VectorParam, FilledCurveParam, ColormapParam>
{
	template <class ...Ops>
	void SetBaseOptions(Ops ...ops)
	{
		GraphParamBase<PointParam, VectorParam, FilledCurveParam, ColormapParam>::SetBaseOptions(ops...);
	}

	void AssignPoint() { this->template emplace<PointParam>(); }
	void AssignVector() { this->template emplace<VectorParam>(); }
	void AssignFilledCurve() { this->template emplace<FilledCurveParam>(); }
	void AssignColormap() { this->template emplace<ColormapParam>(); }

	bool IsPoint() const { return this->index() == 0; }
	bool IsVector() const { return this->index() == 1; }
	bool IsFilledCurve() const { return this->index() == 2; }
	bool IsColormap() const { return this->index() == 3; }

	PointParam& GetPointParam() { return std::get<PointParam>(*this); }
	const PointParam& GetPointParam() const { return std::get<PointParam>(*this); }
	VectorParam& GetVectorParam() { return std::get<VectorParam>(*this); }
	const VectorParam& GetVectorParam() const { return std::get<VectorParam>(*this); }
	FilledCurveParam& GetFilledCurveParam() { return std::get<FilledCurveParam>(*this); }
	const FilledCurveParam& GetFilledCurveParam() const { return std::get<FilledCurveParam>(*this); }
	ColormapParam& GetColormapParam() { return std::get<ColormapParam>(*this); }
	const ColormapParam& GetColormapParam() const { return std::get<ColormapParam>(*this); }
};

using GraphParamCM = GraphParamBaseCM<PointParamCM, VectorParamCM, FilledCurveParamCM, ColormapParam>;

template <class GraphParam>
struct PlotBufferCM
{
	PlotBufferCM(Canvas* g);
	PlotBufferCM(const PlotBufferCM&) = delete;
	PlotBufferCM(PlotBufferCM&& p) noexcept;
	PlotBufferCM& operator=(const PlotBufferCM&) = delete;
	PlotBufferCM& operator=(PlotBufferCM&& p) noexcept;
	virtual ~PlotBufferCM();

	void Flush();

	template <class ...Options> requires all_keyword_args_tagged_with<plot::Point3DOption, Options...>
	PlotBufferCM PlotPoints(RangeReceiver x, RangeReceiver y, RangeReceiver z, Options ...ops);
	template <class ...Options> requires all_keyword_args_tagged_with<plot::Point3DOption, Options...>
	PlotBufferCM PlotPoints(std::string_view filename,
							   std::string_view x, std::string_view y, std::string_view z,
							   Options ...ops);
	template <class ...Options> requires all_keyword_args_tagged_with<plot::Point3DOption, Options...>
	PlotBufferCM PlotPoints(RangeReceiver x, RangeReceiver y, Options ...ops);
	template <class ...Options> requires all_keyword_args_tagged_with<plot::Point3DOption, Options...>
	PlotBufferCM PlotPoints(std::string_view filename,
							   std::string_view x, std::string_view y,
							   Options ...ops);
	template <class ...Options> requires all_keyword_args_tagged_with<plot::Point3DOption, Options...>
	PlotBufferCM PlotPoints(std::string_view equation, Options ...ops);

	template <class ...Options> requires all_keyword_args_tagged_with<plot::Point3DOption, Options...>
	PlotBufferCM PlotLines(RangeReceiver x, RangeReceiver y, RangeReceiver z, Options ...ops);
	template <class ...Options> requires all_keyword_args_tagged_with<plot::Point3DOption, Options...>
	PlotBufferCM PlotLines(std::string_view filename,
							  std::string_view x, std::string_view y, std::string_view z,
							  Options ...ops);
	template <class ...Options> requires all_keyword_args_tagged_with<plot::Point3DOption, Options...>
	PlotBufferCM PlotLines(RangeReceiver x, RangeReceiver y, Options ...ops);
	template <class ...Options> requires all_keyword_args_tagged_with<plot::Point3DOption, Options...>
	PlotBufferCM PlotLines(std::string_view filename,
							  std::string_view x, std::string_view y,
							  Options ...ops);
	template <class ...Options> requires all_keyword_args_tagged_with<plot::Point3DOption, Options...>
	PlotBufferCM PlotLines(std::string_view equation, Options ...ops);

	template <class ...Options> requires all_keyword_args_tagged_with<plot::Vector3DOption, Options...>
	PlotBufferCM PlotVectors(RangeReceiver xfrom, RangeReceiver yfrom, RangeReceiver zfrom,
								RangeReceiver xlen, RangeReceiver ylen, RangeReceiver zlen,
								Options ...ops);
	template <class ...Options> requires all_keyword_args_tagged_with<plot::Vector3DOption, Options...>
	PlotBufferCM PlotVectors(std::string_view filename,
								std::string_view xfrom, std::string_view yfrom, std::string_view zfrom,
								std::string_view xlen, std::string_view ylen, std::string_view zlen,
								Options ...ops);
	template <class ...Options> requires all_keyword_args_tagged_with<plot::Vector3DOption, Options...>
	PlotBufferCM PlotVectors(RangeReceiver xfrom, RangeReceiver yfrom,
								RangeReceiver xlen, RangeReceiver ylen,
								Options ...ops);
	template <class ...Options> requires all_keyword_args_tagged_with<plot::Vector3DOption, Options...>
	PlotBufferCM PlotVectors(std::string_view filename,
								std::string_view xfrom, std::string_view yfrom,
								std::string_view xlen, std::string_view ylen,
								Options ...ops);

	template <class Type, class ...Options> requires all_keyword_args_tagged_with<plot::ColormapOption, Options...>
	PlotBufferCM PlotColormap(const Matrix<Type>& map, RangeReceiver x, RangeReceiver y,
								 Options ...ops);
	template <class Type, class ...Options> requires all_keyword_args_tagged_with<plot::ColormapOption, Options...>
	PlotBufferCM PlotColormap(const Matrix<Type>& map, std::pair<double, double> x, std::pair<double, double> y,
								 Options ...ops);
	template <class ...Options> requires all_keyword_args_tagged_with<plot::ColormapOption, Options...>
	PlotBufferCM PlotColormap(std::string_view filename, std::string_view z, std::string_view x, std::string_view y,
								 Options ...ops);
	template <class ...Options> requires all_keyword_args_tagged_with<plot::ColormapOption, Options...>
	PlotBufferCM PlotColormap(std::string_view equation, Options ...ops);

protected:

	PlotBufferCM Plot(GraphParam& i);

	static std::string PlotCommand(const GraphParam& i, const bool IsInMemoryDataTransferEnabled);
	static std::string InitCommand();

	std::vector<GraphParam> m_param;
	Canvas* m_canvas;
};


template <class GraphParam, template <class> class Buffer>
class CanvasCM : public detail::Axis2D<Canvas>
{
public:

	using _Buffer = Buffer<GraphParam>;

	CanvasCM(const std::string& output, double sizex = 0., double sizey = 0.);
	CanvasCM();

	friend class GPMMultiPlotter;

	template <class ...Options> requires all_keyword_args_tagged_with<plot::Point3DOption, Options...>
	_Buffer PlotPoints(RangeReceiver x, RangeReceiver y, RangeReceiver z, Options ...ops);
	template <class ...Options> requires all_keyword_args_tagged_with<plot::Point3DOption, Options...>
	_Buffer PlotPoints(std::string_view filename,
					   std::string_view x, std::string_view y, std::string_view z,
					   Options ...ops);
	template <class ...Options> requires all_keyword_args_tagged_with<plot::Point3DOption, Options...>
	_Buffer PlotPoints(RangeReceiver x, RangeReceiver y, Options ...ops);
	template <class ...Options> requires all_keyword_args_tagged_with<plot::Point3DOption, Options...>
	_Buffer PlotPoints(std::string_view filename,
					   std::string_view x, std::string_view y,
					   Options ...ops);
	template <class ...Options> requires all_keyword_args_tagged_with<plot::Point3DOption, Options...>
	_Buffer PlotPoints(std::string_view equation, Options ...ops);

	template <class ...Options> requires all_keyword_args_tagged_with<plot::Point3DOption, Options...>
	_Buffer PlotLines(RangeReceiver x, RangeReceiver y, RangeReceiver z, Options ...ops);
	template <class ...Options> requires all_keyword_args_tagged_with<plot::Point3DOption, Options...>
	_Buffer PlotLines(std::string_view filename,
					  std::string_view x, std::string_view y, std::string_view z,
					  Options ...ops);
	template <class ...Options> requires all_keyword_args_tagged_with<plot::Point3DOption, Options...>
	_Buffer PlotLines(RangeReceiver x, RangeReceiver y, Options ...ops);
	template <class ...Options> requires all_keyword_args_tagged_with<plot::Point3DOption, Options...>
	_Buffer PlotLines(std::string_view filename,
					  std::string_view x, std::string_view y,
					  Options ...ops);
	template <class ...Options> requires all_keyword_args_tagged_with<plot::Point3DOption, Options...>
	_Buffer PlotLines(std::string_view equation, Options ...ops);

	template <class ...Options> requires all_keyword_args_tagged_with<plot::Vector3DOption, Options...>
	_Buffer PlotVectors(RangeReceiver xfrom, RangeReceiver yfrom, RangeReceiver zfrom,
						RangeReceiver xlen, RangeReceiver ylen, RangeReceiver zlen,
						Options ...ops);
	template <class ...Options> requires all_keyword_args_tagged_with<plot::Vector3DOption, Options...>
	_Buffer PlotVectors(std::string_view filename,
						std::string_view xfrom, std::string_view yfrom, std::string_view zfrom,
						std::string_view xlen, std::string_view ylen, std::string_view zlen,
						Options ...ops);
	template <class ...Options> requires all_keyword_args_tagged_with<plot::Vector3DOption, Options...>
	_Buffer PlotVectors(RangeReceiver xfrom, RangeReceiver yfrom,
						RangeReceiver xlen, RangeReceiver ylen,
						Options ...ops);
	template <class ...Options> requires all_keyword_args_tagged_with<plot::Vector3DOption, Options...>
	_Buffer PlotVectors(std::string_view filename,
						std::string_view xfrom, std::string_view yfrom,
						std::string_view xlen, std::string_view ylen,
						Options ...ops);

	template <class Type, class ...Options> requires all_keyword_args_tagged_with<plot::ColormapOption, Options...>
	_Buffer PlotColormap(const Matrix<Type>& map, RangeReceiver x, RangeReceiver y,
						 Options ...ops);
	template <class Type, class ...Options> requires all_keyword_args_tagged_with<plot::ColormapOption, Options...>
	_Buffer PlotColormap(const Matrix<Type>& map, std::pair<double, double> x, std::pair<double, double> y,
						 Options ...ops);
	template <class ...Options> requires all_keyword_args_tagged_with<plot::ColormapOption, Options...>
	_Buffer PlotColormap(std::string_view filename, std::string_view z, std::string_view x, std::string_view y,
						 Options ...ops);
	template <class ...Options> requires all_keyword_args_tagged_with<plot::ColormapOption, Options...>
	_Buffer PlotColormap(std::string_view equation, Options ...ops);

	_Buffer GetBuffer();
};

template <class GraphParam>
inline PlotBufferCM<GraphParam>::PlotBufferCM(Canvas* g)
	: m_canvas(g) {}
template <class GraphParam>
inline PlotBufferCM<GraphParam>::PlotBufferCM(PlotBufferCM&& p) noexcept
	: m_param(std::move(p.m_param)), m_canvas(p.m_canvas)
{
	p.m_canvas = nullptr;
}
template <class GraphParam>
inline PlotBufferCM<GraphParam>& PlotBufferCM<GraphParam>::operator=(PlotBufferCM<GraphParam>&& p) noexcept
{
	m_canvas = p.m_canvas; p.m_canvas = nullptr;
	m_param = std::move(p.m_param);
	return *this;
}
template <class GraphParam>
inline PlotBufferCM<GraphParam>::~PlotBufferCM()
{
	//mPipeがnullptrでないときはこのPlotterが最終処理を担当する。
	if (m_canvas != nullptr) Flush();
}
template <class GraphParam>
inline void PlotBufferCM<GraphParam>::Flush()
{
	if (m_canvas == nullptr) throw NotInitialized("Buffer is empty");
	std::string c = "splot";
	for (auto& i : m_param)
	{
		c += PlotCommand(i, m_canvas->IsInMemoryDataTransferEnabled()) + ", ";
	}
	c.erase(c.end() - 2, c.end());
	m_canvas->Command(c);
	m_canvas->Command(InitCommand());
}
struct CoordRange
{
	struct iterator
	{
		iterator(const CoordRange& p)
			: parent(&p), min(0.), center(0.), pos(-1)
		{
			if (parent->x.End())
				throw InvalidArg("The number of datapoints for colormap must be greater than 1.");
			double curr = *parent->x;
			++parent->x;
			if (parent->x.End())
				throw InvalidArg("The number of datapoints for colormap must be greater than 1.");
			double next = *parent->x;
			min = curr - (next - curr) / 2.;
			center = curr;
		}
		void operator++()
		{
			if (pos == 1) throw NoElements();//これが投げられたらバグ。
			if (parent->x.End())
			{
				//xは常にひとつ先の座標を指している。
				//よって、このスコープは最後の、つまりMatrixの端のさらに一つ次の座標へ移動しようとしている。
				pos = 1;
				double prev = center;
				double w_per_2 = center - min;
				min = center + w_per_2;
				center = prev + w_per_2 * 2;
			}
			else
			{
				++parent->x;
				pos = 0;
				double prev = center;
				double curr = *parent->x;
				min = (curr + prev) / 2.;
				center = curr;
			}
		}
		bool End() const { return pos == 1; }
		std::pair<double, double> operator*() const
		{
			return { min, center };
		}

	private:
		const CoordRange* parent;
		double min, center;
		int pos;//-1:first, 0:middle, 1:last
	};
	CoordRange(ArrayData::Range& r)
		: x(r)
	{}
	ArrayData::Range& x;
};
struct CoordMinMax
{
	struct iterator
	{
		iterator(const CoordMinMax& p)
			: current(0), parent(&p) {}
		void operator++() { ++current; }
		bool End() const { return current == parent->size; }
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
	double min;
	double cmin;
	size_t size;
	double width;
};
template <class GraphParam>
inline PlotBufferCM<GraphParam> PlotBufferCM<GraphParam>::Plot(GraphParam& i)
{
	if (i.m_type == GraphParam::DATA)
	{
		if (m_canvas->IsInMemoryDataTransferEnabled())
		{
			i.m_graph = "$" + SanitizeForDataBlock(m_canvas->GetOutput()) + "_" + std::to_string(m_param.size()); // datablock name
		}
		else
		{
			i.m_graph = m_canvas->GetOutput() + ".tmp" + std::to_string(m_param.size()) + ".txt";
		}
		auto GET_ARRAY = [this](ArrayData& X, std::string_view ax,
								std::vector<ArrayData::Range>& it, std::vector<std::string>& column, std::string& labelcolumn)
		{
			switch (X.GetType())
			{
			case ArrayData::NUM_RANGE:
				it.emplace_back(X.GetRange());
				column.emplace_back(std::to_string(it.size()));
				break;
			case ArrayData::STR_RANGE:
				it.emplace_back(X.GetRange());
				if (m_canvas->IsDateTimeEnabled(ax))
					column.emplace_back(std::to_string(it.size()));
				else
					labelcolumn = std::format("{}tic({})", ax, it.size());
				break;
			case ArrayData::COLUMN:
				column.emplace_back(X.GetColumn());
				break;
			case ArrayData::UNIQUE:
				column.emplace_back(std::format("($1-$1+{})", X.GetUnique()));
				break;
			}
		};

		std::vector<std::string> column;
		std::string labelcolumn;

		auto find_axis = [&i](const char* a) { return i.m_axis.find(a) != std::string::npos; };
		std::string x_x2 = find_axis("x2") ? m_canvas->Command("set x2tics"), "x2" : "x";
		std::string y_y2 = find_axis("y2") ? m_canvas->Command("set y2tics"), "y2" : "y";
		std::string z_z2 = find_axis("z2") ? m_canvas->Command("set z2tics"), "z2" : "z";

		//ファイルを作成する。
		if (i.IsColormap())
		{
			auto& m = i.GetColormapParam();
			if (!m.m_z_map) throw InvalidArg("z map is not given");
			if (m.m_z_map.GetType() != MatrixData::NUM_MAT)
				throw InvalidArg("z map in the data plot mode must be given in the form of Matrix<double>.");

			//mapがMatrixであるとき、x、yの座標値も配列かrangeで与えられていなければならない。
			if (!((m.m_x_coord && m.m_x_coord.GetType() == ArrayData::NUM_RANGE) ||
				  m.m_x_range != std::make_pair(DBL_MAX, -DBL_MAX))) throw InvalidArg("");
			if (!((m.m_y_coord && m.m_y_coord.GetType() == ArrayData::NUM_RANGE) ||
				  m.m_y_range != std::make_pair(DBL_MAX, -DBL_MAX))) throw InvalidArg("");

			auto [xsize, ysize] = m.m_z_map.GetMatrix().GetSize();

			column = { "1", "2", "5" };
			if (m.m_x_coord)
			{
				auto x_ = m.m_x_coord.GetRange();
				if (m.m_y_coord)
				{
					auto y_ = m.m_y_coord.GetRange();
					MakeDataObject(m_canvas, i.m_graph, m.m_z_map.GetMatrix(), CoordRange(x_), CoordRange(y_));
				}
				else
				{
					auto y_ = m.m_y_range;
					MakeDataObject(m_canvas, i.m_graph, m.m_z_map.GetMatrix(), CoordRange(x_), CoordMinMax(y_, ysize));
				}
			}
			else
			{
				auto x_ = m.m_x_range;
				if (m.m_y_coord)
				{
					auto y_ = m.m_y_coord.GetRange();
					//if (y.size() != ysize) throw InvalidArg("size of y coordinate list and the y size of mat must be the same.");
					MakeDataObject(m_canvas, i.m_graph, m.m_z_map.GetMatrix(), CoordMinMax(x_, xsize), CoordRange(y_));
				}
				else
				{
					auto y_ = m.m_y_range;
					MakeDataObject(m_canvas, i.m_graph, m.m_z_map.GetMatrix(), CoordMinMax(x_, xsize), CoordMinMax(y_, ysize));
				}
			}

			//最後にcontourを作成する。
			if (m.m_with_contour)
			{
				m_canvas->Command("set contour base");
				if (m.m_cntr_smooth != CntrSmooth::none)
				{
					switch (m.m_cntr_smooth)
					{
					case CntrSmooth::linear: m_canvas->Command("set cntrparam linear"); break;
					case CntrSmooth::cubicspline: m_canvas->Command("set cntrparam cubicspline"); break;
					case CntrSmooth::bspline: m_canvas->Command("set cntrparam bspline"); break;
					default: break;
					}
				}
				if (m.m_cntr_points != -1) m_canvas->Command(std::format("set cntrparam points {}", m.m_cntr_points));
				if (m.m_cntr_order != -1) m_canvas->Command(std::format("set cntrparam order {}", m.m_cntr_order));

				if (m.m_cntr_levels_auto != -1)
				{
					m_canvas->Command(std::format("set cntrparam levels auto {}", m.m_cntr_levels_auto));
				}
				else if (!m.m_cntr_levels_discrete.empty())
				{
					std::string str;
					for (auto x_ : m.m_cntr_levels_discrete) str += std::to_string(x_) + ", ";
					str.erase(str.end() - 2, str.end());
					m_canvas->Command("set cntrparam levels discrete " + str);
				}
				else if (m.m_cntr_levels_incremental != std::tuple<double, double, double>{ 0, 0, 0 })
				{
					double start, incr, end;
					std::tie(start, incr, end) = m.m_cntr_levels_incremental;
					m_canvas->Command(std::format("set cntrparam levels incremental {}, {}, {}", start, incr, end));
				}

				m_canvas->Command("set pm3d implicit");
				m_canvas->Command("set contour base");
				m_canvas->Command("unset surface");
				if (m_canvas->IsInMemoryDataTransferEnabled())
				{
					m_canvas->Command("set table " + i.m_graph + "_cntr");
				}
				else {
					std::string path = i.m_graph;
					path.erase(path.end() - 3, path.end());
					path += "cntr.txt";
					m_canvas->Command("set table '" + path + "'");
				}
				//3:4:column[2]でplotする。
				m_canvas->Command(std::format("splot '{}' using 3:4:{} t '{}'", i.m_graph, column[2], i.m_title));
				m_canvas->Command("unset table");
				m_canvas->Command("set surface");
				m_canvas->Command("unset contour");
				m_canvas->Command("unset pm3d");
			}
		}
		else if (i.IsPoint())
		{
			std::vector<ArrayData::Range> it;
			auto& p = i.GetPointParam();
			if (!p.m_x) throw InvalidArg("x coordinate list is not given.");
			GET_ARRAY(p.m_x, x_x2, it, column, labelcolumn);
			if (!p.m_y) throw InvalidArg("y coordinate list is not given.");
			GET_ARRAY(p.m_y, y_y2, it, column, labelcolumn);
			if (!p.m_z) throw InvalidArg("z coordinate list is not given.");
			GET_ARRAY(p.m_z, z_z2, it, column, labelcolumn);

			if (p.m_x_errorbar)
			{
				GET_ARRAY(p.m_x_errorbar, x_x2, it, column, labelcolumn);
			}
			if (p.m_y_errorbar)
			{
				GET_ARRAY(p.m_y_errorbar, y_y2, it, column, labelcolumn);
			}
			if (p.m_variable_color)
			{
				GET_ARRAY(p.m_variable_color, "cb", it, column, labelcolumn);
			}
			if (p.m_variable_size)
			{
				GET_ARRAY(p.m_variable_size, "variable_size", it, column, labelcolumn);
			}
			MakeDataObject(m_canvas, i.m_graph, it);
		}
		else if (i.IsVector())
		{
			std::vector<ArrayData::Range> it;
			auto& v = i.GetVectorParam();
			if (!v.m_x) throw InvalidArg("x coordinate list is not given.");
			GET_ARRAY(v.m_x, x_x2, it, column, labelcolumn);
			if (!v.m_y) throw InvalidArg("y coordinate list is not given.");
			GET_ARRAY(v.m_y, y_y2, it, column, labelcolumn);
			if (!v.m_z) throw InvalidArg("z coordinate list is not given.");
			GET_ARRAY(v.m_z, z_z2, it, column, labelcolumn);
			if (!v.m_x_len) throw InvalidArg("xlen list is not given.");
			GET_ARRAY(v.m_x_len, x_x2, it, column, labelcolumn);
			if (!v.m_y_len) throw InvalidArg("ylen list is not given.");
			GET_ARRAY(v.m_y_len, y_y2, it, column, labelcolumn);
			if (!v.m_z_len) throw InvalidArg("zlen list is not given.");
			GET_ARRAY(v.m_z_len, z_z2, it, column, labelcolumn);

			if (v.m_variable_color)
			{
				GET_ARRAY(v.m_variable_color, "variable_color", it, column, labelcolumn);
			}
			MakeDataObject(m_canvas, i.m_graph, it);
		}
		if (!labelcolumn.empty()) column.emplace_back(std::move(labelcolumn));
		i.m_column = std::move(column);
	}
	else if (i.m_type == GraphParam::FILE)
	{
		auto ADD_COLUMN = [](const ArrayData& a, const std::string& name, std::vector<std::string>& c)
		{
			if (a.GetType() == ArrayData::COLUMN) c.emplace_back(a.GetColumn());
			else if (a.GetType() == ArrayData::UNIQUE) c.emplace_back("($1-$1+" + std::to_string(a.GetUnique()) + ")");
			else throw InvalidArg(name + "list in the file plot mode must be given in the form of the string column or unique value.");
		};
		std::vector<std::string> column;
		if (i.IsColormap())
		{
			auto& p = i.GetColormapParam();
			if (!p.m_z_map) throw InvalidArg("z coordinate list is not given.");
			if (p.m_z_map.GetType() == MatrixData::COLUMN) column.emplace_back(p.m_z_map.GetColumn());
			else if (p.m_z_map.GetType() == MatrixData::UNIQUE) column.emplace_back("($1-$1+" + std::to_string(p.m_z_map.GetUnique()) + ")");
			else throw InvalidArg("z coordinate list in the file plot mode must be given in the form of the string column or unique value.");

			if (!p.m_x_coord) throw InvalidArg("x coordinate list is not given.");
			ADD_COLUMN(p.m_x_coord, "x", column);
			if (!p.m_y_coord) throw InvalidArg("y coordinate list is not given.");
			ADD_COLUMN(p.m_y_coord, "y", column);
			//既存のファイルからプロットする場合、等高線表示に対応しない。
		}
		else if (i.IsPoint())
		{
			auto& p = i.GetPointParam();
			if (!p.m_x) throw InvalidArg("x coordinate list is not given.");
			ADD_COLUMN(p.m_x, "x", column);
			if (!p.m_y) throw InvalidArg("y coordinate list is not given.");
			ADD_COLUMN(p.m_y, "y", column);
			if (!p.m_z) throw InvalidArg("z coordinate list is not given.");
			ADD_COLUMN(p.m_z, "z", column);
			if (p.m_x_errorbar) ADD_COLUMN(p.m_x_errorbar, "xerrorbar", column);
			if (p.m_y_errorbar) ADD_COLUMN(p.m_y_errorbar, "yerrorbar", column);
			if (p.m_variable_color) ADD_COLUMN(p.m_variable_color, "variable_color", column);
			if (p.m_variable_size) ADD_COLUMN(p.m_variable_size, "variable_size", column);
		}
		else if (i.IsVector())
		{
			auto& v = i.GetVectorParam();
			if (!v.m_x) throw InvalidArg("x coordinate list is not given.");
			ADD_COLUMN(v.m_x, "x", column);
			if (!v.m_y) throw InvalidArg("y coordinate list is not given.");
			ADD_COLUMN(v.m_y, "y", column);
			if (!v.m_z) throw InvalidArg("z coordinate list is not given.");
			ADD_COLUMN(v.m_z, "z", column);
			if (!v.m_x_len) throw InvalidArg("xlen list is not given.");
			ADD_COLUMN(v.m_x_len, "xlen", column);
			if (!v.m_y_len) throw InvalidArg("ylen list is not given.");
			ADD_COLUMN(v.m_y_len, "ylen", column);
			if (!v.m_z_len) throw InvalidArg("zlen list is not given.");
			ADD_COLUMN(v.m_z_len, "zlen", column);
			if (v.m_variable_color) ADD_COLUMN(v.m_variable_color, "variable_color", column);
		}
		//colormapはFilledCurve非対応。
		i.m_column = std::move(column);
	}
	m_param.emplace_back(std::move(i));
	return std::move(*this);
}
template <class GraphParam>
template <class ...Options> requires all_keyword_args_tagged_with<plot::Point3DOption, Options...>
inline PlotBufferCM<GraphParam> PlotBufferCM<GraphParam>::
PlotPoints(RangeReceiver x, RangeReceiver y, RangeReceiver z, Options ...ops)
{
	GraphParam i;
	i.AssignPoint();
	i.m_type = GraphParam::DATA;
	i.SetBaseOptions(ops...);
	//point
	auto& p = i.GetPointParam();
	p.m_x = x;
	p.m_y = y;
	p.m_z = z;
	p.SetOptions(ops...);
	return Plot(i);
}
template <class GraphParam>
template <class ...Options> requires all_keyword_args_tagged_with<plot::Point3DOption, Options...>
inline PlotBufferCM<GraphParam> PlotBufferCM<GraphParam>::
PlotPoints(std::string_view filename,
		   std::string_view x, std::string_view y, std::string_view z,
		   Options ...ops)
{
	GraphParam i;
	i.AssignPoint();
	i.m_type = GraphParam::FILE;
	i.m_graph = filename;
	i.SetBaseOptions(ops...);

	auto& p = i.GetPointParam();
	p.m_x = x;
	p.m_y = y;
	p.m_z = z;
	p.SetOptions(ops...);
	return Plot(i);
}
template <class GraphParam>
template <class ...Options> requires all_keyword_args_tagged_with<plot::Point3DOption, Options...>
inline PlotBufferCM<GraphParam> PlotBufferCM<GraphParam>::
PlotPoints(RangeReceiver x, RangeReceiver y, Options ...ops)
{
	GraphParam i;
	i.AssignPoint();
	i.m_type = GraphParam::DATA;
	i.SetBaseOptions(ops...);

	//point
	auto& p = i.GetPointParam();
	p.m_x = x;
	p.m_y = y;
	p.m_z = 0.;
	p.SetOptions(ops...);
	return Plot(i);
}
template <class GraphParam>
template <class ...Options> requires all_keyword_args_tagged_with<plot::Point3DOption, Options...>
inline PlotBufferCM<GraphParam> PlotBufferCM<GraphParam>::
PlotPoints(std::string_view filename,
		   std::string_view x, std::string_view y,
		   Options ...ops)
{
	GraphParam i;
	i.AssignPoint();
	i.m_type = GraphParam::FILE;
	i.m_graph = filename;
	i.SetBaseOptions(ops...);

	auto& p = i.GetPointParam();
	p.m_x = x;
	p.m_y = y;
	p.m_z = 0.;
	p.SetOptions(ops...);
	return Plot(i);
}
template <class GraphParam>
template <class ...Options> requires all_keyword_args_tagged_with<plot::Point3DOption, Options...>
inline PlotBufferCM<GraphParam> PlotBufferCM<GraphParam>::
PlotPoints(std::string_view equation, Options ...ops)
{
	GraphParam i;
	i.AssignPoint();
	i.m_type = GraphParam::EQUATION;
	i.SetBaseOptions(ops...);
	i.m_graph = equation;

	auto& p = i.GetPointParam();
	p.SetOptions(ops...);
	return Plot(i);
}

template <class GraphParam>
template <class ...Options> requires all_keyword_args_tagged_with<plot::Point3DOption, Options...>
inline PlotBufferCM<GraphParam> PlotBufferCM<GraphParam>::
PlotLines(RangeReceiver x, RangeReceiver y, RangeReceiver z, Options ...ops)
{
	return PlotPoints(x, y, z, plot::style = Style::lines, ops...);
}
template <class GraphParam>
template <class ...Options> requires all_keyword_args_tagged_with<plot::Point3DOption, Options...>
inline PlotBufferCM<GraphParam> PlotBufferCM<GraphParam>::
PlotLines(std::string_view filename,
		  std::string_view x, std::string_view y, std::string_view z,
		  Options ...ops)
{
	return PlotPoints(filename, x, y, z, plot::style = Style::lines, ops...);
}
template <class GraphParam>
template <class ...Options> requires all_keyword_args_tagged_with<plot::Point3DOption, Options...>
inline PlotBufferCM<GraphParam> PlotBufferCM<GraphParam>::
PlotLines(RangeReceiver x, RangeReceiver y, Options ...ops)
{
	return PlotPoints(x, y, plot::style = Style::lines, ops...);
}
template <class GraphParam>
template <class ...Options> requires all_keyword_args_tagged_with<plot::Point3DOption, Options...>
inline PlotBufferCM<GraphParam> PlotBufferCM<GraphParam>::
PlotLines(std::string_view filename,
		  std::string_view x, std::string_view y,
		  Options ...ops)
{
	return PlotPoints(filename, x, y, plot::style = Style::lines, ops...);
}
template <class GraphParam>
template <class ...Options> requires all_keyword_args_tagged_with<plot::Point3DOption, Options...>
inline PlotBufferCM<GraphParam> PlotBufferCM<GraphParam>::
PlotLines(std::string_view equation, Options ...ops)
{
	return PlotPoints(equation, plot::style = Style::lines, ops...);
}

template <class GraphParam>
template <class ...Options> requires all_keyword_args_tagged_with<plot::Vector3DOption, Options...>
inline PlotBufferCM<GraphParam> PlotBufferCM<GraphParam>::
PlotVectors(RangeReceiver xfrom, RangeReceiver yfrom, RangeReceiver zfrom,
			RangeReceiver xlen, RangeReceiver ylen, RangeReceiver zlen,
			Options ...ops)
{
	GraphParam i;
	i.AssignVector();
	i.m_type = GraphParam::DATA;
	i.SetBaseOptions(ops...);

	//vector
	auto& v = i.GetVectorParam();
	v.m_x = xfrom;
	v.m_y = yfrom;
	v.m_z = zfrom;
	v.m_x_len = xlen;
	v.m_y_len = ylen;
	v.m_z_len = zlen;
	v.SetOptions(ops...);
	return Plot(i);
}
template <class GraphParam>
template <class ...Options> requires all_keyword_args_tagged_with<plot::Vector3DOption, Options...>
inline PlotBufferCM<GraphParam> PlotBufferCM<GraphParam>::
PlotVectors(std::string_view filename,
			std::string_view xfrom, std::string_view yfrom, std::string_view zfrom,
			std::string_view xlen, std::string_view ylen, std::string_view zlen,
			Options ...ops)
{
	GraphParam i;
	i.AssignVector();
	i.m_type = GraphParam::FILE;
	i.m_graph = filename;
	i.SetBaseOptions(ops...);

	//vector
	auto& v = i.GetVectorParam();
	v.m_x = xfrom;
	v.m_y = yfrom;
	v.m_z = zfrom;
	v.m_x_len = xlen;
	v.m_y_len = ylen;
	v.m_z_len = zlen;
	v.SetOptions(ops...);
	return Plot(i);
}
template <class GraphParam>
template <class ...Options> requires all_keyword_args_tagged_with<plot::Vector3DOption, Options...>
inline PlotBufferCM<GraphParam> PlotBufferCM<GraphParam>::
PlotVectors(RangeReceiver xfrom, RangeReceiver yfrom,
			RangeReceiver xlen, RangeReceiver ylen,
			Options ...ops)
{
	GraphParam i;
	i.AssignVector();
	i.m_type = GraphParam::DATA;
	i.SetBaseOptions(ops...);

	//vector
	auto& v = i.GetVectorParam();
	v.m_x = xfrom;
	v.m_y = yfrom;
	v.m_z = 0.;
	v.m_x_len = xlen;
	v.m_y_len = ylen;
	v.m_z_len = 0.;
	v.SetOptions(ops...);
	return Plot(i);
}
template <class GraphParam>
template <class ...Options> requires all_keyword_args_tagged_with<plot::Vector3DOption, Options...>
inline PlotBufferCM<GraphParam> PlotBufferCM<GraphParam>::
PlotVectors(std::string_view filename,
			std::string_view xfrom, std::string_view yfrom,
			std::string_view xlen, std::string_view ylen,
			Options ...ops)
{
	GraphParam i;
	i.AssignVector();
	i.m_type = GraphParam::FILE;
	i.m_graph = filename;
	i.SetBaseOptions(ops...);

	//vector
	auto& v = i.GetVectorParam();
	v.m_x = xfrom;
	v.m_y = yfrom;
	v.m_z = 0.;
	v.m_x_len = xlen;
	v.m_y_len = ylen;
	v.m_z_len = 0.;
	v.SetOptions(ops...);
	return Plot(i);
}

template <class GraphParam>
template <class Type, class ...Options> requires all_keyword_args_tagged_with<plot::ColormapOption, Options...>
inline PlotBufferCM<GraphParam> PlotBufferCM<GraphParam>::
PlotColormap(const Matrix<Type>& map, RangeReceiver x, RangeReceiver y,
			 Options ...ops)
{
	GraphParam i;
	i.AssignColormap();
	i.m_type = GraphParam::DATA;
	i.SetBaseOptions(ops...);

	//map
	auto& m = i.GetColormapParam();
	m.m_z_map = map;
	m.m_x_coord = x;
	m.m_y_coord = y;
	m.SetOptions(ops...);
	return Plot(i);
}
template <class GraphParam>
template <class Type, class ...Options> requires all_keyword_args_tagged_with<plot::ColormapOption, Options...>
inline PlotBufferCM<GraphParam> PlotBufferCM<GraphParam>::
PlotColormap(const Matrix<Type>& map, std::pair<double, double> x, std::pair<double, double> y,
			 Options ...ops)
{
	GraphParam i;
	i.AssignColormap();
	i.m_type = GraphParam::DATA;
	i.SetBaseOptions(ops...);

	//map
	auto& m = i.GetColormapParam();
	m.m_z_map = map;
	m.m_x_range = x;
	m.m_y_range = y;
	m.SetOptions(ops...);
	return Plot(i);
}
template <class GraphParam>
template <class ...Options> requires all_keyword_args_tagged_with<plot::ColormapOption, Options...>
inline PlotBufferCM<GraphParam> PlotBufferCM<GraphParam>::
PlotColormap(std::string_view filename, std::string_view z, std::string_view x, std::string_view y,
			 Options ...ops)
{
	GraphParam i;
	i.AssignColormap();
	i.m_type = GraphParam::FILE;
	i.m_graph = filename;
	i.SetBaseOptions(ops...);

	//map
	auto& m = i.GetColormapParam();
	m.m_z_map = z;
	m.m_x_coord = x;
	m.m_y_coord = y;
	m.SetOptions(ops...);
	return Plot(i);
}
template <class GraphParam>
template <class ...Options> requires all_keyword_args_tagged_with<plot::ColormapOption, Options...>
inline PlotBufferCM<GraphParam> PlotBufferCM<GraphParam>::
PlotColormap(std::string_view equation, Options ...ops)
{
	GraphParam i;
	i.AssignColormap();
	i.m_graph = equation;
	i.m_type = GraphParam::DATA;
	i.SetBaseOptions(ops...);

	//map
	auto& m = i.GetColormapParam();
	m.SetOptions(ops...);
	return Plot(i);
}

template <class GraphParam>
inline std::string PlotBufferCM<GraphParam>::PlotCommand(const GraphParam& p, const bool IsInMemoryDataTransferEnabled)
{
	//filename or equation
	std::string c;
	switch (p.m_type) 
	{
	case GraphParam::EQUATION:
		//equation
		c += " " + p.m_graph;
		break;
	case GraphParam::FILE:
		//filename
		c += " '" + p.m_graph + "'";

		//using
		c += " using ";
		for (size_t i = 0; i < p.m_column.size(); ++i)
			c += p.m_column[i] + ":";
		c.pop_back();
		break;
	case GraphParam::DATA:
		if (IsInMemoryDataTransferEnabled) 
		{
			//variable name
			c += " " + p.m_graph;
		}
		else {
			//filename
			c += " '" + p.m_graph + "'";
		}

		//using
		c += " using ";
		for (size_t i = 0; i < p.m_column.size(); ++i)
			c += p.m_column[i] + ":";
		c.pop_back();
		break;
	}

	//title
	if (!p.m_title.empty())
	{
		if (p.m_title == "notitle") c += " notitle";
		else c += " title '" + p.m_title + "'";
	}

	c += " with";
	//カラーマップの場合。
	if (p.IsColormap())
	{
		c += " pm3d";
		auto& m = p.GetColormapParam();
		if (m.m_without_surface) c += " nosurface";
	}
	//ベクトルの場合。
	else if (p.IsVector())
	{
		c += VectorPlotCommand(p.GetVectorParam());
	}
	//点の場合。
	else if (p.IsPoint())
	{
		c += PointPlotCommand(p.GetPointParam());
	}

	//axis
	if (!p.m_axis.empty()) c += " axes " + p.m_axis;

	//もしカラーマップでcontourが有効だったら、
	//更にそれを描画するコマンドを追加する。
	if (p.IsColormap())
	{
		auto& m = p.GetColormapParam();
		if (m.m_with_contour)
		{
			if (IsInMemoryDataTransferEnabled) 
			{
				c += ", " + p.m_graph + "_cntr with line";
			}
			else
			{
				std::string str = "'" + p.m_graph;
				str.erase(str.end() - 3, str.end());
				c += ", " + str + "cntr.txt' with line";
			}
			if (p.m_title == "notitle") c += " notitle";
			else c += " title '" + p.m_title + "'";
			if (m.m_cntr_line_type != -2) c += std::format(" linetype {}", m.m_cntr_line_type);
			if (m.m_cntr_line_width != -1) c += std::format(" linewidth {}", m.m_cntr_line_width);
			if (m.m_variable_cntr_color) c += " linecolor palette";
			else if (!m.m_cntr_color.empty()) c += " linecolor '" + m.m_cntr_color + "'";
		}
	}
	return c;
}
template <class GraphParam>
inline std::string PlotBufferCM<GraphParam>::InitCommand()
{
	std::string c;
	c += "set autoscale\n";
	c += "unset logscale\n";
	c += "unset title\n";
	c += "unset grid\n";
	c += "set size noratio\n";
	c += "unset pm3d\n";
	c += "unset contour\n";
	c += "set surface";
	return c;
}

template <class GraphParam, template <class> class Buffer>
inline CanvasCM<GraphParam, Buffer>::CanvasCM(const std::string& output, double sizex, double sizey)
	: detail::Axis2D<Canvas>(output, sizex, sizey)
{
	if (m_pipe)
	{
		this->Command("set pm3d corners2color c1");
		this->Command("set view map");
	}
}
template <class GraphParam, template <class> class Buffer>
inline CanvasCM<GraphParam, Buffer>::CanvasCM()
{
	if (m_pipe)
	{
		this->Command("set pm3d corners2color c1");
		this->Command("set view map");
	}
}

template <class GraphParam, template <class> class Buffer>
template <class ...Options> requires all_keyword_args_tagged_with<plot::Point3DOption, Options...>
inline Buffer<GraphParam> CanvasCM<GraphParam, Buffer>::
PlotPoints(RangeReceiver x, RangeReceiver y, RangeReceiver z, Options ...ops)
{
	_Buffer p(this);
	return p.PlotPoints(x, y, z, ops...);
}
template <class GraphParam, template <class> class Buffer>
template <class ...Options> requires all_keyword_args_tagged_with<plot::Point3DOption, Options...>
inline Buffer<GraphParam> CanvasCM<GraphParam, Buffer>::
PlotPoints(std::string_view filename,
		   std::string_view x, std::string_view y, std::string_view z,
		   Options ...ops)
{
	_Buffer p(this);
	return p.PlotPoints(filename, x, y, z, ops...);
}
template <class GraphParam, template <class> class Buffer>
template <class ...Options> requires all_keyword_args_tagged_with<plot::Point3DOption, Options...>
inline Buffer<GraphParam> CanvasCM<GraphParam, Buffer>::
PlotPoints(RangeReceiver x, RangeReceiver y, Options ...ops)
{
	_Buffer p(this);
	return p.PlotPoints(x, y, ops...);
}
template <class GraphParam, template <class> class Buffer>
template <class ...Options> requires all_keyword_args_tagged_with<plot::Point3DOption, Options...>
inline Buffer<GraphParam> CanvasCM<GraphParam, Buffer>::
PlotPoints(std::string_view filename,
		   std::string_view x, std::string_view y,
		   Options ...ops)
{
	_Buffer p(this);
	return p.PlotPoints(filename, x, y, ops...);
}
template <class GraphParam, template <class> class Buffer>
template <class ...Options> requires all_keyword_args_tagged_with<plot::Point3DOption, Options...>
inline Buffer<GraphParam> CanvasCM<GraphParam, Buffer>::
PlotPoints(std::string_view equation, Options ...ops)
{
	_Buffer p(this);
	return p.PlotPoints(equation, ops...);
}

template <class GraphParam, template <class> class Buffer>
template <class ...Options> requires all_keyword_args_tagged_with<plot::Point3DOption, Options...>
inline Buffer<GraphParam> CanvasCM<GraphParam, Buffer>::
PlotLines(RangeReceiver x, RangeReceiver y, RangeReceiver z, Options ...ops)
{
	_Buffer p(this);
	return p.PlotLines(x, y, z, ops...);
}
template <class GraphParam, template <class> class Buffer>
template <class ...Options> requires all_keyword_args_tagged_with<plot::Point3DOption, Options...>
inline Buffer<GraphParam> CanvasCM<GraphParam, Buffer>::
PlotLines(std::string_view filename,
		   std::string_view x, std::string_view y, std::string_view z,
		   Options ...ops)
{
	_Buffer p(this);
	return p.PlotLines(filename, x, y, z, ops...);
}
template <class GraphParam, template <class> class Buffer>
template <class ...Options> requires all_keyword_args_tagged_with<plot::Point3DOption, Options...>
inline Buffer<GraphParam> CanvasCM<GraphParam, Buffer>::
PlotLines(RangeReceiver x, RangeReceiver y, Options ...ops)
{
	_Buffer p(this);
	return p.PlotLines(x, y, ops...);
}
template <class GraphParam, template <class> class Buffer>
template <class ...Options> requires all_keyword_args_tagged_with<plot::Point3DOption, Options...>
inline Buffer<GraphParam> CanvasCM<GraphParam, Buffer>::
PlotLines(std::string_view filename,
		  std::string_view x, std::string_view y,
		  Options ...ops)
{
	_Buffer p(this);
	return p.PlotLines(filename, x, y, ops...);
}
template <class GraphParam, template <class> class Buffer>
template <class ...Options> requires all_keyword_args_tagged_with<plot::Point3DOption, Options...>
inline Buffer<GraphParam> CanvasCM<GraphParam, Buffer>::
PlotLines(std::string_view equation, Options ...ops)
{
	_Buffer p(this);
	return p.PlotLines(equation, ops...);
}

template <class GraphParam, template <class> class Buffer>
template <class ...Options> requires all_keyword_args_tagged_with<plot::Vector3DOption, Options...>
inline Buffer<GraphParam> CanvasCM<GraphParam, Buffer>::
PlotVectors(RangeReceiver xfrom, RangeReceiver yfrom, RangeReceiver zfrom,
			RangeReceiver xlen, RangeReceiver ylen, RangeReceiver zlen,
			Options ...ops)
{
	_Buffer p(this);
	return p.PlotVectors(xfrom, yfrom, zfrom, xlen, ylen, zlen, ops...);
}
template <class GraphParam, template <class> class Buffer>
template <class ...Options> requires all_keyword_args_tagged_with<plot::Vector3DOption, Options...>
inline Buffer<GraphParam> CanvasCM<GraphParam, Buffer>::
PlotVectors(std::string_view filename,
			std::string_view xfrom, std::string_view yfrom, std::string_view zfrom,
			std::string_view xlen, std::string_view ylen, std::string_view zlen,
			Options ...ops)
{
	_Buffer p(this);
	return p.PlotVectors(filename, xfrom, yfrom, zfrom, xlen, ylen, zlen, ops...);
}
template <class GraphParam, template <class> class Buffer>
template <class ...Options> requires all_keyword_args_tagged_with<plot::Vector3DOption, Options...>
inline Buffer<GraphParam> CanvasCM<GraphParam, Buffer>::
PlotVectors(RangeReceiver xfrom, RangeReceiver yfrom,
			RangeReceiver xlen, RangeReceiver ylen,
			Options ...ops)
{
	_Buffer p(this);
	return p.PlotVectors(xfrom, yfrom, xlen, ylen, ops...);
}
template <class GraphParam, template <class> class Buffer>
template <class ...Options> requires all_keyword_args_tagged_with<plot::Vector3DOption, Options...>
inline Buffer<GraphParam> CanvasCM<GraphParam, Buffer>::
PlotVectors(std::string_view filename,
			std::string_view xfrom, std::string_view yfrom,
			std::string_view xlen, std::string_view ylen,
			Options ...ops)
{
	_Buffer p(this);
	return p.PlotVectors(filename, xfrom, yfrom, xlen, ylen, ops...);
}

template <class GraphParam, template <class> class Buffer>
template <class Type, class ...Options> requires all_keyword_args_tagged_with<plot::ColormapOption, Options...>
Buffer<GraphParam> CanvasCM<GraphParam, Buffer>::
PlotColormap(const Matrix<Type>& map, RangeReceiver x, RangeReceiver y,
			 Options ...ops)
{
	_Buffer p(this);
	return p.PlotColormap(map, x, y, ops...);
}
template <class GraphParam, template <class> class Buffer>
template <class Type, class ...Options> requires all_keyword_args_tagged_with<plot::ColormapOption, Options...>
Buffer<GraphParam> CanvasCM<GraphParam, Buffer>::
PlotColormap(const Matrix<Type>& map, std::pair<double, double> x, std::pair<double, double> y,
			 Options ...ops)
{
	_Buffer p(this);
	return p.PlotColormap(map, x, y, ops...);
}
template <class GraphParam, template <class> class Buffer>
template <class ...Options> requires all_keyword_args_tagged_with<plot::ColormapOption, Options...>
Buffer<GraphParam> CanvasCM<GraphParam, Buffer>::
PlotColormap(std::string_view filename, std::string_view z, std::string_view x, std::string_view y,
			 Options ...ops)
{
	_Buffer p(this);
	return p.PlotColormap(filename, z, x, y, ops...);
}
template <class GraphParam, template <class> class Buffer>
template <class ...Options> requires all_keyword_args_tagged_with<plot::ColormapOption, Options...>
Buffer<GraphParam> CanvasCM<GraphParam, Buffer>::
PlotColormap(std::string_view equation, Options ...ops)
{
	_Buffer p(this);
	return p.PlotColormap(equation, ops...);
}
template <class GraphParam, template <class> class Buffer>
Buffer<GraphParam> CanvasCM<GraphParam, Buffer>::GetBuffer()
{
	return _Buffer(this);
}

}

using Canvas2D = detail::Canvas2D<detail::GraphParam2D, detail::PlotBuffer2D>;
using CanvasCM = detail::CanvasCM<detail::GraphParamCM, detail::PlotBufferCM>;

}

#endif
