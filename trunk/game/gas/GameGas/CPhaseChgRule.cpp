#include "stdafx.h"
#include "CPhaseChgRule.h"
#include "CNpcEventMsg.h"
#include "CSkillRuleMgr.h"
#include "CNpcAI.h"

CPhaseChangeRule::CPhaseChangeRule(CSkillRuleMgr* pRuleMgr,CSkillRuleData* pSkillRuleData)
:CSkillRule(pRuleMgr,pSkillRuleData)
{
	
}

void CPhaseChangeRule::OnSkillCondSatify(bool bRepeat)
{
	GetSkillRuleMgr()->ClearPhasePreparedSkillList();
	GetSkillRuleMgr()->AddPreparedSkillFront(this);
}

bool CPhaseChangeRule::DoRuleSkill()
{
	CNpcAI* pAI = GetOwner();
	Ast(pAI);
	GetSkillRuleMgr()->ChangeNextPhase();
	pAI->OnSkillSuccessEnd();
	//cout<<"ÇÐ»»µ½½×¶Î£º "<<GetSkillRuleMgr()->m_uCurrentPage<<endl;
	CNpcEventMsg::Create(pAI, eNpcEvent_OnEnterPhase);
	return true;
}

