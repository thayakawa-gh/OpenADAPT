#ifndef ADAPT_PLOT_CANVAS_H
#define ADAPT_PLOT_CANVAS_H

#include <OpenADAPT/Utility/Matrix.h>
#include <OpenADAPT/Utility/Function.h>
#include <OpenADAPT/Utility/KeywordArgs.h>
#include <OpenADAPT/Plot/Core.h>
#include <OpenADAPT/Plot/Axes.h>
#include <OpenADAPT/Plot/PlotCommand.h>

namespace adapt
{

namespace plot_detail
{

struct PlotBuffer2D
{
	PlotBuffer2D(Canvas* g) : m_canvas(g) {}
	PlotBuffer2D(const PlotBuffer2D&) = delete;
	PlotBuffer2D(PlotBuffer2D&& p) noexcept
		: m_commands(std::move(p.m_commands)), m_canvas(p.m_canvas)
	{
		p.m_canvas = nullptr;
	}
	PlotBuffer2D& operator=(const PlotBuffer2D&) = delete;
	PlotBuffer2D& operator=(PlotBuffer2D&& p) noexcept
	{
		m_canvas = p.m_canvas; p.m_canvas = nullptr;
		m_commands = std::move(p.m_commands);
		return *this;
	}
	virtual ~PlotBuffer2D()
	{
		//mCanvasがnullptrでないときはこのPlotBufferが最終処理を担当する。
		if (m_canvas != nullptr) Flush();
	}

	void Flush()
	{
		if (m_canvas == nullptr) throw NotInitialized("Buffer is empty");
		std::string c = "plot";
		for (auto& i : m_commands)
		{
			c += i + ", ";
		}
		c.erase(c.end() - 2, c.end());
		m_canvas->Command(c);
	}

	template <gnuplot_acceptable_arg X, gnuplot_acceptable_arg Y,
			  point_option ...Options>
	PlotBuffer2D PlotPoints(const X& x, const Y& y, Options ...ops)
	{
		auto p = MakePointParam(plot::x = x, plot::y = y, ops...);
		return Plot(p);
	}
	template <point_option ...Options>
	PlotBuffer2D PlotPoints(std::string_view filename, std::string_view xcol, std::string_view ycol, Options ...ops)
	{
		auto p = MakePointParam(plot::input = filename, plot::x = xcol, plot::y = ycol, ops...);
		return Plot(p);
	}
	template <point_option ...Options>
	PlotBuffer2D PlotPoints(std::string_view equation, Options ...ops)
	{
		auto p = MakePointParam(plot::input = equation, ops...);
		return Plot(p);
	}

	template <gnuplot_acceptable_arg X, gnuplot_acceptable_arg Y,
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
		auto p = MakeHistogramParam(plot::data = data, plot::xmin = min, plot::xmax = max, plot::xnbin = nbin, ops...);
		return Plot(p, ops...);
	}

	template <gnuplot_acceptable_arg X, gnuplot_acceptable_arg Y,
			  gnuplot_acceptable_arg XL, gnuplot_acceptable_arg YL,
			  vector_option ...Options>
	PlotBuffer2D PlotVectors(const X& xfrom, const Y& yfrom,
							 const XL& xlen, const YL& ylen,
							 Options ...ops)
	{
		auto p = MakeVectorParam(plot::x = xfrom, plot::y = yfrom, plot::xlen = xlen, plot::ylen = ylen, ops...);
		return Plot(p);
	}
	template <vector_option ...Options>
	PlotBuffer2D PlotVectors(std::string_view filename,
							 std::string_view xfrom, std::string_view yfrom,
							 std::string_view xlen, std::string_view ylen,
							 Options ...ops)
	{
		auto p = MakeVectorParam(plot::input = filename,
								 plot::x = xfrom, plot::xlen = xlen, plot::y = yfrom, plot::ylen = ylen, ops...);
		return Plot(p);
	}

	template <gnuplot_acceptable_arg X, gnuplot_acceptable_arg Y,
			  filledcurve_option ...Options>
	PlotBuffer2D PlotFilledCurves(const X& x, const Y& y, Options ...ops)
	{
		auto p = MakeFilledCurveParam(plot::x = x, plot::y = y, ops...);
		return Plot(p);
	}
	template <filledcurve_option ...Options>
	PlotBuffer2D PlotFilledCurves(std::string_view filename, std::string_view x, std::string_view y, Options ...ops)
	{
		auto p = MakeFilledCurveParam(plot::input = filename, plot::x = x, plot::y = y, ops...);
		return Plot(p);
	}
	template <gnuplot_acceptable_arg X, gnuplot_acceptable_arg Y,
			  gnuplot_acceptable_arg Y2,
			  filledcurve_option ...Options>
	PlotBuffer2D PlotFilledCurves(const X& x, const Y& y, const Y2& ybelow, Options ...ops)
	{
		auto p = MakeFilledCurveParam(plot::x = x, plot::y = y, plot::ybelow = ybelow, ops...);
		return Plot(p);
	}
	template <filledcurve_option ...Options>
	PlotBuffer2D PlotFilledCurves(std::string_view filename, std::string_view x, std::string_view y, std::string_view ybelow,
								  Options ...ops)
	{
		auto p = MakeFilledCurveParam(plot::input = filename, plot::x = x, plot::y = y, plot::ybelow = ybelow, ops...);
		return Plot(p);
	}

protected:

	std::string GetSanitizedOutputName() const
	{
		if (m_canvas->IsInMemoryDataTransferEnabled())
			return "$" + SanitizeForDataBlock(m_canvas->GetOutput()) + "_" + std::to_string(m_commands.size());
		else
			return m_canvas->GetOutput() + ".tmp" + std::to_string(m_commands.size()) + ".txt";
	}

	template <class X, class Y, class XE, class YE, class VC, class VS, class VFC>
	PlotBuffer2D Plot(const PointParam<X, Y, XE, YE, VC, VS, VFC>& p)
	{
		std::string command;
		if (p.IsData())
		{
			std::string output_name = GetSanitizedOutputName();
			auto [x_x2, y_y2] = GetAxes2D(p);
			if (x_x2 == "x2") m_canvas->Command("set x2tics");
			if (y_y2 == "y2") m_canvas->Command("set y2tics");

			std::vector<std::string> column;
			std::string labelcolumn;

			//rangesは各変数のうち空でないものがtupleとしてまとめられている。
			auto ranges =
				ArrangeColumnOption<0, 1>(column, labelcolumn, m_canvas,
										  std::array<std::string_view, 7>{ x_x2, y_y2, "", "", "", "", "" },
										  p.x, p.y, p.xerrorbar, p.yerrorbar, p.variable_color, p.variable_size, p.variable_fillcolor);
			//dataでない場合、コンパイル時にrangesが空になってエラーになりうる。
			//ので、空tupleだったら何もしない。
			if constexpr (std::tuple_size_v<decltype(ranges)> != 0)
				MakeDataObject(m_canvas, output_name, ranges);
			if (!labelcolumn.empty()) column.emplace_back(std::move(labelcolumn));

			command = MakePlotCommandCommon(m_canvas->IsInMemoryDataTransferEnabled(), output_name, column, p);
		}
		else if (p.IsFile())
		{
			//xとyが与えられている場合はファイルプロット。
			//x、yにはカラムの情報が入っている。
			std::vector<std::string> column;
			AddColumn(p.x, "x", column);
			AddColumn(p.y, "y", column);
			AddColumn(p.xerrorbar, "xerrorbar", column);
			AddColumn(p.yerrorbar, "yerrorbar", column);
			AddColumn(p.variable_color, "variable_color", column);
			AddColumn(p.variable_size, "variable_size", column);
			AddColumn(p.variable_fillcolor, "variable_fillcolor", column);
			command = MakePlotCommandCommon(m_canvas->IsInMemoryDataTransferEnabled(), p.input, column, p);
		}
		else if (p.IsEquation())
		{
			command = MakePlotCommandCommon(m_canvas->IsInMemoryDataTransferEnabled(), p.input, std::vector<std::string>{}, p);
		}
		m_commands.push_back(command);
		return std::move(*this);
	}
	template <class Data, class ...Options>
	PlotBuffer2D Plot(const HistogramParam<Data>& p, Options ...ops)
	{
		//stair_style::histogramなら、最後に余計なビンを加える必要はなさそう。
		std::vector<double> hist(p.xnbin, 0);
		double wbin = (p.xmax - p.xmin) / p.xnbin;
		auto ibin = [&p, wbin](double v)
		{
			return (int64_t)((v - p.xmin) / wbin);
		};
		auto bincenter = [&p, wbin](size_t i)
		{
			return p.xmin + wbin * (i + 0.5);
		};

		for (auto&& v : p.data)
		{
			int64_t i = ibin(v);
			if (i < 0 || std::cmp_greater_equal(i, p.xnbin)) continue;
			++hist[i];
		}

		std::vector<double> x(p.xnbin);
		for (size_t i = 0; i < p.xnbin; ++i) x[i] = p.xmin + wbin * (i + 0.5);

		auto p2 = MakePointParam(plot::x = x, plot::y = hist, ops..., plot::s_histeps);
		return Plot(p2);
	}
	template <class X, class Y, class XL, class YL, class VC>
	PlotBuffer2D Plot(const VectorParam<X, Y, XL, YL, VC>& p)
	{
		std::string command;
		if (p.IsData())
		{
			std::string output_name = GetSanitizedOutputName();
			auto [x_x2, y_y2] = GetAxes2D(p);
			if (x_x2 == "x2") m_canvas->Command("set x2tics");
			if (y_y2 == "y2") m_canvas->Command("set y2tics");

			std::vector<std::string> column;
			std::string labelcolumn;

			auto ranges =
				ArrangeColumnOption<0, 1>(column, labelcolumn, m_canvas,
										  std::array<std::string_view, 5>{ x_x2, y_y2, x_x2, y_y2, "" },
										  p.x, p.y, p.xlen, p.ylen, p.variable_color);
			if constexpr (std::tuple_size_v<decltype(ranges)> != 0)
				MakeDataObject(m_canvas, output_name, ranges);
			if (!labelcolumn.empty()) column.emplace_back(std::move(labelcolumn));

			command = MakePlotCommandCommon(m_canvas->IsInMemoryDataTransferEnabled(), output_name, column, p);
		}
		else if (p.IsFile())
		{
			std::vector<std::string> column;
			AddColumn(p.x, "x", column);
			AddColumn(p.y, "y", column);
			AddColumn(p.xlen, "xlen", column);
			AddColumn(p.ylen, "ylen", column);
			AddColumn(p.variable_color, "variable_color", column);
			command = MakePlotCommandCommon(m_canvas->IsInMemoryDataTransferEnabled(), p.input, column, p);
		}
		else if (p.IsEquation())
		{
			throw InvalidArg("Equation plot is not supported for vector plot.");
		}
		m_commands.push_back(command);
		return std::move(*this);
	}
	template <class X, class Y, class Y2, class VC>
	PlotBuffer2D Plot(const FilledCurveParam<X, Y, Y2, VC>& p)
	{
		std::string command;
		if (p.IsData())
		{
			std::string output_name = GetSanitizedOutputName();
			auto [x_x2, y_y2] = GetAxes2D(p);
			if (x_x2 == "x2") m_canvas->Command("set x2tics");
			if (y_y2 == "y2") m_canvas->Command("set y2tics");

			std::vector<std::string> column;
			std::string labelcolumn;

			auto ranges =
				ArrangeColumnOption<0, 1>(column, labelcolumn, m_canvas,
										  std::array<std::string_view, 4>{ x_x2, y_y2, y_y2, ""},
										  p.x, p.y, p.ybelow, p.variable_fillcolor);
			if constexpr (std::tuple_size_v<decltype(ranges)> != 0)
				MakeDataObject(m_canvas, output_name, ranges);
			if (!labelcolumn.empty()) column.emplace_back(std::move(labelcolumn));

			command = MakePlotCommandCommon(m_canvas->IsInMemoryDataTransferEnabled(), output_name, column, p);
		}
		else if (p.IsFile())
		{
			std::vector<std::string> column;
			AddColumn(p.x, "x", column);
			AddColumn(p.y, "y", column);
			AddColumn(p.ybelow, "y2", column);
			AddColumn(p.variable_fillcolor, "variable_fillcolor", column);
			command = MakePlotCommandCommon(m_canvas->IsInMemoryDataTransferEnabled(), p.input, column, p);
		}
		else if (p.IsEquation())
		{
			command = MakePlotCommandCommon(m_canvas->IsInMemoryDataTransferEnabled(), p.input, std::vector<std::string>{}, p);
		}
		m_commands.push_back(command);
		return std::move(*this);
	}

	std::vector<std::string> m_commands;
	Canvas* m_canvas;
};

class Canvas2D : public Axis2D<Canvas>
{
public:

	using Axis2D<Canvas>::Axis2D;

	friend class MultiPlotter;

	template <gnuplot_acceptable_arg X, gnuplot_acceptable_arg Y,
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

	template <gnuplot_acceptable_arg X, gnuplot_acceptable_arg Y,
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

	template <gnuplot_acceptable_arg X, gnuplot_acceptable_arg Y,
			  gnuplot_acceptable_arg XL, gnuplot_acceptable_arg YL,
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

	template <gnuplot_acceptable_arg X, gnuplot_acceptable_arg Y,
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
	template <gnuplot_acceptable_arg X, gnuplot_acceptable_arg Y,
			  gnuplot_acceptable_arg Y2,
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

	PlotBuffer2D GetBuffer()
	{
		return PlotBuffer2D(this);
	}

};

}

namespace plot_detail
{

struct PlotBufferCM
{
	PlotBufferCM(Canvas* g) : m_canvas(g) {}
	PlotBufferCM(const PlotBufferCM&) = delete;
	PlotBufferCM(PlotBufferCM&& p) noexcept
		: m_commands(std::move(p.m_commands)), m_canvas(p.m_canvas)
	{
		p.m_canvas = nullptr;
	}
	PlotBufferCM& operator=(const PlotBufferCM&) = delete;
	PlotBufferCM& operator=(PlotBufferCM&& p) noexcept
	{
		m_canvas = p.m_canvas; p.m_canvas = nullptr;
		m_commands = std::move(p.m_commands);
		return *this;
	}
	virtual ~PlotBufferCM()
	{
		//mPipeがnullptrでないときはこのPlotterが最終処理を担当する。
		if (m_canvas != nullptr) Flush();
	}

	void Flush()
	{
		if (m_canvas == nullptr) throw NotInitialized("Buffer is empty");
		std::string c = "splot";
		for (auto& i : m_commands)
		{
			c += i + ", ";
		}
		c.erase(c.end() - 2, c.end());
		m_canvas->Command(c);
	}

	template <gnuplot_acceptable_arg X, gnuplot_acceptable_arg Y, gnuplot_acceptable_arg Z,
			  point_option ...Options>
	PlotBufferCM PlotPoints(const X& x, const Y& y, const Z& z, Options ...ops)
	{
		auto p = MakePoint3DParam(plot::x = x, plot::y = y, plot::z = z, ops...);
		return Plot(p);
	}
	template <point_option ...Options>
	PlotBufferCM PlotPoints(std::string_view filename,
							std::string_view x, std::string_view y, std::string_view z,
							Options ...ops)
	{
		auto p = MakePoint3DParam(plot::input = filename, plot::x = x, plot::y = y, plot::z = z, ops...);
		return Plot(p);
	}
	template <gnuplot_acceptable_arg X, gnuplot_acceptable_arg Y,
			  point_option ...Options>
	PlotBufferCM PlotPoints(const X& x, const Y& y, Options ...ops)
	{
		auto p = MakePoint3DParam(plot::x = x, plot::y = y, plot::z = 0, ops...);
		return Plot(p);
	}
	template <point_option ...Options>
	PlotBufferCM PlotPoints(std::string_view filename,
							std::string_view x, std::string_view y,
							Options ...ops)
	{
		auto p = MakePoint3DParam(plot::input = filename, plot::x = x, plot::y = y, plot::z = "($1-$1)", ops...);
		return Plot(p);
	}
	template <point_option ...Options>
	PlotBufferCM PlotPoints(std::string_view equation, Options ...ops)
	{
		auto p = MakePoint3DParam(plot::input = equation, plot::z = "($1-$1)", ops...);
		return Plot(p);
	}

	template <gnuplot_acceptable_arg X, gnuplot_acceptable_arg Y, gnuplot_acceptable_arg Z,
			  point_option ...Options>
	PlotBufferCM PlotLines(const X& x, const Y& y, const Z& z, Options ...ops)
	{
		return PlotPoints(x, y, z, plot::style = Style::lines, ops...);
	}
	template <point_option ...Options>
	PlotBufferCM PlotLines(std::string_view filename,
						   std::string_view x, std::string_view y, std::string_view z,
						   Options ...ops)
	{
		return PlotPoints(filename, x, y, z, plot::style = Style::lines, ops...);
	}
	template <gnuplot_acceptable_arg X, gnuplot_acceptable_arg Y,
			  point_option ...Options>
	PlotBufferCM PlotLines(const X& x, const Y& y, Options ...ops)
	{
		return PlotPoints(x, y, plot::style = Style::lines, ops...);
	}
	template <point_option ...Options>
	PlotBufferCM PlotLines(std::string_view filename,
						   std::string_view x, std::string_view y,
						   Options ...ops)
	{
		return PlotPoints(filename, x, y, plot::style = Style::lines, ops...);
	}
	template <point_option ...Options>
	PlotBufferCM PlotLines(std::string_view equation, Options ...ops)
	{
		return PlotPoints(equation, plot::style = Style::lines, ops...);
	}

	template <gnuplot_acceptable_arg X, gnuplot_acceptable_arg Y, gnuplot_acceptable_arg Z,
			  gnuplot_acceptable_arg XL, gnuplot_acceptable_arg YL, gnuplot_acceptable_arg ZL,
			  vector_option ...Options>
	PlotBufferCM PlotVectors(const X& xfrom, const Y& yfrom, const Z& zfrom,
							 const XL& xlen, const YL& ylen, const ZL& zlen,
							 Options ...ops)
	{
		auto p = MakeVectorParam3D(plot::x = xfrom, plot::y = yfrom, plot::z = zfrom,
								   plot::xlen = xlen, plot::ylen = ylen, plot::zlen = zlen, ops...);
		return Plot(p);
	}
	template <vector_option ...Options>
	PlotBufferCM PlotVectors(std::string_view filename,
							 std::string_view xfrom, std::string_view yfrom, std::string_view zfrom,
							 std::string_view xlen, std::string_view ylen, std::string_view zlen,
							 Options ...ops)
	{
		auto p = MakeVectorParam3D(plot::input = filename,
								   plot::x = xfrom, plot::y = yfrom, plot::z = zfrom,
								   plot::xlen = xlen, plot::ylen = ylen, plot::zlen = zlen, ops...);
		return Plot(p);
	}
	template <gnuplot_acceptable_arg X, gnuplot_acceptable_arg Y,
			  gnuplot_acceptable_arg XL, gnuplot_acceptable_arg YL,
			  vector_option ...Options>
	PlotBufferCM PlotVectors(const X& xfrom, const Y& yfrom,
							 const XL& xlen, const YL& ylen,
							 Options ...ops)
	{
		auto p = MakeVectorParam3D(plot::x = xfrom, plot::y = yfrom, plot::z = 0.,
								   plot::xlen = xlen, plot::ylen = ylen, plot::zlen = 0., ops...);
		return Plot(p);
	}
	template <vector_option ...Options>
	PlotBufferCM PlotVectors(std::string_view filename,
							 std::string_view xfrom, std::string_view yfrom,
							 std::string_view xlen, std::string_view ylen,
							 Options ...ops)
	{
		auto p = MakeVectorParam3D(plot::input = filename,
								   plot::x = xfrom, plot::y = yfrom, plot::z = "($1-$1)",
								   plot::xlen = xlen, plot::ylen = ylen, plot::zlen = "($1-$1)", ops...);
		return Plot(p);
	}

	template <gnuplot_acceptable_matrix_range Map,
			  ranges::arithmetic_range XRange, ranges::arithmetic_range YRange,
			  colormap_option ...Options>
	PlotBufferCM PlotColormap(const Map& map_, const XRange& x, const YRange& y,
							  Options ...ops)
	{
		auto p = MakeColormapParam(plot::map = map_, plot::xrange = x, plot::yrange = y, ops...);
		return Plot(p);
	}
	template <gnuplot_acceptable_matrix_range Map, colormap_option ...Options>
	PlotBufferCM PlotColormap(const Map& map_, std::pair<double, double> x, std::pair<double, double> y,
							  Options ...ops)
	{
		auto p = MakeColormapParam(plot::map = map_, plot::xminmax = x, plot::yminmax = y, ops...);
		return Plot(p);
	}
	template <colormap_option ...Options>
	PlotBufferCM PlotColormap(std::string_view filename, std::string_view x, std::string_view y, std::string_view map,
							  Options ...ops)
	{
		auto p = MakeColormapParam(plot::input = filename, plot::map = map, plot::xrange = x, plot::yrange = y, ops...);
		return Plot(p);
	}
	template <colormap_option ...Options>
	PlotBufferCM PlotColormap(std::string_view equation, Options ...ops)
	{
		auto p = MakeColormapParam(plot::input = equation, ops...);
		return Plot(p);
	}

protected:

	std::string GetSanitizedOutputName() const
	{
		if (m_canvas->IsInMemoryDataTransferEnabled())
			return "$" + SanitizeForDataBlock(m_canvas->GetOutput()) + "_" + std::to_string(m_commands.size());
		else
			return m_canvas->GetOutput() + ".tmp" + std::to_string(m_commands.size()) + ".txt";
	}

	template <class Map, class X, class Y>
	PlotBufferCM Plot(const ColormapParam<Map, X, Y>& p)
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
				std::vector<std::string> column{ "1", "2", "5" };
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
				command = MakePlotCommandCommon(m_canvas->IsInMemoryDataTransferEnabled(), output_name, column, p);
			}
		}
		else if (p.IsFile())
		{
			//ファイルプロットの場合、p.map、p.xrange、p.yrangeにカラムの情報が入っている。
			std::vector<std::string> column;
			AddColumn(p.xrange, "x", column);
			AddColumn(p.yrange, "y", column);
			AddColumn(p.map, "map", column);
			command = MakePlotCommandCommon(m_canvas->IsInMemoryDataTransferEnabled(), p.input, column, p);
			if (p.with_contour)
			{
				PrintWarning("Contour plot is not supported for file plot.");
			}
		}
		else if (p.IsEquation())
		{
			command = MakePlotCommandCommon(m_canvas->IsInMemoryDataTransferEnabled(), p.input, std::vector<std::string>{}, p);
		}
		m_commands.push_back(command);
		return std::move(*this);
	}
	template <class X, class Y, class Z, class XE, class YE, class ZE, class VC, class VS, class VFC>
	PlotBufferCM Plot(const PointParam3D<X, Y, Z, XE, YE, ZE, VC, VS, VFC>& p)
	{
		std::string command;
		if (p.IsData())
		{
			std::string output_name = GetSanitizedOutputName();
			auto [x_x2, y_y2, z_z2] = GetAxes3D(p);
			if (x_x2 == "x2") m_canvas->Command("set x2tics");
			if (y_y2 == "y2") m_canvas->Command("set y2tics");
			if (z_z2 == "z2") m_canvas->Command("set z2tics");

			std::vector<std::string> column;
			std::string labelcolumn;

			//rangesは各変数のうち空でないものがtupleとしてまとめられている。
			auto ranges =
				ArrangeColumnOption<0, 1>(column, labelcolumn, m_canvas,
										  std::array<std::string_view, 9>{ x_x2, y_y2, z_z2, "", "", "", "", "", "" },
										  p.x, p.y, p.z, p.xerrorbar, p.yerrorbar, p.zerrorbar, p.variable_color, p.variable_size, p.variable_fillcolor);
			if constexpr (std::tuple_size_v<decltype(ranges)> != 0)
				MakeDataObject(m_canvas, output_name, ranges);
			if (!labelcolumn.empty()) column.emplace_back(std::move(labelcolumn));

			command = MakePlotCommandCommon(m_canvas->IsInMemoryDataTransferEnabled(), output_name, column, p);
		}
		else if (p.IsFile())
		{
			//xとyが与えられている場合はファイルプロット。
			//x、yにはカラムの情報が入っている。
			std::vector<std::string> column;
			AddColumn(p.x, "x", column);
			AddColumn(p.y, "y", column);
			AddColumn(p.z, "z", column);
			AddColumn(p.xerrorbar, "xerrorbar", column);
			AddColumn(p.yerrorbar, "yerrorbar", column);
			AddColumn(p.zerrorbar, "zerrorbar", column);
			AddColumn(p.variable_color, "variable_color", column);
			AddColumn(p.variable_size, "variable_size", column);
			AddColumn(p.variable_fillcolor, "variable_fillcolor", column);
			command = MakePlotCommandCommon(m_canvas->IsInMemoryDataTransferEnabled(), p.input, column, p);
		}
		else if (p.IsEquation())
		{
			command = MakePlotCommandCommon(m_canvas->IsInMemoryDataTransferEnabled(), p.input, std::vector<std::string>{}, p);
		}
		m_commands.push_back(command);
		return std::move(*this);
	}
	template <class X, class Y, class Z, class XL, class YL, class ZL, class VC>
	PlotBufferCM Plot(const VectorParam3D<X, Y, Z, XL, YL, ZL, VC>& p)
	{
		std::string command;
		if (p.IsData())
		{
			std::string output_name = GetSanitizedOutputName();
			auto [x_x2, y_y2, z_z2] = GetAxes3D(p);
			if (x_x2 == "x2") m_canvas->Command("set x2tics");
			if (y_y2 == "y2") m_canvas->Command("set y2tics");
			if (z_z2 == "z2") m_canvas->Command("set z2tics");

			std::vector<std::string> column;
			std::string labelcolumn;

			auto ranges =
				ArrangeColumnOption<0, 1>(column, labelcolumn, m_canvas,
										  std::array<std::string_view, 7>{ x_x2, y_y2, z_z2, x_x2, y_y2, z_z2, "" },
										  p.x, p.y, p.z, p.xlen, p.ylen, p.zlen, p.variable_color);
			if constexpr (std::tuple_size_v<decltype(ranges)> != 0)
				MakeDataObject(m_canvas, output_name, ranges);
			if (!labelcolumn.empty()) column.emplace_back(std::move(labelcolumn));

			command = MakePlotCommandCommon(m_canvas->IsInMemoryDataTransferEnabled(), output_name, column, p);
		}
		else if (p.IsFile())
		{
			std::vector<std::string> column;
			AddColumn(p.x, "x", column);
			AddColumn(p.y, "y", column);
			AddColumn(p.z, "z", column);
			AddColumn(p.xlen, "xlen", column);
			AddColumn(p.ylen, "ylen", column);
			AddColumn(p.zlen, "zlen", column);
			AddColumn(p.variable_color, "variable_color", column);
			command = MakePlotCommandCommon(m_canvas->IsInMemoryDataTransferEnabled(), p.input, column, p);
		}
		else if (p.IsEquation())
		{
			throw InvalidArg("Equation plot is not supported for vector plot.");
		}
		m_commands.push_back(command);
		return std::move(*this);
	}
	//CMはFilledCurveをサポートしない。


	/*PlotBufferCM Plot(GraphParam& i)
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
			std::vector<std::string> column;
			std::string labelcolumn;

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
					else
					{
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

	static std::string PlotCommand(const GraphParam& p, const bool IsInMemoryDataTransferEnabled)
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
	}*/
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
	Canvas* m_canvas;
};


class CanvasCM : public Axis2D<Canvas>
{
public:

	CanvasCM(const std::string& output, double sizex = 0., double sizey = 0.)
		: Axis2D<Canvas>(output, sizex, sizey)
	{
		if (m_pipe)
		{
			this->Command("set pm3d corners2color c1");
			this->Command("set view map");
		}
	}
	CanvasCM()
	{
		if (m_pipe)
		{
			this->Command("set pm3d corners2color c1");
			this->Command("set view map");
		}
	}

	friend class MultiPlotter;

	template <gnuplot_acceptable_arg X, gnuplot_acceptable_arg Y, gnuplot_acceptable_arg Z,
			  point_option ...Options>
	PlotBufferCM PlotPoints(const X& x, const Y& y, const Z& z, Options ...ops)
	{
		PlotBufferCM p(this);
		return p.PlotPoints(x, y, z, ops...);
	}
	template <point_option ...Options>
	PlotBufferCM PlotPoints(std::string_view filename,
							std::string_view x, std::string_view y, std::string_view z,
							Options ...ops)
	{
		PlotBufferCM p(this);
		return p.PlotPoints(filename, x, y, z, ops...);
	}
	template <gnuplot_acceptable_arg X, gnuplot_acceptable_arg Y,
			  point_option ...Options>
	PlotBufferCM PlotPoints(const X& x, const Y& y, Options ...ops)
	{
		PlotBufferCM p(this);
		return p.PlotPoints(x, y, ops...);
	}
	template <point_option ...Options>
	PlotBufferCM PlotPoints(std::string_view filename,
							std::string_view x, std::string_view y,
							Options ...ops)
	{
		PlotBufferCM p(this);
		return p.PlotPoints(filename, x, y, ops...);
	}
	template <point_option ...Options>
	PlotBufferCM PlotPoints(std::string_view equation, Options ...ops)
	{
		PlotBufferCM p(this);
		return p.PlotPoints(equation, ops...);
	}

	template <gnuplot_acceptable_arg X, gnuplot_acceptable_arg Y, gnuplot_acceptable_arg Z,
			  point_option ...Options>
	PlotBufferCM PlotLines(const X& x, const Y& y, const Z& z, Options ...ops)
	{
		PlotBufferCM p(this);
		return p.PlotLines(x, y, z, ops...);
	}
	template <point_option ...Options>
	PlotBufferCM PlotLines(std::string_view filename,
						   std::string_view x, std::string_view y, std::string_view z,
						   Options ...ops)
	{
		PlotBufferCM p(this);
		return p.PlotLines(filename, x, y, z, ops...);
	}
	template <gnuplot_acceptable_arg X, gnuplot_acceptable_arg Y,
			  point_option ...Options>
	PlotBufferCM PlotLines(const X& x, const Y& y, Options ...ops)
	{
		PlotBufferCM p(this);
		return p.PlotLines(x, y, ops...);
	}
	template <point_option ...Options>
	PlotBufferCM PlotLines(std::string_view filename,
						   std::string_view x, std::string_view y,
						   Options ...ops)
	{
		PlotBufferCM p(this);
		return p.PlotLines(filename, x, y, ops...);
	}
	template <point_option ...Options>
	PlotBufferCM PlotLines(std::string_view equation, Options ...ops)
	{
		PlotBufferCM p(this);
		return p.PlotLines(equation, ops...);
	}

	template <gnuplot_acceptable_arg X, gnuplot_acceptable_arg Y, gnuplot_acceptable_arg Z,
			  gnuplot_acceptable_arg XL, gnuplot_acceptable_arg YL, gnuplot_acceptable_arg ZL,
			  vector_option ...Options>
	PlotBufferCM PlotVectors(const X& xfrom, const Y& yfrom, const Z& zfrom,
							 const XL& xlen, const YL& ylen, const ZL& zlen,
							 Options ...ops)
	{
		PlotBufferCM p(this);
		return p.PlotVectors(xfrom, yfrom, zfrom, xlen, ylen, zlen, ops...);
	}
	template <vector_option ...Options>
	PlotBufferCM PlotVectors(std::string_view filename,
							 std::string_view xfrom, std::string_view yfrom, std::string_view zfrom,
							 std::string_view xlen, std::string_view ylen, std::string_view zlen,
							 Options ...ops)
	{
		PlotBufferCM p(this);
		return p.PlotVectors(filename, xfrom, yfrom, zfrom, xlen, ylen, zlen, ops...);
	}
	template <gnuplot_acceptable_arg X, gnuplot_acceptable_arg Y,
			  gnuplot_acceptable_arg XL, gnuplot_acceptable_arg YL,
			  vector_option ...Options>
	PlotBufferCM PlotVectors(const X& xfrom, const Y& yfrom,
							 const XL& xlen, const YL& ylen,
							 Options ...ops)
	{
		PlotBufferCM p(this);
		return p.PlotVectors(xfrom, yfrom, xlen, ylen, ops...);
	}
	template <vector_option ...Options>
	PlotBufferCM PlotVectors(std::string_view filename,
							 std::string_view xfrom, std::string_view yfrom,
							 std::string_view xlen, std::string_view ylen,
							 Options ...ops)
	{
		PlotBufferCM p(this);
		return p.PlotVectors(filename, xfrom, yfrom, xlen, ylen, ops...);
	}

	template <gnuplot_acceptable_matrix_range Map, ranges::arithmetic_range X, ranges::arithmetic_range Y,
			  colormap_option ...Options>
	PlotBufferCM PlotColormap(const Map& map, const X& x, const Y& y,
							  Options ...ops)
	{
		PlotBufferCM p(this);
		return p.PlotColormap(map, x, y, ops...);
	}
	template <gnuplot_acceptable_matrix_range Map, colormap_option ...Options>
	PlotBufferCM PlotColormap(const Map& map, std::pair<double, double> x, std::pair<double, double> y,
							  Options ...ops)
	{
		PlotBufferCM p(this);
		return p.PlotColormap(map, x, y, ops...);
	}
	template <colormap_option ...Options>
	PlotBufferCM PlotColormap(std::string_view filename, std::string_view x, std::string_view y, std::string_view z,
							  Options ...ops)
	{
		PlotBufferCM p(this);
		return p.PlotColormap(filename, x, y, z, ops...);
	}
	template <colormap_option ...Options>
	PlotBufferCM PlotColormap(std::string_view equation, Options ...ops)
	{
		PlotBufferCM p(this);
		return p.PlotColormap(equation, ops...);
	}

	PlotBufferCM GetBuffer()
	{
		return PlotBufferCM(this);
	}
};

}

using Canvas2D = plot_detail::Canvas2D;
using CanvasCM = plot_detail::CanvasCM;

}

#endif
