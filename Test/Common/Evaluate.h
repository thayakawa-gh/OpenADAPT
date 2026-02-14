#ifndef ADAPT_TEST_COMMON_EVALUATE_H
#define ADAPT_TEST_COMMON_EVALUATE_H

#include <Test/Common/Aggregator.h>
#include <ranges>
#include <iterator>

template <node_or_placeholder NP, class Func, LayerType Layer_, FieldType Type_>
struct EvalComp
{
	static constexpr LayerType Layer = Layer_;
	static constexpr FieldType Type = Type_;
	EvalComp(const NP& np, Func f, LayerConstant<Layer_>, Number<Type_>)
		: m_np(np), m_np_s(np), m_func(f)
	{}
	static constexpr bool IsCtti = ctti_node_or_placeholder<NP>;
	static constexpr bool IsRtti = rtti_node_or_placeholder<NP>;
	NP m_np;
	NP m_np_s;
	Func m_func;
};
template <any_container Cont, any_traverser Trav, class Func>
void Evaluate(const Cont& c, const Trav& t, const Bpos& bpos, Func&& f, const std::vector<Class>& cls, BindexType i, BindexType j, BindexType k)
{
	auto Evaluate_trav = []<FieldType Type, class Node>(Number<Type>, const Trav & t, const Node & node)
	{
		if constexpr (stat_type_node<Node>) return node.Evaluate(t);
		else return node.Evaluate(t).template as<Type>();
	};
	auto Evaluate_cnt = []<FieldType Type, class Node>(Number<Type>, const Cont & t, const Bpos & bpos, const Node & node)
	{
		if constexpr (stat_type_node<Node>) return node.Evaluate(t, bpos);
		else return node.Evaluate(t, bpos).template as<Type>();
	};
	EXPECT_EQ(Evaluate_trav(Number<std::decay_t<Func>::Type>{}, t, f.m_np), f.m_func(cls, i, j, k));
	EXPECT_EQ(Evaluate_cnt(Number<std::decay_t<Func>::Type>{}, c, bpos, f.m_np_s), f.m_func(cls, i, j, k));
}
template <class Func>
void CompLayerType(const Func& f)
{
	if constexpr (Func::IsCtti)
	{
		static_assert(f.m_np.GetLayer() == f.m_layer);
		static_assert(f.m_np_s.GetLayer() == f.m_layer);
		static_assert(std::same_as<typename decltype(f.m_np)::RetType, DFieldInfo::TagTypeToValueType<f.m_type.value>>);
	}
	else
	{
		assert(f.m_np.GetLayer() == f.m_layer);
		assert(f.m_np_s.GetLayer() == f.m_layer);
		assert(f.m_np.GetType() == f.m_type.value);
	}
}

template <any_tree Tree, class ...Funcs>
void TestEvaluate_impl(Tree& tree, const std::vector<Class>& cls, Funcs&& ...fs)
{
	[[maybe_unused]] bool is_joined_container = joined_container<Tree>;

	auto range = tree.GetRange(2);
	auto t = range.begin();
	Bpos bpos(2);

	InitAll(t, fs.m_np...);
	InitAll(tree, bpos, fs.m_np_s...);

	for (BindexType i = 0; i < (BindexType)cls.size(); ++i)
	{
		bpos[0] = i;
		const Class& c = cls[i];
		for (BindexType j = 0; j < (BindexType)c.m_students.size(); ++j)
		{
			bpos[1] = j;
			const Student& s = c.m_students[j];
			for (BindexType k = 0; k < (BindexType)s.m_records.size(); ++k)
			{
				bpos[2] = k;
				//const Record& r = s.m_records[k];

				DoNothing((Evaluate(tree, t, bpos, fs, cls, i, j, k), 0)...);
				EXPECT_EQ(i, t.GetPos(0));
				EXPECT_EQ(j, t.GetPos(1));
				EXPECT_EQ(k, t.GetPos(2));
				++t;
			}
		}
	}
}
#define DECL_FUNC(NAME)\
template <any_tree Tree, class Layer0, class Layer1, class Layer2>\
void NAME(Tree& tree, const std::vector<Class>& cls, \
				const Layer0& l0, const Layer1& l1, const Layer2& l2)
#define EXPAND_PH\
	using enum FieldType;\
	[[maybe_unused]] auto [grade, class_] = l0;\
	[[maybe_unused]] auto [number, name] = l1;\
	[[maybe_unused]] auto [exam, math, jpn, eng, sci, soc] = l2;

DECL_FUNC(TestNormalFunc)
{
	EXPAND_PH;

	auto num_name = tostr(number) + ":" + name;
	auto total_score = math + jpn + eng + sci + soc;//その試験の合計点。

	TestEvaluate_impl(tree, cls,
					  EvalComp(num_name, &NumName, 1_layer, Number<Str>{}),
					  EvalComp(total_score, &TotalScore, 2_layer, Number<I32>{}));
}
DECL_FUNC(TestShortCircuit)
{
	EXPAND_PH;
	auto has_failed = math < 40 || jpn < 40;//数学または国語で落第点を取ったかどうか。落第点は40点未満とする。
	auto both_failed = math < 40 && jpn < 40;//数学と国語の両方で落第点を取ったかどうか。
	TestEvaluate_impl(tree, cls,
					  EvalComp(has_failed, &HasFailed, 2_layer, Number<I08>{}),
					  EvalComp(both_failed, &BothFailed, 2_layer, Number<I08>{}));
}

DECL_FUNC(TestSimpleLayerFunc)
{
	EXPAND_PH;

	auto total_score = math + jpn + eng + sci + soc;//その試験の合計点。
	auto all_400 = count_if(total_score >= 400) == 4;//全試験で400点以上を取っているかどうか。
	auto mean_math = mean(cast_f32(math));//生徒ごとの数学4回分の平均点。mathは整数型なので、浮動小数点にキャストしてから計算する。

	TestEvaluate_impl(tree, cls,
					  EvalComp(all_400, &All400, 1_layer, Number<I08>{}),
					  EvalComp(mean_math, &AvgMath, 1_layer, Number<F32>{}));
}

DECL_FUNC(TestAtOperator)
{
	EXPAND_PH;
	auto mean_math_in_class = mean(cast_f64(math.at(0)));//前期中間試験の数学のクラス内平均点。
	auto dev_math_in_class = dev(cast_f64(math.at(0)));//前期中間試験の数学のクラス内標準偏差。
	auto dev_math = (math.at(0) - mean_math_in_class) * 10. / dev_math_in_class + 50.;//前期中間試験の数学のクラス内偏差値。
	TestEvaluate_impl(tree, cls,
					  EvalComp(mean_math_in_class, &AvgMathInClass, 0_layer, Number<F64>{}),
					  EvalComp(dev_math_in_class, &DevMathInClass, 0_layer, Number<F64>{}),
					  EvalComp(dev_math, &DevMath, 1_layer, Number<F64>{}));
}

DECL_FUNC(TestNestedLayerFunc)
{
	EXPAND_PH;
	auto total_score = math + jpn + eng + sci + soc;//その試験の合計点。
	auto is_best = isgreatest(total_score);//4回のうち最高点のときにtrue。
	auto num_stu_200 = count_if(exist(jpn + math + eng >= 200));//クラス内で3科目200点以上を取ったことのある生徒の数。
	auto mean_max = mean(cast_f32(greatest(total_score)));
	TestEvaluate_impl(tree, cls,
					  EvalComp(is_best, &IsBest, 2_layer, Number<I08>{}),
					  EvalComp(num_stu_200, &NumStu200, 0_layer, Number<I64>{}),
					  EvalComp(mean_max, &MeanMax, 0_layer, Number<F32>{}));
}

DECL_FUNC(TestOuterField)
{
	EXPAND_PH;
	auto rank_math_class = count_if2(exam == exam.o(0_depth) && math > math.o(0_depth));//各試験の数学クラス内順位。自分より好成績な生徒の人数を数えている。o(0)はouter(0)の短縮表記。
	auto rank_math_all = count_if3(exam == exam.o(0_depth) && math > math.o(0_depth));//各試験の学年順位。数える範囲がクラス内ではなく学年全体になる。
	TestEvaluate_impl(tree, cls,
					  EvalComp(rank_math_class, &RankMathClass, 2_layer, Number<I64>{}),
					  EvalComp(rank_math_all, &RankMathAll, 2_layer, Number<I64>{}));
}

DECL_FUNC(TestConditionalLayerFunc)
{
	EXPAND_PH;
	auto total_score = math + jpn + eng + sci + soc;//その試験の合計点。
	auto sum_math_over60 = sum_if(math, math > 60);
	auto mean_math_engover60_2 = mean_if2(cast_f32(math), sum(eng) >= 240);
	auto is_second_max_math = isgreatest_if(math, !isgreatest(math));//数学が2番目に高得点だったときの試験でtrue。
	auto is_worst_in_third_exam = isleast_if2(total_score, exam == 2);//後期中間で最低点を取った生徒の試験でtrue。
	TestEvaluate_impl(tree, cls,
					  EvalComp(sum_math_over60, &SumMathOver60, 1_layer, Number<I32>{}),
					  EvalComp(mean_math_engover60_2, &MeanMathEngOver60, 0_layer, Number<F32>{}),
					  EvalComp(is_second_max_math, &IsSecondMaxMath, 2_layer, Number<I08>{}),
					  EvalComp(is_worst_in_third_exam, &IsWorstInThirdExam, 2_layer, Number<I08>{}));
}

DECL_FUNC(TestCttiRttiConversion)
{
	EXPAND_PH;
	static constexpr bool IsAllCtti = ctti_placeholder<decltype(grade)> && ctti_placeholder<decltype(class_)> &&
		ctti_placeholder<decltype(number)> && ctti_placeholder<decltype(name)> &&
		ctti_placeholder<decltype(exam)> && ctti_placeholder<decltype(math)> && ctti_placeholder<decltype(jpn)> &&
		ctti_placeholder<decltype(eng)> && ctti_placeholder<decltype(sci)> && ctti_placeholder<decltype(soc)>;
	static constexpr bool IsAllRtti = rtti_placeholder<decltype(grade)> && rtti_placeholder<decltype(class_)> &&
		rtti_placeholder<decltype(number)> && rtti_placeholder<decltype(name)> &&
		rtti_placeholder<decltype(exam)> && rtti_placeholder<decltype(math)> && rtti_placeholder<decltype(jpn)> &&
		rtti_placeholder<decltype(eng)> && rtti_placeholder<decltype(sci)> && rtti_placeholder<decltype(soc)>;
	static_assert(IsAllCtti || IsAllRtti);
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
	TestEvaluate_impl(tree, cls,
					  EvalComp(type_test1_total_score, &TotalScore, 2_layer, Number<I32>{}),
					  EvalComp(type_test2_total_score, &TotalScore, 2_layer, Number<I32>{}));
}

template <any_tree Tree, class Layer0, class Layer1, class Layer2>
void TestIndexAndSize(const Tree& t, const Layer0& l0, const Layer1& l1, const Layer2& l2)
{
	static constexpr bool IsJoinedContainer = joined_container<Tree>;
	//0層要素。学年とクラス。
	[[maybe_unused]] auto [grade, class_] = l0;
	//1層要素。出席番号、名前、生年月日。
	[[maybe_unused]] auto [number, name] = l1;
	//2層要素。各試験の点数。前期中間、前期期末、後期中間、後期期末の順に並んでいる。
	[[maybe_unused]] auto [exam, math, jpn, eng, sci, soc] = l2;

	[[maybe_unused]] auto size_vs_size = ADAPT_DECL_IF(!IsJoinedContainer, countall(exam) == t.size(2_layer));
	[[maybe_unused]] auto size2 = ADAPT_DECL_IF(!IsJoinedContainer, t.size(0_layer, 2_layer) == 120);
	auto pos0 = t.pos(0_layer);
	auto pos1 = t.pos(1_layer);
	auto pos2 = t.pos(2_layer);
	auto current_math = math.at(pos0, pos1, pos2);// 常にmathそれ自体と等しい。
	if constexpr (!IsJoinedContainer) { EXPECT_EQ(size_vs_size.GetType(), FieldType::I08); }
	if constexpr (!IsJoinedContainer) { EXPECT_EQ(size2.GetType(), FieldType::I08); }
	EXPECT_EQ(pos0.GetType(), FieldType::I64);
	EXPECT_EQ(pos1.GetType(), FieldType::I64);
	EXPECT_EQ(pos2.GetType(), FieldType::I64);
	EXPECT_EQ(current_math.GetType(), FieldType::I32);
	if constexpr (!IsJoinedContainer) { EXPECT_EQ(size_vs_size.GetLayer(), 1_layer); }
	if constexpr (!IsJoinedContainer) { EXPECT_EQ(size2.GetLayer(), 0_layer); }
	EXPECT_EQ(pos0.GetLayer(), 0_layer);
	EXPECT_EQ(pos1.GetLayer(), 1_layer);
	EXPECT_EQ(pos2.GetLayer(), 2_layer);
	EXPECT_EQ(current_math.GetLayer(), 2_layer);

	if constexpr (!IsJoinedContainer) InitAll(t, size_vs_size, size2);
	InitAll(t, pos0, pos1, pos2, current_math);

	auto Evaluate_trav = []<FieldType Type, class Trav, class Node>(Number<Type>, const Trav & trav, const Node & node)
	{
		if constexpr (stat_type_node_or_placeholder<Node>) return node.Evaluate(trav);
		else return node.Evaluate(trav).template as<Type>();
	};
	using enum FieldType;
	for (const auto& trav : t.GetRange(2_layer))
	{
		if constexpr (!IsJoinedContainer) { EXPECT_TRUE(Evaluate_trav(Number<I08>{}, trav, size_vs_size)); }
		if constexpr (!IsJoinedContainer) { EXPECT_TRUE(Evaluate_trav(Number<I08>{}, trav, size2)); }
		EXPECT_EQ(Evaluate_trav(Number<I64>{}, trav, pos0), trav.GetPos(0));
		EXPECT_EQ(Evaluate_trav(Number<I64>{}, trav, pos1), trav.GetPos(1));
		EXPECT_EQ(Evaluate_trav(Number<I64>{}, trav, pos2), trav.GetPos(2));
		EXPECT_EQ(Evaluate_trav(Number<I32>{}, trav, current_math), Evaluate_trav(Number<I32>{}, trav, math));
	}
}


// DJoinedContainerをtable扱いで受け取る場合があるのでany_containerにしている。
template <any_container Table, class Layer>
void TestEvaluate(Table& table, const std::vector<Class>& cls, const Layer& l)
{
	auto [class_, number, name, exam, math, jpn, eng, sci, soc] = l;

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
	auto eval0 = [&table]<class Node, FieldType Type>(const Node & node, Number<Type>)
	{
		if constexpr (stat_type_node<decltype(node)>) return node.Evaluate(table);
		else return node.Evaluate(table).template as<Type>();
	};
	EXPECT_EQ(eval1(sum5, Number<I32>{}), TotalScore(cls, i, j, k));
	EXPECT_EQ(eval0(mean_math0, Number<F64>{}), MeanMath0(cls));
}

#endif