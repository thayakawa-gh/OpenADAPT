#include <Test/Common/Aggregator.h>

template <any_tree Container, class Layer0, class Layer1, class Layer2>
void TestShow(Container& tree, const Layer0& l0, const Layer1& l1, const Layer2& l2)
{
	//0層要素。学年とクラス。
	[[maybe_unused]] auto [grade, class_] = l0;
	//1層要素。出席番号、名前、生年月日。
	[[maybe_unused]] auto [number, name] = l1;
	//2層要素。各試験の点数。前期中間、前期期末、後期中間、後期期末の順に並んでいる。
	[[maybe_unused]] auto [exam, math, jpn, eng, sci, soc] = l2;

	auto sum_3subjs = jpn + math + eng;
	auto rank = count_if3(exam == 0 && sum_3subjs > jpn.o(0_depth) + math.o(0_depth) + eng.o(0_depth)) + 1;

	tree | Filter(sum_3subjs >= 240 && exam == 0) | Show(name, sum_3subjs, jpn, math, eng, rank);
}