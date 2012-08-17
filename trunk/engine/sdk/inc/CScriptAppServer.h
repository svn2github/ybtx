#pragma once
#include "TBaseScriptApp.h"

namespace sqr
{
	class CScript;

	class CFileWatcherData;

	class CMegaLog;

	class CAppConfigServer;
		
	class CScriptAppServer
		:public TBaseScriptApp<CScriptAppServer>
	{
	public:
		typedef CAppConfigServer	Config_t;

		CScriptAppServer(const char* szConfig, const string& strInstancePath, const string& strInstanceName);
		~CScriptAppServer(void);

		string Run( const char* szFileName, const char* szVMFileName );

		CScript* GetChildVM(uint32 uNum);

		uint32 GetChildVMNum() const;
		void ChangeChildVMFile(CFileWatcherData* pData);

		void CreateShellLog(const char* szFileName, const char* szLogFormatVer);
		void DestroyShellLog();
		CMegaLog* GetShellLog();

		static void StartCallBackProfiler();
		static void StopCallBackProfiler();

	private:
		void CreateAllChildVM();
		void ReleaseAllChildVM();

		void RegisterCoreScript();
		#ifndef _WIN32
		int  m_fdLock;
		#endif

		CMegaLog*	m_pShellLog;
	};
}
