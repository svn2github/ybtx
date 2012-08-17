#include "stdafx.h"
#include "LoadSkillCommon.h"
#include "CCoreObjectFollower.h"
#include "CTxtTableFile.h"
#include "CTransferableMagicClient.h"
#include "FXPlayer.h"
#include "CRenderObject.h"
#include "CCypherVariant.h"
#include "CCharacterFollower.h"
#include "StringHelper.h"
#include "CFighterFollower.h"

namespace sqr
{
	extern const wstring PATH_ALIAS_CFG;
}

CTransferableMagicCfgClient::MapTransferableMagicCfgClientById CTransferableMagicCfgClient::m_mapCfgById;

bool CTransferableMagicCfgClient::LoadConfig(const TCHAR* cfgFile)
{
	CTxtTableFile TabFile;
	CTransferableMagicCfgClient*	pCfgNode;

	if (!TabFile.Load(PATH_ALIAS_CFG.c_str(), cfgFile)) return false;

	ClearMap(m_mapCfgById);

	for(int32 i=1; i<TabFile.GetHeight(); i++)
	{
		pCfgNode = new CTransferableMagicCfgClient;
		pCfgNode->m_uId = i;
		pCfgNode->m_sFX = TabFile.GetString(i, szMagic_FxName);
		trimend(pCfgNode->m_sFX);

		CTransferableMagicCfgClientSharedPtr* pCfgNodeSharedPtr = new CTransferableMagicCfgClientSharedPtr(pCfgNode);
		m_mapCfgById.insert(make_pair(pCfgNode->m_uId, pCfgNodeSharedPtr));
	}
	return true;
}

void CTransferableMagicCfgClient::UnloadConfig()
{
	ClearMap(m_mapCfgById);
}

CTransferableMagicCfgClientSharedPtr& CTransferableMagicCfgClient::GetById(uint32 uId)
{
	stringstream ExpStr;
	CTransferableMagicCfgClient::MapTransferableMagicCfgClientById::iterator mapCfgItr;
	mapCfgItr = m_mapCfgById.find(uId);
	if (mapCfgItr == m_mapCfgById.end())
	{
		ExpStr << "传递魔法" << uId << "不存在";
		GenErr(ExpStr.str());
		//return NULL;
	}
	return *(mapCfgItr->second);
}

CTransferableMagicClient::CTransferableMagicClient(CCoreObjectFollower* pCoreObjFol, uint32 uSrcEntityID)
:CMagicClient(pCoreObjFol)
,m_uOwnerEntityID(uSrcEntityID)
{
}

CTransferableMagicClient::~CTransferableMagicClient()
{
	CCharacterFollower* pCharacter = CCharacterFollower::GetCharacterByID(m_uOwnerEntityID);
	if (pCharacter)
	{
		pCharacter->GetFighter()->GetLeadingFX()->StopFX();
	}
}

void CTransferableMagicClient::PlayMagicFx(CEntityClient* pTargetObj, const string& strMagicFx)
{
	string sFXFile;
	string sFXName;	
	uint32 uDelayTime = 0;
	CCharacterFollower* pCharacter = CCharacterFollower::GetCharacterByID(m_uOwnerEntityID);
	if (pCharacter)
	{
		pCharacter->GetActor()->SplitSkillFxFile(strMagicFx,sFXFile,sFXName,uDelayTime,false);
	}

	if (!sFXFile.empty())
	{
		CVector3f vec3f = pTargetObj->GetRenderObject()->GetCenterPosition();
		pCharacter->GetFighter()->GetLeadingFX()->PlayLoopFX(sFXFile, sFXName,vec3f,uDelayTime);
	}
}

void CTransferableMagicClient::OnValueChanged(CCypherVariant *pVariant)
{
	string strVariantName = pVariant->GetName();
	if ("TargetEntityID"==strVariantName)
	{
		CCharacterFollower* pCharacter = CCharacterFollower::GetCharacterByID(m_uOwnerEntityID);
		CCharacterFollower* pFolObj = CCharacterFollower::GetCharacterByID(pVariant->GetNumber<uint32>());
		if (pCharacter&&pFolObj&&pFolObj->GetRenderObject())
		{
			CVector3f vec3f = pFolObj->GetRenderObject()->GetCenterPosition();
			pCharacter->GetFighter()->GetLeadingFX()->AddLeadingPos(vec3f);
		}
	}
}
