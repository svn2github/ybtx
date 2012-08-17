#include "stdafx.h"
#include "CTargetChangeRule.h"
#include "CEnmityMgr.h"
#include "CSkillRuleMgr.h"
#include "CNpcAI.h"
#include "CCharacterDictator.h"

CTargetChangeRule::CTargetChangeRule(CSkillRuleMgr* pRuleMgr,CSkillRuleData* pSkillRuleData)
:CSkillRule(pRuleMgr,pSkillRuleData)
{

}

void CTargetChangeRule::OnSkillCondSatify(bool bRepeat)
{
	GetSkillRuleMgr()->AddPreparedSkillFront(this);		//保证切换目标放在前面
}

bool CTargetChangeRule::DoRuleSkill()
{
	RandomChangeTarget();
	GetOwner()->OnSkillSuccessEnd();
	CheckCoolDown();
	return true;
}

void CTargetChangeRule::RandomChangeTarget()
{
	CNpcAI* pAI = GetOwner();
	Ast(pAI);
	CCharacterDictator* pOwnChar = pAI->GetCharacter();
	Ast(pOwnChar);
	CEnmityMgr* pEnmityMgr = pAI->GetEnmityMgr();
	CCharacterDictator* pTarget = pEnmityMgr->GetRandomEnemy();
	if(pTarget && pTarget != pAI->GetFirstEnmityEnemy())
		pEnmityMgr->ForceLock(pTarget);
}
