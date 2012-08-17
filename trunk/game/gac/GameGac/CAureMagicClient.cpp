#include "stdafx.h"
#include "CAureMagicClient.h"
#include "CTxtTableFile.h"
#include "LoadSkillCommon.h"
#include "BaseHelper.h"
#include "StringHelper.h"

namespace sqr
{
	extern const wstring PATH_ALIAS_CFG;
}

CAureMagicCfgClient::MapAureMagicCfgClientByID		CAureMagicCfgClient::ms_mapAureMagicClientByID;
CAureMagicCfgClient::MapAureMagicCfgClientByName	CAureMagicCfgClient::ms_mapAureMagicClientByName;
CAureMagicCfgClient::MapStanceType					CAureMagicCfgClient::ms_mapStanceType;
CAureMagicCfgClientSharedPtr CAureMagicCfgClient::ms_NULL;

bool CAureMagicCfgClient::LoadAureMagicConfig(const string& szFileName)
{
	InitMapStanceType()	;

	CTxtTableFile TabFile;
	if (!TabFile.Load(PATH_ALIAS_CFG.c_str(), szFileName.c_str())) return false;
	for( int i = 1; i < TabFile.GetHeight(); ++i )
	{	
		CAureMagicCfgClient* pAureMagicCfgClient = new CAureMagicCfgClient;
		pAureMagicCfgClient->m_uID		= i;
		pAureMagicCfgClient->m_strName	= TabFile.GetString(i, szMagic_Name	);
		trimend(pAureMagicCfgClient->m_strName);
		string sType = TabFile.GetString(i, szMagic_StanceType);
		trimend(sType);
		pAureMagicCfgClient->m_eType	= ms_mapStanceType[sType];

		CAureMagicCfgClientSharedPtr* pAureMagicCfgClientSharedPtr = new CAureMagicCfgClientSharedPtr(pAureMagicCfgClient);
		ms_mapAureMagicClientByID.insert(make_pair(pAureMagicCfgClient->m_uID, pAureMagicCfgClientSharedPtr));
		ms_mapAureMagicClientByName.insert(make_pair(pAureMagicCfgClient->m_strName, pAureMagicCfgClientSharedPtr));
	}
	return true;
}

void CAureMagicCfgClient::UnloadAureMagicConfig()
{
	ClearMap(ms_mapAureMagicClientByID);
}

CAureMagicCfgClientSharedPtr& CAureMagicCfgClient::GetAureMagicByID(uint32 uID)
{
	MapAureMagicCfgClientByID::iterator it_AureMagicCfg = ms_mapAureMagicClientByID.find(uID);
	if (it_AureMagicCfg == ms_mapAureMagicClientByID.end())
	{
		stringstream ExpStr;
		ExpStr << "姿态" << uID << "不存在";
		GenErr(ExpStr.str());
		return ms_NULL;
	}
	return *(it_AureMagicCfg->second);
}

CAureMagicCfgClientSharedPtr& CAureMagicCfgClient::GetAureMagicByName(const string& szName)
{
	MapAureMagicCfgClientByName::iterator it_AureMagicCfg = ms_mapAureMagicClientByName.find(szName);
	if(it_AureMagicCfg == ms_mapAureMagicClientByName.end())
	{
		stringstream ExpStr;
		ExpStr << "姿态" << szName << "不存在";
		GenErr(ExpStr.str());
		return ms_NULL;
	}
	return  *(it_AureMagicCfg->second);
}

