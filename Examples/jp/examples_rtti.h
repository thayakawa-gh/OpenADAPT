#include <format>
#include <iostream>
#include <OpenADAPT/ADAPT.h>

using namespace adapt::lit;

template <adapt::container_simplex Tree>
std::array<typename Tree::RttiPlaceholder, 8> GetRttiPlaceholders(const Tree& t)
{
	//静的型、階層情報を持たないRttiPlaceholderを取得する方法。
	//主としてDTreeで利用することになると想定しているが、STreeでも全く同様に使える。
	//RttiPlaceholderはTree:RttiPlaceholder型で統一されており、
	//それぞれのフィールドがどのような型を持ちどの階層に属すのかを、実行時に決定されるメンバ変数として保持する。
	//CttiPlaceholderと比較してラムダ関数使用時のパフォーマンスは不利だが、
	//プレースホルダやラムダ関数をstd::vectorなどに纏めて管理できる利点がある。

	return t.GetPlaceholders("class_", "grade", "number", "name", "exam", "jpn", "math", "eng");

	//STreeの場合、FieldTypeに定義されている型のフィールドに対してしか使えないことに注意。
	//例えばstd::vector<double>型を格納している"vec"という名前のフィールドがあったとすると、
	//そのRttiPlaceholderを取得しようとしてもMismatchTypeが投げられる。

	//なお、ADAPTはプレースホルダを取得するために次のようなヘルパーを提供している。
	[[maybe_unused]] ADAPT_GET_PLACEHOLDERS(t, class_, grade, number, name, exam, jpn, math, eng);
	//これは以下と同等である。
	//auto [class_, grade, number, name, exam, jpn, math, eng] =\
	//	t.GetPlaceholders("class_"_fld, "grade"_fld, "number"_fld, "name"_fld, "exam"_fld, "jpn"_fld, "math"_fld, "eng"_fld);
}

template <class Tree>
void Access_rtti(const Tree& t)
{
	//静的型、階層情報を持たないRttiPlaceholderを用いたデータへのアクセス方法。
	//Cttiとの大きな違いは、値取得時にキャストする必要があるという点。
	auto [class_, grade, number, name, exam, jpn, math, eng] = GetRttiPlaceholders(t);


	//1. operator[]によるアクセス。
	//Treeでのデータ格納の際にも行ったが、
	//Treeの要素へアクセスする場合は、入れ子になったvectorのようなイメージで扱えば良い。
	auto dummy_dummy_ko = t[0][0];//0番目のクラス(3年1組に相当)の0番目の生徒（濃伊田美衣子）への参照を取得する。
	//dummy_dummy_koは参照オブジェクト。

	//RttiPlaceholderは静的型情報を持たないので、str()のように明示的にキャストする必要がある。
	assert(dummy_dummy_ko[name].str() == "濃伊田美衣子");
	//プレースホルダの代わりにフィールド名を与えても良いが、文字列からの検索コストがかかる。
	assert(dummy_dummy_ko["name"].str() == "濃伊田美衣子");

	auto exam2 = dummy_dummy_ko[2];//後期中間試験への参照を取得する。
	assert(exam2[jpn].i32() == 50);
	assert(exam2[math].i32() == 15);
	assert(exam2[eng].i32() == 44);

	//RttiPlaceholderを、静的型情報を持つTypedPlaceholderに変換することができる。
	//Typedとか変な名前だなぁ。なんかいい名前があったら提案して。
	auto name_typed = t.GetPlaceholder("name").str();
	//name_typedの型はTree::TypedPlaceholder<std::string>。
	//auto name_typed = t.GetPlaceholder<adapt::FieldType::Str>("name");としてもよい。
	static_assert(std::same_as<typename decltype(name_typed)::RetType, std::string>);
	assert(dummy_dummy_ko[name_typed] == "濃伊田美衣子");


	//2.GetBranch関数を用いたアクセス。
	auto dummy_dummy_ko_ = t.GetBranch(0, 0);//0番目のクラスの0番目の生徒への参照。t[0][0]と同等。
	assert(dummy_dummy_ko_[name].str() == "濃伊田美衣子");

	//0~L層のインデックスをひとまとめにしたBposを使うこともできる。
	//Bposは0~2層までならBpos内部領域にインデックスを格納できるが、
	//3層以上のインデックスを納める場合は動的メモリ確保が発生することに注意されたい。
	adapt::Bpos bpos{ 0, 1 };//0~1層のインデックスを与える。
	auto kakuu_no_seito = t.GetBranch(bpos);//0番目のクラスの1番目の生徒への参照。t[0][1]と結果は同じだが、内部処理が若干遅い。
	assert(kakuu_no_seito[name].str() == "角兎野誠人");

	//bposは1層までのインデックスが入っているが、そのすべてを使う必要はない。
	//第2引数に階層Lを明示的に指定することで、bposのうち0~L層のインデックスを用いて参照を得ることができる。
	//なおt.GetBranch(bpos)はt.GetBranch(bpos, bpos.GetLayer())と同等。
	auto class_0 = t.GetBranch(bpos, 0_layer);//0番目のクラスへの参照。
	assert(class_0[grade].i08() == 3);
	assert(class_0[class_].i08() == 1);
}

//イテレータのようにコンテナを走査する。
template <class Tree>
void Traverse_rtti(const Tree& t)
{
	//tの各フィールドへアクセスするためのプレースホルダを取得。
	auto [class_, grade, number, name, exam, jpn, math, eng] = GetRttiPlaceholders(t);

	//ADAPTのコンテナはiteratorと似た振る舞いをするTraverserを利用して走査する。
	//Traverserは、階層構造を持ったコンテナを、
	//テーブルに展開されているかのように階層をまたいで走査する機能である。

	//Traverserは動的メモリ確保を伴う。
	//end()は何度呼んでも問題ないが、begin()を何度も呼ぶとその度に動的メモリ確保が発生するので、
	//呼び出しを最小限にすること。
	//また安易にコピーできないようコピーコンストラクタを削除している。
	//動的メモリ確保が必要なことを理解した上でコピーしたい場合は、CopyFromまたはClone関数を使うこと。

	using ConstTraverser = typename Tree::ConstTraverser;
	auto range = t.GetRange(1_layer);//1層（==生徒）を走査するrangeを取得。
	for (ConstTraverser trav = range.begin(); trav != range.end(); ++trav)
	{
		//生徒一人ひとりを走査していく。
		//1層を走査している間は、1層以上のフィールドにはアクセスできる。
		//class_は0層フィールドだが1層より上なのでアクセスできる。
		int8_t class__ = trav[class_].i08();
		int16_t number_ = trav[number].i16();
		static_assert(std::same_as<decltype(trav[name].str()), const std::string&>);
		const std::string& name_ = trav[name].str();
		//int32_t math_ = e[math].i32(); //エラー。1層を走査しているとき、2層以下のフィールドにはそのままではアクセスできない。
		int32_t math_1 = trav.GetField(math, 1).i32();//2層のインデックスを明示的に与えればアクセスできる。今回は1番目の試験結果、つまり前期期末試験の数学の点数を得ている。

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
	for (const ConstTraverser& trav : range)
	{
		std::cout << std::format("name : {}", trav[name].str()) << std::endl;;
	}


	//operator++、--はどの階層を移動する場合でもすべて内部で自動的に処理されていた。
	//しかし、Move関数を使うと特定の階層を強制的に隣の要素へ移動させられる。
	//MoveForwardは次の要素、MoveBackwardは前の要素へ移動する。
	//移動に失敗する（末尾に達した、空でない有効な要素が見つからなかった場合に起こる）とfalseが返る。成功した場合はtrueが返る。
	auto trav = t.GetRange(2_layer).begin();
	for (bool b0 = true; b0; b0 = trav.MoveForward(0_layer))
	{
		for (bool b1 = true; b1; b1 = trav.MoveForward(1_layer))
		{
			for (bool b2 = true; b2; b2 = trav.MoveForward(2_layer))
			{
				std::cout <<
					std::format("class : {}, num : {}, name : {}, math : {}",
						trav[class_].i08(), trav[number].i16(), trav[name].str(), trav[math].i32()) <<
					std::endl;
			}
		}
	}
}

//プレースホルダと様々な関数、演算子を組み合わせたラムダ関数による計算を行う方法。
template <class Tree>
void CalculateWithLambda_rtti(const Tree& t)
{
	//tの各フィールドへアクセスするためのプレースホルダを取得。
	auto [class_, grade, number, name, exam, jpn, math, eng] = GetRttiPlaceholders(t);

	//プレースホルダから何らかの値の計算を行うラムダ関数っぽいものを作成できる。

	//RttiPlaceholderを組み合わせて作られたラムダ関数は、adapt::eval::RttiFuncNode型である。
	//ラムダ関数自体は型情報を持たないが、プレースホルダの型に従ってちゃんと計算する。
	using Lambda = adapt::eval::RttiFuncNode<Tree>;

	//プレースホルダからラムダ関数を構築するときに使用可能な関数、演算子は
	//ADAPT/Evaluator/Function.hとADAPT/Evaluator/LayerFuncNode.hを参照。

	//ラムダ関数の戻り値は、フィールドと同じく属している階層が厳密に求まる。
	//最も基本的なルールは、"何層まで要素を指定すれば、その値を計算できるか"。
	//生徒一人を指定して計算できるなら1層。4回のうちのどの試験かまで指定する必要があれば2層。

	//ラムダ関数を呼ぶときの引数には、
	//1. Traverser
	//2. ConstTraverser
	//3. コンテナ（DTreeやSTree、JoinedContainerなど）と位置情報（Bpos）
	//4. コンテナのみ（戻り値が-1層の場合のみ）
	//のいずれかを与える。これらの位置情報はラムダ関数の階層以下まで十分に指定する必要がある。

	//ラムダ関数は、異なるコンテナのインスタンスに対して使い回してはならない。
	//また、ラムダ関数はTraverser、ConstTraverser、コンテナ+Bposの3通りの呼び方があるが、
	//繰り返し実行する際は初回実行時（または引数ありのInit実行時）に引数として与えたインスタンスを使い続けなければならない。
	//もし異なる引数型、異なるインスタンスで実行したい場合、実行前にsum_3subjs.Init()のように初期化する。


	//例1 シンプルな文字列の足し算。
	Lambda num_name = tostr(number) + ":" + name;//出席番号:名前という文字列を作るラムダ関数。
	assert(num_name.GetType() == adapt::FieldType::Str);//結果はStr型。
	assert(num_name.GetLayer() == 1);//出席番号と名前は1層のフィールドであるため、その演算結果も1層。
	adapt::Bpos bpos = { 0, 0 };
	std::string dummyko_num_name = num_name(t, bpos).str();//濃伊田美衣子の名前と出席番号を得る。
	assert(dummyko_num_name == "0:濃伊田美衣子");


	//例2 ラムダ関数の合成。
	//ラムダ関数にさらに演算を加えたり、ラムダ関数同士を演算してもよい。
	//以下の例では、sum_3subjsはjpn + (math + eng)と同等である。
	Lambda math_eng = math + eng;
	Lambda sum_3subjs = jpn + math_eng;//3科目の合計点を返すラムダ関数。
	assert(sum_3subjs.GetType() == adapt::FieldType::I32);//各点数がI32型なので、それを足した結果もI32。
	assert(sum_3subjs.GetLayer() == 2);//各点数は2層のフィールドのため、その合計値も2層の値である。
	bpos = { 0, 0, 2 };//0～2層のインデックス。
	int32_t dummyko_exam2_sum_3subjs = sum_3subjs(t, bpos).i32();//濃伊田美衣子の後期中間試験について、3科目合計点を取得する。
	assert(dummyko_exam2_sum_3subjs == 50 + 15 + 44);


	//例3 要素の位置を指定した計算。
	//at(i)とすると、プレースホルダの階層のうちi番目の要素から値を取得できる。at(1)であれば前期期末試験から値を取得する。
	Lambda sum_3subjs_1 = jpn.at(1) + math.at(1) + eng.at(1);
	assert(sum_3subjs_1.GetLayer() == 1);//at(i)と2層の位置を指定すると、与えた位置の数だけ階層が繰り上がる（iが定数でなくいずれかの階層に属す場合、この限りではない）。
	bpos = { 0, 0 };//bposは計算したいラムダ関数の階層以下まで指定されていればいいので、本当は再代入しなくともよい。
	int32_t dummyko_sum_3subjs_1 = sum_3subjs_1(t, bpos).i32();
	assert(dummyko_sum_3subjs_1 == 65 + 21 + 35);
	//jpn.at(1, 2)のように、2個以上のインデックスを指定することもできる。
	//この場合、(1, 2)はぞれぞれ1層、2層のインデックスを意味する。
	//当然ながら、与えられるインデックスの数は、プレースホルダの階層分（L層ならL+1個まで）である。


	//例4 階層関数（ある階層に属す要素の走査によって何らかの集計値を求める関数）。
	//ADAPTのラムダ関数には、sum、mean、greatestといった階層構造に作用する特殊な関数が用意されている。
	//階層関数の一覧は、ADAPT/Evaluator/LayerFuncNode.hの末尾の定義を参照。
	//今回、ある生徒一人を指定し、その生徒の試験4回での3科目合計の平均点を出したい場合、
	//mean(jpn + math + eng)とすればよい。
	//この階層関数による算出の対象となる範囲は、
	//   階層関数の引数の階層（引数層）から見て1個上の階層の要素を基準（固定層）とし、
	//   その要素に属す全ての引数層要素に対して階層関数を適用する
	//である。今回の例に当てはめると、
	//   meanの引数であるjpn + math + engの層(==2層)から見て1個上である生徒の層(==1層)の要素を基準として、
	//   その生徒の全ての試験結果を対象に平均値を求める。
	//という結果をもたらす。
	Lambda mean_sum_3subjs = mean(jpn + math + eng);//meanは平均点を求める階層関数。
	assert(mean_sum_3subjs.GetLayer() == 1_layer);//meanを行うと階層が1層分繰り上がる（特殊な使い方をすれば繰り上がらないこともありうる）。
	bpos = { 0, 0 };//濃伊田美衣子の位置。
	int32_t mean_sum_3subjs_ = mean_sum_3subjs(t, bpos).i32();
	assert(mean_sum_3subjs_ == 99);
	//と濃伊田美衣子の位置を指定すれば、濃伊田美衣子の3科目合計点の4試験分平均点を算出することができる。
	//ただし、meanは結果を引数と同じ型で返す。sum_3subjsは上でみたようにI32型なので、結果もI32型になってしまう。
	//これを回避するには、mean(cast_f64(jpn + math + eng))のようにF64型にキャストするとよい。


	//例5 複数階層をまたぐ階層関数。
	//meanNのように階層関数に1以上の整数Nを付与した場合、固定層（走査範囲を決定する基準となる階層）は引数層から見てN層上になる。
	//例えば下記のものだと、引数層は前期期末試験における3科目合計点であるため、引数層は1である。
	//これをmean2で計算する場合、固定層は引数層から2層繰り上がり、-1層（学年全体）となる。
	//この場合、最上位要素に属しているすべての前記期末試験が対象となり、すなわち全クラス全生徒の前期期末試験が対象となる。
	//つまり、以下のラムダ関数は全クラス全生徒について、前期期末試験の3科目合計点の平均を求めることになる。
	//なお、mean1はmeanと同等である。
	Lambda mean2_sum_3subjs_1 = mean2(jpn.at(1) + math.at(1) + eng.at(1));
	assert(mean2_sum_3subjs_1.GetLayer() == -1_layer);//戻り値層も-1層である。いちいちクラスや生徒を指定しなくとも計算可能。
	int32_t mean2_sum_3subjs_1_ = mean2_sum_3subjs_1(t).i32();
	assert(mean2_sum_3subjs_1_== 206);


	//例6 階層関数のネスト。
	//階層関数exist + count。階層関数はネストすることもできる。
	//4回の試験のうち国数英合計240点以上を取ったことが生徒の数を、全クラス全生徒を対象に求める。
	Lambda exist_3subjs_240 = exist(jpn + math + eng >= 240);//ある生徒が240点以上取ったことがあるかどうかを求めるラムダ関数。
	assert(exist_3subjs_240.GetLayer() == 1_layer);//この戻り値は1層。
	//240点以上取ったことのある生徒を全クラスを対象に数える。
	//1層（生徒）から-1層（学年全体）へ2層分引き上げるので、count2を使う。
	//以下はLambda count_student_240 = count2(exist(jpn + math + eng >= 240));と同等である。
	Lambda count_student_240 = count2(exist_3subjs_240);
	assert(count_student_240.GetLayer() == -1_layer);//ちゃんと-1層になっている。
	int64_t num_student_240 = count_student_240(t).i64();//-1層（学年全体）のラムダ関数呼び出しであるため、位置情報には意味がなく、bposを与える必要はない。与えても問題はないが。
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
	auto rank = count2(exam == exam.outer(0) && jpn > jpn.outer(0)) + 1;
	//階層関数は深度（depth）という値を持つ。階層関数外では0で、階層関数で囲われるごとに+1される。
	//例えばcount(exist(jpn > 80))という式では、jpn > 80の部分の深度が2となる。
	//上述のouterは、この深度を与えることで、
	//その深度に該当する位置情報からフィールドの値を取得することができる。
	//以下の例では、exam.outer(0)とjpn.outer(0)は{ 0, 0, 3 }という位置情報を参照して値を得る。
	assert(rank.GetLayer() == 2_layer);//2層であることに注意。どの試験かまで指定しないと計算できない。
	bpos = { 0, 0, 3 };//濃伊田美衣子の後期期末試験の位置。
	assert(rank(t, bpos).i64() == 3);
	//将来的には、階層関数にもouterを付与できるようにしたい。これが必要な状況がいくらか考えられる。


	//例8 ユーザー定義関数
	//ラムダ関数中で使う関数をユーザーが定義することもできる。
	auto average = adapt::UserFunc([](std::integral auto x, std::integral auto y, std::integral auto z) { return (x + y + z) / 3.; });
	//以下のように、intやdoubleなどを引数に取る関数を与えると、動作上は問題ないが警告が出る可能性がある。
	//auto average = adapt::UserFunc([](int32_t a, int32_t b, int32_t c) { return (a + b + c) / 3.; });

	auto avg_3_subjs = average(jpn, math, eng);
	assert(avg_3_subjs(t, bpos).f64() == (48 + 8 + 24) / 3.);
	//ジェネリックラムダのようなテンプレートを含む関数を使う場合は、必ずconceptやSFINAEを用いて制約しておくこと。
	//特にRttiの場合、FieldTypeに定義されている全ての型で呼び出しを試みるため、制約のない汎引数関数はコンパイルエラーに繋がる。
	auto constrained = adapt::UserFunc([](const auto& x, const auto& y) -> decltype(x + y) { return x + y; });
	auto sum_2_subjs = constrained(jpn, math);
	assert(sum_2_subjs(t, bpos).i32() == (48 + 8));


	//例9 型情報の事前付与
	//プレースホルダやラムダ関数には、あらかじめ静的型情報を与えておくこともできる。
	auto typed_jpn = jpn.i32();//typed_jpnはTree::TypedPlaceholder<int32_t>型。
	auto typed_math = math.i32();
	auto typed_eng = eng.i32();
	auto typed_sum_3subjs = typed_jpn + typed_math + typed_eng;
	//すべての引数がtypedであったため、戻り値の型も静的に決定される。
	static_assert(std::same_as<typename decltype(typed_sum_3subjs)::RetType, int32_t>);
	assert(typed_sum_3subjs.GetLayer() == 2_layer);
	bpos = { 0, 0, 2 };
	assert(typed_sum_3subjs(t, bpos) == 50 + 15 + 44);//型情報は既に持っているので、i32()を呼ぶ必要はない。
}

//ラムダ関数およびそれに基づくViewを活用した走査。
template <class Tree>
void TraverseWithLambda_rtti(const Tree& t)
{
	auto [class_, grade, number, name, exam, jpn, math, eng] = GetRttiPlaceholders(t);

	auto mean_math = mean(cast_f64(math));//ある生徒の試験4回の数学平均点。整数のままだと平均値も整数にされるので浮動小数点にキャストしている。
	auto mean_jpn = mean(cast_f64(jpn));//ある生徒の試験4回の数学平均点。整数のままだと平均値も整数にされるので浮動小数点にキャストしている。
	assert(mean_math.GetType() == adapt::FieldType::F64);//戻り値の型はmeanの引数型と同じ。
	auto max_math = greatest(math);//試験4回のうち数学の最高点。
	auto min_math = least(math);//試験4回のうち数学の最低点。
	assert(max_math.GetType() == adapt::FieldType::I32);//戻り値の型はgreatestの引数型と同じ。


	//例1. Filterによって条件を満たす要素だけを走査する方法
	//数学の平均点が60点以上の生徒のみ走査する。
	//このとき、走査層はFilterやTransformなどに与えられたプレースホルダ、ラムダ関数のうちの最下層に設定される。
	//今回、mean_math >= 60が1層のラムダ関数であるため、1層が走査される。
	//FilterにRttiラムダ関数を与える場合、その戻り値はI08型でなければならない。
	//比較演算子など一般にbool値を返す関数、演算子の戻り値は、Rttiの枠内ではI08型で0 or 1を返す。
	assert((mean_math >= 60.).GetType() == adapt::FieldType::I08);
	for (const auto& trav : t | Filter(mean_math >= 60.))
	{
		//Filterの条件を満たすのは角兎野誠人と子虚烏有花。
		std::cout << std::format("name:{}, mean_math:{}, max_math:{}, min_math:{}",
								 trav[name].str(), mean_math(trav).f64(), max_math(trav).i32(), min_math(trav).i32()) << std::endl;
	}

	//明示的に走査する階層を指定したい場合は、最後にGetRangeする。
	//ただし、与えたプレースホルダやラムダ関数の階層以下を指定しなければならない。
	//試験4回分の数学平均点が60点以上の生徒について、2層（各試験の成績）を走査する。
	//以下の2通りの指定でも、同等の結果になる。
	//実のところ、GetRangeは"走査階層を引数の値に引き下げる"という効果しか持たない。
	//（なぜそんな変な仕様なのかって？そのほうが実装が楽だったからだ！）
	//1. t | GetRange(2_layer) | Filter(mean_math >= 60)
	//2. t.GetRange(2_layer) | Filter(mean_math >= 60)
	//異なるTraverserインスタンスに対してラムダ関数を実行しようとしているので、
	//Initで初期化しておく。
	mean_math.Init(); max_math.Init(); min_math.Init();//InitAll(mean_math, max_math, min_math)でもよい。
	for (const auto& trav : t | Filter(mean_math >= 60) | GetRange(2_layer))
	{
		//角兎野誠人と子虚烏有花の数学平均点、最低点、最高点、各定期試験結果が表示される。
		std::cout << std::format("name:{}, mean_math:{}, max_math:{}, min_math:{}, math:{}\n",
			trav[name].str(), mean_math(trav).f64(), max_math(trav).i32(), min_math(trav).i32(), trav[math].i32());
	}
	//Filterに複数の条件を与えた場合、そのすべてを満たす要素だけを走査する。
	//これらを単にFilter(math >= 90 && mean_math >= 80)としても結果は同等だが、
	//階層が異なる条件を個々に分けて与えることで、冗長な条件の計算が省略される。
	//例えばmean_math >= 80は1層の条件であるため、生徒一人に対してただ一度だけ計算すれば十分であり、
	//その一度の計算で条件が満足されなければ、試験結果の条件計算や走査をまるごとスキップして次の生徒に移動できる。
	//なお、Filterに課した条件は、"階層関数中では適用されない"という性質がある。
	//そのため、階層関数meanを利用しているmean_mathの計算時はmath < 90やjpn < 80であるような試験結果も含めて計算される。
	mean_math.Init(); max_math.Init(); min_math.Init();
	for (const auto& trav : t | Filter(math >= 90, mean_math >= 80) | GetRange(2_layer))
	{
		std::cout << std::format("name:{}, mean_math:{}, max_math:{}, min_math:{}, math:{}\n",
			trav[name].str(), mean_math(trav).f64(), max_math(trav).i32(), min_math(trav).i32(), trav[math].i32());
	}


	//例2. Evaluateによるラムダ関数適用結果の取得
	//Evaluateを通すことで、このTraverserはoperator*によってラムダ関数の戻り値を返すようになる。
	//このとき、Evaluateに与えるラムダ関数はTypedまたはCttiにしておくことが望ましい。
	//mean_mathはRttiであるが、mean_math.f64()のように明示的に型を与えてTypedに変換できる。
	//1引数であれば実際のところ、std::views::transformとほぼ同等である。
	mean_math.Init();
	static_assert(adapt::any_node<decltype(mean_math.f64())>);
	for (double mean : t | Filter(mean_math >= 60) | Evaluate(mean_math.f64()) | GetRange(1_layer))
	{
		std::cout << std::format("mean_math:{}\n", mean);
	}
	//Rttiのプレースホルダやラムダ関数をそのまま与えた場合、operator*によってFieldVariantが返される。
	//結果がstd::stringやadapt::JBposの場合はコピーコストがかかるので推奨しない。
	//上のようにTypedに変換する方が良い。
	mean_math.Init();
	for (adapt::FieldVariant&& mean : t | Filter(mean_math >= 60) | Evaluate(mean_math) | GetRange(1_layer))
	{
		std::cout << std::format("mean_math:{}\n", mean.f64());
	}

	//Evaluateに2個以上の引数を与えた場合、operator*の戻り値がstd::tuple<...>となる。
	//また、冗長な計算が省略されるようになる。
	mean_math.Init(); mean_jpn.Init();
	for (auto [mean_, math_] : t | Filter(mean_math >= 60 && mean_jpn >= 60) | Evaluate(mean_math.f64(), math.i32()) | GetRange(2_layer))
	{
		std::cout << std::format("mean_math:{}, math:{}\n", mean_, math_);
	}

	//<ranges>のviewを利用することもできるが、
	//それ以降はADAPT独自のviewやExtractなどを連結させることができなくなる。
	mean_math.Init();
	for (auto& trav : t | Filter(mean_math >= 60) | GetRange(2_layer) | std::views::take(2) /* | Evaluate(...) これはできない*/)
	{
		std::cout << std::format("name : {}, math : {}\n", trav[name].str(), trav[math].i32());
	}

}

template <class Tree>
void Show_rtti(const Tree& t)
{
	auto [class_, grade, number, name, exam, jpn, math, eng] = GetRttiPlaceholders(t);

	//ラムダ関数についてはCalculateWithLambdaの項を参照。
	auto sum_3subjs = jpn + math + eng;
	auto greatest_3subjs = greatest(sum_3subjs);

	//o(a)はouter(a)の短縮版。
	auto rank = count3(exam == 0 && sum_3subjs > jpn.o(0) + math.o(0) + eng.o(0)) + 1;

	//rの全てのフィールドの情報を表示するようにしたいが、未実装。
	//t | Show();

	//4回の試験の3科目合計点が180を超えたことのある生徒の情報を表示する。
	//Showにplaceholderなどを与えるとそれらの情報だけを表示する。
	//ただし、表示できるのはFieldTypeに定義されているものに限られる。STreeなどを表示するときは注意。
	t | Filter(greatest_3subjs >= 180) | Show(number, name, greatest_3subjs, jpn, math, eng);
}

template <class Tree>
void Extract_rtti(const Tree& t)
{
	auto [class_, grade, number, name, exam, jpn, math, eng] = GetRttiPlaceholders(t);

	//Extractはプレースホルダまたはラムダ関数を与えることで、
	//それらをフィールドとして持つ新しいTreeを生成する。
	//非Cttiのプレースホルダやラムダ関数が一つでも入ると、
	//戻り値はDTreeとなる。
	adapt::DTree t2 = t | Extract(name, jpn + math + eng);

	//Extractで生成されるTreeは、与えられたプレースホルダとラムダ関数の階層に応じた構造を自動的に定義する。
	//t2の構造は以下の通りである。
	//-1層、0層 : なし
	// 1層 : 生徒の名前（Str）
	// 2層 : 三科目合計点（I32）
	//また、特に名前を指定しない場合、これらのフィールドにはExtractに与えた順に"fld0"、"fld1"、"fld2"...という名前が与えられる。
	auto [name_e, sum_3subjs_e] = t2.GetPlaceholders("fld0", "fld1");
	assert(name_e.GetLayer() == 1);
	assert(name_e.GetType() == adapt::FieldType::Str);
	assert(sum_3subjs_e.GetLayer() == 2);
	assert(sum_3subjs_e.GetType() == adapt::FieldType::I32);

	//もしExtractしたTreeのフィールドに名前を与えたい場合、次のようにする。
	adapt::DTree t3 = t | Extract(name.named("name"), (jpn + math + eng).named("sum_3subjs"));
	[[maybe_unused]] auto [name_e2, sum_3subjs_e2] = t3.GetPlaceholders("name", "sum_3subjs");

	//もちろん、Filterを通してもよい。三科目合計点が120を下回った試験結果のみを対象に、名前と合計点をフィールドとして持つTreeを生成する。
	[[maybe_unused]] adapt::DTree t4 = t | Filter(jpn + math + eng < 120) | Extract(name.named("name"), (jpn + math + eng).named("sum_3subjs"));

	//namedという関数を個々に書くのは冗長になりがちなため、次のようなヘルパーを用意している。
	auto t5 = t | Extract(ADAPT_NAMED_FIELDS(name, (jpn + math + eng)));
	//これは以下と同等である。
	//auto t6 = t | Extract(name.named("name"_fld), (jpn + math + eng).named("(jpn + math + eng)"_fld));
}

template <class Tree>
void Plot_rtti(const Tree& t)
{
	auto [class_, grade, number, name, exam, jpn, math, eng] = GetRttiPlaceholders(t);

	//ADAPTはデータ可視化のための機能を有するので、簡易的な図であればADAPT内で完結させることもできる。
	//この機能はGnuplotをバックエンドとして使用するので、予めインストールしておく必要がある。

	//Gnuplotのパスは次の順に探索される。
	// 1. Canvas::SetGnuplotPathで設定されたパス。
	// 2. 環境変数GNUPLOT_PATHがある場合、その値を使う。
	// 3. Windowsであれば"C:/Progra~1/gnuplot/bin/gnuplot.exe"、その他OSではパスが通っていると想定し"gnuplot"とする。

	//adapt::Canvas::SetGnuplotPath("path/to/gnuplot");

	//戻り値はstd::tuple<std::vector<double>, std::vector<double>>;
	auto [v_jpn, v_eng, v_math] = t | Filter(exam == 0) | ToVector(jpn.i32(), eng.i32(), math.i32());

	namespace plot = adapt::plot;
	std::string tree_type = adapt::s_tree<Tree> ? "stree" : "dtree";
	adapt::Canvas2D f(std::format("scatter_jpn_eng_rtti_{}.png", tree_type));
	f.SetTitle("jpn vs eng/math");
	f.SetXLabel("jpn");
	f.SetYLabel("eng / math");
	f.SetKeyTopLeft();
	f.SetXRange(0, 100);
	f.SetYRange(0, 100);
	//PlotPointsなどの関数は任意のrangeを受け取ることができる。
	//そのため、v_jpn等の代わりにt | Filter(exam == 0) | Evaluate(jpn.i32())等としてもよい。
	f.PlotPoints(v_jpn, v_eng, plot::c_blue, plot::pointtype = 7, plot::ps_large, plot::title = "jpn vs eng").
		PlotPoints(v_jpn, v_math, plot::c_red, plot::pointtype = 7, plot::ps_large, plot::title = "jpn vs math");

	std::vector<std::string> exams = { "1st-sem-mid", "1st-sem-fin", "2nd-sem-mid", "2nd-sem-fin" };
	auto dummyko_math = t | Filter(name == "濃伊田美衣子") | ToVector(math.i32());
	auto seito_math = t | Filter(name == "角兎野誠人") | ToVector(math.i32());
	auto yuuka_math = t | Filter(name == "子虚烏有花") | ToVector(math.i32());
	adapt::Canvas2D f2(std::format("dummyko_math_rtti_{}.png", tree_type));
	f2.SetTitle("math result");
	f2.SetYLabel("math score");
	f2.SetXTicsRotate(-45);
	f2.SetXRange(-0.5, 3.5);
	f2.SetYRange(0, 100);
	f2.PlotPoints(exams, dummyko_math, plot::title = "dummyko",
				  plot::s_linespoints, plot::pt_fcir, plot::ps_med_large, plot::c_dark_magenta).
		PlotPoints(exams, seito_math, plot::title = "seito",
				   plot::s_linespoints, plot::pt_ftri, plot::ps_med_large, plot::c_dark_cyan).
		PlotPoints(exams, yuuka_math, plot::title = "yuuka",
				   plot::s_linespoints, plot::pt_fbox, plot::ps_med_large, plot::c_dark_yellow);

	//この他、より複雑な描画を行いたい場合はMatplot++などを使うこともできる。
	//各々好ましいものを選択して使うとよい。
}
