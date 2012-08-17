#include "stdafx.h"
#include "CDbThread.h"
#include "ThreadHelper.h"
#include "CDbThreadMgr.h"
#include "ExpHelper.h"
#include "IDatabase.h"
#include "ErrLogHelper.h"
#include "CAppServer.h"
#include "TimeHelper.h"
#include "CQueryJob.h"
#include "BaseHelper.h"
#include "CThreadWatcher.h"
#include "CCpuCheckPoint.h"
#include "TraceHelper.h"

CDbThread::CDbThread(CDbThreadMgr* pMgr)
{
	m_pThreadMgr= pMgr;
	CreateThread(&m_hThread,1024,&CDbThread::Execute,this);
}

CDbThread::~CDbThread(void)
{
	JoinThread(&m_hThread);
}



void CDbThread::Execute(void* pParam)
{
	return static_cast<CDbThread*>(pParam)->DoExecute();
}

void CDbThread::DoExecute()
{
	SetAltSigStackEnabled(true);

#ifdef _WIN32
	if(CatchErrorEnabled())
		_set_se_translator(TransESHFun);
#endif


	CAppServer::Inst()->m_pDatabase->BeginDbThread();

	volatile uint64 uTime=GetProcessTime();

	const char* szThreadName = "DbThread";
	SetThreadName(szThreadName);
	AddWatchCpuThreadID(szThreadName,GetCurTID());

	//db线程不能被唤醒主循环，一旦被唤醒，就会出现于mysql"command out of sync"的错误。
	WatchThread( m_hThread , szThreadName, &uTime , 120*1000 , 0 ,60000 );

	CCpuCheckPoint ccp;

	SQR_TRY
	{		
		for(;;)
		{
			//这个变量被用于死循环检测了，这一行别删除了
			uTime = GetProcessTime();

			const uint64 uCcpElapse=ccp.GetElapse();
			
			if( uCcpElapse > 10*1000 )
			{
				float fUsage=ccp.GetUsage();
				
				if( fUsage > 20 )
				{
					ostringstream strm;

					strm<<"持续时间:"<<uCcpElapse<<" cpu用量:"<<fUsage;
					LogErr("DbThread cpu 消耗大于20%",strm.str());

					if( CThreadWatcher::Inst()->Enabled() )
					{
						fflush(NULL);
						abort();
					}
				}
			}			

			switch( GetSemaphore( &m_pThreadMgr->m_smJobQueue ,10*1000 ) )
			{
			case 1:
				continue;
			case 0:
				break;
			default:
				{
					ostringstream strm;
					strm<<"GetSemaphore failed with error \""<<strerror(errno)<<"\"";
					GenErr("CDbThread::DoExecute GetSemaphore Error", strm.str());
				}
			}

			ccp.SetCheckPoint();
			
			if(m_pThreadMgr->m_bEndThreads)
				break;

			m_pThreadMgr->HandleJob();
		}	
	}

	SQR_CATCH(exp)
	{
		LogExp(exp);
	}
	SQR_TRY_END;

	DoNotWatchThread( m_hThread );

	CAppServer::Inst()->m_pDatabase->EndDbThread();

	SetAltSigStackEnabled(false);
}
