#include "stdafx.h"
#include "CCallBackProfiler.h"
#include "TimeHelper.h"
#include "BaseHelper.h"
#include "ExpHelper.h"
#include "ErrLogHelper.h"
#include "CAppConfigServer.h"
#include "UtfCvs.h"
#include "ThreadHelper.h"

#ifndef _WIN32
#include <malloc.h>
#define _countof(array) (sizeof(array)/sizeof(array[0]))
#endif

CCallBackProfiler::MapCallBackProfiler CCallBackProfiler::m_mapCallBackProfiler;

CCallBackProfiler::CCallBackProfiler()
{
	m_bOpen = false;
	m_szProfileFileName[0] = L'\0';
	m_fdProfile = NULL;
	m_uAllSpendTime = 0;
}

CCallBackProfiler::~CCallBackProfiler()
{
	if (m_bOpen)
		StopCallBackProfiler();

	m_mapCallBackProfiler.clear();
}

void CCallBackProfiler::Init(HTHREADID TID)
{
	m_mapCallBackProfiler.insert(make_pair(TID, this));
}

void CCallBackProfiler::StartCallBackProfiler()
{
	MapCallBackProfiler::iterator iter = m_mapCallBackProfiler.begin();
	for (; iter != m_mapCallBackProfiler.end(); ++iter)
	{
		(*iter).second->OpenProfileFile((*iter).first);
	}
}

void CCallBackProfiler::StopCallBackProfiler()
{
	MapCallBackProfiler::iterator iter = m_mapCallBackProfiler.begin();
	for (; iter != m_mapCallBackProfiler.end(); ++iter)
	{
		(*iter).second->WriteProfileFile();
	}
}

void CCallBackProfiler::OpenProfileFile(HTHREADID TID)
{
	if (m_bOpen)
		return;

	m_bOpen = true;

	wchar_t szTime[64];
	GetCurLogTime(szTime, _countof(szTime));

#ifdef _WIN32
	swprintf_s(m_szProfileFileName, _countof(m_szProfileFileName), L"%s/CallBack_%lsv%u.log", GetLogPath(), szTime, TID);
#else
	swprintf(m_szProfileFileName, _countof(m_szProfileFileName), L"%ls/CallBack_%lsv%u.log", GetLogPath(), szTime, TID);
#endif

	m_szProfileFileName[ _countof(m_szProfileFileName) - 1 ] = L'\0';

	if (!m_fdProfile)
	{
#ifdef _WIN32
		m_fdProfile = _wfopen(m_szProfileFileName, L"w");
#else
		char* strProfileFileName = U16_to_U8(m_szProfileFileName);
		m_fdProfile = fopen(strProfileFileName, "w");
		FreeUtfCvsDest(strProfileFileName);
#endif
	}

	if( !m_fdProfile )
	{
		wostringstream strm;
		strm << "Open file "<< m_szProfileFileName <<" failed.";
		GenErr( U16_to_U8(strm.str().c_str()) );
	}
}

void CCallBackProfiler::WriteProfileFile()
{
	if (!m_bOpen)
		return;

	m_bOpen = false;

	map<uint64, string> mapProfile;
	MapCallBackData::iterator iter = m_mapCallBackData.begin();
	for (; iter != m_mapCallBackData.end(); ++iter)
	{
		uint64 uSpendTime = (*iter).second->m_uSpendTime;
		uint64 uCount = (*iter).second->m_uCount;
		uint64 uOnceTime = uSpendTime / uCount;
		float fPercent = float(uSpendTime * 100) / m_uAllSpendTime;
		const char* szFunName = ((*iter).first).c_str();

		basic_ostringstream<char, char_traits<char>, TMallocAllocator<char> > strm;
		char buf[1024];
		const char format[] = "%6.2f\t%16u%16u%16u\t%s\n";
		sprintf(buf, format, fPercent, (uint32)uSpendTime, (uint32)uCount, (uint32)uOnceTime, szFunName);
		strm << buf;

		mapProfile.insert(make_pair(uSpendTime, strm.str().c_str()));
	}

	char title[1024];
	const char format[] = "%6s\t%16s%16s%16s\t%s\n";
	sprintf(title, format, "Percent", "SpendTime", "Count", "OnceTime", "FunName");
	fprintf(m_fdProfile, title);
	if (!mapProfile.empty())
	{
		map<uint64, string>::iterator it = mapProfile.end();
		size_t line = mapProfile.size();
		for (size_t i = 0; i < line; ++i)
		{
			--it;
			fprintf(m_fdProfile, (*it).second.c_str());
		}
	}

	ClearMap(m_mapCallBackData);
	m_uAllSpendTime = 0;
	m_szProfileFileName[0] = L'\0';

	if (m_fdProfile)
	{
		fclose( m_fdProfile );
		m_fdProfile = NULL;
	}
}

void CCallBackProfiler::ProfileBegin( const char* szFunName )
{
	MapCallBackData::iterator iter = m_mapCallBackData.find(szFunName);
	if (iter != m_mapCallBackData.end())
	{
		(*iter).second->m_uBeginTime = GetProcessTime();
	}
	else
	{
		CCallBackData* pCallBack = new CCallBackData;
		pCallBack->m_uBeginTime = GetProcessTime();
		pCallBack->m_uSpendTime = 0;
		pCallBack->m_uCount = 0;		
		m_mapCallBackData.insert(make_pair(szFunName, pCallBack));
	}
}

void CCallBackProfiler::ProfileEnd( const char* szFunName )
{
	MapCallBackData::iterator iter = m_mapCallBackData.find(szFunName);
	if (iter != m_mapCallBackData.end())
	{
		uint64 uSpendTime = GetProcessTime() - (*iter).second->m_uBeginTime;
		(*iter).second->m_uBeginTime = 0;
		(*iter).second->m_uSpendTime = iter->second->m_uSpendTime + uSpendTime;
		(*iter).second->m_uCount++;
		m_uAllSpendTime += uSpendTime;
	}
}

bool CCallBackProfiler::IsOpen()
{
	return m_bOpen;
}
