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

	template <acceptable_arg X, acceptable_arg Y,
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

	template <acceptable_arg X, acceptable_arg Y,
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

	template <acceptable_arg X, acceptable_arg Y,
			  acceptable_arg XL, acceptable_arg YL,
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

	template <acceptable_arg X, acceptable_arg Y,
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
	template <acceptable_arg X, acceptable_arg Y,
			  acceptable_arg Y2,
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

	template <class X, class Y, class XE, class YE, class XEL, class XEH, class YEL, class YEH, class VC, class VS, class VFC>
	PlotBuffer2D Plot(const PointParam<X, Y, XE, YE, XEL, XEH, YEL, YEH, VC, VS, VFC>& p)
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
																		   "variable_color", "variable_size", "variable_fillcolor" },
										  std::array<std::string_view, 11>{ x_x2, y_y2, "", "", "", "", "", "", "", "", "" },
										  p.x, p.y, p.xerrorbar, p.xerrlow, p.xerrhigh, p.yerrorbar, p.yerrlow, p.yerrhigh,
										  p.variable_color, p.variable_size, p.variable_fillcolor);
			//dataでない場合、コンパイル時にrangesが空になってエラーになりうる。
			//ので、空tupleだったら何もしない。
			if constexpr (std::tuple_size_v<decltype(ranges)> != 0)
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

		for (auto&& v : p.data)
		{
			int64_t i = ibin(v);
			if (i < 0 || std::cmp_greater_equal(i, p.xnbin)) continue;
			++hist[i];
		}

		std::vector<double> x(p.xnbin);
		for (size_t i = 0; i < p.xnbin; ++i) x[i] = p.xmin + wbin * (i + 0.5);

		if constexpr (KeywordExists(plot::err_68, ops...))
		{
			std::vector<double> yerrlow(p.xnbin), yerrhigh(p.xnbin);
			for (size_t i = 0; i < p.xnbin; ++i)
			{
				auto [low, high] = GetPoissonCI68((uint64_t)hist[i]);
				yerrlow[i] = low;
				yerrhigh[i] = high;
			}
			auto p2 = MakePointParam(plot::x = x, plot::y = hist, ops..., plot::s_points, plot::xerrorbar = wbin / 2., plot::yerrlow = yerrlow, plot::yerrhigh = yerrhigh);
			return Plot(p2);
		}
		else
		{
			auto p2 = MakePointParam(plot::x = x, plot::y = hist, ops..., plot::s_histeps);
			return Plot(p2);
		}
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
										  std::array<std::string, 5>{ "x", "y", "xlen", "ylen", "variable_folor" },
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

			std::map<std::string, std::variant<int, std::string>> column;
			std::vector<std::string> labelcolumn;

			auto ranges =
				ArrangeColumnOption<0, 1>(column, labelcolumn, m_canvas,
										  std::array<std::string, 4>{ "x", "y", "ybelow", "variable_fillcolor" },
										  std::array<std::string_view, 4>{ x_x2, y_y2, y_y2, ""},
										  p.x, p.y, p.ybelow, p.variable_fillcolor);
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
			AddColumn(p.ybelow, "y2", column);
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

	std::vector<std::string> m_commands;
	Canvas* m_canvas;
};

class Canvas2D : public Axis2D<Canvas>
{
public:

	using Axis2D<Canvas>::Axis2D;

	friend class MultiPlotter;

	template <acceptable_arg X, acceptable_arg Y,
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

	template <acceptable_arg X, acceptable_arg Y,
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

	template <acceptable_arg X, acceptable_arg Y,
			  acceptable_arg XL, acceptable_arg YL,
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

	template <acceptable_arg X, acceptable_arg Y,
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
	template <acceptable_arg X, acceptable_arg Y,
			  acceptable_arg Y2,
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

	template <acceptable_arg X, acceptable_arg Y, acceptable_arg Z,
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
	template <acceptable_arg X, acceptable_arg Y,
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

	template <acceptable_arg X, acceptable_arg Y, acceptable_arg Z,
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
	template <acceptable_arg X, acceptable_arg Y,
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

	template <acceptable_arg X, acceptable_arg Y, acceptable_arg Z,
			  acceptable_arg XL, acceptable_arg YL, acceptable_arg ZL,
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
	template <acceptable_arg X, acceptable_arg Y,
			  acceptable_arg XL, acceptable_arg YL,
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

	template <acceptable_matrix_range Map,
			  ranges::arithmetic_range XRange, ranges::arithmetic_range YRange,
			  colormap_option ...Options>
	PlotBufferCM PlotColormap(const Map& map_, const XRange& x, const YRange& y,
							  Options ...ops)
	{
		auto p = MakeColormapParam(plot::map = map_, plot::xrange = x, plot::yrange = y, ops...);
		return Plot(p);
	}
	template <acceptable_matrix_range Map, colormap_option ...Options>
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
				//std::vector<std::string> column{ "1", "2", "5" };
				std::map<std::string, std::variant<int, std::string>> column{ { "x", 1 }, { "y", 2 }, { "map", 5} };
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
	template <class X, class Y, class Z, class XE, class XEL, class XEH, class YE, class YEL, class YEH, class VC, class VS, class VFC>
	PlotBufferCM Plot(const PointParam3D<X, Y, Z, XE, XEL, XEH, YE, YEL, YEH, VC, VS, VFC>& p)
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
																			"variable_color", "variable_size", "variable_fillcolor" },
										  std::array<std::string_view, 12>{ x_x2, y_y2, z_z2, "", "", "", "", "", "", "", "", "" },
										  p.x, p.y, p.z, p.xerrorbar, p.xerrlow, p.xerrhigh, p.yerrorbar, p.yerrlow, p.yerrhigh,
										  p.variable_color, p.variable_size, p.variable_fillcolor);
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

	template <acceptable_arg X, acceptable_arg Y, acceptable_arg Z,
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
	template <acceptable_arg X, acceptable_arg Y,
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

	template <acceptable_arg X, acceptable_arg Y, acceptable_arg Z,
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
	template <acceptable_arg X, acceptable_arg Y,
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

	template <acceptable_arg X, acceptable_arg Y, acceptable_arg Z,
			  acceptable_arg XL, acceptable_arg YL, acceptable_arg ZL,
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
	template <acceptable_arg X, acceptable_arg Y,
			  acceptable_arg XL, acceptable_arg YL,
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

	template <acceptable_matrix_range Map, ranges::arithmetic_range X, ranges::arithmetic_range Y,
			  colormap_option ...Options>
	PlotBufferCM PlotColormap(const Map& map, const X& x, const Y& y,
							  Options ...ops)
	{
		PlotBufferCM p(this);
		return p.PlotColormap(map, x, y, ops...);
	}
	template <acceptable_matrix_range Map, colormap_option ...Options>
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
