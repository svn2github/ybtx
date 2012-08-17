#include "stdafx.h"
#include "CAppStress.h"
#include "TExpSafeApp.inl"
#include "TAppWithConsole.inl"

#include "CTimeSystemStress.h"
#include "CoreCommon.h"

#include "CConnMgrStress.h"
#include "IAppStressHandler.h"
#include "CAppConfigStress.h"
#include "CCoreThreadMsg.h"
#include "CCoreSigMsgMgr.h"
#include <csignal>

template class TApp<CTraitsStress>;
template class TExpSafeApp<CTraitsStress>;
template class TAppWithConsole<CTraitsStress>;

void CAppStress::Init()
{
	new CAppStress;
}

void CAppStress::Unit()
{
	delete Inst();
	IntInst() = NULL;
}

CAppStress::CAppStress()
{
	CCoreMsgMgr::Init();
}

CAppStress::~CAppStress()
{
	CCoreMsgMgr::Unit();
}

uint32 CAppStress::GetGlobalTime()const
{
	return (uint32)CTimeSystemStress::Inst()->GetGlobalTime();
}

#ifndef _WIN32
#define SIGCORETERM SIGRTMIN+10
#endif

void CAppStress::OnBeginMainCycleDerived()
{
#ifndef _WIN32
	CCoreSigMsgMgr::Inst().SetSigHandler(&SigHandler);
#else
	if(!SetConsoleCtrlHandler(HandlerRoutine,TRUE))
		GenErr("system call SetConsoleCtrlHandler failed.");
#endif
}


namespace sqr
{
	class CCoreStrMsgSignal : public CCoreThreadMsg
	{
	public:
		CCoreStrMsgSignal() {}
		~CCoreStrMsgSignal()
		{
			CAppStress::Inst()->GetHandler()->OnClose();	
		}
	};
}

#ifndef _WIN32
void CAppStress::SigHandler(int )
{
	CCoreMsgMgr::Inst().PostThreadMsg( new CCoreStrMsgSignal( ) );	
	Inst()->ReactorCancelBlock();
}
#else


BOOL WINAPI CAppStress::HandlerRoutine(DWORD dwCtrlType)
{
	switch( dwCtrlType )
	{
	case CTRL_CLOSE_EVENT:
		TerminateProcess( GetCurrentProcess(),0 );
		break;
	case CTRL_C_EVENT:
		CCoreMsgMgr::Inst().PostThreadMsg( new CCoreStrMsgSignal( ) );		
		break;

	}

	Inst()->ReactorCancelBlock();
	return TRUE;
}

#endif


void CAppStress::Quit()
{
	IntQuit();
}

void CAppStress::OnMainFrameEndedDerived()
{
	ParentApp_t::OnMainFrameEndedDerived();
}

void CAppStress::OnCanceled()
{
	CCoreMsgMgr::Inst().FlushThreadMsg();
}
