#include "stdafx.h"
#include "ScriptX.h"
#include "IAppClientHandler.h"
#include "CBaseScriptApp.h"
#include "CAppClient.h"
#include "ICoreObjectPerformerHandler.h"
#include "ICoreObjectFollowerHandler.h"
#include "ICoreObjectDirectorHandler.h"
#include "CCoreObjectPerformer.h"
#include "CCoreObjectFollower.h"
#include "CCoreObjectDirector.h"
#include "CMetaSceneClient.h"
#include "CMetaSceneMgrClient.h"
#include "CCoreSceneClient.h"
#include "CCoreSceneMgrClient.h"
#include "CRenderSystemClient.h"
#include "CIme.h"
#include "CMainWnd.h"
#include "IConnMgrClientHandler.h"
#include "CConnMgrClient.h"
#include "CConnClient.h"
#include "CSyncVariantClient.h"
#include "CSyncVariantClientHolder.h"
#include "ScriptRpc.h"
#include "CPixelPath.h"
#include "ICoreSceneClientHandler.h"
#include "CLanguageSetting.h"
#include "CDistortedTick.h"
#include "TSqrAllocator.inl"
#include "CAppConfigClient.h"
#include "TAppConfig_inl.inl"

namespace sqr
{

	CIme* GetIme()
	{
		return CIme::GetIme();
	}
}

int32 CBaseScriptApp::RegisterCoreClient(CScript& Script)
{
	REGIST_ENUMTYPE( Script, EClientObjectType );
	REGIST_ENUMERATION( Script, EClientObjectType, eCOT_Performer );
	REGIST_ENUMERATION( Script, EClientObjectType, eCOT_Follower );
	REGIST_ENUMERATION( Script, EClientObjectType, eCOT_Director );

	REGIST_ABSTRACT_D_CLASS( Script, CRenderSystemClient, CRenderSystem );
	
	//×¢²áCBaseCoreObjectClient
	typedef TCoreObject<CTraitsClient> CBaseCoreObjectClient;
	REGIST_ABSTRACT_B_CLASS( Script, CBaseCoreObjectClient );

	//×¢²áCCoreObjectClient
	REGIST_ABSTRACT_D_CLASS( Script, CCoreObjectClient, CBaseCoreObjectClient );

	//REGIST_UNSAFE_ABSTRACT_B_CLASS( Script, ICoreObjectClientHandler );
	REGIST_B_CLASS( Script, ICoreObjectClientHandler );
	REGIST_D_CLASS( Script, ICoreObjectPerformerHandler,ICoreObjectClientHandler );
	REGIST_D_CLASS( Script, ICoreObjectFollowerHandler,ICoreObjectClientHandler );
	REGIST_D_CLASS( Script, ICoreObjectDirectorHandler,ICoreObjectFollowerHandler );

	REGIST_ABSTRACT_D_CLASS( Script, CCoreObjectPerformer, CCoreObjectClient );
	REGIST_ABSTRACT_D_CLASS( Script, CCoreObjectFollower, CCoreObjectClient );
	REGIST_ABSTRACT_D_CLASS( Script, CCoreObjectDirector, CCoreObjectFollower );

	//×¢²áCMetaSceneClient
	typedef TMetaScene<CTraitsClient> CBaseMetaSceneClient;
	REGIST_ABSTRACT_B_CLASS( Script, CBaseMetaSceneClient );
	REGIST_ABSTRACT_D_CLASS( Script, CMetaSceneClient, CBaseMetaSceneClient );
	typedef TMetaSceneMgr<CTraitsClient> CBaseMetaSceneMgrClient;
	REGIST_ABSTRACT_B_CLASS( Script, CBaseMetaSceneMgrClient );
	REGIST_ABSTRACT_D_CLASS( Script, CMetaSceneMgrClient , CBaseMetaSceneMgrClient );

	REGIST_STATICFUNCTION( Script, CBaseMetaSceneMgrClient, Inst, ":(CMetaSceneMgrClient)" );	
	
	//×¢²áCCoreSceneClient
	typedef TCoreScene<CTraitsClient> CBaseCoreSceneClient;
	REGIST_ABSTRACT_B_CLASS( Script, CBaseCoreSceneClient );
	REGIST_ABSTRACT_D_CLASS( Script, CCoreSceneClient, CBaseCoreSceneClient );
	typedef TCoreSceneMgr<CTraitsClient> CBaseCoreSceneMgrClient;
	REGIST_ABSTRACT_B_CLASS( Script, CBaseCoreSceneMgrClient );
	REGIST_ABSTRACT_D_CLASS( Script, CCoreSceneMgrClient, CBaseCoreSceneMgrClient );

	// ×¢²áConnClient
	REGIST_ABSTRACT_B_CLASS(Script, CLogOwner);
	typedef TConnection<CTraitsClient> CBaseConnectionClient;
	REGIST_ABSTRACT_D_CLASS(Script, CBaseConnectionClient, CLogOwner);
	REGIST_ABSTRACT_D_CLASS(Script, CConnClient, CBaseConnectionClient );


	typedef TConnectionMgr<CTraitsClient> CBaseConnMgrClient;
	REGIST_ABSTRACT_B_CLASS( Script, CBaseConnMgrClient );
	REGIST_ABSTRACT_D_CLASS( Script, CConnMgrClient, CBaseConnMgrClient );
	REGIST_B_CLASS( Script, IConnMgrClientHandler );

	// ×¢²áCIme
	REGIST_ABSTRACT_B_CLASS( Script, CIme );

	//×¢²áCMainWnd
	REGIST_ABSTRACT_B_CLASS( Script, CMainWnd);

	// ×¢²áCAppClient
	REGIST_B_CLASS( Script, IAppClientHandler );
	typedef TApp<CTraitsClient> CBaseAppClient;
	REGIST_ABSTRACT_B_CLASS( Script, CBaseAppClient );
	typedef TCoreApp<CTraitsClient> CBaseCoreAppClient;
	REGIST_ABSTRACT_D_CLASS( Script, CBaseCoreAppClient, CBaseAppClient );
	REGIST_ABSTRACT_D_CLASS( Script, CAppClient, CBaseCoreAppClient );
	

	REGIST_STATICFUNCTION( Script,CRenderSystemClient,  Inst, ":(CRenderSystemClient)" );
	REGIST_CLASSFUNCTION( Script, CRenderSystemClient , SetFreeCameraEnabled, "b" );
	REGIST_CLASSFUNCTION( Script, CRenderSystemClient , SetFree3DCameraEnabled, "b" );
	REGIST_CLASSFUNCTION( Script, CRenderSystemClient , IsFreeCameraEnabled, ":b" );
	REGIST_CLASSFUNCTION( Script, CRenderSystemClient , InitCameraInfo, "" );
	REGIST_CLASSFUNCTION( Script, CRenderSystemClient , SetCameraNoChange, "b" );

	REGIST_CLASSFUNCTION( Script, CRenderSystemClient , GetCamMaxXRadian, ":f" );
	REGIST_CLASSFUNCTION( Script, CRenderSystemClient , GetCamMinXRadian, ":f" );
	REGIST_CLASSFUNCTION( Script, CRenderSystemClient , GetCamYRadian, ":f" );
	REGIST_CLASSFUNCTION( Script, CRenderSystemClient , GetCamMaxFovRadian, ":f" );
	REGIST_CLASSFUNCTION( Script, CRenderSystemClient , GetCamMinFovRadian, ":f" );
	REGIST_CLASSFUNCTION( Script, CRenderSystemClient , GetCamMaxNearPlan, ":f" );
	REGIST_CLASSFUNCTION( Script, CRenderSystemClient , GetCamMinNearPlan, ":f" );
	REGIST_CLASSFUNCTION( Script, CRenderSystemClient , GetCamMinYOffset, ":f" );
	REGIST_CLASSFUNCTION( Script, CRenderSystemClient , GetCamMaxYOffset, ":f" );
	REGIST_CLASSFUNCTION( Script, CRenderSystemClient , GetCamMinDist, ":f" );
	REGIST_CLASSFUNCTION( Script, CRenderSystemClient , GetCamMaxDist, ":f" );
	REGIST_CLASSFUNCTION( Script, CRenderSystemClient , GetCamMaxHOffset, ":f" );
	REGIST_CLASSFUNCTION( Script, CRenderSystemClient , GetCamSpeed, ":f" );
	REGIST_CLASSFUNCTION( Script, CRenderSystemClient , GetCamDefDist, ":f" );

	
	//×¢²ámetasceneÏà¹Øº¯Êý
	REGIST_CLASSFUNCTION( Script, CBaseMetaSceneClient, GetSceneName, ":s" );
	REGIST_CLASSFUNCTION( Script, CBaseMetaSceneClient, GetWidthInGrid, ":I" );
	REGIST_CLASSFUNCTION( Script, CBaseMetaSceneClient, GetHeightInGrid, ":I" );
	REGIST_CLASSFUNCTION_OVERLOAD( Script, CBaseCoreSceneClient, CPixelPath*(CBaseCoreSceneClient::*)(const CFPos&,const CFPos&,EFindPathType,EBarrierType), CreatePath, CreatePath, "(CFPos)(CFPos)(EFindPathType)(EBarrierType):(CPixelPath)" );
	REGIST_CLASSFUNCTION( Script, CBaseCoreSceneClient, GetBarrier, "(CPos):(EBarrierType)" );


	

	// ×¢²áObjectÏà¹ØµÄº¯Êý
	REGIST_CLASSFUNCTION( Script, CBaseCoreObjectClient, SetHandler, "(ICoreObjectClientHandler)" );
	REGIST_CLASSFUNCTION( Script, CBaseCoreObjectClient, GetHandler, ":(ICoreObjectClientHandler)" );
	REGIST_CLASSFUNCTION( Script, CBaseCoreObjectClient, GetGridPos, "(CPos)" );
	REGIST_CLASSFUNCTION_OVERLOAD( Script, CBaseCoreObjectClient, void(CBaseCoreObjectClient::*)(CFPos&), GetPixelPos, GetPixelPos, "(CFPos)");
	REGIST_CLASSFUNCTION( Script, CBaseCoreObjectClient, GetLocalID, ":I" );

	REGIST_CLASSFUNCTION( Script, CCoreObjectClient, GetType, ":(EClientObjectType)" );
	REGIST_CLASSFUNCTION( Script, CCoreObjectClient, IsMoving, ":b" );

	REGIST_CLASSFUNCTION( Script, CCoreObjectClient, GetCurGridWayPoint, "(CPos):b" );
	REGIST_CLASSFUNCTION( Script, CCoreObjectClient, GetCurPixelWayPoint, "(CFPos):b" );

	REGIST_STATICFUNCTION( Script, CCoreObjectFollower, GetObjectFollower, "I:(CCoreObjectFollower)" );
	REGIST_CLASSFUNCTION( Script, CCoreObjectFollower, GetHandler, ":(ICoreObjectFollowerHandler)" );
	REGIST_CLASSFUNCTION( Script, CCoreObjectFollower, GetType, ":(EClientObjectType)" );
	REGIST_CLASSFUNCTION( Script, CCoreObjectFollower, IsPassive, ":b" );

	REGIST_CLASSFUNCTION( Script, CCoreObjectDirector, GetHandler, ":(ICoreObjectDirectorHandler)" );
	REGIST_CLASSFUNCTION( Script, CCoreObjectDirector, GetType, ":(EClientObjectType)" );
	REGIST_CLASSFUNCTION( Script, CCoreObjectDirector, IsPassive, ":b" );
	REGIST_CLASSFUNCTION( Script, CCoreObjectDirector, SetSpeed, "f:I" );
	REGIST_CLASSFUNCTION( Script, CCoreObjectDirector, StopMoving, ":I" );
	REGIST_CLASSFUNCTION( Script, CCoreObjectDirector, MoveTo, "(CFPos)fIIf:I" );
	REGIST_CLASSFUNCTION( Script, CCoreObjectDirector, SetPixelPos, "(CFPos):I" );
	REGIST_CLASSFUNCTION( Script, CCoreObjectDirector, SetGridPos, "(CPos):I" );
	REGIST_CLASSFUNCTION( Script, CCoreObjectDirector, ControlCamera, "b:b" );

	REGIST_CLASSFUNCTION( Script, CCoreObjectPerformer, GetHandler, ":(ICoreObjectPerformerHandler)" );
	REGIST_CLASSFUNCTION( Script, CCoreObjectPerformer, GetType, ":(EClientObjectType)" );
	REGIST_CLASSFUNCTION( Script, CCoreObjectPerformer, SetSpeed, "f:I" );
	REGIST_CLASSFUNCTION( Script, CCoreObjectPerformer, StopMoving, ":I" );
	REGIST_CLASSFUNCTION( Script, CCoreObjectPerformer, MoveTo, "(CFPos)fIIf:I" );
	REGIST_CLASSFUNCTION( Script, CCoreObjectPerformer, SetPixelPos, "(CFPos):I" );
	REGIST_CLASSFUNCTION( Script, CCoreObjectPerformer, SetGridPos, "(CPos):I" );
	REGIST_CLASSFUNCTION( Script, CCoreObjectPerformer, GetRenderObject, ":(CRenderObject)" );
	REGIST_CLASSFUNCTION( Script, CCoreObjectPerformer, SetHeight, "f" );
	REGIST_CLASSFUNCTION( Script, CCoreObjectPerformer, GetHeight, ":f" );
	REGIST_CLASSFUNCTION( Script, CCoreObjectPerformer, SetCanBeSelected, "b" );
	REGIST_CLASSFUNCTION( Script, CCoreObjectPerformer, GetCanBeSelected, ":b" );
	REGIST_CLASSFUNCTION( Script, CCoreObjectPerformer, SetLieOnTheTerrain, "b" );

	REGIST_CLASSFUNCTION( Script, ICoreObjectClientHandler, CastToObjFolHandler, ":(ICoreObjectFollowerHandler)" );
	REGIST_CLASSFUNCTION( Script, ICoreObjectClientHandler, CastToObjPerHandler, ":(ICoreObjectPerformerHandler)" );

	REGIST_CLASSFUNCTION( Script, ICoreObjectPerformerHandler, CastToObjPerHandler, ":(ICoreObjectPerformerHandler)" );

	REGIST_CLASSFUNCTION( Script, ICoreObjectFollowerHandler, OnPositionSet, "" );
	REGIST_CLASSFUNCTION( Script, ICoreObjectFollowerHandler, CastToObjFolHandler, ":(ICoreObjectFollowerHandler)" );
	REGIST_CLASSFUNCTION( Script, ICoreObjectFollowerHandler, CastToObjDirHandler, ":(ICoreObjectDirectorHandler)" );

	REGIST_CLASSFUNCTION( Script, ICoreObjectDirectorHandler, CastToObjDirHandler, ":(ICoreObjectDirectorHandler)" );

	REGIST_CLASSFUNCTION( Script, CBaseCoreSceneClient, GetMetaScene, ":(CMetaSceneClient)" );
	
	REGIST_B_CLASS( Script, ICoreSceneClientHandler );

	REGIST_CLASSFUNCTION( Script, CCoreSceneClient, CreateObjectPerformer, "(CFPos)(ICoreObjectPerformerHandler):(CCoreObjectPerformer)" );
	REGIST_CLASSFUNCTION( Script, CCoreSceneClient, DestroyObjectPerformer, "(CCoreObjectPerformer)" );
	REGIST_CLASSFUNCTION( Script, CCoreSceneClient, GetRenderScene, ":(CRenderScene)" );
	REGIST_STATICFUNCTION( Script, CCoreSceneClient, Inst, ":(CCoreSceneClient)" );
	REGIST_CLASSFUNCTION( Script, CCoreSceneClient, SetHandler, "(ICoreSceneClientHandler)" );
	
	REGIST_STATICFUNCTION( Script, CBaseCoreSceneMgrClient, Inst, ":(CCoreSceneMgrClient)" );
	REGIST_CLASSFUNCTION_OVERLOAD( Script, CCoreSceneMgrClient, bool(CCoreSceneMgrClient::*)(const char*),CreateMainScene,CreateMainScene, "s:b");
	REGIST_CLASSFUNCTION( Script, CCoreSceneMgrClient, RefreshCameraDest, "fff");
	REGIST_CLASSFUNCTION( Script, CCoreSceneMgrClient, DestroyMainScene, ":b");
	
	
	REGIST_CLASSFUNCTION( Script, CMainWnd, Show, "b" );

	//IAppClientHandler
	REGIST_CALLBACKFUNCTION( Script, IAppClientHandler, OnCloseMainWnd, "" );
	REGIST_CALLBACKFUNCTION( Script, IAppClientHandler, OnLoading, "f" );
	REGIST_CALLBACKFUNCTION( Script, IAppClientHandler, OnMainSceneCreated, "(CCoreSceneClient)" );
	REGIST_CALLBACKFUNCTION( Script, IAppClientHandler, OnDestroyMainScene, "" );
	REGIST_CALLBACKFUNCTION( Script, IAppClientHandler, OnMainSceneDestroyed, "" );
	REGIST_CALLBACKFUNCTION( Script, IAppClientHandler, OnBreak, "" );
	REGIST_CALLBACKFUNCTION( Script, IAppClientHandler, OnFileChanged, "(CFileWatcherData)" );
	

	REGIST_CLASSFUNCTION( Script, CBaseAppClient, Run, ":i" );
	REGIST_CLASSFUNCTION( Script, CBaseAppClient, GetConnMgr, ":(CConnMgrClient)" );

	REGIST_CLASSFUNCTION_OVERLOAD( Script, CBaseAppClient, void (CBaseAppClient::*)(CTick*,uint32),RegisterTick,RegisterTick, "(CTick)I" );
	REGIST_CLASSFUNCTION( Script, CBaseAppClient, UnRegisterTick, "(CTick)" );
	REGIST_CLASSFUNCTION( Script, CBaseAppClient, RegisterDistortedTick, "(CDistortedTick)I" );
	REGIST_CLASSFUNCTION( Script, CBaseAppClient, UnRegisterDistortedTick, "(CDistortedTick)" );
	REGIST_CLASSFUNCTION( Script, CBaseAppClient, GetFrameTime, ":Z" );
	REGIST_CLASSFUNCTION( Script, CBaseAppClient, SetHandler, "(IAppClientHandler)" );
	REGIST_CLASSFUNCTION( Script, CBaseAppClient, GetHandler, ":(IAppClientHandler)" );
	REGIST_STATICFUNCTION( Script, CBaseAppClient, Inst, ":(CAppClient)" );
	
	REGIST_CLASSFUNCTION_OVERLOAD( Script, CBaseAppClient ,IPipe*(CBaseAppClient::*)(IPipeHandler*)
		,GetIPipe, GetIPipe, "(IPipeHandler):(IPipe)" );

	REGIST_CLASSFUNCTION_OVERLOAD( Script, CBaseAppClient ,IPipe*(CBaseAppClient::*)(IPipeHandler*,size_t,size_t,bool)
		,GetIPipe, GetIPipeWithBufferSize, "(IPipeHandler)IIb:(IPipe)" );

	REGIST_CLASSFUNCTION( Script, CAppClient, GetRootWnd, ":(SQRRootWnd)" );
	REGIST_CLASSFUNCTION( Script, CAppClient, GetMainWnd, ":(CMainWnd)" );
	REGIST_CLASSFUNCTION( Script, CAppClient, GetTGInst, ":(TGManager)" );
	REGIST_CLASSFUNCTION( Script, CAppClient, DebugConsoleOn, ":b");
	REGIST_CLASSFUNCTION( Script, CAppClient, Quit, "" );
	REGIST_CLASSFUNCTION( Script, CAppClient, SetMainWndTitle, "s" );
	
	REGIST_CLASSFUNCTION( Script, CIme, EnableIme, "b" );
	REGIST_CLASSFUNCTION( Script, CIme, GetGuiImeImage, ":(WND_IMAGE_LIST)" );
	//REGIST_CALLBACKFUNCTION( Script, CIme, OnImeChanged, "" );
	REGIST_GLOBALFUNCTION( Script, GetIme, ":(CIme)" );

	REGIST_CLASSFUNCTION_OVERLOAD( Script, CLogOwner, void (CLogOwner::*)(const char*, const char*), SetValue, SetValue, "ss");
	REGIST_CLASSFUNCTION( Script, CLogOwner, GetValue, "s:s");
	REGIST_CLASSFUNCTION( Script, CBaseConnectionClient, IsConnected, ":b" );
	REGIST_CLASSFUNCTION( Script, CBaseConnectionClient, IsShuttingDown, ":b" );
	REGIST_CLASSFUNCTION( Script, CConnClient, Send, "mI:b" );	
	REGIST_CLASSFUNCTION( Script, CConnClient, GetLatency, ":I");
	REGIST_CLASSFUNCTION_OVERLOAD( Script, CConnClient, void(CConnClient::*)(), ShutDown, ShutDown, "" );
	REGIST_CLASSFUNCTION_OVERLOAD( Script, CConnClient, void(CConnClient::*)(const char*), ShutDown, ShutDownByMsg, "s" );
	REGIST_CLASSFUNCTION( Script, CConnClient, StartRecvTrafficStat, "" );
	REGIST_CLASSFUNCTION( Script, CConnClient, CloseRecvTrafficStat, "" );
	REGIST_CLASSFUNCTION( Script, CConnClient, GetLocalPhysicalAddress, ":s" );
	REGIST_CLASSFUNCTION_OVERLOAD( Script, CConnClient, void(CConnClient::*)(const char*, const char*), SetConnValue, SetValue, "ss");
	
	REGIST_CLASSFUNCTION( Script, CBaseConnMgrClient, SetHandler, "(IConnMgrClientHandler)" );
	REGIST_CLASSFUNCTION( Script, CBaseConnMgrClient, GetHandler, ":(IConnMgrClientHandler)" );
	REGIST_CLASSFUNCTION( Script, CConnMgrClient, Connect, "(CAddress):(CConnClient)" );

	// ×¢²áÓïÑÔÉèÖÃµ¥¼þÀà
	REGIST_ABSTRACT_B_CLASS( Script, CLanguageSetting );
	REGIST_STATICFUNCTION( Script, CLanguageSetting, Inst, ":(CLanguageSetting)" );
	REGIST_CLASSFUNCTION( Script, CLanguageSetting, ChangeLanguage, "I" );
	REGIST_CLASSFUNCTION( Script, CLanguageSetting, GetLanguageCount, ":i" );
	REGIST_CLASSFUNCTION( Script, CLanguageSetting, GetCurLanIndex, ":i" );
	REGIST_CLASSFUNCTION( Script, CLanguageSetting, GetLanguageName, "i:s");

	REGIST_ABSTRACT_B_CLASS( Script, CSyncVariantClient );

	REGIST_ABSTRACT_B_CLASS( Script, CSyncVariantClientHolder );
	REGIST_CLASSFUNCTION( Script, CSyncVariantClientHolder,GetString, "s:s" );
	REGIST_CLASSFUNCTION( Script, CSyncVariantClientHolder,IsEmptyTable, "s:b" );
	REGIST_CLASSFUNCTION_OVERLOAD( Script, CSyncVariantClientHolder, double(CSyncVariantClientHolder::*)(const char*)const, GetNumber<double>, GetNumber, "s:d" );
	REGIST_CLASSFUNCTION_OVERLOAD( Script, CSyncVariantClientHolder, double(CSyncVariantClientHolder::*)(const char*)const, GetNumber<double>, GetDouble, "s:d" );
	REGIST_CLASSFUNCTION_OVERLOAD( Script, CSyncVariantClientHolder, int32(CSyncVariantClientHolder::*)(const char*)const, GetNumber<int32>, GetInt32, "s:i" );
	REGIST_CLASSFUNCTION_OVERLOAD( Script, CSyncVariantClientHolder, uint32(CSyncVariantClientHolder::*)(const char*)const, GetNumber<uint32>, GetUInt32, "s:I" );
	REGIST_CLASSFUNCTION_OVERLOAD( Script, CSyncVariantClientHolder, int16(CSyncVariantClientHolder::*)(const char*)const, GetNumber<int16>, GetInt16, "s:h" );
	REGIST_CLASSFUNCTION_OVERLOAD( Script, CSyncVariantClientHolder, uint16(CSyncVariantClientHolder::*)(const char*)const, GetNumber<uint16>, GetUInt16, "s:H" );
	REGIST_CLASSFUNCTION_OVERLOAD( Script, CSyncVariantClientHolder, int8(CSyncVariantClientHolder::*)(const char*)const, GetNumber<int8>, GetInt8, "s:c" );
	REGIST_CLASSFUNCTION_OVERLOAD( Script, CSyncVariantClientHolder, uint8(CSyncVariantClientHolder::*)(const char*)const, GetNumber<uint8>, GetUInt8, "s:C" );
	REGIST_CLASSFUNCTION_OVERLOAD( Script, CSyncVariantClientHolder, bool(CSyncVariantClientHolder::*)(const char*)const, GetNumber<bool>, GetBool, "s:b" );


	typedef TAppConfig<CTraitsClient> CBaseAppConfigClient;

	REGIST_ABSTRACT_B_CLASS( Script, CBaseAppConfigClient );
	REGIST_ABSTRACT_D_CLASS( Script, CAppConfigClient, CBaseAppConfigClient );
	
	REGIST_CLASSFUNCTION_OVERLOAD( Script, CBaseAppConfigClient, double(CBaseAppConfigClient::*)(const char*)const, Get<double>,GetNumber, "s:d" );
	REGIST_CLASSFUNCTION_OVERLOAD( Script, CBaseAppConfigClient, double(CBaseAppConfigClient::*)(const char*,const char*)const, Get<double>,GetNumber2, "ss:d" );
	REGIST_CLASSFUNCTION_OVERLOAD( Script, CBaseAppConfigClient, double(CBaseAppConfigClient::*)(const char*,const char*,const char*)const, Get<double>,GetNumber3, "sss:d" );

	REGIST_CLASSFUNCTION_OVERLOAD( Script, CBaseAppConfigClient, const char*(CBaseAppConfigClient::*)(const char*)const, Get<const char*>,GetString, "s:s" );
	REGIST_CLASSFUNCTION_OVERLOAD( Script, CBaseAppConfigClient, const char*(CBaseAppConfigClient::*)(const char*,const char*)const, Get<const char*>,GetString2, "ss:s" );
	REGIST_CLASSFUNCTION_OVERLOAD( Script, CBaseAppConfigClient, const char*(CBaseAppConfigClient::*)(const char*,const char*,const char*)const, Get<const char*>,GetString3, "sss:s" );

	REGIST_STATICFUNCTION( Script, CBaseAppConfigClient,Inst, ":(CAppConfigClient)" );

	REGIST_STATICFUNCTION( Script,CAppConfigClient , Inst, ":(CAppConfigClient)" );
	REGIST_CLASSFUNCTION( Script, CAppConfigClient , GetDefaultRenderCyc, ":I" );
	REGIST_CLASSFUNCTION( Script, CAppConfigClient , GetDefaultLanguage, ":I" );
	REGIST_CLASSFUNCTION( Script, CAppConfigClient , GetDefaultWndWidth, ":I" );
	REGIST_CLASSFUNCTION( Script, CAppConfigClient , GetDefaultWndHeight, ":I" );
	REGIST_CLASSFUNCTION( Script, CAppConfigClient , IsTestClient, ":b" );


	//ISend
	CREATE_RPC_CHANNEL(Script,CConnClient,CConnClient);
	return 1;
}
