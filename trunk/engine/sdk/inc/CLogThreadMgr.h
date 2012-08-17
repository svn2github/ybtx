#pragma once 
#include "ThreadTypes.h"
#include "CLogMallocAllocator.h"

#ifdef _WIN32
#include <hash_set>
using namespace stdext;
#else
#include <ext/hash_set>
using namespace __gnu_cxx;

#endif

namespace sqr 
{
	class CLogJob;
	class CSyncLog;

	class CMemPool;

	class CLogThreadMgr
	{
		friend class CLogJob;
		friend class CCreateLogJob;
		friend class CDestroyLogJob;
		friend class CWriteLogJob;
		friend class CEnableTimeLogJob;

	public:
		static void Init();
		static void Unit();
		static CLogThreadMgr* Inst();

		void AddJob(CLogJob* pJob);

	private:
		CLogThreadMgr();
		~CLogThreadMgr();

		static void Execute(void* pParam);
		void DoExecute();

		CSyncLog* GetLog(uint32 uID);
		void AddLog(CSyncLog* pLog, uint32 uID);
		CSyncLog* RemoveLog(uint32 uID);

		void AddFlushLog(CSyncLog* pLog);
		void RemoveFlushLog(CSyncLog* pLog);
		void FlushLogs();

		void HandleAllJobs();

	private:
		static CLogThreadMgr*	ms_pInst;

		HTHREAD		m_hThread;

#ifdef _WIN32
		typedef hash_map<uint32, CSyncLog*, hash_compare<uint32, less<uint32> >, 
			CLogMallocAllocator<pair<uint32, CSyncLog*> > >	MapID2Log_t;
#else
		typedef hash_map<uint32, CSyncLog*, hash<uint32>, equal_to<uint32>, 
			CLogMallocAllocator<pair<uint32, CSyncLog*> > > MapID2Log_t;
#endif

		//typedef map<uint32, CSyncLog*, less<uint32>, 
		//	CLogMallocAllocator<pair<uint32, CSyncLog*> > >	MapID2Log_t;
		MapID2Log_t				m_mapID2Log;

#ifdef _WIN32
		typedef hash_set<CSyncLog*, hash_compare<CSyncLog*, less<CSyncLog*> >, CLogMallocAllocator<CSyncLog*> > SetFlushLog_t;
#else
		struct SyncLogHash
		{
			size_t operator() (const CSyncLog* pLog) const
			{
				size_t stPoint = (size_t)pLog;
				return stPoint;
			};
		};
		
		struct SyncLogEqual
		{
			bool operator() (const CSyncLog* pLog1, const CSyncLog* pLog2) const
			{
				return (size_t)pLog1 == (size_t)pLog2;
			}
		};

		typedef hash_set<CSyncLog*, SyncLogHash, SyncLogEqual, CLogMallocAllocator<CSyncLog*> > SetFlushLog_t;
#endif	
		//typedef set<CSyncLog*, less<CSyncLog*>, CLogMallocAllocator<CSyncLog*> >	SetFlushLog_t;
		SetFlushLog_t	m_setFlushLog;

		class VecJob_t : public vector<CLogJob*, CLogMallocAllocator<CLogJob*> >
						, public CLogMallocObject {};

		VecJob_t*	m_pWaitingJobQue;
		VecJob_t*	m_pRuningJobQue;

		HLOCK	m_hLock;

		HSEMAPHORE	m_hSem;

		bool		m_bEndThread;
	};
}
