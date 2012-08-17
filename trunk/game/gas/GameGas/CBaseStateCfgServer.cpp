#include "stdafx.h"
#include "CBaseStateCfgServer.h"
#include "CBaseStateServer.h"
#include "LoadSkillCommon.h"
#include "CCfgCalc.inl"
#include "BaseHelper.h"
#include "CMagicStateCfg.h"
#include "CDamageChangeStateCfg.h"
#include "CTriggerStateCfg.h"
#include "CCumuliTriggerStateCfg.h"
#include "CSpecialStateCfg.h"
#include "CCfgColChecker.inl"

bool CBaseStateCfgServer::__init = CBaseStateCfgServer::Init();
uint32 CBaseStateCfgServer::m_uNeedSyncCount = 0;

bool CBaseStateCfgServer::Init()
{
	return true;
}

void CBaseStateCfgServer::SetNeedSync()
{
	if(m_bNeedSync != true)
	{
		m_bNeedSync = true;
		++m_uNeedSyncCount;
	}
}

void CBaseStateCfgServer::SetNeedSyncByName(TCHAR* sName)
{
	CBaseStateCfg* pBaseStateCfg = CBaseStateCfg::GetByGlobalNameNoExp(sName);
	if(!pBaseStateCfg)
		return;
	CBaseStateCfgServer* pStateCfg = 
		class_cast<CBaseStateCfgServer*>(pBaseStateCfg);
	if(pStateCfg)
	{
		pStateCfg->SetNeedSync();
	}
}

void CBaseStateCfgServer::SetNeedSaveToDB(double dTime)
{
	if ( dTime > 10 )
	{
		m_eNeedSaveToDB = eFSOFNS_ForceOfflineSave;
	}
	if (dTime >= 10*60)
	{
		m_eNeedSaveToDB = eFSOFNS_ForceOnTickSave;
	}
}



bool CBaseStateCfgServer::LoadConfig()
{
	CMagicStateCfgServer::MapMagicStateCfg::iterator it1 = CMagicStateCfgServer::m_mapCfg.begin();
	for(; it1 != CMagicStateCfgServer::m_mapCfg.end(); ++it1)
	{
		CfgChk::InsertMap(m_mapGlobalCfgByName, (CBaseStateCfg*)(it1->second->get()));
		CfgChk::InsertMap(m_mapGlobalCfgById, (CBaseStateCfg*)(it1->second->get()));
	}

	CSpecialStateCfgServer::MapSpecialStateCfg::iterator it2 = CSpecialStateCfgServer::m_mapCfg.begin();
	for(; it2 != CSpecialStateCfgServer::m_mapCfg.end(); ++it2)
	{
		CfgChk::InsertMap(m_mapGlobalCfgByName, (CBaseStateCfg*)(it2->second->get()));
		CfgChk::InsertMap(m_mapGlobalCfgById, (CBaseStateCfg*)(it2->second->get()));
	}

	CTriggerStateCfgServer::MapTriggerStateCfg::iterator it3 = CTriggerStateCfgServer::m_mapCfg.begin();
	for(; it3 != CTriggerStateCfgServer::m_mapCfg.end(); ++it3)
	{
		CfgChk::InsertMap(m_mapGlobalCfgByName, (CBaseStateCfg*)(it3->second->get()));
		CfgChk::InsertMap(m_mapGlobalCfgById, (CBaseStateCfg*)(it3->second->get()));
	}

	CDamageChangeStateCfgServer::MapDamageChangeStateCfg::iterator it4 = CDamageChangeStateCfgServer::m_mapCfg.begin();
	for(; it4 != CDamageChangeStateCfgServer::m_mapCfg.end(); ++it4)
	{
		CfgChk::InsertMap(m_mapGlobalCfgByName, (CBaseStateCfg*)(it4->second->get()));
		CfgChk::InsertMap(m_mapGlobalCfgById, (CBaseStateCfg*)(it4->second->get()));
	}

	CCumuliTriggerStateCfgServer::MapCumuliTriggerStateCfg::iterator it5 = CCumuliTriggerStateCfgServer::m_mapCfg.begin();
	for(; it5 != CCumuliTriggerStateCfgServer::m_mapCfg.end(); ++it5)
	{
		CfgChk::InsertMap(m_mapGlobalCfgByName, (CBaseStateCfg*)(it5->second->get()));
		CfgChk::InsertMap(m_mapGlobalCfgById, (CBaseStateCfg*)(it5->second->get()));
	}
	return true;
}

void CBaseStateCfgServer::UnloadConfig()
{
	m_mapGlobalCfgByName.clear();
	m_mapGlobalCfgById.clear();
}

CBaseStateCfgServer::CBaseStateCfgServer(EStateGlobalType type)
: CBaseStateCfg(type)
, m_eNeedSaveToDB(eFSOFNS_ForceNoSave)
, m_bPersistent(false)
, m_bNeedSync(false)
{
}

CBaseStateCfgServer::CBaseStateCfgServer(const CBaseStateCfgServer& cfg)
:CBaseStateCfg(cfg)
,m_uId(cfg.m_uId)
,m_sName(cfg.m_sName)
,m_bDecrease(cfg.m_bDecrease)
,m_eDecreaseType(cfg.m_eDecreaseType)
,m_bDispellable(cfg.m_bDispellable)
,m_eIconCancelCond(cfg.m_eIconCancelCond)
,m_calcTime(new CCfgCalc(*(cfg.m_calcTime)))
,m_sModel(cfg.m_sModel)
,m_eNeedSaveToDB(cfg.m_eNeedSaveToDB)
,m_bPersistent(cfg.m_bPersistent)
,m_bNeedSync(cfg.m_bNeedSync)
{
}

CBaseStateCfgServer::~CBaseStateCfgServer()
{
	SafeDelete(m_calcTime);
}

const bool CBaseStateCfgServer::GetTouchBattleState()
{
	return m_eDecreaseType >= eDST_TouchBattleStateBegin;
}
