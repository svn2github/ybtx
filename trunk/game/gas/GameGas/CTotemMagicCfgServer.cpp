#include "stdafx.h"
#include "CTotemMagicCfgServer.h"
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


CTotemMagicCfgServer::MapTotemMagicCfgServer  CTotemMagicCfgServer::ms_mapTotemMagic;
CTotemMagicCfgServerSharedPtr CTotemMagicCfgServer::ms_NULL;

bool CTotemMagicCfgServer::LoadTotemMagicConfig(const string&	szFileName)
{
	using namespace CfgChk;

	CTxtTableFile TabFile;
	SetTabFile(TabFile, "图腾魔法");
	if (!TabFile.Load(PATH_ALIAS_CFG.c_str(), szFileName.c_str())) return false;

	ClearMap(ms_mapTotemMagic);
	for( int32 i = 1; i < TabFile.GetHeight(); ++i )
	{
		SetLineNo(i);
		CTotemMagicCfgServer* pTotemMagic = new CTotemMagicCfgServer;
		pTotemMagic->m_uId					= i;
		ReadItem(pTotemMagic->m_sName,			szMagic_Name);
		ReadItem(pTotemMagic->m_pRadius,		szMagic_Radius,			GT,	0.0f);
		ReadItem(pTotemMagic->m_pTime,			szMagic_Time,			GE,	-1);
		ReadItem(pTotemMagic->m_eOperateObject,	szMagic_OperateObject,	CSkillCfg::ms_mapOperateObjectType);
		ReadItem(pTotemMagic, *(pTotemMagic->m_pTouchMagicEff),	szMagic_TouchMagicEff,	CANEMPTY);
		ReadItem(pTotemMagic, *(pTotemMagic->m_pDotMagicEff),	szMagic_DotMagicEff,	CANEMPTY);
		ReadItem(pTotemMagic, *(pTotemMagic->m_pFinalMagicEff),	szMagic_FinalMagicEff,	CANEMPTY);

		CTotemMagicCfgServerSharedPtr* pTotemMagicSharedPtr = new CTotemMagicCfgServerSharedPtr(pTotemMagic);
		if(!InsertStringSharedPtrMap(ms_mapTotemMagic, pTotemMagicSharedPtr))
		{
			SafeDelete(pTotemMagicSharedPtr);
			continue;
		}
	}
	return true;
}

void CTotemMagicCfgServer::UnloadTotemMagicConfig()
{
	ClearMap(ms_mapTotemMagic);
}

void CTotemMagicCfgServer::UpdateCfg(const string& strName)
{
	UpdateCfg::UpdateCfg<MapTotemMagicCfgServer, CTotemMagicCfgServer, CTotemMagicCfgServerSharedPtr>(ms_mapTotemMagic, strName);
}

CTotemMagicCfgServer::CTotemMagicCfgServer()
{
	m_pTouchMagicEff = new MagicEffServerStringRefer(this);
	m_pDotMagicEff = new MagicEffServerStringRefer(this);
	m_pFinalMagicEff = new MagicEffServerStringRefer(this);
}

CTotemMagicCfgServer::CTotemMagicCfgServer(const CTotemMagicCfgServer& cfg)
:CMagicCfgServer(cfg)
,m_eOperateObject(cfg.m_eOperateObject)
{
	string str;
	m_pTouchMagicEff = new MagicEffServerStringRefer(this);
	cfg.m_pTouchMagicEff->GetStringKey(str);
	m_pTouchMagicEff->SetStringKey(str);
	m_pDotMagicEff = new MagicEffServerStringRefer(this);
	cfg.m_pDotMagicEff->GetStringKey(str);
	m_pDotMagicEff->SetStringKey(str);
	m_pFinalMagicEff = new MagicEffServerStringRefer(this);
	cfg.m_pFinalMagicEff->GetStringKey(str);
	m_pFinalMagicEff->SetStringKey(str);
}

CTotemMagicCfgServer::~CTotemMagicCfgServer()
{
	SafeDelete(m_pTouchMagicEff);
	SafeDelete(m_pDotMagicEff);
	SafeDelete(m_pFinalMagicEff);
}

CTotemMagicCfgServerSharedPtr& CTotemMagicCfgServer::GetTotemMagic(const string& szName)
{
	MapTotemMagicCfgServer::iterator it_TotemMagic = ms_mapTotemMagic.find(szName);
	if(it_TotemMagic == ms_mapTotemMagic.end())
	{
		stringstream str; 
		str << szName;
		GenErr("没有这个图腾魔法", str.str().c_str());
	}
	return *(it_TotemMagic->second);
}

const CMagicEffServerSharedPtr& CTotemMagicCfgServer::GetTouchMagicEff()const
{
	return m_pTouchMagicEff->Get();
}

const CMagicEffServerSharedPtr& CTotemMagicCfgServer::GetDotMagicEff()const
{
	return m_pDotMagicEff->Get();
}

const CMagicEffServerSharedPtr& CTotemMagicCfgServer::GetFinalMagicEff()const
{
	return m_pFinalMagicEff->Get();
}
