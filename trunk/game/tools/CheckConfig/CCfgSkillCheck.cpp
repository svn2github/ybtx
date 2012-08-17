#include "stdafx.h"
#include "CCfgSkillCheck.h"
#include "CTxtTableFile.h"
#include "CCfgColChecker.inl"
#include "LoadSkillCommon.h"
#include "CSkillCfg.h"
#include "CCfgProcessCheck.h"

CSkillCfg::MapTargetType	CSkillCfg::ms_mapTargetType;
CSkillCfg::MapCoolDownType	CSkillCfg::ms_mapCoolDownType;
CSkillCfg::MapAttackType	CSkillCfg::ms_mapAttackType;
CCfgSkillCheck::MapSkill	CCfgSkillCheck::ms_mapSkill;

using namespace CfgChk;

CCfgSkillCheck* CCfgSkillCheck::GetInst()
{
	static CCfgSkillCheck ms_SkillCheck;
	return &ms_SkillCheck;
}

bool CCfgSkillCheck::CheckPlayerSkillCfg(const TCHAR* cfgFile)
{
	CSkillCfg::InitMapTargetType();
	CSkillCfg::InitMapCoolDownType();
	CTxtTableFile TabFile;
	SetTabFile(TabFile, "玩家技能表");
	if (!TabFile.Load(PATH_ALIAS_CFG.c_str(), cfgFile))
	{
		stringstream ExpStr;
		ExpStr << " 配置表 加载失败，请查看文件名[" << cfgFile << "]是否正确，或文件是否存在";
		GenExpInfo(ExpStr.str());
	}

	static const uint32 s_uTableFileWide = 20;
	if (TabFile.GetWidth() != s_uTableFileWide)
	{
		stringstream ExpStr;
		ExpStr << "配置表 列数错误 应为: " << s_uTableFileWide << " 列，实为: " << TabFile.GetWidth() << " 列";
		GenExpInfo(ExpStr.str());
	}

	for( int32 i = 1; i < TabFile.GetHeight(); ++i )
	{
		SetLineNo(i);
		string strName, strMagicEff;
		ReadItem(strName, szSkill_Name, CANEMPTY);
		ReadItem(strMagicEff, szSkill_MagicEff, CANEMPTY);
		CCfgMagicEffCheck::CheckMagicEffExist(strMagicEff);
		trimend(strName);
		MapSkill::iterator iter = ms_mapSkill.find(strName);
		if (iter != ms_mapSkill.end())
		{
			stringstream ExpStr;
			ExpStr << " 配置表: " << g_sTabName << " 第 " << i << " 行的 " << szSkill_Name << "["<< strName << "]" << "重复";
			GenExpInfo(ExpStr.str());
		}
		else
		{
			CCfgSkillCheck* pSkill = new CCfgSkillCheck;
			pSkill->m_bIsDoSkillRule = true;
			pSkill->m_strSkillName = strName;
			if (!strMagicEff.empty())
			{
				CCfgMagicEffCheck::MapMagicEff::iterator itr = CCfgMagicEffCheck::ms_mapMagicEff.find(strMagicEff);
				if(itr == CCfgMagicEffCheck::ms_mapMagicEff.end())
				{
					stringstream ExpStr;
					ExpStr << " 配置表: " << g_sTabName << " 第 " << i << " 行的 " << strName << " 的效果[" << strMagicEff << "]" << "不存在";
					GenExpInfo(ExpStr.str());
				}
				pSkill->m_pMagicEff = (*itr).second;
			}
			ms_mapSkill.insert(make_pair(strName, pSkill));
		}

		string sTempSelectType;
		ReadItem(sTempSelectType,	szSkill_SelectTargetType,	CANEMPTY,	"");
		string::size_type pos = sTempSelectType.find_first_of(',');
		if(pos == string::npos)
		{
			if(!CSkillCfg::ms_mapTargetType.count(sTempSelectType))
			{
				stringstream ExpStr;
				ExpStr << "配置表[技能总表]第" << (i + 1) << "行的技能,选中目标类型[" << sTempSelectType << "]错误";
				GenExpInfo(ExpStr.str());
			}
		}
		else
		{
			string sSelectType = sTempSelectType.substr(0, pos);
			if(!CSkillCfg::ms_mapTargetType.count(sSelectType))
			{
				stringstream ExpStr;
				ExpStr << "配置表[技能总表]第" << (i + 1) << "行的技能,选中目标类型[" << sSelectType << "错误";
				GenExpInfo(ExpStr.str());
			}
		}

		//技能拥有时间及次数检查
		ESkillCoolDownType eCoolDownType;
		ReadItem(eCoolDownType,		szSkill_Type,	CSkillCfg::ms_mapCoolDownType);
		string strActiveArg;
		ReadItem(strActiveArg,					szSkill_ActiveTimeAndCount,CANEMPTY);
		if (strActiveArg!= "")
		{
			CCfgCalc* pActiveArg = new CCfgCalc;
			CCfgCalc* pActiveTime = new CCfgCalc;
			pActiveArg->InputString(strActiveArg);
			if (pActiveArg->GetStringCount()>1)
			{
				if (eCoolDownType != eSCDT_TempSkill)
				{
					stringstream ExpStr;
					ExpStr << "配置表[技能总表]第" << (i + 1) << "行的技能[" << strName<< "]的" << szSkill_ActiveTimeAndCount << "错误,只有临时技能才能填次数";
					GenExpInfo(ExpStr.str());
				}
				else
				{
					CCfgCalc* pActiveCount = new CCfgCalc;
					pActiveCount->InputString(pActiveArg->GetString(1));
					if (pActiveCount->GetIntValue()<0)
					{
						stringstream ExpStr;
						ExpStr << "配置表[技能总表]第" << (i + 1) << "行的技能[" << strName<< "]的" << szSkill_ActiveTimeAndCount << "错误,次数不能小于0";
						GenExpInfo(ExpStr.str());
					}
					delete pActiveCount;
				}
			}
			pActiveTime->InputString(pActiveArg->GetString(0));
			if ((float)pActiveTime->GetDblValue() < 0)
			{
				stringstream ExpStr;
				ExpStr << "配置表[技能总表]第" << (i + 1) << "行的技能[" << strName<< "]的" << szSkill_ActiveTimeAndCount << "错误,时间不能小于0";
				GenExpInfo(ExpStr.str());
			}
			delete pActiveArg;
			delete pActiveTime;
		}

		// 特效名检查
		string strFX = TabFile.GetString(i, szSkill_CastEffect);
		if (strFX != "")
		{
			vector<string> sFXTable = CCfgMagicOp::Split(strFX, ",");
			if (sFXTable.size() == 2)
			{
				string strFXName	= sFXTable[1];
				if (strFXName == "")
				{
					stringstream ExpStr;
					ExpStr << "第" << i << "行的 特效名 " << strFX << " 错误, 逗号后必须有特效名!";
					GenExpInfo(ExpStr.str());
				}
			}
			else
			{
				stringstream ExpStr;
				ExpStr << "第" << i << "行的 特效名 " << strFX << " 错误, 必须为逗号隔开的两项!";
				GenExpInfo(ExpStr.str());
			}
		}
	}

	return true;
}



bool CCfgSkillCheck::CheckNPCSkillCfg(const TCHAR* cfgFile)
{
	CSkillCfg::InitMapAttackType();
	CTxtTableFile TabFile;
	SetTabFile(TabFile, "NPC技能总表");
	if (!TabFile.Load(PATH_ALIAS_CFG.c_str(), cfgFile))
	{
		stringstream ExpStr;
		ExpStr << " 配置表 加载失败，请查看文件名[" << cfgFile << "]是否正确，或文件是否存在";
		GenExpInfo(ExpStr.str());
	}

	static const uint32 s_uTableFileWide = 6;
	if (TabFile.GetWidth() != s_uTableFileWide)
	{
		stringstream ExpStr;
		ExpStr << "配置表 列数错误 应为: " << s_uTableFileWide << " 列，实为: " << TabFile.GetWidth() << " 列";
		GenExpInfo(ExpStr.str());
	}

	for( int32 i = 1; i < TabFile.GetHeight(); ++i )
	{
		SetLineNo(i);
		string strName, strMagicEff, strCastAction;
		ReadItem(strName, szSkill_Name, CANEMPTY);
		ReadItem(strMagicEff, szSkill_MagicEff, CANEMPTY);
		ReadItem(strCastAction, szSkill_CastAction, CANEMPTY);

		if (!strCastAction.empty() && CCfgSkillCheck::GetInst()->m_setSkillAction.find(strCastAction) == CCfgSkillCheck::GetInst()->m_setSkillAction.end())
		{
			stringstream ExpStr;
			ExpStr << " 配置表: " << g_sTabName << " 第 " << i << " 行的 " << szSkill_CastAction << "["<< strCastAction << "]" << "错误！允许的出手动作：(birth, attack01-04, cast01-09, sf01-09, skill01-09)";
			GenExpInfo(ExpStr.str());
		}

		CCfgMagicEffCheck::CheckMagicEffExist(strMagicEff);
		trimend(strName);
		MapSkill::iterator iter = ms_mapSkill.find(strName);
		if (iter != ms_mapSkill.end())
		{
			stringstream ExpStr;
			ExpStr << " 配置表: " << g_sTabName << " 第 " << i << " 行的 " << szSkill_Name << "["<< strName << "]" << "重复";
			GenExpInfo(ExpStr.str());
		}
		else
		{
			CCfgSkillCheck* pSkill = new CCfgSkillCheck;
			pSkill->m_strSkillName = strName;
			if (!strMagicEff.empty())
			{
				CCfgMagicEffCheck::MapMagicEff::iterator itr = CCfgMagicEffCheck::ms_mapMagicEff.find(strMagicEff);
				if(itr == CCfgMagicEffCheck::ms_mapMagicEff.end())
				{
					stringstream ExpStr;
					ExpStr << " 配置表: " << g_sTabName << " 第 " << i << " 行的 " << strName << " 的效果[" << strMagicEff << "]" << "不存在";
					GenExpInfo(ExpStr.str());
				}
				pSkill->m_pMagicEff = (*itr).second;
				ReadItem(pSkill->m_bIsDoSkillRule,"是否走技能规则",	CANEMPTY,	YES);
			}
			ms_mapSkill.insert(make_pair(strName, pSkill));
		}

		EAttackType eAttackType;
		ReadItem(eAttackType,	szSkill_AttackType,	CANEMPTY,	eATT_None,	CSkillCfg::ms_mapAttackType);
		if(eAttackType == eATT_FollowWeapon)
		{
			stringstream ExpStr;
			ExpStr << "配置表[" << g_sTabName << "]第" << (g_iLine + 1) << "行的["
				<< szSkill_AttackType << "]列不能填[跟随武器类型]";
			GenExpInfo(ExpStr.str());
		}

		// 特效名检查
		string strFX = TabFile.GetString(i, szSkill_CastEffect);
		if (strFX != "")
		{
			vector<string> sFXTable = CCfgMagicOp::Split(strFX, ",");
			if (sFXTable.size() == 2)
			{
				string strFXName	= sFXTable[1];
				if (strFXName == "")
				{
					stringstream ExpStr;
					ExpStr << "第" << i << "行的 特效名 " << strFX << " 错误, 逗号后必须有特效名!";
					GenExpInfo(ExpStr.str());
				}
			}
			else
			{
				stringstream ExpStr;
				ExpStr << "第" << i << "行的 特效名 " << strFX << " 错误, 必须为逗号隔开的两项!";
				GenExpInfo(ExpStr.str());
			}
		}
	}

	return true;
}

bool CCfgSkillCheck::CheckIntObjSkillCfg(const TCHAR* cfgFile)
{
	CTxtTableFile TabFile;
	SetTabFile(TabFile, "IntObj技能总表");
	if (!TabFile.Load(PATH_ALIAS_CFG.c_str(), cfgFile))
	{
		stringstream ExpStr;
		ExpStr << " 配置表 加载失败，请查看文件名[" << cfgFile << "]是否正确，或文件是否存在";
		GenExpInfo(ExpStr.str());
	}

	static const uint32 s_uTableFileWide = 2;
	if (TabFile.GetWidth() != s_uTableFileWide)
	{
		stringstream ExpStr;
		ExpStr << "配置表 列数错误 应为: " << s_uTableFileWide << " 列，实为: " << TabFile.GetWidth() << " 列";
		GenExpInfo(ExpStr.str());
	}

	for( int32 i = 1; i < TabFile.GetHeight(); ++i )
	{
		SetLineNo(i);
		string strName, strMagicEff;
		ReadItem(strName, szSkill_Name, CANEMPTY);
		ReadItem(strMagicEff, szSkill_MagicEff, CANEMPTY);
		CCfgMagicEffCheck::CheckMagicEffExist(strMagicEff);
		if (CCfgProcessCheck::CheckExist(strName))
		{
			stringstream ExpStr;
			ExpStr << " 配置表: " << g_sTabName << " 第 " << i << " 行的 " << szSkill_Name << "["<< strName << "]" << "不能是流程技能";
			GenExpInfo(ExpStr.str());
		}
		trimend(strName);
		MapSkill::iterator iter = ms_mapSkill.find(strName);
		if (iter != ms_mapSkill.end())
		{
			stringstream ExpStr;
			ExpStr << " 配置表: " << g_sTabName << " 第 " << i << " 行的 " << szSkill_Name << "["<< strName << "]" << "重复";
			GenExpInfo(ExpStr.str());
		}
		else
		{
			CCfgSkillCheck* pSkill = new CCfgSkillCheck;
			pSkill->m_strSkillName = strName;
			if (!strMagicEff.empty())
			{
				CCfgMagicEffCheck::MapMagicEff::iterator itr = CCfgMagicEffCheck::ms_mapMagicEff.find(strMagicEff);
				pSkill->m_pMagicEff = (*itr).second;
			}
			ms_mapSkill.insert(make_pair(strName, pSkill));
		}
	}

	return true;
}

void CCfgSkillCheck::EndCheck()
{
	ClearMap(ms_mapSkill);
}

CCfgSkillCheck::CCfgSkillCheck()
:m_strSkillName("")
,m_pMagicEff(NULL)
{

	m_setSkillAction.insert("birth");
	m_setSkillAction.insert("attack01");
	m_setSkillAction.insert("attack02");
	m_setSkillAction.insert("attack03");
	m_setSkillAction.insert("attack04");
	m_setSkillAction.insert("cast01");
	m_setSkillAction.insert("cast02");
	m_setSkillAction.insert("cast03");
	m_setSkillAction.insert("cast04");
	m_setSkillAction.insert("cast05");
	m_setSkillAction.insert("cast06");
	m_setSkillAction.insert("cast07");
	m_setSkillAction.insert("cast08");
	m_setSkillAction.insert("cast09");
	m_setSkillAction.insert("sf01");
	m_setSkillAction.insert("sf02");
	m_setSkillAction.insert("sf03");
	m_setSkillAction.insert("sf04");
	m_setSkillAction.insert("sf05");
	m_setSkillAction.insert("sf06");
	m_setSkillAction.insert("sf07");
	m_setSkillAction.insert("sf08");
	m_setSkillAction.insert("sf09");
	m_setSkillAction.insert("skill01");
	m_setSkillAction.insert("skill02");
	m_setSkillAction.insert("skill03");
	m_setSkillAction.insert("skill04");
	m_setSkillAction.insert("skill05");
	m_setSkillAction.insert("skill06");
	m_setSkillAction.insert("skill07");
	m_setSkillAction.insert("skill08");
	m_setSkillAction.insert("skill09");

}

CCfgSkillCheck::~CCfgSkillCheck()
{
}

bool CCfgSkillCheck::IsPlayerSkillNameValid(const TCHAR* szName)
{
	string strName = szName;
	return ms_mapSkill.find(strName) != ms_mapSkill.end();
}

bool CCfgSkillCheck::IsNPCSkillNameValid(const TCHAR* szName)
{
	string strName = szName;
	return ms_mapSkill.find(strName) != ms_mapSkill.end();
}

bool CCfgSkillCheck::IsIntObjSkillNameValid(const TCHAR* szName)
{
	string strName = szName;
	return ms_mapSkill.find(strName) != ms_mapSkill.end();
}

bool CCfgSkillCheck::IsNPCSkillDoSkillRule(const TCHAR* szName)
{
	MapSkill::iterator iter = ms_mapSkill.find(szName);
	if (iter != ms_mapSkill.end())
	{
		return iter->second->m_bIsDoSkillRule;
	}
	return false;
}

