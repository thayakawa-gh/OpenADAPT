#ifndef ADAPT_PLOT_CORE_H
#define ADAPT_PLOT_CORE_H

#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <string>
#include <string_view>
#include <cfloat>
#include <OpenADAPT/Utility/Print.h>

namespace adapt
{

enum class Style { none, lines, points, linespoints, dots, impulses, boxes, steps, fsteps, histeps, };
enum class Smooth { none, unique, frequency, cumulative, cnormal, kdensity, csplines, acsplines, bezier, sbezier, };
enum class ArrowHead { head = 0, heads = 1, noheads = 2, filled = 0 << 2, empty = 1 << 2, nofilled = 2 << 2, };

class Canvas
{
public:

	friend class MultiPlot;

	Canvas(std::string_view output, double sizex = 0., double sizey = 0.);
	Canvas(double sizex = 0., double sizey = 0.);
	Canvas(const Canvas&) = delete;
	Canvas(Canvas&&) = delete;
	Canvas& operator=(const Canvas&) = delete;
	Canvas& operator=(Canvas&&) = delete;
	virtual ~Canvas();

	void SetLabel(std::string_view axis, std::string_view label);
	void SetRange(std::string_view axis, double min, double max);
	void SetRangeMin(std::string_view axis, double min);
	void SetRangeMax(std::string_view axis, double max);
	void SetLog(std::string_view axis, double base = 10);
	void SetFormat(std::string_view axis, std::string_view fmt);
	void SetDataTime(std::string_view axis, std::string_view fmt = std::string());
	bool IsDateTimeEnabled(std::string_view axis);

	//見出しを数字から与えられた文字列に置き換える。
	template <class ...Args>
	void SetTics(std::string_view axis, Args&& ...args);
private:
	template <class ...Args>
	void SetTics_make(std::string& tics, std::string_view label, double value, Args&& ...args);
	template <class ...Args>
	void SetTics_make(std::string& tics, std::string_view label, double value, int level, Args&& ...args);
	void SetTics_make(std::string& tics);
public:

	void SetTics(double interval);
	void SetTics(double begin, double end, double interval);
	void SetMTics(double interval);
	void SetMTics(double begin, double end, double interval);

	void SetTicsRotate(std::string_view axis, double ang);

	void SetGrid(std::string_view color = "", int type = -2, int width = -1);

	void SetSize(double x, double y);
	void SetSizeRatio(double ratio);//-1を与えるとticsの幅が等しくなる。

	void SetPaletteDefined(const std::vector<std::pair<double, std::string>>& color);
	void SetPaletteRGBFormulae(int x, int y, int z);
	void SetPaletteCubehelix(double start, double cycles, double saturation);
	void SetPaletteMaxcolors(int num);
	//void SetKeyOff();
	//void SetKey(std::string_view posx, std::string_view posy);//"left", "right", "top", "bottom", "outside", "below"をxyそれぞれに。

	void SetTitle(std::string_view title);

	void SetKeyTopLeft(bool outside = false);
	void SetKeyTopCenter(bool outside = false);
	void SetKeyTopRight(bool outside = false);
	void SetKeyCenterLeft(bool outside = false);
	void SetKeyCenter(bool outside = false);
	void SetKeyCenterRight(bool outside = false);
	void SetKeyBottomLeft(bool outside = false);
	void SetKeyBottomCenter(bool outside = false);
	void SetKeyBottomRight(bool outside = false);

	void SetKeyOpaque(bool b = true);

	void SetKeyBox(bool b = true);

	void SetParametric();

	void SetLeftMargin(double w);
	void SetRightMargin(double w);
	void SetTopMargin(double w);
	void SetBottomMargin(double w);

	void SetMargins(double l, double r, double b, double t);

	void SetOutput(std::string_view output, double sizex, double sizey);
	void Reset();
	const std::string& GetOutput() const;

	template <class ...Args>
	void Command(Args&& ...args);
	void ShowCommands(bool b);

	FILE* GetPipe() const { return m_pipe; }

	// Enable or disable datablock feature of Gnuplot
	// If disabled, temporary files are created to pass data to Gnuplot.
	void EnableInMemoryDataTransfer(bool b);
	bool IsInMemoryDataTransferEnabled();

	static void SetGnuplotPath(std::string_view path);
	static std::string GetGnuplotPath();

protected:

	std::string m_output;
	FILE* m_pipe;
	bool m_show_commands;
	bool m_in_memory_data_transfer; // Use datablock feature of Gnuplot if true (default: false)

	//When any axes (x, y, x2, y2, z) are contained within this variable, 
	//the values of them are treated as DateTime even if the type of values are std::string or a type that is convertible to std::string.
	std::set<std::string> m_date_time_axes;
	template <class = void>
	struct Paths
	{
		static const std::string ms_default_gnuplot_terminal;
		static std::string ms_gnuplot_path;
		static const std::string ms_default_gnuplot_path;
		static FILE* ms_global_pipe;//multiplotなどを利用する際のグローバルなパイプ。これがnullptrでない場合、mPipe==mGlobalPipeとなる。
	};
};


inline Canvas::Canvas(std::string_view output, double sizex, double sizey)
	: m_output(output), m_pipe(nullptr), m_show_commands(false), m_in_memory_data_transfer(true)
{
	if (Paths<>::ms_global_pipe != nullptr) m_pipe = Paths<>::ms_global_pipe;
	else
	{
#if defined(_WIN32)
		if ((m_pipe = _popen(GetGnuplotPath().c_str(), "w")) == nullptr)
#elif defined(__linux__) || defined(__APPLE__)
		if ((mPipe = popen(GetGnuplotPath().c_str(), "w")) == nullptr)
#endif
		{
			std::cerr << "Gnuplot cannot open. " << GetGnuplotPath() << std::endl;
		}
		else
		{
			SetOutput(output, sizex, sizey);
		}
	}
	if (m_pipe)
	{
		//Command("set bars small");
		//Command("set key box");
		Command("set palette defined ( 0 '#000090',1 '#000fff',2 '#0090ff',3 '#0fffee',4 '#90ff70',5 '#ffee00',6 '#ff7000',7 '#ee0000',8 '#7f0000')");
	}
}
inline Canvas::Canvas(double sizex, double sizey)
	: Canvas(Paths<>::ms_default_gnuplot_terminal, sizex, sizey)
{}
inline Canvas::~Canvas()
{
	if (m_pipe != nullptr && m_pipe != Paths<>::ms_global_pipe)
	{
		Command("exit");
#if defined(_WIN32)
		_pclose(m_pipe);
#elif defined(__linux__) || defined(__APPLE__)
		pclose(m_pipe);
#endif
	}
	m_pipe = nullptr;
}

inline void Canvas::SetLabel(std::string_view axis, std::string_view label)
{
	Command(std::format("set {}label '{}'", axis, label));
}
inline void Canvas::SetRange(std::string_view axis, double min, double max)
{
	Command(std::format("set {}range [{}:{}]", axis, min, max));
}
inline void Canvas::SetRangeMin(std::string_view axis, double min)
{
	Command(std::format("set {}range [{}:]", axis, min));
}
inline void Canvas::SetRangeMax(std::string_view axis, double max)
{
	Command(std::format("set {}range [:{}]", axis, max));
}
inline void Canvas::SetLog(std::string_view axis, double base)
{
	Command(std::format("set logscale {} {}", axis, base));
}
inline void Canvas::SetFormat(std::string_view axis, std::string_view fmt)
{
	Command(std::format("set format {} \"{}\"", axis, fmt));
}
inline void Canvas::SetDataTime(std::string_view axis, std::string_view fmt)
{
	m_date_time_axes.insert(std::string(axis));
	Command(std::format("set {}data time", axis));
	if (!fmt.empty()) Command(std::format("set timefmt \"{}\"", fmt));
}
inline bool Canvas::IsDateTimeEnabled(std::string_view axis)
{
	return m_date_time_axes.find(std::string(axis)) != m_date_time_axes.end();
}

inline void Canvas::SetTics_make(std::string& tics)
{
	tics.erase(tics.end() - 2, tics.end());
	tics += ")";
	//途中。
}
inline void Canvas::SetTicsRotate(std::string_view axis, double ang)
{
	Command(std::format("set {}tics rotate by {}", axis, ang));
}

inline void Canvas::SetGrid(std::string_view color, int type, int width)
{
	std::string c;
	if (!color.empty()) c += std::format(" linecolor rgb \"{}\"", color);
	if (type != -2) c += std::format(" linetype {}", type);
	if (width != -1) c += std::format(" linewidth {}", width);
	Command("set grid" + c);
}

inline void Canvas::SetSize(double x, double y)
{
	Command(std::format("set size {}, {}", x, y));
}
inline void Canvas::SetSizeRatio(double ratio)
{
	Command(std::format("set size ratio {}", ratio));
}
inline void Canvas::SetPaletteDefined(const std::vector<std::pair<double, std::string>>& color)
{
	std::string x;
	for (const auto& c : color)
	{
		x += (std::to_string(c.first) + " \"" + c.second + "\", ");
	}
	x.erase(x.end() - 2, x.end());
	Command("set palette defined (" + x + ")");
}
inline void Canvas::SetPaletteRGBFormulae(int x, int y, int z)
{
	Command(std::format("set palette rgbformulae {}, {}, {}", x, y, z));
}
inline void Canvas::SetPaletteCubehelix(double start, double cycles, double saturation)
{
	Command(std::format("set palette cubehelix start {}, cycles {}, saturation {}", start, cycles, saturation));
}
inline void Canvas::SetPaletteMaxcolors(int num)
{
	Command(std::format("set palette naxcolors {}", num));
}
inline void Canvas::SetTitle(std::string_view title)
{
	Command(std::format("set title '{}'", title));
}

inline void Canvas::SetKeyTopLeft(bool outside) { Command("set key", outside ? "outside" : "", "top left"); }
inline void Canvas::SetKeyTopCenter(bool outside) { Command("set key", outside ? "outside" : "", "top center"); }
inline void Canvas::SetKeyTopRight(bool outside) { Command("set key", outside ? "outside" : "", "top right"); }
inline void Canvas::SetKeyCenterLeft(bool outside) { Command("set key", outside ? "outside" : "", "center left"); }
inline void Canvas::SetKeyCenter(bool outside) { Command("set key", outside ? "outside" : "", "center"); }
inline void Canvas::SetKeyCenterRight(bool outside) { Command("set key", outside ? "outside" : "", "center right"); }
inline void Canvas::SetKeyBottomLeft(bool outside) { Command("set key", outside ? "outside" : "", "bottom left"); }
inline void Canvas::SetKeyBottomCenter(bool outside) { Command("set key", outside ? "outside" : "", "bottom center"); }
inline void Canvas::SetKeyBottomRight(bool outside) { Command("set key", outside ? "outside" : "", "bottom right"); }

inline void Canvas::SetKeyOpaque(bool b) { Command("set key", b ? "opaque" : "noopaque"); }

inline void Canvas::SetKeyBox(bool b) { Command("set key", b ? "box" : "nobox"); }

inline void Canvas::SetParametric()
{
	Command("set parametric");
}

inline void Canvas::SetLeftMargin(double w)
{
	Command(std::format("set lmargin {}", w));
}
inline void Canvas::SetRightMargin(double w)
{
	Command(std::format("set rmargin {}", w));
}
inline void Canvas::SetBottomMargin(double w)
{
	Command(std::format("set bmargin {}", w));
}
inline void Canvas::SetTopMargin(double w)
{
	Command(std::format("set tmargin {}", w));
}

inline void Canvas::SetMargins(double l, double r, double b, double t)
{
	SetLeftMargin(l);
	SetRightMargin(r);
	SetBottomMargin(b);
	SetTopMargin(t);
}

inline void Canvas::SetOutput(std::string_view output, double sizex, double sizey)
{
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
inline void Canvas::Reset()
{
	Command("reset");
}
inline const std::string& Canvas::GetOutput() const
{
	return m_output;
}

template <class ...Args>
inline void Canvas::Command(Args&& ...args)
{
	adapt::Print(m_pipe, std::forward<Args>(args)...);
	if (m_show_commands) adapt::Print(std::cout, std::forward<Args>(args)...);
}
inline void Canvas::ShowCommands(bool b)
{
	m_show_commands = b;
}

inline void Canvas::EnableInMemoryDataTransfer(bool b)
{
	m_in_memory_data_transfer = b;
}

inline bool Canvas::IsInMemoryDataTransferEnabled()
{
	return m_in_memory_data_transfer;
}

inline void Canvas::SetGnuplotPath(std::string_view path)
{
	Paths<>::ms_gnuplot_path = path;
}
inline std::string Canvas::GetGnuplotPath()
{
	if (!Paths<>::ms_gnuplot_path.empty()) return Paths<>::ms_gnuplot_path;
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4996)
#endif 
	if (const char* p = std::getenv("GNUPLOT_PATH")) return std::string(p);
#ifdef _MSC_VER
#pragma warning(pop)
#endif
	return Paths<>::ms_default_gnuplot_path;
}
template <class T>
const std::string Canvas::Paths<T>::ms_default_gnuplot_terminal = "wxt";
template <class T>
std::string Canvas::Paths<T>::ms_gnuplot_path = "";
#ifdef _WIN32
template <class T>
const std::string Canvas::Paths<T>::ms_default_gnuplot_path = "C:/Progra~1/gnuplot/bin/gnuplot.exe";
#else
template <class T>
const std::string GPMCanvas::Paths<T>::ms_default_gnuplot_path = "gnuplot";
#endif
template <class T>
FILE* Canvas::Paths<T>::ms_global_pipe = nullptr;

template <class ...Args>
inline void Canvas::SetTics(std::string_view axis, Args&& ...args)
{
	std::string c = "(";
	SetTics_make(c, std::forward<Args>(args)...);
	Command(std::format("set {} tics {}", axis, c));
}
template <class ...Args>
inline void Canvas::SetTics_make(std::string& tics, std::string_view label, double value, Args&& ...args)
{
	tics += std::format("'{}' {}, ", label, value);
	SetTics_make(tics, std::forward<Args>(args)...);
}
template <class ...Args>
inline void Canvas::SetTics_make(std::string& tics, std::string_view label, double value, int level, Args&& ...args)
{
	tics += std::format("'{}' {} {}, ", label, value, level);
	SetTics_make(tics, std::forward<Args>(args)...);
}

class MultiPlot
{
public:

	MultiPlot();
	MultiPlot(std::string_view outputname, int row, int column, double sizex = 0., double sizey = 0.);
	~MultiPlot();

	void Begin(std::string_view output, int row, int column, double sizex = 0., double sizey = 0.);
	void End();

	template <class ...Args>
	void Command(Args&& ...args);
private:
};

inline MultiPlot::MultiPlot() {}
inline MultiPlot::MultiPlot(std::string_view output, int row, int column, double sizex, double sizey)
{
	Begin(output, row, column, sizex, sizey);
}
inline MultiPlot::~MultiPlot()
{
	End();
}
inline void MultiPlot::Begin(std::string_view output, int row, int column, double sizex, double sizey)
{
	if (Canvas::Paths<>::ms_global_pipe != nullptr)
	{
		std::cerr << "Gnuplot has already been open. " << Canvas::GetGnuplotPath() << std::endl;
		return;
	}
	if ((Canvas::Paths<>::ms_global_pipe = _popen(Canvas::GetGnuplotPath().c_str(), "w")) == nullptr)
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
inline void MultiPlot::End()
{
	if (Canvas::Paths<>::ms_global_pipe != nullptr)
	{
		Command("unset multiplot");
		_pclose(Canvas::Paths<>::ms_global_pipe);
		Canvas::Paths<>::ms_global_pipe = nullptr;
	}
}
template <class ...Args>
inline void MultiPlot::Command(Args&& ...args)
{
	adapt::Print(Canvas::Paths<>::ms_global_pipe, std::forward<Args>(args)...);
}

}

#endif