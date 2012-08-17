#include "stdafx.h"
#include "CDoSceneSkillRule.h"
#include "CCharacterDictator.h"
#include "CCoreSceneServer.h"
#include "CSkillRuleBaseData.h"
#include "CoreCommon.h"
#include "CSkillRuleMgr.h"

CDoSceneSkillRule::CDoSceneSkillRule(CSkillRuleMgr *pRuleMgr, CSkillRuleData *pSkillRuleData)
:CSkillRule(pRuleMgr, pSkillRuleData)
{
	SkillNameLst& lstSkillArg = pSkillRuleData->m_lstMultiArgs;
	SkillNameLst::const_iterator iter = lstSkillArg.begin();
	float posX,posY;
	m_strSceneName		= (*iter);
	iter++;
	posX				= static_cast<float>(atof((*iter).c_str())) * eGridSpanForObj;
	iter++;
	posY				= static_cast<float>(atof((*iter).c_str())) * eGridSpanForObj;
	iter++;
	CFPos pos(posX,posY);
	m_PixelPos = pos;
	m_bDoSkill = true;
	CheckSkill();
}

CDoSceneSkillRule::~CDoSceneSkillRule()
{

}

void CDoSceneSkillRule::DoNoRuleSkill()
{
	Ast (GetOwner());
	if (m_bDoSkill)
	{
		if (!SkillRuleCoolDown())
			return;
		CSkillRuleMgr* pSkillMgr = GetSkillRuleMgr();
		GetOwner()->NpcDoPosSkill(m_pSkillServerCfg, m_PixelPos);
		if (pSkillMgr->GetCurrentSkill())
			CheckCoolDown();
	}
}

bool CDoSceneSkillRule::DoRuleSkill()
{
	Ast (GetOwner());
	if (m_bDoSkill)
	{
		if (!SkillRuleCoolDown())
			return true;
		CSkillRuleMgr* pSkillMgr = GetSkillRuleMgr();
		GetOwner()->NpcDoPosSkill(m_pSkillServerCfg, m_PixelPos);
		if (pSkillMgr->GetCurrentSkill())
			CheckCoolDown();
	}
	return true;
}

void CDoSceneSkillRule::CheckSkill()
{
	Ast (GetOwner());
	CCharacterDictator* pCharacter = GetOwner()->GetCharacter();
	Ast (pCharacter);
	CCoreSceneServer* pScene = pCharacter->GetScene();
	Ast (pScene);
	const string& strSceneName = pScene->GetSceneName();
	if (strSceneName != m_strSceneName)
	{
		string strErrType = "Npc Do Scene Skill Rule Err !";
		ostringstream osMsg;
		osMsg<<"NpcName: "<<pCharacter->GetNpcName()<<" 所在场景名: "<<strSceneName<<" 和技能规则中的场景参数："<<m_strSceneName<<" 不一致！"<<endl;
		CfgLogErr(strErrType, osMsg.str());
		m_bDoSkill = false;
	}
	else if (!pScene->IsPixelValid(m_PixelPos))
	{
		string strErrType = "Npc Do Scene Skill Rule Err !";
		ostringstream osMsg;
		osMsg<<"NpcName: "<<pCharacter->GetNpcName()<<" 所在场景名: "<<strSceneName<<" 在技能规则中填写的坐标不在场景内！"<<endl;
		CfgLogErr(strErrType, osMsg.str());
		m_bDoSkill = false;
	}
}

