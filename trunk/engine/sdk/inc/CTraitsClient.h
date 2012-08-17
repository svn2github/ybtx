#pragma once


namespace sqr
{
	#define USE_RENDER_SLOT

	class CAppClient;
	class IAppClientHandler;
	class CAppConfigClient;
	class CTimeSystemClient;
	class CConnClient;
	class CMetaSceneClient;
	class CMetaSceneMgrClient;
	class CObjMoveStateClient;
	class CCoreObjectClient;
	class CCoreSceneClient;
	class CCoreSceneMgrClient;
	class ICoreObjectClientHandler;
	class CConnMgrClient;
	class IConnMgrClientHandler;
#ifdef USE_RENDER_SLOT
	class CRenderSlot;
#else
	class CDistortedTick;
#endif

	class CTraitsClient
	{
	public:
		typedef CAppClient						App_t;
		typedef IAppClientHandler				AppHandler_t;
		typedef CAppConfigClient				AppConfig_t;
		typedef CTimeSystemClient				TimeSystem_t;
		typedef CConnClient						Conn_t;
		typedef CMetaSceneClient				MetaScene_t;
		typedef CMetaSceneMgrClient				MetaSceneMgr_t;
		typedef CCoreObjectClient				CoreObject_t;
		typedef CObjMoveStateClient				ObjMoveState_t;
		typedef CCoreSceneClient				CoreScene_t;
		typedef CCoreSceneMgrClient				CoreSceneMgr_t;
		typedef ICoreObjectClientHandler		CoreObjectHandler_t;
		typedef CConnMgrClient					ConnMgr_t;
		typedef IConnMgrClientHandler			ConnMgrHandler_t;
		typedef CConnClient						ConcreteConn_t;
#ifdef USE_RENDER_SLOT
		typedef CRenderSlot						Tick_t;
#else
		typedef	CDistortedTick					Tick_t;
#endif

		static uint32 GetObjectMoveCyc();
	};
}

