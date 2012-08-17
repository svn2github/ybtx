#include "stdafx.h"
#include "LoadSkillCommon.h"
#include "CTxtTableFile.h"
#include "CPositionMagicClient.h"
#include "BaseHelper.h"
#include "StringHelper.h"

namespace sqr
{
	extern const wstring PATH_ALIAS_CFG;
}

CPositionMagicCfgClient::MapPositionMagicCfgClientById CPositionMagicCfgClient::m_mapCfgById;

bool CPositionMagicCfgClient::LoadConfig(const TCHAR* cfgFile)
{
	CTxtTableFile TabFile;
	CPositionMagicCfgClient*	pCfgNode;

	if (!TabFile.Load(PATH_ALIAS_CFG.c_str(), cfgFile)) return false;

	ClearMap(m_mapCfgById);

	for(int32 i=1; i<TabFile.GetHeight(); i++)
	{
		pCfgNode = new CPositionMagicCfgClient;
		pCfgNode->m_uId = i;
		pCfgNode->m_sFX = TabFile.GetString(i, szMagic_FxName);
		trimend(pCfgNode->m_sFX);
		CPositionMagicCfgClientSharedPtr* pCfgNodeSharedPtr = new CPositionMagicCfgClientSharedPtr(pCfgNode);
		m_mapCfgById.insert(make_pair(pCfgNode->m_uId, pCfgNodeSharedPtr));
	}
	return true;
}

void CPositionMagicCfgClient::UnloadConfig()
{
	ClearMap(m_mapCfgById);
}

CPositionMagicCfgClientSharedPtr& CPositionMagicCfgClient::GetById(uint32 uId)
{
	stringstream ExpStr;
	CPositionMagicCfgClient::MapPositionMagicCfgClientById::iterator mapCfgItr;
	mapCfgItr = m_mapCfgById.find(uId);
	if (mapCfgItr == m_mapCfgById.end())
	{
		ExpStr << "位置魔法" << uId << "不存在";
		GenErr(ExpStr.str());
		//return NULL;
	}
	return *(mapCfgItr->second);
}
