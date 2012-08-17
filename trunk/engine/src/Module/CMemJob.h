#pragma once 
#include <vector>
#include "CMallocObject.h"

namespace sqr
{
	class CMemJob : public CMallocObject
	{
	public:
		virtual ~CMemJob(){}
	};

	class CMemCallStackInfo;
	class CMemCallStackAddrVector;
	struct CMemoryCookie;

	class CMemMallocJob : public CMemJob
	{
	public:
		CMemMallocJob(CMemoryCookie* pCookie);
		~CMemMallocJob();
	private:
		CMemoryCookie*		m_pCookie;
	};

	struct CMemoryCookie;

	class CMemFreeJob : public CMemJob
	{
	public:
		CMemFreeJob(CMemoryCookie* pCookie, CSqrAllocPool* pPool);
		~CMemFreeJob();
	private:
		CMemoryCookie*		m_pCookie;
		CSqrAllocPool*		m_pPool;
	};

#ifndef _WIN32
	class CMemFetchAllSymbolJob : public CMemJob
	{
	public:
		CMemFetchAllSymbolJob();
		~CMemFetchAllSymbolJob();
	};
#endif

	class CMemDumpStackJob : public CMemJob
	{
	public:
		~CMemDumpStackJob();
	};

	class CMemThread;

	class CMemEndThreadsJob : public CMemJob
	{
	public:
		CMemEndThreadsJob (CMemThread *pMemThread);
		~CMemEndThreadsJob ();
	private:
		CMemThread*	m_pMemThread;
	};

}

