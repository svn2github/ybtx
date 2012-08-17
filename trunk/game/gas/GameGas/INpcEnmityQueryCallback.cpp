#include "stdafx.h"
#include "INpcEnmityQueryCallback.h"
#include "CEnmityMgr.h"
#include "CNpcEnmityMember.h"
#include "CCharacterDictator.h"

void INpcEnmityQueryCallback::QueryEnemyJob(CCharacterDictator* pCharacter)
{
	if (!pCharacter)
		return;
	INpcAIHandler* pAIHandler = pCharacter->GetNpcAIHandler();
	if (!pAIHandler)
		return;
	const CEnmityMgr::EnemyListType& List = pAIHandler->GetEnmityMgr()->GetEnemyList();
	CEnmityMgr::EnemyListType::const_iterator iter = List.begin();
	for (; iter != List.end(); iter++)
	{
		CNpcEnmityMember* pMember = (*iter);
		Ast (pMember);
		CCharacterDictator* pEnemy = CCharacterDictator::GetCharacterByID(pMember->GetEnemyEntityID());
		Ast (pEnemy);
		this->Exec(pMember->GetEnemyEntityID(), pEnemy);
	}
}
