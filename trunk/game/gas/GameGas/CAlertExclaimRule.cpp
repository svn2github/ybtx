#include "stdafx.h"
#include "CAlertExclaimRule.h"
#include "CSkillRuleBaseData.h"
#include "ExpHelper.h"
#include "CNpcAI.h"
#include "CCharacterDictator.h"
#include "CCoreSceneServer.h"
#include "ICoreObjectServerHandler.h"

CAlertExclaimRule::CAlertExclaimRule(CSkillRuleMgr *pRuleMgr, CSkillRuleData *pSkillRuleData)
:CSkillRule(pRuleMgr,pSkillRuleData)
,m_lstSkillArgs(pSkillRuleData->m_lstMultiArgs)
{
	SkillNameLst::const_iterator iter = m_lstSkillArgs.begin();
	const TCHAR* sSize		= (*iter).c_str();
	const TCHAR* sNum		= (*(++iter)).c_str();
	const TCHAR* sAlertTime	= (*(++iter)).c_str();
	m_fSize = static_cast<float>(atof(sSize));
	m_uNum	= static_cast<uint32>(atoi(sNum));
	m_uAlertTime = static_cast<uint32>(atoi(sAlertTime));
}

CAlertExclaimRule::~CAlertExclaimRule()
{

}

void CAlertExclaimRule::OnSkillCondSatify(bool bRepeat)
{
	DoNoRuleSkill();
}

void CAlertExclaimRule::DoNoRuleSkill()
{
	NotifyEnterAlert();
}

void CAlertExclaimRule::NotifyEnterAlert()
{
	CNpcAI* pAI = GetOwner();
	Ast(pAI);
	CCharacterDictator* pOwnCharacter = pAI->GetCharacter();
	Ast (pOwnCharacter);
	CCharacterDictator* pEnemy = pAI->GetFirstEnmityEnemy();
	if (!pEnemy)
		return;
	uint32 uEnemyID = pEnemy->GetEntityID();
	//pAI->NpcDoSkill("NPCºô¾È×´Ì¬", pOwnCharacter);

	vector<CCoreObjectServer* > vecObject;
	uint32 uExitNum = 0;
	CFPos pixelPos = pOwnCharacter->GetPixelPos();
	pOwnCharacter->GetScene()->QueryObject(vecObject, pixelPos, m_fSize);
	for(vector<CCoreObjectServer* >::iterator it = vecObject.begin(); it != vecObject.end(); ++it)
	{
		if ( *it == NULL ) 
			break;
		ICoreObjectServerHandler* pHandler = (*it)->GetHandler();
		if ( pHandler == NULL )
			break;
		size_t eType = (size_t)(pHandler->GetTag());
		switch(eType)
		{
		case eCOHT_Character:
			CCharacterDictator *pCharacter = CCharacterDictator::GetCharacterByCoreObj(*it);
			Ast (pCharacter);
			if (!pCharacter->GetNpcAIHandler() || !pCharacter->CppIsLive() || pCharacter == GetCharacter())
				break;
			CNpcAI* pRangeAI = pCharacter->GetNpcAIHandler()->CastToNpcAI();
			if (!pRangeAI->BeInBattle() && pRangeAI->IsExclaimAlert() && pRangeAI->IsInExclaimAlertDist(pEnemy))
			{
				uExitNum++;
				CNpcEventMsg::Create(pRangeAI, eNpcEvent_OnEnterAlertTarget, eNECI_Zero, reinterpret_cast<void*>(uEnemyID), reinterpret_cast<void*>(m_uAlertTime));
			}
			if (uExitNum == m_uNum)
				return;
		}
	}
}

