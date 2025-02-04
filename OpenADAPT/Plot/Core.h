#ifndef ADAPT_PLOT_CORE_H
#define ADAPT_PLOT_CORE_H

#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <string>
#include <string_view>
#include <cfloat>
#include <OpenADAPT/Utility/Macros.h>
#include <OpenADAPT/Utility/Print.h>
#include <OpenADAPT/Utility/Verbose.h>

namespace adapt
{

enum class Style { none, lines, points, linespoints, dots, impulses, boxes, steps, fsteps, histeps, };
enum class Smooth { none, unique, frequency, cumulative, cnormal, kdensity, csplines, acsplines, bezier, sbezier, };
enum class ArrowHead { none, head, heads, nohead, };
enum class ArrowFill { none, filled, empty, nofilled, noborder, };

enum class Contour { none, base, surface, both, };
enum class CntrSmooth { none, linear, cubicspline, bspline };

class Canvas
{
public:

	static FILE* OpenGnuplot(const std::string& path)
	{
#ifdef _MSC_VER
		return _popen(path.c_str(), "w");
#else
		return popen(path.c_str(), "w");
#endif
	}
	static void CloseGnuplot(FILE* pipe)
	{
		if (pipe != nullptr)
		{
			adapt::Print(pipe, "exit");
#ifdef _MSC_VER
			_pclose(pipe);
#else
			pclose(pipe);
#endif
		}
	}

	friend class MultiPlot;

	Canvas(std::string_view output, double sizex = 0., double sizey = 0.)
	{
		Open();
		SetOutput(output, sizex, sizey);
	}
	Canvas(double sizex = 0., double sizey = 0.)
		: Canvas(ms_default_gnuplot_terminal, sizex, sizey)
	{}
	Canvas(const Canvas&) = delete;
	Canvas(Canvas&&) = delete;
	Canvas& operator=(const Canvas&) = delete;
	Canvas& operator=(Canvas&&) = delete;
	virtual ~Canvas()
	{
		Close();
	}

	void SetLabel(std::string_view axis, std::string_view label)
	{
		Command(std::format("set {}label '{}'", axis, label));
	}
	void SetRange(std::string_view axis, double min, double max)
	{
		Command(std::format("set {}range [{}:{}]", axis, min, max));
	}
	void SetRangeMin(std::string_view axis, double min)
	{
		Command(std::format("set {}range [{}:]", axis, min));
	}
	void SetRangeMax(std::string_view axis, double max)
	{
		Command(std::format("set {}range [:{}]", axis, max));
	}
	void SetLog(std::string_view axis, double base = 10)
	{
		Command(std::format("set logscale {} {}", axis, base));
	}
	void SetFormat(std::string_view axis, std::string_view fmt)
	{
		Command(std::format("set format {} \"{}\"", axis, fmt));
	}
	void SetDataTime(std::string_view axis, std::string_view fmt = std::string())
	{
		m_date_time_axes.insert(std::string(axis));
		Command(std::format("set {}data time", axis));
		if (!fmt.empty()) Command(std::format("set timefmt \"{}\"", fmt));
	}
	bool IsDateTimeEnabled(std::string_view axis)
	{
		return m_date_time_axes.find(std::string(axis)) != m_date_time_axes.end();
	}

	//見出しを数字から与えられた文字列に置き換える。
	template <class ...Args>
	void SetTics(std::string_view axis, Args&& ...args)
	{
		std::string c = "(";
		SetTics_make(c, std::forward<Args>(args)...);
		Command(std::format("set {} tics {}", axis, c));
	}
private:
	template <class ...Args>
	void SetTics_make(std::string& tics, std::string_view label, double value, Args&& ...args)
	{
		tics += std::format("'{}' {}, ", label, value);
		SetTics_make(tics, std::forward<Args>(args)...);
	}
	template <class ...Args>
	void SetTics_make(std::string& tics, std::string_view label, double value, int level, Args&& ...args)
	{
		tics += std::format("'{}' {} {}, ", label, value, level);
		SetTics_make(tics, std::forward<Args>(args)...);
	}
	void SetTics_make(std::string& tics)
	{
		tics.erase(tics.end() - 2, tics.end());
		tics += ")";
		//途中。
	}
public:

	void SetTics(double interval);
	void SetTics(double begin, double end, double interval);
	void SetMTics(double interval);
	void SetMTics(double begin, double end, double interval);

	void SetTicsRotate(std::string_view axis, double ang)
	{
		Command(std::format("set {}tics rotate by {}", axis, ang));
	}

	void SetGrid(std::string_view color = "", int type = -2, int width = -1)
	{
		std::string c;
		if (!color.empty()) c += std::format(" linecolor rgb \"{}\"", color);
		if (type != -2) c += std::format(" linetype {}", type);
		if (width != -1) c += std::format(" linewidth {}", width);
		Command("set grid" + c);
	}

	void SetSize(double x, double y) { Command(std::format("set size {}, {}", x, y)); }
	//-1を与えるとticsの幅が等しくなる。
	void SetSizeRatio(double ratio) { Command(std::format("set size ratio {}", ratio)); }

	void SetPaletteDefined(const std::vector<std::pair<double, std::string>>& color)
	{
		std::string x;
		for (const auto& c : color)
		{
			x += (std::to_string(c.first) + " \"" + c.second + "\", ");
		}
		x.erase(x.end() - 2, x.end());
		Command("set palette defined (" + x + ")");
	}
	void SetPaletteRGBFormulae(int x, int y, int z)
	{
		Command(std::format("set palette rgbformulae {}, {}, {}", x, y, z));
	}
	void SetPaletteCubehelix(double start, double cycles, double saturation)
	{
		Command(std::format("set palette cubehelix start {}, cycles {}, saturation {}", start, cycles, saturation));
	}
	void SetPaletteMaxcolors(int num)
	{
		Command(std::format("set palette maxcolors {}", num));
	}
	//void SetKeyOff();
	//void SetKey(std::string_view posx, std::string_view posy);//"left", "right", "top", "bottom", "outside", "below"をxyそれぞれに。

	void SetTitle(std::string_view title) { Command(std::format("set title '{}'", title)); }

	void SetKeyTopLeft(bool outside = false) { Command("set key", outside ? "outside" : "", "top left"); }
	void SetKeyTopCenter(bool outside = false) { Command("set key", outside ? "outside" : "", "top center"); }
	void SetKeyTopRight(bool outside = false) { Command("set key", outside ? "outside" : "", "top right"); }
	void SetKeyCenterLeft(bool outside = false) { Command("set key", outside ? "outside" : "", "center left"); }
	void SetKeyCenter(bool outside = false) { Command("set key", outside ? "outside" : "", "center"); }
	void SetKeyCenterRight(bool outside = false) { Command("set key", outside ? "outside" : "", "center right"); }
	void SetKeyBottomLeft(bool outside = false) { Command("set key", outside ? "outside" : "", "bottom left"); }
	void SetKeyBottomCenter(bool outside = false) { Command("set key", outside ? "outside" : "", "bottom center"); }
	void SetKeyBottomRight(bool outside = false) { Command("set key", outside ? "outside" : "", "bottom right"); }

	void SetKeyOpaque(bool b = true) { Command("set key", b ? "opaque" : "noopaque"); }

	void SetKeyBox(bool b = true) { Command("set key", b ? "box" : "nobox"); }

	void SetParametric() { Command("set parametric"); }

	void SetLeftMargin(double w) { Command(std::format("set lmargin {}", w)); }
	void SetRightMargin(double w) { Command(std::format("set rmargin {}", w)); }
	void SetTopMargin(double w) { Command(std::format("set tmargin {}", w)); }
	void SetBottomMargin(double w) { Command(std::format("set bmargin {}", w)); }

	void SetMargins(double l, double r, double b, double t)
	{
		SetLeftMargin(l);
		SetRightMargin(r);
		SetBottomMargin(b);
		SetTopMargin(t);
	}

	void SetOutput(std::string_view output, double sizex, double sizey)
	{
		m_output = output;
		if (output.size() > 4)
		{
			std::string_view extension(output.substr(output.size() - 4, 4));
			std::string repout = ReplaceStr(output, "\\", "/");
			if (extension == ".png")
			{
				if (sizex == 0 && sizey == 0) sizex = 720, sizey = 540;
				Command(std::format("set terminal pngcairo enhanced size {}, {} font \"Arial\"\nset output '{}'", sizex, sizey, repout));
			}
			else if (extension == ".eps")
			{
				if (sizex == 0 && sizey == 0) sizex = 6, sizey = 4.5;
				Command(std::format("set terminal epscairo enhanced size {}in, {}in font \"Arial\"\nset output '{}'", sizex, sizey, repout));
			}
			else if (extension == ".pdf")
			{
				if (sizex == 0 && sizey == 0) sizex = 6, sizey = 4.5;
				Command(std::format("set terminal pdfcairo enhanced size {}in, {}in font \"Arial\"\nset output '{}'", sizex, sizey, repout));
			}
		}
		else if (output == "qt")
		{
			if (sizex == 0 && sizey == 0) sizex = 720, sizey = 540;
			Command(std::format("set terminal qt size {}, {} font \"Arial\" enhanced", sizex, sizey));
		}
		else if (output == "wxt")
		{
			if (sizex == 0 && sizey == 0) sizex = 720, sizey = 540;
			Command(std::format("set terminal wxt enhanced size {}, {} font \"Arial\"", sizex, sizey));
		}
		else std::cout << "WARNING : " << output << " is not a terminal or has no valid extension. Default terminal is selected." << std::endl;

	}
	void Reset() { Command("reset"); }
	const std::string& GetOutput() const { return m_output; }

	void Open()
	{
		if (m_pipe != nullptr)
		{
			PrintWarning("Gnuplot has already been open. {}", GetGnuplotPath());
			return;
		}
		if (ms_global_pipe != nullptr)
			m_pipe = ms_global_pipe;
		else
		{
			if ((m_pipe = OpenGnuplot(GetGnuplotPath())) == nullptr)
				PrintError("Gnuplot cannot open. {}", GetGnuplotPath());
		}
		if (m_pipe)
		{
			Command("set palette defined ( 0 '#000090',1 '#000fff',2 "
					"'#0090ff',3 '#0fffee',4 '#90ff70',5 '#ffee00',6 "
					"'#ff7000',7 '#ee0000',8 '#7f0000')");
		}
	}
	void Open(std::string_view output, double sizex = 0., double sizey = 0.)
	{
		Open();
		SetOutput(output, sizex, sizey);
	}
	void Close()
	{
		if (m_pipe != nullptr && m_pipe != ms_global_pipe)
		{
			Command("exit");
			CloseGnuplot(m_pipe);
		}
		m_pipe = nullptr;
	}

	template <class ...Args>
	void Command(Args&& ...args)
	{
		adapt::Print(m_pipe, std::forward<Args>(args)...);
		if (m_show_commands) adapt::Print(std::cout, std::forward<Args>(args)...);
	}
	void ShowCommands(bool b) { m_show_commands = b; }

	FILE* GetPipe() const { return m_pipe; }

	// Enable or disable datablock feature of Gnuplot
	// If disabled, temporary files are created to pass data to Gnuplot.
	void EnableInMemoryDataTransfer(bool b)
	{
		m_in_memory_data_transfer = b;
	}
	bool IsInMemoryDataTransferEnabled()
	{
		return m_in_memory_data_transfer;
	}

	static void SetGnuplotPath(std::string_view path)
	{
		ms_gnuplot_path = path;
	}
	static std::string GetGnuplotPath()
	{
		if (!ms_gnuplot_path.empty()) return ms_gnuplot_path;
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4996)
#endif 
		if (const char* p = std::getenv("GNUPLOT_PATH")) return std::string(p);
#ifdef _MSC_VER
#pragma warning(pop)
#endif
		return ms_default_gnuplot_path;
	}

protected:

	std::string m_output;
	FILE* m_pipe = nullptr;
	bool m_show_commands = false;
	bool m_in_memory_data_transfer = true; // Use datablock feature of Gnuplot if true (default: false)

	//When any axes (x, y, x2, y2, z) are contained within this variable, 
	//the values of them are treated as DateTime even if the type of values are std::string or a type that is convertible to std::string.
	std::set<std::string> m_date_time_axes;
	//template <class = void>
	//struct Paths
	//{
	//	static const std::string ms_default_gnuplot_terminal;
	//	static std::string ms_gnuplot_path;
	//	static const std::string ms_default_gnuplot_path;
	//	static FILE* ms_global_pipe;//multiplotなどを利用する際のグローバルなパイプ。これがnullptrでない場合、mPipe==mGlobalPipeとなる。
	//};
	inline static const std::string ms_default_gnuplot_terminal = "wxt";
	inline static std::string ms_gnuplot_path = "";
#ifdef _WIN32
	inline static const std::string ms_default_gnuplot_path = "C:/Progra~1/gnuplot/bin/gnuplot.exe";
#else
	inline static const std::string ms_default_gnuplot_path = "gnuplot";
#endif
	inline static FILE* ms_global_pipe = nullptr;

};



class MultiPlot
{
public:

	MultiPlot() {}
	MultiPlot(std::string_view outputname, int row, int column, double sizex = 0., double sizey = 0.) { Begin(outputname, row, column, sizex, sizey); }
	~MultiPlot() { End(); }

	void Begin(std::string_view output, int row, int column, double sizex = 0., double sizey = 0.)
	{
		if (Canvas::ms_global_pipe != nullptr)
		{
			std::cerr << "Gnuplot has already been open. " << Canvas::GetGnuplotPath() << std::endl;
			return;
		}
		if ((Canvas::ms_global_pipe = Canvas::OpenGnuplot(Canvas::GetGnuplotPath())) == nullptr)
		{
			std::cerr << "Gnuplot cannot open. " << Canvas::GetGnuplotPath() << std::endl;
		}
		else
		{
			//Command("set bars small");
			Command("set palette defined ( 0 '#000090',1 '#000fff',2 '#0090ff',3 '#0fffee',4 '#90ff70',5 '#ffee00',6 '#ff7000',7 '#ee0000',8 '#7f0000')");

			if (output.size() > 4)
			{
				std::string_view extension = output.substr(output.size() - 4, 4);
				std::string repout = ReplaceStr(output, "\\", "/");
				if (extension == ".png")
				{
					if (sizex == 0 && sizey == 0) sizex = 720 * column, sizey = 540 * row;
					Command(std::format("set terminal pngcairo enhanced size {}, {} font \"Arial\"\nset output '{}'", (int)sizex, (int)sizey, repout));
				}
				else if (extension == ".eps")
				{
					if (sizex == 0 && sizey == 0) sizex = 6 * column, sizey = 4.5 * row;
					Command(std::format("set terminal epscairo enhanced size {}in, {}in font \"Arial\"\nset output '{}'", sizex, sizey, repout));
				}
				else if (extension == ".pdf")
				{
					if (sizex == 0 && sizey == 0) sizex = 6 * column, sizey = 4.5 * row;
					Command(std::format("set terminal pdfcairo enhanced size {}in, {}in font \"Arial\"\nset output '{}'", sizex, sizey, repout));
				}
			}
			else if (output == "qt")
			{
				if (sizex == 0 && sizey == 0) sizex = 720 * column, sizey = 540 * row;
				Command(std::format("set terminal qt size {}, {} font \"Arial\" enhanced", sizex, sizey));
			}
			else if (output == "wxt")
			{
				if (sizex == 0 && sizey == 0) sizex = 720 * column, sizey = 540 * row;
				Command(std::format("set terminal wxt size {}, {} enhanced font \"Arial\"", sizex, sizey));
			}
			else std::cout << "WARNING : " << output << " is not a terminal or has no valid extension. Default terminal is selected." << std::endl;

			Command("set multiplot layout " + std::to_string(row) + ", " + std::to_string(column));
		}
	}
	void End()
	{
		if (Canvas::ms_global_pipe != nullptr)
		{
			Command("unset multiplot");
			Canvas::CloseGnuplot(Canvas::ms_global_pipe);
			Canvas::ms_global_pipe = nullptr;
		}
	}

	template <class ...Args>
	void Command(Args&& ...args)
	{
		adapt::Print(Canvas::ms_global_pipe, std::forward<Args>(args)...);
	}

private:
};

}

#endif