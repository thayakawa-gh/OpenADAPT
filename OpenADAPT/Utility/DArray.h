#ifndef ADAPT_DARRAY_H
#define ADAPT_DARRAY_H

#include <cstdlib>
#include <memory>
#include <OpenADAPT/CUF/Utility.h>

namespace adapt
{

template <class T>
class DArray
{
public:

	static_assert(alignof(T) <= sizeof(void*));

	DArray(size_t size, const T& v)
	{
		m_ptr = AlignedAlloc(size, alignof(T));
		m_end = m_ptr + size;
		for (T* p = m_ptr; p < m_end; ++p)
		{

		}
	}

private:
	T* m_ptr;
	const T* m_end;
};

}

#endif