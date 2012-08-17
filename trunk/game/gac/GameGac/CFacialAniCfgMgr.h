#pragma once
#include "ActorDef.h"
#include "CConfigMallocObject.h"
#include "TConfigAllocator.h"

class CFacialAniCfgMgr : public CConfigMallocObject
{
public:

	static bool LoadConfig(const string& szFileName);
	static void UnloadConfig();

	static CFacialAniCfgMgr*	GetFicialAniCfgByName(const TCHAR* actionName);
	static  CFacialAniCfgMgr*	GetFicialAniCfgByIndex(uint8 uIndex);
	const string&				GetStartAniName()		{ return m_strStartAniName;}
	const string&				GetContinueAniName()	 { return m_strContinueAniName;}
	const string&				GetFacialAniName(EActionState eActionState);
	uint8						GetFacialIndex() { return m_uIndex;}
	bool isHaveFirstAni();

private:
	typedef map<string, CFacialAniCfgMgr*, less<string>, 
		TConfigAllocator<pair<string, CFacialAniCfgMgr* > > >	MapFacialAniCfgByName;
	typedef map<uint8, string, less<uint8>, 
		TConfigAllocator<pair<uint8, string > > >				MapIndexToNameFicial;

	static MapFacialAniCfgByName				ms_mapFacialAniCfgByName;
	static MapIndexToNameFicial				ms_mapIndexToNameFicial;
	string m_strFacialAniName;
	string m_strStartAniName;
	string m_strContinueAniName;
	uint8 m_uIndex;

};
