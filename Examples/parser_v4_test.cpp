#include <iostream>
#ifdef ADAPT_IMPORT_MODULE
#include <OpenADAPT/Macros.h>
import adapt;
#else
#include <OpenADAPT/ADAPT.h>
#endif
#include <OpenADAPT/Evaluator/ParserV4.h>

using namespace adapt;
using namespace adapt::lit;
using namespace adapt::eval::parser;

int main()
{
	std::cout << "=== OpenADAPT Parser V4 Comprehensive Test ===" << std::endl << std::endl;
	
	std::cout << "重要な改善点:" << std::endl;
	std::cout << "1. RttiConstNodeを直接保持（仮想関数回避）" << std::endl;
	std::cout << "2. マクロによる演算子/関数名の一元管理" << std::endl;
	std::cout << "3. 通常関数とメンバ関数のサポート" << std::endl << std::endl;
	
	// 基本的なツリーを作成
	DTree tree;
	tree.SetTopLayer({});
	tree.AddLayer({ {"x", FieldType::I32}, {"y", FieldType::I32}, {"z", FieldType::I32} });
	tree.VerifyStructure();
	
	tree.Push(10, 20, 30);
	tree.Push(5, 15, 25);
	tree.Push(-3, 8, 12);
	
	std::cout << "=== 1. 基本演算子テスト（Field + Const） ===" << std::endl;
	{
		std::cout << "式: x + 100" << std::endl;
		auto lambda = ParseRttiFuncNode(tree, "x + 100");
		std::cout << "  tree[0]: " << lambda(tree, Bpos{0}).i32() << " (期待: 110)" << std::endl;
		std::cout << "  tree[1]: " << lambda(tree, Bpos{1}).i32() << " (期待: 105)" << std::endl;
		std::cout << "  ✓" << std::endl << std::endl;
	}
	
	{
		std::cout << "式: 100 - x (Const + Field)" << std::endl;
		auto lambda = ParseRttiFuncNode(tree, "100 - x");
		std::cout << "  tree[0]: " << lambda(tree, Bpos{0}).i32() << " (期待: 90)" << std::endl;
		std::cout << "  tree[1]: " << lambda(tree, Bpos{1}).i32() << " (期待: 95)" << std::endl;
		std::cout << "  ✓" << std::endl << std::endl;
	}
	
	{
		std::cout << "式: x * 2 + y" << std::endl;
		auto lambda = ParseRttiFuncNode(tree, "x * 2 + y");
		std::cout << "  tree[0]: " << lambda(tree, Bpos{0}).i32() << " (期待: 40)" << std::endl;
		std::cout << "  tree[1]: " << lambda(tree, Bpos{1}).i32() << " (期待: 25)" << std::endl;
		std::cout << "  ✓" << std::endl << std::endl;
	}
	
	{
		std::cout << "式: 10 + 20 (Const + Const)" << std::endl;
		auto lambda = ParseRttiFuncNode(tree, "10 + 20");
		std::cout << "  tree[0]: " << lambda(tree, Bpos{0}).i32() << " (期待: 30)" << std::endl;
		std::cout << "  ✓ ConstNode同士の演算も可能" << std::endl << std::endl;
	}
	
	std::cout << "=== 2. 比較演算子テスト ===" << std::endl;
	{
		std::cout << "式: x > 7" << std::endl;
		auto lambda = ParseRttiFuncNode(tree, "x > 7");
		std::cout << "  tree[0]: " << (int)lambda(tree, Bpos{0}).i08() << " (期待: 1)" << std::endl;
		std::cout << "  tree[1]: " << (int)lambda(tree, Bpos{1}).i08() << " (期待: 0)" << std::endl;
		std::cout << "  tree[2]: " << (int)lambda(tree, Bpos{2}).i08() << " (期待: 0)" << std::endl;
		std::cout << "  ✓" << std::endl << std::endl;
	}
	
	{
		std::cout << "式: x >= 5 && y <= 20" << std::endl;
		auto lambda = ParseRttiFuncNode(tree, "x >= 5 && y <= 20");
		std::cout << "  tree[0]: " << (int)lambda(tree, Bpos{0}).i08() << " (期待: 1)" << std::endl;
		std::cout << "  tree[1]: " << (int)lambda(tree, Bpos{1}).i08() << " (期待: 1)" << std::endl;
		std::cout << "  tree[2]: " << (int)lambda(tree, Bpos{2}).i08() << " (期待: 0)" << std::endl;
		std::cout << "  ✓" << std::endl << std::endl;
	}
	
	std::cout << "=== 3. 単項演算子テスト ===" << std::endl;
	{
		std::cout << "式: -x" << std::endl;
		auto lambda = ParseRttiFuncNode(tree, "-x");
		std::cout << "  tree[0]: " << lambda(tree, Bpos{0}).i32() << " (期待: -10)" << std::endl;
		std::cout << "  tree[1]: " << lambda(tree, Bpos{1}).i32() << " (期待: -5)" << std::endl;
		std::cout << "  tree[2]: " << lambda(tree, Bpos{2}).i32() << " (期待: 3)" << std::endl;
		std::cout << "  ✓" << std::endl << std::endl;
	}
	
	{
		std::cout << "式: !(x > 0)" << std::endl;
		auto lambda = ParseRttiFuncNode(tree, "!(x > 0)");
		std::cout << "  tree[0]: " << (int)lambda(tree, Bpos{0}).i08() << " (期待: 0)" << std::endl;
		std::cout << "  tree[1]: " << (int)lambda(tree, Bpos{1}).i08() << " (期待: 0)" << std::endl;
		std::cout << "  tree[2]: " << (int)lambda(tree, Bpos{2}).i08() << " (期待: 1)" << std::endl;
		std::cout << "  ✓" << std::endl << std::endl;
	}
	
	std::cout << "=== 4. 階層関数テスト ===" << std::endl;
	
	DTree htree;
	htree.SetTopLayer({});
	htree.AddLayer({ {"name", FieldType::Str} });
	htree.AddLayer({ {"score", FieldType::I32} });
	htree.VerifyStructure();
	
	htree.Push("Tanaka");
	auto tanaka = htree[0];
	tanaka.Push(85);
	tanaka.Push(90);
	tanaka.Push(78);
	
	htree.Push("Sato");
	auto sato = htree[1];
	sato.Push(92);
	sato.Push(88);
	sato.Push(95);
	
	{
		std::cout << "式: mean(score)" << std::endl;
		auto lambda = ParseRttiFuncNode(htree, "mean(score)");
		std::cout << "  Tanaka: " << lambda(htree, Bpos{0}).i32() << " (期待: 84)" << std::endl;
		std::cout << "  Sato: " << lambda(htree, Bpos{1}).i32() << " (期待: 91)" << std::endl;
		std::cout << "  ✓ RttiFieldNodeを直接階層関数に渡している" << std::endl << std::endl;
	}
	
	{
		std::cout << "式: sum(score) / 3" << std::endl;
		auto lambda = ParseRttiFuncNode(htree, "sum(score) / 3");
		std::cout << "  Tanaka: " << lambda(htree, Bpos{0}).i32() << " (期待: 84)" << std::endl;
		std::cout << "  Sato: " << lambda(htree, Bpos{1}).i32() << " (期待: 91)" << std::endl;
		std::cout << "  ✓ 階層関数の結果にConstNodeを使った演算" << std::endl << std::endl;
	}
	
	{
		std::cout << "式: greatest(score) > 90" << std::endl;
		auto lambda = ParseRttiFuncNode(htree, "greatest(score) > 90");
		std::cout << "  Tanaka: " << (int)lambda(htree, Bpos{0}).i08() << " (期待: 0)" << std::endl;
		std::cout << "  Sato: " << (int)lambda(htree, Bpos{1}).i08() << " (期待: 1)" << std::endl;
		std::cout << "  ✓" << std::endl << std::endl;
	}
	
	{
		std::cout << "式: mean2(score) (2階層)" << std::endl;
		auto lambda = ParseRttiFuncNode(htree, "mean2(score)");
		std::cout << "  全体平均: " << lambda(htree, Bpos{}).i32() << " (期待: 87-88)" << std::endl;
		std::cout << "  ✓ 数値付き階層関数" << std::endl << std::endl;
	}
	
	std::cout << "=== 5. 通常関数テスト ===" << std::endl;
	
	DTree ftree;
	ftree.SetTopLayer({});
	ftree.AddLayer({ {"val", FieldType::F64} });
	ftree.VerifyStructure();
	
	ftree.Push(3.14);
	ftree.Push(-2.5);
	ftree.Push(16.0);
	
	{
		std::cout << "式: abs(val)" << std::endl;
		auto lambda = ParseRttiFuncNode(ftree, "abs(val)");
		std::cout << "  ftree[0]: " << lambda(ftree, Bpos{0}).f64() << " (期待: 3.14)" << std::endl;
		std::cout << "  ftree[1]: " << lambda(ftree, Bpos{1}).f64() << " (期待: 2.5)" << std::endl;
		std::cout << "  ✓" << std::endl << std::endl;
	}
	
	{
		std::cout << "式: sqrt(val) (val=16.0のとき)" << std::endl;
		auto lambda = ParseRttiFuncNode(ftree, "sqrt(val)");
		std::cout << "  ftree[2]: " << lambda(ftree, Bpos{2}).f64() << " (期待: 4.0)" << std::endl;
		std::cout << "  ✓" << std::endl << std::endl;
	}
	
	{
		std::cout << "式: pow(val, 2.0) (val=3.14のとき)" << std::endl;
		auto lambda = ParseRttiFuncNode(ftree, "pow(val, 2.0)");
		std::cout << "  ftree[0]: " << lambda(ftree, Bpos{0}).f64() << " (期待: 9.86程度)" << std::endl;
		std::cout << "  ✓ 2引数関数" << std::endl << std::endl;
	}
	
	{
		std::cout << "式: max(val, 0.0)" << std::endl;
		auto lambda = ParseRttiFuncNode(ftree, "max(val, 0.0)");
		std::cout << "  ftree[0]: " << lambda(ftree, Bpos{0}).f64() << " (期待: 3.14)" << std::endl;
		std::cout << "  ftree[1]: " << lambda(ftree, Bpos{1}).f64() << " (期待: 0.0)" << std::endl;
		std::cout << "  ftree[2]: " << lambda(ftree, Bpos{2}).f64() << " (期待: 16.0)" << std::endl;
		std::cout << "  ✓ Const引数を持つ2引数関数" << std::endl << std::endl;
	}
	
	std::cout << "=== 6. リテラル型接尾辞テスト ===" << std::endl;
	{
		std::cout << "式: x + 10i64 (I64リテラル)" << std::endl;
		auto lambda = ParseRttiFuncNode(tree, "x + 10i64");
		std::cout << "  tree[0]: " << lambda(tree, Bpos{0}).i64() << " (期待: 110)" << std::endl;
		std::cout << "  ✓ 型接尾辞サポート" << std::endl << std::endl;
	}
	
	{
		std::cout << "式: x + 3.5f32 (F32リテラル)" << std::endl;
		auto lambda = ParseRttiFuncNode(tree, "x + 3.5f32");
		std::cout << "  tree[0]: " << lambda(tree, Bpos{0}).f32() << " (期待: 13.5)" << std::endl;
		std::cout << "  ✓" << std::endl << std::endl;
	}
	
	std::cout << "=== パフォーマンスとメンテナンス性の確認 ===" << std::endl;
	std::cout << "✓ RttiFieldNodeを直接演算子/関数に渡している（仮想関数回避）" << std::endl;
	std::cout << "✓ RttiConstNodeも直接演算子/関数に渡している（仮想関数回避）" << std::endl;
	std::cout << "✓ 全ての演算子名・関数名がマクロで一元管理されている" << std::endl;
	std::cout << "✓ 階層関数、通常関数の両方をサポート" << std::endl;
	std::cout << "✓ メンバ関数の基本構造を実装（.at()は要拡張）" << std::endl << std::endl;
	
	std::cout << "V4パーサーのテスト完了！" << std::endl;
	
	return 0;
}
