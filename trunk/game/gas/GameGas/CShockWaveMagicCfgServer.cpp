#include "stdafx.h"
#include "CShockWaveMagicCfgServer.h"
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


CShockWaveMagicCfgServer::MapShockWaveMagicCfgServer  CShockWaveMagicCfgServer::ms_mapShockWaveMagic;
CShockWaveMagicCfgServer::MapBulletTrackType CShockWaveMagicCfgServer::ms_mapBulletTrackType;
CShockWaveMagicCfgServerSharedPtr CShockWaveMagicCfgServer::ms_NULL;

bool CShockWaveMagicCfgServer::LoadShockWaveMagicConfig(const string& szFileName)
{
	using namespace CfgChk;

	InitMapBulletTrackType();

	CTxtTableFile TabFile;
	SetTabFile(TabFile, "冲击波魔法");
	if (!TabFile.Load(PATH_ALIAS_CFG.c_str(), szFileName.c_str())) 
		return false;

	ClearMap(ms_mapShockWaveMagic);
	for( int32 i = 1; i < TabFile.GetHeight(); ++i )
	{
		SetLineNo(i);
		CShockWaveMagicCfgServer* pShockWaveMagic	= new CShockWaveMagicCfgServer;
		pShockWaveMagic->m_uId						= i;

		ReadItem(pShockWaveMagic->m_sName,				szMagic_Name);
		ReadItem(pShockWaveMagic->m_fSpeed,				szMagic_Speed,		GT,	0.0f);
		ReadItem(pShockWaveMagic->m_pRadius,			szMagic_Radius,		GT,	0);
		ReadItem(pShockWaveMagic->m_pRange,				szMagic_Ranges,		GT,	0);
		ReadItem(pShockWaveMagic->m_pAngle,				szMagic_Angle,		GE,	0);
		ReadItem(pShockWaveMagic->m_pAmount,			szMagic_Amount,		GT,	0);
		ReadItem(pShockWaveMagic->m_pAmountLimit,		szMagic_AmountLimit,	GE, -1);
		ReadItem(pShockWaveMagic->m_eBulletTrackType,	szMagic_TrackType,		ms_mapBulletTrackType);
		ReadItem(pShockWaveMagic->m_eOperateObject,		szMagic_OperateObject,	CSkillCfg::ms_mapOperateObjectType);
		ReadItem(pShockWaveMagic, *(pShockWaveMagic->m_pMagicEff),			szMagic_MagicEff);
		ReadItem(pShockWaveMagic->m_bDisappearWhenEnough,	szMagic_DisappearWhenEnough,	CANEMPTY,	NO);

		CShockWaveMagicCfgServerSharedPtr* pShockWaveMagicSharedPtr	= new CShockWaveMagicCfgServerSharedPtr(pShockWaveMagic);
		if(!InsertStringSharedPtrMap(ms_mapShockWaveMagic, pShockWaveMagicSharedPtr))
		{
			SafeDelete(pShockWaveMagicSharedPtr);
			continue;
		}
	}

	return true;
}

void CShockWaveMagicCfgServer::UnloadShockWaveMagicConfig()
{
	ClearMap(ms_mapShockWaveMagic);
}

void CShockWaveMagicCfgServer::UpdateCfg(const string& strName)
{
	UpdateCfg::UpdateCfg<MapShockWaveMagicCfgServer, CShockWaveMagicCfgServer, CShockWaveMagicCfgServerSharedPtr>(ms_mapShockWaveMagic, strName);
}

CShockWaveMagicCfgServer::CShockWaveMagicCfgServer()
{
	m_pMagicEff = new MagicEffServerStringRefer(this);
}

CShockWaveMagicCfgServer::CShockWaveMagicCfgServer(const CShockWaveMagicCfgServer& cfg)
:CMagicCfgServer(cfg)
,m_pRange(cfg.m_pRange?new CCfgCalc(*(cfg.m_pRange)):NULL)
,m_pAngle(cfg.m_pAngle?new CCfgCalc(*(cfg.m_pAngle)):NULL)
,m_pAmount(cfg.m_pAmount?new CCfgCalc(*(cfg.m_pAmount)):NULL)
,m_eBulletTrackType(cfg.m_eBulletTrackType)
,m_bDisappearWhenEnough(cfg.m_bDisappearWhenEnough)
{
	string str;
	m_pMagicEff = new MagicEffServerStringRefer(this);
	cfg.m_pMagicEff->GetStringKey(str);
	m_pMagicEff->SetStringKey(str);
}

CShockWaveMagicCfgServer::~CShockWaveMagicCfgServer()
{
	SafeDelete(m_pRange);
	SafeDelete(m_pAngle);
	SafeDelete(m_pAmount);
	SafeDelete(m_pMagicEff);
}

CShockWaveMagicCfgServerSharedPtr& CShockWaveMagicCfgServer::GetShockWaveMagic(const string&	szName)
{
	MapShockWaveMagicCfgServer::iterator it_ShockWaveMagic = ms_mapShockWaveMagic.find(szName);
	if(it_ShockWaveMagic == ms_mapShockWaveMagic.end())
	{
		stringstream str; 
		str << "没有这个冲击波技能";
		CfgChk::GenExpInfo(str.str(), szName);
		return ms_NULL;
	}
	return *(it_ShockWaveMagic->second);
}

const CMagicEffServerSharedPtr& CShockWaveMagicCfgServer::GetMagicEff()const
{
	return m_pMagicEff->Get();
}
