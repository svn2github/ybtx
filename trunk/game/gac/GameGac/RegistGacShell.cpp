#include "stdafx.h"
#include "RegistGacShell.h"
#include "RegistShellCommon.h"
#include "ScriptX.h"
#include "CScriptAppClient.h"
#include "CSmallMap.h"
#include "CCommpressFile.h"
#include "CFighterDirector.h"
#include "CCoreObjectFollower.h"
#include "RunExe.h"
#include "CCoolDownInfo.h"
#include "CCoolDownMgrClient.h"
#include "CSkillClient.h"
#include "LoadSkillGac.h"
#include "CFighterFollower.h"
#include "CFighterViewInfo.h"
#include "CFighterBaseInfo.h"
#include "TFighterCtrlInfo.h"
#include "TPropertyHolder.inl"
#include "CFighterAgileInfoClient.h"
#include "CMagicStateClient.h"
#include "CConnMgrClientHandler.h"
#include "CConnClient.h"
#include "CBaseConnMgrHandler.inl"
#include "CppGameClient.h"
#include "CCharacterFollower.h"
#include "CCharacterDirector.h"
#include "ICharacterDirectorCallbackHandler.h"
#include "IIntObjCallbackHandler.h"
#include "CIntObjClient.h"
#include "CCoreSceneClient.h"
#include "CEntityClient.h"
#include "FXPlayer.h"
#include "CEntityClientManager.h"
#include "CAutoTracker.h"
#include "CCharacterVariantHandler.h"
#include "IAudioModule.h"
#include "CBigMap.h"
#include "ITeamClientManagerHandler.h"
#include "CTeamClientManager.h"
#include "CBloodRenderDialogPool.h"
#include "CDisplayHurt.h"
#include "CRenderObject.h"
#include "CCypherVariant.h"
#include "CSyncVariantClientHolder.h"
#include "CPixelPath.h"
#include "CApexClient.h"
#include "CGameSceneHandler.h"
#include "CBurstSoulCfgClient.h"
#include "CFightCallbackData.h"
#include "ServantType.h"
#include "CDistortedTick.h"
#include "CBloodTypeCfg.h"
#include "CDirectionInfo.h"
#include "CSoundCDTimeMgr.h"
#include "CModelInfoClient.h"

class CMagicStateClient;
class CTriggerStateClient;
class CDamageChangeStateClient;
class CCumuliTriggerStateClient;
class CSpecialStateClient;

void RegistGacShell(CScript& Script)
{
	//注册ShellCommon
	RegistShellCommon(Script);

	// 调用exe
	REGIST_GLOBALFUNCTION( Script, RunExe,"sss:b");
	REGIST_GLOBALFUNCTION( Script, RunUrl,"s");

	REGIST_GLOBALFUNCTION( Script, CppGameClientInit,":b");
	REGIST_GLOBALFUNCTION( Script, GetRandomOneStepLoginIndex, ":i" );

	//====================== CApexClient =========================
	REGIST_GLOBALFUNCTION( Script, StartApexClient,":i");
	REGIST_GLOBALFUNCTION( Script, StopApexClient,":i");

	REGIST_STATICFUNCTION( Script, CDisplayHurt, AddHurtInfo, "Is");
	REGIST_STATICFUNCTION( Script, CPositionFX, CppPlayPosFX, "ss(CFPos)");
	REGIST_STATICFUNCTION( Script, CTargetDirectionFX, CppPlayTargetDirFX, "ss(CFPos)");
	REGIST_STATICFUNCTION( Script, CTargetDirectionFX, CppCancelTargetDirFX, "");
	REGIST_STATICFUNCTION( Script, CFontSprite, DisplayInfo, "(CRenderObject)ssssibf");

	typedef TPropertyHolder<CFighterViewInfo>	CPropertyHolderViewInfo;
	REGIST_B_CLASS( Script, CPropertyHolderViewInfo );
	REGIST_D_CLASS( Script, CFighterViewInfo, CPropertyHolderViewInfo );
	REGIST_CLASSFUNCTION( Script, CFighterViewInfo, CppInit, "I" );
	REGIST_STATICFUNCTION( Script, CPropertyHolderViewInfo, GetPropertyName, "I:s");
	REGIST_STATICFUNCTION( Script, CPropertyHolderViewInfo, GetPropertyIndex, "s:I");

	REGIST_CLASSFUNCTION_OVERLOAD( Script, CPropertyHolderViewInfo, float(CPropertyHolderViewInfo::*)(const TCHAR*)const, CppGetPropertyValue, CppGetPropertyValueByName, "s:f" );

	REGIST_ABSTRACT_B_CLASS( Script, CSkillClient);
	REGIST_CLASSFUNCTION( Script, CSkillClient, GetName, ":s" );
	REGIST_CLASSFUNCTION( Script, CSkillClient, GetCoolDownType, ":(ESkillCoolDownType)" );
	REGIST_STATICFUNCTION( Script, CSkillClient, GetSkillByNameForLua, "s:(CSkillClient)" );
	REGIST_CLASSFUNCTION( Script, CSkillClient, GetAttackType, ":(EAttackType)" );

	REGIST_ABSTRACT_B_CLASS( Script, CBaseStateCfg );
	REGIST_STATICFUNCTION( Script, CBaseStateCfg, GetByGlobalId , "I:(CBaseStateCfg)" );
	REGIST_CLASSFUNCTION( Script, CBaseStateCfg , GetModel, ":s" );

	REGIST_ABSTRACT_B_CLASS( Script, CBurstSoulCfgClient );
	REGIST_STATICFUNCTION( Script, CBurstSoulCfgClient, GetNeedBurstSoulTimesByLevel , "I:I" );

	REGIST_ENUMTYPE( Script, EConnConnectFailedReason );
	REGIST_ENUMERATION( Script, EConnConnectFailedReason, eCCFR_UNREACHABLE );
	REGIST_ENUMERATION( Script, EConnConnectFailedReason, eCCFR_REFUSED );
	REGIST_ENUMERATION( Script, EConnConnectFailedReason, eCCFR_INVALIDADDR );
	REGIST_ENUMERATION( Script, EConnConnectFailedReason, eCCFR_OSERROR );

	REGIST_B_CLASS( Script, IScriptConnMgrClientHandler);
	REGIST_CALLBACKFUNCTION( Script, IScriptConnMgrClientHandler, OnDataReceived, "(CConnClient)");
	REGIST_CALLBACKFUNCTION( Script, IScriptConnMgrClientHandler, OnConnected, "(CConnClient)" );
	REGIST_CALLBACKFUNCTION( Script, IScriptConnMgrClientHandler, OnConnectFailed, "(CConnClient)(EConnConnectFailedReason)" );
	REGIST_CALLBACKFUNCTION( Script, IScriptConnMgrClientHandler, OnDisconnect, "(CConnClient)" );
	REGIST_CALLBACKFUNCTION( Script, IScriptConnMgrClientHandler, OnError, "(CConnClient)s");

	typedef CBaseConnMgrHandler<CConnClient, IScriptConnMgrClientHandler> CBaseConnClientMgrHandler;
	REGIST_B_CLASS( Script, CBaseConnClientMgrHandler );
	REGIST_CLASSFUNCTION( Script, CBaseConnClientMgrHandler, SetScriptHandler, "(IScriptConnMgrClientHandler)" );
	REGIST_CLASSFUNCTION( Script, CBaseConnClientMgrHandler, GetScriptHandler, ":(IScriptConnMgrClientHandler)" );


	REGIST_D_CLASS_2( Script, CConnMgrClientHandler, IConnMgrClientHandler, CBaseConnClientMgrHandler);
	
	REGIST_CLASSFUNCTION( Script, CConnMgrClientHandler, OnConnected, "(CConnClient)" );
	REGIST_CLASSFUNCTION( Script, CConnMgrClientHandler, OnConnectFailed, "(CConnClient)(EConnConnectFailedReason)" );
	REGIST_CLASSFUNCTION( Script, CConnMgrClientHandler, OnDisconnect, "(CConnClient)" );
	REGIST_CLASSFUNCTION( Script, CConnMgrClientHandler, OnError, "(CConnClient)s");

	// 小地图
	REGIST_B_CLASS( Script, CCommpressFile );
	REGIST_CLASSFUNCTION( Script, CCommpressFile, ReadFile, "sCt:b");
	REGIST_CLASSFUNCTION( Script, CCommpressFile, GetWidth, ":i");
	REGIST_CLASSFUNCTION( Script, CCommpressFile, GetHeight, ":i");

	REGIST_D_CLASS( Script, CSmallMap, SQRWnd );
	REGIST_CLASSFUNCTION( Script, CSmallMap, SetScale, "f");
	REGIST_CLASSFUNCTION( Script, CSmallMap, InitMapData, "sC");
	REGIST_CLASSFUNCTION( Script, CSmallMap, SetPlayerPos, "(CVector3f)");
	REGIST_CLASSFUNCTION( Script, CSmallMap, ReceiveRoleData, "(CCoreObjectFollower)C");
	REGIST_CLASSFUNCTION( Script, CSmallMap, RemoveRoleData, "(CCoreObjectFollower):b");
	REGIST_CLASSFUNCTION( Script, CSmallMap, SetPlayerTex, "");
	REGIST_CLASSFUNCTION( Script, CSmallMap, SetSceneData, "IIfsC");
	REGIST_CLASSFUNCTION( Script, CSmallMap, SetSmallMapWnd, "(SQRWnd)");
	REGIST_CLASSFUNCTION( Script, CSmallMap, SetPlayerDir, "ff");
	REGIST_CLASSFUNCTION( Script, CSmallMap, InitData, "CCCCb");
	REGIST_CLASSFUNCTION( Script, CSmallMap, GetMapFileData, ":(CCommpressFile)");
	REGIST_CLASSFUNCTION( Script, CSmallMap, SetMainPlayerPixelPos, "ff");
	REGIST_CLASSFUNCTION( Script, CSmallMap, DrawFollower, "");
	REGIST_CLASSFUNCTION( Script, CSmallMap, SetMainPlayer, "I");
	REGIST_CLASSFUNCTION( Script, CSmallMap, AddEntity, "IsI");
	REGIST_CLASSFUNCTION( Script, CSmallMap, ChangeEntityImageType, "II");
	REGIST_CLASSFUNCTION( Script, CSmallMap, RemoveEntity, "I");
	REGIST_CLASSFUNCTION( Script, CSmallMap, AddShowRoleType, "I");
	REGIST_CLASSFUNCTION( Script, CSmallMap, RemoveShowRoleType, "I");
	
	REGIST_D_CLASS( Script, CBigMap, SQRWnd );
	REGIST_CLASSFUNCTION( Script, CBigMap, InitMapData, "s");
	REGIST_CLASSFUNCTION( Script, CBigMap, UpdateVisibleRegion, "");
	REGIST_CLASSFUNCTION( Script, CBigMap, AddAreaFrog, "s:b");
	REGIST_CLASSFUNCTION( Script, CBigMap, RemoveAreaFrog, "s:b");

	//------------------------------- CBloodRenderDialogPool -----------------------------------
	REGIST_B_CLASS( Script, CBloodRenderDialogPool );
	REGIST_CLASSFUNCTION(Script, CBloodRenderDialogPool, CreatePool, "(SQRWnd)");
	REGIST_CLASSFUNCTION(Script, CBloodRenderDialogPool, Release, "");

	REGIST_GLOBALFUNCTION( Script, LoadSkillGac, ":b" );
	REGIST_GLOBALFUNCTION( Script, UnloadSkillGac, "" );
	
	//-------------------------------- FightInfoData -------------------------------------------
	REGIST_B_CLASS( Script, CFightInfoData );
	REGIST_CLASSFUNCTION( Script, CFightInfoData, GetInfoIndex, ":(EFightInfoIndex)");
	REGIST_CLASSFUNCTION( Script, CFightInfoData, GetObj1ID, ":I");
	REGIST_CLASSFUNCTION( Script, CFightInfoData, GetObj2ID, ":I");
	REGIST_CLASSFUNCTION( Script, CFightInfoData, GetSkillName, ":s");
	REGIST_CLASSFUNCTION( Script, CFightInfoData, GetStateName, ":s");
	REGIST_CLASSFUNCTION( Script, CFightInfoData, GetArgName, ":s");
	REGIST_CLASSFUNCTION( Script, CFightInfoData, GetHPChanged, ":i");
	REGIST_CLASSFUNCTION( Script, CFightInfoData, GetHPExtra, ":I");
	REGIST_CLASSFUNCTION( Script, CFightInfoData, GetAttackType, ":C");
	REGIST_CLASSFUNCTION( Script, CFightInfoData, GetHour, ":C");
	REGIST_CLASSFUNCTION( Script, CFightInfoData, GetMinute, ":C");
	REGIST_CLASSFUNCTION( Script, CFightInfoData, GetSecond, ":C");
	REGIST_CLASSFUNCTION( Script, CFightInfoData, GetMillisecond, ":H");
	REGIST_CLASSFUNCTION( Script, CFightInfoData, GetRealHPChanged, ":I");
	REGIST_GLOBALFUNCTION( Script, ReloadSkillCfg, "s:b" );
	
	REGIST_B_CLASS( Script, CBuffData );
	REGIST_CLASSFUNCTION( Script, CBuffData, GetObjID, ":I");
	REGIST_CLASSFUNCTION( Script, CBuffData, GetDynamicId, ":I");
	REGIST_CLASSFUNCTION( Script, CBuffData, GetName, ":s");
	REGIST_CLASSFUNCTION( Script, CBuffData, GetCount, ":I");
	REGIST_CLASSFUNCTION( Script, CBuffData, GetTime, ":i");
	REGIST_CLASSFUNCTION( Script, CBuffData, GetRemainTime, ":f");
	REGIST_CLASSFUNCTION( Script, CBuffData, GetDecrease, ":b");
	REGIST_CLASSFUNCTION( Script, CBuffData, GetSkillLevel, ":i");
	REGIST_CLASSFUNCTION( Script, CBuffData, GetDotInterval, ":f");
	REGIST_CLASSFUNCTION( Script, CBuffData, GetInstallerID, ":I");
	REGIST_CLASSFUNCTION( Script, CBuffData, GetCalcValue, ":i");

	//------------------- CEntityClient ------------------------
	REGIST_ABSTRACT_B_CLASS( Script, CEntityClient );
	//REGIST_STATICFUNCTION(Script, CEntityClient, GetEntity, "I:(CEntityClient)");
	REGIST_CLASSFUNCTION( Script, CEntityClient, GetEntityID, ":I");
	REGIST_CLASSFUNCTION( Script, CEntityClient, GetLocalID, ":I");
	REGIST_STATICFUNCTION( Script, CEntityClient, GetSelectedEntity, "(CCoreSceneClient):(CEntityClient)");
	REGIST_CLASSFUNCTION( Script, CEntityClient, GetGameEntityType, ":(EGameEntityType)");
	REGIST_CLASSFUNCTION( Script, CEntityClient, GetGridPos, "(CPos)" );
	REGIST_CLASSFUNCTION_OVERLOAD( Script, CEntityClient, void(CEntityClient::*)(CFPos&)const, GetPixelPos, GetPixelPos, "(CFPos)");
	REGIST_CLASSFUNCTION( Script, CEntityClient, SetCanBeSelected, "b" );
	REGIST_CLASSFUNCTION( Script, CEntityClient, CanBeSelected, ":b" );
	REGIST_CLASSFUNCTION( Script, CEntityClient, SetLieOnTheTerrain, "b" );
	REGIST_CLASSFUNCTION( Script, CEntityClient, CanBeOnTheTerrain, ":b" );
	REGIST_CLASSFUNCTION( Script, CEntityClient, GetSyncVariantHolder, "(ECodeTableIndex):(CSyncVariantClientHolder)" );
	REGIST_CLASSFUNCTION( Script, CEntityClient, GetViewVariantHolder, "(ECodeTableIndex):(CSyncVariantClientHolder)" );
	REGIST_CLASSFUNCTION( Script, CEntityClient, GetViewVariantID, "s:I" );
	REGIST_CLASSFUNCTION( Script, CEntityClient, GetEntityDist, "(CEntityClient):f" );
	REGIST_CLASSFUNCTION( Script, CEntityClient, GetEntityDistInGrid, "(CEntityClient):f" );
	REGIST_CLASSFUNCTION( Script, CEntityClient, RegistDistortedTick, "(CDistortedTick)I");
	REGIST_CLASSFUNCTION( Script, CEntityClient, UnRegistDistortedTick, "(CDistortedTick)");
	REGIST_CLASSFUNCTION( Script, CEntityClient, GetDistortedProcessTime, ":Z");
	REGIST_CLASSFUNCTION( Script, CEntityClient, GetServerPixelPos, "(CFPos)");

	//------------------- CCharacterFollower ---------------------
	REGIST_ABSTRACT_D_CLASS( Script, CCharacterFollower, CEntityClient );
	REGIST_ABSTRACT_D_CLASS( Script, CCharacterDirector, CCharacterFollower);

	REGIST_CLASSFUNCTION( Script, CCharacterFollower, CppGetCtrlState, "(EFighterCtrlState):b" );
	REGIST_CLASSFUNCTION( Script, CCharacterFollower, CppGetClass, ":(EClass)" );
	REGIST_CLASSFUNCTION( Script, CCharacterFollower, CppSetClass, "(EClass)" );
	REGIST_CLASSFUNCTION( Script, CCharacterFollower, CppGetLevel, ":I" );
	REGIST_CLASSFUNCTION( Script, CCharacterFollower, CppSetLevel, "I" );
	REGIST_CLASSFUNCTION( Script, CCharacterFollower, CppGetBirthCamp, ":(ECamp)" );
	REGIST_CLASSFUNCTION( Script, CCharacterFollower, CppGetCamp, ":(ECamp)" );
	REGIST_CLASSFUNCTION( Script, CCharacterFollower, CppSetCamp, "(ECamp)" );
	REGIST_CLASSFUNCTION( Script, CCharacterFollower, CppGetPKState, ":b" );
	//REGIST_CLASSFUNCTION( Script, CCharacterFollower, CppGetPKState, ":(EPKState)" );
	REGIST_CLASSFUNCTION( Script, CCharacterFollower, CppGetPropertyValueByName, "s:f" );
	REGIST_CLASSFUNCTION( Script, CCharacterFollower, CppGetTalentPoint, "s:I");
	REGIST_CLASSFUNCTION( Script, CCharacterFollower, CppIsAlive, ":b");
	REGIST_CLASSFUNCTION( Script, CCharacterFollower, SetTargetBuff, "");
	REGIST_CLASSFUNCTION( Script, CCharacterFollower, GetRemainTime, "sI:f");
	REGIST_CLASSFUNCTION( Script, CCharacterFollower, DoNowAni,"");
	REGIST_CLASSFUNCTION( Script, CCharacterFollower, SetWeaponVisible, "bb" );
	REGIST_CLASSFUNCTION( Script, CCharacterFollower, SetWeaponPlayType, "(EWeaponPlayType)");
	REGIST_CLASSFUNCTION( Script, CCharacterFollower, SetOffWeaponPlayType, "(EWeaponPlayType)");
	REGIST_CLASSFUNCTION( Script, CCharacterFollower, GetWeaponPlayType, ":I");
	REGIST_CLASSFUNCTION( Script, CCharacterFollower, GetOffWeaponPlayType, ":I");
	REGIST_CLASSFUNCTION( Script, CCharacterFollower, GetBindObjParent, ":(CCharacterFollower)");
	REGIST_CLASSFUNCTION( Script, CCharacterFollower, MoveFirstBindObjChild, ":(CCharacterFollower)");
	REGIST_CLASSFUNCTION( Script, CCharacterFollower, MoveNextBindObjChild, ":(CCharacterFollower)");
	REGIST_CLASSFUNCTION( Script, CCharacterFollower, EndOfBindObjChild, ":b");
	REGIST_CLASSFUNCTION( Script, CCharacterFollower, GetCurBindObjChild, ":(CCharacterFollower)");
	REGIST_CLASSFUNCTION( Script, CCharacterFollower, SetStealthState, "b");
	REGIST_CLASSFUNCTION( Script, CCharacterFollower, GetCurMaterialNum, ":I");
	REGIST_CLASSFUNCTION( Script, CCharacterFollower, BeExpOwnerByDirector, ":b");
	REGIST_CLASSFUNCTION( Script, CCharacterFollower, GetExpOwnerID, ":I");
	
	REGIST_CLASSFUNCTION_OVERLOAD( Script, CCharacterFollower, void(CCharacterFollower::*)(EActionState), DoActionState, DoActionState, "I" );
	REGIST_CLASSFUNCTION_OVERLOAD( Script, CCharacterFollower, void(CCharacterFollower::*)(EActionState,uint8), DoActionState, DoAction, "IC" );
	REGIST_CLASSFUNCTION( Script, CCharacterFollower, CastToCharacterDirector,":(CCharacterDirector)");
	REGIST_CLASSFUNCTION( Script, CCharacterFollower, SetIsPlayer,"b");
	REGIST_CLASSFUNCTION( Script, CCharacterFollower, SetIsBoss,"b");
	REGIST_CLASSFUNCTION( Script, CCharacterFollower, SetAttackFxFile,"s");
	REGIST_CLASSFUNCTION( Script, CCharacterFollower, SetActionFxFile,"s");
	REGIST_CLASSFUNCTION( Script, CCharacterFollower, SetSkillFxFile,"s");
	REGIST_CLASSFUNCTION( Script, CCharacterFollower, SetSpliceName,"s");
	REGIST_STATICFUNCTION(Script, CCharacterFollower, GetCharacter, "(CCoreObjectFollower):(CCharacterFollower)");
	REGIST_STATICFUNCTION( Script, CCharacterFollower, GetCharacterByID,"I:(CCharacterFollower)");
	REGIST_CLASSFUNCTION( Script, CCharacterFollower, GetMaster,":(CCharacterFollower)");
	REGIST_CLASSFUNCTION( Script, CCharacterFollower, GetMasterID,":I");
	REGIST_CLASSFUNCTION( Script, CCharacterFollower, GetEntityID, ":I");
	REGIST_CLASSFUNCTION( Script, CCharacterFollower, IsMoving, ":b");
	REGIST_CLASSFUNCTION( Script, CCharacterFollower, GetType, ":(EClientObjectType)");
	REGIST_CLASSFUNCTION( Script, CCharacterFollower, IsBattleHorse, ":b");
	REGIST_CLASSFUNCTION( Script, CCharacterFollower, GetRenderObject, ":(CRenderObject)");
	REGIST_CLASSFUNCTION( Script, CCharacterFollower, GetHorseRenderObj, ":(CRenderObject)");
	REGIST_CLASSFUNCTION( Script, CCharacterFollower, GetMainHandRenderObj, ":(CRenderObject)");
	REGIST_CLASSFUNCTION( Script, CCharacterFollower, GetOffHandRenderObj, ":(CRenderObject)");
	REGIST_CLASSFUNCTION( Script, CCharacterFollower, GetAdditionRenderObj, ":(CRenderObject)");
	REGIST_CLASSFUNCTION( Script, CCharacterFollower, SetWeaponInfo, "ssss");
	REGIST_CLASSFUNCTION( Script, CCharacterFollower, SetHorseEffect, "s");
	REGIST_CLASSFUNCTION( Script, CCharacterFollower, SetAdditionEffect, "s");
	REGIST_CLASSFUNCTION( Script, CCharacterFollower, IsMoving,":b");
	REGIST_CLASSFUNCTION( Script, CCharacterFollower, GetRealName,":s");
	REGIST_CLASSFUNCTION( Script, CCharacterFollower, GetAITypeID, ":I");
	REGIST_CLASSFUNCTION( Script, CCharacterFollower, SetAITypeID, "I");
	REGIST_CLASSFUNCTION( Script, CCharacterFollower, BeActiveNpc, ":b");
	REGIST_CLASSFUNCTION( Script, CCharacterFollower, SetRemovedImmediately, "b");
	REGIST_CLASSFUNCTION( Script, CCharacterFollower, GetRemovedImmediately, ":b");
	REGIST_CLASSFUNCTION( Script, CCharacterFollower, CanBeTakeOver, ":b");
	REGIST_CLASSFUNCTION( Script, CCharacterFollower, GetNpcType, ":I")
	REGIST_CLASSFUNCTION( Script, CCharacterFollower, GetNpcAttackType, ":I");
	REGIST_CLASSFUNCTION( Script, CCharacterFollower, AddHeadBloodRendler, "(CCharacterFollower)(CBloodRenderDialogPool)(SQRWnd)b");
	REGIST_CLASSFUNCTION( Script, CCharacterFollower, DelHeadBloodRendler, "(CBloodRenderDialogPool)");
	REGIST_CLASSFUNCTION( Script, CCharacterFollower, ShowBloodRendler, "b");
	REGIST_CLASSFUNCTION( Script, CCharacterFollower, SetAgileValueBeCare, "b");
	REGIST_CLASSFUNCTION( Script, CCharacterFollower, CppGetGameCamp, ":i");
	REGIST_CLASSFUNCTION( Script, CCharacterFollower, ShowInClient, ":b");
	REGIST_CLASSFUNCTION( Script, CCharacterFollower, SetHorseName, "s");
	REGIST_CLASSFUNCTION( Script, CCharacterFollower, SetHeadBloodTransparent, "f");
	REGIST_CLASSFUNCTION( Script, CCharacterFollower, SetCurOnceActionLevel,"I");

	//------------------- CharacterHandler ---------------------

	REGIST_B_CLASS( Script, ICharacterFollowerCallbackHandler);
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterFollowerCallbackHandler, OnCreated, "(CCharacterFollower)b");
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterFollowerCallbackHandler, OnCreatedEnd, "(CCharacterFollower)");
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterFollowerCallbackHandler, OnDestroy, "(CCharacterFollower)");
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterFollowerCallbackHandler, OnIntoSyncAoi, "(CCharacterFollower)");
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterFollowerCallbackHandler, OnLeaveSyncAoi, "(CCharacterFollower)");
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterFollowerCallbackHandler, OnIntoScreen, "(CCharacterFollower)");
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterFollowerCallbackHandler, OnLeaveScreen, "(CCharacterFollower)");
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterFollowerCallbackHandler, OnAgileValueChanged, "I" );
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterFollowerCallbackHandler, OnCtrlValueChanged, "IIb" );
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterFollowerCallbackHandler, OnCastingBarBegin, "IsIffb" );
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterFollowerCallbackHandler, OnCastingBarEnd, "I" );
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterFollowerCallbackHandler, OnStateBegin, "III" );
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterFollowerCallbackHandler, OnStateEnd, "III" );
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterFollowerCallbackHandler, OnBuffIconUpdate, "(CBuffData)" );
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterFollowerCallbackHandler, OnPhysicalDamageChanged, "Iffffff" );
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterFollowerCallbackHandler, OnMagicDamageChanged, "Iffffff" );
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterFollowerCallbackHandler, OnCalValueChanged, "I" );
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterFollowerCallbackHandler, OnDead, "I" );
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterFollowerCallbackHandler, OnReborn, "I" );
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterFollowerCallbackHandler, SetLockObj, "II" );
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterFollowerCallbackHandler, OnRelationChanged, "I" );
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterFollowerCallbackHandler, OnFireActionEnd, "I" );
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterFollowerCallbackHandler, OnRealNameChange, "I(ENpcType)s");
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterFollowerCallbackHandler, OnResLoaded, "(CCharacterFollower)");
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterFollowerCallbackHandler, GetWeaponTypeString, "Ib:s" );
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterFollowerCallbackHandler, GetArmorTypeString, "I:s" );
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterFollowerCallbackHandler, GetNpcAniFileName, "s:s" );
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterFollowerCallbackHandler, CheckIsCanLock, "(CCharacterFollower):b" );
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterFollowerCallbackHandler, OnMaterialNumChange, "(CCharacterFollower)Ib");
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterFollowerCallbackHandler, OnExpOwnerChange, "(CCharacterFollower)");
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterFollowerCallbackHandler, OnDisplayMsg, "(CCharacterFollower)Is");

	REGIST_CLASSFUNCTION( Script, CCharacterFollower, SetCallbackHandler, "(ICharacterFollowerCallbackHandler)");
	REGIST_CLASSFUNCTION( Script, CCharacterFollower, GetCallbackHandler, ":(ICharacterFollowerCallbackHandler)");

	REGIST_D_CLASS( Script, ICharacterDirectorCallbackHandler, ICharacterFollowerCallbackHandler );
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterDirectorCallbackHandler, OnMagicCondFail, "Is" );
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterDirectorCallbackHandler, OnMagicCondFailOnTick, "sb" );
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterDirectorCallbackHandler, GroundSelector_GetPosX, ":f");
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterDirectorCallbackHandler, GroundSelector_GetPosY, ":f");
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterDirectorCallbackHandler, GroundSelector_IsActive, ":b");
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterDirectorCallbackHandler, GroundSelector_CancelSelectGround, "");
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterDirectorCallbackHandler, BeginSelectGround, "CsCCII");
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterDirectorCallbackHandler, OnCoolDownBegin, "s(ESkillCoolDownType)I");
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterDirectorCallbackHandler, OnCoolDownEnd, "s");
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterDirectorCallbackHandler, OnAllCoolDownEnd, "");
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterDirectorCallbackHandler, OnClearCommonCD, "");
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterDirectorCallbackHandler, OnStanceSkill, "");
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterDirectorCallbackHandler, OnLostTarget, "");
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterDirectorCallbackHandler, OnLevelUp, "i");
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterDirectorCallbackHandler, OnArrayValueChange, "sIII" );
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterDirectorCallbackHandler, OnGridPosChange, "");
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterDirectorCallbackHandler, OnPrintFightInfo, "(CFightInfoData)");
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterDirectorCallbackHandler, OnSetAutoNormalAttack, "b");
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterDirectorCallbackHandler, OnTransfered, "(CCharacterFollower)");
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterDirectorCallbackHandler, OnBeginTransfer, "(CCharacterFollower)s");
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterDirectorCallbackHandler, OnMoveSuccessed, "");
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterDirectorCallbackHandler, OnMoveStopped, "");
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterDirectorCallbackHandler, OnUpdateSmashRate, "I(EAttackType)Ifb");
	REGIST_CALLBACKFUNCTION( Script, ICharacterDirectorCallbackHandler, OnReplaceSkill, "ssb");
	REGIST_CALLBACKFUNCTION( Script, ICharacterDirectorCallbackHandler, OnBurstSoulTimesChanged, "I");
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterDirectorCallbackHandler, ChangeServant, "IIIb" );

	//------------------- CharacterDirector ---------------------
	REGIST_CLASSFUNCTION( Script, CCharacterDirector, DoFunActionState,"s");
	REGIST_CLASSFUNCTION( Script, CCharacterDirector, SetMHNACfg,"sfss" );
	REGIST_CLASSFUNCTION( Script, CCharacterDirector, SetAHNACfg,"sfs" );
	REGIST_CLASSFUNCTION( Script, CCharacterDirector, IsEnemy,"(CCharacterFollower):b" );
	REGIST_CLASSFUNCTION( Script, CCharacterDirector, IsFriend,"(CCharacterFollower):b" );
	REGIST_CLASSFUNCTION( Script, CCharacterDirector, IsNeutral,"(CCharacterFollower):b" );
	REGIST_CLASSFUNCTION( Script, CCharacterDirector, GetServant,"(ENpcType):(CCharacterFollower)");

	// new add
	REGIST_CLASSFUNCTION( Script, CCharacterDirector, GetConsumeType, "s:(EConsumeType)" );
	REGIST_CLASSFUNCTION( Script, CCharacterDirector, GetConsumeValue, "sI:I" );
	REGIST_CLASSFUNCTION( Script, CCharacterDirector, GetSkillDistance, "sI:I" );
	REGIST_CLASSFUNCTION( Script, CCharacterDirector, GetCastingProcessType, "s:(ECastingProcessType)" );
	REGIST_CLASSFUNCTION( Script, CCharacterDirector, GetCastingProcessTime, "sI:f" );
	REGIST_CLASSFUNCTION( Script, CCharacterDirector, GetSkillCoolDownType, "s:I" );
	REGIST_CLASSFUNCTION( Script, CCharacterDirector, GetCoolDownTime, "sI:f" );
	REGIST_CLASSFUNCTION( Script, CCharacterDirector, GetSkillLeftCoolDownTime, "s:I" );
	REGIST_CLASSFUNCTION( Script, CCharacterDirector, IsInCommonCD, "s:b" );
	REGIST_CLASSFUNCTION( Script, CCharacterDirector, IsSwitchEquipSkill, "s:b" );
	REGIST_CLASSFUNCTION( Script, CCharacterDirector, GetFireCondition, "s:I" );

	REGIST_CLASSFUNCTION( Script, CCharacterDirector, CancelCastingProcess, "" );
	REGIST_CLASSFUNCTION( Script, CCharacterDirector, GetTargetCastingProcess,"I");
	REGIST_CLASSFUNCTION( Script, CCharacterDirector, AddLearnSkill, "sI" );
	REGIST_CLASSFUNCTION( Script, CCharacterDirector, DelLearnSkill, "sI" );
	REGIST_CLASSFUNCTION( Script, CCharacterDirector, ClearAllLearnSkill, "" );
	REGIST_CLASSFUNCTION( Script, CCharacterDirector, FightSkill, "sC:(EDoSkillResult)" );
	REGIST_CLASSFUNCTION( Script, CCharacterDirector, DoNormalAttack, "Ib:b" );
	REGIST_CLASSFUNCTION( Script, CCharacterDirector, CheckDoNormalAttack, "I:b" );
	REGIST_CLASSFUNCTION( Script, CCharacterDirector, DoAttack, "sCIII" );
	REGIST_CLASSFUNCTION( Script, CCharacterDirector, UseTempSkill, "sCI" );
	REGIST_CLASSFUNCTION( Script, CCharacterDirector, CancelCoerceMove, "I" );
	REGIST_CLASSFUNCTION( Script, CCharacterDirector, IsPassive, ":b");
	REGIST_CLASSFUNCTION( Script, CCharacterDirector, GetClonedMovePath, ":(CPixelPath)");
	REGIST_CLASSFUNCTION( Script, CCharacterDirector, StopMoving, "");
	REGIST_CLASSFUNCTION( Script, CCharacterDirector, MoveToInAStarPath, "(CFPos)IIfi:I");
	REGIST_CLASSFUNCTION( Script, CCharacterDirector, MoveToInLinePath, "(CFPos)II:I");
	REGIST_CLASSFUNCTION( Script, CCharacterDirector, NotifyServerChangeDir, "(CFPos)");
	REGIST_CLASSFUNCTION( Script, CCharacterDirector, SetTarget, "I");
	REGIST_CLASSFUNCTION( Script, CCharacterDirector, CastToCharacterDirector,":(CCharacterDirector)" );
	REGIST_CLASSFUNCTION( Script, CCharacterDirector, IsNormalAttacking,":b" );
	//REGIST_CLASSFUNCTION( Script, CCharacterDirector, SetAutoNormalAttack,"b" );
	//REGIST_CLASSFUNCTION( Script, CCharacterDirector, IsAutoNormalAttack,":b" );
	REGIST_CLASSFUNCTION( Script, CCharacterDirector, CancelAutoTracker,"b" );
	REGIST_CLASSFUNCTION( Script, CCharacterDirector, CancelNormalAttack,"" );
	REGIST_CLASSFUNCTION( Script, CCharacterDirector, GetNormalAttackTarget,":(CCharacterFollower)");
	REGIST_CLASSFUNCTION( Script, CCharacterDirector, GetMaxSpeed,":f");
	REGIST_CLASSFUNCTION( Script, CCharacterDirector, GetCurSpeed,":f");
	REGIST_CLASSFUNCTION( Script, CCharacterDirector, OpenObjPanel, "I(CFighterViewInfo)bb");
	REGIST_CLASSFUNCTION( Script, CCharacterDirector, CancelState, "I");
	REGIST_CLASSFUNCTION( Script, CCharacterDirector, IsCanCancelState, "s:b");
	REGIST_CLASSFUNCTION( Script, CCharacterDirector, DestroyServant, "");
	REGIST_CLASSFUNCTION( Script, CCharacterDirector, SortTargetList, "");
	REGIST_CLASSFUNCTION( Script, CCharacterDirector, SelectFriendTarget, "");
	REGIST_CLASSFUNCTION( Script, CCharacterDirector, SelectAimTarget, "");
	REGIST_CLASSFUNCTION( Script, CCharacterDirector, ChangeCharacterFollowerList, "(CCharacterFollower)");
	REGIST_CLASSFUNCTION( Script, CCharacterDirector, GetActionState, ":(EActionState)");
	REGIST_CLASSFUNCTION( Script, CCharacterDirector, TurnAndSyncDir, "C");
	REGIST_CLASSFUNCTION( Script, CCharacterDirector, ChangeMoveMode, "b");
	REGIST_CLASSFUNCTION( Script, CCharacterDirector, ChangeDrawWeaponMode, "b");
	REGIST_CLASSFUNCTION( Script, CCharacterDirector, CreateFacingPair, "(CCharacterFollower)(CCharacterFollower)");
	REGIST_CLASSFUNCTION( Script, CCharacterDirector, RemoveFacingPair, "");
	REGIST_CLASSFUNCTION( Script, CCharacterDirector, ChangeShowHurtMOD, "");
	REGIST_CLASSFUNCTION( Script, CCharacterDirector, GetMoveDir, ":I" );
	REGIST_CLASSFUNCTION( Script, CCharacterDirector, ActionIsTurnOver, ":b" );
	REGIST_CLASSFUNCTION( Script, CCharacterDirector, IsStopBeforeTurn, "C:b" );
	REGIST_CLASSFUNCTION( Script, CCharacterDirector, SetTabDistance, "I" );
	REGIST_CLASSFUNCTION( Script, CCharacterDirector, IsCastingTypeSkill, "s:b" );
	REGIST_CLASSFUNCTION( Script, CCharacterDirector, CanUseNonFightSkill, "sC:b" );
	REGIST_CLASSFUNCTION( Script, CCharacterDirector, CanUseSkill, "sC:b" );
	REGIST_CLASSFUNCTION( Script, CCharacterDirector, TurnAroundByNonFightSkill, "s" );
	REGIST_CLASSFUNCTION( Script, CCharacterDirector, SetTargetPos, "(CFPos)" );
	REGIST_CLASSFUNCTION( Script, CCharacterDirector, GetTargetOfTarget, "I");
	REGIST_CLASSFUNCTION( Script, CCharacterDirector, AddCharacterFollowerToSet, "(CCharacterFollower)");
	REGIST_CLASSFUNCTION( Script, CCharacterDirector, ShowHeadBloodDialog, "b");
	REGIST_CLASSFUNCTION( Script, CCharacterDirector, SetMovingByKeyboard, "b");
	REGIST_CLASSFUNCTION( Script, CCharacterDirector, IsMovingByKeyboard, ":b");
	REGIST_CLASSFUNCTION( Script, CCharacterDirector, GetTarget, ":(CCharacterFollower)" );
	REGIST_CLASSFUNCTION( Script, CCharacterDirector, GetActionDir, ":C" );
	REGIST_CLASSFUNCTION( Script, CCharacterDirector, CppGetDoSkillCtrlState, "(EDoSkillCtrlState):b" );
	REGIST_CLASSFUNCTION( Script, CCharacterDirector, ExistState, "s:b" );
	REGIST_CLASSFUNCTION( Script, CCharacterDirector, AddFXCache, "s" );
	REGIST_CLASSFUNCTION( Script, CCharacterDirector, RemoveFXCache, "s" );
	REGIST_CLASSFUNCTION( Script, CCharacterDirector, ClearAllFXCache, "" );
	REGIST_CLASSFUNCTION_OVERLOAD( Script, CCharacterDirector, void (CCharacterDirector::*)(EDoSkillCtrlState, bool), CppSetDoSkillCtrlStateForLua, CppSetDoSkillCtrlState, "(EDoSkillCtrlState)b");
	
	//------------------------ServantType------------------------------
	REGIST_STATICFUNCTION( Script, ServantType, BeServantType, "I:b");

	REGIST_B_CLASS(Script, IAutoTrackerHandler);
	REGIST_CALLBACKFUNCTION(Script, IAutoTrackerHandler, OnReached, "(CEntityClient)(CEntityClient)");
	REGIST_CALLBACKFUNCTION(Script, IAutoTrackerHandler, OnCancel, "(CEntityClient)(CEntityClient)");
	REGIST_CALLBACKFUNCTION(Script, IAutoTrackerHandler, OnCheckStopCond, "(CEntityClient)(CEntityClient):b");

	REGIST_CLASSFUNCTION( Script, CCharacterDirector, InitAutoTracker, "IIf(IAutoTrackerHandler)b:b")

	//------------------------CIntObjClient------------------------------
	REGIST_ABSTRACT_D_CLASS(Script, CIntObjClient, CEntityClient);
	REGIST_B_CLASS(Script, IIntObjCallbackHandler);
	REGIST_CLASSFUNCTION(Script, CIntObjClient, SetCallbackHandler, "(IIntObjCallbackHandler)");;
	REGIST_CLASSFUNCTION( Script, CIntObjClient, CIntObjDoActor, "s" );
	REGIST_CLASSFUNCTION( Script, CIntObjClient, SetIntObjRenderType, "" );
	REGIST_CLASSFUNCTION( Script, CIntObjClient, GetRenderObject, ":(CRenderObject)");
	REGIST_STATICFUNCTION(Script, CIntObjClient, GetIntObj, "(CCoreObjectFollower):(CIntObjClient)");
	REGIST_STATICFUNCTION(Script, CIntObjClient, GetIntObjByID, "I:(CIntObjClient)");
	REGIST_CALLBACKFUNCTION(Script, IIntObjCallbackHandler, OnCreated, "(CIntObjClient)");
	REGIST_CALLBACKFUNCTION(Script, IIntObjCallbackHandler, OnDestroy, "(CIntObjClient)");
	REGIST_CALLBACKFUNCTION(Script, IIntObjCallbackHandler, OnIntoSyncAoi, "(CIntObjClient)");
	REGIST_CALLBACKFUNCTION(Script, IIntObjCallbackHandler, OnLeaveSyncAoi, "(CIntObjClient)");
	REGIST_CALLBACKFUNCTION(Script, IIntObjCallbackHandler, OnObjDoActionEnd, "(CIntObjClient)");
	REGIST_CALLBACKFUNCTION(Script, IIntObjCallbackHandler, OnAnimationEnd, "(CIntObjClient)s");
	//REGIST_STATICFUNCTION( Script, CIntObjClient, CreateIntObj, "(CCoreObjectFollower):(CIntObjClient)");

	//REGIST_ABSTRACT_B_CLASS( Script, CIntObjClientManager );
	//REGIST_CLASSFUNCTION(Script, CIntObjClientManager, SetHandler, "(IIntObjClientManagerHandler)");
	//REGIST_CLASSFUNCTION(Script, CIntObjClientManager, GetHandler, ":(IIntObjClientManagerHandler)");
	//REGIST_STATICFUNCTION(Script, CIntObjClientManager, Get, ":(CIntObjClientManager)");
	//---------------------- CharacterManager --------------------------
	REGIST_B_CLASS( Script, IEntityClientManagerHandler );
	REGIST_CALLBACKFUNCTION(Script, IEntityClientManagerHandler, OnGameEntityAdded, "(CEntityClient)");

	REGIST_ABSTRACT_B_CLASS( Script, CEntityClientManager );
	REGIST_CLASSFUNCTION(Script, CEntityClientManager, SetHandler, "(IEntityClientManagerHandler)");
	REGIST_CLASSFUNCTION(Script, CEntityClientManager, GetHandler, ":(IEntityClientManagerHandler)");
	REGIST_STATICFUNCTION(Script, CEntityClientManager, GetInst, ":(CEntityClientManager)");
	REGIST_STATICFUNCTION(Script, CEntityClientManager, CreateInst, "");
	REGIST_STATICFUNCTION(Script, CEntityClientManager, DestroyInst, "");

	REGIST_GLOBALFUNCTION( Script,PlayCue, "s");
	REGIST_GLOBALFUNCTION( Script,StopCue, "s");
	REGIST_GLOBALFUNCTION( Script,CueIsStop, "s:b");

	REGIST_B_CLASS( Script, ITeamClientManagerHandler );
	REGIST_CALLBACKFUNCTION(Script, ITeamClientManagerHandler, UpdateTeamInfo, "");

	REGIST_STATICFUNCTION(Script, CTeamClientManager, SetHandler, "(ITeamClientManagerHandler)");
	REGIST_STATICFUNCTION(Script, CTeamClientManager, GetHP, "I:I");
	REGIST_STATICFUNCTION(Script, CTeamClientManager, GetMP, "I:I");
	REGIST_STATICFUNCTION(Script, CTeamClientManager, GetEP, "I:I");
	REGIST_STATICFUNCTION(Script, CTeamClientManager, GetRP, "I:I");
	REGIST_STATICFUNCTION(Script, CTeamClientManager, GetBaseHP, "I:I");
	REGIST_STATICFUNCTION(Script, CTeamClientManager, GetBaseMP, "I:I");
	REGIST_STATICFUNCTION(Script, CTeamClientManager, GetBaseEP, "I:I");
	REGIST_STATICFUNCTION(Script, CTeamClientManager, GetBaseRP, "I:I");
	REGIST_STATICFUNCTION(Script, CTeamClientManager, GetPos, "I(CPos)");
	REGIST_STATICFUNCTION(Script, CTeamClientManager, GetLevel, "I:I");
	REGIST_STATICFUNCTION(Script, CTeamClientManager, GetHairResID, "I:I");
	REGIST_STATICFUNCTION(Script, CTeamClientManager, GetFaceResID, "I:I");
	REGIST_STATICFUNCTION(Script, CTeamClientManager, GetHeadIndexID, "I:I");
	REGIST_STATICFUNCTION(Script, CTeamClientManager, GetBodyIndexID, "I:I");
	REGIST_STATICFUNCTION(Script, CTeamClientManager, GetInBattle, "I:b");
	
	REGIST_B_CLASS( Script, CGameSceneHandler );

	REGIST_STATICFUNCTION(Script, CBloodTypeCfg, LoadCfg, ":b");
	REGIST_STATICFUNCTION(Script, CBloodTypeCfg, UnLoadCfg, "");

	//---------------------- CDirectionInfo --------------------------
	REGIST_D_CLASS( Script, CDirectionInfo, CRenderDialog );
	REGIST_CLASSFUNCTION(Script, CDirectionInfo, SetCharacterEntityID, "I");

	REGIST_STATICFUNCTION(Script, CSoundCDTimeMgr, PlaySound, "sI");
	REGIST_STATICFUNCTION(Script, CSoundCDTimeMgr, OnDestroy, "");

	REGIST_ABSTRACT_B_CLASS( Script, CModelInfoClient);
	REGIST_STATICFUNCTION( Script, CModelInfoClient, GetModelInfo, "s(EClass)(ECharSex):(CModelInfoClient)" );
	REGIST_CLASSFUNCTION( Script, CModelInfoClient, GetPieceName, ":s" );
	REGIST_CLASSFUNCTION( Script, CModelInfoClient, GetHidePiece, ":s" );
	REGIST_CLASSFUNCTION( Script, CModelInfoClient, GetModelFileName, ":s" );
	REGIST_CLASSFUNCTION( Script, CModelInfoClient, GetFXFileName, ":s" );
	REGIST_CLASSFUNCTION( Script, CModelInfoClient, GetStyleName, ":s" );
}
