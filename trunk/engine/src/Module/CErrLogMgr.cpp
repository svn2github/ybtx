#include "stdafx.h"
#include "CErrLogMgr.h"
#include "ExpHelper.h"
#include "TimeHelper.h"
#include "CErrLogThreadMgr.h"
#include "ThreadHelper.h"
#include "UtfCvs.h"
#include "ErrLogHelper.h"
#include "ErrLogDefs.h"
#include "CErrLog.h"

CErrLogMgr::CErrLogMgr()
: m_bErrLogEnabled(false)
, m_uInterval(uint32(-1))
, m_uLastLogTime(uint32(-1))
{

}

CErrLogMgr::~CErrLogMgr()
{

}

void CErrLogMgr::SetErrLogEnabled(bool bEnabled)
{
	m_bErrLogEnabled = bEnabled;
	if(CatchErrorEnabled()&&m_bErrLogEnabled)
		CErrLogThreadMgr::Init();
	else if(CErrLogThreadMgr::Inst())
		CErrLogThreadMgr::Unit();
}

void CErrLogMgr::CreateErrLogFile()
{
	uint32 uVersion = GetCurrentRevision();

	uint32 uProcessId = GetCurPID();

	const wchar_t* szFilePath = GetLogPath();

	wchar_t szErrFileName[256];

	const char* sz_guid = GetProcessStringGUID();
	wchar_t* szGuid = U8_to_U16(sz_guid);

	static uint32 uLogNum = 0;
	++uLogNum;

	if (uVersion != 0)
	{
#ifdef _WIN32
		swprintf_s(szErrFileName, COUNTOF(szErrFileName), L"%s/%s_err_%s_%uv%u_%u", szFilePath, m_strLogFile.c_str(), szGuid, uLogNum, uVersion,uProcessId);
#else
		swprintf(szErrFileName, COUNTOF(szErrFileName), L"%ls/%ls_err_%ls_%uv%u_%u", szFilePath, m_strLogFile.c_str(), szGuid, uLogNum, uVersion,uProcessId);
#endif
	}
	else
	{
#ifdef _WIN32
		swprintf_s(szErrFileName, COUNTOF(szErrFileName), L"%s/%s_err_%s_%u_%u", szFilePath, m_strLogFile.c_str(), szGuid, uLogNum,uProcessId);
#else
		swprintf(szErrFileName, COUNTOF(szErrFileName), L"%ls/%ls_err_%ls_%u_%u", szFilePath, m_strLogFile.c_str(), szGuid, uLogNum,uProcessId);
#endif
	}

	CErrLogThreadMgr::Inst()->AddCreateErrlogJob(szErrFileName,m_strDeviceInfo.c_str());
	FreeUtfCvsDest(szGuid);
}

void CErrLogMgr::WriteErrLog(CError& exp)
{
	if ( !CatchErrorEnabled() || !ErrLogEnabled() )
	{
		puts(exp.ErrorTitle());
		if( exp.ErrorMsg()[0] )
		{
			putchar('\t');
			puts(exp.ErrorMsg());
			putchar('\n');
		}
	}
	else
	{
		uint64 uCurTime = GetProcessTime();

		//如果两次记录log的时间大于interval，我们就要换一个log来记录了
		//如果小于这个值，我们仍然使用原来的文件记录
		if(uCurTime - m_uLastLogTime > (uint64)m_uInterval)
		{
			CErrLogThreadMgr::Inst()->AddCloseErrlogJob();
			CreateErrLogFile();
		}
		CErrLogThreadMgr::Inst()->AddWriteErrlogJob(exp,m_strErrInfo.c_str());
		m_uLastLogTime = uCurTime;
	}
}

void CErrLogMgr::WriteDumpErrLog(CError& exp)
{
	if ( !CatchErrorEnabled() || !ErrLogEnabled() )
	{
		puts(exp.ErrorTitle());
		if( exp.ErrorMsg()[0] )
		{
			putchar('\t');
			puts(exp.ErrorMsg());
			putchar('\n');
		}
	}
	else
	{
		CErrLog::Inst().WriteErrLog(exp,m_strErrInfo.c_str());
	}
}

void CErrLogMgr::WriteErrInfoLog(const char* szInfo)
{
	if(!szInfo)
		return;
	m_strErrInfo.append(szInfo).append("\n");
}

void CErrLogMgr::WriteDeviceInfoLog(const char* szInfo)
{
	if(!szInfo)
		return;
	m_strDeviceInfo.append(szInfo).append("\n");
	m_uLastLogTime = uint32(-1);
}

void CErrLogMgr::SetInterval(uint32 uInterval)
{
	if(uInterval == 0)
		return;

	m_uInterval = uInterval;
}

void CErrLogMgr::SetLogFileName(const wchar_t* szFileName)
{
	m_strLogFile.assign(szFileName);
}

