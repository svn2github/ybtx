#pragma once 
#include "CMemPool.h"

//static void mem_alloc();
//static void mem_delloc();

namespace sqr
{
	class CSqrAllocPool;

	template <class T>
	class TMemPoolAllocator
	{
	private:
		CMemPool*					m_pBuff;

		template <class U>
		friend class TMemPoolAllocator;
	public:		
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
			typedef TMemPoolAllocator<U>	other;
		};

		inline pointer allocate(size_type sz)
		{
			pointer p = reinterpret_cast<pointer>(m_pBuff->Alloc(sizeof(value_type)*sz));
			return p;
		}

		inline void deallocate(pointer p, size_type /*n*/)
		{
		}

		inline TMemPoolAllocator(size_t size, CSqrAllocPool* pPool = NULL)
		{
			m_pBuff = new CMemPool( size, pPool );
		}

		inline TMemPoolAllocator(CMemPool* pPool)
		{
			m_pBuff = pPool;
		}

		template<class U>
		inline TMemPoolAllocator(const TMemPoolAllocator<U>& u)
		{
			m_pBuff = u.m_pBuff;
		}

		inline ~TMemPoolAllocator()
		{
		}

		inline bool operator==(const TMemPoolAllocator<T>& Other) const 
		{
			return m_pBuff == Other.m_pBuff;
		}

		inline void release()
		{
			delete m_pBuff;
			delete this;
		}

		inline void construct(pointer p, const value_type& value)
		{
			new(p) value_type(value);
		}

		inline void destroy(pointer p)
		{
			p->~T();
		}

		inline size_type max_size() const
		{
			return size_type(size_type(-1) / sizeof(value_type));
		}

		inline void reset()
		{
			m_pBuff->Clear();
		}

		inline void* AllocMem(size_t size)
		{
			return m_pBuff->Alloc(size);
		}

		inline CMemPool* GetMemPool()const
		{
			return m_pBuff;
		}
	};

}

