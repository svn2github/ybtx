#include "stdafx.h"
#include "CSkillRuleMgr.h"
#include "CMultiSkillRule.h"
#include "CSkillRuleBaseData.h"
#include "TimeHelper.h"
#include "CSkillServer.h"
#include "Random.h"

CMultiSkillRule::CMultiSkillRule(CSkillRuleMgr* pRuleMgr,CSkillRuleData* pSkillRuleData)
:CSkillRule(pRuleMgr,pSkillRuleData)
,m_lstSkillName(pSkillRuleData->m_lstMultiArgs)
{
}

CMultiSkillRule::~CMultiSkillRule()
{
	m_pSkillServerCfg = NULL;
}

void CMultiSkillRule::OnSkillCondSatify(bool bRepeat)
{
	uint32 uLen = m_lstSkillName.size();
	SkillNameLst::iterator iter = m_lstSkillName.begin();
	uint32 num = (uint32) Random::Rand(0,uLen-1);
	for(uint32 i = 0;i<num;i++)
		iter++;
	m_strSkillName = *iter;
	//cout<<"ÊÍ·Å¼¼ÄÜ£º"<<m_strSkillName<<endl;
	m_pSkillServerCfg = &CNormalSkillServer::GetNPCSkill(m_strSkillName);
	CSkillRule::OnSkillCondSatify(bRepeat);
}
