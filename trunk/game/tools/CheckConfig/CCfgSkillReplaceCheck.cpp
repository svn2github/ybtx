#include "stdafx.h"
#include "CCfgSkillReplaceCheck.h"
#include "CTxtTableFile.h"
#include "LoadSkillCommon.h"
#include "CCfgColChecker.inl"
#include "CCfgAllStateCheck.h"
#include "CCfgSkillCheck.h"

CCfgSkillReplaceCheck::MultiMapCSkillReplace CCfgSkillReplaceCheck::ms_multimapSkillReplace;

bool CCfgSkillReplaceCheck::Check(const TCHAR* cfgFile)
{
	using namespace CfgChk;

	CTxtTableFile TableFile;
	SetTabFile(TableFile, "替换技能");
	if (!TableFile.Load(PATH_ALIAS_CFG.c_str(), cfgFile))
	{
		stringstream ExpStr;
		ExpStr << "配置表 加载失败，请查看文件名[" << cfgFile << "]是否正确，或文件是否存在";
		GenExpInfo(ExpStr.str());
	}

	static const uint32 s_uTableFileWide = 3;
	if (TableFile.GetWidth() != s_uTableFileWide)
	{
		stringstream ExpStr;
		ExpStr << "配置表 列数错误 应为: " << s_uTableFileWide << " 列，实为: " << TableFile.GetWidth() << " 列";
		GenExpInfo(ExpStr.str());
	}

	for(int32 i = 1; i < TableFile.GetHeight(); ++i)
	{
		SetLineNo(i);
		CCfgSkillReplaceCheck* pSkillReplace = new CCfgSkillReplaceCheck;
		ReadItem(pSkillReplace->m_sSkillName,			szSkillReplace_SkillName);
		ReadItem(pSkillReplace->m_sStateName,			szSkillReplace_StateName);
		ReadItem(pSkillReplace->m_sReplaceSkillName,	szSkillReplace_ReplaceName);

		//检测
		if(!CCfgMagicStateCheck::CheckExist(pSkillReplace->m_sStateName))
		{
			stringstream ExpStr;
			ExpStr << "配置表[技能替换]第" << (i + 1) << "行的的条件状态" << pSkillReplace->m_sStateName.c_str() << "不存在";
			GenExpInfo(ExpStr.str());
		}
		if (!CCfgSkillCheck::IsPlayerSkillNameValid(pSkillReplace->m_sSkillName.c_str()))
		{
			stringstream ExpStr;
			ExpStr << "配置表[技能替换]第" << (i + 1) << "行的的技能" << pSkillReplace->m_sSkillName.c_str() << "不存在";
			GenExpInfo(ExpStr.str());
		}
		if (!CCfgSkillCheck::IsPlayerSkillNameValid(pSkillReplace->m_sReplaceSkillName.c_str()))
		{
			stringstream ExpStr;
			ExpStr << "配置表[技能替换]第" << (i + 1) << "行的替换技能" << pSkillReplace->m_sReplaceSkillName.c_str() << "不存在";
			GenExpInfo(ExpStr.str());
		}

		//替换技能列不可有重复的
		MultiMapCSkillReplace::iterator it = ms_multimapSkillReplace.begin();
		MultiMapCSkillReplace::iterator itEnd = ms_multimapSkillReplace.end();
		for (;it!=itEnd;it++)
		{
			if (strcmp((*it).second->GetStateName().c_str(), pSkillReplace->m_sStateName.c_str())==0)
			{
				stringstream ExpStr;
				ExpStr << "配置表[技能替换]第" << (i + 1) << "行的的条件状态" << pSkillReplace->m_sStateName.c_str() << "重复";
				GenExpInfo(ExpStr.str());
			}
			if (strcmp((*it).second->GetReplaceSkillName().c_str(), pSkillReplace->m_sReplaceSkillName.c_str())==0)
			{
				stringstream ExpStr;
				ExpStr << "配置表[技能替换]第" << (i + 1) << "行的替换技能" << pSkillReplace->m_sReplaceSkillName.c_str() << "重复";
				GenExpInfo(ExpStr.str());
			}
		}
		ms_multimapSkillReplace.insert(pair<string, CCfgSkillReplaceCheck*>(pSkillReplace->m_sSkillName,pSkillReplace));
	}
	return true;
}

void CCfgSkillReplaceCheck::EndCheck()
{
	ClearMap(ms_multimapSkillReplace);
}
