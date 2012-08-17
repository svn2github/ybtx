#include "stdafx.h"
#include "CSyncLog.h"
#include "ThreadHelper.h"
#include <time.h>
#include <iostream>
#include "ErrLogHelper.h"
#include "ExpHelper.h"
#include "CodeCvs.h"
#include "TSqrAllocator.inl"

CSyncLog::CSyncLog(uint32 uID, const char* szFileName, const char* szMode)
: m_uID(uID)
, m_fdLog(NULL)
, m_bLogTimeEnabled(false)
{
	m_strFileName.append(utf16_to_utf8(GetLogPath()).c_str()).append("/").append(szFileName);

	if(!szMode)
	{
		m_strMode = "wb";
	}
	else
	{
		m_strMode.assign(szMode);
	}
}

CSyncLog::~CSyncLog()
{
	if(m_fdLog)
	{
		fclose(m_fdLog);
	}
}


void CSyncLog::EnableTime(bool bEnable)
{
	m_bLogTimeEnabled=bEnable;
}

void CSyncLog::Write(const char* szLog, size_t stLen)
{
	
	if(!m_fdLog)
	{
#ifdef _WIN32
		m_fdLog = _wfopen(utf8_to_utf16(m_strFileName.c_str()).c_str(), utf8_to_utf16(m_strMode.c_str()).c_str());
#else	
		m_fdLog = fopen(m_strFileName.c_str(), m_strMode.c_str());
#endif
		if(!m_fdLog)
		{
			return;
		}
	}

	if(m_bLogTimeEnabled)
	{
		char buf[64];

		time_t T;
		struct tm * timenow;
		time ( &T );
		timenow = localtime ( &T ); 

		int32 nCount = sprintf(buf, "%02d-%02d %02d:%02d:%02d ", timenow->tm_mon + 1, 
			timenow->tm_mday, timenow->tm_hour,
			timenow->tm_min, timenow->tm_sec);

		fwrite(buf, 1, nCount, m_fdLog);
	}

	fwrite(szLog, 1, stLen, m_fdLog);
}

void CSyncLog::Flush()
{
	fflush(m_fdLog);
}

