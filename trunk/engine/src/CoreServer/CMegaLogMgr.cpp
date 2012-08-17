#include "stdafx.h"
#include "CMegaLogMgr.h"
#include "CMegaLog.h"
#include "CSyncMegaLog.h"
#include "ThreadHelper.inl"
#include "CMegaLogJob.h"
#include "TSqrAllocator.inl"
#include "ExpHelper.h"
#include "ErrLogHelper.h"

CMegaLogMgr* CMegaLogMgr::ms_pInst = NULL;

void CMegaLogMgr::Init()
{
	ms_pInst = new CMegaLogMgr;
}

void CMegaLogMgr::Unit()
{
	delete ms_pInst;
	ms_pInst = NULL;
}

CMegaLogMgr* CMegaLogMgr::Inst()
{
	return ms_pInst;
}

CMegaLogMgr::CMegaLogMgr()
{
	m_bEndThread = false;

	m_pRuningJobQue = new VecJob_t;
	m_pWaitingJobQue = new VecJob_t;

	CreateLock(&m_hLock);
	CreateSemaphore(&m_hSem, 0, INT_MAX);
	CreateSemaphore(&m_hBlockSem, 0, INT_MAX);

	m_uBlockNum = 0;

	CreateThread(&m_hThread,1024,&CMegaLogMgr::Execute,this);
}

CMegaLogMgr::~CMegaLogMgr()
{
	m_bEndThread = true;

	PutSemaphore(&m_hSem);

	JoinThread(&m_hThread);
	DestroySemaphore(&m_hBlockSem);
	DestroySemaphore(&m_hSem);
	DestroyLock(&m_hLock);

	delete m_pRuningJobQue;
	delete m_pWaitingJobQue;
}

void CMegaLogMgr::Execute(void* pParam)
{
	SetThreadName("MegaLogThread");

	return static_cast<CMegaLogMgr*>(pParam)->DoExecute();
}

void CMegaLogMgr::DoExecute()
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
				GenErr("CMegaLogMgr::DoExecute GetSemaphore Error", strm.str());
			}
		}
	}

	SetAltSigStackEnabled(false);
}


void CMegaLogMgr::HandleAllJobs()
{
	uint32 uBlockNum = 0;
	Lock(&m_hLock);

	VecJob_t* pJobQue = m_pRuningJobQue;
	m_pRuningJobQue = m_pWaitingJobQue;
	m_pWaitingJobQue = pJobQue;

	uBlockNum = m_uBlockNum;
	m_uBlockNum = 0;

	Unlock(&m_hLock);

	for(uint32 i = 0; i < uBlockNum; i++)
	{
		PutSemaphore(&m_hBlockSem);
	}

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
				CMegaLogJob* pJob = m_pRuningJobQue->at(i++);
				pJob->Do();
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

void CMegaLogMgr::AddJob(CMegaLogJob* pJob)
{
	size_t stSize = 0;
	bool bBlocked = false;

	Lock(&m_hLock);

	m_pWaitingJobQue->push_back(pJob);
	stSize = m_pWaitingJobQue->size();

	//如果job大于10万了，我们不允许在这个线程再次加job了
	if (stSize > 1000000)
	{
		++m_uBlockNum;
		bBlocked = true;
	}

	Unlock(&m_hLock);

	if (bBlocked)
		GetSemaphore(&m_hBlockSem, 120000);
}


void CMegaLogMgr::AddFlushLog(CSyncMegaLog* pLog)
{
	m_setFlushLog.insert(pLog);
}

void CMegaLogMgr::RemoveFlushLog(CSyncMegaLog* pLog)
{
	m_setFlushLog.erase(pLog);
}

void CMegaLogMgr::FlushLogs()
{
	if (m_setFlushLog.empty())
		return;
	
	SetFlushLog_t::iterator iter = m_setFlushLog.begin();
	SetFlushLog_t::iterator iter_end = m_setFlushLog.end();

	while(iter != iter_end)
	{
		CSyncMegaLog* pLog = *iter;
		pLog->Flush();

		++iter;
	}

	m_setFlushLog.clear();
}
