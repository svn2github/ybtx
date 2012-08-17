#pragma once
#include <cstdlib>
#include "CMallocObject.h"
#include "SqrAlloc.h"

namespace sqr
{
	template <class T> class TMallocAllocator;

	template <class T>
	class TMallocAllocator
	{
	public:
		typedef size_t    size_type;
		typedef ptrdiff_t difference_type;
		typedef T*        pointer;
		typedef const T*  const_pointer;
		typedef T&        reference;
		typedef const T&  const_reference;
		typedef T         value_type;

		template <class U>
		struct rebind
		{
		public:
			typedef TMallocAllocator<U>	other;
		};

		TMallocAllocator()throw() {}

		template <class U>
		TMallocAllocator(const TMallocAllocator<U>& ano)throw() {}
		~TMallocAllocator() throw() {}

		bool operator==(const TMallocAllocator<T>& ano)const throw()
		{
			return true;
		}

		pointer allocate(size_type size)
		{
			return reinterpret_cast<pointer>(SqrMalloc(size*sizeof(T)));
		}

		void deallocate(pointer p, size_type n)
		{
			SqrFree(p);
		}

		size_type max_size() const
		{
			return size_type(size_type(-1) / sizeof(value_type));
		}

		void construct(pointer p, const value_type& value)
		{
			new(p) value_type(value);
		}

		void destroy(pointer p)
		{
			p->~T();
		}
	};
}


