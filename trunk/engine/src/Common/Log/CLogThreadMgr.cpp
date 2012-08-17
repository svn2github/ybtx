#include "stdafx.h"
#include "CLogThreadMgr.h"
#include "ThreadHelper.inl"
#include "CLogJob.h"
#include "ErrLogHelper.h"
#include "ExpHelper.h"
#include "TSqrAllocator.inl"
#include "CMemPool.h"
#include "CSyncLog.h"

CLogThreadMgr* CLogThreadMgr::ms_pInst = NULL;

CLogThreadMgr::CLogThreadMgr()
{
	m_bEndThread = false;

	m_pRuningJobQue = new VecJob_t;
	m_pWaitingJobQue = new VecJob_t;

	CreateLock(&m_hLock);
	CreateSemaphore(&m_hSem, 0, INT_MAX);

	CreateThread(&m_hThread,1024,&CLogThreadMgr::Execute,this);
}

CLogThreadMgr::~CLogThreadMgr()
{
	m_bEndThread = true;

	PutSemaphore(&m_hSem);

	JoinThread(&m_hThread);
	DestroySemaphore(&m_hSem);
	DestroyLock(&m_hLock);

	delete m_pRuningJobQue;
	delete m_pWaitingJobQue;
}

void CLogThreadMgr::Init()
{
	ms_pInst = new CLogThreadMgr();
}

void CLogThreadMgr::Unit()
{
	delete ms_pInst;
	ms_pInst = NULL;
}

CLogThreadMgr* CLogThreadMgr::Inst()
{
	return ms_pInst;
}

void CLogThreadMgr::Execute(void* pParam)
{
	SetThreadName("LogThread");

	return static_cast<CLogThreadMgr*>(pParam)->DoExecute();
}

void CLogThreadMgr::DoExecute()
{
	SetAltSigStackEnabled(true);

#ifdef _WIN32
	if(CatchErrorEnabled())
		_set_se_translator(TransESHFun);
#endif
	
	for (;;)
	{
		if (m_bEndThread)
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
				GenErr("CLogThreadMgr::DoExecute GetSemaphore Error", strm.str());
			}
		}
	}

	SetAltSigStackEnabled(false);
}


void CLogThreadMgr::HandleAllJobs()
{
	Lock(&m_hLock);

	VecJob_t* pJobQue = m_pRuningJobQue;
	m_pRuningJobQue = m_pWaitingJobQue;
	m_pWaitingJobQue = pJobQue;

	Unlock(&m_hLock);

	size_t i = 0;

	if (m_pRuningJobQue->empty())
	{
		return;
	}

	size_t stSize = m_pRuningJobQue->size();

	for(;;)
	{
		SQR_TRY
		{
			while (i < stSize)
			{
				CLogJob* pJob = m_pRuningJobQue->at(i++);
				delete pJob;
			}

			FlushLogs();

			m_pRuningJobQue->clear();
			break;
		}
		SQR_CATCH(exp)
		{
			LogExp(exp);
		}
		SQR_TRY_END;
	}
}

void CLogThreadMgr::AddJob(CLogJob* pJob)
{
	Lock(&m_hLock);
	m_pWaitingJobQue->push_back(pJob);
	Unlock(&m_hLock);
}

void CLogThreadMgr::AddLog(CSyncLog* pLog, uint32 uID)
{
	m_mapID2Log.insert(make_pair(uID, pLog));
}

CSyncLog* CLogThreadMgr::GetLog(uint32 uID)
{
	MapID2Log_t::iterator iter = m_mapID2Log.find(uID);
	if (iter != m_mapID2Log.end())
	{
		return iter->second;
	}

	return NULL;
}

CSyncLog* CLogThreadMgr::RemoveLog(uint32 uID)
{
	CSyncLog* pLog = m_mapID2Log[uID];
	m_mapID2Log.erase(uID);

	return pLog;
}

void CLogThreadMgr::AddFlushLog(CSyncLog* pLog)
{
	m_setFlushLog.insert(pLog);
}

void CLogThreadMgr::RemoveFlushLog(CSyncLog* pLog)
{
	m_setFlushLog.erase(pLog);
}

void CLogThreadMgr::FlushLogs()
{
	if (m_setFlushLog.empty())
	{
		return;
	}

	SetFlushLog_t::iterator iter = m_setFlushLog.begin();
	SetFlushLog_t::iterator iter_end = m_setFlushLog.end();

	while(iter != iter_end)
	{
		CSyncLog* pLog = *iter;
		pLog->Flush();

		++iter;
	}

	m_setFlushLog.clear();
}



