#include "stdafx.h"
#include "CSkillRuleMgr.h"
#include "CSkillAroundRule.h"
#include "CSkillRuleBaseData.h"
#include "CCharacterDictator.h"
#include "CDir.h"
#include "CoreCommon.h"
#include "CSkillServer.h"
#include "TSqrAllocator.inl"
#include "CTargetFilter.h"

CSkillAroundRule::CSkillAroundRule(CSkillRuleMgr* pRuleMgr,CSkillRuleData* pSkillRuleData)
:CSkillRule(pRuleMgr,pSkillRuleData)
{
	CheckIsRuleSkill(pSkillRuleData);
}

CSkillAroundRule::~CSkillAroundRule()
{
}

void CSkillAroundRule::OnSkillCondSatify(bool bRepeat)
{
	DoNoRuleSkill();
}

void CSkillAroundRule::DoNoRuleSkill()
{
	if (!SkillRuleCoolDown())
		return;
	CCharacterDictator* pCharacter = GetOwner()->GetCharacter();
	CSkillRuleMgr* pSkillMgr = GetSkillRuleMgr();
	Ast (pCharacter);
	uint32 uSkillNum	= GetTargetFilter()->m_uTargetArg1;
	uint32 uRadius		= GetTargetFilter()->m_uTargetArg2;
	CFPos ownerPos;
	pCharacter->GetPixelPos(ownerPos);
	for (uint32 i = 0; i < uSkillNum; i++)
	{
		CDir dirCur;
		dirCur.SetDir((i*255)/uSkillNum);
		CVector2f vecDirf;
		dirCur.Get(vecDirf);
		CFPos pos;
		pos.x = ownerPos.x + uRadius *eGridSpanForObj* vecDirf.x;
		pos.y = ownerPos.y + uRadius *eGridSpanForObj* vecDirf.y;

		for (ListSkillIter iter = m_ListSkill.begin(); iter != m_ListSkill.end(); iter++)
		{
			GetOwner()->NpcDoPosSkill((*iter).c_str(),pos);
		}
	}
	if (pSkillMgr->GetCurrentSkill())
		CheckCoolDown();
}

void CSkillAroundRule::CheckIsRuleSkill(CSkillRuleData* pSkillRuleData)
{
	CCharacterDictator* pCharacter = GetOwner()->GetCharacter();
	Ast (pCharacter);
	for (MultiArgList::iterator iter = pSkillRuleData->m_lstMultiArgs.begin();iter != pSkillRuleData->m_lstMultiArgs.end();++iter)
	{
		const CNormalSkillServerSharedPtr& pSkillServer = CNormalSkillServer::GetNPCSkill((*iter));
		if(pSkillServer && pSkillServer->CheckDoSkillRule())
		{
			stringstream sExp;
			sExp <<"Npc: 【"<<pCharacter->m_sNpcName<<"】执行【周围地点释放技能】的技能规则时释放的技能：【"<<(*iter)<<"】必须是不走技能规则的！请修改NPCSkill.ndf表！"<<endl;
			GenErr(sExp.str());
		}
		m_ListSkill.push_back((*iter));
	}
}
