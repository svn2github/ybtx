#include "stdafx.h"
#include "CSyncMegaLog.h"
#include <iostream>
#include "ErrLogHelper.h"
#include "ExpHelper.h"
#include "CodeCvs.h"
#include "TSqrAllocator.inl"
#include <time.h>
#include "TimeHelper.h"

CSyncMegaLog::MapID2Log_t CSyncMegaLog::ms_mapID2Log;

CSyncMegaLog::CSyncMegaLog(uint32 uID, const char* szFileName,  
						   const char* szLogVersion, uint32 uMaxFileSize, uint32 uInterval)
						   : m_uID(uID)
						   , m_sFileName(szFileName)
						   , m_sLogVersion(szLogVersion)
						   , m_uMaxFileSize(uMaxFileSize)
						   , m_uCurFileSize(0)
						   , m_uInterval(uInterval)
						   , m_uLastLogTime(0)
						   , m_fdLog(NULL)
						 
{
	
}

CSyncMegaLog::~CSyncMegaLog()
{
	if(m_fdLog)
	{
		fclose(m_fdLog);
	}
}

void CSyncMegaLog::Write(const char* szLog, size_t stLen)
{
	if( stLen==0 )
		return;

	if ( !m_fdLog )
	{
		if( !CreateLog() )
			return;
	}
	else
	{
		if( szLog[stLen-1] == '\n' )
		{
			//本次log以换行符结尾

			if( ( m_uCurFileSize > m_uMaxFileSize ) || ( GetProcessTime() - m_uLastLogTime > (uint64)m_uInterval ) )
			{
				//时间比限制的长度要长
				if( !CreateLog() )
					return;
			}
		}
	}

	if( 0 == fwrite(szLog, 1, stLen, m_fdLog) )
	{
		if( !CreateLog() )
			return;

		fwrite(szLog, 1, stLen, m_fdLog);
	}

	m_uCurFileSize += (uint32)stLen;

}

void CSyncMegaLog::Flush()
{
	fflush(m_fdLog);
}

bool CSyncMegaLog::CreateLog()
{
	if (m_fdLog)
	{
		m_uCurFileSize = 0;
		fclose(m_fdLog);
	}

	char buf[64];

	time_t T;
	struct tm * timenow;
	time ( &T );
	timenow = localtime ( &T ); 

	sprintf(buf, "_%d-%02d-%02d-%02d-%02d-%02d.log", 
		timenow->tm_year + 1900, timenow->tm_mon + 1, 
		timenow->tm_mday, timenow->tm_hour,
		timenow->tm_min, timenow->tm_sec);

	String_t str;
	str.append(m_sFileName.c_str()).append(buf);

#ifdef _WIN32
	m_fdLog = _wfopen(utf8_to_utf16(str.c_str()).c_str(), L"w");
#else	
	m_fdLog = fopen(str.c_str(), "wb");
#endif

	if(!m_fdLog)
	{
		return false;
	}

	str = m_sLogVersion;
	str.append("\n");
	fwrite(str.c_str(), 1, str.size(), m_fdLog);

	m_uLastLogTime = GetProcessTime();

	return true;
}

void CSyncMegaLog::AddLog(CSyncMegaLog* pLog, uint32 uID)
{
	ms_mapID2Log.insert(make_pair(uID, pLog));
}

CSyncMegaLog* CSyncMegaLog::GetLog(uint32 uID)
{
	MapID2Log_t::iterator iter = ms_mapID2Log.find(uID);
	if (iter != ms_mapID2Log.end())
	{
		return iter->second;
	}

	return NULL;
}

CSyncMegaLog* CSyncMegaLog::RemoveLog(uint32 uID)
{
	CSyncMegaLog* pLog = ms_mapID2Log[uID];
	ms_mapID2Log.erase(uID);

	return pLog;
}

