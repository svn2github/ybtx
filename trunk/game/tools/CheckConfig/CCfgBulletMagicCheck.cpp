#include "stdafx.h"
#include "CCfgAllMagicCheck.h"
#include "CTxtTableFile.h"
#include "CCfgColChecker.inl"
#include "LoadSkillCommon.h"

CCfgBulletMagicCheck::SetRowName CCfgBulletMagicCheck::ms_setRowName;

bool CCfgBulletMagicCheck::Check(const TCHAR* cfgFile)
{
	using namespace CfgChk;

	CTxtTableFile TableFile;
	SetTabFile(TableFile, "子弹魔法");
	if (!TableFile.Load(PATH_ALIAS_CFG.c_str(), cfgFile))
	{
		stringstream ExpStr;
		ExpStr << "配置表 加载失败，请查看文件名[" << cfgFile << "]是否正确，或文件是否存在";
		GenExpInfo(ExpStr.str());
	}

	static const uint32 s_uTableFileWide = 8;
	if (TableFile.GetWidth() != s_uTableFileWide)
	{
		stringstream ExpStr;
		ExpStr << "配置表 列数错误 应为: " << s_uTableFileWide << " 列，实为: " << TableFile.GetWidth() << " 列";
		GenExpInfo(ExpStr.str());
	}

	for(int32 i = 1; i < TableFile.GetHeight(); ++i)
	{
		SetLineNo(i);
		string strName;
		ReadItem(strName, szMagic_Name, CANEMPTY);
		trimend(strName);
		SetRowName::iterator iter = ms_setRowName.find(strName);
		if (iter != ms_setRowName.end())
		{
			stringstream ExpStr;
			ExpStr << " 配置表: " << g_sTabName << " 第 " << i << " 行的 " << szMagic_Name << "["<< strName << "]" << "重复";
			GenExpInfo(ExpStr.str());
		}
		else
		{
			ms_setRowName.insert(strName);
		}

		string strMagicEff;
		ReadItem(strMagicEff, szSkill_MagicEff, CANEMPTY);
		CCfgMagicEffCheck::CheckMagicEffExist(strMagicEff);

		float fSpeed = TableFile.GetFloat(i, szMagic_Speed, 10);
		if (fSpeed <= 0.0f)
		{
			stringstream ExpStr;
			ExpStr << "第" << i << "行的" << "子弹魔法速度不大于零，请更正";
			GenExpInfo(ExpStr.str());
		}

		// 特效名检查
		string strFX = TableFile.GetString(i, szMagic_FxName);
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

void CCfgBulletMagicCheck::EndCheck()
{
	ms_setRowName.clear();
}

bool CCfgBulletMagicCheck::CheckExist(const string& strName)
{
	SetRowName::iterator iter = ms_setRowName.find(strName);
	if (iter == ms_setRowName.end())
	{
		stringstream ExpStr;
		ExpStr << " 违反魔法操作约束: 子弹魔法" << " [" << strName << "] 不存在 ";
		CfgChk::GenExpInfo(ExpStr.str());
		return false;
	}
	return true;
}
