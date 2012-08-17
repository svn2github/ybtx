#include "stdafx.h"
#include "MagicConds_Function.h"
#include "CFighterFollower.h"
#include "CMagicStateClient.h"
#include "COtherStateClient.h"
#include "CCfgCalc.h"
#include "CCharacterFollower.h"
#include "CFighterDirector.h"
#include "CNormalAttackClient.h"
#include "CNormalAttackCfgClient.h"
#include "CSyncVariantClient.h"
#include "CBurstSoulCfgClient.h"
#include "CCharacterDirector.h"

uint32 CTestMagicStateMCOND::Test(uint32 SkillLevel,const CCfgCalc& Arg,const CFighterDirector* pFighter) const
{
	if (pFighter->GetMagicStateMgrClient()->ExistState(Arg.GetString()))
		return eDSR_Success;
	else
		return eDSR_NotInTheMagicState;
}

uint32 CTestNotInMagicStateMCOND::Test(uint32 SkillLevel,const CCfgCalc& Arg,const CFighterDirector* pFighter) const
{
	if (pFighter->GetMagicStateMgrClient()->ExistState(Arg.GetString()))
		return eDSR_InTheMagicState;
	else
		return eDSR_Success;
}

uint32 CTestNotInRepeatedMagicStateMCOND::Test(uint32 SkillLevel,const CCfgCalc& Arg,const CFighterDirector* pFighter) const
{
	if (pFighter->GetMagicStateMgrClient()->ExistState(Arg.GetString()))
		return eDSR_AlreadyInTheMagicState;
	else
		return eDSR_Success;
}

uint32 CTestTriggerStateMCOND::Test(uint32 SkillLevel,const CCfgCalc& Arg,const CFighterDirector* pFighter) const
{
	if (pFighter->GetTriggerStateMgrClient()->ExistState(Arg.GetString()))
		return eDSR_Success;
	else
		return eDSR_NotInTheTriggerState;
}

uint32 CTestDamageChangeStateMCOND::Test(uint32 SkillLevel,const CCfgCalc& Arg,const CFighterDirector* pFighter) const
{
	if (pFighter->GetDamageChangeStateMgrClient()->ExistState(Arg.GetString()))
		return eDSR_Success;
	else
		return eDSR_NotInTheDamageChangeState;
}

uint32 CTestNotInDamageChangeStateMCOND::Test(uint32 SkillLevel,const CCfgCalc& Arg,const CFighterDirector* pFighter) const
{
	if (pFighter->GetDamageChangeStateMgrClient()->ExistState(Arg.GetString()))
		return eDSR_InTheDamageChangeState;
	else
		return eDSR_Success;
}

uint32 CTestSpecialStateMCOND::Test(uint32 SkillLevel,const CCfgCalc& Arg,const CFighterDirector* pFighter) const
{
	if (pFighter->GetSpecialStateMgrClient()->ExistState(Arg.GetString()))
		return eDSR_Success;
	else
		return eDSR_NotInTheSpecialState;
}

uint32 CTestMagicStateCascadeGreaterOrEqualMCOND::Test(uint32 SkillLevel,const CCfgCalc& Arg,const CFighterDirector* pFighter) const
{
	if(pFighter->GetMagicStateMgrClient()->CanDecStateCascade(Arg.GetString(0), atoi(Arg.GetChars(1))))
	{
		return eDSR_Success;
	}
	else
	{
		return eDSR_NotEnoughMagicStateCascade;
	}
}

uint32 CTestAureMagicMCOND::Test(uint32 SkillLevel,const CCfgCalc& Arg,const CFighterDirector* pFighter) const
{
	// 临时TEST
	return eDSR_Success;
}

uint32 CTestShieldEquipMCOND::Test(uint32 SkillLevel,const CCfgCalc& Arg,const CFighterDirector* pFighter) const
{
	// 临时TEST
	if(pFighter->GetNormalAttack()->GetAHWeaponType() == eWT_Shield)
	{
		//cout << " has shield" << endl;
		return eDSR_Success;
	}

	//cout << " not have shield" << endl;
	return eDSR_HaveNotShieldEquip;
}

uint32 CTestMainHandEquipMCOND::Test(uint32 SkillLevel,const CCfgCalc& Arg,const CFighterDirector* pFighter) const
{	
	if(Arg.GetTestString().compare("武器") == 0)
	{
		if(pFighter->GetNormalAttack()->HasWeapon())
		{
			return eDSR_Success;
		}
		else
		{
			return eDSR_HaveNotMainHandEquip;
		}
	}
	for (uint32 i=0;i<Arg.GetStringCount();i++)
	{
		if (pFighter->GetNormalAttack()->GetMHWeaponType() != eWT_None && pFighter->GetNormalAttack()->GetMHWeaponType() == CNormalAttackCfgClient::ms_mapWeaponType[Arg.GetString(i)])
		{
			return eDSR_Success;
		}
	}
	return eDSR_HaveNotMainHandEquip;
}

uint32 CTestAssistHandEquipMCOND::Test(uint32 SkillLevel,const CCfgCalc& Arg,const CFighterDirector* pFighter) const
{	
	if(pFighter->GetNormalAttack()->HasWeapon())
	{
		return eDSR_Success;
	}

	return eDSR_HaveNotAssistHandEquip;
}

uint32 CTestIsExistHorseMCOND::Test(uint32 SkillLevel,const CCfgCalc& Arg,const CFighterDirector* pFighter) const
{
	// 临时TEST
	return eDSR_Success;
}
uint32 CTestIsNotExistHorseMCOND::Test(uint32 SkillLevel,const CCfgCalc& Arg,const CFighterDirector* pFighter) const
{
	// 临时TEST
	return eDSR_Success;
}

uint32 CTestTargetCanBeControlledMCOND::Test(uint32 SkillLevel,const CCfgCalc& Arg,const CFighterDirector* pFighter) const
{
	return eDSR_Success;
}

uint32 CTestBurstSoulTimesGreaterOrEqualMCOND::Test(uint32 SkillLevel,const CCfgCalc& Arg,const CFighterDirector* pFighter) const
{
	uint32 uBurstSoulTimes = const_cast<CFighterDirector*>(pFighter)->GetBurstSoulTimes();
	uint32 uLimit = CBurstSoulCfgClient::GetNeedBurstSoulTimesByLevel(Arg.GetIntValue());
	if(uBurstSoulTimes >= uLimit)
	{
		return eDSR_Success;
	}

	return eDSR_Fail;
}

uint32 CTestTargetIsPlayerMCOND::Test(uint32 SkillLevel,const CCfgCalc& Arg,const CFighterDirector* pFighter) const
{
	//string strArg = Arg.GetString();
	//if(strArg.compare("是") == 0 && pTo->GetType() == eFT_FighterMediator)
	//{
	//	return eDSR_Success;
	//}
	//else if(strArg.compare("否") == 0 && pTo->GetType() == eFT_FighterDictator)
	//{
	//	return eDSR_Success;
	//}
	//return eDSR_Fail;

	// 临时TEST
	return eDSR_Success;
}

uint32 CTestTargetIsNPCMCOND::Test(uint32 SkillLevel,const CCfgCalc& Arg,const CFighterDirector* pFighter) const
{
	return eDSR_Success;
}

uint32 CTestTargetIsSummonMCOND::Test(uint32 SkillLevel,const CCfgCalc& Arg,const CFighterDirector* pFighter) const
{
	return eDSR_Success;
}

uint32 CTestTargetLevelLesserOrEqualMCOND::Test(uint32 SkillLevel,const CCfgCalc& Arg,const CFighterDirector* pFighter) const
{
	// 临时TEST
	return eDSR_Success;
}

uint32 CTestNotInBattleMCOND::Test(uint32 SkillLevel, const CCfgCalc& Arg, const CFighterDirector* pFighter) const
{
	if (!pFighter->GetCtrlState(eFCS_InBattle))
		return eDSR_Success;
	else
		return eDSR_Fail;
}

uint32 CTestNotOnMissionMCOND::Test(uint32 SkillLevel, const CCfgCalc& Arg, const CFighterDirector* pFighter) const
{
	if (!pFighter->GetCtrlState(eFCS_OnMission))
		return eDSR_Success;
	else
		return eDSR_Fail;
}

uint32 CTestIsExistPetMCOND::Test(uint32 SkillLevel,const CCfgCalc& Arg,const CFighterDirector* pFighter) const
{
	CCharacterFollower* pCharacter = pFighter->GetCharacter();
	CCharacterFollower* pPetChar = NULL;
	if (pCharacter->CastToCharacterDirector())
		pPetChar = pCharacter->CastToCharacterDirector()->GetServantByType(ENpcType_Summon);
	if (!pPetChar)
	{
		pPetChar = pCharacter->CastToCharacterDirector()->GetServantByType(ENpcType_BattleHorse);
		if(!pPetChar)
			return eDSR_IsNotExistPet;
	}
	CFighterFollower* pPetFighter = pPetChar->GetFighter();
	if (pPetFighter == NULL)
	{
		return eDSR_IsNotExistPet;
	}

	uint32 uCount = Arg.GetStringCount();
	if (uCount == 0)
		return eDSR_Success;
	CCharacterFollower* pPetCharacter = pPetFighter->GetCharacter();
	const CSyncVariantClient* pViewVariant= pPetCharacter?pPetCharacter->GetViewVariant():NULL;
	CSyncVariantClient* pAttributeVariant= pViewVariant?pViewVariant->GetMember("Attribute"):NULL;
	CSyncVariantClient* pCharNameVariant= pAttributeVariant?pAttributeVariant->GetMember("CharName"):NULL;
	if(pCharNameVariant==NULL)
	{
		return eDSR_IsNotExistPet;
	}
	string strCurPetName = pCharNameVariant->GetString();
	for (uint32 i=0; i< uCount; i++)
	{
		string strPetName=Arg.GetString(i);
		if(strPetName == "" || strPetName == strCurPetName)
		{
			return eDSR_Success;
		}
	}
	return eDSR_Fail;
}
