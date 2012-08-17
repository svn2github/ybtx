#include "stdafx.h"
#include "CPipeCallback.h"
#include "CSyncPipeReactor.h"
#include "CAsynPipeHandler.h"

CPipeCallback::CPipeCallback(CPtMQMsgOwner* pOwner)
:CPtMQMsg(pOwner)
{
}

CPipeCallback::~CPipeCallback()
{
}

void CPipeCallback::Dispatch(CPtMQMsgOwner* pOwner)
{
	Callback(class_cast<CSyncPipe*>(pOwner)->GetHandler());
}

COnNewDataReceivedCallback::COnNewDataReceivedCallback(CPtMQMsgOwner* pOwner)
:CPipeCallback(pOwner)
{
}

void COnNewDataReceivedCallback::Callback(IPipeHandler* pHandler)
{
	pHandler->OnDataReceived();
}

COnSomeDataSentCallback::COnSomeDataSentCallback(CPtMQMsgOwner* pOwner, uint32 uSentSize)
:CPipeCallback(pOwner)
,m_uSentSize(uSentSize)
{
}

void COnSomeDataSentCallback::Callback(IPipeHandler* pHandler)
{
	pHandler->OnDataSent(m_uSentSize);
}


COnSendBufferFreeCallback::COnSendBufferFreeCallback(CPtMQMsgOwner* pOwner)
:CPipeCallback(pOwner)
{
}

void COnSendBufferFreeCallback::Callback(IPipeHandler* pHandler)
{
	pHandler->OnSendBufferFree();
}

COnConnectedCallback::COnConnectedCallback(CPtMQMsgOwner* pOwner)
:CPipeCallback(pOwner)
{
}

void COnConnectedCallback::Callback(IPipeHandler* pHandler)
{
	pHandler->OnConnected();
}


COnDisconnectedCallback::
COnDisconnectedCallback(CPtMQMsgOwner* pOwner, EPipeDisconnectReason eReason, bool bGracefully)
:CPipeCallback(pOwner)
{
	m_eReason		=eReason;
	m_bGracefully	=bGracefully;
}

void COnDisconnectedCallback::Callback(IPipeHandler* pHandler)
{
	pHandler->OnDisconnected(m_eReason, m_bGracefully);
}


COnConnectFailedCallback::COnConnectFailedCallback(CPtMQMsgOwner* pOwner, EPipeConnFailedReason eReason)
:CPipeCallback(pOwner)
{
	m_eReason = eReason;
}

void COnConnectFailedCallback::Callback(IPipeHandler* pHandler)
{
	pHandler->OnConnectFailed(m_eReason);
}

COnAcceptedCallback::COnAcceptedCallback(CPtMQMsgOwner* pOwner, IPipe* pNewPipe)
:CPipeCallback(pOwner)
{
	m_pNewPipe	=pNewPipe;
}

void COnAcceptedCallback::Callback(IPipeHandler* pHandler)
{
	pHandler->OnAccepted( m_pNewPipe );
	
	if( m_pNewPipe->GetHandler() )
	{
		class_cast<CSyncPipe*>( GetOwner() )->m_pReactor->Register( m_pNewPipe );
	}
	else
	{
		if( dynamic_cast<CAsynPipeHandler*>( pHandler ) )
			return;

		m_pNewPipe->Release();
	}
}

COnListenFailedCallback::COnListenFailedCallback(CPtMQMsgOwner* pOwner)
: CPipeCallback( pOwner )
{
}

void COnListenFailedCallback::Callback(IPipeHandler* pHandler)
{
	pHandler->OnListenFailed();
}

COnListenSuccessedCallback::COnListenSuccessedCallback(CPtMQMsgOwner* pOwner)
: CPipeCallback( pOwner )
{
}

void COnListenSuccessedCallback::Callback(IPipeHandler* pHandler)
{
	pHandler->OnListenSuccessed();
}

