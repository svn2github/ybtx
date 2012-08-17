#include "stdafx.h"
#include "CTalentEffServer.h"
#include "CTxtTableFile.h"
#include "LoadSkillCommon.h"
#include "CPropertyCfg.h"
#include "CCfgColChecker.inl"
#include "BaseHelper.h"
#include "TStringRef.inl"
#include "CSkillCfg.h"

namespace sqr
{
	extern const wstring PATH_ALIAS_CFG;
}

CSkillCfg::MapTalentType CSkillCfg::ms_mapTalentType;
CSkillCfg::MapTalentRuleType CSkillCfg::ms_mapTalentRuleType;

CTalentEffCfgServer::MapTalentEffCfgServer CTalentEffCfgServer::ms_mapTalentEff;
CTalentEffCfgServer::MultiMapTalentEffCfgServer CTalentEffCfgServer::ms_multimapClassTalentEff;
CTalentEffCfgServerSharedPtr CTalentEffCfgServer::ms_NULL;

bool CTalentEffCfgServer::LoadTalentEffCfgConfig(const string& szFileName)
{
	using namespace CfgChk;
	CTxtTableFile TabFile;
	CSkillCfg::InitMapTalentType();
	CSkillCfg::InitMapTalentRuleType();
	SetTabFile(TabFile, "Ìì¸³Ð§¹û");
	if (!TabFile.Load(PATH_ALIAS_CFG.c_str(), szFileName.c_str())) 
		return false;

	for( int32 i = 1; i < TabFile.GetHeight(); ++i )
	{
		SetLineNo(i);
		CTalentEffCfgServer* pTalentEffCfg = new CTalentEffCfgServer;
		string strTypeArg;
		ReadItem(pTalentEffCfg->m_sTalentName,			szSkill_TalentName);
		tolower(pTalentEffCfg->m_sTalentName);
		ReadItem(strTypeArg,					szSkill_TypeArg,	CANEMPTY);
		ReadItem(pTalentEffCfg->m_bUpdate,			szSkill_UpdateWithLevel,		CANEMPTY,	NO);
		ReadItem(pTalentEffCfg->m_eTalentType,		szSkill_TalentType, CSkillCfg::ms_mapTalentType);
		ReadItem(pTalentEffCfg, *(pTalentEffCfg->m_pMagicEff),			szSkill_MagicEff);
		switch (pTalentEffCfg->m_eTalentType)
		{
		case eTT_Class:
			{
				EClass eClass= CPropertyCfg::ms_mapClassType[strTypeArg];
				CTalentEffCfgServerSharedPtr* pTalentEffCfgServerSharedPtr = new CTalentEffCfgServerSharedPtr(pTalentEffCfg);
				if(!InsertStringSharedPtrMap(ms_mapTalentEff, pTalentEffCfgServerSharedPtr))
				{
					SafeDelete(pTalentEffCfgServerSharedPtr);
					continue;
				}
				ms_multimapClassTalentEff.insert(pair<uint32, CTalentEffCfgServerSharedPtr*>(eClass, pTalentEffCfgServerSharedPtr));
				break;
			}
		case eTT_Camp:
		case eTT_Level:
			break;
		case eTT_Equip:
			{
				pTalentEffCfg->m_eRuleType= CSkillCfg::ms_mapTalentRuleType[strTypeArg];

				CTalentEffCfgServerSharedPtr* pTalentEffCfgServerSharedPtr = new CTalentEffCfgServerSharedPtr(pTalentEffCfg);
				if(!InsertStringSharedPtrMap(ms_mapTalentEff, pTalentEffCfgServerSharedPtr))
				{
					SafeDelete(pTalentEffCfgServerSharedPtr);
					continue;
				}
				break;
			}
		default :
			CTalentEffCfgServerSharedPtr* pTalentEffCfgServerSharedPtr = new CTalentEffCfgServerSharedPtr(pTalentEffCfg);
			if(!InsertStringSharedPtrMap(ms_mapTalentEff, pTalentEffCfgServerSharedPtr))
			{
				SafeDelete(pTalentEffCfgServerSharedPtr);
				continue;
			}
		}
	}
	return true;
}

void CTalentEffCfgServer::UnloadTalentEffCfgConfig()
{
	ClearMap(ms_mapTalentEff);
	ms_multimapClassTalentEff.clear();
}

CTalentEffCfgServer::CTalentEffCfgServer()
{
	m_pMagicEff = new MagicEffServerStringRefer(this);
}

CTalentEffCfgServer::CTalentEffCfgServer(const CTalentEffCfgServer& cfg)
:m_sTalentName(cfg.m_sTalentName)
,m_eTalentType(cfg.m_eTalentType)
,m_eRuleType(cfg.m_eRuleType)
{
	string str;
	m_pMagicEff = new MagicEffServerStringRefer(this);
	cfg.m_pMagicEff->GetStringKey(str);
	m_pMagicEff->SetStringKey(str);
}

CTalentEffCfgServer::~CTalentEffCfgServer()
{
	SafeDelete(m_pMagicEff);
}

CTalentEffCfgServerSharedPtr& CTalentEffCfgServer::GetTalentEff(const string& szName)
{
	MapTalentEffCfgServer::iterator it_TalentEffCfg = ms_mapTalentEff.find(szName);
	if(it_TalentEffCfg == ms_mapTalentEff.end())
	{
		return ms_NULL;
	}
	return *(it_TalentEffCfg->second);
}

void CTalentEffCfgServer::GetClassTalentEffVec(EClass eClass,VecTalentEffCfgServer& vecTalentEff)
{
	typedef MultiMapTalentEffCfgServer::iterator it_TalentEff;
	pair<it_TalentEff,it_TalentEff> pos = ms_multimapClassTalentEff.equal_range(eClass);
	for (; pos.first != pos.second; ++pos.first)
	{
		vecTalentEff.push_back(pos.first->second);
	}
}

void CTalentEffCfgServer::UpdateCfg(const string& strName)
{
}

const CMagicEffServerSharedPtr& CTalentEffCfgServer::GetMagicEff()const
{
	return m_pMagicEff->Get();
}
