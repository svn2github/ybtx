#include "stdafx.h"
#include "CCfgAllMagicCheck.h"
#include "CTxtTableFile.h"
#include "CCfgColChecker.inl"
#include "LoadSkillCommon.h"

CCfgPositionMagicCheck::MapPositionMagic CCfgPositionMagicCheck::ms_mapPositionMagic;

CCfgPositionMagicCheck::CCfgPositionMagicCheck()
:m_strName("")
,m_pTouchMagicEff(NULL)
,m_bMutex(false)
{
}

CCfgPositionMagicCheck::~CCfgPositionMagicCheck()
{
}

bool CCfgPositionMagicCheck::Check(const TCHAR* cfgFile)
{
	using namespace CfgChk;

	CTxtTableFile TableFile;
	SetTabFile(TableFile, "位置魔法");
	if (!TableFile.Load(PATH_ALIAS_CFG.c_str(), cfgFile))
	{
		stringstream ExpStr;
		ExpStr << "配置表 加载失败，请查看文件名[" << cfgFile << "]是否正确，或文件是否存在";
		GenExpInfo(ExpStr.str());
	}

	static const uint32 s_uTableFileWide = 10;
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
		MapPositionMagic::iterator iter = ms_mapPositionMagic.find(strName);
		if (iter != ms_mapPositionMagic.end())
		{
			stringstream ExpStr;
			ExpStr << " 配置表: " << g_sTabName << " 第 " << i << " 行的 " << szMagic_Name << "["<< strName << "]" << "重复";
			GenExpInfo(ExpStr.str());
		}
		else
		{
			CCfgPositionMagicCheck* pPosition = new CCfgPositionMagicCheck;
			pPosition->m_strName = strName;
			if (!strTouchEff.empty() && CCfgMagicEffCheck::CheckMagicEffExist(strTouchEff))
			{
				CCfgMagicEffCheck::MapMagicEff::iterator itr = CCfgMagicEffCheck::ms_mapMagicEff.find(strTouchEff);
				pPosition->m_pTouchMagicEff = (*itr).second;
			}
			string strMutextype;
			ReadItem(strMutextype, szMagic_MutexType, CANEMPTY);
			if (strMutextype == "是")
				pPosition->m_bMutex = true;

			ms_mapPositionMagic.insert(make_pair(strName, pPosition));
		}
		
		string strDotEff, strFinalEff;
		ReadItem(strDotEff,		szMagic_DotMagicEff,	CANEMPTY);
		if (!strDotEff.empty())
		{
			size_t comma = strDotEff.find(',');
			SetItemTitle(szMagic_DotMagicEff);
			if(comma != string::npos)
			{
				string strDotMagicEff = strDotEff.substr(0, comma);
				CCfgMagicEffCheck::CheckMagicEffExist(strDotMagicEff);
			}
		}
	
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

void CCfgPositionMagicCheck::EndCheck()
{
	ClearMap(ms_mapPositionMagic);
}

bool CCfgPositionMagicCheck::CheckExist(const string& strName)
{
	MapPositionMagic::iterator iter = ms_mapPositionMagic.find(strName);
	if (iter == ms_mapPositionMagic.end())
	{
		stringstream ExpStr;
		ExpStr << " 违反魔法操作约束: 位置魔法" << " [" << strName << "] 不存在 ";
		CfgChk::GenExpInfo(ExpStr.str());
		return false;
	}
	return true;
}
