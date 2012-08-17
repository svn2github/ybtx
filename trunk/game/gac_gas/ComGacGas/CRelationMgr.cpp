#include "stdafx.h"
#include "CFighterBaseInfo.h"
#include "CRelationMgr.h"
#include "GameDef.h"

static const uint32	BOTTOM_PK_LEVEL = 20;

CRelationMgr& CRelationMgr::Inst()
{
	static CRelationMgr relationMgr;
	return relationMgr;
}

ERelationType CRelationMgr::GetRelationType(CFighterBaseInfo* pOne, CFighterBaseInfo* pAnother)
{
	if (pOne == pAnother)
		return eRT_Friend;

	int32 iGameCampOne = pOne->CppGetGameCamp();
	int32 iGameCampAnother = pAnother->CppGetGameCamp();
	//EPKState ePKStateOne = pOne->CppGetPKState();
	//EPKState ePKStateAnother = pAnother->CppGetPKState();
	bool bPKStateOne = pOne->CppGetPKState();
	bool bPKStateAnother = pAnother->CppGetPKState();

	// 玩法活动中 临时阵营 的优先级最高,只有同时设置玩法活动阵营才有效
	if (iGameCampOne && iGameCampAnother)
	{
		if (iGameCampOne == iGameCampAnother)
		{
			if (-1 == iGameCampOne)
				return eRT_Enemy;

			return eRT_Friend;
		}
		else
		{
			return eRT_Enemy;
		}
	}

	ECamp eCampOne = pOne->CppGetCamp();
	ECamp eCampAnother = pAnother->CppGetCamp();

	if (eCamp_None == eCampOne || eCamp_None == eCampAnother)
		return eRT_Neutral;

	if (eCampOne == eCampAnother)
	{
		if (eCamp_Chaos == eCampOne)
			return eRT_Enemy;

		//if (ePKS_Safety == ePKStateOne && ePKS_Rivalry == ePKStateAnother)
		//	return eRT_FriendInRivalry;

		if (!bPKStateOne && bPKStateAnother)
			return eRT_FriendInRivalry;

		return eRT_Friend;
	}
	else
	{
		if (eCamp_Passerby == eCampOne || eCampOne == eCamp_NoQuestPasserby || eCamp_Passerby == eCampAnother || eCampAnother == eCamp_NoQuestPasserby)
			return eRT_Neutral;

		if (eCampOne < eCamp_Passerby && eCampAnother < eCamp_Passerby)		// 玩家对玩家
		{
			// BOTTOM_PK_LEVEL 限制
			if (pAnother->CppGetLevel() < BOTTOM_PK_LEVEL)
				return eRT_TargetUnderPKLevel;
			if (pOne->CppGetLevel() < BOTTOM_PK_LEVEL)
				return eRT_SelfUnderPKLevel;

			//if (ePKS_Killer == ePKStateOne || ePKS_Killer == ePKStateAnother)
			//{
			//	return eRT_Enemy;
			//}
			//else
			//{
			//	if (ePKS_Rivalry == ePKStateOne && ePKS_Rivalry == ePKStateAnother)
			//	{
			//		if (pOne->CppGetZoneType() < eZT_WarZone)
			//		{
			//			return TestOfLevelLimit(pOne->CppGetLevel(), pAnother->CppGetLevel());
			//		}
			//		else
			//		{
			//			return eRT_Enemy;
			//		}
			//	}
			//	else if (ePKS_Safety == ePKStateAnother)
			//	{
			//		return eRT_TargetSafetyState;
			//	}
			//	else // ePKS_Safety == ePKStateOne
			//	{
			//		return eRT_SelfSafetyState;
			//	}	
			//}

			if (bPKStateOne && bPKStateAnother)
			{
				return eRT_Enemy;
			}
			else if (bPKStateAnother)
			{
				return eRT_SelfSafetyState;
			}
			else // bPKStateOne == true
			{
				return eRT_TargetSafetyState;
			}
		}
		else
		{
			// 特殊阵营,只和怪物是敌方
			if (eCampOne == eCamp_Specail || eCampAnother == eCamp_Specail)
			{
				if (eCampAnother == eCamp_Monster || eCampOne == eCamp_Monster)
					return eRT_Enemy;
				else
					return eRT_Neutral;
			}

			if (eCampOne <= eCamp_Chaos && eCampAnother <= eCamp_Chaos)
				return eRT_Enemy;

			//帮会（特殊的对抗玩法里产生）
			if (eCampOne > eCamp_Chaos && eCampAnother > eCamp_Chaos)
				return eRT_Enemy;
		}
	}
	return eRT_Neutral;
}

//ERelationType CRelationMgr::TestOfLevelLimit(uint32 uOneLevel, uint32 uAnotherLevel)
//{
//	bool   bOneLevelHigher = uOneLevel >= uAnotherLevel ? true : false;
//	uint32 uLevelLimit = uint32((bOneLevelHigher ? uOneLevel : uAnotherLevel) * 0.15 + 2);
//	uint32 uDist = bOneLevelHigher ? (uOneLevel - uAnotherLevel) : (uAnotherLevel - uOneLevel);
//
//	if (uDist <= uLevelLimit)
//	{
//		return eRT_Enemy;
//	}	
//	else
//	{
//		if (bOneLevelHigher)
//			return eRT_SelfBeyondLv;
//		else
//			return eRT_TargetBeyondLv;
//	}
//}
