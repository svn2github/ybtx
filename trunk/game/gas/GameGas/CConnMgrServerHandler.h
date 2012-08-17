#pragma once
#include "CBaseConnMgrHandler.h"
#include "IConnMgrServerHandler.h"
#include "IScriptConnMgrServerHandler.h"

namespace sqr
{
	class CConnServer;
	class CExpCounter;
}

class CServerFighterMsgDispatcher;
class CServerCharacterMsgDispatcher;
class CServerApexProxyMsgDispatcher;

class CConnMgrServerHandler 	
	: public CBaseConnMgrHandler<CConnServer, IScriptConnMgrServerHandler>
	, public IConnMgrServerHandler 
{
public:
	CConnMgrServerHandler();
	~CConnMgrServerHandler();

	virtual void OnDataReceived(CConnServer* pConn);
	virtual void OnAccepted(CConnServer* pConn);
	virtual void OnDisconnect(CConnServer* pConn);		
	virtual void OnError(CConnServer* pConn, const TCHAR* szError);
	virtual void OnBeginServiceSucceeded();
	virtual void OnBeginServiceFailed(const char* szErrMsg);
	virtual void OnServiceEnded();
	virtual void OnCheat(CConnServer* pConn);
private:
	CServerFighterMsgDispatcher*	m_pFighterMsgDispatcher;
	CServerCharacterMsgDispatcher*	m_pCharacterMsgDispatcher;
	CServerApexProxyMsgDispatcher*	m_pApexproxyMsgDispatcher;
};
