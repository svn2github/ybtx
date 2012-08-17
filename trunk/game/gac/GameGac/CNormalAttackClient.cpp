#include "stdafx.h"
#include "CNormalAttackClient.h"
#include "CFighterDirector.h"
#include "CoreCommon.h"
#include "CCfgCalc.inl"
#include "CCharacterDirector.h"
#include "CCoreSceneClient.h"
#include "CAutoTracker.h"
#include "CPixelPath.h"
#include "CRelationMgrClient.h"

CNormalAttackClient::CNormalAttackClient(CFighterDirector* pAttacker)
: m_pAttacker(pAttacker)
, m_pTarget(NULL)
, m_uTargetID(0)
, m_bIsNormalAttacking(false)
, m_fAttackerMoveSpeed(0.0f)
, m_bIsAutoNormalAttack(true)
, m_fMainHandWeaponInterval(1.5)
, m_fAssistantWeaponInterval(1.5)
{
}

CNormalAttackClient::~CNormalAttackClient(void)
{
}

uint32 CNormalAttackClient::GetTickTime()
{
	static uint32 ms_uTickTime = 500;
	return ms_uTickTime;
}

void CNormalAttackClient::InitNormalAttack(CFighterFollower* pTarget, float fSpeed, bool bIsNeedAutoTracker)
{
	m_pTarget = pTarget;
	m_uTargetID = pTarget->GetEntityID();
	m_fAttackerMoveSpeed = fSpeed;

	CCharacterDirector* pAttacker = class_cast<CCharacterDirector*>(m_pAttacker->GetCharacter());
	float fReachGridDist = float(m_pNormalAttackCfg->GetMaxAttackDis()->GetDblValue(m_pAttacker)); //之所以乘以0.7是为了解决自动追踪停止的地点有可能还未进入普攻范围的问题

	class CNAAutoTrackHandler : public IAutoTrackerHandler
	{
		virtual bool CreatedInCPP(){ return true; }
		virtual void OnReached(CEntityClient* pAttacker, CEntityClient* pTarget)
		{
		}
		virtual bool OnCheckStopCond(CEntityClient* pAttacker, CEntityClient* pTarget)
		{
			return true;
		}
	};

	if(bIsNeedAutoTracker && !pAttacker->IsMovingByKeyboard())
	{
		CNAAutoTrackHandler* pAutoTrackHandler = new CNAAutoTrackHandler();
		if (!pAttacker->InitAutoTracker(pAttacker->GetEntityID(), m_uTargetID, fReachGridDist, pAutoTrackHandler, true))
		{
			delete pAutoTrackHandler;
		}
	}

	m_bIsNormalAttacking = true;
}

void CNormalAttackClient::CancelNormalAttack()
{
	m_bIsNormalAttacking = false;
	m_pTarget = NULL;
}

bool CNormalAttackClient::IsTargetInNAScope(CFighterFollower* pTarget)
{
	//CFPos posAttacker, posTarget;
	//posAttacker = m_pAttacker->GetEntity()->GetPixelPos();
	//pTarget->GetEntity()->GetServerPixelPos(posTarget);
	//float x = posAttacker.x - posTarget.x;
	//float y = posAttacker.y - posTarget.y;
	//float fDistance = sqrt(x*x + y*y);
	CEntityClient* pEntityFrom = m_pAttacker->GetEntity();
	CEntityClient* pEntityTo = pTarget->GetEntity();
	Ast(pEntityFrom);
	Ast(pEntityTo);
	float fDistance = pEntityFrom->GetEntityDist(pEntityTo);
	float fMaxAttckDis = float(m_pNormalAttackCfg->GetMaxAttackDis()->GetDblValue(m_pAttacker));
	//cout << "fMaxAttckDis = " << fMaxAttckDis*eGridSpanForObj << " fDistance = " << uDistance <<endl;
	return fDistance - fMaxAttckDis*eGridSpanForObj <= 1.0f;
}

uint32 CNormalAttackClient::CheckNACondAndSetNATarget(CFighterFollower* pTarget)
{
	if(!m_pAttacker->CppIsAlive())
	{
		return eDSR_CharacterHasDead;
	}

	if(m_pAttacker->GetCtrlState(eFCS_ForbitNormalAttack))
	{
		return eDSR_ForbitUseSkill;
	}

	CRelationMgrClient& relationMgr = CRelationMgrClient::Inst();
	switch (relationMgr.GetRelationType(m_pAttacker, pTarget))
	{
	case eRT_DuelTargetWrong:
		return eDSR_DuelTargetWrong;
		break;
	case eRT_TargetInDuel:
		return eDSR_TargetInDuel;
		break;
	default:
		break;
	}

	bool bIsTargetInNAScope = IsTargetInNAScope(pTarget);
	if(!bIsTargetInNAScope && !IsAutoNormalAttack())
	{
		return eDSR_OutOfDistance;
	}

	//远程普攻检测是否有障碍
	if(!IsShortDistanceNA() && !IsAutoNormalAttack())
	{
		CPixelPath* pPath = m_pAttacker->GetEntity()->GetScene()->CreatePath(m_pAttacker->GetEntity()->GetPixelPos(), pTarget->GetEntity()->GetPixelPos(), eFPT_HypoLine, eBT_MidBarrier);
		if( pPath == NULL)
			return eDSR_ToTargetHaveBarrier;
		SafeRelease(pPath);
	}
	
	return eDSR_Success;
}

void CNormalAttackClient::SetMHNACfg(const TCHAR* szName,float fMainHandWeaponInterval,EWeaponType MHWeaponType, EAttackType eAttackType)
{
	m_pNormalAttackCfg = CNormalAttackCfgClient::GetNormalAttack(szName);
	m_fMainHandWeaponInterval=fMainHandWeaponInterval;
	SetMHWeaponType(MHWeaponType);
	SetMHAttackType(eAttackType);

	if (m_pAttacker->GetCharacter() && m_pAttacker->GetCharacter()->GetActor())
	{
		uint8 uWalkBackRadius = uint8(m_pNormalAttackCfg->GetMaxAttackDis()->GetDblValue(m_pAttacker)) + 2;
		m_pAttacker->GetCharacter()->GetActor()->SetWalkBackRadius(uWalkBackRadius);
	}
}

void CNormalAttackClient::SetAHNACfg(const TCHAR* szName,float fAssistantWeaponInterval, EWeaponType AHWeaponType)
{
	SetAHWeaponType(AHWeaponType);
	m_fAssistantWeaponInterval=fAssistantWeaponInterval;
}

bool CNormalAttackClient::IsShortDistanceNA()const
{
	if(m_pNormalAttackCfg)	
	{
		float fMaxAttackDis = float(m_pNormalAttackCfg->GetMaxAttackDis()->GetDblValue(m_pAttacker));
		if(fMaxAttackDis > 5.0f)
			return false;		
	}

	return true;
}

bool CNormalAttackClient::HasWeapon()
{
	return true;
}

float CNormalAttackClient::GetNormalAttackDistance()
{
	if (m_pNormalAttackCfg)
	{
		CCfgCalc* pCfg = m_pNormalAttackCfg->GetMaxAttackDis();
		if (pCfg)
			return float(pCfg->GetDblValue(m_pAttacker));
	}
	return 0.0f;
}
