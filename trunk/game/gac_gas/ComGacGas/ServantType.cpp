#include "stdafx.h"
#include "ServantType.h"

bool ServantType::BeServantType(ENpcType eNpcType)
{
	switch(eNpcType)
	{
	case ENpcType_Totem:			//图腾
	case ENpcType_Pet:				//宠物
	case ENpcType_Summon:			//被动类型召唤兽			
	case ENpcType_BattleHorse:		//战斗坐骑
	case ENpcType_OrdnanceMonster:	//军械兽
	case ENpcType_MagicBuilding:	//魔法阵
	case ENpcType_Cannon:			//移动炮台
	case ENpcType_MonsterCard:		//怪物卡
	case ENpcType_Shadow:			//分身
	case ENpcType_Truck:			//运输车
	case ENpcType_BossCortege:		//Boss小弟主动
	case ENpcType_QuestBeckonNpc:	//任务召唤Npc	
	case ENpcType_LittlePet:		//跟随宠物
	case ENpcType_NotCtrlSummon:	//不受控召唤兽
		return true;
	default :
		return false;
	}
}
