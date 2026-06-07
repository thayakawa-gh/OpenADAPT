#include <algorithm>
#include <random>
#include <numbers>
#include <gtest/gtest.h>
#include <OpenADAPT/Optimization/LeastSquares.h>
#include <OpenADAPT/Optimization/FindMin.h>

using namespace adapt;

TEST(Random, Optimization_LeastSquares)
{
	{
		LeastSquaresOptions options;
		options.max_iterations = 1000;
		options.initial_lambda = 1e-3;
		options.lambda_factor = 10.0;
		options.tolerance = 1e-12;
		options.cost_tolerance = 1e-12;
		options.diff_step = 1e-6;
		auto func = [](std::span<const double> d, std::span<const double> params)
		{
			double a = params[0];
			double b = params[1];
			double c = params[2];
			double x = d[0];
			double y = d[1];
			double residual = a * x * x + b * x + c - y;
			return residual * residual;
		};
		std::vector<double> data_x = { -2.0, -1.0, 0.0, 1.0, 2.0 };
		std::vector<double> data_y = { 4.5, 1.5, 0.5, 1.5, 4.5 };
		/*auto grad_func = [&func, &data_x, &data_y](std::span<double> out, const std::vector<double>& x, std::span<const double> params)
		{
			double a = params[0];
			double b = params[1];
			double c = params[2];
			double y_pred = func(x, params);
			double error = y_pred - data_y[&x - &data_x[0]];
			out[0] = error * x[0] * x[0]; // dCost/da
			out[1] = error * x[0];         // dCost/db
			out[2] = error;                // dCost/dc
		};*/
		std::vector<double> initial_params = { 1.0, 1.0, 1.0 };
		auto result = SolveLeastSquares(func, initial_params, data_x, data_y, opts::ls_options = options);
		EXPECT_TRUE(result.converged);
		EXPECT_NEAR(result.params[0], 1.0, 1e-3); // a
		EXPECT_NEAR(result.params[1], 0.0, 1e-3); // b
		EXPECT_NEAR(result.params[2], 0.5, 1e-3); // c
	}
	{
		LeastSquaresOptions options;
		std::uniform_real_distribution<double> unif_dist(-10.0, 10.0);
		std::normal_distribution<double> gauss_dist(0.0, 1.0);
		std::mt19937 rng(42);
		std::vector<double> data_x(500);
		std::vector<double> data_y(500);
		for (size_t i = 0; i < data_x.size(); ++i)
		{
			double x = unif_dist(rng);
			data_x[i] = x;
			data_y[i] = 2.0 * x * x - 3.0 * x + 1.0 + gauss_dist(rng);
		}
		auto func = [](std::span<const double> d, std::span<const double> params)
		{
			double a = params[0];
			double b = params[1];
			double c = params[2];
			double x = d[0];
			double y = d[1];
			double residual = a * x * x + b * x + c - y;
			return residual * residual;
		};
		std::vector<double> initial_params = { -1.0, -1.0, -1.0 };
		auto result = SolveLeastSquares(func, initial_params, data_x, data_y, opts::ls_options = options);
		EXPECT_TRUE(result.converged);
		EXPECT_NEAR(result.params[0], 2.0, 1e-1); // a
		EXPECT_NEAR(result.params[1], -3.0, 1e-1); // b
		EXPECT_NEAR(result.params[2], 1.0, 1e-1); // c
	}
}

TEST(Random, Optimization_FindMin)
{
	{
		auto func = [](std::span<const double> x_)
		{
			double x = x_[0];
			return x * x + 2 * x + 3;
		};
		std::vector<double> initial_params = { 0.0 };
		auto result = FindMin(func, initial_params);
		EXPECT_NEAR(result.params[0], -1., 1e-3);
	}
	{
		auto func = [](std::span<const double> x_)
		{
			double x = x_[0];
			double s = std::sin(x);
			return s * s;
		};
		std::vector<double> initial_params = { 1.0 };
		auto result = FindMin(func, initial_params);
		EXPECT_NEAR(result.params[0], 0., 1e-3);
		initial_params = { 3.0 };
		result = FindMin(func, initial_params);
		EXPECT_NEAR(result.params[0], std::numbers::pi, 1e-3);
	}
}