#include "stdafx.h"
#include "CSkillServer.h"
#include "CTxtTableFile.h"
#include "LoadSkillCommon.h"
#include "CCfgColChecker.inl"
#include "BaseHelper.h"
#include "TStringRef.inl"
#include "CSkillCfg.h"

namespace sqr
{
	extern const wstring PATH_ALIAS_CFG;
}

template<>
void TStringRefer<CNormalSkillServer, CMagicEffServer>::OnRefeeChanged()const
{
	CNormalSkillServer::UpdateCfg(m_pHolder->GetName());
}

template<>
void TStringRefer<CNormalSkillServer, CCastingProcessCfgServer>::OnRefeeChanged()const
{
	CNormalSkillServer::UpdateCfg(m_pHolder->GetName());
}

template class TStringRefer<CNormalSkillServer, CMagicEffServer>;
template class TStringRefer<CNormalSkillServer, CCastingProcessCfgServer>;

CSkillCfg::MapTargetType CSkillCfg::ms_mapTargetType;
CSkillCfg::MapAttackType CSkillCfg::ms_mapAttackType;
CSkillCfg::MapCoolDownType CSkillCfg::ms_mapCoolDownType;

CNormalSkillServer::MapAlterNormalAttackType CNormalSkillServer::ms_mapAlterNormalAttackType;
CNormalSkillServer::MapNormalSkillServer CNormalSkillServer::ms_mapSkill;
CNormalSkillServer::MapNormalSkillServerById CNormalSkillServer::ms_mapSkillById;
CNormalSkillServer::MapBarrierType CNormalSkillServer::ms_mapBarrierType;

bool CNormalSkillServer::LoadSkillConfig(const string& szFileName)
{   
	using namespace CfgChk;

	CSkillCfg::InitMapTargetType();
	CSkillCfg::InitMapAttackType();
	CSkillCfg::InitMapCoolDownType();

	CreateMap(ms_mapAlterNormalAttackType, eANAT_End, "", ("开启并追踪"), ("开启"), ("一定开启并追踪"), ("一定开启"), ("关闭"));

	ms_mapBarrierType.insert(make_pair("低障",eBT_LowBarrier));
	ms_mapBarrierType.insert(make_pair("中障",eBT_MidBarrier));
	ms_mapBarrierType.insert(make_pair("高障",eBT_HighBarrier));

	CTxtTableFile TabFile; 
	SetTabFile(TabFile, "技能总表");
	if (!TabFile.Load(PATH_ALIAS_CFG.c_str(), szFileName.c_str())) return false;
	ms_mapSkill.clear();
	for( int32 i = 1; i < TabFile.GetHeight(); i++ )
	{
		SetLineNo(i);
		CNormalSkillServer* pSkill = new CNormalSkillServer;

		pSkill->m_uId								= i;

		//		 变量								行 列名							可为空	默认值	取值范围	取值范围参数
		ReadItem(pSkill->m_sName,					szSkill_Name);
		ReadItem(pSkill->m_bHorseLimit,				szSkill_HorseLimit,			CANEMPTY,	NO);
		ReadItem(pSkill->m_bIsStanceSkill,			szSkill_IsStanceSkill,		CANEMPTY,	NO);
		ReadItem(pSkill->m_bTargetAliveCheckIsIgnored,			szSkill_TargetIsCorpse,		CANEMPTY,	NO);
		ReadItem(pSkill->m_eAlterNormalAttackType,	szSkill_StartNormalAttack,	CANEMPTY,	eANAT_NoChange, ms_mapAlterNormalAttackType);
		//ReadItem(pSkill->m_bExpendableSkill,		szSkill_IsExpendableSkill,	CANEMPTY,	NO);
		//CCfgCalc* pCalcSelectType;
		//ReadMixedItem(pCalcSelectType,				szSkill_SelectTargetType);
		string sTempSelectType;
		ReadItem(sTempSelectType,					szSkill_SelectTargetType,	CANEMPTY,	"");
		string::size_type pos = sTempSelectType.find_first_of(',');
		if(pos == string::npos)
		{
			pSkill->m_eSelectTargetType = CSkillCfg::ms_mapTargetType[sTempSelectType];
		}
		else
		{
			string sSelectType = sTempSelectType.substr(0, pos);
			pSkill->m_eSelectTargetType = CSkillCfg::ms_mapTargetType[sSelectType];

			if (pSkill->m_eSelectTargetType==eFSTT_SelfDirection || pSkill->m_eSelectTargetType==eFSTT_Position)
			{
				SetValue(pSkill->m_pDistance, sTempSelectType.substr(pos + 1));
			}
		}

		ReadItem(pSkill->m_eBarrierType,			szSkill_BarrierType,		ms_mapBarrierType);
		ReadItem(pSkill->m_eAttackType,				szSkill_AttackType,			CSkillCfg::ms_mapAttackType);
		ReadItem(pSkill->m_pMinCastSkillDistance,	szSkill_MinCastSkillDistance);
		ReadItem(pSkill->m_pMaxCastSkillDistance,	szSkill_MaxCastSkillDistance);
		ReadItem(pSkill->m_eCoolDownType,			szSkill_Type,		CSkillCfg::ms_mapCoolDownType);
		ReadItem(pSkill->m_pCoolDownTime,			szSkill_CoolDownTime);
		string strActiveArg;
		ReadItem(strActiveArg,					szSkill_ActiveTimeAndCount,CANEMPTY);

		if (strActiveArg!= "")
		{
			CCfgCalc* pActiveArg = new CCfgCalc;
			CCfgCalc* pActiveTime = new CCfgCalc;
			pActiveArg->InputString(strActiveArg);
			if (pActiveArg->GetStringCount()>1)
			{
				CCfgCalc* pActiveCount = new CCfgCalc;
				pActiveCount->InputString(pActiveArg->GetString(1));
				pSkill->m_uActiveCount = pActiveCount->GetIntValue();
				delete pActiveCount;
			}
			pActiveTime->InputString(pActiveArg->GetString(0));
			pSkill->m_fActiveTime = (float)pActiveTime->GetDblValue();
			delete pActiveArg;
			delete pActiveTime;
		}
		ReadItem(pSkill, *(pSkill->m_pMagicEff),			szSkill_MagicEff, CANEMPTY);
		ReadMixedItem(pSkill->m_pSkillLevel2Level,		szSkill_LevelMap, CANEMPTY);

		string sTempCastAction;
		ReadItem(sTempCastAction,	szSkill_CastAction,	CANEMPTY,	"");
		pos = sTempCastAction.find_first_of(',');
		if(pos == string::npos)
			pSkill->m_sCastAction = sTempCastAction;
		else
			pSkill->m_sCastAction = sTempCastAction.substr(0, pos);

		if (!pSkill->m_pSkillLevel2Level->GetTestString().empty())
		{
			for (uint32 i =0; i<pSkill->m_pSkillLevel2Level->GetStringCount(); i ++)
			{
				string uTempLevelStr = pSkill->m_pSkillLevel2Level->GetString(i);
				int32 uTempLevel;
				SetValue(uTempLevel,uTempLevelStr);
				CheckUIntType(uTempLevel);
			}
		}

		CNormalSkillServerSharedPtr* pSkillRefPointer = new CNormalSkillServerSharedPtr(pSkill);
		if(!ms_mapSkill.insert(make_pair(pSkillRefPointer->get()->GetName(), pSkillRefPointer)).second)
		{
			stringstream ExpStr;
			ExpStr << "配置表[" << g_sTabName << "]第" << (g_iLine + 1) << "行的[" << pSkillRefPointer->get()->GetName() << "]名称重复";
			GenExpInfo(ExpStr.str());
			SafeDelete(pSkillRefPointer);
			continue;
		}
		if(!InsertSharedPtrVector(ms_mapSkillById, pSkillRefPointer))
		{
			SafeDelete(pSkillRefPointer);
			continue;
		}

		pSkill->m_pCastingProcess->SetStringKey(pSkill->m_sName);
	}
	return true;
}

bool CNormalSkillServer::LoadNPCSkillConfig(const string& szFileName)
{
	using namespace CfgChk;
	CTxtTableFile TabFile; 
	SetTabFile(TabFile, "NPC技能总表");
	if (!TabFile.Load(PATH_ALIAS_CFG.c_str(), szFileName.c_str())) return false;
	for( int32 i = 1; i < TabFile.GetHeight(); i++ )
	{
		SetLineNo(i);
		CNormalSkillServer* pSkill = new CNormalSkillServer;
		pSkill->m_uId								=ms_mapSkillById.size() + 1;
		ReadItem(pSkill->m_sName,					szSkill_Name);
		ReadItem(pSkill->m_eAttackType,				szSkill_AttackType,	CANEMPTY,	eATT_None,	CSkillCfg::ms_mapAttackType);
		ReadItem(pSkill, *(pSkill->m_pMagicEff),	szSkill_MagicEff);
		pSkill->m_sCastAction	= TabFile.GetString(i, szSkill_CastAction);
		trimend(pSkill->m_sCastAction);
		ReadItem(pSkill->m_bIsDoSkillRule,		szSkill_DoSkillRule,	CANEMPTY,	YES);

		CNormalSkillServerSharedPtr* pSkillRefPointer = new CNormalSkillServerSharedPtr(pSkill);
		if(!ms_mapSkill.insert(make_pair(pSkillRefPointer->get()->GetName(), pSkillRefPointer)).second)
		{
			stringstream ExpStr;
			ExpStr << "配置表[" << g_sTabName << "]第" << (g_iLine + 1) << "行的[" << pSkillRefPointer->get()->GetName() << "]名称重复";
			GenExpInfo(ExpStr.str());
			SafeDelete(pSkillRefPointer);
			continue;
		}
		if(!InsertSharedPtrVector(ms_mapSkillById, pSkillRefPointer))
		{
			SafeDelete(pSkillRefPointer);
			continue;
		}
		pSkill->m_pCastingProcess->SetStringKey(pSkill->m_sName);
	}
	return true;
}

bool CNormalSkillServer::LoadIntObjSkillConfig(const string& szFileName)
{
	using namespace CfgChk;
	CTxtTableFile TabFile; 
	SetTabFile(TabFile, "IntObj技能总表");
	if (!TabFile.Load(PATH_ALIAS_CFG.c_str(), szFileName.c_str())) return false;
	for( int32 i = 1; i < TabFile.GetHeight(); i++ )
	{
		SetLineNo(i);
		CNormalSkillServer* pSkill = new CNormalSkillServer;
		pSkill->m_uId								=ms_mapSkillById.size() + 1;
		ReadItem(pSkill->m_sName,					szSkill_Name);
		ReadItem(pSkill, *(pSkill->m_pMagicEff),				szSkill_MagicEff);
		pSkill->m_bIsDoSkillRule = false;

		CNormalSkillServerSharedPtr* pSkillRefPointer = new CNormalSkillServerSharedPtr(pSkill);
		if(!ms_mapSkill.insert(make_pair(pSkillRefPointer->get()->GetName(), pSkillRefPointer)).second)
		{
			stringstream ExpStr;
			ExpStr << "配置表[" << g_sTabName << "]第" << (g_iLine + 1) << "行的[" << pSkillRefPointer->get()->GetName() << "]名称重复";
			GenExpInfo(ExpStr.str());
			SafeDelete(pSkillRefPointer);
			continue;
		}
		if(!InsertSharedPtrVector(ms_mapSkillById, pSkillRefPointer))
		{
			SafeDelete(pSkillRefPointer);
			continue;
		}
		pSkill->m_pCastingProcess->SetStringKey(pSkill->m_sName);
	}
	return true;
}

void CNormalSkillServer::UnloadSkillConfig()
{
	ClearMap(ms_mapSkill);
	ms_mapSkillById.clear();
}

CNormalSkillServer::CNormalSkillServer()
:m_pMinCastSkillDistance(NULL)
,m_pMaxCastSkillDistance(NULL)
,m_pCoolDownTime(NULL)
,m_pSkillLevel2Level(NULL)
,m_pDistance(NULL)
,m_fActiveTime(0)
,m_uActiveCount(1)
,m_bHorseLimit(false)
,m_bIsStanceSkill(false)
,m_bTargetAliveCheckIsIgnored(false)
,m_eAlterNormalAttackType(eANAT_Cancel)
,m_eCoolDownType(eSCDT_OtherSkill)
,m_eAttackType(eATT_None)
,m_sCastAction("")
,m_bIsDoSkillRule(true)
{
	m_pMagicEff = new MagicEffServerStringRefer(this);
	m_pCastingProcess = new CastingProcessCfgServerStringRefer(this);
}

CNormalSkillServer::CNormalSkillServer(const CNormalSkillServer& cfg)
:m_pMinCastSkillDistance(cfg.m_pMinCastSkillDistance?new CCfgCalc(*(cfg.m_pMinCastSkillDistance)):NULL)
,m_pMaxCastSkillDistance(cfg.m_pMaxCastSkillDistance?new CCfgCalc(*(cfg.m_pMaxCastSkillDistance)):NULL)
,m_pCoolDownTime(cfg.m_pCoolDownTime?new CCfgCalc(*(cfg.m_pCoolDownTime)):NULL)
,m_pSkillLevel2Level(cfg.m_pSkillLevel2Level?new CCfgCalc(*(cfg.m_pSkillLevel2Level)):NULL)
,m_pDistance(cfg.m_pDistance?new CCfgCalc(*(cfg.m_pDistance)):NULL)
,m_fActiveTime(cfg.m_fActiveTime)
,m_uActiveCount(cfg.m_uActiveCount)
,m_bHorseLimit(cfg.m_bHorseLimit)
,m_bIsStanceSkill(cfg.m_bIsStanceSkill)
,m_bTargetAliveCheckIsIgnored(cfg.m_bTargetAliveCheckIsIgnored)
,m_eAlterNormalAttackType(cfg.m_eAlterNormalAttackType)
,m_eCoolDownType(cfg.m_eCoolDownType)
,m_uId(cfg.m_uId)
,m_sName(cfg.m_sName)
,m_eSelectTargetType(cfg.m_eSelectTargetType)
,m_eAttackType(cfg.m_eAttackType)
,m_sCastAction(cfg.m_sCastAction)
,m_bIsDoSkillRule(cfg.m_bIsDoSkillRule)
,m_eBarrierType(cfg.m_eBarrierType)
{
	string str;
	m_pMagicEff = new MagicEffServerStringRefer(this);
	cfg.m_pMagicEff->GetStringKey(str);
	m_pMagicEff->SetStringKey(str);
	m_pCastingProcess = new CastingProcessCfgServerStringRefer(this);
	cfg.m_pCastingProcess->GetStringKey(str);
	m_pCastingProcess->SetStringKey(str);
}

CNormalSkillServer::~CNormalSkillServer()
{
	SafeDelete(m_pMinCastSkillDistance);
	SafeDelete(m_pMaxCastSkillDistance);
	SafeDelete(m_pCoolDownTime);
	SafeDelete(m_pSkillLevel2Level);
	SafeDelete(m_pDistance);
	SafeDelete(m_pCastingProcess);
	SafeDelete(m_pMagicEff);
}

CNormalSkillServerSharedPtr& CNormalSkillServer::GetSkillByName(const string& szName)
{
	MapNormalSkillServer::iterator it_Skill = ms_mapSkill.find(szName);
	if(it_Skill == ms_mapSkill.end())
	{
		stringstream str; 
		str << "没有这个技能: " << szName;
		//GenErr(str.str().c_str());
		return ms_NULL;
	}
	return *(it_Skill->second);
}

CNormalSkillServerSharedPtr& CNormalSkillServer::GetSkillByID(uint32 uID)
{
	--uID;
	if(uID >= ms_mapSkillById.size() || (uID < ms_mapSkillById.size() && ms_mapSkillById[uID] == NULL))
	{
		stringstream str; 
		str << "没有这个技能: " << uID;
		//GenErr(str.str().c_str());
		return ms_NULL;
	}
	return *(ms_mapSkillById[uID]);
}

CNormalSkillServerSharedPtr& CNormalSkillServer::GetNPCSkill(const string& szName)
{
	return GetSkillByName(szName);
}

CNormalSkillServerSharedPtr& CNormalSkillServer::GetIntObjSkill(const string& szName)
{
	return GetSkillByName(szName);
}

ESkillType CNormalSkillServer::GetSkillType()const
{
	if (eATT_Puncture == m_eAttackType || eATT_Chop == m_eAttackType || eATT_BluntTrauma == m_eAttackType)
		return eST_Physical;

	if (eATT_Nature == m_eAttackType || eATT_Destroy == m_eAttackType || eATT_Evil == m_eAttackType)
		return eST_Magic;

	return eST_None;
}	

void CNormalSkillServer::UpdateCfg(const string& strName)
{
	MapNormalSkillServer::iterator it = ms_mapSkill.find(strName);
	if(it != ms_mapSkill.end())
	{
		CNormalSkillServerSharedPtr* pOldCfg = it->second;
		ms_mapSkill.erase(it);
		CNormalSkillServerSharedPtr* pNewCfg = new CNormalSkillServerSharedPtr(new CNormalSkillServer(*(pOldCfg->get())));
		ms_mapSkill.insert(make_pair(strName, pNewCfg));

		if(pOldCfg->get()->GetId() <= ms_mapSkillById.size())
		{
			ms_mapSkillById[pOldCfg->get()->GetId() - 1] = NULL;
		}
		CfgChk::InsertSharedPtrVector(ms_mapSkillById, pNewCfg);

		delete pOldCfg;
	}
}

const CMagicEffServerSharedPtr& CNormalSkillServer::GetMagicEff()const
{
	return m_pMagicEff->Get();
}

const CCastingProcessCfgServerSharedPtr& CNormalSkillServer::GetCastingProcess()const
{
	return m_pCastingProcess->Get();
}

