#include "stdafx.h"
#include "CPipeState_Busy.h"
#include "ExpHelper.h"
#include "CSyncPipeReactor.h"



//******************************************************************
// Busy
//******************************************************************

SOCKET CPipeState_Busy::CreateSocket()
{
	//创建socket
	SOCKET Socket = socket(AF_INET, SOCK_STREAM, 0);
	if (Socket == INVALID_SOCKET)
		GenNetworkExp("socket");

	u_long uParam=1;
	if(SOCKET_ERROR==ioctlsocket(Socket,FIONBIO,&uParam))
	{
		closesocket(Socket);
		GenNetworkExp("ioctlsocket");
	}

	int nValue;

//#ifndef _WIN32
//	nValue=0;
//	if(SOCKET_ERROR==setsockopt(Socket,SOL_SOCKET,SO_SNDBUF,reinterpret_cast<const char*>(&nValue),sizeof(nValue)))
//		GenNetworkExp("setsockopt");
//
//	nValue=0;
//	if(SOCKET_ERROR==setsockopt(Socket,SOL_SOCKET,SO_RCVBUF,reinterpret_cast<const char*>(&nValue),sizeof(nValue)))
//		GenNetworkExp("setsockopt");
//#endif

#ifndef _WIN32
	nValue=1;
	if(SOCKET_ERROR==setsockopt(Socket,SOL_SOCKET,SO_REUSEADDR,reinterpret_cast<const char*>(&nValue),sizeof(nValue)))
		GenNetworkExp("setsockopt");
#endif

	nValue = 1;
	if( SOCKET_ERROR==setsockopt(Socket,IPPROTO_TCP,TCP_NODELAY,reinterpret_cast<const char*>(&nValue),sizeof(nValue)) )
		GenNetworkExp("setsockopt");

	return Socket;
}


CPipeState_Busy::CPipeState_Busy(CSyncPipe* pPipe)
:CPipeState(pPipe)
{
	m_Socket = CreateSocket();
}

CPipeState_Busy::CPipeState_Busy(CSyncPipe* pPipe,const SOCKET& Socket)
:CPipeState(pPipe),m_Socket(Socket)
{
	Ast( m_Socket );
}


CPipeState_Busy::~CPipeState_Busy()
{
	if( !m_Socket )
		return;

	if(closesocket(m_Socket))
		GenNetworkExp("closesocket");			
	m_Socket = 0;
}


#ifndef _WIN32

void CPipeState_Busy::EpollCtl(int32 nOp,uint32 uEvent)const
{
	epoll_event ee;
	ee.data.ptr=const_cast<CPipeState_Busy*>(this);
	ee.events=uEvent;
	if(-1==epoll_ctl(GetPipe()->m_pReactor->m_fdEpoll,nOp,m_Socket,&ee))
	{
		ostringstream strm;
		strm<<"epoll_ctl failed with error:"<<strerror(errno);
		GenErr(strm.str());
	}
}

#endif

bool CPipeState_Busy::IsBusy()const
{
	return true;
}




#ifdef _WIN32

CStateWithEvent::CStateWithEvent()
:m_uIndex(uint32(-1)),m_uIndexOfTempVec(uint32(-1))
{
}

void CStateWithEvent::EventSelect(uint32 uEvent)
{
	if(SOCKET_ERROR!=WSAEventSelect(class_cast<CPipeState_Busy*>(GetPipeState())->
		m_Socket,m_Event.Get(),uEvent))
		return;
	GenNetworkExp("WSAEventSelect");
}

void CStateWithEvent::EventAdd(uint32 uEvent)
{
	Ast(m_uIndex==uint32(-1));

	CSyncPipeReactor* pReactor=GetPipeState()->GetPipe()->m_pReactor;

	CSyncPipeReactor::EventStateVec_t& VecState=pReactor->m_vecState;

	if(VecState.size()>=MAXIMUM_WAIT_OBJECTS-2)
	{
		/*ostringstream strm;
		strm<<"There are at most "<<MAXIMUM_WAIT_OBJECTS-2<<" pipes in connecting or listening state.";
		GenErr(strm.str());*/
		
		CSyncPipeReactor::EventStateVec_t& VecStateTemp=pReactor->m_vecStateTemp;
		
		m_uIndexOfTempVec = (uint32)VecStateTemp.size();
		VecStateTemp.push_back(this);
		
		//cout << m_uIndexOfTempVec  << " Save EventAdd Temp Vec state id " << m_uID << endl;
	}
	else
	{
		m_uIndex=(uint32)VecState.size();
		VecState.push_back(this);
		pReactor->m_vecEvent.push_back(m_Event.Get());
		//cout << m_uIndex << " push in vecEvent " << m_uID << endl;
	}
	
	EventSelect(uEvent);
}

void CStateWithEvent::EventDel()
{
	EventSelect(0);

	CSyncPipeReactor* pReactor=GetPipeState()->GetPipe()->m_pReactor;
	
	if (m_uIndex != uint32(-1))
	{
		CSyncPipeReactor::EventVec_t& VecEvent=pReactor->m_vecEvent;

		VecEvent[m_uIndex+1]=VecEvent.back();
		VecEvent.pop_back();
		
		CSyncPipeReactor::EventStateVec_t& VecState=pReactor->m_vecState;

		VecState.back()->m_uIndex=m_uIndex;
		VecState[m_uIndex]=VecState.back();
		VecState.pop_back();
		m_uIndex=uint32(-1);	

		//去临时队列中找
		AddEventFromTemp();
	}
	else
	{
		Ast(m_uIndexOfTempVec!=uint32(-1));

		CSyncPipeReactor::EventStateVec_t& VecStateTemp = pReactor->m_vecStateTemp;
		VecStateTemp.back()->m_uIndexOfTempVec = m_uIndexOfTempVec;
		VecStateTemp[m_uIndexOfTempVec]=VecStateTemp.back();
		VecStateTemp.pop_back();
		m_uIndexOfTempVec=uint32(-1);
	}
}

void CStateWithEvent::AddEventFromTemp()
{
	CSyncPipeReactor* pReactor=GetPipeState()->GetPipe()->m_pReactor;

	CSyncPipeReactor::EventStateVec_t& VecStateTemp=pReactor->m_vecStateTemp;

	if (VecStateTemp.empty())
		return;

	CSyncPipeReactor::EventStateVec_t& VecState=pReactor->m_vecState;

	CStateWithEvent* pState=VecStateTemp.front();

	uint32 uIndex = pState->m_uIndex = (uint32)VecState.size();

	VecState.push_back(pState);

	pReactor->m_vecEvent.push_back(pState->m_Event.Get());

	VecStateTemp.front() = VecStateTemp.back();
	VecStateTemp.pop_back();

	//cout << uIndex << " push in vecEvent from tmp " << pState->m_uID << endl;
}

#endif

