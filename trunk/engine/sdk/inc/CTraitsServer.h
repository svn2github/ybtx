#pragma once

namespace sqr
{

	class CAppServer;
	class CAppConfigServer;
	class CAsynTimeSystemServer;
	class CConnMgrServer;
	class CConnServer;
	class IAppServerHandler;
	class CMetaSceneServer;
	class CMetaSceneMgrServer;
	class CCoreSceneServer;
	class CCoreSceneMgrServer;
	class CDistortedTick;

	class CTraitsServer
	{
	public:
		typedef CAppServer						App_t;
		typedef IAppServerHandler				AppHandler_t;
		typedef CAppConfigServer				AppConfig_t;
		typedef CConnServer						Conn_t;
		typedef CConnMgrServer					ConnMgr_t;
		typedef CAsynTimeSystemServer			TimeSystem_t;
		typedef CMetaSceneServer				MetaScene_t;
		typedef CMetaSceneMgrServer				MetaSceneMgr_t;
		typedef	CCoreSceneServer				CoreScene_t;
		typedef CCoreSceneMgrServer				CoreSceneMgr_t;
		typedef CDistortedTick					Tick_t;
	};

}
