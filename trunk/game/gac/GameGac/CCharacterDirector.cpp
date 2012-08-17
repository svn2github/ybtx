#include "stdafx.h"
#include "CCharacterDirector.h"
#include "CRenderSystemClient.h"
#include "ICharacterDirectorCallbackHandler.h"
#include "CCoreObjectDirector.h"
#include "CFighterDirector.h"
#include "PtrlGac2GasCPPDef.h"
#include "CConnClient.h"
#include "CEntityClient.inl"
#include "CFacingMgr.h"
#include "CEntityClientManager.h"
#include "ICharacterDirectorHandler.h"
#include "ICharacterFollowerHandler.h"
#include "CCharacterFollower.inl"
#include "ICoreObjectFollowerHandler.h"
#include "CAutoTracker.h"
#include "IFighterFollowerHandler.h"
#include "IRenderObjClientHandler.h"
#include "TCharacterAdapterClient.h"
#include "IActorDirectorHandler.h"
#include "IFighterDirectorHandler.h"
#include "TEntityAdapterClient.inl"
#include "CNormalAttackCfgClient.h"
#include "CSyncVariantClient.h"
#include "CoreCommon.h"
#include "CFacialAniCfgMgr.h"
#include "CAllStateMgrClient.h"
#include "CAppClient.h"
#include "CRelationMgrClient.h"
#include "CSoundPlayer.h"
#include "CNPCGazedAtMgr.h"
#include "CDisplayHurt.h"
#include "CCharAniController.h"
#include "CCoreSceneClient.h"
#include "CServantClientMgr.h"
#include "TServantMgr.inl"
#include "CEffectLoader.h"
#include "TCharacterAdapterClient.inl"

CCharacterDirector::CCharacterDirector(CCoreObjectDirector* pCoreObj, uint32 uID)
:m_fRealCurMoveSpeed(0)
,m_pMoveAccelerator(NULL)
,m_pAutoTracker(new CAutoTracker)
,m_uTargetID(0)
,m_uTabDistance(0)
,m_bTransfer(false)
,m_bMovingByKeyboard(false)
,m_pServantMgr(NULL)
{
	Init<CCharacterDirectorTraits>(pCoreObj, uID);
	Ver(const_cast<CEntityClientManager*>(CEntityClientManager::GetInst())->AddGameEntity(this, GetEntityID()));
	const_cast<CEntityClientManager*>(CEntityClientManager::GetInst())->SetDirectorID(uID);
	m_pNPCGazedAtMgr = new CNPCGazedAtMgr(this);
	pCoreObj->GetGridPos(m_LastPosition);
};

CCharacterDirector::~CCharacterDirector()
{
	if (m_pServantMgr)
		SafeDelete(m_pServantMgr);
	SafeDelete(m_pNPCGazedAtMgr);
	SafeDelete(m_pMoveAccelerator);
	SafeDelete(m_pAutoTracker);
}

CCharacterFollower* CCharacterDirector::GetCharacterFromAdapter(ICoreObjectFollowerHandler* pHandler)const
{
	if(pHandler->CastToObjDirHandler() == NULL)
	{
		return class_cast<CCharacterFollowerAdapter*>(pHandler)->GetCharacter();
	}
	else
	{
		return class_cast<CCharacterDirectorAdapter*>(pHandler)->GetCharacter();
	}
}

CActorDirector* CCharacterDirector::GetActor()const 
{
	return class_cast<CActorDirector*>(CEntityClient::GetActor()); 
}

void CCharacterDirector::DoMoveState(EMoveState eMoveState, bool bForce)
{		
	if (GetActor()->DoMoveState(eMoveState, bForce))
		GetActor()->DoMoveAnimation();
}

void CCharacterDirector::DoActionState(EActionState eActionState)
{
	GetActor()->DoActionState(eActionState);
}

void CCharacterDirector::DoFunActionState(TCHAR* sFicialName)
{
	CFacialAniCfgMgr* facialCfg = CFacialAniCfgMgr::GetFicialAniCfgByName(sFicialName);
	uint8 uIndex = facialCfg->GetFacialIndex();
	GetActor()->DoActionState(eAS_Idle_BackWpn);
	if(!facialCfg->GetStartAniName().empty())
		GetActor()->DoFunActionState(eAS_Fun_Start,uIndex);
	else
		GetActor()->DoFunActionState(eAS_Fun_Keep,uIndex);
}

void CCharacterDirector::EndFunActionState()
{
	GetActor()->EndFunActionState();
}

EMoveState CCharacterDirector::GetMoveState()
{
	return GetActor()->GetMoveState();
}

EActionState CCharacterDirector::GetActionState()
{
	return GetActor()->GetActionState();
}

bool CCharacterDirector::IsEnemy(CCharacterFollower* pChar)
{
	return CRelationMgrClient::IsEnemy(GetFighter(), pChar->GetFighter());
}

bool CCharacterDirector::IsFriend(CCharacterFollower* pChar)
{
	return CRelationMgrClient::IsFriend(GetFighter(), pChar->GetFighter());
}

bool CCharacterDirector::IsNeutral(CCharacterFollower* pChar)
{
	return CRelationMgrClient::IsNeutral(GetFighter(), pChar->GetFighter());
}

void CCharacterDirector::SetMHNACfg(const TCHAR* szName,float fMainHandWeaponInterval,const TCHAR* MHWeaponType, const TCHAR* szAttackType)
{
	GetFighter()->CastToFighterDirector()->SetMHNACfg(szName, fMainHandWeaponInterval, CNormalAttackCfgClient::ms_mapWeaponType[MHWeaponType], CNormalAttackCfgClient::ms_mapAttackType[szAttackType]);
}

void CCharacterDirector::SetAHNACfg(const TCHAR* szName, float fAssistantWeaponInterval, const TCHAR* szWeaponType)
{
	//cout << "AHNACfg " << szWeaponType << endl;
	GetFighter()->CastToFighterDirector()->SetAHNACfg(szName, fAssistantWeaponInterval, CNormalAttackCfgClient::ms_mapWeaponType[szWeaponType]);
}

EConsumeType CCharacterDirector::GetConsumeType(const TCHAR* SkillName)
{
	return GetFighter()->CastToFighterDirector()->GetConsumeType(SkillName);
}
uint32 CCharacterDirector::GetConsumeValue(const TCHAR* SkillName, uint32 SkillLevel)
{
	return GetFighter()->CastToFighterDirector()->GetConsumeValue(SkillName, SkillLevel);
}
uint32 CCharacterDirector::GetSkillDistance(const TCHAR* SkillName, uint32 SkillLevel)
{
	return GetFighter()->CastToFighterDirector()->GetSkillDistance(SkillName, SkillLevel);
}
ECastingProcessType CCharacterDirector::GetCastingProcessType(const TCHAR* SkillName)
{
	return GetFighter()->CastToFighterDirector()->GetCastingProcessType(SkillName);
}
float CCharacterDirector::GetCastingProcessTime(const TCHAR* SkillName, uint32 SkillLevel)
{
	return GetFighter()->CastToFighterDirector()->GetCastingProcessTime(SkillName, SkillLevel);
}
uint32 CCharacterDirector::GetSkillCoolDownType(const TCHAR* SkillName)
{
	return GetFighter()->CastToFighterDirector()->GetSkillCoolDownType(SkillName);
}
float CCharacterDirector::GetCoolDownTime(const TCHAR* SkillName, uint32 SkillLevel)
{
	return GetFighter()->CastToFighterDirector()->GetCoolDownTime(SkillName, SkillLevel);
}
uint32 CCharacterDirector::GetSkillLeftCoolDownTime(const TCHAR* SkillName)
{
	return GetFighter()->CastToFighterDirector()->GetSkillLeftCoolDownTime(SkillName);
}
bool CCharacterDirector::IsInCommonCD(const TCHAR* SkillName)
{
	return GetFighter()->CastToFighterDirector()->IsInCommonCD(SkillName);
}

bool CCharacterDirector::IsSwitchEquipSkill(const TCHAR* SkillName)
{
	return GetFighter()->CastToFighterDirector()->IsSwitchEquipSkill(SkillName);
}
uint32 CCharacterDirector::GetFireCondition(const TCHAR* SkillName)
{
	return GetFighter()->CastToFighterDirector()->GetFireCondition(SkillName);
}

float CCharacterDirector::GetNormalAttackDistance() const
{
	CFighterDirector* pFighter = class_cast<CFighterDirector*>(GetFighter());
	if (!pFighter)
		return 0.0f;

	return pFighter->GetNormalAttackDistance();
}

uint32 CCharacterDirector::GetTabDistance()
{
	return m_uTabDistance;
}

void CCharacterDirector::CancelCastingProcess()
{
	GetFighter()->CastToFighterDirector()->CancelCastingProcess();
}

void CCharacterDirector::AddLearnSkill(const TCHAR* SkillName,uint32 SkillLevel)
{
	GetFighter()->CastToFighterDirector()->AddLearnSkill(SkillName,SkillLevel);
}

void CCharacterDirector::DelLearnSkill(const TCHAR* SkillName,uint32 SkillLevel)
{
	GetFighter()->CastToFighterDirector()->DelLearnSkill(SkillName,SkillLevel);
}

void CCharacterDirector::ClearAllLearnSkill()
{
	GetFighter()->CastToFighterDirector()->ClearAllLearnSkill();
}

EDoSkillResult CCharacterDirector::FightSkill(const TCHAR* szName, uint8 uSkillLevel)
{
	EndFunActionState();
	return GetFighter()->CastToFighterDirector()->FightSkill(szName, uSkillLevel,0);
}

void CCharacterDirector::UseTempSkill(const TCHAR* szName, uint8 uSkillLevel, uint32 uPos)
{
	EndFunActionState();
	GetFighter()->CastToFighterDirector()->FightSkill(szName, uSkillLevel, uPos);
}


void CCharacterDirector::DoAttack(const TCHAR* szSkillName, uint8 uSkillLevel, uint32 uPos, uint32 uDir, uint32 uMagicDir)
{
	EndFunActionState();
	GetFighter()->CastToFighterDirector()->DoAttack(szSkillName, uSkillLevel, uPos, uDir, uMagicDir);
}

void CCharacterDirector::CancelCoerceMove(uint32 uID)
{
	GetFighter()->CastToFighterDirector()->CancelCoerceMove(uID);
}

bool CCharacterDirector::DoNormalAttack(uint32 uTargetID, bool bIsNeedAutoTracker)
{
	return class_cast<CFighterDirector*>(GetFighter())->DoNormalAttack(uTargetID, bIsNeedAutoTracker);
}

bool CCharacterDirector::CheckDoNormalAttack(uint32 uTargetID)
{
	return class_cast<CFighterDirector*>(GetFighter())->CheckDoNormalAttack(uTargetID);
}

CFighterFollower* CCharacterDirector::GetSelectedFighter()const
{
	return GetSelectedEntity(CCoreSceneClient::MainScene())?GetSelectedEntity(CCoreSceneClient::MainScene())->GetFighter():NULL;
}

void CCharacterDirector::SetTarget(uint32 uTargetID)
{
	//cout << "SetTarget " << uTargetID << endl;
	if(uTargetID == 0)
	{
		m_uTargetID = uTargetID;
		GetFighter()->SetTarget(NULL);
		return;
	}

	CEntityClient* pEntityClient = CEntityClientManager::GetEntityByID(uTargetID);
	if(pEntityClient != NULL)
	{
		m_uTargetID = uTargetID;
		LockObj(uTargetID);
		
		if(pEntityClient->GetGameEntityType() == eGET_CharacterDirector || pEntityClient->GetGameEntityType() == eGET_CharacterFollower)
		{
			GetFighter()->SetTarget( class_cast<CFighterFollower*>(pEntityClient->GetFighter()));
			//if(GetFighter()->CastToFighterDirector()->IsAutoNormalAttack())
			//{
			//	DoNormalAttack(uTargetID);
			//}
		}
	}
	else
	{
		m_uTargetID = 0;
		GetFighter()->SetTarget(NULL);
	}
}

CEntityClient* CCharacterDirector::GetTarget()const
{
	if(m_uTargetID == 0)
		return NULL;
	return CEntityClientManager::GetEntityByID(m_uTargetID);
}

void CCharacterDirector::CreateAccelerator()
{
	if(IsMoving())
	{
		if(!m_pMoveAccelerator)
		{
			SetCurSpeed(GetSpeed());
		}
		return;
	}

	if (GetActionState() == eAS_Sing)
		DoMoveState(eMS_Move, true);

	//SafeDelete(m_pMoveAccelerator);

	if(!IsPassive() && !m_pMoveAccelerator)
	{
		//cout << "开始加速" << endl;
		m_pMoveAccelerator = new CMoveAccelerator(this);
		if(m_pMoveAccelerator->AccelerateSpeed())
		{
			CAppClient::Inst()->RegisterTick(m_pMoveAccelerator, 33);
		}
		else
		{
			m_pMoveAccelerator = NULL;
		}
	}
}

void CCharacterDirector::OnMoveBegan()
{
	EndFunActionState();
	CCharacterFollower::OnMoveBegan();
}

void CCharacterDirector::OnMoveEnded(EMoveEndedType eMEType)
{
	if (!IsMovingByKeyboard())
	{
		CCharacterFollower::OnMoveEnded(eMEType);
	}
	CFighterDirector* pFighter = class_cast<CFighterDirector*>(GetFighter());

	ICharacterDirectorCallbackHandler* pCallbackHandler = pFighter->GetCallBackHandler();
	if(pCallbackHandler) 
	{
		if (eMET_Reached == eMEType)
		{
			pCallbackHandler->OnMoveSuccessed();
		}
		else
		{
			pCallbackHandler->OnMoveStopped();
		}
	}
}

void CCharacterDirector::OnDestroy()
{
	if (m_bTransfer)
	{
		GetFighter()->Notify(eCE_ChangeMapEnd);
		GetFighter()->CastToFighterDirector()->YieldTick();
		GetActor()->ClearVariantCallbackFunc();
		SafeDelete(m_pIKCtrl);
		GetRenderLinkMgr()->RemoveRenderObject();
		m_pCoreObj->SetHandler( NULL );
		m_uCoreObjID = 0;
		m_pCoreObj=NULL;
	}
	else
	{
		CCharacterFollower* pServant = GetServant(ENpcType_BattleHorse);
		if(pServant && pServant->IsBattleHorse())
			pServant->OnDestroy();

		CCharacterFollower::OnDestroy();
		const_cast<CEntityClientManager*>(CEntityClientManager::GetInst())->SetDirectorID(0);
	}
}

void CCharacterDirector::OnTransfered(CCoreObjectFollower* pCoreObj)
{
	m_bTransfer=false;
	m_pCoreObj=pCoreObj;
	m_uCoreObjID = pCoreObj->GetGlobalID();

	CCharacterDirectorAdapter* pAdapter=class_cast<CCharacterDirectorAdapter*>(GetFighter()->GetHandler());
	pCoreObj->SetHandler( pAdapter );
	for (uint32 i=0; i<(uint32)eCTI_End; i++)
	{
		pCoreObj->SetViewVariantHandler(m_vecViewVariantHandler[i], (ECodeTableIndex)i);
	}
	for (uint32 i=0; i<(uint32)eCTI_End; i++)
	{
		pCoreObj->SetSyncVariantHandler(m_vecSyncVariantHandler[i], (ECodeTableIndex)i);
	}
	GetActor()->RegVariantCallbackFunc();
	GetRenderLinkMgr()->InitRenderObject();
	GetRenderLinkMgr()->UpdatePos(pCoreObj->GetPixelPos());

	if (GetCallbackHandler())
			GetCallbackHandler()->OnTransfered(this);

	DoMoveState(eMS_Stop);
	GetFighter()->GetAllStateMgr()->ReplayFx();
	//恢复所有与CharacterDirector有关的tick
	GetFighter()->CastToFighterDirector()->ResumeTick();
}

void CCharacterDirector::OnDead()
{
	CCharacterFollower::OnDead();

	CFighterDirector* pFighter = class_cast<CFighterDirector*>(GetFighter());

	if(pFighter->GetCallBackHandler() && pFighter->GetCallBackHandler()->GroundSelector_IsActive())
		pFighter->GetCallBackHandler()->GroundSelector_CancelSelectGround();
	
}

void CCharacterDirector::OnMagicCondFail(uint32 uMessageID)
{
	CFighterDirector* pFighter = class_cast<CFighterDirector*>(GetFighter());

	if(pFighter->GetCallBackHandler()) pFighter->GetCallBackHandler()->OnMagicCondFail(uMessageID,"");
	
}
void CCharacterDirector::OnPosChanged()
{
	CCharacterFollower::OnPosChanged();
	CPos NewPos;
	GetGridPos(NewPos);
	if (NewPos!=m_LastPosition)
	{
		m_LastPosition = NewPos;
		CFighterDirector* pFighter = class_cast<CFighterDirector*>(GetFighter());

		if(pFighter->GetCallBackHandler()) pFighter->GetCallBackHandler()->OnGridPosChange();
	}
}

void CCharacterDirector::LockObj(uint32 uLockObjId)
{
	CGac2GasCPP_LockObj cmd;
	cmd.uLockObjId = uLockObjId;
	CConnClient::Inst()->SendCmd(&cmd);
}
void CCharacterDirector::OpenObjPanel(uint32 uObjID, CFighterViewInfo* pViewInfo, bool bIsTargetPanel, bool IsOpen)
{
	if (IsOpen)
		GetFighter()->CastToFighterDirector()->SaveObjPanelByID(uObjID, pViewInfo, bIsTargetPanel);
	else
		GetFighter()->CastToFighterDirector()->DeleteObjPanelByID(uObjID, bIsTargetPanel);
}

bool CCharacterDirector::IsCanCancelState(const TCHAR*  uName)
{
	return GetFighter()->IsCanCancelState(uName);
}

bool CCharacterDirector::ExistState(const TCHAR*  uName)
{
	return GetFighter()->ExistState(uName);
}

void CCharacterDirector::CancelState(uint32 uID)
{
	class_cast<CFighterDirector*>(GetFighter())->CancelState(uID);
}

void CCharacterDirector::DestroyServant()
{
	class_cast<CFighterDirector*>(GetFighter())->DestroyServant();
}

//void CCharacterDirector::SetAutoNormalAttack(bool bIsAutoNormalAttack)
//{
//	GetFighter()->CastToFighterDirector()->SetAutoNormalAttack(bIsAutoNormalAttack);
//}

//bool CCharacterDirector::IsAutoNormalAttack()
//{
//	return GetFighter()->CastToFighterDirector()->IsAutoNormalAttack();
//}

bool CCharacterDirector::IsNormalAttacking() const
{
	CFighterDirector * pFighterDirector = class_cast<CFighterDirector*>(GetFighter());
	return pFighterDirector->IsNormalAttacking();
}

void CCharacterDirector::CancelNormalAttack()
{
	CFighterDirector * pFighterDirector = class_cast<CFighterDirector*>(GetFighter());
	if (GetFighter()->GetTarget() && IsNormalAttacking())
	{
		pFighterDirector->TellMedDoNormalAttack(GetFighter()->GetTarget()->GetEntityID(), false, false);
	}
	CancelAutoTracker(true);
	pFighterDirector->CancelNormalAttack();
	ChangeNormalAttacking(false);
}

uint8 CCharacterDirector::GetActionDir()
{
	return GetActor()->GetActionDir();
}

CCharacterFollower*	CCharacterDirector::GetNormalAttackTarget()
{
	CFighterDirector* pFighterDirector = class_cast<CFighterDirector*>(GetFighter());
	return pFighterDirector->GetNormalAttackTarget();
}

void CCharacterDirector::SetTabDistance(uint32 uTabDistance)
{
	m_uTabDistance = uTabDistance;
}

bool CCharacterDirector::IsCastingTypeSkill(const TCHAR* szSkillName)
{
	return GetFighter()->CastToFighterDirector()->IsCastingTypeSkill(szSkillName);
}

bool CCharacterDirector::CanUseNonFightSkill(const TCHAR* szSkillName, uint8 uSkillLevel)
{
	uint32 Ret =  GetFighter()->CastToFighterDirector()->PreAttack(szSkillName, uSkillLevel);
	if (Ret == eDSR_Success || Ret == eDSR_ForbitUseSkill || Ret == eDSR_Fail)
	{	//ItemSkill可以在ForbitUseSkill时使用,现在只有战斗中使用非战斗技能是Fail,这里认为可以用
		return true;
	}
	return false;
}

bool CCharacterDirector::CanUseSkill(const TCHAR* szSkillName, uint8 uSkillLevel)
{
	uint32 Ret =  GetFighter()->CastToFighterDirector()->PreAttack(szSkillName, uSkillLevel);
	if (Ret == eDSR_Success)
	{
		return true;
	}
	return false;
}

void CCharacterDirector::TurnAroundByNonFightSkill(const TCHAR* szSkillName)
{
	GetFighter()->CastToFighterDirector()->TurnAroundByNonFightSkill(szSkillName);
}

void CCharacterDirector::SetTargetPos(const CFPos& PixelPos)
{
	GetFighter()->CastToFighterDirector()->SetTargetPos(PixelPos);
}

void CCharacterDirector::AddCharacterFollowerToSet(CCharacterFollower* pCharacter)
{
	//所有向list加Follower的都要调用这个,否则Follower析构时,可能会漏了从list删除,只有这里会Attach(eCE_Offline)
	if (!pCharacter)
		return;
	uint32 uCharEntityID = pCharacter->GetEntityID();
	if (uCharEntityID == GetEntityID() || !pCharacter->CppIsAlive())
	{
		return;
	}
	if (CRelationMgrClient::IsFriend(GetFighter(), pCharacter->GetFighter()))
	{
		list<uint32>::iterator iter = m_FriendCharacterFollower.begin();
		for(; iter != m_FriendCharacterFollower.end(); ++iter)
		{
			if ((*iter) == uCharEntityID)
			{
				return;
			}
		}
		InsertTargetToFollwerList(pCharacter,m_FriendCharacterFollower);
	}
	else if (CRelationMgrClient::IsEnemy(GetFighter(), pCharacter->GetFighter()))
	{
		list<uint32>::iterator Aimiter = m_AimCharacterFollower.begin();
		for(; Aimiter != m_AimCharacterFollower.end(); ++Aimiter)
		{
			if ((*Aimiter) == uCharEntityID)
			{
				return;
			}
		}
		InsertTargetToFollwerList(pCharacter,m_AimCharacterFollower);
	}
	else if (!CRelationMgrClient::IsNeutral(GetFighter(), pCharacter->GetFighter()))
	{
		list<uint32>::iterator Otheriter = m_OtherCharacterFollower.begin();
		for(; Otheriter != m_OtherCharacterFollower.end(); ++Otheriter)
		{
			if ((*Otheriter) == uCharEntityID)
			{
				return;
			}
		}
		pCharacter->GetFighter()->Attach(this,eCE_Offline);
		m_OtherCharacterFollower.push_back(uCharEntityID);
	}
}

void CCharacterDirector::InsertTargetToFollwerList(CCharacterFollower* pCharacter, list<uint32>&listCharacterFollower)
{
	pCharacter->GetFighter()->Attach(this,eCE_Offline);
	//优先玩家 //优先朝向180度内的 //优先距离近的
	const CSyncVariantClient* pViewVariant = pCharacter->GetViewVariant();
	const CSyncVariantClient* pAttributeVariant = pViewVariant->GetMember("Attribute");
	uint32 pCharType = pAttributeVariant->GetMember("Type")->GetNumber<uint8>();
	
	CFPos posF;
	CFPos posT;
	posF = GetPixelPos();
	posT = pCharacter->GetPixelPos();
	float MaxDistance = GetEntityDist(pCharacter);
	list<uint32>::iterator iter = listCharacterFollower.begin();
	list<uint32>::iterator Distanceiter = listCharacterFollower.begin();
	for (; Distanceiter != listCharacterFollower.end();)
	{
		CCharacterFollower* pCharacterTemp = CCharacterFollower::GetCharacterByID(*Distanceiter);
		if (pCharacterTemp)
		{
			const CSyncVariantClient* pCharViewVariant = pCharacterTemp->GetViewVariant();
			const CSyncVariantClient* pCharAttributeVariant = pCharViewVariant->GetMember("Attribute");
			uint32 pType = pCharAttributeVariant->GetMember("Type")->GetNumber<uint8>();
			if (pCharType == pType)
			{
				iter = Distanceiter;
				float Distance = GetEntityDist(pCharacterTemp);
				if (Distance >= MaxDistance)
				{
					break;	
				}
			}
		}
		Distanceiter++;
	}
	uint32 uCharEntityID = pCharacter->GetEntityID();
	if (iter != listCharacterFollower.begin())
	{
		listCharacterFollower.insert(iter,uCharEntityID);
	}
	else if (pCharType == 1)
	{
		listCharacterFollower.push_front(uCharEntityID);
	}
	else
	{
		listCharacterFollower.push_back(uCharEntityID);
	}
}

void CCharacterDirector::DelCharacterFollowerFromSet(CCharacterFollower* pCharacter)
{
	uint32 uCharEntityID = pCharacter->GetEntityID();
	m_FriendCharacterFollower.remove(uCharEntityID);
	m_AimCharacterFollower.remove(uCharEntityID);
	m_OtherCharacterFollower.remove(uCharEntityID);
}

void CCharacterDirector::SortTargetList()
{
	SortTargetListOnList(m_AimCharacterFollower);
	//SortTargetListOnList(m_FriendCharacterFollower);
}

void CCharacterDirector::SortTargetListOnList(list<uint32>&listCharacterFollower)
{
	//优先玩家 //优先朝向180度内的 //优先距离近的
	CCharacterFollower* pCharacterTempFol = NULL;
	list<uint32> TempFrontPlayerFolList;
	list<uint32> TempFrontNPCFolList;
	uint32 uListSize = listCharacterFollower.size();
	for (uint32 i=1; i<=uListSize; i++)
	{
		pCharacterTempFol = NULL;
		float MaxDistance = 0.0f;
		list<uint32>::iterator Distanceiter = listCharacterFollower.begin();
		for (; Distanceiter != listCharacterFollower.end();)
		{
			CCharacterFollower* pCharacter = CCharacterFollower::GetCharacterByID(*Distanceiter);
			if (pCharacter)
			{
				float Distance = GetEntityDist(pCharacter);
				if (Distance >= MaxDistance)
				{
					MaxDistance = Distance;
					pCharacterTempFol = pCharacter;//MaxDistance CharacterFollower
				}
			}
			Distanceiter++;
		}

		list<uint32>::iterator iter = listCharacterFollower.begin();
		for (; iter!=listCharacterFollower.end();)
		{
			CCharacterFollower* pCharacter = CCharacterFollower::GetCharacterByID(*iter);
			if (pCharacter && pCharacter==pCharacterTempFol)
			{
				listCharacterFollower.erase(iter++);
				break;
			}
			iter++;
		}
		if (pCharacterTempFol)
		{
			CDir OwnDir,TargetDir;
			GetDirection(OwnDir);
			CVector2f pVec2f;
			OwnDir.Get(pVec2f);
			CFPos posFrom;
			CFPos posTarget;
			posFrom = GetPixelPos();
			posTarget = pCharacterTempFol->GetPixelPos();
			float x = posTarget.x - posFrom.x;
			float y = posTarget.y - posFrom.y ;
			//朝向优先级
			uint32 uType = pCharacterTempFol->GetViewVariant()->GetMember("Attribute")->GetMember("Type")->GetNumber<uint8>();
			if (uType == 1)
			{
				TempFrontPlayerFolList.push_front(pCharacterTempFol->GetEntityID());
			}
			else
			{
				TempFrontNPCFolList.push_front(pCharacterTempFol->GetEntityID());
			}
		}
	}
	list<uint32>::iterator FrontPlayeriter = TempFrontPlayerFolList.begin();
	for (; FrontPlayeriter!=TempFrontPlayerFolList.end();)
	{
		listCharacterFollower.push_back((*FrontPlayeriter++));
	}
	list<uint32>::iterator FrontNPCiter = TempFrontNPCFolList.begin();
	for (; FrontNPCiter!=TempFrontNPCFolList.end();)
	{
		listCharacterFollower.push_back((*FrontNPCiter++));
	}

	TempFrontPlayerFolList.clear();
	TempFrontNPCFolList.clear();
}

void CCharacterDirector::SelectFriendTarget()
{
	if (m_FriendCharacterFollower.empty())
	{
		return;
	}
	list<uint32>::iterator Frienditer = m_FriendCharacterFollower.begin();
	for (; Frienditer != m_FriendCharacterFollower.end();)
	{
		CCharacterFollower* pCharacter = CCharacterFollower::GetCharacterByID(*Frienditer);
		if (!pCharacter)
		{
			Frienditer++;
			continue;
		}
		float Distance = GetEntityDist(pCharacter);
		if(!pCharacter->CppIsAlive() || !pCharacter->CanBeSelected() || Distance > GetTabDistance()*eGridSpanForObj)
		{
			Frienditer++;
			continue;
		}
		if(GetFighter()->GetCallBackHandler())
		{
			//判断Npc是否可以被选中
			if(pCharacter->GetNpcType() != ENpcType_None && GetFighter()->GetCallBackHandler()->CheckIsCanLock(pCharacter) == false)
			{
				Frienditer++;
				continue;
			}
			GetFighter()->GetCallBackHandler()->SetLockObj(GetEntityID(),pCharacter->GetEntityID());
		}

		m_FriendCharacterFollower.push_back((*Frienditer));
		m_FriendCharacterFollower.erase(Frienditer++);
		break;
	}
}

void CCharacterDirector::SelectAimTarget()
{
	if (m_AimCharacterFollower.empty())
	{
		return;
	}
	list<uint32>::iterator Aimiter = m_AimCharacterFollower.begin();
	for (; Aimiter != m_AimCharacterFollower.end();)
	{
		CCharacterFollower* pCharacter = CCharacterFollower::GetCharacterByID(*Aimiter);
		if (!pCharacter || (CppGetCtrlState(eFCS_InDuel) && !pCharacter->CppGetCtrlState(eFCS_InDuel)))
		{
			Aimiter++;
			continue;
		}		
		float Distance = GetEntityDist(pCharacter);
		if(!pCharacter->CppIsAlive() || !pCharacter->CanBeSelected() || Distance > GetTabDistance()*eGridSpanForObj )
		{
			Aimiter++;
			continue;
		}

		if(GetFighter()->GetCallBackHandler())
		{
			//判断Npc是否可以被选中
			if(pCharacter->GetNpcType() != ENpcType_None && GetFighter()->GetCallBackHandler()->CheckIsCanLock(pCharacter) == false)
			{
				Aimiter++;
				continue;
			}
			GetFighter()->GetCallBackHandler()->SetLockObj(GetEntityID(),pCharacter->GetEntityID());
		}

		m_AimCharacterFollower.push_back((*Aimiter));
		m_AimCharacterFollower.erase(Aimiter++);
		break;
	}
}

void CCharacterDirector::RefreshRelationList()
{
	list<uint32>	listAll = m_FriendCharacterFollower;
	m_FriendCharacterFollower.clear();
	listAll.splice(listAll.end(), m_AimCharacterFollower);
	listAll.splice(listAll.end(), m_OtherCharacterFollower);

	list<uint32>::iterator it = listAll.begin();
	for(; it != listAll.end(); ++it)
	{
		CCharacterFollower* pCharacter = CCharacterFollower::GetCharacterByID(*it);
		AddCharacterFollowerToSet(pCharacter);
		pCharacter->SetHeadBloodTex(this);
	}
}

void CCharacterDirector::ChangeCharacterFollowerList(CCharacterFollower* pCharacter)
{
	uint32 uCharEntityID = pCharacter->GetEntityID();
	if (uCharEntityID == GetEntityID())
		return;
	m_FriendCharacterFollower.remove(uCharEntityID);
	m_AimCharacterFollower.remove(uCharEntityID);
	m_OtherCharacterFollower.remove(uCharEntityID);
	AddCharacterFollowerToSet(pCharacter);
	pCharacter->SetHeadBloodTex(this);
}

void CCharacterDirector::OnCOREvent(CPtCORSubject* pSubject,uint32 uEvent,void* pArg)
{
	switch(uEvent)
	{
	case eCE_Offline:
		{
			CFighterFollower* pFighterFollower = (CFighterFollower*)pSubject;
			if(pFighterFollower)
			{
				CCharacterFollower* pCharacterFollower = pFighterFollower->GetCharacter();
				DelCharacterFollowerFromSet(pCharacterFollower);
			}
		}
		break;
	}
}

void CCharacterDirector::TurnAndSyncDir(uint8 uDir)
{
	GetActor()->TurnAndSyncDir(uDir);
}

void CCharacterDirector::ChangeMoveMode(bool bIsWalkMode)
{
	if (GetFighter()->GetCtrlState(eFCS_InBattle))
		return;

	CGac2GasCPP_ChangeMoveMode Cmd;
	Cmd.bIsWalkMode = bIsWalkMode;
	CConnClient::Inst()->SendCmd(&Cmd);
}

void CCharacterDirector::ChangeDrawWeaponMode(bool bForceDraw)
{
	if ( GetFighter()->GetCtrlState(eFCS_OnMission) 
		|| GetFighter()->GetCtrlState(eFCS_InBattle) 
		|| GetFighter()->GetCtrlState(eFCS_InNormalHorse) 
		|| GetFighter()->GetCtrlState(eFCS_InBattleHorse)
		|| GetFighter()->CastToFighterDirector()->GetDoSkillCtrlState(eDSCS_InDoingSkill) )
		return;

	if (IsNormalAttacking())
		return;

	// 动作中不可重复拔收武器
	if (eAPP_SpecialAction == GetActor()->GetCurOnceActionLevel() || GetActor()->GetMoveState() == eMS_LockMove)
		return;

	bool bIsDrawWeaponMode = false;
	if (bForceDraw)
	{
		if (GetFighter()->GetCtrlState(eFCS_InDrawWeaponMode))
			return;

		bIsDrawWeaponMode = true;
	}
	else
		bIsDrawWeaponMode = GetFighter()->GetCtrlState(eFCS_InDrawWeaponMode) ? false : true;

	CGac2GasCPP_ChangeDrawWeaponMode Cmd;
	Cmd.bIsDrawWeaponMode = bIsDrawWeaponMode;
	Cmd.bForceDraw = bForceDraw;
	CConnClient::Inst()->SendCmd(&Cmd);
}

void CCharacterDirector::ChangeNormalAttacking(bool bIsNormalAttacking)
{
	CGac2GasCPP_ChangeNormalAttacking Cmd;
	Cmd.bIsNormalAttacking = bIsNormalAttacking;
	CConnClient::Inst()->SendCmd(&Cmd);
}

void CCharacterDirector::CreateFacingPair(CCharacterFollower* pCharSrc, CCharacterFollower* pCharDst)
{
	GetFacingMgr()->CreateFacingPair(pCharSrc, pCharDst);
}

void CCharacterDirector::RemoveFacingPair()
{
	GetFacingMgr()->RemoveFacingPair();
}

void CCharacterDirector::ChangeShowHurtMOD()
{
	CFontSprite::ms_bShowHurt = !(CFontSprite::ms_bShowHurt);
}

uint32 CCharacterDirector::GetMoveDir()
{
	return (uint32)GetActor()->GetMoveDir();
}

bool CCharacterDirector::ActionIsTurnOver()
{
	return IsTurnOver();
}

void CCharacterDirector::NotifyServerChangeDir(const CFPos& PixelPosDes)
{
	CDir dir;
	CVector2f vecDir;
	CFPos posCur;
	GetPixelPos(posCur);
	vecDir.x = PixelPosDes.x - posCur.x;
	vecDir.y = PixelPosDes.y - posCur.y;
	dir.Set(vecDir);


	CGac2GasCPP_MoveByDirection Cmd;
	Cmd.uObjID = GetEntityID();
	Cmd.uMoveDir = dir.uDir;
	CConnClient::Inst()->SendCmd(&Cmd);
}

void CCharacterDirector::GetTargetCastingProcess(uint32 uTargetID)
{
	GetFighter()->CastToFighterDirector()->GetTargetCastingProcess(uTargetID);
}

void CCharacterDirector::GetTargetOfTarget(uint32 uTargetID)
{
	GetFighter()->CastToFighterDirector()->GetTargetOfTarget(uTargetID);
}

bool CCharacterDirector::InitAutoTracker(uint32 uAttacker, uint32 uTarget, float fReachGridDist, IAutoTrackerHandler* pHandler, bool bIsNormalAttackTracker)
{
	return m_pAutoTracker->Init(uAttacker, uTarget, fReachGridDist, pHandler, bIsNormalAttackTracker);
}

void CCharacterDirector::CancelAutoTracker(bool bCancelNormalAttackOnly)
{
	if(IsAutoTracking())
	{
		if((bCancelNormalAttackOnly && m_pAutoTracker->IsNormalAttackTracker()) ||
			!bCancelNormalAttackOnly)
		{
			m_pAutoTracker->Cancel();
		}
	}
}

bool CCharacterDirector::IsNormalAttackTracker()
{
	if (IsAutoTracking())
		return m_pAutoTracker->IsNormalAttackTracker();
	return false;
}

bool CCharacterDirector::IsAutoTracking()const
{
	return m_pAutoTracker->IsAutoTracking();
}

void CCharacterDirector::ShowHeadBloodDialog(bool bShow)
{
	ShowBloodRendler(bShow);
	list<uint32>::iterator Frienditer = m_FriendCharacterFollower.begin();
	for (; Frienditer != m_FriendCharacterFollower.end(); ++Frienditer)
	{
		CCharacterFollower* pCharacter = CCharacterFollower::GetCharacterByID(*Frienditer);
		if (pCharacter)
			(pCharacter)->ShowBloodRendler(bShow);
	}
	list<uint32>::iterator Aimiter = m_AimCharacterFollower.begin();
	for(; Aimiter != m_AimCharacterFollower.end(); ++Aimiter)
	{
		CCharacterFollower* pCharacter = CCharacterFollower::GetCharacterByID(*Aimiter);
		if (pCharacter)
			(pCharacter)->ShowBloodRendler(bShow);
	}
	list<uint32>::iterator Otheriter = m_OtherCharacterFollower.begin();
	for (; Otheriter != m_OtherCharacterFollower.end(); ++Otheriter)
	{
		CCharacterFollower* pCharacter = CCharacterFollower::GetCharacterByID(*Otheriter);
		if (pCharacter)
			(pCharacter)->ShowBloodRendler(bShow);
	}
}

void CCharacterDirector::CppSetDoSkillCtrlStateForLua(EDoSkillCtrlState eState,bool bSet)
{
	GetFighter()->CastToFighterDirector()->SetDoSkillCtrlState(eState,bSet);
}

bool CCharacterDirector::CppGetDoSkillCtrlState(EDoSkillCtrlState eState)
{
	return GetFighter()->CastToFighterDirector()->GetDoSkillCtrlState(eState);
}

bool CCharacterDirector::BeInTransfer()
{
	return m_bTransfer;	
}

void CCharacterDirector::SetMovingByKeyboard(bool bMovingByKeyboard)
{
	m_bMovingByKeyboard = bMovingByKeyboard;
	if (!m_bMovingByKeyboard && !IsMoving())
	{
		DoMoveState(eMS_Stop);

		string strCueNameTail = "";
		if (GetFighter()->GetCtrlState(eFCS_InNormalHorse) || GetFighter()->GetCtrlState(eFCS_InBattleHorse))
			strCueNameTail = "4";
		CSoundPlayer::PlayFootstep(this, strCueNameTail);
	}
}

void CCharacterDirector::SetCurSpeed(float fSpeed)
{
	m_fRealCurMoveSpeed = fSpeed;
	GetActor()->DoNowAni(true);
}

float CCharacterDirector::GetCurSpeed()const
{
	return min(GetMaxSpeed(), m_fRealCurMoveSpeed);;
}

float CCharacterDirector::GetMaxSpeed()const
{
	CCoreObjectDirector* pCoreObj = class_cast<CCoreObjectDirector*>(GetCoreObj());
	if(pCoreObj)
		return pCoreObj->GetMaxSpeed();
	return 0;
}

void CCharacterDirector::StopMoving()
{
	CCoreObjectDirector* pCoreObj = class_cast<CCoreObjectDirector*>(GetCoreObj());
	if(pCoreObj)
	{
		//cout << "Director Stops moving" << endl;
		pCoreObj->StopMoving();
	}
} 

EMoveToResult CCharacterDirector::MoveToInAStarPath(const CFPos& PixelPosDes, EFindPathType eFindPathType,EBarrierType eBarrierType, float fReachDist, int32 nMaxStep)
{
	CCoreObjectDirector* pCoreObj = class_cast<CCoreObjectDirector*>(GetCoreObj());
	if(pCoreObj)
	{
		float fDirectorMaxSpeed = GetMaxSpeed();
		if(fDirectorMaxSpeed > 0)
		{
			CreateAccelerator();
			GetScene()->GetRenderScene()->SetCameraMaxSpeed(fDirectorMaxSpeed);
			return pCoreObj->MoveToLimit(PixelPosDes, GetCurSpeed(), eFindPathType, eBarrierType, fReachDist, 
nMaxStep);
		}
	}
	return eMTR_NotAllowed;
}

EMoveToResult CCharacterDirector::MoveToInLinePath(const CFPos& PixelPosDes, EFindPathType eFindPathType,EBarrierType eBarrierType)
{
	CCoreObjectDirector* pCoreObj = class_cast<CCoreObjectDirector*>(GetCoreObj());
	if(pCoreObj)
	{
		float fDirectorMaxSpeed = GetMaxSpeed();
		if(fDirectorMaxSpeed > 0)
		{
			CreateAccelerator();
			float fDirectorMaxSpeed = GetMaxSpeed();
			GetScene()->GetRenderScene()->SetCameraMaxSpeed(fDirectorMaxSpeed);
			return pCoreObj->MoveTo(PixelPosDes, GetCurSpeed(), eFindPathType, eBarrierType, 0);
		}
	}
	return eMTR_NotAllowed;
} 

ESetSpeedResult CCharacterDirector::SetCoreObjSpeed(float fSpeed)
{
	CCoreObjectDirector* pCoreObj = class_cast<CCoreObjectDirector*>(GetCoreObj());
	if(pCoreObj)
	{
		return pCoreObj->SetSpeed(fSpeed);
	}
	return eSSR_NotAllowed;
}

CCharacterFollower* CCharacterDirector::GetServant(ENpcType eType)const
{
	//客户端的宠物管理器必须保证每种类型的宠物只有一个!
	if (!m_pServantMgr)
		return NULL;
	CCharacterFollower* pServant = GetServantByType(eType);
	return pServant;
}

CCharacterFollower* CCharacterDirector::GetServantByID(uint32 uServant)
{
	if (!m_pServantMgr)
		return NULL;
	return m_pServantMgr->GetServantByID(uServant);
}

void CCharacterDirector::AddServant(ENpcType eType, uint32 uServantID)
{
	CCharacterFollower* pServant = CCharacterFollower::GetCharacterByID(uServantID);
	if (!pServant)
		return;
	if (!m_pServantMgr)
		m_pServantMgr=new CServantClientMgr(this);

	pServant->SetMaster(GetEntityID());
	RemoveServantByType(eType);
	bool bSuccess = m_pServantMgr->AddServant(eType, pServant->GetEntityID(), pServant);
	if (!bSuccess)
		return;

	GetCallbackHandler()->CastToDirectorHandler()->ChangeServant(eType, GetEntityID(), uServantID, true);

}

void CCharacterDirector::RemoveServant(ENpcType eType, uint32 uServantID)
{
	if (!m_pServantMgr)
		return;
	CCharacterFollower* pServant = GetServantByType(eType);
	m_pServantMgr->RemoveServant(eType,uServantID,pServant);

	GetCallbackHandler()->CastToDirectorHandler()->ChangeServant(eType, GetEntityID(), 0, false);
	
	if (pServant)
		pServant->SetMaster(0);
}

void CCharacterDirector::RemoveServantByType( ENpcType eType )
{
	if (!m_pServantMgr)
		return;
	uint32 uServantID = m_pServantMgr->GetServantIDByType(eType);
	while (uServantID != 0)
	{
		m_pServantMgr->RemoveServant(eType,uServantID,NULL);
		uServantID = m_pServantMgr->GetServantIDByType(eType);
	}
}

uint32 CCharacterDirector::GetFirstServantClient()
{
	if (m_pServantMgr)
	{
		return m_pServantMgr->GetFirstServantID();
	}
	return 0;
}

CCharacterFollower* CCharacterDirector::GetServantByType( ENpcType eType ) const
{
	if (!m_pServantMgr)
	{
		return NULL;
	}
	uint32 pServantID = m_pServantMgr->GetServantIDByType(eType);
	return 	GetCharacterByID(pServantID);
}

void CCharacterDirector::AddFXCache(const TCHAR* szSkillName)
{
	GetFighter()->CastToFighterDirector()->AddFXCache(szSkillName);
}

void CCharacterDirector::RemoveFXCache(const TCHAR* szSkillName)
{
	GetFighter()->CastToFighterDirector()->RemoveFXCache(szSkillName);
}

void CCharacterDirector::ClearAllFXCache()
{
	CEffectLoader::Inst()->Clear();
}


uint32 CMoveAccelerator::ms_uCount = 196/33;

CMoveAccelerator::CMoveAccelerator(CCharacterDirector* pCharacter)
: m_uCount(CMoveAccelerator::ms_uCount)
, m_pCharacter(pCharacter)
{
	Ast(m_uCount > 1); // 不大于1就没什么意义了
	////先把初始速度设为当前速度的一半
	//float fCurSpeed = m_pCharacter->GetSpeed();
	//float fRealSpeed = fCurSpeed / 2;
	//ESetSpeedResult eRet = class_cast<CCoreObjectDirector*>(m_pCharacter->GetCoreObj())->SetSpeed(fRealSpeed);

	//AccelerateSpeed();	
}

CMoveAccelerator::~CMoveAccelerator()
{
	CAppClient::Inst()->UnRegisterTick(this);
	//cout << "结束加速CMoveAccelerator" << endl;
}

bool CMoveAccelerator::AccelerateSpeed()
{
	Ast(m_uCount > 0);
	const float fMaxSpeed = m_pCharacter->GetMaxSpeed();

	const uint32 uBase=0;
	const float fRatio = float(ms_uCount-m_uCount+1+uBase) / (ms_uCount+uBase);
	const float fRealSpeed = min(fMaxSpeed, fMaxSpeed * fRatio);
	
	//cout << "速度改为 " << fRealSpeed << endl;
	ESetSpeedResult eRet = m_pCharacter->SetCoreObjSpeed(fRealSpeed);
	m_pCharacter->SetCurSpeed(fRealSpeed);
	//if(eRet == eSSR_NotAllowed)
	//{
	//	CGac2GasCPP_ChangeMoveSpeed cmd;
	//	cmd.fSpeed = fRealSpeed;
	//	CConnClient::Inst()->SendCmd(&cmd);
	//}

	--m_uCount;
	if(m_uCount == 0 || fRealSpeed == fMaxSpeed)
	{
		m_pCharacter->m_pMoveAccelerator = NULL;
		delete this;
		return false;
	}
	return true;
}

void CMoveAccelerator::OnTick()
{
	AccelerateSpeed();
}

