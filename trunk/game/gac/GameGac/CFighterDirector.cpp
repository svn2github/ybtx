#include "stdafx.h"
#include "CFighterDirector.h"
#include "TPropertyHolder.inl"
#include "IFighterDirectorHandler.h"
#include "CCoolDownMgrClient.h"
#include "CNormalAttackClient.h"
#include "PtrlGac2GasCPPDef.h"
#include "CConnClient.h"
#include "CSkillClient.h"
#include "CCharacterDirector.h"
#include "ICharacterDirectorCallbackHandler.h"
#include "PtrlGas2GacCPPDef.h"
#include "CFighterViewInfo.h"
#include "CSkillCondMgrClient.h"
#include "CRelationMgrClient.h"
#include "CSyncVariantClient.h"
#include "CFightCallbackData.h"
#include "CSkillMgrClient.h"
#include "CTalent.h"
#include "CBaseState.h"
#include "CEffectLoader.h"
#include "CNormalAttackCfgClient.h"

CFighterDirector::CFighterDirector()
: m_bNoSingTimeAll(false)
, m_pCurrentSkillAttackType(eATT_None)
, m_uBurstSoulTimes(0)
{
	m_pNormalAttack = new CNormalAttackClient(this);
	m_pSkillCondMgrClient = new CSkillCondMgrClient(this);
	CCoolDownMgrClient::Init(this);
	m_pCoolDownMgr = &CCoolDownMgrClient::Inst();
	m_pFightInfoData = new CFightInfoData();
	m_pSkillMgrClient = new CSkillMgrClient();
}

CFighterDirector::~CFighterDirector()
{
	m_setNoSingTimeSkill.clear();
	m_mapCFighterViewInfoById.clear();
	m_mapLearnSkill.clear();
	CCoolDownMgrClient::Unit();
	m_pCoolDownMgr=NULL;
	delete m_pNormalAttack;m_pNormalAttack=NULL;
	delete m_pSkillCondMgrClient;m_pSkillCondMgrClient=NULL;
	delete m_pFightInfoData;m_pFightInfoData=NULL;
	delete m_pSkillMgrClient;m_pSkillMgrClient=NULL;
}

IFighterDirectorHandler* CFighterDirector::GetHandler()const
{
	IFighterFollowerHandler* pHandler=CFighterFollower::GetHandler();
	return pHandler?pHandler->CastToFighterDirectorHandler():NULL;
}

ICharacterDirectorCallbackHandler* CFighterDirector::GetCallBackHandler()const	
{
	CCharacterFollower* pCharFol = GetCharacter();
	ICharacterFollowerCallbackHandler* pCallbackHandler = pCharFol->GetCallbackHandler();
	return pCallbackHandler->CastToDirectorHandler();
}

void CFighterDirector::SetMHNACfg(const TCHAR* szName,float fMainHandWeaponInterval,EWeaponType MHWeaponType,EAttackType eAttackType)
{
	m_pNormalAttack->SetMHNACfg(szName, fMainHandWeaponInterval, MHWeaponType, eAttackType);
}

void CFighterDirector::SetAHNACfg(const TCHAR* szName,float fAssistantWeaponInterval, EWeaponType AHWeaponType)
{
	//cout << " equip " << AHWeaponType << endl;
	m_pNormalAttack->SetAHNACfg(szName, fAssistantWeaponInterval, AHWeaponType);
}

CFighterFollower* CFighterDirector::GetNATarget()const
{
	return m_pNormalAttack->GetTarget();
}

void CFighterDirector::CancelNormalAttack()
{
	m_pNormalAttack->CancelNormalAttack();
}

CCharacterFollower*	CFighterDirector::GetNormalAttackTarget()
{
	if(m_pNormalAttack->GetTarget())
		return m_pNormalAttack->GetTarget()->GetCharacter();
	return NULL;
}

bool CFighterDirector::IsNormalAttacking()
{
	return m_pNormalAttack->IsAttacking();
}

//bool CFighterDirector::IsAutoNormalAttack()
//{
//	return m_pNormalAttack->IsAutoNormalAttack();
//}

uint32 CFighterDirector::CheckNACondAndSetNATarget(CFighterFollower* pTarget)
{
	return m_pNormalAttack->CheckNACondAndSetNATarget(pTarget);
}

CNormalAttackClient* CFighterDirector::GetNormalAttack()const
{
	return m_pNormalAttack;
}

float CFighterDirector::NewCoolDownTime(const TCHAR* SkillName, uint32 SkillLevel, uint32 uLeftTime, bool bIsSwitchEquipSkill)
{
	const CSkillClientSharedPtr& pSkill =CSkillClient::GetSkillByName(SkillName);
	float fCoolDownTime = float(pSkill->GetCoolDownTime()->GetDblValue(this,SkillLevel-1));
	ESkillCoolDownType eCoolDownType = pSkill->GetCoolDownType();
	if (eSCDT_FightSkill == eCoolDownType)
	{
		m_pCoolDownMgr->IntNewCoolDown(SkillName, eCoolDownType, max(uLeftTime, CCoolDownMgrClient::ms_uFightSkillCommonCDTime), bIsSwitchEquipSkill);
	}
	else
	{
		m_pCoolDownMgr->IntNewCoolDown(SkillName, eCoolDownType, uLeftTime, bIsSwitchEquipSkill);
	}
	return fCoolDownTime;
}

uint32 CFighterDirector::ResetSingleCoolDownTime(const TCHAR* SkillName)
{
	const CSkillClientSharedPtr& pSkill = CSkillClient::GetSkillByName(SkillName);
	m_pCoolDownMgr->IntNewCoolDown(SkillName, pSkill->GetCoolDownType(), 0);
	return eDSR_Success;
}

EConsumeType CFighterDirector::GetConsumeType(const TCHAR* SkillName)
{
	const CSkillClientSharedPtr& pSkill = CSkillClient::GetSkillByName(SkillName);
	return pSkill->GetConsumeType();
}

uint32 CFighterDirector::GetConsumeValue(const TCHAR* SkillName, uint32 SkillLevel)
{
	const CSkillClientSharedPtr& pSkill = CSkillClient::GetSkillByName(SkillName);
	CCfgCalc* pCfgCalc = pSkill->GetConsumeValue();
	if (pCfgCalc)
	{
		uint32 uConsumeValue = pCfgCalc->GetIntValue(this, SkillLevel - 1 );
		
		EConsumeType eConsumeType = pSkill->GetConsumeType();
		EAttackType eAttackType = pSkill->GetAttackType();
		if (eCT_MP == eConsumeType)
		{
			uConsumeValue = (int32)(uConsumeValue * this->Get(&CFighterSyncToDirectorCalInfo::m_MPConsumeRate));
			if (eAttackType == eATT_Nature)
			{
				uConsumeValue = (int32)(uConsumeValue * this->Get(&CFighterSyncToDirectorCalInfo::m_NatureMPConsumeRate));
			}
			else if (eAttackType == eATT_Destroy)
			{
				uConsumeValue = (int32)(uConsumeValue * this->Get(&CFighterSyncToDirectorCalInfo::m_DestructionMPConsumeRate));
			}
			else if (eAttackType == eATT_Evil)
			{
				uConsumeValue = (int32)(uConsumeValue * this->Get(&CFighterSyncToDirectorCalInfo::m_EvilMPConsumeRate));
			}
		}
		else if (eCT_RG == eConsumeType)
		{	
			uConsumeValue = (int32)(uConsumeValue * this->Get(&CFighterSyncToDirectorCalInfo::m_RPConsumeRate));
		}
		else if (eCT_EG == eConsumeType)
		{
			uConsumeValue = (int32)(uConsumeValue * this->Get(&CFighterSyncToDirectorCalInfo::m_EPConsumeRate));
		}
		return uConsumeValue;
	}
	return 0;	
}

uint32 CFighterDirector::GetSkillDistance(const TCHAR* SkillName, uint32 SkillLevel)
{
	const CSkillClientSharedPtr& pSkill = CSkillClient::GetSkillByName(SkillName);
	return pSkill->GetMaxCastSkillDistance()->GetIntValue(this,SkillLevel-1);
}

ECastingProcessType CFighterDirector::GetCastingProcessType(const TCHAR* SkillName)
{
	const CSkillClientSharedPtr& pSkill = CSkillClient::GetSkillByName(SkillName);
	return pSkill->GetCastingProcessType();
}

float CFighterDirector::GetCastingProcessTime(const TCHAR* SkillName, uint32 SkillLevel)
{
	const CSkillClientSharedPtr& pSkill = CSkillClient::GetSkillByName(SkillName);
	float fProcessTime = 0.0f;
	if (GetCastingProcessType(SkillName) != eCPT_Wink)
	{
		fProcessTime = float(pSkill->GetCastingProcessTime()->GetDblValue(this, SkillLevel-1));
	}
	return fProcessTime;	
}

uint32 CFighterDirector::GetSkillCoolDownType(const TCHAR* SkillName)
{
	const CSkillClientSharedPtr& pSkill = CSkillClient::GetSkillByName(SkillName);
	ESkillCoolDownType eCoolDownType = pSkill->GetCoolDownType();
	return eCoolDownType;
}

float CFighterDirector::GetCoolDownTime(const TCHAR* SkillName, uint32 SkillLevel)
{
	const CSkillClientSharedPtr& pSkill = CSkillClient::GetSkillByName(SkillName);
	float fCoolDownTime = float(pSkill->GetCoolDownTime()->GetDblValue(this,SkillLevel-1));
	return fCoolDownTime;
}

uint32 CFighterDirector::GetSkillLeftCoolDownTime(const TCHAR* SkillName)
{
	const CSkillClientSharedPtr& pSkill = CSkillClient::GetSkillByName(SkillName);
	ESkillCoolDownType eCoolDownType = pSkill->GetCoolDownType();
	uint32 time = m_pCoolDownMgr->GetCoolDownTime(SkillName, eCoolDownType);
	return time;
}

bool CFighterDirector::IsInCommonCD(const TCHAR* SkillName)
{
	const CSkillClientSharedPtr& pSkill = CSkillClient::GetSkillByName(SkillName);
	ESkillCoolDownType eCoolDownType = pSkill->GetCoolDownType();
	return m_pCoolDownMgr->IsInCommonCD(SkillName, eCoolDownType);
}

bool CFighterDirector::IsSwitchEquipSkill(const TCHAR* SkillName)
{
	return m_pCoolDownMgr->IsSwitchEquipSkill(SkillName);
}

uint32 CFighterDirector::GetFireCondition(const TCHAR* SkillName)
{
	const CSkillClientSharedPtr& pSkill = CSkillClient::GetSkillByName(SkillName);

	if (pSkill->GetHorseLimit())
		return 1;
	else if (pSkill->GetIsStanceSkill())
		return 2;
	else if (pSkill->GetTargetIsCorpse())
		return 3;
	
	return 0;
}

float CFighterDirector::GetNormalAttackDistance() const
{
	CNormalAttackClient* pNAClient = GetNormalAttack();
	if (!pNAClient)
		return 0.0f;

	return pNAClient->GetNormalAttackDistance();
}

void CFighterDirector::OnCtrlVariantChanged(uint32 uValue, uint32 uValueTypeID, bool bSet, bool bIsAlive)
{
	CFighterFollower::OnCtrlVariantChanged(uValue, uValueTypeID, bSet, bIsAlive);

	if(uValueTypeID == eFCS_ForbitMove)
	{
		if(bSet == true)
		{
			CCharacterFollower* pChar = GetCharacter();
			if(pChar)
			{
				CCharacterDirector* pCharDir = pChar->CastToCharacterDirector();
				if(pCharDir)
					pCharDir->StopMoving();
			}
		}
	}
}

template<typename CmdType>
inline bool CFighterDirector::SendToGas(const CmdType* pCmd)const
{
	if (CConnClient::Inst())
	{
		CConnClient::Inst()->SendCmd(pCmd);
		return true;
	}
	return false;
}

void  CFighterDirector::StopNormalAttack()
{
	CGac2GasCPP_StopNormalAttack cmd;
	SendToGas(&cmd);
}

void CFighterDirector::SetDoSkillCtrlState(EDoSkillCtrlState eSkillCtrlState, bool bSet)
{
	m_pSkillMgrClient->SetDoSkillCtrlState(eSkillCtrlState,bSet);
}


bool CFighterDirector::GetDoSkillCtrlState(EDoSkillCtrlState eSkillCtrlState)
{
	return m_pSkillMgrClient->GetDoSkillCtrlState(eSkillCtrlState);
}

void CFighterDirector::RevertSkillCtrlState()
{
	m_pSkillMgrClient->RevertSkillCtrlState();
}

bool CFighterDirector::CheckDoNormalAttack(uint32 uTargetID)
{
	CFighterFollower* pTarget = CCharacterDirector::GetCharacterByID(uTargetID)->GetFighter();
	CCharacterDirector* pCharAttacker = class_cast<CCharacterDirector*>(this->GetCharacter());
	if(pTarget->CppIsAlive() && CRelationMgrClient::IsEnemy(pTarget, this))
	{
		if(this->CheckNACondAndSetNATarget(pTarget) == eDSR_Success)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

bool CFighterDirector::DoNormalAttack(uint32 uTargetID,bool bIsNeedAutoTracker)
{
	CCharacterFollower* pCharTarget = CCharacterFollower::GetCharacterByID(uTargetID);
	if(!pCharTarget)
		return false;

	CFighterFollower* pTarget = pCharTarget->GetFighter();
	bool bIsAutoNormalAttack = true;//IsAutoNormalAttack();
	CCharacterDirector* pCharAttacker = class_cast<CCharacterDirector*>(this->GetCharacter());
	
	if(pTarget && pTarget->CppIsAlive() && !CRelationMgrClient::IsFriend(this, pTarget) && !CRelationMgrClient::IsNeutral(this, pTarget))
	{
		uint32 uRet = CheckNACondAndSetNATarget(pTarget);
		if(uRet == eDSR_Success)
		{
			//if(!bIsAutoNormalAttack && pCharAttacker->IsMoving() && !this->IsShortDistNA())
			//{
			//	pCharAttacker->StopMoving();
			//}
			pCharAttacker->ChangeDrawWeaponMode(true);
			pCharAttacker->ChangeNormalAttacking(true);
			m_pNormalAttack->InitNormalAttack(pTarget, pCharAttacker->GetSpeed(), bIsNeedAutoTracker);
			this->TellMedDoNormalAttack(uTargetID, bIsAutoNormalAttack, true);
			//cout << pCharAttacker->GetEntityID() << "\t开始对\t" << uTargetID << "\t进行普攻" << endl;
			return true;
		}
		else
		{
			this->TellMedDoNormalAttack(uTargetID, bIsAutoNormalAttack, false);
			//cout << pCharAttacker->GetEntityID() << "\t对\t" << uTargetID << "\t普攻失败" << endl;
			GetCallBackHandler()->OnMagicCondFail(uRet,"");
			return false;
		}
	}
	return false;
}

void CFighterDirector::TellMedDoNormalAttack(uint32 uTargetID, bool bIsAutoNormalAttack, bool bToBeStarted)
{	
	CGac2GasCPP_DoNormalAttack cmd;
	cmd.uTargetID = uTargetID;
	//cmd.bIsAuto = bIsAutoNormalAttack;
	cmd.bToBeStarted = bToBeStarted;
	SendToGas(&cmd);
}

void CFighterDirector::TellMedStartOrRestartNADueToSkillFailure(uint32 uSkillID)const
{
	CGac2GasCPP_StartOrRestartNADueToSkillFailure cmd;
	cmd.uqbSkillID = uSkillID;
	SendToGas(&cmd);
}

void CFighterDirector::CancelState(uint32 uID)
{
	CGac2GasCPP_CancelState cmd;
	cmd.uID = uID;

	SendToGas(&cmd);
}

void CFighterDirector::DestroyServant()
{
	CGac2GasCPP_DestroyServant cmd;
	cmd.uObjID = GetEntityID();

	SendToGas(&cmd);
}

void CFighterDirector::CancelCoerceMove(uint32 uID)
{
	CGac2GasCPP_CancelCoerceMove cmd;
	cmd.uID = uID;

	SendToGas(&cmd);
}

void CFighterDirector::CancelCastingProcess()
{
	CGac2GasCPP_CancelCastingProcess cmd;
	SendToGas(&cmd);
}

void CFighterDirector::GetTargetCastingProcess(uint32 uTargetID)
{
	CGac2GasCPP_GetTargetCastingProcess cmd;
	cmd.uTargetID = uTargetID;
	SendToGas(&cmd);
}

void CFighterDirector::GetTargetOfTarget(uint32 uTargetID)
{
	CGac2GasCPP_GetTargetOfTarget cmd;
	cmd.uTargetID = uTargetID;
	SendToGas(&cmd);
}

void CFighterDirector::NotifyBurstSoulArrived(EBurstSoulType eBurstSoulType)
{
	CGac2GasCPP_NotifyBurstSoulArrived cmd;
	cmd.uBurstSoulType = eBurstSoulType;
	SendToGas(&cmd);
}

//void CFighterDirector::SetAutoNormalAttack(bool IsAutoNormalAttack)
//{
//	m_pNormalAttack->SetAutoNormalAttack(IsAutoNormalAttack);
//
//	CGac2GasCPP_SetAutoNormalAttack cmd;
//	cmd.uObjID = GetEntityID();
//	cmd.bIsAuto = IsAutoNormalAttack;
//	SendToGas(&cmd);
//}

float	CFighterDirector::GetMainHandWeaponInterval()const
{
	return m_pNormalAttack->GetMainHandWeaponInterval();
}
float	CFighterDirector::GetAssistantWeaponInterval()const
{
	return m_pNormalAttack->GetAssistantWeaponInterval();
}

bool CFighterDirector::IsShortDistNA()const
{
	return m_pNormalAttack->IsShortDistanceNA();
}

void CFighterDirector::SetTargetPos(const CFPos& Pos)
{
	CGac2GasCPP_SetTargetPos cmd;
	cmd.Pos = Pos;
	SendToGas(&cmd);	
	CFighterFollower::SetTargetPos(Pos);
}

//-----------------------------------------------------------------------------
//	技能施放
//-----------------------------------------------------------------------------
// 条件检查
uint32 CFighterDirector::PreAttack( const TCHAR* SkillName, uint32 SkillLevel )
{
	const CSkillClientSharedPtr& pSkill = CSkillClient::GetSkillByName(SkillName);
	if(GetDoSkillCtrlState(eDSCS_InDoingSkill) && (!pSkill->HasCastingProcess() || pSkill->GetCastingProcessType()!=eCPT_GradeSing))
		return eDSR_DoingOtherSkill;

	return PreUseSkill(pSkill,SkillLevel);
}
uint32 CFighterDirector::PreUseSkill(const CSkillClientSharedPtr& pSkill, uint32 uSkillLevel)
{

	// 释放者检查
	if(!CppIsAlive())
		return eDSR_CharacterHasDead;

	// 技能类型检查
	if (pSkill->GetCoolDownType() == eSCDT_NonFightSkill && GetCharacter()->CppGetCtrlState(eFCS_InBattle))
		return eDSR_Fail;

	if( pSkill->GetCoolDownType() != eSCDT_UnrestrainedSkill && pSkill->GetCoolDownType() != eSCDT_UnrestrainedFightSkill
		&& (GetDoSkillCtrlState(eDSCS_ForbitUseSkill)
		|| (GetDoSkillCtrlState(eDSCS_ForbitNatureMagic) && pSkill->GetAttackType() == eATT_Nature)
		|| (GetDoSkillCtrlState(eDSCS_ForbitDestructionMagic) && pSkill->GetAttackType() == eATT_Destroy)
		|| (GetDoSkillCtrlState(eDSCS_ForbitEvilMagic) && pSkill->GetAttackType() == eATT_Evil)
		|| (GetCtrlState(eFCS_OnMission)) ))
	{
		return eDSR_ForbitUseSkill;
	}

	if (GetDoSkillCtrlState(eDSCS_ForbitUseClassSkill) && (pSkill->GetCoolDownType() == eSCDT_FightSkill
		||pSkill->GetCoolDownType() == eSCDT_NoComCDFightSkill
		||pSkill->GetCoolDownType() == eSCDT_UnrestrainedFightSkill))
	{
		return eDSR_ForbitUseClassSkill;
	}

	if (GetDoSkillCtrlState(eDSCS_ForbitUseNonFightSkill) && (pSkill->GetCoolDownType() == eSCDT_NonFightSkill))
	{
		return eDSR_ForbitUseNonFightSkill;
	}

	// 冷却时间检查
	m_pCoolDownMgr = &CCoolDownMgrClient::Inst();
	if (!(m_pCoolDownMgr->IsCoolDown(pSkill->GetName(), pSkill->GetCoolDownType())))
		return eDSR_InCDTime;

	// 目标检查
	CFighterFollower* pTarget = GetTarget();
	if( pSkill->GetSelectTargetType() == eFSTT_EnemyObject || pSkill->GetSelectTargetType() == eFSTT_FriendObject 
		|| pSkill->GetSelectTargetType() == eFSTT_FriendandSelf || pSkill->GetSelectTargetType() == eFSTT_AllObject 
		|| pSkill->GetSelectTargetType() == eFSTT_FriendAndEnemy || pSkill->GetSelectTargetType() == eFSTT_NotNeutral
		|| pSkill->GetSelectTargetType() == eFSTT_TeammatesOrEnemy)
	{
		if( pTarget == NULL)
		{
			if(pSkill->GetSelectTargetType() != eFSTT_FriendandSelf && pSkill->GetSelectTargetType() != eFSTT_AllObject
				&& pSkill->GetSelectTargetType() != eFSTT_NotNeutral && pSkill->GetSelectTargetType() != eFSTT_TeammatesOrEnemy)
				return eDSR_TargetNotExist;
		}
		else
		{
			if (pSkill->GetSelectTargetType() == eFSTT_EnemyObject || pSkill->GetSelectTargetType() == eFSTT_FriendAndEnemy)
			{
				if (this == pTarget)
					return eDSR_TargetIsSelf;

				// 这不用分那么细服务端来发的MESSAGE
				switch(CRelationMgrClient::GetRelationType(this, pTarget))
				{
				case eRT_Friend:
					if(pSkill->GetSelectTargetType() == eFSTT_EnemyObject)
						return eDSR_TargetIsFriend;
					break;
				case eRT_Neutral:
					return eDSR_TargetIsNeutral;
					break;
				default:
					break;
				}
			}
			
			if (pSkill->GetSelectTargetType() == eFSTT_FriendObject)
			{
				switch(CRelationMgrClient::GetRelationType(this, pTarget))
				{
				case eRT_Enemy:
					return eDSR_TargetIsEnemy;
					break;
				case eRT_Neutral:
					return eDSR_TargetIsNeutral;
					break;
				default:
					break;
				}
			}

			if(!pTarget->CppIsAlive() && !pSkill->GetTargetIsCorpse() && pSkill->GetSelectTargetType() != eFSTT_FriendandSelf 
				&& pSkill->GetSelectTargetType() != eFSTT_AllObject && pSkill->GetSelectTargetType() != eFSTT_NotNeutral)
				return eDSR_TargetIsDead;

			if (pTarget->CppIsAlive() && pSkill->GetTargetIsCorpse())
				return eDSR_TargetIsAlive;
		}

		if (pTarget && (pSkill->GetSelectTargetType() != eFSTT_FriendandSelf ||
			(pSkill->GetSelectTargetType() == eFSTT_FriendandSelf && CRelationMgrClient::IsFriend(pTarget, this))))
		{
			// 以目标为对象的施法距离检查
			//CFPos posFrom, posTo;
			//posFrom = GetEntity()->GetPixelPos();
			//posTo = pTarget->GetEntity()->GetPixelPos();

			//float x = posFrom.x - posTo.x;
			//float y = posFrom.y - posTo.y;
			//float fDistance = sqrt( x*x + y*y ) / eGridSpanForObj;
			float fDistance = GetEntity()->GetEntityDistInGrid(pTarget->GetEntity());
			if (fDistance > pSkill->GetMaxCastSkillDistance()->GetIntValue(this, uSkillLevel-1))
				return eDSR_OutOfDistance;

			if (fDistance < pSkill->GetMinCastSkillDistance()->GetIntValue(this, uSkillLevel-1))
				return eDSR_NotEnoughDistance;
		}
	}

	// 坐骑条件检查
	if(pSkill->GetHorseLimit() && !GetCtrlState(eFCS_InBattleHorse))
		return eDSR_IsNotExistHorse;
	// 魔法操作条件检查

	SetCurrentSkillAttackType(pSkill->GetAttackType());
	return pSkill->GetMagicEff() ? pSkill->GetMagicEff()->DoMagicCond(uSkillLevel, this) : eDSR_Success;
}

EDoSkillResult CFighterDirector::FightSkill(const TCHAR* szName, uint8 uSkillLevel,uint8 uPos)
{
	const CSkillClientSharedPtr& pSkillClient = CSkillClient::GetSkillByName(szName);
	EFSTargetType eFSTargetType = pSkillClient->GetSelectTargetType();

	if( GetHandler()->IsPassive() == false)
	{
		if(eFSTargetType == eFSTT_EnemyObject || eFSTargetType == eFSTT_FriendObject 
			|| eFSTargetType == eFSTT_NeutralObject || eFSTargetType == eFSTT_FriendandSelf 
			|| eFSTargetType == eFSTT_AllObject || eFSTargetType == eFSTT_FriendAndEnemy || eFSTargetType == eFSTT_NotNeutral
			|| eFSTargetType == eFSTT_PositionOrFriend || eFSTargetType == eFSTT_PositionOrEnemy 
			|| eFSTargetType == eFSTT_PositionOrLockedTarget || eFSTargetType == eFSTT_TeammatesOrEnemy)
		{
			if(GetCallBackHandler()->GroundSelector_IsActive())
			{
				GetCallBackHandler()->GroundSelector_CancelSelectGround();
			}
		}
	}	

	EDoSkillResult eRet = (EDoSkillResult)PreAttack(szName, uSkillLevel);

	if(eRet == eDSR_Success || eRet == eDSR_HealthPointIsLesser || eRet == eDSR_ManaPointIsLesser ||
		eRet == eDSR_EnergyPointIsLesser || eRet == eDSR_RagePointIsLesser || eRet == eDSR_ComboPointIsLesser)
		// 易变值不满足条件,客户端不拦截施放技能
	{
		// 以地点为目标，调选择地点
		CFighterFollower* pTarget = GetTarget();
		if(eFSTargetType == eFSTT_Position)
		{
			GetCallBackHandler()->BeginSelectGround(eSGT_GroundSelector, szName, uSkillLevel,uPos,pSkillClient->GetPositionArea(this,uSkillLevel), pSkillClient->GetMaxCastSkillDistance()->GetIntValue(this, uSkillLevel - 1));
			return eDSR_Fail;
		}
		else if ((eFSTargetType == eFSTT_PositionOrFriend && (pTarget==NULL || CRelationMgrClient::IsEnemy(pTarget, this)))
			|| (eFSTargetType == eFSTT_PositionOrEnemy && (pTarget==NULL || CRelationMgrClient::IsFriend(pTarget, this)))
			|| (eFSTargetType == eFSTT_PositionOrLockedTarget && pTarget==NULL))
		{
			GetCallBackHandler()->BeginSelectGround(eSGT_MouseSelector, szName, uSkillLevel,uPos,0, pSkillClient->GetMaxCastSkillDistance()->GetIntValue(this, uSkillLevel - 1));
			return eDSR_Fail;
		}
		else if (eFSTargetType == eFSTT_SelectFriendObject && ( (pSkillClient->GetTargetIsCorpse() && (!pTarget || pTarget->CppIsAlive()))
				|| (!pSkillClient->GetTargetIsCorpse() && (!pTarget || !pTarget->CppIsAlive())) ))
		{
			GetCallBackHandler()->BeginSelectGround(eSGT_TargetSelector, szName, uSkillLevel,uPos,0,pSkillClient->GetMaxCastSkillDistance()->GetIntValue(this, uSkillLevel - 1));
			return eDSR_Fail;
		}
		else
		{
			// 执行技能
			DoAttack(szName, uSkillLevel,uPos);
		}
		return eDSR_Success;
	}
	else
	{
		GetCallBackHandler()->OnMagicCondFail((uint32)eRet, szName);
		//
		switch(pSkillClient->GetStartNormalAttack())
		{
		case eANAT_AbsolutelyStartAndAutoTrack:
		case eANAT_AbsolutelyStart:
			TellMedStartOrRestartNADueToSkillFailure(pSkillClient->GetId());
			break;
		default:
			break;
		}

		if (eRet == eDSR_InCDTime)
			return eDSR_InCDTime;
		return eDSR_Fail;
	}
	return eDSR_Fail;

}

void CFighterDirector::DoAttack(const TCHAR* szSkillName, uint8 uSkillLevel, uint8 uPos, uint8 uDir, uint8 uMagicDir)
{
	// 自动普攻状态下，施放普攻
	if (m_pNormalAttack->GetNormalAttackCfg()->GetName() == szSkillName)
		return;

	CFPos posFrom, posTo, pos;
	posFrom = GetEntity()->GetPixelPos();
	CFighterFollower* pTarget = GetTarget();
	if (pTarget)
		posTo = pTarget->GetEntity()->GetPixelPos();

	const CSkillClientSharedPtr& pSkillClient = CSkillClient::GetSkillByName(szSkillName);
	bool bFightSkillMustIdle = pSkillClient->MustIdle() && !IsNoSingTime(pSkillClient->GetId()) && eCPT_ImmovableSing != pSkillClient->GetCastingProcessType();

	CCharacterDirector* pChar = GetCharacter()->CastToCharacterDirector();
	if(bFightSkillMustIdle && GetHandler()->IsMoving() && GetCastingProcessTime(szSkillName,uSkillLevel) != 0)
	{
		GetCharacter()->CastToCharacterDirector()->SetMovingByKeyboard(false);
		pChar->StopMoving();
		GetCharacter()->CastToCharacterDirector()->CancelAutoTracker(false);
	}

	//需要选择对象目标(敌人，友军)的技能
	TurnAround(szSkillName, posFrom, posTo, pTarget);
	//cout << pSkillClient->GetName() << " " << pSkillClient->GetId() << endl;
	switch (pSkillClient->GetCoolDownType())
	{
	case eSCDT_FightSkill:
	case eSCDT_NoComCDFightSkill:
	case eSCDT_NonFightSkill:
	case eSCDT_TempSkill:
	case eSCDT_EquipSkill:
	case eSCDT_UnrestrainedFightSkill:
		{
			DoNormalSkill( pSkillClient->GetId(), uSkillLevel, posFrom, uPos, uDir, uMagicDir);
		}
		break;
	default:
		{
			ostringstream strm;
			strm<< "技能ID " << pSkillClient->GetId() <<endl;
			LogErr("客户端出发的FightSkill不允许这个类型的技能", strm.str());
		}
	}
}

bool CFighterDirector::IsCastingTypeSkill(const TCHAR* szSkillName)
{
	const CSkillClientSharedPtr& pSkillClient = CSkillClient::GetSkillByName(szSkillName);
	bool bFightSkillMustIdle = pSkillClient->MustIdle() && !IsNoSingTime(pSkillClient->GetId());
	if(bFightSkillMustIdle)
	{
		return true;
	}
	return false;
}

void CFighterDirector::DoNormalSkill(uint32 uSkillID, uint8 uSkillLevel, const CFPos& fPosFrom, uint8 uPos, uint8 uDir, uint8 uMagicDir)
{
	//cout << "成功使用技能： " << szSkillName << "#end" << endl;

	CGac2GasCPP_DoSkill cmd;
	cmd.uqbSkillID = uSkillID;
	cmd.uSkillLevel = uSkillLevel;
	cmd.fPosFrom = fPosFrom;
	cmd.uPos = uPos;
	cmd.uDir = uDir;
	cmd.uMagicDir = uMagicDir;
	SendToGas(&cmd);
}

void CFighterDirector::TurnAround(const TCHAR* szSkillName, CFPos posFrom, CFPos posTo, CFighterFollower* pTarget)
{
	const CSkillClientSharedPtr& pSkillClient = CSkillClient::GetSkillByName(szSkillName);
	EFSTargetType eFSTargetType = pSkillClient->GetSelectTargetType();
	bool bIsUpdateDir = pSkillClient->GetIsUpdateDirection();
	CDir Dir;
	CVector2f vecDir;
	switch(eFSTargetType)
	{
	case eFSTT_EnemyObject:
	case eFSTT_FriendObject:
	case eFSTT_NeutralObject:
	case eFSTT_FriendandSelf:
	case eFSTT_AllObject:
	case eFSTT_FriendAndEnemy:
	case eFSTT_NotNeutral:
	case eFSTT_SelectFriendObject:
	case eFSTT_TeammatesOrEnemy:
		{
			if(GetCallBackHandler()->GroundSelector_IsActive())
				GetCallBackHandler()->GroundSelector_CancelSelectGround();

			if(posFrom != posTo && pTarget)
			{
				if (eFSTargetType == eFSTT_FriendandSelf && CRelationMgrClient::IsEnemy(pTarget, this))
				{
					Dir.uDir = GetCharacter()->GetActor()->GetActionDir();
				}
				else
				{
					vecDir.x = posTo.x - posFrom.x;
					vecDir.y = posTo.y - posFrom.y;
					Dir.Set(vecDir);
				}
			}
			else
				Dir.uDir = GetCharacter()->GetActor()->GetActionDir();
		}
		break;
	case eFSTT_PositionOrEnemy:
	case eFSTT_PositionOrFriend:
	case eFSTT_PositionOrLockedTarget:
		{
			if(pTarget && ((CRelationMgrClient::IsFriend(pTarget, this) && eFSTargetType == eFSTT_PositionOrFriend)
				|| (CRelationMgrClient::IsEnemy(pTarget, this) && eFSTargetType == eFSTT_PositionOrEnemy)
				|| eFSTargetType == eFSTT_PositionOrLockedTarget))
			{
				vecDir.x = posTo.x - posFrom.x;
				vecDir.y = posTo.y - posFrom.y;
				Dir.Set(vecDir);
				SetTargetPos(posTo);
			}
			else
			{	
				posFrom = GetEntity()->GetPixelPos();
				posTo.x = GetCallBackHandler()->GroundSelector_GetPosX();
				posTo.y = GetCallBackHandler()->GroundSelector_GetPosY();
				vecDir.x = posTo.x - posFrom.x;
				vecDir.y = posTo.y - posFrom.y;
				Dir.Set(vecDir);
				SetTargetPos(posTo);
				GetCharacter()->SetDirLockedTarget(false);
			}
		}
		break;
	case eFSTT_Position:
		{
			posTo.x = GetCallBackHandler()->GroundSelector_GetPosX();
			posTo.y = GetCallBackHandler()->GroundSelector_GetPosY();
			vecDir.x = (float)(posTo.x - posFrom.x);
			vecDir.y = (float)(posTo.y - posFrom.y);
			Dir.Set(vecDir);
			SetTargetPos(posTo);
			GetCharacter()->SetDirLockedTarget(false);
		}
		break;
	case eFSTT_SelfPosition:
		SetTargetPos(posFrom);
		break;
	case eFSTT_ObjectPosition:
		{
			vecDir.x = posTo.x - posFrom.x;
			vecDir.y = posTo.y - posFrom.y;
			Dir.Set(vecDir);
			SetTargetPos(posTo);
		}
		break;
	}
	if ( eFSTargetType != eFSTT_Self && eFSTargetType != eFSTT_SelfPosition && eFSTargetType != eFSTT_SelfDirection )
	{
		CCharacterDirector* pDirector = GetCharacter()->CastToCharacterDirector();
		if (bIsUpdateDir && pDirector)
			pDirector->TurnAndSyncDir(Dir.uDir);
	}
}

void CFighterDirector::TurnAroundByNonFightSkill(const TCHAR* szSkillName)
{
	CFPos posFrom, posTo, pos;
	posFrom = GetEntity()->GetPixelPos();
	CFighterFollower* pTarget = GetTarget();
	if (pTarget)
		posTo = pTarget->GetEntity()->GetPixelPos();
	
	TurnAround(szSkillName, posFrom, posTo, pTarget);
}

//-----------------------------------------------------------------------------
//	数据相关
//-----------------------------------------------------------------------------
void CFighterDirector::SaveObjPanelByID(uint32 uObjID, CFighterViewInfo* pViewInfo, bool bIsTargetPanel)
{
	if (bIsTargetPanel)
	{
		CCharacterFollower* pCharacter = CCharacterFollower::GetCharacterByID(uObjID);
		const CSyncVariantClient* pViewVariant = pCharacter->GetViewVariant();
		const CSyncVariantClient* pAttributeVariant = pViewVariant->GetMember("Attribute");
		uint32 pCharType = pAttributeVariant->GetMember("Type")->GetNumber<uint8>();
		if (pCharType == 1)//关注碾压值,目标是玩家的没有碾压值
		{
			return;
		}
	}
	CFighterDirector::MapCFighterViewInfoById::iterator mapFighterViewInfoItr = m_mapCFighterViewInfoById.find(uObjID);
	if (mapFighterViewInfoItr == m_mapCFighterViewInfoById.end())
	{
		AdvAddRef(pViewInfo);	
		m_mapCFighterViewInfoById.insert(make_pair(uObjID, pViewInfo));
	}
	CGac2GasCPP_OpenObjPanel cmd;
	cmd.uObjID = uObjID;
	cmd.IsOpenpanel = true;
	cmd.bIsTargetPanel = bIsTargetPanel;
	SendToGas(&cmd);
}
void CFighterDirector::DeleteObjPanelByID(uint32 uObjID, bool bIsTargetPanel)
{
	CFighterDirector::MapCFighterViewInfoById::iterator mapFighterViewInfoItr = m_mapCFighterViewInfoById.find(uObjID);
	if (mapFighterViewInfoItr != m_mapCFighterViewInfoById.end())
	{
		AdvDelRef(mapFighterViewInfoItr->second);
		m_mapCFighterViewInfoById.erase(mapFighterViewInfoItr);	
	}
	CGac2GasCPP_OpenObjPanel cmd;
	cmd.uObjID = uObjID;
	cmd.bIsTargetPanel = bIsTargetPanel;
	cmd.IsOpenpanel = false;
	SendToGas(&cmd);
}
void CFighterDirector::OnOpenObjPanel(uint32 uObjID, EAttackType eAttackType, uint32 uPropertyID)
{
	CFighterDirector::MapCFighterViewInfoById::iterator mapFighterViewInfoItr = m_mapCFighterViewInfoById.find(uObjID);
	if (mapFighterViewInfoItr != m_mapCFighterViewInfoById.end())
	{
		CCharacterFollower* pCharFol = CCharacterFollower::GetCharacterByID(uObjID);
		uint32 uTargetLevel = pCharFol?pCharFol->CppGetLevel():1;
		bool bPhysicalDamageChanged = false;
		bool bMagicDamageChanged = false;
		float fNormalAttackMinDamage = -1.0f;
		float fNormalAttackMaxDamage = -1.0f;
		float fStrikeRate = -1.0f;
		float fPhysicDefenceRate = -1.0f;
		float fPhysicDodgeRate = -1.0f;
		float fParryRate = -1.0f;
		float fMagicResistRate = -1.0f;
		float fCompleteResistanceRate = -1.0f;
		float fNatureResistanceRate = -1.0f;
		float fDestructionResistanceRate = -1.0f;
		float fEvilResistanceRate = -1.0f;
		float fMagicDodgeRate = -1.0f;
		CFighterViewInfo* pTargetViewInfo = mapFighterViewInfoItr->second;
		switch(uPropertyID)
		{
		case ePID_MainHandMaxWeaponDamage:
		case ePID_MainHandMinWeaponDamage:
		case ePID_PhysicalDPS:
		case ePID_MHWeaponIntervalExtraDamageRate:
		case ePID_PunctureDamage:
		case ePID_ChopDamage:
		case ePID_BluntDamage:
			{
				bPhysicalDamageChanged = true;
				//物理普攻伤害计算
				uint32 uMainHandMinWeaponDamage = pTargetViewInfo->m_MainHandMinWeaponDamage.Get(pTargetViewInfo);
				uint32 uMainHandMaxWeaponDamage = pTargetViewInfo->m_MainHandMaxWeaponDamage.Get(pTargetViewInfo);
				uint32 uPhysicalDPS = pTargetViewInfo->m_PhysicalDPS.Get(pTargetViewInfo);
				float uMainHandWeaponInterval = pTargetViewInfo->m_MHWeaponIntervalExtraDamageRate.Get(pTargetViewInfo);
				uint32 uPunctureDamage = pTargetViewInfo->m_PunctureDamage.Get(pTargetViewInfo);
				uint32 uChopDamage = pTargetViewInfo->m_ChopDamage.Get(pTargetViewInfo);
				uint32 uBluntDamage = pTargetViewInfo->m_BluntDamage.Get(pTargetViewInfo);
				fNormalAttackMinDamage = uMainHandMinWeaponDamage + uPhysicalDPS*uMainHandWeaponInterval; 
				fNormalAttackMaxDamage = uMainHandMaxWeaponDamage + uPhysicalDPS*uMainHandWeaponInterval;
				switch(eAttackType)
				{
				case eATT_Puncture:
					fNormalAttackMaxDamage += uPunctureDamage;
					fNormalAttackMinDamage += uPunctureDamage;
					break;
				case eATT_Chop:
					fNormalAttackMaxDamage += uChopDamage;
					fNormalAttackMinDamage += uChopDamage;
					break;
				case eATT_BluntTrauma:
					fNormalAttackMaxDamage += uBluntDamage;
					fNormalAttackMinDamage += uBluntDamage;
					break;
				default:
					break;
				}
				break;
			}
		case ePID_StrikeValue:
		case ePID_ExtraStrikeRate:
			{
				bPhysicalDamageChanged = true;
				float uStrikeValue	= (float)pTargetViewInfo->m_StrikeValue.Get(pTargetViewInfo);
				fStrikeRate	= uStrikeValue/(uStrikeValue+uTargetLevel*52+700) + (float)pTargetViewInfo->m_ExtraStrikeRate.Get(pTargetViewInfo);
				break;
			}
		case ePID_Defence:
		case ePID_ExtraPhysicDefenceRate:
			{
				bPhysicalDamageChanged = true;
				int32 uDefence	= pTargetViewInfo->m_Defence.Get(pTargetViewInfo);
				float fTempDefence = uDefence>0?(float)uDefence:0.0f;
				fPhysicDefenceRate = fTempDefence/(fTempDefence+uTargetLevel*52+700) + (float)pTargetViewInfo->m_ExtraPhysicDefenceRate.Get(pTargetViewInfo);
				fPhysicDefenceRate = fPhysicDefenceRate>0 ? fPhysicDefenceRate:0;
				break;
			}
		case ePID_PhysicalDodgeValue:
		case ePID_ExtraPhysicDodgeRate:
			{
				bPhysicalDamageChanged = true;
				int32 uPhysicalDodgeValue	= pTargetViewInfo->m_PhysicalDodgeValue.Get(pTargetViewInfo);
				float fTempPhysicalDodgeValue = uPhysicalDodgeValue>0?(float)uPhysicalDodgeValue:0.0f;
				fPhysicDodgeRate = fTempPhysicalDodgeValue/(fTempPhysicalDodgeValue+uTargetLevel*52+700) + (float)pTargetViewInfo->m_ExtraPhysicDodgeRate.Get(pTargetViewInfo);
				fPhysicDodgeRate = fPhysicDodgeRate>0 ? fPhysicDodgeRate:0;
				break;
			}
		case ePID_ParryValue:
		case ePID_ExtraParryRate:
			{
				bPhysicalDamageChanged = true;
				int32 uParryValue	= pTargetViewInfo->m_ParryValue.Get(pTargetViewInfo);
				float fTempParryValue	= uParryValue>0?(float)uParryValue:0.0f;
				fParryRate = fTempParryValue/(fTempParryValue+uTargetLevel*52+700) + (float)pTargetViewInfo->m_ExtraParryRate.Get(pTargetViewInfo);
				fParryRate = fParryRate>0 ? fParryRate:0;
				break;
			}
		
		case ePID_NatureResistanceValue:
		case ePID_DestructionResistanceValue:
		case ePID_EvilResistanceValue:
		case ePID_ExtraMagicResistanceRate:
		case ePID_ExtraNatureResistanceRate:
		case ePID_ExtraDestructionResistanceRate:
		case ePID_ExtraEvilResistanceRate:
		case ePID_ExtraCompleteResistanceRate:
			{
				bMagicDamageChanged = true;
				int32 uNatureResistanceValue = pTargetViewInfo->m_NatureResistanceValue.Get(pTargetViewInfo);
				int32 uDestructionResistanceValue = pTargetViewInfo->m_DestructionResistanceValue.Get(pTargetViewInfo);
				int32 uEvilResistanceValue = pTargetViewInfo->m_EvilResistanceValue.Get(pTargetViewInfo);
				float fTempNatureResistanceValue= uNatureResistanceValue>0?(float)uNatureResistanceValue:0.0f;
				float fTempDestructionResistanceValue= uDestructionResistanceValue>0?(float)uDestructionResistanceValue:0.0f;
				float fTempEvilResistanceValue= uEvilResistanceValue>0?(float)uEvilResistanceValue:0.0f;
				fMagicResistRate = (fTempNatureResistanceValue+fTempDestructionResistanceValue+fTempEvilResistanceValue)/((fTempNatureResistanceValue+fTempDestructionResistanceValue+fTempEvilResistanceValue)+
					uTargetLevel*52+700) + (float)pTargetViewInfo->m_ExtraMagicResistanceRate.Get(pTargetViewInfo);
				fNatureResistanceRate = fTempNatureResistanceValue/(fTempNatureResistanceValue+uTargetLevel*52+700) + (float)pTargetViewInfo->m_ExtraNatureResistanceRate.Get(pTargetViewInfo);
				fDestructionResistanceRate = fTempDestructionResistanceValue/(fTempDestructionResistanceValue+uTargetLevel*52+700) + (float)pTargetViewInfo->m_ExtraDestructionResistanceRate.Get(pTargetViewInfo);
				fEvilResistanceRate = fTempEvilResistanceValue/(fTempEvilResistanceValue+uTargetLevel*52+700) + (float)pTargetViewInfo->m_ExtraEvilResistanceRate.Get(pTargetViewInfo);
				fCompleteResistanceRate = (float)((fNatureResistanceRate+fDestructionResistanceRate+fEvilResistanceRate)/3.0f) + (float)pTargetViewInfo->m_ExtraCompleteResistanceRate.Get(pTargetViewInfo);
				
				fMagicResistRate = fMagicResistRate>0 ? fMagicResistRate:0;
				fNatureResistanceRate = fNatureResistanceRate>0 ? fNatureResistanceRate:0;
				fDestructionResistanceRate = fDestructionResistanceRate>0 ? fDestructionResistanceRate:0;
				fEvilResistanceRate = fEvilResistanceRate>0 ? fEvilResistanceRate:0;
				fCompleteResistanceRate =  fCompleteResistanceRate>0 ? fCompleteResistanceRate:0;
				break;
			}
		case ePID_MagicDodgeValue:
		case ePID_ExtraMagicDodgeRate:
			{
				bMagicDamageChanged = true;
				int32 uMagicDodgeValue	= pTargetViewInfo->m_MagicDodgeValue.Get(pTargetViewInfo);
				float fTempMagicDodgeValue	 = uMagicDodgeValue>0?(float)uMagicDodgeValue:0.0f;
				fMagicDodgeRate = fTempMagicDodgeValue/(fTempMagicDodgeValue+uTargetLevel*52+700) + (float)pTargetViewInfo->m_ExtraMagicDodgeRate.Get(pTargetViewInfo);
				fMagicDodgeRate = fMagicDodgeRate>0 ? fMagicDodgeRate:0;
				break;
			}
		
		default:
			break;
		}
		if (bPhysicalDamageChanged)
		{
			this->GetCallBackHandler()->OnPhysicalDamageChanged(uObjID,fNormalAttackMinDamage,fNormalAttackMaxDamage,fStrikeRate,fPhysicDefenceRate,fPhysicDodgeRate,fParryRate);
		}
		if (bMagicDamageChanged)
		{
			this->GetCallBackHandler()->OnMagicDamageChanged(uObjID,fMagicResistRate,fCompleteResistanceRate,fNatureResistanceRate,fDestructionResistanceRate,fEvilResistanceRate,fMagicDodgeRate);
		}
	}
}

void CFighterDirector::AddLearnSkill(const TCHAR* SkillName,uint32 SkillLevel)
{
	m_mapLearnSkill[SkillName] = SkillLevel;
}

void CFighterDirector::DelLearnSkill(const TCHAR* SkillName,uint32 SkillLevel)
{
	CFighterDirector::MapLearnSkill::iterator itr = m_mapLearnSkill.find(SkillName);
	if (itr != m_mapLearnSkill.end())
	{
		m_mapLearnSkill.erase(itr);
	}
}

void CFighterDirector::ClearAllLearnSkill()
{
	m_mapLearnSkill.clear();
}

void CFighterDirector::CondTestOnTick()
{
	for(CFighterDirector::MapLearnSkill::iterator itr = m_mapLearnSkill.begin(); itr != m_mapLearnSkill.end(); ++itr)
	{
		if (GetCallBackHandler() == NULL)
			return;
		const CSkillClientSharedPtr& pSkill = CSkillClient::GetSkillByName(itr->first.c_str());
		uint32 uRet = PreUseSkill(pSkill, itr->second);
		if(uRet != eDSR_Success && uRet != eDSR_InCDTime &&
			uRet != eDSR_TargetNotExist && uRet != eDSR_TargetIsFriend)
		{
			GetCallBackHandler()->OnMagicCondFailOnTick(itr->first.c_str(),false);
		}
		else //if(uRet == eDSR_Success)
		{
			GetCallBackHandler()->OnMagicCondFailOnTick(itr->first.c_str(),true);
		}
	}
}

void CFighterDirector::AddFXCache(const string& strSkillName)
{
	const CSkillClientSharedPtr& pSkill = CSkillClient::GetSkillByName(strSkillName.c_str());
	if (pSkill&&pSkill->GetCastEffect())
	{
		string strCastEffect = pSkill->GetCastEffect();
		string strFXFile,strFXName;
		GetCharacter()->GetActor()->SplitSkillFxFile(strCastEffect,strFXFile,strFXName,true);
		CEffectLoader::Inst()->AddEftGroup(strFXFile.c_str());
	}
}

void CFighterDirector::RemoveFXCache(const string& strSkillName)
{
	const CSkillClientSharedPtr& pSkill = CSkillClient::GetSkillByName(strSkillName.c_str());
	if (pSkill&&pSkill->GetCastEffect())
	{
		string strCastEffect = pSkill->GetCastEffect();
		string strFXFile,strFXName;
		GetCharacter()->GetActor()->SplitSkillFxFile(strCastEffect,strFXFile,strFXName,true);
		CEffectLoader::Inst()->FreeEftGroup(strFXFile.c_str());
	}
}

//-----------------------------------------------------------------------------
//	协议处理
//-----------------------------------------------------------------------------
void CFighterDirector::OnFighterCommand(const CGas2GacCPP_OnMagicCondFail* pCmd)
{
	if (GetCharacter()->CppGetCtrlState(eFCS_InBattle))
		GetCharacter()->SetDirLockedTarget(true);
	string strSkillName((TCHAR*)(pCmd+1), pCmd->udbSkillNameLen);
	GetCallBackHandler()->OnMagicCondFail(pCmd->uMessageID, strSkillName.c_str());
}

void CFighterDirector::OnFighterCommand(const CGas2GacCPP_OnCoolDownBegin* pCmd)
{
	//cout << pCmd->szSkillName << "\tCGas2GacCPP_OnCoolDownBegin" << endl;
	string strSkillName((TCHAR*)(pCmd+1), pCmd->udbSkillNameLen);
	const CSkillClientSharedPtr& pSkill =CSkillClient::GetSkillByName(strSkillName.c_str());
	ESkillCoolDownType eCoolDownType = pSkill->GetCoolDownType();
	NewCoolDownTime(strSkillName.c_str(), pCmd->uSkillLevel, pCmd->uLeftTime, pCmd->bIsSwitchEquipSkill);
	GetCallBackHandler()->OnCoolDownBegin(strSkillName.c_str(), eCoolDownType, pCmd->uLeftTime);
}
void CFighterDirector::OnFighterCommand(const CGas2GacCPP_OnCoolDownEnd* pCmd)
{
	string strSkillName((TCHAR*)(pCmd+1), pCmd->udbSkillNameLen);
	ResetSingleCoolDownTime(strSkillName.c_str());
	GetCallBackHandler()->OnCoolDownEnd(strSkillName.c_str());
}
void CFighterDirector::OnFighterCommand(const CGas2GacCPP_OnAllCoolDownEnd* pCmd)
{
	m_pCoolDownMgr->ResetAllCoolDown();
	GetCallBackHandler()->OnAllCoolDownEnd();
}

void CFighterDirector::OnFighterCommand(const CGas2GacCPP_OnClearCommonCD* pCmd)
{
	m_pCoolDownMgr->ClearCommonCD();
	GetCallBackHandler()->OnClearCommonCD();
}

void CFighterDirector::OnFighterCommand(const CGas2GacCPP_OnAttackTypeChanged* pCmd)
{
	uint32	uTargetID = pCmd->uTargetID;
	
	CFighterDirector::MapCFighterViewInfoById::iterator mapFighterViewInfoItr = m_mapCFighterViewInfoById.find(uTargetID);
	if (mapFighterViewInfoItr != m_mapCFighterViewInfoById.end())
	{
		this->OnOpenObjPanel(uTargetID, EAttackType(pCmd->uAttackType), 0);
	}
}

void CFighterDirector::OnFighterCommand(const CGas2GacCPP_UpdateSmashRate* pCmd)
{
	float	fSmashRate = pCmd->fValue;
	uint32	uObjID = pCmd->uObjID;
	uint32	uTargetID = pCmd->uTargetID;
	

	CCharacterFollower* pTargetChar = CCharacterFollower::GetCharacterByID(uTargetID);
	if(!pTargetChar)
		return;

	CFighterDirector::MapCFighterViewInfoById::iterator mapFighterViewInfoItr = m_mapCFighterViewInfoById.find(uTargetID);
	if (mapFighterViewInfoItr != m_mapCFighterViewInfoById.end())
	{
		CFighterViewInfo* pViewInfo = mapFighterViewInfoItr->second;
		EAttackType eAttackType = pTargetChar->GetNpcAttackType();
		if(eAttackType < eATT_Puncture || eAttackType > eATT_Evil)
			return;
		uint32 uSmashValue = 0;
		switch(eAttackType)
		{
		case eATT_Puncture:
		case eATT_Chop:
		case eATT_BluntTrauma:
			uSmashValue = pViewInfo->m_DefenceSmashValue.Get(pViewInfo);
			break;
		case eATT_Nature:
			uSmashValue = pViewInfo->m_NatureSmashValue.Get(pViewInfo);
			break;
		case eATT_Destroy:
			uSmashValue = pViewInfo->m_DestructionSmashValue.Get(pViewInfo);
			break;
		case eATT_Evil:
			uSmashValue = pViewInfo->m_EvilSmashValue.Get(pViewInfo);
			break;
		}
		bool bTargetIsDead = !pTargetChar->CppIsAlive();
		GetCallBackHandler()->OnUpdateSmashRate(uTargetID, eAttackType, uSmashValue, fSmashRate, bTargetIsDead);
	}
}

void CFighterDirector::OnFighterCommand(const CGas2GacCPP_OnCalValueChanged* pCmd)
{
	uint32	uValueTypeID = pCmd->uValueTypeID;
	float	fValue = max(0,pCmd->fValue);	//将服务端传下来的值向上取0,拒绝负数...
	uint32	uObjID = pCmd->uObjID;
	uint32	uTargetID = pCmd->uTargetID;
	EAttackType eAttackType = EAttackType(pCmd->uAttackType);
	
	CFighterDirector::MapCFighterViewInfoById::iterator mapFighterViewInfoItr = m_mapCFighterViewInfoById.find(uTargetID);
	if (mapFighterViewInfoItr != m_mapCFighterViewInfoById.end())
	{
		mapFighterViewInfoItr->second->CppSetPropertyValue(uValueTypeID, fValue);
		bool uNeedReAccount = false;
		uint32 uPropertyID = (uValueTypeID-ePFT_Adder)/ePFT_Count;
		this->OnOpenObjPanel(uTargetID, eAttackType, uPropertyID);
		GetCallBackHandler()->OnCalValueChanged(uTargetID);
	}
}


void CFighterDirector::OnFighterCommand(const CGas2GacCPP_OnCancelNormalAttack* pCmd)
{
	CancelNormalAttack();
}

void CFighterDirector::OnFighterCommand(const CGas2GacCPP_OnSetAutoNormalAttack* pCmd)
{
	//m_pNormalAttack->SetAutoNormalAttack(pCmd->bIsAutoNA);
	//if(GetCallBackHandler())
	//{
	//	GetCallBackHandler()->OnSetAutoNormalAttack(pCmd->bIsAutoNA);
	//}
}

void CFighterDirector::OnFighterCommand(const CGas2GacCPP_OnDoNormalAttack* pCmd)
{
	if(!pCmd->bAutoTrack)
	{
		GetCharacter()->CastToCharacterDirector()->CancelAutoTracker(false);
	}
	this->DoNormalAttack(pCmd->uTargetID, pCmd->bAutoTrack);
}

void CFighterDirector::OnFighterCommand(const CGas2GacCPP_OnSetNormalSkillTarget* pCmd)
{
	CFighterFollower::OnFighterCommand(pCmd);
}

void CFighterDirector::OnFighterCommand(const CGas2GacCPP_OnCampChanged* pCmd)
{
	CppSetCamp(ECamp(pCmd->uCamp));
	GetCharacter()->CastToCharacterDirector()->RefreshRelationList();
}

void CFighterDirector::OnFighterCommand(const CGas2GacCPP_OnLevelChanged* pCmd)
{
	uint32 CurLevel = CppGetLevel();
	CFighterFollower::OnFighterCommand(pCmd);
	uint32 DesLevel = CppGetLevel();
	GetCharacter()->CastToCharacterDirector()->RefreshRelationList();
	GetCallBackHandler()->OnLevelUp(DesLevel-CurLevel);
	GetCharacter()->CastToCharacterDirector()->RefreshRelationList();
}

void CFighterDirector::OnFighterCommand(const CGas2GacCPP_OnGameCampChanged* pCmd)
{
	GetCharacter()->CppSetGameCamp(pCmd->iGameCamp);
	GetCharacter()->CastToCharacterDirector()->RefreshRelationList();
}

void CFighterDirector::OnFighterCommand(const CGas2GacCPP_OnPKStateChanged* pCmd)
{
	SetPKState(pCmd->bPKState);
	GetCharacter()->CastToCharacterDirector()->RefreshRelationList();
}

//void CFighterDirector::OnFighterCommand(const CGas2GacCPP_OnPKStateChanged* pCmd)
//{
//	SetPKState(EPKState(pCmd->uPKState));
//	GetCharacter()->CastToCharacterDirector()->RefreshRelationList();
//}

void CFighterDirector::OnFighterCommand(const CGas2GacCPP_OnArrayValueChange* pCmd)
{
	string strName((TCHAR*)(pCmd+1), pCmd->udbNameLen);
	GetCallBackHandler()->OnArrayValueChange(strName.c_str(),pCmd->uTotalValue,pCmd->uCurrentValue,pCmd->uSpeed);
}

void CFighterDirector::OnFighterCommand(const CGas2GacCPP_OnArrayCreate* pCmd)
{
	FightSkill("列阵",1,0);
}

void CFighterDirector::OnFighterCommand(const CGas2GacCPP_OnPrintFightInfo* pCmd)
{
	string strArgName((TCHAR*)(pCmd+1), pCmd->uArgNameLen);
	string strSkillName = "";
	if (pCmd->bSkill && pCmd->uCfgId != 0)
	{
		CSkillClientSharedPtr& pCfg = CSkillClient::GetSkillByID(pCmd->uCfgId);
		strSkillName = pCfg.get()->GetName();
	}
	else if (pCmd->uCfgId != 0)
	{
		CNormalAttackCfgClientSharedPtr& pCfg = CNormalAttackCfgClient::GetNormalAttackById(pCmd->uCfgId);
		strSkillName = pCfg.get()->GetName();
	}
	
	m_pFightInfoData->SetInfoIndex((EFightInfoIndex)pCmd->uInfoIndex);
	m_pFightInfoData->SetObj1ID(pCmd->uObj1ID);
	m_pFightInfoData->SetObj2ID(pCmd->uObj2ID);
	m_pFightInfoData->SetSkillName(strSkillName.c_str());
	m_pFightInfoData->SetArgName(strArgName.c_str());
	m_pFightInfoData->SetHPChanged(pCmd->iHPChanged);
	m_pFightInfoData->SetHPExtra(pCmd->uHPExtra);
	m_pFightInfoData->SetAttackType(EAttackType(pCmd->uAttackType));
	m_pFightInfoData->SetHour(pCmd->uHour);
	m_pFightInfoData->SetMinute(pCmd->uMinute);
	m_pFightInfoData->SetSecond(pCmd->uSecond);
	m_pFightInfoData->SetMillisecond(pCmd->uMillisecond);
	m_pFightInfoData->SetRealHPChanged(pCmd->uHPRealChange);
	const TCHAR* szStateName = "";
	if (pCmd->uStateId != 0)
	{
		szStateName = CBaseStateCfg::GetByGlobalId(pCmd->uStateId)->GetName();
	}
	m_pFightInfoData->SetStateName(szStateName);
		
	GetCallBackHandler()->OnPrintFightInfo(m_pFightInfoData);
}

void CFighterDirector::OnFighterCommand(const CGas2GacCPP_OnSetNoSingTime* pCmd)
{
	if(pCmd->uSkillID == 0)
	{
		//下一次所有吟唱时间为0
		m_bNoSingTimeAll = pCmd->bNoSingTime;
	}
	else
	{
		if(!CSkillClient::GetSkillByID(pCmd->uSkillID))
			return;
		if(pCmd->bNoSingTime)
		{
			m_setNoSingTimeSkill.insert(pCmd->uSkillID);
		}
		else
		{
			m_setNoSingTimeSkill.erase(pCmd->uSkillID);
		}
	}
}

void CFighterDirector::OnFighterCommand(const CGas2GacCPP_OnSyncToSelfCalValueChanged* pCmd)
{
	CppSetPropertyValue(pCmd->uValueTypeID, pCmd->fValue);
}

void CFighterDirector::OnFighterCommand(const CGas2GacCPP_OnReplaceSkill* pCmd)
{
	CCharacterFollower* pCharFol = CCharacterFollower::GetCharacterByID(pCmd->uObjID);
	if (pCharFol)
	{
		string strName((TCHAR*)(pCmd+1), pCmd->uSkillNameLen + pCmd->uReplaceSkillNameLen);
		string strSkillName		   = strName.substr(0,pCmd->uSkillNameLen);
		string strReplaceSkillName = strName.substr(pCmd->uSkillNameLen,pCmd->uSkillNameLen+pCmd->uReplaceSkillNameLen);
	GetCallBackHandler()->OnReplaceSkill(strSkillName.c_str(), strReplaceSkillName.c_str(), pCmd->bReplace);
	}
}

void CFighterDirector::OnFighterCommand(const CGas2GacCPP_OnUseSkillStateChanged* pCmd)
{
	CCharacterFollower* pCharFol = CCharacterFollower::GetCharacterByID(pCmd->uObjID);
	CFighterFollower* pFighterFol = pCharFol ? pCharFol->GetFighter():NULL;
	CFighterDirector* pFighter = pFighterFol ? pFighterFol->CastToFighterDirector():NULL;
	if (pFighter)
	{
		pFighter->SetDoSkillCtrlState((EDoSkillCtrlState)pCmd->uSkillCtrlState, pCmd->bSet);
	}
}

void CFighterDirector::OnFighterCommand(const CGas2GacCPP_OnRevertSkillCtrlState* pCmd)
{
	CCharacterFollower* pCharFol = CCharacterFollower::GetCharacterByID(pCmd->uObjID);
	CFighterFollower* pFighterFol = pCharFol ? pCharFol->GetFighter():NULL;
	CFighterDirector* pFighter = pFighterFol ? pFighterFol->CastToFighterDirector():NULL;
	if (pFighter)
	{
		pFighter->RevertSkillCtrlState();
	}
}

void CFighterDirector::OnFighterCommand(const CGas2GacCPP_OnTalentChanged* pCmd)
{
	string strTalentName((TCHAR*)(pCmd+1), pCmd->udbTalentNameLen);
	m_pTalentHolder->ChangeTalent(strTalentName.c_str(), pCmd->iPoint);
}

void CFighterDirector::OnFighterCommand(const CGas2GacCPP_OnRemoveTalent* pCmd)
{
	string strTalentName((TCHAR*)(pCmd+1), pCmd->udbTalentNameLen);
	m_pTalentHolder->RemoveTalent(strTalentName);
}

void CFighterDirector::OnFighterCommand(const CGas2GacCPP_OnBurstSoulTimesChanged* pCmd)
{
	m_uBurstSoulTimes = pCmd->uBurstSoulTimes;
	if (GetCallBackHandler())
	{
		GetCallBackHandler()->OnBurstSoulTimesChanged(m_uBurstSoulTimes);
	}
}

void CFighterDirector::YieldTick()
{
	SafeDelete(m_pSkillCondMgrClient);
}

void CFighterDirector::ResumeTick()
{
	if (m_pSkillCondMgrClient == NULL)
	{
		m_pSkillCondMgrClient = new CSkillCondMgrClient(this);
	}
}

void CFighterDirector::OnFighterCommand(const CGas2GacCPP_OnAddServant* pCmd)
{
	GetCharacter()->CastToCharacterDirector()->AddServant( ENpcType(pCmd->uType), pCmd->uServantObjID);	
}

void CFighterDirector::OnFighterCommand(const CGas2GacCPP_OnDestroyServant* Cmd)
{
	GetCharacter()->CastToCharacterDirector()->RemoveServant(ENpcType(Cmd->uType), Cmd->uServantID);
}
