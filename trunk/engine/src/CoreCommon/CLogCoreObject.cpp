#include "stdafx.h"
#include "CLogCoreObject.h"
#include "CLog.h"
#include "ErrLogHelper.h"
#include "CodeCvs.h"


#ifndef _WIN32
#	define _countof(array)	(sizeof(array)/array[0])
#endif

CLogCoreObject::CLogCoreObject()
	: m_pLog(NULL)
{
	wchar_t szLogFileName[256];
	
	wchar_t szTimeBuf[64];
	GetCurLogTime(szTimeBuf, _countof(szTimeBuf));

#ifdef _WIN32
	swprintf_s(szLogFileName, _countof(szLogFileName), L"log_%s.log", szTimeBuf);
#else
	swprintf(szLogFileName, _countof(szLogFileName), L"log_%ls.log", szTimeBuf);
#endif

	m_pLog = CLog::CreateLog(utf16_to_utf8(szLogFileName).c_str());
	m_pLog->EnableTime(true);
}

CLogCoreObject::~CLogCoreObject()
{
	if(m_pLog)
		m_pLog->Release();
}

CLogCoreObject& CLogCoreObject::Inst()
{
	static CLogCoreObject ls_Inst;
	return ls_Inst;
}

void CLogCoreObject::LogMsg(const string &strMsg)
{
	m_pLog->Write(strMsg.c_str());
}
