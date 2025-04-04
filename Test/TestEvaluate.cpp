#include <Test/Aggregator.h>
#include <ranges>
#include <iterator>

using namespace adapt;
using namespace adapt::lit;

template <class Container, class Layer0, class Layer1, class Layer2, bool Disabled>
void TestEvaluate(Container& tree, const std::vector<Class>& cls,
				  const Layer0& l0, const Layer1& l1, const Layer2& l2,
				  std::bool_constant<Disabled> = std::false_type{})
{
	[[maybe_unused]] bool is_joined_container = joined_container<Container>;

	//0層要素。学年とクラス。
	[[maybe_unused]] auto [grade, class_] = l0;
	//1層要素。出席番号、名前、生年月日。
	[[maybe_unused]] auto [number, name] = l1;
	//2層要素。各試験の点数。前期中間、前期期末、後期中間、後期期末の順に並んでいる。
	[[maybe_unused]] auto [exam, math, jpn, eng, sci, soc] = l2;

	static constexpr bool IsAllCtti = ctti_placeholder<decltype(grade)> && ctti_placeholder<decltype(class_)> &&
									  ctti_placeholder<decltype(number)> && ctti_placeholder<decltype(name)> &&
									  ctti_placeholder<decltype(exam)> && ctti_placeholder<decltype(math)> && ctti_placeholder<decltype(jpn)> &&
									  ctti_placeholder<decltype(eng)> && ctti_placeholder<decltype(sci)> && ctti_placeholder<decltype(soc)>;
	static constexpr bool IsAllRtti = rtti_placeholder<decltype(grade)> && rtti_placeholder<decltype(class_)> &&
									  rtti_placeholder<decltype(number)> && rtti_placeholder<decltype(name)> &&
									  rtti_placeholder<decltype(exam)> && rtti_placeholder<decltype(math)> && rtti_placeholder<decltype(jpn)> &&
									  rtti_placeholder<decltype(eng)> && rtti_placeholder<decltype(sci)> && rtti_placeholder<decltype(soc)>;

	static_assert(IsAllCtti || IsAllRtti);

	auto num_name = tostr(number) + ":" + name;
	auto total_score = math + jpn + eng + sci + soc;//その試験の合計点。
	auto all_400 = count(total_score >= 400) == 4;//全試験で400点以上を取っているかどうか。
	auto mean_math = mean(cast_f32(math));//生徒ごとの数学4回分の平均点。mathは整数型なので、浮動小数点にキャストしてから計算する。
	auto mean_math_in_class = mean(cast_f64(math.at(0)));//前期中間試験の数学のクラス内平均点。
	auto dev_math_in_class = dev(cast_f64(math.at(0)));//前期中間試験の数学のクラス内標準偏差。
	auto dev_math = (math.at(0) - mean_math_in_class) * 10. / dev_math_in_class + 50.;//前期中間試験の数学のクラス内偏差値。
	auto is_best = isgreatest(total_score);//4回のうち最高点のときにtrue。
	auto num_stu_200 = count(exist(jpn + math + eng >= 200));//クラス内で3科目200点以上を取ったことのある生徒の数。
	auto rank_math_class = count2(exam == exam.o(0_depth) && math > math.o(0_depth));//各試験の数学クラス内順位。自分より好成績な生徒の人数を数えている。o(0)はouter(0)の短縮表記。
	auto rank_math_all = count3(exam == exam.o(0_depth) && math > math.o(0_depth));//各試験の学年順位。数える範囲がクラス内ではなく学年全体になる。
	auto mean_max = mean(cast_f32(greatest(total_score)));
	auto sum_math_over60 = sum_if(math, math > 60);
	auto mean_math_engover60_2 = mean_if2(cast_f32(math), sum(eng) >=240);
	auto is_second_max_math = isgreatest_if(math, !isgreatest(math));//数学が2番目に高得点だったときの試験でtrue。
	auto is_worst_in_third_exam = isleast_if2(total_score, exam == 2);//後期中間で最低点を取った生徒の試験でtrue。
	//Cttiの場合はRttiへの変換テスト。Rttiの場合はTypedへの変換テスト。
	auto type_test1_total_score = [&math, &jpn, &eng, &sci, &soc]()
	{
		if constexpr (IsAllCtti) return ConvertToRttiFuncNode(math + jpn + eng + sci + soc);
		else return math.i32() + jpn.i32() + eng.i32() + sci.i32() + soc.i32();
	} ();
	//CttiとRttiの混ざったラムダ関数。
	auto type_test2_total_score = [&math, &jpn, &eng, &sci, &soc]()
	{
		if constexpr (IsAllCtti) return (math + jpn) + ConvertToRttiFuncNode(eng + sci + soc);
		else return (math + jpn).i32() + (eng.i32() + sci.i32() + soc.i32());
	} ();

	auto num_name_s = num_name;
	auto total_score_s = total_score;//その試験の合計点。
	auto all_400_s = all_400;//全試験で400点以上を取っているかどうか。
	auto mean_math_s = mean_math;//生徒ごとの数学4回分の平均点。mathは整数型なので、浮動小数点にキャストしてから計算する。
	auto mean_math_in_class_s = mean_math_in_class;//前期中間試験の数学のクラス内平均点。
	auto dev_math_in_class_s = dev_math_in_class;//前期中間試験の数学のクラス内標準偏差。
	auto dev_math_s = dev_math;//前期中間試験の数学のクラス内偏差値。
	auto is_best_s = is_best;//4回のうち最高点のときにtrue。
	auto num_stu_200_s = num_stu_200;//クラス内で3科目200点以上を取ったことのある生徒の数。
	auto rank_math_class_s = rank_math_class;//各試験の数学クラス内順位。自分より好成績な生徒の人数を数えている。o(0)はouter(0)の短縮表記。
	auto rank_math_all_s = rank_math_all;//各試験の学年順位。数える範囲がクラス内ではなく学年全体になる。
	auto mean_max_s = mean_max;
	auto sum_math_over60_s = sum_math_over60;
	auto mean_math_engover60_2_s = mean_math_engover60_2;
	auto is_second_max_math_s = is_second_max_math;
	auto is_worst_in_third_exam_s = is_worst_in_third_exam;
	auto type_test1_total_score_s = type_test1_total_score;
	auto type_test2_total_score_s = type_test2_total_score;

	auto range = tree.GetRange(2);
	auto t = range.begin();
	Bpos bpos(2);

	InitAll(t, num_name, total_score, all_400,
			mean_math, mean_math_in_class, dev_math_in_class, dev_math,
			is_best, num_stu_200, rank_math_class, rank_math_all, mean_max,
			sum_math_over60, mean_math_engover60_2,
			is_second_max_math, is_worst_in_third_exam,
			type_test1_total_score, type_test2_total_score);
	InitAll(tree, bpos, num_name_s, total_score_s, all_400_s,
			mean_math_s, mean_math_in_class_s, dev_math_in_class_s, dev_math_s,
			is_best_s, num_stu_200_s, rank_math_class_s, rank_math_all_s, mean_max_s,
			sum_math_over60_s, mean_math_engover60_2_s,
			is_second_max_math_s, is_worst_in_third_exam_s,
			type_test1_total_score_s, type_test2_total_score_s);

	if constexpr (IsAllCtti)
	{
		//STreeの場合、階層や型の情報はstaticに決定できる。
		static_assert(num_name.GetLayer() == 1);
		static_assert(total_score.GetLayer() == 2);
		static_assert(all_400.GetLayer() == 1);
		static_assert(mean_math.GetLayer() == 1);
		static_assert(!Disabled && mean_math_in_class.GetLayer() == 0);
		static_assert(!Disabled && dev_math_in_class.GetLayer() == 0);
		static_assert(!Disabled && dev_math.GetLayer() == 1);
		static_assert(is_best.GetLayer() == 2);
		static_assert(num_stu_200.GetLayer() == 0);
		static_assert(rank_math_class.GetLayer() == 2);
		static_assert(rank_math_all.GetLayer() == 2);
		static_assert(mean_max.GetLayer() == 0);
		static_assert(sum_math_over60.GetLayer() == 1);
		static_assert(mean_math_engover60_2.GetLayer() == 0);
		static_assert(is_second_max_math.GetLayer() == 2);
		static_assert(is_worst_in_third_exam.GetLayer() == 2);
		EXPECT_EQ(type_test1_total_score.GetLayer(), 2);
		EXPECT_EQ(type_test2_total_score.GetLayer(), 2);

		static_assert(std::is_same_v<typename decltype(num_name)::RetType, std::string>);
		static_assert(std::is_same_v<typename decltype(total_score)::RetType, int32_t>);
		static_assert(std::is_same_v<typename decltype(all_400)::RetType, bool>);
		static_assert(std::is_same_v<typename decltype(mean_math)::RetType, float>);
		static_assert(!Disabled && std::is_same_v<typename decltype(mean_math_in_class)::RetType, double>);
		static_assert(!Disabled && std::is_same_v<typename decltype(dev_math_in_class)::RetType, double>);
		static_assert(!Disabled && std::is_same_v<typename decltype(dev_math)::RetType, double>);
		static_assert(std::is_same_v<typename decltype(is_best)::RetType, bool>);
		static_assert(std::is_same_v<typename decltype(num_stu_200)::RetType, int64_t>);
		static_assert(std::is_same_v<typename decltype(rank_math_class)::RetType, int64_t>);
		static_assert(std::is_same_v<typename decltype(rank_math_all)::RetType, int64_t>);
		static_assert(std::is_same_v<typename decltype(mean_max)::RetType, float>);
		static_assert(std::is_same_v<typename decltype(sum_math_over60)::RetType, int32_t>);
		static_assert(std::is_same_v<typename decltype(mean_math_engover60_2)::RetType, float>);
		static_assert(std::is_same_v<typename decltype(is_second_max_math)::RetType, bool>);
		static_assert(std::is_same_v<typename decltype(is_worst_in_third_exam)::RetType, bool>);
		static_assert(rtti_node<decltype(type_test1_total_score)>);
		static_assert(rtti_node<decltype(type_test2_total_score)>);
		EXPECT_EQ(type_test1_total_score.GetType(), FieldType::I32);
		EXPECT_EQ(type_test1_total_score.GetType(), FieldType::I32);
	}
	else
	{
		EXPECT_EQ(num_name.GetLayer(), 1);
		EXPECT_EQ(total_score.GetLayer(), 2);
		EXPECT_EQ(all_400.GetLayer(), 1);
		EXPECT_EQ(mean_math.GetLayer(), 1);
		if constexpr (!Disabled) { EXPECT_EQ(mean_math_in_class.GetLayer(), 0); }
		if constexpr (!Disabled) { EXPECT_EQ(dev_math_in_class.GetLayer(), 0); }
		if constexpr (!Disabled) { EXPECT_EQ(dev_math.GetLayer(), 1); }
		EXPECT_EQ(is_best.GetLayer(), 2);
		EXPECT_EQ(num_stu_200.GetLayer(), 0);
		EXPECT_EQ(rank_math_class.GetLayer(), 2);
		EXPECT_EQ(rank_math_all.GetLayer(), 2);
		EXPECT_EQ(mean_max.GetLayer(), 0);
		EXPECT_EQ(sum_math_over60.GetLayer(), 1);
		EXPECT_EQ(mean_math_engover60_2.GetLayer(), 0);
		EXPECT_EQ(is_second_max_math.GetLayer(), 2);
		EXPECT_EQ(is_worst_in_third_exam.GetLayer(), 2);
		EXPECT_EQ(type_test1_total_score.GetLayer(), 2);
		EXPECT_EQ(type_test2_total_score.GetLayer(), 2);

		EXPECT_EQ(num_name.GetType(), FieldType::Str);
		EXPECT_EQ(total_score.GetType(), FieldType::I32);
		EXPECT_EQ(all_400.GetType(), FieldType::I08);
		EXPECT_EQ(mean_math.GetType(), FieldType::F32);
		if constexpr (!Disabled) { EXPECT_EQ(mean_math_in_class.GetType(), FieldType::F64); }
		if constexpr (!Disabled) { EXPECT_EQ(dev_math_in_class.GetType(), FieldType::F64); }
		if constexpr (!Disabled) { EXPECT_EQ(dev_math.GetType(), FieldType::F64); }
		EXPECT_EQ(is_best.GetType(), FieldType::I08);
		EXPECT_EQ(num_stu_200.GetType(), FieldType::I64);
		EXPECT_EQ(rank_math_class.GetType(), FieldType::I64);
		EXPECT_EQ(rank_math_all.GetType(), FieldType::I64);
		EXPECT_EQ(mean_max.GetType(), FieldType::F32);
		EXPECT_EQ(sum_math_over60.GetType(), FieldType::I32);
		EXPECT_EQ(mean_math_engover60_2.GetType(), FieldType::F32);
		EXPECT_EQ(is_second_max_math.GetType(), FieldType::I08);
		EXPECT_EQ(is_worst_in_third_exam.GetType(), FieldType::I08);
		static_assert(stat_type_node<decltype(type_test1_total_score)>);
		static_assert(!ctti_node<decltype(type_test1_total_score)>);
		static_assert(stat_type_node<decltype(type_test2_total_score)>);
		static_assert(!ctti_node<decltype(type_test2_total_score)>);
		static_assert(std::is_same_v<typename decltype(type_test1_total_score)::RetType, int32_t>);
		static_assert(std::is_same_v<typename decltype(type_test2_total_score)::RetType, int32_t>);
	}
	size_t clssize = cls.size();
	for (BindexType i = 0; i < clssize; ++i)
	{
		bpos[0] = i;
		const Class& c = cls[i];
		size_t stusize = c.m_students.size();
		for (BindexType j = 0; j < stusize; ++j)
		{
			bpos[1] = j;
			const Student& s = c.m_students[j];
			size_t recsize = s.m_records.size();
			for (BindexType k = 0; k < recsize; ++k)
			{
				bpos[2] = k;
				const Record& r = s.m_records[k];

				auto Evaluate_trav = []<FieldType Type, class Trav, class Node>(Number<Type>, const Trav & t, const Node& node)
				{
					if constexpr (stat_type_node<Node>) return node.Evaluate(t);
					else return node.Evaluate(t).template as<Type>();
				};
				auto Evaluate_cnt = []<FieldType Type, class Node>(Number<Type>, const Container& t, const Bpos& bpos, const Node& node)
				{
					if constexpr (stat_type_node<Node>) return node.Evaluate(t, bpos);
					else return node.Evaluate(t, bpos).template as<Type>();
				};

				//2層の要素、つまり各学生の学期ごとの成績を走査する。
				EXPECT_EQ(Evaluate_trav(Number<FieldType::Str>{}, t, num_name), std::to_string(s.m_number) + ":" + s.m_name);
				EXPECT_EQ(Evaluate_trav(Number<FieldType::I32>{}, t, total_score), r.m_math + r.m_japanese + r.m_english + r.m_science + r.m_social);
				EXPECT_EQ(Evaluate_trav(Number<FieldType::I08>{}, t, all_400), All400(s));
				EXPECT_EQ(Evaluate_trav(Number<FieldType::F32>{}, t, mean_math), AvgMath(s));
				if constexpr (!Disabled) { EXPECT_EQ(Evaluate_trav(Number<FieldType::F64>{}, t, mean_math_in_class), AvgMathInClass(c)); }
				if constexpr (!Disabled) { EXPECT_EQ(Evaluate_trav(Number<FieldType::F64>{}, t, dev_math_in_class), DevMathInClass(c)); }
				if constexpr (!Disabled) { EXPECT_EQ(Evaluate_trav(Number<FieldType::F64>{}, t, dev_math), DevMath(c, s)); }
				EXPECT_EQ(Evaluate_trav(Number<FieldType::I08>{}, t, is_best), IsBest(s, k));
				EXPECT_EQ(Evaluate_trav(Number<FieldType::I64>{}, t, num_stu_200), NumStu200(c));
				EXPECT_EQ(Evaluate_trav(Number<FieldType::I64>{}, t, rank_math_class), RankMathClass(c, r));
				EXPECT_EQ(Evaluate_trav(Number<FieldType::I64>{}, t, rank_math_all), RankMathAll(cls, r));
				EXPECT_EQ(Evaluate_trav(Number<FieldType::F32>{}, t, mean_max), MeanMax(c));
				EXPECT_EQ(Evaluate_trav(Number<FieldType::I32>{}, t, sum_math_over60), SumMathOver60(s));
				EXPECT_EQ(Evaluate_trav(Number<FieldType::F32>{}, t, mean_math_engover60_2), MeanMathEngOver60(c));
				EXPECT_EQ((bool)Evaluate_trav(Number<FieldType::I08>{}, t, is_second_max_math), IsSecondMaxMath(s, k));
				EXPECT_EQ((bool)Evaluate_trav(Number<FieldType::I08>{}, t, is_worst_in_third_exam), IsWorstInThirdExam(c, j, k));
				EXPECT_EQ(Evaluate_trav(Number<FieldType::I32>{}, t, type_test1_total_score), r.m_math + r.m_japanese + r.m_english + r.m_science + r.m_social);
				EXPECT_EQ(Evaluate_trav(Number<FieldType::I32>{}, t, type_test2_total_score), r.m_math + r.m_japanese + r.m_english + r.m_science + r.m_social);

				EXPECT_EQ(Evaluate_cnt(Number<FieldType::Str>{}, tree, bpos, num_name_s), std::to_string(s.m_number) + ":" + s.m_name);
				EXPECT_EQ(Evaluate_cnt(Number<FieldType::I32>{}, tree, bpos, total_score_s), r.m_math + r.m_japanese + r.m_english + r.m_science + r.m_social);
				EXPECT_EQ((bool)Evaluate_cnt(Number<FieldType::I08>{}, tree, bpos, all_400_s), All400(s));
				EXPECT_EQ(Evaluate_cnt(Number<FieldType::F32>{}, tree, bpos, mean_math_s), AvgMath(s));
				if constexpr (!Disabled) { EXPECT_EQ(Evaluate_cnt(Number<FieldType::F64>{}, tree, bpos, mean_math_in_class_s), AvgMathInClass(c)); }
				if constexpr (!Disabled) { EXPECT_EQ(Evaluate_cnt(Number<FieldType::F64>{}, tree, bpos, dev_math_in_class_s), DevMathInClass(c)); }
				if constexpr (!Disabled) { EXPECT_EQ(Evaluate_cnt(Number<FieldType::F64>{}, tree, bpos, dev_math_s), DevMath(c, s)); }
				EXPECT_EQ((bool)Evaluate_cnt(Number<FieldType::I08>{}, tree, bpos, is_best_s), IsBest(s, k));
				EXPECT_EQ(Evaluate_cnt(Number<FieldType::I64>{}, tree, bpos, num_stu_200_s), NumStu200(c));
				EXPECT_EQ(Evaluate_cnt(Number<FieldType::I64>{}, tree, bpos, rank_math_class_s), RankMathClass(c, r));
				EXPECT_EQ(Evaluate_cnt(Number<FieldType::I64>{}, tree, bpos, rank_math_all_s), RankMathAll(cls, r));
				EXPECT_EQ(Evaluate_cnt(Number<FieldType::F32>{}, tree, bpos, mean_max_s), MeanMax(c));
				EXPECT_EQ(Evaluate_cnt(Number<FieldType::I32>{}, tree, bpos, sum_math_over60_s), SumMathOver60(s));
				EXPECT_EQ(Evaluate_cnt(Number<FieldType::F32>{}, tree, bpos, mean_math_engover60_2_s), MeanMathEngOver60(c));
				EXPECT_EQ((bool)Evaluate_cnt(Number<FieldType::I08>{}, tree, bpos, is_second_max_math_s), IsSecondMaxMath(s, k));
				EXPECT_EQ((bool)Evaluate_cnt(Number<FieldType::I08>{}, tree, bpos, is_worst_in_third_exam_s), IsWorstInThirdExam(c, j, k));
				EXPECT_EQ(Evaluate_cnt(Number<FieldType::I32>{}, tree, bpos, type_test1_total_score_s), r.m_math + r.m_japanese + r.m_english + r.m_science + r.m_social);
				EXPECT_EQ(Evaluate_cnt(Number<FieldType::I32>{}, tree, bpos, type_test2_total_score_s), r.m_math + r.m_japanese + r.m_english + r.m_science + r.m_social);

				EXPECT_EQ(i, t.GetPos(0));
				EXPECT_EQ(j, t.GetPos(1));
				EXPECT_EQ(k, t.GetPos(2));
				++t;
			}
		}
	}
}

TEST_F(Aggregator, DTree_Evaluate)
{
	//0層要素。学年とクラス。
	auto [grade, class_] = m_dtree.GetPlaceholders("grade", "class_");
	//1層要素。出席番号、名前。
	auto [number, name] = m_dtree.GetPlaceholders("number", "name");
	//2層要素。各試験の点数。前期中間、前期期末、後期中間、後期期末の順に並んでいる。
	auto [exam, math, jpn, eng, sci, soc] = m_dtree.GetPlaceholders("exam", "math", "japanese", "english", "science", "social");

	TestEvaluate(m_dtree, m_class,
				 std::make_tuple(grade, class_),
				 std::make_tuple(number, name),
				 std::make_tuple(exam, math, jpn, eng, sci, soc),
				 std::false_type{});
}
TEST_F(Aggregator, STree_Evaluate)
{
	//0層要素。学年とクラス。
	//auto g = m_stree.GetPlaceholder<0, 0>();
	auto [grade, class_] = m_stree.GetPlaceholders<"grade", "class_">();
	//1層要素。出席番号、名前。
	auto [number, name] = m_stree.GetPlaceholders<"number", "name">();
	//2層要素。各試験の点数。前期中間、前期期末、後期中間、後期期末の順に並んでいる。
	auto [exam, math, jpn, eng, sci, soc] = m_stree.GetPlaceholders<"exam", "math", "japanese", "english", "science", "social">();

	using Tree = decltype(m_stree);
	[[maybe_unused]] constexpr auto names = Tree::GetFieldNamesIn(0_layer);
	static_assert(ctti_placeholder<decltype(grade)>);
	static_assert(s_hierarchy<decltype(m_stree)>);
	TestEvaluate(m_stree, m_class,
				 std::make_tuple(grade, class_),
				 std::make_tuple(number, name),
				 std::make_tuple(exam, math, jpn, eng, sci, soc),
				 std::false_type{});
}

TEST_F(Aggregator, DJoinedContainer_Evaluate_0)
{
	auto a = m_dtree.GetPlaceholder("class_");//MakeHashmapはキーの型を特定する必要があるため、
	auto b = m_dtree.GetPlaceholder("grade");//これらのPlaceholderは予め型情報を与えなければならない。
	auto hash = m_dtree | MakeHashtable(a.i08(), b.i08());

	auto jtree = Join(m_dtree, 0_layer, 0_layer, m_dtree);
	//0層要素。学年とクラス。
	auto [gg, cc] = jtree.GetPlaceholders<0>("grade"_fld, "class_"_fld);
	auto [grade, class_] = jtree.GetPlaceholders<1>("grade"_fld, "class_"_fld);
	//1層要素。出席番号、名前。
	auto [number, name] = jtree.GetPlaceholders<1>("number"_fld, "name"_fld);
	//2層要素。各試験の点数。前期中間、前期期末、後期中間、後期期末の順に並んでいる。
	auto [exam, math, jpn, eng, sci, soc] = jtree.GetPlaceholders<1>("exam", "math", "japanese", "english", "science", "social");

	jtree.SetKeyJoint<1>(std::move(hash), cc, gg);

	TestEvaluate(jtree, m_class,
				 std::make_tuple(grade, class_),
				 std::make_tuple(number, name),
				 std::make_tuple(exam, math, jpn, eng, sci, soc),
				 std::false_type{});
}

TEST_F(Aggregator, DJoinedContainer_Evaluate_1)
{
	auto a = m_dtree.GetPlaceholder("number").i16();//MakeHashmapはキーの型を特定する必要があるため、
	auto b = m_dtree.GetPlaceholder("name").str();//これらのPlaceholderは予め型情報を与えなければならない。
	auto hash = m_dtree | MakeHashtable(a, b);

	auto jtree = Join(m_dtree, 1_layer, 1_layer, m_dtree);
	//0層要素。学年とクラス。
	auto [grade, class_] = jtree.GetPlaceholders<0>("grade"_fld, "class_"_fld);
	//1層要素。出席番号、名前。
	auto [nu, na] = jtree.GetPlaceholders<0>("number"_fld, "name"_fld);
	auto [number, name] = jtree.GetPlaceholders<1>("number"_fld, "name"_fld);
	//2層要素。各試験の点数。前期中間、前期期末、後期中間、後期期末の順に並んでいる。
	auto [exam, math, jpn, eng, sci, soc] = jtree.GetPlaceholders<1>("exam"_fld, "math"_fld, "japanese"_fld, "english"_fld, "science"_fld, "social"_fld);

	jtree.SetKeyJoint<1>(std::move(hash), nu, na);

	TestEvaluate(jtree, m_class,
				 std::make_tuple(grade, class_),
				 std::make_tuple(number, name),
				 std::make_tuple(exam, math, jpn, eng, sci, soc),
				 std::false_type{});
}

TEST_F(Aggregator, DJoinedContainer_Evaluate_2)
{
	auto a = m_dtree.GetPlaceholder("number").i16();//MakeHashmapはキーの型を特定する必要があるため、
	auto b = m_dtree.GetPlaceholder("name").str();//これらのPlaceholderは予め型情報を与えなければならない。
	auto c = m_dtree.GetPlaceholder("exam").i08();
	auto hash = m_dtree | MakeHashtable(a, b, c);

	auto jtree = Join(m_dtree, 2_layer, 2_layer, m_dtree);
	//0層要素。学年とクラス。
	auto [grade, class_] = jtree.GetPlaceholders<0>("grade"_fld, "class_"_fld);
	//1層要素。出席番号、名前。
	auto [nu, na] = jtree.GetPlaceholders<0>("number"_fld, "name"_fld);
	auto [number, name] = jtree.GetPlaceholders<1>("number"_fld, "name"_fld);
	//2層要素。各試験の点数。前期中間、前期期末、後期中間、後期期末の順に並んでいる。
	auto ee = jtree.GetPlaceholder<0>("exam"_fld);
	auto [exam, math, jpn, eng, sci, soc] = jtree.GetPlaceholders<1>("exam"_fld, "math"_fld, "japanese"_fld, "english"_fld, "science"_fld, "social"_fld);

	jtree.SetKeyJoint<1>(std::move(hash), nu, na, ee);

	TestEvaluate(jtree, m_class,
				 std::make_tuple(grade, class_),
				 std::make_tuple(number, name),
				 std::make_tuple(exam, math, jpn, eng, sci, soc),
				 std::true_type{});
}

TEST_F(Aggregator, DJoinedContainer_Evaluate_0_1)
{
	auto c = m_dtree.GetPlaceholder("class_").i08();//MakeHashmapはキーの型を特定する必要があるため、
	auto g = m_dtree.GetPlaceholder("grade").i08();//これらのPlaceholderは予め型情報を与えなければならない。
	auto hash1 = m_dtree | MakeHashtable(c, g);

	auto a = m_dtree.GetPlaceholder("number").i16();//MakeHashmapはキーの型を特定する必要があるため、
	auto b = m_dtree.GetPlaceholder("name").str();//これらのPlaceholderは予め型情報を与えなければならない。
	auto hash2 = m_dtree | MakeHashtable(a, b);

	auto jtree = Join(m_dtree, 0_layer, 0_layer, m_dtree, 1_layer, 1_layer, m_dtree);
	//0層要素。学年とクラス。
	auto [grade, class_] = jtree.GetPlaceholders<0>("grade"_fld, "class_"_fld);
	//1層要素。出席番号、名前。
	auto [number, name] = jtree.GetPlaceholders<1>("number"_fld, "name"_fld);
	//2層要素。各試験の点数。前期中間、前期期末、後期中間、後期期末の順に並んでいる。
	[[maybe_unused]] auto ee = jtree.GetPlaceholder<1>("exam"_fld);
	auto [exam, math, jpn, eng, sci, soc] = jtree.GetPlaceholders<2>("exam"_fld, "math"_fld, "japanese"_fld, "english"_fld, "science"_fld, "social"_fld);

	jtree.SetKeyJoint<1>(std::move(hash1), class_, grade);
	jtree.SetKeyJoint<2>(std::move(hash2), number, name);

	TestEvaluate(jtree, m_class,
				 std::make_tuple(grade, class_),
				 std::make_tuple(number, name),
				 std::make_tuple(exam, math, jpn, eng, sci, soc),
				 std::false_type{});
}
TEST_F(Aggregator, DJoinedContainer_Evaluate_1_2)
{
	auto a = m_dtree.GetPlaceholder("number").i16();//MakeHashmapはキーの型を特定する必要があるため、
	auto b = m_dtree.GetPlaceholder("name").str();//これらのPlaceholderは予め型情報を与えなければならない。
	auto hash1 = m_dtree | MakeHashtable(a, b);

	auto c = m_dtree.GetPlaceholder("exam").i08();
	auto hash2 = m_dtree | MakeHashtable(a, b, c);

	auto jtree = Join(m_dtree, 1_layer, 1_layer, m_dtree, 2_layer, 2_layer, m_dtree);
	//0層要素。学年とクラス。
	auto [grade, class_] = jtree.GetPlaceholders<0>("grade"_fld, "class_"_fld);
	//1層要素。出席番号、名前。
	auto [nu, na] = jtree.GetPlaceholders<0>("number"_fld, "name"_fld);
	auto [number, name] = jtree.GetPlaceholders<1>("number"_fld, "name"_fld);
	//2層要素。各試験の点数。前期中間、前期期末、後期中間、後期期末の順に並んでいる。
	auto ee = jtree.GetPlaceholder<1>("exam"_fld);
	auto [exam, math, jpn, eng, sci, soc] = jtree.GetPlaceholders<2>("exam"_fld, "math"_fld, "japanese"_fld, "english"_fld, "science"_fld, "social"_fld);

	jtree.SetKeyJoint<1>(std::move(hash1), nu, na);
	jtree.SetKeyJoint<2>(std::move(hash2), nu, na, ee);

	TestEvaluate(jtree, m_class,
				 std::make_tuple(grade, class_),
				 std::make_tuple(number, name),
				 std::make_tuple(exam, math, jpn, eng, sci, soc),
				 std::true_type{});
}

template <class Table, class Layer>
void TestEvaluate(Table& table, const std::vector<Class>& cls, const Layer& l)
{
	auto [class_, name, number, exam, math, jpn, eng, sci, soc] = l;

	auto sum5 = math + jpn + eng + sci + soc;
	auto mean_math0 = sum((exam == 0) * math) / 120.;
	InitAll(table, sum5, mean_math0);
	using enum FieldType;
	BindexType i = 2;
	BindexType j = 15;
	BindexType k = 3;
	Bpos bpos{ i * 4 * 30 + j * 4 + k };
	auto eval1 = [&table, &bpos]<class Node, FieldType Type>(const Node & node, Number<Type>)
	{
		if constexpr (stat_type_node<decltype(node)>) return node.Evaluate(table, bpos);
		else return node.Evaluate(table, bpos).template as<Type>();
	};
	auto eval0 = [&table]<class Node, FieldType Type>(const Node& node, Number<Type>)
	{
		if constexpr (stat_type_node<decltype(node)>) return node.Evaluate(table);
		else return node.Evaluate(table).template as<Type>();
	};
	const auto& r = cls[i].m_students[j].m_records[k];
	EXPECT_EQ(eval1(sum5, Number<I32>{}), Sum5Subjs(r));
	EXPECT_EQ(eval0(mean_math0, Number<F64>{}), MeanMath0(cls));
}
TEST_F(Aggregator, STable_Evaluate)
{
	const auto& t = m_stable;
	auto [class_, name, number, exam, math, jpn, eng, sci, soc] =
		t.GetPlaceholders("class_"_fld, "name"_fld, "number"_fld,
			"exam"_fld, "math"_fld, "japanese"_fld, "english"_fld, "science"_fld, "social"_fld);
	TestEvaluate(t, m_class, std::make_tuple(class_, name, number, exam, math, jpn, eng, sci, soc));
}
TEST_F(Aggregator, DTable_Evaluate)
{
	const auto& t = m_dtable;
	auto [class_, name, number, exam, math, jpn, eng, sci, soc] =
		t.GetPlaceholders("class_", "name", "number",
			"exam", "math", "japanese", "english", "science", "social");
	TestEvaluate(t, m_class, std::make_tuple(class_, name, number, exam, math, jpn, eng, sci, soc));
}
TEST_F(Aggregator, DJoinedTable_Evaluate)
{
	auto& t = m_dtable;
	auto hash = [&t]()
	{
		auto a = t.GetPlaceholder("number").i16();//MakeHashmapはキーの型を特定する必要があるため、
		auto b = t.GetPlaceholder("name").str();//これらのPlaceholderは予め型情報を与えなければならない。
		auto c = t.GetPlaceholder("exam").i08();//これらのPlaceholderは予め型情報を与えなければならない。
		return t | MakeHashtable(a, b, c);
	}();

	auto jtree = Join(t, 0_layer, 0_layer, t);
	auto [grade, class_, number, name, exam0] = jtree.GetPlaceholders<0>("grade"_fld, "class_"_fld, "number"_fld, "name"_fld, "exam"_fld);
	auto [exam1, math, jpn, eng, sci, soc] = jtree.GetPlaceholders<1>("exam"_fld, "math"_fld, "japanese"_fld, "english"_fld, "science"_fld, "social"_fld);

	jtree.SetKeyJoint<1>(std::move(hash), number, name, exam0);

	TestEvaluate(jtree, m_class,
		std::make_tuple(class_, name, number, exam1, math, jpn, eng, sci, soc));
}