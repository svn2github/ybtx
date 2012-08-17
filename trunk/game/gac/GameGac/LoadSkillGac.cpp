#include "stdafx.h"
#include "CMagicEffClient.h"
#include "GameCommon.h"
#include "CSkillClient.h"
#include "TPropertyHolder.h"
#include "TPropertyHolder.inl"
#include "CFighterDirector.h"
#include "CFighterViewInfo.h"
#include "CFighterFollower.h"
#include "CScriptAppClient.h"
#include "CCfgCalcFunction.h"
#include "CCastingProccessClient.h"
#include "CMagicStateClient.h"
#include "COtherStateClient.h"
#include "CTplStateCfgClient.inl"
#include "CBulletMagicClient.h"
#include "CMoveMagicClient.h"
#include "CPositionMagicClient.h"
#include "CShockWaveMagicClient.h"
#include "CBarrierMagicClient.h"
#include "CTransferableMagicClient.h"
#include "CBattleArrayMagicClient.h"
#include "CMagicOpClient.h"
#include "CNormalAttackCfgClient.h"
#include "CAureMagicClient.h"
#include "CPropertyCfg.h"
#include "CFacialCfg.h"
#include "CActorCfgClient.h"
#include "CFacialAniCfgMgr.h"
#include "CSoundConfig.h"
#include "CAppConfigClient.h"
#include "CRenderFont.h"
#include "CLangCfg.h"
#include "CFontMgr.h"
#include "CBurstSoulCfgClient.h"
#include "StringHelper.h"
#include "CGraphic.h"
#include "CDisplayHurt.h"
#include "CModelInfoClient.h"
#include "CodeCvs.h"

CRenderFont*						  g_pFont = NULL;
void PrepareDirectorCalProperties()
{
	typedef TPropertyHolder<CFighterFollower> CPropertyHolder;
#define CreateAdaptersForDirectorMultiplierCalProperty(Name) \
	CPropertyHolder::CreateAdapter<CPropertyHolder::TImpValueAdapter<CFighterSyncToDirectorCalInfo,C##Name,&CFighterSyncToDirectorCalInfo::m_##Name> >(CPropertyHolder::GetMapAdapter(), CPropertyHolder::GetVecAdapter(), CPropertyHolder::GetVecName(), ePID_##Name * ePFT_Count + ePFT_Value);\
	CPropertyHolder::CreateAdapter<CPropertyHolder::TImpMultiplierAdapter<CFighterSyncToDirectorCalInfo,C##Name,&CFighterSyncToDirectorCalInfo::m_##Name> >(CPropertyHolder::GetMapAdapter(), CPropertyHolder::GetVecAdapter(), CPropertyHolder::GetVecName(), ePID_##Name * ePFT_Count + ePFT_Multiplier);

	CreateAdaptersForDirectorMultiplierCalProperty(MPConsumeRate);
	CreateAdaptersForDirectorMultiplierCalProperty(RPConsumeRate);
	CreateAdaptersForDirectorMultiplierCalProperty(EPConsumeRate);

	CreateAdaptersForDirectorMultiplierCalProperty(NatureMPConsumeRate);
	CreateAdaptersForDirectorMultiplierCalProperty(DestructionMPConsumeRate);
	CreateAdaptersForDirectorMultiplierCalProperty(EvilMPConsumeRate);

#undef CreateAdaptersForDirectorMultiplierCalProperty
}

void PrepareViewInfoCalProperties()
{
	typedef TPropertyHolder<CFighterViewInfo> CPropertyHolder;
#define CreateAdaptersForCalProperty( Name ) \
	CPropertyHolder::CreateAdapter<CPropertyHolder::TImpValueAdapter<CFighterCalInfo,C##Name,&CFighterCalInfo::m_##Name> >(CPropertyHolder::GetMapAdapter(), CPropertyHolder::GetVecAdapter(), CPropertyHolder::GetVecName(), ePID_##Name * ePFT_Count + ePFT_Value);\
	CPropertyHolder::CreateAdapter<CPropertyHolder::TImpAdderAdapter<CFighterCalInfo,C##Name,&CFighterCalInfo::m_##Name> >(CPropertyHolder::GetMapAdapter(), CPropertyHolder::GetVecAdapter(), CPropertyHolder::GetVecName(), ePID_##Name * ePFT_Count + ePFT_Adder);\
	CPropertyHolder::CreateAdapter<CPropertyHolder::TImpMultiplierAdapter<CFighterCalInfo,C##Name,&CFighterCalInfo::m_##Name> >(CPropertyHolder::GetMapAdapter(), CPropertyHolder::GetVecAdapter(), CPropertyHolder::GetVecName(), ePID_##Name * ePFT_Count + ePFT_Multiplier);

	CreateAdaptersForCalProperty(Defence);
	CreateAdaptersForCalProperty(PhysicalDPS)
	CreateAdaptersForCalProperty(StrikeMultiValue);
	CreateAdaptersForCalProperty(StrikeValue);
	CreateAdaptersForCalProperty(PhysicalDodgeValue);
	CreateAdaptersForCalProperty(MagicDodgeValue);
	CreateAdaptersForCalProperty(MagicHitValue);
	CreateAdaptersForCalProperty(ParryValue);
	CreateAdaptersForCalProperty(ResilienceValue);
	CreateAdaptersForCalProperty(StrikeResistanceValue);
	CreateAdaptersForCalProperty(AccuratenessValue);
	CreateAdaptersForCalProperty(MagicDamageValue);
	CreateAdaptersForCalProperty(NatureResistanceValue);
	CreateAdaptersForCalProperty(DestructionResistanceValue);
	CreateAdaptersForCalProperty(EvilResistanceValue);
	CreateAdaptersForCalProperty(BlockDamage);
	CreateAdaptersForCalProperty(PunctureDamage);
	CreateAdaptersForCalProperty(ChopDamage);
	CreateAdaptersForCalProperty(BluntDamage);
	CreateAdaptersForCalProperty(NatureDamageValue);
	CreateAdaptersForCalProperty(DestructionDamageValue);
	CreateAdaptersForCalProperty(EvilDamageValue);
	CreateAdaptersForCalProperty(MainHandMinWeaponDamage);
	CreateAdaptersForCalProperty(MainHandMaxWeaponDamage);
	CreateAdaptersForCalProperty(MHWeaponIntervalExtraDamageRate);
	CreateAdaptersForCalProperty(AHWeaponIntervalExtraDamageRate);
	CreateAdaptersForCalProperty(PenetrationValue);
	CreateAdaptersForCalProperty(ProtectionValue);
	CreateAdaptersForCalProperty(NatureDecreaseResistanceValue);
	CreateAdaptersForCalProperty(DestructionDecreaseResistanceValue);
	CreateAdaptersForCalProperty(EvilDecreaseResistanceValue);
	CreateAdaptersForCalProperty(DefencePenetrationValue);

#undef CreateAdaptersForCalProperty

#define CreateAdaptersForMultiplierCalProperty( Name ) \
	CPropertyHolder::CreateAdapter<CPropertyHolder::TImpValueAdapter<CFighterCalInfo,C##Name,&CFighterCalInfo::m_##Name> >(CPropertyHolder::GetMapAdapter(), CPropertyHolder::GetVecAdapter(), CPropertyHolder::GetVecName(), ePID_##Name * ePFT_Count + ePFT_Value);\
	CPropertyHolder::CreateAdapter<CPropertyHolder::TImpMultiplierAdapter<CFighterCalInfo,C##Name,&CFighterCalInfo::m_##Name> >(CPropertyHolder::GetMapAdapter(), CPropertyHolder::GetVecAdapter(), CPropertyHolder::GetVecName(), ePID_##Name * ePFT_Count + ePFT_Multiplier);

	CreateAdaptersForMultiplierCalProperty(BlockRate);
	CreateAdaptersForMultiplierCalProperty(MissRate);
	CreateAdaptersForMultiplierCalProperty(ExtraStrikeRate);
	CreateAdaptersForMultiplierCalProperty(ExtraPhysicDefenceRate);
	CreateAdaptersForMultiplierCalProperty(ExtraPhysicDodgeRate);
	CreateAdaptersForMultiplierCalProperty(ExtraParryRate);
	CreateAdaptersForMultiplierCalProperty(ExtraMagicDodgeRate);
	CreateAdaptersForMultiplierCalProperty(ExtraMagicResistanceRate);
	CreateAdaptersForMultiplierCalProperty(ExtraNatureResistanceRate);
	CreateAdaptersForMultiplierCalProperty(ExtraDestructionResistanceRate);
	CreateAdaptersForMultiplierCalProperty(ExtraEvilResistanceRate);
	CreateAdaptersForMultiplierCalProperty(ExtraCompleteResistanceRate);

#undef CreateAdaptersForMultiplierCalProperty
}

bool LoadSkillGac()
{
	string path;
	if ( CBaseScriptApp::Inst()->GetIsCfgPkgUsed() )
	{
		path = "cfg";
	}
	else
	{
		path  =  utf8_to_gbk(CBaseScriptApp::Inst()->GetCfgFilePath(""));
	}

	string pathSkill = path + "/skill/";
	string pathPlayer = path + "/player/";
	string strPathAction = path + "/action/";
	string strPathSound = path + "/sound/";
	string strPathRes= path + "/model_res/";

	string skill = pathSkill + "Skill.txt";
	string npcskill = pathSkill + "NPCSkill.txt";
	string intObjSkill = pathSkill + "IntObjSkill.txt";
	string strPlayerMagicEffect	= pathSkill + "PlayerMagicEffect.txt";
	string strPlayerActivityMagicEffect	= pathSkill + "PlayerActivityMagicEffect.txt";
	string strNpcMagicEffect	= pathSkill + "NpcMagicEffect.txt";
	string strNpcActivityMagicEffect	= pathSkill + "NpcActivityMagicEffect.txt";
	string strNpcGameMagicEffect	= pathSkill + "NpcGameMagicEffect.txt";
	string castingProcess  = pathSkill + "CastingProcess.txt";
	string magicState = pathSkill + "MagicState.txt";
	string bulletMagic = pathSkill + "BulletMagic.txt";
	string moveMagic = pathSkill + "MoveMagic.txt";
	string transMagic = pathSkill + "TransferableMagic.txt";
	string shockWaveMagic = pathSkill + "ShockWaveMagic.txt";
	string aureMagic = pathSkill + "AureMagic.txt";
	string barrierMagic = pathSkill + "BarrierMagic.txt";
	string positionMagic = pathSkill + "PositionMagic.txt";
	string battleArrayMagic = pathSkill + "BattleArrayMagic.txt";
	string triggerState = pathSkill + "TriggerState.txt";
	string damageState = pathSkill + "DamageChangeState.txt";
	string cumuliTriggerState = pathSkill + "CumuliTriggerState.txt";
	string specialState = pathSkill + "SpecialState.txt";
	string cfgGlobal = pathSkill + "CfgGlobal.txt";
	string normalAttack = pathSkill + "NormalAttack.txt";
	string npcNormalAttack = pathSkill + "NPCNormalAttack.txt";

	string playerProperty = pathPlayer+"PlayerProperty.txt";
	string facialAni = pathPlayer + "FacialAni_Client.txt";

	string strAction = strPathAction + "Action_Common.txt";
	string strFicialAniName = strPathAction + "FaceAction_Common.txt";

	string strPlayerSound = strPathSound + "PlayerSound_Client.txt";
	string strNPCSound = strPathSound + "NPCSound_Client.txt";

	string burstSoul = pathSkill + "BurstSoul.txt";

	string strModelInfo = strPathRes + "PropRes.txt";

	//读取所有配置表 注意施法流程要放在技能前面
	if(!CCfgGlobal::LoadConfig(cfgGlobal.c_str()) ||
		!CPropertyCfg::LoadConfig(playerProperty.c_str()) ||
		!CMagicEffClient::LoadMagicEff(strPlayerMagicEffect.c_str(), true)||
		!CMagicEffClient::LoadMagicEff(strPlayerActivityMagicEffect.c_str(), false)||
		!CMagicEffClient::LoadMagicEff(strNpcMagicEffect.c_str(), false)||
		!CMagicEffClient::LoadMagicEff(strNpcActivityMagicEffect.c_str(), false)||
		!CMagicEffClient::LoadMagicEff(strNpcGameMagicEffect.c_str(), false)||
		!CMagicOpCfgClient::LoadConfig(strPlayerMagicEffect.c_str(), true)||
		!CMagicOpCfgClient::LoadConfig(strPlayerActivityMagicEffect.c_str(), false)||
		!CMagicOpCfgClient::LoadConfig(strNpcMagicEffect.c_str(), false)||
		!CMagicOpCfgClient::LoadConfig(strNpcActivityMagicEffect.c_str(), false)||
		!CMagicOpCfgClient::LoadConfig(strNpcGameMagicEffect.c_str(), false)||
		!CCastingProcessCfgClient::LoadConfig(castingProcess.c_str()) ||
		!CSkillClient::LoadSkillConfig(skill.c_str()) ||
		!CSkillClient::LoadNPCSkillConfig(npcskill.c_str()) ||
		!CSkillClient::LoadIntObjSkillConfig(intObjSkill.c_str()) ||
		!CAureMagicCfgClient::LoadAureMagicConfig(aureMagic.c_str()) ||
		!CTplStateCfgClient<CMagicStateClient>::LoadConfig(magicState.c_str()) ||
		!CTplStateCfgClient<CTriggerStateClient>::LoadConfig(triggerState.c_str()) ||
		!CTplStateCfgClient<CDamageChangeStateClient>::LoadConfig(damageState.c_str()) ||
		!CTplStateCfgClient<CCumuliTriggerStateClient>::LoadConfig(cumuliTriggerState.c_str()) ||
		!CTplStateCfgClient<CSpecialStateClient>::LoadConfig(specialState.c_str()) ||
		!CBaseStateCfgClient::LoadConfig() ||
		!CBulletMagicCfgClient::LoadConfig(bulletMagic.c_str()) ||
		!CMoveMagicCfgClient::LoadConfig(moveMagic.c_str()) ||
		!CTransferableMagicCfgClient::LoadConfig(transMagic.c_str()) ||
		!CShockWaveMagicCfgClient::LoadConfig(shockWaveMagic.c_str())||
		!CBarrierMagicCfgClient::LoadConfig(barrierMagic.c_str())||
		!CPositionMagicCfgClient::LoadConfig(positionMagic.c_str()) ||
		!CBattleArrayMagicCfgClient::LoadConfig(battleArrayMagic.c_str()) ||
		!CNormalAttackCfgClient::LoadNormalAttackConfig(normalAttack.c_str()) ||
		!CNormalAttackCfgClient::LoadNpcNormalAttackConfig(npcNormalAttack.c_str()) ||
		!CFacialAniCfgclient::LoadConfig(facialAni.c_str()) ||
		!CActorCfgClient::LoadConfig(strAction.c_str()) ||
		!CFacialAniCfgMgr::LoadConfig(strFicialAniName.c_str()) ||
		!CSoundConfig::LoadConfig(strPlayerSound.c_str(), strNPCSound.c_str()) ||
		!CBurstSoulCfgClient::LoadConfig(burstSoul.c_str()) ||
		!CModelInfoClient::LoadConfig(strModelInfo.c_str()) ||
		CfgChk::ExistError())
	{
		if(!CfgChk::ExpIsForbidden())
		{
			CfgChk::SetTabNull();
			return false;
		}
	}

	CfgChk::SetTabNull();
	
	TPropertyHolder<CFighterFollower>::ReserveVectors();
	TPropertyHolder<CFighterFollower>::PrepareAgileProperties();
	PrepareDirectorCalProperties();
	TPropertyHolder<CFighterViewInfo>::ReserveVectors();
	PrepareViewInfoCalProperties();
	CGraphic::GetInst()->CreateFont(CFontMgr::DISTANCE_FONT_ID, false, 0.0f, &g_pFont);	
	return true;
}

bool ReloadSkillCfg(TCHAR* szFileName)
{
	string strFileName = szFileName;
	int pos = (int)strFileName.rfind("/");
	string strCfgName = strFileName.substr(pos+1,strFileName.length());
	const char* szCfgName = strCfgName.c_str();

	string path;
	if ( CBaseScriptApp::Inst()->GetIsCfgPkgUsed() )
	{
		path = "cfg";
	}
	else
	{
		path = utf8_to_gbk(CScriptAppClient::Inst()->GetCfgFilePath(""));
	}
	string pathSkill = path+"/skill/";
	string strPathSound = path + "/sound/";
	
	string skill = pathSkill + "Skill.txt";
	string npcskill = pathSkill + "NPCSkill.txt";
	string intObjSkill = pathSkill + "IntObjSkill.txt";
	string strPlayerSound = strPathSound + "PlayerSound_Client.txt";
	string strNPCSound = strPathSound + "NPCSound_Client.txt";
	//重载配置表
	if (strcmp(szCfgName,"Skill.txt") == 0 || strcmp(szCfgName,"NPCSkill.txt") == 0 || strcmp(szCfgName,"IntObjSkill.txt") == 0)
	{
		CSkillClient::UnloadSkillConfig();
		CSkillClient::LoadSkillConfig(skill);
		CSkillClient::LoadNPCSkillConfig(npcskill);
		CSkillClient::LoadIntObjSkillConfig(intObjSkill);
	}
	else if (strcmp(szCfgName,"PlayerMagicEffect.txt") == 0 || strcmp(szCfgName,"PlayerActivityMagicEffect.txt") == 0 || 
		strcmp(szCfgName,"NpcMagicEffect.txt") == 0 || strcmp(szCfgName,"NpcActivityMagicEffect.txt") == 0 || 
		strcmp(szCfgName,"NpcGameMagicEffect.txt") == 0)
	{
		CMagicEffClient::UnloadMagicEff();
		CMagicEffClient::LoadMagicEff("PlayerMagicEffect.txt", true);
		CMagicEffClient::LoadMagicEff("PlayerActivityMagicEffect.txt", false);
		CMagicEffClient::LoadMagicEff("NpcMagicEffect.txt", false);
		CMagicEffClient::LoadMagicEff("NpcActivityMagicEffect.txt", false);
		CMagicEffClient::LoadMagicEff("NpcGameMagicEffect.txt", false);
		CMagicOpCfgClient::LoadConfig("PlayerMagicEffect.txt", true);
		CMagicOpCfgClient::LoadConfig("PlayerActivityMagicEffect.txt", false);
		CMagicOpCfgClient::LoadConfig("NpcMagicEffect.txt", false);
		CMagicOpCfgClient::LoadConfig("NpcActivityMagicEffect.txt", false);
		CMagicOpCfgClient::LoadConfig("NpcGameMagicEffect.txt", false);
	}
	else if (strcmp(szCfgName,"CastingProcess.txt") == 0)
	{
		CCastingProcessCfgClient::UnloadConfig();
		CCastingProcessCfgClient::LoadConfig(szFileName);
	}
	else if (strcmp(szCfgName,"MagicState.txt") == 0)
	{
		CBaseStateCfgClient::UnloadConfig();
		CTplStateCfgClient<CMagicStateClient>::UnloadConfig();
		CTplStateCfgClient<CMagicStateClient>::LoadConfig(szFileName);
		CBaseStateCfgClient::LoadConfig();
	}
	else if (strcmp(szCfgName,"BulletMagic.txt") == 0)
	{
		CBulletMagicCfgClient::UnloadConfig();
		CBulletMagicCfgClient::LoadConfig(szFileName);
	}
	else if (strcmp(szCfgName,"MoveMagic.txt") == 0)
	{
		CMoveMagicCfgClient::UnloadConfig();
		CMoveMagicCfgClient::LoadConfig(szFileName);
	}
	else if (strcmp(szCfgName,"AureMagic.txt") == 0)
	{
		CAureMagicCfgClient::UnloadAureMagicConfig();
		CAureMagicCfgClient::LoadAureMagicConfig(szFileName);
	}
	else if (strcmp(szCfgName,"BarrierMagic.txt") == 0)
	{
		CBarrierMagicCfgClient::UnloadConfig();
		CBarrierMagicCfgClient::LoadConfig(szFileName);
	}
	else if (strcmp(szCfgName,"TransferableMagic.txt") == 0)
	{
		CTransferableMagicCfgClient::UnloadConfig();
		CTransferableMagicCfgClient::LoadConfig(szFileName);
	}
	else if (strcmp(szCfgName,"ShockWaveMagic.txt") == 0)
	{
		CShockWaveMagicCfgClient::UnloadConfig();
		CShockWaveMagicCfgClient::LoadConfig(szFileName);
	}
	else if (strcmp(szCfgName,"PositionMagic.txt") == 0)
	{
		CPositionMagicCfgClient::UnloadConfig();
		CPositionMagicCfgClient::LoadConfig(szFileName);
	}
	else if (strcmp(szCfgName,"BattleArrayMagic.txt") == 0)
	{
		CBattleArrayMagicCfgClient::UnloadConfig();
		CBattleArrayMagicCfgClient::LoadConfig(szFileName);
	}
	else if (strcmp(szCfgName,"TriggerState.txt") == 0)
	{
		CBaseStateCfgClient::UnloadConfig();
		CTplStateCfgClient<CTriggerStateClient>::UnloadConfig();
		CTplStateCfgClient<CTriggerStateClient>::LoadConfig(szFileName);
		CBaseStateCfgClient::LoadConfig();
	}
	else if (strcmp(szCfgName,"DamageChangeState.txt") == 0)
	{
		CBaseStateCfgClient::UnloadConfig();
		CTplStateCfgClient<CDamageChangeStateClient>::UnloadConfig();
		CTplStateCfgClient<CDamageChangeStateClient>::LoadConfig(szFileName);
		CBaseStateCfgClient::LoadConfig();
	}
	else if (strcmp(szCfgName,"CumuliTriggerState.txt") == 0)
	{
		CBaseStateCfgClient::UnloadConfig();
		CTplStateCfgClient<CCumuliTriggerStateClient>::UnloadConfig();
		CTplStateCfgClient<CCumuliTriggerStateClient>::LoadConfig(szFileName);
		CBaseStateCfgClient::LoadConfig();
	}
	else if (strcmp(szCfgName,"SpecialState.txt") == 0)
	{
		CBaseStateCfgClient::UnloadConfig();
		CTplStateCfgClient<CSpecialStateClient>::UnloadConfig();
		CTplStateCfgClient<CSpecialStateClient>::LoadConfig(szFileName);
		CBaseStateCfgClient::LoadConfig();
	}
	else if (strcmp(szCfgName,"NormalAttack.txt") == 0)
	{
		CNormalAttackCfgClient::UnloadNormalAttackConfig();
		CNormalAttackCfgClient::LoadNormalAttackConfig(szFileName);
	}
	else if (strcmp(szCfgName,"PlayerProperty.txt") == 0)
	{
		CPropertyCfg::UnloadConfig();
		CPropertyCfg::LoadConfig(szFileName);
	}
	else if (strcmp(szCfgName,"BurstSoul.txt") == 0)
	{
		CBurstSoulCfgClient::UnloadConfig();
		CBurstSoulCfgClient::LoadConfig(szFileName);
	}
	else if (strcmp(szCfgName,"FacialAni_Client.txt") == 0)
	{
		CFacialAniCfgclient::UnloadConfig();
		CFacialAniCfgclient::LoadConfig(szCfgName);
	}
	else if (strcmp(szCfgName,"Action_Common.txt") == 0)
	{
		CActorCfgClient::UnloadConfig();
		CActorCfgClient::LoadConfig(szCfgName);
	}
	else if (strcmp(szCfgName,"FaceAction_Common.txt") == 0)
	{
		CFacialAniCfgMgr::UnloadConfig();
		CFacialAniCfgMgr::LoadConfig(szCfgName);
	}
	else if (strcmp(szCfgName,"PlayerSound_Client.txt") == 0 || strcmp(szCfgName,"NPCSound_Client.txt") == 0)
	{
		CSoundConfig::UnloadConfig();
		CSoundConfig::LoadConfig(strPlayerSound.c_str(), strNPCSound.c_str());
	}

	CfgChk::SetTabNull();
	return true;
}


void UnloadSkillGac()
{
	
	CPropertyCfg::UnloadConfig();
	CSkillClient::UnloadSkillConfig();
	CMagicEffClient::UnloadMagicEff();
	CAureMagicCfgClient::UnloadAureMagicConfig();
	CCfgGlobal::UnloadConfig();
	CMagicOpCfgClient::UnloadConfig();
	CCastingProcessCfgClient::UnloadConfig();
	CTplStateCfgClient<CMagicStateClient>::UnloadConfig();
	CTplStateCfgClient<CTriggerStateClient>::UnloadConfig();
	CTplStateCfgClient<CDamageChangeStateClient>::UnloadConfig();
	CTplStateCfgClient<CCumuliTriggerStateClient>::UnloadConfig();
	CTplStateCfgClient<CSpecialStateClient>::UnloadConfig();
	CBaseStateCfgClient::UnloadConfig();
	CBulletMagicCfgClient::UnloadConfig();
	CMoveMagicCfgClient::UnloadConfig();
	CTransferableMagicCfgClient::UnloadConfig();
	CShockWaveMagicCfgClient::UnloadConfig();
	CBarrierMagicCfgClient::UnloadConfig();
	CPositionMagicCfgClient::UnloadConfig();
	CBattleArrayMagicCfgClient::UnloadConfig();
	CNormalAttackCfgClient::UnloadNormalAttackConfig();
	CFacialAniCfgclient::UnloadConfig();
	CActorCfgClient::UnloadConfig();
	CFacialAniCfgMgr::UnloadConfig();
	CSoundConfig::UnloadConfig();
	CBurstSoulCfgClient::UnloadConfig();
	CModelInfoClient::UnloadConfig();
	TPropertyHolder<CFighterDirector>::UnloadAdapterMap();
	TPropertyHolder<CFighterFollower>::UnloadAdapterMap();
	TPropertyHolder<CFighterViewInfo>::UnloadAdapterMap();
	SafeRelease(g_pFont);
}
