#include "stdafx.h"
#include "CAsynPipeHandler.h"
#include "CPipeThreadMgr.h"
#include "CAsynPipe.h"
#include "CSyncPipeReactor.h"
#include "ExpHelper.h"
#include "TSqrAllocator.inl"
#include "CPipeResult.inl"

CAsynPipeHandler::VecOrphanPipeSet_t CAsynPipeHandler::ms_vecOrphanPipeSet;
int32 CAsynPipeHandler::ms_nGlobalID;

CSyncPipe* CAsynPipeHandler::GetPipeFromList(uint32 uThreadId, int32 nPipeId)
{
	CPipeThread* pThread=CPipeThreadMgr::Inst()->GetThread(uThreadId);
	Ast(pThread);
	return pThread->GetPipe( nPipeId );
}

bool CAsynPipeHandler::RemoveWaitedPipeFromList(uint32 uThreadId, IPipe* pPipe)
{
	return ms_vecOrphanPipeSet[uThreadId].erase(pPipe)>0;
}


CAsynPipeHandler::CAsynPipeHandler(int32 nPipeID,bool bFreqDSCallback)
{
	m_uSessionID		= 0;
	m_bAsynInbufFull	= false;
	m_uThreadId			= -1;
	m_bSendBufferFull	= false;
	m_nLocalID			= nPipeID;
	m_bFreqDSCallback	= bFreqDSCallback;
}

CAsynPipeHandler::~CAsynPipeHandler()
{
	while (!m_lstUnsendData.empty())
	{
		UnsendData *pUD = m_lstUnsendData.front();
		delete[] pUD->data;
		delete pUD;
		m_lstUnsendData.pop_front();
	}
	CPipeThread* pThread=CPipeThreadMgr::Inst()->GetThread(m_uThreadId);
	Ast(pThread);
	pThread->DelPipe( m_nLocalID );

	m_pPipe->SetHandler(NULL);
}

void CAsynPipeHandler::IncreSession()
{
	++m_uSessionID;
}

void CAsynPipeHandler::OnConnected()
{
	CSyncPipe* pPipe = GetPipeFromList(m_uThreadId,m_nLocalID);
	Ast( pPipe );

	CAddress LocalAddr, RemoteAddr;
	pPipe->GetLocalAddress(LocalAddr);
	pPipe->GetRemoteAddress(RemoteAddr);
	
	CPipeThread* pThread = CPipeThreadMgr::Inst()->GetThread(m_uThreadId);
	(new (pThread)CPipeConnectSuccResult(m_nLocalID, m_uSessionID, LocalAddr, RemoteAddr, pThread))
		->Add(pThread);
}

void CAsynPipeHandler::OnConnectFailed(EPipeConnFailedReason eReason)
{
	CPipeThread* pThread = CPipeThreadMgr::Inst()->GetThread(m_uThreadId);
	(new (pThread)CPipeConnectFailResult(m_nLocalID, m_uSessionID, eReason))->Add(pThread);
}

void CAsynPipeHandler::OnDisconnected(EPipeDisconnectReason eReason,
									  bool bGracefully)
{
	CPipeThread* pThread= CPipeThreadMgr::Inst()->GetThread(m_uThreadId);
	(new (pThread)CPipeDisconnectResult(m_nLocalID, m_uSessionID, eReason, bGracefully, pThread))->Add(pThread);
	
}

void CAsynPipeHandler::OnThreadSet(int32 nListenPipeId, IPipe* pPipe, uint32 uOldThreadId)
{
	CAddress LocalAddr, RemoteAddr;
	m_pPipe->GetLocalAddress(LocalAddr);
	m_pPipe->GetRemoteAddress(RemoteAddr);

	CPipeThread* pThread = CPipeThreadMgr::Inst()->GetThread(m_uThreadId);
	(new (pThread)CPipeAcceptedResult(nListenPipeId, m_uSessionID, m_nLocalID, 
		m_uThreadId, uOldThreadId, pPipe, LocalAddr, RemoteAddr, pThread))->Add(pThread);
	//这里要强制把res buffer强制交换，然后通知对方处理
	pThread->FlushRightMsg();
}

void CAsynPipeHandler::OnAccepted(IPipe* pPipe)
{
	ms_vecOrphanPipeSet[m_uThreadId].insert(pPipe);

	CPipeThread* pThread = CPipeThreadMgr::Inst()->GetThread(m_uThreadId);
	
	(new (pThread)CPipeSetThreadResult(m_nLocalID, m_uSessionID, pPipe, m_uThreadId,m_bFreqDSCallback))->Add(pThread);
}

void CAsynPipeHandler::OnListenFailed()
{
	CPipeThread* pThread = CPipeThreadMgr::Inst()->GetThread(m_uThreadId);
	(new (pThread)CPipeListenFailedResult(m_nLocalID, m_uSessionID))->Add(pThread);
}

void CAsynPipeHandler::OnListenSuccessed()
{
	CPipeThread* pThread = CPipeThreadMgr::Inst()->GetThread(m_uThreadId);
	(new (pThread)CPipeListenSuccessedResult(m_nLocalID, m_uSessionID))->Add(pThread);
}

void CAsynPipeHandler::OnDataReceived()
{
	//puts("DataReceived");
	if (m_bAsynInbufFull)
		return;

	uint32 uSize = (uint32)m_pPipe->GetRecvDataSize();
	if (uSize == 0)
		return;

	CPipeThread* pThread = CPipeThreadMgr::Inst()->GetThread(m_uThreadId);
	(new (pThread)CPipeNewDataReceivedResult(m_nLocalID, m_uSessionID, (char*)(m_pPipe->GetRecvData()),uSize, pThread))->Add(pThread);

	m_pPipe->PopRecvData(uSize);
}

int CAsynPipeHandler::GetLocalID()const
{
	return m_nLocalID;
}

void CAsynPipeHandler::SetPipe(CSyncPipe* pPipe)
{
	m_pPipe = pPipe;
}

void CAsynPipeHandler::PushUnsendData( char* pData, size_t uSize )
{
	char * pNewData = new char[uSize];
	memcpy(pNewData, pData, uSize);
	UnsendData *ud = new UnsendData;
	ud->data = pNewData;
	ud->size = uSize;
	m_lstUnsendData.push_back(ud);
	CPipeThread* pThread = CPipeThreadMgr::Inst()->GetThread(m_uThreadId);
	(new (pThread)CSendBufferFullResult(GetLocalID(), m_uSessionID, true))->Add(pThread);
}

void CAsynPipeHandler::OnDataSent( uint32 uSentSize )
{
	if( m_bFreqDSCallback )
	{
		CPipeThread* pThread = CPipeThreadMgr::Inst()->GetThread(m_uThreadId);
		(new (pThread)CPipeSomeDateSentResult(m_nLocalID, m_uSessionID, uSentSize))->Add(pThread);
	}

	if( !m_lstUnsendData.empty() )
		PopUnsendData();
}

void CAsynPipeHandler::OnSendBufferFree()
{
}

void CAsynPipeHandler::PopUnsendData()
{
	size_t uLeftEmptySize = m_pPipe->GetSendBufferFreeSize();

	while ( !m_lstUnsendData.empty() && uLeftEmptySize >= m_lstUnsendData.front()->size )
	{
		uLeftEmptySize -= m_lstUnsendData.front()->size;
		UnsendData* ud = m_lstUnsendData.front();
		m_pPipe->Send(ud->data, ud->size);
		delete[] ud->data;
		delete ud;
		m_lstUnsendData.pop_front();
	}

	if( m_lstUnsendData.empty() )
	{
		CPipeThread* pThread = CPipeThreadMgr::Inst()->GetThread(m_uThreadId);
		(new (pThread)CSendBufferFullResult(GetLocalID(), m_uSessionID, false))->Add(pThread);
	}
}

bool CAsynPipeHandler::OutBufIsFull()const
{
	return !m_lstUnsendData.empty();
}

void CAsynPipeHandler::SetAsynInbufFull(bool flag)
{
	m_bAsynInbufFull = flag;	
}

void CAsynPipeHandler::SetThreadId(uint32 uThreadId)
{
	m_uThreadId = uThreadId;
}

uint32 CAsynPipeHandler::GetThreadId()const
{
	return m_uThreadId;
}
