#include "stdafx.h"
#include "CMegaLog.h"
#include "ErrLogHelper.h"
#include <time.h>
#include "ThreadHelper.inl"
#include "CMegaLogMgr.h"
#include "CMegaLogJob.h"

volatile int32 CMegaLog::ms_nLogID = 0;

CMegaLog::CMegaLog(const char* szFileName, const char* szLogVersion, 
				   uint32 uMaxFileSize, uint32 uInterval)
{
	m_uID = Atomic_FetchAndAdd(&ms_nLogID, 1);

	(new CMegaLogCreateJob(m_uID, szFileName, szLogVersion, uMaxFileSize, uInterval))->Add();
}

CMegaLog::~CMegaLog()
{
	(new CMegaLogDestroyJob(m_uID))->Add();
}

void CMegaLog::Release()
{
	delete this;
}

void CMegaLog::Write(const char* szMsg)
{
	(new CMegaLogWriteJob(m_uID, szMsg))->Add();
}

CMegaLog* CMegaLog::CreateLog(const char* szFileName, const char* szLogVersion, uint32 uMaxFileSize, uint32 uInterval)
{
	return new CMegaLog(szFileName, szLogVersion, uMaxFileSize, uInterval);
}

void CMegaLog::Init()
{
	CMegaLogMgr::Init();
}

void CMegaLog::Unit()
{
	CMegaLogMgr::Unit();
}
