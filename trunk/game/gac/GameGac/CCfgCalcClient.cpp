#include "stdafx.h"
#include "CCfgCalc.h"
#include "CCfgCalcFunction.h"
#include "CFighterDirector.h"
#include "CTalent.h"
#include "CNormalAttackClient.h"

enum
{
	eMAID_AgileValueCount = ePID_AgileValueCount * ePFT_Count,
	eMAID_MainHandWeaponInterval = ePID_MainHandWeaponInterval * ePFT_Count + ePFT_Value,
	eMAID_AssistantWeaponInterval = ePID_AssistantWeaponInterval * ePFT_Count + ePFT_Value,
};

template<>
pair<bool, double> CExpression::GetAttrValue<CFighterDirector>(uint32 uVarID, const CFighterDirector* pFighter)
{
	if(uVarID < eMAID_AgileValueCount)
	{
		return make_pair(true, double(pFighter->CppGetPropertyValue(uVarID)));
	}
	else switch (uVarID)
	{
		case eMAID_MainHandWeaponInterval:
			return make_pair(true,pFighter->GetMainHandWeaponInterval());
		case eMAID_AssistantWeaponInterval:
			return make_pair(true,pFighter->GetAssistantWeaponInterval());
		case eMAID_FighterLevel:
			return make_pair(true, pFighter->CppGetLevel());
		case eMAID_MainHandWeaponIsSingle:
			return make_pair(true, pFighter->GetCtrlState(eFCS_ForbitUseWeapon)?true:IsSingleHandsWeapon(pFighter->GetNormalAttack()->GetMHWeaponType()));
		default:
			return make_pair(false, 0.0);
	}
}

template<>
pair<bool, double> CExpression::GetTempValue<CFighterDirector>(UUnitValue pVarStr,const CFighterDirector* pFighter)
{
	return make_pair(false, 0.0);
}

template<>
pair<bool, double> CExpression::GetTalentValue<CFighterDirector>(const string& varName, const CFighterDirector* pFighter)
{
	CTalentHolder* pTalentHolder = pFighter->GetTalentHolder();
	CTalentHolder::MapPoint::iterator itr = pTalentHolder->m_mapPoint.find(varName);
	if(itr != pTalentHolder->m_mapPoint.end())
	{
		return make_pair(true, double(itr->second));
	}
	else
	{
		return make_pair(false, 0.0);
	}
}

template<>
pair<bool, double> CExpression::GetMasterTalentValue<CFighterDirector>(const string& varName,const CFighterDirector* pFighter)
{
	return make_pair(false, 0.0);
}

template<>
pair<bool, double> CExpression::GetServantAttrValue<CFighterDirector>(uint32 uVarID,const CFighterDirector* pFighter)
{
	return make_pair(false, 0.0);
}
template<>
pair<bool, double> CExpression::GetMasterAttrValue<CFighterDirector>(uint32 uVarID,const CFighterDirector* pFighter)
{
	return make_pair(false, 0.0);
}

template<>
pair<bool, double> CExpression::GetMasterTempValue<CFighterDirector>(UUnitValue pVarStr,const CFighterDirector* pFighter)
{
	return make_pair(false, 0.0);
}

template<>
const CFighterDirector* CExpression::GetCalcFighter<CFighterDirector>(const CFighterDirector* pFighter[], ECalcTargetType eTargetType)
{
	if(eTargetType == eCTT_Master)
	{
		return NULL;	
	}
	else
	{
		return pFighter[eTargetType];
	}
}



