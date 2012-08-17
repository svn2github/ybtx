#include "stdafx.h"
#include "CSkillRule.h"
#include "CSkillRuleDef.h"
#include "CSkillRuleBaseData.h"
#include "CRuleCondition.h"
#include "CSkillRuleMgr.h"
#include "CNpcAI.h"
#include "CFighterDictator.h"
#include "CCharacterDictator.h"
#include "CoreCommon.h"
//#include "CDir.h"
#include "CSkillServer.h"
#include "CSkillRuleDoSkillMoveMgr.h"
#include "TSqrAllocator.inl"
#include "CDistortedTick.h"
#include "CTargetFilter.h"
#include "BaseHelper.h"

template class TPtRefer<CSkillRule, CSkillRuleDoSkillMoveMgr>;

class CSkillRuleCoolDownTick
	: public CDistortedTick
	, public CNpcAIMallocObject
{
public:
	CSkillRuleCoolDownTick(CSkillRule* pSkillRule) : m_pSkillRule(pSkillRule){}
	void OnTick();
private:
	CSkillRule*				m_pSkillRule;
};

CSkillRule::CSkillRule(CSkillRuleMgr* pRuleMgr,CSkillRuleData* pSkillRuleData) : m_pSkillRuleMgr(pRuleMgr)
{
	m_bLoadContinueCond = false;
	m_strSkillName		= pSkillRuleData->m_strSkillName;
	m_pSkillServerCfg = pSkillRuleData->m_pSkillServerCfg?new CNormalSkillServerSharedPtr(*pSkillRuleData->m_pSkillServerCfg):NULL;
	if (pSkillRuleData->m_iSkillPage == 0)
		m_eSRKType = eSRK_Fight;
	else
		m_eSRKType = eSRK_FightPhase;
	
	m_fMinDistance = static_cast<float>(pSkillRuleData->m_uMinDistance*eGridSpanForObj);
	m_fMaxDistance = static_cast<float>(pSkillRuleData->m_uMaxDistance*eGridSpanForObj);
	m_iSkillPage = pSkillRuleData->m_iSkillPage;
	m_uCoolDownTime = pSkillRuleData->m_uCoolDownTime;
	m_bCheckDistanceAgain = pSkillRuleData->m_bCheckDistanceAgain;
	m_pCoolDownTick = NULL;
	CreateRuleCond(pSkillRuleData);
	m_pSkillRuleReferHandler.SetHolder(this);
	m_pTargetFilter = new CTargetFilter(this, pSkillRuleData->m_eSRTargetType, pSkillRuleData->m_uTargetArg1, pSkillRuleData->m_uTargetArg2);
}

CSkillRule::~CSkillRule()
{
	for (ListRuleCond::iterator iter = m_listBeginCondition.begin();iter!=m_listBeginCondition.end();)
	{
		delete (*iter);
		m_listBeginCondition.erase(iter++);
	}
	for (ListRuleCond::iterator iter = m_listContinueCondition.begin();iter!=m_listContinueCondition.end();)
	{
		delete (*iter);
		m_listContinueCondition.erase(iter++);
	}
	if (m_pSkillRuleReferHandler.Get())
	{
		delete m_pSkillRuleReferHandler.Get();
	}
	StopDoMoveSkill();
	if (m_pCoolDownTick)
	{
		CCharacterDictator* pCharacter = GetOwner()->GetCharacter();
		Ast (pCharacter);
		pCharacter->UnRegistDistortedTick(m_pCoolDownTick);
		delete m_pCoolDownTick;
		m_pCoolDownTick = NULL;
	}
	delete m_pSkillServerCfg;
	SafeDelete(m_pTargetFilter);
}

CNpcAI* CSkillRule::GetOwner()
{
	return GetSkillRuleMgr()->GetOwner();
}

void CSkillRule::SkillRuleReferHandler(CSkillRuleDoSkillMoveMgr* pDoMoveMgr)
{
	pDoMoveMgr ? m_pSkillRuleReferHandler.Attach(pDoMoveMgr->GetRefsBySkillRule()) : m_pSkillRuleReferHandler.Detach();
}

void CSkillRule::CreateRuleCond(CSkillRuleData* pSkillRuleData)
{
	RuleCondDataList::iterator iter = pSkillRuleData->m_lstRuleCond.begin();
	RuleCondDataList::iterator iterEnd = pSkillRuleData->m_lstRuleCond.end();
	for (;iter != iterEnd;++iter)
	{
		IRuleCondHandler* pRuleCond = NULL;
		switch((*iter)->m_eRCType)
		{
		case eRCT_HpGreater:
			pRuleCond = new CHpGreaterCond(this,*iter);
			break;
		case eRCT_HpLesser:
			pRuleCond = new CHpLesserCond(this,*iter);
			break;
		case eRCT_HpFirstGreater:
			pRuleCond = new CHpFirstGreaterCond(this,*iter);
			break;
		case eRCT_HpFirstLesser:
			pRuleCond = new CHpFirstLesserCond(this,*iter);
			break;
		case eRCT_MpGreater:
			pRuleCond = new CMpGreaterCond(this,*iter);
		    break;
		case eRCT_MpLesser:
			pRuleCond = new CMpLesserCond(this,*iter);
		    break;
		case eRCT_MpFirstGreater:
			pRuleCond = new CMpFirstGreaterCond(this,*iter);
			break;
		case eRCT_MpFirstLesser:
			pRuleCond = new CMpFirstLesserCond(this,*iter);
			break;
		case eRCT_EnterBattle:
			pRuleCond = new CEnterBattleCond(this,*iter);
			break;
		case eRCT_BeHurtValueGreater:
			pRuleCond = new CBeHurtValueGreater(this,*iter);
		    break;
		case eRCT_RandomRate:
			pRuleCond = new CRandomRate(this,*iter);
			break;
		case eRCT_BattleTimeGreater:
			pRuleCond = new CBattleTimerExcess(this,*iter);
			break;
		case eRCT_OnDeadCond:
			pRuleCond = new CDeathCond(this,*iter);
			break;
		case eRCT_OnTimingCond:
			pRuleCond = new CTimerCond(this,*iter);
			break;
		case eRCT_RangeTimingCond:
			pRuleCond = new CRangeTimerCond(this,*iter);
		    break;
		case eRCT_PhaseTimeGreater:
			pRuleCond = new CPhaseTimerExcess(this,*iter);
			break;
		case eRCT_EnterPhase:
			pRuleCond = new CEnterPhaseCond(this,*iter);
			break;
		case eRCT_EnterWander:
			pRuleCond = new CEnterWanderCond(this,*iter);
			break;
		case eRCT_WanderTimeGreater:
			pRuleCond = new CWanderTimerExcess(this,*iter);
			break;
		case eRCT_EnemyDead:
			pRuleCond = new CEnemyDead(this,*iter);
			break;
		default:
			continue;
			break;
		}
		switch((*iter)->m_eRCKType)
		{
		case eRCK_Begin:
			m_listBeginCondition.push_back(pRuleCond);
			break;
		case eRCK_Continue:
			m_listContinueCondition.push_back(pRuleCond);
			break;
		default:
		    break;
		}
	}
}

bool CSkillRule::CheckContinueCond(IRuleCondHandler* pRuleCond)
{
	if(!m_bLoadContinueCond)
	{
		LoadContinueCond();
		m_bLoadContinueCond = true;
	}
	if (!pRuleCond)
	{
		if (m_listContinueCondition.empty())
			return true;
		ListRuleCond::iterator iter = m_listContinueCondition.begin();
		for (;iter != m_listContinueCondition.end();iter++)
		{
			if (!(*iter)->CheckCondition())
				return false;
		}
		return true;	
	}
	else
	{
		ListRuleCond::iterator iter = m_listContinueCondition.begin();
		for (;iter != m_listContinueCondition.end();iter++)
		{
			if ( (*iter) != pRuleCond && !(*iter)->CheckCondition())
				return false;
		}
		return true;
	}
}

void CSkillRule::LoadBeginCond()
{
	ListRuleCond::iterator iter = m_listBeginCondition.begin();
	for (;iter != m_listBeginCondition.end();++iter)
	{
		(*iter)->ReInstallCond();
	}
}
void CSkillRule::LoadContinueCond()
{
	ListRuleCond::iterator iter = m_listContinueCondition.begin();
	for (;iter != m_listContinueCondition.end();++iter)
	{
		(*iter)->ReInstallCond();
	}
}

void CSkillRule::UnInstallBeginCond()
{
	ListRuleCond::iterator iter = m_listBeginCondition.begin();
	for (;iter != m_listBeginCondition.end();++iter)
	{
		(*iter)->UnInstallCond();
	}
}
void CSkillRule::UnInstallContinueCond()
{
	m_bLoadContinueCond = false;
	ListRuleCond::iterator iter = m_listContinueCondition.begin();
	for (;iter != m_listContinueCondition.end();++iter)
	{
		(*iter)->UnInstallCond();
	}
}

CCharacterDictator* CSkillRule::GetCharacter()
{
	if (!m_pSkillRuleMgr)
		return NULL;
	return m_pSkillRuleMgr->GetOwner()->GetCharacter();
}

void CSkillRule::Handle(uint32 uEventID)
{
	CSkillRuleMgr* pSkillRuleMgr = GetSkillRuleMgr();
	ListRuleCond& lstRuleCond = m_mapRuleCondByEvent[uEventID];
	for (ListRuleCond::iterator iter = lstRuleCond.begin(); iter != lstRuleCond.end();++iter)
	{
		if((*iter)->AutoCheck())
		{
			if (eRCK_Begin == (*iter)->GetRuleCondKind())
			{
				if(CheckContinueCond())
					OnSkillCondSatify();
				if (!pSkillRuleMgr->GetCurrentSkill())
					return;
			}
			else
				UnInstallContinueCond();
		}
	}
}

void CSkillRule::AddEventHandler(uint32 uEventID,IRuleCondHandler* pRuleCond)
{
	if (!m_mapRuleCondByEvent.count(uEventID))
	{
		m_mapRuleCondByEvent[uEventID].push_back(pRuleCond);
		GetSkillRuleMgr()->AddEventHandler(uEventID,this);
	}
	else
		m_mapRuleCondByEvent[uEventID].push_back(pRuleCond);
}

void CSkillRule::CreateEventHandler()
{
	for (ListRuleCond::iterator iter = m_listBeginCondition.begin();iter!=m_listBeginCondition.end();++iter)
	{
		(*iter)->CreateEventHandler();
	}
}

EDoSkillType CSkillRule::CheckSkillDistance()
{
	if (GetTargetFilter()->SetSkillRuleTarget())
	{
		ERuleSkillType eType = GetTargetFilter()->GetRuleSkillType();
		CFPos ownPos,targetPos;
		CCharacterDictator* pCharacter = GetOwner()->GetCharacter();
		pCharacter->GetPixelPos(ownPos);
		switch(eType)
		{
		case ERuleSkillType_Target:
			{
				uint32 uTargetID = GetTargetFilter()->GetTargetID();
				CCharacterDictator* pTarget = CCharacterDictator::GetCharacterByID(uTargetID);
				if (!pTarget)
				{
					CNpcEventMsg::Create(GetOwner(), eNpcEvent_OnEnemyLost, eNECI_Fourteen);
					return EDoSkillType_None;
				}
				pTarget->GetPixelPos(targetPos);
			}
			break;

		case ERuleSkillType_Pos:
			{
				GetTargetFilter()->GetTargetPos(targetPos);
			}
			break;

		default:
			{
				CNpcEventMsg::Create(GetOwner(), eNpcEvent_OnEnemyLost, eNECI_Fifteen);
				return EDoSkillType_None;
			}
			break;
		}

		//当给自己放技能的时可能位置相同，这时候直接释放
		if ( ownPos == targetPos)
			return EDoSkillType_DirectDo;

		float fDistance = ownPos.Dist(targetPos);
		if (fDistance>m_fMaxDistance || fDistance<m_fMinDistance)
			return EDoSkillType_MoveDo;
		else
			return EDoSkillType_DirectDo;
	}
	else
	{
		CNpcEventMsg::Create(GetOwner(), eNpcEvent_OnEnemyLost, eNECI_Sixteen);
		return EDoSkillType_None;
	}
}

bool CSkillRule::CheckAndDoSkill()
{
	EDoSkillType eDoType = CheckSkillDistance();
	switch(eDoType)
	{
	case EDoSkillType_DirectDo:
		{
			bool bSuccess = DoRuleSkill();
			return bSuccess;
		}
		break;

	//这里目标类型肯定是对对象，而不是对地点
	case EDoSkillType_MoveDo:
	case EDoSkillType_BackDo:
		{
			CFPos targetPos;
			ERuleSkillType eType = GetTargetFilter()->GetRuleSkillType();
			if (eType == ERuleSkillType_Target)
			{
				uint32 uTargetID = GetTargetFilter()->GetTargetID();
				CCharacterDictator* pTarget = CCharacterDictator::GetCharacterByID(uTargetID);
				Ast (pTarget);
				pTarget->GetPixelPos(targetPos);
			}
			else
			{
				GetTargetFilter()->GetTargetPos(targetPos);
			}

			new CSkillRuleDoSkillMoveMgr(this, targetPos, eDoType);
			return true;
		}
		break;

	default:
		break;
	}
	return false;
}

bool CSkillRule::DoRuleSkill()
{
	CNpcAI* pAI = GetOwner();
	Ast(pAI);
	pAI->StopMoving();
	CSkillRuleMgr* pSkillMgr = GetSkillRuleMgr();
	bool bSuccess = false;
	if (!SkillRuleCoolDown())
		return true;
	ERuleSkillType eType = GetTargetFilter()->GetRuleSkillType();
	switch(eType)
	{
		case ERuleSkillType_Pos:
		{
			CFPos pos;
			GetTargetFilter()->GetTargetPos(pos);
			pAI->FaceToPos(pos);
			pAI->NpcDoPosSkill(m_pSkillServerCfg, pos);
			bSuccess = true;
		}
		break;

		case ERuleSkillType_Target:
		{
			uint32 uTargetID = GetTargetFilter()->GetTargetID();
			CCharacterDictator* pCharacter = CCharacterDictator::GetCharacterByID(uTargetID);
			if (pCharacter)
			{
				pAI->SetTarget(pCharacter->GetFighter(), true);
				pAI->FaceToTarget(pCharacter);
				pAI->NpcDoSkill(m_pSkillServerCfg,pCharacter);
				bSuccess = true;
			}
			else
				bSuccess = false;
		}
		break;

		default:
		break;
	}

	if (bSuccess && pSkillMgr->GetCurrentSkill())
		CheckCoolDown();

	return bSuccess;
}

void CSkillRule::DoNoRuleSkill()
{
	CNpcAI* pAI = GetOwner();
	Ast(pAI);
	CSkillRuleMgr* pSkillMgr = GetSkillRuleMgr();
	ERuleSkillType eType = GetTargetFilter()->GetRuleSkillType();
	bool bSuccess = false;
	if (!SkillRuleCoolDown())
		return;
	
	switch(eType)
	{
	case ERuleSkillType_Pos:
		{
			CFPos pos;
			if(GetTargetFilter()->GetPositionTarget(pos))
				pAI->NpcDoPosSkill(m_pSkillServerCfg,pos);
			bSuccess = true;
		}
		break;

	case ERuleSkillType_Target:
		{
			CCharacterDictator* pCharacter;
			if(GetTargetFilter()->GetCharacterTarget(pCharacter))
				pAI->NpcDoSkill(m_pSkillServerCfg,pCharacter);
			bSuccess = true;
		}
		break;

	default:
		break;
	}
	if (bSuccess && pSkillMgr->GetCurrentSkill())
		CheckCoolDown();
}

void CSkillRule::StopDoMoveSkill()
{
	if (m_pSkillRuleReferHandler.Get())
		delete m_pSkillRuleReferHandler.Get();
}

bool CSkillRule::GetIsRuleSkill()
{
	//const CNormalSkillServerSharedPtr& pSkillServer = CNormalSkillServer::GetNPCSkill(m_strSkillName);
	if(m_pSkillServerCfg && !(*m_pSkillServerCfg)->CheckDoSkillRule())
		return false;
	else
		return true;
}

void CSkillRule::OnSkillCondSatify(bool bRepeat)
{
	if (!GetIsRuleSkill())
	{
		DoNoRuleSkill();
		return;
	} 
	else
		GetSkillRuleMgr()->AddPreparedSkillBack(this, bRepeat);
 }

void CSkillRule::CheckCoolDown()
{
	if (m_uCoolDownTime != 0)
	{
		if (!m_pCoolDownTick)
		{
			m_pCoolDownTick = new CSkillRuleCoolDownTick(this);
			GetOwner()->GetCharacter()->RegistDistortedTick(m_pCoolDownTick, m_uCoolDownTime*1000);
		}
		else
		{
			LogErr("NpcSkillRule CoolDownTick exist");
		}
	}
}

void CSkillRuleCoolDownTick::OnTick()
{
	Ast (m_pSkillRule);
	CCharacterDictator* pOwnCharacter = m_pSkillRule->GetCharacter();
	Ast (pOwnCharacter);
	pOwnCharacter->UnRegistDistortedTick(this);
	m_pSkillRule->m_pCoolDownTick = NULL;
	delete this;
}
