//$id$
//对象系统使用的连接对象,仅负责对象系统相关的网络消息的派发
#include "stdafx.h"
#include "CSynConnServer.h"
#include "CSyncCoreObjectMediator.inl"

#include "TConnection.inl"
#include "TConnection_inl.inl"

#include "TMsgDispatcher.inl"
#include "TMsgDispatcher_inl.inl"
#include "TMsgHandler.inl"

#include "PrtlGac2GasObjectDef.h"
#include "PrtlGac2GasGenericDef.h"
#include "PrtlGas2GacGenericDef.h"

#include "CSyncTimeSystemServer.h"
#include "IConnMgrServerHandler.h"
#include "TimeHelper.h"
#include "CSynConnMgrServer.h"
#include "CAppConfigServer.h"
#include "BaseHelper.h"
#include "CSyncAppServer.h"
#include "TIDPtrMap.inl"
#include "TimeHelper.h"
#include "CCoreConnServerResult.h"
#include "CCoreServerResult.inl"
#include "TSqrAllocator.inl"
#include "GuidHelper.h"
#include "CoreCommon.h"

#if defined _WIN32
#include <direct.h>
#else
#include <unistd.h>
#define _chdir chdir
#define _mkdir mkdir
#define _getcwd getcwd
#endif

static const uint32 CHECK_NET_DELAY_INTERVAL = 15000;

template class TBaseMsgDispatcher<CSynConnServer,uint8>;
template class TConnection<CSyncTraitsServer>;

template void CSynConnServer::FastVerifyData<CGac2GasOC_Tell_Mediator_Stepped>
(const void* pData,size_t stDataSize,const CGac2GasOC_Tell_Mediator_Stepped* pCmd);

CSynConnServer* CSynConnServer::GetSynConn(uint32 uSynConnID)
{
	CSynConnServer* pSynConn;
	return GetSynConnMap().Get(pSynConn, uSynConnID) ? pSynConn : NULL;
}

void CSynConnServer::RegisterMsgHandler()
{
	CSynConnServer::SetHandlerNumber(eGac2GasOCI_Count+eGac2GasGCI_Count);

	//原则上，改变Mediator位置的所有命令都要带timestamp以检查作弊
	//CGac2GasOC_Tell_Mediator_Stepped也改变了Mediator位置，但是因为他的发送有固定的频率，可以推导出timestamp，因此就不需要发送。
	RegisterDirectorCommandBlockByMediator		<CGac2GasOC_Change_Mediator_Move_Target>	();
	RegisterDirectorCommandBlockByMediator		<CGac2GasOC_Tell_Mediator_Reached>			();
	RegisterDirectorCommandBlockByMediator		<CGac2GasOC_Tell_Mediator_Stopped>			();
	RegisterDirectorCommandBlockByMediator		<CGac2GasOC_Change_Mediator_Move_Speed>		();
	RegisterDirectorCommandBlockByMediator		<CGac2GasOC_Move_Mediator_To>				();
	RegisterDirectorCommandBlockByMediator		<CGac2GasOC_Tell_Mediator_Stepped>			();
	RegisterDirectorCommandBlockByMediator		<CGac2GasOC_Tell_Mediator_Max_Speed_Set>();

	
	RegisterDirectorCommand		<CGac2GasOC_Director_Is_Activated>			();
	//for stress test
	RegisterDirectorCommand		<CGac2GasOC_Tell_Mediator_SetPosition>		();
	
	
	RegisterClientCommand		<CGac2GasOC_Report_Invalid_Step>		();
	RegisterClientCommand		<CGac2GasOC_Report_Invalid_Move>		();
	RegisterClientCommand		<CGac2GasGC_Ping_Server>				();
	RegisterClientCommand		<CGac2GasGC_Update_Latency_Server>		();
	RegisterClientCommand		<CGac2GasGC_Main_Scene_Set>				();
	RegisterClientCommand		<CGac2GasGC_Small_Shell_Message>		();
	RegisterClientCommand		<CGac2GasGC_Middle_Shell_Message>		();
	RegisterClientCommand		<CGac2GasGC_Tell_BeTesting>				();
	//RegisterClientCommand		<CGac2GasGC_Tell_OwnerInfo>				();
	RegisterClientCommand		<CGac2GasGC_Client_Guid>				();
	RegisterClientCommand		<CGac2GasOC_Tell_Mediator_Sync_Target_ViewVariant>();
	RegisterClientCommand		<CGac2GasOC_Tell_Mediator_Sync_Target_SyncVariant>();
}

CSynConnServer::CSynConnServer( IPipe* pPipe)
{
	m_uClientLastPingTime	= 0;
	m_uCurrentDelay			= 0;
	m_pMedObj				= NULL;
	m_bTestingConn			= false;
	m_uBlockCount			= 1;
	m_uTotalCheckedNum		= 0;
	m_nTotalTimeError		= 0;

	m_bGuidSet				= false;
	m_uModSignCounter		= 0;
	m_uLastClientKnownServerFrameTime = 0;
	m_uLastClientTime = 0;
	m_uLastDifferentServerFrameTime = 0;
	m_uDiscardedConnTime = 0;
	m_uLastConnTime = 0;
	m_uLastDifferentFrameClientTime = 0;
	m_uCurFrameDiscardedConnTime = 0;
	m_uCurFrameTimeSpanThreshold = 0;

	m_bServerTimeInited = false;
	m_iServerClientTimeDiff = -1;
	m_uServerTime = 0;
	m_uDiscardedServerTime = 0;

	m_vecTimeErr.resize(GetMaxCheckTimeInOneTurn(), 0);

	pPipe->SetHandler(this);
	Init( pPipe );

	str_guid_t szGuidConn;

	CreateStringGuid(szGuidConn);	

	SetConnValue("Conn Guid", szGuidConn);

	StringToGuid(szGuidConn, m_guidConn);
	
	CGas2GacGC_Tell_Guids guid_cmd;
	
	memcpy(guid_cmd.guidServer, GetProcessGUID(), sizeof(guid_cmd.guidServer));
	memcpy(guid_cmd.guidConnection, m_guidConn, sizeof(guid_cmd.guidConnection));
	
	SendCoreCmd(&guid_cmd);
	
	CSyncTimeSystemServer::Inst()->Register(this,60000);		// 每分钟调一次

	SetClientPingTime();

	CGas2GacGC_Tell_Server_Time cmd;
	cmd.uobGlobalTime= CSyncTimeSystemServer::Inst()->GetFrameTime();
	SendCoreCmd(&cmd);
	
	SendCurrentVersion();
	m_uID = GetSynConnMap().Add(this);

	CAddress address;
	if (GetRemoteAddress(address))
	{
		(new CCoreCreateConnServerResult(m_uID, address))->Add();

		string strRemoteAddr;
		address.GetAddressAndPort(strRemoteAddr);
		SetConnValue("Client Addr", strRemoteAddr.c_str());

		if (GetLocalAddress(address))
		{
			string strLocalAddr;
			address.GetAddressAndPort(strLocalAddr);
			SetConnValue("Server Addr", strLocalAddr.c_str());	
		}
	}
	else
	{
		ostringstream strm;
		strm <<"创建连接失败！！！"<<endl;
		CoreShutDown(strm.str().c_str());
	}
}

CSynConnServer::~CSynConnServer(void)
{
	CSyncTimeSystemServer::Inst()->UnRegister(this);
	if (m_pMedObj)
	{
		if(!m_pMedObj->IsActive())
			m_pMedObj->StopFollowing();
		if(m_pMedObj->IsMoving())
			m_pMedObj->StopMoving();
		m_pMedObj->IntSetConnection(NULL);	//这个时候网络已经删除了，但是Mediator还存在
	}

	Ver(GetSynConnMap().Del(m_uID));
	
	(new CCoreDestoryConnServerResult(m_uID))->Add();
}

void CSynConnServer::InitSynConnMap()
{
	IntGetSynConnMap() = new MapSynConn;
}

void CSynConnServer::UninitSynConnMap()
{
	delete IntGetSynConnMap();
	IntGetSynConnMap() = NULL;
}

CSynConnServer::MapSynConn*& CSynConnServer::IntGetSynConnMap()
{
	static MapSynConn* ms_pMapSynConn = NULL;
	return ms_pMapSynConn;
}

CSynConnServer::MapSynConn& CSynConnServer::GetSynConnMap()
{
	Ast(IntGetSynConnMap());
	return *IntGetSynConnMap();
}

template<typename CmdClass>
void CSynConnServer::RegisterClientCommand()
{
	CSynConnServer::SetHandler<CmdClass>(&CSynConnServer::OnClientCommandWithTimeInfo);
}

template<typename CmdClass>
void CSynConnServer::RegisterDirectorCommand()
{
	CSynConnServer::SetHandler<CmdClass>(&CSynConnServer::OnDirectorCommand<CmdClass>);
}

template<typename CmdClass>
void CSynConnServer::RegisterDirectorCommandBlockByMediator()
{
	CSynConnServer::SetHandler<CmdClass>(&CSynConnServer::OnDirectorCommandBlockByMediator<CmdClass>);
}

template<typename CmdClass>
void CSynConnServer::RegisterDirectorCommandWithTimeStamp()
{
	CSynConnServer::SetHandler<CmdClass>(&CSynConnServer::OnDirectorCommandWithTimeStamp<CmdClass>);
}

CSyncCoreObjectMediator* CSynConnServer::GetObjectMediator()const
{
	return m_pMedObj;
}

void CSynConnServer::SendShellMessage(const void* pData,uint32 uSize)
{
	if( uSize <=0xff )
	{
		typedef CGas2GacGC_Small_Shell_Message SmallMsg;
		SmallMsg cmd;
		cmd.usbDataLength = static_cast<uint8>(uSize);
		SendCoreMsg(&cmd,sizeof(cmd));
	}
	else if(uSize <= 0xffff )
	{
		typedef CGas2GacGC_Middle_Shell_Message MiddleMsg;
		MiddleMsg cmd;
		cmd.udbDataLength = static_cast<uint16>(uSize);
		SendCoreMsg(&cmd,sizeof(cmd));
	}
	else
	{
		typedef CGas2GacGC_Big_Shell_Message BigMsg;
		BigMsg cmd;
		cmd.uqbDataLength = uSize;
		SendCoreMsg(&cmd,sizeof(cmd));
	}
	SendCoreMsg(pData, uSize);
}

bool CSynConnServer::IsConnected()const
{
	return GetPipe() != NULL;
}

template<typename CmdClass>
void CSynConnServer::OnDirectorCommand(const CmdClass* pCmd,void* pParam)
{
	if( m_uBlockCount>0 )
		return;
	
	CSyncCoreObjectMediator* pMedObj=GetObjectMediator();
	Ast( pMedObj );

	UpdateClientTimeInfo(pCmd);
	pMedObj->OnClientCommand(pCmd);
}


template<typename CmdClass>
void CSynConnServer::OnDirectorCommandBlockByMediator(const CmdClass* pCmd,void* pParam)
{
	const size_t stDataSize=reinterpret_cast<const char*>(&pCmd->udbSeed) - reinterpret_cast<const char*>(pCmd);
	FastVerifyData( pCmd , stDataSize , pCmd );
	
	if( m_uBlockCount>0 )
		return;
	
	CSyncCoreObjectMediator* pMedObj=GetObjectMediator();
	
	if( !pMedObj )
		GenErr("Connection尚未绑定Mediator对象！");
	
	if( pMedObj->IsBlockingDirCmd() )
		return;

	UpdateClientTimeInfo(pCmd);
	pMedObj->OnClientCommand(pCmd);
}


template<typename CmdClass>
void CSynConnServer::OnDirectorCommandWithTimeStamp(const CmdClass* pCmd,void* pParam)
{
	if( m_uBlockCount>0 )
		return;
	
	CSyncCoreObjectMediator* pMedObj=GetObjectMediator();
	
	if( !pMedObj )
		GenErr("Connection尚未绑定Mediator对象！");
	
	UpdateClientTimeInfo(pCmd);
	pMedObj->OnClientCommand(pCmd);
}

void CSynConnServer::OnTick()
{
	uint64 uCurTime= CSyncTimeSystemServer::Inst()->GetFrameTime();
	if( (uCurTime - m_uClientLastPingTime) > CAppConfigServer::Inst()->GetConnServerTimeOut() * 1000 )
	{
		CAddress addr;
		GetRemoteAddress(addr);
		const char* szAddrIP = addr.GetAddress();

		ostringstream strm;
		strm << szAddrIP 
			<<"上次Ping时间为："<<m_uClientLastPingTime<<endl
			<<"当前时间为："<<uCurTime<<endl
			<<"空闲超时断开连接！"<<endl;
		CoreShutDown(strm.str().c_str());
		return;
	}
}

void CSynConnServer::SendCurrentVersion()
{
	CGas2GacGC_Tell_Current_Version cmd;
	cmd.uCurrentRevision = GetCurrentRevision();
	const char * str = GetCurrentVersion();
	memset(cmd.uCurrentVersion,' ',20*sizeof(char));
	for(int i=0;i<20;i++)
	{
		if(*(str+i)=='\0')
		{
			cmd.uCurrentVersion[i]='\0';
			break;
		}
		cmd.uCurrentVersion[i]=*(str+i);
	}
	SendCoreCmd(&cmd);
}

void CSynConnServer::SendOwnerInfo(const char* szKey, const char* szValue)
{
	string str;
	str.append(szKey).append(":").append(szValue);

	CGas2GacGC_Tell_OwnerInfo cmd;

	int8 len = (int8)str.size();
	cmd.usbDataLength = (int8)len + 1;

	SendCoreMsg(&cmd, sizeof(CGas2GacGC_Tell_OwnerInfo));
	SendCoreMsg(str.c_str(), (len + 1));
}

void CSynConnServer::LogOnOffLineMsg(const char* msg)
{
	CSyncAppServer::Inst()->GetConnMgr()->SetOffLineLog(msg);
}

void CSynConnServer::SetClientPingTime()
{
	m_uClientLastPingTime = CSyncTimeSystemServer::Inst()->GetFrameTime();
}

void CSynConnServer::OnClientCommand(const CGac2GasGC_Ping_Server* pCmd, void * /*pParam*/ )
{
	UpdateTimeError();

	uint32 uStepNum;
	if(m_pMedObj && !m_pMedObj->CheckStepCommandFrequence(uStepNum))
	{
		ostringstream strm;
		strm << " step num " << uStepNum << " | MaxSpeed " << m_pMedObj->m_fDirectorMaxSpeed << endl;
		//SendErrMsg(strm.str().c_str());
		//此处记录log是为了方便查看是否出现误判,等检测方法稳定后可以不用记录
		LogErr(" Step Freq Is Too High ", strm.str(), this);
		(new CCoreCheatResult(m_uID))->Add();
		//CoreShutDown(strm.str().c_str());
		//return;
	}

	SetClientPingTime();

	//------------
	m_uPingClientLastTime = CSyncTimeSystemServer::Inst()->GetFrameTime();
	CGas2GacGC_Ping_Client	ping;
	SendCoreCmd(&ping);
}

uint32 CSynConnServer::GetLatency()const
{
	return m_uCurrentDelay;
}

void CSynConnServer::OnClientCommand(const CGac2GasGC_Update_Latency_Server* pCmd, void* )
{
	m_uCurrentDelay = static_cast<uint32>((CSyncTimeSystemServer::Inst()->GetFrameTime() - m_uPingClientLastTime) / 2);
	(new CCoreConnSetLatency(m_uID, m_uCurrentDelay))->Add();
}

void CSynConnServer::OnClientCommand(const CGac2GasGC_Small_Shell_Message* pCmd, void * pParam)
{
	uint8 uSize = pCmd->usbDataLength;
	
	if( uSize == 0 )
		return;
	
	const char* pData=reinterpret_cast<const char*>(pCmd+1);

	FastVerifyData( pData,uSize,pCmd );
	
	(new CCoreDispatchShellMsgResult(m_uID, pData, uSize))->Add();
}

void CSynConnServer::OnClientCommand(const CGac2GasGC_Middle_Shell_Message* pCmd, void * pParam)
{
	uint16 uSize = pCmd->udbDataLength;
	
	if( uSize == 0 )
		return;

	const char* pData=reinterpret_cast<const char*>(pCmd+1);

	FastVerifyData( pData,uSize,pCmd );
		
	(new CCoreDispatchShellMsgResult(m_uID, pData, uSize))->Add();
}


void CSynConnServer::OnClientCommand(const CGac2GasOC_Report_Invalid_Step* pCmd,void* pParam )
{
	//UpdateCheatScore(100000);
	//GenErr("Invalid step,temp codes");
}

void CSynConnServer::OnClientCommand(const CGac2GasOC_Report_Invalid_Move* pCmd,void* pParam )
{
	//UpdateCheatScore(100000);
	//GenErr("Invalid move,temp codes");m_uBlockCount
}

void CSynConnServer::OnClientCommand(const CGac2GasGC_Main_Scene_Set* pCmd,void* pParam )
{
	--m_uBlockCount;
	if(m_pMedObj)
		m_pMedObj->ResetDirKnownMaxSpeed();
	//cout<<"Block count "<<m_uBlockCount<<endl;
}	


void CSynConnServer::OnClientCommand(const CGac2GasGC_Client_Guid* pCmd, void* pParam)
{
	if( m_bGuidSet )
		GenErr("guid of client is already set!");
	
	m_bGuidSet = true;

	str_guid_t szGuid;
	GuidToString(pCmd->guid, szGuid);
	
	char buffer[100];
	sprintf(buffer, "%s\t%s", szGuid, pCmd->szTime);

	CSynConnMgrServer::Inst()->LogClientGuidInfo(buffer);

	CSynConnMgrServer::Inst()->OnClientGuidSet(this);
}

void CSynConnServer::OnClientCommand(const CGac2GasOC_Tell_Mediator_Sync_Target_ViewVariant* pCmd, void* pParam)
{
	if(m_pMedObj)
		m_pMedObj->SyncViewVariantHolder(pCmd->uTargetGlobalID, pCmd->bSyncStaticView, pCmd->bSyncAgileView, pCmd->uSessionID);
}

void CSynConnServer::OnClientCommand(const CGac2GasOC_Tell_Mediator_Sync_Target_SyncVariant* pCmd, void* pParam)
{
	if(m_pMedObj)
		m_pMedObj->SyncSyncVariantHolder(pCmd->uTargetGlobalID, pCmd->bSyncStaticSync, pCmd->bSyncAgileSync, pCmd->uSessionID);
}

void CSynConnServer::OnClientCommand(const CGac2GasGC_Tell_BeTesting* pCmd, void* pParam)
{
	m_bTestingConn = true;

	CSynConnMgrServer* pMgr = CSyncAppServer::Inst()->GetConnMgr();
	pMgr->m_lstTestingConn.push_front(this);
	m_itTestingConn = pMgr->m_lstTestingConn.begin();

	(new CCoreSetTestingConnResult(m_uID, m_bTestingConn))->Add();
}

int32 CSynConnServer::GetCheatScore()const
{
	return m_nCheatScore;
}

void CSynConnServer::UpdateCheatScore(int32 nCheatScoreModifier)
{
	m_nCheatScore+=nCheatScoreModifier;
}

void CSynConnServer::ShutDown(const char* szMsg)
{
	if (IntShutDown())
	{
		ostringstream strm;
		CAddress addr;
		GetRemoteAddress(addr);
		const char* sUserName = GetValue("UserName");
		sUserName = sUserName?sUserName:"";
		LogTime(strm);
		strm << " account:" << sUserName<< " reason:" << szMsg << " remoteip:" << addr.GetAddress() << " localip: " << endl;
		LogOnOffLineMsg(strm.str().c_str());
	}
}

void CSynConnServer::SendErrMsg(const char* szError)
{
	if (!m_bTestingConn)
		return;

	uint32 len = (uint32)strlen(szError);

	SendErrMsg(szError, len);
}

void CSynConnServer::SendErrMsg(const char* szError, uint32 uLen)
{
	if (!m_bTestingConn)
		return;

	CGas2GacGC_Tell_Error cmd;
	cmd.udbDataLength = (uint16)(uLen + 1);

	SendCoreMsg(&cmd, sizeof(CGas2GacGC_Tell_Error));
	SendCoreMsg(szError, (uLen + 1));

}

void CSynConnServer::DoShutDownAndCallbackDerived()
{
	CSynConnMgrServer* pMgr = CSyncAppServer::Inst()->GetConnMgr();

	if (m_bTestingConn)
	{
		pMgr->m_lstTestingConn.erase(m_itTestingConn);
	}

	if (!m_bGuidSet)
	{
		pMgr->DoShutDownConn(this);
		return;
	}

	if( pMgr)
	{
		//通知逻辑层断开连接了
		(new CCoreOnDisconnectResult(m_uID))->Add();
	}

	//把所有属于这个连接的仲裁对象与连接解开绑定。
	if( m_pMedObj )
		m_pMedObj->OnConnectionShutDown();
	
	//这里会删除pipe
	pMgr->DoShutDownConn(this);
}

void CSynConnServer::SetMediator(CSyncCoreObjectMediator* pMediator)
{
	m_pMedObj = pMediator;
}

void CSynConnServer::AddConnBlockCount()
{
	m_uBlockCount++;
}

uint8 CSynConnServer::GetMaxCheckTimeInOneTurn()
{
	static uint8 ls_uMaxCheckTimeInOneTurn = 5;
	return ls_uMaxCheckTimeInOneTurn;
}

bool CSynConnServer::UpdateTimeError()
{
	//cout << m_uTotalCheckedNum << "\t updatetime error " << m_nTotalTimeError << "->";
	int32 nTimeErr = CHECK_NET_DELAY_INTERVAL - int32(CSyncTimeSystemServer::Inst()->GetFrameTime() - m_uClientLastPingTime);
	m_nTotalTimeError += nTimeErr;
	uint32 uCheckedNumInOneTurn = m_uTotalCheckedNum % GetMaxCheckTimeInOneTurn();
	m_vecTimeErr[uCheckedNumInOneTurn] = nTimeErr;
	//cout << m_nTotalTimeError << endl;
	++m_uTotalCheckedNum;

	if(uCheckedNumInOneTurn >= uint32(GetMaxCheckTimeInOneTurn() - 1))
	{
		int32 nThreshold = int32(CAppConfigServer::Inst()->GetClientTimeStampErrorThreshold());
		if(m_nTotalTimeError > nThreshold)
		{
			if(m_vecTimeErr[0]>0 && m_vecTimeErr[1] > 0 && m_vecTimeErr[2] > 0 && m_vecTimeErr[3] > 0 &&  m_vecTimeErr[4] > 0)
			{
				//cout << m_uID << " cheat " << m_nTotalTimeError << endl;
				ostringstream strm;
				strm << "总误差: " << m_nTotalTimeError << " 总检查次数: " << m_uTotalCheckedNum;
				strm << " 最近5次的误差: " << m_vecTimeErr[0] << " " << m_vecTimeErr[1] << " " << m_vecTimeErr[2] << " " << m_vecTimeErr[3] << " " << m_vecTimeErr[4] << endl;
				//SendErrMsg(strm.str().c_str());
				//此处记录log是为了方便查看是否出现误判,等检测方法稳定后可以不用记录
				LogErr("有加速外挂 - 客户端时间戳异常 ", strm.str(), this);
				(new CCoreCheatResult(m_uID))->Add();
				//CoreShutDown(strm.str().c_str());
				m_vecTimeErr.resize(GetMaxCheckTimeInOneTurn(), 0);
				return false;
			}
		}
		m_vecTimeErr.resize(GetMaxCheckTimeInOneTurn(), 0);
	}
	return true;
}

void CSynConnServer::SetConnValue(const char* szKey, const char* szValue)
{
	CLogOwner::SetValue(szKey, szValue);

	(new CCoreConnSetValueResult(m_uID, szKey, szValue))->Add();
}

//void CSynConnServer::OnClientCommand(const CGac2GasGC_Tell_OwnerInfo* pCmd, void* pParam)
//{
//	const char* pShellMsg = (const char*)pCmd + sizeof(CGac2GasGC_Tell_OwnerInfo);
//	string strMsg;
//	strMsg.append(pShellMsg, pCmd->usbDataLength);
//
//	string strKey;
//	string strValue;
//
//	CLogOwner::SetValue(strMsg.c_str(), strKey, strValue);
//
//	(new CCoreConnSetValueResult(m_uID, strKey.c_str(), strValue.c_str()))->Add();
//}


void CSynConnServer::VerifyClientKnownServerFrameTime(uint64 uClientKnownServerFrameTime)
{
	uint64 uCurFrameTime= CSyncTimeSystemServer::Inst()->GetFrameTime();

	if(uClientKnownServerFrameTime < m_uLastClientKnownServerFrameTime
		 || uClientKnownServerFrameTime > uCurFrameTime)
	{
		ostringstream strm;
		strm << uClientKnownServerFrameTime << " " << m_uLastClientKnownServerFrameTime << " " << uCurFrameTime;
		GenErr("invalid ClientKnownServerFrameTime", strm.str());
	}
	m_uLastClientKnownServerFrameTime = uClientKnownServerFrameTime;
}

uint64 CSynConnServer::GetServerTime()const
{
	//cout << "servertime " << m_uServerTime << " " << m_uDiscardedServerTime << " " << m_iServerClientTimeDiff << endl;
	return m_uServerTime - m_uDiscardedServerTime + m_iServerClientTimeDiff;
}

static const uint32 gs_ServerTimePushingThreshold = 1000;

void CSynConnServer::PushServerTime(const uint64 uCurServerFrameTime)
{
	Ast(uCurServerFrameTime >= m_uServerTime);
	const uint32 uDiff = uint32(uCurServerFrameTime - m_uServerTime);
	m_uServerTime = uCurServerFrameTime;

	if(uDiff > gs_ServerTimePushingThreshold)
	{
		m_uDiscardedServerTime += uDiff - gs_ServerTimePushingThreshold;
	}
}

void CSynConnServer::UpdateClientTimeInfo(uint64 uClientKnownServerFrameTime, uint64 uClientTime)
{
	VerifyClientKnownServerFrameTime(uClientKnownServerFrameTime);

	if(uClientTime < m_uLastClientTime)
	{
		GenErr("uClientTime < m_uLastClientTime");
	}

	const uint64 uCurServerFrameTime = CSyncTimeSystemServer::Inst()->GetFrameTime();

	if(!m_bServerTimeInited)
	{
		m_iServerClientTimeDiff = int32(uClientTime - uCurServerFrameTime);
		m_bServerTimeInited = true;
	}

	if(m_uLastDifferentServerFrameTime == 0)
	{
		m_uServerTime = uCurServerFrameTime;
		m_uLastDifferentServerFrameTime = uCurServerFrameTime;
		m_uLastClientTime = uClientTime;
		m_uLastDifferentFrameClientTime = uClientTime;
		return;
	}

	PushServerTime(uCurServerFrameTime);

	uint32 uClientTimeSpan;
	uint32 uTimeDiffThreshold;

	if(m_uLastDifferentServerFrameTime != uCurServerFrameTime)
	{
		m_uDiscardedConnTime += m_uCurFrameDiscardedConnTime;
		//cout << "diff frame " << m_uDiscardedConnTime << " "  << m_uCurFrameDiscardedConnTime << endl;
		m_uCurFrameDiscardedConnTime = 0;

		m_uLastDifferentFrameClientTime = m_uLastClientTime;

		Ast(uClientTime >= m_uLastDifferentFrameClientTime);
		uClientTimeSpan = uint32(uClientTime - m_uLastDifferentFrameClientTime);
		m_uCurFrameTimeSpanThreshold = min(uint32(uCurServerFrameTime - m_uLastDifferentServerFrameTime + CSyncTimeSystemServer::Inst()->GetBaseCyc()), CSynConnMgrServer::Inst()->GetInterval() / 2);
		m_uLastDifferentServerFrameTime = uCurServerFrameTime;
	}
	else
	{
		if(m_uCurFrameDiscardedConnTime != 0)
		{
			uint32 uDiscardedTime = uint32(uClientTime - m_uLastClientTime);
			if(uDiscardedTime < 500)
				uDiscardedTime = 0;
			m_uCurFrameDiscardedConnTime += uDiscardedTime;
			m_uLastClientTime = uClientTime;
			return;
		}
		Ast(uClientTime >= m_uLastDifferentFrameClientTime);
		uClientTimeSpan = uint32(uClientTime - m_uLastDifferentFrameClientTime);
		uTimeDiffThreshold = uint32(uCurServerFrameTime - m_uLastDifferentServerFrameTime);
	}

	if(uClientTimeSpan > m_uCurFrameTimeSpanThreshold)
	{
		uint32 uDiscardedTime = uint32(uClientTimeSpan - m_uCurFrameTimeSpanThreshold);
		if(uDiscardedTime < 500)
			uDiscardedTime = 0;
		m_uCurFrameDiscardedConnTime = uDiscardedTime;
		//cout << "uClientTimeSpan " << uClientTimeSpan << " threshold " << uTimeDiffThreshold << " dis " << m_uDiscardedConnTime << endl;
	}

	m_uLastClientTime = uClientTime;
}

uint64 CSynConnServer::GetClientTime()const
{
	return m_uLastClientTime;
}

uint64 CSynConnServer::GetConnTime()const
{
	uint64 uServerTime = GetServerTime();
	//控制client process time 递增速度
	//cout << " (" << uServerTime << "," << m_uLastClientTime - m_uDiscardedConnTime - m_uCurFrameDiscardedConnTime << ")" << min(GetServerTime(), m_uLastClientTime - m_uDiscardedConnTime - m_uCurFrameDiscardedConnTime) << " ";
	//cout << m_uLastClientTime << " " << m_uDiscardedConnTime << " " << m_uCurFrameDiscardedConnTime << " " << m_uCurFrameTimeSpanThreshold << endl;
	uint64 uConnTime = min(uServerTime, m_uLastClientTime - m_uDiscardedConnTime - m_uCurFrameDiscardedConnTime);
	m_uLastConnTime = max(uConnTime, m_uLastConnTime);
	return m_uLastConnTime;
}

uint64 CSynConnServer::GetClientKnownServerFrameTime()const
{
	return m_uLastClientKnownServerFrameTime;
}

void CSynConnServer::FastVerifyData(const void* pData,size_t stDataSize,uint8 usbCounter,uint16 udbSeed,uint8 usbModCheckSum)
{
	if( usbCounter != m_uModSignCounter )
	{
		ostringstream strm;
		strm<<uint32( usbCounter) <<","<< uint32( m_uModSignCounter );
		GenErr("Invalid count number while processing modsigned message!",strm.str() );
	}

	if( !sqr::FastVerifyData( m_guidConn,pData,stDataSize,usbCounter,udbSeed,usbModCheckSum ) )
	{
		GenErr("Invalid checksum while processing modsigned message!");
	}

	++m_uModSignCounter;
}

template<typename CmdType>
void CSynConnServer::FastVerifyData(const void* pData,size_t stDataSize,const CmdType* pCmd)
{
	//cout<<typeid(CmdType).name()<<uint32(pCmd->usbCounter)<<","<<uint32(m_uModSignCounter)<<","<<uint32(pCmd->udbSeed)<<endl;
	FastVerifyData(pData, stDataSize, pCmd->usbCounter, pCmd->udbSeed,pCmd->usbModCheckSum);
}


template<typename CmdClass>
void CSynConnServer::UpdateClientTimeInfo(const CmdClass* pCmd)
{
	//cout << typeid(CmdClass).name() << " UpdateClientTimeInfo" << endl;
	UpdateClientTimeInfo(pCmd->uobClientKnownServerFrameTime, pCmd->uobClientTime);
}

template<typename CmdClass>
void CSynConnServer::OnClientCommandWithTimeInfo(const CmdClass* pCmd, void* pParam)
{
	UpdateClientTimeInfo(pCmd);
	OnClientCommand(pCmd, pParam);
}

CLogOwner* CSynConnServer::GetLogOwner()
{
	return this;
}
