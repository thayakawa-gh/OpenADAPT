#include <Test/Aggregator.h>

using namespace adapt;
using namespace adapt::lit;


template <class Container, class Layer0, class Layer1, class Layer2, bool Disabled>
void TestFilter(Container& tree, const std::vector<Class>& clses,
				const Layer0&, const Layer1& l1, const Layer2& l2, std::bool_constant<Disabled> = std::false_type{})
{
	auto [number, name] = l1;
	auto [math, jpn, eng, sci, soc] = l2;

	auto mean_math = mean(cast_f64(math));

	auto range = tree | Filter(math < 60) | GetRange(2_layer);
	auto trav = range.begin();

	InitAll(trav, mean_math);

	using enum adapt::FieldType;
	auto Evaluate = []<FieldType Type, class Trav, class NP>(Number<Type>, const Trav & t, NP & np)
	{
		if constexpr (stat_type_node_or_placeholder<NP>) return np.Evaluate(t);
		else return np.Evaluate(t).template as<Type>();
	};
	for (BindexType i = 0; i < clses.size(); ++i)
	{
		auto& c = clses[i];
		for (BindexType j = 0; j < c.m_students.size(); ++j)
		{
			auto& s = c.m_students[j];
			for (BindexType k = 0; k < s.m_records.size(); ++k)
			{
				auto& r = s.m_records[k];
				if (r.m_math >= 60) continue;

				EXPECT_EQ(i, trav.GetPos(0));
				EXPECT_EQ(j, trav.GetPos(1));
				EXPECT_EQ(k, trav.GetPos(2));

				EXPECT_EQ(Evaluate(Number<Str>{}, trav, name), s.m_name);
				EXPECT_EQ(Evaluate(Number<I32>{}, trav, math), r.m_math);

				//filterは階層関数内部の走査には効力を持たない。
				EXPECT_EQ(Evaluate(Number<F64>{}, trav, mean_math), AvgMath(s));

				++trav;
			}
		}
	}

	assert((mean_math >= 60).GetLayer() == 1);
	auto range2 = tree | Filter(mean_math >= 60, eng >= 60) | GetRange(2_layer);
	auto trav2 = range2.begin();
	InitAll(trav2, mean_math);
	for (BindexType i = 0; i < clses.size(); ++i)
	{
		auto& c = clses[i];
		for (BindexType j = 0; j < c.m_students.size(); ++j)
		{
			auto& s = c.m_students[j];
			if (AvgMath(s) < 60) continue;
			for (BindexType k = 0; k < s.m_records.size(); ++k)
			{
				auto& r = s.m_records[k];
				if (r.m_english < 60) continue;

				EXPECT_EQ(i, trav2.GetPos(0));
				EXPECT_EQ(j, trav2.GetPos(1));
				EXPECT_EQ(k, trav2.GetPos(2));

				EXPECT_EQ(Evaluate(Number<Str>{}, trav2, name), s.m_name);
				EXPECT_EQ(Evaluate(Number<I32>{}, trav2, math), r.m_math);

				//filterは階層関数内部の走査には効力を持たない。
				EXPECT_EQ(Evaluate(Number<F64>{}, trav2, mean_math), AvgMath(s));

				++trav2;
			}
		}
	}
}

TEST_F(Aggregator, DTree_Filter)
{
	auto [number, name] = m_dtree.GetPlaceholders("number", "name");
	auto [math, eng, jpn, sci, soc] = m_dtree.GetPlaceholders("math", "japanese", "english", "science", "social");

	TestFilter(m_dtree, m_class,
		std::make_tuple(),
		std::make_tuple(number, name),
		std::make_tuple(math, eng, jpn, sci, soc),
		std::true_type{});
}
TEST_F(Aggregator, STree_Filter)
{
	auto [number, name] = m_stree.GetPlaceholders<"number", "name">();
	auto [math, eng, jpn, sci, soc] = m_stree.GetPlaceholders<"math", "japanese", "english", "science", "social">();

	TestFilter(m_stree, m_class,
		std::make_tuple(),
		std::make_tuple(number, name),
		std::make_tuple(math, eng, jpn, sci, soc),
		std::true_type{});
}

template <class Container, class Layer>
void TestFilter(Container& table, const std::vector<Class>& clses, const Layer& l)
{
	auto [number, name, exam, math, jpn, eng, sci, soc] = l;

	auto exam_1 = exam == 1;
	auto sum5subjs = math + jpn + eng + sci + soc;

	auto range = table | Filter(exam_1, sum5subjs >= 400) | GetRange(0_layer);
	auto trav = range.begin();

	InitAll(trav, exam_1, sum5subjs);

	using enum adapt::FieldType;
	auto Evaluate = []<FieldType Type, class Trav, class NP>(Number<Type>, const Trav & t, NP & np)
	{
		if constexpr (stat_type_node_or_placeholder<NP>) return np.Evaluate(t);
		else return np.Evaluate(t).template as<Type>();
	};
	for (BindexType i = 0; i < clses.size(); ++i)
	{
		auto& c = clses[i];
		for (BindexType j = 0; j < c.m_students.size(); ++j)
		{
			auto& s = c.m_students[j];
			for (BindexType k = 0; k < s.m_records.size(); ++k)
			{
				auto& r = s.m_records[k];
				if (r.m_exam != 1) continue;
				if (Sum5Subjs(r) < 400) continue;

				EXPECT_EQ(Evaluate(Number<Str>{}, trav, name), s.m_name);

				//filterは階層関数内部の走査には効力を持たない。
				EXPECT_EQ(Evaluate(Number<I32>{}, trav, sum5subjs), Sum5Subjs(r));

				++trav;
			}
		}
	}
}

TEST_F(Aggregator, STable_Filter)
{
	auto [number, name, exam, math, eng, jpn, sci, soc] =
		m_stable.GetPlaceholders("number"_fld, "name"_fld, "exam"_fld, "math"_fld, "japanese"_fld, "english"_fld, "science"_fld, "social"_fld);

	TestFilter(m_stable, m_class,
		std::make_tuple(number, name, exam, math, eng, jpn, sci, soc));
}
TEST_F(Aggregator, DTable_Filter)
{
	auto [number, name, exam, math, eng, jpn, sci, soc] =
		m_dtable.GetPlaceholders("number", "name", "exam", "math", "japanese", "english", "science", "social");

	TestFilter(m_dtable, m_class,
		std::make_tuple(number, name, exam, math, eng, jpn, sci, soc));
}