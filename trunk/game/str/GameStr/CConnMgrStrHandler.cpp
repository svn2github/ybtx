#include "stdafx.h"
#include "CConnMgrStrHandler.h"
#include "BaseHelper.h"
#include "CStrMsgDispatcher.h"
#include "CBaseMsgDispatcher.inl"
#include "CBaseConnMgrHandler.inl"
#include "IRpcCommon.h"
#include "TMsgDispatcher_inl.inl"
#include "TMsgHandler.inl"
#include "TComMsgDispatcher.inl"


CConnMgrStressHandler::CConnMgrStressHandler()
	: m_pFighterMsgDispatcher(new CStressFighterMsgDispatcher)
	, m_pCharacterMsgDispatcher(new CStressCharacterMsgDispatcher)
{
	CStressFighterMsgDispatcher::InitMsgHandlerContainer();
	CStressFighterMsgDispatcher::RegisterMsgHandler();
	CStressCharacterMsgDispatcher::InitMsgHandlerContainer();
	CStressCharacterMsgDispatcher::RegisterMsgHandler();
}

CConnMgrStressHandler::~CConnMgrStressHandler()
{
	SafeDelete(m_pFighterMsgDispatcher);
	SafeDelete(m_pCharacterMsgDispatcher);

	CStressFighterMsgDispatcher::UninitMsgHandlerContainer();
	CStressCharacterMsgDispatcher::UninitMsgHandlerContainer();
}

void CConnMgrStressHandler::OnDataReceived(CConnStress *pConn)
{
	
	while(pConn->GetRecvDataSize() >= 2)
	{
		char *pBuf = reinterpret_cast<char *>(pConn->GetRecvData());
		size_t dataSize = pConn->GetRecvDataSize();

		uint16 id = *(uint16*)(pBuf);

		//如果协议号小于32767，则是lua发送的rpc协议，转移到lua处理，否则则在c++里面分发
		if(id < eLua_CPP_Split_ID)
		{
			SQR_TRY{
				GetScriptHandler()->OnDataReceived(pConn);
			}
			//此处接到的异常只可能是调用rpc函数时函数里面出现问题导致的
			//其他的异常情况我们在unpack的时候就已经断开连接了
			SQR_CATCH(exp){
				clog << exp.ErrorTitle() << endl << exp.ErrorMsg() << endl;
				clog << exp.StackInfo() << endl;
				pConn->OnCaughtNetException();
			}
			SQR_TRY_END;

			//如果相等，则证明数据长度不够，我们需要继续等待，才在做处理
			if(dataSize == pConn->GetRecvDataSize())
				break;
		}
		else
		{
			SQR_TRY
			{
				if (id > eGas2GacCPP_Fighter_Begin && id < eGas2GacCPP_Fighter_End)
				{
					if(!DoDispatch(m_pFighterMsgDispatcher, pConn, pBuf))
						break;
				}
				else if (id > eGas2GacCPP_Character_Begin && id < eGas2GacCPP_Character_End)
				{
					if(!DoDispatch(m_pCharacterMsgDispatcher, pConn, pBuf))
						break;
				}
				else if (id > eGas2GacCpp_ApexProxyMsg_Begin && id < eGas2GacCpp_ApexProxyMsg_End) 
				{
					pConn->PopRecvData(dataSize); //压力测试,不用开ApexServer,否则会被踢
					break;
				}
				else
				{
					std::ostringstream oss;
					oss << "Invalid Msg ID:" << id;
					GenErr(oss.str());
				}
			}
			//当发生消息错位这些异常的时候是不可能忽略的，所以我们直接断线
			SQR_CATCH(exp) {
				clog << exp.ErrorTitle() << endl << exp.ErrorMsg() << endl;
				clog << exp.StackInfo() << endl;
				pConn->ShutDown();
				break;
				
			}
			SQR_TRY_END;
		}
	}

}

void CConnMgrStressHandler::OnConnected(CConnStress* pConn)
{
	GetScriptHandler()->OnConnected(pConn);
}

void CConnMgrStressHandler::OnConnectFailed(CConnStress* pConn)
{
	GetScriptHandler()->OnConnectFailed(pConn);
}		

void CConnMgrStressHandler::OnDisconnect(CConnStress* pConn)
{
	GetScriptHandler()->OnDisconnect(pConn);
}		

void CConnMgrStressHandler::OnError(CConnStress* pConn,const char* szError)
{
	GetScriptHandler()->OnError(pConn, szError);
}



