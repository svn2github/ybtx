#include "stdafx.h"
#include "CCfgAllMagicCheck.h"
#include "CTxtTableFile.h"
#include "CCfgColChecker.inl"
#include "LoadSkillCommon.h"

CCfgBurstSoulCheck::SetRowName CCfgBurstSoulCheck::ms_setRowName;
CCfgBurstSoulCheck::MapBurstSoulType CCfgBurstSoulCheck::ms_mapBurstSoulType;

bool CCfgBurstSoulCheck::Check(const TCHAR* cfgFile)
{
	using namespace CfgChk;

	InitMapBurstSoulType();
	CTxtTableFile TableFile;
	SetTabFile(TableFile, "爆魂");
	if (!TableFile.Load(PATH_ALIAS_CFG.c_str(), cfgFile))
	{
		stringstream ExpStr;
		ExpStr << "配置表 加载失败，请查看文件名[" << cfgFile << "]是否正确，或文件是否存在";
		GenExpInfo(ExpStr.str());
	}

	static const uint32 s_uTableFileWide = 7;
	if (TableFile.GetWidth() != s_uTableFileWide)
	{
		stringstream ExpStr;
		ExpStr << "配置表 列数错误 应为: " << s_uTableFileWide << " 列，实为: " << TableFile.GetWidth() << " 列";
		GenExpInfo(ExpStr.str());
	}

	CCfgCalc* pCountCacl = NULL;
	CCfgCalc* pProbabilityCacl = NULL;
	for(int32 i = 1; i < TableFile.GetHeight(); ++i)
	{
		SetLineNo(i);
		string strName;
		ReadItem(strName, szBurstSoul_DoubleBurst, CANEMPTY);
		trimend(strName);
		SetRowName::iterator iter = ms_setRowName.find(strName);
		if (iter != ms_setRowName.end())
		{
			stringstream ExpStr;
			ExpStr << " 配置表: " << g_sTabName << " 第 " << i << " 行的 " << szBurstSoul_DoubleBurst << "["<< strName << "]" << "重复";
			GenExpInfo(ExpStr.str());
		}
		else
		{
			ms_setRowName.insert(strName);
		}

		string strMagicEff;
		ReadItem(strMagicEff, szSkill_MagicEff, CANEMPTY);
		CCfgMagicEffCheck::CheckMagicEffExist(strMagicEff);

		string	strBurstSoulType;
		ReadItem(strBurstSoulType,	szBurstSoul_Type);
		if(!ms_mapBurstSoulType[strBurstSoulType])
		{
			stringstream ExpStr;
			ExpStr << " 爆魂类型不存在: " << strBurstSoulType ;
			CfgChk::GenExpInfo(ExpStr.str());
		}
		ReadItem(pCountCacl, szBurstSoul_Count);
		pCountCacl->SetTypeExpression();
		ReadItem(pProbabilityCacl, szBurstSoul_Probability);
		pProbabilityCacl->SetTypeExpression();
		delete pCountCacl;
		delete pProbabilityCacl;

		string strBulletMagicName;
		ReadItem(strBulletMagicName,	szBurstSoul_BulletMagicName);
		if (!CCfgBulletMagicCheck::CheckExist(strBulletMagicName))
		{
			CfgChk::GenExpInfo("爆魂子弹不存在, 查看子弹魔法表\n");
		}
	}
	return true;
}

void CCfgBurstSoulCheck::EndCheck()
{
	ms_setRowName.clear();
}

bool CCfgBurstSoulCheck::CheckExist(const string& strName)
{
	SetRowName::iterator iter = ms_setRowName.find(strName);
	if (iter == ms_setRowName.end())
	{
		stringstream ExpStr;
		ExpStr << " 违反魔法操作约束: 爆魂" << " [" << strName << "] 不存在 ";
		CfgChk::GenExpInfo(ExpStr.str());
		return false;
	}
	return true;
}

bool CCfgBurstSoulCheck::CheckExistType(const string& strType)
{
	if(!ms_mapBurstSoulType[strType])
	{
		CfgChk::GenExpInfo("违反魔法操作约束：[对经验拥有者奖励魂附加]参数不正确");
		return false;
	}
	return true;
}
