#pragma once
#include "CCfgCalc.inl"
#include "CFighterMediator.h"

template<>
double CCfgCalc::GetDblValue<CFighterMediator>(const CFighterMediator* pFighter, uint32 index) const
{
	return GetDblValue(static_cast<const CFighterDictator*>(pFighter), index);
}

template<>
int32 CCfgCalc::GetIntValue<CFighterMediator>(const CFighterMediator* pFighter, uint32 index) const
{
	return GetIntValue(static_cast<const CFighterDictator*>(pFighter), index);
}
