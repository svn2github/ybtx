#include "stdafx.h"
#include "IPipeHandler.h"
#include "IPipe.h"
#include "CallBackX.h"

void IPipeHandler::OnConnected() 
{
	CALL_CLASS_CALLBACK() 
}
		
void IPipeHandler::OnListenFailed() 
{
	CALL_CLASS_CALLBACK() 
}

void IPipeHandler::OnListenSuccessed() 
{
	CALL_CLASS_CALLBACK() 
}
		
void IPipeHandler::OnConnectFailed(EPipeConnFailedReason eReason) 
{
	CALL_CLASS_CALLBACK_1(eReason) 
}

void IPipeHandler::OnDisconnected(EPipeDisconnectReason eReason,bool bGracefully)
{
	CALL_CLASS_CALLBACK_2(eReason, bGracefully)
}

void IPipeHandler::OnAccepted(IPipe* pPipe) 
{
	CALL_CLASS_CALLBACK_1(pPipe); 
}

void IPipeHandler::OnDataReceived()
{
	CALL_CLASS_CALLBACK() 
}

void IPipeHandler::OnDataSent(uint32 uSentSize) 
{
	CALL_CLASS_CALLBACK_1(uSentSize) 
}

void IPipeHandler::OnSendBufferFree() 
{
	CALL_CLASS_CALLBACK() 
}

