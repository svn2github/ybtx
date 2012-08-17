#include "stdafx.h"
#include "CAureMagicCfgServer.h"
#include "CTxtTableFile.h"
#include "LoadSkillCommon.h"
#include "CCfgColChecker.inl"
#include "BaseHelper.h"
#include "TStringRef.inl"
#include "TUpdateCfgFunctions.h"
#include "CSkillCfg.h"

namespace sqr
{
	extern const wstring PATH_ALIAS_CFG;
}

CAureMagicCfgServer::MapAureMagicCfgServer CAureMagicCfgServer::ms_mapAureMagicServer;
CAureMagicCfgServer::MapStanceType CAureMagicCfgServer::ms_mapStanceType;
CAureMagicCfgServer::MapAgileType CAureMagicCfgServer::ms_mapAgileType;
CAureMagicCfgServerSharedPtr CAureMagicCfgServer::ms_NULL;

//template class TStringRefer<CAureMagicCfgServer, CMagicEffServer>;

bool CAureMagicCfgServer::LoadAureMagicConfig(const string&	szFileName)
{
	using namespace CfgChk;

	CSkillCfg::InitMapOperateObjectType();
	InitMapStanceType();
	InitMapAgileType();
	CTxtTableFile TabFile;
	SetTabFile(TabFile, "姿态技能");
	if (!TabFile.Load(PATH_ALIAS_CFG.c_str(), szFileName.c_str())) return false;

	for ( int32 i = 1; i < TabFile.GetHeight(); ++i )
	{
		SetLineNo(i);
		CAureMagicCfgServer* pAureMagicServer = new CAureMagicCfgServer;
		pAureMagicServer->m_uId								= i;
		ReadItem(pAureMagicServer->m_sName,					szMagic_Name);
		ReadItem(pAureMagicServer->m_eStanceType,			szMagic_StanceType,			ms_mapStanceType);
		if (pAureMagicServer->m_eStanceType == eSS_Form)
		{
			ReadItem(pAureMagicServer->m_eAgileType,		szMagic_AgileType,			ms_mapAgileType);
		}
		ReadItem(pAureMagicServer->m_pRadius,				szMagic_Radius,				CANEMPTY,	0,	GE,		0);
		ReadItem(pAureMagicServer->m_eOperateObject,		szMagic_OperateObject,		CANEMPTY,	eOOT_None, CSkillCfg::ms_mapOperateObjectType);
		ReadItem(pAureMagicServer, *(pAureMagicServer->m_pMagicEff),				szSkill_MagicEff,			CANEMPTY);
		ReadItem(pAureMagicServer->m_bNeedSaveToDB,			szAureMagic_SaveToDB,			CANEMPTY,	NO);

		CAureMagicCfgServerSharedPtr* pAureMagicServerSharedPtr = new CAureMagicCfgServerSharedPtr(pAureMagicServer);
		if(!InsertStringSharedPtrMap(ms_mapAureMagicServer, pAureMagicServerSharedPtr))
			SafeDelete(pAureMagicServerSharedPtr);
	}
	return true;
}

void CAureMagicCfgServer::UnloadAureMagicConfig()
{
	ClearMap(ms_mapAureMagicServer);
}

void CAureMagicCfgServer::UpdateCfg(const string& strName)
{
	UpdateCfg::UpdateCfg<MapAureMagicCfgServer, CAureMagicCfgServer, CAureMagicCfgServerSharedPtr>(ms_mapAureMagicServer, strName);
}

CAureMagicCfgServer::CAureMagicCfgServer()
{
	m_pMagicEff = new MagicEffServerStringRefer(this);
}

CAureMagicCfgServer::CAureMagicCfgServer(const CAureMagicCfgServer& cfg)
:CMagicCfgServer(cfg)
,m_bNeedSaveToDB(cfg.m_bNeedSaveToDB)
,m_eStanceType(cfg.m_eStanceType)
,m_eAgileType(cfg.m_eAgileType)
{
	string str;
	m_pMagicEff = new MagicEffServerStringRefer(this);
	cfg.m_pMagicEff->GetStringKey(str);
	m_pMagicEff->SetStringKey(str);
}

CAureMagicCfgServer::~CAureMagicCfgServer()
{
	SafeDelete(m_pMagicEff);
}

CAureMagicCfgServerSharedPtr& CAureMagicCfgServer::GetAureMagic(const string& szName)
{
	MapAureMagicCfgServer::iterator it_AureMagic = ms_mapAureMagicServer.find(szName);
	if (it_AureMagic == ms_mapAureMagicServer.end())
	{
		stringstream str;
		str << "没有此姿态：";
		CfgChk::GenExpInfo(str.str(), szName);
		return ms_NULL;
	}
	return  *(it_AureMagic->second);
}

const CMagicEffServerSharedPtr& CAureMagicCfgServer::GetMagicEff()const
{
	return m_pMagicEff->Get();
}
