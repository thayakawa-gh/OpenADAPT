#include <Test/Aggregator.h>

using namespace adapt;
using namespace adapt::lit;

TEST_F(Aggregator, CrossJoint)
{
	{
		auto jt = Join(m_dtree, 2_layer, -1_layer, m_dtree);
		jt.SetCrossJoint<1_rank>();

		auto [math0, english0, name0] = jt.GetPlaceholders<0_rank>("math", "english", "name");
		auto [math1, english1, name1] = jt.GetPlaceholders<1_rank>("math", "english", "name");
		auto e = jt | Filter(english0 - english1 < 20) | Extract(name0 + "-" + name1, math0 - math1);
		ADAPT_GET_PLACEHOLDERS(e, fld0, fld1);
		auto trav = e.GetRange(5_layer).begin();
		for (BindexType i = 0; i < m_class.size(); ++i)
		{
			auto& c0 = m_class[i];
			for (BindexType j = 0; j < c0.m_students.size(); ++j)
			{
				auto& st0 = c0.m_students[j];
				for (BindexType k = 0; k < st0.m_records.size(); ++k)
				{
					auto& r0 = st0.m_records[k];
					for (BindexType l = 0; l < m_class.size(); ++l)
					{
						auto& c1 = m_class[l];
						for (BindexType m = 0; m < c1.m_students.size(); ++m)
						{
							auto& st1 = c1.m_students[m];
							for (BindexType n = 0; n < st1.m_records.size(); ++n)
							{
								auto& r1 = st1.m_records[n];
								if (r0.m_english - r1.m_english >= 20) continue;

								EXPECT_EQ(trav[fld0].str(), st0.m_name + "-" + st1.m_name);
								EXPECT_EQ(trav[fld1].i32(), r0.m_math - r1.m_math);
								++trav;
							}
						}
					}
				}
			}
		}
		EXPECT_TRUE(trav.IsEnd());
	}
	{
		auto jt = Join(m_dtree, 1_layer, -1_layer, m_dtree);
		jt.SetCrossJoint<1_rank>();

		//SetNumOfThreads(1);
		auto [math0, english0, name0] = jt.GetPlaceholders<0_rank>("math", "english", "name");
		auto [math1, english1, name1] = jt.GetPlaceholders<1_rank>("math", "english", "name");
		auto e = jt | Filter((mean(english0) - mean(english1)) < 20) | Extract(name0 + "-" + name1, mean(math0) - mean(math1));
		//auto e = j | Filter((mean(english1)) < 60) | Extract(name0 + "-" + name1, mean(math0) - mean(math1));
		ADAPT_GET_PLACEHOLDERS(e, fld0, fld1);
		auto trav = e.GetRange(3_layer).begin();
		auto get_mean = [](const Student& s, const int8_t Record::* sub)
		{
			int32_t sum = 0;
			for (const auto& r : s.m_records) sum += (int32_t)(r.*sub);
			return int32_t(sum / s.m_records.size());
		};
		for (BindexType i = 0; i < m_class.size(); ++i)
		{
			auto& c0 = m_class[i];
			for (BindexType j = 0; j < c0.m_students.size(); ++j)
			{
				auto& st0 = c0.m_students[j];
				for (BindexType l = 0; l < m_class.size(); ++l)
				{
					auto& c1 = m_class[l];
					for (BindexType m = 0; m < c1.m_students.size(); ++m)
					{
						auto& st1 = c1.m_students[m];
						if (get_mean(st0, &Record::m_english) - get_mean(st1, &Record::m_english) >= 20) continue;
						//if (get_mean(st1, &Record::m_english) >= 60) continue;

						EXPECT_EQ(trav[fld0].str(), st0.m_name + "-" + st1.m_name);
						EXPECT_EQ(trav[fld1].i32(), get_mean(st0, &Record::m_math) - get_mean(st1, &Record::m_math));
						++trav;
					}
				}
			}
		}
		EXPECT_TRUE(trav.IsEnd());
	}
}