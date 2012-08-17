#include "stdafx.h"
#include "CGoBackExclaimRule.h"
#include "CSkillRuleMgr.h"
#include "CNpcAI.h"
#include "CoreCommon.h"
#include "CCharacterDictator.h"
#include "CSkillRuleBaseData.h"
#include "CNpcSpecialSkillCfg.h"

CGoBackExclaimRule::CGoBackExclaimRule(CSkillRuleMgr* pRuleMgr,CSkillRuleData* pSkillRuleData)
:CSkillRule(pRuleMgr,pSkillRuleData)
,m_lstSkillArgs(pSkillRuleData->m_lstMultiArgs)
{
	SkillNameLst::const_iterator iter = m_lstSkillArgs.begin();
	const TCHAR* sSize	= (*iter).c_str();
	const TCHAR* sNum = (*(++iter)).c_str();
	m_fSize = static_cast<float>(atof(sSize));
	m_uNum	= static_cast<uint32>(atoi(sNum));
}

void CGoBackExclaimRule::OnSkillCondSatify(bool bRepeat)
{
	GetSkillRuleMgr()->AddPreparedSkillBack(this, bRepeat);
}

bool CGoBackExclaimRule::DoRuleSkill()
{
	CNpcAI* pAI = GetOwner();
	Ast(pAI);
	if (!SkillRuleCoolDown())
		return true;
	CCharacterDictator* pOwnCharacter = pAI->GetCharacter();
	CSkillRuleMgr* pSkillMgr = GetSkillRuleMgr();
	Ast (pOwnCharacter);
	pAI->NotifyAccompanierEnterBattle(m_fSize, m_uNum, true);
	pAI->OnSkillSuccessEnd();
	pAI->NpcDoSkill(CNpcSpecialSkillCfg::ms_vecNpcSpecialSkill[eSSN_Exclaim], pOwnCharacter);
	if (pSkillMgr->GetCurrentSkill())
		CheckCoolDown();
	return true;
}

EDoSkillType CGoBackExclaimRule::CheckSkillDistance()
{
	CNpcAI* pAI = GetOwner();
	Ast (pAI);
	CCharacterDictator* pCharacter = pAI->GetCharacter();
	Ast (pCharacter);
	CFPos curpos, birthpos;
	pCharacter->GetPixelPos(curpos);
	pAI->GetNpcBirthPos(birthpos);
	if (curpos == birthpos)
		return EDoSkillType_DirectDo;
	return EDoSkillType_BackDo;
}

