#include "stdafx.h"
#include "CTestShockWaveMagic.h"
#include "CCharacterDictator.h"
#include "CCoreSceneServer.h"
#include "CFighterDictator.h"
#include "CoreCommon.h"

bool CTestShockWaveMagic::StartTest()
{
	CTestShockWaveMagic* pTester = CTestShockWaveMagic::GetInst();
	if(pTester->IsTesting())
	{
		//cout << ":(  CTestShockWaveMagic StartTest fail" << endl;
		//return false;
	}
	//start testing
	pTester->m_bIsTesting = true;

	//cout << ":)  CTestShockWaveMagic StartTest succeed" << endl;
	
	vector<CCharacterDictator*>::iterator it = pTester->m_vecAttacker.begin();
	for(; it != pTester->m_vecAttacker.end(); ++it)
	{
		CCharacterDictator* pChar = *it; 
		CPos pos;
		pChar->GetGridPos(pos);
		//pChar->GetFighter()->DoNormalSkill("²âÊÔ³å»÷²¨Ä§·¨", 1, pos.x, pos.y);
	}

	return true;
}


bool CTestShockWaveMagic::ShutDownTest()
{
	CTestShockWaveMagic* pTester = CTestShockWaveMagic::GetInst();
	if(!pTester->IsTesting())
	{
		//cout << ":(  CTestShockWaveMagic ShutDown fail" << endl;
		//return false;
	}
	//Shut down testing
	pTester->m_bIsTesting = false;

	//cout << ":)  CTestShockWaveMagic ShutDown succeed" << endl;
	return true;
}

CTestShockWaveMagic::CTestShockWaveMagic()
:m_bIsTesting(false)
{
	for(uint32 i = 1; i <= 3000; ++i)
	{
		CCharacterDictator* pChar = CCharacterDictator::GetCharacterByID(i);
		if(pChar)
			m_vecAttacker.push_back(pChar);

		CFPos pos;
		pChar->GetPixelPos(pos);
		//vector<CCoreObjectServer*> vecCoreObj;
		//CCoreSceneServer* pScene = pChar->GetScene();
		//pScene->QueryObject(vecCoreObj, pos, float(10.0f));
		//vector<CCoreObjectServer*>::reverse_iterator it_coreObj = vecCoreObj.rbegin();
		//for(; it_coreObj != vecCoreObj.rend(); ++it_coreObj)
		//{
			//CCharacterDictator* pCharTarget = CCharacterDictator::GetCharacterByCoreObj(*it_coreObj);
			//if(pCharTarget == NULL)
				//continue;

		pos.x += 7*eGridSpanForObj;
		pos.y += 7*eGridSpanForObj;
		pChar->GetFighter()->SetTargetPos(pos);
		//}

	}

}
