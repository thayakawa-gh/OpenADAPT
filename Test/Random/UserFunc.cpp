#include <Test/Common/Aggregator.h>

double my_hypot(double a, double b) { return std::hypot(a, b); }

TEST_F(Aggregator_DTree, UserFunc)
{
	auto hypot = UserFunc([](std::integral auto a, std::integral auto b) { return std::hypot((double)a, (double)b); });
	//hypot2のような書き方をすると、int32_tなどの方から勝手に変換して呼び出そうとして、コンパイラが警告を吐く。
	//動作上は問題ないが、気持ち悪ければhypotのようにコンセプトなどを使って制約した方が良い。
	//CttiまたはTypedなら警告も出さない。
	//auto hypot2 = UserFunc(my_hypot);

	DECL_PH_SET_DTree;

	auto l = hypot(japanese, english);
	std::cout << l(*m_tree, { 0, 0, 0 }).f64() << std::endl;
	//auto l2 = hypot2(jpn, eng);
	//std::cout << l2(m_dtree, { 0, 0, 0 }).f64() << std::endl;
}
TEST_F(Aggregator_STree, UserFunc)
{
	auto hypot = UserFunc([](std::integral auto a, std::integral auto b) { return std::hypot((double)a, (double)b); });
	auto hypot2 = UserFunc(my_hypot);

	DECL_PH_SET_STree;

	auto l = hypot(japanese, english);
	std::cout << l(*m_tree, { 0, 0, 0 }) << std::endl;
	auto l2 = hypot2(japanese, english);
	std::cout << l2(*m_tree, { 0, 0, 0 }) << std::endl;
}