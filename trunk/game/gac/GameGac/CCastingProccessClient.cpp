#include "stdafx.h"
#include "CCastingProccessClient.h"
#include "LoadSkillCommon.h"
#include "CTxtTableFile.h"
#include "CCfgCalc.h"
#include "BaseHelper.h"
#include "StringHelper.h"
#include "CSkillCfg.h"

namespace sqr
{
	extern const wstring PATH_ALIAS_CFG;
}

CSkillCfg::MapCastingType CSkillCfg::ms_mapCastingType;
CSkillCfg::MapCastingInterruptType CSkillCfg::ms_mapCastingInterruptType;

CCastingProcessCfgClient::MapCastingProcessCfgById CCastingProcessCfgClient::m_mapCfgById;
CCastingProcessCfgClient::MapCastingProcessCfgByName CCastingProcessCfgClient::m_mapCfgByName;
CCastingProcessCfgClientSharedPtr CCastingProcessCfgClient::ms_NULL;

bool CCastingProcessCfgClient::LoadConfig(const TCHAR* cfgFile)
{
	CSkillCfg::InitMapCastingType();
	CSkillCfg::InitMapCastingInterruptType();
	CTxtTableFile TabFile;
	CCastingProcessCfgClient*	pCfgNode;

	if (!TabFile.Load(PATH_ALIAS_CFG.c_str(), cfgFile)) return false;

	ClearMap(m_mapCfgById);

	for(int32 i=1; i<TabFile.GetHeight(); i++)
	{
		pCfgNode = new CCastingProcessCfgClient;
		pCfgNode->m_uId = i;
		pCfgNode->m_sName = TabFile.GetString(i,szCasting_Name);
		trimend(pCfgNode->m_sName);
		string strCastingType		= TabFile.GetString(i, szCasting_Type);
		trimend(strCastingType);
		pCfgNode->m_eCastingType	= CSkillCfg::ms_mapCastingType[strCastingType];
		pCfgNode->m_pCastingTime = new CCfgCalc();
		string sCastingTime = TabFile.GetString(i, szCastingTime);
		trimend(sCastingTime);
		pCfgNode->m_pCastingTime->InputString(sCastingTime) ;
		pCfgNode->m_sProcessAction	= TabFile.GetString(i, szProcessAction);
		trimend(pCfgNode->m_sProcessAction);
		pCfgNode->m_sProcessEffect	= TabFile.GetString(i, szProcessEffect);
		trimend(pCfgNode->m_sProcessEffect);
		//ReadItem(pCfgNode->m_bMoveInterrupt, szCasting_MoveInterrupt, CANEMPTY,	NO);
		string strMoveInterrupt = TabFile.GetString(i,szCasting_InterruptType);
		trimend(strMoveInterrupt);
		pCfgNode->m_eCastingInterruptType	= CSkillCfg::ms_mapCastingInterruptType[strMoveInterrupt];
		string strLinkEffect = TabFile.GetString(i,szLinkEffect);
		trimend(strLinkEffect);
		pCfgNode->m_bLinkEffect= strLinkEffect.compare("ÊÇ") == 0 ? true : false; 

		CCastingProcessCfgClientSharedPtr* pCfgNodeSharedPtr = new CCastingProcessCfgClientSharedPtr(pCfgNode);
		m_mapCfgById.insert(make_pair(pCfgNode->m_uId, pCfgNodeSharedPtr));
		m_mapCfgByName.insert(make_pair(pCfgNode->m_sName, pCfgNodeSharedPtr));
	}
	return true;
}

CCastingProcessCfgClient::~CCastingProcessCfgClient()
{
	SafeDelete(m_pCastingTime);
}

void CCastingProcessCfgClient::UnloadConfig()
{
	ClearMap(m_mapCfgById);
}

CCastingProcessCfgClientSharedPtr& CCastingProcessCfgClient::GetById(uint32 uId)
{
	CCastingProcessCfgClient::MapCastingProcessCfgById::iterator mapCfgItr;
	mapCfgItr = m_mapCfgById.find(uId);
	if (mapCfgItr == m_mapCfgById.end())
		return ms_NULL;

	return *(mapCfgItr->second);
}

CCastingProcessCfgClientSharedPtr& CCastingProcessCfgClient::GetByName(const string &castingProcessName)
{
	CCastingProcessCfgClient::MapCastingProcessCfgByName::iterator it_mapCfg;
	it_mapCfg = m_mapCfgByName.find(castingProcessName);
	if(it_mapCfg == m_mapCfgByName.end())
		return ms_NULL;

	return *(it_mapCfg->second);
}
