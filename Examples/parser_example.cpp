#include <iostream>
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

int main()
{
	std::cout << "Starting..." << std::endl;
	// DTreeを作成
	DTree tree;
	using enum adapt::FieldType;
	std::cout << "Created DTree" << std::endl;
	
	// トップ層(-1層)には何もなし、0層にデータを持つ
	tree.SetTopLayer({});
	tree.AddLayer({ {"x", I32}, {"y", I32}, {"z", I32} });
	std::cout << "Set structure" << std::endl;
	tree.VerifyStructure();
	std::cout << "Verified structure" << std::endl;
	
	// データを設定
	tree.Push(10, 20, 30);
	std::cout << "Pushed row 0" << std::endl;
	tree.Push(5, 15, 25);
	std::cout << "Pushed row 1" << std::endl;
	
	// プレースホルダを取得
	std::cout << "Getting placeholders..." << std::endl;
	auto [x, y, z] = tree.GetPlaceholders("x", "y", "z");
	std::cout << "Got placeholders" << std::endl;
	
	// プレースホルダマップを作成
	std::map<std::string, DTree::RttiPlaceholder> ph_map;
	ph_map["x"] = x;
	ph_map["y"] = y;
	ph_map["z"] = z;
	
	// パーサーを使って式をパース
	std::cout << "=== OpenADAPT Parser Example ===" << std::endl << std::endl;
	
	// 例1: 簡単な加算
	{
		std::cout << "式: x + y" << std::endl;
		auto lambda = ParseRttiFuncNode<DTree>("x + y", ph_map);
		std::cout << "  tree[0]: " << lambda(tree, Bpos{ 0 }).i32() << " (期待値: 30)" << std::endl;
		std::cout << "  tree[1]: " << lambda(tree, Bpos{ 1 }).i32() << " (期待値: 20)" << std::endl;
		std::cout << std::endl;
	}
	
	// 例2: 複合式
	{
		std::cout << "式: (x + y) * z" << std::endl;
		auto lambda = ParseRttiFuncNode<DTree>("(x + y) * z", ph_map);
		std::cout << "  tree[0]: " << lambda(tree, Bpos{ 0 }).i32() << " (期待値: 900)" << std::endl;
		std::cout << "  tree[1]: " << lambda(tree, Bpos{ 1 }).i32() << " (期待値: 500)" << std::endl;
		std::cout << std::endl;
	}
	
	// 例3: 演算子の優先順位
	{
		std::cout << "式: x + y * z" << std::endl;
		auto lambda = ParseRttiFuncNode<DTree>("x + y * z", ph_map);
		std::cout << "  tree[0]: " << lambda(tree, Bpos{ 0 }).i32() << " (期待値: 610)" << std::endl;
		std::cout << "  tree[1]: " << lambda(tree, Bpos{ 1 }).i32() << " (期待値: 380)" << std::endl;
		std::cout << std::endl;
	}
	
	// 例4: 比較演算子
	{
		std::cout << "式: x < y" << std::endl;
		auto lambda = ParseRttiFuncNode<DTree>("x < y", ph_map);
		std::cout << "  tree[0]: " << (int)lambda(tree, Bpos{ 0 }).i08() << " (期待値: 1=true)" << std::endl;
		std::cout << "  tree[1]: " << (int)lambda(tree, Bpos{ 1 }).i08() << " (期待値: 1=true)" << std::endl;
		std::cout << std::endl;
	}
	
	// 例5: 論理演算子
	{
		std::cout << "式: x < y && y < z" << std::endl;
		auto lambda = ParseRttiFuncNode<DTree>("x < y && y < z", ph_map);
		std::cout << "  tree[0]: " << (int)lambda(tree, Bpos{ 0 }).i08() << " (期待値: 1=true)" << std::endl;
		std::cout << "  tree[1]: " << (int)lambda(tree, Bpos{ 1 }).i08() << " (期待値: 1=true)" << std::endl;
		std::cout << std::endl;
	}
	
	// 例6: 単項マイナス
	{
		std::cout << "式: -x + y" << std::endl;
		auto lambda = ParseRttiFuncNode<DTree>("-x + y", ph_map);
		std::cout << "  tree[0]: " << lambda(tree, Bpos{ 0 }).i32() << " (期待値: 10)" << std::endl;
		std::cout << "  tree[1]: " << lambda(tree, Bpos{ 1 }).i32() << " (期待値: 10)" << std::endl;
		std::cout << std::endl;
	}
	
	std::cout << "パーサーのデモンストレーションが完了しました。" << std::endl;
	
	return 0;
}
