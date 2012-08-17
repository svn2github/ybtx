#include "stdafx.h"
#include "CPipeState_Disconnected.h"
#include "CPipeState_Connecting.h"
#include "CPipeState_Listening.h"
#include "CSyncPipe.h"
#include "ExpHelper.h"
#include "CSyncPipe.inl"

//******************************************************************
// Disconnected
//******************************************************************

CPipeState_Disconnected::CPipeState_Disconnected(CSyncPipe* pPipe)
:CPipeState(pPipe)
{
	CPipeState* pState=GetPipe()->m_pState;
	GetPipe()->m_pState=NULL;
	delete pState;
	GetPipe()->m_pState=this;

	Register();
}

CPipeState_Disconnected::~CPipeState_Disconnected()
{
	Unregister();
}


bool CPipeState_Disconnected::Listen(const CAddress& Address)
{
	SOCKET Socket;
	switch ( CPipeState_Listening::CreateBindedSocket(Socket,Address) )
	{
	case 0:
		GetPipe()->AddEventListenSuccess();
		break;
	default:
		GetPipe()->AddEventListenFailed();
		return false;
	}

	new CPipeState_Listening(GetPipe(),Socket);
	return true;
}


bool CPipeState_Disconnected::Connect(const CAddress& Address)
{
	uint32 uResult=0;
	CPipeState_Connecting* pState=new CPipeState_Connecting(GetPipe(),Address,uResult);
	switch(uResult)
	{
	case 0:
		pState->SwitchToConnected();
		break;
	case 1:
		break;
	case 2:
		new CPipeState_Disconnected(GetPipe());
		return false;
	default:
		{
			ostringstream strm;
			strm<<"Invalid connect result "<<uResult<<" in CPipeState_Disconnected::Connect";
			GenErr(strm.str());
		}
	}
	return true;
}


void CPipeState_Disconnected::ShutDown(bool IfCallback)
{
}

void CPipeState_Disconnected::Register()
{
}

void CPipeState_Disconnected::Unregister()
{
}

