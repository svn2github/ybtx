#pragma once


namespace sqr
{
	class CAppStress;
	class IAppStressHandler;
	class CAppConfigStress;
	class CTimeSystemStress;
	class CConnStress;
	class CConnMgrStress;
	class IConnMgrStressHandler;
	class EmptyClass {};
	class CAppConfigStress;

	class CTraitsStress
	{
	public:
		typedef CAppStress			App_t;
		typedef IAppStressHandler		AppHandler_t;
		typedef CAppConfigStress		AppConfig_t;
		typedef CTimeSystemStress		TimeSystem_t;
		typedef CConnStress			Conn_t;
		typedef CConnMgrStress			ConnMgr_t;
		typedef IConnMgrStressHandler		ConnMgrHandler_t;
		typedef CConnStress			ConcreteConn_t;
		typedef EmptyClass			CoreScene_t;
	
		static uint32 GetObjectMoveCyc();
	};
}
