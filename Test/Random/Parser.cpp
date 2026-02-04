#include <gtest/gtest.h>
#ifdef ADAPT_IMPORT_MODULE
#include <OpenADAPT/Macros.h>
import adapt;
#else
#include <OpenADAPT/ADAPT.h>
#endif
#include <OpenADAPT/Evaluator/Parser.h>

using namespace adapt;
using namespace adapt::lit;
using namespace adapt::eval::parser;

// DTreeでのパーサーテスト
TEST(Parser, DTree_BasicArithmetic)
{
	// テスト用のシンプルなDTreeを作成
	DTree tree;
	tree.SetStructure("a"_fld, "b"_fld, "c"_fld);
	tree.Resize({ 1 });
	tree[0]["a"].i32() = 10;
	tree[0]["b"].i32() = 20;
	tree[0]["c"].i32() = 30;
	
	// プレースホルダマップの作成
	auto ph_map = MakePlaceholderMap(tree, std::array{ "a", "b", "c" });
	
	// 加算のテスト
	{
		auto lambda = ParseRttiFuncNode<DTree>("a + b", ph_map);
		EXPECT_EQ(lambda.GetType(), FieldType::I32);
		EXPECT_EQ(lambda(tree, Bpos{ 0 }).i32(), 30);
	}
	
	// 減算のテスト
	{
		auto lambda = ParseRttiFuncNode<DTree>("b - a", ph_map);
		EXPECT_EQ(lambda(tree, Bpos{ 0 }).i32(), 10);
	}
	
	// 乗算のテスト
	{
		auto lambda = ParseRttiFuncNode<DTree>("a * b", ph_map);
		EXPECT_EQ(lambda(tree, Bpos{ 0 }).i32(), 200);
	}
	
	// 除算のテスト
	{
		auto lambda = ParseRttiFuncNode<DTree>("c / a", ph_map);
		EXPECT_EQ(lambda(tree, Bpos{ 0 }).i32(), 3);
	}
	
	// 剰余のテスト
	{
		auto lambda = ParseRttiFuncNode<DTree>("c % a", ph_map);
		EXPECT_EQ(lambda(tree, Bpos{ 0 }).i32(), 0);
	}
}

TEST(Parser, DTree_ComplexExpression)
{
	DTree tree;
	tree.SetStructure("x"_fld, "y"_fld, "z"_fld);
	tree.Resize({ 1 });
	tree[0]["x"].i32() = 5;
	tree[0]["y"].i32() = 3;
	tree[0]["z"].i32() = 2;
	
	auto ph_map = MakePlaceholderMap(tree, std::array{ "x", "y", "z" });
	
	// 複合式: (x + y) * z
	{
		auto lambda = ParseRttiFuncNode<DTree>("(x + y) * z", ph_map);
		EXPECT_EQ(lambda(tree, Bpos{ 0 }).i32(), 16);
	}
	
	// 複合式: x * y + z
	{
		auto lambda = ParseRttiFuncNode<DTree>("x * y + z", ph_map);
		EXPECT_EQ(lambda(tree, Bpos{ 0 }).i32(), 17);
	}
	
	// 複合式: x + y * z (演算子の優先順位)
	{
		auto lambda = ParseRttiFuncNode<DTree>("x + y * z", ph_map);
		EXPECT_EQ(lambda(tree, Bpos{ 0 }).i32(), 11);
	}
}

TEST(Parser, DTree_Comparison)
{
	DTree tree;
	tree.SetStructure("a"_fld, "b"_fld, "c"_fld);
	tree.Resize({ 1 });
	tree[0]["a"].i32() = 10;
	tree[0]["b"].i32() = 20;
	tree[0]["c"].i32() = 10;
	
	auto ph_map = MakePlaceholderMap(tree, std::array{ "a", "b", "c" });
	
	// 等しい
	{
		auto lambda = ParseRttiFuncNode<DTree>("a == c", ph_map);
		EXPECT_EQ(lambda(tree, Bpos{ 0 }).i08(), 1);
	}
	
	// 等しくない
	{
		auto lambda = ParseRttiFuncNode<DTree>("a != b", ph_map);
		EXPECT_EQ(lambda(tree, Bpos{ 0 }).i08(), 1);
	}
	
	// より小さい
	{
		auto lambda = ParseRttiFuncNode<DTree>("a < b", ph_map);
		EXPECT_EQ(lambda(tree, Bpos{ 0 }).i08(), 1);
	}
	
	// より大きい
	{
		auto lambda = ParseRttiFuncNode<DTree>("b > a", ph_map);
		EXPECT_EQ(lambda(tree, Bpos{ 0 }).i08(), 1);
	}
	
	// 以下
	{
		auto lambda = ParseRttiFuncNode<DTree>("a <= c", ph_map);
		EXPECT_EQ(lambda(tree, Bpos{ 0 }).i08(), 1);
	}
	
	// 以上
	{
		auto lambda = ParseRttiFuncNode<DTree>("b >= a", ph_map);
		EXPECT_EQ(lambda(tree, Bpos{ 0 }).i08(), 1);
	}
}

TEST(Parser, DTree_Logical)
{
	DTree tree;
	tree.SetStructure("a"_fld, "b"_fld, "c"_fld);
	tree.Resize({ 1 });
	tree[0]["a"].i32() = 10;
	tree[0]["b"].i32() = 20;
	tree[0]["c"].i32() = 30;
	
	auto ph_map = MakePlaceholderMap(tree, std::array{ "a", "b", "c" });
	
	// 論理積
	{
		auto lambda = ParseRttiFuncNode<DTree>("a < b && b < c", ph_map);
		EXPECT_EQ(lambda(tree, Bpos{ 0 }).i08(), 1);
	}
	
	// 論理和
	{
		auto lambda = ParseRttiFuncNode<DTree>("a > b || b < c", ph_map);
		EXPECT_EQ(lambda(tree, Bpos{ 0 }).i08(), 1);
	}
	
	// 論理否定
	{
		auto lambda = ParseRttiFuncNode<DTree>("!(a > b)", ph_map);
		EXPECT_EQ(lambda(tree, Bpos{ 0 }).i08(), 1);
	}
}


TEST(Parser, ErrorHandling)
{
	DTree tree;
	tree.SetStructure("a"_fld);
	tree.Resize({ 1 });
	tree[0]["a"].i32() = 10;
	
	auto ph_map = MakePlaceholderMap(tree, std::array{ "a" });
	
	// 未知のプレースホルダ
	EXPECT_THROW(ParseRttiFuncNode<DTree>("x + y", ph_map), std::runtime_error);
	
	// 不正な式
	EXPECT_THROW(ParseRttiFuncNode<DTree>("a +", ph_map), std::runtime_error);
	
	// 括弧の不一致
	EXPECT_THROW(ParseRttiFuncNode<DTree>("(a", ph_map), std::runtime_error);
}
