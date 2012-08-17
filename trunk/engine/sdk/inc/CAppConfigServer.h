#pragma once
#include "TAppConfig.h"
#include "CSyncTraitsServer.h"

namespace sqr
{

	class CAppConfigServer
		:public TAppConfig<CSyncTraitsServer>
	{
	public:
		CAppConfigServer(istream& strmConfig);
		~CAppConfigServer(void);

		uint32 GetAppSleepTime()const;
		uint32 GetFollowerDelay()const;
		uint32 GetLogMemInterval()const;

		uint32 GetConnServerTimeOut()const;
		bool InfiniteLoopDetectionEnabled()const;

		uint32 GetTripleBufferInitDataNum() const;

		void SetClientTimeStampErrorThreshold(uint32 uClientTimeStampErrorThreshold);
		void SetMaxStepCommandRate(float fMaxStepCommandRate);
		uint32 GetClientTimeStampErrorThreshold() const;
		float GetMaxStepCommandRate() const;
		string GetOriLanFolder() const;
		string GetLanFolder() const;

		uint32 GetChildVMGCStep()const;
		uint32 GetChildVMGCStepInterval()const;
		float GetChildVMGCCallbackRatio()const;
		float GetChildVMGCNormalUnpackRatio()const;
		float GetChildVMGCSerialUnpackRatio()const;

		bool GetReload()const;

		int32 MegaLogInterval() const;

		uint32 MegaLogMaxFileSize() const;

		bool TestingServerEnabled() const;

	private:
		typedef TAppConfig<CSyncTraitsServer> ParentConfig_t;

		uint32	m_uFollowerDelay;
		uint32	m_uChildVMNum;
		uint32  m_uConnTimeOut;
		uint32	m_uTripleBufferDataNum;
		uint32	m_uClientTimeStampErrorThreshold;
		uint32	m_uAppSleepTime;
		float	m_fMaxStepCommandRate;
		string  m_szOriLanFolder;
		string  m_szLanFolder;
		bool	m_bReload;
		int32	m_nMegaLogInterval;
		uint32 m_uMegaLogMaxSize;
		bool	m_bTestingServer;	
	};

}

