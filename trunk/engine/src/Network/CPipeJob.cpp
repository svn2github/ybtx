#include "stdafx.h"
#include "CAsynPipe.h"
#include "CPipeJob.inl"
#include "CMulticast.h"
#include "CAsynMulticast.h"
#include "CAsynPipeHandler.h"
#include "CSyncPipeReactor.h"
#include <fstream>
#include "CPipeThreadMgr.h"
#include "CPipeThread.h"
#include "CPipeModule.h"
#include "ExpHelper.h"
#include "ErrLogHelper.h"
#include "TSqrAllocator.inl"
#include "CPipeResult.h"
#include "TTrThreadMsgBuffer.inl"

CPipeJob::CPipeJob(CPipeThread* pThread)
{
	m_uThreadId = pThread->m_uThreadId;
	m_pReactor = pThread->m_pReactor;
}

CPipeJob::CPipeJob(int32 nPipeId, CPipeThread* pThread)
{
	m_nPipeId = nPipeId;
	m_uThreadId = pThread->m_uThreadId;
	m_pReactor = pThread->m_pReactor;

	pThread->AddLeftMsg(this);
}

//CPipeSendDataJob
CPipeSendDataJob::CPipeSendDataJob(int32 nPipeId, const char* pData, uint32 size, CPipeThread* pThread)
: CPipeJob(nPipeId, pThread)
{
	m_pSendData = (char*)CloneData( pData, size, pThread );
	m_uSize = size;
}


CPipeSendDataJob::~CPipeSendDataJob()
{
	CSyncPipe* pPipe= CAsynPipeHandler::GetPipeFromList(m_uThreadId, m_nPipeId);
	if(!pPipe)
		return;

	CAsynPipeHandler* pHandler = class_cast<CAsynPipeHandler*>(pPipe->GetHandler());
	
	if ( pPipe->GetSendBufferFreeSize() < m_uSize )
	{
		pHandler->PushUnsendData(m_pSendData, m_uSize);
		return;
	}
	pPipe->Send(m_pSendData, m_uSize);
}



//CPipeConnectJob
CPipeConnectJob::CPipeConnectJob(int32 nPipeId, const CAddress& address, CPipeThread* pThread)
: CPipeJob(nPipeId, pThread)
{
	m_szAddress = CloneString(address.GetAddress(), pThread);
	m_uPort = address.GetPort();
}

CPipeConnectJob::~CPipeConnectJob()
{
	CSyncPipe* pPipe= CAsynPipeHandler::GetPipeFromList(m_uThreadId, m_nPipeId);
	if( !pPipe )
		return;

	CAddress addr;
	addr.SetAddress(m_szAddress);
	addr.SetPort(m_uPort);
	pPipe->Connect(addr);
}



//CPipeListenJob
CPipeListenJob::CPipeListenJob(int32 nPipeId, const CAddress& address, CPipeThread* pThread)
: CPipeJob(nPipeId, pThread)
{
	m_szAddress = CloneString(address.GetAddress(), pThread);
	m_uPort = address.GetPort();
}

CPipeListenJob::~CPipeListenJob()
{
	CSyncPipe* pPipe= CAsynPipeHandler::GetPipeFromList(m_uThreadId, m_nPipeId);
	if( !pPipe )
		return;

	CAddress addr;
	addr.SetAddress(m_szAddress);
	addr.SetPort(m_uPort);

	pPipe->Listen(addr);
}



//CPipeShutdownJob
CPipeShutdownJob::CPipeShutdownJob(int32 nPipeId, bool bNeedCallback, CPipeThread* pThread)
: CPipeJob(nPipeId, pThread)
{
	m_bNeedCallback = bNeedCallback;
}

CPipeShutdownJob::~CPipeShutdownJob()
{
	CSyncPipe* pPipe = CAsynPipeHandler::GetPipeFromList(m_uThreadId, m_nPipeId);
	if( !pPipe )
		return;

	class_cast<CAsynPipeHandler*>(pPipe->GetHandler())->IncreSession();
	pPipe->ShutDown(m_bNeedCallback);
}


//CPipeCreateJob
CPipeCreateJob::CPipeCreateJob(int32 nPipeId, uint8 uEncodingFlag, CPipeThread* pThread,uint32 uSendBufferSize,uint32 uRecvBufferSize)
: CPipeJob(nPipeId, pThread)
{
	m_uCreationFlag = uEncodingFlag;
	m_uSendBufferSize = uSendBufferSize;
	m_uRecvBufferSize = uRecvBufferSize;
}

CPipeCreateJob::~CPipeCreateJob()
{
	Ast( !CAsynPipeHandler::GetPipeFromList(m_uThreadId, m_nPipeId) );

	CAsynPipeHandler* pHandler = new CAsynPipeHandler(m_nPipeId
		,(m_uCreationFlag & ePCF_DataSentCallback) != 0 );

	IPipe* pIPipe=m_pReactor->
		CreateIPipe(pHandler, m_uSendBufferSize, m_uRecvBufferSize, m_uCreationFlag | ePCF_DataSentCallback);

	CSyncPipe* pPipe = class_cast<CSyncPipe*>(pIPipe);
	pHandler->SetPipe(pPipe);
	pHandler->SetThreadId(m_uThreadId);
	
	CPipeThread* pThread=CPipeThreadMgr::Inst()->GetThread(m_uThreadId);
	Ast(pThread);
	pThread->AddPipe( m_nPipeId,pPipe );
}



//CPipeReleaseJob
CPipeReleaseJob::CPipeReleaseJob(int32 nPipeId, CPipeThread* pThread)
: CPipeJob(nPipeId, pThread)
{

}

CPipeReleaseJob::~CPipeReleaseJob()
{
	CSyncPipe* pPipe = CAsynPipeHandler::GetPipeFromList(m_uThreadId, m_nPipeId);

	Ast(pPipe);

	CAsynPipeHandler* pHandler = class_cast<CAsynPipeHandler*>(pPipe->GetHandler());
	delete pHandler;//这句就会自动删Pipe
}


//InbufFullJob
CPipeInbufFullJob::CPipeInbufFullJob(int32 nPipeId, bool flag, CPipeThread* pThread)
: CPipeJob(nPipeId, pThread)
{
	m_bFull = flag;
}

CPipeInbufFullJob::~CPipeInbufFullJob()
{
	CSyncPipe* pPipe = CAsynPipeHandler::GetPipeFromList(m_uThreadId, m_nPipeId);

	if(!pPipe)
		return;

	class_cast<CAsynPipeHandler*>(pPipe->GetHandler())->SetAsynInbufFull(m_bFull);
}

//CRemoveWaitedPipeFromListJob
CRemoveWaitedPipeFromListJob::CRemoveWaitedPipeFromListJob(IPipe* pPipe, CPipeThread* pThread)
: CPipeJob(pThread)
{
	m_pPipe = pPipe;
}

CRemoveWaitedPipeFromListJob::~CRemoveWaitedPipeFromListJob()
{
	CAsynPipeHandler::RemoveWaitedPipeFromList(m_uThreadId, m_pPipe);
}


//CPipeSetThreadJob
CPipeSetThreadJob::CPipeSetThreadJob(uint32 uOldThreadId, int32 nListenPipeId, IPipe* pPipe, CPipeThread* pThread,bool bFreqDSCallback)
: CPipeJob(nListenPipeId, pThread)
{
	uint32 LocalId = CAsynPipe::GetGlobalId();
	m_pPipe = pPipe;
	m_uLocalId = LocalId;
	m_uOldThreadId = uOldThreadId;
	m_bFreqDSCallback=bFreqDSCallback;
}

CPipeSetThreadJob::~CPipeSetThreadJob()
{
	CSyncPipe* pNewPipe = class_cast<CSyncPipe*>(m_pPipe);
	CAsynPipeHandler* pHandler = new CAsynPipeHandler(m_uLocalId,m_bFreqDSCallback);

	pNewPipe->SetHandler(pHandler);
	pHandler->SetPipe(pNewPipe);
	pHandler->SetThreadId(m_uThreadId);

	class_cast<CSyncPipeReactor*>(CPipeThreadMgr::Inst()->GetSyncPipeReactor(m_uThreadId))
		->Register(pNewPipe);

	CPipeThread* pThread=CPipeThreadMgr::Inst()->GetThread(m_uThreadId);
	Ast(pThread);
	pThread->AddPipe( pHandler->GetLocalID(),pNewPipe );

	pHandler->OnThreadSet(m_nPipeId, m_pPipe, m_uOldThreadId);
}


//CMulticastMsgJob

CMulticastMsgJob::CMulticastMsgJob(size_t uMulticastID, const void* pData,size_t stSize, CPipeThread* pThread)
: CPipeJob(pThread)
{
	m_uMulticastID = uMulticastID;
	m_stDataSize = stSize;
	m_pData = (char*)CloneData( pData, stSize, pThread );
}

CMulticastMsgJob::~CMulticastMsgJob()
{
	CAsynMulticast::MapSyncMulticast* pmap = &CAsynMulticast::m_vecId2Multicast[m_uThreadId];
	CAsynMulticast::MapSyncMulticast::iterator it = pmap->find(m_uMulticastID);

	Ast(it != pmap->end());
	it->second->SendCmd(m_pData, m_stDataSize);
}


//CMulticastAddPipe
CMulticastAddPipe::CMulticastAddPipe(size_t uMulticastID, int nPipeID, CPipeThread* pThread)
: CPipeJob(nPipeID, pThread)
{
	m_uMulticastID = uMulticastID;
}	

CMulticastAddPipe::~CMulticastAddPipe()
{
	CSyncPipe* pPipe = CAsynPipeHandler::GetPipeFromList(m_uThreadId, m_nPipeId);
	if(! pPipe )
		return;

	CAsynMulticast::MapSyncMulticast* pmap = &CAsynMulticast::m_vecId2Multicast[m_uThreadId];
	CAsynMulticast::MapSyncMulticast::iterator it = pmap->find(m_uMulticastID);
	Ast(it != pmap->end());
	it->second->AddPipe(pPipe);
}

//CMulticastDelPipe
CMulticastDelPipe::CMulticastDelPipe(size_t uMulticastID, int nPipeID, CPipeThread* pThread)
: CPipeJob(nPipeID, pThread)
{
	m_uMulticastID = uMulticastID;
}

CMulticastDelPipe::~CMulticastDelPipe()
{
	CSyncPipe* pPipe = CAsynPipeHandler::GetPipeFromList(m_uThreadId, m_nPipeId);
	if(!pPipe)
		return;
	
	CAsynMulticast::MapSyncMulticast* pmap = &CAsynMulticast::m_vecId2Multicast[m_uThreadId];
	CAsynMulticast::MapSyncMulticast::iterator it = pmap->find(m_uMulticastID);
	Ast(it != pmap->end());
	it->second->DelPipe(pPipe);
}


//CPipeDelFromAllMulticastJob
CPipeDelFromAllMulticastJob::CPipeDelFromAllMulticastJob(int32 nPipeId, CPipeThread* pThread)
: CPipeJob(nPipeId, pThread)
{

}

CPipeDelFromAllMulticastJob::~CPipeDelFromAllMulticastJob()
{
	CSyncPipe* pPipe = CAsynPipeHandler::GetPipeFromList(m_uThreadId, m_nPipeId);
	if( !pPipe )
		return;

	pPipe->DelFromAllMulticast();
}



//CAddMulticastJob
CAddMulticastJob::CAddMulticastJob(size_t uMulticastID, CPipeThread* pThread)
: CPipeJob(pThread)
{
	m_uMulticastID = uMulticastID;
}

CAddMulticastJob::~CAddMulticastJob()
{
	CMulticast* pMulticast = new CMulticast;
	pMulticast->SetLocalID(m_uMulticastID);
	
	CAsynMulticast::MapSyncMulticast* pmap = &CAsynMulticast::m_vecId2Multicast[m_uThreadId];
	CAsynMulticast::MapSyncMulticast::iterator it = pmap->find(m_uMulticastID);
	Ast(it == pmap->end());
	pair<CAsynMulticast::MapSyncMulticast::iterator ,bool>p =pmap->insert(make_pair(m_uMulticastID, pMulticast));
	Ast(p.second);
}

//CDelMulticastJob
CDelMulticastJob::CDelMulticastJob(size_t uMulticastID, CPipeThread* pThread)
: CPipeJob(pThread)
{
	m_uMulticastID = uMulticastID;
}
	
CDelMulticastJob::~CDelMulticastJob()
{
	CAsynMulticast::MapSyncMulticast* pmap = &CAsynMulticast::m_vecId2Multicast[m_uThreadId];
	CAsynMulticast::MapSyncMulticast::iterator it = pmap->find(m_uMulticastID);
	Ast(it != pmap->end());
	it->second->Release();
	pmap->erase(it);
}


CPipeEndThreadJob::CPipeEndThreadJob(CPipeThread* pThread)
: CPipeJob(pThread)
{
	m_pThread = pThread;
}

CPipeEndThreadJob::~CPipeEndThreadJob()
{
	m_pThread->Quit();
}
