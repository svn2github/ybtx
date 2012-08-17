#pragma once
#include "CMagicCondServer.h"
#include "CFighterAgileInfoServer.h"
#include "CFighterCalInfo.h"
#include "TFighterCtrlInfo.h"
#include "CValueMagicCondServer.h"

inline static float GetRandFloat()
{
	return rand() / float(RAND_MAX);
}

template<typename PropertyType,PropertyType CFighterAgileInfoServer::*pProperty>
class TAgileValueLesserCond
	:public CValueMagicCondServer
{
public:
	uint32 Test(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,const CFighterDictator* pFrom,const CFighterDictator* pTo) const;
	CValueData MagicCondCalc(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,const CFighterDictator* pFrom,const CFighterDictator* pTo) const;
	bool MagicCondExec(CSkillInstServer* pSkillInst,const CFighterDictator* pFrom,const CFighterDictator* pTo,const CValueData& Value) const;
}; 


template<typename PropertyType,PropertyType CFighterAgileInfoServer::*pProperty>
class TAgileValueGreaterOrEqualCond
	:public TAgileValueLesserCond<PropertyType,pProperty>
{
private:
	typedef TAgileValueLesserCond<PropertyType,pProperty>	Parent_t;
public:
	uint32 Test(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,const CFighterDictator* pFrom,const CFighterDictator* pTo) const;
	CValueData MagicCondCalc(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,const CFighterDictator* pFrom,const CFighterDictator* pTo) const;
	bool MagicCondExec(CSkillInstServer* pSkillInst,const CFighterDictator* pFrom,const CFighterDictator* pTo,const CValueData& Value) const;
}; 

template<typename PropertyType,PropertyType CFighterAgileInfoServer::*pProperty>
class TAgileMaxValueLesserCond
	:public CValueMagicCondServer
{
public:
	uint32 Test(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,const CFighterDictator* pFrom,const CFighterDictator* pTo) const;
	CValueData MagicCondCalc(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,const CFighterDictator* pFrom,const CFighterDictator* pTo) const;
	bool MagicCondExec(CSkillInstServer* pSkillInst,const CFighterDictator* pFrom,const CFighterDictator* pTo,const CValueData& Value) const;
}; 


template<typename PropertyType,PropertyType CFighterAgileInfoServer::*pProperty>
class TAgileMaxValueGreaterOrEqualCond
	:public TAgileMaxValueLesserCond<PropertyType,pProperty>
{
private:
	typedef TAgileMaxValueLesserCond<PropertyType,pProperty>	Parent_t;
public:
	uint32 Test(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,const CFighterDictator* pFrom,const CFighterDictator* pTo) const;
	CValueData MagicCondCalc(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,const CFighterDictator* pFrom,const CFighterDictator* pTo) const;
	bool MagicCondExec(CSkillInstServer* pSkillInst,const CFighterDictator* pFrom,const CFighterDictator* pTo,const CValueData& Value) const;
}; 


template<typename PropertyType,PropertyType CFighterCalInfo::*pProperty>
class TCalValueLesserCond
	:public CValueMagicCondServer
{
public:
	uint32 Test(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,const CFighterDictator* pFrom,const CFighterDictator* pTo) const;
	CValueData MagicCondCalc(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,const CFighterDictator* pFrom,const CFighterDictator* pTo) const;
	bool MagicCondExec(CSkillInstServer* pSkillInst,const CFighterDictator* pFrom,const CFighterDictator* pTo,const CValueData& Value) const;
}; 


template<typename PropertyType,PropertyType CFighterCalInfo::*pProperty>
class TCalValueGreaterOrEqualCond
	:public TCalValueLesserCond<PropertyType,pProperty>
{
private:
	typedef TCalValueLesserCond<PropertyType,pProperty>	Parent_t;
public:
	uint32 Test(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,const CFighterDictator* pFrom,const CFighterDictator* pTo) const;
	CValueData MagicCondCalc(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,const CFighterDictator* pFrom,const CFighterDictator* pTo) const;
	bool MagicCondExec(CSkillInstServer* pSkillInst,const CFighterDictator* pFrom,const CFighterDictator* pTo,const CValueData& Value) const;
}; 

class CProbabilityLesserCond
	:public CValueMagicCondServer
{
public:
	uint32 Test(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,const CFighterDictator* pFrom,const CFighterDictator* pTo) const;
	CValueData MagicCondCalc(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,const CFighterDictator* pFrom,const CFighterDictator* pTo) const;
	bool MagicCondExec(CSkillInstServer* pSkillInst,const CFighterDictator* pFrom,const CFighterDictator* pTo,const CValueData& Value) const;
}; 


class CDistOfAttackerAndTargetLesserCond
	:public CValueMagicCondServer
{
public:
	uint32 Test(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,const CFighterDictator* pFrom,const CFighterDictator* pTo) const;
	CValueData MagicCondCalc(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,const CFighterDictator* pFrom,const CFighterDictator* pTo) const;
	bool MagicCondExec(CSkillInstServer* pSkillInst,const CFighterDictator* pFrom,const CFighterDictator* pTo,const CValueData& Value) const;
}; 

class CDistOfAttackerAndTargetGreaterOrEqualCond
	:public CValueMagicCondServer
{
public:
	uint32 Test(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,const CFighterDictator* pFrom,const CFighterDictator* pTo) const;
	CValueData MagicCondCalc(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,const CFighterDictator* pFrom,const CFighterDictator* pTo) const;
	bool MagicCondExec(CSkillInstServer* pSkillInst,const CFighterDictator* pFrom,const CFighterDictator* pTo,const CValueData& Value) const;
}; 

class CDistOfMagicAndTargetLesserCond
	:public CValueMagicCondServer
{
public:
	uint32 Test(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,const CFighterDictator* pFrom,const CFighterDictator* pTo) const;
	CValueData MagicCondCalc(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,const CFighterDictator* pFrom,const CFighterDictator* pTo) const;
	bool MagicCondExec(CSkillInstServer* pSkillInst,const CFighterDictator* pFrom,const CFighterDictator* pTo,const CValueData& Value) const;
}; 

class CDistOfMagicAndTargetGreaterOrEqualCond
	:public CValueMagicCondServer
{
public:
	uint32 Test(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,const CFighterDictator* pFrom,const CFighterDictator* pTo) const;
	CValueData MagicCondCalc(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,const CFighterDictator* pFrom,const CFighterDictator* pTo) const;
	bool MagicCondExec(CSkillInstServer* pSkillInst,const CFighterDictator* pFrom,const CFighterDictator* pTo,const CValueData& Value) const;
}; 

//定义所有AgileValueMOP
#define DefineAgileValueMCOND(Name) \
	typedef TAgileValueLesserCond<C##Name##Server,&CFighterAgileInfoServer::m_##Name>					C##Name##LesserMCOND;\
	typedef TAgileValueGreaterOrEqualCond<C##Name##Server,&CFighterAgileInfoServer::m_##Name>			C##Name##GreaterOrEqualMCOND;\
	typedef TAgileMaxValueLesserCond<C##Name##Server,&CFighterAgileInfoServer::m_##Name>				CMax##Name##LesserMCOND;\
	typedef TAgileMaxValueGreaterOrEqualCond<C##Name##Server,&CFighterAgileInfoServer::m_##Name>		CMax##Name##GreaterOrEqualMCOND;

DefineAgileValueMCOND(HealthPoint)
DefineAgileValueMCOND(ManaPoint)
DefineAgileValueMCOND(ComboPoint)
DefineAgileValueMCOND(EnergyPoint)
DefineAgileValueMCOND(RagePoint)

#undef DefineAgileValueMCOND

//定义所有CalcValueMOP
#define DefineCalValueMCOND(Name) \
	typedef TCalValueLesserCond<C##Name,&CFighterCalInfo::m_##Name>					C##Name##LesserMCOND;\
	typedef TCalValueGreaterOrEqualCond<C##Name,&CFighterCalInfo::m_##Name>			C##Name##GreaterOrEqualMCOND;

DefineCalValueMCOND(Defence)
DefineCalValueMCOND(NatureResistanceValue)
DefineCalValueMCOND(DestructionResistanceValue)
DefineCalValueMCOND(EvilResistanceValue)
#undef DefineCalValueMCOND

