#pragma once

class CBaseStateCfgServer;

class CPersistentStateCfgServer
{
	typedef map<string, CPersistentStateCfgServer*>		MapPersistentStateCfgServer;
public:
	static bool							LoadConfig(const TCHAR* cfgFile);
	static void							UnloadConfig();

	//const string&						GetName()					{return m_sName;}

private:
	//static MapPersistentStateCfgServer			m_mapCfg;

	//string								m_sName;
};

