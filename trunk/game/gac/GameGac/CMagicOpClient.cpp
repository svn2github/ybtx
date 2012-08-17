#include "stdafx.h"
#include "CMagicOpClient.h"
#include "LoadSkillCommon.h"
#include "CTxtTableFile.h"
#include "StringHelper.h"

namespace sqr
{
	extern const wstring PATH_ALIAS_CFG;
}

CMagicOpCfgClient::MapMagicOpCfgClient CMagicOpCfgClient::m_mapCfgById;
uint32	CMagicOpCfgClient::ms_uMaxLineNum = 0;
CMagicOpCfgClient::MapFxType CMagicOpCfgClient::ms_mapFxType;

bool CMagicOpCfgClient::LoadConfig(const TCHAR* cfgFile, bool bFirstFile)
{
	if (bFirstFile)
	{
		InitMapFxType();
		ClearMap(m_mapCfgById);
	}
	CTxtTableFile TabFile;
	CMagicOpCfgClient*	pCfgNode;

	if (!TabFile.Load(PATH_ALIAS_CFG.c_str(), cfgFile)) return false;

	for(uint32 i=ms_uMaxLineNum+1; i<TabFile.GetHeight()+ms_uMaxLineNum; i++)
	{
		uint32 uLineNum = i-ms_uMaxLineNum;
		string StrName = TabFile.GetString(uLineNum, szMagicEff_Name);
		trimend(StrName);
		if(StrName.empty()) 
		{
			continue;
		}
		pCfgNode = new CMagicOpCfgClient;
		pCfgNode->m_uId = i;
		pCfgNode->m_sName=StrName;
		pCfgNode->m_sFX = TabFile.GetString(uLineNum, szMagicEff_FXName);
		trimend(pCfgNode->m_sFX);
		string strFxType	= TabFile.GetString(uLineNum, szMagicEff_FxType);
		trimend(strFxType);
		pCfgNode->m_eFxType=ms_mapFxType[strFxType];
		
		m_mapCfgById.insert(make_pair(pCfgNode->m_uId, pCfgNode));
	}
	ms_uMaxLineNum = ms_uMaxLineNum + TabFile.GetHeight();
	return true;
}

void CMagicOpCfgClient::UnloadConfig()
{
	ClearMap(m_mapCfgById);
}

CMagicOpCfgClient* CMagicOpCfgClient::GetById(uint32 uId)
{
	stringstream ExpStr;
	CMagicOpCfgClient::MapMagicOpCfgClient::iterator mapCfgItr;
	mapCfgItr = m_mapCfgById.find(uId);
	if (mapCfgItr == m_mapCfgById.end())
	{
		ExpStr << "魔法操作" << uId << "不存在";
		GenErr(ExpStr.str());
		//return NULL;
	}
	return mapCfgItr->second;
}
