#include "stdafx.h"
#include "CTxtTableFile.h"
#include "CFacialCfg.h"
#include "BaseHelper.h"
#include "LoadSkillCommon.h"
#include "StringHelper.h"

namespace sqr
{
	extern const wstring PATH_ALIAS_CFG;
}

CFacialAniCfgclient::MapFacialAniCfgClientByName CFacialAniCfgclient::m_mapCfgByName;

bool CFacialAniCfgclient::LoadConfig(const TCHAR* cfgFile)
{
	CTxtTableFile TabFile;
	stringstream ExpStr;				
	CFacialAniCfgclient*	pCfgNode;

	if (!TabFile.Load(PATH_ALIAS_CFG.c_str(), cfgFile)) return false;

	ClearMap(m_mapCfgByName);

	for(int32 i=1; i<TabFile.GetHeight(); i++)
	{

		pCfgNode = new CFacialAniCfgclient;
		//pCfgNode->m_uId = i;
		pCfgNode->m_sName = TabFile.GetString(i, szFacial_Name);
		trimend(pCfgNode->m_sName);
		pCfgNode->m_sAniName = TabFile.GetString(i, szFacial_AniName);
		trimend(pCfgNode->m_sAniName);
		pCfgNode->m_sAniPath = TabFile.GetString(i, szFacial_AniPath);
		trimend(pCfgNode->m_sAniPath);

		pair<MapFacialAniCfgClientByName::iterator, bool> pr = m_mapCfgByName.insert(make_pair(pCfgNode->m_sName, pCfgNode));
		if(!pr.second)
		{
			ExpStr << "第" << i << "行的" << "表情动作" << pCfgNode->m_sName << "重复";
			GenErr(ExpStr.str());
			//return false;
		}
	}
	return true;
}

void CFacialAniCfgclient::UnloadConfig()
{
	ClearMap(m_mapCfgByName);
}

CFacialAniCfgclient* CFacialAniCfgclient::GetByName(const string& szName)
{
	stringstream ExpStr;
	CFacialAniCfgclient::MapFacialAniCfgClientByName::iterator mapCfgItr;
	mapCfgItr = m_mapCfgByName.find(szName);

	if (mapCfgItr == m_mapCfgByName.end())
	{
		ExpStr << "表情动作" << szName << "不存在";
		GenErr(ExpStr.str());
		//return NULL;
	}
	return mapCfgItr->second;
}