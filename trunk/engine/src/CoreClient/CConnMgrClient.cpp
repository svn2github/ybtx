#include "stdafx.h"
#include "CConnMgrClient.h"
#include "CConnPipeHandler.h"
#include "CCoreObjectFollower.inl"
#include "CConnClient.h"
#include "CTimeSystemClient.h"
#include "IPipeModule.h"
#include "IPipeReactor.h"
#include "CAppClient.h"
#include "IConnMgrClientHandler.h"

#include "TMsgDispatcher_inl.inl"
#include "TMsgHandler.inl"

#include "CConnClient.inl"
#include "TConnectionMgr.inl"
#include "TClientMsgRegister.inl"
#include "TSqrAllocator.inl"

#include "CCoreSceneMgrClient.h"
#include "CLog.h"
#include "ThreadHelper.h"


template class TConnectionMgr<CTraitsClient>;

CConnMgrClient::CConnMgrClient()
{
	wchar_t szLogFileName[256];
	wchar_t szTimeBuf[64];
	GetCurLogTime(szTimeBuf, 64);

	swprintf_s(szLogFileName, _countof(szLogFileName), L"RecvTraffic_%s.log", szTimeBuf);
	m_pRecvTrafficLog = CLog::CreateLog(utf16_to_utf8(szLogFileName).c_str());

	wchar_t szLogConnName[256];
	swprintf_s(szLogConnName, _countof(szLogFileName), L"Conn_%d%s.log", GetCurPID(), szTimeBuf);
	m_pOffLineLog = CLog::CreateLog(utf16_to_utf8(szLogConnName).c_str());
}

CConnMgrClient::~CConnMgrClient(void)
{
	m_pRecvTrafficLog->Release();
	m_pOffLineLog->Release();
	ShutDownAllConn();
}

CConnClient* CConnMgrClient::Connect(const CAddress& addr)
{
	if( CConnClient::Inst())
		return NULL;

	CConnClient* pConn = new CConnClient;
	
	IPipe*	pPipe = CAppClient::Inst()->m_pPipeReactor->
		CreateIPipe(pConn,1024*10240,2*1024*1024,ePCF_SendWayEncryption /*| ePCF_RecvWayDecryption*/ | ePCF_RecvWayDecompression );
	pConn->Init( pPipe );

	m_listAllConn.push_front(pConn);
	pConn->m_itListConn=m_listAllConn.begin();

	pPipe->Connect(addr);

	return CConnClient::Inst();
}

void CConnMgrClient::WriteRecvTrafficLog(const char* pLog)
{
	m_pRecvTrafficLog->Write(pLog);
}

void CConnMgrClient::SetOffLineLog(const char* msg)
{
	m_pOffLineLog->Write(msg);
}