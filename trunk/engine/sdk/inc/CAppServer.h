#pragma once
#include "TApp.h"
#include "CTraitsServer.h"
#include "TAppWithConsole.h"

namespace sqr
{
	class IPipe;
	class IPipeHandler;
	class IPipeModule;
	class IPipeReactor;
	class IMulticast;

	class CError;
	class CTick;
	class CTickMgr;
	class CSyncAppServer;

	class CAppThreadEvent;

	class CConnMgrServer;
	class CSynMultiMsgSender;
	
	class IDatabase;

	class IAppServerHandler;
	class CMetaSceneMgrServer;
	class CCoreSceneMgrServer;
	class CMultiMsgSender;

	class CAppServer
		:public TApp<CTraitsServer>
		,public TAppWithConsole<CTraitsServer>
	{
		typedef TApp<CTraitsServer>	ParentApp_t;
		friend class TApp<CTraitsServer>;
		friend class CSyncAppServer;
		friend class CDbThread;
		friend class CDbConnectionMgr;
		friend class CCoreServerThreadMgr;
		
		friend class CCoreServerAppQuitResult;
		friend class CCoreServerAppCleanResult;
	
		friend class CAsynServerTrMsgBufferSwapper;

	public:
		static void Init();
		static void Unit();
		
		bool DoingQuit()const;
		bool Quit();

		CMultiMsgSender* GetMultiMsgSender()const;
		
		IDatabase* GetDatabase() const;

		void Log(const char* szMsg);

		void PostThreadAppEvent(CAppThreadEvent* pAppThreadEvent);

		void PrintRegisterTickInfo();
		void PrintRegisterTickNum(uint32 uLuaTickNum);

		void CleanUp();

	private:

		CAppServer();
		~CAppServer();

		static const char* GetThreadName();

		void CreatePipeReactor();

		void OnBeginMainCycleDerived();
		void OnMainFrameEndedDerived();		

#ifndef _WIN32
		static void SigHandler(int signum);
#else
		static BOOL WINAPI HandlerRoutine(DWORD dwCtrlType);
#endif

		uint32 OnIdle( uint32 uTimeLeft );
		void OnCanceled();
		
		/**********************
		DataBaseœ‡πÿ
		**********************/
		void CreateDatabase();
		void ReleaseDatabase();

		IDatabase*	m_pDatabase;	

		bool				m_bDoingQuit;

		void IntCleanUp();
		bool				m_bCleanUp;
	};
}
