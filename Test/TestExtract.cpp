#include <Test/Aggregator.h>

using namespace adapt;
using namespace adapt::lit;

template <class Container, class Layer0, class Layer1, class Layer2>
void TestExtract(Container& s, const std::vector<Class>& clses, Layer0 l0, Layer1 l1, Layer2 l2)
{
	SetNumOfThreads(16);
	auto Evaluate = []<FieldType Type, class Trav, class NP>(Number<Type>, const Trav & t, NP & np)
	{
		if constexpr (stat_type_node_or_placeholder<NP>) return np(t);
		else return np(t).template as<Type>();
	};
	using enum adapt::FieldType;
	//test 1
	{
		//0層要素。学年とクラス。
		auto [grade, class_] = l0;
		//1層要素。出席番号、名前、生年月日。
		auto [number, name] = l1;
		//2層要素。各試験の点数。前期中間、前期期末、後期中間、後期期末の順に並んでいる。
		auto [exam, math, jpn, eng, sci, soc] = l2;

		auto count_350 = count(exist(math + jpn + eng + sci + soc >= 350));
		auto mean_sum = mean(cast_f64(math + jpn + eng + sci + soc));
		auto sum = math + jpn + eng + sci + soc;
		auto isbest = isgreatest(sum);
		if constexpr (s_container<Container>)
		{
			static_assert(count_350.GetLayer() == 0_layer);
			static_assert(mean_sum.GetLayer() == 1_layer);
			static_assert(sum.GetLayer() == 2_layer);
			static_assert(isbest.GetLayer() == 2_layer);
		}
		else
		{
			assert(count_350.GetLayer() == 0_layer);
			assert(mean_sum.GetLayer() == 1_layer);
			assert(sum.GetLayer() == 2_layer);
			assert(isbest.GetLayer() == 2_layer);
		}

		auto res = s | Extract(class_, count_350, name, mean_sum, sum, isbest);

		auto [class__, count_350_, name_, mean_sum_, sum_, isbest_] = res.GetPlaceholders("fld0"_fld, "fld1"_fld, "fld2"_fld, "fld3"_fld, "fld4"_fld, "fld5"_fld);

		static_assert(std::ranges::input_range<decltype(s.GetRange(2))>);
		InitAll(count_350, mean_sum, sum, isbest);

		//confirm that the extracted values are correct.
		for (auto [t, r] : views::Zip(s.GetRange(2), res.GetRange(2)))
		{
			EXPECT_EQ(Evaluate(Number<I08>{}, t, class_), Evaluate(Number<I08>{}, r, class__));
			EXPECT_EQ(Evaluate(Number<I64>{}, t, count_350), Evaluate(Number<I64>{}, r, count_350_));
			EXPECT_EQ(Evaluate(Number<Str>{}, t, name), Evaluate(Number<Str>{}, r, name_));
			EXPECT_EQ(Evaluate(Number<F64>{}, t, mean_sum), Evaluate(Number<F64>{}, r, mean_sum_));
			EXPECT_EQ(Evaluate(Number<I32>{}, t, sum), Evaluate(Number<I32>{}, r, sum_));
			EXPECT_EQ(Evaluate(Number<I08>{}, t, isbest), Evaluate(Number<I08>{}, r, isbest_));
		}
	}
	//filter
	//Filter(sum >= 300)が正常に機能しているかどうかを確認するためのもの。
	//text2で誤りがあったとき、それがFilter単体で起こりうるのか、Extractと併用して初めて起こる問題であるかを識別する。
	{
		//0層要素。学年とクラス。
		//auto [grade, class_] = l0;
		//1層要素。出席番号、名前、生年月日。
		//auto [number, name, dob] = l1;
		//2層要素。各試験の点数。前期中間、前期期末、後期中間、後期期末の順に並んでいる。
		auto [exam, math, jpn, eng, sci, soc] = l2;

		auto sum = math + jpn + eng + sci + soc;
		auto bestscore = max(math, max(jpn, max(eng, max(sci, soc))));
		auto worstscore = min(math, min(jpn, min(eng, min(sci, soc))));

		auto range = s | Filter(sum >= 300) | GetRange(2_layer);
		auto trav = range.begin();

		for (BindexType i = 0; i < clses.size(); ++i)
		{
			auto& c = clses[i];
			for (BindexType j = 0; j < c.m_students.size(); ++j)
			{
				auto& st = c.m_students[j];
				for (BindexType k = 0; k < st.m_records.size(); ++k)
				{
					auto& r = st.m_records[k];
					if (r.m_math + r.m_japanese + r.m_english + r.m_science + r.m_social < 300) continue;

					EXPECT_EQ(i, trav.GetPos(0));
					EXPECT_EQ(j, trav.GetPos(1));
					EXPECT_EQ(k, trav.GetPos(2));

					EXPECT_EQ(Evaluate(Number<I32>{}, trav, bestscore), std::max({ r.m_math, r.m_japanese, r.m_english, r.m_science, r.m_social }));
					EXPECT_EQ(Evaluate(Number<I32>{}, trav, worstscore), std::min({ r.m_math, r.m_japanese, r.m_english, r.m_science, r.m_social }));

					++trav;
				}
			}
		}
	}
	//test 2
	{
		//0層要素。学年とクラス。
		//auto [grade, class_] = l0;
		//1層要素。出席番号、名前、生年月日。
		//auto [number, name, dob] = l1;
		//2層要素。各試験の点数。前期中間、前期期末、後期中間、後期期末の順に並んでいる。
		auto [exam, math, jpn, eng, sci, soc] = l2;

		auto sum = math + jpn + eng + sci + soc;
		auto bestscore = max(math, max(jpn, max(eng, max(sci, soc))));
		auto worstscore = min(math, min(jpn, min(eng, min(sci, soc))));
		if constexpr (s_container<Container>)
		{
			static_assert(sum.GetLayer() == 2_layer);
			static_assert(bestscore.GetLayer() == 2_layer);
			static_assert(worstscore.GetLayer() == 2_layer);
		}
		else
		{
			assert(sum.GetLayer() == 2_layer);
			assert(bestscore.GetLayer() == 2_layer);
			assert(worstscore.GetLayer() == 2_layer);
		}

		auto res = s | Filter(sum >= 300) | Extract(bestscore, worstscore);

		auto [best, worst] = res.GetPlaceholders("fld0"_fld, "fld1"_fld);

		static_assert(std::ranges::input_range<decltype(s.GetRange(2))>);
		InitAll(sum, bestscore, worstscore);

		Bpos spos(2);
		Bpos rpos(2);
		auto srange = s | Filter(sum >= 300) | GetRange(2);
		auto rrange = res.GetRange(2);
		//confirm that the extracted values are correct.
		for (auto [t, r] : views::Zip(srange, rrange))
		{
			int32_t sbest = Evaluate(Number<I32>{}, t, bestscore);
			int32_t sworst = Evaluate(Number<I32>{}, t, worstscore);
			int32_t rbest = Evaluate(Number<I32>{}, r, best);
			int32_t rworst = Evaluate(Number<I32>{}, r, worst);
			if (sbest != rbest || sworst != rworst)
			{
				t.GetBpos(spos);
				r.GetBpos(rpos);
				std::cout << "mismatch found" << spos << " " << rpos << std::endl;
			}
			EXPECT_EQ(Evaluate(Number<I32>{}, t, bestscore), Evaluate(Number<I32>{}, r, best));
			EXPECT_EQ(Evaluate(Number<I32>{}, t, worstscore), Evaluate(Number<I32>{}, r, worst));
		}
	}
	//filter3
	{
		//AssignRowで0層の条件を満たさなかった時のバグが見つかったため、そのチェック。
		//0層要素。学年とクラス。
		auto [grade, class_] = l0;
		//1層要素。出席番号、名前、生年月日。
		//auto [number, name, dob] = l1;
		//2層要素。各試験の点数。前期中間、前期期末、後期中間、後期期末の順に並んでいる。
		auto [exam, math, jpn, eng, sci, soc] = l2;
		auto math_1 = math.at(1);

		auto e = s | Filter(class_ == 1) | Extract(math_1);
		auto [math_1_] = e.GetPlaceholders("fld0"_fld);
		auto srange = s | Filter(class_ == 1) | GetRange(1);
		auto erange = e.GetRange(1);
		for (auto [t, r] : views::Zip(srange, erange))
		{
			//EXPECT_EQ(math_1(t).i32(), r[math_1_].i32());
			EXPECT_EQ(Evaluate(Number<I32>{}, t, math_1), Evaluate(Number<I32>{}, r, math_1_));
		}
	}
	//all_fields
	if constexpr (adapt::container_simplex<Container>)
	{
		//0層要素。学年とクラス。
		auto [grade, class_] = l0;
		//1層要素。出席番号、名前、生年月日。
		auto [number, name] = l1;
		//2層要素。各試験の点数。前期中間、前期期末、後期中間、後期期末の順に並んでいる。
		auto [exam, math, jpn, eng, sci, soc] = l2;

		auto e = s | Extract(flags::all_fields, math + jpn + eng + sci + soc);
		auto [egrade, eclass_] = e.GetPlaceholders("grade"_fld, "class_"_fld);
		auto [enumber, ename] = e.GetPlaceholders("number"_fld, "name"_fld);
		auto [eexam, emath, ejpn, eeng, esci, esoc, fld0] = e.GetPlaceholders("exam"_fld, "math"_fld, "japanese"_fld, "english"_fld, "science"_fld, "social"_fld, "fld0"_fld);

		auto school = s.GetPlaceholder("school"_fld);
		auto eschool = e.GetPlaceholder("school"_fld);
		for (auto [st, et] : views::Zip(s.GetRange(2_layer), e.GetRange(2_layer)))
		{
			EXPECT_EQ(Evaluate(Number<Str>{}, st, school), Evaluate(Number<Str>{}, et, eschool));
			EXPECT_EQ(Evaluate(Number<I08>{}, st, grade), Evaluate(Number<I08>{}, et, egrade));
			EXPECT_EQ(Evaluate(Number<I08>{}, st, class_), Evaluate(Number<I08>{}, et, eclass_));
			EXPECT_EQ(Evaluate(Number<I16>{}, st, number), Evaluate(Number<I16>{}, et, enumber));
			EXPECT_EQ(Evaluate(Number<Str>{}, st, name), Evaluate(Number<Str>{}, et, ename));
			EXPECT_EQ(Evaluate(Number<I08>{}, st, exam), Evaluate(Number<I08>{}, et, eexam));
			EXPECT_EQ(Evaluate(Number<I32>{}, st, math), Evaluate(Number<I32>{}, et, emath));
			EXPECT_EQ(Evaluate(Number<I32>{}, st, jpn), Evaluate(Number<I32>{}, et, ejpn));
			EXPECT_EQ(Evaluate(Number<I32>{}, st, eng), Evaluate(Number<I32>{}, et, eeng));
			EXPECT_EQ(Evaluate(Number<I32>{}, st, sci), Evaluate(Number<I32>{}, et, esci));
			EXPECT_EQ(Evaluate(Number<I32>{}, st, soc), Evaluate(Number<I32>{}, et, esoc));
			auto sum = math + jpn + eng + sci + soc;
			EXPECT_EQ(Evaluate(Number<I32>{}, st, sum), Evaluate(Number<I32>{}, et, fld0));
		}
	}
	//all_fields + filter
	if constexpr (adapt::container_simplex<Container>)
	{
		//0層要素。学年とクラス。
		auto [grade, class_] = l0;
		//1層要素。出席番号、名前、生年月日。
		auto [number, name] = l1;
		//2層要素。各試験の点数。前期中間、前期期末、後期中間、後期期末の順に並んでいる。
		auto [exam, math, jpn, eng, sci, soc] = l2;

		auto e = s | Filter(soc < 60) | Extract(flags::all_fields, math + jpn + eng + sci + soc);
		auto [egrade, eclass_] = e.GetPlaceholders("grade"_fld, "class_"_fld);
		auto [enumber, ename] = e.GetPlaceholders("number"_fld, "name"_fld);
		auto [eexam, emath, ejpn, eeng, esci, esoc, fld0] = e.GetPlaceholders("exam"_fld, "math"_fld, "japanese"_fld, "english"_fld, "science"_fld, "social"_fld, "fld0"_fld);

		auto school = s.GetPlaceholder("school"_fld);
		auto eschool = e.GetPlaceholder("school"_fld);
		auto srange = s | Filter(soc < 60) | GetRange(2_layer);
		for (auto [st, et] : views::Zip(srange, e.GetRange(2_layer)))
		{
			EXPECT_EQ(Evaluate(Number<Str>{}, st, school), Evaluate(Number<Str>{}, et, eschool));
			EXPECT_EQ(Evaluate(Number<I08>{}, st, grade), Evaluate(Number<I08>{}, et, egrade));
			EXPECT_EQ(Evaluate(Number<I08>{}, st, class_), Evaluate(Number<I08>{}, et, eclass_));
			EXPECT_EQ(Evaluate(Number<I16>{}, st, number), Evaluate(Number<I16>{}, et, enumber));
			EXPECT_EQ(Evaluate(Number<Str>{}, st, name), Evaluate(Number<Str>{}, et, ename));
			EXPECT_EQ(Evaluate(Number<I08>{}, st, exam), Evaluate(Number<I08>{}, et, eexam));
			EXPECT_EQ(Evaluate(Number<I32>{}, st, math), Evaluate(Number<I32>{}, et, emath));
			EXPECT_EQ(Evaluate(Number<I32>{}, st, jpn), Evaluate(Number<I32>{}, et, ejpn));
			EXPECT_EQ(Evaluate(Number<I32>{}, st, eng), Evaluate(Number<I32>{}, et, eeng));
			EXPECT_EQ(Evaluate(Number<I32>{}, st, sci), Evaluate(Number<I32>{}, et, esci));
			EXPECT_EQ(Evaluate(Number<I32>{}, st, soc), Evaluate(Number<I32>{}, et, esoc));
			auto sum = math + jpn + eng + sci + soc;
			EXPECT_EQ(Evaluate(Number<I32>{}, st, sum), Evaluate(Number<I32>{}, et, fld0));
		}
	}
}

TEST_F(Aggregator, DTree_Extract)
{
	//0層要素。学年とクラス。
	auto [grade, class_] = m_dtree.GetPlaceholders("grade", "class_");
	//1層要素。出席番号、名前、生年月日。
	auto [number, name] = m_dtree.GetPlaceholders("number", "name");
	//2層要素。各試験の点数。前期中間、前期期末、後期中間、後期期末の順に並んでいる。
	auto [exam, math, jpn, eng, sci, soc] = m_dtree.GetPlaceholders("exam", "math", "japanese", "english", "science", "social");

	TestExtract(m_dtree, m_class,
				 std::make_tuple(grade, class_),
				 std::make_tuple(number, name),
				 std::make_tuple(exam, math, jpn, eng, sci, soc));
}

TEST_F(Aggregator, STree_Extract)
{
	//0層要素。学年とクラス。
	auto [grade, class_] = m_stree.GetPlaceholders<"grade", "class_">();
	//1層要素。出席番号、名前。
	auto [number, name] = m_stree.GetPlaceholders<"number", "name">();
	//2層要素。各試験の点数。前期中間、前期期末、後期中間、後期期末の順に並んでいる。
	auto [exam, math, jpn, eng, sci, soc] = m_stree.GetPlaceholders<"exam", "math", "japanese", "english", "science", "social">();
	static_assert(s_named_node_or_placeholder<decltype(name.named(""_fld))>);
	TestExtract(m_stree, m_class,
				std::make_tuple(grade, class_),
				std::make_tuple(number, name),
				std::make_tuple(exam, math, jpn, eng, sci, soc));
}

TEST_F(Aggregator, DJoinedContainer_Extract_1)
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

	TestExtract(jtree, m_class,
				std::make_tuple(grade, class_),
				std::make_tuple(number, name),
				std::make_tuple(exam, math, jpn, eng, sci, soc));
}


template <class Container, class Layer>
void TestExtract(Container& s, Layer l)
{
	auto [grade, class_, number, name, exam, math, jpn, eng, sci, soc] = l;

	auto sum = math + jpn + eng + sci + soc;
	auto topsubj = max(math, max(jpn, max(eng, max(sci, soc))));
	if constexpr (s_container<Container>)
	{
		static_assert(sum.GetLayer() == 0_layer);
		static_assert(topsubj.GetLayer() == 0_layer);
	}
	else
	{
		assert(sum.GetLayer() == 0_layer);
		assert(topsubj.GetLayer() == 0_layer);
	}

	auto res = s | Extract(sum, topsubj);

	auto [sum_, topsubj_] = res.GetPlaceholders("fld0"_fld, "fld1"_fld);

	using enum FieldType;

	auto Evaluate = []<FieldType Type, class Trav, class NP>(Number<Type>, const Trav & t, NP & np)
	{
		if constexpr (stat_type_node_or_placeholder<NP>) return np(t);
		else return np(t).template as<Type>();
	};
	static_assert(std::ranges::input_range<decltype(s.GetRange(0))>);
	InitAll(sum, topsubj);
	for (auto [t, r] : views::Zip(s.GetRange(0), res.GetRange(0)))
	{
		EXPECT_EQ(Evaluate(Number<I32>{}, t, sum), Evaluate(Number<I32>{}, r, sum_));
		EXPECT_EQ(Evaluate(Number<I32>{}, t, topsubj), Evaluate(Number<I32>{}, r, topsubj_));
	}
}
TEST_F(Aggregator, STable_Extract)
{
	auto& t = m_stable;
	auto [grade, class_, number, name, exam, math, jpn, eng, sci, soc] =
		t.GetPlaceholders("grade"_fld, "class_"_fld, "number"_fld, "name"_fld,
			"exam"_fld, "math"_fld, "japanese"_fld, "english"_fld, "science"_fld, "social"_fld);
	TestExtract(t, std::make_tuple(grade, class_, number, name, exam, math, jpn, eng, sci, soc));
}
TEST_F(Aggregator, DTable_Extract)
{
	auto& t = m_dtable;
	auto [grade, class_, number, name, exam, math, jpn, eng, sci, soc] =
		t.GetPlaceholders("grade", "class_", "number", "name", "exam", "math", "japanese", "english", "science", "social");
	TestExtract(t, std::make_tuple(grade, class_, number, name, exam, math, jpn, eng, sci, soc));
}
TEST_F(Aggregator, DJoinedTable_Extract)
{
	auto& t = m_dtable;
	auto jt = Join(m_dtable, 0_layer, 0_layer, m_dtable);
	auto [grade, class_, number, name, exam0] = jt.GetPlaceholders<0>("grade", "class_", "number", "name", "exam");
	auto [exam1, math, jpn, eng, sci, soc] = jt.GetPlaceholders<1>("exam", "math", "japanese", "english", "science", "social");
	auto hash = [&t]()
	{
		auto a = t.GetPlaceholder("number").i16();//MakeHashmapはキーの型を特定する必要があるため、
		auto b = t.GetPlaceholder("name").str();//これらのPlaceholderは予め型情報を与えなければならない。
		auto c = t.GetPlaceholder("exam").i08();//これらのPlaceholderは予め型情報を与えなければならない。
		return t | MakeHashtable(a, b, c);
	}();
	jt.SetKeyJoint<1>(std::move(hash), number, name, exam0);

	TestExtract(jt, std::make_tuple(grade, class_, number, name, exam1, math, jpn, eng, sci, soc));
}



TEST_F(Aggregator, TryJoinInEvaluator)
{
	//auto [number, name, dob] = m_dtree.GetPlaceholders("number", "name", "date_of_birth");
	auto dtree_even = [this]()
	{
		//0層要素。学年とクラス。
		auto [grade, class_] = m_dtree.GetPlaceholders("grade", "class_");
		//1層要素。出席番号、名前。
		auto [number, name] = m_dtree.GetPlaceholders("number", "name");
		//2層要素。各試験の点数。前期中間、前期期末、後期中間、後期期末の順に並んでいる。
		auto [exam, math, jpn, eng, sci, soc] = m_dtree.GetPlaceholders("exam", "math", "japanese", "english", "science", "social");

		return m_dtree | Filter(number % 2 == 0) | Extract(grade.named("grade"), class_.named("class_"),
														   number.named("number"), name.named("name"),
														   math.named("math"), jpn.named("jpn"), eng.named("eng"));
	}();
	auto hash = [&dtree_even]()
	{
		auto [number, name] = dtree_even.GetPlaceholders("number", "name");
		return dtree_even | MakeHashtable(number.i16(), name.str());
	}();
	auto jt = Join(m_dtree, 1_layer, 1_layer, dtree_even);
	auto [jgrade, jclass, jnumber, jname] = jt.GetPlaceholders<0>("grade", "class_", "number", "name");
	auto jnumber2 = jt.GetPlaceholder<1>("number");
	jt.SetKeyJoint<1>(std::move(hash), jnumber, jname);
	auto extr = jt | Extract(jnumber, if_(jt.tryjoin(1_rank), jnumber2, (int16_t)-1));

	auto [orgnumber, ifnumber] = extr.GetPlaceholders("fld0", "fld1");
	auto range = extr.GetRange(1);
	EXPECT_EQ(extr.GetSize(0_layer), 4);
	EXPECT_EQ(extr.GetSize(1_layer), 120);
	size_t oddcount = 0;
	size_t evencount = 0;
	for (const auto& trav : range)
	{
		if (trav[orgnumber].i16() % 2 == 0)
		{
			EXPECT_EQ(trav[orgnumber].i16(), trav[ifnumber].i16());
			++evencount;
		}
		else
		{
			EXPECT_EQ(trav[ifnumber].i16(), -1);
			++oddcount;
		}
	}
	EXPECT_EQ(oddcount, 60);
	EXPECT_EQ(evencount, 60);
}