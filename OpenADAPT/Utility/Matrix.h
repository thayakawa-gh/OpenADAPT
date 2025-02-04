#ifndef ADAPT_UTILITY_MATRIX_H
#define ADAPT_UTILITY_MATRIX_H

#include <memory>
#include <cassert>
#include <numeric>
#include <OpenADAPT/Utility/TypeTraits.h>
#include <OpenADAPT/Utility/Function.h>

namespace adapt
{

template <class T, size_t Dim = 2>
class Matrix
{
	template <size_t X>
	static size_t CalcUnit(std::array<uint32_t, X> sizes)
	{
		return std::accumulate(sizes.begin() + 1, sizes.end(), (size_t)1, std::multiplies<size_t>());
	}
	template <std::integral S, std::integral ...Sizes>
	static size_t CalcUnit(S, Sizes ...sizes)
	{
		return ((size_t)sizes * ...);
	}

	template <size_t Dim_, template <class...> class Qualifier>
	class Iterator;
	template <size_t Dim_, template <class...> class Qualifier>
	class Range
	{
		static constexpr bool IsConst = std::is_const_v<Qualifier<T>>;
	public:

		using iterator = Iterator<Dim_ - 1, std::type_identity_t>;
		using const_iterator = Iterator<Dim_ - 1, std::add_const_t>;

		template <size_t Dim__> requires (Dim__ == Dim_ || Dim__ == Dim_ + 1)
		Range(Qualifier<T>* begin, const std::array<uint32_t, Dim__>& sizes)
			: m_begin(begin)
		{
			for (size_t i = 0; i < Dim_; ++i)
			{
				if constexpr (Dim__ == Dim_) m_sizes[i] = sizes[i];
				else m_sizes[i] = sizes[i + 1];
			}
			m_unit = CalcUnit(m_sizes);
		}
		Range(const Range<Dim_, Qualifier>& r)
			: m_begin(r.m_begin), m_unit(r.m_unit), m_sizes(r.m_sizes)
		{}
		Range<Dim_, Qualifier>& operator=(const Range<Dim_, Qualifier>& r)
		{
			m_begin = r.m_begin;
			m_unit = r.m_unit;
			m_sizes = r.m_sizes;
			return *this;
		}

		uint32_t size() const { return m_sizes[0]; }

		Range<Dim_ - 1, std::type_identity_t> operator[](uint32_t i) requires (!IsConst)
		{
			assert(i < m_sizes[0]);
			return Range<Dim_ - 1, std::type_identity_t>(m_begin + i * m_unit, m_sizes);
		}
		Range<Dim_ - 1, std::add_const_t> operator[](uint32_t i) const
		{
			assert(i < m_sizes[0]);
			return Range<Dim_ - 1, std::add_const_t>(m_begin + i * m_unit, m_sizes);
		}
		iterator begin() requires (!IsConst) { return iterator(m_begin, m_sizes); }
		iterator end() requires (!IsConst) { return iterator(m_begin + m_sizes[0] * m_unit, m_sizes); }
		const_iterator begin() const { return cbegin(); }
		const_iterator end() const { return cend(); }
		const_iterator cbegin() const { return const_iterator(m_begin, m_sizes); }
		const_iterator cend() const { return const_iterator(m_begin + m_sizes[0] * m_unit, m_sizes); }

	private:
		Qualifier<T>* m_begin = nullptr;
		size_t m_unit = 0;
		std::array<uint32_t, Dim_> m_sizes = {};
	};
	template <template <class...> class Qualifier>
	class Range<1, Qualifier>
	{
		static constexpr bool IsConst = std::is_const_v<Qualifier<T>>;
	public:

		using iterator = Iterator<0, std::type_identity_t>;
		using const_iterator = Iterator<0, std::add_const_t>;

		template <size_t Dim_> requires (Dim_ == 1 || Dim_ == 2)
		Range(Qualifier<T>* begin, const std::array<uint32_t, Dim_>& sizes)
			: m_begin(begin), m_end(begin + sizes.back())
		{}
		Range(const Range<1, Qualifier>& r)
			: m_begin(r.m_begin), m_end(r.m_end)
		{}
		Range<1, Qualifier>& operator=(const Range<1, Qualifier>& r)
		{
			m_begin = r.m_begin;
			m_end = r.m_end;
			return *this;
		}

		uint32_t size() const { return uint32_t(m_end - m_begin); }

		const T& operator[](uint32_t i) const
		{
			assert(m_begin + i < m_end);
			return m_begin[i];
		}
		T& operator[](uint32_t i) requires (!IsConst)
		{
			assert(m_begin + i < m_end);
			return m_begin[i];
		}
		iterator begin() requires (!IsConst) { return iterator(m_begin); }
		iterator end() requires (!IsConst) { return iterator(m_end); }
		const_iterator begin() const { return cbegin(); }
		const_iterator end() const { return cend(); }
		const_iterator cbegin() const { return const_iterator(m_begin); }
		const_iterator cend() const { return const_iterator(m_end); }

	private:
		Qualifier<T>* m_begin = nullptr;
		Qualifier<T>* m_end = nullptr;
	};
	template <size_t Dim_, template <class...> class Qualifier>
	struct RangePointerProxy
	{
		Range<Dim_, Qualifier> m_range;
		Range<Dim_, Qualifier>* operator->() { return &m_range; }
	};
	template <size_t Dim_, template <class...> class Qualifier>
	class Iterator
	{
		static constexpr bool IsConst = std::is_const_v<Qualifier<T>>;
	public:
		using difference_type = ptrdiff_t;
		using value_type = Range<Dim_, Qualifier>;
		using reference = Range<Dim_, Qualifier>;
		using iterator_category = std::random_access_iterator_tag;

		Iterator() = default;
		Iterator(Qualifier<T>* pos, const std::array<uint32_t, Dim_ + 1>& sizes)
			: m_current(pos)
		{
			for (size_t i = 0; i < Dim_; ++i) m_sizes[i] = sizes[i + 1];
			m_unit = CalcUnit(sizes);
		}
		Iterator(const Iterator<Dim_, Qualifier>& it) = default;
		Iterator& operator=(const Iterator<Dim_, Qualifier>& it) = default;

		Iterator<Dim_, Qualifier>& operator++()
		{
			m_current += m_unit;
			return *this;
		}
		Iterator<Dim_, Qualifier> operator++(int)
		{
			Iterator<Dim_, Qualifier> res = *this;
			++(*this);
			return res;
		}
		Iterator<Dim_, Qualifier> operator+=(size_t n)
		{
			m_current += m_unit * n;
			return *this;
		}
		Iterator<Dim_, Qualifier> operator+(size_t n) const
		{
			Iterator<Dim_, Qualifier> res = *this;
			res += n;
			return res;
		}
		Iterator<Dim_, Qualifier>& operator--()
		{
			m_current -= m_unit;
			return *this;
		}
		Iterator<Dim_, Qualifier> operator--(int)
		{
			Iterator<Dim_, Qualifier> res = *this;
			--(*this);
			return res;
		}
		Iterator<Dim_, Qualifier> operator-=(size_t n)
		{
			m_current -= m_unit * n;
			return *this;
		}
		Iterator<Dim_, Qualifier> operator-(size_t n) const
		{
			Iterator<Dim_, Qualifier> res = *this;
			res -= n;
			return res;
		}

		bool operator==(const Iterator<Dim_, Qualifier>& it) const
		{
			return m_current == it.m_current;
		}
		bool operator!=(const Iterator<Dim_, Qualifier>& it) const
		{
			return !(*this == it);
		}

		Range<Dim_, Qualifier> operator*() const
		{
			return Range<Dim_, Qualifier>(m_current, m_sizes);
		}
		RangePointerProxy<Dim_, Qualifier> operator->() const
		{
			return RangePointerProxy<Dim_, Qualifier>{ { m_current, m_sizes } };
		}

	private:
		Qualifier<T>* m_current = nullptr;
		size_t m_unit = 0;
		std::array<uint32_t, Dim_> m_sizes = {};
	};
	template <template <class...> class Qualifier>
	class Iterator<0, Qualifier>
	{
		static constexpr bool IsConst = std::is_const_v<Qualifier<T>>;
	public:
		using difference_type = ptrdiff_t;
		using value_type = T;
		using reference = T&;
		using iterator_category = std::random_access_iterator_tag;

		Iterator() = default;
		Iterator(Qualifier<T>* pos)
			: m_current(pos)
		{}
		Iterator(const Iterator<0, Qualifier>& it) = default;
		Iterator<0, Qualifier>& operator=(const Iterator<0, Qualifier>& it) = default;

		Iterator<0, Qualifier>& operator++()
		{
			++m_current;
			return *this;
		}
		Iterator<0, Qualifier> operator++(int)
		{
			Iterator<0, Qualifier> res = *this;
			++(*this);
			return res;
		}
		Iterator<0, Qualifier>& operator+=(size_t n)
		{
			m_current += n;
			return *this;
		}
		Iterator<0, Qualifier> operator+(size_t n) const
		{
			Iterator<0, Qualifier> res = *this;
			res += n;
			return res;
		}
		Iterator<0, Qualifier>& operator--()
		{
			--m_current;
			return *this;
		}
		Iterator<0, Qualifier> operator--(int)
		{
			Iterator<1, Qualifier> res = *this;
			--(*this);
			return res;
		}
		Iterator<0, Qualifier>& operator-=(size_t n)
		{
			m_current -= n;
			return *this;
		}
		Iterator<0, Qualifier> operator-(size_t n) const
		{
			Iterator<0, Qualifier> res = *this;
			res -= n;
			return res;
		}

		bool operator==(const Iterator<0, Qualifier>& it) const
		{
			return m_current == it.m_current;
		}
		bool operator!=(const Iterator<0, Qualifier>& it) const
		{
			return !(*this == it);
		}

		Qualifier<T>& operator*() const { return *m_current; }

	private:
		Qualifier<T>* m_current;
	};
public:

	using iterator = Iterator<Dim - 1, std::type_identity_t>;
	using const_iterator = Iterator<Dim - 1, std::add_const_t>;

	static constexpr int GetDimension() { return Dim; }

	Matrix() {}
	template <std::integral ...Sizes>
		requires (sizeof...(Sizes) == Dim)
	Matrix(const T& i, Sizes ...sizes)
		: m_unit(CalcUnit(sizes...)), m_sizes{ (uint32_t)sizes... }
	{
		size_t cap = GetCapacity();
		Allocate(cap);
		ConstructAll(i, cap);
	}
	template <std::integral ...Sizes>
		requires (sizeof...(Sizes) == Dim)
	Matrix(Sizes ...sizes)
		: m_unit(CalcUnit(sizes...)), m_sizes{ (uint32_t)sizes... }
	{
		size_t cap = GetCapacity();
		Allocate(cap);
		ConstructAll(T{}, cap);
	}
	Matrix(const Matrix& m)
		: m_unit(m.m_unit), m_sizes(m.m_sizes)
	{
		size_t cap = GetCapacity();
		Allocate(cap);
		T* it = m_matrix_data;
		const T* it2 = m.m_matrix_data;
		const T* end = it + cap;
		for (; it != end; ++it, ++it2) new (it) T(*it2);
	}
	Matrix(Matrix&& m) noexcept
		: m_matrix_data(m.m_matrix_data), m_unit(m.m_unit), m_sizes(m.m_sizes)
	{
		m.m_matrix_data = nullptr;
		m.m_unit = 0;
		m.m_sizes = {};
	}
	~Matrix()
	{
		Destroy();
	}

	uint32_t GetSize(uint32_t dim) const { return m_sizes[dim]; }
	uint32_t size() const { return GetSize(0); }
	size_t GetCapacity() const { return m_unit * m_sizes[0]; }

	iterator begin() { return iterator(m_matrix_data, m_sizes); }
	iterator end() { return iterator(m_matrix_data + GetCapacity(), m_sizes); }
	const_iterator begin() const { return cbegin(); }
	const_iterator end() const { return cend(); }
	const_iterator cbegin() const { return const_iterator(m_matrix_data, m_sizes); }
	const_iterator cend() const { return const_iterator(m_matrix_data + GetCapacity(), m_sizes); }

	Range<Dim - 1, std::type_identity_t> operator[](uint32_t i)
	{
		assert(i < m_sizes[0]);
		return Range<Dim - 1, std::type_identity_t>(m_matrix_data + i * m_unit, m_sizes);
	}
	Range<Dim - 1, std::add_const_t> operator[](uint32_t i) const
	{
		assert(i < m_sizes[0]);
		return Range<Dim - 1, std::add_const_t>(m_matrix_data + i * m_unit, m_sizes);
	}

private:

	void Allocate(size_t size)
	{
		assert(m_matrix_data == nullptr);
		char* p = (char*)malloc(size * sizeof(T));
		m_matrix_data = (T*)p;
	}
	void Reallocate(size_t size)
	{
		Destroy();
		Allocate(size);
	}
	void Destroy()
	{
		if (m_matrix_data == nullptr) return;
		DestructAll(GetCapacity());
		char* ptr = (char*)m_matrix_data;
		free(ptr);
		m_matrix_data = nullptr;
	}
	void ConstructAll(const T & t, size_t cap)
	{
		T* it = m_matrix_data;
		T* end = m_matrix_data + cap;
		for (; it != end; ++it) new (it) T(t);
	}
	void DestructAll(size_t cap)
	{
		//メモリ解放を行わない、ただ全要素のデストラクタを呼ぶもの。
		T* it = m_matrix_data;
		T* end = m_matrix_data + cap;
		for (; it != end; ++it) it->~T();
	}

	T* m_matrix_data = nullptr;
	size_t m_unit = 0;
	std::array<uint32_t, Dim> m_sizes = {};
};

}

#endif