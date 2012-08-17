//$id$
//对象系统使用的连接对象,仅负责对象系统相关的网络消息的派发

#include "stdafx.h"
#include "CCoreObjectFollower.h"
#include "CCoreObjectDirector.h"

#include "TConnection.inl"
#include "TConnection_inl.inl"

#include "TMsgDispatcher.inl"
#include "TMsgDispatcher_inl.inl"
#include "TMsgHandler.inl"

#include "PrtlGas2GacObjectDef.h"
#include "PrtlGac2GasGenericDef.h"
#include "PrtlGas2GacGenericDef.h"

#include "CTimeSystemClient.h"
#include "CConnMgrClient.h"
#include "CCoreSceneMgrClient.h"
#include "CCoreSceneClient.h"
#include "CConnClient.inl"
#include "CAppConfigClient.h"
#include "CErrNotifyIcon.h"
#include "GuidHelper.h"
#include "ErrLogHelper.h"
#include "TSqrAllocator.inl"

#include "CSlowSigner.inl"
#include "CAppClient.h"
#include "CServerTimeGuesser.h"

#include "NetworkAdaptersHelper.h"

static const uint32 CHECK_NET_DELAY_INTERVAL = 15000;

template class TBaseMsgDispatcher<CConnClient,uint8>;
template class TConnection<CTraitsClient>;

CConnClient::CConnClient()
{
	Ast(!Inst());
	Inst()=this;

	m_uLastSyncedServerFrameTime		= 0;
	m_pSlowSigner			= NULL;
	m_uCurrentDelay			= 0;
	m_bWaitingDisconnect	= false;
	m_bGlobalTimeSynced		= false;
	m_bShutting = false;
	m_bRecvTrafficStatStarted = false;

	CCoreObjectDirector::GetLastDistortedProcessTime() = 0;
	CCoreObjectDirector::GetLastProcessTime() = 0;
	CCoreObjectDirector::GetLastDistortedFrameTime() = 0;
	CCoreObjectDirector::GetLastFrameTime() = 0;
	m_EstimatedServerTime = new CServerTimeGuesser;
}

CConnClient::~CConnClient(void)
{
	Inst()=NULL;

	delete m_EstimatedServerTime;
	m_EstimatedServerTime = NULL;

	CCoreObjectDirector::GetLastDistortedProcessTime() = 0;
	CCoreObjectDirector::GetLastProcessTime() = 0;
	CCoreObjectDirector::GetLastDistortedFrameTime() = 0;
	CCoreObjectDirector::GetLastFrameTime() = 0;

	GenRecvTrafficStatFile();
}

CConnClient*& CConnClient::Inst()
{
	static CConnClient* ls_Inst=NULL;
	return ls_Inst;
}

void CConnClient::SetConnValue(const char* szKey, const char* szValue)
{
	CLogOwner::SetValue(szKey, szValue);
}

void CConnClient::TellServerBeTesting()
{
	if( !CAppConfigClient::Inst()->IsTestClient() )
		return;

	CGac2GasGC_Tell_BeTesting cmd;
	SendCoreCmd(&cmd);
}

void CConnClient::TellServerClientGuid()
{
	CGac2GasGC_Client_Guid cmd;
	memcpy(cmd.guid, GetProcessGUID(), sizeof(cmd.guid));

	struct tm timenow;
	GetProcessInitTime(&timenow);

	sprintf(cmd.szTime, "%d-%02d-%02d %02d:%02d:%02d", 
		timenow.tm_year + 1900, timenow.tm_mon + 1, 
		timenow.tm_mday, timenow.tm_hour,
		timenow.tm_min, timenow.tm_sec);

	SendCoreCmd(&cmd);
}

void CConnClient::OnTick()
{
	m_uLastPingTime = GetProcessTime();

	CGac2GasGC_Ping_Server ping;
	SendCoreCmd(&ping);
}

void CConnClient::OnServerCommand(const CGas2GacGC_Ping_Client* pCmd)
{
	m_uCurrentDelay = static_cast<uint32>((GetProcessTime() - m_uLastPingTime) / 2);

	CGac2GasGC_Update_Latency_Server transmit;
	SendCoreCmd(&transmit);
}

uint32 CConnClient::GetLatency() const
{ 
	return m_uCurrentDelay;
}

bool CConnClient::IsConnected()const
{
	return GetPipe()->IsConnected() && m_bGlobalTimeSynced;
}

bool CConnClient::Send(const void* pData,uint32 uCurSize)
{
	Ast( uCurSize> 0 );

	if( !IsConnected() )
		return false;

	if(uCurSize<=0xff)
	{
		CGac2GasGC_Small_Shell_Message cmd;

		m_pSlowSigner->SlowSignOnData(pData,uCurSize,&cmd);

		cmd.uobClientTime = CTimeSystemClient::Inst()->GetFrameTime();
		cmd.uobClientKnownServerFrameTime = m_uLastSyncedServerFrameTime;
		cmd.usbDataLength	= static_cast<uint8>(uCurSize);
		TConnection<CTraitsClient>::SendCoreCmd(&cmd);

		if(m_uLastSyncedServerFrameTime > 100000000000000000)
		{
			ostringstream strm;
			strm << "CGac2GasGC_Small_Shell_Message" << m_uLastSyncedServerFrameTime;
			LogErr("ClientKnownServerFrameTime Err", strm.str());
		}
	}
	else if(uCurSize<=0xffff)
	{
		CGac2GasGC_Middle_Shell_Message cmd;

		m_pSlowSigner->SlowSignOnData(pData,uCurSize,&cmd);

		cmd.uobClientTime = CTimeSystemClient::Inst()->GetFrameTime();
		cmd.uobClientKnownServerFrameTime = m_uLastSyncedServerFrameTime;
		cmd.udbDataLength	= static_cast<uint16>(uCurSize);
		TConnection<CTraitsClient>::SendCoreCmd(&cmd);

		if(m_uLastSyncedServerFrameTime > 100000000000000000)
		{
			ostringstream strm;
			strm << "CGac2GasGC_Middle_Shell_Message" << m_uLastSyncedServerFrameTime;
			LogErr("ClientKnownServerFrameTime Err", strm.str());
		}
	}
	else
	{
		LogErr("客户端逻辑消息大小最大不能超过64k");
		CoreShutDown("客户端逻辑消息大小最大不能超过64k");
	}

	SendCoreMsg(pData,uCurSize);

	return true;
}

void CConnClient::OnServerCommand(const CGas2GacGC_Small_Shell_Message* pCmd)
{
	char* pShellMsg = const_cast< char* > ( reinterpret_cast< const char* >( pCmd + 1 ) );
	CacheRecvData(pShellMsg,pCmd->usbDataLength);
	CConnMgrClient::Inst()->GetHandler()->OnDataReceived(this);
	StoreRecvData();
}

void CConnClient::OnServerCommand(const CGas2GacGC_Middle_Shell_Message* pCmd)
{
	char* pShellMsg = (char*)( pCmd + 1 );
	CacheRecvData(pShellMsg,pCmd->udbDataLength);
	CConnMgrClient::Inst()->GetHandler()->OnDataReceived(this);
	StoreRecvData();
}

void CConnClient::OnServerCommand(const CGas2GacGC_Big_Shell_Message* pCmd)
{
	char* pShellMsg = (char*)( pCmd + 1 );
	CacheRecvData(pShellMsg,pCmd->uqbDataLength);
	CConnMgrClient::Inst()->GetHandler()->OnDataReceived(this);
	StoreRecvData();
}

void CConnClient::OnServerCommand(const CGas2GacGC_Tell_Server_Time* pCmd)
{
	uint64 uCurFrameTime = CTimeSystemClient::Inst()->GetFrameTime();
	//cout << "serverFrameTime " << m_uLastSyncedServerFrameTime << endl;
	m_uLastSyncedServerFrameTime = pCmd->uobGlobalTime;
	if(m_uLastSyncedServerFrameTime > 100000000000000000)
	{
		ostringstream strm;
		strm << m_uLastSyncedServerFrameTime;
		LogErr("ServerFrameTime Err", strm.str());
	}

	m_nClientServerTimeDiff = int64(uCurFrameTime)  - int64(m_uLastSyncedServerFrameTime);

	m_EstimatedServerTime->SetBaseServerTime(pCmd->uobGlobalTime);

	if( m_bGlobalTimeSynced )
		return;

	m_bGlobalTimeSynced=true;
	CConnMgrClient::Inst()->GetHandler()->OnConnected(this);
}

void CConnClient::OnServerCommand(const CGas2GacGC_Tell_Error* pCmd)
{
	const char* pShellMsg = (const char*)pCmd + sizeof(CGas2GacGC_Tell_Error);
	string strMsg;
	strMsg.append(pShellMsg, pCmd->udbDataLength);

	CAppClient::Inst()->TellErrMessage(strMsg.c_str(), "Server Error");
}

void CConnClient::OnServerCommand(const CGas2GacGC_Tell_Guids* pCmd)
{
	str_guid_t szGuidServer;
	str_guid_t szGuidConnection;

	GuidToString(pCmd->guidConnection, szGuidConnection);
	ErrLogMsg("Conn Guid", szGuidConnection);

	GuidToString(pCmd->guidServer, szGuidServer);	
	ErrLogMsg("Server GUID", szGuidServer);

	m_pSlowSigner = new CSlowSigner( pCmd->guidConnection );
	//memcpy( m_guidConn , pCmd->guidConnection , sizeof( m_guidConn) );

	CAddress address;

	if (GetRemoteAddress(address))
	{
		string strAddr;
		address.GetAddressAndPort(strAddr);
		ErrLogMsg("Server Addr", strAddr.c_str());
	}

	if (GetLocalAddress(address))
	{	
		string strAddr;
		address.GetAddressAndPort(strAddr);
		ErrLogMsg("Client Addr", strAddr.c_str());
	}
}

void CConnClient::OnServerCommand(const CGas2GacGC_Tell_Current_Version* pCmd)
{
	string uCurrentVersion = pCmd->uCurrentVersion;
	uint32 uCurrentRevision = pCmd->uCurrentRevision;
	uint32 uRevision = GetCurrentRevision();
	string uVersion = GetCurrentVersion();
	if((uCurrentVersion!="")&&(uVersion!=""))
	{
		if(uCurrentVersion!=uVersion)
		{
			ostringstream strm;
			strm << "客户端的版本号 "<< uVersion << " 与服务器的版本号 " << uCurrentVersion << " 不匹配 " 
				<< "\n 请重新运行游戏启动器进行更新" << endl;
			ShowErrorMsg(strm.str().c_str());
			CoreShutDown("客户端的版本号不匹配");
		}
	}
	else if(uCurrentRevision != uRevision)
	{
		ostringstream strm;
		strm << "客户端的版本号 "<< uRevision << " 与服务器的版本号 " << uCurrentRevision << " 不匹配 " 
			<< "\n 请重新运行游戏启动器进行更新" << endl;
		ShowErrorMsg(strm.str().c_str());
		CoreShutDown("客户端的版本号不匹配");
	}
}

void CConnClient::OnServerCommand(const CGas2GacGC_Tell_OwnerInfo* pCmd)
{
	const char* pShellMsg = (const char*)pCmd + sizeof(CGas2GacGC_Tell_OwnerInfo);
	string strMsg;
	strMsg.append(pShellMsg, pCmd->usbDataLength);

	SetValue(strMsg.c_str());
}

void CConnClient::ShutDown(const char* szMsg)
{
	if(m_bShutting)
		return;
	m_bShutting = true;
	CCoreSceneMgrClient::Inst()->DestroyMainSceneIfNotBeingUsedByLogic();
	CTimeSystemClient::Inst()->UnRegister(this);
	if (IntShutDown() && szMsg)
	{
		ostringstream strm;
		CAddress addr;
		const char* szUserName = GetValue("UserName");
		szUserName = szUserName?szUserName:"";
		GetLocalAddress(addr);
		LogTime(strm);
		strm << " account:" << szUserName << " reason:" << szMsg << " localip:" << addr.GetAddress() << " remoteip: " << endl;
		LogOnOffLineMsg(strm.str().c_str());
	}
	Inst()=NULL;
}

void CConnClient::ShutDown()
{
	ShutDown(NULL);
}

void CConnClient::LogOnOffLineMsg(const char* msg)
{
	CConnMgrClient::Inst()->SetOffLineLog(msg);
}

void CConnClient::OnConnected()
{
	CTimeSystemClient::Inst()->Register( this, CHECK_NET_DELAY_INTERVAL );

	//连接之后告诉服务端是否为testing client
	TellServerBeTesting();

	//将客户端的guid信息发送给服务器
	TellServerClientGuid();
}

void CConnClient::OnConnectFailed(EPipeConnFailedReason eReason)
{
	CConnMgrClient* pMgr=CConnMgrClient::Inst();

	if(pMgr->GetHandler())
	{
		EConnConnectFailedReason eConnReason;

		switch( eReason )
		{
		case ePCFR_UNREACHABLE:
			eConnReason=eCCFR_UNREACHABLE;
			break;
		case ePCFR_REFUSED:
			eConnReason=eCCFR_REFUSED;
			break;
		case ePCFR_INVALIDADDR:
			eConnReason=eCCFR_INVALIDADDR;
			break;
		case ePCFR_OSERROR:
			eConnReason=eCCFR_OSERROR;
			break;
		default:
			GenErr("ConnClient收到不合理的回调");
		}
		pMgr->GetHandler()->OnConnectFailed(this,eConnReason);
	}

	if( IsShuttingDown() )
	{
		return;
	}

	pMgr->m_listAllConn.erase(m_itListConn);
	GetPipe()->Release();
	delete this;
}


void CConnClient::DoShutDownAndCallbackDerived()
{
	CConnMgrClient* pMgr=CConnMgrClient::Inst();	

	if( !IsShuttingDown() )
	{
		IConnMgrClientHandler* pConnMgrHandler = pMgr->GetHandler();
		if(m_bGlobalTimeSynced)
		{
			CCoreSceneMgrClient::Inst()->DestroyMainSceneIfNotBeingUsedByLogic();

			CTimeSystemClient::Inst()->UnRegister(this);
			if(pConnMgrHandler)
			{
				SQR_TRY	
				{
					pConnMgrHandler->OnDisconnect(this);
				}
				SQR_CATCH(exp)
				{
					LogExp(exp);
				}
				SQR_TRY_END;
			}
		}
		else
		{
			if(pConnMgrHandler)
				pConnMgrHandler->OnConnectFailed(this,eCCFR_UNREACHABLE);
		}
	}

	pMgr->DoShutDownConn(this);

	delete m_pSlowSigner;
	m_pSlowSigner=NULL;
}

void CConnClient::OnMainSceneSet()
{
	CGac2GasGC_Main_Scene_Set Cmd;
	CConnClient::Inst()->SendCoreCmd(&Cmd);
}

uint64 CConnClient::GetServerFrameTime()const
{
	if( !m_bGlobalTimeSynced )
		GenErr("Global time is not syncronized yet.");
	
	int64 iRet = int64( CTimeSystemClient::Inst()->GetFrameTime() ) - m_nClientServerTimeDiff;
	//cout << "CConnClient::GetServerFrameTime " << int64( CTimeSystemClient::Inst()->GetFrameTime() ) << " " << m_nClientServerTimeDiff << endl;
	return iRet;
}

void CConnClient::SendErrMsg(const char* szError)
{
	CAppClient::Inst()->TellErrMessage(szError);
}

inline bool CConnClient::MoreTraffic(const RecvTrafficData& left, const RecvTrafficData& right)
{
	return left.m_uTraffic > right.m_uTraffic;
}

void CConnClient::GenRecvTrafficStatFile()
{
	if(!m_bRecvTrafficStatStarted)
		return;

	ostringstream strm;
	
	strm << "+++++++++++++++++ begin +++++++++++++++++" << endl;
	strm << "ID\t" << "Name\t" << "Total\t" << "Count" << endl;

	sort(m_vecRecvTrafficDataForCpp.begin(), m_vecRecvTrafficDataForCpp.end(), MoreTraffic);
	for(uint16 u = 0; u < m_vecRecvTrafficDataForCpp.size(); ++u)
	{
		RecvTrafficData& trafficData = m_vecRecvTrafficDataForCpp[u];
		if( trafficData.m_uCount == 0)
			continue;

		strm << u << "\t" << trafficData.m_strProtlName << "\t" << trafficData.m_uTraffic << " \t" << trafficData.m_uCount << endl;
		trafficData.m_uCount = 0;
		trafficData.m_uTraffic = 0;
	}

	sort(m_vecRecvTrafficDataForScript.begin(), m_vecRecvTrafficDataForScript.end(), MoreTraffic);
	for(uint16 u = 0; u < m_vecRecvTrafficDataForScript.size(); ++u)
	{
		RecvTrafficData& trafficData = m_vecRecvTrafficDataForScript[u];
		if( trafficData.m_uCount == 0)
			continue;

		strm << u << "\t\t" << trafficData.m_uTraffic << " \t" << trafficData.m_uCount << endl;
		trafficData.m_uCount = 0;
		trafficData.m_uTraffic = 0;
	}

	MapVariantDecodeTimes::iterator iter = m_mapRecvTrafficVariantData.begin();
	for (; iter != m_mapRecvTrafficVariantData.end(); iter++)
	{
		RecvTrafficData& trafficData = iter->second;
		strm << iter->first << "\t\t" << trafficData.m_uTraffic << " \t" << trafficData.m_uCount << endl;
	}


	strm << "+++++++++++++++++ end +++++++++++++++++" << endl;

	CConnMgrClient::Inst()->WriteRecvTrafficLog(strm.str().c_str());
	m_bRecvTrafficStatStarted = false;
}

void CConnClient::LogMsgRecvTrafficForCpp(const char* szProtlName, uint16 uMsgID, uint32 uMsgLen)
{
	//cout << uMsgID << " " << uMsgLen << endl;
	if(!m_bRecvTrafficStatStarted)
		return;

	if(m_vecRecvTrafficDataForCpp.size() <= uMsgID)
		m_vecRecvTrafficDataForCpp.resize(uMsgID + 1);

	if(m_vecRecvTrafficDataForCpp[uMsgID].m_strProtlName.empty())
		m_vecRecvTrafficDataForCpp[uMsgID].m_strProtlName = szProtlName;

	RecvTrafficData& trafficData = m_vecRecvTrafficDataForCpp[uMsgID];
	trafficData.m_uTraffic += uMsgLen;
	++trafficData.m_uCount;
}

void CConnClient::LogMsgRecvTrafficForScript(uint16 uMsgID, uint32 uMsgLen)
{
	if(!m_bRecvTrafficStatStarted)
		return;

	if(m_vecRecvTrafficDataForScript.size() <= uMsgID)
		m_vecRecvTrafficDataForScript.resize(uMsgID + 1);

	RecvTrafficData& trafficData = m_vecRecvTrafficDataForScript[uMsgID];
	trafficData.m_uTraffic += uMsgLen;
	++trafficData.m_uCount;
}

void CConnClient::LogMsgRecvTrafficForVariantDecode(const char* sName, uint32 uMsgLen)
{
	if(!m_bRecvTrafficStatStarted)
		return;

	MapVariantDecodeTimes::iterator iter = m_mapRecvTrafficVariantData.find(sName);
	if (iter == m_mapRecvTrafficVariantData.end())
	{
		RecvTrafficData trafficData;
		trafficData.m_uTraffic = uMsgLen;
		trafficData.m_uCount = 1;
		m_mapRecvTrafficVariantData.insert(make_pair(sName,trafficData));
		return;
	}

	RecvTrafficData& trafficData = iter->second;
	trafficData.m_uTraffic += uMsgLen;
	++trafficData.m_uCount;
}

void CConnClient::StartRecvTrafficStat()
{
	if(m_bRecvTrafficStatStarted)
		return;

	m_bRecvTrafficStatStarted = true;
	m_vecRecvTrafficDataForCpp.clear();
	m_vecRecvTrafficDataForScript.clear();
	m_mapRecvTrafficVariantData.clear();
}

void CConnClient::CloseRecvTrafficStat()
{
	GenRecvTrafficStatFile();
}

uint64 CConnClient::GetEstimatedServerTime()const
{
	return m_EstimatedServerTime->Guess();
}

uint64 CConnClient::GetEstimatedServerFrameTime()const
{
	return m_EstimatedServerTime->GetElapsedFrameTime();
}

const char* CConnClient::GetLocalPhysicalAddress()
{
	static vector<NetworkAdaptersInfo> vecInfo;
	GetNetworkAdaptersInfo(vecInfo);
	CAddress address;
	if (GetLocalAddress(address))
	{	
		vector<NetworkAdaptersInfo>::iterator it = vecInfo.begin();
		for (;it!=vecInfo.end();++it)
		{
			if((*it).IPAddress==address.GetAddress())
			{
				return (*it).PhysicalAddress.c_str();
			}
		}
	}
	return "";
}

uint64 CConnClient::GetLastSyncedServerFrameTime()const
{
	return m_uLastSyncedServerFrameTime;
}

CLogOwner* CConnClient::GetLogOwner()
{
	return this;
}

