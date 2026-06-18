#ifndef ADAPT_OPTIMIZATION_LEASTSQUARES_H
#define ADAPT_OPTIMIZATION_LEASTSQUARES_H

#include <vector>
#include <span>
#include <ranges>
#include <cmath>
#include <limits>
#include <algorithm>
#include <utility>
#include <array>
#include <tuple>
#include <OpenADAPT/Utility/Common.h>
#include <OpenADAPT/Utility/Exception.h>
#include <OpenADAPT/Utility/Ranges.h>
#include <OpenADAPT/Utility/KeywordArgs.h>

namespace adapt
{

ADAPT_EXPORT
struct LeastSquaresOptions
{
	size_t max_iterations = 1000;
	double initial_lambda = 1e-3;
	double lambda_factor = 10.0;
	double tolerance = 1e-12;
	double cost_tolerance = 1e-12;
	double diff_step = 1e-6;
};

ADAPT_EXPORT
struct LeastSquaresResult
{
	std::vector<double> params;
	double cost = 0.0;
	size_t iterations = 0;
	bool converged = false;
};

namespace detail
{

template <class Func, class X>
concept ls_model_function = requires(Func f, const X & x, std::span<const double> params)
{
	{ f(x, params) } -> std::convertible_to<double>;
};

template <class GradFunc, class X>
concept ls_gradient_with_output = requires(GradFunc f, std::span<double> out, const X & x, std::span<const double> params)
{
	{ f(out, x, params) } -> std::same_as<void>;
};

template <class GradFunc, class X>
concept ls_gradient_returns_range = requires(GradFunc f, const X & x, std::span<const double> params)
{
	{ f(x, params) } -> std::ranges::input_range;
};

}

namespace opts
{
struct LeastSquaresKeywords {};
template <std::invocable GradFunc> struct GradFuncOption {};
template <ranges::arithmetic_range Range> struct ArithmeticRange {};

ADAPT_EXPORT ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(ls_weight, AnyTypeKeyword<ArithmeticRange>, LeastSquaresKeywords);
ADAPT_EXPORT ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(ls_grad, AnyTypeKeyword<GradFuncOption>, LeastSquaresKeywords);
ADAPT_EXPORT ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(ls_options, LeastSquaresOptions, LeastSquaresKeywords);

template <class Option>
concept least_squares_option = keyword_arg_tagged_with<Option, LeastSquaresKeywords>;

}

namespace detail
{

inline bool SolveLinearSystem(std::vector<double>& a, std::vector<double>& b, size_t n)
{
	constexpr double eps = std::numeric_limits<double>::epsilon();
	for (size_t i = 0; i < n; ++i)
	{
		size_t pivot = i;
		double max_val = std::abs(a[i * n + i]);
		for (size_t r = i + 1; r < n; ++r)
		{
			double v = std::abs(a[r * n + i]);
			if (v > max_val)
			{
				max_val = v;
				pivot = r;
			}
		}
		if (max_val < eps) return false;

		if (pivot != i)
		{
			for (size_t c = i; c < n; ++c) std::swap(a[i * n + c], a[pivot * n + c]);
			std::swap(b[i], b[pivot]);
		}

		double diag = a[i * n + i];
		for (size_t r = i + 1; r < n; ++r)
		{
			double factor = a[r * n + i] / diag;
			if (factor == 0.0) continue;
			for (size_t c = i; c < n; ++c) a[r * n + c] -= factor * a[i * n + c];
			b[r] -= factor * b[i];
		}
	}

	for (size_t i = n; i-- > 0;)
	{
		double sum = b[i];
		for (size_t c = i + 1; c < n; ++c) sum -= a[i * n + c] * b[c];
		b[i] = sum / a[i * n + i];
	}
	return true;
}

template <class X, class Func>
double EvaluateCost(std::span<const X> xs,
					std::span<const double> weights,
					std::span<const double> params,
					Func&& func)
{
	double cost = 0.0;
	for (size_t i = 0; i < xs.size(); ++i)
	{
		double w = weights.empty() ? 1.0 : weights[i];
		double c = static_cast<double>(std::invoke(func, xs[i], params));
		cost += w * c;
	}
	return cost;
}

template <class X, class Func>
void EvaluateCostGradientNumerical(std::span<double> out,
								   const X& x,
								   std::vector<double>& params,
								   Func&& func,
								   double diff_step)
{
	for (size_t j = 0; j < params.size(); ++j)
	{
		double base = params[j];
		double h = diff_step * std::max(1.0, std::abs(base));
		if (h == 0.0) h = diff_step;

		params[j] = base + h;
		double cp = static_cast<double>(std::invoke(func, x, std::span<const double>(params)));

		params[j] = base - h;
		double cm = static_cast<double>(std::invoke(func, x, std::span<const double>(params)));

		params[j] = base;
		out[j] = (cp - cm) / (2.0 * h);
	}
}

template <class X, class Func, class GradFunc>
void EvaluateCostGradient(std::span<double> out,
						  const X& x,
						  std::vector<double>& params,
						  Func&& func,
						  GradFunc&& grad,
						  double diff_step)
{
	if constexpr (std::is_same_v<std::nullptr_t, std::decay_t<GradFunc>>)
	{
		EvaluateCostGradientNumerical(out, x, params, std::forward<Func>(func), diff_step);
	}
	else if constexpr (ls_gradient_with_output<GradFunc, X>)
	{
		std::invoke(grad, out, x, std::span<const double>(params));
	}
	else if constexpr (ls_gradient_returns_range<GradFunc, X>)
	{
		auto g = std::invoke(grad, x, std::span<const double>(params));
		size_t idx = 0;
		if constexpr (std::ranges::sized_range<decltype(g)>)
		{
			if (std::ranges::size(g) != params.size())
				throw InvalidArg("Gradient size does not match parameter size.");
		}
		for (auto&& v : g)
		{
			if (idx >= out.size()) break;
			out[idx++] = static_cast<double>(v);
		}
		if (idx != out.size())
			throw InvalidArg("Gradient size does not match parameter size.");
	}
	else
	{
		EvaluateCostGradientNumerical(out, x, params, std::forward<Func>(func), diff_step);
	}
}

template <class X, class Func, class GradFunc>
LeastSquaresResult SolveLeastSquares_impl(std::span<const X> xs,
										  std::span<const double> weights,
										  std::span<const double> params,
										  Func&& func,
										  GradFunc&& grad,
										  const LeastSquaresOptions& options)
{
	if (!weights.empty() && weights.size() != xs.size())
		throw InvalidArg("The size of weights is not equal to the size of x.");
	if (params.empty())
		throw InvalidArg("Parameter size must be greater than zero.");
	if (options.diff_step <= 0.0)
		throw InvalidArg("diff_step must be greater than zero.");

	size_t n = xs.size();
	size_t m = params.size();

	LeastSquaresResult result;
	result.params.assign(params.begin(), params.end());
	double lambda = options.initial_lambda;
	double cost = EvaluateCost(xs, weights, result.params, func);

	std::vector<double> grad_cost(m);
	std::vector<double> a(m * m);
	std::vector<double> b(m);
	std::vector<double> delta(m);

	constexpr double eps = std::numeric_limits<double>::epsilon();

	for (size_t iter = 0; iter < options.max_iterations; ++iter)
	{
		std::fill(a.begin(), a.end(), 0.0);
		std::fill(b.begin(), b.end(), 0.0);

		for (size_t i = 0; i < n; ++i)
		{
			double w = weights.empty() ? 1.0 : weights[i];
			double c = static_cast<double>(std::invoke(func, xs[i], std::span<const double>(result.params)));

			EvaluateCostGradient(grad_cost, xs[i], result.params, func, grad, options.diff_step);

			for (size_t j = 0; j < m; ++j)
			{
				b[j] -= 0.5 * w * grad_cost[j];
				for (size_t k = 0; k < m; ++k)
				{
					double denom = 4.0 * std::max(c, eps);
					a[j * m + k] += w * (grad_cost[j] * grad_cost[k]) / denom;
				}
			}
		}

		for (size_t j = 0; j < m; ++j) a[j * m + j] += lambda;

		delta = b;
		auto a_copy = a;
		if (!SolveLinearSystem(a_copy, delta, m))
		{
			lambda *= options.lambda_factor;
			continue;
		}

		std::vector<double> trial = result.params;
		for (size_t j = 0; j < m; ++j) trial[j] += delta[j];

		double trial_cost = EvaluateCost(xs, weights, trial, func);
		double cost_diff = std::abs(trial_cost - cost);

		if (trial_cost < cost)
		{
			result.params = std::move(trial);
			cost = trial_cost;
			lambda /= options.lambda_factor;

			double delta_norm2 = 0.0;
			for (double v : delta) delta_norm2 += v * v;

			if (delta_norm2 < options.tolerance * options.tolerance ||
				cost_diff < options.cost_tolerance)
			{
				result.converged = true;
				result.iterations = iter + 1;
				result.cost = cost;
				return result;
			}
		}
		else
		{
			lambda *= options.lambda_factor;
		}

		result.iterations = iter + 1;
	}

	result.cost = cost;
	return result;
}

template <ranges::arithmetic_range... Ranges>
auto BuildPointSpans(Ranges&&... ranges)
{
	static_assert(sizeof...(Ranges) > 0);

	auto ranges_tuple = std::forward_as_tuple(ranges...);
	size_t size = std::ranges::size(std::get<0>(ranges_tuple));
	if (((std::ranges::size(ranges) != size) || ...))
		throw InvalidArg("The sizes of x ranges are not equal.");

	constexpr size_t dim = sizeof...(Ranges);
	std::vector<std::array<double, dim>> storage;
	storage.reserve(size);

	for (auto&& values : views::Zip(ranges...))
	{
		std::array<double, dim> point{};
		size_t idx = 0;
		std::apply([&](auto&& ...v)
		{
			((point[idx++] = static_cast<double>(v)), ...);
		}, values);
		storage.push_back(point);
	}

	std::vector<std::span<const double>> spans;
	spans.reserve(storage.size());
	for (const auto& p : storage) spans.emplace_back(p.data(), p.size());

	return std::pair{ std::move(storage), std::move(spans) };
}

ADAPT_EXPORT
template <detail::ls_model_function<std::span<const double>> Func,
	ranges::arithmetic_range ...Data,
	opts::least_squares_option ...Options>
LeastSquaresResult SolveLeastSquares_rec(Func&& func,
										 std::span<const double> params,
										 std::tuple<Data...> xs,
										 std::tuple<Options...> options)
{
	auto apply_build_spans = [&](auto&& ...xs) { return detail::BuildPointSpans(std::forward<decltype(xs)>(xs)...); };
	auto get_weights = [&](auto ...options)
	{
		if constexpr (KeywordExists(opts::ls_weight, options...))
		{
			const auto& w_ = GetKeywordArg(opts::ls_weight, options...);
			if constexpr (std::convertible_to<decltype(w_), std::span<const double>>)
			{
				return MakeTemporaryTuple(w_, std::span<const double>(w_));
			}
			else
			{
				std::vector<double> w_vec;
				std::ranges::copy(w_, std::back_inserter(w_vec));
				return MakeTemporaryTuple(w_vec, std::span<const double>(w_vec));
			}
		}
		else
		{
			return MakeTemporaryTuple(EmptyClass{}, std::span<const double>{});
		}
	};
	auto get_grad = [&](auto ...options)
	{
		if constexpr (KeywordExists(opts::ls_grad, options...))
			return GetKeywordArg(opts::ls_grad, options...);
		else
			return nullptr;
	};

	auto [data_storage, data_spans] = std::apply(apply_build_spans, std::move(xs));
	auto [weights_storage, weights_span] = std::apply(get_weights, options);
	LeastSquaresOptions o{};
	std::apply([&](auto ...opts)
	{
		if constexpr (KeywordExists(opts::ls_options, opts...))
			o = GetKeywordArg(opts::ls_options, opts...);
	}, options);
	auto&& grad = std::apply(get_grad, options);

	return detail::SolveLeastSquares_impl(std::span<const std::span<const double>>(data_spans),
										  weights_span, std::move(params),
										  std::forward<Func>(func), std::forward<decltype(grad)>(grad), o);
}

ADAPT_EXPORT
template <detail::ls_model_function<std::span<const double>> Func,
	ranges::arithmetic_range ...Data,
	opts::least_squares_option ...Options,
	opts::least_squares_option Opt, class ...Args>
LeastSquaresResult SolveLeastSquares_rec(Func&& func,
										 std::span<const double> params,
										 std::tuple<Data...> xs,
										 std::tuple<Options...> options,
										 Opt&& o, Args&& ...args)
{
	auto options_cat = TupleAdd(options, o);
	return SolveLeastSquares_rec(std::forward<Func>(func), std::move(params), std::move(xs), std::move(options_cat), std::forward<Args>(args)...);
}
ADAPT_EXPORT
template <detail::ls_model_function<std::span<const double>> Func,
		  ranges::arithmetic_range ...Data,
		  opts::least_squares_option ...Options,
		  ranges::arithmetic_range Var, class ...Args>
LeastSquaresResult SolveLeastSquares_rec(Func&& func,
										 std::span<const double> params,
										 std::tuple<Data...> xs,
										 std::tuple<Options...> options,
										 Var&& v, Args&& ...args)
{
	auto xs_cat = TupleAdd(xs, std::forward<Var>(v));
	return SolveLeastSquares_rec(std::forward<Func>(func), std::move(params), std::move(xs_cat), std::move(options), std::forward<Args>(args)...);
}

}

ADAPT_EXPORT
template <detail::ls_model_function<std::span<const double>> Func, class ...Args>
LeastSquaresResult SolveLeastSquares(Func&& func,
									 std::span<const double> params,
									 Args&& ...args)
{
	return detail::SolveLeastSquares_rec(
		std::forward<Func>(func), std::move(params),
		std::tuple<>{}, std::tuple<>{},
		std::forward<Args>(args)...);
}

}

#endif