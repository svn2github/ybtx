#include "stdafx.h"
#include "CCfgNormalAttackCheck.h"
#include "CTxtTableFile.h"
#include "CCfgColChecker.inl"
#include "LoadSkillCommon.h"
#include "CSkillCfg.h"

CCfgNormalAttackCheck::SetPlayerNAName	CCfgNormalAttackCheck::ms_setPlayerNAName;
CCfgNormalAttackCheck::SetNPCNAName		CCfgNormalAttackCheck::ms_setNPCNAName;
CCfgNormalAttackCheck::MapNormalAttack	CCfgNormalAttackCheck::ms_mapNormalAttack;

bool CCfgNormalAttackCheck::CheckPlayerNormalAttackCfg(const TCHAR* cfgFile)
{
	using namespace CfgChk;

	CTxtTableFile TableFile;
	SetTabFile(TableFile, "普通攻击");
	if (!TableFile.Load(PATH_ALIAS_CFG.c_str(), cfgFile))
	{
		stringstream ExpStr;
		ExpStr << "配置表 加载失败，请查看文件名[" << cfgFile << "]是否正确，或文件是否存在";
		GenExpInfo(ExpStr.str());
	}

	static const uint32 s_uTableFileWide = 5;
	if (TableFile.GetWidth() != s_uTableFileWide)
	{
		stringstream ExpStr;
		ExpStr << "配置表 列数错误 应为: " << s_uTableFileWide << " 列，实为: " << TableFile.GetWidth() << " 列";
		GenExpInfo(ExpStr.str());
	}

	for( int32 i = 1; i < TableFile.GetHeight(); ++i )
	{
		SetLineNo(i);
		string strName, strMagicEff;
		ReadItem(strName, szNT_Name, CANEMPTY);
		ReadItem(strMagicEff, szSkill_MagicEff, CANEMPTY);
		CCfgMagicEffCheck::CheckMagicEffExist(strMagicEff);
		trimend(strName);
		SetPlayerNAName::iterator iter = ms_setPlayerNAName.find(strName);
		if (iter != ms_setPlayerNAName.end())
		{
			stringstream ExpStr;
			ExpStr << " 配置表: " << g_sTabName << " 第 " << i << " 行的 " << szNT_Name << "["<< strName << "]" << "重复";
			GenExpInfo(ExpStr.str());
		}
		else
		{
			ms_setPlayerNAName.insert(strName);
			CCfgNormalAttackCheck* pNormalAttack = new CCfgNormalAttackCheck;
			pNormalAttack->m_strName = strName;
			if (!strMagicEff.empty() && CCfgMagicEffCheck::CheckMagicEffExist(strMagicEff))
			{
				CCfgMagicEffCheck::MapMagicEff::iterator itr = CCfgMagicEffCheck::ms_mapMagicEff.find(strMagicEff);
				pNormalAttack->m_pMagicEff = (*itr).second;
			}
			ms_mapNormalAttack.insert(make_pair(strName, pNormalAttack));
		}
	}

	return true;
}

bool CCfgNormalAttackCheck::CheckNPCNormalAttackCfg(const TCHAR* cfgFile)
{
	using namespace CfgChk;

	CTxtTableFile TableFile;
	SetTabFile(TableFile, "NPC普通攻击");
	if (!TableFile.Load(PATH_ALIAS_CFG.c_str(), cfgFile))
	{
		stringstream ExpStr;
		ExpStr << "配置表 加载失败，请查看文件名[" << cfgFile << "]是否正确，或文件是否存在";
		GenExpInfo(ExpStr.str());
	}

	static const uint32 s_uTableFileWide = 5;
	if (TableFile.GetWidth() != s_uTableFileWide)
	{
		stringstream ExpStr;
		ExpStr << "配置表 列数错误 应为: " << s_uTableFileWide << " 列，实为: " << TableFile.GetWidth() << " 列";
		GenExpInfo(ExpStr.str());
	}

	for( int32 i = 1; i < TableFile.GetHeight(); ++i )
	{
		SetLineNo(i);
		string strName, strMagicEff;
		EAttackType	eAttackType;
		ReadItem(strName, szNT_Name, CANEMPTY);
		ReadItem(strMagicEff, szSkill_MagicEff, CANEMPTY);
		ReadItem(eAttackType, szSkill_AttackType, CSkillCfg::ms_mapAttackType);
		
		CCfgMagicEffCheck::CheckMagicEffExist(strMagicEff);
		trimend(strName);
		SetNPCNAName::iterator iter = ms_setNPCNAName.find(strName);
		if (iter != ms_setNPCNAName.end())
		{
			stringstream ExpStr;
			ExpStr << " 配置表: " << g_sTabName << " 第 " << i << " 行的 " << szMagicState_Name << "["<< strName << "]" << "重复";
			GenExpInfo(ExpStr.str());
		}
		else
		{
			ms_setNPCNAName.insert(strName);
			CCfgNormalAttackCheck* pNormalAttack = new CCfgNormalAttackCheck;
			pNormalAttack->m_strName = strName;
			if (!strMagicEff.empty() && CCfgMagicEffCheck::CheckMagicEffExist(strMagicEff))
			{
				CCfgMagicEffCheck::MapMagicEff::iterator itr = CCfgMagicEffCheck::ms_mapMagicEff.find(strMagicEff);
				pNormalAttack->m_pMagicEff = (*itr).second;
				EValueMagicOpType eValueMagicOpType = pNormalAttack->m_pMagicEff->GetFirstOpValueMagicOpType();
				switch(eAttackType)
				{
				case eATT_Puncture:
				case eATT_Chop:
				case eATT_BluntTrauma:
					{
						if (eValueMagicOpType == eVMOT_Magic)
						{
							stringstream ExpStr;
							ExpStr << "配置表[" << g_sTabName << "]第" << (g_iLine + 1) << "行的["
								<< szSkill_AttackType << "]和第一个伤害计算效果类型不一致, 攻击类型物理的,伤害是法术的";
							GenExpInfo(ExpStr.str());
						}
						break;
					}
				case eATT_Nature:
				case eATT_Destroy:
				case eATT_Evil:
					{
						if (eValueMagicOpType == eVMOT_Physical)
						{
							stringstream ExpStr;
							ExpStr << "配置表[" << g_sTabName << "]第" << (g_iLine + 1) << "行的["
								<< szSkill_AttackType << "]和第一个伤害计算效果类型不一致, 攻击类型是法术的,伤害是物理的";
							GenExpInfo(ExpStr.str());
						}
						break;
					}
				case eATT_None:
				default:
					break;
				}
			}
			ms_mapNormalAttack.insert(make_pair(strName, pNormalAttack));
		}
		if (eAttackType == eATT_FollowWeapon)
		{
			stringstream ExpStr;
			ExpStr << "配置表[" << g_sTabName << "]第" << (g_iLine + 1) << "行的["
				<< szSkill_AttackType << "]列不能填[跟随武器类型]";
			GenExpInfo(ExpStr.str());
		}
	}

	return true;
}

void CCfgNormalAttackCheck::EndCheck()
{
	ms_setPlayerNAName.clear();
	ms_setNPCNAName.clear();
	ClearMap(ms_mapNormalAttack);
}

bool CCfgNormalAttackCheck::IsPlayerNormalAttackNameValid(const string& strName)
{
	SetPlayerNAName::iterator iter = ms_setPlayerNAName.find(strName);
	if (iter == ms_setPlayerNAName.end())
	{
		stringstream ExpStr;
		ExpStr << " 违反魔法操作约束: 普通攻击" << " [" << strName << "] 不存在 ";
		CfgChk::GenExpInfo(ExpStr.str());
		return false;
	}
	return true;
}

bool CCfgNormalAttackCheck::IsNPCNormalAttackNameValid(const string& strName)
{
	SetNPCNAName::iterator iter = ms_setNPCNAName.find(strName);
	if (iter == ms_setNPCNAName.end())
	{
		stringstream ExpStr;
		ExpStr << " 违反魔法操作约束: NPC普通攻击" << " [" << strName << "] 不存在 ";
		CfgChk::GenExpInfo(ExpStr.str());
		return false;
	}
	return true;
}
