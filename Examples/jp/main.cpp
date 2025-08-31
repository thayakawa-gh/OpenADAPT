#include "examples_join.h"
#include "examples_rtti.h"
#include "examples_ctti.h"
#include "examples_plot.h"

adapt::DTree MakeDTree()
{
	//階層構造とフィールド（列のようなもの）を実行時に決定するDTreeの作成方法。
	//DTreeはインスタンス作成後に関数を呼び出して構造を定義する。
	//イメージ的には、Fields + std::vector<Fields + std::vector<Fields + std::vector<Fields>>>;のように、
	//各階層のフィールドとその下層要素を持つvectorの入れ子構造。

	//DTreeの場合、利用可能な型は以下のものに制限される。STreeの場合は任意の型（制限あり）の値を格納できる。
	//整数型 : I08（int8_t）、I16（int16_t）、I32（int32_t）、I64（int64_t）
	//浮動小数点型 : F32（float）、F64（double）
	//複素数 : C32（std::complex<float>）、C64（std::complex<double>）
	//文字列 : Str（std::string）
	//インデックス : Jbp（adapt::JBpos） ... 各層のインデックスをまとめたようなもの。
	//たぶんそのうちstd::chrono::year_month_dayなどが追加される気がする。

	using enum adapt::FieldType;
	adapt::DTree t;
	t.SetTopLayer({ { "school", Str } });//-1層のフィールド。学校名。-1層は不要なら設定しなくても良い。
	t.AddLayer({ { "grade", I08 }, { "class_", I08 } });//0層のフィールド。学年とクラス（注1）。
	t.AddLayer({ { "number", I16 }, { "name", Str } });//1層のフィールド。生徒の出席番号と名前（注1）。
	t.AddLayer({ { "exam", I08 },  {"jpn", I32}, {"math", I32}, {"eng", I32}, {"sci", I32}, {"soc", I32 } });//2層のフィールド。五教科の試験成績（注1）。
	t.VerifyStructure();
	return t;
}

auto MakeSTree()
{
	//階層構造とフィールド（列のようなもの）をstaticに定義するSTree。
	//実行時に定義するDTreeと異なり、デフォルト構築可能な任意のオブジェクトを格納できる（はず。テスト不十分……）。
	//イメージ的には、Fields + std::vector<Fields + std::vector<Fields + std::vector<Fields>>>;のように、
	//各階層のフィールドとその下層要素を持つvectorの入れ子構造。

	using enum adapt::FieldType;
	using adapt::NamedTuple;
	using adapt::Named;
	using TopLayer = NamedTuple<Named<"school", std::string>>;
	using Layer0 = NamedTuple<Named<"grade", int8_t>, Named<"class_", int8_t>>;
	using Layer1 = NamedTuple<Named<"number", int16_t>, Named<"name", std::string>>;
	using Layer2 = NamedTuple<Named<"exam", int8_t>, Named<"jpn", int32_t>, Named<"math", int32_t>, Named<"eng", int32_t>, Named<"sci", int32_t>, Named<"soc", int32_t>>;
	using STree_ = adapt::STree<TopLayer, Layer0, Layer1, Layer2>;

	STree_ t;
	return t;
}

template <class Tree>
void StoreData(Tree& t)
{
	//データの格納はSTree、DTreeで基本的に同じ。
	t.SetTopFields("胴差県立散布流中学校");//-1層のフィールドを代入する。

	t.Reserve(1);//1クラス分のスペースを用意（注2）。std::vector::reserveみたいなもの。
	t.Push((int8_t)3, (int8_t)1);//3年1組。それぞれ"grade"と"class_"の値として収まる。std::vector::push_backみたいなもの。

	//DTreeの場合、Push_unsafeを使うと型のチェックや分岐をしなくなるため、Pushよりも動作が高速になる。
	//ただし、DTreeの保持する型と厳密に一致している必要がある。例えばI16のフィールドに代入するときは正しくint16_t型で与えなければならない。
	//Str型の場合も、文字列リテラルやchar*ではなくstd::stringに変換して与えなければならない。

	auto cls_1 = t[0];//3年1組の要素への参照を取得。
	cls_1.Reserve(3);//3年1組に生徒3人分のスペースを用意（注2）。限界集落。
	cls_1.Push((int16_t)0, "濃伊田美衣子");//出席番号と名前を格納。
	cls_1.Push((int16_t)1, "角兎野誠人");
	cls_1.Push((int16_t)2, "子虚烏有花");

	auto dummy_dummy_ko = cls_1[0];//濃伊田美衣子の要素への参照。
	dummy_dummy_ko.Reserve(4);//前期中間、前期期末、後期中間、後期期末の順に4回分のスペースを用意（注2）。
	dummy_dummy_ko.Push((int8_t)0, 46, 10, 32, 3, 29);//国数英理社の成績。前期中間。
	dummy_dummy_ko.Push((int8_t)1, 65, 21, 35, 0, 18);//前期期末。
	dummy_dummy_ko.Push((int8_t)2, 50, 15, 44, 22, 37);//後期中間。
	dummy_dummy_ko.Push((int8_t)3, 48, 8, 24, 11, 26);//後期期末。

	auto kakuu_no_seito = cls_1[1];//角兎野誠人の要素への参照。
	kakuu_no_seito.Reserve(4);//前期中間、前期期末、後期中間、後期期末の順に4回分のスペースを用意（注2）。
	kakuu_no_seito.Push((int8_t)0, 76, 91, 88, 94, 69);//国数英理社の成績。前期中間。
	kakuu_no_seito.Push((int8_t)1, 80, 99, 74, 85, 71);//前期期末。
	kakuu_no_seito.Push((int8_t)2, 72, 95, 72, 93, 75);//後期中間。
	kakuu_no_seito.Push((int8_t)3, 84, 89, 77, 83, 82);//後期期末。

	auto shikyo_uyuu_ka = cls_1[2];//子虚烏有花の要素への参照。
	shikyo_uyuu_ka.Push((int8_t)0, 98, 71, 85, 64, 99);//国数英理社の成績。前期中間。
	shikyo_uyuu_ka.Push((int8_t)1, 86, 69, 91, 70, 100);//前期期末。
	shikyo_uyuu_ka.Push((int8_t)2, 90, 75, 78, 80, 92);//後期中間。
	shikyo_uyuu_ka.Push((int8_t)3, 94, 67, 81, 76, 96);//後期期末。


	//（注1）I08はint8_t、I16はint16_t、I32はint32_t、Strはstd::stringに相当する。
	//（注2）最大でstd::numeric_limits<uint32_t>::max()～43億弱まで作成可能。
	//      42億のクラスそれぞれに42億人の生徒が所属しており、各個人にそれぞれ42億回分の定期試験成績を収めることも、
	//      無尽蔵のメモリを扱えるPCであれば原理的には可能。
	//      ただし逆に言えば、43億件以上のデータを格納したい場合、階層化が必須となる。
}

inline adapt::DTree MakeDTree2()
{
	//連結するコンテナのインスタンスを作成する。
	using enum adapt::FieldType;
	adapt::DTree t;
	t.SetTopLayer({ { "city", Str} });//-1層のフィールド。学校名。-1層は不要なら設定しなくても良い。
	t.AddLayer({ { "household", I32 }, { "family_name", Str} });//0層のフィールド。世帯番号と名字。
	t.AddLayer({ { "name", Str }, {"relationship", Str}, {"income", I32} });//1層のフィールド。名前、続柄、年収。
	t.VerifyStructure();
	return t;
}

template <class Tree>
void StoreData2(Tree& t)
{
	//連結するコンテナのデータを格納する。
	t.SetTopFields("胴差県散布流村");

	t.Reserve(3);
	t.Push(0, "隅州");//隅州家
	t.Push(1, "角兎野");//角兎野家
	t.Push(2, "濃伊田");//濃伊田家

	auto smith = t[0];
	smith.Reserve(3);
	smith.Push("隅州父", "本人", 3500000);//名前、続柄、年収
	smith.Push("隅州母", "妻", 5000000);
	smith.Push("隅州慈恩", "子", 0);

	auto kakuu_no = t[1];
	kakuu_no.Reserve(6);
	kakuu_no.Push("角兎野祖父", "本人", 1400000);
	kakuu_no.Push("角兎野祖母", "妻", 700000);
	kakuu_no.Push("角兎野父", "子", 6000000);
	kakuu_no.Push("角兎野母", "子の妻", 1000000);
	kakuu_no.Push("角兎野誠人", "子の子", 0);
	kakuu_no.Push("角兎野妹", "子の子", 0);

	auto dummy_du = t[2];
	dummy_du.Reserve(2);
	dummy_du.Push("濃伊田父", "夫", 4000000);
	dummy_du.Push("濃伊田母", "本人", 4000000);
	dummy_du.Push("濃伊田姉", "子", 0);
	dummy_du.Push("濃伊田美衣子", "子", 0);
}

void QuickStart_dtree()
{
	std::cout << "--------QuickStart_dtree--------" << std::endl;

	adapt::DTree t = MakeDTree();
	StoreData(t);

	auto [name, jpn, math, eng] = t.GetPlaceholders("name", "jpn", "math", "eng");
	auto dummy_dummy_ko = t[0][0];
	std::cout << dummy_dummy_ko[name].str() << std::endl;//濃伊田美衣子
	//str_unsafe()など_unsafeのキャスト関数を使うと、型チェックが省略されるので多少高速である。安全が保証されるときはこちらを。
	std::cout << dummy_dummy_ko[name].str_unsafe() << std::endl;

	//プレースホルダからラムダ関数を作ることができる。
	using Lambda = adapt::eval::RttiFuncNode<adapt::DTree>;
	Lambda total_3subjs = jpn + math + eng;
	std::cout << total_3subjs(t, adapt::Bpos{ 0, 0, 1 }).i32() << std::endl;//121。濃伊田美衣子の前期期末の3科目合計点。

	Lambda exist_240 = exist(total_3subjs >= 240);
	for (auto& trav : t | Filter(exist_240) | GetRange(2_layer))
	{
		//3科目合計240点以上を取ったことのある生徒、
		//つまり角兎野誠人と子虚烏有花の、
		//各試験での3科目合計点が表示される。
		std::cout << std::format("{}: {}\n", trav[name].str(), total_3subjs(trav).i32());
		//角兎野誠人: 255
		//角兎野誠人: 253
		//角兎野誠人: 239   240点を下回っているが"一度でも240点以上を取ったことがあれば条件を満たす"ので、表示される。
		//角兎野誠人: 250
		//子虚烏有花: 254
		//子虚烏有花: 246
		//子虚烏有花: 243
		//子虚烏有花: 242
	}

	//前期中間試験で数学のクラス内最高点を取った生徒のインデックス、名前、点数を表示する。
	t | Filter(isgreatest(math.at(0))) | Show(name, math.at(0));//[   0,   1]  角兎野誠人     91

	//英語で35点未満を取った生徒の名前とその点数をフィールドとして持つ新しいDTreeを生成する。
	//eには1層に生徒名、その生徒の英語の最高点と最低点、2層に各試験の英語の点数が格納される。
	adapt::DTree e = t | Filter(eng < 35) | Extract(name, eng, greatest(eng), least(eng));
	auto [fld0, fld1, fld2, fld3] = e.GetPlaceholders("fld0", "fld1", "fld2", "fld3");
	e | Show(fld0, fld1, fld2, fld3);
	//[   0,   0,   0] 濃伊田美衣子          32          44          24
	//[   0,   0,   1] 濃伊田美衣子          24          44          24

	// Canvas2Dでヒストグラムを描画する。
	namespace plot = adapt::plot;
	std::vector<double> v_jpn = t | ToVector(cast_f64(jpn.at(2)).f64());
	adapt::Canvas2D g("quickstart_dtree.png");
	// 100-120点の範囲を6区間に分けてヒストグラムを描画する。
	// 最後のビンは100点のみが含まれる。
	g.PlotHistogram(v_jpn, 0, 120, 6, plot::notitle);
}

void QuickStart_stree()
{
	std::cout << "--------QuickStart_stree--------" << std::endl;

	using STree = decltype(MakeSTree());
	STree t = MakeSTree();
	StoreData(t);

	auto [name, jpn, math, eng] = t.GetPlaceholders<"name", "jpn", "math", "eng">();
	auto dummy_dummy_ko = t[0][0];
	std::cout << dummy_dummy_ko[name] << std::endl;//濃伊田美衣子

	//プレースホルダからラムダ関数を作ることができる。
	auto total_3subjs = jpn + math + eng;
	std::cout << total_3subjs(t, adapt::Bpos{ 0, 0, 1 }) << std::endl;//121。濃伊田美衣子の前期期末の3科目合計点。

	auto exist_240 = exist(total_3subjs >= 240);
	for (auto& trav : t | Filter(exist_240) | GetRange(2_layer))
	{
		//3科目合計240点以上を取ったことのある生徒、
		//つまり角兎野誠人と子虚烏有花の、
		//各試験での3科目合計点が表示される。
		std::cout << std::format("{}: {}\n", trav[name], total_3subjs(trav));
		//角兎野誠人: 255
		//角兎野誠人: 253
		//角兎野誠人: 239   240点を下回っているが"一度でも240点以上を取ったことがあれば条件を満たす"ので、表示される。
		//角兎野誠人: 250
		//子虚烏有花: 254
		//子虚烏有花: 246
		//子虚烏有花: 243
		//子虚烏有花: 242
	}

	//前期中間試験で数学のクラス内最高点を取った生徒のインデックス、名前、点数を表示する。
	t | Filter(isgreatest(math.at(0))) | Show(name, math.at(0));//[   0,   1]  角兎野誠人     91

	//英語で35点未満を取った生徒の名前とその点数をフィールドとして持つ新しいSTreeを生成する。
	//eには1層に生徒名、その生徒の英語の最高点と最低点、2層に各試験の英語の点数が格納される。
	auto e = t | Filter(eng < 35) | Extract(name, eng, greatest(eng), least(eng));
	auto [fld0, fld1, fld2, fld3] = e.GetPlaceholders<"fld0", "fld1", "fld2", "fld3">();
	e | Show(fld0, fld1, fld2, fld3);
	//[   0,   0,   0] 濃伊田美衣子          32          44          24
	//[   0,   0,   1] 濃伊田美衣子          24          44          24

	// Canvas2Dでヒストグラムを描画する。
	namespace plot = adapt::plot;
	std::vector<double> v_jpn = t | ToVector(cast_f64(jpn.at(2)));
	adapt::Canvas2D g("quickstart_stree.png");
	// 100-120点の範囲を6区間に分けてヒストグラムを描画する。
	// 最後のビンは100点のみが含まれる。
	g.PlotHistogram(v_jpn, 0, 120, 6, plot::notitle);
}

void QuickStart_join()
{
	std::cout << "--------QuickStart_join--------" << std::endl;

	adapt::DTree t1 = MakeDTree();
	StoreData(t1);
	adapt::DTree t2 = MakeDTree2();
	StoreData2(t2);

	//t1の1層要素と、t2の0層要素を連結する。
	//t2の0層はjtの1層、t2の1層はjtの2層要素として認識される。
	auto jt = Join(t1, 1_layer, 0_layer, t2);
	auto key_right = t2.GetPlaceholder("name");
	auto key_left = jt.GetPlaceholder<0_rank>("name");
	//key_leftは、特にそれがStr型などコピーコストが大きい場合、可能な限り事前に型を与えておくことを推奨する。
	jt.SetKeyJoint<1_rank>(key_left, key_right);

	auto [student_name, jpn, math, eng] = jt.GetPlaceholders<0_rank>("name", "jpn", "math", "eng");
	auto [resident_name, income] = jt.GetPlaceholders<1_rank>("name", "income");

	std::cout << jt.GetBranch(adapt::Bpos{ 0, 0, 2 }, 2_layer).GetField(resident_name).str() << std::endl;//濃伊田姉
	auto household_income = sum(income);
	std::cout << household_income(jt, adapt::Bpos{ 0, 1 }).i32() << std::endl;//9100000。角兎野家の世帯年収。

	{
		auto range = jt.GetRange(2_layer);
		for (auto& trav : range | Filter(income >= 3000000))
		{
			std::cout << std::format("{}: {}\n", trav[resident_name].str(), trav[income].i32());
			//生徒の家族のうち、年収3000000以上のもの。
			//通常のGetRangeで得たTraverserはinner join相当の動作をするので、
			//t1にない隅州家、t2にない子虚烏家はスキップされる。
			//濃伊田父: 4000000
			//濃伊田母: 4000000
			//角兎野父: 6000000
		}
	}

	auto mean_math = mean(cast_f64(math));
	household_income.Init();
	{
		auto range = jt.GetRange_delayed(1_layer);
		for (auto& trav : range)
		{
			//GetRange_delayedで得たTraverserはleft joinに近い動作が可能。
			if (trav.TryJoin(1_rank))
			{
				std::cout << std::format("{}: math {}, income {}\n",
					trav[student_name].str(), mean_math(trav).f64(), household_income(trav).i32());
				//濃伊田美衣子: math 13.5, income 8000000
				//角兎野誠人: math 93.5, income 9100000
			}
			else
			{
				//子虚烏家の情報はt2に存在しないため、Joinに失敗する。
				std::cout << std::format("{}: math {}, income nodata\n",
					trav[student_name].str(), mean_math(trav).f64());
				//子虚烏有花: math 70.5, income nodata
			}
		}
	}

	//Extract、Showなどの動作はDTreeと概ね同じ。ただしdelayed jointで動作することに注意。
	//eには1層に生徒名、その生徒の英語の最高点と最低点、2層に各住人の名前と収入が格納される。
	adapt::DTree e = jt | Filter(exist(eng < 30)) | Extract(student_name, resident_name, income, greatest(eng), least(eng));
	auto [fld0, fld1, fld2, fld3, fld4] = e.GetPlaceholders("fld0", "fld1", "fld2", "fld3", "fld4");
	e | Show(fld0, fld1, fld2, fld3, fld4);
	//[   0,   0,   0] 濃伊田美衣子     濃伊田父     4000000          44          24
	//[   0,   0,   1] 濃伊田美衣子     濃伊田母     4000000          44          24
	//[   0,   0,   2] 濃伊田美衣子     濃伊田姉           0          44          24
	//[   0,   0,   3] 濃伊田美衣子     濃伊田美衣子           0          44          24
}

int main()
{
	//ADAPTにおいて重要な用語として、以下のものがある。
	//コンテナ : データを格納する入れ物。テーブルのようなフラットなものではなく、階層構造を持つ。日本-都道府県-市区町村みたいな。
	//要素 : 表計算系ツールで言うところの"行"。都道府県や市町村のいずれか一つを指すもの。ただし階層構造を持つので、一つの要素の中にさらに多数の下層要素が内包される。
	//フィールド : 表計算系ツールで言うところの"列"。各要素の中に含まれる。その市区町村内の人口や都道府県名など。
	//プレースホルダ : 各フィールドに対応する代数のようなもの。コンテナ中のフィールドにアクセスしたり、ラムダ関数を構築したりするのに使う。
	//ラムダ関数 : プレースホルダを組み合わせて構築する、フィールドの値から何かを計算するためのもの。
	//走査 : コンテナの階層構造を横断するように要素一つ一つを参照していくこと。iteratorに似たTraverserというものを使う。

	//ADAPTはDTree、STreeという2種のコンテナと、Rtti、Ctti、Typedという3種のプレースホルダを組み合わせて動作する。
	//DTree/Stree、Rtti/Ctti/Typedの使い方はほぼ同じだが、微妙に異なるところがある。

	//DTreeは階層構造を関数呼び出しによって動的に定義する。
	//一方STreeは、テンプレート引数で静的に定義する。

	//RttiPlaceholderは静的型情報が削除されているので、vectorなどで管理しやすいが、
	//ラムダ関数計算コストは相対的に大きく、また値取得時はキャストが必要である。

	//CttiPlaceholderは静的型情報を持つのでキャストなどは不要であるし動作も少し速いが、
	//個々のテンプレート引数に型情報などが保存されているので管理は手間である。

	//TypdPlaceholderはRttiPlaceholderに静的型情報を付与することで作成される。
	//CttiPlaceholderに近いが、静的階層情報を利用できないので総合的なパフォーマンスはCtti>Typed>Rttiとなる（はず）。

	//基本的には、DTreeではRtti/TypedPlaceholder、STreeでCttiPlaceholderを利用することを想定しているが、
	//どの組み合わせでも動作するよう設計している。


	//Rtti、Cttiの各関数はそれぞれexamples_rtti.h、examples_ctti.hに、
	//Join関連の関数はexamples_join.hに定義されている。

	//0. クイックスタートガイド
	QuickStart_dtree();
	QuickStart_stree();
	QuickStart_join();

	//1. DTree/STreeの取り扱い。
	//  1.1. コンテナクラス
	//    1.1a. DTreeのインスタンス生成
	adapt::DTree dtree = MakeDTree();
	//    1.1b. STreeのインスタンス生成
	auto stree = MakeSTree();

	//  1.2. データの格納
	StoreData(dtree);
	StoreData(stree);

	//  1.3. データへのアクセス
	//    1.3a. 動的型情報で扱う方法
	Access_rtti(dtree);
	Access_rtti(stree);
	//    1.3b. 静的型情報を付与して扱う方法
	Access_ctti(dtree);
	Access_ctti(stree);

	//  1.4. 走査
	//    1.4a. 動的型情報で扱う方法
	Traverse_rtti(dtree);
	Traverse_rtti(stree);
	//    1.4b. 静的型情報を付与して扱う方法
	Traverse_ctti(dtree);
	Traverse_ctti(stree);

	//  1.5. ラムダ関数による計算
	//    1.5a. 動的型情報で扱う方法
	CalculateWithLambda_rtti(dtree);
	CalculateWithLambda_rtti(stree);
	//    1.5b. 静的型情報を付与して扱う方法
	CalculateWithLambda_ctti(dtree);
	CalculateWithLambda_ctti(stree);

	//  1.6. ラムダ関数を利用した走査
	//    1.6a. 動的型情報で扱う方法
	TraverseWithLambda_rtti(dtree);
	TraverseWithLambda_rtti(stree);
	//    1.6b. 静的型情報を付与して扱う方法
	TraverseWithLambda_ctti(dtree);
	TraverseWithLambda_ctti(stree);

	//  1.7. コンソール上に表示
	//    1.7a. 動的型情報で扱う方法
	Show_rtti(dtree);
	Show_rtti(stree);
	//    1.7b. 静的型情報を付与して扱う方法
	Show_ctti(dtree);
	Show_ctti(stree);

	//  1.8. サブサンプルの抽出
	//    1.8a. 動的型情報で扱う方法
	Extract_rtti(dtree);
	Extract_rtti(stree);
	//    1.8b. 静的型情報を付与して扱う方法
	Extract_ctti(dtree);
	Extract_ctti(stree);

	//  1.9. OpenADAPT/Plot/Canvas.hを用いたプロット
	//    1.9a. 動的型情報で扱う方法
	Plot_rtti(dtree);
	Plot_rtti(stree);
	//    1.9b. 静的型情報を付与して扱う方法
	Plot_ctti(dtree);
	Plot_ctti(stree);
	//    1.9c. 多様なプロット方法
	PlotVariations();

	//  1.10. Matplot++によるプロット
	//    1.10a. 動的型情報で扱う方法
	//Matplot_rtti(dtree);
	//Matplot_rtti(stree);
	//    1.10b. 静的型情報を付与して扱う方法
	//Matplot_ctti(dtree);
	//Matplot_ctti(stree);

	//2. Join（複数コンテナの疑似結合）
	adapt::DTree dtree_join = MakeDTree2();
	StoreData2(dtree_join);
	auto jt = Join(dtree, dtree_join);

	//  2.1 JoinedContainerの取り扱い。
	AccessAndCalc_jtree(jt);
	Traverse_jtree(jt);
	Plot_jtree(jt);


}