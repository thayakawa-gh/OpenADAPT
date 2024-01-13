#include <format>
#include <iostream>
#include <OpenADAPT/Container/Tree.h>
#include <OpenADAPT/Aggregator/Extractor.h>
#include <OpenADAPT/Aggregator/Filter.h>
#include <OpenADAPT/Aggregator/Transform.h>
#include <OpenADAPT/Aggregator/Output.h>

using namespace adapt::lit;

void LambdaFromString(const adapt::DTree& t)
{
	//正規フィールド（FieldInfoで定義されている型のフィールド）についてのみ、文字列からラムダ関数を構築可能にしたい。
	//例えば次のように。
	adapt::eval::RttiFuncNode<adapt::DTree> lambda = Lambda(t, "mean(cast_f64(math))");
	//これは以下と同等な結果を返す。
	adapt::DTree::RttiPlaceholder math = t.GetPlaceholder("math");
	adapt::eval::RttiFuncNode<adapt::DTree> lambda = mean(cast_f64(math));
	//こうすれば、いちいちプレースホルダを取得しなくとも良いし、何らかのアプリケーションに対して文字情報として条件入力が可能になる。
}

template <class STree1, class STree2>
void StaticJoin(const STree1& t1, const STree2& t2)
{
	//例えば以下のように、
	//連結層指定はLayerConstantしか受け付けず、かつCttiPlaceholderの使用を強要されるが、
	//Extractの結果がSTreeとなるようなJoin方法。
	//走査処理などもある程度コンパイル時に計算できるかもしれない。
	auto joined_tree = StaticJoin(t1, 1_layer, 0_layer, t2);
	auto [ctti_abc, ctti_def] = joined_tree.GetPlaceholders<"abc", "def">();
	auto ctti_lambda = ctti_abc + ctti_def;
	auto stree = joined_tree | Extract(ctti_abc, ctti_def, ctti_lambda);
	//streeはadapt::STree型で、構造はExtractの引数からコンパイル時に自動的に決定される。
}

void GroupBy(const adapt::DTree& t)
{
	//PandasのGroupByのような機能。
	//ADAPT v1系では下記のように、Extractの際にグループ化するプレースホルダやラムダ関数を与えていたが、
	//この方法で十分なのかは要検討。
	auto abc = t.GetPlaceholder("abc");
	auto [def, ghi] = t.GetPlaceholders("def", "ghi");
	std::vector<adapt::DTree> g = t | Extract(def, ghi).GroupBy(abc);
}

int main()
{
	//将来的な拡張計画

	LambdaFromString(t);
}