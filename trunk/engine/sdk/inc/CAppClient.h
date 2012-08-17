#pragma once

#include "TCoreApp.h"
#include "TAppWithConsole.h"
#include "IMainWndHandler.h"
#include "CTraitsClient.h"
#include "TTickHandler.h"
#include "SQRRootWnd.h"
#include "TGManager.h"


namespace sqr
{
	class CMainWnd;
	class CTestSuiteCoreObjectClient;
	class IConnMgrClientHandler;
	class CConnMgrClient;
	class CGuiHandler;
	class CAppThreadEvent;
	class CLogOwner;

	class CAppClient
		: public TCoreApp<CTraitsClient>
		, public TAppWithConsole<CTraitsClient>
		, public IMainWndHandler
	{
	public:
		static void Init();
		static void Unit();

		bool DebugConsoleOn()const;

		void SetMainWndTitle(const char* title);

		CMainWnd* GetMainWnd()const
		{
			return m_pMainWnd;
		}

		SQRWnd* GetRootWnd()const
		{
			return m_pRootWnd;
		}

		TGManager* GetTGInst()const
		{
			return m_pTGInst;
		}

		void Render();

		void Quit();

		void PostThreadAppEvent(CAppThreadEvent* pEvent);

	private:
		typedef TCoreApp<CTraitsClient> ParentApp_t;
		friend class TApp<CTraitsClient>;
		friend class CConnMgrClient;
		friend class CConnClient;
		friend class CTestSuiteCoreObjectClient;
		friend class CCoreSceneMgrClient;

		uint32 OnIdle( uint32 uTimeLeft );
		
		//CTimeCheckPoint	m_tcpRender;

		TGManager*				m_pTGInst;		// think gear 实例

		SQRRootWnd*		m_pRootWnd;
		CGuiHandler*	m_pGuiFxRenderer;

		uint32	m_uTimerPeriodMin;

		uint32	m_uProcessorNum;		//cpu内核数目

		CMainWnd* m_pMainWnd;
		CIRect m_rtPreWnd;

		bool m_bIsRendering;			//是否处于渲染状态

		//----------模糊限针------------------
		uint64	m_uLimitCyc;
		uint64	m_uLimitMaxBackCyc;
		uint64	m_uLimitMinBackCyc;
		uint64	m_uLimitBackCyc;
		uint64	m_uUseTime;
		uint64	m_uMaxNum;
		uint64	m_uMinNum;
		uint64	m_u64LastTime;
		uint64	m_uLimitFPS;
		uint32	m_uLimitNum;
		//uint8	m_uLimitTime;
		bool	m_bIsRendered;
		uint32  m_ActiveIndex;
		static  uint64 st_StaticCyc[5];
		static	uint64 st_StaticLevel[4];
		static	uint64 st_StaticLevelUp[4];
		//------------------------------------

		CAppClient();
		~CAppClient();

		void OnBeginMainCycleDerived();
		void OnMainFrameEndedDerived();

		static BOOL WINAPI HandlerRoutine(DWORD dwCtrlType);

		void EnableRendering(bool bEnable);

		uint32 OnInputMessage(uint32 uMsg,uint32 wParam,uint32 lParam);
		void OnMinimize();
		void OnRestore();
		void OnClose();
		void OnCanceled();

		LPTOP_LEVEL_EXCEPTION_FILTER m_pPrevSEFilter;

		static LONG WINAPI SEFilter(PEXCEPTION_POINTERS pExceptionInfo);

		static void OnErrLogCallBack(const char* szErr, CLogOwner* pOwner);	
		static void TellErrMessage(const char* szErr, const char* szType = "Client Error");
		static void ShowNotifyErrIcon();
	};
}
