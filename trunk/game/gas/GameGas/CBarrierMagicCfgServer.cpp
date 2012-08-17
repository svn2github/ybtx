#include "stdafx.h"
#include "CBarrierMagicCfgServer.h"
#include "CTxtTableFile.h"
#include "CCfgColChecker.inl"
#include "LoadSkillCommon.h"
#include "BaseHelper.h"
#include "TSqrAllocator.inl"
#include "TStringRef.inl"
#include "TUpdateCfgFunctions.h"

namespace sqr
{
	extern const wstring PATH_ALIAS_CFG;
}


CBarrierMagicCfgServer::MapBarrierMagicCfgServer	CBarrierMagicCfgServer::ms_mapBarrierMagic;
CBarrierMagicCfgServer::MapBarrierType			CBarrierMagicCfgServer::ms_mapBarrierType;
CBarrierMagicCfgServer::MapBarrierMagicType		CBarrierMagicCfgServer::ms_mapBarrierMagicType;
CBarrierMagicCfgServerSharedPtr CBarrierMagicCfgServer::ms_NULL;

bool CBarrierMagicCfgServer::LoadBarrierMagicConfig(const string&	szFileName)
{
	using namespace CfgChk;

	CreateMap(ms_mapBarrierType, eBT_HighBarrier + 1, "", ("低"), ("中"), ("高"));
	CreateMap(ms_mapBarrierMagicType, eBMT_End, ("普通"), ("反射"));

	CTxtTableFile TabFile;
	SetTabFile(TabFile, "障碍魔法");
	if (!TabFile.Load(PATH_ALIAS_CFG.c_str(), szFileName.c_str())) 
		return false;

	ClearMap(ms_mapBarrierMagic);
	for( int32 i = 1; i < TabFile.GetHeight(); ++i )
	{
		SetLineNo(i);
		CBarrierMagicCfgServer* pBarrierMagic	= new CBarrierMagicCfgServer;
		pBarrierMagic->m_uId				= i;

		ReadItem(pBarrierMagic->m_sName,				szMagic_Name);
		ReadItem(pBarrierMagic->m_eBarrierType,			szMagic_BarrierType,	ms_mapBarrierType);
		ReadItem(pBarrierMagic->m_eBarrierMagicType,	szMagic_BarrierMagicType,	ms_mapBarrierMagicType);
		ReadItem(pBarrierMagic->m_pTime,				szMagic_Time,			GT,	0);

		string strCoordinate			= TabFile.GetString(i, szMagic_Coordinate);
		trimend(strCoordinate);
		if (!strCoordinate.empty())
		{
			vector<string> vecBarrier = Split(strCoordinate, "|");
			vector<string>::iterator it_Barrier = vecBarrier.begin();
			for(; it_Barrier != vecBarrier.end(); ++it_Barrier)
			{
				VecPos_t vecPos;
				string strPos = sqr::erase(*it_Barrier, "(");
				strPos = sqr::erase(*it_Barrier, ")");
				vector<string> vecStrPos = Split(strPos, ",");
				Ast(vecStrPos.size() % 2 == 0);
				for(uint32 i=0;i<vecStrPos.size();i+=2)
				{
					CPos pos = CPos(atoi(vecStrPos[i].c_str()),atoi(vecStrPos[i+1].c_str()));
					vecPos.push_back(pos);
				}
				pBarrierMagic->m_vecCoord.push_back(vecPos);
			}
		}

		CBarrierMagicCfgServerSharedPtr* pBarrierMagicSharedPtr	= new CBarrierMagicCfgServerSharedPtr(pBarrierMagic);
		if(!InsertStringSharedPtrMap(ms_mapBarrierMagic, pBarrierMagicSharedPtr))
		{
			SafeDelete(pBarrierMagicSharedPtr);
			continue;
		}
	}
	return true;
}

void CBarrierMagicCfgServer::UnloadBarrierMagicConfig()
{
	ClearMap(ms_mapBarrierMagic);
}

void CBarrierMagicCfgServer::UpdateCfg(const string& strName)
{
	UpdateCfg::UpdateCfg<MapBarrierMagicCfgServer, CBarrierMagicCfgServer, CBarrierMagicCfgServerSharedPtr>(ms_mapBarrierMagic, strName);
}

CBarrierMagicCfgServerSharedPtr& CBarrierMagicCfgServer::GetBarrierMagic(const string& szName)
{
	//如果没有魔法效果则返回NULL
	if(szName.empty())
	{
		return ms_NULL;
	}

	MapBarrierMagicCfgServer::iterator it_BarrierMagic = ms_mapBarrierMagic.find(szName);
	if(it_BarrierMagic == ms_mapBarrierMagic.end())
	{
		stringstream str; 
		str << "没有这个障碍魔法";
		CfgChk::GenExpInfo(str.str(), szName);
		return ms_NULL;
	}
	return *(it_BarrierMagic->second);
}

CBarrierMagicCfgServer::CBarrierMagicCfgServer()
{
	m_pMagicEff = new MagicEffServerStringRefer(this);
}

CBarrierMagicCfgServer::CBarrierMagicCfgServer(const CBarrierMagicCfgServer& cfg)
:CMagicCfgServer(cfg)
,m_vecCoord(cfg.m_vecCoord)
,m_eBarrierType(cfg.m_eBarrierType)
,m_eBarrierMagicType(cfg.m_eBarrierMagicType)
{
	string str;
	m_pMagicEff = new MagicEffServerStringRefer(this);
	cfg.m_pMagicEff->GetStringKey(str);
	m_pMagicEff->SetStringKey(str);
}

CBarrierMagicCfgServer::~CBarrierMagicCfgServer()
{
	SafeDelete(m_pMagicEff);
}

const CMagicEffServerSharedPtr& CBarrierMagicCfgServer::GetMagicEff()const
{
	return m_pMagicEff->Get();
}
