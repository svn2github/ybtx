#pragma once 
#include "ThreadTypes.h"
#include <list>
#include <vector>
#include "CMallocObject.h"
#include "TMallocAllocator.h"

namespace sqr
{
	//请查看CMemoryLeakageMgr.h里面的说明内容


	class CMemThread;
	class CMemJob;
	struct CMemoryCookie;
	class CSqrAllocPool;
	class CMemoryLeakageMgr;

	class CMemCallStackInfo;
	class CMemCallStackAddrVector;

	class CMemThreadMgr : public CMallocObject
	{
		friend class CMemThread;
		friend class CMemEndThreadsJob;
		friend class CMemoryLeakageMgr;
	public:
		void HandleAllJobs();

		static CMemThreadMgr* Inst();
		
		void FlushDbJobs();
		
		void	AddMemMallocJob(CMemoryCookie* pCookie);
		void	AddMemFreeJob(CMemoryCookie* pCookie, CSqrAllocPool* pPool);
		void	AddDumpStackJob();

#ifndef _WIN32
		void	AddFetchAllSymbolJob();
		void	WaitFetchAllSymbolEnded();
		void	SetFetchAllSymbolEnded();
#endif

	private:
		CMemThreadMgr();
		~CMemThreadMgr();
	
		void	AddDelayedMemJob(CMemJob* pJob);
		void	AddEndThreadJob();
		
	private:
		bool		m_bEndThreads;

		bool		m_bHandleJobDone;		

		HSEMAPHORE	m_smJobQueue;
		HSEMAPHORE	m_smFetchSymbol;

		HLOCK	m_slkJobQueue;

		CMemThread*	m_pThread;
		
		typedef std::list<CMemJob*, TMallocAllocator<CMemJob*> >	ListMemJob;
		ListMemJob	m_lstMemDoingJob;	//在线程里面执行的job
	};
}

