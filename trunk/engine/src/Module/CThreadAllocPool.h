#pragma once
#include "CMallocObject.h"
#include "CMemoryCookie.h"

namespace sqr
{
	struct CMemoryCookie;
	class CSqrAllocPool;

	class CThreadAllocPool
		:public CMallocObject
	{
	public:
		CThreadAllocPool(CSqrAllocPool* pPool);
		~CThreadAllocPool(void);

		void* Alloc(size_t stSize);
		void Dealloc(void* pMem);
		void* Realloc(void* pMem,size_t stNewSize);
		static size_t GetMemSize(void* pMem);

		void* GetMSpace()const;

		size_t GetMemUsage()const;
		size_t GetCapacity()const;

		void FreeFromMemThread( void* pMem );

		void DelayTrim();

	private:
		void PushToFreeQueue( CMemoryCookie* pCookie );
		void HandleFreeQueue();
		inline void ConfirmIsChildPool( CThreadAllocPool* pThreadPoolOfCookie );

	private:
		void*	m_dlmspace;

		CSqrAllocPool*	m_pParentPool;

#if defined(_WIN64) || (!defined(_WIN32))
		volatile int64	m_nMemUsage;
#else
		volatile int32	m_nMemUsage;
#endif

		CMemoryCookie *	volatile m_pFreeQueueIn;
		CMemoryCookie *	volatile m_pFreeQueueOut;
		bool m_bDelayTrim;

	};

}
