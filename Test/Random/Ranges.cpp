#include <algorithm>
#include <gtest/gtest.h>
#include <OpenADAPT/Utility/Ranges.h>

TEST(Random, Range_RepeatView)
{
	auto rep = adapt::views::Repeat(42, 5);
	size_t count = 0;
	for (auto v : rep)
	{
		++count;
		EXPECT_EQ(v, 42);
	}
	EXPECT_EQ(count, 5);

	std::vector<int> vec = { 1, 2, 3, 4, 5 };
	for (auto [v, r] : adapt::views::Zip(vec, adapt::views::Repeat(10)))
	{
		EXPECT_EQ(r * v, 10 * v);
	}
}