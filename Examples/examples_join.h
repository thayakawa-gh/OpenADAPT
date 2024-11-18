#include <OpenADAPT/ADAPT.h>
#include <matplot/matplot.h>

using namespace adapt::lit;

template <class Tree1, class Tree2>
auto Join(const Tree1& t1, const Tree2& t2)
{
	//ADAPTにおけるJoinは、2個以上のコンテナを連結し、擬似的に一個のコンテナに見せかけるものである。
	//SQLやPandasのJoinに近いが、ADAPTの場合、階層構造を踏まえて結合している点は注意が必要。
	//"見せかける"とあるように、Joinは即時にコンテナ間連結と新規コンテナ作成は行わない。遅延評価である。
	//Traverserによってその要素を指し示した時や、実際にフィールドへのアクセスが発生した時に、
	//改めて連結先となる要素を検索し、連結処理を行う。

	//今、t1は-1層"学年" --- 0層"クラス" --- 1層"生徒" --- 2層"成績"という階層構造をなしている。
	//ここで例えば、学校内の生徒の成績と、その世帯収入の相関を見たくなったとする。
	//そこで、市の各世帯及びその住人、そして各人の収入のデータを引っ張ってきてJoinしてみる。
	//（そんなデータをどうやって入手するのかって？知らん、頑張れ。）
	//t2にはこのデータが次のような階層構造で格納されている。
	//-1層"市内の世帯" --- 0層"世帯" --- 1層"住人"
	//このうち1層の"住人"が各々年収のデータを持っている。
	//これらのJoinは次のように行う。

	adapt::DJoinedContainer<std::add_const_t, adapt::DTree, adapt::DTree> jt = Join(t1, 1_layer, 0_layer, t2);
	//Join関数によって上記のようにコンテナを連結してみると、
	//作成されたjtの中ではt1の1層とt2の0層とが連結され、以下のような階層構造として扱われる。
	//
	//      -1層        0層         1層        2層
	//     "学年" --- "クラス" --- "生徒" --- (試験)
	//                               |
	//      (市)                   "世帯" --- "住人"
	//
	//このとき、t2の"世帯"はt1の"生徒"に連結され、同一要素と見做される。
	//つまりjtにとっては、"世帯"も1層の要素に等しくなる。また"住人"は2層の要素となる。
	//さらに、今回はt2を0層で連結しているため存在しないが、
	//もしt2を1層"住人"で連結していた場合、0層"世帯"もjtにとっては1層要素としてアクセス可能となる。
	//ただし、t1の2層である"試験"は、sum(math)のようにラムダ関数等で生徒層まで繰り上がらない限りはアクセス不能となる。

	//なお、連結されたコンテナそれぞれには順位（rank）が割り当てられる。Join関数に与えた順に0位、1位……となる。
	//今回の場合、t1が0位、t2が1位である。この順位はjtからプレースホルダを取得するときなどに、どのコンテナのものかを明示するために使う。


	//さて、Joinのためにはもう一つ手間がかかる。どの要素同士を結びつけるかの指定が必要なためである。
	//ここでは"生徒"の名前と、"住人"の名前が一致する場合に、その"世帯"を連結することにしてみる。
	//（住人と世帯は階層が異なるが、連結したい階層よりも下層のプレースホルダないしラムダ関数を与えていればよい。）

	//まず、連結に用いるt1側のプレースホルダを取得する。**こちらは連結後であるjtのプレースホルダを得る必要がある。**
	auto student_name = jt.GetPlaceholder<0>("name");//こちらはjtから順位を指定して取得する。
	//次に、t2の連結先となる住人の名前のプレースホルダを取得する。**こちらは逆に、連結前のt2から取得する。**
	auto resident_name = t2.GetPlaceholder("name");//こちらはt2から取得する。

	//1位のコンテナ（==t2）の連結方法を指定する。
	//このKeyJointというのが、両者のキーの一致する要素同士を連結する方法である。
	jt.SetKeyJoint<1>(student_name, resident_name.str());
	//なお、jt.SetKeyJoint<1>(student_name, resident_name);のように、
	//被検索側の型を明示しない場合、検索時のオーバーヘッドが増える。
	//特にStr、Jbpなど動的メモリ確保を伴うものをキーとする場合、
	//型を明示しないと検索時にコピーコストが発生するので、極力避けるように。

	//2個以上のキーを指定する場合、以下のようにstd::forward_as_tupleで括る。
	//jt.SetKeyJoint<1>(std::forward_as_tuple(student_name, ...), std::forward_as_tuple(resident_name, ...));

	//今回はキーにプレースホルダを直接与えたが、もちろんラムダ関数でも良い。

	//上記のようにSetKeyJointにそれぞれのキーを与えた場合、内部で自動的にハッシュテーブルが作成される。
	//が、場合によっては自前でハッシュテーブルを用意してもよい。
	//例えば、ハッシュテーブルに格納する要素を絞り込み、連結対象を制限したい場合や、
	//ハッシュテーブルを何度も使いまわしたい場合などに。
	//連結対象にしたくない要素があるのなら、Filterを噛ませたりしても良い。
	//auto income = t2.GetPlaceholder("income");
	//adapt::Hashtable<std::string> hashtable = t2 | Filter(sum(income) > 8000000) | MakeHashtable(resident_name.str());
	//jt.SetKeyJoint<1>(hashtable, student_name);


	//Joinは基本的にinner joinまたはleft joinと似ているが、厳密に言えばどちらとも異なる。
	//right join相当機能は存在しないが、連結の順序を入れ替えれば同じような動作となる。
	//outer join相当機能は存在しないし、作るのは極めて困難だと思われる。
	//詳細は例として用意した2種の関数、Travese_joinとExtract_joinを参照。

	return jt;
}

template <class JTree>
auto GetRttiPlaceholder_jtree(const JTree& jt)
{
	//Joinするところでも説明しているが、
	//JoinedContainerからプレースホルダを取得する方法は、通常のDTree/STreeとは少し異なり、順位を与える必要がある。
	auto [class_, student_name, jpn, math, eng] = jt.template GetPlaceholders<0>("class", "name", "jpn", "math", "eng");
	auto [family_name, resident_name, income] = jt.template GetPlaceholders<1>("family_name", "name", "income");

	assert(class_.GetLayer() == 0_layer);
	assert(student_name.GetLayer() == 1_layer);
	assert(jpn.GetLayer() == -3_layer);//通常アクセス不能となっているフィールドの階層は定義できないため-3が返ってくる。
	assert(math.GetLayer() == -3_layer);
	assert(eng.GetLayer() == -3_layer);
	assert(family_name.GetLayer() == 1_layer);
	assert(resident_name.GetLayer() == 2_layer);
	assert(income.GetLayer() == 2_layer);

	return std::make_tuple(class_, student_name, jpn, math, eng, family_name, resident_name, income);
}

template <class JTree>
void AccessAndCalc_jtree(const JTree& jt)
{
	auto [class_, student_name, jpn, math, eng, family_name, resident_name, income] = GetRttiPlaceholder_jtree(jt);

	//JoinedContainerに対してはoperator[]が現時点で動作しない。
	//JoinedContainerはどうしても各階層の要素が明確に区分できない状況が増える上に、
	//連結が遅延評価であることも手伝って、
	//インデックスを1個ずつしか受け取れない添字演算子では複雑な実装になってしまうため。
	//代わりにGetBranchを使う。現状、bposを与えるものしか使えない。
	//GetBranch(0, 0, 0)[resident_name]のようにインデックスを直接与えるものはそのうち作る。
	adapt::Bpos bpos1{ 0, 0 };
	assert(jt.GetBranch(bpos1)[student_name].str() == "濃伊田美衣子");
	adapt::Bpos bpos2{ 0, 0, 1 };
	assert(jt.GetBranch(bpos2)[resident_name].str() == "濃伊田母");

	//成績の要素はアクセス不能であったが、
	//meanによって階層が繰り上がることで1層要素としてアクセスできるようになる。
	auto mean_sum_3subjs = mean(jpn + math + eng);
	assert(mean_sum_3subjs.GetLayer() == 1_layer);
	assert(mean_sum_3subjs(jt, bpos1).i32() == 99);

	auto household_income = sum(income);
	assert(household_income.GetLayer() == 1);
	assert(household_income(jt, bpos1).i32() == 8000000);

	adapt::Bpos bpos3{ 0, 2 };
	assert(jt.GetBranch(bpos3)[student_name].str() == "子虚烏有花");
	//連結したt2の方には子虚烏家の情報がなかったので、
	//その情報へはアクセスしようとすると例外SkipOverが投げられる。
	adapt::Bpos bpos4{ 0, 2, 0 };
	bool cannot_access = false;
	try
	{
		jt.GetBranch(bpos4)[resident_name].str();
	}
	catch (adapt::JointError)
	{
		cannot_access = true;
	}
	assert(cannot_access);
}


template <class JTree>
void Traverse_jtree(const JTree& jt)
{
	auto [class_, student_name, jpn, math, eng, family_name, resident_name, income] = GetRttiPlaceholder_jtree(jt);

	//JonedContainerのTraverserには、DelayedJointとPromptJointの2種類の動作がある。


	//PromptJointは、"その要素へ移動したときに即時連結させる"モードである。
	//jt.GetRange(1_layer)やjt | Filter(...)のように普通にrangeを取得した場合、PromptJointモードとなる。
	//非常に大雑把に言えば、inner joinのような動作をする。
	//例えば今回、t1にはあるがt2にない要素として、子虚烏有花がいる。
	//forループ中の++演算子によって角兎野誠人から子虚烏有花へとTraverserが移動した時、
	//子虚烏有花の世帯情報がないために連結に失敗し、そのまま子虚烏有花はスキップされる。
	//もちろん、t2にしか存在しない隅州慈恩も無視される。
	auto mean_sum_3subjs = mean(jpn + math + eng);
	auto household_income = sum(income);
	//Traverserは概ねDTree/STreeと同様に動作する。
	for (auto& trav : jt.GetRange(1_layer))
	{
		//濃伊田美衣子と角兎野誠人の3科目平均点と世帯収入が表示される。
		std::cout << std::format("{}: score {}, household_income {}\n",
			trav[student_name].str(), mean_sum_3subjs(trav).i32(), household_income(trav).i32());
	}
	//Filterなどももちろん使える。
	//収入が0でない住人の情報を表示する。
	for (auto& trav : jt | Filter(income > 0) | GetRange(2_layer))
	{
		std::cout << std::format("{}: income {}\n", trav[resident_name].str(), trav[income].i32());
	}


	//DelayedJointは"可能な限り連結を遅延させる"モードである。
	//階層関数中、ExtractやMakeHashtableなどではこちらが使われる。
	//明示的にDelayedJointモードを使いたい場合は、GetRange_delayed()を呼ぶ。
	//こちらでは、例えばtrav[resident_name]のように
	//"その順位以下の要素へのアクセスが発生した時"に初めて連結処理が行われる。
	//よって、擬似的にだがleft joinに近い動作が可能である。
	InitAll(mean_sum_3subjs, household_income);
	for (auto& trav : jt.GetRange_delayed(1_layer))
	{
		if (trav.TryJoin(1_rank))
		{
			//DelayedモードでTryJoinを実行すると、0～指定順位（今回は1_rank）までの連結を試みる。
			//連結可能な要素が見つかった場合、jtは指定順位まで連結された状態となり、
			//TryJoinはtrueを返す。
			//濃伊田美衣子と角兎野誠人は連結可能な世帯情報があるので、こちらで3科目平均点と世帯収入が表示される。
			std::cout << std::format("{}: score {}, household_income {}\n",
				trav[student_name].str(), mean_sum_3subjs(trav).i32(), household_income(trav).i32());
		}
		else
		{
			//TryJoinで連結に失敗した場合、指定順位までのうち可能な限り連結を行った上で、falseが返ってくる。
			//このとき当然指定順位以下の要素にはアクセスできないが、
			//連結に成功した順位の要素まではアクセスできる。
			//子虚烏有花のみがこちらの
			std::cout << std::format("{}: score {}, household_income nodata\n",
				trav[student_name].str(), mean_sum_3subjs(trav).i32());
		}
	}
	size_t dummy_count = 0;
	for (auto& trav : jt.GetRange_delayed(2_layer))
	{
		//DelayedJointにおいて特に注意しなければならないのは、
		//連結されないままで次の要素へ移動すると、未連結要素は不要とみなされ丸ごとスキップされる、ということである。

		//濃伊田美衣子を指している時、TryJoinよりも前にcontinueしてみる。
		//このとき、1_rankへのアクセスは発生していないので、Delayedモードで動かしている場合はまだ連結されていない。
		//そのままoperator++などで次の要素へ移動すると、濃伊田家の住人の情報を丸ごとスキップしてしまう。
		//これはExtractなどの決まりきった処理をする場合は不要な要素を丸ごとスキップできて速度的に有利なのだが、
		//ユーザーが自分でDelayedモードを使う場合は意図しない挙動となる可能性があるため、注意が必要。
		if (trav[student_name].str() == "濃伊田美衣子")
		{
			dummy_count++;
			continue;
		}
		if (!trav.TryJoin(1_rank)) continue;
		//子虚烏家の住人へアクセスしようとしたときは連結できないため例外SkipOverが投げられる。
		//TryJoinとどちらを使うかは状況次第。
		std::cout << trav[resident_name].str() << std::endl;
	}
	//濃伊田家の住人は4人なので4になっているかと思いきや、
	//濃伊田家住人の要素へ一切アクセスされず未連結状態のまま次の要素へ移動したことで、
	//濃伊田家は丸ごとスキップされてしまった。
	assert(dummy_count == 1);
}

template <class JTree>
void Plot_jtree(const JTree& jt)
{
	auto [class_, student_name, jpn, math, eng, family_name, resident_name, income] = GetRttiPlaceholder_jtree(jt);

	auto [v_math, v_income] = jt | ToVector(mean(cast_f64(math)).f64(), cast_f64(sum(income)).f64());
	namespace plot = adapt::plot;
	adapt::Canvas2D c("scatter_math_vs_income.png");
	c.SetTitle("math vs income");
	c.SetXLabel("mean(math)");
	c.SetYLabel("household income");
	c.PlotPoints(v_math, v_income, plot::pt_circle, plot::ps_med_large, plot::no_title);
}