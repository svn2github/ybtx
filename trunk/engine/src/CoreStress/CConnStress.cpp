//$id$
//对象系统使用的连接对象,仅负责对象系统相关的网络消息的派发

#include "stdafx.h"
#include "CAppConfigStress.h"

#include "TConnection.inl"
#include "TConnection_inl.inl"

#include "TMsgDispatcher.inl"
#include "TMsgDispatcher_inl.inl"
#include "TMsgHandler.inl"

#include "PrtlGas2GacObjectDef.h"
#include "PrtlGac2GasGenericDef.h"
#include "PrtlGas2GacGenericDef.h"

#include "CTimeSystemStress.h"
#include "TimeHelper.h"
#include "CConnMgrStress.h"
#include "IConnMgrStressHandler.h"
#include "CCoreObjectStress.h"
#include "CConnStress.inl"
#include "TSqrAllocator.inl"
#include "GuidHelper.h"

#include "CSlowSigner.inl"

#include <iostream>

template class TBaseMsgDispatcher<CConnStress,uint8>;
template class TConnection<CTraitsStress>;


CConnStress::CConnStress()
{
	m_pSlowSigner			=NULL;
	m_uCurrentDelay			=0;
	m_bGlobalTimeSynced		=false;
	m_bWaitingDisconnect	=false;
	m_uLastSyncedServerFrameTime = 0;
}

CConnStress::~CConnStress(void)
{
}

void CConnStress::OnTick()
{
	m_uLastPingTime = GetProcessTime();

	CGac2GasGC_Ping_Server ping;
	SendCoreCmd(&ping);
}

void CConnStress::OnServerCommand(const CGas2GacGC_Ping_Client* pCmd)
{
	m_uCurrentDelay = static_cast<uint32>((GetProcessTime() - m_uLastPingTime) / 2);

	CGac2GasGC_Update_Latency_Server transmit;
	SendCoreCmd(&transmit);
}

uint32 CConnStress::GetLatency()const
{ 
	return	m_uCurrentDelay;
}

bool CConnStress::IsConnected()const
{
	return GetPipe()->IsConnected() && m_bGlobalTimeSynced;
}

bool CConnStress::Send(const void* pData,uint32 uCurSize)
{
	if( !IsConnected() )
		return false;

	if(uCurSize<=0xff)
	{
		CGac2GasGC_Small_Shell_Message cmd;
		
		m_pSlowSigner->SlowSignOnData(pData,uCurSize,&cmd);
		cmd.uobClientTime = CTimeSystemStress::Inst()->GetFrameTime();
		cmd.uobClientKnownServerFrameTime = m_uLastSyncedServerFrameTime;
		cmd.usbDataLength	= static_cast<uint8>(uCurSize);
		TConnection<CTraitsStress>::SendCoreCmd(&cmd);
	}
	else if(uCurSize<=0xffff)
	{
		CGac2GasGC_Middle_Shell_Message cmd;
		
		m_pSlowSigner->SlowSignOnData(pData,uCurSize,&cmd);
		cmd.uobClientTime = CTimeSystemStress::Inst()->GetFrameTime();
		cmd.uobClientKnownServerFrameTime = m_uLastSyncedServerFrameTime;
		cmd.udbDataLength	= static_cast<uint16>( uCurSize);
		TConnection<CTraitsStress>::SendCoreCmd(&cmd);
	}
	else
	{
		LogErr("客户端逻辑消息大小最大不能超过64k");
		CoreShutDown("客户端逻辑消息大小最大不能超过64k");
	}

	SendCoreMsg(pData,uCurSize);
	
	return true;
}

void CConnStress::OnServerCommand(const CGas2GacGC_Small_Shell_Message* pCmd)
{
	char* pShellMsg = (char*)( pCmd + 1 );
	CacheRecvData(pShellMsg,pCmd->usbDataLength);
	CConnMgrStress::Inst()->GetHandler()->OnDataReceived(this);
	StoreRecvData();
}

void CConnStress::OnServerCommand(const CGas2GacGC_Middle_Shell_Message* pCmd)
{
	char* pShellMsg = (char*)( pCmd + 1 );
	CacheRecvData(pShellMsg,pCmd->udbDataLength);
	CConnMgrStress::Inst()->GetHandler()->OnDataReceived(this);
	StoreRecvData();
}

void CConnStress::OnServerCommand(const CGas2GacGC_Big_Shell_Message* pCmd)
{
	char* pShellMsg = (char*)( pCmd + 1 );
	CacheRecvData(pShellMsg,pCmd->uqbDataLength);
	CConnMgrStress::Inst()->GetHandler()->OnDataReceived(this);
	StoreRecvData();
}

void CConnStress::OnServerCommand(const CGas2GacGC_Tell_Error* pCmd)
{
	const char* pShellMsg = (const char*)pCmd + sizeof(CGas2GacGC_Tell_Error);
	string strMsg;
	strMsg.append(pShellMsg, pCmd->udbDataLength);
	cerr << strMsg << endl;
}


void CConnStress::OnServerCommand(const CGas2GacGC_Tell_Server_Time* pCmd)
{
	m_uLastSyncedServerFrameTime = pCmd->uobGlobalTime;

	if(m_bGlobalTimeSynced)
		return;

	m_bGlobalTimeSynced=true;
	CConnMgrStress::Inst()->GetHandler()->OnConnected(this);
}

void CConnStress::OnServerCommand(const CGas2GacGC_Tell_Guids* pCmd)
{
	m_pSlowSigner = new CSlowSigner( pCmd->guidConnection );
}

void CConnStress::OnServerCommand(const CGas2GacOC_Set_Main_Scene* pCmd)
{
	CGac2GasGC_Main_Scene_Set Cmd;
	SendStressCmd( &Cmd );
}

void CConnStress::OnConnected()
{
	CTimeSystemStress::Inst()->Register( this, 45000 );		// 每45秒就向服务器发一次包	

	CGac2GasGC_Client_Guid cmd;
	memcpy(cmd.guid, GetProcessGUID(), sizeof(cmd.guid));

	struct tm timenow;
	GetProcessInitTime(&timenow);

	sprintf(cmd.szTime, "%d-%02d-%02d %02d:%02d:%02d", 
		timenow.tm_year + 1900, timenow.tm_mon + 1, 
		timenow.tm_mday, timenow.tm_hour,
		timenow.tm_min, timenow.tm_sec);

	SendStressCmd(&cmd);
}

void CConnStress::OnConnectFailed(EPipeConnFailedReason eReason)
{
	std::cerr << "Conn Failed Reason: "<< eReason << std::endl;
	CConnMgrStress* pMgr=CConnMgrStress::Inst();
	pMgr->GetHandler()->OnConnectFailed(this);
	pMgr->m_listAllConn.erase(m_itListConn);
	delete this;
}

void CConnStress::DoShutDownAndCallbackDerived()
{
	//注销ping tick
	CTimeSystemStress::Inst()->UnRegister( this );

	CConnMgrStress* pMgr=CConnMgrStress::Inst();
	if( m_bGlobalTimeSynced && pMgr->GetHandler() && !IsShuttingDown())
		pMgr->GetHandler()->OnDisconnect(this);

	pMgr->DoShutDownConn(this);

	delete m_pSlowSigner;
	m_pSlowSigner=NULL;
}

void CConnStress::ShutDown()
{
	IntShutDown();
}

void CConnStress::ShutDown(const char* sMsg)
{
	ShutDown();
}

bool CConnStress::GetLocalAddress(CAddress& Address) const
{
	bool bResult = IsConnected();
	if( bResult)
		GetPipe()->GetLocalAddress(Address);

	return bResult;
}

bool CConnStress::GetRemoteAddress(CAddress& Address) const
{
	bool bResult = IsConnected();
	if(bResult)
		GetPipe()->GetRemoteAddress(Address);

	return bResult;
}

void CConnStress::SendErrMsg(const char* )
{
	//nothing to do
}

CLogOwner* CConnStress::GetLogOwner()
{
	return this;
}

uint64 CConnStress::GetLastSyncedServerFrameTime()const
{
	return m_uLastSyncedServerFrameTime;
}
