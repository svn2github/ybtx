#include "stdafx.h"
#include "CErrLogThreadMgr.h"
#include "CErrLogJob.h"
#include "ThreadHelper.h"
#include "ExpHelper.h"
#include "ErrLogHelper.h"

CErrLogThreadMgr* CErrLogThreadMgr::ms_pInst = NULL;

CErrLogThreadMgr::CErrLogThreadMgr()
:m_bEndThread(false)
{
	CreateLock(&m_hLock);
	CreateSemaphore(&m_hSem, 0, INT_MAX);

	CreateThread(&m_hThread,1024,&CErrLogThreadMgr::Execute,this);
}

CErrLogThreadMgr::~CErrLogThreadMgr()
{
	m_bEndThread = true;

	PutSemaphore(&m_hSem);

	JoinThread(&m_hThread);
	DestroySemaphore(&m_hSem);
	DestroyLock(&m_hLock);
}

void CErrLogThreadMgr::Init()
{
	ms_pInst = new CErrLogThreadMgr();
}

void CErrLogThreadMgr::Unit()
{
	delete ms_pInst;
	ms_pInst = NULL;
}

void CErrLogThreadMgr::Execute(void* pParam)
{
	SetThreadName("ErrLogThread");

	return static_cast<CErrLogThreadMgr*>(pParam)->DoExecute();
}

void CErrLogThreadMgr::DoExecute()
{
	SetAltSigStackEnabled(true);

#ifdef _WIN32
	if(CatchErrorEnabled())
		_set_se_translator(TransESHFun);
#endif
	
	for (;;)
	{
		if (m_bEndThread||!ErrLogEnabled())
		{
			break;
		}

		switch( GetSemaphore( &m_hSem, 1000 ) )
		{
		case 0:	
		case 1:
			HandleAllJobs();
			break;
		default:
			{
				ostringstream strm;
				strm<<"GetSemaphore failed with error \""<<strerror(errno)<<"\"";
				GenErr("CErrLogThreadMgr::DoExecute GetSemaphore Error", strm.str());
			}
		}
	}

	SetAltSigStackEnabled(false);
}


void CErrLogThreadMgr::HandleAllJobs()
{
	Lock(&m_hLock);
	swap(m_queWaitingJob,m_queRuningJob);
	Unlock(&m_hLock);

	for(VecJob_t::iterator it = m_queRuningJob.begin(); it != m_queRuningJob.end(); ++it )
	{
		CErrLogJob *pJob = (*it);	
		delete pJob;
	}
	m_queRuningJob.clear();
}

void CErrLogThreadMgr::AddJob(CErrLogJob* pJob)
{
	Lock(&m_hLock);
	m_queWaitingJob.push_back(pJob);
	Unlock(&m_hLock);
}

void	CErrLogThreadMgr::AddCreateErrlogJob(const wchar_t* szFileName, const char* szDeviceInfo)
{
	AddJob(new CCreateErrLogJob(szFileName,szDeviceInfo));
}

void	CErrLogThreadMgr::AddCloseErrlogJob()
{
	AddJob(new CCloseErrLogJob);
}

void	CErrLogThreadMgr::AddWriteErrlogJob(const CError& exp,const char* szErrlogInfo)
{
	AddJob(new CWriteErrLogJob(exp,szErrlogInfo));
}
