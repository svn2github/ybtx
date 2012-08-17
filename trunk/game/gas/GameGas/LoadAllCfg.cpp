#include "stdafx.h"
#include "LoadAllCfg.h"
#include "CSkillServer.h"
#include "CAureMagicCfgServer.h"
#include "CMagicStateCfg.h"
#include "CTriggerStateCfg.h"
#include "CDamageChangeStateCfg.h"
#include "CCumuliTriggerStateCfg.h"
#include "CSpecialStateCfg.h"
#include "CCastingProcessCfg.h"
#include "CBulletMagicCfgServer.h"
#include "CShockWaveMagicCfgServer.h"
#include "CTransferableMagicCfgServer.h"
#include "CMoveMagicCfgServer.h"
#include "CBarrierMagicCfgServer.h"
#include "CPositionMagicCfgServer.h"
#include "CTotemMagicCfgServer.h"
#include "CBattleArrayMagicCfgServer.h"
#include "TPropertyHolder.h"
#include "TPropertyHolder.inl"
#include "CFighterMediator.h"
#include "CScriptAppServer.h"
#include "CMagicEffServer.h"
#include "CCfgCalcFunction.h"
#include "CNormalAttackCfg.h"
#include "CTriggerEvent.h"
#include "CTalentEffServer.h"
#include "CNpcStateMachine.h"
#include "CNpcStateTransit.h"
#include "CCfgRelationDeal.h"
#include "CPropertyCfg.h"
#include "CAniKeyFrameCfg.h"
#include "CAppServer.h"
#include "TimeHelper.h"
#include "CForceNoSaveState.h"
#include "CPersistentState.h"
#include "CActorCfg.h"
#include "CBurstSoulCfgServer.h"
#include "DebugHelper.h"
#include "CCfgColChecker.h"
#include "CPkgFile.h"
#include "CSkillReplaceServer.h"
#include "CPropertyTransaction.h"
#include "CCpuUsageMgr.h"

void PrepareEquipCalProperties()
{
	typedef TPropertyHolder<CFighterDictator> CPropertyHolder;
#define CreateAdaptersForCalProperty( Name ) \
	CPropertyHolder::CreateAdapter<CPropertyHolder::TImpAdderAdapter<CFighterCalInfo,C##Name,&CFighterCalInfo::m_##Name> >(CPropertyHolder::GetMapAdapter(), CPropertyHolder::GetVecAdapter(), CPropertyHolder::GetVecName(), ePID_##Name * ePFT_Count + ePFT_Adder);\
	CPropertyHolder::CreateAdapter<CPropertyHolder::TImpMultiplierAdapter<CFighterCalInfo,C##Name,&CFighterCalInfo::m_##Name> >(CPropertyHolder::GetMapAdapter(), CPropertyHolder::GetVecAdapter(), CPropertyHolder::GetVecName(), ePID_##Name * ePFT_Count + ePFT_Multiplier);\

	CreateAdaptersForCalProperty(HPUpdateRate);
	CreateAdaptersForCalProperty(MPUpdateRate);
	CreateAdaptersForCalProperty(RevertPer);
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
	CreateAdaptersForCalProperty(AssistantMinWeaponDamage);
	CreateAdaptersForCalProperty(AssistantMaxWeaponDamage);
	CreateAdaptersForCalProperty(PenetrationValue);
	CreateAdaptersForCalProperty(ProtectionValue);
	CreateAdaptersForCalProperty(NatureDecreaseResistanceValue);
	CreateAdaptersForCalProperty(DestructionDecreaseResistanceValue);
	CreateAdaptersForCalProperty(EvilDecreaseResistanceValue);
	CreateAdaptersForCalProperty(DefencePenetrationValue);
	CreateAdaptersForCalProperty(MainHandWeaponInterval);
	CreateAdaptersForCalProperty(MHWeaponIntervalExtraDamageRate);
	CreateAdaptersForCalProperty(AssistantWeaponInterval);
	CreateAdaptersForCalProperty(AHWeaponIntervalExtraDamageRate);
#undef CreateAdaptersForCalProperty

	typedef TPropertyHolder<CFighterDictator> CPropertyHolder;
#define CreateAdaptersForMultiplierProperty( Name ) \
	CPropertyHolder::CreateAdapter<CPropertyHolder::TImpMultiplierAdapter<CFighterCalInfo,C##Name,&CFighterCalInfo::m_##Name> >(CPropertyHolder::GetMapAdapter(), CPropertyHolder::GetVecAdapter(), CPropertyHolder::GetVecName(), ePID_##Name * ePFT_Count + ePFT_Multiplier);\

	CreateAdaptersForMultiplierProperty(BlockRate);

#undef CreateAdaptersForMultiplierProperty
}

void PrepareCCfgCalcCalProperties()
{
	typedef TPropertyHolder<CFighterDictator> CPropertyHolder;
#define CreateAdaptersForCalProperty( Name ) \
	CPropertyHolder::CreateAdapter<CPropertyHolder::TImpValueAdapter<CFighterCalInfo,C##Name,&CFighterCalInfo::m_##Name> >(CPropertyHolder::GetMapAdapter(),CPropertyHolder::GetVecAdapter(), CPropertyHolder::GetVecName(), ePID_##Name * ePFT_Count + ePFT_Value);
	CreateAdaptersForCalProperty(MHWeaponIntervalExtraDamageRate);
	CreateAdaptersForCalProperty(AHWeaponIntervalExtraDamageRate);
	CreateAdaptersForCalProperty(MainHandWeaponInterval);
	CreateAdaptersForCalProperty(AssistantWeaponInterval);
	CreateAdaptersForCalProperty(MainHandMinWeaponDamage);
	CreateAdaptersForCalProperty(MainHandMaxWeaponDamage);
	CreateAdaptersForCalProperty(RunSpeed);
	CreateAdaptersForCalProperty(HPUpdateRate);
	CreateAdaptersForCalProperty(MPUpdateRate);
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
	CreateAdaptersForCalProperty(BlockRate);
	CreateAdaptersForCalProperty(BlockDamage);
#undef CreateAdaptersForCalProperty
}

bool LoadSkillCfg()
{
	CCpuUsageMgr::Init();
	CThreadCpuUsageMgr::Init();

	string path;
	if ( CBaseScriptApp::Inst()->GetIsCfgPkgUsed()  )
	{
		path = "cfg";
	}
	else
	{
		path  =  utf8_to_gbk(CBaseScriptApp::Inst()->GetCfgFilePath(""));
	}

	string pathSkill = path + "/skill/";
	string pathPlayer = path + "/player/";
	string pathNpc = path + "/model_res/";

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
	string aureMagic = pathSkill + "AureMagic.txt";
	string barrierMagic = pathSkill + "BarrierMagic.txt";
	string transMagic = pathSkill + "TransferableMagic.txt";
	string shockWaveMagic = pathSkill + "ShockWaveMagic.txt";
	string positionMagic = pathSkill + "PositionMagic.txt";
	string totemMagic = pathSkill + "TotemMagic.txt";
	string battleArrayMagic = pathSkill + "BattleArrayMagic.txt";
	string triggerState = pathSkill + "TriggerState.txt";
	string damageState = pathSkill + "DamageChangeState.txt";
	string cumuliTriggerState = pathSkill + "CumuliTriggerState.txt";
	string specialState = pathSkill + "SpecialState.txt";
	string cfgGlobal = pathSkill + "CfgGlobal.txt";
	string normalAttack = pathSkill + "NormalAttack.txt";
	string NPCNormalAttack = pathSkill + "NPCNormalAttack.txt";
	string talentEffect = pathSkill + "TalentEffect.txt";
	string strAniKeyFrame = pathSkill + "AniKeyFrame.txt";
	string playerProperty = pathPlayer+"PlayerProperty.txt";

	string forceNoSaveState = pathSkill + "ForceNoSaveState.txt";
	string persistentState = pathSkill + "PersistentState.txt";

	string burstSoul = pathSkill + "BurstSoul.txt";
	string skillReplace = pathSkill + "SkillReplace.txt";

	cout << "[" << (CfgChk::g_uTimeForTest = GetProcessTime()) << "]技能配置表读取开始";
	uint64 uTimeForSkillAllTest = CfgChk::g_uTimeForTest;

	//读取所有配置表
	if(	!CCfgGlobal::LoadConfig(cfgGlobal.c_str()) || 
		!CPropertyCfg::LoadConfig(playerProperty.c_str()) ||
		!CMagicEffServer::LoadMagicEff(strPlayerMagicEffect.c_str(),false,true) ||
		!CMagicEffServer::LoadMagicEff(strPlayerActivityMagicEffect.c_str(),false,false) ||
		!CMagicEffServer::LoadMagicEff(strNpcMagicEffect.c_str(),false,false) ||
		!CMagicEffServer::LoadMagicEff(strNpcActivityMagicEffect.c_str(),false,false) ||
		!CMagicEffServer::LoadMagicEff(strNpcGameMagicEffect.c_str(),false,false) ||
		!CCastingProcessCfgServer::LoadConfig(castingProcess.c_str()) ||
		!CNormalSkillServer::LoadSkillConfig(skill.c_str()) ||	
		!CNormalSkillServer::LoadNPCSkillConfig(npcskill.c_str()) ||	
		!CNormalSkillServer::LoadIntObjSkillConfig(intObjSkill.c_str()) ||	
		!CAureMagicCfgServer::LoadAureMagicConfig(aureMagic.c_str())||
		!CMagicStateCfgServer::LoadConfig(magicState.c_str()) ||
		!CTriggerStateCfgServer::LoadConfig(triggerState.c_str()) ||
		!CDamageChangeStateCfgServer::LoadConfig(damageState.c_str()) ||
		!CCumuliTriggerStateCfgServer::LoadConfig(cumuliTriggerState.c_str()) ||
		!CSpecialStateCfgServer::LoadConfig(specialState.c_str()) ||
		!CBaseStateCfgServer::LoadConfig() ||
		!CForceNoSaveStateCfgServer::LoadConfig(forceNoSaveState.c_str()) ||
		/*!CPersistentStateCfgServer::LoadConfig(persistentState.c_str()) ||*/
		!CBulletMagicCfgServer::LoadBulletMagicConfig(bulletMagic.c_str()) ||
		!CMoveMagicCfgServer::LoadMoveMagicConfig(moveMagic.c_str()) ||
		!CBarrierMagicCfgServer::LoadBarrierMagicConfig(barrierMagic.c_str()) ||
		!CTransferableMagicCfgServer::LoadTransferableMagicConfig(transMagic.c_str()) ||
		!CShockWaveMagicCfgServer::LoadShockWaveMagicConfig(shockWaveMagic.c_str())||
		!CPositionMagicCfgServer::LoadPositionMagicConfig(positionMagic.c_str()) ||
		!CTotemMagicCfgServer::LoadTotemMagicConfig(totemMagic.c_str()) ||
		!CBattleArrayMagicCfgServer::LoadBattleArrayMagicConfig(battleArrayMagic.c_str()) ||
		!CNormalAttackCfg::LoadNormalAttackConfig(normalAttack.c_str()) ||
		!CNormalAttackCfg::LoadNPCNormalAttackConfig(NPCNormalAttack.c_str()) ||
		!CTalentEffCfgServer::LoadTalentEffCfgConfig(talentEffect.c_str()) ||
		!CAniKeyFrameCfg::LoadAniKeyFrameConfig(strAniKeyFrame.c_str()) ||
		!CBurstSoulCfgServer::LoadConfig(burstSoul.c_str()) ||
		!CSkillReplace::LoadConfig(skillReplace.c_str()) ||
		!CCfgRelationDeal::RelationDeal() ||
		CfgChk::ExistError()
		)
	{
		if(!CfgChk::ExpIsForbidden())
		{
			CfgChk::SetTabNull();
			return false;
		}
	}

	CfgChk::SetTabNull();
	CActorCfg::BuildMapString2Enum();

	uint64 uCurTime = GetProcessTime();
	cout << "\t耗时" << uCurTime - CfgChk::g_uTimeForTest << "毫秒\n[" << uCurTime << "]技能配置表读取完毕!";
	cout << "\t总耗时" << uCurTime - uTimeForSkillAllTest << "毫秒\n";
	
	uint64 uTimeForInitMagicOpArg = GetProcessTime();
	CMagicEffServer::InitMagicOpArg();
	uCurTime = GetProcessTime();
	cout << "初始化魔法效果表操作参数耗时:" << uCurTime - uTimeForInitMagicOpArg << "毫秒" << endl;

	TPropertyHolder<CFighterDictator>::ReserveVectors();
    TPropertyHolder<CFighterDictator>::PrepareAgileProperties();
	PrepareCCfgCalcCalProperties();
	PrepareEquipCalProperties();

	CFighterDictator::BuildCalPropertyTable();
	CPropertyTransactionMgr::BuildBasePropertyTable();



    return true;
}	

bool ReloadSkillCfg(TCHAR* szFileName)
{
	string strFileName = szFileName;
	int pos = (int)strFileName.rfind("/");
	string strCfgName = strFileName.substr(pos+1,strFileName.length());
	const char* szCfgName = strCfgName.c_str();

	string path  = utf8_to_gbk(CScriptAppServer::Inst()->GetCfgFilePath(""));

	string pathSkill = path+"/skill/";
	string skill = pathSkill + "Skill.txt";
	string npcskill = pathSkill + "NPCSkill.txt";
	string intObjSkill = pathSkill + "IntObjSkill.txt";

	//重载配置表
	if (strcmp(szCfgName,"Skill.txt") == 0 || strcmp(szCfgName,"NPCSkill.txt") == 0 || strcmp(szCfgName,"IntObjSkill.txt") == 0)
	{
		CNormalSkillServer::UnloadSkillConfig();
		CNormalSkillServer::LoadSkillConfig(skill);
		CNormalSkillServer::LoadNPCSkillConfig(npcskill);
		CNormalSkillServer::LoadIntObjSkillConfig(intObjSkill);
	}
	else if (strcmp(szCfgName,"PlayerMagicEffect.txt") == 0)
	{
		CMagicEffServer::LoadMagicEff(szFileName, true, true);
	}
	else if (strcmp(szCfgName,"PlayerActivityMagicEffect.txt") == 0 ||
		strcmp(szCfgName,"NpcMagicEffect.txt") == 0 ||
		strcmp(szCfgName,"NpcActivityMagicEffect.txt") == 0 ||
		strcmp(szCfgName,"NpcGameMagicEffect.txt") == 0)
	{
		CMagicEffServer::LoadMagicEff(szFileName,true,false);
	}
	else if (strcmp(szCfgName,"CastingProcess.txt") == 0)
	{
		CCastingProcessCfgServer::LoadConfig(szFileName,true);
	}
	else if (strcmp(szCfgName,"MagicState.txt") == 0)
	{
		CBaseStateCfgServer::UnloadConfig();
		CMagicStateCfgServer::UnloadConfig();
		CMagicStateCfgServer::LoadConfig(szFileName);
		CBaseStateCfgServer::LoadConfig();
	}
	else if (strcmp(szCfgName,"BulletMagic.txt") == 0)
	{
		CBulletMagicCfgServer::UnloadBulletMagicConfig();
		CBulletMagicCfgServer::LoadBulletMagicConfig(szFileName);
	}
	else if (strcmp(szCfgName,"MoveMagic.txt") == 0)
	{
		CMoveMagicCfgServer::UnloadMoveMagicConfig();
		CMoveMagicCfgServer::LoadMoveMagicConfig(szFileName);
	}
	else if (strcmp(szCfgName,"AureMagic.txt") == 0)
	{
		CAureMagicCfgServer::UnloadAureMagicConfig();
		CAureMagicCfgServer::LoadAureMagicConfig(szFileName);
	}
	else if (strcmp(szCfgName,"BarrierMagic.txt") == 0)
	{
		CBarrierMagicCfgServer::UnloadBarrierMagicConfig();
		CBarrierMagicCfgServer::LoadBarrierMagicConfig(szFileName);
	}
	else if (strcmp(szCfgName,"TransferableMagic.txt") == 0)
	{
		CTransferableMagicCfgServer::UnloadTransferableMagicConfig();
		CTransferableMagicCfgServer::LoadTransferableMagicConfig(szFileName);
	}
	else if (strcmp(szCfgName,"ShockWaveMagic.txt") == 0)
	{
		CShockWaveMagicCfgServer::UnloadShockWaveMagicConfig();
		CShockWaveMagicCfgServer::LoadShockWaveMagicConfig(szFileName);
	}
	else if (strcmp(szCfgName,"PositionMagic.txt") == 0)
	{
		CPositionMagicCfgServer::UnloadPositionMagicConfig();
		CPositionMagicCfgServer::LoadPositionMagicConfig(szFileName);
	}
	else if (strcmp(szCfgName,"TotemMagic.txt") == 0)
	{
		CTotemMagicCfgServer::UnloadTotemMagicConfig();
		CTotemMagicCfgServer::LoadTotemMagicConfig(szFileName);
	}
	else if (strcmp(szCfgName,"BattleArrayMagic.txt") == 0)
	{
		CBattleArrayMagicCfgServer::UnloadBattleArrayMagicConfig();
		CBattleArrayMagicCfgServer::LoadBattleArrayMagicConfig(szFileName);
	}
	else if (strcmp(szCfgName,"TriggerState.txt") == 0)
	{
		CBaseStateCfgServer::UnloadConfig();
		CTriggerStateCfgServer::UnloadConfig();
		CTriggerStateCfgServer::LoadConfig(szFileName);
		CBaseStateCfgServer::LoadConfig();
	}
	else if (strcmp(szCfgName,"DamageChangeState.txt") == 0)
	{
		CBaseStateCfgServer::UnloadConfig();
		CDamageChangeStateCfgServer::UnloadConfig();
		CDamageChangeStateCfgServer::LoadConfig(szFileName);
		CBaseStateCfgServer::LoadConfig();
	}
	else if (strcmp(szCfgName,"SpecialState.txt") == 0)
	{
		CBaseStateCfgServer::UnloadConfig();
		CSpecialStateCfgServer::UnloadConfig();
		CSpecialStateCfgServer::LoadConfig(szFileName);
		CBaseStateCfgServer::LoadConfig();
	}
	else if (strcmp(szCfgName,"NormalAttack.txt") == 0)
	{
		CNormalAttackCfg::UnloadNormalAttackConfig();
		CNormalAttackCfg::LoadNormalAttackConfig(szFileName);
	}
	else if (strcmp(szCfgName,"NPCNormalAttack.txt") == 0)
	{
		CNormalAttackCfg::UnloadNormalAttackConfig();
		CNormalAttackCfg::LoadNPCNormalAttackConfig(szFileName);
	}
	else if (strcmp(szCfgName,"TalentEffect.txt") == 0)
	{
		CTalentEffCfgServer::UnloadTalentEffCfgConfig();
		CTalentEffCfgServer::LoadTalentEffCfgConfig(szFileName);
	}
	else if (strcmp(szCfgName,"AniKeyFrame.txt") == 0)
	{
		CAniKeyFrameCfg::UnloadAniKeyFrameConfig();
		CAniKeyFrameCfg::LoadAniKeyFrameConfig(szFileName);
	}
	else if (strcmp(szCfgName,"PlayerProperty.txt") == 0)
	{
		CPropertyCfg::UnloadConfig();
		CPropertyCfg::LoadConfig(szFileName);
	}
	else if (strcmp(szCfgName,"ForceNoSaveState.txt") == 0)
	{
		CForceNoSaveStateCfgServer::UnloadConfig();
		CForceNoSaveStateCfgServer::LoadConfig(szFileName);
	}
	else if (strcmp(szCfgName,"BurstSoul.txt") == 0)
	{
		CBurstSoulCfgServer::UnloadConfig();
		CBurstSoulCfgServer::LoadConfig(szFileName);
	}

	CfgChk::SetTabNull();
	return true;
}

void UnloadSkillCfg()
{
	CCfgGlobal::UnloadConfig();
	CPropertyCfg::UnloadConfig();
	CMagicEffServer::UnloadMagicEff();
	CNormalSkillServer::UnloadSkillConfig();
	CAureMagicCfgServer::UnloadAureMagicConfig();
	CCastingProcessCfgServer::UnloadConfig();
	CMagicStateCfgServer::UnloadConfig();
	CTriggerStateCfgServer::UnloadConfig();
	CDamageChangeStateCfgServer::UnloadConfig();
	CCumuliTriggerStateCfgServer::UnloadConfig();
	CSpecialStateCfgServer::UnloadConfig();
	CBaseStateCfgServer::UnloadConfig();
	CForceNoSaveStateCfgServer::UnloadConfig();
	//CPersistentStateCfgServer::UnloadConfig();
	CBulletMagicCfgServer::UnloadBulletMagicConfig();
	CMoveMagicCfgServer::UnloadMoveMagicConfig();
	CBarrierMagicCfgServer::UnloadBarrierMagicConfig();
	CTransferableMagicCfgServer::UnloadTransferableMagicConfig();
	CShockWaveMagicCfgServer::UnloadShockWaveMagicConfig();
	CPositionMagicCfgServer::UnloadPositionMagicConfig();
	CTotemMagicCfgServer::UnloadTotemMagicConfig();
	CBattleArrayMagicCfgServer::UnloadBattleArrayMagicConfig();
	CNormalAttackCfg::UnloadNormalAttackConfig();
	CTalentEffCfgServer::UnloadTalentEffCfgConfig();
	CAniKeyFrameCfg::UnloadAniKeyFrameConfig();
	CBurstSoulCfgServer::UnloadConfig();
	CSkillReplace::UnloadConfig();
	TPropertyHolder<CFighterDictator>::UnloadAdapterMap();
	CFighterDictator::ReleaseCalPropertyTable();
	CPropertyTransactionMgr::ReleaseBasePropertyTable();
	CActorCfg::CleanMap();
	CThreadCpuUsageMgr::Unit();
	CCpuUsageMgr::Unit();
}
