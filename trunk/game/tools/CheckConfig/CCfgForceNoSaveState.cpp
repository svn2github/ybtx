#include "stdafx.h"
#include "CCfgForceNoSaveState.h"
#include "CCfgColChecker.inl"
#include "LoadSkillCommon.h"
#include "CCfgAllStateCheck.h"

namespace sqr
{
	extern const wstring PATH_ALIAS_CFG;
}

CForceNoSaveStateCfgServer::SetRowName CForceNoSaveStateCfgServer::ms_setRowName;

bool CForceNoSaveStateCfgServer::Check(const TCHAR* cfgFile)
{
	using namespace CfgChk;

	CTxtTableFile TabFile;
	SetTabFile(TabFile, "强制存不存数据库的状态");
	if (!TabFile.Load(PATH_ALIAS_CFG.c_str(), cfgFile)) return false;

	string sName;
	for(int32 i=1; i<TabFile.GetHeight(); ++i)
	{
		SetLineNo(i);
		ReadItem(sName,	szGlobalState_Name);
		string sSave;
		ReadItem(sSave,	szState_ForceSaveOrForceNoSave);

		CCfgAllStateCheck* pGlobalCfg = CCfgAllStateCheck::GetStateByName(sName);
		if(pGlobalCfg)
		{
			if(sSave == "下线存" || sSave == "定时存" )
			{
				pGlobalCfg->m_bNeedSaveToDB = true;
			}
			else if (sSave == "不存")
			{
				pGlobalCfg->m_bNeedSaveToDB = false;
			}
			else
			{
				stringstream ExpStr;
				ExpStr << " 配置表: " << g_sTabName << " 第 " << i << " 行的 " << szState_ForceSaveOrForceNoSave << "["<< sSave << "]" << "不符合规则";
				GenExpInfo(ExpStr.str());
			}
		}
		SetRowName::iterator iter = ms_setRowName.find(sName);
		if (iter != ms_setRowName.end())
		{
			stringstream ExpStr;
			ExpStr << " 配置表: " << g_sTabName << " 第 " << i << " 行的 " << szMagicState_Name << "["<< sName << "]" << "重复";
			GenExpInfo(ExpStr.str());
		}
		else
		{
			ms_setRowName.insert(sName);
		}
	}

	return true;
}

void CForceNoSaveStateCfgServer::EndCheck()
{
}

