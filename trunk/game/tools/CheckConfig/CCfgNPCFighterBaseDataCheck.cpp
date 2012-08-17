#include "stdafx.h"
#include "CCfgNPCFighterBaseDataCheck.h"
#include "CTxtTableFile.h"
#include "CCfgSkillCheck.h"
#include "CCfgColChecker.inl"
#include "CCfgNormalAttackCheck.h"
#include "CCfgSkillRuleCheck.h"

CCfgNPCFighterBaseDataCheck::MapNpcFightBaseData	CCfgNPCFighterBaseDataCheck::ms_mapNpcFightBaseData;
CCfgNPCFighterBaseDataCheck::MapName2SkillRule CCfgNPCFighterBaseDataCheck::ms_mapName2SkillRule;

bool CCfgNPCFighterBaseDataCheck::Check(const TCHAR* cfgFile)
{
	CTxtTableFile TableFile;
	Load(TableFile, cfgFile, "NpcFightBaseData_Server");
	string	sBornSkill, strNorAttackName;
	bool bSucc = true;
	for(int32 i = 1; i < TableFile.GetHeight(); ++i)
	{
		string sNpcName = TableFile.GetString(i,"名字");
		string strSkillRule = TableFile.GetString(i, "技能规则");
		CheckOverlap("名字", i);
		sBornSkill = TableFile.GetString(i, "出生释放技能");
		strNorAttackName = TableFile.GetString(i, "普通攻击");
		int32 iRandMaxAttackSpeed;
		iRandMaxAttackSpeed	= TableFile.GetInteger(i, "最大随机攻击速度", -1);
		if (iRandMaxAttackSpeed < 0)
		{
			ostringstream ExpStr;
			ExpStr << "【NpcFightBaseData_Server】配置表第" << i << "行的最大随机攻击速度小于0!";
			CfgChk::GenExpInfo(ExpStr.str());
		}
		CheckNpcBornSkill(sBornSkill,i);
		CheckNpcNormalAttack(strNorAttackName, i);
		if ( !MakeRuleMap(sNpcName, strSkillRule) )
			bSucc = false;
		
		CCfgNPCFighterBaseDataCheck* pCfg = new CCfgNPCFighterBaseDataCheck();
		pCfg->m_uAttackDist = TableFile.GetInteger(i, "普攻距离", 0);
		MapNpcFightBaseData::iterator iter = ms_mapNpcFightBaseData.find(sNpcName);
		if (iter != ms_mapNpcFightBaseData.end())
		{
			ostringstream ExpStr;
			ExpStr << "【NpcFightBaseData_Server】配置表第" << i << "行的Npc名已存在!";
			CfgChk::GenExpInfo(ExpStr.str());
		}
		else
		{
			ms_mapNpcFightBaseData.insert(make_pair(sNpcName,pCfg));
		}
	}
	return bSucc;
}

void CCfgNPCFighterBaseDataCheck::CheckNpcNormalAttack(string& sAttackName, uint32 uLineNum)
{	
	if (!CCfgNormalAttackCheck::IsNPCNormalAttackNameValid(sAttackName))
	{
		ostringstream ExpStr;
		ExpStr << "【NpcFightBaseData_Server】配置表第" << uLineNum << "行的[" << uLineNum+1 << "]Npc普攻在Npc的普攻表中不存在!";
		CfgChk::GenExpInfo(ExpStr.str());
	}
}

void CCfgNPCFighterBaseDataCheck::CheckNpcBornSkill(string& strSkill, uint32 uLineNum)
{
	if(strSkill.empty())
		return;
	erase(strSkill, " ");
	erase(strSkill,"\"");
	replace(strSkill, "，", ",");
	int32 startPos = -1;
	string strTemp;
	while (true)
	{
		if(strSkill.empty())
			return;
		startPos = strSkill.find(',');
		if(-1 == startPos)
		{
			if (!CCfgSkillCheck::IsNPCSkillNameValid(strSkill.c_str()))
			{
				ostringstream ExpStr;
				ExpStr << "【NpcFightBaseData_Server】配置表第" << uLineNum << "行的[" << strSkill << "]技能不存在!";
				CfgChk::GenExpInfo(ExpStr.str());
			}
			return;
		}
		strTemp = strSkill.substr(0,startPos);
		if(strTemp.empty())
		{
			if(startPos < int32(strSkill.length())-1)
			{
				ostringstream ExpStr;
				ExpStr << "【NpcFightBaseData_Server】配置表第" << uLineNum << "行的出生释放技能列格式不对，出现连续 ,, 的情况"<< endl;
				CfgChk::GenExpInfo(ExpStr.str());
			}
			else
				return;
		}
		if (!CCfgSkillCheck::IsNPCSkillNameValid(strTemp.c_str()))
		{
			ostringstream ExpStr;
			ExpStr << "【NpcFightBaseData_Server】配置表第" << uLineNum << "行的[" << strTemp << "]技能不存在!";
			CfgChk::GenExpInfo(ExpStr.str());
		}
		strSkill = strSkill.substr(startPos+1,strSkill.length());
	}
}

void CCfgNPCFighterBaseDataCheck::EndCheck()
{
	EndCheckOverlap();
}

bool CCfgNPCFighterBaseDataCheck::BeExist(string sNpcName)
{
	if (ms_mapNpcFightBaseData.find(sNpcName) != ms_mapNpcFightBaseData.end())
		return true;
	return false;
}

uint32 CCfgNPCFighterBaseDataCheck::GetAttackDistByName(const TCHAR* szName)
{
	string strNpcName(szName);
	MapNpcFightBaseData::iterator iter = ms_mapNpcFightBaseData.find(strNpcName);
	if (iter != ms_mapNpcFightBaseData.end())
	{
		return iter->second->m_uAttackDist;
	}
	return 0;
}

bool CCfgNPCFighterBaseDataCheck::MakeRuleMap(string& strNpcName, string& strSkillRule)
{
	if(strSkillRule.empty())
		return true;
	erase(strSkillRule, " ");
	erase(strSkillRule,"\"");
	replace(strSkillRule, "，", ",");
	replace(strSkillRule, "（","(");
	replace(strSkillRule, "）", ")");

	LstSkillRuleName lstSkillRuleName;

	bool bSucc = true;
	if (strSkillRule.find(",") == -1) //说明只有一个技能规则的Npc
	{
		if ( !CCfgSkillRuleCheck::CheckExist(strSkillRule) )
		{
			cout << "【NpcFightBaseData_Server】配置表Npc名为【" << strNpcName << "】的技能规则【" << strSkillRule << "】在SkillRuleBaseData_Server表不存在" <<endl;
			bSucc = false;
		}
		else
		{
			//ms_mapName2SkillRule.insert(make_pair(strNpcName, strSkillRule));
			lstSkillRuleName.insert(strSkillRule);
			ms_mapName2SkillRule.insert(make_pair(strNpcName, lstSkillRuleName));
		}
	}
	else
	{
		uint32 uNumber = 0;
		//下面是有多个技能规则的情况
		while (true)
		{
			if(strSkillRule.empty())
				break;
			string strSkillRuleNode;
			int32 uNext = strSkillRule.find(",");
			if (uNext != -1)
			{
				strSkillRuleNode = strSkillRule.substr(0, uNext);
				strSkillRule = strSkillRule.substr(uNext+1, strSkillRule.length());
			}
			else
			{
				strSkillRuleNode = strSkillRule;
				strSkillRule.clear();
			}
			Ast (!strSkillRuleNode.empty());
			int32 uStatrt = strSkillRuleNode.find("(");
			int32 uEnd = strSkillRuleNode.find(")");
			if (uStatrt == -1 || uEnd ==-1)
			{
				ostringstream strm;
				strm<<"NpcFightBaseData_Server表中 Npc："<< strNpcName<<" 的技能规则填写错误！"<< endl;
				GenErr(strm.str());
			}
			string strSkillRuleName = strSkillRuleNode.substr(0, uStatrt);
			string strNum = strSkillRuleNode.substr(uStatrt+1, uEnd-uStatrt-1);
			uNumber = uNumber + static_cast<uint32>(atoi(strNum.c_str()));
			if ( !CCfgSkillRuleCheck::CheckExist(strSkillRuleName) )
			{
				cout << "【NpcFightBaseData_Server】配置表Npc名为【" << strNpcName << "】的技能规则【" << strSkillRuleName << "】在SkillRuleBaseData_Server表不存在" <<endl;
				bSucc = false;
			}
			else
			{
				lstSkillRuleName.insert(strSkillRuleName);
			}
		}
		if (uNumber != 100)
		{
			ostringstream strm;
			strm<<"NpcFightBaseData_Server表中 Npc："<< strNpcName<<" 的技能规则总概率不为100，轻查实！"<< endl;
			GenErr(strm.str());
		}
		ms_mapName2SkillRule.insert(make_pair(strNpcName, lstSkillRuleName));
	}
	return bSucc;
}

bool CCfgNPCFighterBaseDataCheck::CheckSkillRule(string& strNpcName, string& strSkillRuleName )
{
	if (ms_mapName2SkillRule.find(strNpcName) != ms_mapName2SkillRule.end())
	{
		LstSkillRuleName& lstSkillRuleName = ms_mapName2SkillRule[strNpcName];
		if (lstSkillRuleName.find(strSkillRuleName) != lstSkillRuleName.end())
			return false;
	}
	return true;
}
