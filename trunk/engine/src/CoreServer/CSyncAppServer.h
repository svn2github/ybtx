#pragma once

#include "TCoreApp.h"
#include "CSyncTraitsServer.h"


namespace sqr
{
	class CConnMgrServer;
	class CSynMultiMsgSender;
	class CLog;
	class CError;
	class CAppServer;
	class CCoreMsgMgr;
	class CLogMemTick;
	class CCoreMsgSignal;

	class CSyncAppServer
		: public TCoreApp<CSyncTraitsServer>
	{
		typedef TCoreApp<CSyncTraitsServer>	ParentApp_t;

		friend class CAppServer;
		friend class CConnMgrServer;
		friend class TApp<CSyncTraitsServer>;
		friend class CAoiThreadMgr;
		friend class CSyncCoreSceneMgrServer;
		friend class CCoreServerThreadMgr;
		friend class CCoreServerThreadMgrHandler;
		friend class CCoreServerAppCleanJob;
		friend class CCoreMsgSignal;

		friend class CSyncServerTrMsgBufferMgrSwapper;

	public:
		static void Init();
		static void Unit();

		static uint32& GetCounter();

		bool DoingQuit()const;
		bool Quit();
		
		CSynMultiMsgSender* GetMultiMsgSender(uint32 uID)const;
		
		void Log(const char* szMsg);
		void LogMem();

		void CleanUp();

	private:
		void OnMainFrameEndedDerived();
		void OnMainCycleEndedDerived();

		void OnBeginMainCycleDerived();

		uint32 OnIdle( uint32 uTimeLeft );

		static const char* GetThreadName();

#ifndef _WIN32
		void FlushSigMsgDerived();
#endif

		CSyncAppServer();
		~CSyncAppServer();

		bool OnCanceled();
#ifdef _WIN32
		void OnSysMsg();
#endif
		void OnSceneMgrHalted();
		
		//server app logœ‡πÿ
		void CreateAppLog();
		void ReleaseAppLog();

		CLog*			m_pAppLog;
		CLog*			m_pMemLog;
		CLogMemTick*	m_pMemTick;

		void IntCleanUp();
		bool	m_bCleanUp;
	};
}
