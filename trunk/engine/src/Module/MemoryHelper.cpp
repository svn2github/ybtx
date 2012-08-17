#include "stdafx.h"
#include "MemoryHelper.h"
#include "CMemThreadMgr.h"
#include "CMemoryLeakageMgr.h"

#ifdef _WIN32
#include <Psapi.h>
#endif

namespace sqr 
{
	void GetProcessMemInfo(ProcessMemInfo* pMemInfo)	
	{
#ifdef _WIN32
		MEMORYSTATUS	memStatus;

		memStatus.dwLength = sizeof(MEMORYSTATUS);

		GlobalMemoryStatus(&memStatus);

		DWORD dwPhysMem = memStatus.dwAvailPhys;
		DWORD dwPageMem = memStatus.dwAvailPageFile;

		dwPhysMem = (dwPhysMem/1024)/1024;
		dwPageMem = (dwPageMem/1024)/1024;

		PROCESS_MEMORY_COUNTERS curProMemInfo;
		curProMemInfo.cb = sizeof(curProMemInfo);
		GetProcessMemoryInfo(GetCurrentProcess(), &curProMemInfo, sizeof(curProMemInfo));

		DWORD dwCurProPhysUsed = curProMemInfo.WorkingSetSize;
		DWORD dwCurProPageUsed = curProMemInfo.PagefileUsage; 
		dwCurProPhysUsed = (dwCurProPhysUsed/1024)/1024;
		dwCurProPageUsed = (dwCurProPageUsed/1024)/1024;

		pMemInfo->stAvailPhysMem = dwPhysMem;
		pMemInfo->stAvailPageMem = dwPageMem;
		pMemInfo->stProcessPhys = dwCurProPhysUsed;
		pMemInfo->stProcessPage = dwCurProPageUsed;
#else
		(void)pMemInfo;
#endif

	}

	void WatchLeakage_Begin(int nCheckLevel)
	{
		CMemoryLeakageMgr::Init(nCheckLevel);
	}

	void WatchLeakage_End()
	{
		CMemoryLeakageMgr::Unit();
	}

#ifndef _WIN32
	void WatchLeakage_FetchAllSymbol()
	{
		CMemoryLeakageMgr::Inst().FetchAllSymbol();
	}
#endif

	void WatchLeakage_PrintMemInfo()
	{
		CMemoryLeakageMgr::Inst().DumpStack();
	}

	void WatchLeakage_MemLog(const char* szName)
	{
		CMemoryLeakageMgr::Inst().WriteMemLog(szName);
	}

}
