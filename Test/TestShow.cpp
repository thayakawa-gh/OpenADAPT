#include <Test/Aggregator.h>

TEST_F(Aggregator, Show)
{
	auto [class_, number, name, exam, jpn, math, eng] = m_dtree.GetPlaceholders("class_", "number", "name", "exam", "japanese", "math", "english");

	auto sum_3subjs = jpn + math + eng;
	auto rank = count3(exam == 0 && sum_3subjs > jpn.o(0) + math.o(0) + eng.o(0)) + 1;

	m_dtree | Filter(sum_3subjs >= 240 && exam == 0) | Show(name, sum_3subjs, jpn, math, eng, rank);
}
