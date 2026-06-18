#ifndef ADAPT_PLOT_CANVAS_H
#define ADAPT_PLOT_CANVAS_H

#include <OpenADAPT/Utility/Macros.h>
#include <OpenADAPT/Utility/Math.h>
#include <OpenADAPT/Utility/Matrix.h>
#include <OpenADAPT/Utility/Function.h>
#include <OpenADAPT/Utility/KeywordArgs.h>
#include <OpenADAPT/Plot/Core.h>
#include <OpenADAPT/Plot/Axes.h>
#include <OpenADAPT/Plot/PlotCommand.h>
#include <OpenADAPT/Optimization/LeastSquares.h>

namespace adapt
{

namespace plot_detail
{

class Canvas2D;
class Canvas3D;

struct PlotBuffer2D
{
	PlotBuffer2D(Canvas2D* g) : m_canvas(g) {}
	PlotBuffer2D(const PlotBuffer2D&) = delete;
	PlotBuffer2D(PlotBuffer2D&& p) noexcept
		: m_commands(std::move(p.m_commands)), m_canvas(p.m_canvas),
		m_min(p.m_min), m_max(p.m_max), m_nbin(p.m_nbin),
		m_stacked_histogram_bins(std::move(p.m_stacked_histogram_bins)),
		m_stacked_histogram_data(std::move(p.m_stacked_histogram_data))
	{
		p.m_canvas = nullptr;
	}
	PlotBuffer2D& operator=(const PlotBuffer2D&) = delete;
	PlotBuffer2D& operator=(PlotBuffer2D&& p) noexcept
	{
		m_canvas = p.m_canvas; p.m_canvas = nullptr;
		m_commands = std::move(p.m_commands);
		m_min = p.m_min; m_max = p.m_max; m_nbin = p.m_nbin;
		m_stacked_histogram_bins = std::move(p.m_stacked_histogram_bins);
		m_stacked_histogram_data = std::move(p.m_stacked_histogram_data);
		return *this;
	}
	virtual ~PlotBuffer2D();

	void Flush();

private:
	template <acceptable_arg_except_string X, acceptable_arg_except_string Y, point_option ...Options>
	auto MakeFitPointParam(const X& x, const Y& y, Options ...ops)
	{
		if constexpr (KeywordExists(plot::fitting, ops...))
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
			auto&& fit_opt = GetKeywordArg(plot::fitting, ops...);
			std::span<const double> params = fit_opt.params;
			auto&& [x_span, x_storage] = get_double_span(x);
			auto&& [y_span, y_storage] = get_double_span(y);
			LeastSquaresResult fit_result;
			if constexpr (KeywordExists(plot::yerrorbar))
			{
				auto&& [w_span, w_storage] = get_double_span(GetKeywordArg(plot::yerrorbar, ops...));
				fit_result = SolveLeastSquares(fit_opt.func, params, x_span, y_span, opts::ls_weight = w_span);
				std::ranges::copy(fit_result.params, std::ranges::begin(fit_opt.params));
			}
			else
			{
				fit_result = SolveLeastSquares(fit_opt.func, params, x_span, y_span);
				std::ranges::copy(fit_result.params, std::ranges::begin(fit_opt.params));
			}
			std::string equation = fit_opt.func.GetEquation(fit_opt.params);
			auto make = [&equation](auto&&... args) { return MakePointParam(plot::input = equation, args...); };
			std::string title = fit_opt.func.GetTitle(fit_opt.params);
			return std::apply(make, TupleCat(fit_opt.options, std::forward_as_tuple(plot::title = title, plot::s_lines)));
		}
		else
			return EmptyClass{};
	}
public:

	template <acceptable_arg_except_string X, acceptable_arg_except_string Y,
			  point_option ...Options>
	PlotBuffer2D PlotPoints(const X& x, const Y& y, Options ...ops)
	{
		auto p_fit = MakeFitPointParam(x, y, ops...);
		auto p = MakePointParam(plot::x = x, plot::y = y, ops...);
		auto buf = PlotPoints(p);
		if constexpr (!std::is_same_v<std::decay_t<decltype(p_fit)>, EmptyClass>)
			return buf.PlotPoints(p_fit);
		else
			return buf;
	}
	template <point_option ...Options>
	PlotBuffer2D PlotPoints(std::string_view filename, std::string_view xcol, std::string_view ycol, Options ...ops)
	{
		auto p = MakePointParam(plot::input = filename, plot::x = xcol, plot::y = ycol, ops...);
		return PlotPoints(p);
	}
	template <point_option ...Options>
	PlotBuffer2D PlotPoints(std::string_view equation, Options ...ops)
	{
		auto p = MakePointParam(plot::input = equation, ops...);
		return PlotPoints(p);
	}

	template <acceptable_arg_except_string X, acceptable_arg_except_string Y,
			  point_option ...Options>
	PlotBuffer2D PlotLines(X x, Y y, Options ...ops)
	{
		return PlotPoints(x, y, plot::style = Style::lines, ops...);
	}
	template <point_option ...Options>
	PlotBuffer2D PlotLines(std::string_view filename, std::string_view xcol, std::string_view ycol, Options ...ops)
	{
		return PlotPoints(filename, xcol, ycol, plot::style = Style::lines, ops...);
	}
	template <point_option ...Options>
	PlotBuffer2D PlotLines(std::string_view equation, Options ...ops)
	{
		return PlotPoints(equation, plot::style = Style::lines, ops...);
	}

	template <ranges::arithmetic_range Data, histogram_option ...Options>
	PlotBuffer2D PlotHistogram(const Data& data, double min, double max, size_t nbin, Options ...ops)
	{
		auto p = MakeHistogramParam(plot::data = data, plot::min = min, plot::max = max, plot::nbin = nbin, ops...);
		return PlotHistogram(p, ops...);
	}
	template <ranges::arithmetic_range Data, histogram_option ...Options>
	PlotBuffer2D PlotHistogram(const Data& data, Options ...ops)
	{
		static_assert(KeywordExists(plot::stack, ops...));
		if ((this->m_min == 0 && this->m_max == 0 && this->m_nbin == 0) || this->m_stacked_histogram_data.empty())
		{
			throw InvalidArg("When stack option is specified, min, max, nbin options must be specified or there must be already plotted histogram data to be stacked on.");
		}
		auto p = MakeHistogramParam(plot::data = data, plot::min = this->m_min, plot::max = this->m_max, plot::nbin = this->m_nbin, ops...);
		return PlotHistogram(p, ops...);
	}
	template <ranges::arithmetic_range DataX, ranges::arithmetic_range DataY, binscatter_option ...Options>
	PlotBuffer2D PlotBinscatter(const DataX& x, double xmin, double xmax, size_t xnbin,
								const DataY& y, double ymin, double ymax, size_t ynbin,
								Options ...ops)
	{
		auto p = MakeBinscatterParam(plot::datax = x, plot::xmin = xmin, plot::xmax = xmax, plot::xnbin = xnbin,
									 plot::datay = y, plot::ymin = ymin, plot::ymax = ymax, plot::ynbin = ynbin,
									 ops...);
		return PlotBinscatter(p, ops...);
	}

	template <acceptable_arg_except_string X, acceptable_arg_except_string Y,
			  acceptable_arg_except_string XL, acceptable_arg_except_string YL,
			  vector_option ...Options>
	PlotBuffer2D PlotVectors(const X& xfrom, const Y& yfrom,
							 const XL& xlen, const YL& ylen,
							 Options ...ops)
	{
		auto p = MakeVectorParam(plot::x = xfrom, plot::y = yfrom, plot::xlen = xlen, plot::ylen = ylen, ops...);
		return PlotVectors(p);
	}
	template <vector_option ...Options>
	PlotBuffer2D PlotVectors(std::string_view filename,
							 std::string_view xfrom, std::string_view yfrom,
							 std::string_view xlen, std::string_view ylen,
							 Options ...ops)
	{
		auto p = MakeVectorParam(plot::input = filename,
								 plot::x = xfrom, plot::xlen = xlen, plot::y = yfrom, plot::ylen = ylen, ops...);
		return PlotVectors(p);
	}

	template <acceptable_arg_except_string X, acceptable_arg_except_string Y,
			  filledcurve_option ...Options>
	PlotBuffer2D PlotFilledCurves(const X& x, const Y& y, Options ...ops)
	{
		auto p = MakeFilledCurveParam(plot::x = x, plot::y = y, ops...);
		return PlotFilledCurves(p);
	}
	template <filledcurve_option ...Options>
	PlotBuffer2D PlotFilledCurves(std::string_view filename, std::string_view x, std::string_view y, Options ...ops)
	{
		auto p = MakeFilledCurveParam(plot::input = filename, plot::x = x, plot::y = y, ops...);
		return PlotFilledCurves(p);
	}
	template <acceptable_arg_except_string X, acceptable_arg_except_string Y,
			  acceptable_arg_except_string Y2,
			  filledcurve_option ...Options>
	PlotBuffer2D PlotFilledCurves(const X& x, const Y& y, const Y2& ybelow, Options ...ops)
	{
		auto p = MakeFilledCurveParam(plot::x = x, plot::y = y, plot::ybelow = ybelow, ops...);
		return PlotFilledCurves(p);
	}
	template <filledcurve_option ...Options>
	PlotBuffer2D PlotFilledCurves(std::string_view filename, std::string_view x, std::string_view y, std::string_view ybelow,
								  Options ...ops)
	{
		auto p = MakeFilledCurveParam(plot::input = filename, plot::x = x, plot::y = y, plot::ybelow = ybelow, ops...);
		return PlotFilledCurves(p);
	}

	template <acceptable_arg_except_string X, acceptable_arg_except_string Y, acceptable_arg_except_string L,
			  label_option ...Options>
	PlotBuffer2D PlotLabels(const X& x, const Y& y, const L& label, Options ...ops)
	{
		auto p = MakeLabelParam(plot::x = x, plot::y = y, plot::label = label, ops...);
		return PlotLabels(p);
	}
	template <label_option ...Options>
	PlotBuffer2D PlotLabels(std::string_view filename, std::string_view xcol, std::string_view ycol, std::string_view labelcol, Options ...ops)
	{
		auto p = MakeLabelParam(plot::input = filename, plot::x = xcol, plot::y = ycol, plot::label = labelcol, ops...);
		return PlotLabels(p);
	}

	template <acceptable_matrix_range Map,
		ranges::arithmetic_range XRange, ranges::arithmetic_range YRange,
		colormap_option ...Options>
	PlotBuffer2D PlotColormap(const Map& map_, const XRange& x, const YRange& y,
							  Options ...ops)
	{
		auto p = MakeColormapParam(plot::map = map_, plot::xrange = x, plot::yrange = y, ops...);
		return PlotColormap(p);
	}
	template <acceptable_matrix_range Map, colormap_option ...Options>
	PlotBuffer2D PlotColormap(const Map& map_, std::pair<double, double> x, std::pair<double, double> y,
							  Options ...ops)
	{
		auto p = MakeColormapParam(plot::map = map_, plot::xminmax = x, plot::yminmax = y, ops...);
		return PlotColormap(p);
	}
	template <colormap_option ...Options>
	PlotBuffer2D PlotColormap(std::string_view filename, std::string_view x, std::string_view y, std::string_view map,
							  Options ...ops)
	{
		auto p = MakeColormapParam(plot::input = filename, plot::map = map, plot::xrange = x, plot::yrange = y, ops...);
		return PlotColormap(p);
	}
	template <colormap_option ...Options>
	PlotBuffer2D PlotColormap(std::string_view equation, Options ...ops)
	{
		auto p = MakeColormapParam(plot::input = equation, ops...);
		return PlotColormap(p);
	}

protected:

	std::string GetSanitizedOutputName() const;

	template <class X, class Y, class XE, class YE, class XEL, class XEH, class YEL, class YEH, class VC, class VS>
	PlotBuffer2D PlotPoints(const PointParam<X, Y, XE, YE, XEL, XEH, YEL, YEH, VC, VS>& p);
	template <bool MakeDataObj, class X, class Y, class XE, class YE, class XEL, class XEH, class YEL, class YEH, class VC, class VS>
	PlotBuffer2D PlotPoints(std::bool_constant<MakeDataObj>, const PointParam<X, Y, XE, YE, XEL, XEH, YEL, YEH, VC, VS>& p);
	template <class Data, class Weight, class ...Options>
	PlotBuffer2D PlotHistogram(const HistogramParam<Data, Weight>& p, Options ...ops);
	template <class X, class Y, class Weight, class ...Options>
	PlotBuffer2D PlotBinscatter(const BinscatterParam<X, Y, Weight>& p, Options ...ops);
	template <class X, class Y, class XL, class YL, class VC>
	PlotBuffer2D PlotVectors(const VectorParam<X, Y, XL, YL, VC>& p);
	template <class X, class Y, class Y2, class VC>
	PlotBuffer2D PlotFilledCurves(const FilledCurveParam<X, Y, Y2, VC>& p);
	template <bool MakeDataObj, class X, class Y, class Y2, class VC>
	PlotBuffer2D PlotFilledCurves(std::bool_constant<MakeDataObj>, const FilledCurveParam<X, Y, Y2, VC>& p);
	template <class X, class Y, class L, class VTC>
	PlotBuffer2D PlotLabels(const LabelParam<X, Y, L, VTC>& p);
	template <class Map, class X, class Y>
	PlotBuffer2D PlotColormap(const ColormapParam<Map, X, Y>& p);

	std::vector<std::string> m_commands;
	Canvas2D* m_canvas;

	//スタックヒストグラムのデータを保持する。PlotHistogram_implでデータを追加していき、PlotHistogramの最後にまとめて出力する。
	double m_min = 0, m_max = 0;
	size_t m_nbin = 0;
	std::vector<double> m_stacked_histogram_bins;//スタックヒストグラムの累積値を保持する。PlotHistogram_implで更新していく。
	std::vector<std::pair<std::string, std::vector<double>>> m_stacked_histogram_data;
};

class Canvas2D : public Canvas<AxisX<Canvas2D>, AxisY<Canvas2D>, AxisX2<Canvas2D>, AxisY2<Canvas2D>, AxisCB<Canvas2D>>
{
public:

	using Canvas<AxisX<Canvas2D>, AxisY<Canvas2D>, AxisX2<Canvas2D>, AxisY2<Canvas2D>, AxisCB<Canvas2D>>::Canvas;
	friend class MultiPlotter;

	template <acceptable_arg_except_string X, acceptable_arg_except_string Y,
			  point_option ...Options>
	PlotBuffer2D PlotPoints(const X& x, const Y& y, Options ...ops)
	{
		PlotBuffer2D r(this);
		return r.PlotPoints(x, y, ops...);
	}
	template <point_option ...Options>
	PlotBuffer2D PlotPoints(std::string_view filename, std::string_view xcol, std::string_view ycol, Options ...ops)
	{
		PlotBuffer2D r(this);
		return r.PlotPoints(filename, xcol, ycol, ops...);
	}
	template <point_option ...Options>
	PlotBuffer2D PlotPoints(std::string_view equation, Options ...ops)
	{
		PlotBuffer2D r(this);
		return r.PlotPoints(equation, ops...);
	}

	template <acceptable_arg_except_string X, acceptable_arg_except_string Y,
			  point_option ...Options>
	PlotBuffer2D PlotLines(X x, Y y, Options ...ops)
	{
		PlotBuffer2D r(this);
		return r.PlotLines(x, y, ops...);
	}
	template <point_option ...Options>
	PlotBuffer2D PlotLines(std::string_view filename, std::string_view xcol, std::string_view ycol, Options ...ops)
	{
		PlotBuffer2D r(this);
		return r.PlotLines(filename, xcol, ycol, ops...);
	}
	template <point_option ...Options>
	PlotBuffer2D PlotLines(std::string_view equation, Options ...ops)
	{
		PlotBuffer2D r(this);
		return r.PlotLines(equation, ops...);
	}

	template <ranges::arithmetic_range Data, histogram_option ...Options>
	PlotBuffer2D PlotHistogram(const Data& data, double min, double max, size_t nbin, Options ...ops)
	{
		PlotBuffer2D r(this);
		return r.PlotHistogram(data, min, max, nbin, ops...);
	}

	template <ranges::arithmetic_range DataX, ranges::arithmetic_range DataY, binscatter_option ...Options>
	PlotBuffer2D PlotBinscatter(const DataX& x, double xmin, double xmax, size_t xnbin,
								const DataY& y, double ymin, double ymax, size_t ynbin,
								Options ...ops)
	{
		PlotBuffer2D r(this);
		return r.PlotBinscatter(x, xmin, xmax, xnbin, y, ymin, ymax, ynbin, ops...);
	}

	template <acceptable_arg_except_string X, acceptable_arg_except_string Y,
			  acceptable_arg_except_string XL, acceptable_arg_except_string YL,
			  vector_option ...Options>
	PlotBuffer2D PlotVectors(const X& xfrom, const Y& yfrom,
							 const XL& xlen, const YL& ylen,
							 Options ...ops)
	{
		PlotBuffer2D r(this);
		return r.PlotVectors(xfrom, yfrom, xlen, ylen, ops...);
	}
	template <vector_option ...Options>
	PlotBuffer2D PlotVectors(std::string_view filename,
							 std::string_view xbegin, std::string_view xlen,
							 std::string_view ybegin, std::string_view ylen,
							 Options ...ops)
	{
		PlotBuffer2D r(this);
		return r.PlotVectors(filename, xbegin, ybegin, xlen, ylen, ops...);
	}

	template <acceptable_arg_except_string X, acceptable_arg_except_string Y,
			  filledcurve_option ...Options>
	PlotBuffer2D PlotFilledCurves(const X& x, const Y& y, Options ...ops)
	{
		PlotBuffer2D r(this);
		return r.PlotFilledCurves(x, y, ops...);
	}
	template <filledcurve_option ...Options>
	PlotBuffer2D PlotFilledCurves(std::string_view filename, std::string_view x, std::string_view y, Options ...ops)
	{
		PlotBuffer2D r(this);
		return r.PlotFilledCurves(filename, x, y, ops...);
	}
	template <acceptable_arg_except_string X, acceptable_arg_except_string Y,
			  acceptable_arg_except_string Y2,
			  filledcurve_option ...Options>
	PlotBuffer2D PlotFilledCurves(const X& x, const Y& y, const Y2& y2, Options ...ops)
	{
		PlotBuffer2D r(this);
		return r.PlotFilledCurves(x, y, y2, ops...);
	}
	template <filledcurve_option ...Options>
	PlotBuffer2D PlotFilledCurves(std::string_view filename, std::string_view x, std::string_view y, std::string_view y2,
								  Options ...ops)
	{
		PlotBuffer2D r(this);
		return r.PlotFilledCurves(filename, x, y, y2, ops...);
	}
	template <acceptable_arg_except_string X, acceptable_arg_except_string Y, acceptable_arg_except_string L,
		label_option ...Options>
	PlotBuffer2D PlotLabels(const X& x, const Y& y, const L& label, Options ...ops)
	{
		PlotBuffer2D r(this);
		return r.PlotLabels(x, y, label, ops...);
	}
	template <label_option ...Options>
	PlotBuffer2D PlotLabels(std::string_view filename, std::string_view xcol, std::string_view ycol, std::string_view labelcol, Options ...ops)
	{
		PlotBuffer2D r(this);
		return r.PlotLabels(filename, xcol, ycol, labelcol, ops...);
	}
	template <acceptable_matrix_range Map, ranges::arithmetic_range X, ranges::arithmetic_range Y,
		colormap_option ...Options>
	PlotBuffer2D PlotColormap(const Map& map, const X& x, const Y& y,
							  Options ...ops)
	{
		PlotBuffer2D p(this);
		return p.PlotColormap(map, x, y, ops...);
	}
	template <acceptable_matrix_range Map, colormap_option ...Options>
	PlotBuffer2D PlotColormap(const Map& map, std::pair<double, double> x, std::pair<double, double> y,
							  Options ...ops)
	{
		PlotBuffer2D p(this);
		return p.PlotColormap(map, x, y, ops...);
	}
	template <colormap_option ...Options>
	PlotBuffer2D PlotColormap(std::string_view filename, std::string_view x, std::string_view y, std::string_view z,
							  Options ...ops)
	{
		PlotBuffer2D p(this);
		return p.PlotColormap(filename, x, y, z, ops...);
	}
	template <colormap_option ...Options>
	PlotBuffer2D PlotColormap(std::string_view equation, Options ...ops)
	{
		PlotBuffer2D p(this);
		return p.PlotColormap(equation, ops...);
	}

	PlotBuffer2D GetBuffer()
	{
		return PlotBuffer2D(this);
	}
};

inline PlotBuffer2D::~PlotBuffer2D()
{
	//mCanvasがnullptrでないときはこのPlotBufferが最終処理を担当する。
	if (m_canvas != nullptr)
	{
		if (!m_stacked_histogram_data.empty())
		{
			size_t stack_size = m_stacked_histogram_data.size();
			for (size_t i = 0; i < stack_size; ++i)
			{
				// 自身より後ろのヒストグラムをすべて加算する。
				for (size_t j = i + 1; j < stack_size; ++j)
				{
					auto& self = m_stacked_histogram_data[i].second;
					const auto& data = m_stacked_histogram_data[j].second;
					for (size_t k = 0; k < data.size(); ++k)
					{
						self[k] += data[k];
					}
				}
				MakeDataObject(m_canvas, m_stacked_histogram_data[i].first, std::tie(m_stacked_histogram_bins, m_stacked_histogram_data[i].second));
			}
		}
		Flush();
	}
}
inline void PlotBuffer2D::Flush()
{
	if (m_canvas == nullptr) throw NotInitialized("Buffer is empty");
	std::string c = "plot";
	for (auto& i : m_commands)
	{
		c += i + ", ";
	}
	c.erase(c.end() - 2, c.end());
	m_canvas->Command(c);
	m_canvas = nullptr;
}
inline std::string PlotBuffer2D::GetSanitizedOutputName() const
{
	if (m_canvas->IsInMemoryDataTransferEnabled())
		return "$" + SanitizeForDataBlock(m_canvas->GetOutput()) + "_" + std::to_string(m_commands.size());
	else
		return m_canvas->GetOutput() + ".tmp" + std::to_string(m_commands.size()) + ".txt";
}
template <class X, class Y, class XE, class YE, class XEL, class XEH, class YEL, class YEH, class VC, class VS>
PlotBuffer2D PlotBuffer2D::PlotPoints(const PointParam<X, Y, XE, YE, XEL, XEH, YEL, YEH, VC, VS>& p)
{
	return PlotPoints(std::true_type{}, p);
}
template <bool MakeDataObj, class X, class Y, class XE, class YE, class XEL, class XEH, class YEL, class YEH, class VC, class VS>
PlotBuffer2D PlotBuffer2D::PlotPoints(std::bool_constant<MakeDataObj>, const PointParam<X, Y, XE, YE, XEL, XEH, YEL, YEH, VC, VS>& p)
{
	std::string command;
	if (p.IsData())
	{
		std::string output_name = GetSanitizedOutputName();
		auto [x_x2, y_y2] = GetAxes2D(p);
		if (x_x2 == "x2") m_canvas->Command("set x2tics");
		if (y_y2 == "y2") m_canvas->Command("set y2tics");

		//変数名とカラムのセット。
		std::map<std::string, std::variant<int, std::string>> column;
		std::vector<std::string> labelcolumn;

		//rangesは各変数のうち空でないものがtupleとしてまとめられている。
		auto ranges =
			ArrangeColumnOption<0, 1>(column, labelcolumn, m_canvas,
									  std::array<std::string, 11>{ "x", "y", "xerrorbar", "xerrlow", "xerrhigh",
									  "yerrorbar", "yerrlow", "yerrhigh",
									  "variable_color", "variable_size" },
									  std::array<std::string_view, 11>{ x_x2, y_y2, "", "", "", "", "", "", "", "", "" },
									  p.x, p.y, p.xerrorbar, p.xerrlow, p.xerrhigh, p.yerrorbar, p.yerrlow, p.yerrhigh,
									  p.variable_color, p.variable_size);
		//dataでない場合、コンパイル時にrangesが空になってエラーになりうる。
		//ので、空tupleだったら何もしない。
		if constexpr (std::tuple_size_v<decltype(ranges)> != 0 && MakeDataObj)
			MakeDataObject(m_canvas, output_name, ranges);
		//if (!labelcolumn.empty()) column.emplace_back(std::move(labelcolumn));

		command = MakePlotCommandCommon(m_canvas->IsInMemoryDataTransferEnabled(), output_name, column, labelcolumn, p);
	}
	else if (p.IsFile())
	{
		//xとyが与えられている場合はファイルプロット。
		//x、yにはカラムの情報が入っている。
		std::map<std::string, std::variant<int, std::string>> column;
		std::vector<std::string> labelcolumn;
		AddColumn(p.x, "x", column);
		AddColumn(p.y, "y", column);
		AddColumn(p.xerrorbar, "xerrorbar", column);
		AddColumn(p.xerrlow, "xerrlow", column);
		AddColumn(p.xerrhigh, "xerrhigh", column);
		AddColumn(p.yerrorbar, "yerrorbar", column);
		AddColumn(p.yerrlow, "yerrlow", column);
		AddColumn(p.yerrhigh, "yerrhigh", column);
		AddColumn(p.variable_color, "variable_color", column);
		AddColumn(p.variable_size, "variable_size", column);
		command = MakePlotCommandCommon(m_canvas->IsInMemoryDataTransferEnabled(), p.input, column, labelcolumn, p);
	}
	else if (p.IsEquation())
	{
		std::map<std::string, std::variant<int, std::string>> column;
		std::vector<std::string> labelcolumn;
		command = MakePlotCommandCommon(m_canvas->IsInMemoryDataTransferEnabled(), p.input, column, labelcolumn, p);
	}
	m_commands.push_back(command);
	return std::move(*this);
}
template <class Data, class Weight, class ...Options>
PlotBuffer2D PlotBuffer2D::PlotHistogram(const HistogramParam<Data, Weight>& p, Options ...ops)
{
	constexpr bool HasWeight = !std::same_as<std::ranges::empty_view<double>, Weight>;

	BinError be = p.binerror;
	Style sty = Style::none;
	// ユーザーからの指定がある場合は何よりそれを優先する。
	if constexpr (KeywordExists(plot::style, ops...))
		sty = GetKeywordArg(plot::style, ops...);
	else
	{
		if (p.stack)
		{
			//stackオプションが指定されている場合は、fillstepsスタイルで出力する。
			//stackのときはerrorを使用できない。
			if (be != BinError::none) PrintWarning("BinError option is not supported when stack option is specified. BinError option will be ignored.");
			sty = Style::steps;
		}
		else
		{
			//binerrorオプションが指定されている場合は、通常のヒストグラムでも累積でもpointsでよい。
			//ただし、weightオプションが指定されているときはBinerror::poissonは使用不可であるので、histepsかlinespointsにする。
			if (be != BinError::none)
			{
				if constexpr (HasWeight)
				{
					if (be == BinError::poisson68 || be == BinError::poisson95)
					{
						PrintWarning("Poisson error is not supported when weight option is specified. BinError option will be ignored.");
						sty = (p.cumul || p.inv_cumul) ? Style::linespoints : Style::histeps;
					}
					else sty = (p.cumul || p.inv_cumul) ? Style::linespoints : Style::points;
				}
				else sty = (p.cumul || p.inv_cumul) ? Style::linespoints : Style::points;
			}
			else
			{
				sty = (p.cumul || p.inv_cumul) ? Style::linespoints : Style::histeps;
			}
		}
	}
	//histepsなら最後のビンに0を追加する必要はないらしい。
	std::vector<double> hist;
	//累積ヒストグラムの場合、ビンの中身ではなく両側での値を境界部分にプロットする必要があるため、ビン数+1のサイズにする。
	//またstepsを使うには最後のビンに0が入っていなければならないので、ビン数+1のサイズにする必要がある。
	size_t binsize = (p.cumul || p.inv_cumul || sty == Style::steps) ? (p.xnbin + 1) : p.xnbin;
	hist.resize(binsize, 0);
	int64_t firstbin = p.cumul ? 1 : 0;
	int64_t lastbin = p.cumul ? p.xnbin + 1 : p.xnbin;
	double wbin = (p.xmax - p.xmin) / p.xnbin;
	double xerrors = 0.;
	std::vector<double> yerrors;
	if (be == BinError::normal68 || be == BinError::normal95)
	{
		yerrors.resize(binsize, 0.);
		if (!(p.cumul || p.inv_cumul))
		{
			//累積相対度数の場合はyerrorlinesでプロットしたいので、xerrorsは0のままにしておく。
			xerrors = wbin / 2.;
		}
	}
	std::vector<double> yerrlow, yerrhigh;
	if (be == BinError::poisson68 || be == BinError::poisson95)
	{
		yerrlow.resize(binsize, 0.);
		yerrhigh.resize(binsize, 0.);
		if (!(p.cumul || p.inv_cumul))
		{
			//累積相対度数の場合はyerrorlinesでプロットしたいので、xerrorsは0のままにしておく。
			xerrors = wbin / 2.;
		}
	}
	//cumul==trueのときだけ（inv_cumulでは不要）最初のビンをずらす必要がある。
	auto ibin = [&p, wbin, firstbin](double v) { return (int64_t)((v - p.xmin) / wbin) + firstbin; };
	if constexpr (HasWeight)
	{
		auto weight_ = [](const auto& w)
		{
			if constexpr (arithmetic<std::remove_cvref_t<decltype(w)>>)
				return views::Repeat(w);
			else
				return w;
		} (p.weight);
		for (auto [v, w] : views::Zip(p.data, weight_))
		{
			int64_t i = ibin(v);
			if (i < firstbin || std::cmp_greater_equal(i, lastbin)) continue;
			hist[i] += w;
			if (be == BinError::normal68 || be == BinError::normal95) yerrors[i] += (w * w);
		}
		//累積ヒストグラムの場合、単にビンごとに振り分けるだけではなく、
		//その中身の累積を計算し割合にする必要がある。
		if (p.cumul)
		{
			//最初のビンをずらしたことで、cumulのときだけはhist[0]が必ず0になっている。
			for (size_t i = 1; i < binsize; ++i)
			{
				hist[i] += hist[i - 1];
				if (be == BinError::normal68 || be == BinError::normal95) yerrors[i] += yerrors[i - 1];
			}
			double total = hist.back();
			for (size_t i = 0; i < binsize; ++i)
			{
				hist[i] /= total;
				if (be == BinError::normal68 || be == BinError::normal95) yerrors[i] /= (total * total);
			}
		}
		else if (p.inv_cumul)
		{
			//inv_cumulのときは最後のビンが必ず0になっている。
			for (size_t i = binsize; i > 0; --i)
			{
				hist[i - 1] += hist[i];
				if (be == BinError::normal68 || be == BinError::normal95) yerrors[i - 1] += yerrors[i];
			}
			double total = hist[0];
			for (size_t i = 0; i < binsize; ++i)
			{
				hist[i] /= total;
				if (be == BinError::normal68 || be == BinError::normal95) yerrors[i] /= (total * total);
			}
		}

		if (be == BinError::normal68 || be == BinError::normal95)
		{
			for (size_t i = 0; i < binsize; ++i)
			{
				if (be == BinError::normal68) yerrors[i] = std::sqrt(yerrors[i]);
				else yerrors[i] = std::sqrt(yerrors[i]) * 1.96;
			}
		}
	}
	else
	{
		for (auto&& v : p.data)
		{
			int64_t i = ibin(v);
			if (i < firstbin || std::cmp_greater_equal(i, lastbin)) continue;
			++hist[i];
		}
		//累積ヒストグラムの場合、単にビンごとに振り分けるだけではなく、
		//その中身の累積を計算し割合にする必要がある。
		if (p.cumul)
		{
			for (size_t i = 1; i < binsize; ++i)
				hist[i] += hist[i - 1];
		}
		else if (p.inv_cumul)
		{
			for (size_t i = binsize - 1; i > 0; --i)
				hist[i - 1] += hist[i];
		}

		if (be == BinError::normal68 || be == BinError::normal95)
		{
			for (size_t i = 0; i < binsize; ++i)
			{
				if (be == BinError::normal68) yerrors[i] = std::sqrt(hist[i]);
				else yerrors[i] = std::sqrt(hist[i]) * 1.96;
			}
		}
		else if (be == BinError::poisson68 || be == BinError::poisson95)
		{
			for (size_t i = 0; i < binsize; ++i)
			{
				if (be == BinError::poisson68)
					std::tie(yerrlow[i], yerrhigh[i]) = GetPoissonCI68((uint64_t)hist[i]);
				else
					std::tie(yerrlow[i], yerrhigh[i]) = GetPoissonCI95((uint64_t)hist[i]);
			}
		}

		//最後に、cumul/inv_cumulの場合はビンごとの値を割合にする必要がある。
		if (p.cumul || p.inv_cumul)
		{
			double total = p.cumul ? hist.back() : hist[0];
			for (auto& v : hist) v /= total;
			if (be == BinError::normal68 || be == BinError::normal95)
			{
				for (auto& v : yerrors) v /= total;
			}
			else if (be == BinError::poisson68 || be == BinError::poisson95)
			{
				for (size_t i = 0; i < binsize; ++i)
				{
					yerrlow[i] /= total;
					yerrhigh[i] /= total;
				}
			}
		}
	}

	std::vector<double> x;// (p.xnbin);
	if (p.cumul || p.inv_cumul || sty == Style::steps)
	{
		x.resize(binsize);
		for (size_t i = 0; i < binsize; ++i) x[i] = p.xmin + wbin * i;
	}
	else
	{
		x.resize(binsize);
		for (size_t i = 0; i < binsize; ++i) x[i] = p.xmin + wbin * (i + 0.5);
	}

	if (p.stack)
	{
		//スタックヒストグラムの場合、データをまとめる必要がある。m_stacked_histogram_dataにデータを追加していき、Flushのときにまとめて出力する。
		auto p2 = MakeFilledCurveParam(plot::x = x, plot::y = hist, ops..., plot::style = sty, plot::fillsolid = 0.5);

		if (m_stacked_histogram_bins.empty())
		{
			m_min = p.xmin;
			m_max = p.xmax;
			m_nbin = p.xnbin;
			std::ranges::copy(x, std::back_inserter(m_stacked_histogram_bins));
		}
		std::vector<double> y(hist.size());
		std::ranges::copy(hist, y.begin());
		m_stacked_histogram_data.emplace_back(GetSanitizedOutputName(), std::move(y));

		return PlotFilledCurves(std::false_type{}, p2);
	}
	else
	{
		if (!yerrors.empty())
		{
			if (xerrors != 0.)
			{
				//auto p2 = MakePointParam(plot::x = x, plot::y = hist, ops..., plot::style = sty, plot::xerrorbar = xerrors, plot::yerrorbar = yerrors);
				return PlotPoints(x, hist, ops..., plot::style = sty, plot::xerrorbar = xerrors, plot::yerrorbar = yerrors);
			}
			else
			{
				//auto p2 = MakePointParam(plot::x = x, plot::y = hist, ops..., plot::style = sty, plot::yerrorbar = yerrors);
				return PlotPoints(x, hist, ops..., plot::style = sty, ops..., plot::style = sty, plot::yerrorbar = yerrors);
			}
		}
		else if (!yerrlow.empty() && !yerrhigh.empty())
		{
			if (xerrors != 0.)
			{
				//auto p2 = MakePointParam(plot::x = x, plot::y = hist, ops..., plot::style = sty, plot::xerrorbar = xerrors, plot::yerrlow = yerrlow, plot::yerrhigh = yerrhigh);
				return PlotPoints(x, hist, ops..., plot::style = sty, plot::xerrorbar = xerrors, plot::yerrlow = yerrlow, plot::yerrhigh = yerrhigh);
			}
			else
			{
				//auto p2 = MakePointParam(plot::x = x, plot::y = hist, ops..., plot::style = sty, plot::yerrlow = yerrlow, plot::yerrhigh = yerrhigh);
				return PlotPoints(x, hist, ops..., plot::style = sty, plot::yerrlow = yerrlow, plot::yerrhigh = yerrhigh);
			}
		}
		else
		{
			//auto p2 = MakePointParam(plot::x = x, plot::y = hist, ops..., plot::style = sty);
			return PlotPoints(x, hist, plot::y = hist, ops..., plot::style = sty);
		}
	}
}
/*template <class X, class Y, class XE, class YE, class XEL, class XEH, class YEL, class YEH, class VC, class VS>
PlotBuffer2D PlotBuffer2D::PlotHistogram_impl(bool stack, double min, double max, size_t nbin, const PointParam<X, Y, XE, YE, XEL, XEH, YEL, YEH, VC, VS>& p)
{
	std::string output_name = GetSanitizedOutputName();
	auto [x_x2, y_y2] = GetAxes2D(p);
	if (x_x2 == "x2") m_canvas->Command("set x2tics");
	if (y_y2 == "y2") m_canvas->Command("set y2tics");

	//変数名とカラムのセット。
	std::map<std::string, std::variant<int, std::string>> column;
	std::vector<std::string> labelcolumn;

	//rangesは各変数のうち空でないものがtupleとしてまとめられている。
	auto ranges =
		ArrangeColumnOption<0, 1>(column, labelcolumn, m_canvas,
								  std::array<std::string, 11>{ "x", "y", "xerrorbar", "xerrlow", "xerrhigh",
								  "yerrorbar", "yerrlow", "yerrhigh",
								  "variable_color", "variable_size" },
								  std::array<std::string_view, 11>{ x_x2, y_y2, "", "", "", "", "", "", "", "" },
								  p.x, p.y, p.xerrorbar, p.xerrlow, p.xerrhigh, p.yerrorbar, p.yerrlow, p.yerrhigh,
								  p.variable_color, p.variable_size);

	MakeDataObject(m_canvas, output_name, ranges);

	std::string command = MakePlotCommandCommon(m_canvas->IsInMemoryDataTransferEnabled(), output_name, column, labelcolumn, p);

	m_commands.push_back(command);
	return std::move(*this);
}
template <class X, class Y, class Y2, class VC>
PlotBuffer2D PlotBuffer2D::PlotHistogram_impl(double min, double max, size_t nbin, const FilledCurveParam<X, Y, Y2, VC>& p)
{
	std::string output_name = GetSanitizedOutputName();
	auto [x_x2, y_y2] = GetAxes2D(p);
	if (x_x2 == "x2") m_canvas->Command("set x2tics");
	if (y_y2 == "y2") m_canvas->Command("set y2tics");

	//変数名とカラムのセット。
	std::map<std::string, std::variant<int, std::string>> column;
	std::vector<std::string> labelcolumn;

	if (m_stacked_histogram_bins.empty())
	{
		m_min = min;
		m_max = max;
		m_nbin = nbin;
		std::ranges::copy(p.x, std::back_inserter(m_stacked_histogram_bins));
	}
	std::vector<double> y(p.y.size());
	std::ranges::copy(p.y, y.begin());
	m_stacked_histogram_data.emplace_back(output_name, std::move(y));
	column[x_x2] = 1;
	column[y_y2] = 2;

	std::string command = MakePlotCommandCommon(m_canvas->IsInMemoryDataTransferEnabled(), output_name, column, labelcolumn, p);
	m_commands.push_back(command);
	return std::move(*this);
}*/

template <class X, class Y, class Weight, class ...Options>
PlotBuffer2D PlotBuffer2D::PlotBinscatter(const BinscatterParam<X, Y, Weight>& p, Options ...ops)
{
	constexpr bool HasWeight = !std::same_as<std::ranges::empty_view<double>, Weight>;
	Matrix<double, 2> hist(p.xnbin, p.ynbin);
	std::vector<double> vx; vx.reserve(p.x.size());
	std::vector<double> vy; vy.reserve(p.y.size());
	double wxbin = (p.xmax - p.xmin) / p.xnbin;
	double wybin = (p.ymax - p.ymin) / p.ynbin;
	auto ibin = [&p, wxbin, wybin](double x, double y)
	{
		return std::make_pair((int64_t)((x - p.xmin) / wxbin), (int64_t)((y - p.ymin) / wybin));
	};
	auto w_ = []([[maybe_unused]] auto& p) { if constexpr (HasWeight) return p.weight; else return views::Repeat(0.); } (p);
	for ([[maybe_unused]] auto&& [x, y, w] : views::Zip(p.x, p.y, w_))
	{
		auto [ix, iy] = ibin(x, y);
		if (ix < 0 || std::cmp_greater_equal(ix, p.xnbin) || iy < 0 || std::cmp_greater_equal(iy, p.ynbin)) continue;
		if constexpr (HasWeight) hist[(uint32_t)ix][(uint32_t)iy] += w;
		else ++hist[(uint32_t)ix][(uint32_t)iy];
		if (p.bs_points)
		{
			vx.push_back(x);
			vy.push_back(y);
		}
	}
	if (p.bs_points)
	{
		std::vector<double> dens(vx.size(), 0);
		for (auto&& [x, y, d] : views::Zip(vx, vy, dens))
		{
			auto [ix, iy] = ibin(x, y);
			assert(ix >= 0 && std::cmp_less(ix, p.xnbin) && iy >= 0 && std::cmp_less(iy, p.ynbin));
			d = hist[(uint32_t)ix][(uint32_t)iy];
		}
		auto p2 = MakePointParam(plot::x = vx, plot::y = vy, plot::variable_color = dens, ops..., plot::pt_fcir, plot::ps_ex_small);
		return PlotPoints(p2);
	}
	else
	{
		std::pair<double, double> xminmax = { p.xmin + wxbin / 2, p.xmax - wxbin / 2 };
		std::pair<double, double> yminmax = { p.ymin + wybin / 2, p.ymax - wybin / 2 };
		if (p.bs_lower != std::numeric_limits<double>::lowest() ||
			p.bs_upper != std::numeric_limits<double>::max())
		{
			for (uint32_t i = 0; i < p.xnbin; ++i)
			{
				for (uint32_t j = 0; j < p.ynbin; ++j)
				{
					if (hist[i][j] <= p.bs_lower) hist[i][j] = std::numeric_limits<double>::quiet_NaN();
					else if (hist[i][j] > p.bs_upper) hist[i][j] = std::numeric_limits<double>::quiet_NaN();
				}
			}
		}
		auto p2 = MakeColormapParam(plot::map = hist, plot::xminmax = xminmax, plot::yminmax = yminmax, ops...);
		return PlotColormap(p2);
	}
}
template <class X, class Y, class XL, class YL, class VC>
PlotBuffer2D PlotBuffer2D::PlotVectors(const VectorParam<X, Y, XL, YL, VC>& p)
{
	std::string command;
	if (p.IsData())
	{
		std::string output_name = GetSanitizedOutputName();
		auto [x_x2, y_y2] = GetAxes2D(p);
		if (x_x2 == "x2") m_canvas->Command("set x2tics");
		if (y_y2 == "y2") m_canvas->Command("set y2tics");

		std::map<std::string, std::variant<int, std::string>> column;
		std::vector<std::string> labelcolumn;

		auto ranges =
			ArrangeColumnOption<0, 1>(column, labelcolumn, m_canvas,
									  std::array<std::string, 5>{ "x", "y", "xlen", "ylen", "variable_color" },
									  std::array<std::string_view, 5>{ x_x2, y_y2, x_x2, y_y2, "" },
									  p.x, p.y, p.xlen, p.ylen, p.variable_color);
		if constexpr (std::tuple_size_v<decltype(ranges)> != 0)
			MakeDataObject(m_canvas, output_name, ranges);
		//if (!labelcolumn.empty()) column.emplace_back(std::move(labelcolumn));

		command = MakePlotCommandCommon(m_canvas->IsInMemoryDataTransferEnabled(), output_name, column, labelcolumn, p);
	}
	else if (p.IsFile())
	{
		std::map<std::string, std::variant<int, std::string>> column;
		std::vector<std::string> labelcolumn;
		AddColumn(p.x, "x", column);
		AddColumn(p.y, "y", column);
		AddColumn(p.xlen, "xlen", column);
		AddColumn(p.ylen, "ylen", column);
		AddColumn(p.variable_color, "variable_color", column);
		command = MakePlotCommandCommon(m_canvas->IsInMemoryDataTransferEnabled(), p.input, column, labelcolumn, p);
	}
	else if (p.IsEquation())
	{
		throw InvalidArg("Equation plot is not supported for vector plot.");
	}
	m_commands.push_back(command);
	return std::move(*this);
}
template <class X, class Y, class Y2, class VC>
PlotBuffer2D PlotBuffer2D::PlotFilledCurves(const FilledCurveParam<X, Y, Y2, VC>& p)
{
	return PlotFilledCurves(std::true_type{}, p);
}
template <bool MakeDataObj, class X, class Y, class Y2, class VC>
PlotBuffer2D PlotBuffer2D::PlotFilledCurves(std::bool_constant<MakeDataObj>, const FilledCurveParam<X, Y, Y2, VC>& p)
{
	std::string command;
	std::string output_name;
	std::map<std::string, std::variant<int, std::string>> column;
	std::vector<std::string> labelcolumn;

	auto set_point_params = [&p](auto& pp)
	{
		if (p.style == Style::boxes) pp.style = Style::boxes;
		else if (p.style == Style::steps) pp.style = Style::steps;
		if (!p.bordercolor.empty()) pp.color = p.bordercolor;
		else if (!p.color.empty()) pp.color = p.color;
		if (p.bordertype != -2) pp.linetype = p.bordertype;
	};
	if (p.IsData())
	{
		output_name = GetSanitizedOutputName();
		auto [x_x2, y_y2] = GetAxes2D(p);
		if (x_x2 == "x2") m_canvas->Command("set x2tics");
		if (y_y2 == "y2") m_canvas->Command("set y2tics");
		auto ranges =
			ArrangeColumnOption<0, 1>(column, labelcolumn, m_canvas,
									  std::array<std::string, 4>{ "x", "y", "ybelow", "variable_color" },
									  std::array<std::string_view, 4>{ x_x2, y_y2, y_y2, "" },
									  p.x, p.y, p.ybelow, p.variable_color);
		if constexpr (std::tuple_size_v<decltype(ranges)> != 0 && MakeDataObj)
			MakeDataObject(m_canvas, output_name, ranges);
		//if (!labelcolumn.empty()) column.emplace_back(std::move(labelcolumn));

		command = MakePlotCommandCommon(m_canvas->IsInMemoryDataTransferEnabled(), output_name, column, labelcolumn, p);
		if (!p.noborder)
		{
			auto pp = MakePointParam(plot::x = p.x, plot::y = p.y, plot::variable_color = p.variable_color, plot::s_lines, plot::notitle, plot::axis = p.axis, plot::smooth = p.smooth);
			set_point_params(pp);
			command += ", " + MakePlotCommandCommon(m_canvas->IsInMemoryDataTransferEnabled(), output_name, column, labelcolumn, pp);
			if constexpr (FilledCurveParam<X, Y, Y2, VC>::HasYBelow())
			{
				auto pp = MakePointParam(plot::x = p.x, plot::y = p.ybelow, plot::variable_color = p.variable_color, plot::s_lines, plot::notitle, plot::axis = p.axis, plot::smooth = p.smooth);
				set_point_params(pp);
				command += ", " + MakePlotCommandCommon(m_canvas->IsInMemoryDataTransferEnabled(), output_name, column, labelcolumn, pp);
			}
		}
	}
	else if (p.IsFile())
	{
		output_name = p.input;
		AddColumn(p.x, "x", column);
		AddColumn(p.y, "y", column);
		AddColumn(p.ybelow, "y2", column);
		AddColumn(p.variable_color, "variable_color", column);
		command = MakePlotCommandCommon(m_canvas->IsInMemoryDataTransferEnabled(), p.input, column, labelcolumn, p);
		if (!p.noborder)
		{
			auto pp = MakePointParam(plot::input = p.input, plot::x = p.x, plot::y = p.y, plot::variable_color = p.variable_color, plot::s_lines, plot::notitle, plot::axis = p.axis, plot::smooth = p.smooth);
			set_point_params(pp);
			command += ", " + MakePlotCommandCommon(m_canvas->IsInMemoryDataTransferEnabled(), output_name, column, labelcolumn, pp);
			if constexpr (FilledCurveParam<X, Y, Y2, VC>::HasYBelow())
			{
				auto pp = MakePointParam(plot::input = p.input, plot::x = p.x, plot::y = p.ybelow, plot::variable_color = p.variable_color, plot::s_lines, plot::notitle, plot::axis = p.axis, plot::smooth = p.smooth);
				set_point_params(pp);
				command += ", " + MakePlotCommandCommon(m_canvas->IsInMemoryDataTransferEnabled(), output_name, column, labelcolumn, pp);
			}
		}
	}
	else if (p.IsEquation())
	{
		output_name = p.input;
		command = MakePlotCommandCommon(m_canvas->IsInMemoryDataTransferEnabled(), p.input, column, labelcolumn, p);
		if (!p.noborder)
		{
			auto pp = MakePointParam(plot::input = p.input, plot::variable_color = p.variable_color, plot::s_lines, plot::notitle, plot::axis = p.axis, plot::smooth = p.smooth);
			set_point_params(pp);
			command += ", " + MakePlotCommandCommon(m_canvas->IsInMemoryDataTransferEnabled(), output_name, column, labelcolumn, pp);
			if constexpr (FilledCurveParam<X, Y, Y2, VC>::HasYBelow())
			{
				auto pp = MakePointParam(plot::input = p.input, plot::variable_color = p.variable_color, plot::s_lines, plot::notitle, plot::axis = p.axis, plot::smooth = p.smooth);
				set_point_params(pp);
				command += ", " + MakePlotCommandCommon(m_canvas->IsInMemoryDataTransferEnabled(), output_name, column, labelcolumn, pp);
			}
		}
	}
	m_commands.push_back(command);
	return std::move(*this);
}
template <class X, class Y, class L, class VTC>
PlotBuffer2D PlotBuffer2D::PlotLabels(const LabelParam<X, Y, L, VTC>& p)
{
	std::string command;
	if (p.IsData())
	{
		std::string output_name = GetSanitizedOutputName();
		auto [x_x2, y_y2] = GetAxes2D(p);
		if (x_x2 == "x2") m_canvas->Command("set x2tics");
		if (y_y2 == "y2") m_canvas->Command("set y2tics");

		//変数名とカラムのセット。
		std::map<std::string, std::variant<int, std::string>> column;
		std::vector<std::string> labelcolumn;

		//rangesは各変数のうち空でないものがtupleとしてまとめられている。
		auto ranges =
			ArrangeColumnOption<0, 1>(column, labelcolumn, m_canvas,
									  std::array<std::string, 4>{ "x", "y", "label", "variable_color" },
									  std::array<std::string_view, 4>{ x_x2, y_y2, "", "" },
									  p.x, p.y, p.label, p.variable_color);
		//dataでない場合、コンパイル時にrangesが空になってエラーになりうる。
		//ので、空tupleだったら何もしない。
		if constexpr (std::tuple_size_v<decltype(ranges)> != 0)
			MakeDataObject(m_canvas, output_name, ranges);

		command = MakePlotCommandCommon(m_canvas->IsInMemoryDataTransferEnabled(), output_name, column, labelcolumn, p);
	}
	else if (p.IsFile())
	{
		//xとyが与えられている場合はファイルプロット。
		//x、yにはカラムの情報が入っている。
		std::map<std::string, std::variant<int, std::string>> column;
		std::vector<std::string> labelcolumn;
		AddColumn(p.x, "x", column);
		AddColumn(p.y, "y", column);
		AddColumn(p.label, "label", column);
		AddColumn(p.variable_color, "variable_color", column);
		command = MakePlotCommandCommon(m_canvas->IsInMemoryDataTransferEnabled(), p.input, column, labelcolumn, p);
	}
	else if (p.IsEquation())
	{
		std::map<std::string, std::variant<int, std::string>> column;
		std::vector<std::string> labelcolumn;
		command = MakePlotCommandCommon(m_canvas->IsInMemoryDataTransferEnabled(), p.input, column, labelcolumn, p);
	}
	m_commands.push_back(command);
	return std::move(*this);
}
template <class Map, class X, class Y>
PlotBuffer2D PlotBuffer2D::PlotColormap(const ColormapParam<Map, X, Y>& p)
{
	constexpr bool xrange_assigned = !PlotParamBase::IsEmptyView<X>();
	constexpr bool yrange_assigned = !PlotParamBase::IsEmptyView<Y>();

	std::string command;
	if (p.IsData())
	{
		//p.mapがデータでない場合にコンパイルエラーになるのを防ぐため、constexpr ifで括っておく。
		if constexpr (ranges::arithmetic_matrix_range<Map>)
		{
			//データプロットの場合。
			std::string output_name = GetSanitizedOutputName();
			if (!p.IsXAssigned()) throw InvalidArg("xrange or xminmax must be specified.");
			if (!p.IsYAssigned()) throw InvalidArg("yrange or yminmax must be specified.");
			//std::vector<std::string> column{ "1", "2", "5" };
			std::map<std::string, std::variant<int, std::string>> column{ { "x", 3 }, { "y", 4 }, { "map", 5} };
			std::vector<std::string> labelcolumn;
			size_t xsize = p.map.size();
			size_t ysize = p.map.front().size();
			if constexpr (xrange_assigned)
			{
				if constexpr (yrange_assigned)
					MakeDataObject(m_canvas, output_name, p.map, CoordRange<X>(p.xrange), CoordRange<Y>(p.yrange));
				else
					MakeDataObject(m_canvas, output_name, p.map, CoordRange<X>(p.xrange), CoordMinMax(p.yminmax, ysize));
			}
			else
			{
				if constexpr (yrange_assigned)
					MakeDataObject(m_canvas, output_name, p.map, CoordMinMax(p.xminmax, xsize), CoordRange<Y>(p.yrange));
				else
					MakeDataObject(m_canvas, output_name, p.map, CoordMinMax(p.xminmax, xsize), CoordMinMax(p.yminmax, ysize));
			}
			if (p.with_contour)
			{
				m_canvas->Command(MakeContourPlotCommand(output_name, m_canvas->IsInMemoryDataTransferEnabled(), p));
			}
			command = MakePlotCommandCommon(m_canvas->IsInMemoryDataTransferEnabled(), output_name, column, labelcolumn, p);
		}
	}
	else if (p.IsFile())
	{
		//ファイルプロットの場合、p.map、p.xrange、p.yrangeにカラムの情報が入っている。
		std::map<std::string, std::variant<int, std::string>> column;
		std::vector<std::string> labelcolumn;
		AddColumn(p.xrange, "x", column);
		AddColumn(p.yrange, "y", column);
		AddColumn(p.map, "map", column);
		command = MakePlotCommandCommon(m_canvas->IsInMemoryDataTransferEnabled(), p.input, column, labelcolumn, p);
		if (p.with_contour)
		{
			PrintWarning("Contour plot is not supported for file plot.");
		}
	}
	else if (p.IsEquation())
	{
		std::map<std::string, std::variant<int, std::string>> column;
		std::vector<std::string> labelcolumn;
		command = MakePlotCommandCommon(m_canvas->IsInMemoryDataTransferEnabled(), p.input, column, labelcolumn, p);
	}
	m_commands.push_back(command);
	return std::move(*this);
}

}

namespace plot_detail
{

struct PlotBuffer3D
{
	PlotBuffer3D(Canvas3D* g) : m_canvas(g) {}
	PlotBuffer3D(const PlotBuffer3D&) = delete;
	PlotBuffer3D(PlotBuffer3D&& p) noexcept
		: m_commands(std::move(p.m_commands)), m_canvas(p.m_canvas)
	{
		p.m_canvas = nullptr;
	}
	PlotBuffer3D& operator=(const PlotBuffer3D&) = delete;
	PlotBuffer3D& operator=(PlotBuffer3D&& p) noexcept
	{
		m_canvas = p.m_canvas; p.m_canvas = nullptr;
		m_commands = std::move(p.m_commands);
		return *this;
	}
	virtual ~PlotBuffer3D()
	{
		//mPipeがnullptrでないときはこのPlotterが最終処理を担当する。
		if (m_canvas != nullptr) Flush();
	}

	void Flush();

	template <acceptable_arg_except_string X, acceptable_arg_except_string Y, acceptable_arg_except_string Z,
			  point_option ...Options>
	PlotBuffer3D PlotPoints(const X& x, const Y& y, const Z& z, Options ...ops)
	{
		auto p = MakePoint3DParam(plot::x = x, plot::y = y, plot::z = z, ops...);
		return Plot(p);
	}
	template <point_option ...Options>
	PlotBuffer3D PlotPoints(std::string_view filename,
							std::string_view x, std::string_view y, std::string_view z,
							Options ...ops)
	{
		auto p = MakePoint3DParam(plot::input = filename, plot::x = x, plot::y = y, plot::z = z, ops...);
		return Plot(p);
	}
	template <acceptable_arg_except_string X, acceptable_arg_except_string Y,
			  point_option ...Options>
	PlotBuffer3D PlotPoints(const X& x, const Y& y, Options ...ops)
	{
		auto p = MakePoint3DParam(plot::x = x, plot::y = y, plot::z = 0, ops...);
		return Plot(p);
	}
	template <point_option ...Options>
	PlotBuffer3D PlotPoints(std::string_view filename,
							std::string_view x, std::string_view y,
							Options ...ops)
	{
		auto p = MakePoint3DParam(plot::input = filename, plot::x = x, plot::y = y, plot::z = "($1-$1)", ops...);
		return Plot(p);
	}
	template <point_option ...Options>
	PlotBuffer3D PlotPoints(std::string_view equation, Options ...ops)
	{
		auto p = MakePoint3DParam(plot::input = equation, plot::z = "($1-$1)", ops...);
		return Plot(p);
	}

	template <acceptable_arg_except_string X, acceptable_arg_except_string Y, acceptable_arg_except_string Z,
			  point_option ...Options>
	PlotBuffer3D PlotLines(const X& x, const Y& y, const Z& z, Options ...ops)
	{
		return PlotPoints(x, y, z, plot::style = Style::lines, ops...);
	}
	template <point_option ...Options>
	PlotBuffer3D PlotLines(std::string_view filename,
						   std::string_view x, std::string_view y, std::string_view z,
						   Options ...ops)
	{
		return PlotPoints(filename, x, y, z, plot::style = Style::lines, ops...);
	}
	template <acceptable_arg_except_string X, acceptable_arg_except_string Y,
			  point_option ...Options>
	PlotBuffer3D PlotLines(const X& x, const Y& y, Options ...ops)
	{
		return PlotPoints(x, y, plot::style = Style::lines, ops...);
	}
	template <point_option ...Options>
	PlotBuffer3D PlotLines(std::string_view filename,
						   std::string_view x, std::string_view y,
						   Options ...ops)
	{
		return PlotPoints(filename, x, y, plot::style = Style::lines, ops...);
	}
	template <point_option ...Options>
	PlotBuffer3D PlotLines(std::string_view equation, Options ...ops)
	{
		return PlotPoints(equation, plot::style = Style::lines, ops...);
	}

	template <acceptable_arg_except_string X, acceptable_arg_except_string Y, acceptable_arg_except_string Z,
			  acceptable_arg_except_string XL, acceptable_arg_except_string YL, acceptable_arg_except_string ZL,
			  vector_option ...Options>
	PlotBuffer3D PlotVectors(const X& xfrom, const Y& yfrom, const Z& zfrom,
							 const XL& xlen, const YL& ylen, const ZL& zlen,
							 Options ...ops)
	{
		auto p = MakeVectorParam3D(plot::x = xfrom, plot::y = yfrom, plot::z = zfrom,
								   plot::xlen = xlen, plot::ylen = ylen, plot::zlen = zlen, ops...);
		return Plot(p);
	}
	template <vector_option ...Options>
	PlotBuffer3D PlotVectors(std::string_view filename,
							 std::string_view xfrom, std::string_view yfrom, std::string_view zfrom,
							 std::string_view xlen, std::string_view ylen, std::string_view zlen,
							 Options ...ops)
	{
		auto p = MakeVectorParam3D(plot::input = filename,
								   plot::x = xfrom, plot::y = yfrom, plot::z = zfrom,
								   plot::xlen = xlen, plot::ylen = ylen, plot::zlen = zlen, ops...);
		return Plot(p);
	}
	template <acceptable_arg_except_string X, acceptable_arg_except_string Y,
			  acceptable_arg_except_string XL, acceptable_arg_except_string YL,
			  vector_option ...Options>
	PlotBuffer3D PlotVectors(const X& xfrom, const Y& yfrom,
							 const XL& xlen, const YL& ylen,
							 Options ...ops)
	{
		auto p = MakeVectorParam3D(plot::x = xfrom, plot::y = yfrom, plot::z = 0.,
								   plot::xlen = xlen, plot::ylen = ylen, plot::zlen = 0., ops...);
		return Plot(p);
	}
	template <vector_option ...Options>
	PlotBuffer3D PlotVectors(std::string_view filename,
							 std::string_view xfrom, std::string_view yfrom,
							 std::string_view xlen, std::string_view ylen,
							 Options ...ops)
	{
		auto p = MakeVectorParam3D(plot::input = filename,
								   plot::x = xfrom, plot::y = yfrom, plot::z = "($0-$0)",
								   plot::xlen = xlen, plot::ylen = ylen, plot::zlen = "($0-$0)", ops...);
		return Plot(p);
	}

	template <acceptable_arg_except_string X, acceptable_arg_except_string Y, acceptable_arg_except_string L,
		label_option ...Options>
	PlotBuffer3D PlotLabels(const X& x, const Y& y, const L& label, Options ...ops)
	{
		auto p = MakeLabelParam3D(plot::x = x, plot::y = y, plot::z = 0., plot::label = label, ops...);
		return Plot(p);
	}
	template <label_option ...Options>
	PlotBuffer3D PlotLabels(std::string_view filename, std::string_view xcol, std::string_view ycol, std::string_view labelcol, Options ...ops)
	{
		auto p = MakeLabelParam3D(plot::input = filename, plot::x = xcol, plot::y = ycol, plot::z = "($0-$0)",
								  plot::label = labelcol, ops...);
		return Plot(p);
	}
	template <acceptable_arg_except_string X, acceptable_arg_except_string Y, acceptable_arg_except_string Z, acceptable_arg_except_string L,
		label_option ...Options>
	PlotBuffer3D PlotLabels(const X& x, const Y& y, const Z& z, const L& label, Options ...ops)
	{
		auto p = MakeLabelParam3D(plot::x = x, plot::y = y, plot::z = z, plot::label = label, ops...);
		return Plot(p);
	}
	template <label_option ...Options>
	PlotBuffer3D PlotLabels(std::string_view filename,
							std::string_view xcol, std::string_view ycol, std::string_view zcol,
							std::string_view labelcol, Options ...ops)
	{
		auto p = MakeLabelParam3D(plot::input = filename, plot::x = xcol, plot::y = ycol, plot::z = zcol,
								  plot::label = labelcol, ops...);
		return Plot(p);
	}

	template <acceptable_matrix_range Map,
			  ranges::arithmetic_range XRange, ranges::arithmetic_range YRange,
			  colormap_option ...Options>
	PlotBuffer3D PlotColormap(const Map& map_, const XRange& x, const YRange& y,
							  Options ...ops)
	{
		auto p = MakeColormapParam(plot::map = map_, plot::xrange = x, plot::yrange = y, ops...);
		return Plot(p);
	}
	template <acceptable_matrix_range Map, colormap_option ...Options>
	PlotBuffer3D PlotColormap(const Map& map_, std::pair<double, double> x, std::pair<double, double> y,
							  Options ...ops)
	{
		auto p = MakeColormapParam(plot::map = map_, plot::xminmax = x, plot::yminmax = y, ops...);
		return Plot(p);
	}
	template <colormap_option ...Options>
	PlotBuffer3D PlotColormap(std::string_view filename, std::string_view x, std::string_view y, std::string_view map,
							  Options ...ops)
	{
		auto p = MakeColormapParam(plot::input = filename, plot::map = map, plot::xrange = x, plot::yrange = y, ops...);
		return Plot(p);
	}
	template <colormap_option ...Options>
	PlotBuffer3D PlotColormap(std::string_view equation, Options ...ops)
	{
		auto p = MakeColormapParam(plot::input = equation, ops...);
		return Plot(p);
	}

	template <acceptable_matrix_range Map, ranges::arithmetic_range X, ranges::arithmetic_range Y,
		surface_option ...Options>
	PlotBuffer3D PlotSurface(const Map& z, const X& x, const Y& y, Options ...ops)
	{
		auto p = MakeSurfaceParam(plot::map = z, plot::x = x, plot::y = y, ops...);
		return Plot(p);
	}
	template <acceptable_matrix_range Map, surface_option ...Options>
	PlotBuffer3D PlotSurface(const Map& z, std::pair<double, double> x, std::pair<double, double> y, Options ...ops)
	{
		auto p = MakeSurfaceParam(plot::map = z, plot::xminmax = x, plot::yminmax = y, ops...);
		return Plot(p);
	}
	template <surface_option ...Options>
	PlotBuffer3D PlotSurface(std::string_view filename, std::string_view x, std::string_view y, std::string_view z, Options ...ops)
	{
		auto p = MakeSurfaceParam(plot::input = filename, plot::map = z, plot::xrange = x, plot::yrange = y, ops...);
		return Plot(p);
	}
	template <surface_option ...Options>
	PlotBuffer3D PlotSurface(std::string_view equation, Options ...ops)
	{
		auto p = MakeSurfaceParam(plot::input = equation, ops...);
		return Plot(p);
	}

protected:

	std::string GetSanitizedOutputName() const;

	template <class X, class Y, class Z, class XE, class XEL, class XEH, class YE, class YEL, class YEH, class VC, class VS>
	PlotBuffer3D Plot(const PointParam3D<X, Y, Z, XE, XEL, XEH, YE, YEL, YEH, VC, VS>& p);
	template <class X, class Y, class Z, class XL, class YL, class ZL, class VC>
	PlotBuffer3D Plot(const VectorParam3D<X, Y, Z, XL, YL, ZL, VC>& p);
	//CMはFilledCurveをサポートしない。
	template <class X, class Y, class Z, class L, class VTC>
	PlotBuffer3D Plot(const LabelParam3D<X, Y, Z, L, VTC>& p);
	template <class Map, class X, class Y>
	PlotBuffer3D Plot(const ColormapParam<Map, X, Y>& p);
	template <class Map, class X, class Y, class VC, class VS>
	PlotBuffer3D Plot(const SurfaceParam<Map, X, Y, VC, VS>& p);

	static std::string InitCommand()
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

	std::vector<std::string> m_commands;
	Canvas3D* m_canvas;
};


class Canvas3D : public Canvas<AxisX<Canvas3D>, AxisY<Canvas3D>, AxisZ<Canvas3D>, AxisX2<Canvas3D>, AxisY2<Canvas3D>, AxisCB<Canvas3D>>
{
public:

	using Base = Canvas<AxisX<Canvas3D>, AxisY<Canvas3D>, AxisZ<Canvas3D>, AxisX2<Canvas3D>, AxisY2<Canvas3D>, AxisCB<Canvas3D>>;
	Canvas3D(const std::string& output, double sizex = 0., double sizey = 0.)
		: Base(output, sizex, sizey)
	{}
	Canvas3D()
	{}

	friend class MultiPlotter;

	void SetViewMap() { this->Command("set view map"); }
	void SetXYPlaneRelative(double frac) { this->Command("set xyplane relative ", frac); }
	void SetXYPlaneAt(double z) { this->Command("set xyplane at ", z); }

	template <acceptable_arg_except_string X, acceptable_arg_except_string Y, acceptable_arg_except_string Z,
			  point_option ...Options>
	PlotBuffer3D PlotPoints(const X& x, const Y& y, const Z& z, Options ...ops)
	{
		PlotBuffer3D p(this);
		return p.PlotPoints(x, y, z, ops...);
	}
	template <point_option ...Options>
	PlotBuffer3D PlotPoints(std::string_view filename,
							std::string_view x, std::string_view y, std::string_view z,
							Options ...ops)
	{
		PlotBuffer3D p(this);
		return p.PlotPoints(filename, x, y, z, ops...);
	}
	template <acceptable_arg_except_string X, acceptable_arg_except_string Y,
			  point_option ...Options>
	PlotBuffer3D PlotPoints(const X& x, const Y& y, Options ...ops)
	{
		PlotBuffer3D p(this);
		return p.PlotPoints(x, y, ops...);
	}
	template <point_option ...Options>
	PlotBuffer3D PlotPoints(std::string_view filename,
							std::string_view x, std::string_view y,
							Options ...ops)
	{
		PlotBuffer3D p(this);
		return p.PlotPoints(filename, x, y, ops...);
	}
	template <point_option ...Options>
	PlotBuffer3D PlotPoints(std::string_view equation, Options ...ops)
	{
		PlotBuffer3D p(this);
		return p.PlotPoints(equation, ops...);
	}

	template <acceptable_arg_except_string X, acceptable_arg_except_string Y, acceptable_arg_except_string Z,
			  point_option ...Options>
	PlotBuffer3D PlotLines(const X& x, const Y& y, const Z& z, Options ...ops)
	{
		PlotBuffer3D p(this);
		return p.PlotLines(x, y, z, ops...);
	}
	template <point_option ...Options>
	PlotBuffer3D PlotLines(std::string_view filename,
						   std::string_view x, std::string_view y, std::string_view z,
						   Options ...ops)
	{
		PlotBuffer3D p(this);
		return p.PlotLines(filename, x, y, z, ops...);
	}
	template <acceptable_arg_except_string X, acceptable_arg_except_string Y,
			  point_option ...Options>
	PlotBuffer3D PlotLines(const X& x, const Y& y, Options ...ops)
	{
		PlotBuffer3D p(this);
		return p.PlotLines(x, y, ops...);
	}
	template <point_option ...Options>
	PlotBuffer3D PlotLines(std::string_view filename,
						   std::string_view x, std::string_view y,
						   Options ...ops)
	{
		PlotBuffer3D p(this);
		return p.PlotLines(filename, x, y, ops...);
	}
	template <point_option ...Options>
	PlotBuffer3D PlotLines(std::string_view equation, Options ...ops)
	{
		PlotBuffer3D p(this);
		return p.PlotLines(equation, ops...);
	}

	template <acceptable_arg_except_string X, acceptable_arg_except_string Y, acceptable_arg_except_string Z,
			  acceptable_arg_except_string XL, acceptable_arg_except_string YL, acceptable_arg_except_string ZL,
			  vector_option ...Options>
	PlotBuffer3D PlotVectors(const X& xfrom, const Y& yfrom, const Z& zfrom,
							 const XL& xlen, const YL& ylen, const ZL& zlen,
							 Options ...ops)
	{
		PlotBuffer3D p(this);
		return p.PlotVectors(xfrom, yfrom, zfrom, xlen, ylen, zlen, ops...);
	}
	template <vector_option ...Options>
	PlotBuffer3D PlotVectors(std::string_view filename,
							 std::string_view xfrom, std::string_view yfrom, std::string_view zfrom,
							 std::string_view xlen, std::string_view ylen, std::string_view zlen,
							 Options ...ops)
	{
		PlotBuffer3D p(this);
		return p.PlotVectors(filename, xfrom, yfrom, zfrom, xlen, ylen, zlen, ops...);
	}
	template <acceptable_arg_except_string X, acceptable_arg_except_string Y,
			  acceptable_arg_except_string XL, acceptable_arg_except_string YL,
			  vector_option ...Options>
	PlotBuffer3D PlotVectors(const X& xfrom, const Y& yfrom,
							 const XL& xlen, const YL& ylen,
							 Options ...ops)
	{
		PlotBuffer3D p(this);
		return p.PlotVectors(xfrom, yfrom, xlen, ylen, ops...);
	}
	template <vector_option ...Options>
	PlotBuffer3D PlotVectors(std::string_view filename,
							 std::string_view xfrom, std::string_view yfrom,
							 std::string_view xlen, std::string_view ylen,
							 Options ...ops)
	{
		PlotBuffer3D p(this);
		return p.PlotVectors(filename, xfrom, yfrom, xlen, ylen, ops...);
	}

	template <acceptable_arg_except_string X, acceptable_arg_except_string Y, acceptable_arg_except_string L,
		label_option ...Options>
	PlotBuffer3D PlotLabels(const X& x, const Y& y, const L& label, Options ...ops)
	{
		PlotBuffer3D p(this);
		return p.PlotLabels(x, y, label, ops...);
	}
	template <label_option ...Options>
	PlotBuffer3D PlotLabels(std::string_view filename, std::string_view xcol, std::string_view ycol, std::string_view labelcol, Options ...ops)
	{
		PlotBuffer3D p(this);
		return p.PlotLabels(filename, xcol, ycol, labelcol, ops...);
	}
	template <acceptable_arg_except_string X, acceptable_arg_except_string Y, acceptable_arg_except_string Z, ranges::string_range L,
		label_option ...Options>
	PlotBuffer3D PlotLabels(const X& x, const Y& y, const Z& z, const L& label, Options ...ops)
	{
		PlotBuffer3D p(this);
		return p.PlotLabels(x, y, z, label, ops...);
	}
	template <label_option ...Options>
	PlotBuffer3D PlotLabels(std::string_view filename,
							std::string_view xcol, std::string_view ycol, std::string_view zcol,
							std::string_view labelcol, Options ...ops)
	{
		PlotBuffer3D p(this);
		return p.PlotLabels(filename, xcol, ycol, zcol, labelcol, ops...);
	}

	template <acceptable_matrix_range Map, ranges::arithmetic_range X, ranges::arithmetic_range Y,
			  colormap_option ...Options>
	PlotBuffer3D PlotColormap(const Map& map, const X& x, const Y& y,
							  Options ...ops)
	{
		PlotBuffer3D p(this);
		return p.PlotColormap(map, x, y, ops...);
	}
	template <acceptable_matrix_range Map, colormap_option ...Options>
	PlotBuffer3D PlotColormap(const Map& map, std::pair<double, double> x, std::pair<double, double> y,
							  Options ...ops)
	{
		PlotBuffer3D p(this);
		return p.PlotColormap(map, x, y, ops...);
	}
	template <colormap_option ...Options>
	PlotBuffer3D PlotColormap(std::string_view filename, std::string_view x, std::string_view y, std::string_view z,
							  Options ...ops)
	{
		PlotBuffer3D p(this);
		return p.PlotColormap(filename, x, y, z, ops...);
	}
	template <colormap_option ...Options>
	PlotBuffer3D PlotColormap(std::string_view equation, Options ...ops)
	{
		PlotBuffer3D p(this);
		return p.PlotColormap(equation, ops...);
	}

	template <acceptable_matrix_range Map, ranges::arithmetic_range X, ranges::arithmetic_range Y,
		surface_option ...Options>
	PlotBuffer3D PlotSurface(const Map& z, const X& x, const Y& y, Options ...ops)
	{
		PlotBuffer3D p(this);
		return p.PlotSurface(z, x, y, ops...);
	}
	template <acceptable_matrix_range Map, surface_option ...Options>
	PlotBuffer3D PlotSurface(const Map& z, std::pair<double, double> x, std::pair<double, double> y, Options ...ops)
	{
		PlotBuffer3D p(this);
		return p.PlotSurface(z, x, y, ops...);
	}
	template <surface_option ...Options>
	PlotBuffer3D PlotSurface(std::string_view filename, std::string_view x, std::string_view y, std::string_view z, Options ...ops)
	{
		PlotBuffer3D p(this);
		return p.PlotSurface(filename, x, y, z, ops...);
	}
	template <surface_option ...Options>
	PlotBuffer3D PlotSurface(std::string_view equation, Options ...ops)
	{
		PlotBuffer3D p(this);
		return p.PlotSurface(equation, ops...);
	}

	PlotBuffer3D GetBuffer()
	{
		return PlotBuffer3D(this);
	}
};

inline void PlotBuffer3D::Flush()
{
	if (m_canvas == nullptr) throw NotInitialized("Buffer is empty");
	std::string c = "splot";
	for (auto& i : m_commands)
	{
		c += i + ", ";
	}
	c.erase(c.end() - 2, c.end());
	m_canvas->Command(c);
	m_canvas = nullptr;
}
inline std::string PlotBuffer3D::GetSanitizedOutputName() const
{
	if (m_canvas->IsInMemoryDataTransferEnabled())
		return "$" + SanitizeForDataBlock(m_canvas->GetOutput()) + "_" + std::to_string(m_commands.size());
	else
		return m_canvas->GetOutput() + ".tmp" + std::to_string(m_commands.size()) + ".txt";
}

template <class X, class Y, class Z, class XE, class XEL, class XEH, class YE, class YEL, class YEH, class VC, class VS>
PlotBuffer3D PlotBuffer3D::Plot(const PointParam3D<X, Y, Z, XE, XEL, XEH, YE, YEL, YEH, VC, VS>& p)
{
	std::string command;
	if (p.IsData())
	{
		std::string output_name = GetSanitizedOutputName();
		auto [x_x2, y_y2, z_z2] = GetAxes3D(p);
		if (x_x2 == "x2") m_canvas->Command("set x2tics");
		if (y_y2 == "y2") m_canvas->Command("set y2tics");
		if (z_z2 == "z2") m_canvas->Command("set z2tics");

		std::map<std::string, std::variant<int, std::string>> column;
		std::vector<std::string> labelcolumn;

		//rangesは各変数のうち空でないものがtupleとしてまとめられている。
		auto ranges =
			ArrangeColumnOption<0, 1>(column, labelcolumn, m_canvas,
									  std::array<std::string, 12>{ "x", "y", "z", "xerrorbar", "xerrlow", "xerrhigh",
									  "yerrorbar", "yerrlow", "yerrhigh",
									  "variable_color", "variable_size" },
									  std::array<std::string_view, 12>{ x_x2, y_y2, z_z2, "", "", "", "", "", "", "", "" },
									  p.x, p.y, p.z, p.xerrorbar, p.xerrlow, p.xerrhigh, p.yerrorbar, p.yerrlow, p.yerrhigh,
									  p.variable_color, p.variable_size);
		if constexpr (std::tuple_size_v<decltype(ranges)> != 0)
			MakeDataObject(m_canvas, output_name, ranges);
		//if (!labelcolumn.empty()) column.emplace_back(std::move(labelcolumn));

		command = MakePlotCommandCommon(m_canvas->IsInMemoryDataTransferEnabled(), output_name, column, p);
	}
	else if (p.IsFile())
	{
		//xとyが与えられている場合はファイルプロット。
		//x、yにはカラムの情報が入っている。
		std::map<std::string, std::variant<int, std::string>> column;
		std::vector<std::string> labelcolumn;
		AddColumn(p.x, "x", column);
		AddColumn(p.y, "y", column);
		AddColumn(p.z, "z", column);
		AddColumn(p.xerrorbar, "xerrorbar", column);
		AddColumn(p.xerrlow, "xerrlow", column);
		AddColumn(p.xerrhigh, "xerrhigh", column);
		AddColumn(p.yerrorbar, "yerrorbar", column);
		AddColumn(p.yerrlow, "yerrlow", column);
		AddColumn(p.yerrhigh, "yerrhigh", column);
		AddColumn(p.variable_color, "variable_color", column);
		AddColumn(p.variable_size, "variable_size", column);
		AddColumn(p.variable_fillcolor, "variable_fillcolor", column);
		command = MakePlotCommandCommon(m_canvas->IsInMemoryDataTransferEnabled(), p.input, column, labelcolumn, p);
	}
	else if (p.IsEquation())
	{
		std::map<std::string, std::variant<int, std::string>> column;
		std::vector<std::string> labelcolumn;
		command = MakePlotCommandCommon(m_canvas->IsInMemoryDataTransferEnabled(), p.input, column, labelcolumn, p);
	}
	m_commands.push_back(command);
	return std::move(*this);
}
template <class X, class Y, class Z, class XL, class YL, class ZL, class VC>
PlotBuffer3D PlotBuffer3D::Plot(const VectorParam3D<X, Y, Z, XL, YL, ZL, VC>& p)
{
	std::string command;
	if (p.IsData())
	{
		std::string output_name = GetSanitizedOutputName();
		auto [x_x2, y_y2, z_z2] = GetAxes3D(p);
		if (x_x2 == "x2") m_canvas->Command("set x2tics");
		if (y_y2 == "y2") m_canvas->Command("set y2tics");
		if (z_z2 == "z2") m_canvas->Command("set z2tics");

		std::map<std::string, std::variant<int, std::string>> column;
		std::vector<std::string> labelcolumn;

		auto ranges =
			ArrangeColumnOption<0, 1>(column, labelcolumn, m_canvas,
									  std::array<std::string, 7>{ "x", "y", "z", "xlen", "ylen", "zlen", "variable_color" },
									  std::array<std::string_view, 7>{ x_x2, y_y2, z_z2, x_x2, y_y2, z_z2, "" },
									  p.x, p.y, p.z, p.xlen, p.ylen, p.zlen, p.variable_color);
		if constexpr (std::tuple_size_v<decltype(ranges)> != 0)
			MakeDataObject(m_canvas, output_name, ranges);
		//if (!labelcolumn.empty()) column.emplace_back(std::move(labelcolumn));

		command = MakePlotCommandCommon(m_canvas->IsInMemoryDataTransferEnabled(), output_name, column, labelcolumn, p);
	}
	else if (p.IsFile())
	{
		std::map<std::string, std::variant<int, std::string>> column;
		std::vector<std::string> labelcolumn;
		AddColumn(p.x, "x", column);
		AddColumn(p.y, "y", column);
		AddColumn(p.z, "z", column);
		AddColumn(p.xlen, "xlen", column);
		AddColumn(p.ylen, "ylen", column);
		AddColumn(p.zlen, "zlen", column);
		AddColumn(p.variable_color, "variable_color", column);
		command = MakePlotCommandCommon(m_canvas->IsInMemoryDataTransferEnabled(), p.input, column, labelcolumn, p);
	}
	else if (p.IsEquation())
	{
		throw InvalidArg("Equation plot is not supported for vector plot.");
	}
	m_commands.push_back(command);
	return std::move(*this);
}
//CMはFilledCurveをサポートしない。
template <class X, class Y, class Z, class L, class VTC>
PlotBuffer3D PlotBuffer3D::Plot(const LabelParam3D<X, Y, Z, L, VTC>& p)
{
	std::string command;
	if (p.IsData())
	{
		std::string output_name = GetSanitizedOutputName();
		auto [x_x2, y_y2, z_z2] = GetAxes3D(p);
		if (x_x2 == "x2") m_canvas->Command("set x2tics");
		if (y_y2 == "y2") m_canvas->Command("set y2tics");
		if (z_z2 == "z2") m_canvas->Command("set z2tics");

		//変数名とカラムのセット。
		std::map<std::string, std::variant<int, std::string>> column;
		std::vector<std::string> labelcolumn;

		//rangesは各変数のうち空でないものがtupleとしてまとめられている。
		auto ranges =
			ArrangeColumnOption<0, 1>(column, labelcolumn, m_canvas,
									  std::array<std::string, 5>{ "x", "y", "z", "label", "variable_color" },
									  std::array<std::string_view, 5>{ x_x2, y_y2, z_z2, "", "" },
									  p.x, p.y, p.z, p.label, p.variable_color);
		//dataでない場合、コンパイル時にrangesが空になってエラーになりうる。
		//ので、空tupleだったら何もしない。
		if constexpr (std::tuple_size_v<decltype(ranges)> != 0)
			MakeDataObject(m_canvas, output_name, ranges);

		command = MakePlotCommandCommon(m_canvas->IsInMemoryDataTransferEnabled(), output_name, column, labelcolumn, p);
	}
	else if (p.IsFile())
	{
		//xとyが与えられている場合はファイルプロット。
		//x、yにはカラムの情報が入っている。
		std::map<std::string, std::variant<int, std::string>> column;
		std::vector<std::string> labelcolumn;
		AddColumn(p.x, "x", column);
		AddColumn(p.y, "y", column);
		AddColumn(p.z, "z", column);
		AddColumn(p.label, "label", column);
		AddColumn(p.variable_color, "variable_color", column);
		command = MakePlotCommandCommon(m_canvas->IsInMemoryDataTransferEnabled(), p.input, column, labelcolumn, p);
	}
	else if (p.IsEquation())
	{
		std::map<std::string, std::variant<int, std::string>> column;
		std::vector<std::string> labelcolumn;
		command = MakePlotCommandCommon(m_canvas->IsInMemoryDataTransferEnabled(), p.input, column, labelcolumn, p);
	}
	m_commands.push_back(command);
	return std::move(*this);
}
template <class Map, class X, class Y>
PlotBuffer3D PlotBuffer3D::Plot(const ColormapParam<Map, X, Y>& p)
{
	constexpr bool xrange_assigned = !PlotParamBase::IsEmptyView<X>();
	constexpr bool yrange_assigned = !PlotParamBase::IsEmptyView<Y>();

	std::string command;
	if (p.IsData())
	{
		//p.mapがデータでない場合にコンパイルエラーになるのを防ぐため、constexpr ifで括っておく。
		if constexpr (ranges::arithmetic_matrix_range<Map>)
		{
			//データプロットの場合。
			std::string output_name = GetSanitizedOutputName();
			if (!p.IsXAssigned()) throw InvalidArg("xrange or xminmax must be specified.");
			if (!p.IsYAssigned()) throw InvalidArg("yrange or yminmax must be specified.");
			//std::vector<std::string> column{ "1", "2", "5" };
			std::map<std::string, std::variant<int, std::string>> column{ { "x", 3 }, { "y", 4 }, { "map", 5} };
			std::vector<std::string> labelcolumn;
			size_t xsize = p.map.size();
			size_t ysize = p.map.front().size();
			if constexpr (xrange_assigned)
			{
				if constexpr (yrange_assigned)
					MakeDataObject(m_canvas, output_name, p.map, CoordRange<X>(p.xrange), CoordRange<Y>(p.yrange));
				else
					MakeDataObject(m_canvas, output_name, p.map, CoordRange<X>(p.xrange), CoordMinMax(p.yminmax, ysize));
			}
			else
			{
				if constexpr (yrange_assigned)
					MakeDataObject(m_canvas, output_name, p.map, CoordMinMax(p.xminmax, xsize), CoordRange<Y>(p.yrange));
				else
					MakeDataObject(m_canvas, output_name, p.map, CoordMinMax(p.xminmax, xsize), CoordMinMax(p.yminmax, ysize));
			}
			if (p.with_contour)
			{
				m_canvas->Command(MakeContourPlotCommand(output_name, m_canvas->IsInMemoryDataTransferEnabled(), p));
			}
			command = MakePlotCommandCommon(m_canvas->IsInMemoryDataTransferEnabled(), output_name, column, labelcolumn, p);
		}
	}
	else if (p.IsFile())
	{
		//ファイルプロットの場合、p.map、p.xrange、p.yrangeにカラムの情報が入っている。
		std::map<std::string, std::variant<int, std::string>> column;
		std::vector<std::string> labelcolumn;
		AddColumn(p.xrange, "x", column);
		AddColumn(p.yrange, "y", column);
		AddColumn(p.map, "map", column);
		command = MakePlotCommandCommon(m_canvas->IsInMemoryDataTransferEnabled(), p.input, column, labelcolumn, p);
		if (p.with_contour)
		{
			PrintWarning("Contour plot is not supported for file plot.");
		}
	}
	else if (p.IsEquation())
	{
		std::map<std::string, std::variant<int, std::string>> column;
		std::vector<std::string> labelcolumn;
		command = MakePlotCommandCommon(m_canvas->IsInMemoryDataTransferEnabled(), p.input, column, labelcolumn, p);
	}
	m_commands.push_back(command);
	return std::move(*this);
}
template <class Map, class X, class Y, class VC, class VS>
PlotBuffer3D PlotBuffer3D::Plot(const SurfaceParam<Map, X, Y, VC, VS>& p)
{
	constexpr bool xrange_assigned = !PlotParamBase::IsEmptyView<X>();
	constexpr bool yrange_assigned = !PlotParamBase::IsEmptyView<Y>();

	std::string command;
	if (p.IsData())
	{
		//p.mapがデータでない場合にコンパイルエラーになるのを防ぐため、constexpr ifで括っておく。
		if constexpr (ranges::arithmetic_matrix_range<Map>)
		{
			//データプロットの場合。
			std::string output_name = GetSanitizedOutputName();
			if (!p.IsXAssigned()) throw InvalidArg("xrange or xminmax must be specified.");
			if (!p.IsYAssigned()) throw InvalidArg("yrange or yminmax must be specified.");
			//std::vector<std::string> column{ "1", "2", "5" };
			std::map<std::string, std::variant<int, std::string>> column{ { "x", 3 }, { "y", 4 }, { "z", 5} };
			int c = 6;
			if constexpr (SurfaceParam<Map, X, Y, VC, VS>::HasVariableColor()) column.emplace("variable_color", c), ++c;
			if constexpr (SurfaceParam<Map, X, Y, VC, VS>::HasVariableSize()) column.emplace("variable_size", c);
			std::vector<std::string> labelcolumn;
			size_t xsize = p.z.size();
			size_t ysize = p.z.front().size();
			if constexpr (xrange_assigned)
			{
				if constexpr (yrange_assigned)
					MakeDataObject(m_canvas, output_name, p.z, CoordRange<X>(p.xrange), CoordRange<Y>(p.yrange));
				else
					MakeDataObject(m_canvas, output_name, p.z, CoordRange<X>(p.xrange), CoordMinMax(p.yminmax, ysize));
			}
			else
			{
				if constexpr (yrange_assigned)
					MakeDataObject(m_canvas, output_name, p.z, CoordMinMax(p.xminmax, xsize), CoordRange<Y>(p.yrange));
				else
					MakeDataObject(m_canvas, output_name, p.z, CoordMinMax(p.xminmax, xsize), CoordMinMax(p.yminmax, ysize));
			}
			if (p.with_contour)
			{
				m_canvas->Command(MakeContourPlotCommand(output_name, m_canvas->IsInMemoryDataTransferEnabled(), p));
			}
			command = MakePlotCommandCommon(m_canvas->IsInMemoryDataTransferEnabled(), output_name, column, labelcolumn, p);
		}
	}
	else if (p.IsFile())
	{
		//ファイルプロットの場合、p.map、p.xrange、p.yrangeにカラムの情報が入っている。
		std::map<std::string, std::variant<int, std::string>> column;
		std::vector<std::string> labelcolumn;
		AddColumn(p.xrange, "x", column);
		AddColumn(p.yrange, "y", column);
		AddColumn(p.z, "z", column);
		command = MakePlotCommandCommon(m_canvas->IsInMemoryDataTransferEnabled(), p.input, column, labelcolumn, p);
		if (p.with_contour)
		{
			PrintWarning("Contour plot is not supported for file plot.");
		}
	}
	else if (p.IsEquation())
	{
		std::map<std::string, std::variant<int, std::string>> column;
		std::vector<std::string> labelcolumn;
		command = MakePlotCommandCommon(m_canvas->IsInMemoryDataTransferEnabled(), p.input, column, labelcolumn, p);
	}
	m_commands.push_back(command);
	return std::move(*this);
}

}

ADAPT_EXPORT
using Canvas2D = plot_detail::Canvas2D;
ADAPT_EXPORT
using Canvas3D = plot_detail::Canvas3D;

}

#endif
