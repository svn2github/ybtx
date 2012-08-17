#include "stdafx.h"
#include "CForceNoSaveState.h"
#include "CCfgColChecker.inl"
#include "LoadSkillCommon.h"
#include "CBaseStateServer.h"

namespace sqr
{
	extern const wstring PATH_ALIAS_CFG;
}

//CForceNoSaveStateCfgServer::MapForceNoSaveStateCfgServer CForceNoSaveStateCfgServer::m_mapCfg;
CForceNoSaveStateCfgServer::MapFSOFNSType CForceNoSaveStateCfgServer::ms_mapForceSaveOrForceNoSaveType;

bool CForceNoSaveStateCfgServer::LoadConfig(const TCHAR* cfgFile)
{
	using namespace CfgChk;

	CreateMap(ms_mapForceSaveOrForceNoSaveType, 3, ("不存"), ("下线存"), ("定时存"));

	CTxtTableFile TabFile;
	//CForceNoSaveStateCfgServer*	pCfgNode;
	SetTabFile(TabFile, "强制存不存数据库的状态");
	if (!TabFile.Load(PATH_ALIAS_CFG.c_str(), cfgFile)) return false;

	UnloadConfig();

	string sName;
	EForceSaveOrForceNoSave eSave;

	for(int32 i=1; i<TabFile.GetHeight(); ++i)
	{
		SetLineNo(i);
		//pCfgNode = new CForceNoSaveStateCfgServer;
		ReadItem(sName,					szGlobalState_Name);
		ReadItem(eSave,					szState_ForceSaveOrForceNoSave,		ms_mapForceSaveOrForceNoSaveType);
		
		CBaseStateCfgServer* pGlobalCfg = static_cast<CBaseStateCfgServer*>(CBaseStateCfgServer::GetByGlobalName(sName));
		if(pGlobalCfg)
		{
			pGlobalCfg->m_eNeedSaveToDB = eSave;
		}

		//InsertMap(m_mapCfg, pCfgNode);
	}
	
	return true;
}

void CForceNoSaveStateCfgServer::UnloadConfig()
{
	//ClearMap(m_mapCfg);
}

