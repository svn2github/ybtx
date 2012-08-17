#include "stdafx.h"
#include "CNpcEnmityMgr.h"
#include "CCharacterDictator.h"
#include "CNpcAI.h"
#include "CNpcEnmityMember.h"
#include "NpcInfoMgr.h"

CNpcEnmityMgr::CNpcEnmityMgr(CNpcAI *pOwner) : CEnmityMgr(pOwner)
{
}

CNpcEnmityMgr::~CNpcEnmityMgr()
{
}

bool CNpcEnmityMgr::JudgeIsEnemy(CCharacterDictator* pTarget)
{
	if(NULL == pTarget)
		return false;
	CNpcAI* pOwnAI = GetOwner();
	Ast(pOwnAI);
	CCharacterDictator* pOwnCharacter = pOwnAI->GetCharacter();
	Ast(pOwnCharacter);
	Ast(pOwnCharacter->GetFighter());
	if(!pOwnAI->CanFight(pTarget) || BeTargetOutOfLockEnemyDis(pTarget))
		return false;
	else 
		return true;
}

void CNpcEnmityMgr::ClearAllEnmity()
{
	if(IsEnemyListEmpty())
		return;
	CNpcAI* pOwnAI = GetOwner();
	Ast(pOwnAI);
	EnemyListType& enemyLst = GetEnemyList();

	do 
	{
		SQR_TRY
		{
			while(!enemyLst.empty())
			{
				CNpcEnmityMember* pMember = enemyLst.front();
				enemyLst.pop_front();
				delete pMember;
			}
			break;
		}
		SQR_CATCH(exp)
		{
			LogExp(exp);
		}
		SQR_TRY_END;
	} while(true);
	
	pOwnAI->SetTarget(NULL);
	UpDateNpcBattleState();
}

