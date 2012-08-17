#include "stdafx.h"
#include "CTransferableMagicCfgServer.h"
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

CTransferableMagicCfgServer::MapTransferableMagicCfgServer CTransferableMagicCfgServer::ms_mapTransferableMagic;
CTransferableMagicCfgServer::MapTransType CTransferableMagicCfgServer::ms_mapTransType;
CTransferableMagicCfgServerSharedPtr CTransferableMagicCfgServer::ms_NULL;

bool CTransferableMagicCfgServer::LoadTransferableMagicConfig(const string&	szFileName)
{
	using namespace CfgChk;

	InitMapTransType();

	CTxtTableFile TabFile;
	SetTabFile(TabFile, "传递魔法");
	if (!TabFile.Load(PATH_ALIAS_CFG.c_str(), szFileName.c_str())) 
		return false;

	ClearMap(ms_mapTransferableMagic);
	for( int32 i = 1; i < TabFile.GetHeight(); ++i )
	{
		SetLineNo(i);
		CTransferableMagicCfgServer* pTransferableMagic		= new CTransferableMagicCfgServer;
		pTransferableMagic->m_uId							= i;
		ReadItem(pTransferableMagic->m_sName,				szMagic_Name);
		ReadItem(pTransferableMagic->m_fSpeed,				szMagic_Speed,			GT,	0.0F);
		ReadItem(pTransferableMagic->m_pAmountLimit,		szMagic_AmountLimit,	GT,	0);
		ReadItem(pTransferableMagic->m_pRadius,				szMagic_Radius,			GT,	0);
		ReadItem(pTransferableMagic->m_eTransType,			szMagic_TransType,		ms_mapTransType);
		ReadItem(pTransferableMagic->m_eOperateObject,		szMagic_OperateObject,	CSkillCfg::ms_mapOperateObjectType);
		ReadItem(pTransferableMagic, *(pTransferableMagic->m_pMainMagicEff),		szMagic_MainMagicEff);
		ReadItem(pTransferableMagic, *(pTransferableMagic->m_pSecondMagicEff),		szMagic_SecondMagicEff);

		CTransferableMagicCfgServerSharedPtr* pTransferableMagicSharedPtr		= new CTransferableMagicCfgServerSharedPtr(pTransferableMagic);
		if(!InsertStringSharedPtrMap(ms_mapTransferableMagic, pTransferableMagicSharedPtr))
		{
			SafeDelete(pTransferableMagicSharedPtr);
			continue;
		}
	}
	return true;
}

void CTransferableMagicCfgServer::UnloadTransferableMagicConfig()
{
	ClearMap(ms_mapTransferableMagic);
}

void CTransferableMagicCfgServer::UpdateCfg(const string& strName)
{
	UpdateCfg::UpdateCfg<MapTransferableMagicCfgServer, CTransferableMagicCfgServer, CTransferableMagicCfgServerSharedPtr>(ms_mapTransferableMagic, strName);
}

CTransferableMagicCfgServer::CTransferableMagicCfgServer()
{
	m_pMainMagicEff = new MagicEffServerStringRefer(this);
	m_pSecondMagicEff = new MagicEffServerStringRefer(this);
}

CTransferableMagicCfgServer::CTransferableMagicCfgServer(const CTransferableMagicCfgServer& cfg)
:CMagicCfgServer(cfg)
,m_eTransType(cfg.m_eTransType)
{
	string str;
	m_pMainMagicEff = new MagicEffServerStringRefer(this);
	cfg.m_pMainMagicEff->GetStringKey(str);
	m_pMainMagicEff->SetStringKey(str);
	m_pSecondMagicEff = new MagicEffServerStringRefer(this);
	cfg.m_pSecondMagicEff->GetStringKey(str);
	m_pSecondMagicEff->SetStringKey(str);
}

CTransferableMagicCfgServer::~CTransferableMagicCfgServer()
{
	SafeDelete(m_pAmountLimit);
	SafeDelete(m_pMainMagicEff);
	SafeDelete(m_pSecondMagicEff);
}

CTransferableMagicCfgServerSharedPtr& CTransferableMagicCfgServer::GetTransferableMagic(const string& szName)
{
	MapTransferableMagicCfgServer::iterator it_TransferableMagic = ms_mapTransferableMagic.find(szName);
	if(it_TransferableMagic == ms_mapTransferableMagic.end())
	{
		stringstream str; 
		str << "没有这个传递魔法";
		CfgChk::GenExpInfo(str.str(), szName);
		return ms_NULL;
	}
	return  *(it_TransferableMagic->second);
}


const CMagicEffServerSharedPtr& CTransferableMagicCfgServer::GetMainMagicEff()const
{
	return	m_pMainMagicEff->Get();
}

const CMagicEffServerSharedPtr& CTransferableMagicCfgServer::GetSecondMagicEff()const
{
	return	m_pSecondMagicEff->Get();
}
