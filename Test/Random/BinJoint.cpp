#include <Test/Common/Aggregator.h>

using namespace adapt;
using namespace adapt::lit;

using Aggregator_BinJoin = Aggregator_DTree;

struct SameScore
{
	bool operator<(const SameScore& that) const
	{
		if (class_1 != that.class_1) return class_1 < that.class_1;
		if (number1 != that.number1) return number1 < that.number1;
		if (name1 != that.name1) return name1 < that.name1;
		if (class_2 != that.class_2) return class_2 < that.class_2;
		if (number2 != that.number2) return number2 < that.number2;
		return name2 < that.name2;
	}
	bool operator==(const SameScore& that) const
	{
		return class_1 == that.class_1 &&
			number1 == that.number1 &&
			name1 == that.name1 &&
			class_2 == that.class_2 &&
			number2 == that.number2 &&
			name2 == that.name2;
	}
	int8_t class_1;
	int16_t number1;
	std::string name1;
	int8_t class_2;
	int16_t number2;
	std::string name2;
};

TEST_F(Aggregator_BinJoin, BinJoin)
{
	std::set<SameScore> list_bj, list_raw;
	size_t pair_count = 0;

	ADAPT_GET_PLACEHOLDERS(*m_tree, class_, number, name, exam, math, english);
	auto tree = *m_tree | Filter(exam == 3) | ADAPT_EXTRACT(class_, number, name, math);
	auto hist = *m_tree | Filter(exam == 3) | ADAPT_HIST(cast_f64(math).named("math"), 5., class_, number, name);
	{
		// BinJointからリスト化
		auto jt = Join(tree, 2_layer, 0_layer, hist);
		auto [class_r0, number_r0, name_r0, math_r0] = jt.GetPlaceholders<0>("class_", "number", "name", "math");
		auto [class_r1, number_r1, name_r1, math_r1] = jt.GetPlaceholders<1>("class_", "number", "name", "math");
		jt.SetBinJoint<1_rank>(cast_i32(math_r0 / 5.));
		auto v_bj = jt | Filter(math_r0 == math_r1, !(name_r0 == name_r1 && number_r0 == number_r1)) |
			ToVector(opts::combine, class_r0.i08(), number_r0.i16(), name_r0.str(), math_r0.i32(), class_r1.i08(), number_r1.i16(), name_r1.str(), cast_i32(math_r1).i32());
		pair_count = v_bj.size();
		for (const auto& [c1, n1, na1, m1, c2, n2, na2, m2] : v_bj)
		{
			SameScore ss;
			ss.class_1 = c1;
			ss.number1 = n1;
			ss.name1 = na1;
			ss.class_2 = c2;
			ss.number2 = n2;
			ss.name2 = na2;
			[[maybe_unused]] auto [it, inserted] = list_bj.insert(ss);
			EXPECT_TRUE(inserted);
		}
	}
	{
		// m_classからリスト化
		for (const auto& c1 : m_class)
		{
			for (const auto& s1 : c1.m_students)
			{
				const auto& r1 = s1.m_records[3];
				for (const auto& c2 : m_class)
				{
					for (const auto& s2 : c2.m_students)
					{
						const auto& r2 = s2.m_records[3];
						// ここで同じ生徒は除外
						if (s1.m_name == s2.m_name && s1.m_number == s2.m_number) continue;
						// mathが同じでなければ除外
						if (r1.m_math != r2.m_math) continue;
						SameScore ss;
						ss.class_1 = c1.m_class;
						ss.number1 = s1.m_number;
						ss.name1 = s1.m_name;
						ss.class_2 = c2.m_class;
						ss.number2 = s2.m_number;
						ss.name2 = s2.m_name;
						[[maybe_unused]] auto [it, inserted] = list_raw.insert(ss);
						EXPECT_TRUE(inserted);
					}
				}
			}
		}
	}
	EXPECT_EQ(list_bj.size(), pair_count);
	EXPECT_EQ(list_raw.size(), pair_count);
	EXPECT_TRUE(std::equal(list_bj.begin(), list_bj.end(), list_raw.begin()));
}