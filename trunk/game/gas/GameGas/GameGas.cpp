//$id$
//game server的启动程序的主体

#include "stdafx.h"
#include "ScriptX.h"
#include "ExpHelper.h"
#include "RegistGasShell.h"
#include "CScriptAppServer.h"
#include "CPathMgr.h"
#include "Memory.h"
#include "Module.h"
#include "ErrLogHelper.h"
#include "RegistShellCommon.h"
#include "MemoryHelper.h"
#include "PkgStream.h"
#include "CodeCvs.h"
#include "StringHelper.h"
#include "ErrLogHelper.h"
#include "CGameServerProxyBank.h"

#ifdef _WIN32
#include <direct.h>
#else
#include <sys/stat.h>
#include <sys/types.h>
#endif

DEFINITION_OF_OPERATOR_NEW

#ifdef _WIN32
SQRENTRY int32 SqrMain(int32 argc, wchar_t* argv[])
#else
SQRENTRY int32 SqrMain(int32 argc, char* argv[])
#endif
{
	/*
	const size_t psize=1024*1024*10;
	char** p=(char**)SqrChunkNew( psize * sizeof(char*) );
	for( int i=0;i<psize;++i )
	{
		p[i]=new char[40];
	}
	for( int i=0;i<psize;++i )
	{
		delete[] p[i];
	}
	SqrDelete(p);
	for(;;)		delete[] new char[40];
	//SqrTrim(0);
	*/


	SQR_TRY
	{
		string strInstanceName = "1";
		if (argc > 1)
		{
#ifdef _WIN32
			strInstanceName = utf16_to_gbk( wstring(argv[1]) );
#else
			strInstanceName= string(argv[1]);
#endif
		}

		InitGameServerProxyBank();		

		{
			CScriptAppServer Runner("etc/gas/GasConfig.xml", "etc/gas", strInstanceName);

			//对于windows来说，我们将logic的log放到var/gas下面
			//linux放到~/ybtx_log下面
#ifdef _WIN32
			string sLogPath = utf16_to_utf8(GetLogPath());
			_mkdir(sLogPath.c_str());
#else
			char* path = getenv("HOME");
			string sLogPath;
			sLogPath.append(path).append("/").append("ybtx_log");
			mkdir(sLogPath.c_str(), 0777);
#endif

			string sShellLog(sLogPath);
			sShellLog.append("/").append("Game_").append(strInstanceName);

			ostringstream strm;
			strm<<GetCurrentRevision();
			Runner.CreateShellLog(sShellLog.c_str(), strm.str().c_str());

			CScript* pScript = Runner.GetMainVM();

			string sError;

			//载入部署文件，设置head msg
			stringstream sDeploy;
			sDeploy << "etc_gas_require \"GasDeployConfig\"" << endl;
			sDeploy << "ErrLogHeadMsg(\"Process Name\", DeployConfig.ServerName)" << endl;
			sError=pScript->RunString(sDeploy.str().c_str());
			if( !sError.empty() )
				GenErr(sError);

			RegistGasShell(*pScript);
			string strScriptCode = format("g_CurServerId = %s", strInstanceName.c_str());
			pScript->RunString(strScriptCode.c_str());

			uint32 uChildVMNum = Runner.GetChildVMNum();

			for(uint32 i = 0; i < uChildVMNum; i++)
			{
				pScript = Runner.GetChildVM(i);
				RegistShellCommon(*pScript);
				pScript->RunString(strScriptCode.c_str());
			}

			string gas_path = pScript->GetLoadPath("gas");
			string gas_main = gas_path + string("framework/main_frame/GasMain");
			string gas_thread_main = gas_path + string("framework/main_frame/ThreadGasMain");
			sError=Runner.Run(gas_main.c_str(), gas_thread_main.c_str());

			Runner.DestroyShellLog();

			SetErrLogEnabled(false);

			if( !sError.empty() )
				GenErr(sError);	

		}

		UnitGameServerProxyBank();

	}
	SQR_CATCH(exp)
	{
		LogExp(exp);
	}
	SQR_TRY_END;

	return 0;
}

