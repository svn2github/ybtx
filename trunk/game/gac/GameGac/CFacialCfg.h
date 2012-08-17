#pragma once
#include "CConfigMallocObject.h"
#include "TConfigAllocator.h"

class CFacialAniCfgclient : public CConfigMallocObject
{
public:
	typedef map<string, CFacialAniCfgclient*, less<string>, 
		TConfigAllocator<pair<string, CFacialAniCfgclient* > > >				MapFacialAniCfgClientByName;
	static bool									LoadConfig(const TCHAR* cfgFile);
	static void									UnloadConfig();
	static CFacialAniCfgclient*					GetByName(const string& name);				
	const TCHAR*			GetName()				{return m_sName.c_str();}
	const TCHAR*			GetAniName()			{return m_sAniName.c_str();}
	const TCHAR*			GetAniPath()			{return m_sAniPath.c_str();}

private:
	static MapFacialAniCfgClientByName			m_mapCfgByName;
	string				m_sName;				// 动作名
	string				m_sAniName;				// 动作动画名
	string				m_sAniPath;				// 动作动画路径
};