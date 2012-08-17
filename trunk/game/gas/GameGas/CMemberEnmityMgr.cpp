#include "stdafx.h"
#include "CMemberEnmityMgr.h"
#include "CNpcAI.h"
#include "CDummyAI.h"
#include "CCharacterDictator.h"

CMemberEnmityMgr::CMemberEnmityMgr(CNpcAI *pOwner) : CNpcEnmityMgr(pOwner)
{
}

CMemberEnmityMgr::~CMemberEnmityMgr()
{
}

bool CMemberEnmityMgr::BeTargetOutOfLockEnemyDis(CCharacterDictator* pCharacter)
{
	if (!pCharacter)
		return true;
	CCharacterDictator* pOwnerCharacter = GetOwner()->GetCharacter();
	Ast(pOwnerCharacter);
	CCharacterDictator* pLeader = pOwnerCharacter->GetGroupLeader();
	if (!pLeader)
	{
		LogErr("群组Leader不存在","可能是npc创建代码有bug 或是用GM指令加的群组成员");
		return true;
	}
	CDummyAI* pLaederIA = pLeader->GetNpcAIHandler()->CastToDummyAI();
	if (pLeader->GetScene() != pCharacter->GetScene())
		return true;
	if (!pLaederIA->IsInLockEnemyRange(pCharacter->GetFighter()))
		return true;
	return false;
}

bool CMemberEnmityMgr::AddEnemy(CCharacterDictator* pEnemy, bool bHurtEffect)
{
	CCharacterDictator* pLeader = GetOwner()->GetCharacter()->GetGroupLeader();
	if(!pLeader)
		return false;
	CDummyAI* pLeaderAI = pLeader->GetNpcAIHandler()->CastToDummyAI();
	if(CEnmityMgr::AddEnemy(pEnemy, bHurtEffect))
	{
		if(pLeaderAI)
			pLeaderAI->NotifyMemberBeAttacked(pEnemy);
		return true;
	}
	else
		return false;

}

