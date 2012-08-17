#include "stdafx.h"
#include "CBurstSoulCfgServer.h"
#include "CTxtTableFile.h"
#include "LoadSkillCommon.h"
#include "CCfgCalc.inl"
#include "CCfgColChecker.inl"
#include "BaseHelper.h"
#include "TSqrAllocator.inl"
#include "TStringRef.inl"
#include "TUpdateCfgFunctions.h"

namespace sqr
{
	extern const wstring PATH_ALIAS_CFG;
}

CBurstSoulCfgServer::MapBurstSoulCfg CBurstSoulCfgServer::ms_mapBurstSoulCfg;
CBurstSoulCfgServer::MapBurstSoulType CBurstSoulCfgServer::ms_mapBurstSoulType;

bool CBurstSoulCfgServer::LoadConfig(const TCHAR* cfgFile)
{
	using namespace CfgChk;
	CTxtTableFile TabFile;
	InitMapBurstSoulType();

	stringstream ExpStr;				//用于异常输出
	SetTabFile(TabFile, "爆魂");
	if (!TabFile.Load(PATH_ALIAS_CFG.c_str(), cfgFile)) return false;
	ClearMap(ms_mapBurstSoulCfg);
	for (int32 i=1;i<TabFile.GetHeight();++i)
	{
		SetLineNo(i);
		CBurstSoulCfgServer* pCfgNode = new CBurstSoulCfgServer();
		CBurstSoulCfgServerSharedPtr* pCfgNodeSharedPtr = new CBurstSoulCfgServerSharedPtr(pCfgNode);
		
		ReadItem(pCfgNode->m_sName,				szBurstSoul_DoubleBurst);
		ReadItem(pCfgNode->m_pDoubleBurst,		szBurstSoul_DoubleBurst);
		ReadItem(pCfgNode->m_eBurstSoulType,	szBurstSoul_Type,	ms_mapBurstSoulType);
		ReadItem(pCfgNode->m_pSoulCount,		szBurstSoul_Count);
		ReadItem(pCfgNode->m_sMagicEffName,		szBurstSoul_MagicEff,	CANEMPTY,	"");
		ReadItem(pCfgNode->m_sBulletMagicName,	szBurstSoul_BulletMagicName,	CANEMPTY,	"");
		ReadItem(pCfgNode->m_fProbability,		szBurstSoul_Probability,	CANEMPTY);
		string sBurstSoulTimesLevel;
		ReadItem(sBurstSoulTimesLevel,			szBurstSoul_TimesLevel,	CANEMPTY,	"");
		string::size_type pos = sBurstSoulTimesLevel.find_first_of(',');
		if(pos != string::npos)
		{
			SetValue(pCfgNode->m_pLevel, sBurstSoulTimesLevel.substr(0, pos));
			SetValue(pCfgNode->m_pNeedBurstTimes, sBurstSoulTimesLevel.substr(pos + 1));
		}
		else
		{
			pCfgNode->m_pLevel = NULL;
			pCfgNode->m_pNeedBurstTimes = NULL;
		}

		pCfgNode->m_pExtraMagicEff->SetStringKey(pCfgNode->m_sMagicEffName);
		if(!InsertStringSharedPtrMap(ms_mapBurstSoulCfg, pCfgNodeSharedPtr))
		{
			SafeDelete(pCfgNodeSharedPtr);
			continue;
		}
	}
	return true;
}

void CBurstSoulCfgServer::UnloadConfig()
{
	ClearMap(ms_mapBurstSoulCfg);
}

void CBurstSoulCfgServer::UpdateCfg(const string& strName)
{
	UpdateCfg::UpdateCfg<MapBurstSoulCfg, CBurstSoulCfgServer, CBurstSoulCfgServerSharedPtr>(ms_mapBurstSoulCfg, strName);
}

CBurstSoulCfgServer::CBurstSoulCfgServer()
{
	m_pExtraMagicEff = new MagicEffServerStringRefer(this);
}

CBurstSoulCfgServer::CBurstSoulCfgServer(const CBurstSoulCfgServer& cfg)
:m_sName(cfg.m_sName)
,m_pDoubleBurst(cfg.m_pDoubleBurst?new CCfgCalc(*(cfg.m_pDoubleBurst)):NULL)
,m_eBurstSoulType(cfg.m_eBurstSoulType)
,m_pSoulCount(cfg.m_pSoulCount?new CCfgCalc(*(cfg.m_pSoulCount)):NULL)
,m_sMagicEffName(cfg.m_sMagicEffName)
,m_sBulletMagicName(cfg.m_sBulletMagicName)
,m_fProbability(cfg.m_fProbability?new CCfgCalc(*(cfg.m_fProbability)):NULL)
,m_pLevel(cfg.m_pLevel?new CCfgCalc(*(cfg.m_pLevel)):NULL)
,m_pNeedBurstTimes(cfg.m_pNeedBurstTimes?new CCfgCalc(*(cfg.m_pNeedBurstTimes)):NULL)
{
	string str;
	m_pExtraMagicEff = new MagicEffServerStringRefer(this);
	cfg.m_pExtraMagicEff->GetStringKey(str);
	m_pExtraMagicEff->SetStringKey(str);
}

CBurstSoulCfgServer::~CBurstSoulCfgServer()
{
	SafeDelete(m_pDoubleBurst);
	SafeDelete(m_pSoulCount);
	SafeDelete(m_fProbability);
	SafeDelete(m_pLevel);
	SafeDelete(m_pNeedBurstTimes);
	SafeDelete(m_pExtraMagicEff);
}

const CMagicEffServerSharedPtr& CBurstSoulCfgServer::GetMagicEff()const
{
	return m_pExtraMagicEff->Get();
}
