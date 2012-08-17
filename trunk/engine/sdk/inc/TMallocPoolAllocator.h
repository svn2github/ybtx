#pragma once 

#include "CMallocObject.h"
#include "CMallocPool.h"

namespace sqr
{
	template <class T>
	class TMallocPoolAlloc : public CMallocObject
	{
	private:
		typedef CMallocPool Pool_t;
	public:
		Pool_t*					m_pPool;
		typedef T					value_type;
		typedef value_type*			pointer;
		typedef const value_type*	const_pointer;
		typedef value_type&			reference;
		typedef const value_type&	const_reference;
		typedef size_t				size_type;
		typedef ptrdiff_t			difference_type;

		template <class U>
		struct rebind
		{
		public:
			typedef TMallocPoolAlloc<U>	other;
		};

		pointer allocate(size_type sz)
		{
			pointer p = reinterpret_cast<pointer>(m_pPool->Alloc(sizeof(value_type)*sz));
			return p;
		}

		void deallocate(pointer p, size_type /*n*/)
		{
		}

		TMallocPoolAlloc(size_t size)
		{
			m_pPool = new Pool_t(size);
		}

		template<class U>
		TMallocPoolAlloc(const TMallocPoolAlloc<U>& u)
		{
			m_pPool = u.m_pPool;
		}

		~TMallocPoolAlloc()
		{
			//此处不能删除m_pPool,否则过程中复制的多个allocator共用m_pPool就全坏掉。
		}

		void release()
		{
			delete m_pPool;
			delete this;
		}

		void construct(pointer p, const value_type& value)
		{
			new(p) value_type(value);
		}

		void destroy(pointer p)
		{
			p->~T();
		}

		void* AllocMem(size_t size)
		{
			return m_pPool->Alloc(size);
		}

		size_type max_size() const
		{
			return size_type(size_type(-1) / sizeof(value_type));
		}
	};

}

