#ifndef ADAPT_PLOT_ARRAYDATA_H
#define ADAPT_PLOT_ARRAYDATA_H

#include <vector>
#include <string>
#include <string_view>
#include <OpenADAPT/Utility/Macro.h>
#include <OpenADAPT/Utility/TypeTraits.h>
#include <OpenADAPT/Utility/Any.h>
#include <OpenADAPT/Utility/Matrix.h>
#include <OpenADAPT/Utility/Print.h>
#include <OpenADAPT/Utility/Ranges.h>

namespace adapt
{

namespace detail
{

class AnyAcceptableArg
{
public:

	class sentinel
	{
	public:
		template <class T>
		sentinel(T&& t) : m_st(std::forward<T>(t)) {};
	private:
		Any m_st;
	};
	class iterator
	{
	public:
		template <class T>
		iterator(T&& t) : m_it(std::forward<T>(t)) {}

		template <class Stream>
		void Output(Stream& str) const
		{
			adapt::Print(str, )
		}
		const iterator& operator++() { m_ft.increment(m_it.GetPtr()); return *this; }

	private:

		struct FuncTable
		{
			template <class Iter>
			static void Output(FILE* pipe, const void* it)
			{

			}
			template <class Iter>
			static void Increment(const void* it)
			{
				++(*static_cast<const Iter*>(it));
			}
			template <class Iter>
			static bool Compare(const void* it, const sentinel& last)
			{
				return *static_cast<const Iter*>(it) == last;
			}

			bool(*is_arithmetic_range)(const void*);
			bool(*is_string_range)(const void*);
			double(*get_double)(const void*);
			std::string_view(*get_string)(const void*);
			void(*increment)(const void*);
		};
		template <class T>
		inline constexpr FuncTable m_ft{ &FuncTable::IsArithmeticRange<T>, &FuncTable::IsStringRange<T>,
										 &FuncTable::GetDouble<T>, &FuncTable::GetString<T>,
										 &FuncTable::Increment<T> };

		detail::Any_impl<40, true> m_it;
	};

	iterator begin() const
	{

	}

private:
	struct FuncTable
	{
		template <class T>
		iterator Begin(const void* arr)
		{
			if constexpr (std::)
			auto it = static_cast<const T*>(arr)->begin();
			return iterator(std::move(it));
		}
		template <class T>
		sentinel End(const void* arr)
		{

		}
	};
	AnyCRef m_array;
};


struct ToFPtr
{
	template <class ...Args>
	void operator()(Args&& ...args) const
	{
		adapt::Print(pipe, std::forward<Args>(args)...);
	}
	FILE* pipe;
};
struct ToOStr
{
	template <class ...Args>
	void operator()(Args&& ...args)
	{
		adapt::Print(ofs, std::forward<Args>(args)...);
	}
	std::ofstream& ofs;
};

struct ArrayDataBase
{
	template <class T>
	static constexpr bool IsNumericRange()
	{
		if constexpr (!std::ranges::input_range<std::remove_cvref_t<T>>) return false;
		//char[N]のような固定長配列やstd::stringはarithmeticかつrangeであるが、
		//IsColumnと識別するために、
		//std::string_viewに変換可能なものは弾く。
		else if constexpr (std::is_convertible_v<T, std::string_view>) return false;
		else
		{
			using ValueType = std::remove_cvref_t<decltype(*std::ranges::begin(std::declval<T&>()))>;
			if constexpr (std::is_arithmetic_v<ValueType>) return true;
			else return false;
		}
	}
	template <class T>
	static constexpr bool IsStringRange()
	{
		if constexpr (!std::ranges::input_range<std::remove_cvref_t<T>>) return false;
		else
		{
			using ValueType = decltype(*std::ranges::begin(std::declval<T&>()));
			if constexpr (std::is_convertible_v<ValueType, std::string_view>) return true;
			else return false;
		}
	}
	template <class T>
	static constexpr bool IsColumn()
	{
		return std::is_convertible_v<T, std::string_view>;
	}
	template <class T>
	static constexpr bool IsUnique()
	{
		return std::is_arithmetic_v<T>;
	}

	struct Increment_impl
	{
		using ArgTypes = std::tuple<>;
		using RetType = void;
		template <class T, class S>
		void operator()(T& it, const S&) const
		{
			//Genericsにstd::anyを使う場合、Tがconstとnon constの両方の場合がコンパイル対象になる。
			//可能なら呼び出し不可能な方は自動的に例外でも投げるべきなんだが、難しい。
			if constexpr (std::is_const_v<T>) throw InvalidArg("");
			else ++it;
		}
	};
	struct Indirection_impl
	{
		using ArgTypes = std::tuple<>;
		using RetType = double;
		template <class T, class S>
		double operator()(const T& it, const S&) const
		{
			if constexpr (std::is_convertible_v<decltype(*it), double>) return (double)*it;
			else throw MismatchType("");
		}
	};
	struct Compare_impl
	{
		using ArgTypes = std::tuple<>;
		using RetType = bool;
		template <class T, class S>
		bool operator()(const T& it, const S& last) const
		{
			return it == last;
		}
	};
	template <class Func>
	struct OutFunc
	{
		using ArgTypes = std::tuple<Func>;
		using RetType = void;
		template <class T, class S>
		void operator()(Func output_func, const T& it, const S&) const
		{
			output_func(" ", *it, adapt::print::end<'\0'>);
		}
	};

	struct Range
	{
		template <class A, class B>
		Range(A&& a, B&& b) : mRange(std::forward<A>(a), std::forward<B>(b)) {}
		void operator++() { mRange.Visit<0>(); }
		double operator*() const { return mRange.Visit<1>(); }
		bool End() const { return mRange.Visit<2>(); }
		void Output(ToFPtr o) const { mRange.Visit<3>(o); }
		void Output(ToOStr o) const { mRange.Visit<4>(o); }
	private:
		Generics<std::tuple<Any, Any>,
			std::tuple<Increment_impl, Indirection_impl, Compare_impl, OutFunc<ToFPtr>, OutFunc<ToOStr>>>
			mRange;
	};

	struct GetRange_impl
	{
		using ArgTypes = std::tuple<>;
		using RetType = Range;
		template <class T>
		Range operator()([[maybe_unused]] const T& v) const
		{
			if constexpr (!IsNumericRange<T>() && !IsStringRange<T>()) throw MismatchType("contained value is not a range");
			else return Range(std::ranges::begin(v), std::ranges::end(v));
		}
	};
	struct GetColumn_impl
	{
		using ArgTypes = std::tuple<>;
		using RetType = std::string_view;
		template <class T>
		std::string_view operator()([[maybe_unused]] T t) const
		{
			if constexpr (IsColumn<T>()) return t;
			else throw MismatchType("contained value is not a std::string");
		}
	};
	struct GetUnique_impl
	{
		using ArgTypes = std::tuple<>;
		using RetType = double;
		template <class T>
		double operator()([[maybe_unused]] const T& d) const
		{
			if constexpr (IsUnique<T>()) return d;
			else throw MismatchType("contained value is not a double");
		}
	};
};

struct ArrayData : public ArrayDataBase
{
	enum Type { NUM_RANGE, STR_RANGE, COLUMN, UNIQUE, };

	struct GetType_impl
	{
		using ArgTypes = std::tuple<>;
		using RetType = Type;
		template <class T>
		Type operator()(const T&) const
		{
			if constexpr (IsNumericRange<T>()) return NUM_RANGE;
			else if constexpr (IsStringRange<T>()) return STR_RANGE;
			else if constexpr (IsColumn<T>()) return COLUMN;
			else if constexpr (IsUnique<T>()) return UNIQUE;
			else throw MismatchType("no value is contained");
		}
	};

	using GenCRef = Generics<AnyCRef, std::tuple<GetType_impl, GetRange_impl, GetColumn_impl, GetUnique_impl>>;

public:

	struct RangeReceiver
	{
		template <class T, std::enable_if_t<IsNumericRange<T>() || IsStringRange<T>(), std::nullptr_t> = nullptr>
		RangeReceiver(const T& v) : mRange(v) {}
		GenCRef mRange;
	};

	ArrayData() : mAnyCRef(EmptyClass{}) {}
	template <class T, std::enable_if_t<IsNumericRange<T>(), std::nullptr_t> = nullptr>
	ArrayData(const T& nr) : mAnyCRef(nr) {}
	template <class T, std::enable_if_t<IsStringRange<T>(), std::nullptr_t> = nullptr>
	ArrayData(const T& sr) : mAnyCRef(sr) {}
	template <class T, std::enable_if_t<IsColumn<T>(), std::nullptr_t> = nullptr>
	ArrayData(const T& c) : mAnyCRef(c) {}
	template <class T, std::enable_if_t<IsUnique<T>(), std::nullptr_t> = nullptr>
	ArrayData(const T& u) : mAnyCRef(u) {}
	ArrayData(RangeReceiver r) : mAnyCRef(std::move(r.mRange)) {}
	Type GetType() const { return mAnyCRef.Visit<0>(); }
	Range GetRange() const { return mAnyCRef.Visit<1>(); }
	std::string_view GetColumn() const { return mAnyCRef.Visit<2>(); }
	double GetUnique() const { return mAnyCRef.Visit<3>(); }

	bool IsEmpty() const { return mAnyCRef.GetAny<0>().IsEmpty(); }
	operator bool() const { return !mAnyCRef.GetAny<0>().IsEmpty(); }

private:
	GenCRef mAnyCRef;
};

struct MatrixDataBase
{
	template <class X> using Mat = adapt::Matrix<X>;
	template <class T>
	static constexpr bool IsNumericMatrix()
	{
		return IsBaseOf_T_v<Mat, T>;
	}
	template <class T>
	static constexpr bool IsColumn()
	{
		return std::is_convertible_v<T, std::string_view>;
	}
	template <class T>
	static constexpr bool IsUnique()
	{
		return std::is_arithmetic_v<T>;
	}

	struct GetSize_impl
	{
		using ArgTypes = std::tuple<>;
		using RetType = std::pair<uint32_t, uint32_t>;
		template <class T>
		std::pair<uint32_t, uint32_t> operator()(const T& t) const
		{
			return { t.GetSize(0), t.GetSize(1) };
		}
	};
	template <class Func>
	struct Output_impl
	{
		using ArgTypes = std::tuple<Func, double, double, double, double, uint32_t, uint32_t>;
		using RetType = void;
		template <class T>
		void operator()(Func output_func,
						double x, double y, double cx, double cy,
						uint32_t ix, uint32_t iy, const T& map) const
		{
			output_func(x, y, cx, cy, map[ix][iy]);
		}
	};

	struct MatRef
	{
		template <class T>
		MatRef(const Matrix<T>& m) : mMat(m) {}
		std::pair<uint32_t, uint32_t> GetSize() const { return mMat.Visit<0>(); }
		void Out(ToFPtr f, double x, double y, double cx, double cy, uint32_t ix, uint32_t iy) const
		{
			mMat.Visit<1>(f, x, y, cx, cy, ix, iy);
		}
		void Out(ToOStr f, double x, double y, double cx, double cy, uint32_t ix, uint32_t iy) const
		{
			mMat.Visit<2>(f, x, y, cx, cy, ix, iy);
		}
	private:
		Generics<AnyCRef, std::tuple<GetSize_impl, Output_impl<ToFPtr>, Output_impl<ToOStr>>> mMat;
	};

	struct GetNumMat_impl
	{
		using ArgTypes = std::tuple<>;
		using RetType = MatRef;
		template <class T>
		MatRef operator()([[maybe_unused]] const T& t) const
		{
			if constexpr (IsNumericMatrix<T>()) return t;
			else throw MismatchType("contained value is not a adapt::Matrix");
		}
	};
	struct GetColumn_impl
	{
		using ArgTypes = std::tuple<>;
		using RetType = std::string_view;
		template <class T>
		std::string_view operator()([[maybe_unused]] T t) const
		{
			if constexpr (IsColumn<T>()) return t;
			else throw MismatchType("contained value is not a std::string");
		}
	};
	struct GetUnique_impl
	{
		using ArgTypes = std::tuple<>;
		using RetType = double;
		template <class T>
		double operator()([[maybe_unused]] const T& d) const
		{
			if constexpr (IsUnique<T>()) return d;
			else throw MismatchType("contained value is not a double");
		}
	};
};

struct MatrixData : public MatrixDataBase
{
	enum Type { NUM_MAT, COLUMN, UNIQUE, };

	struct GetType_impl
	{
		using ArgTypes = std::tuple<>;
		using RetType = Type;
		template <class T>
		Type operator()(const T&) const
		{
			if constexpr (IsNumericMatrix<T>()) return NUM_MAT;
			else if constexpr (IsColumn<T>()) return COLUMN;
			else if constexpr (IsUnique<T>()) return UNIQUE;
			else throw MismatchType("no value is contained");
		}
	};

	using CRef = Generics<AnyCRef, std::tuple<GetType_impl, GetNumMat_impl, GetColumn_impl, GetUnique_impl>>;

public:

	MatrixData() : mAnyCRef(EmptyClass{}) {}
	template <class T>
	MatrixData(const Matrix<T>& matrix) : mAnyCRef(matrix) {}
	template <class T, std::enable_if_t<IsColumn<T>(), std::nullptr_t> = nullptr>
	MatrixData(const T& c) : mAnyCRef(c) {}
	template <class T, std::enable_if_t<IsUnique<T>(), std::nullptr_t> = nullptr>
	MatrixData(const T& u) : mAnyCRef(u) {}

	Type GetType() const { return mAnyCRef.Visit<0>(); }
	MatRef GetMatrix() const { return mAnyCRef.Visit<1>(); }
	std::string_view GetColumn() const { return mAnyCRef.Visit<2>(); }
	double GetUnique() const { return mAnyCRef.Visit<3>(); }

	bool IsEmpty() const { return mAnyCRef.GetAny<0>().IsEmpty(); }
	operator bool() const { return !IsEmpty(); }

private:

	CRef mAnyCRef;
};

}

}

#endif