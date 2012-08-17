#pragma once

namespace sqr
{
	class CConnServer;

	class IConnMgrServerHandler
		:public virtual CDynamicObject
	{
	public:
		virtual void OnAccepted(CConnServer* pConn){};
		virtual void OnDisconnect(CConnServer* pConn){} ;		//函数返回之前，pConn都是完全可用的，并且仍然处于Connected状态
		virtual void OnDataReceived(CConnServer* pConn){};
		virtual void OnError(CConnServer* pConn,const char* szError){};
		virtual void OnBeginServiceFailed(const char* szErrMsg){};
		virtual void OnBeginServiceSucceeded(){};
		virtual void OnServiceEnded(){};
		virtual void OnCheat(CConnServer* pConn){cout << "IConnMgrServerHandler::OnCheat" << endl;}
	};
}

