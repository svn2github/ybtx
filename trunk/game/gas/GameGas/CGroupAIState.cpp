#include "stdafx.h"
#include "CDummyAI.h"
#include "CMemberAI.h"
#include "CDistortedTick.h"
#include "BaseHelper.h"
#include "CNpcGroupMgr.h"
#include "CCharacterDictator.h"
#include "NpcInfoMgr.h"

CDummyAI* CNpcAI::CNpcStateDummyBase::GetOwner() const
{
	return class_cast<CDummyAI*>(CNpcStateBase::GetOwner());
}

CMemberAI* CNpcAI::CNpcStateMemberBase::GetOwner() const
{
	return class_cast<CMemberAI*>(CNpcStateBase::GetOwner());
}

class CRelayTimeTick
	: public CDistortedTick
	, public CNpcAIMallocObject
{
public:
	CRelayTimeTick(CDummyAI* pAI) : m_pAI(pAI){}
	void OnTick()
	{
		m_pAI->GetCharacter()->UnRegistDistortedTick(this);
		m_pAI->NotifyEventToMember(eNpcEvent_OnMemberMoveTo, eNECI_One);
		CNpcEventMsg::Create(m_pAI, eNpcEvent_OnMoveToDest);

	}
private:
	CDummyAI* m_pAI;
};

DEFINE_NPCSTATE_METHOD(PatrolForGroupLeader,EnterState)
{
	m_pRelayTimeTick = NULL;
	m_bIsMemberFighting = false;
	Ast(GetOwner());
	GetOwner()->CreatePatrolPath();
	//m_pRelayTimeTick = new CRelayTimeTick(GetOwner());
	GetOwner()->NotifyEventToMember(eNpcEvent_OnMemberMoveTo, eNECI_Two);
	CNpcEventMsg::Create(GetOwner(), eNpcEvent_OnMoveToDest);
}
END_DEFINE_NPCSTATE_METHOD

DEFINE_NPCSTATE_METHOD(PatrolForGroupLeader,OnEvent)
{
	Ast(GetOwner());

	CCharacterDictator* pOwnCharacter = GetOwner()->GetCharacter();
	switch(pEvent->GetID())
	{
	case eNpcEvent_OnMoveEnded:
		{
			if(!m_bIsMemberFighting)
				GetOwner()->MoveToDestEnded();
		}
		break;
	case eNpcEvent_OnMoveToDest:
		{
			if(GetOwner()->GetMovePathPointCount() == 0)
				return pEvent;
			if(m_bIsMemberFighting)
				return pEvent; 
			if(GetOwner()->m_bIsMoveToFirstPoint && GetOwner()->m_uCurrentPoint != 1)
				GetOwner()->m_bIsMoveToFirstPoint = false; //cout<<"出生移动到第一点结束"<<endl;
			GetOwner()->MoveToPixelInLinePath(GetOwner()->m_DestPos, GetOwner()->m_bDefaultWalkState);
			GetOwner()->SetMemberSynState(true);
		}
		break;
	case eNpcEvent_OnGroupMemberBeAttacked:
		{
			if(!m_bIsMemberFighting)
			{
				GetOwner()->StopMoving();
				GetOwner()->SetMemberSynState(false);
				m_bIsMemberFighting = true;
			}	
		}
		break;
	case eNpcEvent_OnMemberReady:
		{
			if(m_bIsMemberFighting)
			{
				Ast(pOwnCharacter->GetGroupMgr());
				const MemberIDMapType& MemberMap = pOwnCharacter->GetGroupMgr()->GetMemberIDMapType();
				MemberIDMapType::const_iterator it = MemberMap.begin();
				for(;it!=MemberMap.end();it++)
				{
					CCharacterDictator* pMember =  (*it).second;
					CMemberAI *pMemAI = pMember->GetNpcAIHandler()->CastToMemberAI();
					if(pMemAI && !pMemAI->m_bMemberReady)
						return pEvent;	
				}
				CEnmityMgr* pEnmityMgr = GetOwner()->GetEnmityMgr();
				pEnmityMgr->ClearAllEnmity();
				CNpcEventMsg::Create(GetOwner(), eNpcEvent_OnMemberAllReady);
			}
			else
			{
				size_t uMemberID = reinterpret_cast<size_t>(pEvent->GetTag());
				if(pOwnCharacter->AdjustIsGroupMember(uMemberID))
				{
					CCharacterDictator* pMemberCharacter = CCharacterDictator::GetCharacterByID(uMemberID);
					if(!pMemberCharacter)
						return pEvent;
					CMemberAI *pMemAI = pMemberCharacter->GetNpcAIHandler()->CastToMemberAI();
					if(!pMemAI)
						return pEvent;
					if(GetOwner()->GetMovePathPointCount() == 0)
						return pEvent;
					if(!pMemAI->m_bIsBeSynState)
					{
						pMemAI->m_bIsBeSynState = true;
						pMemAI->DoAttachLeader();
					}
					pMemAI->m_bMemberReady = false;
				}
			}
		}
		break;
	case eNpcEvent_OnMemberAllReady:
		{
			if(m_bIsMemberFighting)
			{
				m_bIsMemberFighting = false;
				Ast(pOwnCharacter->GetGroupMgr());
				const MemberIDMapType& MemberMap = pOwnCharacter->GetGroupMgr()->GetMemberIDMapType();
				MemberIDMapType::const_iterator it = MemberMap.begin();
				for(;it!=MemberMap.end();it++)
				{
					CCharacterDictator* pMember =  (*it).second;
					CMemberAI *pMemAI = pMember->GetNpcAIHandler()->CastToMemberAI();
					if(pMemAI)
						pMemAI->m_bMemberReady = false;		
				}
				CNpcEventMsg::Create(GetOwner(), eNpcEvent_OnMoveToDest);
			}
		}
		break;
	case eNpcEvent_OnGroupMemberAllDead:
		{
			m_bIsMemberFighting= false;
			CNpcEventMsg::Create(GetOwner(), eNpcEvent_OnMoveToDest);
		}
		break;
	default: 
		break;
	}
}
END_DEFINE_NPCSTATE_METHOD

DEFINE_NPCSTATE_METHOD(PatrolForGroupLeader,LeaveState)
{
	if(m_pRelayTimeTick)
	{
		GetOwner()->GetCharacter()->UnRegistDistortedTick(m_pRelayTimeTick);
		SafeDelete(m_pRelayTimeTick);
	}
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


DEFINE_NPCSTATE_METHOD(PatrolForGroupMember,EnterState)
{
	//cout<<"NPC ID = "<<GetOwner()->GetCharacter()->GetEntityID()<<"------PatrolForGroupMember,EnterState ==="<<pEvent->GetName()<<endl;
	Ast(GetOwner());
	CDummyAI* pDummyAI = GetOwner()->GetGroupLeaderDummyAI();
	if (!pDummyAI)
		return pEvent;
	if(pDummyAI->GetIsInitMemberPath())
	{
		if(pDummyAI->GetMovePathPointCount())
			GetOwner()->MemberAutoSynPos();
		else
			GetOwner()->CheckAndDoAction();
		pDummyAI->CopyEnmityListToMember(GetOwner(), pEvent);
		CCharacterDictator* pEnemy;
		CEnmityMgr* pOwnEnmity = GetOwner()->GetEnmityMgr();
		if(pOwnEnmity->IsEnemyListEmpty())
		{
			pEnemy = GetOwner()->GetNearestVisionEnemy();
			if(pEnemy)
			{
				GetOwner()->BeginAttack(pEnemy->GetEntityID());
				return pEvent;
			}
		}
		else
			return pEvent;
	}

	GetOwner()->m_bMemberReady = true;
	uint32 uOwnID = GetOwner()->GetCharacter()->GetEntityID();
	CNpcEventMsg::Create(pDummyAI, eNpcEvent_OnMemberReady,eNECI_Zero,reinterpret_cast<void*>(uOwnID));
}
END_DEFINE_NPCSTATE_METHOD


DEFINE_NPCSTATE_METHOD(PatrolForGroupMember,OnEvent)
{
	//cout<<"NPC ID = "<<GetOwner()->GetCharacter()->GetEntityID()<<"-----PatrolForGroupMember,OnEvent ==="<<pEvent->GetName()<<endl;
	switch(pEvent->GetID())
	{
	case eNpcEvent_OnBeAttacked:
		{
			CDummyAI* pDummyAI = GetOwner()->GetGroupLeaderDummyAI();
			if (!pDummyAI)
				return pEvent;
			CNpcEventMsg::Create(pDummyAI, eNpcEvent_OnGroupMemberBeAttacked,eNECI_Zero,pEvent->GetTag());
		}
		break;
	case eNpcEvent_OnCharacterInSight:
		{
			ENpcAIType AIType = GetOwner()->GetAITypeID();
			if(NpcInfoMgr::BeActiveNpc(AIType))
			{
				GetOwner()->BeginAttack((size_t)pEvent->GetTag());
				return pEvent;
			}
		}
		break;
	case eNpcEvent_OnMemberMoveTo:
		{
			CDummyAI* pDummyAI = GetOwner()->GetGroupLeaderDummyAI();
			if(pDummyAI && pDummyAI->GetIsInitMemberPath())
			{
				if(!pDummyAI->GetMovePathPointCount())
					GetOwner()->CheckAndDoAction();
			}
		}
		break;
	default:
		break;
	}
}
END_DEFINE_NPCSTATE_METHOD

DEFINE_NPCSTATE_METHOD(PatrolForGroupMember,LeaveState)
{
	//cout<<"NPC ID = "<<GetOwner()->GetCharacter()->GetEntityID()<<"-----PatrolForGroupMember,LeaveState ==="<<pEvent->GetName()<<endl;
	GetOwner()->DestoryActionTick();
	if(GetOwner()->m_bIsBeSynState)
	{
		GetOwner()->m_bIsBeSynState = false;
		GetOwner()->DoDetachLeader();
	}
	GetOwner()->m_bMemberReady = false;
}
END_DEFINE_NPCSTATE_METHOD

