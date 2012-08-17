#include "stdafx.h"
#include "CSoundConfig.h"
#include "CTxtTableFile.h"
#include "LoadSkillCommon.h"
#include "ExpHelper.h"
#include "BaseHelper.h"

namespace sqr
{
	extern const wstring PATH_ALIAS_CFG;
}

CSoundConfig::MapSoundCfgByName CSoundConfig::ms_mapSoundCfgByName;

bool CSoundConfig::LoadConfig(const TCHAR* szPlayerCfgFile, const TCHAR* szNPCCfgFile)
{
	CTxtTableFile TabFile;
	stringstream ExpStr;				
	CSoundConfig* pCfgNode = NULL;

	ClearMap(ms_mapSoundCfgByName);

	// Player
	if (!TabFile.Load(PATH_ALIAS_CFG.c_str(), szPlayerCfgFile)) 
		return false;

	for(int32 i = 1; i < TabFile.GetHeight(); ++i)
	{
		pCfgNode = new CSoundConfig;
		pCfgNode->m_strIndexName	= TabFile.GetString(i, szSound_PlayerTypeName);
		pCfgNode->m_strSuffer		= TabFile.GetString(i, szSound_Suffer);
		pCfgNode->m_strDie			= TabFile.GetString(i, szSound_Die);
		pCfgNode->m_strFootstep		= TabFile.GetString(i, szSound_Footstep);
		pair<MapSoundCfgByName::iterator, bool> pr = ms_mapSoundCfgByName.insert(make_pair(pCfgNode->m_strIndexName, pCfgNode));
		if(!pr.second)
		{
			ExpStr << szPlayerCfgFile << "第" << i << "行的" << "音效" << pCfgNode->m_strIndexName << "重复";
			GenErr(ExpStr.str());
		}
	}

	// NPC
	if (!TabFile.Load(PATH_ALIAS_CFG.c_str(), szNPCCfgFile)) 
		return false;

	for(int32 i = 1; i < TabFile.GetHeight(); ++i)
	{
		pCfgNode = new CSoundConfig;
		pCfgNode->m_strIndexName	= TabFile.GetString(i, szSound_AniFileName);
		pCfgNode->m_strEnterBattle	= TabFile.GetString(i, szSound_EnterBattle);
		pCfgNode->m_strSuffer		= TabFile.GetString(i, szSound_Suffer);
		pCfgNode->m_strWeaponType	= TabFile.GetString(i, szSound_WeaponType);
		pCfgNode->m_strArmorType	= TabFile.GetString(i, szSound_ArmorType);
		pCfgNode->m_strFootstep		= TabFile.GetString(i, szSound_Footstep);
		pair<MapSoundCfgByName::iterator, bool> pr = ms_mapSoundCfgByName.insert(make_pair(pCfgNode->m_strIndexName, pCfgNode));
		if(!pr.second)
		{
			ExpStr << szNPCCfgFile << "第" << i << "行的" << "音效" << pCfgNode->m_strIndexName << "重复";
			GenErr(ExpStr.str());
		}
	}

	return true;
}

void CSoundConfig::UnloadConfig()
{
	ClearMap(ms_mapSoundCfgByName);
}

CSoundConfig* CSoundConfig::GetSoundCfg(string strName)
{
	CSoundConfig::MapSoundCfgByName::iterator mapCfgItr;
	mapCfgItr = ms_mapSoundCfgByName.find(strName);

	if (mapCfgItr == ms_mapSoundCfgByName.end())
	{
		return NULL;
	}
	return mapCfgItr->second;
}

string CSoundConfig::GetMeterialString(uint32 uMeterialID)
{
	switch (uMeterialID)
	{
		case 0: return "grass"; break;
		case 1: return "dirt";	break;
		case 2: return "wood";	break;
		case 3: return "stone"; break;
		case 4: return "water"; break;
		case 5: return "sand";	break;
		case 6: return "snow";	break;
		case 7: return "metal"; break;
	}
	return "";
}
