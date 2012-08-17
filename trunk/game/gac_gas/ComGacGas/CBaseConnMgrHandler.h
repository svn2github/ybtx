#pragma once 
#include "CDynamicObject.h"
#include "CConnMsgMallocObject.h"
	
template < typename Conn, typename ScriptHandler >
class CBaseConnMgrHandler : public virtual CDynamicObject , public CConnMsgMallocObject
{
public:
	CBaseConnMgrHandler();
	virtual ~CBaseConnMgrHandler();
	void SetScriptHandler(ScriptHandler *pScriptHandler);
	ScriptHandler* GetScriptHandler() const;

protected:
	template < class Dispatcher >
	bool DoDispatch(Dispatcher *pMsgDispatcher, Conn *pConn, TCHAR* pBuf);

private:
	Conn *m_pCurrentConn;		
	ScriptHandler *m_pScriptHandler;
};
