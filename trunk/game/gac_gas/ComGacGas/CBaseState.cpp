#include "stdafx.h"
#include "CBaseState.h"
#include "CCfgColChecker.inl"
#include "TSqrAllocator.inl"

CBaseStateCfg::MapBaseStateCfgByName CBaseStateCfg::m_mapGlobalCfgByName;
CBaseStateCfg::MapBaseStateCfgById CBaseStateCfg::m_mapGlobalCfgById;
MapIconCancelCond CBaseStateCfg::ms_mapIconCancelCond;
MapDecreaseStateType CBaseStateCfg::ms_mapDecreaseType;
MapStringVector CBaseStateCfg::ms_vecTypeName;
bool CBaseStateCfg::__init = CBaseStateCfg::InitMap();

bool CBaseStateCfg::InitMap()
{
	using namespace CfgChk;
	CreateMap(ms_mapIconCancelCond, eICC_End, ("不可"), ("自己"), ("所有"));
	CreateVector(ms_vecTypeName, eSGT_End, ("不明状态"), ("魔法状态"), ("触发器状态"), ("伤害变更状态"), ("积累触发状态"), ("特殊状态"));

	ms_mapDecreaseType["坐骑"] = eDST_Riding;
	ms_mapDecreaseType["正面"] = eDST_Increase;
	ms_mapDecreaseType["假死"] = eDST_FeignDeath;
	//ms_mapDecreaseType["正面进战斗"] = eDST_IncreaseTouchBattleState;
	ms_mapDecreaseType["控制"] = eDST_Control;
	ms_mapDecreaseType["定身"] = eDST_Pause;
	ms_mapDecreaseType["减速"] = eDST_Crippling;
	ms_mapDecreaseType["负面"] = eDST_Debuff;
	ms_mapDecreaseType["伤害"] = eDST_DOT;
	ms_mapDecreaseType["特殊"] = eDST_Special;

	return true;
}

CBaseStateCfg*	CBaseStateCfg::GetByGlobalName(const string& name)
{
	CBaseStateCfg::MapBaseStateCfgByName::iterator mapCfgItr;
	mapCfgItr = m_mapGlobalCfgByName.find(name);
	if (mapCfgItr == m_mapGlobalCfgByName.end())
	{
		stringstream ExpStr;
		ExpStr << "状态名不存在";
		CfgChk::GenExpInfo(ExpStr.str(), name);
		return NULL;
	}
	return mapCfgItr->second;
}

CBaseStateCfg*	CBaseStateCfg::GetByGlobalNameNoExp(const string& name)
{
	CBaseStateCfg::MapBaseStateCfgByName::iterator mapCfgItr;
	mapCfgItr = m_mapGlobalCfgByName.find(name);
	if (mapCfgItr == m_mapGlobalCfgByName.end())
	{
		stringstream ExpStr;
		ExpStr << "状态名不存在";
		return NULL;
	}
	return mapCfgItr->second;
}


CBaseStateCfg*	CBaseStateCfg::GetByGlobalId(uint32 uId)
{
	CBaseStateCfg::MapBaseStateCfgById::iterator mapCfgItr;
	mapCfgItr = m_mapGlobalCfgById.find(uId);
	if (mapCfgItr == m_mapGlobalCfgById.end())
	{
		stringstream ExpStr;
		ExpStr << "状态ID不存在";
		CfgChk::GenExpInfo(ExpStr.str(), uId);
		//return NULL;
	}
	return mapCfgItr->second;
}

CBaseStateCfg::CBaseStateCfg(EStateGlobalType type)
:m_eGlobalType(type)
{
}

CBaseStateCfg::CBaseStateCfg(const CBaseStateCfg& cfg)
:m_eGlobalType(cfg.m_eGlobalType)
,m_fScale(cfg.m_fScale)
,m_fScaleTime(cfg.m_fScaleTime)
{
}

