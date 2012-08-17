#include "stdafx.h"
#include "CCastingProcessCfg.h"
#include "CTxtTableFile.h"
#include "LoadSkillCommon.h"
#include "CCfgCalc.inl"
#include "CCfgColChecker.inl"
#include "BaseHelper.h"
#include "TSqrAllocator.inl"
#include "TStringRef.inl"
#include "TUpdateCfgFunctions.h"
#include "CSkillCfg.h"

namespace sqr
{
	extern const wstring PATH_ALIAS_CFG;
}

CSkillCfg::MapCastingType CSkillCfg::ms_mapCastingType;
CSkillCfg::MapCastingInterruptType CSkillCfg::ms_mapCastingInterruptType;

CCastingProcessCfgServerSharedPtr CCastingProcessCfgServer::ms_NULL;

bool CCastingProcessCfgServer::LoadConfig(const TCHAR* cfgFile, bool bReload)
{
	using namespace CfgChk;
	CSkillCfg::InitMapCastingType();
	CSkillCfg::InitMapCastingInterruptType();
	CTxtTableFile TabFile;
	stringstream ExpStr;				//用于异常输出
	SetTabFile(TabFile, "流程");
	if (!TabFile.Load(PATH_ALIAS_CFG.c_str(), cfgFile)) return false;
	if (!bReload)
	{
		ClearMap(GetStringKeyMap());
	}
	for (int32 i=1;i<TabFile.GetHeight();++i)
	{
		SetLineNo(i);
		CCastingProcessCfgServer*	pCfgNode = new CCastingProcessCfgServer;
		CCastingProcessCfgServerSharedPtr*	pCfgNodeSharedPtr = new CCastingProcessCfgServerSharedPtr(pCfgNode);

		pCfgNode->m_uId = i;

		ReadItem(pCfgNode->m_sName,					szCasting_Name);
		ReadItem(pCfgNode->m_eCastingType,			szCasting_Type,			CSkillCfg::ms_mapCastingType);
		ReadItem(pCfgNode->m_eMoveInterrupt,		szCasting_InterruptType, CSkillCfg::ms_mapCastingInterruptType);
		ReadItem(pCfgNode->m_pCastingTime,			szCastingTime);
		ReadItem(pCfgNode->m_pInterruptPercent,		szInterPercent);
		pCfgNode->m_fChannelInterval	= 1.0f;

		CCfgCalc* magicEff;
		ReadMixedItem(magicEff,		szMagicEff,	CANEMPTY,	"");

		if(magicEff->GetStringCount() != 0)
		{
			SetItemTitle(szMagicEff);
			if (pCfgNode->m_eCastingType == eCPT_GradeSing || pCfgNode->m_eCastingType == eCPT_Sing ||
				pCfgNode->m_eCastingType == eCPT_ImmovableSing )
			{
				for (uint32 i =0; i<magicEff->GetStringCount(); i++)
				{
					pCfgNode->m_vecMagicEff.push_back(new MagicEffServerStringRefer(pCfgNode, magicEff->GetString(i)));
				}
			}
			else
			{
				pCfgNode->m_vecMagicEff.push_back(new MagicEffServerStringRefer(pCfgNode, magicEff->GetString(0)));
				if (magicEff->GetStringCount() == 2)
					SetValue(pCfgNode->m_fChannelInterval, magicEff->GetString(1));
				//CheckUIntType(pCfgNode->m_fChannelInterval);
			}
		}
		SafeDelete(magicEff);
		ReadItem(pCfgNode, *(pCfgNode->m_pSelfCancelableMagicEff),	szSelfCancelableMagicEff,	CANEMPTY);
		ReadItem(pCfgNode, *(pCfgNode->m_pObjCancelableMagicEff),	szObjCancelableMagicEff,	CANEMPTY);
		ReadItem(pCfgNode, *(pCfgNode->m_pFinalMagicEff),		szFinalMagicEff,		CANEMPTY);

		if (bReload)
		{
			CCastingProcessCfgServerSharedPtr* pCfgNodeSharedPtrBak = NULL; 
			MapCastingProcessCfg::iterator it = GetStringKeyMap().find(szCasting_Name);
			if(it != GetStringKeyMap().end())
			{
				pCfgNodeSharedPtrBak = it->second;
				GetStringKeyMap().erase(it);
			}
			InsertStringSharedPtrMap(GetStringKeyMap(), pCfgNodeSharedPtr);
			if (pCfgNodeSharedPtrBak)
			{
				pCfgNodeSharedPtrBak->get()->NotifyRefer();
				delete pCfgNodeSharedPtrBak;
			}
		}
		else if(!InsertStringSharedPtrMap(GetStringKeyMap(), pCfgNodeSharedPtr))
		{
			SafeDelete(pCfgNodeSharedPtr);
		}
	}
	return true;
}

void CCastingProcessCfgServer::UnloadConfig()
{
	ClearMap(GetStringKeyMap());
}


CCastingProcessCfgServer::CCastingProcessCfgServer()
{
	m_pSelfCancelableMagicEff = new MagicEffServerStringRefer(this);
	m_pObjCancelableMagicEff = new MagicEffServerStringRefer(this);
	m_pFinalMagicEff = new MagicEffServerStringRefer(this);
}

CCastingProcessCfgServer::CCastingProcessCfgServer(const CCastingProcessCfgServer& cfg)
:m_uId(cfg.m_uId)
,m_sName(cfg.m_sName)
,m_eCastingType(cfg.m_eCastingType)
,m_eMoveInterrupt(cfg.m_eMoveInterrupt)
,m_pCastingTime(new CCfgCalc(*(cfg.m_pCastingTime)))
,m_fChannelInterval(cfg.m_fChannelInterval)
,m_pInterruptPercent(new CCfgCalc(*(cfg.m_pInterruptPercent)))
{
	string str;
	m_pSelfCancelableMagicEff = new MagicEffServerStringRefer(this);
	cfg.m_pSelfCancelableMagicEff->GetStringKey(str);
	m_pSelfCancelableMagicEff->SetStringKey(str);
	m_pObjCancelableMagicEff = new MagicEffServerStringRefer(this);
	cfg.m_pObjCancelableMagicEff->GetStringKey(str);
	m_pObjCancelableMagicEff->SetStringKey(str);
	m_pFinalMagicEff = new MagicEffServerStringRefer(this);
	cfg.m_pFinalMagicEff->GetStringKey(str);
	m_pFinalMagicEff->SetStringKey(str);
	VecMagicEff::const_iterator it = cfg.m_vecMagicEff.begin();
	for(; it != cfg.m_vecMagicEff.end(); ++it)
	{
		MagicEffServerStringRefer* pMagicEff = new MagicEffServerStringRefer(this);
		(*it)->GetStringKey(str);
		pMagicEff->SetStringKey(str);
		m_vecMagicEff.push_back(pMagicEff);
	}
}

CCastingProcessCfgServer::~CCastingProcessCfgServer()
{
	ClearVector(m_vecMagicEff);
	SafeDelete(m_pCastingTime);
	SafeDelete(m_pInterruptPercent);
	SafeDelete(m_pSelfCancelableMagicEff);
	SafeDelete(m_pObjCancelableMagicEff);
	SafeDelete(m_pFinalMagicEff);
}

void CCastingProcessCfgServer::UpdateCfg(const string& strName)
{
	UpdateCfg::UpdateCfgAndNotifyStringRefer<MapCastingProcessCfg, CCastingProcessCfgServer, CCastingProcessCfgServerSharedPtr>(GetStringKeyMap(), strName);
}

CCastingProcessCfgServer::MapCastingProcessCfg& CCastingProcessCfgServer::GetStringKeyMap()
{
	return ParentType::GetImpClassMap();
}

CCastingProcessCfgServerSharedPtr& CCastingProcessCfgServer::Get(const string &castingProcessName)
{
	CCastingProcessCfgServer::MapCastingProcessCfg::iterator it_mapCfg;
	it_mapCfg = GetStringKeyMap().find(castingProcessName);
	if(it_mapCfg == GetStringKeyMap().end())
	{
		return ms_NULL;
	}
	return *(it_mapCfg->second);
}

const CMagicEffServerSharedPtr& CCastingProcessCfgServer::GetMagicEff(uint32 uIndex)const
{
	if (m_vecMagicEff.size() >= uIndex)
		return m_vecMagicEff.at(uIndex-1)->Get();
	else
		return m_vecMagicEff.back()->Get();
}

const CMagicEffServerSharedPtr& CCastingProcessCfgServer::GetSelfCancelableMagicEff()const
{
	return m_pSelfCancelableMagicEff->Get();
}

const CMagicEffServerSharedPtr& CCastingProcessCfgServer::GetObjCancelableMagicEff()const
{
	return m_pObjCancelableMagicEff->Get();
}

const CMagicEffServerSharedPtr& CCastingProcessCfgServer::GetFinalMagicEff()const
{
	return m_pFinalMagicEff->Get();
}

bool CCastingProcessCfgServer::GetMoveInterrupt()
{
	return m_eMoveInterrupt == eCIT_Move || m_eMoveInterrupt == eCIT_TurnAround;
}

