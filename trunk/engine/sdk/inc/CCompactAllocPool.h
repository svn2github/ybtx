#pragma once
#include "ISqrAllocPool.h"
#include "CMallocObject.h"

namespace sqr
{
	//单线程并且footprint不会记录内存块大小的分配器，依赖外部每次传入内存大小

	class MODULE_API CCompactAllocPool
		: public CMallocObject
		, public ISqrAllocPool
	{
	public:
		CCompactAllocPool(size_t stCapacity, const char* szPoolName);

		~CCompactAllocPool();

		void* Alloc(size_t stSize);
		void Dealloc(void* pMem,size_t stSize);
		void* Realloc(void* pMem,size_t stOldSize,size_t stNewSize);

		size_t GetMemUsage()const;
		size_t GetCapacity()const;

		const char* GetPoolName()const;
		void Trim();

	private:
		bool CheckLeakageEnabled()const;

		char		m_szPoolName[256];
		
		void*		m_pSpace;

		size_t		m_stMemUsage;

		bool		m_bDelayTrim;
	};

}

