#include "stdafx.h"
#include "CSkillClient.h"
#include "CTxtTableFile.h"
#include "LoadSkillCommon.h"
#include "CCfgCalc.h"
#include "CFighterDirector.h"
#include "CCastingProccessClient.h"
#include "CCfgColChecker.inl"
#include "BaseHelper.h"
#include "StringHelper.h"
#include "CSkillCfg.h"

namespace sqr
{
	extern const wstring PATH_ALIAS_CFG;
}

CSkillCfg::MapTargetType CSkillCfg::ms_mapTargetType;
CSkillCfg::MapAttackType CSkillCfg::ms_mapAttackType;
CSkillCfg::MapCoolDownType CSkillCfg::ms_mapCoolDownType;
CSkillClient::MapAlterNormalAttackType CSkillClient::ms_mapAlterNormalAttackType;

CSkillClient::MapSkillClientByName CSkillClient::ms_mapSkillByName;
CSkillClient::MapSkillClientByID CSkillClient::ms_mapSkillByID;

bool CSkillClient::LoadSkillConfig(const string& szFileName)
{   
	using namespace CfgChk;

	CSkillCfg::InitMapTargetType();
	CSkillCfg::InitMapAttackType();
	CSkillCfg::InitMapCoolDownType();
	CreateMap(ms_mapAlterNormalAttackType, eANAT_End, "", ("开启并追踪"), ("开启"), ("一定开启并追踪"), ("一定开启"), ("关闭"));

	CTxtTableFile TabFile; 
	if (!TabFile.Load(PATH_ALIAS_CFG.c_str(), szFileName.c_str())) return false;
	SetTabFile(TabFile, "技能总表");
	UnloadSkillConfig();										


	for( int i = 1; i < TabFile.GetHeight(); i++ )
	{
		SetLineNo(i);
		CSkillClient* pSkill			= new CSkillClient;
		pSkill->m_pMinCastSkillDistance	= new CCfgCalc();
		pSkill->m_pMaxCastSkillDistance	= new CCfgCalc();
		pSkill->m_pCoolDownTime			= new CCfgCalc();
		pSkill->m_pSkillLevel2Level		= new CCfgCalc();
		pSkill->m_uId					= i;
		pSkill->m_sName					= TabFile.GetString(i, szSkill_Name);
		trimend(pSkill->m_sName);
		ReadItem(pSkill->m_bHorseLimit,				szSkill_HorseLimit,			CANEMPTY,	NO);
		ReadItem(pSkill->m_bIsStanceSkill,			szSkill_IsStanceSkill,		CANEMPTY,	NO);
		ReadItem(pSkill->m_bTargetIsCorpse,			szSkill_TargetIsCorpse,		CANEMPTY,	NO);
		ReadItem(pSkill->m_eCoolDownType,			szSkill_Type,		CSkillCfg::ms_mapCoolDownType);
		ReadItem(pSkill->m_bIsUpdateDirection,		szSkill_IsUpdateDirection,	CANEMPTY,	YES);
		ReadItem(pSkill->m_eAttackType,				szSkill_AttackType,			CSkillCfg::ms_mapAttackType);
		ReadItem(pSkill->m_eAlterNormalAttackType,	szSkill_StartNormalAttack,	CANEMPTY,	eANAT_NoChange, ms_mapAlterNormalAttackType);
		pSkill->m_pMinCastSkillDistance->InputString(TabFile.GetString(i, szSkill_MinCastSkillDistance)) ;
		pSkill->m_pMaxCastSkillDistance->InputString(TabFile.GetString(i, szSkill_MaxCastSkillDistance)) ;
		pSkill->m_pCoolDownTime->InputString(TabFile.GetString(i, szSkill_CoolDownTime)) ;
		pSkill->m_sReachUpAction	= TabFile.GetString(i, szSkill_ReachUpAction);
		trimend(pSkill->m_sReachUpAction);
		pSkill->m_sReachUpEffect	= TabFile.GetString(i, szSkill_ReachUpEffect);
		trimend(pSkill->m_sReachUpEffect);

		pSkill->m_bAllBodyCastAni = false;
		string sTempCastAction;
		ReadItem(sTempCastAction,	szSkill_CastAction,	CANEMPTY,	"");
		string::size_type pos = sTempCastAction.find_first_of(',');
		if(pos == string::npos)
		{
			pSkill->m_sCastAction = sTempCastAction;
		}
		else
		{
			pSkill->m_sCastAction = sTempCastAction.substr(0, pos);
			if ( "allbody" == sTempCastAction.substr(pos + 1))
				pSkill->m_bAllBodyCastAni = true;
		}

		pSkill->m_sCastEffect		= TabFile.GetString(i, szSkill_CastEffect);
		trimend(pSkill->m_sCastEffect);
		pSkill->m_sMagicEff			= TabFile.GetString(i, szSkill_MagicEff);	
		trimend(pSkill->m_sMagicEff);
		pSkill->m_pSkillLevel2Level->InputString(TabFile.GetString(i,szSkill_LevelMap));

		string sTempSelectType;
		ReadItem(sTempSelectType,						szSkill_SelectTargetType,	CANEMPTY,	"");
		pos = sTempSelectType.find_first_of(',');
		if(pos == string::npos)
		{
			pSkill->m_eSelectTargetType = CSkillCfg::ms_mapTargetType[sTempSelectType];
		}
		else
		{
			string sSelectType = sTempSelectType.substr(0, pos);
			pSkill->m_eSelectTargetType = CSkillCfg::ms_mapTargetType[sSelectType];
			if (pSkill->m_eSelectTargetType==eFSTT_Position)
			{
				SetValue(pSkill->m_pPositionArea, sTempSelectType.substr(pos + 1));
			}
		}

		CCastingProcessCfgClientSharedPtr& pCastingProcess = CCastingProcessCfgClient::GetByName(pSkill->
m_sName);
		pSkill->InitCastingProcess(pCastingProcess);

		CSkillClientSharedPtr* pSkillSharedPtr = new CSkillClientSharedPtr(pSkill);
		ms_mapSkillByName.insert(pair<string, CSkillClientSharedPtr*>(pSkill->m_sName, pSkillSharedPtr));
		InsertSharedPtrVector(ms_mapSkillByID, pSkillSharedPtr);
	}
	return true;
}

bool CSkillClient::LoadNPCSkillConfig(const string& szFileName)
{
	using namespace CfgChk;
	CTxtTableFile TabFile; 
	if (!TabFile.Load(PATH_ALIAS_CFG.c_str(), szFileName.c_str())) return false;
	SetTabFile(TabFile, "NPC技能总表");						
	for( int i = 1; i < TabFile.GetHeight(); i++ )
	{
		SetLineNo(i);
		CSkillClient* pSkill = new CSkillClient;
		pSkill->m_uId					=ms_mapSkillByID.size() + 1;
		pSkill->m_sName					= TabFile.GetString(i, szSkill_Name);
		trimend(pSkill->m_sName);
		pSkill->m_sCastAction			= TabFile.GetString(i, szSkill_CastAction);
		trimend(pSkill->m_sCastAction);
		pSkill->m_sCastEffect			= TabFile.GetString(i, szSkill_CastEffect);
		pSkill->m_bAllBodyCastAni		= false;
		trimend(pSkill->m_sCastEffect);
		pSkill->m_sMagicEff				= TabFile.GetString(i, szSkill_MagicEff);	
		trimend(pSkill->m_sMagicEff);

		CCastingProcessCfgClientSharedPtr& pCastingProcess = CCastingProcessCfgClient::GetByName(pSkill->
m_sName);
		pSkill->InitCastingProcess(pCastingProcess);

		CSkillClientSharedPtr* pSkillSharedPtr = new CSkillClientSharedPtr(pSkill);
		InsertSharedPtrVector(ms_mapSkillByID, pSkillSharedPtr);
	}
	return true;
}

bool CSkillClient::LoadIntObjSkillConfig(const string& szFileName)
{
	using namespace CfgChk;
	CTxtTableFile TabFile; 
	if (!TabFile.Load(PATH_ALIAS_CFG.c_str(), szFileName.c_str())) return false;
	SetTabFile(TabFile, "IntObj技能总表");						
	for( int i = 1; i < TabFile.GetHeight(); i++ )
	{
		SetLineNo(i);
		CSkillClient* pSkill = new CSkillClient;
		pSkill->m_uId					=ms_mapSkillByID.size() + 1;
		pSkill->m_sName					= TabFile.GetString(i, szSkill_Name);
		trimend(pSkill->m_sName);
		pSkill->m_sMagicEff				= TabFile.GetString(i, szSkill_MagicEff);	
		trimend(pSkill->m_sMagicEff);

		CCastingProcessCfgClientSharedPtr& pCastingProcess = CCastingProcessCfgClient::GetByName(pSkill->
m_sName);
		pSkill->InitCastingProcess(pCastingProcess);

		CSkillClientSharedPtr* pSkillSharedPtr = new CSkillClientSharedPtr(pSkill);
		InsertSharedPtrVector(ms_mapSkillByID, pSkillSharedPtr);
	}
	return true;
}

void CSkillClient::UnloadSkillConfig()
{
	ClearVector(ms_mapSkillByID);
}
CSkillClient::CSkillClient()
:m_pCoolDownTime(NULL)
,m_pMinCastSkillDistance(NULL)
,m_pMaxCastSkillDistance(NULL)
,m_pSkillLevel2Level(NULL)
,m_pPositionArea(NULL)
{

}
CSkillClient::~CSkillClient()
{
	SafeDelete(m_pMinCastSkillDistance);
	SafeDelete(m_pMaxCastSkillDistance);
	SafeDelete(m_pCoolDownTime);
	SafeDelete(m_pSkillLevel2Level);
	SafeDelete(m_pPositionArea);
	SafeDelete(m_pCastingProcessTime);
}

CSkillClient* CSkillClient::GetSkillByNameForLua(const TCHAR* szName)
{
	return GetSkillByName(szName).get();
}

CSkillClientSharedPtr& CSkillClient::GetSkillByName(const TCHAR* szName)
{
	MapSkillClientByName::iterator it_Skill = ms_mapSkillByName.find(szName);
	if(it_Skill == ms_mapSkillByName.end())
	{
		stringstream str; 
		str << "没有这个技能: " << szName;
		GenErr(str.str().c_str());
	}
	return *(it_Skill->second);
}

CSkillClientSharedPtr& CSkillClient::GetSkillByID(uint32 uID)
{
	stringstream ExpStr;
	--uID;
	if(uID >= ms_mapSkillByID.size() || (uID < ms_mapSkillByID.size() && ms_mapSkillByID[uID] == NULL))
	{
		ExpStr << "技能" << uID << "不存在";
		GenErr(ExpStr.str());
	}
	return *(ms_mapSkillByID[uID]);
}

CMagicEffClientSharedPtr& CSkillClient::GetMagicEff()
{
	if(m_sMagicEff.empty())
		return CMagicEffClient::ms_NULL;
	return CMagicEffClient::GetMagicEff(m_sMagicEff);
}

uint32 CSkillClient::GetPositionArea(CFighterDirector* pFrom, uint8 uSkillLevel)
{
	//return 0;
	return m_pPositionArea->GetIntValue(pFrom, uSkillLevel-1);
}

bool CSkillClient::MustIdle()
{
	return m_bMustIdle;
}

bool CSkillClient::HasCastingProcess()const
{
	return m_bHasCastingProcess;
}

EConsumeType CSkillClient::GetConsumeType()
{
	CMagicEffClientSharedPtr& pMagicEffClient = this->GetMagicEff();
	if(!pMagicEffClient)
		return eCT_None;
	return pMagicEffClient->GetEConsumeType();
}

CCfgCalc* CSkillClient::GetConsumeValue()
{
	CMagicEffClientSharedPtr& pMagicEffClient = this->GetMagicEff();
	if(!pMagicEffClient)
		return NULL;

	EConsumeType eConsumeType = GetConsumeType();
	if (eCT_HP == eConsumeType)
	{
		return pMagicEffClient->GetMagicOpArg("生命值大于等于");
	}
	else if (eCT_MP == eConsumeType)
	{
		return pMagicEffClient->GetMagicOpArg("魔法值大于等于");
	}
	else if (eCT_RG == eConsumeType)
	{
		return pMagicEffClient->GetMagicOpArg("怒气值大于等于");
	}
	else if (eCT_EG == eConsumeType)
	{
		return pMagicEffClient->GetMagicOpArg("能量值大于等于");
	}
	else if (eCT_CP == eConsumeType)
	{
		return pMagicEffClient->GetMagicOpArg("连击点大于等于");
	}
	return NULL;
}

ECastingProcessType CSkillClient::GetCastingProcessType()
{
	return m_eCastingProcessType;
}

CCfgCalc* CSkillClient::GetCastingProcessTime()
{
	return m_pCastingProcessTime;
}

void CSkillClient::InitCastingProcess(const CCastingProcessCfgClientSharedPtr& cfg)
{
	if(cfg)
	{
		m_pCastingProcessTime = new CCfgCalc(*(cfg->GetCastingTime()));
		m_eCastingProcessType = cfg->GetCastingType();
		m_bMustIdle = cfg->GetMoveInterrupt();
		m_bHasCastingProcess = true;
	}
	else
	{
		m_pCastingProcessTime = NULL;
		m_eCastingProcessType = eCPT_Wink;
		m_bMustIdle = false;
		m_bHasCastingProcess = false;
	}
}
