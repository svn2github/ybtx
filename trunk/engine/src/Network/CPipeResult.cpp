#include "stdafx.h"
#include "CPipeResult.inl"
#include "CAsynPipe.h"
#include "CSyncPipe.h"
#include "LogHelper.h"
#include "CPipeThread.h"
#include "CPipeThreadMgr.h"
#include "CAsynPipeHandler.h"
#include "CAsynPipeReactor.h"
#include "CAsynMulticast.h"
#include "ExpHelper.h"
#include "CPipeJob.inl"

CPipeResult::CPipeResult(int32 uPipeID, uint32 uSessionID)
{
	m_uPipeId = uPipeID;
	m_uSessionID = uSessionID;
}

CPipeNewDataReceivedResult::CPipeNewDataReceivedResult(int32 nPipeId,uint32 uSessionID,const char* pData, uint32 uDataSize, CPipeThread* pThread)
: CPipeResult(nPipeId, uSessionID)
{
	m_pData = CloneData(pData, uDataSize, pThread);
	m_uRecvDataSize = uDataSize;
}

CPipeNewDataReceivedResult::~CPipeNewDataReceivedResult()
{
	CAsynPipe* pAsyPipe = CAsynPipe::GetAsynPipe(m_uPipeId);
	if (!pAsyPipe || (pAsyPipe->GetSessionID() != m_uSessionID) )
		return;

	//从Sync的pipe里面读取多长
	pAsyPipe->CopyDataFromSyncPipe((char*)m_pData, m_uRecvDataSize);

	pAsyPipe->GetHandler()->OnDataReceived();
}

CPipeAcceptedResult::CPipeAcceptedResult(int32 nPipeId,uint32 uSessionID, int32 newPipeId, uint32 threadId, 
										 uint32 oldthreadid, IPipe* pPipe, const CAddress& LocalAddr, const CAddress& RemoteAddr, CPipeThread* pThread)
										 :CPipeResult(nPipeId, uSessionID)
{
	m_szLocalAddr = CloneString( LocalAddr.GetAddress(), pThread );
	m_uLocalPort = LocalAddr.GetPort();

	m_szRemoteAddr = CloneString( RemoteAddr.GetAddress(), pThread );
	m_uRemotePort = RemoteAddr.GetPort();

	m_pPipe = pPipe;
	m_uOldThreadId = oldthreadid;
	m_nNewPipeID = newPipeId;
	m_uThreadId = threadId;
}

CPipeAcceptedResult::~CPipeAcceptedResult()
{
	CAsynPipe* pAsyPipe = CAsynPipe::GetAsynPipe(m_uPipeId);
	if (!pAsyPipe || (pAsyPipe->GetSessionID() != m_uSessionID))
		return;

	if (!pAsyPipe->IsListening())
		return;

	CAsynPipe* pNewAsynPipe = new CAsynPipe( NULL, pAsyPipe->GetSendBufferSize(), pAsyPipe->GetRecvBufferSize(), m_nNewPipeID );

	CAsynPipeReactor* pReactor=class_cast<CAsynPipeReactor*>(pAsyPipe->GetReactor());

	pReactor->Register(pNewAsynPipe);

	pNewAsynPipe->SetThreadId(m_uThreadId);

	CAddress LocalAddr;
	LocalAddr.SetAddress(m_szLocalAddr);
	LocalAddr.SetPort(m_uLocalPort);

	CAddress RemoteAddr;
	RemoteAddr.SetAddress(m_szRemoteAddr);
	RemoteAddr.SetPort(m_uRemotePort);

	pNewAsynPipe->SetLocalAddr(LocalAddr);
	pNewAsynPipe->SetRemoteAddr(RemoteAddr);

	pAsyPipe->GetHandler()->OnAccepted(pNewAsynPipe);

	if( !pNewAsynPipe->GetHandler() )
	{
		pReactor->Unregister( pNewAsynPipe );

		CPipeThread* pThread = CPipeThreadMgr::Inst()->GetThread(m_uThreadId);
		(new(pThread) CPipeReleaseJob(pNewAsynPipe->GetLocalID(), pThread))->Add(pThread);
		pNewAsynPipe->Release();		
	}

	CPipeThread* pOldThread = CPipeThreadMgr::Inst()->GetThread(m_uOldThreadId);
	(new(pOldThread) CRemoveWaitedPipeFromListJob(m_pPipe, pOldThread))->Add(pOldThread);
}

CPipeConnectSuccResult::CPipeConnectSuccResult(
	int32 nPipeId, uint32 uSessionID, 
	const CAddress& LocalAddr, 
	const CAddress& RemoteAddr, CPipeThread* pThread)
	: CPipeResult(nPipeId, uSessionID)
{
	m_szLocalAddr = CloneString(LocalAddr.GetAddress(), pThread);
	m_uLocalPort = LocalAddr.GetPort();

	m_szRemoteAddr = CloneString(RemoteAddr.GetAddress(), pThread);
	m_uRemotePort = RemoteAddr.GetPort();
}

CPipeConnectSuccResult::~CPipeConnectSuccResult()
{
	CAsynPipe* pAsyPipe = CAsynPipe::GetAsynPipe(m_uPipeId);
	if (!pAsyPipe || (pAsyPipe->GetSessionID() != m_uSessionID))
		return;

	CAddress LocalAddr;
	LocalAddr.SetAddress(m_szLocalAddr);
	LocalAddr.SetPort(m_uLocalPort);

	CAddress RemoteAddr;
	RemoteAddr.SetAddress(m_szRemoteAddr);
	RemoteAddr.SetPort(m_uRemotePort);

	pAsyPipe->SetLocalAddr(LocalAddr);
	pAsyPipe->SetRemoteAddr(RemoteAddr);
	pAsyPipe->m_eState=eAPS_Connected;

	pAsyPipe->GetHandler()->OnConnected();
}

CPipeConnectFailResult::CPipeConnectFailResult(int32 nPipeId, uint32 uSessionID, 
											   EPipeConnFailedReason eReason)
											   : CPipeResult(nPipeId, uSessionID)
{
	m_eReason = eReason;
}

CPipeConnectFailResult::~CPipeConnectFailResult()
{
	CAsynPipe* pAsyPipe = CAsynPipe::GetAsynPipe(m_uPipeId);
	if (!pAsyPipe || (pAsyPipe->GetSessionID() != m_uSessionID))
		return;

	pAsyPipe->m_eState=eAPS_Disconnected;

	pAsyPipe->GetHandler()->OnConnectFailed( m_eReason );
}

CPipeSomeDateSentResult::CPipeSomeDateSentResult(int32 nPipeId, uint32 uSessionID, uint32 uSentSize)
: CPipeResult(nPipeId, uSessionID)
,m_uSentSize(uSentSize)
{

}

CPipeSomeDateSentResult::~CPipeSomeDateSentResult()
{
	CAsynPipe* pAsyPipe = CAsynPipe::GetAsynPipe(m_uPipeId);
	if (!pAsyPipe || (pAsyPipe->GetSessionID() != m_uSessionID))
		return;

	pAsyPipe->GetHandler()->OnDataSent(m_uSentSize);
}


CPipeDisconnectResult::CPipeDisconnectResult(int32 nPipeId, uint32 uSessionID, 
											 EPipeDisconnectReason eReason, bool bGracefully,
											 CPipeThread* pThread)
											 : CPipeResult(nPipeId, uSessionID)
											 , m_eReason(eReason), m_bGracefully(bGracefully)
{
}

CPipeDisconnectResult::~CPipeDisconnectResult()
{
	CAsynPipe* pAsyPipe = CAsynPipe::GetAsynPipe(m_uPipeId);
	if (!pAsyPipe || (pAsyPipe->GetSessionID() != m_uSessionID))
		return;

	pAsyPipe->m_eState=eAPS_Disconnected;

	pAsyPipe->GetHandler()->OnDisconnected(m_eReason,m_bGracefully);

	pAsyPipe->IncreSessionID();
}

CSendBufferFullResult::CSendBufferFullResult(int32 nPipeId, uint32 uSessionID, bool bFlag)
: CPipeResult(nPipeId, uSessionID)
{
	m_bFlag = bFlag;
}

CSendBufferFullResult::~CSendBufferFullResult()
{
	CAsynPipe* pAsyPipe = CAsynPipe::GetAsynPipe(m_uPipeId);
	if (!pAsyPipe || (pAsyPipe->GetSessionID() != m_uSessionID))
		return;

	pAsyPipe->SetSendBufferFull(m_bFlag);
	
	if( !m_bFlag )
		pAsyPipe->GetHandler()->OnSendBufferFree();	
}

CPipeListenFailedResult::CPipeListenFailedResult(int32 nPipeId, uint32 uSessionID)
: CPipeResult(nPipeId, uSessionID)
{

}


CPipeListenFailedResult::~CPipeListenFailedResult()
{
	CAsynPipe* pAsyPipe = CAsynPipe::GetAsynPipe(m_uPipeId);
	if (!pAsyPipe || (pAsyPipe->GetSessionID() != m_uSessionID))
		return;

	pAsyPipe->m_eState=eAPS_Disconnected;
	pAsyPipe->GetHandler()->OnListenFailed();
}

CPipeListenSuccessedResult::CPipeListenSuccessedResult(int32 nPipeId, uint32 uSessionID)
: CPipeResult(nPipeId, uSessionID)
{

}


CPipeListenSuccessedResult::~CPipeListenSuccessedResult()
{
	CAsynPipe* pAsyPipe = CAsynPipe::GetAsynPipe(m_uPipeId);
	if (!pAsyPipe || (pAsyPipe->GetSessionID() != m_uSessionID))
		return;

	pAsyPipe->m_eState=eAPS_Listening;
	pAsyPipe->GetHandler()->OnListenSuccessed();
}


////
CPipeSetThreadResult::CPipeSetThreadResult(int32 nPipeId, uint32 uSessionId, IPipe* pPipe, uint32 uOldThreadID,bool bFreqDSCallback)
: CPipeResult(nPipeId, uSessionId)
{
	m_pPipe = pPipe;
	m_uOldThreadId = uOldThreadID;
	m_bFreqDSCallback = bFreqDSCallback;
}

CPipeSetThreadResult::~CPipeSetThreadResult()
{
	CAsynPipe* pAsyPipe = CAsynPipe::GetAsynPipe(m_uPipeId);
	if ( !pAsyPipe || ( pAsyPipe->GetSessionID() != m_uSessionID ) )
		return;

	uint32 uThreadId = CPipeThreadMgr::Inst()->GetIdleThreadID();
	CPipeThread* pThread = CPipeThreadMgr::Inst()->GetThread(uThreadId);

	++pThread->m_stPipeNum;

	(new(pThread) CPipeSetThreadJob(m_uOldThreadId, m_uPipeId, m_pPipe, pThread,m_bFreqDSCallback ))->Add(pThread);
}



CPipeThreadEndedResult::CPipeThreadEndedResult()
: CPipeResult(0, 0)
{

}

CPipeThreadEndedResult::~CPipeThreadEndedResult()
{
	CPipeThreadMgr::Inst()->OnHalted();
}
