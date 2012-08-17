#include "stdafx.h"
#include "CMoveMagicCfgServer.h"
#include "LoadSkillCommon.h"
#include "CCfgColChecker.inl"
#include "BaseHelper.h"
#include "TStringRef.inl"
#include "TUpdateCfgFunctions.h"
#include "CMoveMagicCfg.h"
#include "CSkillCfg.h"

namespace sqr
{
	extern const wstring PATH_ALIAS_CFG;
}

CMoveMagicCfg::MapMoveType		CMoveMagicCfg::ms_mapMoveType;
CMoveMagicCfg::MapBarrierType	CMoveMagicCfg::ms_mapBarrierType;
CMoveMagicCfg::MapMoveArg		CMoveMagicCfg::ms_mapMoveArg;
CMoveMagicCfg::MapActionType	CMoveMagicCfg::ms_mapActionType;
CMoveMagicCfg::MapMoveArgLimit	CMoveMagicCfg::ms_mapMoveArgLimit;

CMoveMagicCfgServer::MapMoveMagicCfgServer CMoveMagicCfgServer::ms_mapMoveMagic;
CMoveMagicCfgServerSharedPtr CMoveMagicCfgServer::ms_NULL;

bool CMoveMagicCfgServer::LoadMoveMagicConfig(const string&	szFileName)
{
	using namespace CfgChk;

	CMoveMagicCfg::InitMapMoveType();
	CMoveMagicCfg::InitMapBarrierType();
	CMoveMagicCfg::InitMapMoveArg();
	CMoveMagicCfg::InitMapActionType();
	CMoveMagicCfg::InitMapMoveArgLimit();

	CTxtTableFile TabFile;
	SetTabFile(TabFile, "移动魔法");
	if (!TabFile.Load(PATH_ALIAS_CFG.c_str(), szFileName.c_str()))
		return false;

	ClearMap(ms_mapMoveMagic);
	for ( int32 i = 1; i < TabFile.GetHeight(); ++i )
	{
		SetLineNo(i);
		CMoveMagicCfgServer* pMoveMagic	= new CMoveMagicCfgServer;
		CCfgCalc* pMoveArg = new CCfgCalc;
		CCfgCalc* pOperateObject = new CCfgCalc;
		string strMoveArg;
		string strOpObject;
		pMoveMagic->m_uId				= i;

		ReadItem(pMoveMagic->m_sName,			szMagic_Name);
		ReadItem(pMoveMagic->m_fSpeed,			szMagic_Speed);
		ReadItem(pMoveMagic->m_eMoveType,		szMagic_MoveType,	CMoveMagicCfg::ms_mapMoveType);
		ReadItem(pMoveMagic->m_pRadius,			szMagic_Radius,	CANEMPTY);
		ReadItem(strMoveArg,					szMagic_MoveArg);
		pMoveArg->InputString(strMoveArg);
		if (pMoveArg->GetStringCount()>1)
		{
			pMoveMagic->m_pMoveArgValue=new CCfgCalc;
			string strMoveArgValue;
			if (pMoveArg->GetStringCount()==3)
			{
				strMoveArgValue = pMoveArg->GetString(2);
			}
			else
			{
				strMoveArgValue	= pMoveArg->GetString(2)+","+pMoveArg->GetString(3);
			}

			string::size_type it = strMoveArgValue.find("(");
			if (it!=string::npos)
			{
				strMoveArgValue.erase(it,1);
			}
			it = strMoveArgValue.find(")");
			if (it!=string::npos)
			{
				strMoveArgValue.erase(it,1);
			}
			pMoveMagic->m_pMoveArgValue->InputString(strMoveArgValue);
			string strMoveArgLimit = pMoveArg->GetString(1);
			pMoveMagic->m_eMoveArgLimit= CMoveMagicCfg::ms_mapMoveArgLimit[strMoveArgLimit];
		}
		else
		{
			pMoveMagic->m_pMoveArgValue=new CCfgCalc;
			pMoveMagic->m_pMoveArgValue->InputString("");
			pMoveMagic->m_eMoveArgLimit=eMMAL_None;
		}
		string strMoveArgType = pMoveArg->GetString(0);
		pMoveMagic->m_eMoveArgType= CMoveMagicCfg::ms_mapMoveArg[strMoveArgType];

		ReadItem(strOpObject,		szMagic_OperateObject);
		pOperateObject->InputString(strOpObject);
		string strOperateObject = pOperateObject->GetString(0);
		pMoveMagic->m_eOperateObject= CSkillCfg::ms_mapOperateObjectType[strOperateObject];

		if (pOperateObject->GetStringCount()>1)
		{
			string strAreaOperateObject = pOperateObject->GetString(1);
			pMoveMagic->m_eAreaOperateObject = CSkillCfg::ms_mapOperateObjectType[strAreaOperateObject];;
		}
		else
		{
			pMoveMagic->m_eAreaOperateObject= eOOT_None;
		}

		ReadItem(pMoveMagic->m_eBarrierType, szMagic_BarrierType, CMoveMagicCfg::ms_mapBarrierType);
		ReadItem(pMoveMagic->m_eActionType,	szMagic_ActionType, CANEMPTY, eMAT_None, CMoveMagicCfg::ms_mapActionType);
		ReadItem(pMoveMagic, *(pMoveMagic->m_pMagicEff), szMagic_MagicEff,	CANEMPTY);
		ReadItem(pMoveMagic, *(pMoveMagic->m_pCancelableMagicEff), szMagic_CancelableMagicEff,	CANEMPTY);

		CMoveMagicCfgServerSharedPtr* pMoveMagicSharedPtr = new CMoveMagicCfgServerSharedPtr(pMoveMagic);
		if(!InsertStringSharedPtrMap(ms_mapMoveMagic, pMoveMagicSharedPtr))
		{
			SafeDelete(pMoveMagicSharedPtr);
			SafeDelete(pMoveArg);
			SafeDelete(pOperateObject);
			continue;
		}
		delete pMoveArg;
		delete pOperateObject;
	}
	return true;
}

void CMoveMagicCfgServer::UnloadMoveMagicConfig()
{
	ClearMap(ms_mapMoveMagic);
}

void CMoveMagicCfgServer::UpdateCfg(const string& strName)
{
	UpdateCfg::UpdateCfg<MapMoveMagicCfgServer, CMoveMagicCfgServer, CMoveMagicCfgServerSharedPtr>(ms_mapMoveMagic, strName);
}


CMoveMagicCfgServer::CMoveMagicCfgServer()
{
	m_pMagicEff = new MagicEffServerStringRefer(this);
	m_pCancelableMagicEff = new MagicEffServerStringRefer(this);
}

CMoveMagicCfgServer::CMoveMagicCfgServer(const CMoveMagicCfgServer& cfg)
:CMagicCfgServer(cfg)
,m_eMoveType(cfg.m_eMoveType)
,m_eMoveArgType(cfg.m_eMoveArgType)
,m_eMoveArgLimit(cfg.m_eMoveArgLimit)
,m_pMoveArgValue(new CCfgCalc(*(cfg.m_pMoveArgValue)))
,m_eAreaOperateObject(cfg.m_eAreaOperateObject)
,m_eBarrierType(cfg.m_eBarrierType)
,m_eActionType(cfg.m_eActionType)
{
	string str;
	m_pMagicEff = new MagicEffServerStringRefer(this);
	cfg.m_pMagicEff->GetStringKey(str);
	m_pMagicEff->SetStringKey(str);
	m_pCancelableMagicEff = new MagicEffServerStringRefer(this);
	cfg.m_pCancelableMagicEff->GetStringKey(str);
	m_pCancelableMagicEff->SetStringKey(str);
}

CMoveMagicCfgServer::~CMoveMagicCfgServer()
{
	SafeDelete(m_pMoveArgValue);
	SafeDelete(m_pMagicEff);
	SafeDelete(m_pCancelableMagicEff);
}

CMoveMagicCfgServerSharedPtr& CMoveMagicCfgServer::GetMoveMagic(const string& szName)
{
	//如果没有魔法效果则返回NULL
	if (szName.empty())
	{
		return ms_NULL;
	}

	MapMoveMagicCfgServer::iterator it_MoveMagic = ms_mapMoveMagic.find(szName);
	if (it_MoveMagic == ms_mapMoveMagic.end())
	{
		stringstream str;
		str << "没有这个位移魔法";
		CfgChk::GenExpInfo(str.str(), szName);
		return ms_NULL;
	}
	return *(it_MoveMagic->second);
}

const CMagicEffServerSharedPtr& CMoveMagicCfgServer::GetMagicEff()const
{
	return m_pMagicEff->Get();
}

const CMagicEffServerSharedPtr& CMoveMagicCfgServer::GetCancelableMagicEff()const
{
	return	m_pCancelableMagicEff->Get();
}
