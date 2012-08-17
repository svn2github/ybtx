#include "stdafx.h"
#include "CCfgCalc.inl"
#include "CCfgCalcFunction.h"
#include "CFighterDictator.h"
#include "CFighterMediator.h"
#include "CTempVarServer.h"
#include "MagicOps_Damage.h"
#include "CCharacterDictator.h"
#include "IFighterServerHandler.h"
#include "TPropertyHolder.inl"
#include "CTalentEffServer.h"
#include "NormalAttack.h"

template<>
pair<bool, double> CExpression::GetTempValue<CFighterDictator>(UUnitValue uVarPtr, const CFighterDictator* pFighter)
{
	return make_pair(true, (double)pFighter->GetTempVarMgr()->GetVarValue(uVarPtr));	//这种写法必须把GetTempValue()放在所有GetValue()之后
}

template<>
pair<bool, double> CExpression::GetAttrValue<CFighterDictator>(uint32 uVarID, const CFighterDictator* pFighter)
{
	switch(uVarID)
	{
	case eMAID_NADamage:
		{
			CCalculateMainHandPhysicsHurtMop mop;

			return make_pair(true, double(mop.CalInitialDamage(pFighter, NULL)));
		}
	case eMAID_FighterLevel:
		return make_pair(true, pFighter->CppGetLevel());
	case eMAID_MainHandWeaponIsSingle:
		return make_pair(true, pFighter->GetCtrlState(eFCS_ForbitUseWeapon)?true:IsSingleHandsWeapon(pFighter->GetNormalAttackMgr()->GetMHNormalAttack()->GetWeaponType()));
	case eMAID_Undefined:
		return make_pair(false, 0.0);
	default:
		return make_pair(true, double(pFighter->CppGetPropertyValue(uVarID)));
	}
}

template<>
pair<bool, double> CExpression::GetTalentValue<CFighterDictator>(const string& varName, const CFighterDictator* pFighter)
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
pair<bool, double> CExpression::GetMasterTalentValue<CFighterDictator>(const string& varName, const CFighterDictator* pFighter)
{
	IFighterServerHandler* pFH = pFighter->GetHandler();
	if(!pFH) return make_pair(false, 0.0);
	CCharacterDictator* pMaster = pFH->GetCharacter()->GetMaster();
	if(!pMaster) return make_pair(false, 0.0);
	return GetTalentValue(varName, pMaster->GetFighter());
}

template<>
pair<bool, double> CExpression::GetMasterAttrValue<CFighterDictator>(uint32 uVarID, const CFighterDictator* pFighter)
{
	IFighterServerHandler* pFH = pFighter->GetHandler();
	if(!pFH) return make_pair(false, 0.0);
	CCharacterDictator* pMaster = pFH->GetCharacter()->GetMaster();
	if(!pMaster) return make_pair(false, 0.0);
	return GetAttrValue(uVarID, pMaster->GetFighter());
}

template<>
pair<bool, double> CExpression::GetServantAttrValue<CFighterDictator>(uint32 uVarID, const CFighterDictator* pFighter)
{
	CFighterDictator* pServant = pFighter->GetPet();
	if(!pServant) return make_pair(false, 0.0);
	return GetAttrValue(uVarID, pServant);
}


template<>
pair<bool, double> CExpression::GetMasterTempValue<CFighterDictator>(UUnitValue pVarPtr, const CFighterDictator* pFighter)
{
	IFighterServerHandler* pFH = pFighter->GetHandler();
	if(!pFH) return make_pair(false, 0.0);
	CCharacterDictator* pMaster = pFH->GetCharacter()->GetMaster();
	if(!pMaster) return make_pair(false, 0.0);
	return GetTempValue(pVarPtr, pMaster->GetFighter());
}

template<>
const CFighterDictator* CExpression::GetCalcFighter<CFighterDictator>(const CFighterDictator* pFighter[], ECalcTargetType eTargetType)
{
	if(eTargetType == eCTT_Master)
	{
		IFighterServerHandler* pFH = pFighter[eCTT_Self]->GetHandler();
		if(!pFH) return NULL;
		CCharacterDictator* pMaster = pFH->GetCharacter()->GetMaster();
		if(!pMaster) return NULL;
		return pMaster->GetFighter();

	}
	else
	{
		return pFighter[eTargetType];
	}
}

//for CFighterMediator
//template<>
//pair<bool, double> CExpression::GetTempValue<CFighterMediator>(const string& varName, const CFighterMediator* pFighter)
//{
//	return GetTempValue(varName, static_cast<const CFighterDictator*>(pFighter));
//}
//
//template<>
//pair<bool, double> CExpression::GetAttrValue<CFighterMediator>(const string& varName, const CFighterMediator* pFighter)
//{
//	return GetAttrValue(varName, static_cast<const CFighterDictator*>(pFighter));
//}
//
//template<>
//pair<bool, double> CExpression::GetTalentValue<CFighterMediator>(const string& varName, const CFighterMediator* pFighter)
//{
//	return GetTalentValue(varName, static_cast<const CFighterDictator*>(pFighter));
//}
//
//template<>
//pair<bool, double> CExpression::GetMasterTalentValue<CFighterMediator>(const string& varName, const CFighterMediator* pFighter)
//{
//	return GetMasterTalentValue(varName, static_cast<const CFighterDictator*>(pFighter));
//}
//
//template<>
//pair<bool, double> CExpression::GetMasterAttrValue<CFighterMediator>(const string& varName, const CFighterMediator* pFighter)
//{
//	return GetMasterAttrValue(varName, static_cast<const CFighterDictator*>(pFighter));
//}
//
//template<>
//pair<bool, double> CExpression::GetServantAttrValue<CFighterMediator>(const string& varName, const CFighterMediator* pFighter)
//{
//	return GetServantAttrValue(varName, static_cast<const CFighterDictator*>(pFighter));
//}
//
//
//template<>
//pair<bool, double> CExpression::GetMasterTempValue<CFighterMediator>(const string& varName, const CFighterMediator* pFighter)
//{
//	return GetMasterTempValue(varName, static_cast<const CFighterDictator*>(pFighter));
//}

//template<>
//const CFighterDictator* CExpression::GetCalcFighter<CFighterMediator, CFighterDictator>(const CFighterMediator* pFighter[], ECalcTargetType eTargetType)
//{
//	return GetCalcFighter<CFighterDictator, CFighterDictator>(static_cast<const CFighterDictator*>(pFighter), eTargetType);
//}


