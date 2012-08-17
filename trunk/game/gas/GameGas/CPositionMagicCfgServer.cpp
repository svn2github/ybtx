#include "stdafx.h"
#include "CPositionMagicCfgServer.h"
#include "CTxtTableFile.h"
#include "LoadSkillCommon.h"
#include "CMagicEffServer.h"
#include "CCfgColChecker.inl"
#include "BaseHelper.h"
#include "TStringRef.inl"
#include "TUpdateCfgFunctions.h"
#include "CSkillCfg.h"

namespace sqr
{
	extern const wstring PATH_ALIAS_CFG;
}


CPositionMagicCfgServer::MapPositionMagicCfgServer  CPositionMagicCfgServer::ms_mapPositionMagic;
CPositionMagicCfgServerSharedPtr CPositionMagicCfgServer::ms_NULL;

bool CPositionMagicCfgServer::LoadPositionMagicConfig(const string&	szFileName)
{
	using namespace CfgChk;
	CTxtTableFile TabFile;
	SetTabFile(TabFile, "位置魔法");
	if (!TabFile.Load(PATH_ALIAS_CFG.c_str(), szFileName.c_str())) return false;

	ClearMap(ms_mapPositionMagic);
	for( int32 i = 1; i < TabFile.GetHeight(); ++i )
	{
		SetLineNo(i);
		CPositionMagicCfgServer* pPositionMagic = new CPositionMagicCfgServer;
		pPositionMagic->m_uId					= i;

		ReadItem(pPositionMagic->m_sName,				szMagic_Name);
		ReadItem(pPositionMagic->m_pTime,				szMagic_Time,			GE,	-1);
		ReadItem(pPositionMagic->m_pAmountLimit,				szMagic_AmountLimit,			GE,	-1);
		ReadItem(pPositionMagic->m_pRadius,				szMagic_Radius,			GE,	0);
		ReadItem(pPositionMagic->m_bMutexType,			szMagic_MutexType,		CANEMPTY);
		pPositionMagic->m_bSingleObject = false;
		string tempstring;
		ReadItem(tempstring,							szMagic_OperateObject);
		if(!tempstring.empty())
		{
			size_t comma = tempstring.find(',');
			if(comma != string::npos)
			{
				string sValue = tempstring.substr(0, comma);
				trimend(sValue);
				CheckEmpty(sValue, sValue, false, sValue);
				CheckField(sValue, CSkillCfg::ms_mapOperateObjectType);
				SetValue(pPositionMagic->m_eOperateObject, sValue, CSkillCfg::ms_mapOperateObjectType);
				sValue = tempstring.substr(comma + 1);
				trimend(sValue);
				if(sValue == "单体")
				{
					pPositionMagic->m_bSingleObject = true;
				}
			}
			else
			{
				ReadItem(pPositionMagic->m_eOperateObject,		szMagic_OperateObject,	CSkillCfg::ms_mapOperateObjectType);
			}
		}
		ReadItem(pPositionMagic, *(pPositionMagic->m_pTouchMagicEff),		szMagic_TouchMagicEff,	CANEMPTY);

		pPositionMagic->m_iDotInterval = 1;
		ReadItem(tempstring,							szMagic_DotMagicEff,		CANEMPTY);
		if(tempstring.empty())
		{
			//pPositionMagic->m_pDotMagicEff = NULL;
		}
		else
		{
			size_t comma = tempstring.find(',');
			SetItemTitle(szMagicState_DotMagicEff);
			if(comma != string::npos)
			{
				pPositionMagic->m_pDotMagicEff->SetStringKey(tempstring.substr(0, comma));
				SetValue(pPositionMagic->m_iDotInterval, tempstring.substr(comma + 1));
				CheckUIntType(pPositionMagic->m_iDotInterval);
			}
			else
			{
				pPositionMagic->m_pDotMagicEff->SetStringKey(tempstring);
			}
		}

		ReadItem(pPositionMagic, *(pPositionMagic->m_pFinalMagicEff),		szMagic_FinalMagicEff,	CANEMPTY);

		CPositionMagicCfgServerSharedPtr* pPositionMagicSharedPtr = new CPositionMagicCfgServerSharedPtr(pPositionMagic);
		if(!InsertStringSharedPtrMap(ms_mapPositionMagic, pPositionMagicSharedPtr))
		{
			SafeDelete(pPositionMagicSharedPtr);
			continue;
		}
	}
	return true;
}

void CPositionMagicCfgServer::UnloadPositionMagicConfig()
{
	ClearMap(ms_mapPositionMagic);
}

void CPositionMagicCfgServer::UpdateCfg(const string& strName)
{	
	UpdateCfg::UpdateCfg<MapPositionMagicCfgServer, CPositionMagicCfgServer, CPositionMagicCfgServerSharedPtr>(ms_mapPositionMagic, strName);
}

CPositionMagicCfgServer::CPositionMagicCfgServer()
{
	m_pTouchMagicEff = new MagicEffServerStringRefer(this);
	m_pDotMagicEff = new MagicEffServerStringRefer(this);
	m_pFinalMagicEff = new MagicEffServerStringRefer(this);
}

CPositionMagicCfgServer::CPositionMagicCfgServer(const CPositionMagicCfgServer& cfg)
:CMagicCfgServer(cfg)
,m_bMutexType(cfg.m_bMutexType)
,m_bSingleObject(cfg.m_bSingleObject)
,m_iDotInterval(cfg.m_iDotInterval)
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

CPositionMagicCfgServer::~CPositionMagicCfgServer()
{
	SafeDelete(m_pTouchMagicEff);
	SafeDelete(m_pDotMagicEff);
	SafeDelete(m_pFinalMagicEff);
}

CPositionMagicCfgServerSharedPtr& CPositionMagicCfgServer::GetPositionMagic(const string& szName)
{
	MapPositionMagicCfgServer::iterator it_PositionMagic = ms_mapPositionMagic.find(szName);
	if(it_PositionMagic == ms_mapPositionMagic.end())
	{
		stringstream str; 
		str << "没有这个位置魔法";
		CfgChk::GenExpInfo(str.str(), szName);
		return ms_NULL;
	}
	return *(it_PositionMagic->second);
}

const CMagicEffServerSharedPtr& CPositionMagicCfgServer::GetTouchMagicEff()const
{
	return m_pTouchMagicEff->Get();
}

const CMagicEffServerSharedPtr& CPositionMagicCfgServer::GetDotMagicEff()const
{
	return m_pDotMagicEff->Get();
}

const CMagicEffServerSharedPtr& CPositionMagicCfgServer::GetFinalMagicEff()const
{
	return m_pFinalMagicEff->Get();
}
