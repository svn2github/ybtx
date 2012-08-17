#include "stdafx.h"
#include "CMemThreadMgr.h"
#include "CMemJob.h"
#include "CMemThread.h"
#include "ThreadHelper.h"
#include "ExpHelper.h"
#include "CMemoryCookie.h"
#include <climits>
#include "ErrLogHelper.h"
#include "CJmpStack.h"

//请查看CMemoryLeakageMgr.h里面的说明内容

static CMemThreadMgr* gs_pInst=NULL;

CMemThreadMgr::CMemThreadMgr()
{
	m_bEndThreads=false;
	m_bHandleJobDone=true;
	m_pThread=NULL;

	CreateSemaphore(&m_smJobQueue,0,INT_MAX);
	CreateSemaphore(&m_smFetchSymbol, 0, INT_MAX);

	CreateLock(&m_slkJobQueue);
	
	m_pThread = new CMemThread(this);

	Ast(! gs_pInst);
	gs_pInst=this;
}

CMemThreadMgr::~CMemThreadMgr()
{
	gs_pInst=NULL;

	if( !m_pThread )
		return;

	AddEndThreadJob();

	delete m_pThread;
	m_pThread=NULL;

	DestroySemaphore(&m_smJobQueue);
	DestroySemaphore(&m_smFetchSymbol);
	DestroyLock(&m_slkJobQueue);
}

CMemThreadMgr* CMemThreadMgr::Inst()
{
	return gs_pInst;
}

void CMemThreadMgr::HandleAllJobs()
{
	ListMemJob lstMemJob;

	Lock(&m_slkJobQueue);
	lstMemJob.splice(lstMemJob.end(), m_lstMemDoingJob);
	Unlock(&m_slkJobQueue);

	while (!lstMemJob.empty())
	{
		ListMemJob::iterator it = lstMemJob.begin();

		CMemJob *pJob = (*it);
		lstMemJob.erase(it);		
		
		SQR_TRY
		{
			delete pJob;
		}
		SQR_CATCH(exp)
		{
			LogExp(exp);
		}
		SQR_TRY_END;
	}

	Lock(&m_slkJobQueue);
	m_bHandleJobDone = true;
	Unlock(&m_slkJobQueue);
}

void CMemThreadMgr::AddDelayedMemJob(CMemJob* pJob)
{
	Lock(&m_slkJobQueue);
	m_lstMemDoingJob.push_back(pJob);
	if (m_bHandleJobDone)
	{
		//有了m_bHandleJobDone之后，唤醒MemThread的次数被大大减少
		m_bHandleJobDone = false;
		PutSemaphore(&m_smJobQueue);
	}
	Unlock(&m_slkJobQueue);
}

void CMemThreadMgr::AddMemMallocJob(CMemoryCookie* pCookie)
{
	AddDelayedMemJob(new CMemMallocJob(pCookie));
}

void CMemThreadMgr::AddMemFreeJob(CMemoryCookie* pCookie, CSqrAllocPool* pPool)
{
	AddDelayedMemJob(new CMemFreeJob(pCookie, pPool));
}

void CMemThreadMgr::AddDumpStackJob()
{
	AddDelayedMemJob(new CMemDumpStackJob);
}

void CMemThreadMgr::AddEndThreadJob()
{
	//这里不走m_bHandleJobDone判断，会立即唤醒MemoryThread
	Lock(&m_slkJobQueue);
	m_lstMemDoingJob.push_back(new CMemEndThreadsJob(m_pThread));
	PutSemaphore(&m_smJobQueue);
	Unlock(&m_slkJobQueue);	
}


#ifndef _WIN32
void CMemThreadMgr::AddFetchAllSymbolJob()
{
	//这里不走m_bHandleJobDone判断，会立即唤醒MemoryThread
	Lock(&m_slkJobQueue);
	m_lstMemDoingJob.push_back(new CMemFetchAllSymbolJob());
	PutSemaphore(&m_smJobQueue);
	Unlock(&m_slkJobQueue);	
}

void CMemThreadMgr::WaitFetchAllSymbolEnded()
{
	switch( GetSemaphore( &m_smFetchSymbol) )
	{
	case 0:
		break;
	default:
		{
			ostringstream strm;
			strm<<"GetSemaphore failed with error \""<<strerror(errno)<<"\"";
			GenErr(strm.str());
		}
	}
}

void CMemThreadMgr::SetFetchAllSymbolEnded()
{
	PutSemaphore(&m_smFetchSymbol);
}

#endif

