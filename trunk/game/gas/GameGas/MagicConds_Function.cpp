#include "stdafx.h"
#include "MagicConds_Function.h"
#include "CAureMagicServer.h"
#include "CAureMagicCfgServer.h"
#include "CFighterMediator.h"
#include "CAllStateMgr.h"
#include "CCfgCalc.h"
#include "CSkillMgrServer.h"
#include "CMagicStateMgr.h"
#include "CTriggerStateMgr.h"
#include "CDamageChangeStateMgr.h"
#include "CSpecialStateMgr.h"
#include "CSkillInstServer.h"
#include "COtherStateMgr.inl"
#include "NormalAttack.h"
#include "CNormalAttackCfg.h"
#include "CCharacterDictator.h"
#include "INpcAIHandler.h"
#include "CNpcAI.h"
#include "CPropertyCfg.h"
#include "CSkillServer.h"
#include "MagicOps_Function.h"
#include "CSyncVariantServer.h"
#include "MoveMagicServerHelper.h"
#include "CMagicMgrServer.h"
#include "CoreCommon.h"
#include "CCoreSceneServer.h"
#include "CPixelPath.h"
#include "BaseHelper.h"
#include "CRelationMgrServer.h"
#include "CCastingProcess.h"
#include "FighterProperty.inl"
#include "CBurstSoulMgrServer.h"
#include "CPositionMagicServer.h"
#include "CPositionMagicCfgServer.h"
#include "CSkillCfg.h"
#include "CMagicOpCfgArg.h"
#include "TCoolDownMgr.h"
#include "ICharacterMediatorCallbackHandler.h"
#include "CTalent.h"

uint32 CTestMagicStateMCOND::Test(CSkillInstServer* pSkillInst,const CCfgArg* pArg,const CFighterDictator* pFrom,const CFighterDictator* pTo) const
{
	CAllStateMgrServer* pStateMgr = pTo->GetAllStateMgr();
	CMagicStateMgrServer* pMagicStateMgr = pStateMgr?pStateMgr->GetMagicStateMgrServer():NULL;
	CMagicStateCfgArg* pMagicStateCfg = static_cast<CMagicStateCfgArg*>(const_cast<CCfgArg*>(pArg));
	if (pMagicStateMgr&&pMagicStateMgr->ExistState(*pMagicStateCfg->m_pCfg))
		return eDSR_Success;
	else
		return eDSR_NotInTheMagicState;
}

CCfgArg* CTestMagicStateMCOND::InitArg(const string& szArg)
{
	return new CMagicStateCfgArg(&CMagicStateCfgServer::Get(szArg));
}

uint32 CNotTestMagicStateAndSelfExistTalentMCOND::Test(CSkillInstServer* pSkillInst,const CCfgArg* pArg,const CFighterDictator* pFrom,const CFighterDictator* pTo) const
{
	CAllStateMgrServer* pStateMgr = pTo->GetAllStateMgr();
	CMagicStateMgrServer* pMagicStateMgr = pStateMgr?pStateMgr->GetMagicStateMgrServer():NULL;
	CMagicStateCfgMultiArg* pMagicStateCfg = static_cast<CMagicStateCfgMultiArg*>(const_cast<CCfgArg*>(pArg));
	const string& TalentName = 	pMagicStateCfg->m_pArg->GetString().c_str();
	uint32 uPoint = pFrom?pFrom->GetTalentHolder()->GetPoint(TalentName.c_str()):0;
	if (pMagicStateMgr&&pMagicStateMgr->ExistState(*pMagicStateCfg->m_pCfg)&&uPoint<=0)
		return eDSR_Fail;
	else
		return eDSR_Success;
}

CCfgArg* CNotTestMagicStateAndSelfExistTalentMCOND::InitArg(const string& szArg)
{
	CCfgCalc pCfgCalc(szArg);
	return new CMagicStateCfgMultiArg(&CMagicStateCfgServer::Get(pCfgCalc.GetString(0)), pCfgCalc.GetString(1));
}

uint32 CTestNotInMagicStateMCOND::Test(CSkillInstServer* pSkillInst,const CCfgArg* pArg,const CFighterDictator* pFrom,const CFighterDictator* pTo) const
{
	CAllStateMgrServer* pStateMgr = pTo->GetAllStateMgr();
	CMagicStateMgrServer* pMagicStateMgr = pStateMgr?pStateMgr->GetMagicStateMgrServer():NULL;
	CMagicStateCfgArg* pMagicStateCfg = static_cast<CMagicStateCfgArg*>(const_cast<CCfgArg*>(pArg));
	if (pMagicStateMgr&&pMagicStateMgr->ExistState(*pMagicStateCfg->m_pCfg))
		return eDSR_InTheMagicState;
	else
		return eDSR_Success;
}

CCfgArg* CTestNotInMagicStateMCOND::InitArg(const string& szArg)
{
	return new CMagicStateCfgArg(&CMagicStateCfgServer::Get(szArg));
}

uint32 CTestNotInRepeatedMagicStateMCOND::Test(CSkillInstServer* pSkillInst,const CCfgArg* pArg,const CFighterDictator* pFrom,const CFighterDictator* pTo) const
{
	CAllStateMgrServer* pStateMgr = pTo->GetAllStateMgr();
	CMagicStateMgrServer* pMagicStateMgr = pStateMgr?pStateMgr->GetMagicStateMgrServer():NULL;
	CMagicStateCfgArg* pMagicStateCfg = static_cast<CMagicStateCfgArg*>(const_cast<CCfgArg*>(pArg));
	if (pMagicStateMgr&&pMagicStateMgr->ExistState(*pMagicStateCfg->m_pCfg))
		return eDSR_AlreadyInTheMagicState;
	else
		return eDSR_Success;
}

CCfgArg* CTestNotInRepeatedMagicStateMCOND::InitArg(const string& szArg)
{
	return new CMagicStateCfgArg(&CMagicStateCfgServer::Get(szArg));
}

uint32 CTestSelfMagicStateMCOND::Test(CSkillInstServer* pSkillInst,const CCfgArg* pArg,const CFighterDictator* pFrom,const CFighterDictator* pTo) const
{
	CAllStateMgrServer* pStateMgr = pTo->GetAllStateMgr();
	CMagicStateMgrServer* pMagicStateMgr = pStateMgr?pStateMgr->GetMagicStateMgrServer():NULL;
	CMagicStateCfgArg* pMagicStateCfg = static_cast<CMagicStateCfgArg*>(const_cast<CCfgArg*>(pArg));
	if (pMagicStateMgr&&pMagicStateMgr->ExistState(*pMagicStateCfg->m_pCfg, pFrom))
		return eDSR_Success;
	else
		return eDSR_NotInTheMagicState;
}

CCfgArg* CTestSelfMagicStateMCOND::InitArg(const string& szArg)
{
	return new CMagicStateCfgArg(&CMagicStateCfgServer::Get(szArg));
}


uint32 CTestMagicStateCascadeGreaterOrEqualMCOND::Test(CSkillInstServer* pSkillInst,const CCfgArg* pArg,const CFighterDictator* pFrom,const CFighterDictator* pTo) const
{
	CAllStateMgrServer* pStateMgr = pTo->GetAllStateMgr();
	CMagicStateMgrServer* pMagicStateMgr = pStateMgr?pStateMgr->GetMagicStateMgrServer():NULL;
	CMagicStateCfgMultiArg* pMagicStateCfg = static_cast<CMagicStateCfgMultiArg*>(const_cast<CCfgArg*>(pArg));
	if(pMagicStateMgr&&pMagicStateMgr->CanDecStateCascade(*pMagicStateCfg->m_pCfg, atoi(pMagicStateCfg->m_pArg->GetString().c_str())))
	{
		return eDSR_Success;
	}
	else
	{
		return eDSR_NotEnoughMagicStateCascade;
	}
}

CCfgArg* CTestMagicStateCascadeGreaterOrEqualMCOND::InitArg(const string& szArg)
{
	CCfgCalc pCfgCalc(szArg);
	return new CMagicStateCfgMultiArg(&CMagicStateCfgServer::Get(pCfgCalc.GetString(0)), pCfgCalc.GetString(1));
}

uint32 CTestTriggerStateMCOND::Test(CSkillInstServer* pSkillInst,const CCfgArg* pArg,const CFighterDictator* pFrom,const CFighterDictator* pTo) const
{
	CAllStateMgrServer* pStateMgr = pTo->GetAllStateMgr();
	CTriggerStateMgrServer* pTriggerStateMgr =pStateMgr? pStateMgr->GetTriggerStateMgrServer():NULL;
	CTriggerStateCfgArg* pTriggerStateCfgArg = static_cast<CTriggerStateCfgArg*>(const_cast<CCfgArg*>(pArg));
	if (pTriggerStateMgr&&pTriggerStateMgr->ExistState(*pTriggerStateCfgArg->m_pCfg))
		return eDSR_Success;
	else
		return eDSR_NotInTheTriggerState;
}

CCfgArg* CTestTriggerStateMCOND::InitArg(const string& szArg)
{
	return new CTriggerStateCfgArg(&CTriggerStateCfgServer::Get(szArg));
}

uint32 CTestSelfTriggerStateMCOND::Test(CSkillInstServer* pSkillInst,const CCfgArg* pArg,const CFighterDictator* pFrom,const CFighterDictator* pTo) const
{
	CAllStateMgrServer* pStateMgr = pTo->GetAllStateMgr();
	CTriggerStateMgrServer* pTriggerStateMgr =pStateMgr? pStateMgr->GetTriggerStateMgrServer():NULL;
	CTriggerStateCfgArg* pTriggerStateCfgArg = static_cast<CTriggerStateCfgArg*>(const_cast<CCfgArg*>(pArg));
	if (pTriggerStateMgr&&pTriggerStateMgr->ExistState(*pTriggerStateCfgArg->m_pCfg, pFrom))
		return eDSR_Success;
	else
		return eDSR_NotInTheTriggerState;
}

CCfgArg* CTestSelfTriggerStateMCOND::InitArg(const string& szArg)
{
	return new CTriggerStateCfgArg(&CTriggerStateCfgServer::Get(szArg));
}

uint32 CTestDamageChangeStateMCOND::Test(CSkillInstServer* pSkillInst,const CCfgArg* pArg,const CFighterDictator* pFrom,const CFighterDictator* pTo) const
{
	CAllStateMgrServer* pStateMgr = pTo->GetAllStateMgr();
	CDamageChangeStateMgrServer* pDamageChangeStateMgr =pStateMgr? pStateMgr->GetDamageChangeStateMgrServer():NULL;
	CDamageChangeStateCfgArg* pDamageChangeStateCfgArg = static_cast<CDamageChangeStateCfgArg*>(const_cast<CCfgArg*>(pArg));
	if (pDamageChangeStateMgr&&pDamageChangeStateMgr->ExistState(*pDamageChangeStateCfgArg->m_pCfg))
		return eDSR_Success;
	else
		return eDSR_NotInTheDamageChangeState;
}

CCfgArg* CTestDamageChangeStateMCOND::InitArg(const string& szArg)
{
	return new CDamageChangeStateCfgArg(&CDamageChangeStateCfgServer::Get(szArg));
}

uint32 CTestSelfDamageChangeStateMCOND::Test(CSkillInstServer* pSkillInst,const CCfgArg* pArg,const CFighterDictator* pFrom,const CFighterDictator* pTo) const
{
	CAllStateMgrServer* pStateMgr = pTo->GetAllStateMgr();
	CDamageChangeStateMgrServer* pDamageChangeStateMgr =pStateMgr? pStateMgr->GetDamageChangeStateMgrServer():NULL;
	CDamageChangeStateCfgArg* pDamageChangeStateCfgArg = static_cast<CDamageChangeStateCfgArg*>(const_cast<CCfgArg*>(pArg));
	if (pDamageChangeStateMgr&&pDamageChangeStateMgr->ExistState(*pDamageChangeStateCfgArg->m_pCfg, pFrom))
		return eDSR_Success;
	else
		return eDSR_NotInTheDamageChangeState;
}

CCfgArg* CTestSelfDamageChangeStateMCOND::InitArg(const string& szArg)
{
	return new CDamageChangeStateCfgArg(&CDamageChangeStateCfgServer::Get(szArg));
}

uint32 CTestNotInDamageChangeStateMCOND::Test(CSkillInstServer* pSkillInst,const CCfgArg* pArg,const CFighterDictator* pFrom,const CFighterDictator* pTo) const
{
	CAllStateMgrServer* pStateMgr = pTo->GetAllStateMgr();
	Ast(pStateMgr);
	CDamageChangeStateMgrServer* pDamageChangeStateMgr = pStateMgr->GetDamageChangeStateMgrServer();
	CDamageChangeStateCfgArg* pDamageChangeStateCfgArg = static_cast<CDamageChangeStateCfgArg*>(const_cast<CCfgArg*>(pArg));
	if (pDamageChangeStateMgr&&pDamageChangeStateMgr->ExistState(*pDamageChangeStateCfgArg->m_pCfg))
		return eDSR_InTheDamageChangeState;
	else
		return eDSR_Success;
}

CCfgArg* CTestNotInDamageChangeStateMCOND::InitArg(const string& szArg)
{
	return new CDamageChangeStateCfgArg(&CDamageChangeStateCfgServer::Get(szArg));
}

uint32 CTestSpecialStateMCOND::Test(CSkillInstServer* pSkillInst,const CCfgArg* pArg,const CFighterDictator* pFrom,const CFighterDictator* pTo) const
{
	CAllStateMgrServer* pStateMgr = pTo->GetAllStateMgr();
	Ast(pStateMgr);
	CSpecialStateMgrServer* pSpecialStateMgr = pStateMgr->GetSpecialStateMgrServer();
	CSpecialStateCfgArg* pSpecialStateCfgArg = static_cast<CSpecialStateCfgArg*>(const_cast<CCfgArg*>(pArg));
	if (pSpecialStateMgr&&pSpecialStateMgr->ExistState(*pSpecialStateCfgArg->m_pCfg))
		return eDSR_Success;
	else
		return eDSR_NotInTheSpecialState;
}

CCfgArg* CTestSpecialStateMCOND::InitArg(const string& szArg)
{
	return new CSpecialStateCfgArg(&CSpecialStateCfgServer::Get(szArg));
}

uint32 CTestNotInSpecialStateMCOND::Test(CSkillInstServer* pSkillInst,const CCfgArg* pArg,const CFighterDictator* pFrom,const CFighterDictator* pTo) const
{
	CAllStateMgrServer* pStateMgr = pTo->GetAllStateMgr();
	CSpecialStateMgrServer* pSpecialStateMgr = pStateMgr?pStateMgr->GetSpecialStateMgrServer():NULL;
	CSpecialStateCfgArg* pSpecialStateCfgArg = static_cast<CSpecialStateCfgArg*>(const_cast<CCfgArg*>(pArg));
	if (pSpecialStateMgr&&pSpecialStateMgr->ExistState(*pSpecialStateCfgArg->m_pCfg))
		return eDSR_InTheSpecialState;
	else
		return eDSR_Success;
}

CCfgArg* CTestNotInSpecialStateMCOND::InitArg(const string& szArg)
{
	return new CSpecialStateCfgArg(&CSpecialStateCfgServer::Get(szArg));
}

uint32 CTestSelfSpecialStateMCOND::Test(CSkillInstServer* pSkillInst,const CCfgArg* pArg,const CFighterDictator* pFrom,const CFighterDictator* pTo) const
{
	CAllStateMgrServer* pStateMgr = pTo->GetAllStateMgr();
	Ast(pStateMgr);
	CSpecialStateMgrServer* pSpecialStateMgr = pStateMgr->GetSpecialStateMgrServer();
	CSpecialStateCfgArg* pSpecialStateCfgArg = static_cast<CSpecialStateCfgArg*>(const_cast<CCfgArg*>(pArg));
	if (pSpecialStateMgr&&pSpecialStateMgr->ExistState(*pSpecialStateCfgArg->m_pCfg, pFrom))
		return eDSR_Success;
	else
		return eDSR_NotInTheSpecialState;
}

CCfgArg* CTestSelfSpecialStateMCOND::InitArg(const string& szArg)
{
	return new CSpecialStateCfgArg(&CSpecialStateCfgServer::Get(szArg));
}

uint32 CTestNotInStateByDecreaseTypeMCOND::Test(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,const CFighterDictator* pFrom,const CFighterDictator* pTo) const
{
	CAllStateMgrServer* pStateMgr = pTo->GetAllStateMgr();
	Ast(pStateMgr);
	if(Arg.GetTestString() == "")
	{
		if(!pStateMgr->ExistDecreaseState())
			return eDSR_Success;
		else
			return eDSR_InDecreaseState;
	}
	else
	{
		if(!pStateMgr->ExistStateByDecreaseType(CBaseStateCfgServer::GetDecreaseType(Arg.GetString())))
			return eDSR_Success;
		else
			return eDSR_InDecreaseState;
	}
}

uint32 CNotTestInStateByDecreaseTypeAndSelfExistTalentMCOND::Test(CSkillInstServer* pSkillInst,const CCfgArg* pArg,const CFighterDictator* pFrom,const CFighterDictator* pTo)const
{
	CTwoStringArg* pTowArg = static_cast<CTwoStringArg*>(const_cast<CCfgArg*>(pArg));
	const string& TalentName =  pTowArg->m_strArg2;
	CAllStateMgrServer* pStateMgr = pTo->GetAllStateMgr();
	Ast(pStateMgr);
	uint32 uPoint = pFrom?pFrom->GetTalentHolder()->GetPoint(TalentName.c_str()):0;
	if(pStateMgr->ExistStateByDecreaseType(CBaseStateCfgServer::GetDecreaseType(pTowArg->m_strArg1))&&uPoint<=0)
		return eDSR_Fail;
	else
		return eDSR_Success;
}

CCfgArg* CNotTestInStateByDecreaseTypeAndSelfExistTalentMCOND::InitArg(const string& szArg)
{
	CCfgCalc pCfgCalc(szArg);
	return new CTwoStringArg(pCfgCalc.GetString(0), pCfgCalc.GetString(1));
}


uint32 CTestAureMagicMCOND::Test(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,const CFighterDictator* pFrom,const CFighterDictator* pTo) const
{
	Ast(pTo->GetMagicMgr());
	const string& StanceName = Arg.GetString();
	CAureMagicCfgServerSharedPtr& pCfg = CAureMagicCfgServer::GetAureMagic(StanceName);
	if (CAureMagicServer::HasAureMagic(pCfg,pFrom))
	{
		return eDSR_Success;
	}
	return eDSR_NotInTheStanceSkill;
}

uint32 CTestShieldEquipMCOND::Test(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, const CFighterDictator* pFrom, const CFighterDictator* pTo) const
{
	if (pTo->GetNormalAttackMgr()->GetAHNormalAttack()->GetWeaponType() == eWT_Shield)
		return eDSR_Success;
	else
		return eDSR_HaveNotShieldEquip;
}

uint32 CTestMainHandEquipMCOND::Test(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, const CFighterDictator* pFrom, const CFighterDictator* pTo) const
{	
	CNormalAttackMgr* pAttackMgr = pTo->GetNormalAttackMgr();
	Ast(pAttackMgr);
	CSingleHandNATick* pMainAttack = pAttackMgr->GetMHNormalAttack();
	Ast(pMainAttack);
	//暂时用攻击力来判断是否装备武器, 如果以后需要区分装备的武器的类型的话, 需要获取角色身上装备的信息来判断
	if(Arg.GetTestString().compare("武器") == 0)
	{

		if(pMainAttack->HasWeapon() && pMainAttack->GetWeaponType() != eWT_None)
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
		if (pMainAttack->GetWeaponType() != eWT_None && pMainAttack->GetWeaponType() == CNormalAttackCfg::ms_mapWeaponType[Arg.GetString(i)])
		{
			return eDSR_Success;
		}
	}

	return eDSR_HaveNotMainHandEquip;
}

uint32 CTestAssistHandEquipMCOND::Test(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, const CFighterDictator* pFrom, const CFighterDictator* pTo) const
{	
	CNormalAttackMgr* pAttackMgr = pTo->GetNormalAttackMgr();
	Ast(pAttackMgr);
	CSingleHandNATick* pAssistAttack = pAttackMgr->GetAHNormalAttack();
	Ast(pAssistAttack);

	if(pAssistAttack->HasWeapon() && pAssistAttack->GetWeaponType() != eWT_None)
	{
		return eDSR_Success;
	}
	return eDSR_HaveNotAssistHandEquip;
}

uint32 CTestIsExistPetMCOND::Test(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,const CFighterDictator* pFrom,const CFighterDictator* pTo) const
{
	CFighterDictator* pPetFighter = pTo->GetPet();
	if (pPetFighter == NULL)
	{
		return eDSR_IsNotExistPet;
	}

	if (Arg.GetTestString() == "")
		return eDSR_Success;
	CCharacterDictator* pPetCharacter = pPetFighter->GetCharacter();
	CSyncVariantServer* pViewVariant= pPetCharacter?pPetCharacter->GetViewVariant():NULL;
	CSyncVariantServer* pAttributeVariant= pViewVariant?pViewVariant->GetMember("Attribute"):NULL;
	CSyncVariantServer* pCharNameVariant= pAttributeVariant?pAttributeVariant->GetMember("CharName"):NULL;
	if(pCharNameVariant==NULL)
	{
		return eDSR_IsNotExistPet;
	}
	uint32 uCount = Arg.GetStringCount();
	const string& strCurPetName = pCharNameVariant->GetString();
	for (uint32 i=0; i< uCount; i++)
	{
		const string& strPetName=Arg.GetString(i);
		if(strPetName == "" || strPetName == strCurPetName)
		{
			return eDSR_Success;
		}
	}
	return eDSR_Fail;
}

uint32 CTestIsNotExistPetMCOND::Test(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,const CFighterDictator* pFrom,const CFighterDictator* pTo) const
{
	CFighterDictator* pPetFighter = pTo->GetPet();
	if (pPetFighter == NULL)
	{
		return eDSR_Fail;
	}
	if (Arg.GetTestString() == "")
		return eDSR_Success;

	const string& strPetName=Arg.GetString();
	CCharacterDictator* pPetCharacter = pPetFighter->GetCharacter();
	CSyncVariantServer* pViewVariant= pPetCharacter?pPetCharacter->GetViewVariant():NULL;
	CSyncVariantServer* pAttributeVariant= pViewVariant?pViewVariant->GetMember("Attribute"):NULL;
	CSyncVariantServer* pCharNameVariant= pAttributeVariant?pAttributeVariant->GetMember("CharName"):NULL;
	if(pCharNameVariant==NULL)
	{
		return eDSR_Success;
	}
	const string& strCurPetName = pCharNameVariant->GetString();
	if(strPetName != "" && strPetName != strCurPetName)
	{
		return eDSR_Success;
	}
	return eDSR_Fail;
}

uint32 CTestIsExistTotemMCOND::Test(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,const CFighterDictator* pFrom,const CFighterDictator* pTo) const
{
	if (!pTo->GetTotem())
	{
		return eDSR_IsNotExistTotem;
	}
	return eDSR_Success;
}

uint32 CTestIsExistHorseMCOND::Test(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,const CFighterDictator* pFrom,const CFighterDictator* pTo) const
{
	if (pTo->GetCtrlState(eFCS_InNormalHorse) || pTo->GetCtrlState(eFCS_InBattleHorse))
	{
		return eDSR_Success;
	}
	return eDSR_IsNotExistHorse;
}

uint32 CTestIsNotExistHorseMCOND::Test(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,const CFighterDictator* pFrom,const CFighterDictator* pTo) const
{
	if (pTo->GetCtrlState(eFCS_InNormalHorse) || pTo->GetCtrlState(eFCS_InBattleHorse))
	{
		return eDSR_IsExistHorse;
	}
	return eDSR_Success;
}

uint32 CTestTargetCanBeControlledMCOND::Test(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, const CFighterDictator* pFrom, const CFighterDictator* pTo) const
{
	if(const_cast<CFighterDictator*>(pTo)->CastToFighterMediator())
	{
		return eDSR_CannotBeControll;
	}
	CCharacterDictator* pCharTo = pTo->GetCharacter();
	if(pCharTo && pCharTo->CanAIChange())
	{
		return eDSR_Success;
	}
	return eDSR_CannotBeControll;
}

uint32 CTestTargetCanBeRavinMCOND::Test(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, const CFighterDictator* pFrom, const CFighterDictator* pTo) const
{
	if(const_cast<CFighterDictator*>(pTo)->CastToFighterMediator())
	{
		return eDSR_CannotBeRavin;
	}
	CCharacterDictator* pCharTo = pTo->GetCharacter();
	if(pCharTo && pCharTo->CanBeRavin())
	{
		return eDSR_Success;
	}
	return eDSR_CannotBeRavin;
}

uint32 CTestTargetIsPlayerMCOND::Test(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, const CFighterDictator* pFrom, const CFighterDictator* pTo) const
{
	if(const_cast<CFighterDictator*>(pTo)->CastToFighterMediator())
	{
		return eDSR_Success;
	}
	return eDSR_Fail;
}

uint32 CTestTargetIsFriendMCOND::Test(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, const CFighterDictator* pFrom, const CFighterDictator* pTo) const
{
	if (!pFrom || !pTo)
		return eDSR_Fail;
	CRelationMgrServer& relationMgr = CRelationMgrServer::Inst();
	ERelationType eRelation = relationMgr.GetRelationType(pFrom, pTo);
	if(eRelation==eRT_Friend || eRelation==eRT_FriendInRivalry)
		return eDSR_Success;
	else
		return eDSR_Fail;
}

uint32 CTestTargetIsFriendsServantMCOND::Test(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, const CFighterDictator* pFrom, const CFighterDictator* pTo) const
{
	if (!pFrom || !pTo)
		return eDSR_Fail;
	CCharacterDictator* pMaster = pTo->GetCharacter()->GetMaster();
	if (pMaster)
	{
		CTestTargetIsFriendMCOND testTargetIsFriendMCOND;
		return testTargetIsFriendMCOND.Test(pSkillInst,Arg,pFrom,pMaster->GetFighter());
	}
	return eDSR_Fail;
}

uint32 CTestTargetIsEnemyMCOND::Test(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, const CFighterDictator* pFrom, const CFighterDictator* pTo) const
{
	if (!pFrom || !pTo)
		return eDSR_Fail;
	CRelationMgrServer& relationMgr = CRelationMgrServer::Inst();
	if(relationMgr.IsEnemy(pFrom, pTo))
		return eDSR_Success;
	else
		return eDSR_Fail;
}

uint32 CTestTargetClassMCOND::Test(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, const CFighterDictator* pFrom, const CFighterDictator* pTo) const
{
	const string& strName=Arg.GetString();
	EClass eClass = CPropertyCfg::ms_mapClassType[strName];
	if (pTo->CppGetClass()==eClass)
	{
		return eDSR_Success;
	}
	return eDSR_Fail;
}

uint32 CTestTargetIsNPCMCOND::Test(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, const CFighterDictator* pFrom, const CFighterDictator* pTo) const
{
	CCharacterDictator* pCharTo = pTo->GetCharacter(); 
	if(pCharTo && !pCharTo->CastToCharacterMediator())
	{
		if (Arg.GetTestString() == "")
			return eDSR_Success;
		CSyncVariantServer* pViewVariant= pCharTo->GetViewVariant();
		CSyncVariantServer* pAttributeVariant= pViewVariant?pViewVariant->GetMember("Attribute"):NULL;
		CSyncVariantServer* pCharNameVariant= pAttributeVariant?pAttributeVariant->GetMember("CharName"):NULL;
		const string& strCurNpcName = pCharNameVariant?pCharNameVariant->GetString():"";
		uint32 uCount = Arg.GetStringCount();
		for (uint32 i=0; i< uCount; i++)
		{
			const string& strNpcName=Arg.GetString(i);
			if(strNpcName == "" || strNpcName == strCurNpcName)
			{
				return eDSR_Success;
			}
		}
		return eDSR_Fail;
	}
	return eDSR_IsNotNPC;
}

uint32 CTestTargetIsSummonMCOND::Test(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, const CFighterDictator* pFrom, const CFighterDictator* pTo) const
{
	INpcAIHandler* pHandler=pTo->GetCharacter()->GetNpcAIHandler();
	CNpcAI* pNpcAI = pHandler?pHandler->CastToNpcAI():NULL;
	if(pNpcAI&&pNpcAI->GetNpcType()==ENpcType_Summon)
	{
		return eDSR_Success;
	}
	return eDSR_Fail;
}

uint32 CTestTargetLevelLesserOrEqualMCOND::Test(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, const CFighterDictator* pFrom, const CFighterDictator* pTo) const
{
	uint32 uTargetLevel = pTo->CppGetLevel();
	uint32 uSelfLevel = pFrom->CppGetLevel();
	if (uTargetLevel > uSelfLevel)
		return eDSR_TargetLevelIsbigger;

	uint32 uArgLevel = Arg.GetIntValue();
	uint32 uDiffLevel = uSelfLevel - uTargetLevel;

	if(uArgLevel <= uDiffLevel)
	{
		return eDSR_Success;
	}

	return eDSR_Fail;
}

uint32 CTestTargetIsSelfCOND::Test(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, const CFighterDictator* pFrom, const CFighterDictator* pTo) const
{
	if(!pFrom)
		return eDSR_Fail;

	if(pFrom->GetEntityID() == pTo->GetEntityID())
	{
		return eDSR_Success;
	}
	return eDSR_Fail;
}

uint32 CTestNotInBattleMCOND::Test(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, const CFighterDictator* pFrom, const CFighterDictator* pTo) const
{
	if (!pTo->GetCtrlState(eFCS_InBattle))
		return eDSR_Success;
	else
		return eDSR_Fail;
}

uint32 CTestNotOnMissionMCOND::Test(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, const CFighterDictator* pFrom, const CFighterDictator* pTo) const
{
	if (!pTo->GetCtrlState(eFCS_OnMission))
		return eDSR_Success;
	else
		return eDSR_Fail;
}

uint32 CTestNotBattleHorseStateMCOND::Test(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, const CFighterDictator* pFrom, const CFighterDictator* pTo) const
{
	//处于普通坐骑状态, 非召唤兽坐骑
	if ((pTo->GetCtrlState(eFCS_InBattleHorse) || pTo->GetCtrlState(eFCS_InNormalHorse))&& pTo->GetCharacter() && pTo->GetCharacter()->GetServantByType(ENpcType_BattleHorse) == NULL)
		return eDSR_Success;
	else
		return eDSR_NotInNormalHorseState;
}


uint32 CTestWeaponTypeMCOND::Test(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, const CFighterDictator* pFrom, const CFighterDictator* pTo) const
{
	CNormalAttackMgr* pAttackMgr = pTo->GetNormalAttackMgr();
	Ast(pAttackMgr);
	CSingleHandNATick* pMainAttack = pAttackMgr->GetMHNormalAttack();
	Ast(pMainAttack);
	const string& strName = Arg.GetString();
	EAttackType eAttackType = pMainAttack->GetAttackType();
	if(CSkillCfg::ms_mapAttackType.find(strName)->second == eAttackType)
	{
		return eDSR_Success;
	}
	else
	{
		return eDSR_Fail;
	}
}

//骑乘状态检查
uint32 CTestRidingMCOND::Test(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, const CFighterDictator* pFrom, const CFighterDictator* pTo) const
{
	const string& strHorseName = pTo->GetCharacter()->GetHorseName();
	if (strHorseName != "")
	{
		CEraseRidingStateMOP eraseRidingState;
		eraseRidingState.Do(pSkillInst, Arg, const_cast<CFighterDictator*>(pFrom), const_cast<CFighterDictator*>(pTo));
		uint32 uCount = Arg.GetStringCount();
		for (uint32 i=0; i<uCount; i++)
		{
			if (strHorseName == Arg.GetString(i))
				return eDSR_SkillEndWithOutSing;	
		}
	}

	return eDSR_Success;
}

uint32 CTestTrianglePathMCOND::Test(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, const CFighterDictator* pFrom, const CFighterDictator* pTo) const
{
	CCharacterDictator* pCharFrom = pFrom->GetCharacter();
	CCoreSceneServer* pScene = pCharFrom->GetScene();
	CFPos posFrom, posTo, posBegin, posLeft, posRight;
	posFrom = pCharFrom->GetPixelPos();
	posTo.x = float(pFrom->GetTargetPos().x * eGridSpanForObj);
	posTo.y = float(pFrom->GetTargetPos().y * eGridSpanForObj);
	CalTriangleVertex(posFrom, posTo, posBegin, posLeft, posRight);

	CPixelPath* pPath1 = NULL;
	CPixelPath* pPath2 = NULL;
	CPixelPath* pPath3 = NULL;
	CPixelPath* pPath4 = NULL;

	pPath1 = pScene->CreatePath(pCharFrom->GetPixelPos(), posBegin, eFPT_Line, eBT_LowBarrier);
	pPath2 = pScene->CreatePath(posBegin, posLeft, eFPT_Line, eBT_LowBarrier);  	
	pPath3 = pScene->CreatePath(posLeft, posRight, eFPT_Line, eBT_LowBarrier);  	
	pPath4 = pScene->CreatePath(posRight, posBegin, eFPT_Line, eBT_LowBarrier);  	

	if(pPath1 == NULL || pPath2 == NULL || pPath3 == NULL || pPath4 == NULL)
	{
		return eDSR_ToTargetHaveBarrier;
	}

	SafeRelease(pPath1);
	SafeRelease(pPath2);
	SafeRelease(pPath3);
	SafeRelease(pPath4);

	return eDSR_Success;
}

uint32 CTestIsAvailableDeadBobyMCOND::Test(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, const CFighterDictator* pFrom, const CFighterDictator* pTo) const
{
	if (!pTo->CppIsAlive() && !pTo->GetAllStateMgr()->GetSpecialStateMgrServer()->ExistState(Arg.GetString(0)) && !pTo->GetAllStateMgr()->GetSpecialStateMgrServer()->ExistState(Arg.GetString(1)))
	{
		//cout << "找到可用尸体 " << pTo->GetEntityID() << endl;
		return eDSR_Success;
	}

	//cout << "没有可用尸体" << endl;
	return eDSR_TargetNotExist;
}

uint32 CTestTargetLevelIsInScopeMCOND::Test(CSkillInstServer* pSkillInst, const CCfgArg* pArg, const CFighterDictator* pFrom, const CFighterDictator* pTo) const
{
	uint32 uTargetLevel = pTo->CppGetLevel();
	CTwoCfgCalcArg* pTowCfgCalcArg = static_cast<CTwoCfgCalcArg*>(const_cast<CCfgArg*>(pArg));
	uint32 uLeftLimit = pTowCfgCalcArg->m_pArg1->GetIntValue(pFrom);
	uint32 uRightLimit =  pTowCfgCalcArg->m_pArg2->GetIntValue(pFrom);

	if(uTargetLevel <= uRightLimit && uTargetLevel >= uLeftLimit)
	{
		return eDSR_Success;
	}

	return eDSR_Fail;
}

CCfgArg* CTestTargetLevelIsInScopeMCOND::InitArg(const string& szArg)
{
	CCfgCalc pCfgCalc(szArg);
	return new CTwoCfgCalcArg(pCfgCalc.GetString(0), pCfgCalc.GetString(1));
}

uint32 CTestTargetDistIsLesserThanMCOND::Test(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, const CFighterDictator* pFrom, const CFighterDictator* pTo) const
{
	float fDistThreshold = float(Arg.GetDblValue(pFrom)) * eGridSpanForObj;
	if(pFrom == NULL || pTo == NULL)
	{
		return eDSR_Fail;
	}

	//CFPos posFrom, posTo;
	//pFrom->GetPixelPos(posFrom);
	//pTo->GetPixelPos(posTo);
	//float x = posFrom.x - posTo.x;
	//float y = posFrom.y - posTo.y;
	//float fCurDist = sqrt(x*x + y*y);
	float fCurDist = pFrom->GetFighterDist(pTo);

	if(fCurDist <= fDistThreshold)
	{
		return eDSR_Success;
	}

	return eDSR_Fail;
}

uint32 CTestTargetIsVestInSelfMCOND::Test(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, const CFighterDictator* pFrom, const CFighterDictator* pTo) const
{
	if (pFrom == NULL || pTo == NULL)
	{
		return eDSR_Fail;
	}
	if (pTo->GetCharacter()->GetExpOwnerID() == pFrom->GetEntityID())
		return eDSR_Success;
	else
		return eDSR_Fail;
}

uint32 CTestTargetIsRaidmatesMCOND::Test(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, const CFighterDictator* pFrom, const CFighterDictator* pTo) const
{
	if (pFrom == NULL || pTo == NULL)
		return eDSR_Fail;

	CRelationMgrServer& relationMgr = CRelationMgrServer::Inst();
	if(relationMgr.InTheSameTroop(pFrom, pTo))
		return eDSR_Success;
	else
		return eDSR_TargetIsNotRaidmates;
}

uint32 CTestTargetInCastingProcess::Test(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, const CFighterDictator* pFrom, const CFighterDictator* pTo) const
{
	if (!pTo)
		return eDSR_Fail;

	CCastingProcess* pCast = pTo->GetSkillMgr()->GetCurCasting();
	if(!pCast)
	{
		return eDSR_Fail;
	}

	CNormalSkillServer* pSkill = pCast->GetNormalSkill();
	Ast(pSkill);
	EAttackType eAttackType = pSkill->GetAttackType();
	const string& sArg = Arg.GetString();
	if(sArg == "" || eAttackType == CSkillCfg::ms_mapAttackType[sArg])
	{
		return eDSR_Success;
	}
	else
	{
		return eDSR_Fail;
	}
}

uint32 CTestTargetIsMoving::Test(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, const CFighterDictator* pFrom, const CFighterDictator* pTo) const
{
	if (pTo == NULL)
	{
		return eDSR_Fail;
	}
	if (pTo->IsMoving())
	{
		return eDSR_Success;
	}
	return eDSR_Fail;
}

uint32 CTestTargetIsMoveMagic::Test(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, const CFighterDictator* pFrom, const CFighterDictator* pTo) const
{
	if (!pTo)
		return eDSR_Fail;
	if (pTo->IsExistMoveMgr())
	{
		return eDSR_Success;
	}
	return eDSR_Fail;
}

uint32 CTestTargetNotIsMoveMagic::Test(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, const CFighterDictator* pFrom, const CFighterDictator* pTo) const
{
	if (!pTo)
		return eDSR_Fail;
	if (!pTo->IsExistMoveMgr())
	{
		return eDSR_Success;
	}
	return eDSR_Fail;
}

uint32 CTestTargetIsCamp::Test(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, const CFighterDictator* pFrom, const CFighterDictator* pTo) const
{
	if (!pTo)
		return eDSR_Fail;
	if (pTo->CppGetBirthCamp() == Arg.GetIntValue())
	{
		return eDSR_Success;
	}
	return eDSR_Fail;
}

uint32 CTestRidingAureMagicMCOND::Test(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, const CFighterDictator* pFrom, const CFighterDictator* pTo) const
{
	if (!pTo)
		return eDSR_Fail;
	const string& StanceName = Arg.GetString();
	CAureMagicCfgServerSharedPtr& cfg = CAureMagicCfgServer::GetAureMagic(StanceName);
	CAureMagicServer::CancelTheExistedStance(cfg, const_cast<CFighterDictator*>(pTo));
	return eDSR_Success;
}

uint32 CTestBurstSoulTimesGreaterOrEqualMCOND::Test(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, const CFighterDictator* pFrom, const CFighterDictator* pTo) const
{
	if (!const_cast<CFighterDictator*>(pTo)->CastToFighterMediator())
		return eDSR_Fail;
	CBurstSoulMgrServer* pBurstSoulMgrServer = const_cast<CFighterDictator*>(pTo)->CastToFighterMediator()->GetBurstSoulMgrServer();
	uint32 uBurstSoulTimes = pBurstSoulMgrServer->GetBurstSoulTimes();
	uint32 uLimit = pBurstSoulMgrServer->GetNeedBurstTimesByLevel(Arg.GetIntValue());

	if(uBurstSoulTimes >= uLimit)
	{
		return eDSR_Success;
	}

	return eDSR_Fail;
}

uint32 CTestContinuousBurstSoulTimesEqualMCOND::Test(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, const CFighterDictator* pFrom, const CFighterDictator* pTo) const
{
	if (!const_cast<CFighterDictator*>(pTo)->CastToFighterMediator())
		return eDSR_Fail;
	CBurstSoulMgrServer* pBurstSoulMgrServer = const_cast<CFighterDictator*>(pTo)->CastToFighterMediator()->GetBurstSoulMgrServer();
	uint32 uBurstSoulTimes = pBurstSoulMgrServer->GetContinuousBurstTimes();
	uint32 uLimitTimes = Arg.GetIntValue(pTo);
	if(uBurstSoulTimes == uLimitTimes)
	{
		return eDSR_Success;
	}

	return eDSR_Fail;
}

uint32 CTestTargetHavePositionMagic::Test(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, const CFighterDictator* pFrom, const CFighterDictator* pTo) const
{
	if (!pTo)
		return eDSR_Fail;
	CMagicMgrServer* pMagicMgr= pTo->GetMagicMgr();
	if(pMagicMgr)
	{
		CPositionMagicServer* pPosMagic = pMagicMgr->GetPositionMagic();
		if(pPosMagic && pPosMagic->GetCfgSharedPtr()->GetName() == Arg.GetString())
		{
			return eDSR_Success;
		}
	}
	return eDSR_Fail;
}

uint32 CTestExpIsVestInSelfAndTriggerStateMCOND::Test(CSkillInstServer *pSkillInst, const CCfgArg* pArg, const CFighterDictator *pFrom, const CFighterDictator *pTo) const
{
	if (!pTo)
		return eDSR_Fail;
	uint32 uExpOwnerID = pTo->GetCharacter()->GetExpOwnerID();
	CFighterDictator* pFighter = NULL;
	CRelationMgrServer& relationMgr = CRelationMgrServer::Inst();
	if(!relationMgr.IsNotJoinTeam(pFrom))
	{
		CCharacterDictator* pCharacter = CCharacterDictator::GetCharacterByID(uExpOwnerID);
		pFighter = pCharacter?pCharacter->GetFighter():NULL;
	}
	if (uExpOwnerID == pFrom->GetEntityID() || (pFighter && relationMgr.InTheSameTeam(pFrom, pFighter)))
	{
		CTestTriggerStateMCOND TestTriggerStateMCOND;
		return TestTriggerStateMCOND.Test(pSkillInst,pArg,pFrom,pTo);
	}
	return eDSR_Fail;
}

CCfgArg* CTestExpIsVestInSelfAndTriggerStateMCOND::InitArg(const string& szArg)
{
	return new CTriggerStateCfgArg(&CTriggerStateCfgServer::Get(szArg));
}

uint32 CTestTargetLearnSkillMCOND::Test(CSkillInstServer *pSkillInst, const CCfgCalc& Arg, const CFighterDictator *pFrom, const CFighterDictator *pTo) const
{
	if (pTo&&pTo->IsSkillActive(Arg.GetString(),1))
	{
		return eDSR_Success;	
	}
	return eDSR_Fail;
}

uint32 CTestTargetLevelLesserOrEqualToExpOwnerMCOND::Test(CSkillInstServer *pSkillInst, const CCfgCalc& Arg, const CFighterDictator *pFrom, const CFighterDictator *pTo) const
{
	CCharacterDictator* pChar = pTo->GetCharacter();
	uint32 uTargetID = pChar?pChar->GetExpOwnerID():0;
	CCharacterDictator* pCharacter = CCharacterDictator::GetCharacterByID(uTargetID);
	CFighterDictator* pTarget = pCharacter?pCharacter->GetFighter():NULL;
	if (pTarget == NULL || !pTarget->CastToFighterMediator())
		return eDSR_Fail;
	uint32 uDiffLevel = (uint32)Arg.GetIntValue();
	if (pTarget->CppGetLevel() > pTo->CppGetLevel()+uDiffLevel)
		return eDSR_Success;
	return eDSR_Fail;
}

uint32 CTestTargetSkillIsCoolDownMCOND::Test(CSkillInstServer *pSkillInst, const CCfgCalc& Arg, const CFighterDictator *pFrom, const CFighterDictator *pTo) const
{
	const string& SkillName = Arg.GetString();
	const CNormalSkillServerSharedPtr& pSkill = CNormalSkillServer::GetSkillByName(SkillName);	
	if (!pSkill)
	{
		return eDSR_Success;
	}
	ESkillCoolDownType eCoolDownType = pSkill->GetCoolDownType();
	return pTo->GetCoolDownMgr()->IsCoolDown(SkillName.c_str(),eCoolDownType)?eDSR_Success:eDSR_Fail;
}

uint32 CTestTargetInScene::Test(CSkillInstServer *pSkillInst, const CCfgCalc& Arg, const CFighterDictator *pFrom, const CFighterDictator *pTo) const
{
	CCharacterDictator* pCharacter = pTo?pTo->GetCharacter():NULL;
	if (pCharacter && const_cast<CFighterDictator*>(pTo)->CastToFighterMediator())
	{
		CCoreSceneServer* pScene = pCharacter->GetScene();
		if (pScene->GetSceneName() != Arg.GetString(0))
		{
			return eDSR_Success;
		}
		if (pScene->GetSceneName() == Arg.GetString(0))
		{
			if(!const_cast<CFighterDictator*>(pTo)->CastToFighterMediator()->GetCallBackHandler()->InSceneArea(pCharacter->CastToCharacterMediator(), Arg.GetString(1).c_str()))
				return eDSR_Success;
		}
	}
	return eDSR_NowSceneForbidUse;
}
