#include "stdafx.h"
#include "CPipeThread.h"
#include "CPipeThreadMgr.h"
#include "ThreadHelper.h"
#include "CPipeJob.inl"
#include "CPipeResult.inl"
#include "ExpHelper.h"
#include "TimeHelper.h"
#include "CSyncPipe.h"
#include "CAsynPipe.h"
#include "CAsynMulticast.h"
#include "CSyncPipeReactor.h"
#include "CAsynPipeReactor.h"
#include "ErrLogHelper.h"
#include "CPipeTrMsgBufferSwapper.h"
#include "TTrThreadMsgBuffer.inl"
#include "TSqrAllocator.inl"


uint32 CPipeThreadMgr::GetIdleThreadID()
{
	//—∞’“ø’œ–œﬂ≥Ã
	size_t	uThreadIndexLower = 0;
	size_t	uMinNum = INT_MAX;

	size_t stSize = m_vecThread.size();
	for (size_t i = 0; i < stSize; ++i)
	{
		size_t uPipeNum =m_vecThread[i]->m_stPipeNum;

		if (uPipeNum < uMinNum)
		{
			uThreadIndexLower = i;
			uMinNum = uPipeNum;
		}
	}

	//transfer reactor
	uint32 realThreadId = (uint32)uThreadIndexLower;

	return realThreadId;
}



void CPipeThreadMgr::CreatePipeThreads(uint32 num)
{
	m_uExecThreadNum = num;
	m_vecThread.resize(num);
	CAsynMulticast::InitStaticInfo(num);
	CAsynPipe::InitStaticInfo();
	CAsynPipeHandler::ms_vecOrphanPipeSet.resize(num);
	for (size_t uIndex = 0; uIndex < num; ++uIndex)
	{
		CSyncPipeReactor* pReactor = new CSyncPipeReactor( true );
		m_vecThread[uIndex] = new CPipeThread(this, pReactor, (uint32)uIndex);
	}
}

void CPipeThreadMgr::ReleaseThread()
{
	CAsynMulticast::ReleaseIdMaker();
	CAsynPipe::ReleaesIdMaker();

	size_t stSize = m_vecThread.size();
	for (size_t uIndex=0; uIndex < stSize; ++uIndex)
	{
		delete m_vecThread[uIndex];

		while (!CAsynPipeHandler::ms_vecOrphanPipeSet[uIndex].empty())
		{
			CAsynPipeHandler::OrphanPipeSet_t::iterator it = CAsynPipeHandler::ms_vecOrphanPipeSet[uIndex].begin();
			CAsynPipeHandler::ms_vecOrphanPipeSet[uIndex].erase(it);
			(*it)->Release();
		}
	}
}

CPipeThreadMgr::CPipeThreadMgr(uint32 num, CAsynPipeReactor* pReactor)
:m_bEndThreads(false), m_pAsynPipeReactor(pReactor)
{
	new CPipeTrMsgBufferSwapper;
	CreatePipeThreads(num);
}

CPipeThreadMgr::~CPipeThreadMgr()
{
	ReleaseThread();

	delete CPipeTrMsgBufferSwapper::Inst();
}


inline void CPipeThreadMgr::AddPipeResult(CPipeResult* pResult, CPipeThread* pThread)
{
	pThread->AddRightMsg(pResult);
}

CPipeThreadMgr* CPipeThreadMgr::ms_pInst = NULL;

CPipeThreadMgr* CPipeThreadMgr::Inst()
{
	return ms_pInst;
}

void CPipeThreadMgr::Init(uint32 num, CAsynPipeReactor* pPipeReactor)
{
	Ast(!ms_pInst);
	ms_pInst=new CPipeThreadMgr(num, pPipeReactor);
}

void CPipeThreadMgr::Unit()
{
	delete ms_pInst;
	ms_pInst=NULL;
}


void CPipeThreadMgr::ReactorCancelBlock()
{
	m_pAsynPipeReactor->CancelBlock();
}


CSyncPipeReactor* CPipeThreadMgr::GetSyncPipeReactor(uint32 uIndex)const
{
	return m_vecThread[uIndex]->m_pReactor;
}

void CPipeThreadMgr::Halt()
{
	size_t stSize = m_vecThread.size();
	for(size_t i = 0; i < stSize; ++i)
	{
		CPipeThread* pPool = m_vecThread[i];
		(new (pPool)CPipeEndThreadJob(pPool))->Add(pPool);
	}
}
	
bool CPipeThreadMgr::Halted()
{
	return m_uExecThreadNum == 0;
}

void CPipeThreadMgr::OnHalted()
{
	--m_uExecThreadNum;
	Ast(m_uExecThreadNum >= 0);
}
