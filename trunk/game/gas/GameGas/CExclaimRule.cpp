#include "stdafx.h"
#include "CExclaimRule.h"
#include "CSkillRuleMgr.h"
#include "CNpcAI.h"
#include "CoreCommon.h"
#include "CSkillRuleBaseData.h"
#include "CCharacterDictator.h"
#include "CNpcSpecialSkillCfg.h"

CExclaimRule::CExclaimRule(CSkillRuleMgr* pRuleMgr,CSkillRuleData* pSkillRuleData, bool bShowEffect)
:CSkillRule(pRuleMgr,pSkillRuleData)
,m_lstSkillArgs(pSkillRuleData->m_lstMultiArgs)
{
	SkillNameLst::const_iterator iter = m_lstSkillArgs.begin();
	const TCHAR* sSize = (*iter).c_str();
	const TCHAR* sNum	= (*(++iter)).c_str();
	m_fSize = static_cast<float>(atof(sSize));
	m_uNum	= static_cast<uint32>(atoi(sNum));
	m_bShowEffect = bShowEffect;
}

void CExclaimRule::OnSkillCondSatify(bool bRepeat)
{
	GetSkillRuleMgr()->AddPreparedSkillBack(this, bRepeat);
}

bool CExclaimRule::DoRuleSkill()
{
	CNpcAI* pAI = GetOwner();
	Ast(pAI);
	if (!SkillRuleCoolDown())
		return true;
	CCharacterDictator* pOwnCharacter = pAI->GetCharacter();
	CSkillRuleMgr* pSkillMgr = GetSkillRuleMgr();
	Ast (pOwnCharacter);
	pAI->NotifyAccompanierEnterBattle(m_fSize, m_uNum, m_bShowEffect);
	pAI->OnSkillSuccessEnd();
	if (m_bShowEffect)
		pAI->NpcDoSkill(CNpcSpecialSkillCfg::ms_vecNpcSpecialSkill[eSSN_Exclaim], pOwnCharacter);
	if (pSkillMgr->GetCurrentSkill())
		CheckCoolDown();
	return true;
}

