#include "stdafx.h"
#include "ScriptX.h"
#include "CBaseScriptApp.h"

#include "TimeHelper.h"
#include "CTimeCheckPoint.h"


int32 CBaseScriptApp::RegisterModule(CScript& Script)
{
	REGIST_B_CLASS( Script, CTimeCheckPoint );

	REGIST_CLASSFUNCTION_OVERLOAD( Script, CTimeCheckPoint, void (CTimeCheckPoint::*)(uint32),SetCheckPoint,SetCheckPoint, "I");
	REGIST_CLASSFUNCTION( Script, CTimeCheckPoint, SetBaseTime, "Z");
	REGIST_CLASSFUNCTION( Script, CTimeCheckPoint, GetElapse, ":Z");
	REGIST_GLOBALFUNCTION(Script, GetProcessTime, ":Z" );
	REGIST_GLOBALFUNCTION(Script, GetHDProcessTime, ":Z");

	return 1;
}

