#include <Test/Common/Aggregator.h>

using namespace adapt;
using namespace adapt::lit;

template <FieldType Type, class Trav, class NP>
auto Evaluate(Number<Type>, const Trav& t, NP& np)
{
	if constexpr (stat_type_node_or_placeholder<NP>) return np(t);
	else return np(t).template as<Type>();
}
template <any_container Container, class Layer0, class Layer1, class Layer2>
void TestSimpleExtract(Container& s, const std::vector<Class>&, Layer0 l0, Layer1 l1, Layer2 l2)
{
	SetNumOfThreads(16);
	using enum adapt::FieldType;
	//test 1
		//0層要素。学年とクラス。
	auto [grade, class_] = l0;
	//1層要素。出席番号、名前、生年月日。
	auto [number, name] = l1;
	//2層要素。各試験の点数。前期中間、前期期末、後期中間、後期期末の順に並んでいる。
	auto [exam, math, jpn, eng, sci, soc] = l2;

	auto count_350 = count_if(exist(math + jpn + eng + sci + soc >= 350));
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
template <any_container Container, class Layer0, class Layer1, class Layer2>
void TestFilterComfirmation(Container& s, const std::vector<Class>& clses, Layer0, Layer1, Layer2 l2)
{
	SetNumOfThreads(16);
	using enum adapt::FieldType;
	//filter
	//Extractのテストではない。Filter(sum >= 300)が正常に機能しているかどうかを確認するためのもの。
	//TestFilteredExtractで誤りがあったとき、それがFilter単体で起こりうるのか、
	//Extractと併用して初めて起こる問題であるかを識別する。
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

	for (BindexType i = 0; i < (BindexType)clses.size(); ++i)
	{
		auto& c = clses[i];
		for (BindexType j = 0; j < (BindexType)c.m_students.size(); ++j)
		{
			auto& st = c.m_students[j];
			for (BindexType k = 0; k < (BindexType)st.m_records.size(); ++k)
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
template <any_container Container, class Layer0, class Layer1, class Layer2>
void TestFilteredExtract(Container& s, const std::vector<Class>&, Layer0, Layer1, Layer2 l2)
{
	SetNumOfThreads(16);
	using enum adapt::FieldType;
	//test 2
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
template <any_container Container, class Layer0, class Layer1, class Layer2>
void TestExtractBugcheck(Container& s, const std::vector<Class>&, Layer0 l0, Layer1, Layer2 l2)
{
	SetNumOfThreads(16);
	using enum adapt::FieldType;
	//filter3
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
template <container_simplex Container, class Layer0, class Layer1, class Layer2>
void TestExtractAllFields(Container& s, const std::vector<Class>&, Layer0 l0, Layer1 l1, Layer2 l2)
{
	SetNumOfThreads(16);
	using enum adapt::FieldType;
	//all_fields
		//0層要素。学年とクラス。
	auto [grade, class_] = l0;
	//1層要素。出席番号、名前、生年月日。
	auto [number, name] = l1;
	//2層要素。各試験の点数。前期中間、前期期末、後期中間、後期期末の順に並んでいる。
	auto [exam, math, jpn, eng, sci, soc] = l2;

	auto e = s | Extract(opts::all_fields);
	auto [egrade, eclass_] = e.GetPlaceholders("grade"_fld, "class_"_fld);
	auto [enumber, ename] = e.GetPlaceholders("number"_fld, "name"_fld);
	auto [eexam, emath, ejpn, eeng, esci, esoc] = e.GetPlaceholders("exam"_fld, "math"_fld, "japanese"_fld, "english"_fld, "science"_fld, "social"_fld);

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
	}
}
template <container_simplex Container, class Layer0, class Layer1, class Layer2>
void TestExtractAllFieldsPlusAlpha(Container& s, const std::vector<Class>&, Layer0 l0, Layer1 l1, Layer2 l2)
{
	SetNumOfThreads(16);
	using enum adapt::FieldType;
	//all_fields + something
		//0層要素。学年とクラス。
	auto [grade, class_] = l0;
	//1層要素。出席番号、名前、生年月日。
	auto [number, name] = l1;
	//2層要素。各試験の点数。前期中間、前期期末、後期中間、後期期末の順に並んでいる。
	auto [exam, math, jpn, eng, sci, soc] = l2;

	auto e = s | Extract(opts::all_fields, math + jpn + eng + sci + soc);
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
template <container_simplex Container, class Layer0, class Layer1, class Layer2>
void TestFilteredExtractAllFields(Container& s, const std::vector<Class>&, Layer0 l0, Layer1 l1, Layer2 l2)
{
	SetNumOfThreads(16);
	using enum adapt::FieldType;
	//all_fields + filter
		//0層要素。学年とクラス。
	auto [grade, class_] = l0;
	//1層要素。出席番号、名前、生年月日。
	auto [number, name] = l1;
	//2層要素。各試験の点数。前期中間、前期期末、後期中間、後期期末の順に並んでいる。
	auto [exam, math, jpn, eng, sci, soc] = l2;

	auto e = s | Filter(soc < 60) | Extract(opts::all_fields, math + jpn + eng + sci + soc);
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




template <class Container, class Layer>
void TestExtract(Container& s, Layer l)
{
	auto [class_, number, name, exam, math, jpn, eng, sci, soc] = l;

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

