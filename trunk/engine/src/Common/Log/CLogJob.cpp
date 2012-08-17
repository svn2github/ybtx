#include "stdafx.h"
#include "CLogJob.h"
#include "CLogThreadMgr.h"
#include "CSyncLog.h"
#include "CLog.h"
#include "TSqrAllocator.inl"

CLogJob::CLogJob()
{

}

CLogJob::~CLogJob()
{

}


void CLogJob::Add()
{
	CLogThreadMgr::Inst()->AddJob(this);
}

CCreateLogJob::CCreateLogJob(uint32 uID, const char* szFileName, const char* szMode)
{
	m_uID = uID;
	m_sFileName = szFileName;
	m_sMode = szMode;
}

CCreateLogJob::~CCreateLogJob()
{
	CSyncLog* pLog = new CSyncLog(m_uID, m_sFileName.c_str(), m_sMode.c_str());
	
	CLogThreadMgr::Inst()->AddLog(pLog, m_uID);
}

CDestroyLogJob::CDestroyLogJob(uint32 uID)
{
	m_uID = uID;
}

CDestroyLogJob::~CDestroyLogJob()
{
	CSyncLog* pLog = CLogThreadMgr::Inst()->RemoveLog(m_uID);
	if (!pLog)
	{
		return;
	}

	CLogThreadMgr::Inst()->RemoveFlushLog(pLog);

	delete pLog;
}

CWriteLogJob::CWriteLogJob(uint32 uID, const char* szMsg)
{
	m_uID = uID;

	size_t len = strlen(szMsg) + 1;

	m_sMsg.reserve(len + 1);

	m_sMsg = szMsg;
	m_sMsg.append("\n");
}

CWriteLogJob::CWriteLogJob(uint32 uID, const void* pBuffer, uint32 uLen)
{
	m_uID = uID;
		
	m_sMsg.reserve(uLen);

	m_sMsg.append((const char*)pBuffer, uLen);
}

CWriteLogJob::~CWriteLogJob()
{
	CSyncLog* pLog = CLogThreadMgr::Inst()->GetLog(m_uID);
	if (!pLog)
	{
		return;
	}

	pLog->Write(m_sMsg.data(), m_sMsg.size());

	CLogThreadMgr::Inst()->AddFlushLog(pLog);
}

CEnableTimeLogJob::CEnableTimeLogJob(uint32 uID, bool bEnable)
{
	m_uID = uID;
	m_bEnabled = bEnable;
}

CEnableTimeLogJob::~CEnableTimeLogJob()
{
	CSyncLog* pLog = CLogThreadMgr::Inst()->GetLog(m_uID);
	if (!pLog)
	{
		return;
	}

	pLog->EnableTime(m_bEnabled);
}


