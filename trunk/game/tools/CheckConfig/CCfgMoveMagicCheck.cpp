#include "stdafx.h"
#include "CCfgAllMagicCheck.h"
#include "CTxtTableFile.h"
#include "CCfgColChecker.inl"
#include "LoadSkillCommon.h"
#include "CMoveMagicCfg.h"
#include "CSkillCfg.h"

CMoveMagicCfg::MapMoveArgLimit CMoveMagicCfg::ms_mapMoveArgLimit;
CMoveMagicCfg::MapMoveArg CMoveMagicCfg::ms_mapMoveArg;
CSkillCfg::MapOperateObjectType CSkillCfg::ms_mapOperateObjectType;
CCfgMoveMagicCheck::MapMoveMagic CCfgMoveMagicCheck::ms_mapMoveMagic;

CCfgMoveMagicCheck::CCfgMoveMagicCheck()
:m_strName("")
,m_pCancelMagicEff(NULL)
{
}

CCfgMoveMagicCheck::~CCfgMoveMagicCheck()
{
}

bool CCfgMoveMagicCheck::Check(const TCHAR* cfgFile)
{
	using namespace CfgChk;

	CMoveMagicCfg::InitMapMoveArgLimit();
	CMoveMagicCfg::InitMapMoveArg();
	CSkillCfg::InitMapOperateObjectType();

	CTxtTableFile TableFile;
	SetTabFile(TableFile, "移动魔法");
	if (!TableFile.Load(PATH_ALIAS_CFG.c_str(), cfgFile))
	{
		stringstream ExpStr;
		ExpStr << "配置表 加载失败，请查看文件名[" << cfgFile << "]是否正确，或文件是否存在";
		GenExpInfo(ExpStr.str());
	}

	static const uint32 s_uTableFileWide = 16;
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

		string strMagicEff, strCancelEff;
		ReadItem(strMagicEff,	szMagic_MagicEff,			CANEMPTY);
		CCfgMagicEffCheck::CheckMagicEffExist(strMagicEff);
		ReadItem(strCancelEff,	szMagic_CancelableMagicEff,	CANEMPTY);
		CCfgMagicEffCheck::CheckMagicEffExist(strCancelEff);
		MapMoveMagic::iterator iter = ms_mapMoveMagic.find(strName);
		if (iter != ms_mapMoveMagic.end())
		{
			stringstream ExpStr;
			ExpStr << " 配置表: " << g_sTabName << " 第 " << i << " 行的 " << szMagic_Name << "["<< strName << "]" << "重复";
			GenExpInfo(ExpStr.str());
		}
		else
		{
			CCfgMoveMagicCheck* pMoveMagic = new CCfgMoveMagicCheck;
			pMoveMagic->m_strName = strName;
			if (!strCancelEff.empty() && CCfgMagicEffCheck::CheckMagicEffExist(strCancelEff))
			{
				CCfgMagicEffCheck::MapMagicEff::iterator itr = CCfgMagicEffCheck::ms_mapMagicEff.find(strCancelEff);
				pMoveMagic->m_pCancelMagicEff = (*itr).second;
			}
			ms_mapMoveMagic.insert(make_pair(strName, pMoveMagic));
		}

		if(!strMagicEff.empty() && !strCancelEff.empty() && strMagicEff == strCancelEff)
		{
			stringstream str;
			str << "\n第" << i << "行违反表约束：同一行的不同魔法效果不能同名";
			GenExpInfo(str.str());
		}

		CCfgCalc* pMoveArg = new CCfgCalc;
		string strMoveArg;
		ReadItem(strMoveArg,					szMagic_MoveArg);
		pMoveArg->InputString(strMoveArg);
		if(pMoveArg->GetStringCount()>1)
		{
			if (pMoveArg->GetStringCount()<3)
			{
				stringstream ExpStr;
				ExpStr << "配置表第" << (i + 1) << "行的技能[" << TableFile.GetString(i, szMagic_Name) << "移动参数少于3个";
				GenExpInfo(ExpStr.str());
			}

			string strMoveArgLimit = pMoveArg->GetString(1);
			if (!CMoveMagicCfg::ms_mapMoveArgLimit.count(strMoveArgLimit))
			{
				stringstream ExpStr;
				ExpStr << "配置表第" << (i + 1) << "行的技能[" << TableFile.GetString(i, szMagic_Name) << "]移动参数限制[" << strMoveArgLimit << "]错误";
				GenExpInfo(ExpStr.str());
			}	
		}

		string strMoveArgType = pMoveArg->GetString(0);
		if (!CMoveMagicCfg::ms_mapMoveArg.count(strMoveArgType))
		{
			stringstream ExpStr;
			ExpStr << "配置表第" << (i + 1) << "行的技能[" << TableFile.GetString(i, szMagic_Name) << "]移动参数[" << strMoveArgType << "]错误";
			GenExpInfo(ExpStr.str());
		}
		delete pMoveArg;

		string strOpObject;
		ReadItem(strOpObject,		szMagic_OperateObject);
		CCfgCalc* pOperateObject = new CCfgCalc;
		pOperateObject->InputString(strOpObject);
		string strOperateObject = pOperateObject->GetString(0);
		if (!CSkillCfg::ms_mapOperateObjectType.count(strOperateObject))
		{
			stringstream ExpStr;
			ExpStr << "配置表第" << (i + 1) << "行的技能[" << TableFile.GetString(i, szMagic_Name) << "]作用对象[" << strOperateObject << "]错误";
			GenExpInfo(ExpStr.str());
		}
		
		if (pOperateObject->GetStringCount()>1)
		{
			string strAreaOperateObject = pOperateObject->GetString(1);
			if (!CSkillCfg::ms_mapOperateObjectType.count(strAreaOperateObject))
			{
				stringstream ExpStr;
				ExpStr << "配置表第" << (i + 1) << "行的技能[" << TableFile.GetString(i, szMagic_Name) << "]作用对象[" << strAreaOperateObject << "]错误";
				GenExpInfo(ExpStr.str());
			}
		}
		delete pOperateObject;

		// 特效名检查
		string strFX = TableFile.GetString(i, szMagic_ProcessFxName);
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

void CCfgMoveMagicCheck::EndCheck()
{
	CMoveMagicCfg::ms_mapMoveArg.clear();
	CMoveMagicCfg::ms_mapMoveArgLimit.clear();
	CSkillCfg::ms_mapOperateObjectType.clear();
	ClearMap(ms_mapMoveMagic);
}

bool CCfgMoveMagicCheck::CheckExist(const string& strName)
{
	MapMoveMagic::iterator iter = ms_mapMoveMagic.find(strName);
	if (iter == ms_mapMoveMagic.end())
	{
		stringstream ExpStr;
		ExpStr << " 违反魔法操作约束: 移动魔法" << " [" << strName << "] 不存在 ";
		CfgChk::GenExpInfo(ExpStr.str());
		return false;
	}
	return true;
}
