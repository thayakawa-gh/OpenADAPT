#include <iostream>
#ifdef ADAPT_IMPORT_MODULE
#include <OpenADAPT/Macros.h>
import adapt;
#else
#include <OpenADAPT/ADAPT.h>
#endif
#include <OpenADAPT/Evaluator/ParserV2.h>

using namespace adapt;
using namespace adapt::lit;
using namespace adapt::eval::parser;

int main()
{
	// DTreeを作成
	DTree tree;
	using enum adapt::FieldType;
	
	// トップ層(-1層)には何もなし、0層にデータを持つ
	tree.SetTopLayer({});
	tree.AddLayer({ {"x", I32}, {"y", I32}, {"z", I32} });
	tree.VerifyStructure();
	
	// データを設定
	tree.Push(10, 20, 30);
	tree.Push(5, 15, 25);
	
	// 新しいAPI: プレースホルダマップ不要！
	std::cout << "=== OpenADAPT Parser V2 Example ===" << std::endl << std::endl;
	std::cout << "新しいAPI: ParseRttiFuncNode(tree, \"expression\")" << std::endl << std::endl;
	
	// 例1: 簡単な加算
	{
		std::cout << "式: x + y" << std::endl;
		auto lambda = ParseRttiFuncNode(tree, "x + y");
		std::cout << "  tree[0]: " << lambda(tree, Bpos{ 0 }).i32() << " (期待値: 30)" << std::endl;
		std::cout << "  tree[1]: " << lambda(tree, Bpos{ 1 }).i32() << " (期待値: 20)" << std::endl;
		std::cout << std::endl;
	}
	
	// 例2: 複合式
	{
		std::cout << "式: (x + y) * z" << std::endl;
		auto lambda = ParseRttiFuncNode(tree, "(x + y) * z");
		std::cout << "  tree[0]: " << lambda(tree, Bpos{ 0 }).i32() << " (期待値: 900)" << std::endl;
		std::cout << "  tree[1]: " << lambda(tree, Bpos{ 1 }).i32() << " (期待値: 500)" << std::endl;
		std::cout << std::endl;
	}
	
	// 例3: C++準拠の演算子優先順位
	{
		std::cout << "式: x + y * z (演算子優先順位: C++準拠)" << std::endl;
		auto lambda = ParseRttiFuncNode(tree, "x + y * z");
		std::cout << "  tree[0]: " << lambda(tree, Bpos{ 0 }).i32() << " (期待値: 610)" << std::endl;
		std::cout << "  tree[1]: " << lambda(tree, Bpos{ 1 }).i32() << " (期待値: 380)" << std::endl;
		std::cout << std::endl;
	}
	
	// 例4: 比較演算子
	{
		std::cout << "式: x < y" << std::endl;
		auto lambda = ParseRttiFuncNode(tree, "x < y");
		std::cout << "  tree[0]: " << (int)lambda(tree, Bpos{ 0 }).i08() << " (期待値: 1=true)" << std::endl;
		std::cout << "  tree[1]: " << (int)lambda(tree, Bpos{ 1 }).i08() << " (期待値: 1=true)" << std::endl;
		std::cout << std::endl;
	}
	
	// 例5: 論理演算子
	{
		std::cout << "式: x < y && y < z" << std::endl;
		auto lambda = ParseRttiFuncNode(tree, "x < y && y < z");
		std::cout << "  tree[0]: " << (int)lambda(tree, Bpos{ 0 }).i08() << " (期待値: 1=true)" << std::endl;
		std::cout << "  tree[1]: " << (int)lambda(tree, Bpos{ 1 }).i08() << " (期待値: 1=true)" << std::endl;
		std::cout << std::endl;
	}
	
	// 例6: 単項マイナス
	{
		std::cout << "式: -x + y" << std::endl;
		auto lambda = ParseRttiFuncNode(tree, "-x + y");
		std::cout << "  tree[0]: " << lambda(tree, Bpos{ 0 }).i32() << " (期待値: 10)" << std::endl;
		std::cout << "  tree[1]: " << lambda(tree, Bpos{ 1 }).i32() << " (期待値: 10)" << std::endl;
		std::cout << std::endl;
	}
	
	std::cout << "=== 改善点テスト ===" << std::endl << std::endl;
	
	// 例7: 階層構造のテスト（階層関数は後で実装）
	std::cout << "階層関数のサポートは開発中..." << std::endl;
	
	try {
		// 階層関数のテスト（開発中）
		// auto lambda = ParseRttiFuncNode(tree, "sum(x)");
		// std::cout << "sum(x) result: " << lambda(tree).i32() << std::endl;
	} catch (const std::exception& e) {
		std::cout << "  エラー（予想通り）: " << e.what() << std::endl;
	}
	
	std::cout << std::endl << "V2パーサーのデモンストレーションが完了しました。" << std::endl;
	
	return 0;
}
