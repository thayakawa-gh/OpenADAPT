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
#include <OpenADAPT/Plot/Options.h>

namespace adapt
{

namespace plot_detail
{

inline const std::string g_default_gnuplot_terminal = "wxt";
inline std::string g_gnuplot_path = "";
#ifdef _WIN32
inline const std::string g_default_gnuplot_path = "C:/Progra~1/gnuplot/bin/gnuplot.exe";
#else
inline const std::string g_default_gnuplot_path = "gnuplot";
#endif

inline constexpr std::array<double, 2> g_default_canvas_size_px = { 1200., 900. };
inline constexpr std::array<double, 2> g_default_canvas_size_in = { 8., 6. };

inline constexpr double g_default_font_size = 19.0;
inline constexpr double g_default_line_width = 2.0;
inline constexpr double g_default_point_scale = 2.0;
inline const std::string g_default_font_name = "Arial";

template <class Command>
std::string SetTerminal(Command c, std::string_view output, double sizex, double sizey, int row, int column, double& size_ratio)
{
	auto command = [&](std::string_view ext, std::string_view unit, const std::array<double, 2>& defxy)
	{
		if (sizex == 0 && sizey == 0)
		{
			sizex = defxy[0] * column;
			sizey = defxy[1] * row;
		}
		else size_ratio = std::sqrt((sizex * sizey) / (defxy[0] * column * defxy[1] * row));
		std::string com;
		com += std::format("set terminal {} enhanced size {}{}, {}{} font \"{}, {}\"",
						   ext, sizex, unit, sizey, unit,
						   g_default_font_name, g_default_font_size * size_ratio);
		//pdfcairoのときlinewidthやpointscaleの挙動がおかしいので、指定しないことにする。
		//代わりにSetOutputの方でSetBorderWidthを呼ぶことで対応する。
		if (ext != "pdfcairo")
		{
			com += std::format(" linewidth {}", g_default_line_width * size_ratio);
			com += std::format(" pointscale {}", g_default_point_scale * size_ratio);
			com += std::format(" dashlength {}", size_ratio * 0.5);
		}
		c(com);
	};
	try
	{
		if (output.size() > 4)
		{
			std::string extension(output.substr(output.size() - 4, 4));
			std::string repout = ReplaceStr(output, "\\", "/");
			if (extension == ".png") command("pngcairo", "", g_default_canvas_size_px);
			//else if (extension == ".eps") command("epscairo", "in", g_default_canvas_size_in);
			else if (extension == ".pdf") command("pdfcairo", "in", g_default_canvas_size_in);
			else throw BadFile("");
			c(std::format("set output '{}'", repout));
			return extension;
		}
		else if (output == "qt") command("qt", "", g_default_canvas_size_px);
		else if (output == "wxt") command("wxt", "", g_default_canvas_size_px);
		else throw BadFile("");
		return std::string(output);
	}
	catch(const BadFile&)
	{
		//terminalの指定に失敗した場合は、デフォルトのターミナルを選択する。
		std::cout << "WARNING : " << output << " is not a terminal or has no valid extension. Default terminal is selected." << std::endl;
		command(plot_detail::g_default_gnuplot_terminal, "", plot_detail::g_default_canvas_size_px);
		return plot_detail::g_default_gnuplot_terminal;
	}
}

inline FILE* OpenGnuplot(const std::string& path)
{
	#ifdef _MSC_VER
	return _popen(path.c_str(), "w");
	#else
	return popen(path.c_str(), "w");
	#endif
}
inline void CloseGnuplot(FILE* pipe)
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

}

inline void SetGnuplotPath(std::string_view path)
{
	plot_detail::g_gnuplot_path = path;
}
inline std::string GetGnuplotPath()
{
	if (!plot_detail::g_gnuplot_path.empty()) return plot_detail::g_gnuplot_path;
	#ifdef _MSC_VER
	#pragma warning(push)
	#pragma warning(disable: 4996)
	#endif 
	if (const char* p = std::getenv("GNUPLOT_PATH")) return std::string(p);
	#ifdef _MSC_VER
	#pragma warning(pop)
	#endif
	return plot_detail::g_default_gnuplot_path;
}

ADAPT_EXPORT
class MultiPlot
{
public:

	MultiPlot() {}
	template <keyword_arg_tagged_with<plot_detail::MultiplotOption> ...Opts>
	MultiPlot(std::string_view outputname, int row, int column, Opts ...opts)
	{
		Begin(outputname, row, column, opts...);
	}
	~MultiPlot() { End(); }

	static bool IsOpen()
	{
		return ms_global_pipe != nullptr;
	}
	static std::string GetNextOutput() { return std::format("{}_{}", ms_output, ms_count++); }
	static std::string GetExtension() { return ms_extension; }

	static FILE* GetPipe() { return ms_global_pipe; }
	static double GetSizeRatio() { return ms_size_ratio; }

	template <keyword_arg_tagged_with<plot_detail::MultiplotOption> ...Opts>
	static void Begin(std::string_view output, int row, int column, Opts ...opts)
	{
		if (IsOpen())
		{
			std::cerr << "Gnuplot has already been open. " << GetGnuplotPath() << std::endl;
			return;
		}
		if ((ms_global_pipe = plot_detail::OpenGnuplot(GetGnuplotPath())) == nullptr)
		{
			std::cerr << "Gnuplot cannot open. " << GetGnuplotPath() << std::endl;
		}
		else
		{
			double sizex = 0.;
			double sizey = 0.;
			if constexpr (KeywordExists(plot::multiplot_size, opts...))
			{
				auto s = GetKeywordArg(plot::multiplot_size, opts...);
				sizex = s.first;
				sizey = s.second;
			}
			ms_output = output;
			//Command("set bars small");
			Command("set palette defined ( 0 '#000090',1 '#000fff',2 '#0090ff',3 '#0fffee',4 '#90ff70',5 '#ffee00',6 '#ff7000',7 '#ee0000',8 '#7f0000')");
			ms_extension = plot_detail::SetTerminal([]<class ...Args>(Args&& ...args) { MultiPlot::Command(std::forward<Args>(args)...); },
													output, sizex, sizey, row, column, ms_size_ratio);
			std::string com = std::format("set multiplot layout {}, {}", row, column);
			if constexpr (KeywordExists(plot::page_title, opts...))
				com += std::format(" title '{}' font \"{},{:>.1f}\"",
								   GetKeywordArg(plot::page_title, opts...),
								   plot_detail::g_default_font_name, plot_detail::g_default_font_size * ms_size_ratio * 1.4);
			if constexpr (KeywordExists(plot::fillorder, opts...))
			{
				MPFillOrder fillorder = GetKeywordArg(plot::fillorder, opts...);
				if (fillorder == MPFillOrder::rowfirst) com += " rowfirst";
				else com += " columnfirst";
			}
			if constexpr (KeywordExists(plot::vertical_direction, opts...))
			{
				MPVerticalDirection vertical_direction = GetKeywordArg(plot::vertical_direction, opts...);
				if (vertical_direction == MPVerticalDirection::topdown) com += " vertical";
				else com += " horizontal";
			}
			//Command("set multiplot layout " + std::to_string(row) + ", " + std::to_string(column));
			//Command(std::format("set title font \"{},{:>.1f}", plot_detail::g_default_font_name, plot_detail::g_default_font_size * ms_size_ratio * 1.1));
			Command(com);
		}
	}
	static void End()
	{
		if (ms_global_pipe != nullptr)
		{
			Command("unset multiplot");
			plot_detail::CloseGnuplot(ms_global_pipe);
			ms_global_pipe = nullptr;
		}
		ms_output.clear();
		ms_extension.clear();
		ms_count = 0;
		ms_size_ratio = 1.0;
	}

	template <class ...Args>
	static void Command(Args&& ...args)
	{
		adapt::Print(MultiPlot::ms_global_pipe, std::forward<Args>(args)...);
	}

private:
	inline static std::string ms_output = {};
	inline static std::string ms_extension = {};//gnuplotの出力形式を表す文字列。例えば、".png"や".pdf"、"qt"など。
	inline static size_t ms_count = 0;
	inline static FILE* ms_global_pipe = nullptr;
	inline static double ms_size_ratio = 1.0;
};

namespace plot_detail
{

template <class ...Axes>
class Canvas : public Axes...
{
public:

	friend class MultiPlot;

	Canvas(std::string_view output, double sizex = 0., double sizey = 0.)
	{
		Open();
		SetOutput(output, sizex, sizey);
	}
	Canvas(double sizex = 0., double sizey = 0.)
		: Canvas(g_default_gnuplot_terminal, sizex, sizey)
	{
	}
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

	void SetGrid(std::string_view color = "", int type = 1, double width = -1, std::string_view dashtype = "")
	{
		//dashtypeはtypeを1以上の有効値にしないと機能しないらしい。
		std::string c;
		if (!color.empty()) c += std::format(" linecolor rgb \"{}\"", color);
		if (type != -2) c += std::format(" linetype {}", type);
		if (width != -1) c += std::format(" linewidth {}", width);
		if (!dashtype.empty()) c += std::format(" dashtype {}", dashtype);
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
	void SetKeyBoxWidth(double w) { Command(std::format("set key box linewidth {}", w)); }

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

	void SetBorderWidth(double width) { Command(std::format("set border linewidth {}", width)); }
	void SetLineWidth(double width) { Command(std::format("set style line linewidth {}", width)); }

	void SetFont(std::string_view name, std::string_view font, double size)
	{
		if (size == 0.0) size = g_default_font_size * m_size_ratio;
		Command(std::format("set {} font \"{}, {:>.1f}\"", name, font, size));
	}
	void SetTitleFont(std::string_view font, double size = 0.0) { SetFont("title", font, size); }
	void SetTicsFont(std::string_view font, double size = 0.0) { ((Axes::SetTicsFont(font, size), 0) + ...); }
	void SetLabelFont(std::string_view font, double size = 0.0) { ((Axes::SetLabelFont(font, size), 0) + ...); }
	void SetKeyFont(std::string_view font, double size = 0.0) { SetFont("key", font, size); }

	void SetOutput(std::string_view output, double sizex, double sizey)
	{
		std::string ext;
		if (MultiPlot::IsOpen())
		{
			if (output != g_default_gnuplot_terminal)
				PrintWarning("In MultiPlot mode, output setting of Canvas is ignored. Output is controlled by MultiPlot.");
			if (sizex != 0. || sizey != 0.)
				PrintWarning("In MultiPlot mode, size setting of Canvas is ignored. Size is controlled by MultiPlot.");
			m_output = MultiPlot::GetNextOutput();
			m_size_ratio = MultiPlot::GetSizeRatio();
			ext = MultiPlot::GetExtension();
		}
		else
		{
			m_output = output;
			ext = SetTerminal([this]<class ...Args>(Args&& ...args) { Command(std::forward<Args>(args)...); },
							  output, sizex, sizey, 1, 1, m_size_ratio);
		}
		SetTitleFont(g_default_font_name, g_default_font_size * m_size_ratio * 1.3);
		SetLabelFont(g_default_font_name, g_default_font_size * m_size_ratio * 1.2);
		SetKeyFont(g_default_font_name, g_default_font_size * m_size_ratio);
		if (ext == ".pdf")
		{
			//pdfcairoのときlinewidthやpointscaleの挙動がおかしいので、terminal側では一切指定せず、
			//代わりにこちらで指定する。
			SetBorderWidth(g_default_line_width * m_size_ratio);
		}
		std::string dashtype = std::format("({},{})", 10 * m_size_ratio, 5 * m_size_ratio);
		SetGrid("light-gray", 1, g_default_line_width * m_size_ratio * 0.4, dashtype);
		Command("set tics front");
	}
	void Reset() { Command("reset"); }
	const std::string& GetOutput() const { return m_output; }

	void Open()
	{
		if (m_pipe != nullptr)
		{
			PrintWarning("Gnuplot has already been open. {}", adapt::GetGnuplotPath());
			return;
		}
		if (MultiPlot::IsOpen())
		{
			m_pipe = MultiPlot::GetPipe();
			return;
		}
		if ((m_pipe = OpenGnuplot(adapt::GetGnuplotPath())) == nullptr)
		{
			PrintError("Gnuplot cannot open. {}", adapt::GetGnuplotPath());
			return;
		}
		// デフォルト設定
		Command("set palette defined ( 0 '#000090',1 '#000fff',2 "
				"'#0090ff',3 '#0fffee',4 '#90ff70',5 '#ffee00',6 "
				"'#ff7000',7 '#ee0000',8 '#7f0000')");
	}
	void Open(std::string_view output, double sizex = 0., double sizey = 0.)
	{
		Open();
		SetOutput(output, sizex, sizey);
	}
	void Close()
	{
		if (m_pipe != nullptr && m_pipe != MultiPlot::GetPipe())
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

	[[deprecated("please use adapt::SetGnuplotPath.")]] static void SetGnuplotPath(std::string_view path)
	{
		adapt::SetGnuplotPath(path);
	}
	[[deprecated("please use adapt::GetGnuplotPath.")]] static std::string GetGnuplotPath()
	{
		return adapt::GetGnuplotPath();
	}

protected:

	std::string m_output;
	FILE* m_pipe = nullptr;
	bool m_show_commands = false;
	bool m_in_memory_data_transfer = true; // Use datablock feature of Gnuplot if true (default: true)

	//When any axes (x, y, x2, y2, z) are contained within this variable, 
	//the values of them are treated as DateTime even if the type of values are std::string or a type that is convertible to std::string.
	std::set<std::string> m_date_time_axes;

	//The ratio of font size / border width to default when the size of canvas is changed.
	//This is used to keep the font size and border width relative to the canvas size.
	double m_size_ratio = 1.0;
};

}

}

#endif