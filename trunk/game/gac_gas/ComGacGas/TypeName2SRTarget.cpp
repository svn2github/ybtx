#include "stdafx.h"
#include "TypeName2SRTarget.h"

TypeName2SRTarget::TypeName2SRTarget()
{
	InitTargetTypeMap();
	InitCondTypeMap();
}
void TypeName2SRTarget::InitTargetTypeMap()
{
	mapTargetType["仇恨目标"]						= eSRTT_T_EnemyObject;
	mapTargetType["目标位置"]						= eSRTT_P_EnemyPostion;
	mapTargetType["自身"]							= eSRTT_T_Self;
	mapTargetType["自身位置"]						= eSRTT_P_SelfPosition;
	mapTargetType["随机目标位置"]					= eSRTT_P_RandomEnemyPostion;
	mapTargetType["自身朝向"]						= eSRTT_P_SelfDirection;
	mapTargetType["自身反向"]						= eSRTT_P_SelfReverseDirection;
	mapTargetType["随机目标"]						= eSRTT_T_RandomEnemy;
	mapTargetType["随机友方目标"]					= eSRTT_T_RandomFriend;
	mapTargetType["最高血量仇恨目标"]				= eSRTT_T_MaxHpEnemy;
	mapTargetType["最高血量仇恨目标位置"]			= eSRTT_P_MaxHpEnemyPos;
	mapTargetType["最低血量仇恨目标"]				= eSRTT_T_MinHpEnemy;
	mapTargetType["最低血量仇恨目标位置"]			= eSRTT_P_MinHpEnemyPos;
	mapTargetType["最高魔法仇恨目标"]				= eSRTT_T_MaxMpEnemy;
	mapTargetType["最高魔法仇恨目标位置"]			= eSRTT_P_MaxMpEnemyPos;
	mapTargetType["最低魔法仇恨目标"]				= eSRTT_T_MinMpEnemy;
	mapTargetType["最低魔法仇恨目标位置"]			= eSRTT_P_MinMpEnemyPos;
	mapTargetType["随机方向"]						= eSRTT_P_RandomDirection;
	mapTargetType["最近敌人"]						= eSRTT_T_NearestEnemy;
	mapTargetType["最近敌人位置"]					= eSRTT_P_NearestEnemyPos;
	mapTargetType["最远敌人"]						= eSRTT_T_FarthestEnemy;
	mapTargetType["最远敌人位置"]					= eSRTT_P_FarthestEnemyPos;
	mapTargetType["最近友方"]						= eSRTT_T_NearestFriend;
	mapTargetType["最近友方位置"]					= eSRTT_P_NearestFriendPos;
	mapTargetType["最远友方"]						= eSRTT_T_FarthestFriend;
	mapTargetType["最远友方位置"]					= eSRTT_P_FarthestFriendPos;
	mapTargetType["非仇恨目标随机目标"]				= eSRTT_T_RandomUnEnmity;
	mapTargetType["非仇恨目标随机目标位置"]			= eSRTT_P_RandomUnEnmityPos;
	mapTargetType["非召唤兽随机目标"]				= eSRTT_T_RandomUnServant;
	mapTargetType["非召唤兽随机目标位置"]			= eSRTT_P_RandomUnServantPos;
	mapTargetType["非仇恨目标非召唤兽随机目标"]		= eSRTT_T_RamdomUnEnmityUnServant;
	mapTargetType["非仇恨目标非召唤兽随机目标位置"]	= eSRTT_P_RamdomUnEnmityUnServantPos;
	mapTargetType["最高血量友方"]					= eSRTT_T_MaxHpFriend;
	mapTargetType["最高血量友方位置"]				= eSRTT_P_MaxHpFriendPos;
	mapTargetType["最低血量友方"]					= eSRTT_T_MinHpFriend;
	mapTargetType["最低血量友方位置"]				= eSRTT_P_MinHpFriendPos;
	mapTargetType["最高魔法友方"]					= eSRTT_T_MaxMpFriend;
	mapTargetType["最高魔法友方位置"]				= eSRTT_P_MaxMpFriendPos;
	mapTargetType["最低魔法友方"]					= eSRTT_T_MinMpFriend;
	mapTargetType["最低魔法友方位置"]				= eSRTT_P_MinMpFriendPos;
	mapTargetType["周围地点"]						= eSRTT_P_AroundPos;
	mapTargetType["随机位置"]						= eSRTT_P_RandomPos;
}

void TypeName2SRTarget::InitCondTypeMap()
{
	mapCondType["生命高于"]		= eRCT_HpGreater;
	mapCondType["生命低于"]		= eRCT_HpLesser;
	mapCondType["生命首次高于"] = eRCT_HpFirstGreater;
	mapCondType["生命首次低于"] = eRCT_HpFirstLesser;
	mapCondType["魔法高于"]		= eRCT_MpGreater;
	mapCondType["魔法低于"]		= eRCT_MpLesser;
	mapCondType["魔法首次高于"] = eRCT_MpFirstGreater;
	mapCondType["魔法首次低于"] = eRCT_MpFirstLesser;
	mapCondType["进入战斗"]		= eRCT_EnterBattle;
	mapCondType["伤害高于"]		= eRCT_BeHurtValueGreater;
	mapCondType["随机"]			= eRCT_RandomRate;
	mapCondType["战斗时间超过"] = eRCT_BattleTimeGreater;
	mapCondType["死亡释放"]		= eRCT_OnDeadCond;
	mapCondType["定时"]			= eRCT_OnTimingCond;
	mapCondType["范围定时"]		= eRCT_RangeTimingCond;
	mapCondType["阶段时间超过"] = eRCT_PhaseTimeGreater;
	mapCondType["进入阶段"]		= eRCT_EnterPhase;
	mapCondType["进入待机状态"] = eRCT_EnterWander;
	mapCondType["待机时间超过"] = eRCT_WanderTimeGreater;
	mapCondType["敌人死亡"]		= eRCT_EnemyDead;
}

