#include <Test/Common/Hist.h>

TEST(Random, DHist)
{
	DHist hist;
	TestHist(hist);
}
TEST(Random, SHist)
{
	using HistLayer1 = ADAPT_S_DEFINE_LAYER(x, double, y, double, xy, double);
	SHist2D<HistLayer1> hist;
	TestHist(hist);
}
