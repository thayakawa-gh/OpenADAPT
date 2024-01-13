#include <Test/Aggregator.h>

using namespace adapt;
using namespace adapt::lit;

template <class Container, class Layer0, class Layer1, class Layer2>
void TestExtract(Container& s, Layer0 l0, Layer1 l1, Layer2 l2)
{
	//0層要素。学年とクラス。
	auto [grade, class_] = l0;
	//1層要素。出席番号、名前、生年月日。
	auto [number, name, dob] = l1;
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

	using enum FieldType;

	auto Evaluate = []<FieldType Type, class Trav, class NP>(Number<Type>, const Trav& t, NP& np)
	{
		if constexpr (typed_node_or_placeholder<NP>) return np(t);
		else return np(t).template as<Type>();
	};
	static_assert(std::ranges::input_range<decltype(s.GetRange(2))>);
	InitAll(count_350, mean_sum, sum, isbest);
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

TEST_F(Aggregator, DTree_Extract)
{
	//0層要素。学年とクラス。
	auto [grade, class_] = m_dtree.GetPlaceholders("grade", "class");
	//1層要素。出席番号、名前、生年月日。
	auto [number, name, dob] = m_dtree.GetPlaceholders("number", "name", "date_of_birth");
	//2層要素。各試験の点数。前期中間、前期期末、後期中間、後期期末の順に並んでいる。
	auto [exam, math, jpn, eng, sci, soc] = m_dtree.GetPlaceholders("exam", "math", "japanese", "english", "science", "social");

	TestExtract(m_dtree,
				 std::make_tuple(grade, class_),
				 std::make_tuple(number, name, dob),
				 std::make_tuple(exam, math, jpn, eng, sci, soc));
}

TEST_F(Aggregator, STree_Extract)
{
	//0層要素。学年とクラス。
	auto [grade, class_] = m_stree.GetPlaceholders<"grade", "class">();
	//1層要素。出席番号、名前、生年月日。
	auto [number, name, dob] = m_stree.GetPlaceholders<"number", "name", "date_of_birth">();
	//2層要素。各試験の点数。前期中間、前期期末、後期中間、後期期末の順に並んでいる。
	auto [exam, math, jpn, eng, sci, soc] = m_stree.GetPlaceholders<"exam", "math", "japanese", "english", "science", "social">();
	static_assert(s_named_node_or_placeholder<decltype(name.named(""_fld))>);
	TestExtract(m_stree,
				std::make_tuple(grade, class_),
				std::make_tuple(number, name, dob),
				std::make_tuple(exam, math, jpn, eng, sci, soc));
}

TEST_F(Aggregator, DJoinedContainer_Extract_1)
{
	auto a = m_dtree.GetPlaceholder("number").i16();//MakeHashmapはキーの型を特定する必要があるため、
	auto b = m_dtree.GetPlaceholder("name").str();//これらのPlaceholderは予め型情報を与えなければならない。
	auto hash = m_dtree | MakeHashtable(a, b);

	auto jtree = Join(m_dtree, 1_layer, 1_layer, m_dtree);
	//0層要素。学年とクラス。
	auto [grade, class_] = jtree.GetPlaceholders<0>("grade"_fld, "class"_fld);
	//1層要素。出席番号、名前、生年月日。
	auto [nu, na] = jtree.GetPlaceholders<0>("number"_fld, "name"_fld);
	auto [number, name, dob] = jtree.GetPlaceholders<1>("number"_fld, "name"_fld, "date_of_birth"_fld);
	//2層要素。各試験の点数。前期中間、前期期末、後期中間、後期期末の順に並んでいる。
	auto [exam, math, jpn, eng, sci, soc] = jtree.GetPlaceholders<1>("exam"_fld, "math"_fld, "japanese"_fld, "english"_fld, "science"_fld, "social"_fld);

	jtree.SetKeyJoint<1>(std::move(hash), nu, na);

	TestExtract(jtree,
				std::make_tuple(grade, class_),
				std::make_tuple(number, name, dob),
				std::make_tuple(exam, math, jpn, eng, sci, soc));
}
