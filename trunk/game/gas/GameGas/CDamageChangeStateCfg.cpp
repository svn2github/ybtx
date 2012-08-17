#include "stdafx.h"
#include "CDamageChangeStateCfg.h"
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

CDamageChangeStateCfgServerSharedPtr CDamageChangeStateCfgServer::ms_NULL;

CDamageChangeStateCfgServer::CDamageChangeStateCfgServer()
: CTriggerableStateCfgServer(eSGT_DamageChangeState)
{
	m_pFinalMagicEff = new MagicEffServerStringRefer(this);
	m_pCancelableMagicEff = new MagicEffServerStringRefer(this); 
}

CDamageChangeStateCfgServer::CDamageChangeStateCfgServer(const CDamageChangeStateCfgServer& cfg)
: CTriggerableStateCfgServer(cfg)
,m_sTempVar(cfg.m_sTempVar)
,m_calcTempValue(new CCfgCalc(*(cfg.m_calcTempValue)))
,m_bApplyTempValue(cfg.m_bApplyTempValue)
{
	string str;
	m_pFinalMagicEff = new MagicEffServerStringRefer(this);
	cfg.m_pFinalMagicEff->GetStringKey(str);
	m_pFinalMagicEff->SetStringKey(str);
	m_pCancelableMagicEff = new MagicEffServerStringRefer(this); 
	cfg.m_pCancelableMagicEff->GetStringKey(str);
	m_pCancelableMagicEff->SetStringKey(str);
}

CDamageChangeStateCfgServer::~CDamageChangeStateCfgServer()
{
	SafeDelete(m_calcTempValue);
	SafeDelete(m_pFinalMagicEff);
	SafeDelete(m_pCancelableMagicEff);
}

//初始化静态成员
CDamageChangeStateCfgServer::MapDamageChangeStateCfg CDamageChangeStateCfgServer::m_mapCfg;
CDamageChangeStateCfgServer::MapDamageChangeStateCfgById CDamageChangeStateCfgServer::m_mapCfgById;
CSkillCfg::MapTempVarType			CSkillCfg::ms_mapTempVarType;
CSkillCfg::MapPassivityTempVarType	CSkillCfg::ms_mapPassivityTempVarType;

bool CDamageChangeStateCfgServer::LoadConfig(const TCHAR* cfgFile)
{
	using namespace CfgChk;

	CTxtTableFile TabFile;
	stringstream ExpStr;				//用于异常输出
	CDamageChangeStateCfgServer* pCfgNode;

	SetTabFile(TabFile, "伤害变更状态");
	if (!TabFile.Load(PATH_ALIAS_CFG.c_str(), cfgFile)) return false;

	m_mapCfg.clear();
	CSkillCfg::InitDCStateTempVarMap();
	CSkillCfg::InitPassivityDCStateTempVarMap();
	for(int32 i=1; i<TabFile.GetHeight(); i++)
	{
		SetLineNo(i);
		pCfgNode = new CDamageChangeStateCfgServer;
		pCfgNode->m_eGlobalType = eSGT_DamageChangeState;
		pCfgNode->m_uId = eSIC_DamageChangeState + i;

		ReadItem(pCfgNode->m_sName,				szDamageChangeState_Name);
		ReadItem(pCfgNode->m_eDecreaseType,		szTplState_DecreateType,			CANEMPTY,	eDST_Increase,	ms_mapDecreaseType);
		ReadItem(pCfgNode->m_bDispellable,			szTplState_Dispellable,			CANEMPTY,	YES);
		ReadItem(pCfgNode->m_eIconCancelCond,	szBaseState_CancelCond,				CANEMPTY,	eICC_None,		ms_mapIconCancelCond);
		ReadItem(pCfgNode->m_calcTime,			szDamageChangeState_Time,			GE,			-1);
		ReadItem(pCfgNode->m_calcInitialValue,	szDamageChangeState_InitialValue,	GE,			-1);
		//ReadItem(pCfgNode->m_eTargetChange,		szDamageChangeState_ChangeTrigger,	m_mapTargetChange);

		//if(pCfgNode->m_eTargetChange != eTC_Self && pCfgNode->m_eTargetChange != eTC_Installer)
		//{
		//	GenExpInfo("违反表约束：在伤害变更状态里不能填安装者和拥有者以外的值", TabFile.GetString(i, szDamageChangeState_ChangeTrigger));
		//}

		ReadItem(pCfgNode->m_calcProbability,	szDamageChangeState_Probability,	IsRate);

		bool bArg2;
		ReadItem(bArg2,							szDamageChangeState_Passivity, CANEMPTY, NO);

		EHurtResult eArg1;
		ESpecialEventType eAnotherArg1;
		uint32 uResult = ReadItem(eArg1, eAnotherArg1, szDamageChangeState_TriggerEvent, CANEMPTY, 
			CTriggerEvent::m_smapEHurtResult, CTriggerEvent::m_smapESpecialEventType);
		switch(uResult)
		{
		case 0:
			pCfgNode->m_uTriggerEvent = 0;
			break;
		case 1:
			ESkillType eArg3;
			ReadItem(eArg3, szDamageChangeState_SkillType, CANEMPTY, eST_None, CTriggerEvent::m_smapESkillType);
			EAttackType eArg4;
			ReadItem(eArg4, szDamageChangeState_AttackType, CANEMPTY, eATT_None, CTriggerEvent::m_smapAttackType);
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
		ReadItem(strTriggerEvent,		szDamageChangeState_TriggerEvent);
		if (bArg2)
			pCfgNode->m_sTempVar = CSkillCfg::ms_mapPassivityTempVarType[strTriggerEvent];
		else
			pCfgNode->m_sTempVar = CSkillCfg::ms_mapTempVarType[strTriggerEvent];
		ReadItem(pCfgNode->m_calcTempValue,		szDamageChangeState_ChangeToValue);
		//ReadItem(pCfgNode->m_bApplyTempValue,	szDamageChangeState_ApplyTempValue,	CANEMPTY,	YES);
		ReadItem(pCfgNode, *(pCfgNode->m_pCancelableMagicEff),szTriggerState_CancelableMagicEff,	CANEMPTY);
		ReadItem(pCfgNode, *(pCfgNode->m_pFinalMagicEff),	szDamageChangeState_FinalMagicEff,	CANEMPTY);

		pCfgNode->m_bDecrease = pCfgNode->m_eDecreaseType <= eDST_IncreaseEnd ? false : true;

		if (pCfgNode->m_calcTime->IsSingleNumber() && (eAnotherArg1 != eSET_KillByInstaller && eAnotherArg1 != eSET_InstallerDie))
		{
			pCfgNode->SetNeedSaveToDB(pCfgNode->m_calcTime->GetDblValue());
		}

		////如果可撤销属性为否、持续时间为无限并初始次数为无限则该触发器状态在人物死亡后仍然不撤销
		//if(pCfgNode->m_eIconCancelCond == eICC_None
		//   && (pCfgNode->m_calcTime->IsSingleNumber() && pCfgNode->m_calcTime->GetIntValue() < 0)
		//   && (pCfgNode->m_calcInitialValue->IsSingleNumber() && pCfgNode->m_calcInitialValue->GetIntValue() < 0))
		//{
		//	//cout << pCfgNode->GetName() << endl;
		//	pCfgNode->m_bPersistent = true;
		//}
		//else
		//{
		//	pCfgNode->m_bPersistent = false;
		//}

		ReadItem(pCfgNode->m_bPersistent,			szState_Persistent,				CANEMPTY,		NO);

		CDamageChangeStateCfgServerSharedPtr* pCfgNodeSharedPtr = new CDamageChangeStateCfgServerSharedPtr(pCfgNode);
		InsertStringSharedPtrMap(m_mapCfg, pCfgNodeSharedPtr);
		InsertUint32SharedPtrMap(m_mapCfgById, pCfgNodeSharedPtr);
	}

	return true;
}

void CDamageChangeStateCfgServer::UnloadConfig()
{
	ClearMap(m_mapCfg);
}

CDamageChangeStateCfgServerSharedPtr& CDamageChangeStateCfgServer::Get(const string& damageChangeStateName)
{
	MapDamageChangeStateCfg::iterator mapCfgItr;
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

void CDamageChangeStateCfgServer::UpdateCfg(const string& strName)
{
	MapDamageChangeStateCfg::iterator it_name = m_mapCfg.find(strName);
	if(it_name != m_mapCfg.end())
	{
		CDamageChangeStateCfgServerSharedPtr* pOldCfg = it_name->second;
		CDamageChangeStateCfgServerSharedPtr* pNewCfg = new CDamageChangeStateCfgServerSharedPtr(new CDamageChangeStateCfgServer(*(pOldCfg->get())));
		m_mapCfg.erase(it_name);
		CfgChk::InsertStringSharedPtrMap(m_mapCfg, pNewCfg);
		MapDamageChangeStateCfgById::iterator it_id2 = m_mapCfgById.find(pOldCfg->get()->GetId());
		if(it_id2 != m_mapCfgById.end())
		{
			m_mapCfgById.erase(it_id2);
			CfgChk::InsertUint32SharedPtrMap(m_mapCfgById, pNewCfg);
		}

		delete pOldCfg;
	}
}

const CMagicEffServerSharedPtr& CDamageChangeStateCfgServer::GetFinalMagicEff()const
{
	return m_pFinalMagicEff->Get();
}

const CMagicEffServerSharedPtr& CDamageChangeStateCfgServer::GetCancelableMagicEff()const
{
	return m_pCancelableMagicEff->Get();
}
