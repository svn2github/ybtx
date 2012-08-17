#include "stdafx.h"
#include "CBaseScriptApp.h"
#include "ScriptX.h"
#include "CoreObjectDefs.h"
#include "TSqrAllocator.inl"

int32 CBaseScriptApp::RegisterCoreScriptCommon(CScript& Script)
{
	REGIST_ABSTRACT_B_CLASS( Script, CBaseScriptApp );

	REGIST_STATICFUNCTION( Script, CBaseScriptApp, Inst, ":(CBaseScriptApp)" );
	REGIST_CLASSFUNCTION( Script, CBaseScriptApp, GetRootFilePath, "s:s" );
	
	return true;
}

