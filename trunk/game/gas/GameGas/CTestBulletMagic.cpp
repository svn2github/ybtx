#include "stdafx.h"
#include "CTestBulletMagic.h"
#include "CCharacterDictator.h"
#include "CCoreSceneServer.h"
#include "CFighterMediator.h"
#include "BaseHelper.h"
#include "CSkillMgrServer.h"
#include "CPixelPath.h"
#include "CSkillServer.h"
#include "CGenericTarget.h"


bool CTestBulletMagic::StartTest(uint32 uGlobalId)
{
	CTestBulletMagic* pTester = CTestBulletMagic::GetInst();
	if(pTester->IsTesting())
	{
	}
	pTester->m_bIsTesting = true;

	CCharacterDictator* pChar = CCharacterDictator::GetCharacterByID(uGlobalId); 
	CPos pos;
	pChar->GetGridPos(pos);
	EDoSkillResult eResult=eDSR_Success;
	CSkillMgrServer* pSkillMgr = pChar->GetFighter()->GetSkillMgr();
	CFighterMediator* pMedFighter = pChar->GetFighter()->CastToFighterMediator();
	if(pMedFighter)
		pMedFighter->SetFighterPos(pChar->GetPixelPos());	// 隐藏得很深的bug。。。，不加这句玩家放出子弹都是从（0,0）点飞出

	if(pSkillMgr != NULL)
	{
		const CNormalSkillServerSharedPtr& pSkill = CNormalSkillServer::GetSkillByName("测试子弹魔法");	
		if(pSkill)
		{
			CFighterDictator* pTargetObject = pChar->GetFighter()->GetTarget();

			if(pTargetObject)
			{
				CGenericTarget genericTarget;
				genericTarget.SetFighter(pTargetObject);
				pSkillMgr->NoConditionDoSkill(pSkill, 1, &genericTarget, false);
			}
		}
	}
	else
	{
		cout << "施放失败 " << eResult << endl;
	}

	return true;
}


bool CTestBulletMagic::ShutDownTest()
{
	CTestBulletMagic* pTester = CTestBulletMagic::GetInst();
	if(!pTester->IsTesting())
	{
		////count << ":(  CTestBulletMagic ShutDown fail" << endl;
		//return false;
	}
	//Shut down testing
	pTester->m_bIsTesting = false;

	//count << ":)  CTestBulletMagic ShutDown succeed" << endl;
	return true;
}

CTestBulletMagic::CTestBulletMagic()
:m_bIsTesting(false)
{
	uint32 uSuccNum = 0;
	for(uint32 i = 1; i <= 10000; ++i)
	{
		CCharacterDictator* pChar = CCharacterDictator::GetCharacterByID(i);
		if(pChar != NULL && pChar->CastToCharacterMediator())
		{
			m_vecAttacker.push_back(pChar);
			m_vecAttackerByID.push_back(pChar->GetEntityID());
		}
		else
		{
			continue;
		}

		vector<CCoreObjectServer*> vecCoreObj;
		CCoreSceneServer* pScene = pChar->GetScene();
		CFPos pixelPos = pChar->GetPixelPos();
		pScene->QueryObject(vecCoreObj, pixelPos, float(10.0f));
		vector<CCoreObjectServer*>::reverse_iterator it_coreObj = vecCoreObj.rbegin();
		for(; it_coreObj != vecCoreObj.rend(); ++it_coreObj)
		{
			CCharacterDictator* pCharTarget = CCharacterDictator::GetCharacterByCoreObj(*it_coreObj);
			if(pCharTarget == NULL || pCharTarget == pChar)
				continue;

			CPixelPath* pPath = pScene->CreatePath(pChar->GetPixelPos(), pCharTarget->GetPixelPos(), eFPT_HypoLine, eBT_MidBarrier);
			if(pPath == NULL)
				continue;
			SafeRelease(pPath);
			pChar->GetFighter()->SetTarget(pCharTarget->GetFighter());
			pPath->Release();
			break;
		}
		++uSuccNum;

	}
	//count << uSuccNum << " 个玩家设置了目标" << endl;

}
