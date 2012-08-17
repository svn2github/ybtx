//该文件将CoreServer的函数导出到lua空间

#include "stdafx.h"
#include "ScriptX.h"

#include "CMetaSceneServer.h"
#include "CMetaSceneMgrServer.h"
#include "CCoreSceneServer.h"
#include "CCoreSceneMgrServer.h"
#include "CCoreObjectMediator.h"
#include "CCoreObjectDictator.h"
#include "CCoreObjectCalculator.h"
#include "CAppServer.h"
#include "IAppServerHandler.h"
#include "ICoreObjectMediatorHandler.h"
#include "ICoreObjectDictatorHandler.h"
#include "ICoreObjectCalculatorHandler.h"
#include "CMultiMsgSender.h"
#include "CPixelPath.h"
#include "BaseHelper.h"
#include "IPipe.h"
#include "CTick.h"
#include "IConnMgrServerHandler.h"
#include "ScriptRpc.h"
#include "CSyncVariantServer.h"
#include "CSyncVariantServerHolder.h"
#include "CScriptSerialHelper.h"
#include "CScriptThreadMgr.h"
#include "CConnMgrServer.h"
#include "CConnServer.h"
#include "IPipeHandler.h"
#include "MultiRpc.h"
#include "CDimensionInfo.h"
#include "CDistortedTick.h"
#include "TSqrAllocator.inl"
#include "CAppConfigServer.h"
#include "TAppConfig_inl.inl"
#include "CScriptAppServer.h"
#include "CMegaLog.h"

#ifdef GetObject
#undef GetObject
#endif

namespace sqr
{
	void WriteMegaLog(const char* szMsg)
	{
		CMegaLog* pLog = CScriptAppServer::Inst()->GetShellLog();
		if (!pLog)
			return;

		pLog->Write(szMsg);
	}
}

int32 CBaseScriptApp::RegisterCoreServer(CScript& Script)
{
	//////////////////////////////////////////////////////
	// Register Classes
	//////////////////////////////////////////////////////

	REGIST_ABSTRACT_B_CLASS( Script, ISend );

	REGIST_ENUMERATION( Script, EServerObjectType, eSOT_Dictator );
	REGIST_ENUMERATION( Script, EServerObjectType, eSOT_Mediator );

	REGIST_ENUMERATION( Script, EServerObjectFlag, eSOF_GridPosChangedNotifycation );

	//Objects
	REGIST_ABSTRACT_B_CLASS( Script, CCoreObjectServer );

	REGIST_ABSTRACT_D_CLASS( Script, CCoreObjectCalculator, CCoreObjectServer );
	REGIST_ABSTRACT_D_CLASS( Script, CCoreObjectDictator, CCoreObjectServer );
	REGIST_ABSTRACT_D_CLASS( Script, CCoreObjectMediator, CCoreObjectDictator );

	REGIST_STATICFUNCTION( Script, CCoreObjectMediator, SetMoveSpeedCheckArg, "If" );

	REGIST_B_CLASS( Script, ICoreObjectServerHandler );
	REGIST_D_CLASS( Script, ICoreObjectCalculatorHandler, ICoreObjectServerHandler );
	REGIST_D_CLASS( Script, ICoreObjectDictatorHandler, ICoreObjectServerHandler );
	REGIST_D_CLASS( Script, ICoreObjectMediatorHandler, ICoreObjectDictatorHandler );


	REGIST_B_CLASS( Script, IAppServerHandler );
	typedef TApp<CTraitsServer>	 CBaseAppServer;
	REGIST_ABSTRACT_B_CLASS( Script, CBaseAppServer );

	REGIST_ABSTRACT_D_CLASS( Script, CAppServer, CBaseAppServer );


	//MetaScene
	typedef TBaseMetaSceneServer<CTraitsServer> CBaseMetaSceneServer;
	REGIST_ABSTRACT_B_CLASS( Script, CBaseMetaSceneServer );
	REGIST_ABSTRACT_D_CLASS( Script, CMetaSceneServer, CBaseMetaSceneServer );

	typedef TMetaSceneMgr<CTraitsServer> CBaseMetaSceneMgrServer;
	REGIST_ABSTRACT_B_CLASS( Script, CBaseMetaSceneMgrServer );
	REGIST_ABSTRACT_D_CLASS( Script, CMetaSceneMgrServer, CBaseMetaSceneMgrServer );

	REGIST_STATICFUNCTION( Script, CBaseMetaSceneMgrServer, Inst, ":(CMetaSceneMgrServer)" );
	

	//CoreScene
	REGIST_ABSTRACT_B_CLASS( Script, CCoreSceneServer );

	REGIST_ABSTRACT_B_CLASS( Script, CCoreSceneMgrServer );
	REGIST_STATICFUNCTION( Script, CCoreSceneMgrServer, Inst, ":(CCoreSceneMgrServer)" );

	//for MultiMsgSender
	REGIST_D_CLASS(Script, CMultiMsgSender, ISend);	

	REGIST_B_CLASS(Script, CLogOwner);

	//CoreConnection
	REGIST_ABSTRACT_D_CLASS_2(Script, CConnServer, CLogOwner, ISend);

	//REGIST_ABSTRACT_D_CLASS_2(Script, CSynConnMgrServer, CBaseConnMgrServer, ISend );
	REGIST_B_CLASS(Script, CConnMgrServer);
	REGIST_B_CLASS(Script, IConnMgrServerHandler);

	//CMetaSceneServer
	REGIST_CLASSFUNCTION( Script, CMetaSceneServer, GetWidthInGrid, ":I");
	REGIST_CLASSFUNCTION( Script, CMetaSceneServer, GetHeightInGrid, ":I");

	//CMetaScene
	REGIST_CLASSFUNCTION_OVERLOAD( Script, CCoreSceneServer, CPixelPath*(CCoreSceneServer::*)(const CFPos&,const CFPos&,EFindPathType,EBarrierType), CreatePath, CreatePath, "(CFPos)(CFPos)(EFindPathType)(EBarrierType):(CPixelPath)" );
	REGIST_CLASSFUNCTION( Script, CBaseMetaSceneServer, GetWidthInGrid, ":I" );
	REGIST_CLASSFUNCTION( Script, CBaseMetaSceneServer, GetHeightInGrid, ":I" );
	REGIST_CLASSFUNCTION( Script, CCoreSceneServer, GetBarrier, "(CPos):(EBarrierType)" );


	//CMetaSceneMgrServer
	REGIST_CLASSFUNCTION( Script, CMetaSceneMgrServer, CreateMetaScene, "s:(CMetaSceneServer)" );
	REGIST_CLASSFUNCTION( Script, CMetaSceneMgrServer, DestroyMetaScene, "(CMetaSceneServer)" );


	//CCoreSceneServer
	REGIST_CLASSFUNCTION( Script, CCoreSceneServer, CreateObjectDictator, "(CFPos)(ICoreObjectDictatorHandler)I:(CCoreObjectDictator)" );
	REGIST_CLASSFUNCTION( Script, CCoreSceneServer, CreateObjectCalculator, "(CFPos)(ICoreObjectCalculatorHandler):(CCoreObjectCalculator)" );
	REGIST_CLASSFUNCTION( Script, CCoreSceneServer, DestroyObject, "(CCoreObjectServer):b" );
	REGIST_CLASSFUNCTION( Script, CCoreSceneServer, GetMetaScene, ":(CMetaSceneServer)" );
	REGIST_CLASSFUNCTION( Script, CCoreSceneServer, Destroying, ":b");
	REGIST_CLASSFUNCTION( Script, CCoreSceneServer, IsPixelValid, "(CFPos):b");
	REGIST_CLASSFUNCTION( Script, CCoreSceneServer, SetTimeDistortedRatio, "f");

	//DimensionInfo
	REGIST_B_CLASS( Script, CDimensionInfo );

	//CCoreSceneMgrServer
	REGIST_CLASSFUNCTION( Script, CCoreSceneMgrServer, DestroyCoreScene, "(CCoreSceneServer):b" );
	REGIST_CLASSFUNCTION( Script, CCoreSceneMgrServer, CreateCoreScene, "(CMetaSceneServer)ss(CDimensionInfo):(CCoreSceneServer)" );

	//TApp
	REGIST_CLASSFUNCTION_OVERLOAD( Script, CBaseAppServer, void (CBaseAppServer::*)(CTick*,uint32),RegisterTick,RegisterTick, "(CTick)I" );
	REGIST_CLASSFUNCTION( Script, CBaseAppServer, UnRegisterTick, "(CTick)" );
	REGIST_CLASSFUNCTION( Script, CBaseAppServer, RegisterDistortedTick, "(CDistortedTick)I" );
	REGIST_CLASSFUNCTION( Script, CBaseAppServer, UnRegisterDistortedTick, "(CDistortedTick)" );
	REGIST_CLASSFUNCTION( Script, CBaseAppServer, GetFrameTime, ":Z" );
	REGIST_CLASSFUNCTION( Script, CBaseAppServer, SetHandler, "(IAppServerHandler)" );
	REGIST_CLASSFUNCTION( Script, CBaseAppServer, GetHandler, ":(IAppServerHandler)" );
	
	REGIST_CLASSFUNCTION_OVERLOAD( Script, CBaseAppServer ,IPipe*(CBaseAppServer::*)(IPipeHandler*)
		,GetIPipe, GetIPipe, "(IPipeHandler):(IPipe)" );
	REGIST_CLASSFUNCTION_OVERLOAD( Script, CBaseAppServer ,IPipe*(CBaseAppServer::*)(IPipeHandler*,size_t,size_t,bool)
		,GetIPipe, GetIPipeWithBufferSize, "(IPipeHandler)ttb:(IPipe)" );
	REGIST_CLASSFUNCTION( Script, CBaseAppServer, GetConnMgr, ":(CConnMgrServer)" );
	REGIST_STATICFUNCTION( Script, CBaseAppServer, Inst, ":(CAppServer)" );


	REGIST_CLASSFUNCTION( Script, CAppServer, Run, ":i" );
		
	REGIST_CLASSFUNCTION( Script, CAppServer, Quit, ":b" );
	REGIST_CLASSFUNCTION( Script, CAppServer, GetMultiMsgSender, ":(CMultiMsgSender)" );
	REGIST_CLASSFUNCTION( Script, CAppServer, Log, "s:" );
	REGIST_CLASSFUNCTION( Script, CAppServer, PrintRegisterTickNum, "I");
	REGIST_CLASSFUNCTION( Script, CAppServer, PrintRegisterTickInfo, "");
	REGIST_CLASSFUNCTION( Script, CAppServer, DoingQuit, ":b");

	//IAppServerHandler
	REGIST_CALLBACKFUNCTION( Script, IAppServerHandler, OnBreak, "" );
	REGIST_CALLBACKFUNCTION( Script, IAppServerHandler, OnClose, "" );
	REGIST_CALLBACKFUNCTION( Script, IAppServerHandler, OnFileChanged, "(CFileWatcherData)");

	REGIST_CLASSFUNCTION( Script, CMultiMsgSender, AddPipe, "(CConnServer)");
	REGIST_CLASSFUNCTION( Script, CMultiMsgSender, DelPipe, "(CConnServer)");
	REGIST_CLASSFUNCTION( Script, CMultiMsgSender, Release, "");
	
	/*******************
	IDbStatement
	*******************/
	/*REGIST_CLASSFUNCTION( Script, IDbStatement, CreateParamBinder, ":(IDbParamBinder)" );
	REGIST_CLASSFUNCTION( Script, IDbStatement, CreateResultBinder, ":(IDbResultBinder)" );*/
	/*REGIST_CLASSFUNCTION( Script, IDbStatement, FetchResultRow, "I" );
	REGIST_CLASSFUNCTION( Script, IDbStatement, GetParamNum, ":I" );
	REGIST_CLASSFUNCTION( Script, IDbStatement, GetResultColNum, ":I" );
	REGIST_CLASSFUNCTION( Script, IDbStatement, GetResultRowNum, ":I" );
	REGIST_CLASSFUNCTION( Script, IDbStatement, GetConnection, ":(IDbConnection)" );
	REGIST_CLASSFUNCTION( Script, IDbStatement, Execute, "" );
	REGIST_CLASSFUNCTION( Script, IDbStatement, Release, "" );
	REGIST_CLASSFUNCTION( Script, IDbStatement, SetTag, "" );
	REGIST_CLASSFUNCTION( Script, IDbStatement, GetTag, "" );*/

	///********************
	//IDbResultBinder
	//********************/
	//REGIST_CLASSFUNCTION( Script, IDbResultBinder, SetResultDouble, "pdpbpbI" );
	//REGIST_CLASSFUNCTION( Script, IDbResultBinder, SetResultText, "mIpIpbpbI" );
	//REGIST_CLASSFUNCTION( Script, IDbResultBinder, GetStatement, ":(IDbStatement)" );
	//REGIST_CLASSFUNCTION( Script, IDbResultBinder, Bind, "" );
	//REGIST_CLASSFUNCTION( Script, IDbResultBinder, Release, "" );


	///********************
	//IDbParamBinder
	//********************/
	//REGIST_CLASSFUNCTION( Script, IDbParamBinder, SetParamDouble, "pdI" );
	//REGIST_CLASSFUNCTION( Script, IDbParamBinder, SetParamText, "sIpII" );
	//REGIST_CLASSFUNCTION( Script, IDbParamBinder, SetParamNull, "pbI" );
	//REGIST_CLASSFUNCTION( Script, IDbParamBinder, GetStatement, ":(IDbStatement)" );
	//REGIST_CLASSFUNCTION( Script, IDbParamBinder, Bind, "" );
	//REGIST_CLASSFUNCTION( Script, IDbParamBinder, Release, "" );


	///********************
	//IDbTextResult
	//********************/
	//REGIST_CLASSFUNCTION( Script, IDbTextResult, Locate, "I" );
	//REGIST_CLASSFUNCTION( Script, IDbTextResult, GetRowNum, ":I" );
	//REGIST_CLASSFUNCTION( Script, IDbTextResult, GetData, "I:s" );
	//REGIST_CLASSFUNCTION( Script, IDbTextResult, Release, "" );

	//CCoreConnMgrServer
	REGIST_CLASSFUNCTION( Script, CConnMgrServer, BeginService, "(CAddress)II" );
	REGIST_CLASSFUNCTION( Script, CConnMgrServer, EndService, "" );
	REGIST_CLASSFUNCTION( Script, CConnMgrServer, GetHandler, ":(IConnMgrServerHandler)" );
	REGIST_CLASSFUNCTION( Script, CConnMgrServer, SetHandler, "(IConnMgrServerHandler)" );

	REGIST_CLASSFUNCTION( Script, CLogOwner, GetValue, "s:s");

	//CConnServer
	REGIST_CLASSFUNCTION_OVERLOAD( Script, CConnServer, void(CConnServer::*)(const char*, const char*), SetConnValue, SetValue, "ss");
	REGIST_CLASSFUNCTION( Script, CConnServer, ShutDown, "s" );
	REGIST_CLASSFUNCTION( Script, CConnServer, GetLatency, ":I")
	REGIST_CLASSFUNCTION( Script, CConnServer, LogOnOffLineMsg, "s");
	REGIST_CLASSFUNCTION( Script, CConnServer, GetRemoteAddress, "(CAddress):b" );

	REGIST_ABSTRACT_B_CLASS( Script, CSyncVariantServer );
	REGIST_ABSTRACT_B_CLASS( Script, CSyncVariantServerHolder );
	REGIST_CLASSFUNCTION( Script, CSyncVariantServerHolder,GetString, "s:s" );
	REGIST_CLASSFUNCTION( Script, CSyncVariantServerHolder,SetString, "ss:(CSyncVariantServer)" );
	REGIST_CLASSFUNCTION( Script, CSyncVariantServerHolder,SetDynRoot, "s:" );
	REGIST_CLASSFUNCTION( Script, CSyncVariantServerHolder,IsEmptyTable, "s:b" );
	REGIST_CLASSFUNCTION_OVERLOAD( Script, CSyncVariantServerHolder, CSyncVariantServer*(CSyncVariantServerHolder::*)(const char*,double), SetNumber<double>, SetNumber, "sd:(CSyncVariantServer)" );
	REGIST_CLASSFUNCTION_OVERLOAD( Script, CSyncVariantServerHolder, double(CSyncVariantServerHolder::*)(const char*)const, GetNumber<double>, GetNumber, "s:d" );
	REGIST_CLASSFUNCTION_OVERLOAD( Script, CSyncVariantServerHolder, CSyncVariantServer*(CSyncVariantServerHolder::*)(const char*,double), SetNumber<double>, SetDouble, "sd:(CSyncVariantServer)" );
	REGIST_CLASSFUNCTION_OVERLOAD( Script, CSyncVariantServerHolder, double(CSyncVariantServerHolder::*)(const char*)const, GetNumber<double>, GetDouble, "s:d" );
	REGIST_CLASSFUNCTION_OVERLOAD( Script, CSyncVariantServerHolder, CSyncVariantServer*(CSyncVariantServerHolder::*)(const char*,int32), SetNumber<int32>, SetInt32, "si:(CSyncVariantServer)" );
	REGIST_CLASSFUNCTION_OVERLOAD( Script, CSyncVariantServerHolder, int32(CSyncVariantServerHolder::*)(const char*)const, GetNumber<int32>, GetInt32, "s:i" );
	REGIST_CLASSFUNCTION_OVERLOAD( Script, CSyncVariantServerHolder, CSyncVariantServer*(CSyncVariantServerHolder::*)(const char*,uint32), SetNumber<uint32>, SetUInt32, "sI:(CSyncVariantServer)" );
	REGIST_CLASSFUNCTION_OVERLOAD( Script, CSyncVariantServerHolder, uint32(CSyncVariantServerHolder::*)(const char*)const, GetNumber<uint32>, GetUInt32, "s:I" );
	REGIST_CLASSFUNCTION_OVERLOAD( Script, CSyncVariantServerHolder, CSyncVariantServer*(CSyncVariantServerHolder::*)(const char*,int16), SetNumber<int16>, SetInt16, "sh:(CSyncVariantServer)" );
	REGIST_CLASSFUNCTION_OVERLOAD( Script, CSyncVariantServerHolder, int16(CSyncVariantServerHolder::*)(const char*)const, GetNumber<int16>, GetInt16, "s:h" );
	REGIST_CLASSFUNCTION_OVERLOAD( Script, CSyncVariantServerHolder, CSyncVariantServer*(CSyncVariantServerHolder::*)(const char*,uint16), SetNumber<uint16>, SetUInt16, "sH:(CSyncVariantServer)" );
	REGIST_CLASSFUNCTION_OVERLOAD( Script, CSyncVariantServerHolder, uint16(CSyncVariantServerHolder::*)(const char*)const, GetNumber<uint16>, GetUInt16, "s:H" );
	REGIST_CLASSFUNCTION_OVERLOAD( Script, CSyncVariantServerHolder, CSyncVariantServer*(CSyncVariantServerHolder::*)(const char*,int8), SetNumber<int8>, SetInt8, "sc:(CSyncVariantServer)" );
	REGIST_CLASSFUNCTION_OVERLOAD( Script, CSyncVariantServerHolder, int8(CSyncVariantServerHolder::*)(const char*)const, GetNumber<int8>, GetInt8, "s:c" );
	REGIST_CLASSFUNCTION_OVERLOAD( Script, CSyncVariantServerHolder, CSyncVariantServer*(CSyncVariantServerHolder::*)(const char*,uint8), SetNumber<uint8>, SetUInt8, "sC:(CSyncVariantServer)" );
	REGIST_CLASSFUNCTION_OVERLOAD( Script, CSyncVariantServerHolder, uint8(CSyncVariantServerHolder::*)(const char*)const, GetNumber<uint8>, GetUInt8, "s:C" );
	REGIST_CLASSFUNCTION_OVERLOAD( Script, CSyncVariantServerHolder, CSyncVariantServer*(CSyncVariantServerHolder::*)(const char*,bool), SetNumber<bool>, SetBool, "sb:(CSyncVariantServer)" );
	REGIST_CLASSFUNCTION_OVERLOAD( Script, CSyncVariantServerHolder, bool(CSyncVariantServerHolder::*)(const char*)const, GetNumber<bool>, GetBool, "s:b" );

	typedef TAppConfig<CSyncTraitsServer> CBaseAppConfigServer;

	REGIST_ABSTRACT_B_CLASS( Script, CBaseAppConfigServer );
	REGIST_ABSTRACT_D_CLASS( Script, CAppConfigServer, CBaseAppConfigServer );

	REGIST_CLASSFUNCTION_OVERLOAD( Script, CBaseAppConfigServer, double(CBaseAppConfigServer::*)(const char*)const, Get<double>,GetNumber, "s:d" );
	REGIST_CLASSFUNCTION_OVERLOAD( Script, CBaseAppConfigServer, double(CBaseAppConfigServer::*)(const char*,const char*)const, Get<double>,GetNumber2, "ss:d" );
	REGIST_CLASSFUNCTION_OVERLOAD( Script, CBaseAppConfigServer, double(CBaseAppConfigServer::*)(const char*,const char*,const char*)const, Get<double>,GetNumber3, "sss:d" );

	REGIST_CLASSFUNCTION_OVERLOAD( Script, CBaseAppConfigServer, const char*(CBaseAppConfigServer::*)(const char*)const, Get<const char*>,GetString, "s:s" );
	REGIST_CLASSFUNCTION_OVERLOAD( Script, CBaseAppConfigServer, const char*(CBaseAppConfigServer::*)(const char*,const char*)const, Get<const char*>,GetString2, "ss:s" );
	REGIST_CLASSFUNCTION_OVERLOAD( Script, CBaseAppConfigServer, const char*(CBaseAppConfigServer::*)(const char*,const char*,const char*)const, Get<const char*>,GetString3, "sss:s" );

	REGIST_STATICFUNCTION( Script, CBaseAppConfigServer,Inst, ":(CAppConfigServer)" );

	//Rpc Server
	CREATE_RPC_CHANNEL(Script,ISend,CConnServer);
	
	REGISTER_SERIAL( Script, "CallDbTransaction" );	

	//多服rpc
	REGIST_MULTI_RPC(Script, IPipe, IPipe);

	REGIST_GLOBALFUNCTION(Script, WriteMegaLog, "s");

	return 1;
}
