#include "stdafx.h"

#include "RegistStrShell.h"
#include "RegistShellCommon.h"

#include "ScriptX.h"
#include "CConnMgrStrHandler.h"
#include "CBaseConnMgrHandler.inl"
#include "CScriptAppStress.h"
#include "CFighterStress.h"

void RegistStressShell(CScript& Script)
{
	RegistShellCommon(Script);

	REGIST_B_CLASS( Script, IScriptConnMgrStressHandler);
	REGIST_CALLBACKFUNCTION( Script, IScriptConnMgrStressHandler, OnDataReceived, "(CConnStress)");
	REGIST_CALLBACKFUNCTION( Script, IScriptConnMgrStressHandler, OnConnected, "(CConnStress)" );
	REGIST_CALLBACKFUNCTION( Script, IScriptConnMgrStressHandler, OnConnectFailed, "(CConnStress)" );
	REGIST_CALLBACKFUNCTION( Script, IScriptConnMgrStressHandler, OnDisconnect, "(CConnStress)" );
	REGIST_CALLBACKFUNCTION( Script, IScriptConnMgrStressHandler, OnError, "(CConnStress)s");

	typedef CBaseConnMgrHandler<CConnStress, IScriptConnMgrStressHandler> CBaseConnStressMgrHandler;
	REGIST_B_CLASS( Script, CBaseConnStressMgrHandler );
	REGIST_CLASSFUNCTION( Script, CBaseConnStressMgrHandler, SetScriptHandler, "(IScriptConnMgrStressHandler)" );
	REGIST_CLASSFUNCTION( Script, CBaseConnStressMgrHandler, GetScriptHandler, ":(IScriptConnMgrStressHandler)" );


	REGIST_D_CLASS_2( Script, CConnMgrStressHandler, IConnMgrStressHandler, CBaseConnStressMgrHandler);
	
	REGIST_CLASSFUNCTION( Script, CConnMgrStressHandler, OnConnected, "(CConnStress)" );
	REGIST_CLASSFUNCTION( Script, CConnMgrStressHandler, OnConnectFailed, "(CConnStress)" );
	REGIST_CLASSFUNCTION( Script, CConnMgrStressHandler, OnDisconnect, "(CConnStress)" );
	REGIST_CLASSFUNCTION( Script, CConnMgrStressHandler, OnError, "(CConnStress)s");

	REGIST_B_CLASS( Script, CFighterStress );
	REGIST_STATICFUNCTION( Script, CFighterStress, InitFighterStress, "(CConnStress):(CFighterStress)" );
	REGIST_CLASSFUNCTION( Script, CFighterStress, RemoveFighterStress, "" );
	REGIST_CLASSFUNCTION( Script, CFighterStress, GetGlobalID, ":I" );
	REGIST_CLASSFUNCTION( Script, CFighterStress, DoNormalSkill, "ICffI" );
}

