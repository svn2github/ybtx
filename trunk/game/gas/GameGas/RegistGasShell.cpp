#include "stdafx.h"
#include "RegistGasShell.h"
#include "ScriptX.h"
#include "CScriptAppServer.h"
#include "RegistShellCommon.h"
#include "CppGameServer.h"
#include "LoadAllCfg.h"
#include "CConnMgrServerHandler.h"
#include "CCharacterDictator.h"
#include "CCharacterMediator.h"
#include "ICharacterMediatorCallbackHandler.h"
#include "ICharacterDictatorCallbackHandler.h"
#include "CEntityServerCreator.h"
#include "CNpcServerCreator.h"
#include "CIntObjServer.h"
#include "IIntObjCallbackHandlerServer.h"
#include "CStateDBData.inl"
#include "CEntityServer.h"
#include "CBaseConnMgrHandler.inl"
#include "CNpcServerBaseData.h"
#include "IGameGridRadiusCallback.h"
#include "CEntityServerManager.h"
#include "CTestBulletMagic.h"
#include "CTestShockWaveMagic.h"
#include "CTeamServerManager.h"
#include "CFightSkillProf.h"
#include "CAoiDimMgr.h"
#include "CMoveServerManager.h"
#include "CTestHurtResult.h"
#include "CheckSkill.h"
#include "PatternRef.inl"
#include "NpcInfoMgr.h"
#include "CNpcResServerData.h"
#include "CActorCfg.h"
#include "CCoreSceneServer.h"
#include "CCoreObjectServer.h"
#include "CApexProxy.h"
#include "CBaseStateServer.h"
#include "TSqrAllocator.inl"
#include "NpcDefs.h"
#include "IServantQueryCallback.h"
#include "CDimensionInfo.h"
#include "IApexProxyCallBackHandler.h"
#include "INpcEnmityQueryCallback.h"
#include "CCpuUsageMgr.h"
#include "CYYServer.h"
#include "CPlayerSet.h"
//#include "WeiBo/CWeiBoServer.h"

template class TPtRefee<ICharacterDictatorCallbackHandler,CCharacterDictator>;

void RegistGasShell(CScript& Script)
{	
	// 公用注册调用
	RegistShellCommon(Script);
	
	// 注册全局函数
	REGIST_GLOBALFUNCTION( Script, CppGameServerInit,":b" );
	REGIST_GLOBALFUNCTION( Script, CppGameServerClear,"" );
	REGIST_GLOBALFUNCTION( Script, LoadSkillCfg, ":b" );
	REGIST_GLOBALFUNCTION( Script, UnloadSkillCfg, "" );
	REGIST_GLOBALFUNCTION( Script, ReloadSkillCfg, "s:b");

	REGIST_B_CLASS( Script, IApexProxyCallBackHandler );
	REGIST_CALLBACKFUNCTION( Script, IApexProxyCallBackHandler, OnNoticeBeKilledUser, "(CConnServer)");

	REGIST_B_CLASS( Script, CApexProxy );
	REGIST_STATICFUNCTION( Script, CApexProxy, CreateInst, "" );
	REGIST_STATICFUNCTION( Script, CApexProxy, DestroyInst, "" );
	REGIST_STATICFUNCTION( Script, CApexProxy, GetInst, ":(CApexProxy)" );
	REGIST_CLASSFUNCTION( Script, CApexProxy, StartApexProxy, "(CAddress)" );
	REGIST_CLASSFUNCTION( Script, CApexProxy, StopApexProxy, "" );
	REGIST_CLASSFUNCTION( Script, CApexProxy, NoticeApexProxy_UserLogin, "Is(CConnServer):I" );
	REGIST_CLASSFUNCTION( Script, CApexProxy, NoticeApexProxy_UserLogout, "Is:I" );
	REGIST_CLASSFUNCTION( Script, CApexProxy, SetApexProxyCallBackHandler, "(IApexProxyCallBackHandler)" );

	REGIST_B_CLASS( Script, CYYServer );
	REGIST_STATICFUNCTION(	Script,	CYYServer,	CreateInst, "s" );
	REGIST_STATICFUNCTION(	Script,	CYYServer,	GetInst, ":(CYYServer)");
	REGIST_CLASSFUNCTION(	Script,	CYYServer,	DestroyInst, "" );
	REGIST_CLASSFUNCTION(	Script,	CYYServer,	RecvLoginRequest, "I:s");
	REGIST_CLASSFUNCTION(	Script,	CYYServer,	RecvLoginInfo, "sI:i");
	REGIST_CLASSFUNCTION(	Script,	CYYServer,	GetUserID,"s:s");
	REGIST_CLASSFUNCTION(	Script,	CYYServer,	CardNumber,"s:s");

	// 注册 IScriptConnMgrServerHandler 及回调函数
	REGIST_B_CLASS( Script, IScriptConnMgrServerHandler);
	REGIST_CALLBACKFUNCTION( Script, IScriptConnMgrServerHandler, OnDataReceived, "(CConnServer)");
	REGIST_CALLBACKFUNCTION(Script, IScriptConnMgrServerHandler, OnAccepted, "(CConnServer)");
	REGIST_CALLBACKFUNCTION(Script, IScriptConnMgrServerHandler, OnDisconnect, "(CConnServer)");
	REGIST_CALLBACKFUNCTION(Script, IScriptConnMgrServerHandler, OnError, "(CConnServer)s");
	REGIST_CALLBACKFUNCTION(Script, IScriptConnMgrServerHandler, OnBeginServiceSucceeded, "");
	REGIST_CALLBACKFUNCTION(Script, IScriptConnMgrServerHandler, OnServiceEnded, "");
	REGIST_CALLBACKFUNCTION(Script, IScriptConnMgrServerHandler, OnBeginServiceFailed, "s");
	REGIST_CALLBACKFUNCTION(Script, IScriptConnMgrServerHandler, OnCheat, "(CConnServer)");

	// 注册 CBaseConnServerMgrHandler 及回调函数
	typedef CBaseConnMgrHandler<CConnServer, IScriptConnMgrServerHandler> CBaseConnServerMgrHandler;
	REGIST_B_CLASS( Script, CBaseConnServerMgrHandler );
	REGIST_CLASSFUNCTION( Script, CBaseConnServerMgrHandler, SetScriptHandler, "(IScriptConnMgrServerHandler)" );
	REGIST_CLASSFUNCTION( Script, CBaseConnServerMgrHandler, GetScriptHandler, ":(IScriptConnMgrServerHandler)" );

	// 注册 CConnMgrServerHandler 及回调函数
	REGIST_D_CLASS_2( Script, CConnMgrServerHandler, IConnMgrServerHandler, CBaseConnServerMgrHandler);
	REGIST_CLASSFUNCTION(Script, CConnMgrServerHandler, OnAccepted, "(CConnServer)");
	REGIST_CLASSFUNCTION(Script, CConnMgrServerHandler, OnDisconnect, "(CConnServer)");
	REGIST_CLASSFUNCTION(Script, CConnMgrServerHandler, OnError, "(CConnServer)s");

	REGIST_ENUMTYPE(Script, EObjectAoiType);
	REGIST_ENUMERATION(Script, EObjectAoiType, EAoi_TaskNpc);
	REGIST_ENUMERATION(Script, EObjectAoiType, EAoi_ActiveNpc);
	REGIST_ENUMERATION(Script, EObjectAoiType, EAoi_PassiveNpc);
	REGIST_ENUMERATION(Script, EObjectAoiType, EAoi_NatureNpc);
	REGIST_ENUMERATION(Script, EObjectAoiType, EAoi_NatureActiveNpc);
	REGIST_ENUMERATION(Script, EObjectAoiType, EAoi_NaturePassiveNpc);
	REGIST_ENUMERATION(Script, EObjectAoiType, EAoi_ActiveServant);
	REGIST_ENUMERATION(Script, EObjectAoiType, EAoi_PassiveServant);
	REGIST_ENUMERATION(Script, EObjectAoiType, EAoi_IntObjSeeNone);
	REGIST_ENUMERATION(Script, EObjectAoiType, EAoi_IntObjSeeAll);
	REGIST_ENUMERATION(Script, EObjectAoiType, EAoi_TrapSeeAll);
	REGIST_ENUMERATION(Script, EObjectAoiType, EAoi_TrapSeePlayer);
	REGIST_ENUMERATION(Script, EObjectAoiType, EAoi_TrapSeeNpc);
	REGIST_ENUMERATION(Script, EObjectAoiType, EAoi_SleepNPC);

	REGIST_ENUMTYPE(Script, EAoiItemSizeType);
	REGIST_ENUMERATION(Script, EAoiItemSizeType, eAIST_PlayerOrServant);
	REGIST_ENUMERATION(Script, EAoiItemSizeType, eAIST_Magic);
	REGIST_ENUMERATION(Script, EAoiItemSizeType, eAIST_TaskNPC);
	REGIST_ENUMERATION(Script, EAoiItemSizeType, eAIST_IntObj);
	REGIST_ENUMERATION(Script, EAoiItemSizeType, eAIST_Monster);
	REGIST_ENUMERATION(Script, EAoiItemSizeType, eAIST_InteractObj);
	REGIST_ENUMERATION(Script, EAoiItemSizeType, eAIST_InterestingMonster);
	REGIST_ENUMERATION(Script, EAoiItemSizeType, eAIST_InterestingNPC);
	REGIST_ENUMERATION(Script, EAoiItemSizeType, eAIST_InterestingIntObj);

	//------------------- CEntityServer ---------------------
	REGIST_ABSTRACT_B_CLASS( Script, CEntityServer );
	REGIST_CLASSFUNCTION( Script, CEntityServer, GetEntityID, ":I" );
	REGIST_CLASSFUNCTION( Script, CEntityServer, GetGameEntityType, ":(EGameEntityType)");
	REGIST_CLASSFUNCTION( Script, CEntityServer, GetScene, ":(CCoreSceneServer)" );
	REGIST_CLASSFUNCTION( Script, CEntityServer, IsMoving, ":b" );
	REGIST_CLASSFUNCTION( Script, CEntityServer, MoveToInAStarPath, "(CFPos)fIIfi:I");
	REGIST_CLASSFUNCTION( Script, CEntityServer, StopMoving, ":I" );
	REGIST_CLASSFUNCTION( Script, CEntityServer, SetPixelPos, "(CFPos):I" );
	REGIST_CLASSFUNCTION_OVERLOAD( Script, CEntityServer, void(CEntityServer::*)(CFPos&)const, GetPixelPos, GetPixelPos, "(CFPos)");
	REGIST_CLASSFUNCTION_OVERLOAD( Script, CEntityServer, ISend*(CEntityServer::*)(uint32)const, GetIS, GetIS, "I:(ISend)");
	REGIST_CLASSFUNCTION( Script, CEntityServer, SetGridPos, "(CPos):I" );
	REGIST_CLASSFUNCTION( Script, CEntityServer, SetEyeSight, "f" );
	REGIST_CLASSFUNCTION( Script, CEntityServer, SetSecondEyeSight, "f" );
	REGIST_CLASSFUNCTION( Script, CEntityServer, SetDoubleEyeSight, "ff");
	REGIST_CLASSFUNCTION( Script, CEntityServer, SetSize, "f" );
	REGIST_CLASSFUNCTION( Script, CEntityServer, SetKeenness, "f" );
	REGIST_CLASSFUNCTION( Script, CEntityServer, SetStealth, "f" );
	REGIST_CLASSFUNCTION( Script, CEntityServer, SetViewGroup, "I" );
	//REGIST_CLASSFUNCTION( Script, CEntityServer, SetEyeSightType, "(EAoiItemEyeSightType)");
	REGIST_CLASSFUNCTION( Script, CEntityServer, SetSizeType, "(EAoiItemSizeType)");
	//REGIST_CLASSFUNCTION( Script, CEntityServer, SetAoiType, "(EAoiItemEyeSightType)(EAoiItemSizeType)");
	REGIST_CLASSFUNCTION( Script, CEntityServer, SetObjectAoiType, "(EObjectAoiType)");
	REGIST_CLASSFUNCTION( Script, CEntityServer, UniteObjectAoiType, "(EObjectAoiType)");
	REGIST_CLASSFUNCTION( Script, CEntityServer, GetEyeSight, ":f" );
	REGIST_CLASSFUNCTION( Script, CEntityServer, GetSize, ":f" );
	REGIST_CLASSFUNCTION( Script, CEntityServer, SetSecondEyeSight, "f");
	REGIST_CLASSFUNCTION( Script, CEntityServer, GetKeenness, ":f" );
	REGIST_CLASSFUNCTION( Script, CEntityServer, GetStealth, ":f" );
	REGIST_CLASSFUNCTION( Script, CEntityServer, IsInViewAoi, "I:b" );
	REGIST_CLASSFUNCTION( Script, CEntityServer, SetDynamicBarrier, "(EBarrierType)" );
	REGIST_CLASSFUNCTION( Script, CEntityServer, DisableDirectorMoving, "b" );
	REGIST_CLASSFUNCTION( Script, CEntityServer, GetEntityDistInGrid, "(CEntityServer):f" );
	REGIST_CLASSFUNCTION( Script, CEntityServer, GetEntityDist, "(CEntityServer):f" );
	//REGIST_CLASSFUNCTION( Script, CEntityServer, SetBarrierSize, "f" );
	REGIST_CLASSFUNCTION( Script, CEntityServer, SetBottomSize, "f" );
	REGIST_CLASSFUNCTION( Script, CEntityServer, GetSyncVariantHolder, "(ECodeTableIndex):(CSyncVariantServerHolder)" );
	REGIST_CLASSFUNCTION( Script, CEntityServer, GetViewVariantHolder, "(ECodeTableIndex):(CSyncVariantServerHolder)" );
	REGIST_CLASSFUNCTION( Script, CEntityServer, SetObjInValidByDestory, "");
	REGIST_CLASSFUNCTION( Script, CEntityServer, GetObjValidState, ":b");

	REGIST_B_CLASS( Script, CStateDBData );
	//REGIST_CLASSMEMBER( Script, CStateDBData, m_eType, "(EStateGlobalType)" );
	//REGIST_CLASSMEMBER( Script, CStateDBData, m_sName, "s");
	//REGIST_CLASSMEMBER( Script, CStateDBData, m_iTime, "i" );
	//REGIST_CLASSMEMBER( Script, CStateDBData, m_iRemainTime, "i" );
	//REGIST_CLASSMEMBER( Script, CStateDBData, m_iCount, "i" );
	//REGIST_CLASSMEMBER( Script, CStateDBData, m_fProbability, "f" );
	//REGIST_CLASSMEMBER( Script, CStateDBData, m_uCancelableInstID, "I" );
	//REGIST_CLASSMEMBER( Script, CStateDBData, m_uDotInstID, "I" );
	//REGIST_CLASSMEMBER( Script, CStateDBData, m_uFinalInstID, "I" );
	//REGIST_CLASSMEMBER( Script, CStateDBData, m_uSkillLevel, "I" );
	//REGIST_CLASSMEMBER( Script, CStateDBData, m_sSkillName, "s" );
	//REGIST_CLASSMEMBER( Script, CStateDBData, m_eAttackType, "(EAttackType)" );
	//REGIST_CLASSMEMBER( Script, CStateDBData, m_bIsDot, "b" );
	//REGIST_CLASSMEMBER( Script, CStateDBData, m_bFromEqualsOwner, "b" );
	REGIST_CLASSFUNCTION( Script, CStateDBData, GetType, ":(EStateGlobalType)" );
	REGIST_CLASSFUNCTION( Script, CStateDBData, GetName, ":s");
	REGIST_CLASSFUNCTION( Script, CStateDBData, GetTime, ":i" );
	REGIST_CLASSFUNCTION( Script, CStateDBData, GetRemainTime, ":i" );
	REGIST_CLASSFUNCTION( Script, CStateDBData, GetCount, ":i" );
	REGIST_CLASSFUNCTION( Script, CStateDBData, GetProbability, ":f" );
	REGIST_CLASSFUNCTION( Script, CStateDBData, GetCancelableInstID, ":I" );
	REGIST_CLASSFUNCTION( Script, CStateDBData, GetDotInstID, ":I" );
	REGIST_CLASSFUNCTION( Script, CStateDBData, GetFinalInstID, ":I" );
	REGIST_CLASSFUNCTION( Script, CStateDBData, GetSkillLevel, ":C" );
	REGIST_CLASSFUNCTION( Script, CStateDBData, GetSkillName, ":s" );
	REGIST_CLASSFUNCTION( Script, CStateDBData, GetAttackType, ":(EAttackType)" );
	REGIST_CLASSFUNCTION( Script, CStateDBData, GetIsDot, ":b" );
	REGIST_CLASSFUNCTION( Script, CStateDBData, GetFromEqualsOwner, ":b" );
	REGIST_CLASSFUNCTION( Script, CStateDBData, SetType, "(EStateGlobalType)" );
	REGIST_CLASSFUNCTION( Script, CStateDBData, SetName, "s");
	REGIST_CLASSFUNCTION( Script, CStateDBData, SetTime, "i" );
	REGIST_CLASSFUNCTION( Script, CStateDBData, SetRemainTime, "i" );
	REGIST_CLASSFUNCTION( Script, CStateDBData, SetCount, "i" );
	REGIST_CLASSFUNCTION( Script, CStateDBData, SetProbability, "f" );
	REGIST_CLASSFUNCTION( Script, CStateDBData, SetCancelableInstID, "I" );
	REGIST_CLASSFUNCTION( Script, CStateDBData, SetDotInstID, "I" );
	REGIST_CLASSFUNCTION( Script, CStateDBData, SetFinalInstID, "I" );
	REGIST_CLASSFUNCTION( Script, CStateDBData, SetSkillLevel, "C" );
	REGIST_CLASSFUNCTION( Script, CStateDBData, SetSkillName, "s" );
	REGIST_CLASSFUNCTION( Script, CStateDBData, SetAttackType, "(EAttackType)" );
	REGIST_CLASSFUNCTION( Script, CStateDBData, SetIsDot, "b" );
	REGIST_CLASSFUNCTION( Script, CStateDBData, SetFromEqualsOwner, "b" );

	REGIST_B_CLASS( Script, CStoredObjDBData );
	//REGIST_CLASSMEMBER( Script, CStoredObjDBData, m_uInstID, "I");
	//REGIST_CLASSMEMBER( Script, CStoredObjDBData, m_uCascadeID, "I");
	//REGIST_CLASSMEMBER( Script, CStoredObjDBData, m_uMOPID, "I" );
	//REGIST_CLASSMEMBER( Script, CStoredObjDBData, m_iMOPArg, "i" );
	//REGIST_CLASSMEMBER( Script, CStoredObjDBData, m_uMOPRet, "I" );
	REGIST_CLASSFUNCTION( Script, CStoredObjDBData, GetInstID, ":I");
	REGIST_CLASSFUNCTION( Script, CStoredObjDBData, GetCascadeID, ":I");
	REGIST_CLASSFUNCTION( Script, CStoredObjDBData, GetMOPID, ":I" );
	REGIST_CLASSFUNCTION( Script, CStoredObjDBData, GetMOPArg, ":i" );
	REGIST_CLASSFUNCTION( Script, CStoredObjDBData, GetMOPRet, ":I" );
	REGIST_CLASSFUNCTION( Script, CStoredObjDBData, SetInstID, "I");
	REGIST_CLASSFUNCTION( Script, CStoredObjDBData, SetCascadeID, "I");
	REGIST_CLASSFUNCTION( Script, CStoredObjDBData, SetMOPID, "I" );
	REGIST_CLASSFUNCTION( Script, CStoredObjDBData, SetMOPArg, "i" );
	REGIST_CLASSFUNCTION( Script, CStoredObjDBData, SetMOPRet, "I" );

	REGIST_ABSTRACT_B_CLASS( Script, CBaseStateCfg );
	REGIST_ABSTRACT_D_CLASS( Script, CBaseStateCfgServer, CBaseStateCfg );
	REGIST_STATICFUNCTION( Script, CBaseStateCfgServer, SetNeedSyncByName, "s");

	REGIST_B_CLASS( Script, CSkillCoolDownDBData );
	REGIST_CLASSFUNCTION( Script, CSkillCoolDownDBData, GetLeftTime, ":I");
	REGIST_CLASSFUNCTION( Script, CSkillCoolDownDBData, GetSkillName, ":s");
	REGIST_CLASSFUNCTION( Script, CSkillCoolDownDBData, GetIsSwitchEquipSkill, ":b")
	REGIST_CLASSFUNCTION( Script, CSkillCoolDownDBData, SetLeftTime, "I");
	REGIST_CLASSFUNCTION( Script, CSkillCoolDownDBData, SetSkillName, "s");
	REGIST_CLASSFUNCTION( Script, CSkillCoolDownDBData, SetIsSwitchEquipSkill, "b");;

	REGIST_B_CLASS( Script, CAureMagicDBData );
	REGIST_CLASSFUNCTION( Script, CAureMagicDBData, GetSkillLevel, ":C");
	REGIST_CLASSFUNCTION( Script, CAureMagicDBData, GetSkillName, ":s");
	REGIST_CLASSFUNCTION( Script, CAureMagicDBData, GetAttackType, ":I");
	REGIST_CLASSFUNCTION( Script, CAureMagicDBData, GetAureMagicName, ":s");

	REGIST_B_CLASS( Script, CStateDBDataVec );
	REGIST_CLASSFUNCTION( Script, CStateDBDataVec, CreateDBData, ":(CStateDBData)");
	REGIST_CLASSFUNCTION( Script, CStateDBDataVec, Get, "I:(CStateDBData)");
	REGIST_CLASSFUNCTION( Script, CStateDBDataVec, GetCount, ":I");
	REGIST_CLASSFUNCTION( Script, CStateDBDataVec, PushBack, "(CStateDBData)");

	REGIST_B_CLASS( Script, CStoredObjDBDataVec );
	REGIST_CLASSFUNCTION( Script, CStoredObjDBDataVec, CreateDBData, ":(CStoredObjDBData)");
	REGIST_CLASSFUNCTION( Script, CStoredObjDBDataVec, Get, "I:(CStoredObjDBData)");
	REGIST_CLASSFUNCTION( Script, CStoredObjDBDataVec, GetCount, ":I");
	REGIST_CLASSFUNCTION( Script, CStoredObjDBDataVec, PushBack, "(CStoredObjDBData)");

	REGIST_B_CLASS( Script, CStateDBDataSet );
	REGIST_CLASSFUNCTION( Script, CStateDBDataSet, GetStateVec, ":(CStateDBDataVec)");
	REGIST_CLASSFUNCTION( Script, CStateDBDataSet, GetStoredObjVec, ":(CStoredObjDBDataVec)");
	//REGIST_CLASSMEMBER( Script, CStateDBDataSet, m_pStateVec, "(CStateDBDataVec)" );
	//REGIST_CLASSMEMBER( Script, CStateDBDataSet, m_pStoredObjVec, "(CStoredObjDBDataVec)" );
	REGIST_CLASSFUNCTION( Script, CStateDBDataSet, Release, "");

	REGIST_B_CLASS( Script, CSkillCoolDownDBDataVec );
	REGIST_CLASSFUNCTION( Script, CSkillCoolDownDBDataVec, Get, "I:(CSkillCoolDownDBData)");
	REGIST_CLASSFUNCTION( Script, CSkillCoolDownDBDataVec, CreateDBData, ":(CSkillCoolDownDBData)");
	REGIST_CLASSFUNCTION( Script, CSkillCoolDownDBDataVec, GetCount, ":I");
	REGIST_CLASSFUNCTION( Script, CSkillCoolDownDBDataVec, PushBack, "(CSkillCoolDownDBData)");

	REGIST_B_CLASS( Script, CSkillCoolDownDBDataMgr );
	REGIST_CLASSFUNCTION( Script, CSkillCoolDownDBDataMgr, GetSkillCoolDownDBDateVec, ":(CSkillCoolDownDBDataVec)");
	REGIST_CLASSFUNCTION( Script, CSkillCoolDownDBDataMgr, Release, "");

	REGIST_B_CLASS( Script, CAureMagicDBDataVec );
	REGIST_CLASSFUNCTION( Script, CAureMagicDBDataVec, Get, "I:(CAureMagicDBData)");
	REGIST_CLASSFUNCTION( Script, CAureMagicDBDataVec, GetCount, ":I");

	REGIST_B_CLASS( Script, CAureMagicDBDataMgr );
	REGIST_CLASSFUNCTION( Script, CAureMagicDBDataMgr, GetAureMagicDBDateVec, ":(CAureMagicDBDataVec)");

	//------------------- CharacterDictator ---------------------
	REGIST_ABSTRACT_D_CLASS( Script, CCharacterDictator, CEntityServer );
	REGIST_ABSTRACT_D_CLASS( Script, CCharacterMediator, CCharacterDictator);
	REGIST_B_CLASS( Script, ICharacterDictatorCallbackHandler );
	REGIST_D_CLASS( Script, ICharacterMediatorCallbackHandler, ICharacterDictatorCallbackHandler );
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, Release, "" );
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, CppGetCtrlState, "(EFighterCtrlState):b" );
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, CppReborn, "" );
	//REGIST_CLASSFUNCTION( Script, CCharacterDictator, OnDoFacialAction, "Is");
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, OnSendWarningEvent, "");
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, OnSendChangeModelEvent, "");
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, OnSendExitSubMachineEvent, "")
	REGIST_CLASSFUNCTION_OVERLOAD( Script, CCharacterDictator, bool (CCharacterDictator::*)(EFighterCtrlState, bool), CppSetCtrlStateForLua, CppSetCtrlState, "(EFighterCtrlState)b:b");
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, CppGetClass, ":(EClass)" );
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, CppSetClass, "(EClass)" );
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, CppGetLevel, ":I" );
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, CppSetLevel, "I" );
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, CppGetBirthCamp, ":(ECamp)" );
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, CppSetBirthCamp, "(ECamp)" );
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, CppGetCamp, ":(ECamp)" );
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, CppSetCamp, "(ECamp)" );
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, CppGetPKState, ":b" );
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, CppSetPKState, "b" );
	//REGIST_CLASSFUNCTION( Script, CCharacterDictator, CppGetPKState, ":(EPKState)" );
	//REGIST_CLASSFUNCTION( Script, CCharacterDictator, CppSetPKState, "(EPKState)" );
	//REGIST_CLASSFUNCTION( Script, CCharacterDictator, CppGetZoneType, ":(EZoneType)" );
	//REGIST_CLASSFUNCTION( Script, CCharacterDictator, CppSetZoneType, "(EZoneType)" );
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, CppGetHP, ":f");
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, CppGetMP, ":f");
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, CppGetMaxHP, ":f");
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, CppGetMaxMP, ":f");
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, GM_CppSetPropertyValueByName, "sf");
	//REGIST_CLASSFUNCTION( Script, CCharacterDictator, CppChangePropertyValueByName, "sf");
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, GetCallbackHandler, ":(ICharacterDictatorCallbackHandler)" );
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, SetCallbackHandler, "(ICharacterDictatorCallbackHandler)" );
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, CppInit,"(EClass)I(ECamp)");
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, AddServant, "(CCharacterDictator)" );
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, AddGroupMember, "(CCharacterDictator)" );
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, SetMaster,"(CCharacterDictator)");
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, SetGroupLeader,"(CCharacterDictator)");
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, SetGroupMemberID, "I");
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, MoveToDest,"II");
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, SetOnDead,"b");
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, KillNpcBySpecialAction,"(CCharacterDictator)s");
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, ServerDoNoRuleSkill,"s(CEntityServer)");
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, ExistState,"s:b");
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, GetStateCascade,"s:I");
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, ClearState,"s");
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, EraseErasableState,"s:b");
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, CppIsLive,":b");
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, AddSkill, "sC" );
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, IsSkillActive, "s:b" );
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, RemoveSkill, "s" );
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, ClearAllSkill,"");
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, GetTarget,":(CCharacterDictator)");
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, ForceLock,"(CCharacterDictator)I");
	REGIST_CLASSFUNCTION_OVERLOAD( Script, CCharacterDictator, void (CCharacterDictator::*)(EActionState), SetAndSyncActionState,SetAndSyncActionState,"(EActionState)");
	REGIST_CLASSFUNCTION_OVERLOAD( Script, CCharacterDictator, void (CCharacterDictator::*)(uint8), SetAndSyncActionDir,SetAndSyncActionDir, "C");
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, SetDefaultWalkState,"b");
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, SetMoveRange,"I");
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, GetGridPos,"(CPos)");
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, SetGridPos,"(CPos):(ESetPosResult)");
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, GetIS, "I:(ISend)" );
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, CreateEnded, "" );
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, SetNatureInfo, "ssIsIssI")
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, GetOneActorAround, ":I");
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, OnBeginRandomMove, "");
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, GetOneNpcAround, ":I");
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, GetRandomOneAround, ":I");
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, SetTarget, "I");
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, GetNpcType, ":(ENpcType)");
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, OnBeAttacked, "I");
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, SetDeadAfterBattle, ""); 
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, IsSingTypeSkill, "s:b");
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, MoveToPathPoint, "C");
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, SetMovePath, "Cff");
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, SetMoveType, "C");
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, FacialActionHappened, "Is");
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, SetOnDisappear, "b");
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, MasterAttackCommand, "I");
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, MasterRetreatCommand, "I");
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, MasterDisbandCommand, "I");
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, MasterSpreadCommand, "I");
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, MasterOrderServantMoveCommand, "III:b");
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, IsMasterHaveServant, "I:b");
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, DestoryServant, "I");
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, GetMaster, ":(CCharacterDictator)" );
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, CppSetHP, "I");
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, IsBattleHorse, ":b" );
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, SetGridPosByTransport, "(CPos):I" );
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, SetPixelPosByTransport, "(CFPos):I" );
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, GetActionDir, ":I" );
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, OnChangeWFAniKeyFrame, "s");
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, GetKeyFrameNumOfTakeAni, ":I");
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, GetKeyFrameNumOfFireAni, ":I");
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, GetEndFrameNumOfFireAni, ":I");
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, TalkWithPlayerStart, "I" );
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, TalkWithPlayerEnd, "" );
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, ReSetNpcExistTick, "I");
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, GetNpcExistTime, ":I");
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, SetNoDissolve, "");
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, RemoveServantFromServantMgr, "I");
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, RemoveServant, "(CCharacterDictator)");
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, ChangeAIType, "(CCharacterDictator)III");
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, SynToFollowCanBeTakeOver, "");
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, SynToFollowCanNotTakeOver, "");
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, GetGroupMemberRebornPosition, "I(CFPos)" );
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, IsInitMemberPathPoint, "I:b" );
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, ReSetNpcGridAttackScope, "f");
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, RevertNpcAttackScope, "");
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, SetCanDelInBattle, "b");
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, ClearAllServantByMasterDie, "");
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, TheaterMoveToGird, "(CPos)b");
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, CheckPlayInNpcSleepDim, "");
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, SetNpcSleepState, "b");
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, BeBossType, ":b");
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, SerializeStateToDB, "I:(CStateDBDataSet)" );
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, SaveStateToDBEnd, "" );
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, LoadStateFromDB, "(CStateDBDataSet):b" );
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, HaveBeSetPath, ":b");
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, GetAITypeID, ":I");
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, GetServantByType, "(ENpcType):(CCharacterDictator)");
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, CppSetGameCamp, "i");
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, CppGetGameCamp, ":i");
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, ReSetGameCamp, "");
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, SetNpcDeadNormal, "");
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, SetNpcBirthDir, "C");
	REGIST_STATICFUNCTION( Script, CCharacterDictator, GetCharacterByID,"I:(CCharacterDictator)");
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, ChangeTruckState, "C");
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, ExistBirthAniFrame, ":b");
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, SetNpcSkillLevel, "I");
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, SetRealName, "s");
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, ClearServantType, "I");
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, GetNpcLeftLifeTime, ":i");
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, NpcMoveInPath, "");
	REGIST_CLASSFUNCTION_OVERLOAD( Script, CCharacterDictator, void (CCharacterDictator::*)(EDoSkillCtrlState, bool), CppSetDoSkillCtrlStateForLua, CppSetDoSkillCtrlState, "(EDoSkillCtrlState)b");
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, CppGetDoSkillCtrlState, "(EDoSkillCtrlState):b" );
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, GetDefence, ":I");
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, GetNatureResistance, ":I");
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, GetDestructionResistance, ":I");
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, GetEvilResistance, ":I");
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, GetTeamID, ":I");
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, GetTroopID, ":I");
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, GetTongID, ":I");
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, SetTeamID, "I");
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, SetTroopID, "I");
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, SetTongID, "I");
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, GetExpOwnerID, ":I");
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, SetDoSpecialActionProbability, "I");
	REGIST_CLASSFUNCTION( Script, CCharacterDictator, ClearAllEnmity, "");

	REGIST_STATICFUNCTION(Script, CActorCfg, GetEnumByString, "s:(EActionState)");

	//------------------- CharacterMediator ---------------------
	REGIST_CLASSFUNCTION( Script, CCharacterMediator, SetTarget,"(CCharacterDictator)");
	REGIST_CLASSFUNCTION( Script, CCharacterMediator, CppInit, "(EClass)I(ECamp)" );
	REGIST_CLASSFUNCTION( Script, CCharacterMediator, CppLevelUp, "" );
	REGIST_CLASSFUNCTION( Script, CCharacterMediator, CppInitHPMP, "II" );
	REGIST_CLASSFUNCTION( Script, CCharacterMediator, CppFullRage, "b" );
	REGIST_CLASSFUNCTION( Script, CCharacterMediator, RemoteInit,"");
	REGIST_CLASSFUNCTION( Script, CCharacterMediator, CppInitByDead, "" );
	REGIST_CLASSFUNCTION( Script, CCharacterMediator, InitSpeed,"");
	REGIST_CLASSFUNCTION( Script, CCharacterMediator, ReSendSpeedToSelf,"");
	REGIST_CLASSFUNCTION( Script, CCharacterMediator, CppInitMHNA, "sssfb");
	REGIST_CLASSFUNCTION( Script, CCharacterMediator, CppInitAHNA, "sssfb");
	REGIST_CLASSFUNCTION( Script, CCharacterMediator, CancelNormalAttack, "");
	REGIST_CLASSFUNCTION( Script, CCharacterMediator, PlayerDoItemSkill, "sI:I" );
	REGIST_CLASSFUNCTION( Script, CCharacterMediator, PlayerDoFightSkill, "sI:I" );
	REGIST_CLASSFUNCTION( Script, CCharacterMediator, PlayerDoPosSkill, "sI(CFPos):I" );
	REGIST_CLASSFUNCTION( Script, CCharacterMediator, PlayerDoFightSkillWithoutLevel, "s:I" );
	REGIST_CLASSFUNCTION( Script, CCharacterMediator, PlayerDoPosSkillWithoutLevel, "s(CFPos):I" );
	REGIST_CLASSFUNCTION( Script, CCharacterMediator, PlayerDoPassiveSkill, "sI:I" );
	REGIST_CLASSFUNCTION( Script, CCharacterMediator, AddTempSkill, "sC:b" );
	REGIST_CLASSFUNCTION_OVERLOAD( Script, CCharacterMediator, void(CCharacterMediator::*)(const TCHAR*, uint8), RemoveTempSkill,RemoveTempSkill, "sC" );
	REGIST_CLASSFUNCTION( Script, CCharacterMediator, RemoveAllTempSkill, "");
	REGIST_CLASSFUNCTION( Script, CCharacterMediator, IsAttacking, ":b" );
	REGIST_CLASSFUNCTION( Script, CCharacterMediator, DirectorMovingIsDisabled, ":b");
	REGIST_CLASSFUNCTION( Script, CCharacterMediator, InsertTalent, "sI" );
	REGIST_CLASSFUNCTION( Script, CCharacterMediator, ClearAllTalent, "" );
	REGIST_CLASSFUNCTION( Script, CCharacterMediator, GetSkillCoolDownTime, ":(CSkillCoolDownDBDataMgr)" );
	REGIST_CLASSFUNCTION( Script, CCharacterMediator, GetSkillCoolDownTimeByName,"s:I");
	REGIST_CLASSFUNCTION( Script, CCharacterMediator, SaveSkillCoolDownTimeToDBEnd, "" );
	REGIST_CLASSFUNCTION( Script, CCharacterMediator, CPPInitSkillCoolDownTime, "(CSkillCoolDownDBDataMgr)" );
	REGIST_CLASSFUNCTION( Script, CCharacterMediator, SerializeAureMagicToDB, ":(CAureMagicDBDataMgr)" );
	REGIST_CLASSFUNCTION( Script, CCharacterMediator, SaveAureMagicToDBEnd, "" );
	REGIST_CLASSFUNCTION( Script, CCharacterMediator, LoadAureMagicFromDB, "sCsI" );
	REGIST_CLASSFUNCTION( Script, CCharacterMediator, ResetAllCoolDown, "" );
	REGIST_CLASSFUNCTION( Script, CCharacterMediator, ReSendSyncInfoToSelf, "");
	REGIST_CLASSFUNCTION( Script, CCharacterMediator, ReSendSkillCoolDownTimeToSelf, "");
	REGIST_CLASSFUNCTION( Script, CCharacterMediator, ReSendHPMPToSelf, "");
	REGIST_CLASSFUNCTION( Script, CCharacterMediator, ReSendToSelf, "");
	REGIST_CLASSFUNCTION( Script, CCharacterMediator, GetConnection, ":(CConnServer)" );
	REGIST_CLASSFUNCTION( Script, CCharacterMediator, SetConnection, "(CConnServer)" );
	REGIST_CLASSFUNCTION( Script, CCharacterMediator, AddEquipSuiteEff, "sI" );
	REGIST_CLASSFUNCTION( Script, CCharacterMediator, RemoveEquipSuiteEff, "sI:b" );
	REGIST_CLASSFUNCTION( Script, CCharacterMediator, CreateBattleArray, "sss" );
	REGIST_CLASSFUNCTION( Script, CCharacterMediator, RemoteInitBattleHorse, "(CCharacterDictator)" );
	REGIST_CLASSFUNCTION( Script, CCharacterMediator, RemoteInitBattleHorseToSelf, "" );
	REGIST_CLASSFUNCTION( Script, CCharacterMediator, AddListeningWnd, "");
	REGIST_CLASSFUNCTION( Script, CCharacterMediator, DelListeningWnd, "");
	REGIST_CLASSFUNCTION( Script, CCharacterMediator, SetInGatherProcess, "b");
	REGIST_CLASSFUNCTION( Script, CCharacterMediator, OffBattleHorse, "");
	REGIST_CLASSFUNCTION( Script, CCharacterMediator, CreateBattleHorse, "IssC(CFPos)");
	REGIST_CLASSFUNCTION( Script, CCharacterMediator, CancelAutoTracking, "");
	REGIST_CLASSFUNCTION( Script, CCharacterMediator, ClearQuestBeckonNpc, "");
	REGIST_CLASSFUNCTION( Script, CCharacterMediator, GetSkillCoolDownType, "s:I");
	REGIST_CLASSFUNCTION( Script, CCharacterMediator, ChangeMoveSpeedByGMCommand, "f");
	REGIST_CLASSFUNCTION( Script, CCharacterMediator, OnStartDuel, "(CCharacterMediator)");
	REGIST_CLASSFUNCTION( Script, CCharacterMediator, OnFinishDuel, "b");
	REGIST_CLASSFUNCTION( Script, CCharacterMediator, GetBeAttackTimes, ":I");
	REGIST_CLASSFUNCTION( Script, CCharacterMediator, GetBurstSoulTimes, ":I");
	REGIST_CLASSFUNCTION( Script, CCharacterMediator, SetArmorValueRate, "fff");
	REGIST_CLASSFUNCTION( Script, CCharacterMediator, CreateTransactionID, ":I");
	REGIST_CLASSFUNCTION( Script, CCharacterMediator, DestroyTransaction, "I");
	REGIST_CLASSFUNCTION( Script, CCharacterMediator, EnableTransaction, "I");
	REGIST_CLASSFUNCTION( Script, CCharacterMediator, DisableTransaction, "I");
	REGIST_CLASSFUNCTION( Script, CCharacterMediator, CommitTransaction, "I");
	REGIST_CLASSFUNCTION( Script, CCharacterMediator, ChangeTransactionPropertyValue, "Isf");
	REGIST_CLASSFUNCTION( Script, CCharacterMediator, GetEquipPropertyValue, "s:f");
	REGIST_CLASSFUNCTION( Script, CCharacterMediator, SetBurstSoulTimes, "I");
	REGIST_CLASSFUNCTION( Script, CCharacterMediator, GetContinuousBurstTimes, ":I");
	REGIST_CLASSFUNCTION( Script, CCharacterMediator, CancelCastingProcess, "");
	REGIST_CLASSFUNCTION( Script, CCharacterMediator, SetCloseDurabilityChange, "b");
	REGIST_CLASSFUNCTION( Script, CCharacterMediator, GetCloseDurabilityChange, ":b");
	REGIST_CLASSFUNCTION( Script, CCharacterMediator, ChangeScene, "sI(CCoreSceneServer)(CFPos):b" );
	REGIST_CLASSFUNCTION( Script, CCharacterMediator, BeginStatistic, "s");
	REGIST_CLASSFUNCTION( Script, CCharacterMediator, EndStatistic, "s");
	REGIST_CLASSFUNCTION( Script, CCharacterMediator, GetStatisticFightHurt, "s:I");
	REGIST_CLASSFUNCTION( Script, CCharacterMediator, GetStatisticFightHeal, "s:I");
	REGIST_CLASSFUNCTION( Script, CCharacterMediator, GetStatisticBeSubHurt, "s:I");
	REGIST_CLASSFUNCTION( Script, CCharacterMediator, GetStatisticDps, "s:f");
	REGIST_CLASSFUNCTION( Script, CCharacterMediator, GetStatisticHps, "s:f");
	REGIST_CLASSFUNCTION( Script, CCharacterMediator, GetStatisticDeadTimes, "s:I");
	REGIST_CLASSFUNCTION( Script, CCharacterMediator, ClearAllStatisticData, "");
	REGIST_CLASSFUNCTION( Script, CCharacterMediator, TriggerSetupWeapon, "");
	REGIST_CLASSFUNCTION( Script, CCharacterMediator, TriggerChangeEquip, "");
	REGIST_CLASSFUNCTION( Script, CCharacterMediator, InitPlayerAoi, "");
	REGIST_CLASSFUNCTION( Script, CCharacterMediator, CppGetPropertyValueByName, "s:f");
	REGIST_CLASSFUNCTION( Script, CCharacterMediator, CppSuicide, "");
	REGIST_CLASSFUNCTION( Script, CCharacterMediator, GetActivityValue, ":I");
	REGIST_CLASSFUNCTION( Script, CCharacterMediator, ClearActivityValue, "");

	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterDictatorCallbackHandler, OnCreateServant, "IsIffb" );
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterDictatorCallbackHandler, OnCreateServantWithTarget, "IsIIffb" );
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterDictatorCallbackHandler, OnCreateBattleHorse, "IssIff" );
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterDictatorCallbackHandler, OnCreateNPCWithTarget, "IIIsIff" );
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterDictatorCallbackHandler, OnCreateDeadNPC, "IIff" );
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterDictatorCallbackHandler, OnCreateShadow, "IsIffb" );
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterDictatorCallbackHandler, OnDestroyNpc, "(CCharacterDictator)" );
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterDictatorCallbackHandler, OnNPCDead,"(CCharacterDictator)IIb");
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterDictatorCallbackHandler, OnDeadToQuest,"(CCharacterDictator)sII");
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterDictatorCallbackHandler, OnDeadByTick,"(CCharacterDictator)");
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterDictatorCallbackHandler, OnReaction2Facial, "IsI:b");
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterDictatorCallbackHandler, DoFacialReaction, "IsI:b");
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterDictatorCallbackHandler, HaveNature, "I:b");
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterDictatorCallbackHandler, InitNatureData, "I");
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterDictatorCallbackHandler, OnDisappear,"(CCharacterDictator)b");
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterDictatorCallbackHandler, OnLeaveBattleState,"(CCharacterDictator)");
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterDictatorCallbackHandler, OnEnterBattleState,"(CCharacterDictator)");
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterDictatorCallbackHandler, SendNpcRpcMessage, "III");
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterDictatorCallbackHandler, DoNatureAction, "Is");
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterDictatorCallbackHandler, ReplaceNpc, "(CCharacterDictator)(CCharacterDictator)sssI" );
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterDictatorCallbackHandler, MoveEndMessage, "(CCharacterDictator)" );
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterDictatorCallbackHandler, ChangeDir, "III" );
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterDictatorCallbackHandler, SetDrivePig, "II" );
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterDictatorCallbackHandler, DestoryObj, "I");
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterDictatorCallbackHandler, SetNpcAlphaValue, "IC");
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterDictatorCallbackHandler, SetNpcCanBeTakeOver, "III(ECamp)");
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterDictatorCallbackHandler, CreateServantSkillToMaster, "(CCharacterDictator)s");
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterDictatorCallbackHandler, DelServantSkillFromMaster, "(CCharacterDictator)s");
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterDictatorCallbackHandler, MasterReBornCortege, "II");
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterDictatorCallbackHandler, MasterRevertNpcGroup, "I");
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterDictatorCallbackHandler, NpcShowContentBySkill, "(CCharacterDictator)sI");
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterDictatorCallbackHandler, BecomeTargetShadow, "(CCharacterDictator)(CCharacterDictator)");
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterDictatorCallbackHandler, TakeOverTruckByNewMaster, "(CCharacterDictator)(CCharacterDictator)");
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterDictatorCallbackHandler, ClearNpcAlertEffect, "(CCharacterDictator)");
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterDictatorCallbackHandler, ReplaceTruck, "(CCharacterDictator)(CCharacterDictator)");
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterDictatorCallbackHandler, PosCreateNpcWithMaster, "Isff");

	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterMediatorCallbackHandler, OnDestroyed, "(CCharacterMediator)" );
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterMediatorCallbackHandler, OnSetLockObj, "II" );
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterMediatorCallbackHandler, OnDead, "(CCharacterDictator)(CCharacterDictator)" );
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterMediatorCallbackHandler, OnDoNonCombatStateMagicOp, "sdII" );
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterMediatorCallbackHandler, OnCreateTempSkill, "IsCfI" );
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterMediatorCallbackHandler, OnRemoveTempSkill, "II" );
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterMediatorCallbackHandler, OnCastTempSkill, "II" );
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterMediatorCallbackHandler, OnCancelNonCombatStateMagicOp, "sdII" );
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterMediatorCallbackHandler, ListeningOpenWnd, "(CCharacterDictator)" );
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterMediatorCallbackHandler, OnRidingHorseChanged, "Iss" );
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterMediatorCallbackHandler, OnForbitUseWeapon, "Ib" );
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterMediatorCallbackHandler, OnTransfered, "(CCharacterDictator)(CCoreSceneServer)ff(CCoreSceneServer)" );
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterMediatorCallbackHandler, OnBreakGatherProcess, "I" );
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterMediatorCallbackHandler, OnItemSkillEnd,"Is");
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterMediatorCallbackHandler, OnBurstSoulPrize,"IIf");
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterMediatorCallbackHandler, OnChangeModulus,"IIf");
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterMediatorCallbackHandler, OnPermitReborn,"II");
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterMediatorCallbackHandler, OnChangeScene,"IsII");
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterMediatorCallbackHandler, OnFinishDuel, "(CCharacterMediator)b");
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterMediatorCallbackHandler, OnAttackTimesChanged, "II");
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterMediatorCallbackHandler, DoAlertPhaseEffect, "(CCharacterMediator)(CCharacterDictator)I");
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterMediatorCallbackHandler, DoAlertEndEffect, "(CCharacterMediator)(CCharacterDictator)");
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterMediatorCallbackHandler, OnMasterDisbandTruck, "(CCharacterMediator)(CCharacterDictator)");
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterMediatorCallbackHandler, OnMasterDestoryTruck, "(CCharacterMediator)(CCharacterDictator)");
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterMediatorCallbackHandler, SaveServantInfo, "(CCharacterMediator)");
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterMediatorCallbackHandler, SaveServantInfoOnServantChange, "(CCharacterMediator)");
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterMediatorCallbackHandler, BeClearServantByTransfer, "s:b");
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterMediatorCallbackHandler, InSceneArea, "(CCharacterMediator)s:b");
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterMediatorCallbackHandler, OnPlayerEnterBattleState, "(CCharacterMediator)");
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterMediatorCallbackHandler, OnPlayerLeaveBattleState, "(CCharacterMediator)");
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterMediatorCallbackHandler, OnLogSpeedChangeReason, "(CCharacterMediator)fs");
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterMediatorCallbackHandler, OnDoWorldSkill, "(CCharacterMediator)sI");
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterMediatorCallbackHandler, OnDoCampSkill, "(CCharacterMediator)sI");
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterMediatorCallbackHandler, OnDoArmyCorpsSkill, "(CCharacterMediator)sI");
	REGIST_PURE_CALLBACKFUNCTION( Script, ICharacterMediatorCallbackHandler, OnDoTongSkill, "(CCharacterMediator)sI");

	//------------------- CIntObjServer ---------------------
	REGIST_ABSTRACT_D_CLASS( Script, CIntObjServer, CEntityServer);
	REGIST_B_CLASS( Script, IIntObjCallbackHandlerServer );

	REGIST_CLASSFUNCTION( Script, CIntObjServer, Release, "" );
	REGIST_CLASSFUNCTION( Script, CIntObjServer, GetCallbackHandler, ":(IIntObjCallbackHandlerServer)" );
	REGIST_CLASSFUNCTION( Script, CIntObjServer, SetCallbackHandler, "(IIntObjCallbackHandlerServer)" );
	REGIST_CLASSFUNCTION( Script, CIntObjServer, GetGridPos,"(CPos)");
	REGIST_CLASSFUNCTION( Script, CIntObjServer, SetOwner,"(CCharacterDictator)");
	REGIST_CLASSFUNCTION( Script, CIntObjServer, GetOwner,":(CCharacterDictator)");
	REGIST_CLASSFUNCTION( Script, CIntObjServer, GetActionDir, ":C");
	REGIST_CLASSFUNCTION( Script, CIntObjServer, SetObjName, "s");
	REGIST_STATICFUNCTION(Script, CIntObjServer, GetIntObjServer,"(CCoreObjectServer):(CIntObjServer)");
	REGIST_STATICFUNCTION(Script, CIntObjServer, GetIntObjServerByID,"I:(CIntObjServer)");
	REGIST_CLASSFUNCTION_OVERLOAD( Script, CIntObjServer, void (CIntObjServer::*)(uint8), SetAndSyncActionDir,SetAndSyncActionDir, "C");
	REGIST_CLASSFUNCTION( Script, CIntObjServer, DoPosIntObjSkill,"sI(CFPos)");
	REGIST_CLASSFUNCTION( Script, CIntObjServer, DoIntObjSkill,"sI(CEntityServer)");
	REGIST_CLASSFUNCTION( Script, CIntObjServer, SetIntObjSeeType, "I");
	REGIST_CLASSFUNCTION( Script, CIntObjServer, InsertNpcSet, "s");
	REGIST_CLASSFUNCTION( Script, CIntObjServer, OnSetNpcArgEnd, "");

	REGIST_CALLBACKFUNCTION( Script, IIntObjCallbackHandlerServer, OnSetLockObj, "II" );
	REGIST_CALLBACKFUNCTION( Script, IIntObjCallbackHandlerServer, OnDestroyObj, "I");
	REGIST_CALLBACKFUNCTION( Script, IIntObjCallbackHandlerServer, OnIntoTrapViewAoi, "I(CIntObjServer)(CCharacterDictator)" );
	REGIST_CALLBACKFUNCTION( Script, IIntObjCallbackHandlerServer, OnLeaveTrapViewAoi, "I(CIntObjServer)(CCharacterDictator)");
	REGIST_CALLBACKFUNCTION( Script, IIntObjCallbackHandlerServer, OnObjAoiMsgFromSelf, "I(CIntObjServer)");

	//------------------- EntityCreator ----------------------
	REGIST_STATICFUNCTION( Script, CEntityServerCreator, CreateCharacterDictator, "(CCoreSceneServer)(CFPos)ssb:(CCharacterDictator)");
	REGIST_STATICFUNCTION( Script, CEntityServerCreator, CreateCharacterDictatorWithMaster, "(CCharacterDictator)ss:(CCharacterDictator)");
	REGIST_STATICFUNCTION( Script, CEntityServerCreator, CreateCharacterMediator, "(CCoreSceneServer)(CFPos)s:(CCharacterMediator)");
	REGIST_STATICFUNCTION( Script, CEntityServerCreator, CreateServerIntObject, "(CCoreSceneServer)(CFPos)ss:(CIntObjServer)");
	REGIST_STATICFUNCTION( Script, CEntityServerCreator, DestroyEntity, "(CEntityServer)");

	//------------------- CheckSkill --------------------------
	REGIST_STATICFUNCTION( Script, CheckSkill,TestInSkillList, "s:b");

	//------------------- NpcCreator ----------------------
	REGIST_STATICFUNCTION( Script, CNpcServerCreator, CreateServerNpcAI, "(CCharacterDictator)sIss(EClass)(ECamp)sI");

	//------------------- NpcInfoMgr ---------------------------------------
	REGIST_STATICFUNCTION( Script, NpcInfoMgr, BeActiveNpc, "I:b");
	REGIST_STATICFUNCTION( Script, NpcInfoMgr, GetAINameByAIType, "I:s");
	REGIST_STATICFUNCTION( Script, NpcInfoMgr, GetTypeNameByType, "I:s");
	REGIST_STATICFUNCTION( Script, NpcInfoMgr, BeServantType, "I:b");
	REGIST_STATICFUNCTION( Script, NpcInfoMgr, GetAITypeByAIName, "s:I");
	REGIST_STATICFUNCTION( Script, NpcInfoMgr, GetTypeByTypeName, "s:I");
	REGIST_STATICFUNCTION( Script, NpcInfoMgr, CanTakeOnlyOne, "I:b");

	//------------------ NpcServerBaseData ----------------
	REGIST_ABSTRACT_B_CLASS( Script, CNpcServerBaseData );
	REGIST_STATICFUNCTION( Script, CNpcServerBaseData, Create, "ssssIIII:(CNpcServerBaseData)");
	REGIST_CLASSFUNCTION( Script, CNpcServerBaseData, SetCanBeChangeAI, "b");
	REGIST_CLASSFUNCTION( Script, CNpcServerBaseData, SetBeSleep, "b");
	REGIST_CLASSFUNCTION( Script, CNpcServerBaseData, SetBeSynToClient, "b");
	REGIST_CLASSFUNCTION( Script, CNpcServerBaseData, SetCanBeSelected, "b");
	REGIST_CLASSFUNCTION( Script, CNpcServerBaseData, SetCanBeRavin, "b");

	//-----------------  CNpcResServerData  -----------------------
	REGIST_ABSTRACT_B_CLASS(Script, CNpcResServerData);
	REGIST_STATICFUNCTION( Script, CNpcResServerData, Create, "sffbs:(CNpcResServerData)");
	REGIST_STATICFUNCTION( Script, CNpcResServerData, CheckNpcAniKeyFrameCfg, ":b");

	//-------------------- IGameGridRadiusCallback ----------------
	REGIST_B_CLASS( Script, IGameGridRadiusCallback);
	REGIST_CLASSFUNCTION( Script, IGameGridRadiusCallback, ForEachInRadius, "(CCoreSceneServer)(CPos)f");
	REGIST_CLASSFUNCTION( Script, IGameGridRadiusCallback, ForEachMediatorInRadius, "(CCoreSceneServer)(CPos)f");
	REGIST_CALLBACKFUNCTION( Script, IGameGridRadiusCallback, Exec, "(CEntityServer)"); 

	//--------------------- IServantQueryCallback -------------------
	REGIST_B_CLASS(Script, IServantQueryCallback);
	REGIST_CLASSFUNCTION(Script, IServantQueryCallback, QueryServantJob, "(CCharacterDictator)");
	REGIST_CALLBACKFUNCTION(Script, IServantQueryCallback, Exec, "I(CCharacterDictator)");

	//---------------------  INpcEnmityQueryCallback ---------------------
	REGIST_B_CLASS(Script, INpcEnmityQueryCallback);
	REGIST_CLASSFUNCTION(Script, INpcEnmityQueryCallback, QueryEnemyJob, "(CCharacterDictator)");
	REGIST_CALLBACKFUNCTION(Script, INpcEnmityQueryCallback, Exec, "I(CCharacterDictator)");

	REGIST_STATICFUNCTION(Script, CEntityServerManager, GetEntityByID, "I:(CEntityServer)");
	REGIST_STATICFUNCTION(Script, CEntityServerManager, CreateInst, "");
	REGIST_STATICFUNCTION(Script, CEntityServerManager, DestroyInst, "");

	REGIST_STATICFUNCTION(Script, CTestBulletMagic, StartTest, "I:b");
	REGIST_STATICFUNCTION(Script, CTestBulletMagic, ShutDownTest, ":b");
	REGIST_STATICFUNCTION(Script, CTestShockWaveMagic, StartTest, ":b");
	REGIST_STATICFUNCTION(Script, CTestShockWaveMagic, ShutDownTest, ":b");
	REGIST_STATICFUNCTION(Script, CTestHurtResult, CalPhysicsProbability, "");
	REGIST_STATICFUNCTION(Script, CTestHurtResult, CalMagicProbability, "");

	REGIST_STATICFUNCTION(Script, CTeamServerManager, CppAddTeamMember, "I(ETeamType)I");
	REGIST_STATICFUNCTION(Script, CTeamServerManager, CppRemoveTeamMember, "I(ETeamType)I");
	REGIST_STATICFUNCTION(Script, CTeamServerManager, CppCreateTeam, "I(ETeamType)I:b");
	REGIST_STATICFUNCTION(Script, CTeamServerManager, CppDestroyTeam, "I(ETeamType)");
	REGIST_STATICFUNCTION(Script, CTeamServerManager, CppChangeTeamMaster, "I(ETeamType)I");
	REGIST_STATICFUNCTION(Script, CTeamServerManager, CppDestroyTeamManager, "");

	//REGIST_ABSTRACT_B_CLASS(Script, CFightSkillProf);
	//REGIST_STATICFUNCTION(Script, CFightSkillProf, GetProf, ":(CFightSkillProf)");
	//REGIST_CLASSFUNCTION(Script, CFightSkillProf, TurnOnFightSkillProf, "I");
	//REGIST_CLASSFUNCTION(Script, CFightSkillProf, TurnOffFightSkillProf, "");

	REGIST_STATICFUNCTION(Script, CAoiDimFilter, DimensionInfo, ":(CDimensionInfo)" );
	REGIST_STATICFUNCTION(Script, CAoiDimFilter, Unload, "" );

	REGIST_ENUMTYPE(Script, ENpcMoveType);
	REGIST_ENUMERATION(Script, ENpcMoveType, ENpcMove_None);
	REGIST_ENUMERATION(Script, ENpcMoveType, ENpcMove_Cycle);
	REGIST_ENUMERATION(Script, ENpcMoveType, ENpcMove_Trip);
	REGIST_ENUMERATION(Script, ENpcMoveType, ENpcMove_Random);
	REGIST_ENUMERATION(Script, ENpcMoveType, ENpcMove_SingleLine);
	REGIST_ENUMERATION(Script, ENpcMoveType, ENpcMove_Single2Random);
	REGIST_ENUMERATION(Script, ENpcMoveType, ENpcMove_MoveInPath);

	REGIST_STATICFUNCTION(Script, CCpuUsageMgr, GetSystemCpuUsage, "I:i" );
	REGIST_STATICFUNCTION(Script, CThreadCpuUsageMgr, GetTopThreadCpuUsage, "I:i" );

	REGIST_D_CLASS( Script, CPlayerSet,CMultiMsgSender);
	REGIST_CLASSFUNCTION( Script, CPlayerSet, AddPlayer, "(CCharacterMediator)");
	REGIST_CLASSFUNCTION( Script, CPlayerSet, DelPlayer, "(CCharacterMediator)");
	REGIST_CLASSFUNCTION( Script, CPlayerSet, DoSkill, "sIs");

	//---------------------- WeiBo --------------------------
	//REGIST_B_CLASS(Script, CWeiBoServer);
	//REGIST_CLASSFUNCTION(Script, CWeiBoServer, RequestStart, "" );
	//REGIST_CLASSFUNCTION(Script, CWeiBoServer, GetAccessToken, "s" );
	//REGIST_CALLBACKFUNCTION(Script, CWeiBoServer, OpenWebBrowser, "s" );
	//REGIST_CALLBACKFUNCTION(Script, CWeiBoServer, WeiBoLogin, "s" );
	//REGIST_CALLBACKFUNCTION(Script, CWeiBoServer, LoginFaild, "" );
}

