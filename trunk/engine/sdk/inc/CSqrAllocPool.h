#pragma once

#include "ModuleDefs.h"
#include "CMallocObject.h"
#include "TMallocAllocator.h"
#include "ISqrAllocPool.h"
#include "ThreadTypes.h"

namespace sqr
{
	class CThreadAllocPool;

#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable:4251) //dll linkage
#endif

	class MODULE_API CSqrAllocPool
		: public CMallocObject
		, public ISqrAllocPool
	{
		friend class CMemFreeJob;
		friend class CThreadAllocPool;
		friend class CSqrAllocPoolMgr;
	public:
		CSqrAllocPool(const char* szPoolName);
		
		void* Alloc(size_t stSize);
		void Dealloc(void* pMem);
		void* Realloc(void* pMem,size_t stNewSize);

		static size_t GetMemSize(void* pMem);

		size_t GetMemUsage()const;
		size_t GetCapacity()const;
		
		bool CheckLeakageEnabled()const;

		const char* GetPoolName()const;	
	private:
		//析构函数作为私有函数的意思就是，SqrAllocPool一旦创建就不要删除
		~CSqrAllocPool();

		void EnableFindLeakage();
		CThreadAllocPool* GetPool();
		void FreeFromMemThread( void* pMem );
		void Trim();
	private:	
		char		m_szPoolName[256];

		typedef deque<CThreadAllocPool*,TMallocAllocator<CThreadAllocPool*> >	PoolDeq_t;
		
		HTLSKEY				m_Key;
		mutable HLOCK		m_lkDeqPool;
		PoolDeq_t			m_deqPool;

		bool		m_bFindLeakage;

		volatile int32	m_nMemUsage;
	};

#ifdef _WIN32
#pragma warning(pop)
#endif

}

