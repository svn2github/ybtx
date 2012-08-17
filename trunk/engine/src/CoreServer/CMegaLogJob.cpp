#include "stdafx.h"
#include "CMegaLogJob.h"
#include "CMegaLogMgr.h"
#include "CSyncMegaLog.h"
#include "TSqrAllocator.inl"

CMegaLogJob::CMegaLogJob(uint32 uID)
: m_uID(uID)
{

}

CMegaLogJob::~CMegaLogJob()
{
}


void CMegaLogJob::Add()
{
	CMegaLogMgr::Inst()->AddJob(this);
}

CMegaLogCreateJob::CMegaLogCreateJob(uint32 uID, const char* szFileName, const char* szLogVersion, 
									 uint32 uMaxFileSize, uint32 uInterval)
: CMegaLogJob(uID)
, m_sFileName(szFileName)
, m_sLogVersion(szLogVersion)
, m_uMaxFileSize(uMaxFileSize)
, m_uInterval(uInterval)
{
}

void CMegaLogCreateJob::Do()
{
	CSyncMegaLog* pLog = new CSyncMegaLog(m_uID, m_sFileName.c_str(), m_sLogVersion.c_str(), 
		m_uMaxFileSize, m_uInterval);
	CSyncMegaLog::AddLog(pLog, m_uID);
}

CMegaLogDestroyJob::CMegaLogDestroyJob(uint32 uID)
: CMegaLogJob(uID)
{
}

void CMegaLogDestroyJob::Do()
{
	CSyncMegaLog* pLog = CSyncMegaLog::RemoveLog(m_uID);
	if (!pLog)
	{
		return;
	}

	CMegaLogMgr::Inst()->RemoveFlushLog(pLog);

	delete pLog;
}

CMegaLogWriteJob::CMegaLogWriteJob(uint32 uID, const char* szMsg)
: CMegaLogJob(uID)
{
	size_t len = strlen(szMsg) + 1;

	m_sMsg.reserve( len );

	m_sMsg = szMsg;
	m_sMsg.append("\n");
}

void CMegaLogWriteJob::Do()
{
	CSyncMegaLog* pLog = CSyncMegaLog::GetLog(m_uID);
	if (!pLog)
		return;

	pLog->Write(m_sMsg.c_str(), m_sMsg.size());

	CMegaLogMgr::Inst()->AddFlushLog(pLog);
}



