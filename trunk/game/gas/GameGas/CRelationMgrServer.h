#pragma once
#include "CRelationMgr.h"

class CFighterDictator;

class CRelationMgrServer
{
public:
	static CRelationMgrServer& Inst();
	static ERelationType GetRelationType(const CFighterDictator* pOne, const CFighterDictator* pAnother);

	static bool IsFriend(const CFighterDictator* pOne, const CFighterDictator* pAnother);
	static bool IsEnemy(const CFighterDictator* pOne, const CFighterDictator* pAnother);

	static bool InTheSameTeam(const CFighterDictator* pOne, const CFighterDictator* pAnother);
	static bool InTheSameTroop(const CFighterDictator* pOne, const CFighterDictator* pAnother);
	static bool IsNotJoinTeam(const CFighterDictator* pOne);
	static bool InTheSameToon(const CFighterDictator* pOne, const CFighterDictator* pAnother);
};
