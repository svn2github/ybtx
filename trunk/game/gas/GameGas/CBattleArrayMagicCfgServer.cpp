#include "stdafx.h"
#include "CBattleArrayMagicCfgServer.h"
#include "LoadSkillCommon.h"
#include "CCfgColChecker.inl"
#include "BaseHelper.h"
#include "TStringRef.inl"
#include "TUpdateCfgFunctions.h"

namespace sqr
{
	extern const wstring PATH_ALIAS_CFG;
}


CBattleArrayMagicCfgServer::MapBattleArrayMagicCfgServer CBattleArrayMagicCfgServer::ms_mapBattleArrayMagic;
CBattleArrayMagicCfgServerSharedPtr CBattleArrayMagicCfgServer::ms_NULL;

bool CBattleArrayMagicCfgServer::LoadBattleArrayMagicConfig(const string& szFileName)
{
	using namespace CfgChk;

	CTxtTableFile TabFile;
	SetTabFile(TabFile, "Õó·¨Ä§·¨");
	if (!TabFile.Load(PATH_ALIAS_CFG.c_str(), szFileName.c_str())) 
		return false;

	ClearMap(ms_mapBattleArrayMagic);
	for( int32 i = 1; i < TabFile.GetHeight(); ++i )
	{
		SetLineNo(i);
		CBattleArrayMagicCfgServer* pBattleArrayMagic = new CBattleArrayMagicCfgServer;
		pBattleArrayMagic->m_uId							= i;

		ReadItem(pBattleArrayMagic->m_sName,				szMagic_Name);
		ReadItem(pBattleArrayMagic->m_strPosSkill[0],		szMagic_BAPosSkill_1,CANEMPTY,"");
		ReadItem(pBattleArrayMagic->m_strPosSkill[1],		szMagic_BAPosSkill_2,CANEMPTY,"");
		ReadItem(pBattleArrayMagic->m_strPosSkill[2],		szMagic_BAPosSkill_3,CANEMPTY,"");
		ReadItem(pBattleArrayMagic->m_strPosSkill[3],		szMagic_BAPosSkill_4,CANEMPTY,"");
		ReadItem(pBattleArrayMagic->m_strPosSkill[4],		szMagic_BAPosSkill_5,CANEMPTY,"");
		ReadItem(pBattleArrayMagic->m_uMaxAddUpValue,		szMagic_BattleArrayValue);

		CBattleArrayMagicCfgServerSharedPtr* pBattleArrayMagicSharedPtr = new CBattleArrayMagicCfgServerSharedPtr(pBattleArrayMagic);
		if(!InsertStringSharedPtrMap(ms_mapBattleArrayMagic, pBattleArrayMagicSharedPtr))
		{
			SafeDelete(pBattleArrayMagicSharedPtr);
			continue;
		}
	}
	return true;
}

void CBattleArrayMagicCfgServer::UnloadBattleArrayMagicConfig()
{
	ClearMap(ms_mapBattleArrayMagic);
}

void CBattleArrayMagicCfgServer::UpdateCfg(const string& strName)
{
	UpdateCfg::UpdateCfg<MapBattleArrayMagicCfgServer, CBattleArrayMagicCfgServer, CBattleArrayMagicCfgServerSharedPtr>(ms_mapBattleArrayMagic, strName);
}

CBattleArrayMagicCfgServerSharedPtr& CBattleArrayMagicCfgServer::GetBattleArrayMagic(const string& szName)
{
	MapBattleArrayMagicCfgServer::iterator it_BattleArrayMagic = ms_mapBattleArrayMagic.find(szName);
	if(it_BattleArrayMagic != ms_mapBattleArrayMagic.end())
	{
		return *(it_BattleArrayMagic->second);
	}
	return ms_NULL;
}

CBattleArrayMagicCfgServer::CBattleArrayMagicCfgServer()
:CMagicCfgServer()
{
	m_pMagicEff = new MagicEffServerStringRefer(this);
}

CBattleArrayMagicCfgServer::CBattleArrayMagicCfgServer(const CBattleArrayMagicCfgServer& cfg)
:CMagicCfgServer(cfg)
,m_uMaxAddUpValue(cfg.m_uMaxAddUpValue)
{
	m_strPosSkill[0] = cfg.m_strPosSkill[0];
	m_strPosSkill[1] = cfg.m_strPosSkill[1];
	m_strPosSkill[2] = cfg.m_strPosSkill[2];
	m_strPosSkill[3] = cfg.m_strPosSkill[3];
	m_strPosSkill[4] = cfg.m_strPosSkill[4];
	string str;
	m_pMagicEff = new MagicEffServerStringRefer(this);
	cfg.m_pMagicEff->GetStringKey(str);
	m_pMagicEff->SetStringKey(str);
}

CBattleArrayMagicCfgServer::~CBattleArrayMagicCfgServer()
{
	SafeDelete(m_pMagicEff);
}

const CMagicEffServerSharedPtr& CBattleArrayMagicCfgServer::GetMagicEff()const
{
	return m_pMagicEff->Get();
}
