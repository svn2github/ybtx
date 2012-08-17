#include "stdafx.h"
#include "CCfgAllMagicCheck.h"
#include "CTxtTableFile.h"
#include "CCfgColChecker.inl"
#include "LoadSkillCommon.h"

CCfgTotemMagicCheck::MapTotemMagic CCfgTotemMagicCheck::ms_mapTotemMagic;

CCfgTotemMagicCheck::CCfgTotemMagicCheck()
:m_strName("")
,m_pTouchMagicEff(NULL)
{
}

CCfgTotemMagicCheck::~CCfgTotemMagicCheck()
{
}

bool CCfgTotemMagicCheck::Check(const TCHAR* cfgFile)
{
	using namespace CfgChk;

	CTxtTableFile TableFile;
	SetTabFile(TableFile, "图腾魔法");
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

	for(int32 i = 1; i < TableFile.GetHeight(); ++i)
	{
		SetLineNo(i);
		string strName, strTouchEff;
		ReadItem(strName, szMagic_Name, CANEMPTY);
		ReadItem(strTouchEff, szMagic_TouchMagicEff, CANEMPTY);
		CCfgMagicEffCheck::CheckMagicEffExist(strTouchEff);
		trimend(strName);
		MapTotemMagic::iterator iter = ms_mapTotemMagic.find(strName);
		if (iter != ms_mapTotemMagic.end())
		{
			stringstream ExpStr;
			ExpStr << " 配置表: " << g_sTabName << " 第 " << i << " 行的 " << szMagic_Name << "["<< strName << "]" << "重复";
			GenExpInfo(ExpStr.str());
		}
		else
		{
			CCfgTotemMagicCheck* pTotem = new CCfgTotemMagicCheck;
			pTotem->m_strName = strName;
			if (!strTouchEff.empty() && CCfgMagicEffCheck::CheckMagicEffExist(strTouchEff))
			{
				CCfgMagicEffCheck::MapMagicEff::iterator itr = CCfgMagicEffCheck::ms_mapMagicEff.find(strTouchEff);
				pTotem->m_pTouchMagicEff = (*itr).second;
			}
			ms_mapTotemMagic.insert(make_pair(strName, pTotem));
		}

		string strDotEff, strFinalEff;
		ReadItem(strDotEff,		szMagic_DotMagicEff,	CANEMPTY);
		CCfgMagicEffCheck::CheckMagicEffExist(strDotEff);
		ReadItem(strFinalEff,	szMagic_FinalMagicEff,	CANEMPTY);
		CCfgMagicEffCheck::CheckMagicEffExist(strFinalEff);

		if ( (!strTouchEff.empty() && !strDotEff.empty() && strTouchEff == strDotEff) ||
			(!strTouchEff.empty() && !strFinalEff.empty() && strTouchEff == strFinalEff) ||
			(!strDotEff.empty() && !strFinalEff.empty() && strDotEff == strFinalEff) )
		{
			stringstream str;
			str << "\n第" << i << "行违反表约束：同一行的不同魔法效果不能同名";
			GenExpInfo(str.str());
		}
	}
	return true;
}

void CCfgTotemMagicCheck::EndCheck()
{
	ClearMap(ms_mapTotemMagic);
}

bool CCfgTotemMagicCheck::CheckExist(const string& strName)
{
	MapTotemMagic::iterator iter = ms_mapTotemMagic.find(strName);
	if (iter == ms_mapTotemMagic.end())
	{
		stringstream ExpStr;
		ExpStr << " 违反魔法操作约束: 图腾魔法" << " [" << strName << "] 不存在 ";
		CfgChk::GenExpInfo(ExpStr.str());
		return false;
	}
	return true;
}
