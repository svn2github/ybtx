#include "stdafx.h"
#include "CRelationMgrClient.h"
#include "CCharacterFollower.h"
#include "CFighterFollower.h"

CRelationMgrClient& CRelationMgrClient::Inst()
{
	static CRelationMgrClient relationMgr;
	return relationMgr;
}

ERelationType CRelationMgrClient::GetRelationType(const CFighterFollower* pOne, const CFighterFollower* pAnother)
{
	CCharacterFollower* pCharOne = pOne->GetCharacter();
	CCharacterFollower* pCharAnother = pAnother->GetCharacter();
	if (!pCharOne || !pCharAnother)
		return eRT_Error;

	if (pOne == pAnother || pCharOne->GetMaster() == pCharAnother || pCharAnother->GetMaster() == pCharOne)
		return eRT_Friend;

	if (pOne->GetCtrlState(eFCS_InDuel))
	{
		if (pAnother->GetCtrlState(eFCS_InDuel))
		{
			if (pOne->CppGetGameCamp() == pAnother->CppGetGameCamp())
				return eRT_Enemy;
			else
				return eRT_DuelTargetWrong;
		}
		else
		{
			return eRT_DuelTargetWrong;
		}
	}
	else if (pAnother->GetCtrlState(eFCS_InDuel))
	{
		return eRT_TargetInDuel;
	}	

	return CRelationMgr::GetRelationType((CFighterBaseInfo*)pOne, (CFighterBaseInfo*)pAnother);
}


bool CRelationMgrClient::IsFriend(const CFighterFollower* pOne, const CFighterFollower* pAnother)
{
	return GetRelationType(pOne, pAnother) == eRT_Friend;
}

bool CRelationMgrClient::IsEnemy(const CFighterFollower* pOne, const CFighterFollower* pAnother)
{
	return GetRelationType(pOne, pAnother) == eRT_Enemy;
}

bool CRelationMgrClient::IsNeutral(const CFighterFollower* pOne, const CFighterFollower* pAnother)
{
	return GetRelationType(pOne, pAnother) == eRT_Neutral;
}
