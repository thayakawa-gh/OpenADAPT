#include <Test/Aggregator.h>

double my_hypot(double a, double b) { return std::hypot(a, b); }

TEST_F(Aggregator, UserFunc_dtree)
{
	auto hypot = UserFunc([](std::integral auto a, std::integral auto b) { return std::hypot((double)a, (double)b); });
	//hypot2のような書き方をすると、int32_tなどの方から勝手に変換して呼び出そうとして、コンパイラが警告を吐く。
	//動作上は問題ないが、気持ち悪ければhypotのようにコンセプトなどを使って制約した方が良い。
	//CttiまたはTypedなら警告も出さない。
	//auto hypot2 = UserFunc(my_hypot);

	//0層要素。学年とクラス。
	//auto [grade, class_] = m_dtree.GetPlaceholders("grade", "class");
	//1層要素。出席番号、名前、生年月日。
	//auto [number, name, dob] = m_dtree.GetPlaceholders("number", "name", "date_of_birth");
	//2層要素。各試験の点数。前期中間、前期期末、後期中間、後期期末の順に並んでいる。
	auto [exam, math, jpn, eng, sci, soc] = m_dtree.GetPlaceholders("exam", "math", "japanese", "english", "science", "social");

	auto l = hypot(jpn, eng);
	std::cout << l(m_dtree, { 0, 0, 0 }).f64() << std::endl;
	//auto l2 = hypot2(jpn, eng);
	//std::cout << l2(m_dtree, { 0, 0, 0 }).f64() << std::endl;
}
TEST_F(Aggregator, UserFunc_stree)
{
	auto hypot = UserFunc([](std::integral auto a, std::integral auto b) { return std::hypot((double)a, (double)b); });
	auto hypot2 = UserFunc(my_hypot);

	//0層要素。学年とクラス。
	//auto [grade, class_] = m_stree.GetPlaceholders("grade"_fld, "class"_fld);
	//1層要素。出席番号、名前、生年月日。
	//auto [number, name, dob] = m_stree.GetPlaceholders("number"_fld, "name"_fld, "date_of_birth"_fld);
	//2層要素。各試験の点数。前期中間、前期期末、後期中間、後期期末の順に並んでいる。
	auto [exam, math, jpn, eng, sci, soc] = m_stree.GetPlaceholders("exam"_fld, "math"_fld, "japanese"_fld, "english"_fld, "science"_fld, "social"_fld);

	auto l = hypot(jpn, eng);
	std::cout << l(m_stree, { 0, 0, 0 }) << std::endl;
	auto l2 = hypot2(jpn, eng);
	std::cout << l2(m_stree, { 0, 0, 0 }) << std::endl;
}