#pragma once
#include "ThreadTypes.h"
#include "CLock.h"
#include "TTickHandler.h"

namespace sqr
{
	class CThreadLoopInfo;

	class CThreadWatcher
	{
	public:
		void Begin();
		void End();
		bool Enabled()const;
		
		static CThreadWatcher* Inst();

	private:
		CThreadWatcher(void);
		~CThreadWatcher(void);

		uint32		m_uBeginCounter;

		HTHREAD		m_hThread;
		HSEMAPHORE	m_semQuit;
		static void EntryProc( void* pArg);

		void CheckInfiniteLoop();
		void CheckBurstCpuUsage();
		void LowerSignalCounter();

		void Execute();

		CLock		m_lkBeginEnd;		
	};

}

