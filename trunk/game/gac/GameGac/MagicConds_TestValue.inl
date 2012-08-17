#pragma once
#include "MagicConds_TestValue.h"
#include "CFighterDirector.h"
#include "CMagicOpArg.h"
#include "TPropertyHolder_inl.inl"
#include "CCfgCalc.inl"

class CSkillInstNull;

template<typename PropertyType, PropertyType CFighterAgileInfo::*pProperty>
uint32 TAgileValueLesserCond<PropertyType, pProperty>::
Test(uint32 SkillLevel, const CCfgCalc& Arg, const CFighterDirector* pFighter) const
{
	typedef typename PropertyType::Value_t ValueType;
	ValueType ArgValue = CMagicOpArgGetter<ValueType>::Acquire(&Arg,pFighter, (CSkillInstNull*)NULL, SkillLevel);
	
	string sRawName;
#ifdef _WIN32
	sRawName=typeid(PropertyType).raw_name();
	sRawName = sRawName.substr(4,sRawName.size()-6);
#else
	sRawName=typeid(PropertyType).name();
	sRawName = sRawName.substr(2,sRawName.size()-2);
#endif
	EAttackType eAttackType = pFighter->GetCurrentSkillAttackType();
	if(sRawName.compare("CManaPoint") == 0)
	{
		ArgValue = (int32)(ArgValue * pFighter->Get(&CFighterSyncToDirectorCalInfo::m_MPConsumeRate));
		if (eAttackType == eATT_Nature)
		{
			ArgValue = (int32)(ArgValue * pFighter->Get(&CFighterSyncToDirectorCalInfo::m_NatureMPConsumeRate));
		}
		else if (eAttackType == eATT_Destroy)
		{
			ArgValue = (int32)(ArgValue * pFighter->Get(&CFighterSyncToDirectorCalInfo::m_DestructionMPConsumeRate));
		}
		else if (eAttackType == eATT_Evil)
		{
			ArgValue = (int32)(ArgValue * pFighter->Get(&CFighterSyncToDirectorCalInfo::m_EvilMPConsumeRate));
		}
	}
	else if (sRawName.compare("CRagePoint") == 0)
	{	
		ArgValue = (int32)(ArgValue * pFighter->Get(&CFighterSyncToDirectorCalInfo::m_RPConsumeRate));
	}
	else if (sRawName.compare("CEnergyPoint") == 0)
	{
		ArgValue = (int32)(ArgValue * pFighter->Get(&CFighterSyncToDirectorCalInfo::m_EPConsumeRate));
	}

	if ((pFighter->*pProperty).LimitGet()<ArgValue && pFighter->CppIsAlive())
		return eDSR_Success;
	else
		return eDSR_HealthPointIsGreater;
}

template<typename PropertyType, PropertyType CFighterAgileInfo::*pProperty>
uint32 TAgileValueGreaterOrEqualCond<PropertyType ,pProperty>::
Test(uint32 SkillLevel, const CCfgCalc& Arg, const CFighterDirector* pFighter) const
{
	if (Parent_t::Test(SkillLevel, Arg, pFighter) != eDSR_Success)
		return eDSR_Success;
	else
	{
		string sRawName;
#ifdef _WIN32
		sRawName=typeid(PropertyType).raw_name();
		sRawName = sRawName.substr(4,sRawName.size()-6);
#else
		sRawName=typeid(PropertyType).name();
		sRawName = sRawName.substr(2,sRawName.size()-2);
#endif

		if(sRawName.compare("CHealthPoint") == 0)
			return eDSR_HealthPointIsLesser;
		else if(sRawName.compare("CManaPoint") == 0)
			return eDSR_ManaPointIsLesser;
		else if(sRawName.compare("CEnergyPoint") == 0)
			return eDSR_EnergyPointIsLesser;
		else if(sRawName.compare("CRagePoint") == 0)
			return eDSR_RagePointIsLesser;
		else if(sRawName.compare("CComboPoint") == 0)
			return eDSR_ComboPointIsLesser;
		else
			return eDSR_Fail;
	}	
}

uint32 CProbabilityLesserCond::Test(uint32 SkillLevel, const CCfgCalc& Arg, const CFighterDirector* pFighter) const
{
	if (Arg.GetDblValue(pFighter) > 0)
		return eDSR_Success;
	else
		return eDSR_Fail;
}

//对所有的AgileValueMCOND的类模板进行实例化
#define InstantiateAgileValueMCOND(Name) \
	template class TAgileValueLesserCond<C##Name,&CFighterAgileInfo::m_##Name>;\
	template class TAgileValueGreaterOrEqualCond<C##Name,&CFighterAgileInfo::m_##Name>;

InstantiateAgileValueMCOND(HealthPoint)
InstantiateAgileValueMCOND(ManaPoint)
InstantiateAgileValueMCOND(ComboPoint)
InstantiateAgileValueMCOND(RagePoint)
InstantiateAgileValueMCOND(EnergyPoint)

#undef InstantiateAgileValueMCOND
