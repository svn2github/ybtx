#include "stdafx.h"
#include "CConnMgrStress.h"
#include "CTimeSystemStress.h"
#include "IPipeModule.h"
#include "IPipeReactor.h"
#include "CAppStress.h"
#include "IConnMgrStressHandler.h"
#include "TConnectionMgr.inl"
#include "CCoreSceneMgrClient.h"
#include "TSqrAllocator.inl"

#include "TMsgDispatcher_inl.inl"
#include "TMsgHandler.inl"

#include "CConnStress.inl"
#include "TClientMsgRegister.inl"


template class TConnectionMgr<CTraitsStress>;

CConnMgrStress::CConnMgrStress()
{
	
}

CConnMgrStress::~CConnMgrStress(void)
{
	ShutDownAllConn();
}

CConnStress* CConnMgrStress::Connect(const CAddress& addr)
{
	CConnStress* pConn = new CConnStress;

	IPipe* pPipe=CAppStress::Inst()->m_pPipeReactor->
		CreateIPipe(pConn,1024*256,1024*512,
			ePCF_SendWayEncryption|/*ePCF_RecvWayDecryption|*/ePCF_RecvWayDecompression);
	pConn->Init( pPipe );
	m_listAllConn.push_front(pConn);
	pConn->m_itListConn=m_listAllConn.begin();

	pPipe->Connect(addr);

	return pConn;
}
