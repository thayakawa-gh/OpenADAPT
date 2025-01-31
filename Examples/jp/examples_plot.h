#ifndef EXAMPLES_PLOT_H
#define EXAMPLES_PLOT_H

#include <random>
#include <filesystem>
#include <OpenADAPT/ADAPT.h>

int example_2d(const std::string& output_filename, bool enable_in_memory_data_transfer)
{
	std::string norm = std::to_string(250. / std::sqrt(2 * 3.1415926535));
	std::string equation = norm + "*exp(-x*x/2)";

	std::mt19937_64 mt(0);
	std::normal_distribution<> nd(0., 1.);
	std::vector<double> x1(32, 0);
	std::vector<double> y1(32, 0);
	std::vector<double> e1(32);
	for (int i = 0; i < 1000; ++i)
	{
		double x = nd(mt);
		if (x < -4.0 || x >= 4.0) continue;
		++y1[static_cast<size_t>(std::floor(x / 0.25) + 16)];
	}
	for (int i = 0; i < 32; ++i)
	{
		x1[i] = i * 0.25 - 4. + 0.125;
		e1[i] = std::sqrt(y1[i]);
	}

	/* options for PlotPoints(...)
	title          ... title. If you specify "notitle", the title is not displayed.
	  no_title     ... the same as title = "notitle".
	axis           ... set of axes to scale lines. (e.g. plot::axis = "x1y2")
	  axis_x1y1    ... the same as axis = "x1y1"
	  axis_x1y2    ... the same as axis = "x1y2"
	color          ... uniform color.
	  c_black      ... the same as color = "black". Various color options are available, such as c_red, c_green, etc.
	variable_color ... different colors at each point.
	linetype       ... line type. see GNUPLOT's keyword "linetype"
	linewidth      ... uniform line width.
	  lw_mediun    ... the same as linewidth = 1.0. lw_thin, lw_thick, lw_ex_thick, etc. are also available.
	style          ... displaying style.
	  s_lines      ... the same as style = "lines". s_points, s_boxes, s_steps, etc. are also available.
	pointtype      ... each point is drawn as the symbol specified by this option.
	pointsize      ... uniform point size.
	  ps_medium    ... the same as pointsize = 1.0. ps_small, ps_large, ps_ex_large, etc. are also available.
	variable_size  ... different sizes at each point.
	smooth         ... interpolation and approximation by some routines.
	xerrorbar      ... xerrorbar. This is available only if you specify Style::lines, points or boxes option.
	yerrorbar      ... yerrorbar. This is available only if you specify Style::lines, points or boxes option.

	//The following options are available only with Style::boxes or Style::steps
	fillpattern    ... fill with a colored pattern.
	fillsolid      ... fill with a solid color specified by fillcolor with density [ 0.0, 1.0 ].
	filltransparent... make the filled area transparent to the background color.
	fillcolor      ... specify the color of filled area.
	variable_fillcolor ...
	bordercolor    ... specify the color of the border.
	bordertype     ... specify the type of the border.
	*/
	namespace plot = adapt::plot;
	adapt::Canvas2D g(output_filename);
	//g.ShowCommands(true);
	g.EnableInMemoryDataTransfer(enable_in_memory_data_transfer); // Enable or disable datablock feature of gnuplot
	g.SetTitle("example\\_2d");
	g.SetXRange(-4.0, 4.0);
	g.SetXLabel("x");
	g.SetYLabel("y");
	g.PlotPoints(equation, plot::title = "mu = 0, sigma = 1",
				 plot::s_lines).
		PlotPoints(x1, y1, plot::xerrorbar = 0.125, plot::yerrorbar = e1,
				   plot::title = "data", plot::c_black,
				   plot::s_points, plot::pt_fcir, plot::ps_med_small);

	if (!enable_in_memory_data_transfer)
	{
		adapt::Canvas2D g2(output_filename + ".fileplot.png");
		//g2.ShowCommands(true);
		g2.SetTitle("example\\_2d");
		g2.SetXRange(-4.0, 4.0);
		g2.SetXLabel("x");
		g2.SetYLabel("y");
		g2.PlotPoints(equation, plot::title = "mu = 0, sigma = 1",
					  plot::s_lines).
			PlotPoints(output_filename + ".tmp1.txt", "1", "2", plot::xerrorbar = 0.125, plot::yerrorbar = "3",
					   plot::title = "data", plot::c_black,
					   plot::s_points, plot::pt_fcir, plot::ps_med_small);
	}
	return 0;
}

int example_scatter(const std::string& output_filename, bool enable_in_memory_data_transfer)
{
	std::vector<double> longitudes{ 141.3469, 140.74, 141.1526, 140.8694, 140.1023, 140.3633, 140.4676, 140.4468, 139.8836, 139.0608, 139.6489, 140.1233, 139.6917, 139.6423, 139.0235, 137.2113, 136.6256, 136.2219, 138.5684, 138.1812, 136.7223, 138.3828, 136.9066, 136.5086, 135.8686, 135.7556, 135.5023, 135.183, 135.8048, 135.1675, 134.2383, 133.0505, 133.9344, 132.4553, 131.4714, 134.5594, 134.0434, 132.7657, 133.5311, 130.4017, 130.3009, 129.8737, 130.7417, 131.6126, 131.4202, 130.5581, 127.6809 };
	std::vector<double> latitudes{ 43.0642, 40.8244, 39.7036, 38.2682, 39.7186, 38.2404, 37.7503, 36.3418, 36.5658, 36.3911, 35.8569, 35.6051, 35.6895, 35.4475, 37.9026, 36.6953, 36.5944, 36.0652, 35.6642, 36.6513, 35.3912, 34.9756, 35.1802, 34.7303, 35.0045, 35.021, 34.6937, 34.6913, 34.6851, 34.226, 35.5036, 35.4723, 34.6618, 34.3853, 34.1858, 34.0658, 34.3402, 33.8416, 33.5597, 33.5902, 33.2635, 32.7448, 32.7898, 33.2382, 31.9077, 31.5602, 26.2124 };
	std::vector<double> populations{ 5250, 1230, 1220, 2330, 970, 1070, 1840, 2860, 1940, 1930, 7330, 6290, 13960, 9200, 2200, 1040, 1140, 770, 810, 2030, 1970, 3630, 7550, 1790, 1410, 2580, 8820, 5450, 1310, 930, 550, 670, 1890, 2810, 1320, 720, 960, 1340, 690, 5100, 810, 1280, 1720, 1130, 1080, 1590, 1450 };
	auto pop_size = populations | std::views::transform([](double x) { return x / 1000; });

	namespace plot = adapt::plot;
	adapt::Canvas2D g(output_filename);
	g.EnableInMemoryDataTransfer(enable_in_memory_data_transfer);
	g.SetXRange(128, 150);
	g.SetYRange(29, 46);
	g.SetSizeRatio(1);
	g.SetXLabel("longitude");
	g.SetYLabel("latitude");
	g.SetTitle("Japan population distribution");
	// world_10m.txt can be downloaded from https://gnuplotting.org/plotting-the-world-revisited/
	g.PlotPoints("PlotExamples/world_10m.txt", "1", "2", plot::notitle, plot::c_dark_gray, plot::s_lines).
		PlotPoints(longitudes, latitudes, plot::notitle,
				   plot::s_points, plot::pt_fcir, plot::color_rgb = "0xAA6688FF", plot::variable_size = pop_size);

	return 0;
}

double calc_r(double x, double y)
{
	return std::sqrt(x * x + y * y);
};
double potential(double x, double y)
{
	double r1 = calc_r(x - 3, y);
	double r2 = calc_r(x + 3, y);
	if (r1 == 0.) r1 = 0.000001;
	if (r2 == 0.) r2 = 0.000001;
	double p1 = 1 / r1;
	double p2 = 3 / r2;
	return p1 - p2;
};
double fieldx(double x, double y)
{
	double f1 = (x - 3) / std::pow(calc_r(x - 3, y), 3);
	double f2 = 3 * (x + 3) / std::pow(calc_r(x + 3, y), 3);
	return f1 - f2;
}
double fieldy(double x, double y)
{
	double f1 = y / std::pow(calc_r(x - 3, y), 3);
	double f2 = 3 * y / std::pow(calc_r(x + 3, y), 3);
	return f1 - f2;
}
int example_colormap(const std::string& output_filename, bool enable_in_memory_data_transfer)
{
	adapt::Matrix<double> m(100, 100);
	std::pair<double, double> xrange = { -9.9, 9.9 };
	std::pair<double, double> yrange = { -9.9, 9.9 };
	for (int iy = -50; iy < 50; ++iy)
	{
		double y = iy * 0.2 + 0.1;
		for (int ix = -50; ix < 50; ++ix)
		{
			double x = ix * 0.2 + 0.1;
			m[ix + 50][iy + 50] = potential(x, y);
		}
	}
	std::vector<double> xfrom(441), yfrom(441), xlen(441), ylen(441);
	std::vector<double> arrowcolor(441);
	for (int iy = -10; iy <= 10; ++iy)
	{
		for (int ix = -10; ix <= 10; ++ix)
		{
			size_t jx = (ix + 10);
			size_t jy = (iy + 10);
			double xlen_ = fieldx(ix, iy);
			double ylen_ = fieldy(ix, iy);
			double len = std::sqrt(xlen_ * xlen_ + ylen_ * ylen_);
			xlen_ = xlen_ / len * 0.8;
			ylen_ = ylen_ / len * 0.8;
			xlen[jy * 21 + jx] = xlen_;
			ylen[jy * 21 + jx] = ylen_;
			xfrom[jy * 21 + jx] = ix - xlen_ / 2.;
			yfrom[jy * 21 + jx] = iy - ylen_ / 2.;
			arrowcolor[jy * 21 + jx] = potential(ix - xlen_ / 2., iy - ylen_ / 2.);
		}
	}

	/* options for Colormap
	title           ... title.
	  no_title      ... the same as title = "notitle".
	axis            ... set of axes to scale lines. (e.g. plot::axis = "x1y2")
	  axis_x1y1     ... the same as axis = "x1y1"
	  axis_x1y2     ... the same as axis = "x1y2"

	//contour options
	with_contour    ... plot contour
	without_surface ... plot without surface. Basically used with "with_contour".
	cntrsmooth      ... interpolation by CntrSmooth::linear, cubicspline, bspline
	cntropints      ... the number of lines for cpline and bspline
	cntrorder       ... order for bspline, [2, 10]
	cntrlevels_auto
	cntrlevels_discrete ... cntrlevels_discrete <z1>, <z2>, ... generate contours at z1, z2, ...
	cntrlevels_incremental ... cntrlevels_incremental <start>, <incr>, <end> generate contours at start, start+incr, start+2*incr, ...
	cntrcolor
	variable_cntrcolor ... color contours with its z value.
	cntrlinetype    ... linetype. see gnuplot's "linetype" option.
	cntrlinewidth
	*/

	namespace plot = adapt::plot;
	{
		adapt::MultiPlot multi(output_filename, 1, 2, 1200, 600);

		adapt::CanvasCM g1(output_filename + ".map_tmp");
		//g1.ShowCommands(true);
		g1.EnableInMemoryDataTransfer(enable_in_memory_data_transfer); // Enable or disable datablock feature of gnuplot
		g1.SetTitle("example\\_colormap");
		g1.SetPaletteDefined({ {0, "yellow" }, { 4.5, "red" }, { 5., "black" }, { 5.5, "blue"}, { 10, "cyan" } });
		g1.SetSizeRatio(-1);
		g1.SetXLabel("x");
		g1.SetYLabel("y");
		g1.SetXRange(-10, 10);
		g1.SetYRange(-10, 10);
		g1.SetCBRange(-5, 5);
		g1.PlotColormap(m, xrange, yrange, plot::notitle).
			PlotVectors(xfrom, yfrom, xlen, ylen, plot::notitle, plot::c_white);

		//sleep for a short time to avoid the output image broken by multiplot.
		std::this_thread::sleep_for(std::chrono::milliseconds(300));

		adapt::CanvasCM g2(output_filename + ".cntr_tmp");
		//g2.ShowCommands(true);
		g2.EnableInMemoryDataTransfer(enable_in_memory_data_transfer); // Enable or disable datablock feature of gnuplot
		g2.SetTitle("example\\_contour");
		g2.SetPaletteDefined({ {0, "yellow" }, { 4.5, "red" }, { 5., "black" }, { 5.5, "blue"}, { 10, "cyan" } });
		g2.SetSizeRatio(-1);
		g2.SetXLabel("x");
		g2.SetYLabel("y");
		g2.SetXRange(-10, 10);
		g2.SetYRange(-10, 10);
		g2.SetCBRange(-5, 5);
		g2.PlotColormap(m, xrange, yrange, plot::notitle,
						plot::with_contour, plot::without_surface, plot::variable_cntrcolor,
						plot::cntrlevels_incremental = { -20., 0.2, 20. }).
			PlotVectors(xfrom, yfrom, xlen, ylen, plot::notitle, plot::variable_color = arrowcolor);
	}

	if (!enable_in_memory_data_transfer)
	{
		adapt::CanvasCM g1(output_filename + ".fileplot.png");
		//g1.ShowCommands(true);
		g1.SetTitle("example\\_colormap");
		g1.SetPaletteDefined({ {0, "yellow" }, { 4.5, "red" }, { 5., "black" }, { 5.5, "blue"}, { 10, "cyan" } });
		g1.SetSizeRatio(-1);
		g1.SetXLabel("x");
		g1.SetYLabel("y");
		g1.SetXRange(-10, 10);
		g1.SetYRange(-10, 10);
		g1.SetCBRange(-5, 5);
		g1.PlotColormap(output_filename + ".map_tmp.tmp0.txt", "1", "2", "5", plot::notitle).
			PlotVectors(output_filename + ".map_tmp.tmp1.txt", "1", "2", "4", "5", plot::notitle, plot::c_white);
	}
	return 0;
}

struct ChiSquare
{
	ChiSquare(double k) : k(k) {}
	double operator()(double x) const
	{
		return std::pow(x, k / 2 - 1) * std::exp(-x / 2) / (std::pow(2, k / 2) * tgamma(k / 2));
	}
	double k;
};
int example_filledcurve(const std::string& output_filename, bool enable_in_memory_data_transfer)
{
	std::vector<double> x(401, 0);
	std::vector<double> y1(401, 0);
	std::vector<double> y2(401, 0);
	std::vector<double> y3(401, 0);
	ChiSquare c1(1);
	ChiSquare c2(2);
	ChiSquare c3(3);

	for (int i = 0; i <= 400; ++i)
	{
		x[i] = i * 0.02;
		y1[i] = c1(x[i]);
		y2[i] = c2(x[i]);
		y3[i] = c3(x[i]);
	}
	y1[0] = 10;

	/*
	1. PlotFilledCurves(const std::vector<double>& x, const std::vector<double>& y, Options ...options)
	2. PlotFilledCurves(const std::vector<double>& x, const std::vector<double>& y, const std::vector<double>& y2, Options ...options)
	the area between the curve and (1)a line given by baseline option(y=0 by default) (2)y2 curve is filled.
	 * options for FilledCurves
	title          ... title.
	  no_title     ... the same as title = "notitle".
	axis           ... set of axes to scale lines. (e.g. plot::axis = "x1y2")
	  axis_x1y1    ... the same as axis = "x1y1"
	  axis_x1y2    ... the same as axis = "x1y2"
	fillpattern    ... fill with a colored pattern.
	fillsolid      ... fill with a solid color specified by fillcolor with density [ 0.0, 1.0 ].
	filltransparent... make the filled area transparent to the background color.
	fillcolor      ... specify the color of filled area.
	variable_fillcolor ...
	bordercolor    ... specify the color of the border.
	bordertype     ... specify the type of the border.
	baseline       ... the area between the curve this line is filled. (e.g. plot::baseline = "y=10")
	closed         ... the points are treated as a closed polygon and its inside is filled.
	above          ... the filled area is limited to the above side of baseline or y2 curve.
	below          ... the filled area is limited to the below side of baseline or y2 curve.
	*/

	namespace plot = adapt::plot;
	adapt::Canvas2D g(output_filename);
	//g.ShowCommands(true);
	g.EnableInMemoryDataTransfer(enable_in_memory_data_transfer); // Enable or disable datablock feature of gnuplot
	g.SetTitle("example\\_filledcurve");
	g.SetXRange(0, 8.0);
	g.SetYRange(0, 1.0);
	g.SetXLabel("x");
	g.SetYLabel("y");
	g.PlotFilledCurves(x, y1, plot::title = "k = 1",
					   plot::fillcolor = "red", plot::fillsolid = 0.4, plot::filltransparent).
		PlotFilledCurves(x, y2, plot::title = "k = 2",
						 plot::fillcolor = "blue", plot::fillsolid = 0.4, plot::filltransparent).
		PlotFilledCurves(x, y3, plot::title = "k = 3",
						 plot::fillcolor = "green", plot::fillsolid = 0.4, plot::filltransparent);
	return 0;
}

int GetTested(int d)
{
	static std::random_device rd;
	static std::mt19937_64 mt(rd());
	double a = d * d * 0.05;
	double b = a * std::pow((std::sin(2 * 3.141592 * d / 120) + 1) / 2, 3) + d;
	return b <= 0 ? 0 : std::poisson_distribution<>(b)(mt);
}
int GetPositive(int t)
{
	static std::random_device rd;
	static std::mt19937_64 mt(rd());
	return t == 0 ? 0 : std::binomial_distribution<>(t, 0.05)(mt);
}

std::pair<int, int> GetDate(int a)
{
	static constexpr std::array<int, 12> x = { 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366 };
	int m = 0;
	int d = 0;
	int p = 0;
	for (int i = 0; i < 12; ++i)
	{
		if (a + 1 <= x[i]) { m = i + 1, d = a + 1 - p; break; };
		p = x[i];
	}
	return { m, d };
}

struct Accumulator
{
	int operator()(int t) const { return sum += t; }
	mutable int sum;
};
int example_datetime(const std::string& output_filename, bool enable_in_memory_data_transfer)
{
	std::vector<int> yr(366);
	std::iota(yr.begin(), yr.end(), 0);
	auto x = std::views::all(yr) | std::views::transform([](int a) { auto [m, d] = GetDate(a); return std::format("2020-{}-{}", m, d); });
	auto y = std::views::all(yr) | std::views::transform([](int a) { return GetTested(a); });
	auto y2 = y | std::views::transform([](int t) { return GetPositive(t); });
	auto y3 = y | std::views::transform(Accumulator{});
	auto y4 = y2 | std::views::transform(Accumulator{});

	namespace plot = adapt::plot;
	adapt::Canvas2D g(output_filename);
	//g.ShowCommands(true);
	g.EnableInMemoryDataTransfer(enable_in_memory_data_transfer);
	g.SetXTicsRotate(-45);
	g.SetTitle("example\\_datetime");
	g.SetYLabel("per day");
	g.SetY2Label("total");
	g.SetKeyTopRight();
	g.SetKeyOpaque();
	g.SetKeyBox();
	g.SetXDataTime("%Y-%m-%d");
	g.SetFormatX("%02m/%02d");
	g.PlotPoints(x, y, plot::title = "tested", plot::s_steps, plot::fillsolid = 0.5).
		PlotPoints(x, y2, plot::title = "positive", plot::s_steps, plot::fillsolid = 0.5).
		PlotLines(x, y3, plot::title = "tested\\_total", plot::ax_x1y2, plot::lw_ex_thick).
		PlotLines(x, y4, plot::title = "positive\\_total", plot::ax_x1y2, plot::lw_ex_thick);

	return 0;
}

int example_string(const std::string& output_filename, bool enable_in_memory_data_transfer)
{
	std::vector<std::string> x;
	std::vector<double> y;

	x.push_back("label-one");   y.push_back(1);
	x.push_back("label-two");   y.push_back(2);
	x.push_back("label-three"); y.push_back(3);
	x.push_back("label-four");  y.push_back(4);
	x.push_back("label-five");  y.push_back(5);
	x.push_back("label-six");   y.push_back(6);
	x.push_back("label-seven"); y.push_back(7);
	x.push_back("label-eight"); y.push_back(8);
	x.push_back("label-nine");  y.push_back(9);
	x.push_back("label-ten");   y.push_back(10);

	namespace plot = adapt::plot;
	adapt::Canvas2D g(output_filename);
	//g.ShowCommands(true);
	g.EnableInMemoryDataTransfer(enable_in_memory_data_transfer); // Enable or disable datablock feature of gnuplot
	g.SetXRange(-1, 10);
	g.SetYRange(0, 11);
	g.SetXTicsRotate(-45);
	g.SetTitle("example\\_string");
	g.PlotPoints(x, y, plot::s_boxes, plot::notitle);
	return 0;
}

int example_for_loop(const std::string& output_filename, bool enable_in_memory_data_transfer)
{
	namespace plot = adapt::plot;
	adapt::Canvas2D g(output_filename);
	//g.ShowCommands(true);
	g.EnableInMemoryDataTransfer(enable_in_memory_data_transfer); // Enable or disable datablock feature of gnuplot
	g.SetKeyTopLeft();
	g.SetTitle("example\\_for\\_loop");

	auto buf = g.GetBuffer();
	for (int i = 0; i < 5; ++i)
	{
		std::string eq = std::to_string(i) + "*x";
		buf = buf.PlotPoints(eq, plot::title = eq, plot::s_lines);
	}
	buf.Flush();
	return 0;
}

void PlotVariations()
{
	std::filesystem::exists("PlotExamples") || std::filesystem::create_directory("PlotExamples");
	//example_2d("PlotExamples/example_2d.png", false);
	example_2d("PlotExamples/example_2d-inmemory.png", true);

	//example_scatter("PlotExamples/example_scatter.png", false);
	example_scatter("PlotExamples/example_scatter-inmemory.png", true);

	example_colormap("PlotExamples/example_colormap.png", false);
	example_colormap("PlotExamples/example_colormap-inmemory.png", true);

	//example_filledcurve("PlotExamples/example_filledcurve.png", false);
	example_filledcurve("PlotExamples/example_filledcurve-inmemory.png", true);

	//example_datetime("PlotExamples/example_datetime.png", false);
	example_datetime("PlotExamples/example_datetime-inmemory.png", true);

	//example_string("PlotExamples/example_string_label.png", false);
	example_string("PlotExamples/example_string_label-inmemory.png", true);

	//example_for_loop("PlotExamples/example_for_loop.png", false);
	example_for_loop("PlotExamples/example_for_loop-inmemory.png", true);
}

#endif // EXAMPLES_PLOT_H