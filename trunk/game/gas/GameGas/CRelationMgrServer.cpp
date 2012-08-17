#include "stdafx.h"
#include "CRelationMgrServer.h"
#include "CFighterDictator.h"
#include "CCharacterDictator.h"

CRelationMgrServer& CRelationMgrServer::Inst()
{
	static CRelationMgrServer relationMgr;
	return relationMgr;
}

ERelationType CRelationMgrServer::GetRelationType(const CFighterDictator* pOne, const CFighterDictator* pAnother)
{
	if (!pOne || !pAnother)
		return eRT_Error;
	CCharacterDictator* pCharOne = pOne->GetCharacter();
	CCharacterDictator* pCharAnother = pAnother->GetCharacter();
	if (!pCharOne || !pCharAnother)
		return eRT_Error;
	if (pOne == pAnother)
		return eRT_Friend;

	if (pCharOne->GetMaster() || pCharAnother->GetMaster())
	{
		if (pCharOne->GetMaster() == pCharAnother || pCharAnother->GetMaster() == pCharOne || pCharOne->GetMaster() == pCharAnother->GetMaster())
		{
			return eRT_Friend;
		}
	}

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

bool CRelationMgrServer::IsFriend(const CFighterDictator* pOne, const CFighterDictator* pAnother)
{
	return GetRelationType(pOne, pAnother) == eRT_Friend;
}

bool CRelationMgrServer::IsEnemy(const CFighterDictator* pOne, const CFighterDictator* pAnother)
{
	return GetRelationType(pOne, pAnother) == eRT_Enemy;
}

bool CRelationMgrServer::InTheSameTeam(const CFighterDictator* pOne, const CFighterDictator* pAnother)
{
	if (!pOne || !pAnother)
		return false;

	CCharacterDictator* pCharacterOne = pOne->GetCharacter();
	CCharacterDictator* pCharacterAnother = pAnother->GetCharacter();
	if (!pCharacterOne || !pCharacterAnother)
		return false;

	if (pOne == pAnother)
		return true;

	uint32 uOneTeamID = pCharacterOne->GetTeamID();
	uint32 uAnotherTeamID = pCharacterAnother->GetTeamID();

	if (uOneTeamID == 0 || uAnotherTeamID == 0)
		return false;

	if (uOneTeamID == uAnotherTeamID && (!pOne->GetCtrlState(eFCS_InDuel) && !pAnother->GetCtrlState(eFCS_InDuel)))
		return true;

	return false;
}

bool CRelationMgrServer::InTheSameTroop(const CFighterDictator* pOne, const CFighterDictator* pAnother)
{
	if (!pOne || !pAnother)
		return false;

	CCharacterDictator* pCharacterOne = pOne->GetCharacter();
	CCharacterDictator* pCharacterAnother = pAnother->GetCharacter();
	if (!pCharacterOne || !pCharacterAnother)
		return false;

	if (pOne == pAnother)
		return true;

	uint32 uOneTeamID = pCharacterOne->GetTeamID();
	uint32 uAnotherTeamID = pCharacterAnother->GetTeamID();

	if (uOneTeamID == 0 || uAnotherTeamID == 0)
		return false;

	if (uOneTeamID == uAnotherTeamID && (!pOne->GetCtrlState(eFCS_InDuel) && !pAnother->GetCtrlState(eFCS_InDuel)))
		return true;

	return false;
}

bool CRelationMgrServer::IsNotJoinTeam(const CFighterDictator* pOne)
{
	if (!pOne)
		return false;

	CCharacterDictator* pCharacterOne = pOne->GetCharacter();
	if (!pCharacterOne)
		return false;

	return pCharacterOne->GetTeamID() == 0 ? true : false;
}

bool CRelationMgrServer::InTheSameToon(const CFighterDictator* pOne, const CFighterDictator* pAnother)
{
	if (!pOne || !pAnother)
		return false;

	CCharacterDictator* pCharacterOne = pOne->GetCharacter();
	CCharacterDictator* pCharacterAnother = pAnother->GetCharacter();
	if (!pCharacterOne || !pCharacterAnother)
		return false;

	uint32 uOneToonID;
	CCharacterMediator* pMediatorOne = pCharacterOne->CastToCharacterMediator();
	if (pMediatorOne)
	{
		uOneToonID = pCharacterOne->GetTongID();
	}
	else
	{
		uOneToonID = pCharacterOne->GetMasterTongID();
	}

	uint32 uAnotherToonID = pCharacterAnother->GetTongID();

	if (uOneToonID == 0 || uAnotherToonID == 0)
		return false;

	if (uOneToonID == uAnotherToonID)
		return true;
	else
		return false;
}

