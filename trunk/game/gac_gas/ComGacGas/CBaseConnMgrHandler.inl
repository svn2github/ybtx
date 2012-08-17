#pragma once 
#include "TMsgDispatcher.h"
#include "CBaseConnMgrHandler.h"
#include "BaseHelper.h"
#include "ExpHelper.h"

template <typename Conn, typename ScriptHandler>
CBaseConnMgrHandler <Conn, ScriptHandler>::CBaseConnMgrHandler()
: m_pScriptHandler(NULL)
{

}


template<typename Conn, typename ScriptHandler>
CBaseConnMgrHandler<Conn, ScriptHandler>::~CBaseConnMgrHandler()
{
	
}

template<typename Conn, typename ScriptHandler>
void CBaseConnMgrHandler<Conn, ScriptHandler>::SetScriptHandler(ScriptHandler *pScriptHandler)
{
	AdvDelRef(m_pScriptHandler);
	m_pScriptHandler = pScriptHandler;
	AdvAddRef(m_pScriptHandler);
}

template<typename Conn, typename ScriptHandler>
ScriptHandler* CBaseConnMgrHandler<Conn, ScriptHandler>::GetScriptHandler() const
{
	return m_pScriptHandler;
}

template<typename Conn, typename ScriptHandler>
template < typename Dispatcher >
bool CBaseConnMgrHandler<Conn, ScriptHandler>::DoDispatch(Dispatcher *pMsgDispatcher, Conn *pConn, char* pBuf)
{
	pMsgDispatcher->SetCurDispatchConn(pConn);

	uint32 uProcessSize;
	bool bRet = false;
	switch(pMsgDispatcher->Dispatch(reinterpret_cast<void*>(pBuf),uint32( pConn->GetRecvDataSize() ), NULL, uProcessSize, true)) 
	{
	case eSuccess:
		bRet = true;
		pConn->PopRecvData(uProcessSize);
		break;
	case eCanceled:
	case eCallAgain:
		break;
	case eHandlerOver:
	case eHandlerNotFound:
	case eCallError:
	default:
		GenErr("Corrupted receive buffer.");
	}

	pMsgDispatcher->SetCurDispatchConn(NULL);
	return bRet;
}
