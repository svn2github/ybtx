#include "stdafx.h"
#include "CLog.h"
#include "ThreadHelper.inl"
#include <time.h>
#include <iostream>
#include "ErrLogHelper.h"
#include "ExpHelper.h"
#include "CLogThreadMgr.h"
#include "CLogJob.h"

uint32 CLog::ms_uLogID = 0;

CLog::CLog()
{
	m_uID = Atomic_FetchAndAdd((int32 volatile*)&ms_uLogID, 1);
}

CLog::~CLog()
{

}

CLog* CLog::CreateLog(const char* szFileName, const char* szMode)
{
	if(!szFileName)
		return NULL;

	CLog* pLog = new CLog;

	(new CCreateLogJob(pLog->GetID(), szFileName, szMode))->Add();	

	return pLog;
}


void CLog::Release()
{
	(new CDestroyLogJob(m_uID))->Add();
	delete this;
}

void CLog::EnableTime(bool bEnable)
{
	(new CEnableTimeLogJob(m_uID, bEnable))->Add();
}

void CLog::Write(const char* szLog)
{
	(new CWriteLogJob(m_uID, szLog))->Add();
}

void CLog::Write(const void* pBuffer, uint32 uLen)
{
	(new CWriteLogJob(m_uID, pBuffer, uLen))->Add();	
}
