//该文件将CoreStress的函数导出到lua空间

#include "stdafx.h"
#include "ScriptX.h"
#include "IAppStressHandler.h"
#include "CBaseScriptApp.h"
#include "CAppStress.h"

#include "IConnMgrStressHandler.h"

#include "CConnMgrStress.h"
#include "CConnStress.h"
#include "IPipe.h"
#include "ScriptRpc.h"

#include "CCoreObjectStress.h"
#include "TSqrAllocator.inl"

int32 CBaseScriptApp::RegisterCoreStress(CScript& Script)
{
	// 注册StressConnClient
	typedef TConnection<CTraitsStress> CBaseConnectionStress;
	REGIST_ABSTRACT_B_CLASS(Script, CBaseConnectionStress );
	REGIST_ABSTRACT_D_CLASS(Script, CConnStress, CBaseConnectionStress );


	typedef TConnectionMgr<CTraitsStress> CBaseConnMgrStress;
	REGIST_ABSTRACT_B_CLASS( Script, CBaseConnMgrStress );
	REGIST_ABSTRACT_D_CLASS( Script, CConnMgrStress, CBaseConnMgrStress );
	REGIST_B_CLASS( Script, IConnMgrStressHandler );

	// 注册CAppStress
	REGIST_B_CLASS( Script, IAppStressHandler );
	typedef TApp<CTraitsStress> CBaseAppStress;
	REGIST_ABSTRACT_B_CLASS( Script, CBaseAppStress );
	REGIST_ABSTRACT_D_CLASS( Script, CAppStress, CBaseAppStress );

	//REGIST_B_CLASS(Script, CCoreObjectStress);
	REGIST_STATICFUNCTION(Script, CCoreObjectStress, SetBeginPosition, "(CConnStress)ff");
	REGIST_STATICFUNCTION(Script, CCoreObjectStress, SetEndPosition, "(CConnStress)ff");
	REGIST_STATICFUNCTION(Script, CCoreObjectStress, MoveTo, "(CConnStress)ff");

	REGIST_CALLBACKFUNCTION( Script, IAppStressHandler, OnClose, "" );

	REGIST_CLASSFUNCTION( Script, CBaseAppStress, Run, ":i" );
	REGIST_CLASSFUNCTION( Script, CBaseAppStress, GetConnMgr, ":(CConnMgrStress)" );
	REGIST_CLASSFUNCTION_OVERLOAD( Script, CBaseAppStress, void (CBaseAppStress::*)(CTick*,uint32),RegisterTick,RegisterTick, "(CTick)I" );
	REGIST_CLASSFUNCTION( Script, CBaseAppStress, UnRegisterTick, "(CTick)" );
	REGIST_CLASSFUNCTION( Script, CBaseAppStress, GetFrameTime, ":Z" );
	REGIST_CLASSFUNCTION( Script, CBaseAppStress, SetHandler, "(IAppStressHandler)" );
	REGIST_CLASSFUNCTION( Script, CBaseAppStress, GetHandler, ":(IAppStressHandler)" );
	REGIST_STATICFUNCTION( Script, CBaseAppStress, Inst, ":(CAppStress)" );

	REGIST_CLASSFUNCTION( Script, CAppStress, Quit, "" );
	

	REGIST_CLASSFUNCTION_OVERLOAD( Script, CBaseAppStress ,IPipe*(CBaseAppStress::*)(IPipeHandler*)
		,GetIPipe, GetIPipe, "(IPipeHandler):(IPipe)" );

	REGIST_CLASSFUNCTION_OVERLOAD( Script, CBaseAppStress ,IPipe*(CBaseAppStress::*)(IPipeHandler*,size_t,size_t,bool)
		,GetIPipe, GetIPipeWithBufferSize, "(IPipeHandler)ttb:(IPipe)" );

	//REGIST_CLASSFUNCTION( Script, CBaseConnectionStress, Send, "PII:b" );
	REGIST_CLASSFUNCTION( Script, CBaseConnectionStress, IsConnected, ":b" );
	REGIST_CLASSFUNCTION( Script, CBaseConnectionStress, IsShuttingDown, ":b" );
	REGIST_CLASSFUNCTION( Script, CConnStress, GetLatency, ":I");
	REGIST_CLASSFUNCTION_OVERLOAD( Script, CConnStress, void (CConnStress::*)(), ShutDown, ShutDown, "" );


	REGIST_CLASSFUNCTION( Script, CBaseConnMgrStress, SetHandler, "(IConnMgrStressHandler)" );
	REGIST_CLASSFUNCTION( Script, CBaseConnMgrStress, GetHandler, ":(IConnMgrStressHandler)" );
	//REGIST_CLASSFUNCTION( Script, CBaseConnMgrStress, DoShutDownConn, "(CConnStress)" );
	REGIST_CLASSFUNCTION( Script, CConnMgrStress, Connect, "(CAddress):(CConnStress)" );

	//ISend
	CREATE_RPC_CHANNEL(Script,CConnStress,CConnStress);
	return 1;

}
