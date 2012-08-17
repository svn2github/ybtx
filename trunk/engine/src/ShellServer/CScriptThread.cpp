#include "stdafx.h"
#include "CScriptThread.h"
#include "CScriptThreadMgr.h"
#include "ThreadHelper.inl"
#include "CScript.h"
#include "ErrLogHelper.h"
#include "ExpHelper.h"
#include "CScriptJob.inl"
#include "CScriptResult.inl"
#include "CAppConfigServer.h"
#include "TimeHelper.h"
#include "IScriptThreadHandler.h"
#include "CPkgFile.h"
#include "CScriptAppServer.h"
#include "CAppServer.h"
#include "TTrThreadMsgBuffer.inl"
#include "CAsynServerTrMsgBufferSwapper.h"
#include "CodeCvs.h"
#include "CBaseScriptApp.h"
#include "TSqrAllocator.inl"
#include "IDbConnectionMgr.h"
#include "IDbThreadScriptHandler.h"
#include "TraceHelper.h"


template class TTrThreadMsgBuffer<CScriptThreadTraits>;

CScriptThread::CScriptThread(CScriptThreadMgr* pThreadMgr)
: Parent_t(1024, 1024 * 1024, CAsynServerTrMsgBufferSwapper::Inst())
{
	m_pDbThreadMgr = NULL;
	
	m_pThreadMgr=pThreadMgr;

	m_uIndex=0;	

	m_pScript=NULL;

	m_bQuit=false;

	m_pHandler=NULL;

	m_nHasLeftMsg=0;
	m_nHasDbResult=0;

	CreateSemaphore(&m_smThread, 0,INT_MAX);

	const float fCallbackRatio		=CAppConfigServer::Inst()->GetChildVMGCCallbackRatio();
	const float fNormalUnpackRatio	=CAppConfigServer::Inst()->GetChildVMGCNormalUnpackRatio();
	const float fSerialUnpackRatio	=CAppConfigServer::Inst()->GetChildVMGCSerialUnpackRatio();

	if ( CatchErrorEnabled() )
	{
		m_pScript = new CScript(eSH_None,fCallbackRatio,fNormalUnpackRatio,fSerialUnpackRatio);
	}
	else
	{	
		m_pScript = new CScript(eSH_Debuger,fCallbackRatio,fNormalUnpackRatio,fSerialUnpackRatio);
	}
}

CScriptThread::~CScriptThread()
{
	delete m_pScript;

	DestroySemaphore(&m_smThread);
}

CScript* CScriptThread::GetScript() const
{
	return m_pScript;
}


CScriptThread* CScriptThread::Inst()
{
	CScriptThreadMgr* pThreadMgr = CScriptThreadMgr::Inst();

	CScriptThread* pThread = (CScriptThread*)TLS_GetValue(pThreadMgr->m_tlsThreadKey);

	return pThread;
}

void CScriptThread::SetHandler(IScriptThreadHandler* pHandler)
{
	AdvDelRef(m_pHandler);
	m_pHandler = pHandler;
	AdvAddRef(m_pHandler);
}

IScriptThreadHandler* CScriptThread::GetHandler() const
{
	return m_pHandler;
}

void CScriptThread::Spawn(const char* szFileName)
{
	CAppServer::Inst()->Log("Start ScriptThread");

	m_strRunFile.assign(szFileName);

	CreateThread(&m_hThread, 1024, &CScriptThread::Execute, this);
}

void CScriptThread::Join()
{
	CAppServer::Inst()->Log("Stop ScriptThread");

	PutSemaphore(&m_smThread);
	JoinThread(&m_hThread);

	CAppServer::Inst()->Log("ScriptThread stopped");
}

void CScriptThread::Execute(void* pParam)
{
	SetThreadName("ScriptThread");
	return static_cast<CScriptThread*>(pParam)->DoExecute();
}

void CScriptThread::DoExecute()
{
#ifdef _WIN32
	if ( _configthreadlocale(_ENABLE_PER_THREAD_LOCALE) == -1 )
		GenErr("_configthreadlocale failed.");
#endif

	SetAltSigStackEnabled(true);

	CBaseScriptApp::InitCfgTableTlsValue();
	CBaseScriptApp::InitStrIndexTlsValue();
	CBaseScriptApp::InitStr64IndexTlsValue();

#ifdef _WIN32
	_set_se_translator(TransESHFun);
#endif

	AddWatchCpuThreadID("ScriptThread",GetCurTID());
	HTHREADID id = GetCurTID();
	m_pScript->SetRunningThread(id);

#ifndef _WIN32
	SetLogCallBackFunc(CScriptThread::OnErrLogCallBack);
#endif

	RegistThreadCoreServer(*m_pScript);

	TLS_SetValue(m_pThreadMgr->m_tlsThreadKey, this);

	CPkgFile::InitTLSData();

	stringstream strm;

	strm<<"engine_require(\"server/thread/ThreadCoreServer\")\n"
		"ThreadCoreServer.Run(\""<<m_strRunFile.c_str()<<"\")";

	string sError = m_pScript->RunString(strm.str().c_str(),NULL);
	if (!sError.empty())
	{
		LogErr(sError.c_str());
	}

	CPkgFile::UnitTLSData();

	TLS_SetValue(m_pThreadMgr->m_tlsThreadKey, NULL);

	CBaseScriptApp::UnInitCfgTableTlsValue();
	CBaseScriptApp::UnInitStrIndexTlsValue();
	CBaseScriptApp::UnInitStr64IndexTlsValue();

#ifndef _WIN32	
	SetLogCallBackFunc(NULL);
#endif

	m_pScript->DetachRunningThread();

	SetAltSigStackEnabled(false);

}


void CScriptThread::HandleErrMsg(const char* szMsg)
{
	(new (this) CScriptTellErrMsgResult(szMsg, this))->Add(this);
}



void CScriptThread::OnErrLogCallBack(const char* szError, CLogOwner* )
{
	CScriptThread* pThread = CScriptThread::Inst();
	if(!pThread)
		return;

	pThread->HandleErrMsg(szError);
}

/////////////////////////////////////////////////////////////////////

void CScriptThread::Quit()
{
	m_bQuit = true;
	PutSemaphore(&m_smThread);
}

void CScriptThread::Run()
{
	bool bEnding = false;

	volatile uint64 uTime= GetProcessTime();

	uint64 uGCTime = uTime;
	
	const uint32 uInterval=min<uint32>( 500,CAppConfigServer::Inst()->GetChildVMGCStepInterval() );
	const uint32 uStep=CAppConfigServer::Inst()->GetChildVMGCStep();
	
	WatchThread( m_hThread , GetThreadName(), &uTime ,30*1000, 2,5000 );

	SQR_TRY
	{
		LowerCurThreadPriority();

		for(;;)
		{
			uTime=GetProcessTime();

			while( uTime >= uGCTime )
			{
				uGCTime+=uInterval;
				GetScript()->GCStep(uStep);
			}

			switch( GetSemaphore( &m_smThread,uInterval  ) )
			{
			case 1:
				continue;
			case 0:
				break;
			default:
				{
					ostringstream strm;
					strm<<"GetSemaphore failed with error \""<<strerror(errno)<<"\"";
					GenErr(strm.str());
				}
			}

			if( Atomic_Swap( &m_nHasDbResult, 0 ) )
				m_pScriptDbHandler->OnSomeQueryJobDone();

			if( Atomic_Swap( &m_nHasLeftMsg, 0 ) )
				HandleAllLeftMsg();

			FlushRightMsg();			
			
			if(m_bQuit)
			{
				break;
			}
			else
			{
				if(!bEnding)
				{
					if(m_pThreadMgr->m_bQuitSignal)
					{
						bEnding = true;
						m_pHandler->OnEndThread();
					}
				}					
			}
		}

		GetScript()->GCAll();
	}
	SQR_CATCH(exp)
	{
		LogExp(exp);
	}
	SQR_TRY_END;

	DoNotWatchThread( m_hThread );
}




IDbConnectionMgr* CScriptThread::GetDbConnectionMgr( uint8 uNumConn, const char* szHost,unsigned short uPort
													, const char* szUser,const char* szPassword, const char* szDatabase)
{
	return sqr::GetDbConnectionMgr(CAppServer::Inst()->GetDatabase(),uNumConn,szHost,uPort,szUser,szPassword,szDatabase);
}

IDbConnectionMgr* CScriptThread::GetDbConnectionMgr(uint8 uNumConn, const char* szPipe
													, const char* szUser, const char* szPassword, const char* szDatabase)
{
	return sqr::GetDbConnectionMgr(CAppServer::Inst()->GetDatabase(),uNumConn,szPipe,szUser,szPassword,szDatabase);
}

IDbThreadMgr* CScriptThread::GetDbThreadMgr(uint8 uNumThread, IDbThreadScriptHandler* pHandler)
{
	if( m_pDbThreadMgr )
		GenErr("There can only be one DbThreadMgr for each CScriptRhead.");

	m_pScriptDbHandler = pHandler;

	AdvAddRef(m_pScriptDbHandler);

	m_pDbThreadMgr = sqr::GetDbThreadMgr(uNumThread,this);

	return m_pDbThreadMgr;
}

void CScriptThread::ReleaseDbThreadsMgr()
{
	if( !m_pDbThreadMgr )
		return;

	AdvDelRef(m_pScriptDbHandler);
	m_pScriptDbHandler = NULL;

	delete m_pDbThreadMgr;
	m_pDbThreadMgr=NULL;
}

void CScriptThread::ReleaseDbConnectionMgr(IDbConnectionMgr* pMgr)
{
	pMgr->Release();
}

void CScriptThread::OnQueryJobDone()
{
	//次函数从db线程调用过来
	if( !Atomic_CompareAndSwap( &m_nHasDbResult, m_nHasDbResult, 1 ) )
		PutSemaphore(&m_smThread);
}

void CScriptThread::OnWakeRight()
{
	Atomic_Swap( &m_nHasLeftMsg, 1 );
	PutSemaphore(&m_smThread);
}

