#pragma once
#include "GameDef.h"

enum ERelationType
{
	eRT_Friend,				// 友方
	eRT_Enemy,				// 敌方
	eRT_Neutral,			// 中立方
	eRT_SelfUnderPKLevel,	// 自己等级低于PK底线等级，不能送死
	eRT_TargetUnderPKLevel,	// 目标等级低于PK底线等级，不能收割
	eRT_SelfBeyondLv,		// 目标等级过低
	eRT_TargetBeyondLv,		// 目标等级过高
	eRT_SelfSafetyState,	// 自己处于安全状态，无法攻击
	eRT_TargetSafetyState,	// 目标处于安全状态，无法攻击
	eRT_FriendInRivalry,	// 自己处于安全状态，友方处于对抗状态
	eRT_TargetInDuel,		// 目标处于决斗状态，不能进行该操作
	eRT_DuelTargetWrong,	// 你处于挑战状态中，目标选择错误
	eRT_Error,
};

class CFighterBaseInfo;

class CRelationMgr
{
public:
	static CRelationMgr& Inst();
	static ERelationType GetRelationType(CFighterBaseInfo* pOne, CFighterBaseInfo* pAnother);

//private:
//	static ERelationType TestOfLevelLimit(uint32 uOneLevel, uint32 uAnotherLevel);
};
