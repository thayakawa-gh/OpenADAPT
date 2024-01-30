#include <Test/Aggregator.h>
#include <ranges>
#include <iterator>

using namespace adapt;
using namespace adapt::lit;

template <class Container, class Layer0, class Layer1, class Layer2>
void TestContainer(Container& tree, const std::vector<Class>& cls,
				 const Layer0& l0, const Layer1& l1, const Layer2& l2)
{
	//0層要素。学年とクラス。
	auto [grade, class_] = l0;
	//1層要素。出席番号、名前、生年月日。
	auto [number, name, dob] = l1;
	//2層要素。各試験の点数。前期中間、前期期末、後期中間、後期期末の順に並んでいる。
	auto [exam, math, jpn, eng, sci, soc] = l2;

	EXPECT_EQ(tree.GetSize(0_layer), 4);
	EXPECT_EQ(tree.GetSize(1_layer), 120);
	EXPECT_EQ(tree.GetSize(2_layer), 480);

	size_t clssize = cls.size();
	for (BindexType i = 0; i < clssize; ++i)
	{
		const auto& tc = tree[i];
		const Class& c = cls[i];

		EXPECT_EQ(tc.GetSize(1_layer), 30);
		EXPECT_EQ(tc.GetSize(2_layer), 120);

		if constexpr (d_container<Container>)
		{
			EXPECT_EQ(tc[grade].i08(), c.m_grade);
			EXPECT_EQ(tc[class_].i08(), c.m_class);
		}
		else
		{
			EXPECT_EQ(tc[grade], c.m_grade);
			EXPECT_EQ(tc[class_], c.m_class);
		}

		size_t stusize = c.m_students.size();
		for (BindexType j = 0; j < stusize; ++j)
		{
			const auto& ts = tc[j];
			const Student& s = c.m_students[j];

			EXPECT_EQ(ts.GetSize(2_layer), 4);

			if constexpr (d_container<Container>)
			{
				EXPECT_EQ(ts[number].i16(), s.m_number);
				EXPECT_EQ(ts[name].str(), s.m_name);
				EXPECT_EQ(ts[dob].i32(), s.m_date_of_birth);
			}
			else
			{
				EXPECT_EQ(ts[number], s.m_number);
				EXPECT_EQ(ts[name], s.m_name);
				EXPECT_EQ(ts[dob], s.m_date_of_birth);
			}

			size_t recsize = s.m_records.size();
			for (BindexType k = 0; k < recsize; ++k)
			{
				const auto& tr = ts[k];
				const Record& r = s.m_records[k];

				if constexpr (d_container<Container>)
				{
					EXPECT_EQ(tr[exam].i08(), r.m_exam);
					EXPECT_EQ(tr[math].i32(), r.m_math);
					EXPECT_EQ(tr[jpn].i32(), r.m_japanese);
					EXPECT_EQ(tr[eng].i32(), r.m_english);
					EXPECT_EQ(tr[sci].i32(), r.m_science);
					EXPECT_EQ(tr[soc].i32(), r.m_social);
				}
				else
				{
					EXPECT_EQ(tr[exam], r.m_exam);
					EXPECT_EQ(tr[math], r.m_math);
					EXPECT_EQ(tr[jpn], r.m_japanese);
					EXPECT_EQ(tr[eng], r.m_english);
					EXPECT_EQ(tr[sci], r.m_science);
					EXPECT_EQ(tr[soc], r.m_social);
				}
			}
		}
	}
}

TEST_F(Aggregator, DTree_Container)
{
	//0層要素。学年とクラス。
	auto [grade, class_] = m_dtree.GetPlaceholders("grade", "class");
	//1層要素。出席番号、名前、生年月日。
	auto [number, name, dob] = m_dtree.GetPlaceholders("number", "name", "date_of_birth");
	//2層要素。各試験の点数。前期中間、前期期末、後期中間、後期期末の順に並んでいる。
	auto [exam, math, jpn, eng, sci, soc] = m_dtree.GetPlaceholders("exam", "math", "japanese", "english", "science", "social");

	TestContainer(m_dtree, m_class,
				 std::make_tuple(grade, class_),
				 std::make_tuple(number, name, dob),
				 std::make_tuple(exam, math, jpn, eng, sci, soc));
}
TEST_F(Aggregator, STree_Container)
{
	//0層要素。学年とクラス。
	auto [grade, class_] = m_stree.GetPlaceholders<"grade", "class">();
	//1層要素。出席番号、名前、生年月日。
	auto [number, name, dob] = m_stree.GetPlaceholders<"number", "name", "date_of_birth">();
	//2層要素。各試験の点数。前期中間、前期期末、後期中間、後期期末の順に並んでいる。
	auto [exam, math, jpn, eng, sci, soc] = m_stree.GetPlaceholders<"exam", "math", "japanese", "english", "science", "social">();

	TestContainer(m_stree, m_class,
				std::make_tuple(grade, class_),
				std::make_tuple(number, name, dob),
				std::make_tuple(exam, math, jpn, eng, sci, soc));
}
