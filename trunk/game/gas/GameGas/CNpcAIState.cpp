#include "stdafx.h"
#include "CNpcAI.h"
#include "CCharacterDictator.h"
#include "ICharacterDictatorCallbackHandler.h"
#include "CFighterDictator.h"
#include "CServantAI.h"
#include "NormalAttack.h"
#include "CoreCommon.h"
#include "CNpcAINatureMgr.h"
#include "CIntObjServer.h"
#include "CAllStateMgr.h"
#include "CSyncVariantServer.h"
#include "CBattleStateManager.h"
#include "Random.h"
#include "NpcInfoMgr.h"
#include "CSkillRuleMgr.h"
#include "CNpcAlertMgr.h"
#include "BaseHelper.h"
#include "CLittlePetAI.h"
#include "CDir.h"

//-------------------------- NPC状态函数定义 -------------------------
DEFINE_NPCSTATE_METHOD(MoveBase,LeaveState)
{
}
END_DEFINE_NPCSTATE_METHOD

DEFINE_NPCSTATE_METHOD(PatrolForNpc, EnterState)
{
	//cout<<"PatrolForNpc,EnterState ==="<<pEvent->GetName()<<endl;
	Ast(GetOwner());
	if(eNpcEvent_OnStop == pEvent->GetID() || eNpcEvent_OnDead == pEvent->GetID())
		return pEvent;
	GetOwner()->MoveToPixelInLinePath(GetOwner()->m_DestPos, GetOwner()->m_bDefaultWalkState);
	GetOwner()->CheckNpcMoveState(GetOwner()->m_DestPos, "DEFINE_NPCSTATE_METHOD(PatrolForNpc, EnterState)");
}
END_DEFINE_NPCSTATE_METHOD

DEFINE_NPCSTATE_METHOD(PatrolForNpc, OnEvent)
{
	//cout<<"PatrolForNpc,OnEvent ==="<<pEvent->GetName()<<endl;
	Ast(GetOwner());
	Ast(GetOwner()->GetCharacter());
	switch(pEvent->GetID())
	{
	case eNpcEvent_OnMoveEnded:
			if (GetOwner()->m_bAlertState)
				return pEvent;
			GetOwner()->MoveToDestEnded();
			GetOwner()->CheckPlayInNpcSleepDim();
			break;
	case eNpcEvent_OnMoveToDest:
	case eNpcEvent_OnCanMove:
			GetOwner()->MoveToPixelInLinePath(GetOwner()->m_DestPos, GetOwner()->m_bDefaultWalkState);
			GetOwner()->CheckNpcMoveState(GetOwner()->m_DestPos, "DEFINE_NPCSTATE_METHOD(PatrolForNpc, OnEvent)");
			break;
	case eNpcEvent_OnEnterAlertTarget:
	case eNpcEvent_OnCharacterInSight:
		{
			ENpcAIType AIType = GetOwner()->GetRealAIType();
			if (NpcInfoMgr::BeActiveNpc(AIType))
			{
				CCharacterDictator* pCharacter = CCharacterDictator::GetCharacterByID((size_t)pEvent->GetTag());
				if (NULL == pCharacter || NULL == GetOwner()->GetCharacter())
					return pEvent;
				if (GetOwner()->CanFight(pCharacter) && GetOwner()->IsInLockEnemyRange(pCharacter->GetFighter()))
				{
					if (GetOwner()->BeAlertNpc((size_t)pEvent->GetTag()))
					{
						if (!GetOwner()->GetNpcAlertMgr())
						{
							if (pEvent->GetID() == eNpcEvent_OnCharacterInSight)
							{
								GetOwner()->CreateAlertMgr((size_t)pEvent->GetTag());
							}
							else
							{
								uint32 uAlertTime = (size_t)pEvent->GetArg();
								GetOwner()->CreateAlertMgr((size_t)pEvent->GetTag(), false, uAlertTime);
							}
						}
						else 
							GetOwner()->GetNpcAlertMgr()->AddAlertTarget((size_t)pEvent->GetTag());
					}
					else
					{
						GetOwner()->BeginAttack((size_t)pEvent->GetTag());
						return pEvent;
					}
				}
			}
		}
		break;
	case eNpcEvent_OnLeaveAlertTarget:
		{
			CCharacterDictator* pCharacter = CCharacterDictator::GetCharacterByID((size_t)pEvent->GetTag());
			if (pCharacter && GetOwner()->GetNpcAlertMgr())
			{
				GetOwner()->GetNpcAlertMgr()->RemoveAlertTarget((size_t)pEvent->GetTag());
				if (!GetOwner()->GetNpcAlertMgr())
				{
					EnterState(pEvent);
				}
			}
		}
		break;
	default:
		break;
	}
}
END_DEFINE_NPCSTATE_METHOD

DEFINE_NPCSTATE_METHOD(PatrolForNpc, LeaveState)
{
	GetOwner()->DestoryNpcAlertMgr();
	if (GetOwner()->m_pPatrolTick)
	{
		GetOwner()->GetCharacter()->UnRegistDistortedTick(GetOwner()->m_pPatrolTick);
		SafeDelete(GetOwner()->m_pPatrolTick);
	}
	if (GetOwner()->m_pSpecialActionTick)
	{
		GetOwner()->GetCharacter()->UnRegistDistortedTick(GetOwner()->m_pSpecialActionTick);
		SafeDelete(GetOwner()->m_pSpecialActionTick);
	}
}
END_DEFINE_NPCSTATE_METHOD

DEFINE_NPCSTATE_METHOD(Idle, EnterState)
{
	Ast(GetOwner());
	//cout<<"NPC ID = "<<GetOwner()->GetCharacter()->GetEntityID()<<"--"<<pEvent->GetName()<<"==Idle, EnterState"<<endl;

	class DoSpecialActionTick
		: public CDistortedTick 
		, public CNpcAIMallocObject
	{
	public:
		DoSpecialActionTick(CNpcAI* pAI) :m_pAI(pAI)
		{};

		void OnTick()
		{
			m_pAI->GetCharacter()->UnRegistDistortedTick(this);
			Ast(m_pAI);
			CCharacterDictator* pAICharacterDictator = m_pAI->GetCharacter();
			Ast(pAICharacterDictator);
			pAICharacterDictator->SetAndSyncActionState(eAS_Idle_Special);
 			uint32 utime = Random::Rand(10, 20);
			m_pAI->GetCharacter()->RegistDistortedTick(this, utime * 1000);
		};

	private:
		CNpcAI* m_pAI;
	};
	
	uint32 utime = Random::Rand(10, 20);
	m_pDoSpecialActionTick = new DoSpecialActionTick(GetOwner());
	GetOwner()->GetCharacter()->RegistDistortedTick(m_pDoSpecialActionTick, 1000 * utime);

}
END_DEFINE_NPCSTATE_METHOD

DEFINE_NPCSTATE_METHOD(Idle, LeaveState)
{
	//cout<<"NPC ID = "<<GetOwner()->GetCharacter()->GetEntityID()<<"--"<<pEvent->GetName()<<"==Idle, LeaveState"<<endl;
	if (m_pDoSpecialActionTick)
	{
		GetOwner()->GetCharacter()->UnRegistDistortedTick(m_pDoSpecialActionTick);
		delete m_pDoSpecialActionTick;
		m_pDoSpecialActionTick = NULL;
	}
}
END_DEFINE_NPCSTATE_METHOD

DEFINE_NPCSTATE_METHOD(Wander, EnterState)
{
	Ast(GetOwner());
	if (GetOwner()->GetNatureMgr() && GetOwner()->GetNatureMgr()->IsEventListend(eNpcEvent_OnSleepyIdle))
	{
		CNpcEventMsg::Create(GetOwner(),eNpcEvent_OnSleepyIdle);
		return pEvent;
	}
	
	GetOwner()->CheckAndDoAction();

	GetOwner()->OnEnterWanderState();
}
END_DEFINE_NPCSTATE_METHOD

DEFINE_NPCSTATE_METHOD(Wander, OnEvent)
{
	switch(pEvent->GetID())
	{
	case eNpcEvent_OnMoveEnded:
		GetOwner()->StopMoving();
		break;
	case eNpcEvent_OnBeginRandomMove:
		{
			GetOwner()->CheckAndDoAction();
		}
		break;
	default:
		break;
	}
}
END_DEFINE_NPCSTATE_METHOD

DEFINE_NPCSTATE_METHOD(Wander, LeaveState)
{
	GetOwner()->DestoryActionTick();
	GetOwner()->OnLeaveWanderState();
}
END_DEFINE_NPCSTATE_METHOD

//主动怪的主动状态
DEFINE_NPCSTATE_METHOD(ActiveState, EnterState)
{
	//cout<<"NPC ID = "<<GetOwner()->GetCharacter()->GetEntityID()<<"--"<<pEvent->GetName()<<"==导致进入了状态ActiveState, EnterState"<<endl;
	Ast(GetOwner());
	if(eNpcEvent_OnStop == pEvent->GetID() || eNpcEvent_OnDead == pEvent->GetID())
		return pEvent;
	
	if (GetOwner()->GetNatureMgr() && GetOwner()->GetNatureMgr()->IsEventListend(eNpcEvent_OnSleepyIdle))
	{
		CNpcEventMsg::Create(GetOwner(),eNpcEvent_OnSleepyIdle);
		return pEvent;
	}

	GetOwner()->CheckAndDoAction();

	GetOwner()->OnEnterWanderState();
}
END_DEFINE_NPCSTATE_METHOD

DEFINE_NPCSTATE_METHOD(ActiveState, OnEvent)
{
	//cout<<"NPC ID = "<<GetOwner()->GetCharacter()->GetEntityID()<<"--"<<pEvent->GetName()<<"==````ActiveState, OnEvent"<<endl;
	CCharacterDictator *pCharacter = NULL;
	ENpcAIType AIType;
	switch(pEvent->GetID())
	{
	
	case eNpcEvent_OnMoveEnded:
		GetOwner()->StopMoving();
		return pEvent;
	case eNpcEvent_OnEnterAlertTarget:
	case eNpcEvent_OnCharacterInSight:
		{
			AIType = GetOwner()->GetRealAIType();
			if (NpcInfoMgr::BeActiveNpc(AIType))
			{
				pCharacter = CCharacterDictator::GetCharacterByID((size_t)pEvent->GetTag());
				if (NULL == pCharacter || NULL == GetOwner()->GetCharacter())
					return pEvent;
				if (GetOwner()->CanFight(pCharacter) && GetOwner()->IsInLockEnemyRange(pCharacter->GetFighter()))
				{
					if (GetOwner()->BeAlertNpc((size_t)pEvent->GetTag()))
					{
						if (!GetOwner()->GetNpcAlertMgr())
						{
							if (pEvent->GetID() == eNpcEvent_OnCharacterInSight)
							{
								GetOwner()->CreateAlertMgr((size_t)pEvent->GetTag());
							}
							else
							{
								uint32 uAlertTime = (size_t)pEvent->GetArg();
								GetOwner()->CreateAlertMgr((size_t)pEvent->GetTag(), false, uAlertTime);
							}
						}
						else 
							GetOwner()->GetNpcAlertMgr()->AddAlertTarget((size_t)pEvent->GetTag());
					}
					else
					{
						GetOwner()->BeginAttack((size_t)pEvent->GetTag());
						return pEvent;
					}
				}
			}
		}
		break;
		//case eNpcEvent_OnCharacterOutOfSight:
	case eNpcEvent_OnLeaveAlertTarget:
		{
			CCharacterDictator* pCharacter = CCharacterDictator::GetCharacterByID((size_t)pEvent->GetTag());
			if (pCharacter && GetOwner()->GetNpcAlertMgr())
			{
				GetOwner()->GetNpcAlertMgr()->RemoveAlertTarget((size_t)pEvent->GetTag());
				if (!GetOwner()->GetNpcAlertMgr())
				{
					EnterState(pEvent);
				}
			}
		}
		break;
	case eNpcEvent_OnBeginRandomMove:
		{
			GetOwner()->CheckAndDoAction();
		}
		break;
	default:
		break;
	}
}
END_DEFINE_NPCSTATE_METHOD

DEFINE_NPCSTATE_METHOD(ActiveState, LeaveState)
{
	//cout<<"NPC ID = "<<GetOwner()->GetCharacter()->GetEntityID()<<"--"<<pEvent->GetName()<<"导致离开````ActiveState, LeaveState"<<endl;
	GetOwner()->DestoryNpcAlertMgr();
	GetOwner()->DestoryActionTick();
	GetOwner()->OnLeaveWanderState();
}
END_DEFINE_NPCSTATE_METHOD

DEFINE_NPCSTATE_METHOD(DeathOrDestoryState, EnterState)
{
	//cout<<"DEFINE_NPCSTATE_METHOD(DeathOrDestoryState, EnterState)"<<endl;
	Ast (GetOwner());
	CEnmityMgr* pEnemyMgr = GetOwner()->GetEnmityMgr();
	Ast (pEnemyMgr);
	pEnemyMgr->ClearAllEnmity();
	CCharacterDictator* pCharacter = GetOwner()->GetCharacter();
	Ast(pCharacter);
	//pCharacter->SetMaster(NULL);
}
END_DEFINE_NPCSTATE_METHOD

DEFINE_NPCSTATE_METHOD(Building, EnterState)
{
	Ast(GetOwner());
	CCharacterDictator* pCharacter = GetOwner()->GetCharacter();
	Ast(GetOwner());
	pCharacter->CppSetCtrlState(eFCS_ForbitMove, true);
	pCharacter->CppSetCtrlState(eFCS_ForbitTurnAround, true);
}
END_DEFINE_NPCSTATE_METHOD

DEFINE_NPCSTATE_METHOD(LockEnemy, EnterState)
{
	//cout<<"NPC ID = "<<GetOwner()->GetCharacter()->GetEntityID()<<"--"<<pEvent->GetName()<<"==进入LockEnemy, EnterState"<<endl;
	Ast(GetOwner());
	CCharacterDictator* pOwnCharacter = GetOwner()->GetCharacter();
	Ast(pOwnCharacter);
	
	CFighterDictator* pTargetFighter = GetOwner()->GetFirstEnenmyFighter();
	if (pTargetFighter && GetOwner()->CanFight(pTargetFighter->GetCharacter()))
	{
		pOwnCharacter->SetEyeSight(0);	
		pOwnCharacter->GetPixelPos(GetOwner()->m_ChaseBeginPos);
		GetOwner()->BeginChaseOriginTrace();
		CNpcEventMsg::Create(GetOwner(), eNpcEvent_OnBeginAttackEnemy, eNECI_One);
		GetOwner()->StopMoving();
	}
	else
	{
		CNpcEventMsg::Create(GetOwner(),eNpcEvent_OnEnemyLost, eNECI_Eight);
	}
}
END_DEFINE_NPCSTATE_METHOD

void CNpcAI::CNpcStateChaseEnemy::StartTrace()
{
	CNpcAI* pAI = GetOwner();
	Ast(pAI);
	CCharacterDictator* pOwnCharacter = pAI->GetCharacter();
	Ast(pOwnCharacter);
	CFighterDictator* pOwnFighter = pOwnCharacter->GetFighter();
	Ast(pOwnFighter);
	CEnmityMgr* pOwnEnmityMgr = GetOwner()->GetEnmityMgr();
	Ast (pOwnEnmityMgr);

	if (pOwnFighter->GetCtrlState(eFCS_ForbitMove))
		return;
	ENpcType eNpcType = pAI->GetRealNpcType();
	if (eNpcType == ENpcType_AttackNearest)
		pOwnEnmityMgr->GetNearestEnemy();
	pAI->ReSetTarget();
	CCharacterDictator* pEnemyCharacter = pAI->GetFirstEnmityEnemy();
	if (!pEnemyCharacter)
	{
		CNpcEventMsg::Create(GetOwner(),eNpcEvent_OnEnemyLost, eNECI_Nine);
		return;
	}
	CFighterDictator* pEnemyFighter = pEnemyCharacter->GetFighter();
	Ast(pEnemyFighter);
	if (GetOwner()->IsInAttackRange(pEnemyFighter))
	{
		if (GetOwner()->AttackTargetHaveHighBarrier())
		{
			CFPos targetPos;
			pEnemyCharacter->GetPixelPos(targetPos);	
			bool bWalkState = false;
			if (pOwnCharacter->CppGetCtrlState(eFCS_InWalkState) != bWalkState)
				pOwnCharacter->CppSetCtrlState(eFCS_InWalkState, bWalkState);
			GetOwner()->MoveToPixel(targetPos, bWalkState, 0);
			GetOwner()->CheckNpcMoveState(targetPos, "CNpcAI::MoveToTaretPos()");
			return;
		}
		CNpcEventMsg::Create(GetOwner(),eNpcEvent_OnBeginAttackEnemy,eNECI_Two);
		return;
	}

	float fDist = GetOwner()->GetAttackScope() + pEnemyCharacter->GetBottomSize() + pOwnCharacter->GetBottomSize(); 
	//if (!GetOwner()->BeRegulate())		//如果不排队就直接走近一点确保在攻击距离内
		fDist = (fDist-1) > 0 ? (fDist - 1) : 0 ;
	if (m_eBarrierType == eBT_LowBarrier )
		pAI->TraceEnemy(pEnemyCharacter,eBT_LowBarrier, fDist);
	else
		pAI->TraceEnemy(pEnemyCharacter,eBT_HighBarrier, fDist);
}

DEFINE_NPCSTATE_METHOD(ChaseEnemy, EnterState)
{
	Ast (GetOwner());
	m_eBarrierType = eBT_LowBarrier;
	m_bInRegulate = false;
	GetOwner()->StopMoving();
	StartTrace();
}
END_DEFINE_NPCSTATE_METHOD

DEFINE_NPCSTATE_METHOD(ChaseEnemy, OnEvent)
{
	Ast (GetOwner());
	CCharacterDictator* pOwnerCharacter = GetOwner()->GetCharacter();
	Ast (pOwnerCharacter);
	CCharacterDictator* pTarget = GetOwner()->GetFirstEnmityEnemy();
	switch(pEvent->GetID())
	{
		case eNpcEvent_OnTraceSuccess:
		{
			if (GetOwner()->BeRegulate())
			{
				if (pTarget && !pTarget->IsMoving() && GetOwner()->RegulateNpcPlace())
					m_bInRegulate = true;
				else
					CNpcEventMsg::Create(GetOwner(),eNpcEvent_OnBeginAttackEnemy, eNECI_Three);
			}
			else
				CNpcEventMsg::Create(GetOwner(),eNpcEvent_OnBeginAttackEnemy, eNECI_Four);
		}
		break;

		case eNpcEvent_OnTraceFailed:
		{
			m_eBarrierType = eBT_HighBarrier;
			StartTrace();
			m_bInRegulate = false;
		}
		break;

		case eNpcEvent_OnMoveEnded:
		{
			if (m_bInRegulate)
			{
				CNpcEventMsg::Create(GetOwner(),eNpcEvent_OnBeginAttackEnemy,eNECI_Five);
				m_bInRegulate = false;
				break;
			}
		}
		default:
		{
			if (!pOwnerCharacter->IsTracing() && !m_bInRegulate)	//当没有在trce并且没有在排队的时候就trace
				StartTrace();
		}
		break;
	}
}
END_DEFINE_NPCSTATE_METHOD

DEFINE_NPCSTATE_METHOD(ChaseEnemy, LeaveState)
{
	Ast (GetOwner());
	GetOwner()->StopTraceEnemy();
}
END_DEFINE_NPCSTATE_METHOD

void CNpcAI::CNpcStateAttackEnemy::CreateAttackAniTick()
{
	struct AnimationTick 
		: public CDistortedTick
		, public CNpcAIMallocObject 
	{
		AnimationTick(CNpcStateAttackEnemy* pState) { m_pState = pState; }
		void OnTick()
		{
			CCharacterDictator* pCharacter = m_pState->GetOwner()->GetCharacter();
			pCharacter->UnRegistDistortedTick(this);
			Ast(m_pState);
			Ast(m_pState->GetOwner());
			m_pState->GetOwner()->SetIsDoingAttack(false);
			CNpcEventMsg::Create(m_pState->GetOwner(),eNpcEvent_OnAttackAnimationEnd);
		}
		CNpcStateAttackEnemy* m_pState;
	};

	CCharacterDictator* pCharacter = GetOwner()->GetCharacter();
	Ast(pCharacter);
	CFighterDictator* pFighter = pCharacter->GetFighter();
	Ast(pFighter);
	GetOwner()->FaceToEnemy();
	GetOwner()->ReSetTarget();
	uint32 uAttackTime = 1000;
	bool bForbitAttack = pFighter->GetCtrlState(eFCS_ForbitNormalAttack);
	if (!bForbitAttack)
	{
		CNormalAttackMgr* pNormalAttackMgr = pFighter->GetNormalAttackMgr();
		Ast(pNormalAttackMgr);
		CSingleHandNATick* pAssistHandNATick = pNormalAttackMgr->GetMHNormalAttack();
		Ast(pAssistHandNATick);
		uint8 uAniKeyFrame = pAssistHandNATick->DoNormalAttackOnce();
		uAttackTime = uAniKeyFrame*33;
	}
	if(!m_pAttackEnemyAnimationTick)
		m_pAttackEnemyAnimationTick = new AnimationTick(this);
	GetOwner()->GetCharacter()->RegistDistortedTick(m_pAttackEnemyAnimationTick, uAttackTime);
}

void CNpcAI::CNpcStateAttackEnemy::CreateNormalAttackCoolDownTick()
{
	struct NormalAttackCoolDownTick
		: public CDistortedTick
		, public CNpcAIMallocObject 
	{
		NormalAttackCoolDownTick(CNpcStateAttackEnemy* pState) { m_pState = pState; }
		void OnTick()
		{
			m_pState->GetOwner()->GetCharacter()->UnRegistDistortedTick(this);
			Ast(m_pState);
			Ast(m_pState->GetOwner());
			m_pState->m_bInNormalAttackEnemyCoolDown = true;
			CNpcEventMsg::Create(m_pState->GetOwner(), eNpcEvent_OnWeaponCoolDownEnd);
		}
		CNpcStateAttackEnemy* m_pState;
	};
	CCharacterDictator* pCharacter = GetOwner()->GetCharacter();
	Ast (pCharacter);
	CFighterDictator* pFighter = pCharacter->GetFighter();
	Ast (pFighter);
	float fWeaponInterval = GetOwner()->GetWeaponSpeed();
	bool bForbitAttack = pFighter->GetCtrlState(eFCS_ForbitNormalAttack);
	if (bForbitAttack)				//如果禁止普攻的话则1秒冷却
		fWeaponInterval = 1.0f;		
	fWeaponInterval = fWeaponInterval * 1000;
	uint32 uRandMaxAttackSpeed = GetOwner()->GetRandMaxAttackSpeed();
	if (GetOwner()->GetNormalAttackFirstTime() && uRandMaxAttackSpeed > 1)
	{
		fWeaponInterval = (float)Random::Rand(1,uRandMaxAttackSpeed);
		GetOwner()->SetNormalAttackFirstTime(false);
	}
	if(!m_pNormalAttackEnemyCoolDownTick)
		m_pNormalAttackEnemyCoolDownTick = new NormalAttackCoolDownTick(this);
	GetOwner()->GetCharacter()->RegistDistortedTick(m_pNormalAttackEnemyCoolDownTick, (uint32)(fWeaponInterval));
}

void CNpcAI::CNpcStateAttackEnemy::CreateDelayDoAttackTick()
{
	struct DelayDoAttackTick
		: public CDistortedTick
		, public CNpcAIMallocObject 
	{
		DelayDoAttackTick(CNpcStateAttackEnemy* pState) { m_pState = pState; }
		void OnTick()
		{
			m_pState->GetOwner()->GetCharacter()->UnRegistDistortedTick(this);
			Ast (m_pState);
			m_pState->CreateAttackAniTick();
			m_pState->CreateNormalAttackCoolDownTick();		
		}
		CNpcStateAttackEnemy* m_pState;
	};

	Ast (GetOwner());
	CCharacterDictator* pCharacter = GetOwner()->GetCharacter();
	uint32 uDelayTime = pCharacter->GetFollowerDelay() + 50;
	GetOwner()->SetIsDoingAttack(true);
	GetOwner()->StopMoving();
	m_bInNormalAttackEnemyCoolDown = false;
	if(!m_pDelayAttackEnemyTick)
		m_pDelayAttackEnemyTick = new DelayDoAttackTick(this);
	GetOwner()->GetCharacter()->RegistDistortedTick(m_pDelayAttackEnemyTick, uDelayTime);
}

void CNpcAI::CNpcStateAttackEnemy::CancelNormalAttack(bool bHurt)
{
	Ast (GetOwner());
	CCharacterDictator* pCharacter = GetOwner()->GetCharacter();
	Ast (pCharacter);
	CFighterDictator* pFighter = pCharacter->GetFighter();
	Ast (pFighter);
	pFighter->CancelAttack(bHurt);
	if (m_pAttackEnemyAnimationTick)
		GetOwner()->GetCharacter()->UnRegistDistortedTick(m_pAttackEnemyAnimationTick);
	if (m_pDelayAttackEnemyTick && m_pDelayAttackEnemyTick->Registered())
	{
		GetOwner()->GetCharacter()->UnRegistDistortedTick(m_pDelayAttackEnemyTick);
		m_bInNormalAttackEnemyCoolDown = true;
	}
	/*if (m_pNormalAttackEnemyCoolDownTick)
		GetOwner()->GetCharacter()->UnRegistDistortedTick(m_pNormalAttackEnemyCoolDownTick);*/
	GetOwner()->SetIsDoingAttack(false);
}

bool CNpcAI::CNpcStateAttackEnemy::CheckAttackDis()
{
	CCharacterDictator* pEnemy = GetOwner()->GetFirstEnmityEnemy();
	if (!pEnemy)
	{
		CNpcEventMsg::Create(GetOwner(),eNpcEvent_OnEnemyLost, eNECI_Ten);
		return false;
	}
	CFighterDictator* pEnemyFighter = pEnemy->GetFighter();
	if (!GetOwner()->IsInAttackRange(pEnemyFighter))
	{
		if (GetOwner()->IsChaosEnmity())
			GetOwner()->GetEnmityMgr()->GetNearestEnemy();
		return false;
	}
	return true;
}

bool CNpcAI::CNpcStateAttackEnemy::CanDoSkillRule()
{
	Ast (GetOwner());
	CCharacterDictator* pCharacter = GetOwner()->GetCharacter();
	Ast (pCharacter);
	CFighterDictator* pFighter = pCharacter->GetFighter();
	Ast (pFighter);
	CSkillRuleMgr* pSkillRuleMgr = GetOwner()->GetSkillRuleMgr();
	if (pSkillRuleMgr
		&& pSkillRuleMgr->GetIsHavePreparedSkill() 
		&& !GetOwner()->GetIsDoingSkillRule() 
		&& !pFighter->GetDoSkillCtrlState(eDSCS_ForbitUseSkill) 
		&& !pFighter->GetDoSkillCtrlState(eDSCS_InDoingSkill))
	{
		return true;
	}
	else
	{
		return false;
	}

}

DEFINE_NPCSTATE_METHOD(AttackEnemy, EnterState)
{
	//cout<<"进入AttackEnemy状态: "<<pEvent->GetName()<<endl;
	Ast (GetOwner());
	CCharacterDictator* pCharacter = GetOwner()->GetCharacter();
	Ast (pCharacter);
	CFighterDictator* pFighter = pCharacter->GetFighter();
	Ast (pFighter);
	m_pAttackEnemyAnimationTick = NULL;
	m_pNormalAttackEnemyCoolDownTick = NULL;
	m_pDelayAttackEnemyTick = NULL;
	m_bInNormalAttackEnemyCoolDown = true;
	GetOwner()->StopMoving();
	GetOwner()->SetIsDoingAttack(false);
	GetOwner()->SetIsDoingSkillRule(false);
	GetOwner()->ReSetTarget();
	CSkillRuleMgr* pSkillMgr = GetOwner()->GetSkillRuleMgr();
	if (pSkillMgr)
		pSkillMgr->ClearCurSkill();
	GetOwner()->FaceToEnemy();	
	pCharacter->GetFightDirVariant()->SetNumber((bool)true);
	pFighter->Notify(eCE_BeInterrupted);
	OnEvent(pEvent);
}
END_DEFINE_NPCSTATE_METHOD

DEFINE_NPCSTATE_METHOD(AttackEnemy, OnEvent)
{
	Ast (GetOwner());
	CCharacterDictator* pCharacter = GetOwner()->GetCharacter();
	Ast (pCharacter);
	CFighterDictator* pFighter = pCharacter->GetFighter();
	Ast (pFighter);
	CSkillRuleMgr* pSkillRuleMgr = GetOwner()->GetSkillRuleMgr();

	switch(pEvent->GetID())
	{
	case eNpcEvent_OnSkillSuccessEnd:
		{
			pSkillRuleMgr->CreateSkillAniTick();
		}
		break;

	//普攻结束，技能结束，技能失败都要检测是否需要释放新技能
	case eNpcEvent_OnBeginAttackEnemy:
	case eNpcEvent_OnAttackAnimationEnd:
	case eNpcEvent_OnSkillFailEnd:
	case eNpcEvent_OnSkillAnimationEnd:
		{
			GetOwner()->ReSetTarget();
			GetOwner()->SetIsDoingSkillRule(false);
			if (pSkillRuleMgr)
				pSkillRuleMgr->ClearCurSkill();
			pCharacter->GetFightDirVariant()->SetNumber((bool)true);
			
			if (!GetOwner()->AttackTargetHaveHighBarrier())
			{
				//普攻或者技能结束后进入下一次循环
				if (CanDoSkillRule())
					CNpcEventMsg::Create(GetOwner(),eNpcEvent_OnEnterAttackEnemyState);
				else
				{
					//这个地方是唯一检测是否需要追击的地方
					if (CheckAttackDis())
						CNpcEventMsg::Create(GetOwner(), eNpcEvent_OnInAttackScope);
					else
						CNpcEventMsg::Create(GetOwner(), eNpcEvent_OnEnemyOutOfAttackRange);
				}
			}
			else
			{
				CNpcEventMsg::Create(GetOwner(), eNpcEvent_OnEnemyOutOfAttackRange);
			}
		}
		break;

	//所有到这里的消息都会驱动进行普攻或者放技能
	default:										
		{
			//当有新技能满足情况下，不管是否有普攻，一律取消进入技能
			if (CanDoSkillRule())
			{
				CancelNormalAttack(true);
				bool bSuccess = pSkillRuleMgr->ExecuteSkill();		//此时技能列表里面一定有技能
				if (bSuccess)
				{
					pCharacter->GetFightDirVariant()->SetNumber((bool)false);
					GetOwner()->SetIsDoingSkillRule(true);
				}
				else
				{
					//当技能释放失败的时候直接退出战斗
					CNpcEventMsg::Create(GetOwner(), eNpcEvent_OnEnemyLost, eNECI_Eleven);
					break;
				}
			}
			else
			{
				//如果当前正在释放技能流程则跳过
				if (GetOwner()->GetIsDoingSkillRule())
					break;

				//在没有进行普攻流程，并且普攻已经冷却的情况下才会去进行普攻
				if (!GetOwner()->GetIsDoingAttack() && m_bInNormalAttackEnemyCoolDown)
				{
					CreateDelayDoAttackTick();
				}
			}
		}
		break;
	}
}
END_DEFINE_NPCSTATE_METHOD

DEFINE_NPCSTATE_METHOD(AttackEnemy, LeaveState)
{
	//cout<<"离开AttackEnemy状态: "<<pEvent->GetName()<<endl;
	Ast (GetOwner());
	CCharacterDictator* pCharacter = GetOwner()->GetCharacter();
	Ast (pCharacter);
	CFighterDictator* pFighter = pCharacter->GetFighter();
	Ast (pFighter);
	pFighter->CancelAttack();
	GetOwner()->SetIsDoingAttack(false);
	GetOwner()->SetIsDoingSkillRule(false);
	CSkillRuleMgr* pSkillMgr = GetOwner()->GetSkillRuleMgr();
	if (pSkillMgr)
		pSkillMgr->ClearCurSkill();
	pCharacter->GetFightDirVariant()->SetNumber((bool)false);
	pFighter->Notify(eCE_BeInterrupted);
	if (m_pAttackEnemyAnimationTick)
	{
		GetOwner()->GetCharacter()->UnRegistDistortedTick(m_pAttackEnemyAnimationTick);
		SafeDelete(m_pAttackEnemyAnimationTick);
	}
	if (m_pNormalAttackEnemyCoolDownTick)
	{
		GetOwner()->GetCharacter()->UnRegistDistortedTick(m_pNormalAttackEnemyCoolDownTick);
		SafeDelete(m_pNormalAttackEnemyCoolDownTick);
	}
	if (m_pDelayAttackEnemyTick)
	{
		GetOwner()->GetCharacter()->UnRegistDistortedTick(m_pDelayAttackEnemyTick);
		SafeDelete(m_pDelayAttackEnemyTick);
	}
}
END_DEFINE_NPCSTATE_METHOD

DEFINE_NPCSTATE_METHOD(ChaseBack, EnterState)
{
	//cout<<"NPC ID = "<<GetOwner()->GetCharacter()->GetEntityID()<<"--"<<pEvent->GetName()<<"==导致进入状态(ChaseBack, EnterState)"<<endl;
	Ast(GetOwner());
	CCharacterDictator* pOwnCharacterDictator = GetOwner()->GetCharacter();
	Ast(pOwnCharacterDictator);
	CFighterDictator* pOwnFighterDictator = pOwnCharacterDictator->GetFighter();
	Ast(pOwnFighterDictator);
	GetOwner()->SetInChaseBackState(true);
	m_uMoveID = 0;

	if (!NpcInfoMgr::BeServantType(GetOwner()->GetNpcType()))
	{
		CBattleStateManager* pBattleStateMgr = pOwnFighterDictator->GetBattleStateMgr();
		Ast (pBattleStateMgr);
		pBattleStateMgr->LeaveBattleState();
	}
	CEnmityMgr* pEnmityMgr = GetOwner()->GetEnmityMgr();
	Ast(pEnmityMgr);
	pEnmityMgr->ClearAllEnmity();

	GetOwner()->EndChaseOriginTrace();
	
	if(!NpcInfoMgr::BeServantType(GetOwner()->GetNpcType()))	//非召唤兽
	{
		GetOwner()->ClearDebufferAndSettingImmunity();
		pOwnFighterDictator->GetAllStateMgr()->EraseDecreaseState();	//清除所有的负面状态，包括控制状态
		ENpcMoveType eMoveType = GetOwner()->m_eMoveType;
		if ( (eMoveType == ENpcMove_SingleLine || eMoveType == ENpcMove_Single2Random) && GetOwner()->m_uCurrentPoint != GetOwner()->m_mapMovePath.size())
			CNpcEventMsg::Create(GetOwner(),eNpcEvent_OnExitChaseBack,eNECI_One);
		else
		{
			//一下这段代码都是直接调用characterDictator身上的函数，原因是此时Npc可能正受移位魔法的影响，所以必须强制调用结束唯一魔法，直接返回
			pOwnCharacterDictator->StopMoving();
			int32 iMaxStep = (int32) (GetOwner()->GetNpcLockEnemyDis());
			CFPos curpos;
			pOwnCharacterDictator->GetPixelPos(curpos);
			if (curpos != GetOwner()->m_ChaseBeginPos)
			{
				float fRunSpeed = pOwnFighterDictator->m_RunSpeed.Get(pOwnFighterDictator);
				EMoveToResult Result = pOwnCharacterDictator->MoveToInAStarPath(GetOwner()->m_ChaseBeginPos, fRunSpeed, eFPT_AStarIgnoreEnd, eBT_LowBarrier, 0.0f, iMaxStep);
				if (Result != eMTR_Success)
					pOwnCharacterDictator->MoveToInLinePath(GetOwner()->m_ChaseBeginPos, fRunSpeed, eFPT_Line, eBT_HighBarrier);
				m_uMoveID = pOwnCharacterDictator->GetLastMoveID();
				//cout<<"ChaseBack::m_uMoveID: "<<m_uMoveID<<endl;
			}
			else
			{
				CNpcEventMsg::Create(GetOwner(),eNpcEvent_OnExitChaseBack,eNECI_Two);
			}
		}
	}
	else			//召唤兽
	{
		CServantAI* pServerAI = GetOwner()->CastToServantAI();
		if (pServerAI->BeInCommandMoveState())
		{
			pServerAI->MoveToLastMoveCommandPos();
		}
		else
		{
			pServerAI->Follow();
		}
		CNpcEventMsg::Create(GetOwner(),eNpcEvent_OnExitChaseBack,eNECI_Three);
	}
}
END_DEFINE_NPCSTATE_METHOD

DEFINE_NPCSTATE_METHOD(ChaseBack, OnEvent)
{
	//cout<<"NPC ID = "<<GetOwner()->GetCharacter()->GetEntityID()<<"--"<<pEvent->GetName()<<"==ChaseBack, OnEvent"<<endl;
	Ast(GetOwner());
	CCharacterDictator* pOwnCharacterDictator = GetOwner()->GetCharacter();
	Ast (pOwnCharacterDictator);
	CFighterDictator* pOwnFighter = pOwnCharacterDictator->GetFighter();
	Ast (pOwnFighter);

	switch(pEvent->GetID())
	{
	case eNpcEvent_OnMoveEnded:
		{
			uint32 uCurMoveID= reinterpret_cast<size_t>(pEvent->GetTag());
			//cout<<"OnEvent::uCurMoveID: "<<uCurMoveID<<endl;
			if (uCurMoveID >= m_uMoveID)
			{
				CFPos curPos;
				pOwnCharacterDictator->GetPixelPos(curPos);
				float fDis = curPos.Dist(GetOwner()->m_ChaseBeginPos);
				if (fDis > eGridSpanForObj)
				{
					pOwnCharacterDictator->StopMoving();
					pOwnCharacterDictator->SetPixelPos(GetOwner()->m_ChaseBeginPos);
				}
				CNpcEventMsg::Create(GetOwner(),eNpcEvent_OnExitChaseBack,eNECI_Four);
			}
		}
		break;
	default:
		break;
	}
}
END_DEFINE_NPCSTATE_METHOD

DEFINE_NPCSTATE_METHOD(ChaseBack, LeaveState)
{
	//cout<<"NPC ID = "<<GetOwner()->GetCharacter()->GetEntityID()<<"--"<<pEvent->GetName()<<"==离开ChaseBack, LeaveState"<<endl;
	Ast(GetOwner());
	CCharacterDictator* pOwnCharacter = GetOwner()->GetCharacter();
	Ast (pOwnCharacter);
	CFighterDictator* pOwnFighter = pOwnCharacter->GetFighter();
	Ast (pOwnFighter);
	GetOwner()->SetInChaseBackState(false);
	if(!NpcInfoMgr::BeServantType(GetOwner()->GetNpcType()))
		GetOwner()->ClearImmunity();
	GetOwner()->RevertNpcBirthDir();

	//打开SysSize，自动触发AOI回调
	pOwnCharacter->SetEyeSight(GetOwner()->GetAIBaseData()->m_uEyeSize);
	pOwnFighter->Attach(GetOwner(), eCE_BeHurtFirstTime);
}
END_DEFINE_NPCSTATE_METHOD

DEFINE_NPCSTATE_METHOD(ExitSubMachine, EnterState)
{
	CCharacterDictator* pCharacter = GetOwner()->GetCharacter();
	Ast (pCharacter);
	if (pCharacter->CppIsLive())
		pCharacter->SetAndSyncActionState(eAS_Idle_BackWpn);
	GetOwner()->EndChaseOriginTrace();
	GetOwner()->SetExpOwnerID(0);
	CNpcEventMsg::Create(GetOwner(),eNpcEvent_OnExitSubMachine);
	GetOwner()->PopSubMachine(pEvent);
}
END_DEFINE_NPCSTATE_METHOD


/*====================个性===================*/
//个性状态
DEFINE_NPCSTATE_METHOD(InitPersonality, EnterState)
{
	CNpcNatureMgr* pNatureMgr = GetOwner()->GetNatureMgr();
	if (!pNatureMgr)
	{
		CNpcEventMsg::Create(GetOwner(),eNpcEvent_OnExitNature);
		pNatureMgr->EnableNature();
		pNatureMgr->SetNature(NULL);
		return pEvent;
	}

	Ast( GetOwner()->GetCharacter() );

	pNatureMgr->DisableNature();
	pNatureMgr->Dispatch(pEvent);
	
}
END_DEFINE_NPCSTATE_METHOD


//怯懦状态
DEFINE_NPCSTATE_METHOD(CowardState, EnterState)
{
	Ast(GetOwner());
	CNpcNatureMgr* pNatureMgr = GetOwner()->GetNatureMgr();
	Ast(pNatureMgr);
	CNpcNature* pNature = pNatureMgr->GetNature();
	if (NULL == pNature)
	{
		CNpcEventMsg::Create(GetOwner(),eNpcEvent_OnExitNature);
		return pEvent;
	}
	uint32 pRate = pNature->GetExtraArg();
	if ((uint32)Random::Rand(0,99) < pRate)
	{
		pNatureMgr->AfterSatisfied();
		CNpcEventMsg::Create(GetOwner(),eNpcEvent_OnExitNature);
	}
}
END_DEFINE_NPCSTATE_METHOD

DEFINE_NPCSTATE_METHOD(CowardState, LeaveState)
{
	Ast(GetOwner());
	CNpcNatureMgr* pNatureMgr = GetOwner()->GetNatureMgr();
	Ast(pNatureMgr);
	pNatureMgr->EnableNature();
	pNatureMgr->SetNature(NULL);
	GetOwner()->MoveToBirthPos();
}
END_DEFINE_NPCSTATE_METHOD

//嗜睡状态
DEFINE_NPCSTATE_METHOD(SleepyState, EnterState)
{
	m_pWakeUpTick = NULL;
	m_pSleepStateEndTick = NULL;
	Ast(GetOwner());
	CCharacterDictator* pOwnCharacterDictator = GetOwner()->GetCharacter();
	Ast(pOwnCharacterDictator);
	CNpcNatureMgr* pNatureMgr = GetOwner()->GetNatureMgr();
	Ast(pNatureMgr);
	CNpcNature* pNature = pNatureMgr->GetNature();
	if (NULL == pNature)
	{
		CNpcEventMsg::Create(GetOwner(),eNpcEvent_OnExitNature);
		return pEvent;
	}
	m_pMostTime = pNature->GetExtraArg();
	uint32 pRandTime = Random::Rand(0, m_pMostTime);

	struct WakeUpTick
		: public CDistortedTick
		, public CNpcAIMallocObject 
	{
	public:
		WakeUpTick(CNpcAI* pAI):m_pAI(pAI){};
		void OnTick()
		{
			Ast(m_pAI);
			CCharacterDictator* pCharacter = m_pAI->GetCharacter();
			Ast(pCharacter);
			pCharacter->SetEyeSight(m_pAI->GetAIBaseData()->m_uEyeSize);
			pCharacter->SetAndSyncActionState(eAS_Idle_Battle);
			CNpcNatureMgr* pNatureMgr = m_pAI->GetNatureMgr();
			Ast(pNatureMgr)
			pNatureMgr->AfterSatisfied();
			pCharacter->UnRegistDistortedTick(this);
		}
	private:
		CNpcAI* m_pAI;
	};

	struct SleepStateEndTick
		: public CDistortedTick
		, public CNpcAIMallocObject 
	{
	public:
		SleepStateEndTick(CNpcAI* pAI):m_pAI(pAI){};
		void OnTick()
		{
			CNpcEventMsg::Create(m_pAI, eNpcEvent_OnExitNature);
			m_pAI->GetCharacter()->UnRegistDistortedTick(this);
		}
	private:
		CNpcAI* m_pAI;
	};

	Ast (pOwnCharacterDictator);
	pOwnCharacterDictator->SetEyeSight(2);
	pOwnCharacterDictator->SetAndSyncActionState(eAS_Die);

	m_pWakeUpTick = new WakeUpTick(GetOwner());
	GetOwner()->GetCharacter()->RegistDistortedTick(m_pWakeUpTick, pRandTime*1000);

	m_pSleepStateEndTick = new SleepStateEndTick(GetOwner());
	GetOwner()->GetCharacter()->RegistDistortedTick(m_pSleepStateEndTick, m_pMostTime*1000);
}
END_DEFINE_NPCSTATE_METHOD

DEFINE_NPCSTATE_METHOD(SleepyState, OnEvent)
{
	CCharacterDictator* pCharacter = NULL;
	Ast(GetOwner());
	CCharacterDictator* pOwnCharacterDictator = GetOwner()->GetCharacter();
	Ast(pOwnCharacterDictator);
	CNpcNatureMgr* pNatureMgr = GetOwner()->GetNatureMgr();
	Ast(pNatureMgr);
	CNpcNature* pNature = pNatureMgr->GetNature();
	if (NULL == pNature)
	{
		CNpcEventMsg::Create(GetOwner(),eNpcEvent_OnExitNature);
		return pEvent;
	}

	switch(pEvent->GetID())
	{
	case eNpcEvent_OnCharacterInSight:
		{
			pCharacter = CCharacterDictator::GetCharacterByID((size_t)pEvent->GetTag());
			if (pCharacter->GetNpcAIHandler())
				return pEvent;
			pOwnCharacterDictator->SetAndSyncActionState(eAS_Idle_HoldWpn);
			pOwnCharacterDictator->SetEyeSight(GetOwner()->GetAIBaseData()->m_uEyeSize);
		}
		break;
	case eNpcEvent_OnBeAttacked:
		{
			pOwnCharacterDictator->SetAndSyncActionState(eAS_Idle_Battle);
			pOwnCharacterDictator->SetEyeSight(GetOwner()->GetAIBaseData()->m_uEyeSize);
			CNpcEventMsg::Create(GetOwner(),eNpcEvent_OnNpcNature2NormalAttack);
		}
		break;
	default:
		break;
	}
}
END_DEFINE_NPCSTATE_METHOD

DEFINE_NPCSTATE_METHOD(SleepyState, LeaveState)
{
	Ast(GetOwner());
	CCharacterDictator* pOwnCharacterDictator = GetOwner()->GetCharacter();
	Ast(pOwnCharacterDictator);
	pOwnCharacterDictator->SetEyeSight(GetOwner()->GetAIBaseData()->m_uEyeSize);
	CNpcNatureMgr* pNatureMgr = GetOwner()->GetNatureMgr();
	Ast(pNatureMgr);
	pNatureMgr->EnableNature();
	pNatureMgr->SetNature(NULL);

	if (m_pWakeUpTick != NULL)
	{
		GetOwner()->GetCharacter()->UnRegistDistortedTick(m_pWakeUpTick);
		delete m_pWakeUpTick;
		m_pWakeUpTick = NULL;
	}
	if (m_pSleepStateEndTick)
	{
		GetOwner()->GetCharacter()->UnRegistDistortedTick(m_pSleepStateEndTick);
		delete m_pSleepStateEndTick;
		m_pSleepStateEndTick = NULL;
	}
	GetOwner()->MoveToBirthPos();
}
END_DEFINE_NPCSTATE_METHOD

//吸引状态
DEFINE_NPCSTATE_METHOD(AttractState, EnterState)
{
	m_pAttractTick = NULL;
	Ast(GetOwner());
	CNpcNatureMgr* pNatureMgr = GetOwner()->GetNatureMgr();
	Ast(pNatureMgr);
	CNpcNature* pNature = pNatureMgr->GetNature();
	if (NULL == pNature)
	{
		CNpcEventMsg::Create(GetOwner(),eNpcEvent_OnExitNature);
		return pEvent;
	}
	CIntObjServer* pIntObj = CIntObjServer::GetIntObjServerByID((size_t)(pNature->GetTag()));
	if (NULL == pIntObj)
	{
		CNpcEventMsg::Create(GetOwner(),eNpcEvent_OnExitNature);
		return pEvent;
	}
	CPos targetPos;
	pIntObj->GetGridPos(targetPos);
	GetOwner()->MoveToGrid(targetPos, GetOwner()->m_bDefaultWalkState, 1);
}
END_DEFINE_NPCSTATE_METHOD

DEFINE_NPCSTATE_METHOD(AttractState, OnEvent)
{	
	class AttractTick
		: public CDistortedTick  
		, public CNpcAIMallocObject
	{
	public:
		AttractTick(CNpcAI* pAI): m_pAI(pAI)
		{}

		void OnTick()
		{
			m_pAI->GetNatureMgr()->AfterSatisfied();
			CNpcEventMsg::Create( m_pAI, eNpcEvent_OnExitNature);
			m_pAI->GetCharacter()->UnRegistDistortedTick(this);
		}

	private:
		CNpcAI* m_pAI;
	};
	Ast(GetOwner());
	CCharacterDictator* pOwnCharacterDictator = GetOwner()->GetCharacter();
	Ast(pOwnCharacterDictator);
	CNpcNatureMgr* pNatureMgr = GetOwner()->GetNatureMgr();
	Ast(pNatureMgr);
	CNpcNature* pNature = pNatureMgr->GetNature();
	if (NULL == pNature)
	{
		CNpcEventMsg::Create(GetOwner(),eNpcEvent_OnExitNature);
		return pEvent;
	}
	ICharacterDictatorCallbackHandler* pOwnICharacterDictatorCallbackHandler = pOwnCharacterDictator->GetCallbackHandler();
	Ast(pOwnICharacterDictatorCallbackHandler);
	switch(pEvent->GetID())
	{
	case eNpcEvent_OnMoveEnded:
		{
			uint32 utime = pNature->GetExtraArg();
			m_pAttractTick = new AttractTick(GetOwner());
			GetOwner()->GetCharacter()->RegistDistortedTick(m_pAttractTick, utime*1000);
			pOwnICharacterDictatorCallbackHandler->DoNatureAction(pOwnCharacterDictator->GetEntityID(),
				pNature->GetNatureAction().c_str());
			break;
		}
	case eNpcEvent_OnBeAttacked:
		{
			CNpcEventMsg::Create(GetOwner(),eNpcEvent_OnNpcNature2NormalAttack);
			break;
		}
	default:
		break;

	}
}
END_DEFINE_NPCSTATE_METHOD

DEFINE_NPCSTATE_METHOD(AttractState,LeaveState)
{
	CNpcNatureMgr* pNatureMgr = GetOwner()->GetNatureMgr();
	Ast(pNatureMgr);
	pNatureMgr->EnableNature();
	pNatureMgr->SetNature(NULL);
	if (m_pAttractTick != NULL)
	{
		GetOwner()->GetCharacter()->UnRegistDistortedTick(m_pAttractTick);
		delete m_pAttractTick;
		m_pAttractTick = NULL;
	}
	GetOwner()->MoveToBirthPos();
}
END_DEFINE_NPCSTATE_METHOD



//喜好
DEFINE_NPCSTATE_METHOD(LikeState, EnterState)
{
	Ast(GetOwner());
	Ast(GetOwner()->GetCharacter());
	CNpcNatureMgr* pNatureMgr = GetOwner()->GetNatureMgr();
	Ast(pNatureMgr);
	CNpcNature* pNature = pNatureMgr->GetNature();
	if (NULL == pNature)
	{
		CNpcEventMsg::Create(GetOwner(),eNpcEvent_OnExitNature);
		return pEvent;
	}

	m_DoLikeThingsTick = NULL;
	size_t food_id = (size_t) pNature->GetTag();
	CIntObjServer* pIntObj = CIntObjServer::GetIntObjServerByID(food_id);
	if (NULL == pIntObj)
	{
		CNpcEventMsg::Create(GetOwner(),eNpcEvent_OnExitNature);
		return pEvent;
	}
	CSyncVariantServer* pIntObjCSyncVariantServer = pIntObj->GetViewVariant();
	Ast(pIntObjCSyncVariantServer);
	CSyncVariantServer* pIntObjMemberCSyncVariantServerAttribute = pIntObjCSyncVariantServer->GetMember("Attribute");
	Ast(pIntObjMemberCSyncVariantServerAttribute);
	CSyncVariantServer*  pIntObjMemberCSyncVariantServerName = pIntObjMemberCSyncVariantServerAttribute->GetMember("CharName");
	Ast(pIntObjMemberCSyncVariantServerName);
	const TCHAR* obj_name = pIntObjMemberCSyncVariantServerName->GetString();
	if (strcmp(obj_name, pNature->GetArg().c_str()) != 0)
	{
		CNpcEventMsg::Create(GetOwner(),eNpcEvent_OnExitNature);
		return pEvent;
	}
	DoMove(NULL);

}
END_DEFINE_NPCSTATE_METHOD

DEFINE_NPCSTATE_METHOD(LikeState, DoMove)
{
	Ast(GetOwner());
	CNpcNatureMgr* pNatureMgr = GetOwner()->GetNatureMgr();
	Ast(pNatureMgr);
	CNpcNature* pNature = pNatureMgr->GetNature();
	Ast(pNature);
	size_t obj_id = (size_t)pNature->GetTag();
	CIntObjServer* pIntObj = CIntObjServer::GetIntObjServerByID(obj_id);
	if (NULL == pIntObj)
	{
		CNpcEventMsg::Create(GetOwner(),eNpcEvent_OnExitNature);
		return pEvent;
	}
	
	CPos targetPos;
	pIntObj->GetGridPos(targetPos);
	GetOwner()->MoveToGrid(targetPos, GetOwner()->m_bDefaultWalkState, 1);

}
END_DEFINE_NPCSTATE_METHOD

DEFINE_NPCSTATE_METHOD(LikeState, OnEvent)
{
	Ast(GetOwner());
	CCharacterDictator* pOwnCharacter = GetOwner()->GetCharacter();
	Ast(pOwnCharacter);
	CNpcNatureMgr* pNatureMgr = GetOwner()->GetNatureMgr();
	Ast(pNatureMgr);
	CNpcNature* pNature = pNatureMgr->GetNature();
	if (NULL == pNature)
	{
		CNpcEventMsg::Create(GetOwner(),eNpcEvent_OnExitNature);
		return pEvent;
	}

	CPos targetPos;
	CIntObjServer* pIntObj = NULL;
	size_t obj_id;
	switch(pEvent->GetID())
	{
	case eNpcEvent_OnBeAttacked:
		CNpcEventMsg::Create(GetOwner(),eNpcEvent_OnNpcNature2NormalAttack);
		return pEvent;

	case eNpcEvent_OnMoveEnded:
		obj_id = (size_t) pNature->GetTag();
		pIntObj = CIntObjServer::GetIntObjServerByID(obj_id);
		if (NULL == pIntObj || m_DoLikeThingsTick)
		{
			CNpcEventMsg::Create(GetOwner(),eNpcEvent_OnExitNature);
			return pEvent;
		}
		struct DoLikeThings
			: public CDistortedTick
			, public CNpcAIMallocObject 
		{
			public:
				DoLikeThings(CNpcAI* pAI, uint32 pObjId)
					:m_pAI(pAI), m_ObjId(pObjId)
					{
					};

				void OnTick()
				{
					Ast(m_pAI);
					CCharacterDictator* pCharacter = m_pAI->GetCharacter();
					Ast(pCharacter);
					ICharacterDictatorCallbackHandler* pCallBackHandle  = pCharacter->GetCallbackHandler();
					Ast(pCallBackHandle);
					pCallBackHandle->DestoryObj(m_ObjId);
					CNpcNatureMgr* pNpcNatureMgr = m_pAI->GetNatureMgr();
					Ast(pNpcNatureMgr);
					pNpcNatureMgr->AfterSatisfied();
					CNpcEventMsg::Create(m_pAI, eNpcEvent_OnExitNature);
					pCharacter->UnRegistDistortedTick(this);
				}

			private:
				CNpcAI* m_pAI;
				uint32 m_ObjId;
		};

		if (pIntObj)
		{
			uint32 utime = pNature->GetExtraArg();
			m_DoLikeThingsTick = new DoLikeThings(GetOwner(),obj_id);
			GetOwner()->GetCharacter()->RegistDistortedTick(m_DoLikeThingsTick, utime*1000);
			ICharacterDictatorCallbackHandler* pCharacterCBHandle  = pOwnCharacter->GetCallbackHandler();
			Ast(pCharacterCBHandle);
			pCharacterCBHandle->DoNatureAction(pOwnCharacter->GetEntityID(),
												pNature->GetNatureAction().c_str());
		}
		return pEvent;
	default:
		break;
	}
}
END_DEFINE_NPCSTATE_METHOD

DEFINE_NPCSTATE_METHOD(LikeState, LeaveState)
{
	Ast(GetOwner());
	Ast(GetOwner()->GetCharacter());
	CNpcNatureMgr* pNatureMgr = GetOwner()->GetNatureMgr();
	Ast(pNatureMgr);
	pNatureMgr->EnableNature();
	pNatureMgr->SetNature(NULL);

	if (m_DoLikeThingsTick)
	{
		GetOwner()->GetCharacter()->UnRegistDistortedTick(m_DoLikeThingsTick);
		delete m_DoLikeThingsTick;
		m_DoLikeThingsTick = NULL;
	}

	GetOwner()->MoveToBirthPos();
}
END_DEFINE_NPCSTATE_METHOD

//厌恶
DEFINE_NPCSTATE_METHOD(AbhorState, DoMove)
{

	Ast(GetOwner());
	CCharacterDictator* pOwnCharacter = GetOwner()->GetCharacter();
	Ast(pOwnCharacter);
	CNpcNatureMgr* pNatureMgr = GetOwner()->GetNatureMgr();
	Ast(pNatureMgr);
	CNpcNature* pNature = pNatureMgr->GetNature();
	if (NULL == pNature)
	{
		CNpcEventMsg::Create(GetOwner(),eNpcEvent_OnExitNature);
		return pEvent;
	}

	size_t obj_id = (size_t) pNature->GetTag();
	CIntObjServer* pIntObj = CIntObjServer::GetIntObjServerByID(obj_id);
	if (NULL == pIntObj)
	{
		CNpcEventMsg::Create(GetOwner(),eNpcEvent_OnExitNature);
		return pEvent;
	}

	CPos MyPos; 
	pOwnCharacter->GetGridPos(MyPos);
	CPos TargetPos;
	
	uint32 utarget = (uint32) pOwnCharacter->GetEyeSight(); 
	pIntObj->GetGridPos(TargetPos);
	CPos PosToTarget = GetOwner()->GetPosToTarget(TargetPos, int32(MyPos.Dist(TargetPos) + utarget));
	
	ICharacterDictatorCallbackHandler* pCharacterCBHandle  = pOwnCharacter->GetCallbackHandler();
	Ast(pCharacterCBHandle);

	pCharacterCBHandle->ChangeDir(pOwnCharacter->GetEntityID(), PosToTarget.x, PosToTarget.y);
	GetOwner()->MoveToGrid(PosToTarget,false,0);

	pCharacterCBHandle->SetDrivePig(obj_id, (uint32)pOwnCharacter->GetEntityID());

}
END_DEFINE_NPCSTATE_METHOD

DEFINE_NPCSTATE_METHOD(AbhorState, EnterState)
{
	m_pMoveBackTick = NULL;
	Ast(GetOwner());
	Ast(GetOwner()->GetCharacter());
	CNpcNatureMgr* pNatureMgr = GetOwner()->GetNatureMgr();
	Ast(pNatureMgr);
	CNpcNature* pNature = pNatureMgr->GetNature();
	if (NULL == pNature)
	{
		CNpcEventMsg::Create(GetOwner(),eNpcEvent_OnExitNature);
		return pEvent;
	}

	size_t food_id = (size_t) pNature->GetTag();
	CIntObjServer* pIntObj = CIntObjServer::GetIntObjServerByID(food_id);
	if (NULL == pIntObj)
	{
		CNpcEventMsg::Create(GetOwner(),eNpcEvent_OnExitNature);
		return pEvent;
	}
	CSyncVariantServer* pIntObjCSyncVariantServer = pIntObj->GetViewVariant();
	Ast(pIntObjCSyncVariantServer);
	CSyncVariantServer* pIntObjMemberCSyncVariantServerAttribute = pIntObjCSyncVariantServer->GetMember("Attribute");
	Ast(pIntObjMemberCSyncVariantServerAttribute);
	CSyncVariantServer*  pIntObjMemberCSyncVariantServerName = pIntObjMemberCSyncVariantServerAttribute->GetMember("CharName");
	Ast(pIntObjMemberCSyncVariantServerName);
	const TCHAR* obj_name = pIntObjMemberCSyncVariantServerName->GetString();
	if (strcmp(obj_name, pNature->GetArg().c_str()) == 0)
		DoMove(NULL);
	else
		CNpcEventMsg::Create(GetOwner(),eNpcEvent_OnExitNature);
}
END_DEFINE_NPCSTATE_METHOD

DEFINE_NPCSTATE_METHOD(AbhorState, OnEvent)
{

	class CMoveBackTick
		: public CDistortedTick
		, public CNpcAIMallocObject
	{
	public:
		CMoveBackTick(CNpcAI* pAI): m_pAI(pAI)
		{
		};
		void OnTick()
		{
			CNpcEventMsg::Create(m_pAI, eNpcEvent_OnExitNature);
			m_pAI->GetNatureMgr()->AfterSatisfied();
			m_pAI->MoveToBirthPos();
			m_pAI->GetCharacter()->UnRegistDistortedTick(this);
		}
	private:
		CNpcAI* m_pAI;
	};

	Ast(GetOwner());
	Ast(GetOwner()->GetCharacter());
	CNpcNatureMgr* pNatureMgr = GetOwner()->GetNatureMgr();
	Ast(pNatureMgr);
	CNpcNature* pNature = pNatureMgr->GetNature();
	if (NULL == pNature)
	{
		CNpcEventMsg::Create(GetOwner(),eNpcEvent_OnExitNature);
		return pEvent;
	}

	switch(pEvent->GetID())
	{
	case eNpcEvent_OnBeAttacked:
		CNpcEventMsg::Create(GetOwner(),eNpcEvent_OnNpcNature2NormalAttack);
		return pEvent;
	case eNpcEvent_OnMoveEnded:
		{
			if (m_pMoveBackTick)
				break;
			m_pMoveBackTick = new CMoveBackTick(GetOwner());
			uint32 utime = pNature->GetExtraArg();
			GetOwner()->GetCharacter()->RegistDistortedTick(m_pMoveBackTick, utime * 1000);
			pNatureMgr->EnableNature();
		}
		break;
	default:
		break;
	}
}
END_DEFINE_NPCSTATE_METHOD

DEFINE_NPCSTATE_METHOD(AbhorState, LeaveState)
{
	Ast(GetOwner());
	Ast(GetOwner()->GetCharacter());
	CNpcNatureMgr* pNatureMgr = GetOwner()->GetNatureMgr();
	Ast(pNatureMgr);
	pNatureMgr->EnableNature();
	//pNatureMgr->SetNature(NULL);
	if (m_pMoveBackTick)
	{
		GetOwner()->GetCharacter()->UnRegistDistortedTick(m_pMoveBackTick);
		delete m_pMoveBackTick;
		m_pMoveBackTick = NULL;
	}
}
END_DEFINE_NPCSTATE_METHOD

//相聚状态
DEFINE_NPCSTATE_METHOD(GreetState, EnterState)
{
	Ast(GetOwner());
	CCharacterDictator* pOwnCharacter = GetOwner()->GetCharacter();
	Ast(pOwnCharacter);
	CNpcNatureMgr* pNatureMgr = GetOwner()->GetNatureMgr();
	Ast(pNatureMgr);
	CNpcNature* pNature = pNatureMgr->GetNature();
	if (NULL == pNature)
	{
		CNpcEventMsg::Create(GetOwner(),eNpcEvent_OnExitNature);
		return NULL;
	}
	
	ICharacterDictatorCallbackHandler* pCharacterCBHandle  = pOwnCharacter->GetCallbackHandler();
	Ast(pCharacterCBHandle);

	CCharacterDictator* pTarget = CCharacterDictator::GetCharacterByID((size_t)pNature->GetTag());
	if (pTarget->CppIsLive())
	{
		pCharacterCBHandle->SendNpcRpcMessage(
											pOwnCharacter->GetEntityID(), 
											(size_t)pNature->GetTag(),
											pNature->GetExtraArg());
	}

	CNpcEventMsg::Create(GetOwner(),eNpcEvent_OnExitNature);

}
END_DEFINE_NPCSTATE_METHOD

DEFINE_NPCSTATE_METHOD(GreetState, LeaveState)
{
	Ast(GetOwner());
	Ast(GetOwner()->GetCharacter());
	CNpcNatureMgr* pNatureMgr = GetOwner()->GetNatureMgr();
	Ast(pNatureMgr);
	pNatureMgr->EnableNature();
	pNatureMgr->SetNature(NULL);
}
END_DEFINE_NPCSTATE_METHOD


//警戒
DEFINE_NPCSTATE_METHOD(AlertState, EnterState)
{
	struct ExistWarningStateTick
		: public CDistortedTick
		, public CNpcAIMallocObject 
	{
		ExistWarningStateTick(CNpcAI *pAI) : m_pAI(pAI)
		{
		}
		void OnTick()
		{
			m_pAI->GetCharacter()->UnRegistDistortedTick(this);
			CNpcEventMsg::Create(m_pAI, eNpcEvent_OnExitAlertState);
		}
	private:
		CNpcAI* m_pAI;
	};

	m_pLeaveWarningStateTick = new ExistWarningStateTick(GetOwner());

	GetOwner()->GetCharacter()->RegistDistortedTick(m_pLeaveWarningStateTick, uint32(60*1000));
}
END_DEFINE_NPCSTATE_METHOD


DEFINE_NPCSTATE_METHOD(AlertState, LeaveState)
{
	GetOwner()->GetCharacter()->UnRegistDistortedTick(m_pLeaveWarningStateTick);
}
END_DEFINE_NPCSTATE_METHOD


//表情动作相应
DEFINE_NPCSTATE_METHOD(FacialState, EnterState)
{
	Ast(GetOwner());
	CCharacterDictator* pOwnCharacter = GetOwner()->GetCharacter();
	Ast(pOwnCharacter);
	ICharacterDictatorCallbackHandler* pOwnCBHandle = pOwnCharacter->GetCallbackHandler();
	Ast(pOwnCBHandle);
	CCharacterDictator* pTargetCharacter = pOwnCharacter->GetTarget();
	Ast(pTargetCharacter);
	//const TCHAR* szFacialName = GetOwner()->GetFacialActionHandler()->GetFacialName();
	const TCHAR* szFacialName = "XXXXX";
	pOwnCBHandle->DoFacialReaction(
							pOwnCharacter->GetEntityID(), 
							szFacialName, 
							pTargetCharacter->GetEntityID());
	CNpcEventMsg::Create(GetOwner(),eNpcEvent_OnFacialCold);
	pOwnCharacter->OnSendExitSubMachineEvent();
}
END_DEFINE_NPCSTATE_METHOD

DEFINE_NPCSTATE_METHOD(FacialColdState, EnterState)
{
	class CFacicalCold
		: public CDistortedTick
		, public CNpcAIMallocObject
	{
	public:
		CFacicalCold(CNpcAI* pAI):m_pAI(pAI)
		{
		};

		void OnTick()
		{
			Ast(m_pAI);
			CCharacterDictator* pCharacter = m_pAI->GetCharacter();
			Ast(pCharacter);
			pCharacter->OnSendExitSubMachineEvent();
			pCharacter->UnRegistDistortedTick(this);
		};

	private:
		CNpcAI* m_pAI;
	};

	//m_pFacialColdTick = new CFacicalCold(this->GetOwner());
	//GetOwner()->GetCharacter()->RegistDistortedTick(m_pFacialColdTick,(uint32)(10*1000));//暂时默认十秒对表情动作的冷却时间

}
END_DEFINE_NPCSTATE_METHOD

DEFINE_NPCSTATE_METHOD(FacialColdState, OnEvent)
{
	Ast(GetOwner());
	CCharacterDictator* pOwnCharacter = GetOwner()->GetCharacter();
	Ast(pOwnCharacter);

	switch (pEvent->GetID())
	{
		case eNpcEvent_OnBeAttacked:
			pOwnCharacter->OnSendExitSubMachineEvent();
			CNpcEventMsg::Create(GetOwner(),eNpcEvent_OnBeAttacked);
			break;
		default:
			break;
	}
}
END_DEFINE_NPCSTATE_METHOD

DEFINE_NPCSTATE_METHOD(FacialColdState, LeaveState)
{
	/*if (m_pFacialColdTick)
	{
		GetOwner()->GetCharacter()->UnRegistDistortedTick(m_pFacialColdTick);
		delete m_pFacialColdTick;
		m_pFacialColdTick = NULL;
	}*/


}
END_DEFINE_NPCSTATE_METHOD

DEFINE_NPCSTATE_METHOD(Task, EnterState)
{
	//cout<<"Task,EnterState ==="<<pEvent->GetName()<<endl;
	m_pRemainTimeTick = NULL;

	class NpcRemainTick
		: public CDistortedTick
		, public CNpcAIMallocObject
	{
	public:
		NpcRemainTick(CNpcAI* pAI)
		{
			m_pAI = pAI;
		}
		void OnTick()
		{
			CNpcEventMsg::Create(m_pAI, eNpcEvent_OnTaskDialogAllEnd);
			m_pAI->GetCharacter()->UnRegistDistortedTick(this);
			return;
		};
	private:
		CNpcAI*	m_pAI;
	};
	
	if(!m_pRemainTimeTick)
		m_pRemainTimeTick = new NpcRemainTick(this->GetOwner());

	if(eNpcEvent_OnTaskDialogBegin == pEvent->GetID())
	{
		GetOwner()->GetCharacter()->RegistDistortedTick(m_pRemainTimeTick,60*1000);
		GetOwner()->StopMoving();
		uint32 uEntityID = reinterpret_cast<size_t>(pEvent->GetTag());
		CCharacterDictator* pTarget = CCharacterDictator::GetCharacterByID(uEntityID);
		if (pTarget)
		{
			GetOwner()->FaceToTarget(pTarget);
		}
	}

	GetOwner()->CheckAndDoAction();
}
END_DEFINE_NPCSTATE_METHOD

DEFINE_NPCSTATE_METHOD(Task, OnEvent)
{
	switch(pEvent->GetID())
	{
		case eNpcEvent_OnTaskDialogBegin:
			{
				if(m_pRemainTimeTick)
				{
					GetOwner()->GetCharacter()->UnRegistDistortedTick(m_pRemainTimeTick);
					GetOwner()->GetCharacter()->RegistDistortedTick(m_pRemainTimeTick,60*1000);
				}
			}
			break;
		case eNpcEvent_OnBeginRandomMove:
			{
				GetOwner()->CheckAndDoAction();
			}
			break;
		default:
			break;
	}
}
END_DEFINE_NPCSTATE_METHOD

DEFINE_NPCSTATE_METHOD(Task, LeaveState)
{	
	GetOwner()->DestoryActionTick();	

	if (NULL != m_pRemainTimeTick)
	{
		GetOwner()->GetCharacter()->UnRegistDistortedTick(m_pRemainTimeTick);
		delete m_pRemainTimeTick;
		m_pRemainTimeTick = NULL;
	}
}
END_DEFINE_NPCSTATE_METHOD

DEFINE_NPCSTATE_METHOD(TowerWander, EnterState)
{
	Ast(GetOwner());
	CCharacterDictator* pCharacter = GetOwner()->GetCharacter();
	Ast(pCharacter);
	pCharacter->CppSetCtrlState(eFCS_ForbitMove, true);
	pCharacter->CppSetCtrlState(eFCS_ForbitTurnAround, true);
	if(eNpcEvent_OnStop == pEvent->GetID() || eNpcEvent_OnDead == pEvent->GetID())
		return pEvent;
	GetOwner()->OnEnterWanderState();
}
END_DEFINE_NPCSTATE_METHOD

DEFINE_NPCSTATE_METHOD(TowerWander, OnEvent)
{
	//cout<<"TowerWander OnEvent =="<<GetOwner()->GetCharacter()->GetEntityID()<<pEvent->GetName()<<endl;
	switch(pEvent->GetID())
	{
	case eNpcEvent_OnCharacterInSight:
		{
			CCharacterDictator* pTargetCharacter = CCharacterDictator::GetCharacterByID((size_t)pEvent->GetTag());
			if (GetOwner()->CanFight(pTargetCharacter))
			{
				GetOwner()->BeginAttack((size_t)pEvent->GetTag());
				return pEvent;
			} 
		}
		break;
	default:
	    break;
	}
}
END_DEFINE_NPCSTATE_METHOD

DEFINE_NPCSTATE_METHOD(TowerWander, LeaveState)
{
	//cout<<"TowerWander LeaveState =="<<GetOwner()->GetCharacter()->GetEntityID()<<pEvent->GetName()<<endl;
	GetOwner()->OnLeaveWanderState();
}
END_DEFINE_NPCSTATE_METHOD

DEFINE_NPCSTATE_METHOD(TowerLockEnemy, EnterState)
{
	Ast (GetOwner());
	CCharacterDictator* pCharacter = GetOwner()->GetCharacter();
	Ast (pCharacter);
	CFighterDictator* pTargetFighter = GetOwner()->GetFirstEnenmyFighter();
	if (pTargetFighter)
	{
		pCharacter->SetEyeSight(0);	
		CNpcEventMsg::Create(GetOwner(), eNpcEvent_OnBeginAttackEnemy, eNECI_Six);
	}
	else
	{
		CNpcEventMsg::Create(GetOwner(),eNpcEvent_OnEnemyLost, eNECI_Twelve);
	}
}
END_DEFINE_NPCSTATE_METHOD

DEFINE_NPCSTATE_METHOD(TowerLeaveBattle, EnterState)
{
	Ast (GetOwner());
	CCharacterDictator* pCharacter = GetOwner()->GetCharacter();
	Ast (pCharacter);
	CFighterDictator* pFighter = pCharacter->GetFighter();
	Ast (pFighter);

	SQR_TRY
	{	
		CBattleStateManager* pBattleStateMgr = pFighter->GetBattleStateMgr();
		Ast (pBattleStateMgr);
		pBattleStateMgr->LeaveBattleState();
		CEnmityMgr* pEnmityMgr = GetOwner()->GetEnmityMgr();
		Ast(pEnmityMgr);
		pEnmityMgr->ClearAllEnmity();
	}
	SQR_CATCH(exp)
	{
		LogExp(exp);
	}
	SQR_TRY_END;

	CNpcEventMsg::Create(GetOwner(), eNpcEvent_OnExitTowerAttackSubMachine);
}
END_DEFINE_NPCSTATE_METHOD

DEFINE_NPCSTATE_METHOD(TowerLeaveBattle, LeaveState)
{
	Ast (GetOwner());
	CCharacterDictator* pCharacter = GetOwner()->GetCharacter();
	Ast (pCharacter);
	CFighterDictator* pFighter = pCharacter->GetFighter();
	Ast (pFighter);
	//打开SysSize，自动触发AOI回调
	pCharacter->SetEyeSight(GetOwner()->GetAIBaseData()->m_uEyeSize);
	pFighter->Attach(GetOwner(), eCE_BeHurtFirstTime);
}
END_DEFINE_NPCSTATE_METHOD


DEFINE_NPCSTATE_METHOD(ActiveBossInit, EnterState)
{
	Ast(GetOwner());
	CCharacterDictator* pCharacter = GetOwner()->GetCharacter();
	Ast(pCharacter);
	Ast(pCharacter->GetCallbackHandler());
	pCharacter->GetCallbackHandler()->MasterRevertNpcGroup(pCharacter->GetEntityID());
	
	GetOwner()->CheckAndDoAction();
}
END_DEFINE_NPCSTATE_METHOD

DEFINE_NPCSTATE_METHOD(ActiveBossInit, OnEvent)
{
	ENpcAIType AIType;
	switch(pEvent->GetID())
	{
	case eNpcEvent_OnMoveEnded:
		GetOwner()->StopMoving();
		break;
	case eNpcEvent_OnCharacterInSight:
		{
			AIType = GetOwner()->GetRealAIType();
			if (NpcInfoMgr::BeActiveNpc(AIType))
			{
				GetOwner()->BeginAttack((size_t)pEvent->GetTag());
			}
		}
		break;
	case eNpcEvent_OnBeginRandomMove:
		{
			GetOwner()->CheckAndDoAction();
		}
		break;
	default:
		break;
	}
}
END_DEFINE_NPCSTATE_METHOD

DEFINE_NPCSTATE_METHOD(ActiveBossInit, LeaveState)
{
	GetOwner()->DestoryActionTick();
}
END_DEFINE_NPCSTATE_METHOD

DEFINE_NPCSTATE_METHOD(PassivityBossInit, EnterState)
{
	Ast(GetOwner());
	CCharacterDictator* pCharacter = GetOwner()->GetCharacter();
	Ast(pCharacter);
	Ast(pCharacter->GetCallbackHandler());
	pCharacter->GetCallbackHandler()->MasterRevertNpcGroup(pCharacter->GetEntityID());

	GetOwner()->CheckAndDoAction();
}
END_DEFINE_NPCSTATE_METHOD

DEFINE_NPCSTATE_METHOD(PassivityBossInit, OnEvent)
{
	switch(pEvent->GetID())
	{
		case eNpcEvent_OnMoveEnded: 
			GetOwner()->StopMoving();
			break;
		case eNpcEvent_OnBeginRandomMove:
			{
				GetOwner()->CheckAndDoAction();
			}
			break;
		default:
			break;
	}
}
END_DEFINE_NPCSTATE_METHOD

DEFINE_NPCSTATE_METHOD(PassivityBossInit, LeaveState)
{
	GetOwner()->DestoryActionTick();
}
END_DEFINE_NPCSTATE_METHOD

DEFINE_NPCSTATE_METHOD(BossPatrol, EnterState)
{
	Ast(GetOwner());
	CCharacterDictator* pCharacter = GetOwner()->GetCharacter();
	Ast(pCharacter);
	Ast(pCharacter->GetCallbackHandler());
	pCharacter->GetCallbackHandler()->MasterRevertNpcGroup(pCharacter->GetEntityID());
	GetOwner()->MoveToPixelInLinePath(GetOwner()->m_DestPos, GetOwner()->m_bDefaultWalkState);
	GetOwner()->CheckNpcMoveState(GetOwner()->m_DestPos, "DEFINE_NPCSTATE_METHOD(BossPatrol, EnterState)");
}
END_DEFINE_NPCSTATE_METHOD

DEFINE_NPCSTATE_METHOD(BossPatrol, OnEvent)
{
	Ast(GetOwner());
	Ast(GetOwner()->GetCharacter());
	switch(pEvent->GetID())
	{
	case eNpcEvent_OnMoveEnded:
			GetOwner()->MoveToDestEnded();
			break;
	case eNpcEvent_OnMoveToDest:
	case eNpcEvent_OnCanMove:
			GetOwner()->MoveToPixelInLinePath(GetOwner()->m_DestPos, GetOwner()->m_bDefaultWalkState);
			GetOwner()->CheckNpcMoveState(GetOwner()->m_DestPos, "DEFINE_NPCSTATE_METHOD(BossPatrol, OnEvent)");
			break;
	case eNpcEvent_OnCharacterInSight:
		{
			ENpcAIType AIType = GetOwner()->GetAITypeID();
			if (NpcInfoMgr::BeActiveNpc(AIType))
			{
				GetOwner()->BeginAttack((size_t)pEvent->GetTag());
				return pEvent;
			}
		}
	default:
		break;
	}
}
END_DEFINE_NPCSTATE_METHOD

DEFINE_NPCSTATE_METHOD(BossPatrol, LeaveState)
{
	if (GetOwner()->m_pPatrolTick)
	{
		GetOwner()->GetCharacter()->UnRegistDistortedTick(GetOwner()->m_pPatrolTick);
		SafeDelete(GetOwner()->m_pPatrolTick);
	}
	if (GetOwner()->m_pSpecialActionTick)
	{
		GetOwner()->GetCharacter()->UnRegistDistortedTick(GetOwner()->m_pSpecialActionTick);
		SafeDelete(GetOwner()->m_pSpecialActionTick);
	}
}
END_DEFINE_NPCSTATE_METHOD

/************************************************************************/
/*	剧场Npc本身不会移动，所有的移动行为都是由剧场控制，比比如移动*/
/************************************************************************/
DEFINE_NPCSTATE_METHOD(Theater, EnterState)
{
}
END_DEFINE_NPCSTATE_METHOD

DEFINE_NPCSTATE_METHOD(Theater, OnEvent)
{
	Ast(GetOwner());
	Ast(GetOwner()->GetCharacter());
	switch(pEvent->GetID())
	{
	case eNpcEvent_OnMoveEnded:
		{
			//通知剧场移动结束
			CCharacterDictator* pCharacterDictator = GetOwner()->GetCharacter();
			Ast(pCharacterDictator);
			ICharacterDictatorCallbackHandler* pICharacterDictatorCallbackHandler = pCharacterDictator->GetCallbackHandler();
			Ast(pICharacterDictatorCallbackHandler);
			pICharacterDictatorCallbackHandler->MoveEndMessage(pCharacterDictator);
		}
		break;
	default:
		break;
	}
}
END_DEFINE_NPCSTATE_METHOD

//进入休眠状态后，将Npc的除休眠层意外的其他AOI层都设置成不可见，减少AOI消耗
DEFINE_NPCSTATE_METHOD(NpcSleep, EnterState)
{
	Ast (GetOwner());
	CCharacterDictator* pCharacter = GetOwner()->GetCharacter();
	Ast (pCharacter);
	pCharacter->SetEyeSight(0);
	pCharacter->SetStealth(0);
	pCharacter->SetKeenness(0);

	//让处于各种非状态的Npc都站立不动
	GetOwner()->StopMoving();
	GetOwner()->m_bInSleepState = true;
}
END_DEFINE_NPCSTATE_METHOD

//退出休眠状态后将，将Npc的所有AOI设置都恢复
DEFINE_NPCSTATE_METHOD(NpcSleep, LeaveState)
{
	Ast (GetOwner());
	CCharacterDictator* pCharacter = GetOwner()->GetCharacter();
	Ast (pCharacter);
	pCharacter->SetEyeSight(GetOwner()->GetAIBaseData()->m_uEyeSize);
	pCharacter->SetStealth(0);
	pCharacter->SetKeenness(0);
	GetOwner()->m_bInSleepState = false;
}
END_DEFINE_NPCSTATE_METHOD

void CNpcAI::CNpcStateEscapeState::OnBeginEscape()
{
	Ast (GetOwner());
	CCharacterDictator* pOwnCharacter = GetOwner()->GetCharacter();
	Ast (pOwnCharacter);
	CVector2f vecDir(0,0);		//最终逃跑方向的向量
	CFPos ownpos;
	pOwnCharacter->GetPixelPos(ownpos);
	MapEntityIDInNpcSight& mapEntity = GetOwner()->GetEntityInSightMap();
	if (mapEntity.size() == 0)
	{
		if (m_pEscapeTick)
		{
			pOwnCharacter->UnRegistDistortedTick(m_pEscapeTick);
			SafeDelete(m_pEscapeTick);
		}
		return;
	}
	MapEntityIDInNpcSight::const_iterator iter = mapEntity.begin();
	for (; iter != mapEntity.end(); iter++)
	{
		CCharacterDictator* pTarget = CCharacterDictator::GetCharacterByID((*iter).first);
		if (pTarget)
		{
			CVector2f localDir;
			CFPos targetpos;
			pTarget->GetPixelPos(targetpos);
			localDir.x = ownpos.x - targetpos.x;
			localDir.y = ownpos.y - targetpos.y;
			vecDir = vecDir + localDir;
		}
	}
	vecDir.Normalize();//矢量单位化
	CFPos DstPos;
	uint32 uEscapeDis = GetOwner()->GetEscapeDistace();
	DstPos.x = ownpos.x + uEscapeDis*eGridSpanForObj * vecDir.x;
	DstPos.y = ownpos.y + uEscapeDis*eGridSpanForObj * vecDir.y;
	pOwnCharacter->NpcMoveToPixel(DstPos, false);
}

DEFINE_NPCSTATE_METHOD(EscapeState, EnterState)
{
	Ast (GetOwner());
	m_pEscapeTick = NULL;
	GetOwner()->SetMoveType(ENpcMove_Random);
	
	GetOwner()->CheckAndDoAction();
}
END_DEFINE_NPCSTATE_METHOD

DEFINE_NPCSTATE_METHOD(EscapeState, OnEvent)
{
	class CEscapeTick 
		: public CDistortedTick
		, public CNpcAIMallocObject
	{
	public:
		CEscapeTick(CNpcStateEscapeState* pState):m_pState(pState){}
		~CEscapeTick(){}
		void OnTick()
		{
			m_pState->OnBeginEscape();
		}
	private:
		CNpcStateEscapeState* m_pState;
	};

	switch(pEvent->GetID())
	{
	case eNpcEvent_OnCharacterInSight:
		{
			if (!m_pEscapeTick)
			{
				m_pEscapeTick = new CEscapeTick(this);
				GetOwner()->GetCharacter()->RegistDistortedTick(m_pEscapeTick, 1000*GetOwner()->GetEscapeTime());
				OnBeginEscape();
			}
		}
		break;
	default:
		break;
	}
}
END_DEFINE_NPCSTATE_METHOD

DEFINE_NPCSTATE_METHOD(EscapeState, LeaveState)
{
	if (m_pEscapeTick)
	{
		GetOwner()->GetCharacter()->UnRegistDistortedTick(m_pEscapeTick);
		SafeDelete(m_pEscapeTick);
	}
	GetOwner()->DestoryActionTick();
}
END_DEFINE_NPCSTATE_METHOD

class CLittlePetDoActTick
	: public CDistortedTick
	, public CNpcAIMallocObject
{
public:
	CLittlePetDoActTick(CCharacterDictator* pLittlePet);
	void OnTick();
private:
	CCharacterDictator* m_pLittlePet;
};

CLittlePetDoActTick::CLittlePetDoActTick(CCharacterDictator* pLittlePet)
:m_pLittlePet(pLittlePet)
{
}

void CLittlePetDoActTick::OnTick()
{
	Ast(m_pLittlePet);
	CLittlePetAI* pAI = m_pLittlePet->GetNpcAIHandler()->CastToLittlePetAI();
	Ast (pAI);
	uint8 uActionState = Random::Rand(1,2);
	switch(uActionState)
	{
	case 1:
		{
			if (!m_pLittlePet->IsMoving())
				m_pLittlePet->SetAndSyncActionState(eAS_Idle_Special);
		}
		break;
	case 2:
		{
			CFPos pos,masterpos;
			CCharacterDictator* pMaster = m_pLittlePet->GetMaster();
			if (pMaster)
			{
				pMaster->GetPixelPos(masterpos);
				int32 MoveRange = pAI->m_uMoveRange;
				pos.x = masterpos.x + Random::Rand(-MoveRange*eGridSpanForObj, MoveRange*eGridSpanForObj);		
				pos.y =	masterpos.y + Random::Rand(-MoveRange*eGridSpanForObj, MoveRange*eGridSpanForObj);
				if (pos != masterpos)
					pAI->MoveToPixelInLinePath(pos, pAI->m_bDefaultWalkState);
			}
		}
		break;
	default:
		break;
	}

}

DEFINE_NPCSTATE_METHOD(LittlePet, EnterState)
{
	Ast (GetOwner());
	CCharacterDictator* pCharacter = GetOwner()->GetCharacter();
	Ast (pCharacter);
	m_pDoActionTick = new CLittlePetDoActTick(pCharacter);
	pCharacter->RegistDistortedTick(m_pDoActionTick, 5000);
}
END_DEFINE_NPCSTATE_METHOD

DEFINE_NPCSTATE_METHOD(LittlePet, LeaveState)
{
	if (m_pDoActionTick)
	{
		GetOwner()->GetCharacter()->UnRegistDistortedTick(m_pDoActionTick);
		SafeDelete(m_pDoActionTick);
	}
}
END_DEFINE_NPCSTATE_METHOD

