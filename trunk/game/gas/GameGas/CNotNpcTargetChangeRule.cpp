#include "stdafx.h"
#include "CNotNpcTargetChangeRule.h"
#include "CEnmityMgr.h"
#include "CSkillRuleMgr.h"
#include "CNpcAI.h"
#include "CCharacterDictator.h"

CNotNpcTargetChangeRule::CNotNpcTargetChangeRule(CSkillRuleMgr* pRuleMgr,CSkillRuleData* pSkillRuleData)
:CSkillRule(pRuleMgr,pSkillRuleData)
{

}

void CNotNpcTargetChangeRule::OnSkillCondSatify(bool bRepeat)
{
	GetSkillRuleMgr()->AddPreparedSkillFront(this);		//保证切换目标放在前面
}

bool CNotNpcTargetChangeRule::DoRuleSkill()
{
	RandomChangeTarget();
	GetOwner()->OnSkillSuccessEnd();
	CheckCoolDown();
	return true;
}

void CNotNpcTargetChangeRule::RandomChangeTarget()
{
	CNpcAI* pAI = GetOwner();
	Ast(pAI);
	CCharacterDictator* pOwnChar = pAI->GetCharacter();
	Ast(pOwnChar);
	CEnmityMgr* pEnmityMgr = pAI->GetEnmityMgr();
	CCharacterDictator* pTarget = pEnmityMgr->GetRandomNotNpcEnemy();
	if(pTarget && pTarget != pAI->GetFirstEnmityEnemy())
		pEnmityMgr->ForceLock(pTarget);
}
