#include <Test/Common/Aggregator.h>
#include <OpenADAPT/Parser.h>

using namespace adapt;
using namespace adapt::lit;

template <FieldType Type, class Trav, class NP1, class NP2>
bool compare(Number<Type>, const Trav& t, NP1& np1, NP2& np2)
{
	if constexpr (stat_type_node_or_placeholder<NP1>) return np1(t) == np2(t).template as<Type>();
	else return np1(t).template as<Type>() == np2(t).template as<Type>();
}
template <d_tree Tree, class Layer0, class Layer1, class Layer2>
void TestParser(Tree& tree, const std::vector<Class>&, Layer0 l0, Layer1 l1, Layer2 l2)
{
	//0層要素。学年とクラス。
	auto [grade, class_] = l0;
	//1層要素。出席番号、名前、生年月日。
	auto [number, name] = l1;
	//2層要素。各試験の点数。前期中間、前期期末、後期中間、後期期末の順に並んでいる。
	auto [exam, math, japanese, english, science, social] = l2;

	auto lambda1 = math + japanese + english;
	auto strlambda1 = Parse(tree, "math + japanese + english");

	auto lambda2 = math + japanese * english;
	auto strlambda2 = Parse(tree, "math + japanese * english");

	//優先度が同じ演算子の連続。左結合であることを確認する。
	auto lambda3 = math > english > 50;
	auto strlambda3 = Parse(tree, "math > english > 50");

	auto lambda4 = !(japanese > english);
	auto strlambda4 = Parse(tree, "!(japanese > english)");

	//通常関数
	auto lambda5 = max(math, japanese);
	auto strlambda5 = Parse(tree, "max(math, japanese)");

	auto lambda6 = max(math, japanese) > 80 && social > 80;
	auto strlambda6 = Parse(tree, "max(math, japanese) > 80 && social > 80");

	//階層関数
	auto lambda7 = sum(math + japanese + english);
	auto strlambda7 = Parse(tree, "sum(math + japanese + english)");

	using enum FieldType;
	for (const auto& trav : tree.GetRange(2_layer))
	{
		EXPECT_TRUE(compare(Number<I32>{}, trav, lambda1, strlambda1));
		EXPECT_TRUE(compare(Number<I32>{}, trav, lambda2, strlambda2));
		EXPECT_TRUE(compare(Number<I08>{}, trav, lambda3, strlambda3));
		EXPECT_TRUE(compare(Number<I08>{}, trav, lambda4, strlambda4));
		EXPECT_TRUE(compare(Number<I32>{}, trav, lambda5, strlambda5));
		EXPECT_TRUE(compare(Number<I08>{}, trav, lambda6, strlambda6));
		EXPECT_TRUE(compare(Number<I32>{}, trav, lambda7, strlambda7));
	}
}
template <any_tree Container, class ...Args>
	requires (!d_tree<Container>)
void TestParser(const Container&, Args&& ...)
{

}
template <d_table Table, class Layer0>
void TestParser(Table& table, const std::vector<Class>&, Layer0 l)
{
	auto [class_, number, name, exam, math, japanese, english, science, social] = l;

	auto lambda1 = math + japanese + english;
	auto strlambda1 = Parse(table, "math + japanese + english");

	auto lambda2 = math + japanese * english;
	auto strlambda2 = Parse(table, "math + japanese * english");

	//優先度が同じ演算子の連続。左結合であることを確認する。
	auto lambda3 = math > english > 50;
	auto strlambda3 = Parse(table, "math > english > 50");

	auto lambda4 = !(japanese > english);
	auto strlambda4 = Parse(table, "!(japanese > english)");

	//通常関数
	auto lambda5 = max(math, japanese);
	auto strlambda5 = Parse(table, "max(math, japanese)");

	auto lambda6 = max(math, japanese) > 80 && social > 80;
	auto strlambda6 = Parse(table, "max(math, japanese) > 80 && social > 80");

	using enum FieldType;
	for (const auto& trav : table.GetRange(0_layer))
	{
		EXPECT_TRUE(compare(Number<I32>{}, trav, lambda1, strlambda1));
		EXPECT_TRUE(compare(Number<I32>{}, trav, lambda2, strlambda2));
		EXPECT_TRUE(compare(Number<I08>{}, trav, lambda3, strlambda3));
		EXPECT_TRUE(compare(Number<I08>{}, trav, lambda4, strlambda4));
		EXPECT_TRUE(compare(Number<I32>{}, trav, lambda5, strlambda5));
		EXPECT_TRUE(compare(Number<I08>{}, trav, lambda6, strlambda6));
	}
}
template <any_table Table, class Layer0>
	requires (!d_table<Table>)
void TestParser(Table&, const std::vector<Class>&, Layer0)
{

}