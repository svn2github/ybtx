#pragma once
#include "CSyncPipe.h"
#include "CPipeCallback.h"
#include "CSyncPipeReactor.h"


inline bool CSyncPipe::IsSyncCallback()const
{
	return !m_pReactor->m_pMQMsgMgr;
}

inline void CSyncPipe::AddEventNewDataReceived()
{
	if( IsSyncCallback() )
	{
		m_pReactor->m_bHasEvent=true;
		m_pHandler->OnDataReceived();
		return;
	}
	new (this) COnNewDataReceivedCallback(this);
}


inline void CSyncPipe::AddEventSomeDataSent(uint32 uSentSize)
{
	if( IsSyncCallback() )
	{
		m_pReactor->m_bHasEvent=true;
		m_pHandler->OnDataSent( uSentSize );
		return;
	}
	new (this)  COnSomeDataSentCallback(this, uSentSize);
}

inline void CSyncPipe::AddEventConnected()
{
	if( IsSyncCallback() )
	{
		m_pReactor->m_bHasEvent=true;
		m_pHandler->OnConnected();
		return;
	}
	new (this)  COnConnectedCallback(this) ;
}


inline void CSyncPipe::AddEventListenFailed()
{
	if( IsSyncCallback() )
	{
		m_pReactor->m_bHasEvent=true;
		m_pHandler->OnListenFailed();
		return;
	}
	new (this)  COnListenFailedCallback(this) ;
}

inline void CSyncPipe::AddEventListenSuccess()
{
	if( IsSyncCallback() )
	{
		m_pReactor->m_bHasEvent=true;
		m_pHandler->OnListenSuccessed();
		return;
	}
	new (this) COnListenSuccessedCallback(this);
}


inline void CSyncPipe::AddEventDisconnected(EPipeDisconnectReason eReason,bool bGracefully)
{
	if( IsSyncCallback() )
	{
		m_pReactor->m_bHasEvent=true;
		m_pHandler->OnDisconnected( eReason,bGracefully );
		return;
	}
	new (this) COnDisconnectedCallback(this,eReason,bGracefully); 
}


inline void CSyncPipe::AddEventConnectFailed(EPipeConnFailedReason eReason)
{
	if( IsSyncCallback() )
	{
		m_pReactor->m_bHasEvent=true;
		m_pHandler->OnConnectFailed( eReason );
		return;
	}
	new (this)  COnConnectFailedCallback(this,eReason) ;
}


inline void CSyncPipe::AddEventAccepted(CSyncPipe* pNewPipe)
{
	if( IsSyncCallback() )
	{
		m_pReactor->m_bHasEvent=true;
		m_pHandler->OnAccepted( pNewPipe );
		return;
	}
	new (this)  COnAcceptedCallback(this, pNewPipe) ;
}

inline void CSyncPipe::AddEventSendBufferFree()
{
	if( IsSyncCallback() )
	{
		m_pReactor->m_bHasEvent=true;
		m_pHandler->OnSendBufferFree();
		return;
	}
	new (this)  COnSendBufferFreeCallback(this);
}


inline bool CSyncPipe::NeedSomeDataSentCallback()const
{
	return (m_uCreationFlag & ePCF_DataSentCallback) != 0;
}

