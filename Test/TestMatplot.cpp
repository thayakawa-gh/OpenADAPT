#include <Test/Aggregator.h>
#include <cmath>
#include <matplot/matplot.h>
#include <random>

struct DummyIterable
{
	using const_iteor = const int*;
	const_iteor begin() const { return v; }
	const_iteor end() const { return v + 10; }
	size_t size() const { return 10; }
	int v[10];
};

TEST_F(Aggregator, Matplot)
{
	[[maybe_unused]] auto [number, name, dob] = m_dtree.GetPlaceholders("number", "name", "date_of_birth");
	[[maybe_unused]] auto [exam, math, eng, jpn, sci, soc] = m_dtree.GetPlaceholders("exam", "math", "japanese", "english", "science", "social");

	//戻り値はstd::tuple<std::vector<double>, std::vector<double>>;
	auto [v_math, v_sci] = m_dtree | Filter(exam == 0) | ToVector(cast_f64(math).f64(), cast_f64(sci).f64());

	matplot::figure_handle f = matplot::figure(true);

	matplot::scatter(v_math, v_sci, 3)->marker_face(true);
	matplot::title("math vs science");
	matplot::xlabel("math");
	matplot::ylabel("science");
	matplot::save("math_vs_science.pdf");

	std::vector<double> v_soc = m_dtree | Filter(exam == 0) | ToVector(cast_f64(sci).f64());
	auto h = matplot::hist(v_soc);
	h->bin_width(10);
	matplot::title("distribution of science scores");
	matplot::xlabel("science score");
	matplot::ylabel("number of students");
	matplot::save("dist_science.pdf");
}