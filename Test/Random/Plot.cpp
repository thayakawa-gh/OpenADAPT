#include <algorithm>
#include <random>
#include <gtest/gtest.h>
#include <OpenADAPT/Plot/Canvas.h>

using namespace adapt;

TEST(Random, Plot_XYErrorBars)
{
	std::vector<double> x = { 1, 2, 3, 4, 5 };
	std::vector<double> y = { 1, 2, 3, 4, 5 };
	std::vector<double> xerr = { 0.1, 0.1, 0.1, 0.1, 0.1 };
	std::vector<double> yerr = { 0.1, 0.2, 0.3, 0.4, 0.5 };
	std::vector<double> vc = { 1, 2, 3, 4, 5 };

	Canvas2D g("Plot_xyerrorbars.png");
	g.SetTitle("Plot\\_xyerrorbars");
	g.SetXLabel("X");
	g.SetYLabel("Y");
	g.PlotPoints(x, y, plot::xerrorbar = xerr, plot::yerrorbar = yerr, plot::variable_color = vc);
}

TEST(Random, Plot_Fitting)
{
	std::normal_distribution<double> gauss_dist(1.0, 2.0);
	std::mt19937 rng(42);
	std::vector<double> data_x(500);
	for (size_t i = 0; i < data_x.size(); ++i)
	{
		double x = gauss_dist(rng);
		data_x[i] = x;
	}

	{
		Canvas2D g("Plot_fitting.png");
		g.ShowCommands(true);
		g.SetXRange(-9, 11);
		std::vector<double> params{ 0., 1., 1. };
		g.PlotHistogram(data_x, -9, 11, 50, plot::title = "data", plot::c_dark_persimmon, plot::fit_normal(params, plot::c_olive, plot::s_lines));
	}
	{
		Canvas2D g("Plot_fitting_2.png");
		g.ShowCommands(true);
		g.SetXRange(-9, 11);
		std::vector<double> params{ 0., 1., 1. };
		auto gauss_fit = [](double x, double /*y*/, std::span<const double> params)
		{
			double mean = params[0];
			double stddev = params[1];
			double factor = params[2];
			double d = (x - mean) / stddev;
			return std::exp(-0.5 * d * d) / (stddev * std::sqrt(2 * std::numbers::pi)) * factor;
		};
		g.PlotHistogram(data_x, -9, 11, 50, plot::title = "data", plot::c_dark_persimmon, plot::fit(gauss_fit, params, plot::c_olive, plot::s_lines));
	}
}