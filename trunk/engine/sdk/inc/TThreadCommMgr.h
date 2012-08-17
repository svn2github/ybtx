#pragma once
#include "TMemPoolAllocator.h"
#include "CThreadTypes.h"

namespace sqr
{
	class CError;

	template<typename Traits>
	class TThreadCommMgr
	{
		typedef typename Traits::CommMgr		ImpCommMgr_t;
		typedef typename Traits::JobType		ImpJobType_t;
		typedef typename Traits::ResultType		ImpResultType_t;

#ifdef _WIN32
		friend ImpCommMgr_t;
#else
		friend class Traits::CommMgr;
#endif
	protected:
		TThreadCommMgr();
		~TThreadCommMgr();

		void AddJob(ImpJobType_t* pJob);
		void AddResult(ImpResultType_t* pResult);

		void HandleAllJobs();
		void HandleAllResults();

		void SpliceJobs();
		void SpliceResults();

		void OnHandleResultsDerived();
		void OnSpliceJobsDerived();
		void OnSwapJobReadyDerived();
		void OnHandleResultExpDerived(CError& exp);
		void OnDoJobExpDerived(CError& exp);
		
	protected:
		void* AllocJobMem(size_t stSize);
		void AllocWaitedJobList();
		void DeallocJobList();

		void* AllocResultMem(size_t stSize);
		void AllocWaitedResultList();
		void DeallocResultList();

	protected:
		typedef TMemPoolAllocator<ImpJobType_t*>		JobAlloc_t;
		typedef TMemPoolAllocator<ImpResultType_t*>		ResultAlloc_t;

		typedef deque<ImpJobType_t*, JobAlloc_t>		DequeJob_t;
		struct JobAndAlloc
		{
			DequeJob_t*		m_queJob;
			JobAlloc_t*		m_alloc;
		};

		std::list<JobAndAlloc*>					m_lstWaitedJob;  //等待list中的job
		std::list<JobAndAlloc*>					m_lstDoingJob;	//在线程里面执行的job
		std::list<JobAndAlloc*>					m_lstSwapJob;   //用于线程之间交换job

		typedef list<ImpResultType_t*, ResultAlloc_t>	DequeResult_t;
		struct ResultAndAlloc
		{
			DequeResult_t*	m_queResult;
			ResultAlloc_t*	m_alloc;
		};

		std::list<ResultAndAlloc*>				m_lstWaitedResult;
		std::list<ResultAndAlloc*>				m_lstDoingResult;
		std::list<ResultAndAlloc*>				m_lstSwapResult;
		
		
		HSPINLOCK					m_slkJobQueue;
		HSPINLOCK					m_slkResQueue;
	};

}
