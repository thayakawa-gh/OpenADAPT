#include <Test/Aggregator.h>
#include <OpenADAPT/Plot/Canvas.h>


TEST_F(Aggregator, Plot_xyerrorbars)
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