#pragma once
#include "TThreadCommMgr.h"
#include "ThreadHelper.h"
#include "ExpHelper.h"

template<typename Traits>
TThreadCommMgr<Traits>::TThreadCommMgr()
{
	CreateSpinLock(&m_slkJobQueue);
	CreateSpinLock(&m_slkResQueue);

	AllocWaitedJobList();
	AllocWaitedResultList();
}

template<typename Traits>
TThreadCommMgr<Traits>::~TThreadCommMgr()
{
	DestroySpinLock(&m_slkResQueue);
	DestroySpinLock(&m_slkJobQueue);

	DeallocJobList();
	DeallocResultList();
}

template<typename Traits>
void TThreadCommMgr<Traits>::SpliceJobs()
{
	SpinLock(&m_slkJobQueue);
    m_lstSwapJob.splice(m_lstSwapJob.end(), m_lstWaitedJob);
    SpinUnlock(&m_slkJobQueue);

	static_cast<ImpCommMgr_t*>(this)->OnSwapJobReadyDerived();

	AllocWaitedJobList();
}

template<typename Traits>
void TThreadCommMgr<Traits>::HandleAllJobs()
{
    SpinLock(&m_slkJobQueue);
    m_lstDoingJob.splice(m_lstDoingJob.end(), m_lstSwapJob);
    SpinUnlock(&m_slkJobQueue);
	
    for(typename list<JobAndAlloc*>::iterator it=m_lstDoingJob.begin(); 
		it != m_lstDoingJob.end(); ++it)
    {
		JobAndAlloc* pJobAlloc = *it;
	
		DequeJob_t *pQueJob = pJobAlloc->m_queJob;
		for(typename DequeJob_t::iterator it1 = pQueJob->begin(); 
			it1 != pQueJob->end(); ++it1)
		{
			ImpJobType_t* pJob = *it1;
			SQR_TRY
			{		
				pJob->DoJob();
				pJob->~ImpJobType_t();
			}
			SQR_CATCH(exp)
			{
				static_cast<ImpCommMgr_t*>(this)->OnDoJobExpDerived(exp);
			}
			SQR_TRY_END;
		}
    }

	while (!m_lstDoingJob.empty())
	{
		JobAndAlloc* pJobAlloc = m_lstDoingJob.front();
		m_lstDoingJob.pop_front();
		pJobAlloc->m_alloc->release();
	}
}

template<typename Traits>
void TThreadCommMgr<Traits>::SpliceResults()
{
	if(m_lstWaitedResult.back()->m_queResult->empty())
		return;

	bool bEmpty = false;
	SpinLock(&m_slkResQueue);
	bEmpty = m_lstSwapResult.empty();
	m_lstSwapResult.splice(m_lstSwapResult.end(), m_lstWaitedResult);
	SpinUnlock(&m_slkResQueue);

	if(bEmpty)
		static_cast<ImpCommMgr_t*>(this)->OnHandleResultsDerived();

	AllocWaitedResultList();
}

template<typename Traits>
void TThreadCommMgr<Traits>::HandleAllResults()
{
	SpinLock(&m_slkResQueue);
	m_lstDoingResult.splice(m_lstDoingResult.end(), m_lstSwapResult);
	SpinUnlock(&m_slkResQueue);

	 for(typename list<ResultAndAlloc*>::iterator it=m_lstDoingResult.begin(); 
		 it != m_lstDoingResult.end(); ++it)
    {
		ResultAndAlloc* pResultAlloc = *it;
	
		DequeResult_t *pQueResult = pResultAlloc->m_queResult;
		for(typename DequeResult_t::iterator it1 = pQueResult->begin(); 
			it1 != pQueResult->end(); ++it1)
		{
			ImpResultType_t* pResult = *it1;
			SQR_TRY
			{		
				pResult->HandleResult();
				pResult->~ImpResultType_t();
			}
			SQR_CATCH(exp)
			{
				static_cast<ImpCommMgr_t*>(this)->OnHandleResultExpDerived(exp);
			}
			SQR_TRY_END;
		}
    }

	while (!m_lstDoingResult.empty())
	{
		ResultAndAlloc* pResultAlloc = m_lstDoingResult.front();
		m_lstDoingResult.pop_front();
		pResultAlloc->m_alloc->release();
	}
}

//----------------------------------------------------------------------

template<typename Traits>
void TThreadCommMgr<Traits>::AllocWaitedJobList()
{
	JobAlloc_t* pJobAlloc = new JobAlloc_t(1024);
	CMemPool* pPool = pJobAlloc->GetMemPool();

	JobAndAlloc* pNewWaitedJob = new (pPool->Alloc(sizeof(JobAndAlloc))) JobAndAlloc;
	pNewWaitedJob->m_alloc = pJobAlloc;
	pNewWaitedJob->m_queJob = new (pPool->Alloc(sizeof(DequeJob_t))) 
		DequeJob_t(*pJobAlloc);

	m_lstWaitedJob.push_back(pNewWaitedJob);
}

template<typename Traits>
void* TThreadCommMgr<Traits>::AllocJobMem(size_t stSize)
{
	return m_lstWaitedJob.back()->m_alloc->AllocMem(stSize);
}

template<typename Traits>
void TThreadCommMgr<Traits>::DeallocJobList()
{
	if(!m_lstWaitedJob.empty())
	{
		JobAndAlloc* pJobAlloc = m_lstWaitedJob.back();
		pJobAlloc->m_alloc->release();	
	}

	if(!m_lstSwapJob.empty())
	{
		JobAndAlloc* pJobAlloc = m_lstSwapJob.back();
		pJobAlloc->m_alloc->release();	
	}
}


template<typename Traits>
void TThreadCommMgr<Traits>::AllocWaitedResultList()
{
	ResultAlloc_t* pResultAlloc = new ResultAlloc_t(1024);
	CMemPool* pPool = pResultAlloc->GetMemPool();

	ResultAndAlloc* pNewWaitedResult = new (pPool->Alloc(sizeof(JobAndAlloc))) ResultAndAlloc;
	pNewWaitedResult->m_alloc = pResultAlloc;
	pNewWaitedResult->m_queResult = new (pPool->Alloc(sizeof(DequeResult_t))) 
		DequeResult_t(*pResultAlloc);

	m_lstWaitedResult.push_back(pNewWaitedResult);
}

template<typename Traits>
void* TThreadCommMgr<Traits>::AllocResultMem(size_t stSize)
{
	return m_lstWaitedResult.back()->m_alloc->AllocMem(stSize);
}

template<typename Traits>
void TThreadCommMgr<Traits>::DeallocResultList()
{
	if(!m_lstWaitedResult.empty())
	{
		ResultAndAlloc* pResultAlloc = m_lstWaitedResult.back();
		pResultAlloc->m_alloc->release();
	}

	if(!m_lstSwapResult.empty())
	{
		ResultAndAlloc* pResultAlloc = m_lstSwapResult.back();
		pResultAlloc->m_alloc->release();
	}
}

template<typename Traits>
inline void TThreadCommMgr<Traits>::AddJob(ImpJobType_t* pJob)
{
	JobAndAlloc* pJobAndAlloc = m_lstWaitedJob.back();
	
    if (pJobAndAlloc->m_queJob->empty())
		static_cast<ImpCommMgr_t*>(this)->OnSpliceJobsDerived();
    
	pJobAndAlloc->m_queJob->push_back(pJob);
}


template<typename Traits>
inline void TThreadCommMgr<Traits>::AddResult(ImpResultType_t* pResult)
{
	ResultAndAlloc* pResultAndAlloc = m_lstWaitedResult.back();
	
	pResultAndAlloc->m_queResult->push_back(pResult);
}


template<typename Traits>
void TThreadCommMgr<Traits>::OnHandleResultsDerived()
{
	GenErr("Must Not Call This");	
}
		
template<typename Traits>
void TThreadCommMgr<Traits>::OnSpliceJobsDerived()
{
	GenErr("Must Not Call This");
}

template<typename Traits>
void TThreadCommMgr<Traits>::OnSwapJobReadyDerived()
{

}

template<typename Tratis>
void TThreadCommMgr<Tratis>::OnDoJobExpDerived(CError& exp)
{
	ErrLog(exp);
}

template<typename Traits>
void TThreadCommMgr<Traits>::OnHandleResultExpDerived(CError& exp)
{
	ErrLogAndTellAll(exp);
}

