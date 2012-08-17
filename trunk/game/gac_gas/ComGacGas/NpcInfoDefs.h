#pragma once

#include "GameDef.h"

struct TypeMapRgst
{
	TypeMapRgst()
	{
		mapNpcTypeMap["普通"]			= ENpcType_Normal;
		mapNpcTypeMap["图腾"]			= ENpcType_Totem;
		mapNpcTypeMap["宠物"]			= ENpcType_Pet;
		mapNpcTypeMap["召唤兽"]			= ENpcType_Summon;
		mapNpcTypeMap["战斗坐骑"]		= ENpcType_BattleHorse;
		mapNpcTypeMap["虚拟Npc"]		= ENpcType_Dummy;
		mapNpcTypeMap["团队成员"]		= ENpcType_Member;
		mapNpcTypeMap["军械兽"]			= ENpcType_OrdnanceMonster;
		mapNpcTypeMap["魔法阵"]			= ENpcType_MagicBuilding;
		mapNpcTypeMap["移动炮台"]		= ENpcType_Cannon;
		mapNpcTypeMap["怪物卡"]			= ENpcType_MonsterCard;
		mapNpcTypeMap["分身"]			= ENpcType_Shadow;
		mapNpcTypeMap["运输车"]			= ENpcType_Truck;
		mapNpcTypeMap["Boss小弟"]		= ENpcType_BossCortege;
		mapNpcTypeMap["任务召唤Npc"]	= ENpcType_QuestBeckonNpc;
		mapNpcTypeMap["攻击最近"]		= ENpcType_AttackNearest;
		mapNpcTypeMap["跟随宠物"]		= ENpcType_LittlePet;
		mapNpcTypeMap["不受控召唤兽"]	= ENpcType_NotCtrlSummon;
	}
	typedef map<string, ENpcType> mapNpcType_T;
	typedef mapNpcType_T::const_iterator  mapNpcType_itr;
	mapNpcType_T mapNpcTypeMap;
};
static TypeMapRgst NpcType;

struct NpcTypeMapName
{
	NpcTypeMapName()
	{
		mNpcTypeMapName[ENpcType_Normal]			= "普通";
		mNpcTypeMapName[ENpcType_Totem]				= "图腾";
		mNpcTypeMapName[ENpcType_Pet]				= "宠物";
		mNpcTypeMapName[ENpcType_Summon]			= "召唤兽";
		mNpcTypeMapName[ENpcType_BattleHorse]		= "战斗坐骑";
		mNpcTypeMapName[ENpcType_Dummy]				= "虚拟Npc";
		mNpcTypeMapName[ENpcType_Member]			= "团队成员";
		mNpcTypeMapName[ENpcType_OrdnanceMonster]	= "军械兽";
		mNpcTypeMapName[ENpcType_MagicBuilding]		= "魔法阵";
		mNpcTypeMapName[ENpcType_Cannon]			= "移动炮台";
		mNpcTypeMapName[ENpcType_MonsterCard]		= "怪物卡";
		mNpcTypeMapName[ENpcType_Shadow]			= "分身";
		mNpcTypeMapName[ENpcType_Truck]				= "运输车";
		mNpcTypeMapName[ENpcType_BossCortege]		= "Boss小弟";
		mNpcTypeMapName[ENpcType_QuestBeckonNpc]	= "任务召唤Npc";
		mNpcTypeMapName[ENpcType_AttackNearest]		= "攻击最近";
		mNpcTypeMapName[ENpcType_LittlePet]			= "跟随宠物";
		mNpcTypeMapName[ENpcType_NotCtrlSummon]		= "不受控召唤兽";
	}
	typedef map<ENpcType, string> mapNpcType_T;
	typedef mapNpcType_T::const_iterator  mapNpcType_itr;
	mapNpcType_T mNpcTypeMapName;
};
static NpcTypeMapName sNpcTypeMapName; 

struct AITypeAoiRgst
{
	AITypeAoiRgst()
	{
		mapObjectAoiType["普通怪被动支持巡逻"]						= EAoi_PassiveNpc;
		mapObjectAoiType["新版规则测试用AI"]						= EAoi_PassiveNpc;
		mapObjectAoiType["普通怪主动支持巡逻"]						= EAoi_ActiveNpc;
		mapObjectAoiType["普通怪主动支持巡逻可见Npc"]				= EAoi_ActiveServant;
		mapObjectAoiType["任务NPC支持巡逻"]							= EAoi_PassiveNpc;		//任务Npc需要被冲击波魔法看见，所以改成被动怪AOI
		mapObjectAoiType["物件Npc"]									= EAoi_PassiveNpc;		//物件（建筑什么的也要被魔法看到）
		mapObjectAoiType["被打不反击"]								= EAoi_PassiveNpc;
		mapObjectAoiType["图腾"]									= EAoi_ActiveServant;
		mapObjectAoiType["召唤兽"]									= EAoi_PassiveServant;
		mapObjectAoiType["虚拟Npc支持巡逻"]							= EAoi_SleepNPC;	
		mapObjectAoiType["群组成员被动怪"]							= EAoi_PassiveNpc;
		mapObjectAoiType["群组成员主动怪"]							= EAoi_ActiveNpc;
		mapObjectAoiType["箭塔AI"]									= EAoi_ActiveServant;
		mapObjectAoiType["战斗坐骑"]								= EAoi_BattleHorse;
		mapObjectAoiType["军械兽"]									= EAoi_PassiveNpc;
		mapObjectAoiType["魔法阵"]									= EAoi_ActiveServant;			//需要看到主角和Npc的主动类型
		mapObjectAoiType["移动炮台"]								= EAoi_PassiveNpc;
		mapObjectAoiType["怪物卡"]									= EAoi_ActiveServant;
		mapObjectAoiType["宠物"]									= EAoi_PassiveNpc;
		mapObjectAoiType["Boss小弟"]								= EAoi_ActiveServant;
		mapObjectAoiType["Boss主动支持巡逻"]						= EAoi_ActiveServant;
		mapObjectAoiType["Boss被动支持巡逻"]						= EAoi_PassiveNpc;
		mapObjectAoiType["剧场Npc"]									= EAoi_TaskNpc;
		mapObjectAoiType["战斗坐骑"]								= EAoi_BattleHorse;
		mapObjectAoiType["被打不还手不转向非任务Npc"]				= EAoi_PassiveNpc;
		mapObjectAoiType["运输车"]									= EAoi_PassiveNpc;
		mapObjectAoiType["逃跑AI"]									= EAoi_ActiveNpc;
		mapObjectAoiType["跟随宠物"]								= EAoi_TaskNpc;
	}	
	typedef map<string, EObjectAoiType> mapObjectAoi_T;
	typedef mapObjectAoi_T::const_iterator  mapObjectAoi_itr;
	mapObjectAoi_T mapObjectAoiType;
};
static AITypeAoiRgst sObjectAoiType; 

struct AITypeMap
{
	AITypeMap()
	{
		mapNpcAIType["普通怪被动支持巡逻"]						= ENpcAIType_NormalPassivityPatrol;
		mapNpcAIType["普通怪主动支持巡逻"]						= ENpcAIType_NormalInitiativePatrolNoFefer;
		mapNpcAIType["普通怪主动支持巡逻可见Npc"]				= ENpcAIType_NormalInitiativePatrolNoFeferCanSeeNpc;
		mapNpcAIType["任务NPC支持巡逻"]							= ENpcAIType_Task;
		mapNpcAIType["被打不反击"]								= ENpcAIType_PassivityCopIt;
		mapNpcAIType["图腾"]									= ENpcAIType_Totem;
		mapNpcAIType["召唤兽"]									= ENpcAIType_Servant;
		mapNpcAIType["虚拟Npc支持巡逻"]							= ENpcAIType_VirtualNpc;	
		mapNpcAIType["群组成员被动怪"]							= ENpcAIType_GroupMemberPassivity;
		mapNpcAIType["群组成员主动怪"]							= ENpcAIType_GroupMemberInitiative;
		mapNpcAIType["箭塔AI"]									= ENpcAIType_TowerAttack;
		mapNpcAIType["战斗坐骑"]								= ENpcAIType_BattleHorse;
		mapNpcAIType["物件Npc"]									= ENpcAIType_Building;
		mapNpcAIType["军械兽"]									= ENpcAIType_OrdnanceMonster;
		mapNpcAIType["魔法阵"]									= ENpcAIType_MagicBuilding;
		mapNpcAIType["移动炮台"]								= ENpcAIType_Cannon;
		mapNpcAIType["怪物卡"]									= ENpcAIType_MonsterCard;
		mapNpcAIType["宠物"]									= ENpcAIType_Pet;
		mapNpcAIType["Boss小弟"]								= ENpcAIType_BossCortege;
		mapNpcAIType["Boss主动支持巡逻"]						= ENpcAIType_BossActive;
		mapNpcAIType["Boss被动支持巡逻"]						= ENpcAIType_BossPassivity;
		mapNpcAIType["剧场Npc"]									= ENpcAIType_Theater;
		mapNpcAIType["被打不还手不转向非任务Npc"]				= ENpcAIType_NotFightNonTask;
		mapNpcAIType["运输车"]									= ENpcAIType_Truck;
		mapNpcAIType["逃跑AI"]									= ENpcAIType_Escape;
		mapNpcAIType["跟随宠物"]								= ENpcAIType_LittlePet;
	}
	typedef map<string, ENpcAIType> AITypeMapped_T;
	typedef AITypeMapped_T::const_iterator  mapNpcAIType_itr;
	AITypeMapped_T mapNpcAIType;
};
static AITypeMap sNpcAIType; 

struct AINameByTypeMap
{
	AINameByTypeMap()
	{
		mapNpcAINameByNpcAIType[ENpcAIType_NormalPassivityPatrol]					= "普通怪被动支持巡逻";
		mapNpcAINameByNpcAIType[ENpcAIType_NormalInitiativePatrolNoFefer]			= "普通怪主动支持巡逻";
		mapNpcAINameByNpcAIType[ENpcAIType_NormalInitiativePatrolNoFeferCanSeeNpc]	= "普通怪主动支持巡逻可见Npc";
		mapNpcAINameByNpcAIType[ENpcAIType_Task]									= "任务NPC支持巡逻";
		mapNpcAINameByNpcAIType[ENpcAIType_PassivityCopIt]							= "被打不反击";
		mapNpcAINameByNpcAIType[ENpcAIType_Totem]									= "图腾";
		mapNpcAINameByNpcAIType[ENpcAIType_Servant]									= "召唤兽";
		mapNpcAINameByNpcAIType[ENpcAIType_VirtualNpc]								= "虚拟Npc支持巡逻";	
		mapNpcAINameByNpcAIType[ENpcAIType_GroupMemberPassivity]					= "群组成员被动怪";
		mapNpcAINameByNpcAIType[ENpcAIType_GroupMemberInitiative]					= "群组成员主动怪";
		mapNpcAINameByNpcAIType[ENpcAIType_TowerAttack]								= "箭塔AI";
		mapNpcAINameByNpcAIType[ENpcAIType_BattleHorse]								= "战斗坐骑";
		mapNpcAINameByNpcAIType[ENpcAIType_Building]								= "物件Npc";
		mapNpcAINameByNpcAIType[ENpcAIType_OrdnanceMonster]							= "军械兽";
		mapNpcAINameByNpcAIType[ENpcAIType_MagicBuilding]							= "魔法阵";
		mapNpcAINameByNpcAIType[ENpcAIType_Cannon]									= "移动炮台";
		mapNpcAINameByNpcAIType[ENpcAIType_MonsterCard]								= "怪物卡";
		mapNpcAINameByNpcAIType[ENpcAIType_Pet]										= "宠物";
		mapNpcAINameByNpcAIType[ENpcAIType_BossCortege]								= "Boss小弟";
		mapNpcAINameByNpcAIType[ENpcAIType_BossActive]								= "Boss主动支持巡逻";
		mapNpcAINameByNpcAIType[ENpcAIType_BossPassivity]							= "Boss被动支持巡逻";
		mapNpcAINameByNpcAIType[ENpcAIType_Theater]									= "剧场Npc";
		mapNpcAINameByNpcAIType[ENpcAIType_NotFightNonTask]							= "被打不还手不转向非任务Npc";
		mapNpcAINameByNpcAIType[ENpcAIType_Truck]									= "运输车";
		mapNpcAINameByNpcAIType[ENpcAIType_Escape]									= "逃跑AI";
		mapNpcAINameByNpcAIType[ENpcAIType_LittlePet]								= "跟随宠物";
	}
	typedef map<ENpcAIType, string> AITypeMapped_T;
	typedef AITypeMapped_T::const_iterator  mapNpcAIType_itr;
	AITypeMapped_T mapNpcAINameByNpcAIType;
};
static AINameByTypeMap sNpcAINameByNpcAIType; 
