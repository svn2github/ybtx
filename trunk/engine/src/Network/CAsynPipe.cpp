#include "stdafx.h"
#include "CAsynPipe.h"
#include "CAsynPipeInBuffer.h"
#include "CAsynPipeHandler.h"
#include "CAsynPipeReactor.h"
#include "LogHelper.h"
#include "CPipeCallback.h"
#include "ExpHelper.h"
#include "BaseHelper.h"
#include "TimeHelper.h"
#include "CAsynMulticast.h"
#include "CPipeThreadMgr.h"
#include "CIdMaker.h"
#include "TSqrAllocator.inl"
#include "CPipeJob.inl"

CAsynPipe::VecAsynPipe_t CAsynPipe::m_vecAsynPipe;
IdMaker* CAsynPipe::m_idMaker;
const uint32 CAsynPipe::M_SBLOCK=128;
const uint8 CAsynPipe::MAX_STATISTIC_DATAFLOW_TIME=5;

void CAsynPipe::InitStaticInfo()
{
	m_vecAsynPipe.resize(M_SBLOCK);
	m_idMaker = new IdMaker(M_SBLOCK);
}

void CAsynPipe::ReleaesIdMaker()
{
	delete m_idMaker;
}

CAsynPipe* CAsynPipe::GetAsynPipe(uint32 id)
{
	Ast( id < m_vecAsynPipe.size() );
	return m_vecAsynPipe[id];
}

uint32 CAsynPipe::GetGlobalId()
{
	uint32 id = 0;
	bool flag = m_idMaker->GetId(id);
	
	if (!flag)
	{
		m_idMaker->Resize(m_idMaker->GetCurrentMax() + M_SBLOCK);
		m_vecAsynPipe.resize(m_idMaker->GetCurrentMax() + M_SBLOCK);
		m_idMaker->GetId(id);
	}

	return id;
}

CAsynPipe::CAsynPipe(IPipeHandler* pHandler,size_t stOutBufferSize,size_t stInBufferSize, int32 nPipeId)
:m_InBuffer(stInBufferSize)	
{
	m_eState				=eAPS_Disconnected;
	m_pHandler				=NULL;
	m_pReactor				=NULL;
	m_stInBufferSize		=stInBufferSize;
	m_stOutBufferSize		=stOutBufferSize;
	m_uSessionID			=0;
	m_bSendBufferFull		=false;
	m_uThreadId				=0;
	
	IntSetHandler(pHandler);

	if ( nPipeId )
		m_uID = nPipeId;
	else
		m_uID = GetGlobalId();

	m_vecAsynPipe[m_uID] = this;
}

CAsynPipe::~CAsynPipe()
{
	IntSetHandler(NULL);

	if(m_pReactor)
	{
		m_pReactor->Unregister(this);
	}

	m_vecAsynPipe[m_uID] = NULL;
	m_idMaker->ReleaseId(m_uID);
}


void CAsynPipe::SetHandler(IPipeHandler* pHandler)
{
	IntSetHandler( pHandler );
	if(!m_pHandler )
		Release();
}

void CAsynPipe::IntSetHandler(IPipeHandler* pHandler)
{
	AdvDelRef(m_pHandler);
	m_pHandler=pHandler;
	AdvAddRef(m_pHandler);
}

void CAsynPipe::CopyDataFromSyncPipe(char* pData, uint32 uDataSize)
{
	const bool bWasFull=m_InBuffer.IsFull();

	m_InBuffer.InBufferPushData( pData,uDataSize );

	if( bWasFull )
		return;

	if( !m_InBuffer.IsFull() )
		return;

	if( !IsConnected() )
		return;

	CPipeThread* pThread = CPipeThreadMgr::Inst()->GetThread(GetThreadId());
	(new(pThread) CPipeInbufFullJob(m_uID,true, pThread))->Add(pThread);
}

int32 CAsynPipe::GetLocalID()const
{
	return m_uID;
}


void CAsynPipe::IncreSessionID()
{
	++m_uSessionID;
}

IPipeHandler* CAsynPipe::GetHandler()const
{
	return m_pHandler;
}

IPipeReactor* CAsynPipe::GetReactor()const
{
	return m_pReactor;
}

bool CAsynPipe::IsConnected()const
{
	return m_eState == eAPS_Connected;
}

bool CAsynPipe::IsListening()const
{
	return m_eState == eAPS_Listening;
}

bool CAsynPipe::IsConnecting()const
{
	return m_eState == eAPS_Connecting;
}

bool CAsynPipe::Listen(const CAddress& Address)
{
	m_eState = eAPS_Listening;

	CPipeThread* pThread = CPipeThreadMgr::Inst()->GetThread(GetThreadId());

	(new(pThread) CPipeListenJob(GetLocalID(), Address, pThread)) -> Add(pThread);

	return true;
}

bool CAsynPipe::Connect(const CAddress& Address)
{
	m_eState = eAPS_Connecting;
	CPipeThread* pThread = CPipeThreadMgr::Inst()->GetThread(GetThreadId());
	(new(pThread) CPipeConnectJob(GetLocalID(), Address, pThread))->Add(pThread);
	return true;
}

void CAsynPipe::ShutDown()
{
	ShutDown(false);
}

void CAsynPipe::ShutDown(bool bNeedCallback)
{
	DelFromAllMulticast();
	m_eState = eAPS_Disconnected;
	m_InBuffer.InBufferClear();
	IncreSessionID();

	CPipeThread* pThread = CPipeThreadMgr::Inst()->GetThread(GetThreadId());
	(new(pThread) CPipeShutdownJob(GetLocalID(), bNeedCallback, pThread))->Add(pThread);
}

size_t CAsynPipe::Send(const void* pData,size_t stSize)
{
	if ( m_bSendBufferFull || (stSize == 0) )
		return 0;

	Ast(m_pReactor);

	if (!IsConnected())
		return 0;

	const char* pByteData = reinterpret_cast<const char*>(pData);
	
	CPipeThread* pThread = CPipeThreadMgr::Inst()->GetThread(GetThreadId());
	(new(pThread) CPipeSendDataJob(GetLocalID(), pByteData, (uint32)stSize, pThread))->Add(pThread);

	return stSize;
}

size_t CAsynPipe::GetSendBufferSize()const
{
	return m_stOutBufferSize;
}

void* CAsynPipe::GetRecvData()const
{
	return m_InBuffer.InBufferGetData();
}

size_t CAsynPipe::GetRecvDataSize()const
{
	return m_InBuffer.InBufferGetDataSize();
}

void CAsynPipe::PopRecvData(size_t stSize)
{
	const bool bWasFull = m_InBuffer.IsFull();

	m_InBuffer.InBufferPopData(stSize);

	if( !bWasFull )
		return;

	CPipeThread* pThread = CPipeThreadMgr::Inst()->GetThread(GetThreadId());
	(new(pThread) CPipeInbufFullJob(m_uID,false, pThread))->Add(pThread);
}

void CAsynPipe::ClearRecvBuffer()
{
	m_InBuffer.InBufferClear();
}

bool CAsynPipe::RecvBufferFull()const
{
	return m_InBuffer.IsFull();
}

size_t CAsynPipe::GetRecvBufferSize()const
{
	return m_stInBufferSize;
}

bool CAsynPipe::GetLocalAddress(CAddress& Address)const
{
	if (m_eState != eAPS_Connected)
		return false;

	Address = m_LocalAddr;
	return true;
}

bool CAsynPipe::GetRemoteAddress(CAddress& Address)const
{
	if (m_eState != eAPS_Connected)
		return false;

	Address = m_RemoteAddr;	
	return true;
}

void CAsynPipe::Release()
{
	m_eState = eAPS_Disconnected;
	DelFromAllMulticast();

	if( m_pReactor )
	{
		CPipeThread* pThread = CPipeThreadMgr::Inst()->GetThread(GetThreadId());
		VerGt( pThread->m_stPipeNum--,0 );
		(new(pThread) CPipeReleaseJob(GetLocalID(), pThread))->Add(pThread);
	}
	delete this;
}

void CAsynPipe::SetLocalAddr(const CAddress& LocalAddr)
{
	m_LocalAddr = LocalAddr;
}

void CAsynPipe::SetRemoteAddr(const CAddress& RemoteAddr)
{
	m_RemoteAddr = RemoteAddr;
}

bool CAsynPipe::DelFrMulticast(IMulticast* pMulticast)
{
	SetWithinMulticast_t::iterator it = m_setWithinMulticast.find(pMulticast);
	if(it == m_setWithinMulticast.end())
		return false;

	m_setWithinMulticast.erase(it);
	return true;
}

bool CAsynPipe::AddToMulticast(IMulticast* pMulticast)
{
	const pair<SetWithinMulticast_t::iterator, bool> p = m_setWithinMulticast.insert(pMulticast);
	return p.second;
}

void CAsynPipe::DelFromAllMulticast()
{
	for (SetWithinMulticast_t::iterator it = m_setWithinMulticast.begin();
		it != m_setWithinMulticast.end();)
	{
		(*(it++))->DelPipe(this);
	}
	m_setWithinMulticast.clear();

	if( m_pReactor )
	{
		CPipeThread* pThread = CPipeThreadMgr::Inst()->GetThread(GetThreadId());	
		(new(pThread) CPipeDelFromAllMulticastJob(GetLocalID(), pThread))->Add(pThread);
	}
}

void CAsynPipe::SetSendBufferFull(bool bFlag)
{
	m_bSendBufferFull = bFlag;
}

void CAsynPipe::SetThreadId(uint32 uThreadId)
{
	m_uThreadId = uThreadId;
}

uint32 CAsynPipe::GetThreadId()const
{
	return m_uThreadId;
}
