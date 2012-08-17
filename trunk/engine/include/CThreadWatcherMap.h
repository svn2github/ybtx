#pragma once
#include "ModuleDefs.h"
#include "CMallocObject.h"
#include "TMallocAllocator.h"
#include "CLock.h"

#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable:4275)	//dll linkage
#pragma warning(disable:4251)	//dll linkage
#endif


namespace sqr
{
	class MODULE_API CThreadLoopInfo
		: public CMallocObject
	{
	public:
		CThreadLoopInfo(const char* szThreadName,volatile uint64* pTimeVar 
			, uint64 uAllowedDelay ,uint32 uAllowdTimes,uint64 uSlowValue );

		volatile uint64*	m_pTimeVar;
		uint64	m_uSlowValue;
		uint64	m_uAllowedDelay;
		uint32	m_uWakeSignalCounter;
		uint32	m_uAllowedTimes;	//允许发生死循环的次数
		uint32	m_uLogStackSignalCounter;
		char	m_szThreadName[32];
		bool	m_bEnabled;
	};

	class MODULE_API CThreadWatcherMap
	{
	public:
		static void Init();
		static void Unit();
		static CThreadWatcherMap* Inst();

		void AddThread(HTHREAD hThread,const char* szThreadName,volatile uint64* pTimeVar
			,uint64 uAllowedDelay,uint32 uAllowedTimes,uint64 uSlowValue);
		void DelThread(HTHREAD hThread);

		void EnableWatch(HTHREAD hThread, bool bEnabled);

		void Lock();
		void Unlock();

		typedef map< HTHREAD , CThreadLoopInfo*, 
			less<HTHREAD>, TMallocAllocator<pair<HTHREAD, CThreadLoopInfo*> > >	VarMap_t;
		
		VarMap_t m_mapVar;

		CLock		m_lkVarMap;

		static uint64 GetMaxAllowedDelay();

	private:
		static CThreadWatcherMap*	ms_pThreadMap;
		void Clear();
		CThreadWatcherMap();
		~CThreadWatcherMap();

	};
}

#ifdef _WIN32
#pragma warning(pop)
#endif
