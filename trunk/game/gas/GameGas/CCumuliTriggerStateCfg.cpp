#include "stdafx.h"
#include "CCumuliTriggerStateCfg.h"
#include "CTriggerEvent.h"
#include "LoadSkillCommon.h"
#include "CTxtTableFile.h"
#include "CCfgCalc.inl"
#include "CCfgColChecker.inl"
#include "CTempVarServer.h"
#include "ErrLogHelper.h"
#include "BaseHelper.h"
#include "TStringRef.inl"
#include "CSkillCfg.h"

namespace sqr
{
	extern const wstring PATH_ALIAS_CFG;
}

CCumuliTriggerStateCfgServerSharedPtr CCumuliTriggerStateCfgServer::ms_NULL;

CCumuliTriggerStateCfgServer::CCumuliTriggerStateCfgServer()
: CTriggerableStateCfgServer(eSGT_CumuliTriggerState)
{
	m_pTriggerEff = new MagicEffServerStringRefer(this);
}

CCumuliTriggerStateCfgServer::CCumuliTriggerStateCfgServer(const CCumuliTriggerStateCfgServer& cfg)
: CTriggerableStateCfgServer(cfg)
,m_sTempVar(cfg.m_sTempVar)
,m_calcMaxNumInSingleEvent(new CCfgCalc(*(cfg.m_calcMaxNumInSingleEvent)))
{
	string str;
	m_pTriggerEff = new MagicEffServerStringRefer(this);
	cfg.m_pTriggerEff->GetStringKey(str);
	m_pTriggerEff->SetStringKey(str);
}

CCumuliTriggerStateCfgServer::~CCumuliTriggerStateCfgServer()
{
	SafeDelete(m_pTriggerEff);
	SafeDelete(m_calcMaxNumInSingleEvent);
}

//初始化静态成员
CCumuliTriggerStateCfgServer::MapCumuliTriggerStateCfg CCumuliTriggerStateCfgServer::m_mapCfg;
CCumuliTriggerStateCfgServer::MapCumuliTriggerStateCfgById CCumuliTriggerStateCfgServer::m_mapCfgById;

bool CCumuliTriggerStateCfgServer::LoadConfig(const TCHAR* cfgFile)
{
	using namespace CfgChk;

	CTxtTableFile TabFile;
	stringstream ExpStr;				//用于异常输出
	CCumuliTriggerStateCfgServer* pCfgNode;

	SetTabFile(TabFile, "积累触发状态");
	if (!TabFile.Load(PATH_ALIAS_CFG.c_str(), cfgFile)) return false;

	m_mapCfg.clear();
	CSkillCfg::InitDCStateTempVarMap();
	CSkillCfg::InitPassivityDCStateTempVarMap();
	for(int32 i=1; i<TabFile.GetHeight(); i++)
	{
		SetLineNo(i);
		pCfgNode = new CCumuliTriggerStateCfgServer;
		pCfgNode->m_eGlobalType = eSGT_CumuliTriggerState;
		pCfgNode->m_uId = eSIC_CumuliTriggerState + i;

		ReadItem(pCfgNode->m_sName,				szTriggerState_Name);
		ReadItem(pCfgNode->m_eDecreaseType,		szTplState_DecreateType,			CANEMPTY,	eDST_Increase,	ms_mapDecreaseType);
		ReadItem(pCfgNode->m_bDispellable,			szTplState_Dispellable,			CANEMPTY,	YES);
		ReadItem(pCfgNode->m_eIconCancelCond,	szBaseState_CancelCond,				CANEMPTY,	eICC_None,		ms_mapIconCancelCond);
		ReadItem(pCfgNode->m_calcTime,			szTriggerState_Time,			GE,			-1);
		ReadItem(pCfgNode->m_calcInitialValue,	szCumuliTriggerState_UnitValue,	GE,			-1);
		ReadItem(pCfgNode->m_calcMaxNumInSingleEvent,	szCumuliTriggerState_MaxNumInSingleEvent,			GE,			-1);
		//ReadItem(pCfgNode->m_eTargetChange,		szTriggerState_ChangeTrigger,	m_mapTargetChange);

		//if(pCfgNode->m_eTargetChange != eTC_Self && pCfgNode->m_eTargetChange != eTC_Installer)
		//{
		//	GenExpInfo("违反表约束：在伤害变更状态里不能填安装者和拥有者以外的值", TabFile.GetString(i, szCumuliTriggerState_ChangeTrigger));
		//}

		ReadItem(pCfgNode->m_calcProbability,	szTriggerState_Probability,	IsRate);

		bool bArg2;
		ReadItem(bArg2,							szTriggerState_Passivity, CANEMPTY, NO);

		EHurtResult eArg1;
		ESpecialEventType eAnotherArg1;
		uint32 uResult = ReadItem(eArg1, eAnotherArg1, szTriggerState_TriggerEvent, CANEMPTY, 
			CTriggerEvent::m_smapEHurtResult, CTriggerEvent::m_smapESpecialEventType);
		switch(uResult)
		{
		case 0:
			pCfgNode->m_uTriggerEvent = 0;
			break;
		case 1:
			ESkillType eArg3;
			ReadItem(eArg3, szTriggerState_SkillType, CANEMPTY, eST_None, CTriggerEvent::m_smapESkillType);
			EAttackType eArg4;
			ReadItem(eArg4, szTriggerState_AttackType, CANEMPTY, eATT_None, CTriggerEvent::m_smapAttackType);
			pCfgNode->m_uTriggerEvent = CTriggerEvent::GetID(eArg1, bArg2, eArg3, eArg4);
			break;
		case 2:
			pCfgNode->m_uTriggerEvent = CTriggerEvent::GetID(eAnotherArg1, bArg2);
			break;
		}

		ReadItem(pCfgNode->m_sModel,			szTplState_Model,					CANEMPTY);
		//ReadItem(pCfgNode->m_fScale,			szTplState_Scale,					CANEMPTY,		1.0f,		GT,		0.0f);

		CCfgCalc* pCalc = NULL;
		ReadMixedItem(pCalc,			szTplState_Scale,				CANEMPTY,	"");
		if(pCalc->GetTestString().empty())
		{
			pCfgNode->m_fScale = 1.0f;
			pCfgNode->m_fScaleTime = 0.0f;
		}
		else
		{
			int32 iValueCount = pCalc->GetValueCount();
			pCfgNode->m_fScale = (float)pCalc->GetDblValue(0);
			if(iValueCount > 1)
			{
				pCfgNode->m_fScaleTime = (float)pCalc->GetDblValue(1);
			}
			else
			{
				pCfgNode->m_fScaleTime = 0.0f;
			}
		}

		string sFx;
		ReadItem(sFx,				szTplState_FXID,		CANEMPTY,		"");
		if(!pCfgNode->m_sModel.empty() || !pCfgNode->m_sModel.empty() || !pCalc->GetTestString().empty()
			|| !sFx.empty())
		{
			pCfgNode->SetNeedSync();
		}

		delete pCalc;

		string strTriggerEvent;
		ReadItem(strTriggerEvent,		szTriggerState_TriggerEvent);
		if (bArg2)
			pCfgNode->m_sTempVar = CSkillCfg::ms_mapPassivityTempVarType[strTriggerEvent];
		else
			pCfgNode->m_sTempVar = CSkillCfg::ms_mapTempVarType[strTriggerEvent];
		ReadItem(pCfgNode, *(pCfgNode->m_pTriggerEff),		szTriggerState_TriggerEff,			CANEMPTY);

		pCfgNode->m_bDecrease = pCfgNode->m_eDecreaseType <= eDST_IncreaseEnd ? false : true;

		if (pCfgNode->m_calcTime->IsSingleNumber() && (eAnotherArg1 != eSET_KillByInstaller && eAnotherArg1 != eSET_InstallerDie))
		{
			pCfgNode->SetNeedSaveToDB(pCfgNode->m_calcTime->GetDblValue());
		}
		ReadItem(pCfgNode->m_bPersistent,			szState_Persistent,				CANEMPTY,		NO);

		CCumuliTriggerStateCfgServerSharedPtr* pCfgNodeSharedPtr = new CCumuliTriggerStateCfgServerSharedPtr(pCfgNode);
		InsertStringSharedPtrMap(m_mapCfg, pCfgNodeSharedPtr);
		InsertUint32SharedPtrMap(m_mapCfgById, pCfgNodeSharedPtr);
	}

	return true;
}

void CCumuliTriggerStateCfgServer::UnloadConfig()
{
	ClearMap(m_mapCfg);
}

CCumuliTriggerStateCfgServerSharedPtr& CCumuliTriggerStateCfgServer::Get(const string& damageChangeStateName)
{
	MapCumuliTriggerStateCfg::iterator mapCfgItr;
	mapCfgItr = m_mapCfg.find(damageChangeStateName);
	if (mapCfgItr == m_mapCfg.end())
	{
		stringstream ExpStr;
		ExpStr << "伤害变更魔法状态名不存在";
		CfgChk::GenExpInfo(ExpStr.str(), damageChangeStateName);
		return ms_NULL;
	}
	return *(mapCfgItr->second);
}

void CCumuliTriggerStateCfgServer::UpdateCfg(const string& strName)
{
	MapCumuliTriggerStateCfg::iterator it_name = m_mapCfg.find(strName);
	if(it_name != m_mapCfg.end())
	{
		CCumuliTriggerStateCfgServerSharedPtr* pOldCfg = it_name->second;
		CCumuliTriggerStateCfgServerSharedPtr* pNewCfg = new CCumuliTriggerStateCfgServerSharedPtr(new CCumuliTriggerStateCfgServer(*(pOldCfg->get())));
		m_mapCfg.erase(it_name);
		CfgChk::InsertStringSharedPtrMap(m_mapCfg, pNewCfg);
		MapCumuliTriggerStateCfgById::iterator it_id2 = m_mapCfgById.find(pOldCfg->get()->GetId());
		if(it_id2 != m_mapCfgById.end())
		{
			m_mapCfgById.erase(it_id2);
			CfgChk::InsertUint32SharedPtrMap(m_mapCfgById, pNewCfg);
		}

		delete pOldCfg;
	}
}

const CMagicEffServerSharedPtr& CCumuliTriggerStateCfgServer::GetTriggerEff()const
{
	return m_pTriggerEff->Get();
}

