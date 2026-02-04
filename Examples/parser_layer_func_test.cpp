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
	// 階層構造を持つDTreeを作成
	DTree tree;
	using enum adapt::FieldType;
	
	// トップ層(-1層): クラス情報なし
	// 0層: 生徒情報 (name)
	// 1層: 試験結果 (score)
	tree.SetTopLayer({});
	tree.AddLayer({ {"name", Str} });
	tree.AddLayer({ {"score", I32} });
	tree.VerifyStructure();
	
	// 生徒1: 田中 (試験3回分のスコア)
	tree.Push("Tanaka");
	auto tanaka = tree[0];
	tanaka.Push(85);  // 試験1
	tanaka.Push(90);  // 試験2
	tanaka.Push(78);  // 試験3
	
	// 生徒2: 佐藤 (試験3回分のスコア)
	tree.Push("Sato");
	auto sato = tree[1];
	sato.Push(92);  // 試験1
	sato.Push(88);  // 試験2
	sato.Push(95);  // 試験3
	
	std::cout << "=== 階層関数テスト ===" << std::endl << std::endl;
	
	// 例1: mean() - 各生徒の平均点を計算
	try {
		std::cout << "式: mean(score)" << std::endl;
		std::cout << "説明: 各生徒の全試験の平均点を計算" << std::endl;
		auto lambda = ParseRttiFuncNode(tree, "mean(score)");
		
		std::cout << "  Tanaka: " << lambda(tree, Bpos{ 0 }).i32() << " (期待値: 84)" << std::endl;
		std::cout << "  Sato: " << lambda(tree, Bpos{ 1 }).i32() << " (期待値: 91)" << std::endl;
		std::cout << "  ✓ 成功" << std::endl;
	} catch (const std::exception& e) {
		std::cout << "  ✗ エラー: " << e.what() << std::endl;
	}
	std::cout << std::endl;
	
	// 例2: sum() - 各生徒の合計点を計算
	try {
		std::cout << "式: sum(score)" << std::endl;
		std::cout << "説明: 各生徒の全試験の合計点を計算" << std::endl;
		auto lambda = ParseRttiFuncNode(tree, "sum(score)");
		
		std::cout << "  Tanaka: " << lambda(tree, Bpos{ 0 }).i32() << " (期待値: 253)" << std::endl;
		std::cout << "  Sato: " << lambda(tree, Bpos{ 1 }).i32() << " (期待値: 275)" << std::endl;
		std::cout << "  ✓ 成功" << std::endl;
	} catch (const std::exception& e) {
		std::cout << "  ✗ エラー: " << e.what() << std::endl;
	}
	std::cout << std::endl;
	
	// 例3: count() - 試験の回数を数える
	try {
		std::cout << "式: count(score)" << std::endl;
		std::cout << "説明: 各生徒の試験回数を数える" << std::endl;
		auto lambda = ParseRttiFuncNode(tree, "count(score)");
		
		std::cout << "  Tanaka: " << lambda(tree, Bpos{ 0 }).i64() << " (期待値: 3)" << std::endl;
		std::cout << "  Sato: " << lambda(tree, Bpos{ 1 }).i64() << " (期待値: 3)" << std::endl;
		std::cout << "  ✓ 成功" << std::endl;
	} catch (const std::exception& e) {
		std::cout << "  ✗ エラー: " << e.what() << std::endl;
	}
	std::cout << std::endl;
	
	// 例4: greatest() - 最高点を取得
	try {
		std::cout << "式: greatest(score)" << std::endl;
		std::cout << "説明: 各生徒の最高点を取得" << std::endl;
		auto lambda = ParseRttiFuncNode(tree, "greatest(score)");
		
		std::cout << "  Tanaka: " << lambda(tree, Bpos{ 0 }).i32() << " (期待値: 90)" << std::endl;
		std::cout << "  Sato: " << lambda(tree, Bpos{ 1 }).i32() << " (期待値: 95)" << std::endl;
		std::cout << "  ✓ 成功" << std::endl;
	} catch (const std::exception& e) {
		std::cout << "  ✗ エラー: " << e.what() << std::endl;
	}
	std::cout << std::endl;
	
	// 例5: least() - 最低点を取得
	try {
		std::cout << "式: least(score)" << std::endl;
		std::cout << "説明: 各生徒の最低点を取得" << std::endl;
		auto lambda = ParseRttiFuncNode(tree, "least(score)");
		
		std::cout << "  Tanaka: " << lambda(tree, Bpos{ 0 }).i32() << " (期待値: 78)" << std::endl;
		std::cout << "  Sato: " << lambda(tree, Bpos{ 1 }).i32() << " (期待値: 88)" << std::endl;
		std::cout << "  ✓ 成功" << std::endl;
	} catch (const std::exception& e) {
		std::cout << "  ✗ エラー: " << e.what() << std::endl;
	}
	std::cout << std::endl;
	
	// 例6: exist() - 条件を満たす要素の存在をチェック
	try {
		std::cout << "式: exist(score > 90)" << std::endl;
		std::cout << "説明: 90点超えの試験が存在するかチェック" << std::endl;
		auto lambda = ParseRttiFuncNode(tree, "exist(score > 90)");
		
		std::cout << "  Tanaka: " << (int)lambda(tree, Bpos{ 0 }).i08() << " (期待値: 0)" << std::endl;
		std::cout << "  Sato: " << (int)lambda(tree, Bpos{ 1 }).i08() << " (期待値: 1)" << std::endl;
		std::cout << "  ✓ 成功" << std::endl;
	} catch (const std::exception& e) {
		std::cout << "  ✗ エラー: " << e.what() << std::endl;
	}
	std::cout << std::endl;
	
	// 例7: 階層関数with番号 - mean2() で全生徒の平均を計算
	try {
		std::cout << "式: mean2(score)" << std::endl;
		std::cout << "説明: 全生徒全試験の平均点（2層上がる）" << std::endl;
		auto lambda = ParseRttiFuncNode(tree, "mean2(score)");
		
		std::cout << "  全体: " << lambda(tree).i32() << " (期待値: 88)" << std::endl;
		std::cout << "  ✓ 成功" << std::endl;
	} catch (const std::exception& e) {
		std::cout << "  ✗ エラー: " << e.what() << std::endl;
	}
	std::cout << std::endl;
	
	// 例8: 複合式 - 階層関数と算術演算の組み合わせ
	try {
		std::cout << "式: mean(score) + 10" << std::endl;
		std::cout << "説明: 平均点に10点加算" << std::endl;
		auto lambda = ParseRttiFuncNode(tree, "mean(score) + 10");
		
		std::cout << "  Tanaka: " << lambda(tree, Bpos{ 0 }).i32() << " (期待値: 94)" << std::endl;
		std::cout << "  Sato: " << lambda(tree, Bpos{ 1 }).i32() << " (期待値: 101)" << std::endl;
		std::cout << "  ✓ 成功" << std::endl;
	} catch (const std::exception& e) {
		std::cout << "  ✗ エラー: " << e.what() << std::endl;
	}
	std::cout << std::endl;
	
	std::cout << "階層関数テスト完了！" << std::endl;
	
	return 0;
}
