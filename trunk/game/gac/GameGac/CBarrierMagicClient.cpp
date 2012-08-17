#include "stdafx.h"
#include "LoadSkillCommon.h"
#include "CTxtTableFile.h"
#include "CBarrierMagicClient.h"
#include "BaseHelper.h"
#include "StringHelper.h"

namespace sqr
{
	extern const wstring PATH_ALIAS_CFG;
}

CBarrierMagicCfgClient::MapBarrierMagicCfgClientById CBarrierMagicCfgClient::m_mapCfgById;

bool CBarrierMagicCfgClient::LoadConfig(const TCHAR* cfgFile)
{
	CTxtTableFile TabFile;
	CBarrierMagicCfgClient*	pCfgNode;

	if (!TabFile.Load(PATH_ALIAS_CFG.c_str(), cfgFile)) return false;

	ClearMap(m_mapCfgById);

	for(int32 i=1; i<TabFile.GetHeight(); i++)
	{
		pCfgNode = new CBarrierMagicCfgClient;
		pCfgNode->m_uId = i;
		pCfgNode->m_sFX = TabFile.GetString(i, szMagic_FxName);
		trimend(pCfgNode->m_sFX);
		CBarrierMagicCfgClientSharedPtr* pCfgNodeSharedPtr = new CBarrierMagicCfgClientSharedPtr(pCfgNode);
		m_mapCfgById.insert(make_pair(pCfgNode->m_uId, pCfgNodeSharedPtr));
	}
	return true;
}

void CBarrierMagicCfgClient::UnloadConfig()
{
	ClearMap(m_mapCfgById);
}

CBarrierMagicCfgClientSharedPtr& CBarrierMagicCfgClient::GetById(uint32 uId)
{
	stringstream ExpStr;
	CBarrierMagicCfgClient::MapBarrierMagicCfgClientById::iterator mapCfgItr;
	mapCfgItr = m_mapCfgById.find(uId);
	if (mapCfgItr == m_mapCfgById.end())
	{
		ExpStr << "ÕÏ°­Ä§·¨" << uId << "²»´æÔÚ";
		GenErr(ExpStr.str());
	}
	return *(mapCfgItr->second);
}

