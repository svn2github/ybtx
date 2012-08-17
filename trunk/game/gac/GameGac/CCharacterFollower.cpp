#include "stdafx.h"
#include "CCharacterFollower.h"
#include "CCoreObjectFollower.h"
#include "CCharacterVariantHandler.h"
#include "TPropertyHolder.inl"
#include "ICharacterFollowerCallbackHandler.h"
#include "CEntityClient.inl"
#include "CoreCommon.h"
#include "CEntityClientManager.h"
#include "CFacingMgr.h"
#include "ICharacterDirectorHandler.h"
#include "ICharacterFollowerHandler.h"
#include "IActorFollowerHandler.h"
#include "IActorDirectorHandler.h"
#include "CCharacterDirector.h"
#include "CCharacterFollower.inl"
#include "ErrLogHelper.h"
#include "TCharacterAdapterClient.h"
#include "CFighterFollower.h"
#include "IFighterDirectorHandler.h"
#include "TCharacterAdapterClient.inl"
#include "TEntityAdapterClient.inl"
#include "CRenderObject.h"
#include "CBloodRenderDialogPool.h"
#include "CAppClient.h"
#include "CRelationMgrClient.h"
#include "CSyncVariantClientHolder.h"
#include "CFighterVariantCallback.h"
#include "CSyncVariantClient.h"
#include "CNPCGazedAtMgr.h"
#include "CRenderLinkMgr.h"
#include "CCharAniController.h"
#include "CDirectionMgr.h"
#include "CDisplayHurt.h"
#include "CRenderSystemClient.h"
#include "IRenderObjClientHandler.h"
#include "CActorCfgClient.h"
#include "CBloodTypeCfg.h"
#include "CSoundPlayer.h"

CCharacterFollower::CCharacterFollower()
:m_bIsBattleHorse(false)
,m_uMasterID(0)
,m_strRealName("")
,m_strHorseName("")
,m_bActiveNpc(false)
,m_bRemovedImmediately(false)
,m_bCanBeTakeOver(false)
,m_pIKCtrl(NULL)
,m_eNpcType(ENpcType_None)
,m_eNpcAttackType(eATT_None)
,m_bDirLockedTarget(false)
,m_bShowInClient(true)
,m_HeadBloodRendlerDialog(NULL)
,m_bAgileValueBeCare(false)
,m_fMoveSpeed(0)
,m_pChangeNpcDirTick(NULL)
,m_pNpcAlertTick(NULL)
,m_bIsShadow(false)
,m_pDisplayHurtTick(NULL)
,m_bBloodRenderIsShow(false)
,m_bIsBoss(false)
,m_bDestroying(false)
{
}

CCharacterFollower::CCharacterFollower(CCoreObjectFollower* pCoreObj, uint32 uID, bool bIsBattleHorse)
:m_bIsBattleHorse(bIsBattleHorse)
,m_uMasterID(0)
,m_strRealName("")
,m_strHorseName("")
,m_NpcAIType(ENpcAIType_None)
,m_bActiveNpc(false)
,m_bRemovedImmediately(false)
,m_bCanBeTakeOver(false)
,m_pIKCtrl(NULL)
,m_eNpcType(ENpcType_None)
,m_eNpcAttackType(eATT_None)
,m_bDirLockedTarget(false)
,m_bShowInClient(true)
,m_HeadBloodRendlerDialog(NULL)
,m_bAgileValueBeCare(false)
,m_pChangeNpcDirTick(NULL)
,m_pNpcAlertTick(NULL)
,m_bIsShadow(false)
,m_pDisplayHurtTick(NULL)
,m_bBloodRenderIsShow(false)
,m_bIsBoss(false)
,m_bDestroying(false)
{
	Init<CCharacterFollowerTraits>(pCoreObj, uID);
	Ver(const_cast<CEntityClientManager*>(CEntityClientManager::GetInst())->AddGameEntity(this, GetEntityID()));
}

CCharacterFollower::~CCharacterFollower()
{
	SafeDelete(m_pDisplayHurtTick);
	SafeDelete(m_HeadBloodRendlerDialog);
	SetCallbackHandler(NULL);
	GetActor()->ClearVariantCallbackFunc();
	for (uint32 i=0; i<(uint32)eCTI_End; i++)
	{
		SafeDelete(m_vecSyncVariantHandler[i]);
	}
	for (uint32 i=0; i<(uint32)eCTI_End; i++)
	{
		SafeDelete(m_vecViewVariantHandler[i]);
	}
	m_vecSyncVariantHandler.clear();
	m_vecViewVariantHandler.clear();
	if (m_pChangeNpcDirTick)
	{
		CAppClient::Inst()->UnRegisterTick(m_pChangeNpcDirTick);
		SafeDelete(m_pChangeNpcDirTick);
	}
	SafeDelete(m_pFacingMgr);
	SafeDelete(m_pDirectionMgr);
	SafeDelete(m_pIKCtrl);
	SafeDelete(m_pRenderLinkMgr);
	const_cast<CEntityClientManager*>(CEntityClientManager::GetInst())->RemoveGameEntity(this);
}

void CCharacterFollower::SetCallbackHandler(ICharacterFollowerCallbackHandler *pCallbackHandler )
{
	AdvDelRef(m_pCallbackHandler);
	m_pCallbackHandler = pCallbackHandler;
	AdvAddRef(m_pCallbackHandler);
}

CActorFollower* CCharacterFollower::GetActor()const 
{
	return class_cast<CActorFollower*>(CEntityClient::GetActor()); 
}

void CCharacterFollower::RegSyncVarAddedCallback(const TCHAR* szName, CVariantCallback* pCallback, ECodeTableIndex eIndex)
{
	m_vecSyncVariantHandler[eIndex]->AddVariantAddedCallback(szName, pCallback);
}

void CCharacterFollower::RegSyncVarChangedCallback(const TCHAR* szName, CVariantCallback* pCallback, ECodeTableIndex eIndex)
{
	m_vecSyncVariantHandler[eIndex]->AddVariantChangedCallback(szName, pCallback);
}

void CCharacterFollower::RegSyncVarDeletedCallback(const TCHAR* szName, CVariantCallback* pCallback, ECodeTableIndex eIndex)
{
	m_vecSyncVariantHandler[eIndex]->AddVariantDeletedCallback(szName, pCallback);
}

void CCharacterFollower::RegSyncTableClearCallback(const TCHAR* szName, CVariantCallback* pCallback, ECodeTableIndex eIndex)
{
	m_vecSyncVariantHandler[eIndex]->AddTableClearCallback(szName, pCallback);
}

void CCharacterFollower::RegViewVarAddedCallback(const TCHAR* szName, CVariantCallback* pCallback, ECodeTableIndex eIndex)
{
	m_vecViewVariantHandler[uint32(eIndex)]->AddVariantAddedCallback(szName, pCallback);
}

void CCharacterFollower::RegViewVarChangedCallback(const TCHAR* szName, CVariantCallback* pCallback, ECodeTableIndex eIndex)
{
	m_vecViewVariantHandler[uint32(eIndex)]->AddVariantChangedCallback(szName, pCallback);
}

void CCharacterFollower::RegViewVarDeletedCallback(const TCHAR* szName, CVariantCallback* pCallback, ECodeTableIndex eIndex)
{
	m_vecViewVariantHandler[uint32(eIndex)]->AddVariantDeletedCallback(szName, pCallback);
}

void CCharacterFollower::RegViewTableClearCallback(const TCHAR* szName, CVariantCallback* pCallback, ECodeTableIndex eIndex)
{
	m_vecViewVariantHandler[uint32(eIndex)]->AddTableClearCallback(szName, pCallback);
}

void CCharacterFollower::UnRegSyncVarAddedCallback(const TCHAR* szName, CVariantCallback* pCallback, ECodeTableIndex eIndex)
{
	m_vecSyncVariantHandler[eIndex]->RemoveVariantAddedCallback(szName, pCallback);
}

void CCharacterFollower::UnRegSyncVarChangedCallback(const TCHAR* szName, CVariantCallback* pCallback, ECodeTableIndex eIndex)
{
	m_vecSyncVariantHandler[eIndex]->RemoveVariantChangedCallback(szName, pCallback);
}

void CCharacterFollower::UnRegSyncVarDeletedCallback(const TCHAR* szName, CVariantCallback* pCallback, ECodeTableIndex eIndex)
{
	m_vecSyncVariantHandler[eIndex]->RemoveVariantDeletedCallback(szName, pCallback);
}

void CCharacterFollower::UnRegSyncTableClearCallback(const TCHAR* szName, CVariantCallback* pCallback, ECodeTableIndex eIndex)
{
	m_vecSyncVariantHandler[eIndex]->RemoveTableClearCallback(szName, pCallback);
}

void CCharacterFollower::UnRegViewVarAddedCallback(const TCHAR* szName, CVariantCallback* pCallback, ECodeTableIndex eIndex)
{
	m_vecViewVariantHandler[(uint32)eIndex]->RemoveVariantAddedCallback(szName, pCallback);
}

void CCharacterFollower::UnRegViewVarChangedCallback(const TCHAR* szName, CVariantCallback* pCallback, ECodeTableIndex eIndex)
{
	m_vecViewVariantHandler[(uint32)eIndex]->RemoveVariantChangedCallback(szName, pCallback);
}

void CCharacterFollower::UnRegViewVarDeletedCallback(const TCHAR* szName, CVariantCallback* pCallback, ECodeTableIndex eIndex)
{
	m_vecViewVariantHandler[(uint32)eIndex]->RemoveVariantDeletedCallback(szName, pCallback);
}

void CCharacterFollower::UnRegViewTableClearCallback(const TCHAR* szName, CVariantCallback* pCallback, ECodeTableIndex eIndex)
{
	m_vecViewVariantHandler[(uint32)eIndex]->RemoveTableClearCallback(szName, pCallback);
}

void CCharacterFollower::OnDestroy()
{	
	if (m_bDestroying)
		return;
	
	m_bDestroying = true;
	GetFighter()->ClearAllState();

	GetFighter()->Notify(eCE_Offline);

	if (GetCallbackHandler())
		GetCallbackHandler()->OnDestroy(this);
	
	if (m_pNpcAlertTick)
	{
		CAppClient::Inst()->UnRegisterTick(m_pNpcAlertTick);
		SafeDelete(m_pNpcAlertTick);
	}
	delete GetFighter();
	delete this;
}

void CCharacterFollower::OnRenderObjDestroyed()
{
	m_pRenderLinkMgr->OnRenderObjDestroyed();
}

CCharacterFollower* CCharacterFollower::GetCharacter(const CCoreObjectFollower* pCoreObj)
{
	if(pCoreObj == NULL) return NULL;
	ICoreObjectFollowerHandler* pHandler = pCoreObj->GetHandler() ? pCoreObj->GetHandler() : NULL;
	if ( pHandler == NULL )
		return NULL;

	int32 eType = (int32)(pHandler->GetTag());
	if(eType == eCOHT_Character)
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
	return NULL;
}

CCharacterFollower* CCharacterFollower::GetCharacterByID(uint32 uEntityID)
{
	return	CEntityClientManager::GetInst()->GetCharacter(uEntityID);
}

bool CCharacterFollower::CppGetCtrlState(EFighterCtrlState eState)const
{
	return GetFighter()->GetCtrlState(eState);
}

EClass CCharacterFollower::CppGetClass()const
{
	return GetFighter()->CppGetClass();
}

void CCharacterFollower::CppSetClass(EClass eClass)
{
	GetFighter()->CppSetClass(eClass);
}

uint32 CCharacterFollower::CppGetLevel()const
{
	return GetFighter()->CppGetLevel();
}

void CCharacterFollower::CppSetLevel(uint32 uLevel)
{
	GetFighter()->CppSetLevel(limit2(uLevel,uint32(1),uint32(eMaxLevel)));
}

ECamp CCharacterFollower::CppGetBirthCamp()const
{
	return GetFighter()->CppGetBirthCamp();
}

void CCharacterFollower::CppSetBirthCamp(ECamp eBirthCamp)
{
	GetFighter()->CppSetBirthCamp(eBirthCamp);
}

ECamp CCharacterFollower::CppGetCamp()const
{
	return GetFighter()->CppGetCamp();
}

void CCharacterFollower::CppSetCamp(ECamp eCamp)
{
	GetFighter()->CppSetCamp(eCamp);
}

bool CCharacterFollower::CppGetPKState() const
{
	return GetFighter()->CppGetPKState();
}

void CCharacterFollower::CppSetPKState(bool bPKState)
{
	GetFighter()->CppSetPKState(bPKState);
}

//EPKState CCharacterFollower::CppGetPKState() const
//{
//	return GetFighter()->CppGetPKState();
//}
//
//void CCharacterFollower::CppSetPKState(EPKState ePKState)
//{
//	GetFighter()->CppSetPKState(ePKState);
//}
//
//void CCharacterFollower::CppSetZoneType(EZoneType eZoneType)
//{
//	GetFighter()->CppSetZoneType(eZoneType);
//}

float CCharacterFollower::CppGetPropertyValueByName(const TCHAR* szName)
{
	return GetFighter()->CppGetPropertyValue(szName);
}

uint32 CCharacterFollower::CppGetTalentPoint(const TCHAR* szName)
{
	return GetFighter()->GetTalentPoint(szName);
}

bool CCharacterFollower::CppIsAlive()const
{
	return GetFighter()->CppIsAlive();
}

void CCharacterFollower::DisplayHurt(int iValue,EAgileType eAgileType,EHurtResult eHurtResult,bool bOverTime, bool IsSkill, const CFPos& Pos)
{
	if (!m_pDisplayHurtTick)
	{
		m_pDisplayHurtTick = new CDisplayHurt(this);
	}
	m_pDisplayHurtTick->DisplayHurt(iValue,eAgileType,eHurtResult,bOverTime,IsSkill,Pos);
}

void CCharacterFollower::ClearDisplayHurt()
{
	m_pDisplayHurtTick=NULL;
}
void CCharacterFollower::DoNowAni()
{
	GetActor()->DoNowAni();
}

void CCharacterFollower::SetWeaponVisible(bool bIsMainHand, bool bVisible)
{
	GetRenderLinkMgr()->SetWeaponVisible(bIsMainHand, bVisible);
}

void CCharacterFollower::SetWeaponPlayType(EWeaponPlayType eWeaponPlayType)
{
	GetRenderLinkMgr()->SetWeaponPlayType(eWeaponPlayType);
}

uint32 CCharacterFollower::GetWeaponPlayType()
{
	return GetRenderLinkMgr()->GetWeaponPlayType();
}
uint32 CCharacterFollower::GetOffWeaponPlayType()
{
	return GetRenderLinkMgr()->GetOffWeaponPlayType();
}

void CCharacterFollower::SetOffWeaponPlayType(EWeaponPlayType eWeaponPlayType)
{
	GetRenderLinkMgr()->SetOffWeaponPlayType(eWeaponPlayType);
}

EMoveState CCharacterFollower::GetMoveState()
{
	return GetActor()->GetMoveState();
}

EActionState CCharacterFollower::GetActionState()
{
	return GetActor()->GetActionState();
}

void CCharacterFollower::DoMoveState(EMoveState eMoveState, bool bForce)
{
	if (GetActor()->DoMoveState(eMoveState, bForce))
		GetActor()->DoMoveAnimation();		
}

void CCharacterFollower::DoActionState(EActionState eActionState)
{
	uint8 uDir = GetActor()->GetActionDir();
	DoActionState(eActionState, uDir);
}

void CCharacterFollower::DoActionState(EActionState eActionState, uint8 udir)
{
	GetActor()->DoActionState(eActionState, udir);
}

uint8 CCharacterFollower::GetMoveDir()const
{
	return GetActor()->GetMoveDir();
}

void CCharacterFollower::SetMoveDir(uint8 Dir)
{
	GetActor()->SetMoveDir(Dir);
}

void CCharacterFollower::OnEnteredSyncAoi()
{
	CFighterFollower* pFighterFollower = GetFighter();
	const CSyncVariantClientHolder* pSyncVarHolder = GetSyncVariantHolder(eCTI_Agile);
	const CSyncVariantClientHolder* pViewVarHolder = GetViewVariantHolder();
	const CSyncVariantClientHolder* pAgileViewVarHolder = GetViewVariantHolder(eCTI_Agile);
	Ast (pSyncVarHolder);
	Ast (pViewVarHolder);
	uint32 uDirectorID = CEntityClientManager::GetInst()->GetDirectorID();
	CCharacterFollower* pCharacter = CCharacterFollower::GetCharacterByID(uDirectorID);
	if (pCharacter)
	{
		ECharacterType eType = (ECharacterType) pViewVarHolder->GetNumber<uint8>("Attribute/Type");
		if (eType == eCT_Npc)
		{
			CCharacterDirector* pDirector = pCharacter->CastToCharacterDirector();
			uint32 uMasterID = pAgileViewVarHolder->GetNumber<uint32>("Master/MasterID");
			//cout << "CCharacterFollower::OnEnteredSyncAoi()召唤兽" << GetEntityID() << "的MasterID = " <<
			//	uMasterID << ", DirectorID = " << uDirectorID << "\n";
			if (uMasterID == uDirectorID)
				pDirector->AddServant(GetNpcType(), GetEntityID());				
			else
				SetMaster(uMasterID);
		}
	}
	InitIsShadowVariant();	

	if (CheckSyncDataReceived())
	{
		if (GetCallbackHandler()) 
			GetCallbackHandler()->OnLeaveSyncAoi(this);
	}

	if (GetCallbackHandler()) 
		GetCallbackHandler()->OnIntoSyncAoi(this);

	if(!pFighterFollower->m_pSetStateVariantCallback)
	{
		pFighterFollower->m_pSetStateVariantCallback = new CFighterSetStateVariantCallback(pFighterFollower);
		RegSyncVarChangedCallback("zzzzzStateIsSync", pFighterFollower->m_pSetStateVariantCallback, eCTI_Agile);
		const CSyncVariantClient* pStateVariant = pSyncVarHolder->GetRootVariant()->GetMember("State");
		if(pStateVariant)
			pSyncVarHolder->EnumVariant("State", &CFighterFollower::OnSetStateForInit);
	}
	SetSyncDataReceived(true);

	pSyncVarHolder->EnumVariant("CastingProcess", &CFighterFollower::OnSetStateForInit);

	// 面部朝向
	uint32 uTargetID = pSyncVarHolder->GetNumber<uint32>("Attribute1/TargetID");
	if (uTargetID != 0)
	{
		CCharacterFollower* pCharDst = CEntityClientManager::GetInst()->GetCharacter(uTargetID);
		if (!pCharDst)
			GetFacingMgr()->SetDstEntitiyID(uTargetID);
		GetFacingMgr()->CreateFacingPair(this, pCharDst);
	}
	CCharacterDirector* pDirector = pCharacter->CastToCharacterDirector();
	if (pDirector && GetAITypeID() == ENpcAIType_Task)
		pDirector->GetNPCGazedAtMgr()->AddSyncNPC(this);

	//初始化动作
	EActionState eActionState = (EActionState)pSyncVarHolder->GetNumber<uint8>("Actor/ActionState");
	if (IsMoving())
	{
		DoMoveState(eMS_Move);
	}
	else
	{
		switch (eActionState)
		{
		case eAS_Die:
			GetActor()->DoActionState(eAS_Dead);
			break;
		case eAS_HoldStill:
			GetActor()->DoActionState(eAS_HoldStill);
			break;
		case eAS_ResumeAni:
			DoMoveState(eMS_Stop);
			break;
		default:
			{
				CActorCfgClient* pActorAniCfg = CActorCfgClient::GetActorCfgByActionState(eActionState);
				if (pActorAniCfg->GetActorPlayMode() != eAPM_UpBodyOnce && eActionState != eAS_Strike)
				{
					GetActor()->DoActionState(eActionState);
				}
				else
				{
					DoMoveState(eMS_Stop);
				}
			}
			break;
		}
	}

	if (GetNpcType() != ENpcType_None)
	{
		ENpcAIType eNpcAIType = (ENpcAIType)pViewVarHolder->GetNumber<uint8>("Attribute/NpcAIType");
		if (eNpcAIType == ENpcAIType_Building || eNpcAIType == ENpcAIType_TowerAttack)
		{
			uint8 uActionDir = (uint8)pSyncVarHolder->GetNumber<uint8>("Actor/ActionDir");
			DoActionDir(uActionDir, true);
		}
	}

	//初始化警戒
	if (GetNpcType() != ENpcType_None)
		pSyncVarHolder->EnumVariant("Alert", &CFighterFollower::OnSetStateForInit);
}

void CCharacterFollower::OnLeftedSyncAoi()
{
	if (CastToCharacterDirector())
		return;
	CFighterFollower* pFighterFollower = GetFighter();
	if(pFighterFollower && pFighterFollower->m_pSetStateVariantCallback)
	{
		UnRegSyncVarChangedCallback("zzzzzStateIsSync", pFighterFollower->m_pSetStateVariantCallback, eCTI_Agile);
		pFighterFollower->m_pSetStateVariantCallback = NULL;
	}
	GetFacingMgr()->RemoveFacingPair();
	uint32 uDirectorID = CEntityClientManager::GetInst()->GetDirectorID();
	CCharacterDirector* pDirector = CCharacterFollower::GetCharacterByID(uDirectorID)->CastToCharacterDirector();
	if (pDirector)
	{
		if (GetAITypeID() == ENpcAIType_Task)
		{
			pDirector->GetNPCGazedAtMgr()->DelSyncNPC(this);
		}
	}

	GetFighter()->ClearAllState();
	if (GetCallbackHandler()) 
		GetCallbackHandler()->OnLeaveSyncAoi(this);
	
	SetSyncDataReceived(false);
	SetNpcAlertState(0);
}

void CCharacterFollower::OnRefreshRenderObjPos(const CFPos& PixelPos)
{
	if (NeedUpdateRenderObjPos())
	{
		m_pRenderLinkMgr->UpdatePos(PixelPos);
	}
}

void CCharacterFollower::OnPosChanged()
{
}

void CCharacterFollower::OnMoveBegan()
{
	GetFighter()->Notify(eCE_MoveBegan);
	DoMoveState(eMS_Move);
}

void CCharacterFollower::OnMoveEnded(EMoveEndedType eMEType)
{
	if (eMEType==eMET_Reached)
	{
		GetFighter()->Notify(eCE_MoveSuccess);
	}
	else if (eMEType==eMET_Stopped)
	{
		GetFighter()->Notify(eCE_MoveStop);
	}
	GetFighter()->Notify(eCE_MoveEnd);
	if (GetActionState() != eAS_MoveEnd)
		DoMoveState(eMS_Stop);

	string strCueNameTail = "";
	if (GetFighter()->GetCtrlState(eFCS_InNormalHorse) || GetFighter()->GetCtrlState(eFCS_InBattleHorse))
		strCueNameTail = "4";
	CSoundPlayer::PlayFootstep(this, strCueNameTail);
}

void CCharacterFollower::OnMovePathChanged()
{	
	GetFighter()->Notify(eCE_MovePathChange);
	DoMoveState(eMS_Move);
}

void CCharacterFollower::OnWayPointChanged()
{	
	DoMoveState(eMS_Move);
}

void CCharacterFollower::OnBeSelected()
{
	if(CanBeSelected())CClientRenderScene::GetInst()->SetSelectedObj(GetRenderObject());
}

void CCharacterFollower::OnDead()
{
	GetFighter()->CppSetAlive(false);
	//DoMoveState(eMS_LockStop);

	GetFacingMgr()->RemoveFacingPair();
	if (GetCallbackHandler())
		GetCallbackHandler()->OnDead(GetEntityID());
}

void CCharacterFollower::OnReborn()
{
	GetFighter()->CppSetAlive(true);
	GetActor()->ChangeMoveState(eMS_Stop, true);
	GetActor()->SetCurOnceActionLevel(eAPP_NULL);
	GetActor()->DoActionState(eAS_Idle_BackWpn, GetActor()->GetActionDir());
	Ast(GetRenderObject());
	GetRenderObject()->SetTerrainInfluence(false);
	if (GetCallbackHandler())
		GetCallbackHandler()->OnReborn(GetEntityID());
}

void CCharacterFollower::SetIsPlayer(bool bIsPlayer)
{
	m_bIsPlayer = bIsPlayer;
}

bool CCharacterFollower::GetIsPlayer()
{
	return m_bIsPlayer;
}

void CCharacterFollower::SetIsBoss(bool bIsBoss)
{
	m_bIsBoss = bIsBoss;
}

bool CCharacterFollower::GetIsBoss()
{
	return m_bIsBoss;
}

void CCharacterFollower::SetAttackFxFile(const TCHAR* strAttackFxFile)
{
	GetActor()->SetAttackFxFile(strAttackFxFile);
}

void CCharacterFollower::SetActionFxFile(const TCHAR* strActionFxFile)
{
	GetActor()->SetActionFxFile(strActionFxFile);
}

void CCharacterFollower::SetSkillFxFile(const TCHAR* strSkillFxFile)
{
	GetActor()->SetSkillFxFile(strSkillFxFile);
}

void CCharacterFollower::SetSpliceName(const TCHAR* strSpliceName)
{
	GetActor()->SetSpliceName(strSpliceName);
}

CRenderObject* CCharacterFollower::GetHorseRenderObj()const
{
	return GetRenderLinkMgr()->GetHorseRenderObj();
}

CRenderObject* CCharacterFollower::GetMainHandRenderObj()const
{
	return GetRenderLinkMgr()->GetMainHandRenderObj();
}

CRenderObject* CCharacterFollower::GetOffHandRenderObj()const
{
	return GetRenderLinkMgr()->GetOffHandRenderObj();
}

CRenderObject* CCharacterFollower::GetAdditionRenderObj()const
{
	return GetRenderLinkMgr()->GetAdditionRenderObj();
}

void CCharacterFollower::SetWeaponInfo(const TCHAR* strMainHandLinkName, const TCHAR* strOffHandLinkName,const TCHAR* strMainHandLinkEffect,const TCHAR* strOffHandLinkEffect)
{
	GetRenderLinkMgr()->SetWeaponInfo(strMainHandLinkName,strOffHandLinkName,strMainHandLinkEffect,strOffHandLinkEffect);
}

void CCharacterFollower::SetHorseEffect(const TCHAR* strHorseEffect)
{
	GetRenderLinkMgr()->SetHorseEffect(strHorseEffect);
}

void CCharacterFollower::SetAdditionEffect(const TCHAR* strAdditionEffect)
{
	GetRenderLinkMgr()->SetAdditionEffect(strAdditionEffect);
}

CCharacterFollower* CCharacterFollower::GetMaster() const
{
	CCharacterFollower* pMaster = CCharacterFollower::GetCharacterByID(m_uMasterID);
	if (NULL==pMaster)
		return NULL;
	return pMaster;
}

// 战斗移动模式 true 倒退走 false 普通
EMoveMode CCharacterFollower::GetMoveMode(uint8 uWalkBackRadius)
{
	CFighterFollower* pTarget = GetFighter()->GetTarget();
	if(!GetIsPlayer() || !pTarget || GetFighter() == pTarget
		|| GetFighter()->GetCtrlState(eFCS_InNormalHorse) || GetFighter()->GetCtrlState(eFCS_OnMission))
		return eMM_Run;

	//if (GetActionState() == eAS_Cast)		// 忘了做什么的了，暂时注释
	//	return eMM_WalkBack;

	if (!GetFighter()->GetCtrlState(eFCS_InBattle) || GetFighter()->GetCtrlState(eFCS_ForbitTurnAround))
		return eMM_Run;

	CFPos posSelf	= GetPixelPos();
	CFPos posTarget	= GetFighter()->GetTarget()->GetEntity()->GetPixelPos();
	float fDistance = GetEntityDistInGrid(pTarget->GetEntity());

	if (fDistance > uWalkBackRadius)
		return eMM_Run;

	CDir dirTarget;
	CVector2f vecDir;
	vecDir.x = posTarget.x - posSelf.x;
	vecDir.y = posTarget.y - posSelf.y;
	dirTarget.Set(vecDir);
	
	uint8 uSelfMoveDir = GetActor()->GetMoveDir();
	uint8 uSelfActionDir = GetActor()->GetActionDir();

	if ( abs(uSelfMoveDir - dirTarget.uDir) > 64 && abs(uSelfMoveDir - dirTarget.uDir) < 192)
	{
		if ( abs(uSelfActionDir - dirTarget.uDir) < 64 || abs(uSelfActionDir - dirTarget.uDir) > 192)
		{
			uint8 uActionDir = uSelfMoveDir + 128;
			GetActor()->SetActionDir(uActionDir);
			return eMM_WalkBack;
		}
	}

	return eMM_Run;
}

void CCharacterFollower::ResumeMovingActionDir(uint8 uWalkBackRadius)
{
	CFighterFollower* pTarget = GetFighter()->GetTarget();
	if(!GetIsPlayer() || !pTarget || GetFighter() == pTarget || GetFighter()->GetCtrlState(eFCS_InNormalHorse) 
		|| GetFighter()->GetCtrlState(eFCS_OnMission))
	{
		DoActionDir(GetMoveDir());
		return;
	}

	if (!GetFighter()->GetCtrlState(eFCS_InBattle) || GetFighter()->GetCtrlState(eFCS_ForbitTurnAround))
	{
		DoActionDir(GetMoveDir());
		return;
	}

	CFPos posSelf	= GetPixelPos();
	CFPos posTarget	= GetFighter()->GetTarget()->GetEntity()->GetPixelPos();
	float fDistance = GetEntityDistInGrid(pTarget->GetEntity());

	if (fDistance > uWalkBackRadius)
	{
		DoActionDir(GetMoveDir());
		return;
	}

	CDir dirTarget;
	CVector2f vecDir;
	vecDir.x = posTarget.x - posSelf.x;
	vecDir.y = posTarget.y - posSelf.y;
	dirTarget.Set(vecDir);

	uint8 uSelfMoveDir = GetActor()->GetMoveDir();
	uint8 uSelfActionDir = GetActor()->GetActionDir();

	if ( abs(uSelfMoveDir - dirTarget.uDir) > 64 && abs(uSelfMoveDir - dirTarget.uDir) < 192)
	{
		if ( abs(uSelfActionDir - dirTarget.uDir) < 64 || abs(uSelfActionDir - dirTarget.uDir) > 192)
		{
			uint8 uActionDir = uSelfMoveDir + 128;
			DoActionDir(uActionDir);
			return;
		}
	}

	DoActionDir(GetMoveDir());
}

bool CCharacterFollower::IsBattleHorse()const
{
	//if(m_pMaster != NULL && GetCoreObj() == m_pMaster->GetCoreObj())
	//{
	//	return true;
	//}
	//return false;
	return m_bIsBattleHorse;
	//return m_eNpcType == ENpcType_BattleHorse;
}

void CCharacterFollower::SetTargetBuff()
{
	GetFighter()->SetTargetBuff();
}

float CCharacterFollower::GetRemainTime(const TCHAR* sName, uint32 uId)
{
	return GetFighter()->GetRemainTime(sName, uId);
}

void CCharacterFollower::SetAITypeID(ENpcAIType eNpcAITypeID)
{
	m_NpcAIType = eNpcAITypeID;
}

ENpcAIType CCharacterFollower::GetAITypeID()
{
	return m_NpcAIType;
}

void CCharacterFollower::OnSpeedChanged()
{
	GetActor()->DoNowAni(true);
}

void CCharacterFollower::SetActiveNpc(bool bActive)
{
	m_bActiveNpc = bActive;	
}

bool CCharacterFollower::BeActiveNpc()
{
	return m_bActiveNpc;
}

void CCharacterFollower::SetRemovedImmediately(bool bImme)
{
	m_bRemovedImmediately = bImme;
}

bool CCharacterFollower::GetRemovedImmediately()
{
	return m_bRemovedImmediately;
}

void CCharacterFollower::SetCanBeTakeOver(bool bTakeOver)
{
	m_bCanBeTakeOver = bTakeOver;
}

bool CCharacterFollower::CanBeTakeOver()
{
	return m_bCanBeTakeOver;
}

void CCharacterFollower::SetNpcType(ENpcType eType)
{
	m_eNpcType = eType;
}

ENpcType CCharacterFollower::GetNpcType()
{
	return m_eNpcType;
}

void  CCharacterFollower::SetNpcAttackType(EAttackType eNpcAttackType)
{
	m_eNpcAttackType = eNpcAttackType;
}

EAttackType CCharacterFollower::GetNpcAttackType()
{
	return m_eNpcAttackType;
}

void CCharacterFollower::InitRealName()
{
	const TCHAR* szRealName = (const TCHAR*)GetViewVariant()->GetMember("Attribute")->GetMember("RealName")->GetString();
	SetRealName(szRealName);
}

void CCharacterFollower::SetRealName(const string& strRealName)
{
	m_strRealName = strRealName;

	GetCallbackHandler()->OnRealNameChange(GetEntityID(),GetNpcType(),strRealName.c_str());
}


void CCharacterFollower::SetHeadBloodTex(CCharacterFollower* pCharacterDirector)
{
	if (!m_HeadBloodRendlerDialog)
		return;
	EBloodDialogType eBDType = CBloodTypeCfg::GetBloodType(CppGetClass());
	m_HeadBloodRendlerDialog->SetHeadBloodTex(eBDType,CRelationMgrClient::IsEnemy(pCharacterDirector->GetFighter(),GetFighter()));
}

void CCharacterFollower::SetHeadBloodTransparent(float fTrans)
{
	if (!m_HeadBloodRendlerDialog)
		return;
	m_HeadBloodRendlerDialog->SetTransparent(fTrans);
}

void CCharacterFollower::AddHeadBloodRendler(CCharacterFollower* pCharacterDirector, CBloodRenderDialogPool* pDialogPool, SQRWnd* pParentWnd, bool bShow)
{
	if (!pCharacterDirector || CRelationMgrClient::IsNeutral(GetFighter(), pCharacterDirector->GetFighter())
		|| GetNpcType() == ENpcType_LittlePet)
		return;	
	DelHeadBloodRendler(pDialogPool);
	m_HeadBloodRendlerDialog = pDialogPool->GetHeadBloodDialog(pParentWnd, CppGetClass());
	GetRenderObject()->AddChild(m_HeadBloodRendlerDialog, LT_SKELETAL, "Bip01 Head");
	SetHeadBloodTex( pCharacterDirector);

	if (bShow)
		ShowBloodRendler(true);
	m_bBloodRenderIsShow = bShow;
}

void CCharacterFollower::DelHeadBloodRendler(CBloodRenderDialogPool* pDialogPool)
{
	if(m_HeadBloodRendlerDialog != NULL)
	{
		GetRenderObject()->DelChild(m_HeadBloodRendlerDialog);
		ShowBloodRendler(false);
		pDialogPool->ReleaseHeadBloodDialog(m_HeadBloodRendlerDialog, CppGetClass());
		m_HeadBloodRendlerDialog = NULL;
	}
}

void CCharacterFollower::UpdateHeadBloodRendler()
{
	if (m_HeadBloodRendlerDialog && m_bBloodRenderIsShow)
	{
		ShowBloodRendler(true);
	}
}

void CCharacterFollower::ShowBloodRendler(bool bShow)
{
	if (!m_HeadBloodRendlerDialog)
		return;
	if (bShow)
	{
		m_HeadBloodRendlerDialog->UpdateInfo((uint32)(CppGetPropertyValueByName("HealthPoint")),
			(uint32)(CppGetPropertyValueByName("HealthPointAgile")),CppGetCtrlState(eFCS_FeignDeath));
	}
	else
	{
		m_HeadBloodRendlerDialog->ShowWnd(false);
	}
	SetBloodRenderIsShow(bShow);
}

void CCharacterFollower::CppSetGameCamp(int32 iGameCamp)
{
	Ast (GetFighter());
	GetFighter()->CppSetGameCamp(iGameCamp);
}

int32 CCharacterFollower::CppGetGameCamp()
{
	Ast(GetFighter());
	return GetFighter()->CppGetGameCamp();
}

void CCharacterFollower::CreateNpcDirTick()
{
	if (m_pChangeNpcDirTick)
	{
		CAppClient::Inst()->UnRegisterTick(m_pChangeNpcDirTick);
		SafeDelete(m_pChangeNpcDirTick);
	}
	m_pChangeNpcDirTick = new NpcFaceEnemyTick(this);
	CAppClient::Inst()->RegisterTick(m_pChangeNpcDirTick, 33);
}

void CCharacterFollower::DestroyNpcDirTick()
{
	if (m_pChangeNpcDirTick)
	{
		CAppClient::Inst()->UnRegisterTick(m_pChangeNpcDirTick);
		SafeDelete(m_pChangeNpcDirTick);
	}
}

void CCharacterFollower::SetNpcAlertState(uint32 uAlertTargetID)
{
	if (uAlertTargetID != 0)
	{
		CCharacterFollower* pTarget = CCharacterFollower::GetCharacterByID(uAlertTargetID);
		if (!pTarget)
			return;
		
		GetFighter()->SetTarget(pTarget->GetFighter());
		CFPos ownPos = GetPixelPos();
		CFPos targetPos = pTarget->GetPixelPos();
		CDir dirTarget;
		CVector2f vecDir;
		vecDir.x = targetPos.x - ownPos.x;
		vecDir.y = targetPos.y - ownPos.y;
		dirTarget.Set(vecDir);

		uint8 uCurDir = GetActor()->GetActionDir();
		if (uint8(dirTarget.uDir - uCurDir) >= 86 && uint8(uCurDir - dirTarget.uDir) >= 86)
		{
			SetFirstDexterity(2.0f);
			DoActionDir(dirTarget.uDir);
			SetFirstDexterity(1.0f);
		}
		else if (uint8(dirTarget.uDir - uCurDir) >= 42 && uint8(uCurDir - dirTarget.uDir) >= 42)
		{
			DoActionDir(dirTarget.uDir);
		}

		SetInterestedObj(pTarget);

		if (m_pNpcAlertTick)
		{
			CAppClient::Inst()->UnRegisterTick(m_pNpcAlertTick);
			SafeDelete(m_pNpcAlertTick);
		}
		m_pNpcAlertTick = new NpcAlertTick(this, uAlertTargetID);
		CAppClient::Inst()->RegisterTick(m_pNpcAlertTick, 33);
	}
	else
	{
		GetFighter()->SetTarget(NULL);
		if (m_pNpcAlertTick)
		{
			CAppClient::Inst()->UnRegisterTick(m_pNpcAlertTick);
			SafeDelete(m_pNpcAlertTick);
		}
		ClearInterested();
	}
}

void CCharacterFollower::NpcAlertTick::OnTick()
{
	Ast (m_pOwner);
	CCharacterFollower* pTarget = CCharacterFollower::GetCharacterByID(m_uTarget);
	if (!pTarget || !m_pSyncRootVariant)
		return;
	
	CFPos ownPos = m_pOwner->GetPixelPos();
	CFPos targetPos = pTarget->GetPixelPos();
	CDir dirTarget;
	CVector2f vecDir;
	vecDir.x = targetPos.x - ownPos.x;
	vecDir.y = targetPos.y - ownPos.y;
	dirTarget.Set(vecDir);

	if (!m_bLockedTarget)
	{
		uint8 uCurDir = m_pOwner->GetActor()->GetActionDir();
		if (uint8(dirTarget.uDir - uCurDir) >= 42 && uint8(uCurDir - dirTarget.uDir) >= 42)
		{
			m_bLockedTarget = true;
			m_pOwner->ClearInterested();
		}
		else
		{
			m_pOwner->SetInterestedObj(pTarget);
		}
	}
	else
	{
		m_pOwner->DoActionDir(dirTarget.uDir);
	}

	bool bIdleBattle = (bool)m_pSyncRootVariant->GetNumber<bool>("Alert/AlertAction");
	if (bIdleBattle)
		m_pOwner->GetActor()->DoActionState(eAS_Idle_Battle);
}

void CCharacterFollower::NpcFaceEnemyTick::OnTick()
{
	Ast (m_pOwner);
	EMoveState eState = m_pOwner->GetMoveState();
	if (m_pOwner->IsMoving())
		return;
	const CSyncVariantClient* pSyncVariant = m_pOwner->GetSyncVariant(eCTI_Agile)->GetMember("Attribute1");
	if (!pSyncVariant)
		return;
	const CSyncVariantClient* pTargetIDVariant = pSyncVariant->GetMember("TargetID");
	if(!pTargetIDVariant)
		return;
	uint32 uTargetID = pTargetIDVariant->GetNumber<uint32>();
	CCharacterFollower* pTarget = CCharacterFollower::GetCharacterByID(uTargetID);
	if (pTarget)
	{
		CFPos ownPos = m_pOwner->GetPixelPos();
		CFPos targetPos = pTarget->GetPixelPos();
		CDir dirTarget;
		CVector2f vecDir;
		vecDir.x = targetPos.x - ownPos.x;
		vecDir.y = targetPos.y - ownPos.y;
		dirTarget.Set(vecDir);
		m_pOwner->DoActionDir(dirTarget.uDir);
	}
}

void CCharacterFollower::SetShowInClient(bool bShow)
{
	m_bShowInClient = bShow;
}

bool CCharacterFollower::ShowInClient()
{
	return m_bShowInClient;
}

void CCharacterFollower::RegistNpcVariantCallback()
{
	RegViewVarChangedCallback("Level", new LevelChangeVariantCallback(this), eCTI_Static);
	RegViewVarChangedCallback("Class", new SetClassVariantCallback(this), eCTI_Static);
	RegViewVarChangedCallback("Camp", new SetCampVariantCallback(this), eCTI_Static);
	RegViewVarChangedCallback("GameCamp", new SetNpcGameCampVariantCallback(this), eCTI_Static);
	RegViewVarChangedCallback("NpcType", new SetNpcTypeVariantCallback(this), eCTI_Static);
	RegViewVarChangedCallback("NpcAIType", new SetNpcAITypeVariantCallback(this), eCTI_Static);
	RegViewVarChangedCallback("BeActiveNpc", new SetNpcActiveTypeVariantCallback(this), eCTI_Static);
	RegViewVarChangedCallback("AttackType", new SetAttackTypeVariantCallback(this), eCTI_Static);
	RegViewVarChangedCallback("RealName", new SetNpcRealNameVariantCallback(this), eCTI_Static);
	RegViewVarChangedCallback("ShowInClient", new SetBeShowInClientVariantCallback(this), eCTI_Static);
	RegViewVarChangedCallback("PKSwitchType", new SetPKSwitchVariantCallback(this), eCTI_Agile);
	RegViewVarChangedCallback("MasterID", new AddMasterVariantCallback(this), eCTI_Static);
	//RegViewVarChangedCallback("ZoneType", new SetZoneTypeVariantCallback(this));
	RegViewVarChangedCallback("FightDir", new SetFighteDirVariantCallback(this), eCTI_Static);
	RegSyncVarChangedCallback("Speed", new OnSpeedChangeVariantCallback(this), eCTI_Agile);
	RegSyncVarChangedCallback("AlertTargetID", new SetAlertTargetVariantCallback(this), eCTI_Agile);
	RegSyncVarChangedCallback("bIsShadow", new ShadowVariantVariantCallback(this), eCTI_Static);
	RegSyncVarChangedCallback("MaterialNum", new MaterialNumVariantCallback(this), eCTI_Agile);
	RegSyncVarChangedCallback("ExpOwnerID", new ExpOwnerIDVariantCallback(this), eCTI_Agile);
}

void CCharacterFollower::RegistCommonVarriantCallback()
{
	RegViewVarChangedCallback("Level", new LevelChangeVariantCallback(this), eCTI_Static);
	RegViewVarChangedCallback("Class", new SetClassVariantCallback(this), eCTI_Static);
	RegViewVarChangedCallback("Camp", new SetCampVariantCallback(this), eCTI_Static);
	RegViewVarChangedCallback("GameCamp", new SetNpcGameCampVariantCallback(this), eCTI_Static);
	RegViewVarChangedCallback("BirthCamp", new BirthCampChangeVariantCallback(this), eCTI_Static);
	RegViewVarChangedCallback("PKSwitchType", new SetPKSwitchVariantCallback(this), eCTI_Agile);
	//RegViewVarChangedCallback("ZoneType", new SetZoneTypeVariantCallback(this));
	RegSyncVarChangedCallback("Speed", new OnSpeedChangeVariantCallback(this), eCTI_Agile);
	RegSyncVarChangedCallback("Stealth", new StealthVariantCallback(this), eCTI_Agile);
}

void CCharacterFollower::InitIsShadowVariant()
{
	const CSyncVariantClient* pViewVariant = GetViewVariant();
	const CSyncVariantClient* pAttributeVariant = pViewVariant->GetMember("Attribute");
	uint32 pCharType = pAttributeVariant->GetMember("Type")->GetNumber<uint8>();
	if (pCharType == 2)
	{
		const CSyncVariantClientHolder* pSyncVarHolder = GetSyncVariantHolder(eCTI_Static);
		if (pSyncVarHolder)
		{
			m_bIsShadow = pSyncVarHolder->GetNumber<bool>("MasterRes/bIsShadow");
		}
	}
}

void CCharacterFollower::SetInterestedObj(CCharacterFollower* pTargetObj)
{
	
	if( NULL == m_pIKCtrl &&GetRenderObject() )
	{	
		CModel* pModel = GetRenderObject();
		if (pModel&&pModel->GetSketetalFrame() && GetRenderObject()->IsValid())
		{
			m_pIKCtrl = new CCharacterController(this);
		}
	}
	if( NULL != m_pIKCtrl && pTargetObj&&pTargetObj->GetRenderObject())
		m_pIKCtrl->SetInterestedObj(pTargetObj);
}

bool CCharacterFollower::ClearInterested(bool bImme)
{
	if( NULL!=m_pIKCtrl )
	{
		m_pIKCtrl->ClearInterested( bImme );
		return true;
	}
	else
		return false;
}

bool CCharacterFollower::GetInterestedPos( CVector3f& outVec )
{
	if( NULL!=m_pIKCtrl )
		return m_pIKCtrl->GetInterestedPos(outVec);
	return false;
}

void CCharacterFollower::SetIsCtrlAll(bool SetCtrlAll) 
{ 
	if(m_pIKCtrl)
		m_pIKCtrl->SetCtrlAll(SetCtrlAll); 
}


void CCharacterFollower::SetFirstDexterity(float fValue)
{
	m_pDirectionMgr->SetFirstDexterity(fValue);
}

bool CCharacterFollower::IsStopBeforeTurn(uint8 uDesDirection)
{
	return m_pDirectionMgr->IsStopBeforeTurn(uDesDirection);
}

bool CCharacterFollower::IsTurnOver()
{
	return m_pDirectionMgr->IsTurnOver();
}


void CCharacterFollower::DoActionDir(uint8 uActionDir, bool bForce, bool bImmed)
{
	if (GetActor()->GetActionDir() == uActionDir)
		return;

	if (!CppGetCtrlState(eFCS_ForbitTurnAround) || bForce)
	{
		GetActor()->SetActionDir(uActionDir);
		CDir dir;
		dir.uDir = GetActor()->GetActionDir();
		m_pDirectionMgr->SetDirection(uActionDir, bImmed);
		//SetDirection(dir, bImmed, uTurnTimes);
	}
}

void CCharacterFollower::GetDirection(CDir& dir)const
{
	GetHorseRenderObj()->GetDirection(dir);
}

void CCharacterFollower::SetStealthState(bool bSteatlth)
{
	if (bSteatlth)
		GetHorseRenderObj()->SetAlphaValue(128);
	else
		GetHorseRenderObj()->SetAlphaValue(255);
}

CCharacterFollower* CCharacterFollower::GetBindObjParent() const
{
	CCoreObjectFollower* pCoreObjParent = m_pCoreObj->GetBindObjParent();
	if(!pCoreObjParent)
		return NULL;

	return GetCharacter(pCoreObjParent);
}

CCharacterFollower* CCharacterFollower::MoveFirstBindObjChild()
{
	CCoreObjectFollower* pCoreObjCurChild = m_pCoreObj->MoveFirstBindObjChild();
	if(!pCoreObjCurChild)
		return NULL;		//EOS
	CCharacterFollower* pCharCurChild = GetCharacter(pCoreObjCurChild);
	while(!pCharCurChild)
	{
		pCoreObjCurChild = m_pCoreObj->MoveNextBindObjChild();
		if(!pCoreObjCurChild)
			return NULL;		//EOS
		pCharCurChild = GetCharacter(pCoreObjCurChild);
	}
	return pCharCurChild;
}

CCharacterFollower* CCharacterFollower::MoveNextBindObjChild()
{
	CCoreObjectFollower* pCoreObjCurChild = m_pCoreObj->MoveNextBindObjChild();
	if(!pCoreObjCurChild)
		return NULL;		//EOS
	CCharacterFollower* pCharCurChild = GetCharacter(pCoreObjCurChild);
	while(!pCharCurChild)
	{
		pCoreObjCurChild = m_pCoreObj->MoveNextBindObjChild();
		if(!pCoreObjCurChild)
			return NULL;		//EOS
		pCharCurChild = GetCharacter(pCoreObjCurChild);
	}
	return pCharCurChild;
}

bool CCharacterFollower::EndOfBindObjChild() const
{
	return m_pCoreObj->EndOfBindObjChild();
}

CCharacterFollower* CCharacterFollower::GetCurBindObjChild() const
{
	return GetCharacter(m_pCoreObj->GetCurBindObjChild());
}

bool CCharacterFollower::BeDangerForDirector()
{
	if (CastToCharacterDirector())
		return false;
	uint32 uDirector = CEntityClientManager::GetInst()->GetDirectorID();
	CCharacterFollower* pDirector = CCharacterFollower::GetCharacterByID(uDirector);
	Ast (pDirector);
	CFighterFollower* pDirectorFighter = pDirector->GetFighter();
	Ast (pDirectorFighter);
	bool bEnemy = CRelationMgrClient::Inst().IsEnemy(GetFighter(), pDirectorFighter);
	const CSyncVariantClient* pViewVariant = GetViewVariant();
	ECharacterType eType = (ECharacterType)pViewVariant->GetMember("Attribute")->GetMember("Type")->GetNumber<uint8>();
	if (eType == eCT_Player)
	{
		return bEnemy;
	}
	else
	{
		if (bEnemy && (BeActiveNpc() || GetFighter()->GetTarget() == pDirectorFighter))
			return true;
		return false;
	}
}

void CCharacterFollower::OnMaterialNumChange(uint32 uMaterialNum)
{
	if (GetNpcType() != ENpcType_Truck)
		return;

	uint32 uDirector = CEntityClientManager::GetInst()->GetDirectorID();
	CCharacterFollower* pDirector = CCharacterFollower::GetCharacterByID(uDirector);
	Ast (pDirector);
	CFighterFollower* pDirectorFighter = pDirector->GetFighter();
	Ast (pDirectorFighter);
	
	//当此运输车是主角的运输车或者是主角的当前目标的话触发回调
	if (GetMaster() && GetMaster()->GetEntityID() == uDirector)
		GetCallbackHandler()->OnMaterialNumChange(this, uMaterialNum, true);
	else if (pDirectorFighter->GetTarget() == GetFighter())
		GetCallbackHandler()->OnMaterialNumChange(this, uMaterialNum, false);
}

uint32 CCharacterFollower::GetCurMaterialNum()
{
	if (GetNpcType() == ENpcType_Truck)
	{
		return GetSyncVariantHolder(eCTI_Agile)->GetNumber<uint32>("Attribute1/MaterialNum");
	}
	return 0;
}

void CCharacterFollower::OnExpOwnerChange()
{
	if (GetCallbackHandler())
		GetCallbackHandler()->OnExpOwnerChange(this);
}

bool CCharacterFollower::BeExpOwnerByDirector()
{
	uint32 uExpOwnerID = GetSyncVariantHolder(eCTI_Agile)->GetNumber<uint32>("Attribute1/ExpOwnerID");
	uint32 uDirector = CEntityClientManager::GetInst()->GetDirectorID();
	if (uDirector == uExpOwnerID)
		return true;
	else 
		return false;
}

uint32 CCharacterFollower::GetExpOwnerID()
{
	return (uint32)GetSyncVariantHolder(eCTI_Agile)->GetNumber<uint32>("Attribute1/ExpOwnerID");
}

void CCharacterFollower::SetCurOnceActionLevel(uint32 uActionLevel)
{
	GetActor()->SetCurOnceActionLevel(EActionPlayPriority(uActionLevel));
};
