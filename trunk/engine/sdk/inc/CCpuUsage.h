#pragma once
#include "CommonDefs.h"
#include "TSingleton.h"
#include "ThreadTypes.h"

namespace sqr
{
	class COMMON_API CSystemCpuUsage
	{
	public:
		CSystemCpuUsage();
		~CSystemCpuUsage();
		int32 CpuUsage();
		int32 CpuUsageMultiCore(uint32& numCores,uint32*& array);
	private:
		int32 CpuUsageMultiCorebyPIDandTID(uint32& numCores,uint32*& array);
		void GetData(int64& busy, int64& idle, int64*& busyArray,int64*& idleArray);

		int64 m_oldBusyTime;
		int64 m_oldIdleTime;

		int64* m_oldBusyTimeMulti;
		int64* m_oldIdleTimeMulti;

		int64* m_idleArray;
		int64* m_busyArray;
		uint32* m_resultArray;
		uint32  m_numCores;
	};

	class COMMON_API CProcessCpuUsage 
	{
	public:
		CProcessCpuUsage();
		int32 CpuUsageCurProcess();
	private:
		void GetData(int64& processTime, int64& totelTime);
		int64 m_oldProcessCpuTime;
		int64 m_oldTotelCpuTime;
		uint32  m_numCores;
	};

	class COMMON_API CThreadCpuUsage
	{
	public:
		CThreadCpuUsage(HTHREADID tid);
		int32 CpuUsageCurProcessbyTID(HTHREADID tid);
	private:
		void GetData(int64& processTime, int64& totelTime,HTHREADID tid);
		int64 m_oldThreadCpuTime;
		int64 m_oldTotelCpuTime;
		uint32  m_numCores;
	};

	class COMMON_API CWatchCpuThreadID : public TSingleton<CWatchCpuThreadID>
	{
		friend class TSingleton<CWatchCpuThreadID>;
	public:
		void AddWatchCpuThreadID(const char* szThreadName,HTHREADID tid);
		const vector<pair<string,HTHREADID> >& GetThreadIDVec()const {return m_vecThreadID;}
	private:
		CWatchCpuThreadID();
		~CWatchCpuThreadID();
		vector<pair<string,HTHREADID> > m_vecThreadID;
		HLOCK	m_hLock;
	};
} 
