#pragma once
#include "CRelationMgr.h"

class CFighterFollower;

class CRelationMgrClient
{
public:
	static CRelationMgrClient& Inst();
	static ERelationType GetRelationType(const CFighterFollower* pOne, const CFighterFollower* pAnother);

	static bool IsFriend(const CFighterFollower* pOne, const CFighterFollower* pAnother);
	static bool IsEnemy(const CFighterFollower* pOne, const CFighterFollower* pAnother);
	static bool IsNeutral(const CFighterFollower* pOne, const CFighterFollower* pAnother);
};