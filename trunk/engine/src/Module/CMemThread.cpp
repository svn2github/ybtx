#include "stdafx.h"
#include "CMemThread.h"
#include "CMemThreadMgr.h"
#include "CMemJob.h"
#include "ThreadHelper.h"
#include "ExpHelper.h"

CMemThread::CMemThread(CMemThreadMgr* pThreadMgr)
{
	m_pThreadMgr=pThreadMgr;
	m_bQuit = false;
	CreateThread(&m_hThread,1024,&CMemThread::Execute,this);
}

CMemThread::~CMemThread()
{
	JoinThread(&m_hThread);
}

void CMemThread::Execute(void* pParam)
{
	static_cast<CMemThread*>(pParam)->DoExecute();
}

void CMemThread::DoExecute()
{
	for(;;)
	{
		switch( GetSemaphore( &m_pThreadMgr->m_smJobQueue ) )
		{
		case 0:
			break;
		default:
			{
				ostringstream strm;
				strm<<"GetSemaphore failed with error \""<<strerror(errno)<<"\"";
				GenErr(strm.str());
			}
		}
		
		m_pThreadMgr->HandleAllJobs();

		if(m_bQuit)
		{
			break;
		}
	}
}

