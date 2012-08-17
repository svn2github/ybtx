#include "stdafx.h"
#include "ReadSkillCfg.h"
#include "CTableFile.h"
#include "LoadSkillCommon.h"


MapSkillCfgCheck CSkillCfgCheck::m_mapSkill;

bool CSkillCfgCheck::CheckConfig(const string& szFileName)
{   
	CTabFile TabFile; 
	if(!TabFile.Load(szFileName.c_str()))
	{
		cout << "文件 [" << szFileName << "] 不存在\n";
		return false;
	}

	for( int i = 1; i < TabFile.GetHeight(); i++ )
	{
		CSkillCfgCheck* pCfg = new CSkillCfgCheck;
		pCfg->m_uId						= i;
		pCfg->m_sName					= TabFile.GetString(i, szSkill_Name);
		pCfg->m_sBeginSkillAni			= DeQuot(TabFile.GetString(i, szSkill_ReachUpAction));
		pCfg->m_sSkillFX				= DeQuot(TabFile.GetString(i, szSkill_ReachUpEffect));
		
		pair<MapSkillCfgCheck::iterator, bool> pr = m_mapSkill.insert(make_pair(pCfg->m_sName, pCfg));
		if(!pr.second)
		{
			stringstream ExpStr;
			ExpStr << "第" << i << "行的" << "技能" << pCfg->m_sName << "重复";
			GenErr(ExpStr.str());
		}

		PropResChecker::CheckSkillAllAnim(pCfg->m_sName, pCfg->m_sBeginSkillAni, "技能开始动作");
		PropResChecker::CheckSkillAllEff(pCfg->m_sName, pCfg->m_sSkillFX, true, "技能特效");
	}
	return true;
}

MapCastingCfgCheck CCastingCfgCheck::m_mapCasting;

bool CCastingCfgCheck::CheckConfig(const string& szFileName)
{
	CTabFile TabFile;
	if(!TabFile.Load(szFileName.c_str()))
	{
		cout << "文件 [" << szFileName << "] 不存在\n";
		return false;
	}

	for( int i = 1; i < TabFile.GetHeight(); i++ )
	{
		CCastingCfgCheck* pCfg = new CCastingCfgCheck;
		pCfg->m_uId						= i;
		pCfg->m_sName					= TabFile.GetString(i, szCasting_Name);
		pCfg->m_sAni					= DeQuot(TabFile.GetString(i, szProcessAction));
		pCfg->m_sFX						= DeQuot(TabFile.GetString(i, szProcessEffect));

		pair<MapCastingCfgCheck::iterator, bool> pr = m_mapCasting.insert(make_pair(pCfg->m_sName, pCfg));
		if(!pr.second)
		{
			stringstream ExpStr;
			ExpStr << "第" << i << "行的" << "流程" << pCfg->m_sName << "重复";
			GenErr(ExpStr.str());
		}

		PropResChecker::CheckSkillAllAnim(pCfg->m_sName, pCfg->m_sAni, "流程动作");
		PropResChecker::CheckSkillAllEff(pCfg->m_sName, pCfg->m_sFX, true, "流程特效");
	}
	return true;
}

MapMagicOpCfgCheck CMagicOpCfgCheck::m_mapMagicOp;

bool CMagicOpCfgCheck::CheckConfig(const string& szFileName)
{
	CTabFile TabFile;
	if(!TabFile.Load(szFileName.c_str()))
	{
		cout << "文件 [" << szFileName << "] 不存在\n";
		return false;
	}

	for( int i = 1; i < TabFile.GetHeight(); i++ )
	{
		CMagicOpCfgCheck* pCfg = new CMagicOpCfgCheck;
		pCfg->m_uId						= i;
		pCfg->m_sName					= TabFile.GetString(i, szMagicEff_Name);
		pCfg->m_sFX						= DeQuot(TabFile.GetString(i, szMagicEff_FXName));

		pair<MapMagicOpCfgCheck::iterator, bool> pr = m_mapMagicOp.insert(make_pair(pCfg->m_uId, pCfg));
		if(!pr.second)
		{
			stringstream ExpStr;
			ExpStr << "第" << i << "行的" << "流程" << pCfg->m_sName << "重复";
			GenErr(ExpStr.str());
		}
		
		PropResChecker::CheckSkillAllEff(pCfg->m_sName, pCfg->m_sFX, false, "魔法操作特效");
	}
	return true;
}

MapStateCfgCheck CStateCfgCheck::m_mapState;

bool CStateCfgCheck::CheckConfig(const string& szFileName, const string& szTypeName)
{
	CTabFile TabFile;
	if(!TabFile.Load(szFileName.c_str()))
	{
		cout << "文件 [" << szFileName << "] 不存在\n";
		return false;
	}

	for( int i = 1; i < TabFile.GetHeight(); i++ )
	{
		CStateCfgCheck* pCfg = new CStateCfgCheck;
		pCfg->m_uId						= i;
		pCfg->m_sName					= TabFile.GetString(i, "名称");
		pCfg->m_sModel					= DeQuot(TabFile.GetString(i, "模型"));
		pCfg->m_sFX						= DeQuot(TabFile.GetString(i, "对应特效编号"));

		pair<MapStateCfgCheck::iterator, bool> pr = m_mapState.insert(make_pair(pCfg->m_sName, pCfg));
		if(!pr.second)
		{
			stringstream ExpStr;
			ExpStr << "第" << i << "行的" << szTypeName << "状态" << pCfg->m_sName << "重复";
			GenErr(ExpStr.str());
		}
		
		string sEffTitle = szTypeName + "状态特效";
		PropResChecker::CheckSkillAllEff(pCfg->m_sName, pCfg->m_sFX, false, sEffTitle);
	}
	return true;
}

MapStanceSkillCfgCheck CStanceSkillCfgCheck::m_mapStanceSkill;

bool CStanceSkillCfgCheck::CheckConfig(const string& szFileName)
{
	CTabFile TabFile;
	if(!TabFile.Load(szFileName.c_str()))
	{
		cout << "文件 [" << szFileName << "] 不存在\n";
		return false;
	}

	for( int i = 1; i < TabFile.GetHeight(); i++ )
	{
		CStanceSkillCfgCheck* pCfg = new CStanceSkillCfgCheck;
		pCfg->m_uId						= i;
		pCfg->m_sName					= TabFile.GetString(i, "名称");
		pCfg->m_sFX						= DeQuot(TabFile.GetString(i, "特效名"));

		pair<MapStanceSkillCfgCheck::iterator, bool> pr = m_mapStanceSkill.insert(make_pair(pCfg->m_sName, pCfg));
		if(!pr.second)
		{
			stringstream ExpStr;
			ExpStr << "第" << i << "行的" << "姿态技能" << pCfg->m_sName << "重复";
			GenErr(ExpStr.str());
		}

		PropResChecker::CheckSkillAllEff(pCfg->m_sName, pCfg->m_sFX, false, "姿态技能特效");
	}
	return true;
}

MapBulletMagicCfgCheck CBulletMagicCfgCheck::m_mapBulletMagic;

bool CBulletMagicCfgCheck::CheckConfig(const string& szFileName)
{
	CTabFile TabFile;
	if(!TabFile.Load(szFileName.c_str()))
	{
		cout << "文件 [" << szFileName << "] 不存在\n";
		return false;
	}

	for( int i = 1; i < TabFile.GetHeight(); i++ )
	{
		CBulletMagicCfgCheck* pCfg = new CBulletMagicCfgCheck;
		pCfg->m_uId						= i;
		pCfg->m_sName					= TabFile.GetString(i, "名称");
		pCfg->m_sModel					= DeQuot(TabFile.GetString(i, "模型"));
		pCfg->m_sFX						= DeQuot(TabFile.GetString(i, "飞行特效文件名"));
		pCfg->m_sExplodeFX				= DeQuot(TabFile.GetString(i, "爆在特效文件名"));

		pair<MapBulletMagicCfgCheck::iterator, bool> pr = m_mapBulletMagic.insert(make_pair(pCfg->m_sName, pCfg));
		if(!pr.second)
		{
			stringstream ExpStr;
			ExpStr << "第" << i << "行的" << "姿态技能" << pCfg->m_sName << "重复";
			GenErr(ExpStr.str());
		}

		PropResChecker::CheckSkillAllEff(pCfg->m_sName, pCfg->m_sFX, false, "子弹魔法飞行特效");
		PropResChecker::CheckSkillAllEff(pCfg->m_sName, pCfg->m_sExplodeFX, false, "子弹魔法爆炸特效");
	}
	return true;
}

MapMagicCfgCheck CMagicCfgCheck::m_mapMagic;

bool CMagicCfgCheck::CheckConfig(const string& szFileName, const string& szTypeName)
{
	CTabFile TabFile;
	if(!TabFile.Load(szFileName.c_str()))
	{
		cout << "文件 [" << szFileName << "] 不存在\n";
		return false;
	}

	for( int i = 1; i < TabFile.GetHeight(); i++ )
	{
		CMagicCfgCheck* pCfg = new CMagicCfgCheck;
		pCfg->m_uId						= i;
		pCfg->m_sName					= TabFile.GetString(i, "名称");
		pCfg->m_sModel					= DeQuot(TabFile.GetString(i, szTypeName == "冲击波" ? "子弹模型" : "模型"));
		pCfg->m_sFX						= DeQuot(TabFile.GetString(i, "特效名"));

		pair<MapMagicCfgCheck::iterator, bool> pr = m_mapMagic.insert(make_pair(pCfg->m_sName, pCfg));
		if(!pr.second)
		{
			stringstream ExpStr;
			ExpStr << "第" << i << "行的" << szTypeName << "魔法" << pCfg->m_sName << "重复";
			GenErr(ExpStr.str());
		}

		string sEffTitle = szTypeName + "魔法特效";
		PropResChecker::CheckSkillAllEff(pCfg->m_sName, pCfg->m_sFX, false, sEffTitle);
	}
	return true;
}

bool CheckAllConfig()
{
	string pathSkill = g_SettingCfgPath + "/skill/";
	string skill = pathSkill + "Skill.txt";
	string stanceskill = pathSkill + "StanceSkill.txt";
	string magicEffect = pathSkill + "MagicEffect.txt";
	string castingProcess  = pathSkill + "CastingProcess.txt";
	string magicState = pathSkill + "MagicState.txt";
	string bulletMagic = pathSkill + "BulletMagic.txt";
	string moveMagic = pathSkill + "MoveMagic.txt";
	string transMagic = pathSkill + "TransferableMagic.txt";
	string shockWaveMagic = pathSkill + "ShockWaveMagic.txt";
	string positionMagic = pathSkill + "PositionMagic.txt";
	string totemMagic = pathSkill + "TotemMagic.txt";
	string triggerState = pathSkill + "TriggerState.txt";
	string damageState = pathSkill + "DamageChangeState.txt";
	string specialState = pathSkill + "SpecialState.txt";
	string normalAttack = pathSkill + "NormalAttack.txt";
	
	CSkillCfgCheck::CheckConfig(skill);
	CCastingCfgCheck::CheckConfig(castingProcess);
	CMagicOpCfgCheck::CheckConfig(magicEffect);
	CStateCfgCheck::CheckConfig(magicState, "魔法");
	CStateCfgCheck::CheckConfig(triggerState, "触发器");
	CStateCfgCheck::CheckConfig(damageState, "伤害变更");
	CStateCfgCheck::CheckConfig(specialState, "特殊");
	CStanceSkillCfgCheck::CheckConfig(stanceskill);
	CMagicCfgCheck::CheckConfig(moveMagic, "位移");
	CBulletMagicCfgCheck::CheckConfig(bulletMagic);
	CMagicCfgCheck::CheckConfig(shockWaveMagic, "冲击波");
	CMagicCfgCheck::CheckConfig(transMagic, "传递");
	CMagicCfgCheck::CheckConfig(positionMagic, "位置");
	CMagicCfgCheck::CheckConfig(totemMagic, "图腾");

	return true;
}


