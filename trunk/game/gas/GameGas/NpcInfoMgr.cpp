#include "stdafx.h"
#include "NpcInfoMgr.h"
#include "NpcInfoDefs.h"
#include "CNpcServerBaseData.h"
#include "ExpHelper.h"
#include "CCharacterDictator.h"
#include "ServantType.h"

bool NpcInfoMgr::BeFightNpc(const TCHAR* szNpcName)
{
	const CNpcServerBaseData* pData = CNpcServerBaseDataMgr::GetInst()->GetEntity(szNpcName);
	if (NULL == pData)
	{
		const string& sName = szNpcName;
		const string& sErrorInfo = "Npc: 【" + sName + "】在Npc_Common.dif表中已经不存在！不过在NpcRes_Common.dif表中还存在！请策划及时清除！";
		//	CfgErr(sErrorInfo.c_str());
		cout<<sErrorInfo<<endl;
		return true;
	}
	ENpcAIType eAIType = pData->m_eAIType;
	switch(eAIType)
	{
	case ENpcAIType_NormalPassivityPatrol:						//普通怪被动支持巡逻
	case ENpcAIType_NormalInitiativePatrolNoFefer:				//普通怪主动支持巡逻
	case ENpcAIType_NormalInitiativePatrolNoFeferCanSeeNpc:		//普通怪主动支持巡逻可见Npc
	case ENpcAIType_Servant:									//召唤兽
	case ENpcAIType_GroupMemberPassivity:						//群组成员被动怪
	case ENpcAIType_GroupMemberInitiative:						//群组成员主动怪
	case ENpcAIType_TowerAttack:								//箭塔AI
	case ENpcAIType_BattleHorse:								//战斗坐骑
	case ENpcAIType_OrdnanceMonster:							//军械兽
	case ENpcAIType_MagicBuilding:								//魔法阵
	case ENpcAIType_Cannon:										//移动炮台
	case ENpcAIType_MonsterCard:								//怪物卡
	case ENpcAIType_BossCortege:								//Boss小弟
	case ENpcAIType_BossActive:									//Boss主动支持巡逻
	case ENpcAIType_BossPassivity:								//Boss被动支持巡逻
		return true;
	default:
		return false;
	}
	return false;
}


bool NpcInfoMgr::BeActiveNpc(ENpcAIType eNpcAIType)
{
	switch(eNpcAIType)
	{
	case ENpcAIType_NormalInitiativePatrolNoFefer:
	case ENpcAIType_NormalInitiativePatrolNoFeferCanSeeNpc: 
	case ENpcAIType_MagicBuilding:
	case ENpcAIType_MonsterCard:
	case ENpcAIType_GroupMemberInitiative:
	case ENpcAIType_BossActive:
		return true;
	default:
		return false;
	}
}

const TCHAR* NpcInfoMgr::GetAINameByAIType(ENpcAIType eAIType)
{
	return sNpcAINameByNpcAIType.mapNpcAINameByNpcAIType[eAIType].c_str();
}

ENpcAIType NpcInfoMgr::GetAITypeByAIName(const TCHAR* szAIName)
{
	return sNpcAIType.mapNpcAIType[szAIName];
}

const TCHAR* NpcInfoMgr::GetTypeNameByType(ENpcType eNpcType)
{
	return sNpcTypeMapName.mNpcTypeMapName[eNpcType].c_str();
}

ENpcType NpcInfoMgr::GetTypeByTypeName(const TCHAR* szTypeName)
{
	return NpcType.mapNpcTypeMap[szTypeName];
}

bool NpcInfoMgr::CanTakeOnlyOne(ENpcType eNpcType)
{
	switch(eNpcType)
	{
	case ENpcType_Pet:					//宠物
	case ENpcType_Summon:				//被动类型召唤兽			
	case ENpcType_BattleHorse:			//战斗坐骑
	case ENpcType_OrdnanceMonster:		//军械兽
	case ENpcType_Cannon:				//移动炮台
		//case ENpcType_MonsterCard:			//怪物卡
	case ENpcType_Truck:			//运输车
	case ENpcType_QuestBeckonNpc:		//任务召唤Npc	
	case ENpcType_LittlePet:
	case ENpcType_NotCtrlSummon:
		return true;
	default :
		return false;
	}
}

bool NpcInfoMgr::BeServantType(ENpcType eNpcType)
{
	return ServantType::BeServantType(eNpcType);
}

bool NpcInfoMgr::BeServantTypeNeedToSync(ENpcType eNpcType)
{
	switch(eNpcType)
	{
	case ENpcType_Pet:				//宠物
	case ENpcType_Summon:			//被动类型召唤兽			
	case ENpcType_BattleHorse:		//战斗坐骑
	case ENpcType_Truck:			//运输车
		return true;
	default :
		return false;
	}
}

bool NpcInfoMgr::BeFollowMaster(ENpcType eNpcType)
{
	switch (eNpcType)
	{
	case ENpcType_Pet:				//宠物
	case ENpcType_Summon:			//被动类型召唤兽			
	case ENpcType_OrdnanceMonster:	//军械兽
	case ENpcType_Cannon:			//移动炮台
	case ENpcType_MonsterCard:		//怪物卡
	case ENpcType_Shadow:			//分身	
		//case ENpcType_BossCortegeReBorn:	//Boss小弟被动
	case ENpcType_BossCortege:		//Boss小弟主动
	case ENpcType_QuestBeckonNpc:	//任务召唤Npc	
	case ENpcType_NotCtrlSummon:
		return true;
	default :
		return false;
	}
}

//主人死亡后需要被删除的Npc类型
bool NpcInfoMgr::BeKillServantByMasterDestory(ENpcType eNpcType)
{
	switch(eNpcType)
	{
	case ENpcType_Totem:			//图腾
	case ENpcType_Pet:				//宠物
	case ENpcType_Summon:			//被动类型召唤兽			
	case ENpcType_BattleHorse:		//战斗坐骑
		//case ENpcType_OrdnanceMonster:	//军械兽
	case ENpcType_MagicBuilding:	//魔法阵
		//case ENpcType_Cannon:			//移动炮台
	case ENpcType_MonsterCard:		//怪物卡
	case ENpcType_Shadow:			//分身
	case ENpcType_QuestBeckonNpc:	//任务召唤Npc
	case ENpcType_LittlePet:		//小宠物
	case ENpcType_NotCtrlSummon:
		return true;
	default :
		return false;
	}
}

ENpcEnmityType NpcInfoMgr::GetEnmityMgrType(ENpcType eNpcType)
{
	switch(eNpcType)
	{
	case ENpcType_Normal:			//普通类型Npc：任务Npc，被动怪，主动怪，箭塔等等
		//case ENpcType_BossCortegeReBorn:		//Boss小弟战斗重生
	case ENpcType_BossCortege:	//Boss小弟退出战斗重生
	case ENpcType_Truck:		//运输车
	case ENpcType_AttackNearest:		//攻击最近
	case ENpcType_Totem:				//图腾
	case ENpcType_Pet:				//宠物
	case ENpcType_QuestBeckonNpc:	//任务召唤Npc	
	case ENpcType_Summon:			//被动类型召唤兽			
	case ENpcType_BattleHorse:		//战斗坐骑
	case ENpcType_OrdnanceMonster:	//军械兽
	case ENpcType_MagicBuilding:	//魔法阵
	case ENpcType_Cannon:			//移动炮台
	case ENpcType_MonsterCard:		//怪物卡
	case ENpcType_Shadow:			//分身
	case ENpcType_LittlePet:		//小宠物
	case ENpcType_NotCtrlSummon:
		return ENpcEnmityType_Nomal;
	case ENpcType_Dummy:	
		return ENpcEnmityType_Dummy;
	case ENpcType_Member:
		return ENpcEnmityType_Member;
	default:
		return ENpcEnmityType_None;
	}
}

bool NpcInfoMgr::BeFightNpc(ENpcAIType eNpcAIType)
{
	switch(eNpcAIType)
	{
	case ENpcAIType_Task:										//任务Npc
	case ENpcAIType_PassivityCopIt:								//被打不反击
	case ENpcAIType_VirtualNpc:									//虚拟Npc支持巡逻
	case ENpcAIType_Building:									//物件Npc(建筑，箱子，刷怪器等等)
	case ENpcAIType_Pet:										//宠物
	case ENpcAIType_Theater:									//剧场Npc
	case ENpcAIType_NotFightNonTask:							//被打不还手不转向非任务Npc
	case ENpcAIType_Escape:										//逃跑AI
		return false;
	default:
		return true;
	}
}

bool NpcInfoMgr::CanBeChangeAI(ENpcAIType eAIType, EClass eClass, ECamp eCamp)
{
	switch (eAIType)
	{
	case ENpcAIType_None:
	case ENpcAIType_Task:
	case ENpcAIType_Totem:
	case ENpcAIType_PassivityCopIt:
	case ENpcAIType_VirtualNpc:
	case ENpcAIType_GroupMemberPassivity:
	case ENpcAIType_GroupMemberInitiative:
	case ENpcAIType_TowerAttack:
	case ENpcAIType_BattleHorse:
	case ENpcAIType_Building:
	case ENpcAIType_OrdnanceMonster:
	case ENpcAIType_MagicBuilding:
	case ENpcAIType_Cannon:
	case ENpcAIType_MonsterCard:
	case ENpcAIType_Pet:
	case ENpcAIType_Theater:
	case ENpcAIType_NotFightNonTask:
		return false;
	default:
		break;
	}
	switch(eClass)
	{
	case eCL_006:
	case eCL_012:
	case eCL_014:
	case eCL_018:
	case eCL_032:
	case eCL_036:
	case eCL_037:
	case eCL_040:
	case eCL_041:
	case eCL_042:
	case eCL_043:
		return false;
	default:
		break;
	}

	//只有1,2,3,5阵营的怪物才能被抓
	switch(eCamp)
	{
	case eCamp_Monster:
	case eCamp_AmyEmpire:
	case eCamp_WestEmpire:
	case eCamp_DevilIsland:
		break;

	default:
		return false;
	}

	return true;
}

bool NpcInfoMgr::CanChangeCamp(ENpcType eType)
{
	switch(eType)
	{
	case ENpcType_LittlePet:
		return false;
		break;
	default:
		return true;
	}
}

