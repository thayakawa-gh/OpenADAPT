#include <format>
#include <iostream>
#include <OpenADAPT/ADAPT.h>
#include <matplot/matplot.h>

using namespace adapt::lit;

template <adapt::container_simplex Tree>
auto GetCttiPlaceholders(const Tree& t)
{
	//フィールドの型と階層を静的に保持するCttiPlaceholderを取得する方法。
	//主としてSTreeで利用することを想定しているが、DTreeでも使うことはできる。
	//CttiPlaceholderはTree::CttiPlaceholder<Layer, Type>型である。
	//静的型情報などを持つ関係でRttiPlaceholderと異なりstd::vectorなどに纏めて格納することは出来ないが、
	//ラムダ関数の計算コストが最小限になる、アクセス時のキャストが不要などのメリットがある。

	//STree、DTreeで取得する方法が異なる。
	if constexpr (adapt::s_container<Tree>)
	{
		//STreeからCttiPlaceholderを取得するときは、名前をテンプレート引数で与えるか、
		//_fldのユーザー定義リテラルで与えると良い。
		auto phs = t.template GetPlaceholders<"class_", "grade", "number", "name", "exam", "jpn", "math", "eng">();
		[[maybe_unused]] auto phs2 = t.GetPlaceholders("class_"_fld, "grade"_fld, "number"_fld, "name"_fld, "exam"_fld, "jpn"_fld, "math"_fld, "eng"_fld);
		//以上はどちらも同じ結果を返す。phs/phs2はstd::tuple<CttiPlaceholders...>型。
		return phs;

		//なお、ADAPTはプレースホルダを取得するために次のようなヘルパーを提供している。
		ADAPT_GET_PLACEHOLDERS(t, class_, grade, number, name, exam, jpn, math, eng);
		//これは以下と同等である。
		//auto [class_, grade, number, name, exam, jpn, math, eng] =\
		//	t.GetPlaceholders("class_"_fld, "grade"_fld, "number"_fld, "name"_fld, "exam"_fld, "jpn"_fld, "math"_fld, "eng"_fld);
	}
	else if constexpr (adapt::d_container<Tree>)
	{
		//DTreeはもともと静的構造情報を持たないので、
		//こちらから明示的に型や階層を与えなければならない。
		//型や階層を間違えると例外MismatchTypeが投げられる。
		auto class_ = t.template GetPlaceholder<0, int8_t>("class_");
		auto grade = t.template GetPlaceholder<0, int8_t>("grade");
		auto number = t.template GetPlaceholder<1, int16_t>("number");
		auto name = t.template GetPlaceholder<1, std::string>("name");
		auto exam = t.template GetPlaceholder<2, int8_t>("exam");
		auto jpn = t.template GetPlaceholder<2, int32_t>("jpn");
		auto math = t.template GetPlaceholder<2, int32_t>("math");
		auto eng = t.template GetPlaceholder<2, int32_t>("eng");
		return std::make_tuple(class_, grade, number, name, exam, jpn, math, eng);
	}
}

template <class Tree>
void Access_ctti(const Tree& t)
{
	auto [class_, grade, number, name, exam, jpn, math, eng] = GetCttiPlaceholders(t);

	//静的型、階層情報を持つCttiPlaceholderを使いデータにアクセスする方法。
	//Rttiとは異なり、値の取得時にいちいちキャストする必要はない。

	//1. operator[]によるアクセス。
	//Treeでのデータ格納の際にも行ったが、
	//Treeの要素へアクセスする場合は、入れ子になったvectorのようなイメージで扱えば良い。
	auto dummy_dummy_ko = t[0][0];//0番目のクラス(3年1組に相当)の0番目の生徒（濃伊田美衣子）への参照を取得する。
	//dummy_dummy_koは参照オブジェクト。

	//CttiPlaceholderは静的型情報を持つので、DTreeとは異なり明示的にキャストする必要はない。
	assert(dummy_dummy_ko[name] == "濃伊田美衣子");

	//STreeのみ、フィールド名を直接与えても良い。ただし通常の文字列リテラルではなく_fldで与えること。
	//_fldはadapt::StaticString型へ変換するユーザー定義リテラルである。
	//_fldなしだとRttiPlaceholderを使ってアクセスすることになってしまい、
	//実行時の名前検索のコストがかかり、かつキャストが必要になる。
	if constexpr (adapt::s_container<Tree>)
		assert(dummy_dummy_ko["name"_fld] == "濃伊田美衣子");

	auto exam2 = dummy_dummy_ko[2];//後期中間試験への参照を取得する。
	assert(exam2[jpn] == 50);
	assert(exam2[math] == 15);
	assert(exam2[eng] == 44);


	//2.GetBranch関数を用いたアクセス。
	auto dummy_dummy_ko_ = t.GetBranch(0, 0);//0番目のクラスの0番目の生徒への参照。t[0][0]と同等。
	auto exam2_ = dummy_dummy_ko_[2];
	assert(exam2_[jpn] == 50);
	assert(exam2_[math] == 15);
	assert(exam2_[eng] == 44);

	//0~L層のインデックスをひとまとめにしたBposを使うこともできる。
	//Bposは0~2層までならBpos内部領域にインデックスを格納できるが、
	//3層以上のインデックスを納める場合は動的メモリ確保が発生することに注意されたい。
	adapt::Bpos bpos{ 0, 1 };//0~1層のインデックスを与える。
	auto kakuu_no_seito = t.GetBranch(bpos);//0番目のクラスの1番目の生徒への参照。t[0][1]と結果は同じだが、内部処理が若干遅い。
	assert(kakuu_no_seito[name] == "角兎野誠人");

	//bposは1層までのインデックスが入っているが、そのすべてを使う必要はない。
	//第2引数に階層Lを明示的に指定することで、bposのうち0~L層のインデックスを用いて参照を得ることができる。
	//なおt.GetBranch(bpos)はt.GetBranch(bpos, bpos.GetLayer())と同等。
	auto class_0 = t.GetBranch(bpos, 0_layer);//0番目のクラスへの参照。
	assert(class_0[grade] == 3);
	assert(class_0[class_] == 1);
}

//イテレータのようにコンテナを走査する。
template <class Tree>
void Traverse_ctti(const Tree& t)
{
	auto [class_, grade, number, name, exam, jpn, math, eng] = GetCttiPlaceholders(t);

	//ADAPTのコンテナはiteratorと似た振る舞いをするTraverserを利用して走査する。
	//Traverserは、階層構造を持ったコンテナを、
	//テーブルに展開されているかのように階層をまたいで走査する機能である。

	//Traverserは動的メモリ確保を伴う。
	//end()は何度呼んでも問題ないが、begin()を何度も呼ぶとその度に動的メモリ確保が発生するので、
	//呼び出しを最小限にすること。

	using ConstTraverser = typename Tree::ConstTraverser;
	auto range = t.GetRange(1_layer);//1層（=生徒）を走査するrangeを取得。
	for (ConstTraverser trav = range.begin(); trav != range.end(); ++trav)
	{
		//生徒一人ひとりを走査していく。
		//1層を走査している間は、1層以上のフィールドにはアクセスできる。
		//class_は0層フィールドだが1層より上なのでアクセスできる。
		int8_t class__ = trav[class_];
		int16_t number_ = trav[number];
		const std::string& name_ = trav[name];
		//int32_t math_ = e[math]; //エラー。1層を走査しているとき、2層以下のフィールドにはそのままではアクセスできない。
		int32_t math_1 = trav.GetField(math, 1);//2層のインデックスを明示的に与えればアクセスできる。今回は1番目の試験結果、つまり前期期末試験の数学の点数を得ている。

		/* GetField関数について
		* trav.GetField(ph)のように単純にプレースホルダのみを与えた場合、これはtrav[ph]と同等である。
		* trav.GetField(ph, i1, i2, ..., iN)のようにインデックスをN個与えることで、
		* 0-L-N層まではtravの位置を参照し、L-N+1層からL層までのインデックスを引数から指定して値を取得できる。
		* i1はL-N+1層、iNがL層のインデックスを意味する。
		*/

		//各生徒のクラス、出席番号、名前、後期中間数学の点数が表示される。
		std::cout <<
			std::format("class : {}, num : {}, name : {}, math_1 : {}",
						class__, number_, name_, math_1) <<
			std::endl;
	}

	//range-based for loopももちろん使える。
	//ConstTraverser::operator*の戻り値はConstTraverser自身。
	for (const ConstTraverser& trav : range)
	{
		std::cout << std::format("name : {}", trav[name]);
	}


	//operator++、--はどの階層を移動する場合でもすべて内部で自動的に処理されていた。
	//しかし、Move関数を使うと特定の階層を強制的に隣の要素へ移動させられる。
	//MoveForwardは次の要素、MoveBackwardは前の要素へ移動する。
	//移動に失敗する（空でない有効な要素が見つからなかった場合に起こる）とfalseが返る。成功した場合はtrueが返る。
	auto trav = t.GetRange(2_layer).begin();
	for (bool b0 = true; b0; b0 = trav.MoveForward(0_layer))
	{
		for (bool b1 = true; b1; b1 = trav.MoveForward(1_layer))
		{
			for (bool b2 = true; b2; b2 = trav.MoveForward(2_layer))
			{
				std::cout <<
					std::format("class : {}, num : {}, name : {}, math : {}",
								trav[class_], trav[number], trav[name], trav[math]) <<
					std::endl;
			}
		}
	}
}

//プレースホルダと様々な関数、演算子を組み合わせたラムダ関数による計算を行う方法。
template <class Tree>
void CalculateWithLambda_ctti(const Tree& t)
{
	auto [class_, grade, number, name, exam, jpn, math, eng] = GetCttiPlaceholders(t);

	//プレースホルダから何らかの値の計算を行うラムダ関数っぽいものを作成できる。

	//CttiPlaceholderを組み合わせて作られたラムダ関数は、adapt::eval::CttiFuncNode<...>型である。
	//すべての関数呼び出し順序や型情報などがテンプレートで保存されている。
	//したがって、RttiFuncNodeと異なり、std::vectorにまとめたりは出来ない。

	//ラムダ関数の戻り値は、フィールドと同じく属している階層が厳密に求まる。
	//最も基本的なルールは、"何層まで要素を指定すれば、その値を計算できるか"。
	//生徒一人を指定して計算できるなら1層。4回のうちのどの試験かまで指定する必要があれば2層。

	//ラムダ関数を呼ぶときの引数には、
	//1. Traverser
	//2. ConstTraverser
	//3. コンテナ（DTreeやSTree、JoinedContainerなど）と位置情報（Bpos）
	//4. コンテナのみ（戻り値が-1層の場合のみ）
	//のいずれかを与える。これらの位置情報はラムダ関数の階層以下まで十分に指定する必要がある。

	//ラムダ関数は、異なるコンテナに対して使い回してはならない。
	//また、ラムダ関数はTraverser、ConstTraverser、コンテナ+Bposの3通りの呼び方があるが、
	//繰り返し実行する際は初回実行時（または引数ありのInit実行時）に引数として与えたインスタンスを使い続けなければならない。
	//もし異なる引数型、異なるインスタンスで実行したい場合、実行前にsum_3subjs.Init()のように初期化する。


	//例1 シンプルな文字列の足し算。
	auto num_name = tostr(number) + ":" + name;//出席番号:名前という文字列を作るラムダ関数。
	static_assert(std::same_as<typename decltype(num_name)::RetType, std::string>);//結果はStr型。
	static_assert(num_name.GetLayer() == 1);//出席番号と名前は1層のフィールドであるため、その演算結果も1層。またCttiなので階層もコンパイル時に判定される。
	adapt::Bpos bpos = { 0, 0 };//0~1層のインデックス。インデックスは少なくともラムダ関数の階層まで必要。
	std::string dummyko_num_name = num_name(t, bpos);//濃伊田美衣子の名前と出席番号を得る。
	assert(dummyko_num_name == "0:濃伊田美衣子");


	//例2 ラムダ関数の合成。
	//ラムダ関数にさらに演算を加えたり、ラムダ関数同士を演算してもよい。
	//以下の例では、sum_3subjsはjpn + math + engと同等である。
	auto math_eng = math + eng;
	auto sum_3subjs = jpn + math_eng;//3科目の合計点を返すラムダ関数。
	static_assert(std::same_as<typename decltype(sum_3subjs)::RetType, int32_t>);//戻り値の型はint32_t + int32_tの結果に準拠している。
	static_assert(sum_3subjs.GetLayer() == 2);//各点数は2層のフィールドのため、その合計値も2層の値である。
	bpos = { 0, 0, 2 };//0～2層のインデックス。
	int32_t dummyko_exam2_sum_3subjs = sum_3subjs(t, bpos);//濃伊田美衣子の後期中間試験について、3科目合計点を取得する。
	assert(dummyko_exam2_sum_3subjs == 50 + 15 + 44);


	//例3 要素の位置を指定した計算。
	//at(i)とすると、プレースホルダの階層のうちi番目の要素から値を取得できる。at(1)であれば前期期末試験から値を取得する。
	auto sum_3subjs_1 = jpn.at(1) + math.at(1) + eng.at(1);
	static_assert(sum_3subjs_1.GetLayer() == 1);//at(i)と2層の位置を指定すると、与えた位置の数だけ階層が繰り上がる（iが定数でなくいずれかの階層に属す場合、この限りではない）。
	bpos = { 0, 0 };//bposは計算したいラムダ関数の階層以下まで指定されていればいいので、本当は再代入しなくともよい。
	int32_t dummyko_sum_3subjs_1 = sum_3subjs_1(t, bpos);
	assert(dummyko_sum_3subjs_1 == 65 + 21 + 35);
	//jpn.at(1, 2)のように、2個以上のインデックスを指定することもできる。
	//この場合、(1, 2)はぞれぞれ1層、2層のインデックスを意味する。
	//当然ながら、与えられるインデックスの数は、プレースホルダの階層分（L層ならL+1個まで）である。


	//例4 階層関数（要素の走査によって何らかの集計値を求める関数）。
	//ADAPTのラムダ関数には、sum、mean、greatestといった階層構造に作用する特殊な関数が用意されている。
	//今回、ある生徒一人を指定し、その生徒の試験4回での3科目合計の平均点を出したいとする。
	//このとき、mean(jpn + math + eng)とすると、3科目合計点の平均を出す、という意味になる。
	//この階層関数による算出の対象となる範囲は、
	//   階層関数の引数層から見て1個上の階層の要素を基準（固定層）とし、
	//   その要素に属す全ての引数層要素に対して階層関数を適用する
	//である。今回の例に当てはめると、
	//   meanの引数であるjpn + math + engの層(==2層)から見て1個上である生徒の層(==1層)の要素を基準として、
	//   その生徒の全ての試験結果を対象に平均値を求める。
	//という結果をもたらす。
	auto mean_sum_3subjs = mean(jpn + math + eng);//meanは平均点を求める階層関数。
	static_assert(mean_sum_3subjs.GetLayer() == 1_layer);//meanを行うと階層が1層分繰り上がる。
	bpos = { 0, 0 };//濃伊田美衣子の位置。
	int32_t mean_sum_3subjs_ = mean_sum_3subjs(t, bpos);
	assert(mean_sum_3subjs_ == 99);
	//と濃伊田美衣子の位置を指定すれば、濃伊田美衣子の3科目合計点の4試験分平均点を算出することができる。
	//ただし、meanは結果を引数と同じ型で返す。sum_3subjsは上でみたようにI32型なので、結果もI32型になってしまう。
	//これを回避するには、mean(cast_f64(sum_3subjs))のようにF64型にキャストするとよい。


	//例5 複数階層をまたぐ階層関数。
	//meanNのように階層関数に数字を付与した場合、固定層（走査範囲を決定する基準となる階層）は引数層から見てN層上になる。
	//例えば下記のものだと、引数層は前期期末試験における3科目合計点であるため、引数層は1である。
	//これをmean2で計算する場合、固定層は引数層から2層繰り上がり、-1層（最上位要素）となる。なお、meanはmean1と同等である。
	//この場合、最上位要素に属しているすべての前記期末試験が対象となり、すなわち全クラス全生徒の前期期末試験が対象となる。
	//つまり、以下のラムダ関数は全クラス全生徒について、前期期末試験の3科目合計点の平均を求めることになる。
	auto mean2_sum_3subjs_1 = mean2(jpn.at(1) + math.at(1) + eng.at(1));
	static_assert(mean2_sum_3subjs_1.GetLayer() == -1_layer);//戻り値層も-1層である。いちいちクラスや生徒を指定しなくとも計算可能。
	int32_t mean2_sum_3subjs_1_ = mean2_sum_3subjs_1(t);
	assert(mean2_sum_3subjs_1_ == 206);


	//例6 階層関数のネスト
	//階層関数exist + count。階層関数はネストすることもできる。
	//4回の試験のうち国数英合計240点以上を取ったことが生徒の数を求める。
	auto exist_3subjs_240 = exist(jpn + math + eng >= 180);//ある生徒が240点以上取ったことがあるかどうかを求めるラムダ関数。
	static_assert(exist_3subjs_240.GetLayer() == 1_layer);//この戻り値は1層。
	//既存のラムダ関数を使って新たなラムダ関数を作っても良い。
	//以下はauto count_student_240 = count2(exist(jpn + math + eng >= 180));と同等である。
	auto count_student_240 = count2(exist_3subjs_240);//240点以上取ったことのある生徒を全クラスを対象に数える。1層（生徒）から-1層（学年全体）へ2層分引き上げるので、count2を使う。
	static_assert(count_student_240.GetLayer() == -1_layer);//ちゃんと-1層になっている。
	int64_t num_student_240 = count_student_240(t);//-1層（学年全体）のラムダ関数呼び出しであるため、位置情報には意味がなく、bposを与える必要はない。与えても問題はないが。
	assert(num_student_240 == 2);//該当するのは角兎野誠人と子虚烏有花。


	//例7 階層関数の外側のフィールドの取得
	//階層関数はその内部でTraverserを生成し、要素一つ一つを走査しながら値を集計していく。
	//しかし、例えばこんな状況はどうだろう。
	//今、3年1組の生徒に対して、各試験の国語のクラス内順位を求めたくなったとする。
	//方法としては、count2(exam == {{参照中の試験}} && jpn > {{順位を求めたい生徒の点数}}) + 1のように、
	//クラス内でその生徒より高得点を取った人数 + 1を順位とすれば良さそうである。
	//しかし{{参照中の試験}}と{{順位を求めたい生徒の点数}}はどのように表記すればよいのか。
	//jpnは順位を求めたい生徒ではなく、count内のTraverserが現在指し示している生徒から取得されるので、適切ではない。
	//これを可能にするのが次の表記である。
	auto rank = count2(exam == exam.outer(0_depth) && jpn > jpn.outer(0_depth)) + 1;//Rttiと違い_depthが必要なことに注意。
	//階層関数は深度（depth）という値を持つ。階層関数外では0で、階層関数で囲われるごとに+1される。
	//例えばcount(exist(jpn > 80))という式では、jpn > 80の部分の深度が2となる。
	//上述のouterは、この深度を与えることで、
	//その深度に該当する位置情報からフィールドの値を取得することができる。
	//以下の例では、exam.outer(0_depth)とjpn.outer(0_depth)は{ 0, 0, 3 }という位置情報を参照して値を得る。
	static_assert(rank.GetLayer() == 2_layer);//2層であることに注意。どの試験かまで指定しないと計算できない。
	bpos = { 0, 0, 3 };//濃伊田美衣子の後期期末試験の位置。
	assert(rank(t, bpos) == 3);
	//将来的には、階層関数にもouterを付与できるようにしたい。これが必要な状況がいくらか考えられる。


	//例8 ユーザー定義関数
	//ラムダ関数中で使う関数をユーザーが定義することもできる。
	auto average = adapt::UserFunc([](int32_t a, int32_t b, int32_t c) { return (a + b + c) / 3.; });
	auto avg_3_subjs = average(jpn, math, eng);
	assert(avg_3_subjs(t, bpos) == (48 + 8 + 24) / 3.);
	//ジェネリックラムダのようなテンプレートを含む関数を使う場合は、必ずconceptやSFINAEを用いて制約しておくこと。
	//特にRttiの場合、FieldTypeに定義されている全ての型で呼び出しを試みるため、制約のない汎用引数の関数はコンパイルエラーに繋がる。
	auto constrained = adapt::UserFunc([](const auto& x, const auto& y) -> decltype(x + y) { return x + y; });
	auto sum_2_subjs = constrained(jpn, math);
	assert(sum_2_subjs(t, bpos) == (48 + 8));


	//例9 Rttiへの変換
	//CttiPlaceholderから作られたラムダ関数は実行される関数などの情報をstaticに持っており、
	//Rttiの場合と異なりラムダ関数ごとに型が異なる。
	//そのため通常はvectorに纏めたりすることは出来ないのだが、
	//RttiFuncNode（Rttiのラムダ関数の型）に変換することはできる。
	using RttiLambda = adapt::eval::RttiFuncNode<Tree>;
	RttiLambda rtti = ConvertToRttiFuncNode(mean(jpn + math + eng));
	assert(rtti(t, bpos).i32() == 99);
}

//ラムダ関数およびそれに基づくViewを活用した走査。
template <class Tree>
void TraverseWithLambda_ctti(const Tree& t)
{
	auto [class_, grade, number, name, exam, jpn, math, eng] = GetCttiPlaceholders(t);

	auto mean_math = mean(cast_f64(math));//ある生徒の試験4回の数学平均点。整数のままだと平均値も整数にされるので浮動小数点にキャストしている。
	auto mean_jpn = mean(cast_f64(jpn));//ある生徒の試験4回の数学平均点。整数のままだと平均値も整数にされるので浮動小数点にキャストしている。
	static_assert(std::is_same_v<typename decltype(mean_math)::RetType, double>);//戻り値の型はmeanの引数型と同じ。
	auto max_math = greatest(math);//試験4回のうち数学の最高点。
	auto min_math = least(math);//試験4回のうち数学の最低点。
	static_assert(std::is_same_v<typename decltype(max_math)::RetType, int32_t>);//戻り値の型はgreatestの引数型と同じ。

	//数学の平均点が60点以上の生徒のみ走査する。
	//このとき、走査層はFilterやTransformなどに与えられたプレースホルダ、ラムダ関数のうちの最下層に設定される。
	//今回、mean_math >= 60が1層のラムダ関数であるため、1層が走査される。
	//FilterにCttiのラムダ関数を与える場合は、戻り値がbool値として評価可能なものであれば良い。
	static_assert(std::same_as<typename decltype(mean_math >= 60.)::RetType, bool>);
	for (const auto& trav : t | Filter(mean_math >= 60.))
	{
		//Filterの条件を満たすのは角兎野誠人と子虚烏有花。
		std::cout << std::format("name:{}, mean_math:{}, max_math:{}, min_math:{}\n",
								 trav[name], mean_math(trav), max_math(trav), min_math(trav)) << std::endl;
	}

	//明示的に走査する階層を指定したい場合は、最後にGetRangeする。
	//ただし、与えたプレースホルダやラムダ関数の階層以下を指定しなければならない。
	//試験4回分の数学平均点が60点以上の生徒について、2層（各試験の成績）を走査する。
	//以下の2通りの指定でも、同等の結果になる。
	//1. t | GetRange(2_layer) | Filter(mean_math >= 60)
	//2. t.GetRange(2_layer) | Filter(mean_math >= 60)
	//実のところ、GetRangeは"走査階層を引数の値に引き下げる"という効果しか持たない。
	//（なぜそんな変な仕様なのかって？そのほうが実装が楽だったからだ！）
	//異なるTraverserインスタンスに対してラムダ関数を実行しようとしているので、
	//Initで初期化しておく。
	mean_math.Init(); max_math.Init();//InitAll(mean_math, max_math)でもよい。
	for (const auto& trav : t | Filter(mean_math >= 60) | GetRange(2_layer))
	{
		//角兎野誠人と子虚烏有花の数学平均点、最低点、最高点、各定期試験結果が表示される。
		std::cout << std::format("name:{}, mean_math:{}, max_math:{}, min_math:{}, math:{}\n",
								 trav[name], mean_math(trav), max_math(trav), min_math(trav), trav[math]);
	}

	//Filterに複数の条件を与えた場合、そのすべてを満たす要素だけを走査する。
	//これらを単にFilter(math >= 90 && mean_math >= 80)としても結果的には同等だが、
	//階層が異なる条件を個々に分けて与えることで、冗長な条件の計算が省略される。
	//例えばmean_math >= 80は1層の条件であるため、生徒一人に対してただ一度だけ計算すれば十分であり、
	//その一度の計算で条件が満足されなければ、いちいちmath >= 90を計算することなく次の生徒に移動できる。
	//なお、Filterに課した条件は、"階層関数中では適用されない"という性質がある。
	//そのため、階層関数meanを利用しているmean_mathの計算時はmath < 90やjpn < 80であるような試験結果も含めて計算される。
	mean_math.Init(); max_math.Init(); min_math.Init();
	for (const auto& trav : t | Filter(math >= 90, mean_math >= 80) | GetRange(2_layer))
	{
		std::cout << std::format("name:{}, mean_math:{}, max_math:{}, min_math:{}, math:{}\n",
			trav[name], mean_math(trav), max_math(trav), min_math(trav), trav[math]);
	}

	//Evaluateを使う例。
	//Evaluateを通すことで、このTraverserはoperator*によってラムダ関数の戻り値を返すようになる。
	//RttiPlaceholderやそららから成るラムダ関数の場合は、Evaluateに与える際に明示的に型を指定する必要があったが、
	//元々型情報を持っているCttiPlaceholderやそこから作られたラムダ式ではれば不要。
	//1引数であれば実際のところ、std::views::transformとほぼ同等である。
	for (double mean : t | Filter(mean_math >= 60) | Evaluate(mean_math) | GetRange(1_layer))
	{
		std::cout << std::format("mean_math:{}\n", mean);
	}
	//Evaluateに2個以上の引数を与えた場合、operator*の戻り値がstd::tuple<...>となる。
	//また、冗長な計算が省略されるようになる。
	for (auto [mean, math_] : t | Filter(mean_math >= 60 && mean_jpn >= 60) | Evaluate(mean_math, math) | GetRange(2_layer))
	{
		std::cout << std::format("mean_math:{}, math:{}\n", mean, math_);
	}

	//<ranges>のviewを利用することもできるが、
	//それ以降はADAPT独自のviewやExtractなどを連結させることができなくなる。
	//
	mean_math.Init();
	for (auto& trav : t | Filter(mean_math >= 60) | GetRange(2_layer) | std::views::take(3) /* | Evaluate(...) これはできない*/)
	{
		std::cout << std::format("name : {}, math : {}\n", trav[name], trav[math]);
	}

}

template <class Tree>
void Show_ctti(const Tree& t)
{
	auto [class_, grade, number, name, exam, jpn, math, eng] = GetCttiPlaceholders(t);

	//ラムダ関数についてはCalculateWithLambdaの項を参照。
	auto sum_3subjs = jpn + math + eng;
	auto greatest_3subjs = greatest(sum_3subjs);

	//o(a_depth)はouter(a_depth)の短縮版。
	auto rank = count3(exam == 0 && sum_3subjs > jpn.o(0_depth) + math.o(0_depth) + eng.o(0_depth)) + 1;

	//rの全てのフィールドの情報を表示するようにしたいが、未実装。
	//t | Show();

	//4回の試験の3科目合計点が180を超えたことのある生徒の情報を表示する。
	//Showに何の引数も与えない場合、全フィールドを表示するが、placeholderなどを与えるとそれらの情報だけを表示する。
	//ただし、表示できるのはFieldTypeに定義されているものに限られる。
	//Cttiのプレースホルダやラムダ関数の場合、FieldTypeに対応する型が存在しない独自の型だと表示できない。
	t | Filter(greatest_3subjs >= 180) | Show(number, name, greatest_3subjs, jpn, math, eng);
}

template <class Tree>
void Extract_ctti(const Tree& t)
{
	auto [class_, grade, number, name, exam, jpn, math, eng] = GetCttiPlaceholders(t);

	//Extractはプレースホルダまたはラムダ関数を与えることで、
	//それらをフィールドとして持つ新しいTreeを生成する。
	//すべてCttiPlaceholderまたはそこから成るラムダ関数を与えた場合、
	//すべてのフィールドの型と階層をコンパイル時に判定できるため、戻り値はSTreeとなる。
	auto t2 = t | Extract(name, jpn + math + eng);//t2はSTree。

	//Extractで生成されるTreeは、与えられたプレースホルダとラムダ関数の階層に応じた構造を自動的に定義する。
	//t2の構造は以下の通りである。
	//-1層、0層 : なし
	// 1層 : 生徒の名前（Str）
	// 2層 : 三科目合計点（I32）
	//また、特に名前を指定しない場合、これらのフィールドにはExtractに与えた順に"fld0"、"fld1"、"fld2"...という名前が与えられる。
	auto [name_e, sum_3subjs_e] = t2.template GetPlaceholders<"fld0", "fld1">();
	static_assert(decltype(name_e)::GetLayer() == 1);
	static_assert(std::same_as<typename decltype(name_e)::RetType, std::string>);
	static_assert(decltype(sum_3subjs_e)::GetLayer() == 2);
	static_assert(std::same_as<typename decltype(sum_3subjs_e)::RetType, int32_t>);

	//もしExtractするTreeのフィールドに自分で名前を与えたい場合、テンプレート引数で与えるか、_fldを使う。
	auto t3 = t | Extract(name.template named<"name">(), (jpn + math + eng).named("sum_3subjs"_fld));
	[[maybe_unused]] auto [name_e2, sum_3subjs_e2] = t3.template GetPlaceholders<"name", "sum_3subjs">();

	//namedの引数に一つでもstd::stringや実行時にしか決定できない形が含まれていると戻り値がDTreeになるので注意。
	[[maybe_unused]] adapt::DTree tx = t | Extract(name.named("name"), (jpn + math + eng).named("sum_3subjs"));//この書き方はアウト。

	//もちろん、Filterを通してもよい。三科目合計点が120を下回った試験結果のみを対象に、名前と合計点をフィールドとして持つTreeを生成する。
	[[maybe_unused]] auto t4 = t | Filter(jpn + math + eng < 120) | Extract(name.template named<"name">(), (jpn + math + eng).template named<"sum_3subjs">());

	//namedという関数を個々に書くのは冗長になりがちなため、次のようなヘルパーを用意している。
	auto t5 = t | Extract(ADAPT_NAMED_FIELDS(name, (jpn + math + eng)));
	//これは以下と同等である。
	//auto t6 = t | Extract(name.named("name"_fld), (jpn + math + eng).named("(jpn + math + eng)"_fld));
}

template <class Tree>
void Plot_ctti(const Tree& t)
{
	auto [class_, grade, number, name, exam, jpn, math, eng] = GetCttiPlaceholders(t);

	//ADAPTはデータ可視化のための機能を有するので、簡易的な図であればADAPT内で完結させることもできる。
	//この機能はGnuplotをバックエンドとして使用するので、予めインストールしておく必要がある。

	//Gnuplotのパスは次の順に探索される。
	// 1. Canvas::SetGnuplotPathで設定されたパス。
	// 2. 環境変数GNUPLOT_PATHがある場合、その値を使う。
	// 3. Windowsであれば"C:/Progra~1/gnuplot/bin/gnuplot.exe"、その他OSではパスが通っていると想定し"gnuplot"とする。

	//adapt::Canvas::SetGnuplotPath("path/to/gnuplot");

	//戻り値はstd::tuple<std::vector<double>, std::vector<double>>;
	auto [v_jpn, v_eng, v_math] = t | Filter(exam == 0) | ToVector(jpn, eng, math);

	namespace plot = adapt::plot;
	std::string tree_type = adapt::s_tree<Tree> ? "stree" : "dtree";
	adapt::Canvas2D f(std::format("scatter_jpn_eng_ctti_{}.png", tree_type));
	f.SetTitle("jpn vs eng/math");
	f.SetXLabel("jpn");
	f.SetYLabel("eng / math");
	f.SetXRange(0, 100);
	f.SetYRange(0, 100);
	//PlotPointsなどの関数は任意のrangeを受け取ることができる。
	//そのため、v_jpn等の代わりにt | Filter(exam == 0) | Evaluate(jpn)等としてもよい。
	f.PlotPoints(v_jpn, v_eng, plot::c_blue, plot::pointtype = 7, plot::ps_large, plot::title = "jpn vs eng").
		PlotPoints(v_jpn, v_math, plot::c_red, plot::pointtype = 7, plot::ps_large, plot::title = "jpn vs math");

	std::vector<std::string> exams = { "1set-sem-mid", "1st-sem-fin", "2nd-sem-mid", "2nd-sem-fin" };
	auto dummyko_math = t | Filter(name == "濃伊田美衣子") | ToVector(math);
	auto seito_math = t | Filter(name == "角兎野誠人") | ToVector(math);
	auto yuuka_math = t | Filter(name == "子虚烏有花") | ToVector(math);
	adapt::Canvas2D f2(std::format("dummyko_math_ctti_{}.png", tree_type));
	f2.SetTitle("math result");
	f2.SetYLabel("math score");
	f2.SetXTicsRotate(-45);
	f2.SetXRange(-0.5, 3.5);
	f2.SetYRange(0, 100);
	f2.PlotPoints(exams, dummyko_math, plot::title = "dummyko",
				  plot::s_linespoints, plot::pt_fcircle, plot::ps_med_large, plot::c_dark_magenta).
		PlotPoints(exams, seito_math, plot::title = "seito",
				   plot::s_linespoints, plot::pt_ftri, plot::ps_med_large, plot::c_dark_cyan).
		PlotPoints(exams, yuuka_math, plot::title = "yuuka",
				   plot::s_linespoints, plot::pt_fbox, plot::ps_med_large, plot::c_dark_yellow);

	//この他、より複雑な描画を行いたい場合はMatplot++などを使うこともできる。
	//各々好ましいものを選択して使うとよい。
}
