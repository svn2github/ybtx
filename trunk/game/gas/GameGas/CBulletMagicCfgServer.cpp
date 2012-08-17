#include "stdafx.h"
#include "CBulletMagicCfgServer.h"
#include "CTxtTableFile.h"
#include "LoadSkillCommon.h"
#include "CCfgColChecker.inl"
#include "BaseHelper.h"
#include "TStringRef.inl"
#include "TUpdateCfgFunctions.h"

namespace sqr
{
	extern const wstring PATH_ALIAS_CFG;
}


CBulletMagicCfgServer::MapBulletMagicCfgServer CBulletMagicCfgServer::ms_mapBulletMagic;
CBulletMagicCfgServer::MapBulletTrackType CBulletMagicCfgServer::ms_mapBulletTrackType;
CBulletMagicCfgServerSharedPtr CBulletMagicCfgServer::ms_NULL;

bool CBulletMagicCfgServer::LoadBulletMagicConfig(const string&	szFileName)
{
	using namespace CfgChk;

	InitMapBulletTrackType();

	CTxtTableFile TabFile;
	SetTabFile(TabFile, "子弹魔法");
	if (!TabFile.Load(PATH_ALIAS_CFG.c_str(), szFileName.c_str())) 
		return false;

	ClearMap(ms_mapBulletMagic);
	for( int32 i = 1; i < TabFile.GetHeight(); ++i )
	{
		SetLineNo(i);
		CBulletMagicCfgServer* pBulletMagic = new CBulletMagicCfgServer;
		pBulletMagic->m_uId						= i;

		ReadItem(pBulletMagic->m_sName,				szMagic_Name);
		ReadItem(pBulletMagic->m_fSpeed,			szMagic_Speed);
		ReadItem(pBulletMagic->m_eBulletTrackType,	szMagic_TrackType,	ms_mapBulletTrackType);
		ReadItem(pBulletMagic, *(pBulletMagic->m_pMagicEff),			szMagic_MagicEff);

		CBulletMagicCfgServerSharedPtr* pBulletMagicSharedPtr = new CBulletMagicCfgServerSharedPtr(pBulletMagic);
		if(!InsertStringSharedPtrMap(ms_mapBulletMagic, pBulletMagicSharedPtr))
		{
			SafeDelete(pBulletMagicSharedPtr);
			continue;
		}
	}
	return true;
}

void CBulletMagicCfgServer::UnloadBulletMagicConfig()
{
	ClearMap(ms_mapBulletMagic);
}

void CBulletMagicCfgServer::UpdateCfg(const string& strName)
{
	UpdateCfg::UpdateCfg<MapBulletMagicCfgServer, CBulletMagicCfgServer, CBulletMagicCfgServerSharedPtr>(ms_mapBulletMagic, strName);
}

CBulletMagicCfgServer::CBulletMagicCfgServer()
{
	m_pMagicEff = new MagicEffServerStringRefer(this);
}

CBulletMagicCfgServer::CBulletMagicCfgServer(const CBulletMagicCfgServer& cfg)
:CMagicCfgServer(cfg)
,m_eBulletTrackType(cfg.m_eBulletTrackType)
{
	string str;
	m_pMagicEff = new MagicEffServerStringRefer(this);
	cfg.m_pMagicEff->GetStringKey(str);
	m_pMagicEff->SetStringKey(str);
}

CBulletMagicCfgServer::~CBulletMagicCfgServer()
{
	SafeDelete(m_pMagicEff);
}

CBulletMagicCfgServerSharedPtr& CBulletMagicCfgServer::GetBulletMagic(const string&	szName)
{
	MapBulletMagicCfgServer::iterator it_BulletMagic = ms_mapBulletMagic.find(szName);
	if(it_BulletMagic == ms_mapBulletMagic.end())
	{
		stringstream str; 
		str << szName;
		GenErr("没有这个子弹魔法", str.str().c_str());
	}
	return *(it_BulletMagic->second);
}

const CMagicEffServerSharedPtr& CBulletMagicCfgServer::GetMagicEff()const
{
	return m_pMagicEff->Get();
}

