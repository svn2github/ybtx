#pragma once

#include "CBaseMsgDispatcher.h"

template<typename T, typename Conn>
bool CBaseMsgDispatcher<T, Conn>::DispatchIsCanceled() const
{
	return m_pCurrentConn->IsShuttingDown();
}

template<typename T, typename Conn>
void CBaseMsgDispatcher<T, Conn>::SetCurDispatchConn(Conn *pConn)
{
	m_pCurrentConn = pConn; 
}

template<typename T, typename Conn>
void CBaseMsgDispatcher<T, Conn>::OnCaughtNetException()
{
}

template<typename T, typename Conn>
void CBaseMsgDispatcher<T, Conn>::SendErrMsg(const char* szError)
{
	m_pCurrentConn->SendErrMsg(szError);
}

template<typename T, typename Conn>
CLogOwner* CBaseMsgDispatcher<T, Conn>::GetLogOwner()
{
	return m_pCurrentConn;
}
