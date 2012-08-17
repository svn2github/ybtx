#pragma once
#include "CMagicCondClient.h"
#include "CFighterAgileInfoClient.h"
#include "CFighterCalInfo.h"
#include "TFighterCtrlInfo.h"

template<typename PropertyType,PropertyType CFighterAgileInfo::*pProperty>
class TAgileValueLesserCond
	:public CMagicCondClient
{
public:
	uint32 Test(uint32 SkillLevel,const CCfgCalc& Arg,const CFighterDirector* pFighter) const;
}; 

template<typename PropertyType,PropertyType CFighterAgileInfo::*pProperty>
class TAgileValueGreaterOrEqualCond
	:public TAgileValueLesserCond<PropertyType,pProperty>
{
private:
	typedef TAgileValueLesserCond<PropertyType,pProperty>	Parent_t;
public:
	uint32 Test(uint32 SkillLevel,const CCfgCalc& Arg,const CFighterDirector* pFighter) const;
};

class CProbabilityLesserCond
	:public CMagicCondClient
{
public:
	uint32 Test(uint32 SkillLevel,const CCfgCalc& Arg,const CFighterDirector* pFighter) const;
};

//定义所有AgileValueMOP
#define DefineAgileValueMCOND(Name) \
	typedef TAgileValueLesserCond<C##Name,&CFighterAgileInfo::m_##Name>					C##Name##LesserMCOND;\
	typedef TAgileValueGreaterOrEqualCond<C##Name,&CFighterAgileInfo::m_##Name>			C##Name##GreaterOrEqualMCOND;

DefineAgileValueMCOND(HealthPoint)
DefineAgileValueMCOND(ManaPoint)
DefineAgileValueMCOND(EnergyPoint)
DefineAgileValueMCOND(RagePoint)
DefineAgileValueMCOND(ComboPoint)

#undef DefineAgileValueMCOND
