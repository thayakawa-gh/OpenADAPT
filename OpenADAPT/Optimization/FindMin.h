#ifndef ADAPT_OPTIMIZATION_FINDMIN_H
#define ADAPT_OPTIMIZATION_FINDMIN_H

#include <vector>
#include <span>
#include <cmath>
#include <limits>
#include <algorithm>
#include <utility>
#include <OpenADAPT/Utility/Common.h>
#include <OpenADAPT/Utility/Exception.h>
#include <OpenADAPT/Utility/KeywordArgs.h>

namespace adapt
{

ADAPT_EXPORT
struct FindMinOptions
{
	size_t max_iterations = 1000;
	double tolerance = 1e-12;
	double grad_tolerance = 1e-8;
	double step = 1.0;
	double step_shrink = 0.5;
	double step_grow = 1.2;
	double min_step = 1e-12;
	double armijo = 1e-4;
	double diff_step = 1e-6;
};

ADAPT_EXPORT
struct FindMinResult
{
	std::vector<double> params;
	double value = 0.0;
	size_t iterations = 0;
	bool converged = false;
};

namespace detail
{

template <class Func>
concept fm_objective_function = requires(Func f, std::span<const double> params)
{
	{ f(params) } -> std::convertible_to<double>;
};

template <class GradFunc>
concept fm_gradient_with_output = requires(GradFunc f, std::span<double> out, std::span<const double> params)
{
	{ f(out, params) } -> std::same_as<void>;
};

template <class GradFunc>
concept fm_gradient_returns_range = requires(GradFunc f, std::span<const double> params)
{
	{ f(params) } -> std::ranges::input_range;
};

inline void EvaluateGradientNumerical(std::span<double> out,
									  std::vector<double>& params,
									  auto&& func,
									  double diff_step)
{
	for (size_t j = 0; j < params.size(); ++j)
	{
		double base = params[j];
		double h = diff_step * std::max(1.0, std::abs(base));
		if (h == 0.0) h = diff_step;

		params[j] = base + h;
		double fp = static_cast<double>(std::invoke(func, std::span<const double>(params)));

		params[j] = base - h;
		double fm = static_cast<double>(std::invoke(func, std::span<const double>(params)));

		params[j] = base;
		out[j] = (fp - fm) / (2.0 * h);
	}
}

template <class Func, class GradFunc>
inline void EvaluateGradient(std::span<double> out,
							 std::vector<double>& params,
							 Func&& func,
							 GradFunc&& grad,
							 double diff_step)
{
	if constexpr (std::is_same_v<std::nullptr_t, std::decay_t<GradFunc>>)
	{
		EvaluateGradientNumerical(out, params, std::forward<Func>(func), diff_step);
	}
	else if constexpr (fm_gradient_with_output<GradFunc>)
	{
		std::invoke(grad, out, std::span<const double>(params));
	}
	else if constexpr (fm_gradient_returns_range<GradFunc>)
	{
		auto g = std::invoke(grad, std::span<const double>(params));
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
		EvaluateGradientNumerical(out, params, std::forward<Func>(func), diff_step);
	}
}

template <class Func, class GradFunc>
FindMinResult FindMin_impl(std::vector<double> params,
						   Func&& func,
						   GradFunc&& grad,
						   const FindMinOptions& options)
{
	if (params.empty())
		throw InvalidArg("Parameter size must be greater than zero.");
	if (options.diff_step <= 0.0)
		throw InvalidArg("diff_step must be greater than zero.");
	if (options.min_step <= 0.0)
		throw InvalidArg("min_step must be greater than zero.");
	if (options.step_shrink <= 0.0 || options.step_shrink >= 1.0)
		throw InvalidArg("step_shrink must be in (0, 1).");
	if (options.step_grow <= 1.0)
		throw InvalidArg("step_grow must be greater than 1.");

	FindMinResult result;
	result.params = std::move(params);
	result.value = static_cast<double>(std::invoke(func, std::span<const double>(result.params)));

	std::vector<double> grad_vec(result.params.size());
	std::vector<double> direction(result.params.size());
	std::vector<double> trial(result.params.size());

	double step = options.step;

	for (size_t iter = 0; iter < options.max_iterations; ++iter)
	{
		EvaluateGradient(grad_vec, result.params, func, grad, options.diff_step);

		double grad_norm2 = 0.0;
		for (double v : grad_vec) grad_norm2 += v * v;

		if (std::sqrt(grad_norm2) < options.grad_tolerance)
		{
			result.converged = true;
			result.iterations = iter;
			return result;
		}

		for (size_t j = 0; j < direction.size(); ++j) direction[j] = -grad_vec[j];

		bool accepted = false;
		double current_value = result.value;
		double local_step = step;

		while (local_step >= options.min_step)
		{
			for (size_t j = 0; j < trial.size(); ++j)
				trial[j] = result.params[j] + local_step * direction[j];

			double trial_value = static_cast<double>(std::invoke(func, std::span<const double>(trial)));
			double expected = current_value - options.armijo * local_step * grad_norm2;

			if (trial_value <= expected)
			{
				result.params = trial;
				result.value = trial_value;
				accepted = true;
				step = local_step * options.step_grow;
				break;
			}

			local_step *= options.step_shrink;
		}

		result.iterations = iter + 1;

		if (!accepted)
			return result;

		if (std::abs(current_value - result.value) < options.tolerance)
		{
			result.converged = true;
			return result;
		}
	}

	return result;
}

}

namespace opts
{
struct FindMinKeywords {};
template <std::invocable GradFunc> struct FMGradFuncOption {};

ADAPT_EXPORT ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(fm_grad, AnyTypeKeyword<FMGradFuncOption>, FindMinKeywords);
ADAPT_EXPORT ADAPT_DEFINE_TAGGED_KEYWORD_OPTION_WITH_VALUE(fm_options, FindMinOptions, FindMinKeywords);

template <class Option>
concept find_min_option = keyword_arg_tagged_with<Option, FindMinKeywords>;
}

ADAPT_EXPORT
template <detail::fm_objective_function Func, opts::find_min_option ...Options>
FindMinResult FindMin(Func&& func,
					  std::vector<double> params,
					  Options&& ...options)
{
	auto get_grad = [&](auto ...opts)
	{
		if constexpr (KeywordExists(opts::fm_grad, opts...))
			return GetKeywordArg(opts::fm_grad, opts...);
		else
			return nullptr;
	};
	FindMinOptions o{};
	if constexpr (KeywordExists(opts::fm_options, options...))
		o = GetKeywordArg(opts::fm_options, options...);

	auto&& grad = get_grad(options...);

	return detail::FindMin_impl(std::move(params),
								std::forward<Func>(func),
								std::forward<decltype(grad)>(grad),
								o);
}

}

#endif