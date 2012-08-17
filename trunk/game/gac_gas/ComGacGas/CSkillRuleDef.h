#pragma once

enum ESRTargetType
{
	eSRTT_None,
	eSRTT_NoArg,							//无参数	--------------	
	eSRTT_T_EnemyObject,					//仇恨目标
	eSRTT_P_EnemyPostion,					//目标位置
	eSRTT_T_Self,							//自身
	eSRTT_P_SelfPosition,					//自身位置
	eSRTT_P_RandomEnemyPostion,				//随机目标位置
	eSRTT_T_NearestEnemy,					//最近敌人
	eSRTT_P_NearestEnemyPos,				//最近敌人位置
	eSRTT_T_FarthestEnemy,					//最远敌人
	eSRTT_P_FarthestEnemyPos,				//最远敌人位置
	eSRTT_T_RandomEnemy,					//随机目标
	eSRTT_T_MaxHpEnemy,						//最高血量仇恨目标
	eSRTT_P_MaxHpEnemyPos,					//最高血量仇恨目标位置
	eSRTT_T_MinHpEnemy,						//最低血量仇恨目标
	eSRTT_P_MinHpEnemyPos,					//最低血量仇恨目标位置
	eSRTT_T_MaxMpEnemy,						//最高魔法仇恨目标
	eSRTT_P_MaxMpEnemyPos,					//最高魔法仇恨目标位置
	eSRTT_T_MinMpEnemy,						//最低魔法仇恨目标
	eSRTT_P_MinMpEnemyPos,					//最低魔法仇恨目标位置
	eSRRT_OneArg,							//一个参数  ----------------
	eSRTT_P_SelfDirection,					//自身朝向
	eSRTT_P_SelfReverseDirection,			//自身反向
	eSRTT_T_RandomFriend,					//随机友方目标
	eSRTT_P_RandomFriendPos,				//随机友方目标位置
	eSRTT_P_RandomDirection,				//随机方向
	eSRTT_T_NearestFriend,					//最近友方
	eSRTT_P_NearestFriendPos,				//最近友方位置
	eSRTT_T_FarthestFriend,					//最远友方
	eSRTT_P_FarthestFriendPos,				//最远友方位置
	eSRTT_T_RandomUnEnmity,					//非仇恨目标随机目标
	eSRTT_P_RandomUnEnmityPos,				//非仇恨目标随机目标位置
	eSRTT_T_RandomUnServant,				//非召唤兽随机目标
	eSRTT_P_RandomUnServantPos,				//非召唤兽随机目标位置
	eSRTT_T_RamdomUnEnmityUnServant,		//非仇恨目标非召唤兽随机目标
	eSRTT_P_RamdomUnEnmityUnServantPos,		//非仇恨目标非召唤兽随机目标位置
	eSRTT_T_MaxHpFriend,					//最高血量友方
	eSRTT_P_MaxHpFriendPos,					//最高血量友方位置
	eSRTT_T_MinHpFriend,					//最低血量友方
	eSRTT_P_MinHpFriendPos,					//最低血量友方位置
	eSRTT_T_MaxMpFriend,					//最高魔法友方
	eSRTT_P_MaxMpFriendPos,					//最高魔法友方位置
	eSRTT_T_MinMpFriend,					//最低魔法友方
	eSRTT_P_MinMpFriendPos,					//最低魔法友方位置
	eSRTT_TwoArg,							//两个参数		-----------------------
	eSRTT_P_AroundPos,						//周围地点
	eSRTT_P_RandomPos,						//随机位置
	eSRTT_End,								//				-----------------------
};

enum ERCondType
{
	eRCT_None,
	eRCT_HpGreater,						//生命高于
	eRCT_HpLesser,						//生命低于
	eRCT_HpFirstGreater,				//生命首次高于
	eRCT_HpFirstLesser,					//生命首次低于
	eRCT_MpGreater,						//魔法高于
	eRCT_MpLesser,						//魔法低于
	eRCT_MpFirstGreater,				//魔法首次高于
	eRCT_MpFirstLesser,					//魔法首次低于
	eRCT_EnterBattle,					//进入战斗
	eRCT_BeHurtValueGreater,			//伤害高于
	eRCT_RandomRate,					//随机
	eRCT_BattleTimeGreater,				//战斗时间超过
	eRCT_OnDeadCond,					//死亡释放
	eRCT_OnTimingCond,					//定时
	eRCT_RangeTimingCond,				//范围定时
	eRCT_PhaseTimeGreater,				//阶段时间超过
	eRCT_EnterPhase,					//进入阶段
	eRCT_EnterWander,					//进入待机状态
	eRCT_WanderTimeGreater,				//待机时间超过
	eRCT_EnemyDead,						//敌人死亡

	eRCT_End,
};

enum ESkillRuleTypeName
{
	eSKRT_None,
	eSKRT_Normal,				//普通规则
	eSKRT_PhaseChange,			//切换阶段
	eSKRT_TargetChange,			//随机改变目标
	eSKRT_NotNpcTargetChange,	//随机改变非npc目标
	eSKRT_Exclaim,				//呼救
	eSKRT_GoBackExclaim,		//返回呼救
	eSKRT_CMultiSkill,			//多选一
	eSKRT_SkillAround,			//周围地点释放技能
	eSKRT_Exclaim_NoEffect,		//无特效呼救
	eSKRT_AlertExclaim,			//呼救警戒
	eSKRT_DoSceneSkill,			//场景坐标释放
	eSKRT_DoSkillOnStateCond,	//状态条件施放

	ESKRT_End,
};

enum ERuleCondKind
{
	eRCK_Begin,
	eRCK_Continue,
};

enum ESkillRuleKind
{
	eSRK_Wander,
	eSRK_FightPhase,
	eSRK_Fight,
	eSRK_Dead,
};

enum ERuleSkillType
{
	ERuleSkillType_None,
	ERuleSkillType_Target,
	ERuleSkillType_Pos,
};

enum EDoSkillType
{
	EDoSkillType_None,
	EDoSkillType_DirectDo,
	EDoSkillType_MoveDo,
	EDoSkillType_BackDo,
};

