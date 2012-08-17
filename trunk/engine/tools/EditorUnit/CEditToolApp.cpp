#include "stdafx.h"
#include "CEditToolApp.h"
#include "IPipeReactor.h"
#include "CTick.h"
#include "CRenderSystem.h"
#include "SyncSystem.h"
#include "CGraphic.h"
#include "EditorExp.h"
#include "CEditContext.h"
#include "CEventState.h"
#include "CEditorConfig.h"
#include "CCtrlImpFactory.h"
#include "CmdMgr.h"
#include "../../src/CoreCommon/TTimeSystem.inl"

CEditTimeSystem::CEditTimeSystem(uint32 uBaseCyc)
:Parent_t(uBaseCyc){}


CEditToolApp::CEditToolApp(void)
: m_bQuit(false)
, m_pfnCanceled(NULL)
, m_pfnIdle(NULL)
, m_TimeSystem(2)
{
	CEditTimeSystem::Inst() = &m_TimeSystem;
	m_vecEvent.reserve(MAXIMUM_WAIT_OBJECTS);
	m_evCancelBlock=CreateEvent(NULL, FALSE, FALSE, NULL);
	if(!m_evCancelBlock)
		GenErr("CreateEvent Failed");
	m_vecEvent.push_back(m_evCancelBlock);

	if(!CEventState::HasInst())
		CEventState::Create();

	CEditorConfig::GetInst()->InitEditorConfig();
	CCtrlImpFactory::Create();
	CCmdMgr::Create();
}

CEditToolApp::~CEditToolApp(void)
{
	CCmdMgr::Destroy();
}

void	CEditToolApp::Release(void)
{
	CloseHandle(m_evCancelBlock);
	if(CEventState::HasInst())
		CEventState::Destroy();

	CCtrlImpFactory::Destroy();
}

ICtrlImpFactory* CEditToolApp::GetCtrlImpFactory(void)
{
	return CCtrlImpFactory::GetInst();
}

void CEditToolApp::Run(void)
{
	while(MainFrame());
	Release();
}


void	CEditToolApp::Close(void)
{
	OnCanceled();
}

void CEditToolApp::SetOnCanceled( SysSigCallBack pBack )
{
	m_pfnCanceled = pBack;
}

void CEditToolApp::SetOnIdle( SysSigCallBack pBack )
{
	m_pfnIdle = pBack;
}

bool	CEditToolApp::MainFrame(void)
{
	ImpTimeSystem_t::Inst()->PushLogicTime();

	int32 nTimeError = ImpTimeSystem_t::Inst()->GetTimeError();

	if( nTimeError>=0 )
		nTimeError = OnIdle( uint32(nTimeError) );

	if( nTimeError < 0 )
		nTimeError = 0;

	uint32 uResult = GetEvent( uint32( nTimeError ) );

	for( ;; )
	{
		switch( uResult )
		{
		case eGER_TimedOut:
			return !m_bQuit;
		case eGER_SysMsg:
			OnSysMsg();
			break;
		case eGER_Iocp:
			//OnIocp();
			break;
		case eGER_NetMsg:
			break;
		case eGER_Canceled:
			OnCanceled();
			if( m_bQuit )
				return false;
			break;
		default:
			{
				ostringstream strm;
				strm<<"m_pPipeReactor->GetEvent return unknown value "<<uResult;
				GenErr(strm.str());
			}
		}
		uResult = GetEvent( 0 );
	}
	return !m_bQuit;
}

void	CEditToolApp::OnSysMsg(void)
{
	MSG msg;

	while( PeekMessageW( &msg, NULL, 0, 0, PM_NOREMOVE ) )
	{
		if( !GetMessageW( &msg, NULL, 0, 0 ) )
			GenErr( "GetMessage shouldn't return 0." );
		TranslateMessage( &msg );
		DispatchMessageW( &msg );
	}
}

#define MIN_TIME 5000

uint32	CEditToolApp::OnIdle(uint32 uErrTime)
{
	if(!CGraphic::HasInst() || !CRenderSystem::HasInst())
		return 0;

	if(m_pfnIdle)
		m_pfnIdle();

	m_uCurTime = uint32(GetHDProcessTime());
	CSyncSystem::BeginSyncFun( uint64(MIN_TIME) );


	EDITOR_TRY
	{		
		CRenderSystem::GetInst()->UpdateAudioSystem();
		VecContext::iterator it,eit = m_vecContext.end();
		for( it = m_vecContext.begin(); it!=eit; ++it )
		{
			EDITOR_TRY
			{
				(*it)->Bump(m_uCurTime);	
			}
			EDITOR_CATCH(exp)
			{
				exp.AppendMsg("Test");
			}
			EDITOR_TRY_END;
		}
	}
	EDITOR_CATCH(exp)
	{
		GfkLogExpOnce(exp);
	}
	EDITOR_TRY_END;
	
	return ImpTimeSystem_t::Inst()->GetTimeError();
}

void	CEditToolApp::OnCanceled(void)
{
	m_bQuit = true;
	if(m_pfnCanceled)
		m_pfnCanceled();
}

uint32	CEditToolApp::GetEvent(unsigned uWaitTime) //原类型在CAynPipeReactor中
{
	const uint32 uResult=MsgWaitForMultipleObjectsEx((uint32)m_vecEvent.size(),&m_vecEvent[0],
		uWaitTime,QS_ALLINPUT, MWMO_ALERTABLE|MWMO_INPUTAVAILABLE );

	switch( uResult )
	{
	case WAIT_OBJECT_0:
		return eGER_Canceled;
	case WAIT_TIMEOUT:
		return eGER_TimedOut;
	case WAIT_IO_COMPLETION:
		return eGER_Iocp;
	case WAIT_FAILED:
		{
			ostringstream strm;
			strm<<"MsgWaitForMultipleObjectsEx failed";
			GenErr(strm.str());
		}
	default:
		{
			const uint32 uMsg=WAIT_OBJECT_0+m_vecEvent.size();
			if( uResult == uMsg  )
			{
				return eGER_SysMsg;
			}
			else if (uResult < uMsg || uResult > WAIT_OBJECT_0)
			{
				return eGER_NetMsg;
			}
			else
			{
				GenErr("MsgWaitForMultipleObjectsEx return invalid result.");
			}
		}
	}

	return eGER_NetMsg;
}

void	CEditToolApp::_AddContext( CEditContext* pContext )
{
	m_vecContext.push_back(pContext);
}

void	CEditToolApp::_DelContext( CEditContext* pContext )
{
	VecContext::iterator it,eit = m_vecContext.end();
	for( it = m_vecContext.begin(); it!=eit; ++it )
	{
		if(*it==pContext)
		{
			m_vecContext.erase(it);
			return;
		}
	}
}

uint32	CEditToolApp::GetCurTime(void)
{
	return m_uCurTime;
}

void	CEditToolApp::RegisterTick(CTick* pTick,uint32 uCyc)
{
	m_TimeSystem.Register( pTick, uCyc );
}

void	CEditToolApp::UnRegisterTick(CTick* pTick)
{
	m_TimeSystem.UnRegister(pTick);
}

void	IApplication::InitApp()
{
	static CEditToolApp Inst;
}
