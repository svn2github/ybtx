#include "stdafx.h"
#ifdef _WIN32
#include "CPipeEventObject.h"
#include "ExpHelper.h"


void CPipeEventObject::Create(const SOCKET& Socket)
{
	m_Event=WSACreateEvent();
	if(WSA_INVALID_EVENT==m_Event)
	{
		ostringstream strm;
		strm<<"WSACreateEvent failed with error code "<< SocketGetLastError() <<"."<<ends;
		GenErr(strm.str());
	}
}


void CPipeEventObject::Destroy(void)
{
	if(!WSACloseEvent(m_Event))
	{
		ostringstream strm;
		strm<<"WSACloseEvent failed with error code "<< SocketGetLastError() <<"."<<ends;
		GenErr(strm.str());
	}
}

const WSAEVENT& CPipeEventObject::Get()const
{
	return m_Event;
}
#endif
