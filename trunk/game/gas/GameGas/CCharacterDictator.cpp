#include "stdafx.h"
#include "CCharacterMediator.h"
#include "ICharacterMediatorHandler.h"
#include "PtrlGas2GacCPPDef.h"
#include "TEntityTraitsServer.h"
#include "CCharacterDictator.inl"
#include "CSyncVariantServer.h"
#include "ICharacterDictatorHandler.h"
#include "CMemberAI.h"
#include "CDummyAI.h"
#include "CNpcGroupMgr.h"
#include "CAllStateMgr.h"
#include "TEntityAdapterServer.h"
#include "TPropertyHolder.inl"
#include "ICharacterDictatorCallbackHandler.h"
#include "CFighterDictator.h"
#include "CGenericTarget.h"
#include "TCharacterAdapterServer.h"
#include "CActorServer.h"
#include "CNpcQueueMgr.h"
#include "IActorServerHandler.h"
#include "CEntityServer.inl"
#include "TCharacterAdapterServer.inl"
#include "TEntityAdapterServer.inl"
#include "IFighterServerHandler.h"
#include "CServantAI.h"
#include "CTriggerEvent.h"
#include "CAniKeyFrameCfg.h"
#include "CSkillServer.h"
#include "ErrLogHelper.h"
#include "CCoreSceneServer.h"
#include "PatternRef.inl"
#include "CoreCommon.h"
#include "NpcInfoMgr.h"
#include "TServantMgr.inl"
#include "CServantServerMgr.h"
#include "CStateSync.h"
#include "FighterProperty.inl"
#include "TSqrAllocator.inl"
#include "CTruckAI.h"
#include "CCastingProcessCfg.h"
#include "Random.h"
#include "CNpcChaserQueueMgr.h"

template class TPtRefer<CCharacterDictator,CCharacterDictator>;

CCharacterDictator::CCharacterDictator()
:m_eNpcType(ENpcType_None)
,m_strHorseName("")
,m_strRealName("")
,m_uSkillLevel(0)
,m_sNpcName("")
,m_bCanBeTakeOver(false)
,m_pServantMgr(NULL)
,m_pGroupMgr(NULL)
,m_pNpcQueueMgr(NULL) 
,m_NpcChaserQueueMgr(NULL)
{
	m_RefsByIntObj.SetHolder(this);
	m_RefsByCharacter.SetHolder(this);
	m_pMaster.SetHolder(this);
	m_pGroupLeader.SetHolder(this);
	m_RefsByNpcAI.SetHolder(this);
	m_RefsByGroupMgr.SetHolder(this);
	m_CallbackHandler.SetHolder(this);
	m_NpcAIHandler.SetHolder(this);
}

//Npc的构造函数
CCharacterDictator::CCharacterDictator(CCoreObjectDictator *pCoreObj)
:m_eNpcType(ENpcType_None)
,m_strHorseName("")
,m_strRealName("")
,m_uSkillLevel(0)
,m_sNpcName("")
,m_bCanBeTakeOver(false)
,m_pServantMgr(NULL)
,m_pGroupMgr(NULL)
,m_pNpcQueueMgr(NULL)
,m_NpcChaserQueueMgr(NULL)
{
	Init<CCharacterDictatorTraits>(pCoreObj);
	m_RefsByIntObj.SetHolder(this);
	m_RefsByCharacter.SetHolder(this);
	m_pMaster.SetHolder(this);
	m_pGroupLeader.SetHolder(this);
	m_RefsByNpcAI.SetHolder(this);
	m_RefsByGroupMgr.SetHolder(this);
	m_CallbackHandler.SetHolder(this);
	m_NpcAIHandler.SetHolder(this);
	ClearAllChaser();
}

CCharacterDictator::~CCharacterDictator()
{
	ClearAllChaser();
	m_pGroupLeader.Detach();
	const_cast<CEntityServerManager*>(CEntityServerManager::GetInst())->RemoveGameEntity(this);
	AdvDelRef(GetCallbackHandler());
	SetCallbackHandler(NULL);
}

void CCharacterDictator::InitNpcVariant()
{
	CSyncVariantServer* pViewVariantAttribute = GetViewVariant()->SetMember("Attribute");
	(m_pViewVariantAttribute_EntityID = pViewVariantAttribute->SetMember("EntityID"))
		->SetNumber((uint32)GetEntityID());
	(m_pViewVariantAttribute_Type = pViewVariantAttribute->SetMember("Type"))
		->SetNumber((uint8)eCT_Npc);
	(m_pViewVariantAttribute_Camp = pViewVariantAttribute->SetMember("Camp"))
		->SetNumber((uint8)eCamp_None);
	(m_pViewVariantAttribute_BirthCamp = pViewVariantAttribute->SetMember("BirthCamp"))
		->SetNumber((uint8)eCamp_None);
	(m_pViewVariantAttribute_GameCamp = pViewVariantAttribute->SetMember("GameCamp"))
		->SetNumber((int32)0);
	(m_pViewVariantAttribute_Level = pViewVariantAttribute->SetMember("Level"))
		->SetNumber((uint8)1);
	(m_pViewVariantAttribute_Class = pViewVariantAttribute->SetMember("Class"))
		->SetNumber((uint8)eCL_NoneClass);
	(m_pViewVariantAttribute_NpcType = pViewVariantAttribute->SetMember("NpcType"))
		->SetNumber((uint8)ENpcType_None);
	(m_pViewVariantAttribute_NpcAIType = pViewVariantAttribute->SetMember("NpcAIType"))
		->SetNumber((uint8)ENpcAIType_None);
	(m_pViewVariantAttribute_BeActiveNpc = pViewVariantAttribute->SetMember("BeActiveNpc"))
		->SetNumber((bool)false);
	(m_pViewVariantAttribute_AttackType = pViewVariantAttribute->SetMember("AttackType"))
		->SetNumber((uint8)eATT_None);
	(m_pViewVariantAttribute_RealName = pViewVariantAttribute->SetMember("RealName"))
		->SetString("");
	(m_pViewVariantAttribute_ShowInClient = pViewVariantAttribute->SetMember("ShowInClient"))
		->SetNumber((bool)true);
	(m_pViewVariantAttribute_FightDir = pViewVariantAttribute->SetMember("FightDir"))
		->SetNumber((bool)false);
	
	CSyncVariantServer*	pViewVariantPKSwitch = GetViewVariant(eCTI_Agile)->SetMember("PKSwitch");
	(m_pViewVariantPKSwitch_PKSwitchType = pViewVariantPKSwitch->SetMember("PKSwitchType"))
		->SetNumber(false);
	//(m_pViewVariantPKSwitch_PKSwitchType = pViewVariantPKSwitch->SetMember("PKSwitchType"))
	//	->SetNumber((uint8)ePKS_Safety);
	//(m_pViewVariantPKSwitch_ZoneType = pViewVariantPKSwitch->SetMember("ZoneType"))
	//	->SetNumber((uint8)eZT_CommonZone);
	CSyncVariantServer* pSyncVariantAttribute1 = GetSyncVariant(eCTI_Agile)->SetMember("Attribute1");
	(m_pSyncVariantAttribute1_TargetID = pSyncVariantAttribute1->SetMember("TargetID"))
		->SetNumber((uint32)0);
	(m_pSyncVariantAttribute1_Speed = pSyncVariantAttribute1->SetMember("Speed"))
		->SetNumber((float)0.0f);
	(m_pSyncVariantAttribute1_ExpOwnerID = pSyncVariantAttribute1->SetMember("ExpOwnerID"))
		->SetNumber((uint32)0);
	m_pSyncVariantAttribute1_Stealth = NULL;

	CSyncVariantServer* pSyncVariantAlert = GetSyncVariant(eCTI_Agile)->SetMember("Alert");
	(m_pSyncVariantAlert_AlertTargetID = pSyncVariantAlert->SetMember("AlertTargetID"))
		->SetNumber((uint32)0);
	(m_pSyncVariantAlert_AlertAction = pSyncVariantAlert->SetMember("AlertAction"))
		->SetNumber((bool)false);

	CSyncVariantServer*	pViewVariantMaster = GetViewVariant(eCTI_Agile)->SetMember("Master");
	(m_pViewVariantMaster_MasterID = pViewVariantMaster->SetMember("MasterID"))
		->SetNumber((uint32)0);
	(m_pViewVariantMaster_MasterTeamID = pViewVariantMaster->SetMember("MasterTeamID"))
		->SetNumber((uint32)0);
	(m_pViewVariantMaster_MasterTroopID = pViewVariantMaster->SetMember("MasterTroopID"))
		->SetNumber((uint32)0);
	(m_pViewVariantMaster_MasterTongID = pViewVariantMaster->SetMember("MasterTongID"))
		->SetNumber((uint32)0);
}

void CCharacterDictator::InitPlayerVariant()
{
	CSyncVariantServer* pViewVariantAttribute = GetViewVariant(eCTI_Static)->SetMember("Attribute");
	(m_pViewVariantAttribute_EntityID = pViewVariantAttribute->SetMember("EntityID"))
		->SetNumber((uint32)GetEntityID());
	(m_pViewVariantAttribute_Type = pViewVariantAttribute->SetMember("Type"))
		->SetNumber((uint8)eCT_Player);
	(m_pViewVariantAttribute_Camp = pViewVariantAttribute->SetMember("Camp"))
		->SetNumber((uint8)eCamp_None);
	(m_pViewVariantAttribute_BirthCamp = pViewVariantAttribute->SetMember("BirthCamp"))
		->SetNumber((uint8)eCamp_None);
	(m_pViewVariantAttribute_GameCamp = pViewVariantAttribute->SetMember("GameCamp"))
		->SetNumber((int32)0);
	(m_pViewVariantAttribute_Level = pViewVariantAttribute->SetMember("Level"))
		->SetNumber((uint8)1);
	(m_pViewVariantAttribute_Class = pViewVariantAttribute->SetMember("Class"))
		->SetNumber((uint8)eCL_NoneClass);

	m_pViewVariantAttribute_NpcType = NULL;
	m_pViewVariantAttribute_NpcAIType = NULL;
	m_pViewVariantAttribute_BeActiveNpc = NULL;
	m_pViewVariantAttribute_AttackType = NULL;
	m_pViewVariantAttribute_RealName = NULL;
	m_pViewVariantAttribute_ShowInClient = NULL;
	m_pViewVariantAttribute_FightDir = NULL;
	
	CSyncVariantServer*	pViewVariantPKSwitch = GetViewVariant(eCTI_Agile)->SetMember("PKSwitch");
	(m_pViewVariantPKSwitch_PKSwitchType = pViewVariantPKSwitch->SetMember("PKSwitchType"))
		->SetNumber(false);
	//(m_pViewVariantPKSwitch_PKSwitchType = pViewVariantPKSwitch->SetMember("PKSwitchType"))
	//	->SetNumber((uint8)ePKS_Safety);
	//(m_pViewVariantPKSwitch_ZoneType = pViewVariantPKSwitch->SetMember("ZoneType"))
	//	->SetNumber((uint8)eZT_CommonZone);

	CSyncVariantServer* pSyncVariantAttribute1 = GetSyncVariant(eCTI_Agile)->SetMember("Attribute1");
	(m_pSyncVariantAttribute1_TargetID = pSyncVariantAttribute1->SetMember("TargetID"))
		->SetNumber((uint32)0);
	(m_pSyncVariantAttribute1_Speed = pSyncVariantAttribute1->SetMember("Speed"))
		->SetNumber((float)0.0f);
	(m_pSyncVariantAttribute1_Stealth = pSyncVariantAttribute1->SetMember("Stealth"))
		->SetNumber((bool)false);

	m_pSyncVariantAlert_AlertTargetID = NULL;
	m_pSyncVariantAlert_AlertAction = NULL;
	m_pSyncVariantAttribute1_ExpOwnerID = NULL;

	m_pViewVariantMaster_MasterID = NULL;
	m_pViewVariantMaster_MasterTeamID = NULL;
	m_pViewVariantMaster_MasterTroopID = NULL;
	m_pViewVariantMaster_MasterTongID = NULL;
}

void CCharacterDictator::CppInit(EClass eClass, uint32 uLevel, ECamp eCamp)
{
	Ast(GetFighter());
	GetFighter()->CppInit(eClass, uLevel,eCamp);
	Ast(GetActor());
	GetActor()->InitActorVariant();
}

void CCharacterDictator::TheaterMoveToGird(const CPos& gPos, bool bWalk)
{
	Ast (GetNpcAIHandler())
	GetNpcAIHandler()->CastToNpcAI()->TheaterMoveToGird(gPos, bWalk);
}

void CCharacterDictator::NpcMoveToPixel(const CFPos& fPos, bool bWalk)
{
	Ast (GetNpcAIHandler())
	GetNpcAIHandler()->CastToNpcAI()->MoveToPixel(fPos, bWalk,0);
}

void CCharacterDictator::NpcMoveInPath()
{
	Ast (GetNpcAIHandler());
	GetNpcAIHandler()->NpcMoveInPath();
}	

void CCharacterDictator::SetTeamID(uint32 uTeamID)const
{
	CSyncVariantServer* pViewVariantAttribute = GetViewVariant()->SetMember("Attribute");
	pViewVariantAttribute->SetMember("TeamID")->SetNumber((uint32)uTeamID);
	if(GetServantMgr())	
	{
		const CServantServerMgr::ServantIDMapType& ServantMap = GetServantMgr()->GetServantIDMapType();
		CServantServerMgr::ServantIDMapType::const_iterator it = ServantMap.begin();
		for(;it!=ServantMap.end();++it)
		{
			uint32 uEntityID = it->first;
			CCharacterDictator* pServant = CCharacterDictator::GetCharacterByID(uEntityID);
			if (pServant)
				pServant->GetMasterTeamIDVariant()->SetNumber((uint32)uTeamID);
		}
	}
}

uint32 CCharacterDictator::GetTeamID()const
{
	CSyncVariantServer* pVariant= GetViewVariant();
	if (!pVariant)
		return 0;
	CSyncVariantServer* pAttributeVariant=pVariant->GetMember("Attribute");
	if (pAttributeVariant->IsEmptyTable())
		return 0;
	CSyncVariantServer* pTeamIDVariant=pAttributeVariant->GetMember("TeamID");
	if (!pTeamIDVariant)
		return 0;
	return pTeamIDVariant->GetNumber<uint32>();
}

void CCharacterDictator::SetTroopID(uint32 uTroopID)const
{
	CSyncVariantServer* pViewVariantAttribute = GetViewVariant()->SetMember("Attribute");
	pViewVariantAttribute->SetMember("TroopID")->SetNumber((uint32)uTroopID);
	if(GetServantMgr())	
	{
		const CServantServerMgr::ServantIDMapType& ServantMap = GetServantMgr()->GetServantIDMapType();
		CServantServerMgr::ServantIDMapType::const_iterator it = ServantMap.begin();
		for(;it!=ServantMap.end();++it)
		{
			uint32 uEntityID = it->first;
			CCharacterDictator* pServant = CCharacterDictator::GetCharacterByID(uEntityID);
			if (pServant)
				pServant->GetMasterTroopIDVariant()->SetNumber((uint32)uTroopID);
		}
	}
}

uint32 CCharacterDictator::GetTroopID()const
{
	CSyncVariantServer* pVariant= GetViewVariant();
	if (!pVariant)
		return 0;
	CSyncVariantServer* pAttributeVariant=pVariant->GetMember("Attribute");
	if (pAttributeVariant->IsEmptyTable())
		return 0;
	CSyncVariantServer* pTeamIDVariant=pAttributeVariant->GetMember("TroopID");
	if (!pTeamIDVariant)
		return 0;
	return pTeamIDVariant->GetNumber<uint32>();
}

void CCharacterDictator::SetTongID(uint32 uTongID) const
{
	CSyncVariantServer* pViewVariantAttribute = GetViewVariant()->SetMember("Attribute");
	pViewVariantAttribute->SetMember("TongID")->SetNumber((uint32)uTongID);
	if(GetServantMgr())	
	{
		const CServantServerMgr::ServantIDMapType& ServantMap = GetServantMgr()->GetServantIDMapType();
		CServantServerMgr::ServantIDMapType::const_iterator it = ServantMap.begin();
		for(;it!=ServantMap.end();++it)
		{
			uint32 uEntityID = it->first;
			CCharacterDictator* pServant = CCharacterDictator::GetCharacterByID(uEntityID);
			if (pServant)
				pServant->GetMasterTongIDVariant()->SetNumber((uint32)uTongID);
		}
	}
}

uint32 CCharacterDictator::GetTongID() const
{
	CSyncVariantServer* pVariant= GetViewVariant();
	if (!pVariant)
		return 0;
	CSyncVariantServer* pAttributeVariant=pVariant->GetMember("Attribute");
	if (pAttributeVariant->IsEmptyTable())
		return 0;
	CSyncVariantServer* pTongIDVariant=pAttributeVariant->GetMember("TongID");
	if (!pTongIDVariant)
		return 0;
	return pTongIDVariant->GetNumber<uint32>();
}

uint32 CCharacterDictator::GetMasterTongID() const
{
	CSyncVariantServer* pVariant= GetViewVariant();
	if (!pVariant)
		return 0;
	CSyncVariantServer* pAttributeVariant=pVariant->GetMember("Attribute");
	if (pAttributeVariant->IsEmptyTable())
		return 0;
	CSyncVariantServer* pMasterToonIDVaraint = GetMasterTongIDVariant();
	if(!pMasterToonIDVaraint)
	{
		return 0;
	}
	return pMasterToonIDVaraint->GetNumber<uint32>();
}

// About Actor							 
void CCharacterDictator::OnChangeWFAniKeyFrame(const TCHAR* szAniFileName)
{
	CActorServer* pActor = GetActor();
	Ast(pActor);
	pActor->OnChangeWFAniKeyFrame(szAniFileName);
}

uint32 CCharacterDictator::GetKeyFrameNumOfTakeAni()
{
	CActorServer* pActor = GetActor();
	Ast(pActor);
	return (uint32)pActor->GetHitFrameByActionState(eAS_Take);
}

uint32 CCharacterDictator::GetKeyFrameNumOfFireAni()
{
	CActorServer* pActor = GetActor();
	Ast(pActor);
	return (uint32)pActor->GetHitFrameByActionState(eAS_Fire);
}

uint32 CCharacterDictator::GetEndFrameNumOfFireAni()
{
	CActorServer* pActor = GetActor();
	Ast(pActor);
	return (uint32)pActor->GetEndFrameByActionState(eAS_Fire);
}

void CCharacterDictator::SetWalkBackRadius(uint8 uWalkBackRadius)
{
	CActorServer* pActor = GetActor();
	Ast(pActor);
	pActor->GetVariantWalkBackRadius()->SetNumber(uWalkBackRadius);
}

uint8 CCharacterDictator::GetWalkBackRadius() const
{
	CActorServer* pActor = GetActor();
	Ast(pActor);
	return pActor->GetVariantWalkBackRadius()->GetNumber<uint8>();
}

bool CCharacterDictator::SetAndSyncMoveState(EMoveState eMoveState, bool bForce)
{
	CActorServer* pActor = GetActor();
	Ast(pActor);
	if (!pActor->ChangeMoveState(eMoveState, bForce))
		return false;

	pActor->GetVariantStateForce()->SetNumber(bForce);
	pActor->GetVariantMoveState()->SetNumber((uint8)eMoveState);
	return true;
}

EMoveState CCharacterDictator::GetMoveState()const
{
	CActorServer* pActor = GetActor();
	Ast(pActor);
	return EMoveState(pActor->GetVariantMoveState()->GetNumber<uint8>());
}

void CCharacterDictator::SetAndSyncMoveDir(uint8 uMoveDir)
{
	CActorServer* pActor = GetActor();
	Ast(pActor);
	pActor->GetVariantMoveDir()->SetNumber(uMoveDir);
}

uint8 CCharacterDictator::GetMoveDir()const
{
	CActorServer* pActor = GetActor();
	Ast(pActor);
	return pActor->GetVariantMoveDir()->GetNumber<uint8>();
}

void CCharacterDictator::SetAndSyncActionState(EActionState eActionState, float fNARhythm, EHurtResult eHurtResult)
{
	CActorServer* pActor = GetActor();
	Ast(pActor);
	pActor->ChangeActionState(eActionState);

	pActor->GetVariantActionRhythm()->SetNumber(fNARhythm);
	//pActor->GetVariantActionIndex()->SetNumber(pActor->GetActionIndex());
	pActor->GetVariantHurtResult()->SetNumber((uint8)eHurtResult);
	pActor->GetVariantActionState()->SetNumber((uint8)eActionState);
}

EActionState CCharacterDictator::GetActionState()const
{
	CActorServer* pActor = GetActor();
	Ast(pActor);
	return EActionState(pActor->GetVariantActionState()->GetNumber<uint8>());
}

void CCharacterDictator::SetAndSyncActionDir(uint8 uDir, bool bIgnoreForbitTurn, bool bDirGac2Gas)
{
	CActorServer* pActor = GetActor();
	Ast(pActor);
	pActor->SetServerDir(uDir);
	pActor->GetVariantDirForce()->SetNumber(bIgnoreForbitTurn);
	pActor->GetVariantDirGac2Gas()->SetNumber(bDirGac2Gas);
	pActor->GetVariantActionDir()->SetNumber(uDir);
}

void CCharacterDictator::SetAndSyncActionDirByPos(CFPos posTarget, bool bIgnoreForbitTurn)
{
	CFPos posSelf = GetPixelPos();
	CDir dir;
	CVector2f vecDir;
	vecDir.x = posTarget.x - posSelf.x;
	vecDir.y = posTarget.y - posSelf.y;
	dir.Set(vecDir);
	if (GetActionDir() != dir.uDir)
		SetAndSyncActionDir(dir.uDir, bIgnoreForbitTurn);
}

void CCharacterDictator::SetNpcBirthDir(uint8 uDir)
{
	SetAndSyncActionDir(uDir);
	if (GetNpcAIHandler())
		GetNpcAIHandler()->SetNpcBirthDir(uDir);
}

uint32 CCharacterDictator::GetActionDir()const
{
	CActorServer* pActor = GetActor();
	Ast(pActor);
	return (uint32)pActor->GetSeverDir();
	//return (uint32)pActor->GetVariantActionDir()->GetNumber<uint8>();
}
// About Actor End

void CCharacterDictator::OnMoveBegan()
{
	Ast(GetFighter());
	GetFighter()->Notify(eCE_MoveBegan);

	Ast(GetActor());
	GetActor()->OnMoveBegan();

	if (GetNpcAIHandler())
		GetNpcAIHandler()->OnMoveBegan();

	CTriggerEvent::MessageEvent(eTST_Trigger,GetFighter(), NULL, eSET_MoveBegin);
}

void CCharacterDictator::OnMoveEnded(EMoveEndedType eMEType, uint32 uMoveID)
{
	//cout << GetEntityID() << " moveend " << uMoveID << endl;
	Ast(GetFighter());
	if (eMEType==eMET_Reached)
	{
		GetFighter()->Notify(eCE_MoveSuccess, &uMoveID);
	}
	else if (eMEType==eMET_Stopped)
	{
		GetFighter()->Notify(eCE_MoveStop, &uMoveID);
	}
	GetFighter()->Notify(eCE_MoveEnd, &uMoveID);

	Ast(GetActor());
	GetActor()->OnMoveEnded();

	if (GetNpcAIHandler())
		GetNpcAIHandler()->OnMoveEnded(uMoveID);

	CTriggerEvent::MessageEvent(eTST_Trigger,GetFighter(), NULL, eSET_MoveEnd);
}

void CCharacterDictator::OnCoreEvent(const void* pCoreEvent)
{
	GetFighter()->Notify(eCE_CoreEvent,const_cast<void*>(pCoreEvent));
}

void CCharacterDictator::OnCaughtViewSightOf(uint32 uObjGlobalId, uint32 uDimension)
{
	Ast (GetNpcAIHandler());
	GetNpcAIHandler()->OnCaughtViewSightOf(uObjGlobalId, uDimension);
}

void CCharacterDictator::OnLostViewSightOf(uint32 uObjGlobalId, uint32 uDimension)
{
	Ast (GetNpcAIHandler());
	GetNpcAIHandler()->OnLostViewSightOf(uObjGlobalId, uDimension);
}	

void CCharacterDictator::FacialActionHappened(uint32 uPlayerID, const TCHAR* szFacialName)
{
	if (GetNpcAIHandler())
		GetNpcAIHandler()->OnFacialAction(uPlayerID, szFacialName);
}

void CCharacterDictator::SetNatureInfo(const TCHAR* szNatureName, const TCHAR* szNatureArg, uint32 uNatureExtraArg, 
									   const TCHAR* szNatureAction, uint32 uColdTime, const TCHAR* szAction, const TCHAR* szActionArg, uint32 uLevle)
{
	if (GetNpcAIHandler())
		GetNpcAIHandler()->OnSetNatureData(szNatureName, szNatureArg, uNatureExtraArg, szNatureAction, uColdTime, szAction, szActionArg, uLevle);
}

void CCharacterDictator::OnSendWarningEvent()
{
	Ast(GetNpcAIHandler());
	GetNpcAIHandler()->SendNpcEvent(eNpcEvent_OnWarning);
}

void CCharacterDictator::OnSendChangeModelEvent()
{
	Ast(GetNpcAIHandler());
	GetNpcAIHandler()->SendNpcEvent(eNpcEvent_OnChangeModel);
}

void CCharacterDictator::OnSendExitSubMachineEvent()
{
	Ast(GetNpcAIHandler());
	GetNpcAIHandler()->SendNpcEvent(eNpcEvent_OnExitSubMachine);
}

void CCharacterDictator::MoveToDest(uint32 uX,uint32 uY)
{
	if(GetNpcAIHandler())
		GetNpcAIHandler()->MoveToDest(uX,uY);
}
void CCharacterDictator::MoveToPathPoint(uint8 point)
{
	if(GetNpcAIHandler())
		GetNpcAIHandler()->OnMoveToPathPoint(point);
}

void CCharacterDictator::SetGroupMemberID(uint32 id)
{
	if(GetNpcAIHandler())
		GetNpcAIHandler()->SetGroupMemberID(id);
}

void CCharacterDictator::OnChangeSpeedBySkill()
{
	INpcAIHandler *pAIHandler = GetNpcAIHandler();
	if(pAIHandler)
	{
		CNpcAI* pAI = pAIHandler->CastToNpcAI();
		if(pAI)
			pAI->OnChangeSpeedBySkill();
	}
}

void CCharacterDictator::OnTraceEnded(ETraceEndedType eType)
{
	INpcAIHandler *pAIHandler = GetNpcAIHandler();
	if(pAIHandler)
	{
		CNpcAI* pAI = pAIHandler->CastToNpcAI();
		if(pAI)
			pAI->OnTraceEnded(eType);
	}
}

void CCharacterDictator::TalkWithPlayerStart(uint32 uEntityID)
{
	if(GetNpcAIHandler())
		GetNpcAIHandler()->TalkWithPlayerStart(uEntityID);
}
void CCharacterDictator::TalkWithPlayerEnd()
{
	if(GetNpcAIHandler())
		GetNpcAIHandler()->TalkWithPlayerEnd();
}

void CCharacterDictator::KillNpcBySpecialAction(CCharacterDictator* pAttacker, const TCHAR* sActionName)
{
	Ast(GetFighter());
	if (pAttacker == NULL)
		return;
	Ast(pAttacker->GetFighter());
	GetFighter()->CppDie(pAttacker->GetFighter(),sActionName);
	uint32 uFromID = pAttacker->GetFighter() ? pAttacker->GetFighter()->GetEntityID() : 0;
	uint32 uToID = GetFighter() ? GetFighter()->GetEntityID() : 0;
	CTriggerEvent::MessageEvent(eTST_Trigger, uFromID, uToID, eSET_Killed);
	if (GetFighter()->CastToFighterMediator())
	{
		CTriggerEvent::MessageEvent(eTST_Trigger, uFromID, uToID, eSET_KillPlayer);
	}
	else
	{
		CTriggerEvent::MessageEvent(eTST_Trigger, uFromID, uToID, eSET_KillNPC);
	}
}

ESetPosResult CCharacterDictator::SetGridPosByTransport(const CPos& GridPos)
{
	ESetPosResult Ret = SetGridPos(GridPos);
	GetFighter()->Notify(eCE_Transport);
	return Ret;
}

ESetPosResult CCharacterDictator::SetPixelPosByTransport(const CFPos& PixelPos)
{
	ESetPosResult Ret = SetPixelPos(PixelPos);
	GetFighter()->Notify(eCE_Transport);
	return Ret;
}

bool CCharacterDictator::CppIsLive()
{
	return GetFighter()->CppIsAlive();
}
void CCharacterDictator::SetCallbackHandler( ICharacterDictatorCallbackHandler* pCallbackHandler)
{
	AdvAddRef(pCallbackHandler);
	AdvDelRef(m_CallbackHandler.Get());
	pCallbackHandler ? m_CallbackHandler.Attach(pCallbackHandler->GetRefByCharacter()) : m_CallbackHandler.Detach();
}

ICharacterDictatorCallbackHandler* CCharacterDictator::GetCallbackHandler() const
{
	return m_CallbackHandler.Get();
}

void CCharacterDictator::SetNpcAIHandler( INpcAIHandler* pNpcAIHandler)
{
	pNpcAIHandler ? m_NpcAIHandler.Attach(pNpcAIHandler->GetRefByCharacter()) : m_NpcAIHandler.Detach();
}

INpcAIHandler* CCharacterDictator::GetNpcAIHandler() const
{
	return m_NpcAIHandler.Get();
}

void CCharacterDictator::Release()
{
	if (GetMaster())
		GetMaster()->RemoveServantFromServantMgr(GetEntityID());
	DestoryQueueMgr();
	DestroyServantMgr();
	DestroyGroupMgr();

	//有时序依赖
	GetFighter()->Notify(eCE_Offline);
	GetFighter()->ClearAllStateMgr();
	Ast (GetNpcAIHandler());
	delete GetNpcAIHandler();
	delete GetFighter();
	m_pFighter = NULL;
	delete this;
}

bool CCharacterDictator::CppGetCtrlState(EFighterCtrlState eState)const
{
	Ast(GetFighter());
	return GetFighter()->GetCtrlState(eState);
}

bool CCharacterDictator::SetCtrlState(EFighterCtrlState eState, bool bSet, CSkillInstServer* pSkillInst)
{
	Ast(GetFighter());
	return GetFighter()->SetCtrlState(eState, bSet, pSkillInst);
}

bool CCharacterDictator::CppSetCtrlState(EFighterCtrlState eState, bool bSet)
{
	Ast(GetFighter());
	bool bSucc = GetFighter()->CppSetCtrlState(eState,bSet);
	if (bSucc && eState == eFCS_InDuel)
		GetFighter()->Notify(eCE_BattleRelationChange);
	return bSucc;
}

bool CCharacterDictator::CppSetCtrlStateForLua(EFighterCtrlState eState, bool bSet)
{
	Ast(GetFighter());
	bool bSucc = GetFighter()->CppSetCtrlState(eState,bSet);
	if (bSucc && eState == eFCS_InDuel)
		GetFighter()->Notify(eCE_BattleRelationChange);
	return bSucc;
}

void CCharacterDictator::CppSetDoSkillCtrlStateForLua(EDoSkillCtrlState eState,bool bSet)
{
	Ast(GetFighter());
	GetFighter()->SetDoSkillCtrlState(eState,bSet);
}

bool CCharacterDictator::CppGetDoSkillCtrlState(EDoSkillCtrlState eState)
{
	Ast(GetFighter());
	bool bSucc = GetFighter()->GetDoSkillCtrlState(eState);
	return bSucc;
}

void CCharacterDictator::CppReborn()
{
	Ast(GetFighter());
	GetFighter()->CppReborn();
}

EClass CCharacterDictator::CppGetClass()const
{
	Ast(GetFighter());
	return GetFighter()->CppGetClass();
}

void CCharacterDictator::CppSetClass(EClass eClass)
{
	Ast(GetFighter());
	GetFighter()->CppSetClass(eClass);
}

uint32 CCharacterDictator::CppGetLevel()const
{
	Ast(GetFighter());
	return GetFighter()->CppGetLevel();
}

void CCharacterDictator::CppSetLevel(uint32 uLevel)
{
	Ast(GetFighter());
	GetFighter()->CppSetLevel(limit2(uLevel,uint32(1),uint32(eMaxLevel)));
	GetFighter()->OnLevelChanged(GetFighter()->CppGetLevel());
	GetFighter()->m_HealthPoint.FullFill(GetFighter());
	GetFighter()->m_ManaPoint.FullFill(GetFighter());
	GetFighter()->Notify(eCE_BattleRelationChange);
}

ECamp CCharacterDictator::CppGetBirthCamp()const
{
	Ast(GetFighter());
	return GetFighter()->CppGetBirthCamp();
}

void CCharacterDictator::CppSetBirthCamp(ECamp eCamp)
{
	Ast(GetFighter());
	GetFighter()->CppSetBirthCamp(eCamp);
}

ECamp CCharacterDictator::CppGetCamp()const
{
	Ast(GetFighter());
	return GetFighter()->CppGetCamp();
}

//主人改变阵营，小弟们也应该同时改变
void CCharacterDictator::CppSetCamp(ECamp eCamp)
{
	Ast(GetFighter());
	GetFighter()->CppSetCamp(eCamp);
}

bool CCharacterDictator::CppGetPKState() const
{
	Ast(GetFighter());
	return GetFighter()->CppGetPKState();
}

void CCharacterDictator::CppSetPKState(bool bPKState)
{
	Ast(GetFighter());
	GetFighter()->CppSetPKState(bPKState);
}

//EPKState CCharacterDictator::CppGetPKState() const
//{
//	Ast(GetFighter());
//	return GetFighter()->CppGetPKState();
//}
//
//void CCharacterDictator::CppSetPKState(EPKState ePKState)
//{
//	Ast(GetFighter());
//	GetFighter()->CppSetPKState(ePKState);
//}
//
//EZoneType CCharacterDictator::CppGetZoneType() const
//{
//	Ast(GetFighter());
//	return GetFighter()->CppGetZoneType();
//}
//
//void CCharacterDictator::CppSetZoneType(EZoneType eZoneType)
//{
//	Ast(GetFighter());
//	GetFighter()->CppSetZoneType(eZoneType);
//}

float CCharacterDictator::CppGetMaxHP()
{
	Ast(GetFighter());
	return (float)GetFighter()->m_HealthPoint.Get(GetFighter());
}

float CCharacterDictator::CppGetMaxMP()
{
	Ast(GetFighter());
	return (float)GetFighter()->m_ManaPoint.Get(GetFighter());
}

float CCharacterDictator::CppGetHP()
{
	Ast(GetFighter());
	return (float)GetFighter()->m_HealthPoint.LimitGet();
}

void CCharacterDictator::CppSetHP(uint32 uHp)
{
	Ast(GetFighter());
	GetFighter()->m_HealthPoint.LimitSet(uHp, GetFighter());
}

float CCharacterDictator::CppGetMP()
{
	Ast(GetFighter());
	return (float)GetFighter()->m_ManaPoint.LimitGet();
}

void CCharacterDictator::GM_CppSetPropertyValueByName(const TCHAR* szName, float fValue)

{
	Ast(GetFighter());
	GetFighter()->CppSetPropertyValue(szName, fValue);
}


void CCharacterDictator::CppChangePropertyValueByName(const TCHAR* szName, float fValue)
{
	Ast(GetFighter());
	GetFighter()->CppChangePropertyValue(szName, fValue);
}

CCharacterDictator* CCharacterDictator::GetCharacterByCoreObj(CCoreObjectServer* pCoreObj)
{
	if(pCoreObj == NULL) return NULL;
	ICoreObjectDictatorHandler* pHandler = pCoreObj->GetHandler() ? pCoreObj->GetHandler()->CastToObjDicHandler() : NULL;
	if ( pHandler == NULL )
		return NULL;

	size_t eType = (size_t)(pHandler->GetTag());
	if (eType == eCOHT_Character)
		if (pHandler->CastToObjMedHandler())
			return class_cast<CCharacterMediatorAdapter*>(pHandler)->GetCharacter();
		else
			return class_cast<CCharacterDictatorAdapter*>(pHandler)->GetCharacter();

	return NULL;
}

CCharacterDictator* CCharacterDictator::GetCharacterByID(uint32 uEntityID)
{
	return CEntityServerManager::GetInst()->GetCharacter(uEntityID);
}

void CCharacterDictator::CreateEnded()
{
	if (GetNpcAIHandler())
		GetNpcAIHandler()->OnCreateEnded();
}

void CCharacterDictator::AddServant(CCharacterDictator* pServant)
{
	if (!pServant)
		return;
	ENpcType eType = pServant->GetNpcType();
	if (!NpcInfoMgr::BeServantType(eType))
		return;
	if (!m_pServantMgr)
		m_pServantMgr=new CServantServerMgr(this);
	if (m_pServantMgr->GetServantByID(pServant->GetEntityID()))
		return;
	if (NpcInfoMgr::CanTakeOnlyOne(eType))
	{
		CCharacterDictator* pChar = GetServantByType(eType);
		if (pChar)
		{
			if (eType == ENpcType_Truck)
				pChar->GetCallbackHandler()->ReplaceTruck(pChar, pServant);
			RemoveServant( pChar);
		}	
	}
	else
	{
		if (eType == ENpcType_Totem)	//同名的图腾不允许带多个
		{
			const CServantServerMgr::ServantSetType& setServant=m_pServantMgr->GetServantSetByType(eType);
			CServantServerMgr::ServantSetType::const_iterator it=setServant.begin();
			for (;it!=setServant.end();++it)
			{
				CCharacterDictator* pCharacter = CCharacterDictator::GetCharacterByID(*it);
				if(!pCharacter)
					continue;
				if (pCharacter->GetNpcName() == pServant->GetNpcName())
				{
					RemoveServant(pCharacter);
					break;
				}
			}
		}
	}
	m_pServantMgr->AddServant( eType, pServant->GetEntityID(), pServant);
	GetFighter()->OnAddServant(eType, pServant->GetEntityID());
}

void CCharacterDictator::AddGroupMember(CCharacterDictator* pMember)
{
	Ast(GetFighter());
	if (!pMember)
		return;
	if (!m_pGroupMgr)
		m_pGroupMgr=new CNpcGroupMgr(this);

	if(m_pGroupMgr->AddMember(pMember))
	{
		CDummyAI* pDummyAI = GetNpcAIHandler()->CastToDummyAI();
		Ast (pDummyAI);
		pDummyAI->OnMemberJoinGroup(pMember);
	}
}

void CCharacterDictator::RemoveGroupMember(CCharacterDictator* pMember)
{
	if (!m_pGroupMgr)
		return;
	m_pGroupMgr->RemoveMember(pMember);
}

void CCharacterDictator::RemoveAllGroupMember()
{
	if (!m_pGroupMgr)
		return;
	m_pGroupMgr->RemoveAllMember();
}

bool CCharacterDictator::AdjustIsGroupMember(uint32 uGlobalID)
{
	if (!m_pGroupMgr)
		return false;
	else
		return m_pGroupMgr->IsInMemberList(uGlobalID);
}

void CCharacterDictator::DestroyServantMgr()
{
	if (m_pServantMgr != NULL)
	{
		delete m_pServantMgr;
		m_pServantMgr = NULL;
	}
}

void CCharacterDictator::DestroyGroupMgr()
{
	if (m_pGroupMgr != NULL)
	{
		delete m_pGroupMgr;
		m_pGroupMgr = NULL;
	}
}

void CCharacterDictator::SetOnDisappear(bool bReborn)
{
	Ast(GetFighter());
	if (GetNpcAIHandler())
		GetNpcAIHandler()->ClearAllEnmity();
	CCharacterDictator* pMaster = GetMaster();
	if (NULL != pMaster)
		pMaster->RemoveServantFromServantMgr(this->GetEntityID());
	GetFighter()->Notify(eCE_Die);
	GetFighter()->IntDie();
	StopMoving();
	SetAndSyncMoveState(eMS_LockStop);
	SetNoDissolve();
	if (GetCallbackHandler())
		GetCallbackHandler()->OnDisappear( this, bReborn);
}

void CCharacterDictator::SetOnDead(bool bNormalDead)
{
	if (!CppIsLive())
		return;
	ClearAllServantByMasterDie();
	if (GetNpcAIHandler())
		GetNpcAIHandler()->ClearAllEnmity();
	CCharacterDictator* pMaster = GetMaster();
	if (pMaster)
		pMaster->RemoveServantFromServantMgr(this->GetEntityID());
	if (!bNormalDead)
		SetNoDissolve();
	OnBeginDestory(bNormalDead);
}

void CCharacterDictator::KillNpcByLifeTimeEnd()
{
	if (GetCallbackHandler())
		GetCallbackHandler()->OnDeadByTick(this);
	SetOnDead();
}

void CCharacterDictator::SetNpcDeadNormal()
{
	Ast (GetFighter());
	GetFighter()->CppDie(NULL, NULL);
}

void CCharacterDictator::RemoveServantFromServantMgr(uint32 uEntityID)
{
	if (!m_pServantMgr)
		return;
	CCharacterDictator* pServant = m_pServantMgr->GetServantByID(uEntityID);
	if (!pServant)
		return;
	m_pServantMgr->RemoveServant( pServant->GetNpcType(), pServant->GetEntityID(), pServant);
	if (pServant->GetNpcAIHandler())
		pServant->GetNpcAIHandler()->StopFollowMaster();
}

void CCharacterDictator::RemoveServant( CCharacterDictator* pServant )
{
	Ast(GetFighter());
	if (!m_pServantMgr||!pServant)
		return;
	if (m_pServantMgr->GetServantByID(pServant->GetEntityID()))
	{
		RemoveServantFromServantMgr(pServant->GetEntityID());
		if (pServant->GetNpcAIHandler())
			pServant->GetNpcAIHandler()->StopFollowMaster();
		pServant->OnBeginDestory();
	}
}
CCharacterDictator* CCharacterDictator::GetServantByID( uint32 uGlobalID ) const
{
	if (!m_pServantMgr)
	{
		return NULL;
	}
	return m_pServantMgr->GetServantByID(uGlobalID);
}

CCharacterDictator* CCharacterDictator::GetServantByType( ENpcType eType ) const
{
	if (!m_pServantMgr)
	{
		return NULL;
	}
	uint32 pServantID = m_pServantMgr->GetServantIDByType(eType);
	return 	CEntityServerManager::GetInst()->GetCharacter(pServantID);
}

void CCharacterDictator::ClearServantType(ENpcType eNpcType)
{
	if (!m_pServantMgr)
		return;
	const CServantServerMgr::ServantSetType ServantSet = m_pServantMgr->GetServantSetByType(eNpcType);
	CServantServerMgr::ServantSetType::const_iterator iter = ServantSet.begin();
	for (; iter != ServantSet.end(); iter++)
	{
		CCharacterDictator* pServant = CCharacterDictator::GetCharacterByID((*iter));
		if (pServant)
		{
			pServant->SetOnDead();
		}
	}
	
}

void CCharacterDictator::ClearAllServant()
{
	if (!m_pServantMgr)
		return;
	m_pServantMgr->ClearAllServant();
}

void CCharacterDictator::ClearAllServantByMasterDie()
{
	if (!m_pServantMgr)
		return;
	m_pServantMgr->ClearAllServantByMasterDie();
}

void CCharacterDictator::SetRealName(const char* szRealName)
{
	m_strRealName = szRealName;
	Ast(GetFighter());
	GetFighter()->OnRealNameChange(GetNpcType(), szRealName);
}

void CCharacterDictator::AddSkill(const TCHAR* szSkillName,uint8 uSkillLevel)
{
	Ast(GetFighter());
	GetFighter()->AddSkill(szSkillName,uSkillLevel);
}

bool CCharacterDictator::IsSkillActive(const TCHAR* szSkillName)
{
	Ast(GetFighter());
	return GetFighter()->IsSkillActive(szSkillName,0);
}

void CCharacterDictator::RemoveSkill(const TCHAR*  szSkillName)
{
	Ast(GetFighter());
	GetFighter()->RemoveSkill(szSkillName);
}
void CCharacterDictator::ClearAllSkill()
{
	Ast(GetFighter());
	GetFighter()->ClearAllSkill();
}

CCharacterDictator* CCharacterDictator::GetTarget()
{
	Ast(GetFighter());
	CFighterDictator* pTarget = GetFighter()->GetTarget();
	if(pTarget)
		return pTarget->GetCharacter();
	return NULL;
}

void CCharacterDictator::OnMasterDestroyed()
{
	if (GetNpcAIHandler())
		GetNpcAIHandler()->OnMasterDestroyed();
}

void CCharacterDictator::OnGroupDestroyed()
{
	SetGroupLeader(NULL);
	if (GetNpcAIHandler())
		GetNpcAIHandler()->OnGroupDestroyed();
}

void CCharacterDictator::OnEnterBattleState()
{
	if (GetCallbackHandler())
		GetCallbackHandler()->OnEnterBattleState(this);
	if (GetNpcAIHandler())
		GetNpcAIHandler()->OnEnterBattleState();
}

void CCharacterDictator::OnLeaveBattleState()
{
	ClearAllChaser();
	if (GetCallbackHandler())
		GetCallbackHandler()->OnLeaveBattleState(this);
	if (GetNpcAIHandler())
		GetNpcAIHandler()->OnLeaveBattleState();
}

void CCharacterDictator::OnAgileValueChanged(uint32 uValueTypeID, float fValue) const
{
	if (GetNpcAIHandler())
		GetNpcAIHandler()->OnAgileValueChanged(uValueTypeID, fValue);
}

void CCharacterDictator::OnCtrlValueChanged(uint32 uValue, uint32 uValueTypeID, bool bSet)const
{
	if (GetNpcAIHandler())
		GetNpcAIHandler()->OnCtrlValueChanged(uValue,uValueTypeID,bSet);
}

void CCharacterDictator::OnDoSkillCtrlStateChanged(uint32 uStateID, bool bSet)const
{
	if (GetNpcAIHandler())
		GetNpcAIHandler()->OnDoSkillCtrlStateChanged(uStateID,bSet);
}

void CCharacterDictator::OnSkillBegin()const
{
	if(GetNpcAIHandler())
		GetNpcAIHandler()->OnSkillBegin();
}

void CCharacterDictator::OnSkillEnd(bool bSucceed)const
{
	if (GetNpcAIHandler())
	{
		if(bSucceed)
			GetNpcAIHandler()->OnSkillSuccessEnd();
		else
			GetNpcAIHandler()->OnSkillFailEnd();
	}
}

void CCharacterDictator::SetNpcType(ENpcType eNpcType)
{
	m_eNpcType = eNpcType; 
	GetNpcTypeVariant()->SetNumber((uint8)eNpcType);
}

void CCharacterDictator::ForceLock(CCharacterDictator* pTarget, uint32 uChaosTime)
{
	if (GetNpcAIHandler())
		GetNpcAIHandler()->ForceLock(pTarget, uChaosTime);
}

CCharacterDictator*	CCharacterDictator::GetMaster()
{
	return m_pMaster.Get();
}

CCharacterDictator*	CCharacterDictator::GetGroupLeader()
{
	return m_pGroupLeader.Get();
}
void CCharacterDictator::GetGroupMemberRebornPosition(uint32 uMemID,CFPos &pos)
{
	CFPos rebornPos(0,0);
	INpcAIHandler* pLeaderAI = GetNpcAIHandler();
	if(pLeaderAI)
	{
		CDummyAI* pGroupLeaderAI = pLeaderAI->CastToDummyAI();
		if(pGroupLeaderAI)
			rebornPos = pGroupLeaderAI->GetMemberReBornPosition(uMemID);
	}
	pos = rebornPos;
}

bool CCharacterDictator::IsInitMemberPathPoint(uint32 uMemID)
{
	INpcAIHandler* pLeaderAI = GetNpcAIHandler();
	if(pLeaderAI)
	{
		CDummyAI* pGroupLeaderAI = pLeaderAI->CastToDummyAI();
		if(pGroupLeaderAI)
			return pGroupLeaderAI->IsInitMemberPathPoint(uMemID);
		else
		{
			ostringstream ExpStr;
			const string &strName = "虚拟NPC CCharacterDictator对象存在，但CDummyAI对象已经被干掉";
			ExpStr<<"INpcAIHandler 存在!"<<strName.c_str();
			GenErr(ExpStr.str());
			return false;
		}
	}
	else
	{
		ostringstream ExpStr;
		const string &strName = "虚拟NPC CCharacterDictator对象存在，但CDummyAI对象已经被干掉";
		ExpStr<<"INpcAIHandler不存在!"<<strName.c_str();
		GenErr(ExpStr.str());
		return false;
	}
}

CCharacterDictator* CCharacterDictator::GetGroupMaster()
{
	return (m_pMaster.Get() != NULL && m_pMaster.Get()->GetNpcType() == ENpcType_Dummy) ? m_pMaster.Get() : NULL;
}

void CCharacterDictator::SetMaster(CCharacterDictator* pMaster)
{
	if(pMaster == this)
		return;
	if(pMaster)
		m_pMaster.Attach(pMaster->m_RefsByCharacter);
	else
		m_pMaster.Detach();
	INpcAIHandler* pHandler =GetNpcAIHandler();
	CMemberAI* pMemberAI= NULL;
	if(pHandler)
		pMemberAI = pHandler->CastToMemberAI();
	if (pMaster && pHandler&&pMemberAI)
		pMemberAI->InitMemberAIDate();
	if (pMaster && GetMasterIDVariant())
		GetMasterIDVariant()->SetNumber((uint32)pMaster->GetEntityID());
	else
		GetMasterIDVariant()->SetNumber((uint32)0);
}

void CCharacterDictator::SetGroupLeader(CCharacterDictator* pLeader)
{
	if(pLeader == this)
		return;
	if(pLeader)
	{
		m_pGroupLeader.Attach(pLeader->m_RefsByCharacter);
		CMemberAI* pMemberAI= GetNpcAIHandler()->CastToMemberAI();
		if (pMemberAI)
			pMemberAI->InitMemberAIDate();
	}
	else
		m_pGroupLeader.Detach();
}


void CCharacterDictator::SetDefaultWalkState(bool bWalkState)
{	
	if (GetNpcAIHandler())
	GetNpcAIHandler()->SetDefaultWalkState(bWalkState);
}

void CCharacterDictator::SetMoveRange(uint32 range)
{
	if (GetNpcAIHandler())
	GetNpcAIHandler()->SetMoveRange(range);
}

void CCharacterDictator::OnBeginRandomMove()
{
	if (GetNpcAIHandler())
	{
		GetNpcAIHandler()->OnBeginRandomMove();
	}
}

void CCharacterDictator::SetMovePath(uint8 uPoint, float x, float y)
{
	CFPos pos(x,y);
	if (!GetScene()->IsPixelValid(pos))
	{
		ostringstream ExpStr;
		ExpStr<<"场景: 【"<<GetScene()->GetSceneName()<<"】中Npc: 【"<<m_sNpcName<<"】 设置第: 【"<<(uint32)uPoint<<"】 巡逻点坐标: ("<<x<<","<<y<<") 不在地图范围内！"<<endl;
		//LogErr("设置Npc的巡逻点不在地图范围内！", ExpStr.str().c_str());
		GenErr( ExpStr.str());
	}
	if (GetNpcAIHandler())
	GetNpcAIHandler()->OnCreateMovePath(uPoint, x, y);
}

void CCharacterDictator::SetMoveType(uint8 uMoveType)
{
	if (GetNpcAIHandler())
		GetNpcAIHandler()->SetMoveType(uMoveType);
}

bool CCharacterDictator::IsBattleHorse()const
{
	if(m_eNpcType == ENpcType_BattleHorse)
		return true;
	return false;
}

bool CCharacterDictator::IsServant()const
{
	if(GetNpcType() == ENpcType_Summon)
		return true;
	return false;
}

CCharacterDictator* CCharacterDictator::GetBattleHorse() const
{
	return GetServantByType(ENpcType_BattleHorse);
}

void CCharacterDictator::DoPosNPCSkill(const TCHAR* sName,const CFPos& Pos) 
{
	Ast (GetFighter());
	Ast (GetNpcAIHandler());
	CGenericTarget Target(Pos);
	GetFighter()->DoNPCSkill(sName,GetNpcAIHandler()->GetNpcDoSkillLevel(),&Target);
}

void CCharacterDictator::DoPosNPCSkill(const CNormalSkillServerSharedPtr* pCfg,const CFPos& Pos) 
{
	Ast (GetFighter());
	Ast (GetNpcAIHandler());
	CGenericTarget Target(Pos);
	GetFighter()->DoNPCSkill(pCfg,GetNpcAIHandler()->GetNpcDoSkillLevel(),&Target);
}

void CCharacterDictator::DoNPCSkill(const TCHAR* sName, CEntityServer* pTarget)
{
	Ast (GetFighter());
	Ast (GetNpcAIHandler());
	CGenericTarget Target(pTarget->GetFighter());
	GetFighter()->DoNPCSkill(sName,GetNpcAIHandler()->GetNpcDoSkillLevel(),&Target);
}

void CCharacterDictator::DoNPCSkill(const CNormalSkillServerSharedPtr* pCfg, CEntityServer* pTarget)
{
	Ast (GetFighter());
	Ast (GetNpcAIHandler());
	CGenericTarget Target(pTarget->GetFighter());
	GetFighter()->DoNPCSkill(pCfg,GetNpcAIHandler()->GetNpcDoSkillLevel(),&Target);
}

void CCharacterDictator::ServerDoNoRuleSkill(const TCHAR* sName, CEntityServer* pTarget)
{
	Ast (GetFighter());
	Ast (GetNpcAIHandler());
	CGenericTarget Target(pTarget->GetFighter());
	GetFighter()->DoNPCSkill(sName,GetNpcAIHandler()->GetNpcDoSkillLevel(),&Target,false);
}

void CCharacterDictator::ClearState(const TCHAR* sName)
{
	Ast(GetFighter());
	GetFighter()->ClearState(sName);
}

bool CCharacterDictator::EraseErasableState(const TCHAR* sName)
{
	Ast(GetFighter());
	return GetFighter()->EraseErasableState(sName);
}

bool CCharacterDictator::ExistState(const TCHAR* sName)
{
	Ast(GetFighter());
	return GetFighter()->ExistState(sName);
}

uint32 CCharacterDictator::GetStateCascade(const TCHAR* sName)
{
	Ast(GetFighter());
	return GetFighter()->GetStateCascade(sName);
}

bool CCharacterDictator::IsSingTypeSkill(const TCHAR* sName)
{
	CCastingProcessCfgServerSharedPtr& p_Cfg = CCastingProcessCfgServer::Get(sName);
	if (p_Cfg && p_Cfg->GetCastingType() == eCPT_Sing)
		return true;
	return false;
}

uint32 CCharacterDictator::GetSkillCastAniFrameCnt(const TCHAR* szSkillName)
{
	uint32 uFrameCnt = 0;
	const CNormalSkillServerSharedPtr& pSkill = CNormalSkillServer::GetNPCSkill(szSkillName);
	if (pSkill)
	{
		const string& strCastAction = pSkill->GetCastAction();
		if (strCastAction != "")
		{
			uFrameCnt = CAniKeyFrameCfg::GetNPCAniKeyFrame(GetNpcName().c_str(), strCastAction.c_str(), "e");
		}
	}
	return uFrameCnt;
}

bool CCharacterDictator::ExistBirthAniFrame()
{
	return GetNpcBirthAniFrameCnt() != 0 ? true : false;
}

uint32 CCharacterDictator::GetNpcBirthAniFrameCnt()
{
	return CAniKeyFrameCfg::GetNPCAniKeyFrame(GetNpcName().c_str(), "birth", "e");
}

bool CCharacterDictator::ExistSpecialActionAni()
{
	return GetSpecialActionAniCnt() != 0;
}

uint32 CCharacterDictator::GetSpecialActionAniCnt()
{
	return CAniKeyFrameCfg::GetNPCAniKeyFrame(GetNpcName().c_str(), "stand03", "e");
}

uint32 CCharacterDictator::GetOneActorAround()
{
	if (GetNpcAIHandler())
		return GetNpcAIHandler()->GetOneActorAround();
	return 0;
}

uint32 CCharacterDictator::GetExpOwnerID()
{
	if (GetNpcAIHandler())
		return GetNpcAIHandler()->GetExpOwnerID();
	return 0;
}

uint32 CCharacterDictator::GetOneNpcAround()
{
	if (GetNpcAIHandler())
		return GetNpcAIHandler()->GetOneNpcAround();
	return 0;
}

uint32 CCharacterDictator::GetRandomOneAround()
{
	if (GetNpcAIHandler())
		return GetNpcAIHandler()->GetRandomOneAround();
	return 0;
}

void CCharacterDictator::SetTarget(uint32 uTargetId)
{
	Ast(GetFighter());
	CCharacterDictator* pTarget = CEntityServerManager::GetInst()->GetCharacter(uTargetId);
	if (pTarget)
	{
		CFighterDictator* pFighter = pTarget->GetFighter();
		GetFighter()->SetTarget(pFighter);
	}
}

void CCharacterDictator::OnBeAttacked(uint32 uTargetId)
{
	CCharacterDictator* pTarget = CEntityServerManager::GetInst()->GetCharacter(uTargetId);
	if (GetNpcAIHandler())
		GetNpcAIHandler()->OnBeAttacked(pTarget);
}

void CCharacterDictator::AddChaser(uint32 uEntityID)
{
	CCharacterDictator* pCharacter = CCharacterDictator::GetCharacterByID(uEntityID);
	if (!pCharacter)
		return;
	ChaserMapIter iter = m_MapChaser.find(uEntityID);
	if (iter == m_MapChaser.end())
	{
		m_MapChaser.insert(make_pair(uEntityID, pCharacter));
		if (!m_NpcChaserQueueMgr)
			m_NpcChaserQueueMgr = new CNpcChaserQueueMgr(this);
	}
}

void CCharacterDictator::DelChaser(uint32 uEntityID)
{
	CCharacterDictator* pCharacter = CCharacterDictator::GetCharacterByID(uEntityID);
	if (!pCharacter)
		return;
	ChaserMapIter iter = m_MapChaser.find(uEntityID);
	if (iter != m_MapChaser.end())
	{
		m_MapChaser.erase(iter);
		if (m_MapChaser.empty())
		{
			ClearAllChaser();
			return;
		}
		m_NpcChaserQueueMgr->DelChaser(uEntityID);
	}
}

void CCharacterDictator::ClearAllChaser()
{
	m_MapChaser.clear();
	if (m_NpcChaserQueueMgr)
	{
		delete m_NpcChaserQueueMgr;
		m_NpcChaserQueueMgr = NULL;
	}
}

void CCharacterDictator::GetRegulateNpcPlace(uint32 uEntityID, CFPos& Pos)
{
	if (m_NpcChaserQueueMgr)
		m_NpcChaserQueueMgr->GetRegulateNpcPlace(uEntityID, Pos);
}

void CCharacterDictator::MasterAttackCommand(ENpcType eType)
{
	if (GetServantMgr())
	{
		CCharacterDictator* pTarget = GetTarget();
		CCharacterDictator* pServant = GetServantByType(eType); 
		if (pTarget && pServant && pTarget != pServant)
		{
			pServant->GetNpcAIHandler()->CastToServantAI()->Attack(pTarget);
		}
	}
}

void CCharacterDictator::MasterRetreatCommand(ENpcType eType)
{	
	if (GetServantMgr())
	{
		CCharacterDictator* pServant = GetServantByType(eType); 
		if (pServant)
		{
			pServant->GetNpcAIHandler()->CastToServantAI()->Retreat();
		}
	}
}
void CCharacterDictator::MasterDisbandCommand(ENpcType eType)
{
	if (GetServantMgr())
	{
		CCharacterDictator* pServant = GetServantByType(eType); 
		if (pServant)
		{
			pServant->GetNpcAIHandler()->CastToServantAI()->Disband();
		}
	}
}

void CCharacterDictator::MasterSpreadCommand(ENpcType eType)
{
	if (GetServantMgr())
	{
		CCharacterDictator* pServat = GetServantByType(eType);
		if (pServat)
		{
			pServat->GetNpcAIHandler()->CastToServantAI()->Spread();
		}
	}
}

bool CCharacterDictator::MasterOrderServantMoveCommand(ENpcType eType, uint32 x, uint32 y)
{
	if (GetServantMgr())
	{
		CCharacterDictator* pServat = GetServantByType(eType);
		if (pServat)
		{
			return pServat->GetNpcAIHandler()->CastToServantAI()->Move(x,y);
		}
	}
	return false;
}

bool CCharacterDictator::IsMasterHaveServant(ENpcType eType)
{
	
	if (m_pServantMgr)
	{
		CCharacterDictator* pServant = GetServantByType(eType);
		if (pServant)
		{
			return true;
		}
	}

	return false;
}

void CCharacterDictator::DestoryServant(ENpcType eType)
{
	if(m_pServantMgr == NULL)
	{
		return;
	}
	CCharacterDictator* pServant = GetServantByType(eType);
	if(pServant == NULL)
	{
		return;
	}
	GetServantMgr()->RemoveServant(pServant->GetNpcType(),pServant->GetEntityID(),pServant);
	GetCallbackHandler()->OnDestroyNpc(pServant);
}

ENpcAIType CCharacterDictator::GetAITypeID()
{
	if (GetNpcAIHandler())
	{
		return GetNpcAIHandler()->GetRealAIType();
	}
	return ENpcAIType_None;
}

void CCharacterDictator::SetNpcAIType(ENpcAIType eAIType)
{
	GetNpcAITypeVariant()->SetNumber((uint8)eAIType);
}

void CCharacterDictator::OnBeginDestory(bool bNormalDead, uint32 uExpOwnerID, uint32 KillerID, bool bServantChangeScene)
{
	GetFighter()->Notify(eCE_Die);
	GetFighter()->IntDie();
	StopMoving();
	if (!bServantChangeScene)
		SetAndSyncActionState(eAS_Die);
	SetAndSyncMoveState(eMS_LockStop);

	CCharacterDictator* pMaster = GetMaster();
	if (pMaster)
	{
		CAllStateMgrServer* pStateMgr= pMaster->GetFighter()->GetAllStateMgr();
		if (pMaster->CppGetCtrlState(eFCS_InBattleHorse) && (int32)GetNpcType() == (int32)ENpcAIType_BattleHorse)
			pStateMgr->EraseState(eDST_Riding);
	}
	if (GetNpcAIHandler())
		GetNpcAIHandler()->OnDead();
	if (GetCallbackHandler())
		GetCallbackHandler()->OnNPCDead(this, uExpOwnerID, KillerID, bNormalDead);
}

void CCharacterDictator::ReSetNpcExistTick(uint32 uExitTime)
{
	if (GetNpcAIHandler())
	{
		GetNpcAIHandler()->ReSetNpcExistTick(uExitTime);
	}
}

uint32 CCharacterDictator::GetNpcExistTime()
{
	if (GetNpcAIHandler())
	{
		return GetNpcAIHandler()->GetExistTime();
	}
	return 0;
}

int32 CCharacterDictator::GetNpcLeftLifeTime()
{
	if (GetNpcAIHandler())
	{
		return GetNpcAIHandler()->GetLeftLifeTime();
	}
	return -1;
}

EAttackType CCharacterDictator::GetNpcAttackType()
{
		if(CastToCharacterMediator() != NULL)
			return eATT_None;
		if (!CppIsLive())
			return eATT_None;
		if (GetNpcAIHandler() == NULL )
			return eATT_None;
		return GetNpcAIHandler()->GetNpcAttackType();

	return eATT_None;
}

void CCharacterDictator::SetNoDissolve()
{
	CGas2GacCPP_OnRemoved Cmd;
	Cmd.uEntityID = GetEntityID();
	GetFighter()->SendCmdToGac(&Cmd);
}

bool CCharacterDictator::CanAIChange()
{
	if (!GetNpcAIHandler())
		return false;
	if (GetNpcAIHandler()->CanAIBeChange())
		return true;
	else
		return false;
}

bool CCharacterDictator::CanBeRavin()
{
	if (!GetNpcAIHandler())
		return false;
	return GetNpcAIHandler()->CanBeRavin();
}

bool CCharacterDictator::CanBeSelected()
{
	if (!GetNpcAIHandler())
		return true;
	if (GetNpcAIHandler()->CanBeSelected())
		return true;
	else
		return false;
}

void CCharacterDictator::ChangeAIType( CCharacterDictator* pMaster, ENpcAIType eAIType, ENpcType eNpcType, ECamp eCamp)
{
	const TCHAR* pNpcAIName = NpcInfoMgr::GetAINameByAIType(eAIType);
	const TCHAR* pNpcType = NpcInfoMgr::GetTypeNameByType(eNpcType);
	GetCallbackHandler()->ReplaceNpc(this, pMaster, pNpcAIName, pNpcType, "", eCamp);
}

void CCharacterDictator::SynToFollowCanBeTakeOver()
{
	m_bCanBeTakeOver = true;
	if (GetFighter())
		GetFighter()->SynToFollowCanBeTakeOver();
}

void CCharacterDictator::SynToFollowCanNotTakeOver()
{
	m_bCanBeTakeOver = false;
	if (GetFighter())
		GetFighter()->SynToFollowCanNotTakeOver();
}

void CCharacterDictator::ReSetNpcGridAttackScope(float fScope)
{
	if (GetNpcAIHandler())
		GetNpcAIHandler()->ReSetNpcGridAttackScope(fScope);
}

void CCharacterDictator::RevertNpcAttackScope()
{
	if (GetNpcAIHandler())
		GetNpcAIHandler()->RevertNpcAttackScope();
}

void CCharacterDictator::NpcShowContentBySkill(const TCHAR* sSkillName, uint32 uTargetID)
{
	if(GetCallbackHandler())
		GetCallbackHandler()->NpcShowContentBySkill(this, sSkillName, uTargetID);
}

int32 CCharacterDictator::AddMemberToQueueMgr(uint32 uEntityID)
{
	if (!m_pNpcQueueMgr)
		m_pNpcQueueMgr = new CNpcQueueMgr();
	return m_pNpcQueueMgr->AddMemberAndGetAngle(uEntityID);
}

void CCharacterDictator::RemoveMemberFromQueueMgr(uint32 uEntityID)
{
	if (m_pNpcQueueMgr)
		m_pNpcQueueMgr->RemoveMember(uEntityID);
}

void CCharacterDictator::DestoryQueueMgr()
{
	if (m_pNpcQueueMgr)
	{
		delete m_pNpcQueueMgr;
		m_pNpcQueueMgr = NULL;
	}
}

bool CCharacterDictator::CanDelInBattle()
{
	if (GetNpcAIHandler())
		return GetNpcAIHandler()->CanDelInBattle();
	return false;
}

void CCharacterDictator::SetCanDelInBattle(bool bCanDel)
{
	if (GetNpcAIHandler())
		GetNpcAIHandler()->SetCanDelInBattle(bCanDel);
}

double CCharacterDictator::GetStateZoomRate()
{
	Ast(GetFighter());
	return GetFighter()->GetAllStateMgr()->GetStateZoomRate();
}

void CCharacterDictator::SetDeadAfterBattle()
{
	if (GetNpcAIHandler())
		GetNpcAIHandler()->SetDeadAfterBattle();
}

void CCharacterDictator::CheckPlayInNpcSleepDim()
{
	if (GetNpcAIHandler())
		GetNpcAIHandler()->CheckPlayInNpcSleepDim();
}

void CCharacterDictator::SetNpcSleepState(bool bSleep)
{
	if (GetNpcAIHandler())
		GetNpcAIHandler()->SetNpcSleepState(bSleep);
}

bool CCharacterDictator::BeBossType()
{
	EClass eClass = CppGetClass();
	switch(eClass)
	{
		case eCL_014:
		case eCL_032:
		case eCL_037:
		case eCL_041:
		case eCL_042:
		case eCL_043:
		case eCL_151:
			return true;
		default:
			return false;
	}
	return false;
}

CStateDBDataSet* CCharacterDictator::SerializeStateToDB(uint32 uGrade)
{
	Ast(GetFighter());
	//CFighterMediator* pFighter=GetFighter()->CastToFighterMediator();
	////cout << "Ast( " << typeid(*GetFighter()).name() << " == " << typeid(CFighterMediator).name() << ")\n";
	//if(typeid(*GetFighter()) != typeid(CFighterMediator))
	//{
	//	stringstream s;
	//	s << "*GetFighter()不是CFighterMediator：" << 
	//		", typeid(*GetFighter()).name() = " << typeid(*GetFighter()).name() << "\n";
	//	GenErr(s.str());
	//}
	////Ast(typeid(*GetFighter()) == typeid(CFighterMediator));
	//Ast(pFighter);
	CAllStateMgrServer* pStateMgr = GetFighter()->GetAllStateMgr();
	Ast(pStateMgr);
	return pStateMgr->SerializeToDB(uGrade);
}

void CCharacterDictator::SaveStateToDBEnd()
{
	Ast(GetFighter());
	//CFighterMediator* pFighter=GetFighter()->CastToFighterMediator();
	//Ast(pFighter);
	CAllStateMgrServer* pStateMgr = GetFighter()->GetAllStateMgr();
	Ast(pStateMgr);
	pStateMgr->SaveStateToDBEnd();
}

bool CCharacterDictator::LoadStateFromDB(CStateDBDataSet* pRet)
{
	Ast(GetFighter());
	//CFighterMediator* pFighter=GetFighter()->CastToFighterMediator();
	//Ast(pFighter);
	CAllStateMgrServer* pStateMgr = GetFighter()->GetAllStateMgr();
	Ast(pStateMgr);
	return pStateMgr->LoadFromDB(pRet);
}

bool CCharacterDictator::HaveBeSetPath()
{
	if (GetNpcAIHandler())
		return GetNpcAIHandler()->HaveBeSetPath();
	return false;
}

void CCharacterDictator::OnBeHurt(int32 iHpChange)
{
	if (GetNpcAIHandler())
		GetNpcAIHandler()->OnBeHurt(iHpChange);
}

void CCharacterDictator::RemoveEnemyFromServantEnmityList(CCharacterDictator* pEnemy)
{
	CServantServerMgr* pServantMgr = GetServantMgr();
	if(pServantMgr)	
	{
		const CServantServerMgr::ServantIDMapType& ServantMap = pServantMgr->GetServantIDMapType();
		CServantServerMgr::ServantIDMapType::const_iterator it = ServantMap.begin();
		for(;it!=ServantMap.end();)
		{
			CCharacterDictator* pServant = (*it).second;
			++it;
			{
				if (pServant && pServant->GetNpcAIHandler())
				{
					pServant->GetNpcAIHandler()->RemoveOneEnemy(pEnemy);
				}
			}
		}
	}
}

void CCharacterDictator::CppSetGameCamp(int32 iGameCamp)
{
	Ast (GetFighter());
	GetFighter()->CppSetGameCamp(iGameCamp);
}

int32 CCharacterDictator::CppGetGameCamp()
{
	Ast (GetFighter());
	return GetFighter()->CppGetGameCamp();
}

void CCharacterDictator::ReSetGameCamp()
{
	Ast (GetFighter());
	GetFighter()->CppSetGameCamp(0);
}

void CCharacterDictator::SendNpcDestoryMsg()
{
	Ast (GetNpcAIHandler());
	GetNpcAIHandler()->OnDestoryBegin();
}

float CCharacterDictator::GetNpcSize()
{
	Ast (GetNpcAIHandler());
	return GetNpcAIHandler()->GetNpcSize();
}

void CCharacterDictator::SetBeShowInClient(bool bShow)
{
	GetShowInClientVariant()->SetNumber((bool)bShow);
}

void CCharacterDictator::SetActiveState(bool bActiveNpc)
{
	GetBeActiveVarint()->SetNumber((bool)bActiveNpc);
}

void CCharacterDictator::SetAttackType(EAttackType eAttackType)
{
	GetNpcAttackTypeVariant()->SetNumber((uint8)eAttackType);
}

void CCharacterDictator::SetFightDir(bool bFaceEnemy)
{
	GetFightDirVariant()->SetNumber((bool)bFaceEnemy);
}

CSyncVariantServer* CCharacterDictator::GetCampVariant() const
{
	return m_pViewVariantAttribute_Camp;
}

CSyncVariantServer* CCharacterDictator::GetBirthCampVariant() const
{
	return m_pViewVariantAttribute_BirthCamp;
}

CSyncVariantServer* CCharacterDictator::GetGameCampVariant() const
{
	return m_pViewVariantAttribute_GameCamp;
}

CSyncVariantServer* CCharacterDictator::GetLevelVariant() const
{
	return m_pViewVariantAttribute_Level;
}

CSyncVariantServer* CCharacterDictator::GetClassVariant() const
{
	return m_pViewVariantAttribute_Class;
}

CSyncVariantServer* CCharacterDictator::GetNpcTypeVariant() const
{
	return m_pViewVariantAttribute_NpcType;
}

CSyncVariantServer* CCharacterDictator::GetNpcAITypeVariant() const
{
	return m_pViewVariantAttribute_NpcAIType;
}

CSyncVariantServer* CCharacterDictator::GetBeActiveVarint() const
{
	return m_pViewVariantAttribute_BeActiveNpc;
}

CSyncVariantServer* CCharacterDictator::GetNpcAttackTypeVariant() const
{
	return m_pViewVariantAttribute_AttackType;
}

CSyncVariantServer* CCharacterDictator::GetRealNameVariant() const
{
	return m_pViewVariantAttribute_RealName;
}

CSyncVariantServer* CCharacterDictator::GetShowInClientVariant() const
{
	return m_pViewVariantAttribute_ShowInClient;
}

CSyncVariantServer* CCharacterDictator::GetFightDirVariant() const
{
	return m_pViewVariantAttribute_FightDir;
}

CSyncVariantServer* CCharacterDictator::GetPKSwitchTypeVariant() const
{
	return m_pViewVariantPKSwitch_PKSwitchType;
}

CSyncVariantServer* CCharacterDictator::GetZoneTypeVariant() const
{
	return m_pViewVariantPKSwitch_ZoneType;
}

CSyncVariantServer* CCharacterDictator::GetTargetIDVariant() const
{
	return m_pSyncVariantAttribute1_TargetID;
}

CSyncVariantServer* CCharacterDictator::GetSpeedVarint() const
{
	return m_pSyncVariantAttribute1_Speed;
}

CSyncVariantServer* CCharacterDictator::GetMasterIDVariant() const
{
	return m_pViewVariantMaster_MasterID;
}

CSyncVariantServer* CCharacterDictator::GetMasterTeamIDVariant() const
{
	return m_pViewVariantMaster_MasterTeamID;
}

CSyncVariantServer* CCharacterDictator::GetMasterTroopIDVariant() const
{
	return m_pViewVariantMaster_MasterTroopID;
}

CSyncVariantServer* CCharacterDictator::GetMasterTongIDVariant() const
{
	return m_pViewVariantMaster_MasterTongID;
}

CSyncVariantServer* CCharacterDictator::GetStealthVariant() const
{
	return m_pSyncVariantAttribute1_Stealth;
}

CSyncVariantServer* CCharacterDictator::GetAlertTargetIDVariant() const
{
	return m_pSyncVariantAlert_AlertTargetID;
}

CSyncVariantServer* CCharacterDictator::GetAlertActionVariant() const
{
	return m_pSyncVariantAlert_AlertAction;
}

CSyncVariantServer*	CCharacterDictator::GetExpOwnerIDVariant() const
{
	return m_pSyncVariantAttribute1_ExpOwnerID;
}

bool CCharacterDictator::TruckCanBeTakeOver(CCharacterDictator* pNewMaster)
{
	if (GetNpcAIHandler())
	{
		CTruckAI* pTruckAI = GetNpcAIHandler()->CastToTruckAI();
		if (pTruckAI && pTruckAI->CanBeTakeOver(pNewMaster))
			return true;
	}
	return false;
}

void CCharacterDictator::TakeOverTruck(CCharacterDictator* pTruck)
{
	if (!pTruck || !pTruck->GetNpcAIHandler())
		return;
	CCharacterDictator* pTruckOldMaster = pTruck->GetMaster();
	if (pTruckOldMaster)
		pTruckOldMaster->RemoveServantFromServantMgr(pTruck->GetEntityID());
	CTruckAI* pTruckAI = pTruck->GetNpcAIHandler()->CastToTruckAI();
	if (pTruckAI && pTruckAI->CanBeTakeOver(this))
	{
		pTruckAI->TakeOverByNewMaster(this);
		pTruck->GetCallbackHandler()->TakeOverTruckByNewMaster(this, pTruck);
	}
}

void CCharacterDictator::ChangeTruckState(uint8 uState)
{
	if (GetNpcAIHandler())
	{
		CTruckAI* pTruckAI = GetNpcAIHandler()->CastToTruckAI();
		if (pTruckAI)
			pTruckAI->SetTruckFollowState((ETruckState)uState);
	}
}

void CCharacterDictator::SetNpcSkillLevel(uint32 uSkillLevel)
{
	m_uSkillLevel = uSkillLevel;
}

uint32 CCharacterDictator::GetNpcSkillLevel()
{
	return m_uSkillLevel;
}

void CCharacterDictator::ClearNpcAlertEffect()
{
	Ast (GetCallbackHandler());
	GetCallbackHandler()->ClearNpcAlertEffect(this);
}

void CCharacterDictator::ChaseBack()
{
	if (GetNpcAIHandler())
	{
		GetNpcAIHandler()->MasterNotifyLeaveBattle();
		GetNpcAIHandler()->ClearAllEnmity();
	}
}

uint32 CCharacterDictator::GetNpcDoSkillLevel()
{
	if (GetNpcAIHandler())
		return GetNpcAIHandler()->GetNpcDoSkillLevel();
	return 1;
}

uint32 CCharacterDictator::GetDefence()
{
	Ast(GetFighter());
	return GetFighter()->GetDefence();
}

uint32 CCharacterDictator::GetNatureResistance()
{
	Ast(GetFighter());
	return GetFighter()->GetNatureResistance();
}

uint32 CCharacterDictator::GetDestructionResistance()
{
	Ast(GetFighter());
	return GetFighter()->GetDestructionResistance();
}

uint32 CCharacterDictator::GetEvilResistance()
{
	Ast(GetFighter());
	return GetFighter()->GetEvilResistance();
}

void CCharacterDictator::SetDoSpecialActionProbability(uint32 uProbability)
{
	Ast (GetNpcAIHandler());
	GetNpcAIHandler()->SetDoSpecialActionProbability(uProbability);
}

void CCharacterDictator::ClearAllEnmity()
{
	Ast (GetNpcAIHandler());
	GetNpcAIHandler()->ClearAllEnmity();
}
