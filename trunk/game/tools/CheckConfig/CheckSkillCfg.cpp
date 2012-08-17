#include "stdafx.h"
#include "CheckSkillCfg.h"
#include "CCfgColChecker.h"
#include "TimeHelper.h"
#include "CCfgPropertyCheck.h"
#include "CCfgMagicEffCheck.h"
#include "CCfgProcessCheck.h"
#include "CCfgSkillCheck.h"
#include "CCfgAllMagicCheck.h"
#include "CCfgAllStateCheck.h"
#include "CCfgAllStateCheck.h"
#include "CCfgNormalAttackCheck.h"
#include "CCfgTalentEffCheck.h"
#include "CCfgMagicOp.h"
#include "CTalent.h"
#include "CCfgTempVarCheck.h"
#include "CCfgCalcFunction.h"
#include "CCfgRelChecker.h"
#include "CCfgSkillReplaceCheck.h"
#include "CScriptAppServer.h"
#include "CCfgForceNoSaveState.h"

bool CheckSkillCfg()
{
	using namespace CfgChk;

	string strCfgFilePath	= CScriptAppServer::Inst()->GetCfgFilePath("");

	string strPlayerPath	= strCfgFilePath + "/player/";
	string strCfgProperty	= strPlayerPath + "PlayerProperty.txt";

	string strSkillCfgPath	= strCfgFilePath + "/skill/";
	string strCfgGlobal		= strSkillCfgPath + "CfgGlobal.txt";
	string strPlayerMagicEffect	= strSkillCfgPath + "PlayerMagicEffect.txt";
	string strPlayerActivityMagicEffect	= strSkillCfgPath + "PlayerActivityMagicEffect.txt";
	string strNpcMagicEffect	= strSkillCfgPath + "NpcMagicEffect.txt";
	string strNpcActivityMagicEffect	= strSkillCfgPath + "NpcActivityMagicEffect.txt";
	string strNpcGameMagicEffect	= strSkillCfgPath + "NpcGameMagicEffect.txt";
	string strProcess		= strSkillCfgPath + "CastingProcess.txt";
	string strPlayerSkill	= strSkillCfgPath + "Skill.txt";
	string strNPCskill		= strSkillCfgPath + "NPCSkill.txt";
	string strIntObjSkill	= strSkillCfgPath + "IntObjSkill.txt";
	string strAureMagic		= strSkillCfgPath + "AureMagic.txt";
	string strMagicState	= strSkillCfgPath + "MagicState.txt";
	string strTriggerState	= strSkillCfgPath + "TriggerState.txt";
	string strDamageState	= strSkillCfgPath + "DamageChangeState.txt";
	string strCumuliTriggerState	= strSkillCfgPath + "CumuliTriggerState.txt";
	string strSpecialState	= strSkillCfgPath + "SpecialState.txt";
	string strNoSaveState	= strSkillCfgPath + "ForceNoSaveState.txt";
	string strBulletMagic	= strSkillCfgPath + "BulletMagic.txt";
	string strMoveMagic		= strSkillCfgPath + "MoveMagic.txt";
	string strBarrierMagic	= strSkillCfgPath + "BarrierMagic.txt";
	string strTransMagic	= strSkillCfgPath + "TransferableMagic.txt";
	string strShockWaveMagic	= strSkillCfgPath + "ShockWaveMagic.txt";
	string strPositionMagic = strSkillCfgPath + "PositionMagic.txt";
	string strTotemMagic	= strSkillCfgPath + "TotemMagic.txt";
	string strBattleArrayMagic	= strSkillCfgPath + "BattleArrayMagic.txt";
	string strNormalAttack	= strSkillCfgPath + "NormalAttack.txt";
	string strNPCNormalAttack	= strSkillCfgPath + "NPCNormalAttack.txt";
	string strTalentEffect	= strSkillCfgPath + "TalentEffect.txt";
	string strBurstSoul		= strSkillCfgPath + "BurstSoul.txt";
	string strSkillReplace	= strSkillCfgPath + "SkillReplace.txt";
	string strForceNoSaveState = strSkillCfgPath + "ForceNoSaveState.txt";

	cout << "[" << (g_uTimeForTest = GetProcessTime()) << "]技能配置表检查开始";
	uint64 uTimeForSkillAllTest = g_uTimeForTest;

	if(	!CCfgGlobal::LoadConfig(strCfgGlobal.c_str()) ||
		!CCfgPropertyCheck::Check(strCfgProperty.c_str()) ||
		!CCfgMagicEffCheck::Check(strPlayerMagicEffect.c_str(),true) ||
		!CCfgMagicEffCheck::Check(strPlayerActivityMagicEffect.c_str(),false) ||
		!CCfgMagicEffCheck::Check(strNpcMagicEffect.c_str(),false) ||
		!CCfgMagicEffCheck::Check(strNpcActivityMagicEffect.c_str(),false) ||
		!CCfgMagicEffCheck::Check(strNpcGameMagicEffect.c_str(),false) ||
		!CCfgProcessCheck::Check(strProcess.c_str()) ||
		!CCfgSkillCheck::CheckPlayerSkillCfg(strPlayerSkill.c_str()) ||	
		!CCfgSkillCheck::CheckNPCSkillCfg(strNPCskill.c_str()) ||
		!CCfgSkillCheck::CheckIntObjSkillCfg(strIntObjSkill.c_str()) ||
		!CCfgAureMagicCheck::Check(strAureMagic.c_str()) ||
		!CCfgMagicStateCheck::Check(strMagicState.c_str()) ||
		!CCfgTriggerStateCheck::Check(strTriggerState.c_str()) ||
		!CCfgDamageChangeStateCheck::Check(strDamageState.c_str()) ||
		!CCfgCumuliTriggerStateCheck::Check(strCumuliTriggerState.c_str()) ||
		!CCfgSpecialStateCheck::Check(strSpecialState.c_str()) ||
		!CCfgBulletMagicCheck::Check(strBulletMagic.c_str()) ||
		!CCfgMoveMagicCheck::Check(strMoveMagic.c_str()) ||
		!CCfgBarrierMagicCheck::Check(strBarrierMagic.c_str()) ||
		!CCfgTransMagicCheck::Check(strTransMagic.c_str()) ||
		!CCfgShockWaveMagicCheck::Check(strShockWaveMagic.c_str())||
		!CCfgPositionMagicCheck::Check(strPositionMagic.c_str()) ||
		!CCfgTotemMagicCheck::Check(strTotemMagic.c_str()) ||
		!CCfgBattleArrayMagicCheck::Check(strBattleArrayMagic.c_str()) ||
		!CCfgNormalAttackCheck::CheckPlayerNormalAttackCfg(strNormalAttack.c_str()) ||
		!CCfgNormalAttackCheck::CheckNPCNormalAttackCfg(strNPCNormalAttack.c_str()) ||
		!CCfgTalentEffCheck::Check(strTalentEffect.c_str()) ||
		!CCfgBurstSoulCheck::Check(strBurstSoul.c_str()) ||
		!CCfgSkillReplaceCheck::Check(strSkillReplace.c_str()) ||
		!CForceNoSaveStateCfgServer::Check(strForceNoSaveState.c_str()) ||
		!CCfgTempVarCheck::Check() ||
		!CCfgMagicEffCheck::CheckMagicOpAvail() ||
		!CCfgRelChecker::CheckRelationAvail() ||
		ExistError()
		)
	{
		if(!ExpIsForbidden())
		{
			SetTabNull();
			return false;
		}
	}

	SetTabNull();

	uint64 uCurTime = GetProcessTime();
	cout << "\t耗时" << uCurTime - g_uTimeForTest << "毫秒\n[" << uCurTime << "]技能配置表检查完毕!";
	cout << "\t总耗时" << uCurTime - uTimeForSkillAllTest << "毫秒\n";

	return true;
}

void EndCheckSkillCfg()
{
	CCfgGlobal::UnloadConfig();
	CCfgPropertyCheck::EndCheck();
	CCfgMagicEffCheck::EndCheck();
	CCfgProcessCheck::EndCheck();
	CCfgSkillCheck::EndCheck();
	CCfgAureMagicCheck::EndCheck();
	CCfgMagicStateCheck::EndCheck();
	CCfgTriggerStateCheck::EndCheck();
	CCfgDamageChangeStateCheck::EndCheck();
	CCfgCumuliTriggerStateCheck::EndCheck();
	CCfgSpecialStateCheck::EndCheck();
	CCfgAllStateCheck::EndCheck();
	CCfgBulletMagicCheck::EndCheck();
	CCfgMoveMagicCheck::EndCheck();
	CCfgBarrierMagicCheck::EndCheck();
	CCfgTransMagicCheck::EndCheck();
	CCfgShockWaveMagicCheck::EndCheck();
	CCfgPositionMagicCheck::EndCheck();
	CCfgTotemMagicCheck::EndCheck();
	CCfgBattleArrayMagicCheck::EndCheck();
	CCfgNormalAttackCheck::EndCheck();
	CCfgTalentEffCheck::EndCheck();
	CCfgBurstSoulCheck::EndCheck();
	CCfgSkillReplaceCheck::EndCheck();
	CForceNoSaveStateCfgServer::EndCheck();
	CCfgTempVarCheck::EndCheck();
}

bool IsIntObjSkillNameValid(const TCHAR* szName)
{
	return CCfgSkillCheck::IsIntObjSkillNameValid(szName) != NULL;
}

bool IsPlayerSkillNameValid(const TCHAR* szName)
{
	return CCfgSkillCheck::IsPlayerSkillNameValid(szName) != NULL;
}

bool IsNPCSkillNameValid(const TCHAR* szName)
{
	return CCfgSkillCheck::IsNPCSkillNameValid(szName) != NULL;
}

void InsertTalentName(const TCHAR* sName)
{
	CTalentHolder::InsertName(sName);
}

bool IsStateNameValid(const TCHAR* szName)
{
	return CCfgAllStateCheck::CheckExistAllState(szName);
}
