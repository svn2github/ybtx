#pragma once



namespace sqr
{
	class CSyncAppServer;
	class IAppServerHandler;
	class CAppConfigServer;
	class CSyncTimeSystemServer;
	class CSynConnServer;
	class CSyncMetaSceneServer;
	class CSyncMetaSceneMgrServer;
	class CSyncCoreObjectServer;
	class CObjMoveStateServer;
	class CSyncCoreSceneServer;
	class CSyncCoreSceneMgrServer;
	class ISyncCoreObjectServerHandler;
	class CConnServer;
	class CSynConnMgrServer;
	class IConnMgrServerHandler;
	class CDistortedTick;
	class CConnMgrServer;
	class ISynConnMgrServerHandler;

	class CSyncTraitsServer
	{
	public:
		typedef CSyncAppServer					App_t;
		typedef IAppServerHandler				AppHandler_t;
		typedef CAppConfigServer				AppConfig_t;
		typedef CSyncTimeSystemServer			TimeSystem_t;
		typedef CSynConnServer					Conn_t;
		typedef CSyncMetaSceneServer			MetaScene_t;
		typedef CSyncMetaSceneMgrServer			MetaSceneMgr_t;
		typedef CSyncCoreObjectServer			CoreObject_t;
		typedef CObjMoveStateServer				ObjMoveState_t;
		typedef	CSyncCoreSceneServer			CoreScene_t;
		typedef CSyncCoreSceneMgrServer			CoreSceneMgr_t;
		typedef ISyncCoreObjectServerHandler	CoreObjectHandler_t;
		typedef CSynConnMgrServer				ConnMgr_t;
		typedef ISynConnMgrServerHandler		ConnMgrHandler_t;
		typedef CDistortedTick					Tick_t;

		static uint32 GetObjectMoveCyc();
		static uint32 GetDefaultFollowerDelay();
	};
}

