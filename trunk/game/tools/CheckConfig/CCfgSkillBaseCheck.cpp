#include "stdafx.h"
#include "CCfgSkillBaseCheck.h"
#include "CCfgColChecker.inl"

CCfgSkillBaseCheck::SetRowName CCfgSkillBaseCheck::ms_setRowName;
string CCfgSkillBaseCheck::m_strTableName;

void CCfgSkillBaseCheck::Load(CTxtTableFile& TxtTableFile, const TCHAR* cfgFile, string strTableName)
{
	EndCheckOverlap();
	CfgChk::SetTabFile(TxtTableFile, strTableName);
	m_strTableName = strTableName;
	if (!TxtTableFile.Load(PATH_ALIAS_CFG.c_str(), cfgFile))
	{
		stringstream ExpStr;
		ExpStr << " 配置表: " << strTableName << " 加载失败，请查看文件名[" << cfgFile << "]是否正确，或文件是否存在";
		CfgChk::GenExpInfo(ExpStr.str());
	}
}

bool CCfgSkillBaseCheck::CheckOverlap(string strColName, int32 nRowNum)
{
	CfgChk::SetLineNo(nRowNum);
	string strName;
	CfgChk::ReadItem(strName, strColName.c_str(), CANEMPTY);
	trimend(strName);
	SetRowName::iterator iter = ms_setRowName.find(strName);
	if (iter != ms_setRowName.end())
	{
		stringstream ExpStr;
		ExpStr << " 配置表: " << m_strTableName << " 第 " << nRowNum << " 行的 " << strColName << "["<< strName << "]" << "重复";
		CfgChk::GenExpInfo(ExpStr.str());
	}
	else
	{
		ms_setRowName.insert(strName);
	}

	return true;
}

void CCfgSkillBaseCheck::EndCheckOverlap()
{
	ms_setRowName.clear();
}

bool CCfgSkillBaseCheck::CheckExist(const string& strMagicName)
{
	SetRowName::iterator iter = ms_setRowName.find(strMagicName);
	if (iter == ms_setRowName.end())
	{
		stringstream ExpStr;
		ExpStr << " 违反魔法操作约束: " << m_strTableName << " [" << strMagicName << "] 不存在 ";
		CfgChk::GenExpInfo(ExpStr.str());
		return false;
	}
	return true;
}
