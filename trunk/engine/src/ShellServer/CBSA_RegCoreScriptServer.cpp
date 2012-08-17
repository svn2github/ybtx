#include "stdafx.h"
#include "CBaseScriptApp.h"
#include "CScriptAppServer.h"
#include "ScriptX.h"
#include "CFileWatcherData.h"
#include "TSqrAllocator.inl"

int32 CBaseScriptApp::RegisterCoreScriptServer(CScript& Script)
{
	REGIST_ABSTRACT_D_CLASS( Script, CScriptAppServer, CBaseScriptApp );

	REGIST_CLASSFUNCTION( Script, CScriptAppServer, GetChildVMNum, ":I" );
	
	REGIST_CLASSFUNCTION( Script, CScriptAppServer, ChangeChildVMFile, "(CFileWatcherData)" );

	REGIST_STATICFUNCTION(Script, CScriptAppServer, StartCallBackProfiler, "");
	REGIST_STATICFUNCTION(Script, CScriptAppServer, StopCallBackProfiler, "");

	return true;
}

