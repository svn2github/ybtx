#pragma once
#include "IConnMgrStressHandler.h"
#include "CBaseConnMgrHandler.h"
#include "CConnStress.h"
#include "IScriptConnMgrStressHandler.h"
#include "CStrCharacterMsgDispatch.h"
#include "CStrFighterMsgDispatch.h"

class CStressFighterMsgDispatcher;
class CStressCharacterMsgDispatcher;

class CConnMgrStressHandler 	
	: public IConnMgrStressHandler 
	, public CBaseConnMgrHandler<CConnStress, IScriptConnMgrStressHandler>
{
public:
	CConnMgrStressHandler();
	~CConnMgrStressHandler();

	void OnDataReceived(CConnStress *pConn);

	void OnConnected(CConnStress* pConn);
	void OnConnectFailed(CConnStress* pConn);
	void OnDisconnect(CConnStress* pConn);		
	void OnError(CConnStress* pConn,const char* szError);

private:
	CStressFighterMsgDispatcher *m_pFighterMsgDispatcher;
	CStressCharacterMsgDispatcher *m_pCharacterMsgDispatcher;
};
