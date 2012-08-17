#pragma once

class CForceNoSaveStateCfgServer
{
public:
	static bool	Check(const TCHAR* cfgFile);
	static void	EndCheck();

	typedef set<string>	SetRowName;
	static SetRowName	ms_setRowName;
};
