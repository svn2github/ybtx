#pragma once 
#include "CMegaLogAllocator.h"
#include "ThreadTypes.h"

#ifdef _WIN32
#include <hash_set>
using namespace stdext;
#else
#include <ext/hash_set>
using namespace __gnu_cxx;

#endif

namespace sqr 
{
	class CMegaLogJob;
	class CMemPool;
	class CSyncMegaLog;

	class CMegaLogMgr
	{
	public:
		static void Init();
		static void Unit();
		static CMegaLogMgr* Inst();

		void AddJob(CMegaLogJob* pJob);

		CMegaLogMgr();
		~CMegaLogMgr();

		static void Execute(void* pParam);
		void DoExecute();

		void AddFlushLog(CSyncMegaLog* pLog);
		void RemoveFlushLog(CSyncMegaLog* pLog);
		void FlushLogs();

		void HandleAllJobs();

	private:
		static CMegaLogMgr*		ms_pInst;

		HTHREAD		m_hThread;


#ifdef _WIN32
		typedef hash_set<CSyncMegaLog*, hash_compare<CSyncMegaLog*, less<CSyncMegaLog*> >,
			CMegaLogAllocator<CSyncMegaLog*> > SetFlushLog_t;
#else
		struct SyncLogHash
		{
			size_t operator() (const CSyncMegaLog* pLog) const
			{
				size_t stPoint = (size_t)pLog;
				return stPoint;
			};
		};

		struct SyncLogEqual
		{
			bool operator() (const CSyncMegaLog* pLog1, const CSyncMegaLog* pLog2) const
			{
				return (size_t)pLog1 == (size_t)pLog2;
			}
		};

		typedef hash_set<CSyncMegaLog*, SyncLogHash, SyncLogEqual, 
			CMegaLogAllocator<CSyncMegaLog*> > SetFlushLog_t;
#endif	

		SetFlushLog_t	m_setFlushLog;

		class VecJob_t
			: public vector<CMegaLogJob*, CMegaLogAllocator<CMegaLogJob*> >
			, public CMegaLogMallocObject {};

		VecJob_t*	m_pWaitingJobQue;
		VecJob_t*	m_pRuningJobQue;

		HLOCK	m_hLock;

		HSEMAPHORE	m_hSem;

		HSEMAPHORE	m_hBlockSem;

		uint32		m_uBlockNum;

		bool		m_bEndThread;
	};
}
