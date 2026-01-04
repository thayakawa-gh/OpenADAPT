#include <Test/Common/Aggregator.h>

using Aggregator_TryJoin = Aggregator_DTree;

TEST_F(Aggregator_TryJoin, TryJoinInEvaluator)
{
	//auto [number, name, dob] = m_dtree.GetPlaceholders("number", "name", "date_of_birth");
	auto dtree_even = [this]()
	{
		DECL_TREE_PH_SET(*m_dtree);
		return *m_dtree | Filter(number % 2 == 0) | ADAPT_EXTRACT(grade, class_, number, name, math, japanese, english);
	}();
	auto hash = [&dtree_even]()
	{
		auto [number, name] = dtree_even.GetPlaceholders("number", "name");
		return dtree_even | Hash(number.i16(), name.str());
	}();
	auto jt = Join(*m_dtree, 1_layer, 1_layer, dtree_even);
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