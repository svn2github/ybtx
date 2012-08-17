#pragma once

#include "BaseTypes.h"
#include "ExpHelper.h"
#include "FightDef.h"
#include "CCfgCalc.h"


template<typename ValueType>
class CMagicOpArgGetter
{
};

template<>
class CMagicOpArgGetter<float>
{
public:
	template<typename FighterInfoType, typename SkillInstType>
	static float Acquire(const CCfgCalc* pArg,FighterInfoType* pInfo,SkillInstType* pInst,uint32 uMOPLevel)
	{
		return (float)pArg->GetDblValue(pInfo,pInst,uMOPLevel);
	}
};

template<>
class CMagicOpArgGetter<uint32>
{
public:
	template<typename FighterInfoType,typename SkillInstType>
	static uint32 Acquire(const CCfgCalc* pArg, FighterInfoType* pInfo,SkillInstType* pInst,uint32 uMOPLevel)
	{
		return (uint32)pArg->GetIntValue(pInfo,uMOPLevel);
	}
};

template<>
class CMagicOpArgGetter<int32>
{
public:
	template<typename FighterInfoType,typename SkillInstType>
	static uint32 Acquire(const CCfgCalc* pArg, FighterInfoType* pInfo,SkillInstType* pInst,uint32 uMOPLevel)
	{
		return (uint32)pArg->GetIntValue(pInfo,uMOPLevel);
	}
};
