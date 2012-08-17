#pragma once
#include "TConfigAllocator.h"
#include "FightDef.h"

class CBaseStateCfgServer;

class CForceNoSaveStateCfgServer
{
	typedef map<string, CForceNoSaveStateCfgServer*, less<string>, TConfigAllocator<pair<string, CForceNoSaveStateCfgServer* > > > MapForceNoSaveStateCfgServer;
	typedef map<string, EForceSaveOrForceNoSave, less<string>, TConfigAllocator<pair<string, EForceSaveOrForceNoSave > > > MapFSOFNSType;

public:
	static bool							LoadConfig(const TCHAR* cfgFile);
	static void							UnloadConfig();
	static MapFSOFNSType				ms_mapForceSaveOrForceNoSaveType;

	//const string&						GetName()					{return m_sName;}

private:
	//static MapForceNoSaveStateCfgServer			m_mapCfg;

	//string								m_sName;
};

