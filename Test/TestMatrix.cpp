#include <gtest/gtest.h>
#include <OpenADAPT/Utility/Matrix.h>
#include <OpenADAPT/Plot/Canvas.h>

TEST(Utility, Matrix)
{
	adapt::Matrix<double, 3> m(0., 3, 5, 7);
	adapt::Matrix<double, 2> m2(0., 3, 5);
	//static_assert(adapt::plot_detail::acceptable_matrix_range<const adapt::Matrix<double, 2>&>);
	static_assert(std::input_or_output_iterator<decltype(static_cast<const adapt::Matrix<double, 2>&>(m2).begin())>);
	//std::ranges::begin(m);
	//std::ranges::end(m);
	for (uint32_t x = 0; x < 3; ++x)
	{
		for (uint32_t y = 0; y < 5; ++y)
		{
			for (uint32_t z = 0; z < 7; ++z)
			{
				m[x][y][z] = x * 100 +  y * 10 + z;
			}
		}
	}

	for (uint32_t x = 0; x < 3; ++x)
	{
		for (uint32_t y = 0; y < 5; ++y)
		{
			for (uint32_t z = 0; z < 7; ++z)
			{
				EXPECT_EQ(m[x][y][z], x * 100 + y * 10 + z);
			}
		}
	}
}