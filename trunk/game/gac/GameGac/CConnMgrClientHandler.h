#pragma once
#include "CBaseConnMgrHandler.h"
#include "CConnClient.h"
#include "IScriptConnMgrClientHandler.h"
#include "IConnMgrClientHandler.h"

class CClientFighterMsgDispatcher;
class CClientCharacterMsgDispatcher;
class CClientApexMsgDispatcher;

class CConnMgrClientHandler 	
	: public CBaseConnMgrHandler<CConnClient, IScriptConnMgrClientHandler>
	, public IConnMgrClientHandler
{
public:
	CConnMgrClientHandler();
	~CConnMgrClientHandler();

	void OnDataReceived(CConnClient *pConn);
	void OnConnected(CConnClient* pConn);
	void OnConnectFailed(CConnClient* pConn,EConnConnectFailedReason eReason);
	void OnDisconnect(CConnClient* pConn);		
	void OnError(CConnClient* pConn,const TCHAR* szError);

private:
	CClientFighterMsgDispatcher *m_pFighterMsgDispatcher;
	CClientCharacterMsgDispatcher *m_pCharacterMsgDispatcher;
	CClientApexMsgDispatcher * m_pApexMsgDispatcher;
};
