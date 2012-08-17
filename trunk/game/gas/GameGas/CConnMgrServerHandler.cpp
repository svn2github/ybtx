#include "stdafx.h"
#include "CConnMgrServerHandler.h"
#include "PtrlGac2GasCPPDcl.h"
#include "IRpcCommon.h"
#include "CBaseMsgDispatcher.inl"
#include "CBaseConnMgrHandler.inl"
#include "CServerFighterMsgDispatcher.h"
#include "CServerCharacterMsgDispatcher.h"
#include "CServerApexProxyMsgDispatcher.h"
#include "TMsgDispatcher.inl"
#include "ExpHelper.h"
#include "ErrLogHelper.h"
#include "CExpCounter.h"

CConnMgrServerHandler::CConnMgrServerHandler()
	: m_pFighterMsgDispatcher(new CServerFighterMsgDispatcher)
	, m_pCharacterMsgDispatcher(new CServerCharacterMsgDispatcher)
	, m_pApexproxyMsgDispatcher(new CServerApexProxyMsgDispatcher)
{
	CServerFighterMsgDispatcher::InitMsgHandlerContainer();
	CServerFighterMsgDispatcher::RegisterMsgHandler();
	CServerCharacterMsgDispatcher::InitMsgHandlerContainer();
	CServerCharacterMsgDispatcher::RegisterMsgHandler();
	CServerApexProxyMsgDispatcher::InitMsgHandlerContainer();
	CServerApexProxyMsgDispatcher::RegisterMsgHandler();
}

CConnMgrServerHandler::~CConnMgrServerHandler()
{
	SafeDelete(m_pFighterMsgDispatcher);
	SafeDelete(m_pCharacterMsgDispatcher);
	SafeDelete(m_pApexproxyMsgDispatcher);

	CServerFighterMsgDispatcher::UninitMsgHandlerContainer();
	CServerCharacterMsgDispatcher::UninitMsgHandlerContainer();
	CServerApexProxyMsgDispatcher::UninitMsgHandlerContainer();
}


void CConnMgrServerHandler::OnDataReceived(CConnServer *pConn)
{
	while ( pConn->GetRecvDataSize() >= 2 )
	{
		char *pBuf = reinterpret_cast<char *>(pConn->GetRecvData());
		size_t dataSize = pConn->GetRecvDataSize();

		uint16 id = *(uint16*)(pBuf);

		//如果协议号小于32767，则是lua发送的rpc协议，转移到lua处理，否则则在c++里面分发
		if ( id < eLua_CPP_Split_ID )
		{
			SQR_TRY
			{
				if( pConn->IsShuttingDown() )
					break;

				GetScriptHandler()->OnDataReceived(pConn);
			}
			//此处接到的异常只可能是调用rpc函数时函数里面出现问题导致的
			//其他的异常情况我们在unpack的时候就已经断开连接了
			SQR_CATCH(exp)
			{
				LogExp(exp, pConn);
			}
			SQR_TRY_END;
			//如果相等，则证明数据长度不够，我们需要继续等待，才在做处理
			if (dataSize == pConn->GetRecvDataSize())
				break;
		}
		else
		{
			if (id > eGac2GasCPP_Fighter_Begin && id < eGac2GasCPP_Fighter_End)
			{
				if (!DoDispatch(m_pFighterMsgDispatcher, pConn, pBuf))
					break;
			}
			else if (id > eGac2GasCPP_Character_Begin && id < eGac2GasCPP_Character_End)
			{
				if (!DoDispatch(m_pCharacterMsgDispatcher, pConn, pBuf))
					break;
			}
			else if (id > eGac2GasCPP_ApexProxy_Begin && id < eGac2GasCPP_ApexProxy_End)
			{
				if (!DoDispatch(m_pApexproxyMsgDispatcher, pConn, pBuf))
					break;
			}
			else
			{
				std::ostringstream oss;
				oss << id;
				GenErr("Invalid Msg ID", oss.str());
			}			
		}
	}
}

void CConnMgrServerHandler::OnAccepted(CConnServer* pConn)
{
	GetScriptHandler()->OnAccepted(pConn);
}

void CConnMgrServerHandler::OnDisconnect(CConnServer* pConn)
{
	GetScriptHandler()->OnDisconnect(pConn);
}

void CConnMgrServerHandler::OnError(CConnServer* pConn,const char* szError)
{
	GetScriptHandler()->OnError(pConn, szError);
}

void CConnMgrServerHandler::OnBeginServiceSucceeded()
{
	GetScriptHandler()->OnBeginServiceSucceeded();
}

void CConnMgrServerHandler::OnBeginServiceFailed(const char* szErrMsg)
{
	GetScriptHandler()->OnBeginServiceFailed(szErrMsg);
}

void CConnMgrServerHandler::OnServiceEnded()
{
	GetScriptHandler()->OnServiceEnded();
}

void CConnMgrServerHandler::OnCheat(CConnServer* pConn)
{
	GetScriptHandler()->OnCheat(pConn);
}
