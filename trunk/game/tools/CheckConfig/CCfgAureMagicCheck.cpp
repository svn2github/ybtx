#include "stdafx.h"
#include "CCfgAllMagicCheck.h"
#include "CTxtTableFile.h"
#include "CCfgColChecker.inl"
#include "LoadSkillCommon.h"

CCfgAureMagicCheck::MapAureMagic CCfgAureMagicCheck::ms_mapAureMagic;

CCfgAureMagicCheck::CCfgAureMagicCheck()
:m_strName("")
,m_bPersistent(false)
,m_pMagicEff(NULL)
{
}

CCfgAureMagicCheck::~CCfgAureMagicCheck()
{
}

bool CCfgAureMagicCheck::Check(const TCHAR* cfgFile)
{
	using namespace CfgChk;

	CTxtTableFile TableFile;
	SetTabFile(TableFile, "姿态技能");
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
		string strName, strType, strStanceType, strMagicEff;
		ReadItem(strName, szMagic_Name, CANEMPTY);
		ReadItem(strStanceType, szMagic_StanceType, CANEMPTY);

		ReadItem(strMagicEff, szSkill_MagicEff, CANEMPTY);
		CCfgMagicEffCheck::CheckMagicEffExist(strMagicEff);

		bool bNeedSaveToDB;
		ReadItem(bNeedSaveToDB,	szAureMagic_SaveToDB, CANEMPTY,	NO);

		trimend(strName);
		MapAureMagic::iterator iter = ms_mapAureMagic.find(strName);
		if (iter != ms_mapAureMagic.end())
		{
			stringstream ExpStr;
			ExpStr << " 配置表: " << g_sTabName << " 第 " << i << " 行的 " << szMagic_Name << "["<< strName << "]" << "重复";
			GenExpInfo(ExpStr.str());
		}
		else
		{	
			CCfgAureMagicCheck* pAure = new CCfgAureMagicCheck;
			pAure->m_strName = strName;
			pAure->m_bNeedSaveToDB = bNeedSaveToDB;
			pAure->m_bPersistent = (strStanceType == "姿态" || strStanceType == "互斥姿态") ? true : false;
			if (!strMagicEff.empty() && CCfgMagicEffCheck::CheckMagicEffExist(strMagicEff))
			{
				CCfgMagicEffCheck::MapMagicEff::iterator itr = CCfgMagicEffCheck::ms_mapMagicEff.find(strMagicEff);
				pAure->m_pMagicEff = (*itr).second;
			}
			ms_mapAureMagic.insert(make_pair(strName, pAure));
		}

		// 特效名检查
		string strFX = TableFile.GetString(i, szMagic_FxName);
		if (strFX != "")
		{
			vector<string> sFXTable = CCfgMagicOp::Split(strFX, ",");
			if (sFXTable.size() == 2 || sFXTable.size() == 3)
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

void CCfgAureMagicCheck::EndCheck()
{
	ClearMap(ms_mapAureMagic);
}

bool CCfgAureMagicCheck::CheckExist(const string& strName)
{
	MapAureMagic::iterator iter = ms_mapAureMagic.find(strName);
	if (iter == ms_mapAureMagic.end())
	{
		stringstream ExpStr;
		ExpStr << " 违反魔法操作约束: 姿态技能" << " [" << strName << "] 不存在 ";
		CfgChk::GenExpInfo(ExpStr.str());
		return false;
	}
	return true;
}
