 #include "stdafx.h"
#include "CApexProxy.h"
#include "PtrlGas2GacCPPDef.h"
#include "CAppServer.h"
#include "PtrlGac2GasCPPDef.h"
#include "ErrLogHelper.h"
#include "IApexProxyCallBackHandler.h"
#include "BaseHelper.h"

#ifdef _WIN32
#include <WinSock2.h>
#else
#include <arpa/inet.h>
#endif

CApexProxy* CApexProxy::ms_pInst = NULL;

CApexProxy::CApexProxy()
:m_pApexProxyCallBackHandler(NULL)
{
	m_ApexConnHandler = new CApexConnHandler();
	m_ApexLog = CLog::CreateLog("Apex.log");
	m_ApexLog->EnableTime(true);
}

CApexProxy::~CApexProxy()
{
	delete m_ApexConnHandler;
	m_ApexLog->Release();
}

void CApexProxy::CreateInst()
{
	Ast(ms_pInst == NULL);
	ms_pInst = new CApexProxy;
}

void CApexProxy::DestroyInst()
{
	Ast( ms_pInst );
	delete ms_pInst;
	ms_pInst = NULL;
}

uint32 CApexProxy::NoticeApexProxy_UserLogin(uint32 nSendId, TCHAR* PlayAccount, CConnServer* conn)
{
	//要保证,每一个玩家,进入的时候,都调用这个函数;
	//Apex Proxy 只允许一个玩家 只调用 1 次 这个函数;
	ConnectionIDMap::iterator iter = m_ConnIDMap.find(nSendId);
	if (iter != m_ConnIDMap.end())
	{
		//顶号时有延时Tick,这里直接清空上一个Login
		NoticeApexProxy_UserLogout(nSendId,PlayAccount);
	}
	if (!conn)
		return 0;

	if(m_ApexConnHandler)
	{
		string AccountName = PlayAccount;
		SendToItemServer('L',nSendId,PlayAccount,AccountName.length()); //'L',UserId, UserName, USerName_len
		CAddress addr;
		conn->GetRemoteAddress(addr);
		if(!(&addr))
			return 2;

		TCHAR sBuffer[5];
		sBuffer[0] = 1;
		*(reinterpret_cast<int32*>(sBuffer+1)) = inet_addr(addr.GetAddress());
		SendToItemServer('S', nSendId, (TCHAR*)sBuffer,5);
		if (!m_ConnIDMap.insert(make_pair(nSendId, conn->GetGlobalID())).second)
			return false;
	}
	return 1;
}

uint32	CApexProxy::NoticeApexProxy_UserLogout(uint32 nSendId, TCHAR* PlayAccount)
{
	//要保证,每一个玩家,离开的时候,都调用这个函数;
	//Apex Proxy 允许一个玩家 调用几次 这个函数;
	string AccountName = PlayAccount;
	SendToItemServer('G',nSendId,PlayAccount,AccountName.length());
	ConnectionIDMap::iterator iter = m_ConnIDMap.find(nSendId);
	if (iter != m_ConnIDMap.end())
		m_ConnIDMap.erase(iter);
	return 1;
}

CConnServer* CApexProxy::GetConnctionById(uint32 uId)
{
	ConnectionIDMap::iterator iter = m_ConnIDMap.find(uId);
	if (iter == m_ConnIDMap.end())
		return NULL;
	return CConnServer::GetConnServer(iter->second);
}


uint32 CApexProxy::GetIdBuConnction(CConnServer* conn)
{
	ConnectionIDMap::iterator iter = m_ConnIDMap.begin();
	for (; iter!=m_ConnIDMap.end(); iter++)
	{
		if (iter->second == conn->GetGlobalID())
			return iter->first;
	}
	return 0;
}

uint32 CApexProxy::NoticeApexProxy_UserData(uint32 nSendId, char* pBuf, uint32 nBufLen)
{
	SendToItemServer('T',nSendId,(TCHAR*)(pBuf),nBufLen);
	return 0;
}

void CApexProxy::SendToItemServer(char MsgID, uint32 nSendId, TCHAR* pBuf, uint32 nBufLen)
{
	SQR_TRY
	{
		if(m_ApexConnHandler)
		{
			TCHAR sBuffer[9];
			sBuffer[4] = MsgID;
			*(reinterpret_cast<int32*>(sBuffer+HEAD_PACKETLEN_SIZE+HEAD_MSGID_SIZE)) = (int32)nSendId;
			*(reinterpret_cast<int32*>(sBuffer)) = nBufLen+9;

			m_ApexConnHandler->Send(sBuffer, HEAD_PACKETLEN_SIZE + HEAD_MSGID_SIZE + HEAD_SENDID_SIZE);
			m_ApexConnHandler->Send(pBuf, nBufLen);
		}
	}
	SQR_CATCH(exp)
	{
		LogExp(exp);
	}
	SQR_TRY_END;
}

uint32 CApexProxy::RecvConnData(void* pData, size_t uSize)
{
	uint32 uHEAD_Size = HEAD_PACKETLEN_SIZE + HEAD_MSGID_SIZE + HEAD_SENDID_SIZE;
	char* pRecvData = reinterpret_cast<char*>(pData);
	uint32 nLen = *(int*)(pRecvData);
	if (nLen > uSize) //长度不够
	{
		ostringstream strm;
		strm<<"In Recv fun ApexMsg Length is wrong  "<< nLen << " uSize " << uSize <<endl;
		m_ApexLog->Write(strm.str().c_str());
		return 0;
	}

	char MsgID = *(pRecvData+HEAD_PACKETLEN_SIZE);
	uint32 nSendID = *(int*)(pRecvData+HEAD_PACKETLEN_SIZE+HEAD_MSGID_SIZE);
	
	if (MsgID == 'K')
	{
		if (uSize < uHEAD_Size+4)
			return 0;
		uint32 Action = *(int*)(pRecvData+uHEAD_Size);
		GameServerKillUser(nSendID,Action);
	}
	else if(MsgID == 'D')
	{
		uint32 uBufferLen = nLen - uHEAD_Size;
		NetSendToGameClient(nSendID,pRecvData+uHEAD_Size,uBufferLen);
	}
	return nLen;
}

void CApexProxy::OnApexProxyCommand(const CGac2GasCPP_ApexProxyMsg* pCmd, CConnServer* conn)
{
	uint32 userAccountId = GetIdBuConnction(conn);
	if (userAccountId != 0)
	{
		NoticeApexProxy_UserData(userAccountId, (char*)(pCmd+1), (uint32)pCmd->uLen);
	}
}

void CApexProxy::NetSendToGameClient(uint32 nSendId,TCHAR* pBuffer,uint32 uSize)
{
	SQR_TRY
	{
		CGas2GacCpp_ApexProxyMsg cmd;	
		cmd.uLen = (uint16)uSize;
		CConnServer* pConn = const_cast<CApexProxy*>(CApexProxy::GetInst())->GetConnctionById(nSendId);
		if (pConn)
		{
			pConn->SendCmd(&cmd);
			pConn->Send(pBuffer,uSize);
		}
	}
	SQR_CATCH(exp)
	{
		LogExp(exp);
	}
	SQR_TRY_END;
};

void CApexProxy::GameServerKillUser(uint32 nSendId,uint32 Action)
{
	CConnServer* pConn = const_cast<CApexProxy*>(CApexProxy::GetInst())->GetConnctionById(nSendId);
	if (pConn)
	{
		if (GetApexProxyCallBackHandler())
		{
			GetApexProxyCallBackHandler()->OnNoticeBeKilledUser(pConn);
		}
		pConn->ShutDown("Apex Kill");
	}
};

void CApexProxy::StartApexProxy(const CAddress& addr)
{
	m_ApexConnHandler->StartApexProxy(addr);
}

void CApexProxy::StopApexProxy()
{
	if (m_ApexConnHandler)
	{
		m_ApexConnHandler->StopApexProxy();
	}
}

void CApexProxy::SetApexProxyCallBackHandler(IApexProxyCallBackHandler* pApexProxyCallBackHandler)
{
	AdvAddRef(pApexProxyCallBackHandler);
	AdvDelRef(m_pApexProxyCallBackHandler);
	m_pApexProxyCallBackHandler = pApexProxyCallBackHandler;

}

IApexProxyCallBackHandler* CApexProxy::GetApexProxyCallBackHandler()
{
	return m_pApexProxyCallBackHandler;
}
