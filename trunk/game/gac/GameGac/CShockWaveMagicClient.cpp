#include "stdafx.h"
#include "LoadSkillCommon.h"
#include "CRenderObject.h"
#include "CTxtTableFile.h"
#include "CShockWaveMagicClient.h"
#include "CCoreObjectFollower.h"
#include "StringHelper.h"

namespace sqr
{
	extern const wstring PATH_ALIAS_CFG;
}

CShockWaveMagicCfgClient::MapShockWaveMagicCfgClientById CShockWaveMagicCfgClient::m_mapCfgById;

bool CShockWaveMagicCfgClient::LoadConfig(const TCHAR* cfgFile)
{
	CTxtTableFile TabFile;
	CShockWaveMagicCfgClient*	pCfgNode;

	if (!TabFile.Load(PATH_ALIAS_CFG.c_str(), cfgFile)) return false;

	ClearMap(m_mapCfgById);

	for(int32 i=1; i<TabFile.GetHeight(); i++)
	{
		pCfgNode = new CShockWaveMagicCfgClient;
		pCfgNode->m_uId = i;
		pCfgNode->m_sFX = TabFile.GetString(i, szMagic_FxName);
		trimend(pCfgNode->m_sFX);

		CShockWaveMagicCfgClientSharedPtr* pCfgNodeSharedPtr = new CShockWaveMagicCfgClientSharedPtr(pCfgNode);
		m_mapCfgById.insert(make_pair(pCfgNode->m_uId, pCfgNodeSharedPtr));
	}
	return true;
}

void CShockWaveMagicCfgClient::UnloadConfig()
{
	ClearMap(m_mapCfgById);
}

CShockWaveMagicCfgClientSharedPtr& CShockWaveMagicCfgClient::GetById(uint32 uId)
{
	stringstream ExpStr;
	CShockWaveMagicCfgClient::MapShockWaveMagicCfgClientById::iterator mapCfgItr;
	mapCfgItr = m_mapCfgById.find(uId);
	if (mapCfgItr == m_mapCfgById.end())
	{
		ExpStr << "³å»÷²¨Ä§·¨" << uId << "²»´æÔÚ";
		GenErr(ExpStr.str());
		//return NULL;
	}
	return *(mapCfgItr->second);
}

CShockWaveMagicClient::CShockWaveMagicClient(CCoreObjectFollower* pCoreObjFol)
:CMagicClient(pCoreObjFol)
{
	AdjustDirection();
}

void CShockWaveMagicClient::AdjustDirection()
{
	CFPos posSrc, posDest;

	if(m_pCoreObj->GetDestOfCurPath(posDest))
	{
		CDir dir;

		posSrc = m_pCoreObj->GetPixelPos();

		CVector2f vecDir(posDest.x - posSrc.x, posDest.y - posSrc.y);
		dir.Set(vecDir);

		CRenderObject* pRenderObj = GetRenderObject();
		if( pRenderObj )
			pRenderObj->SetDirection(dir);
	}
}