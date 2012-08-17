#include "stdafx.h"
#include "CMemJob.h"
#include <vector>
#include "CMemCallStackInfo.h"
#include "CStackFrame.h"
#include "CStackFrameMgr.h"
#include "CMemCallStackInfoMgr.h"
#include "ExpHelper.h"
#include "CMemoryCookie.h"
#include "CMemThreadMgr.h"
#include "CMemCallStackInfoSet.h"
#include "MemoryHelper.h"
#include "CSqrAllocPool.h"
#include "CMemThread.h"

CMemMallocJob::CMemMallocJob(CMemoryCookie* pCookie)
:m_pCookie( pCookie )
{

}

CMemMallocJob::~CMemMallocJob()
{
	CMemCallStackAddrVector* pVecAddr=m_pCookie->m_pStackInfo->TakeAddrVec();
	CMemCallStackInfo* pStackInfo=m_pCookie->m_pStackInfo;

	const size_t stSize = pVecAddr->size();

	pStackInfo->reserve(stSize);

	for (uint32 i = 0; i < stSize; i++)
		pStackInfo->push_back(CStackFrameMgr::Inst().AddFrame(pVecAddr->at(i)));

	CMemCallStackInfoMgr::Inst().AddStack(m_pCookie->m_pStackInfo, m_pCookie->m_stSize );
}


CMemFreeJob::CMemFreeJob(CMemoryCookie* pCookie, CSqrAllocPool* pPool)
{
	m_pCookie =pCookie;
	m_pPool = pPool;
}

CMemFreeJob::~CMemFreeJob()
{
	CMemCallStackInfoMgr::Inst().DelStack(m_pCookie->m_pStackInfo, m_pCookie->m_stSize);
	m_pPool->FreeFromMemThread( m_pCookie );
}

#ifndef _WIN32

CMemFetchAllSymbolJob::CMemFetchAllSymbolJob()
{

}

CMemFetchAllSymbolJob::~CMemFetchAllSymbolJob()
{
	CStackFrameMgr::Inst().FetchAllSymbol();
	CMemThreadMgr::Inst()->SetFetchAllSymbolEnded();
}

#endif



CMemDumpStackJob::~CMemDumpStackJob()
{
	CMemCallStackInfoMgr::Inst().Print();
}


CMemEndThreadsJob ::CMemEndThreadsJob (CMemThread *pThread)
{
	m_pMemThread = pThread;
}

CMemEndThreadsJob::~CMemEndThreadsJob()
{
	m_pMemThread->m_bQuit = true;
}
