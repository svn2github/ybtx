#include "stdafx.h"
#include "CTriggerStateCfg.h"
#include "CTxtTableFile.h"
#include "LoadSkillCommon.h"
#include "CCfgColChecker.inl"
#include "CCfgCalc.inl"
#include "CTriggerEvent.h"
#include "ErrLogHelper.h"
#include "BaseHelper.h"
#include "TStringRef.inl"

namespace sqr
{
	extern const wstring PATH_ALIAS_CFG;
}


//初始化静态成员

CTriggerStateCfgServer::MapTriggerStateCfg CTriggerStateCfgServer::m_mapCfg;
CTriggerStateCfgServer::MapTriggerStateCfgById CTriggerStateCfgServer::m_mapCfgById;
//CTriggerStateCfgServer::MapCascadeType CTriggerStateCfgServer::m_mapCascadeType;
CTriggerStateCfgServerSharedPtr CTriggerStateCfgServer::ms_NULL;

bool CTriggerStateCfgServer::LoadConfig(const TCHAR* cfgFile)
{
	using namespace CfgChk;

	CreateMap(m_mapTargetChange, eTc_End, ("触发者"), ("安装者"), ("触发者的目标"), ("拥有者"), ("拥有者的目标"), ("拥有者尸体"));
	//InitMapCascadeType();

	CTxtTableFile TabFile;
	stringstream ExpStr;				//用于异常输出
	CTriggerStateCfgServer*	pCfgNode;

	SetTabFile(TabFile, "触发器状态");
	if (!TabFile.Load(PATH_ALIAS_CFG.c_str(), cfgFile)) return false;

	m_mapCfg.clear();

	for(int32 i=1; i<TabFile.GetHeight(); ++i)
	{
		SetLineNo(i);
		pCfgNode = new CTriggerStateCfgServer;
		pCfgNode->m_eGlobalType = eSGT_TriggerState;
		pCfgNode->m_uId = eSIC_TriggerState + i;

		ReadItem(pCfgNode->m_sName,				szTriggerState_Name);
		ReadItem(pCfgNode->m_eDecreaseType,		szTplState_DecreateType,		CANEMPTY,	eDST_Increase,	ms_mapDecreaseType);
		ReadItem(pCfgNode->m_bDispellable,			szTplState_Dispellable,			CANEMPTY,	YES);
		ReadItem(pCfgNode->m_eIconCancelCond,	szBaseState_CancelCond,			CANEMPTY,	eICC_None,		ms_mapIconCancelCond);
		//ReadItem(pCfgNode->m_eCascadeType,		szTriggerState_CascadeType,		m_mapCascadeType);
		ReadItem(pCfgNode->m_calcTime,			szTriggerState_Time,			GE,			-1);
		ReadItem(pCfgNode->m_calcInitialValue,	szTriggerState_InitialCount,	GE,			-1);
		ReadItem(pCfgNode->m_eTargetChange,		szTriggerState_ChangeTrigger,	CANEMPTY,	eTC_Self,		m_mapTargetChange);
		ReadItem(pCfgNode->m_calcProbability,	szTriggerState_Probability,		IsRate);

		bool bArg2;
		ReadItem(bArg2,							szTriggerState_Passivity,		CANEMPTY,	NO);

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
			{
				ESkillType eArg3;
				ReadItem(eArg3, szTriggerState_SkillType, CANEMPTY, eST_None, CTriggerEvent::m_smapESkillType);
				EAttackType eArg4;
				ReadItem(eArg4, szTriggerState_AttackType, CANEMPTY, eATT_None, CTriggerEvent::m_smapAttackType);
				pCfgNode->m_uTriggerEvent = CTriggerEvent::GetID(eArg1, bArg2, eArg3, eArg4);
			}
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

		ReadItem(pCfgNode, *(pCfgNode->m_pTriggerEff),		szTriggerState_TriggerEff,			CANEMPTY);
		ReadItem(pCfgNode, *(pCfgNode->m_pCancelableMagicEff),szTriggerState_CancelableMagicEff,	CANEMPTY);

		pCfgNode->m_bDecrease = pCfgNode->m_eDecreaseType <= eDST_IncreaseEnd ? false : true;

		if (pCfgNode->m_calcTime->IsSingleNumber() && (eAnotherArg1 != eSET_KillByInstaller && eAnotherArg1 != eSET_InstallerDie))
		{
			pCfgNode->SetNeedSaveToDB(pCfgNode->m_calcTime->GetDblValue());
		}
		////如果可撤销属性为否、持续时间为无限并初始次数为无限则该触发器状态在人物死亡后仍然不撤销
		//if(pCfgNode->m_eIconCancelCond == eICC_None
		//	&& (pCfgNode->m_calcTime->IsSingleNumber() && pCfgNode->m_calcTime->GetIntValue() < 0)
		//	&& (pCfgNode->m_calcInitialValue->IsSingleNumber() && pCfgNode->m_calcInitialValue->GetIntValue() < 0))
		//{
		//	//cout << pCfgNode->GetName() << endl;
		//	pCfgNode->m_bPersistent = true;
		//}
		//else
		//{
		//	pCfgNode->m_bPersistent = false;
		//}

		ReadItem(pCfgNode->m_bPersistent,			szState_Persistent,				CANEMPTY,		NO);

		SetItemTitle(szTriggerState_ChangeTrigger);

		CTriggerStateCfgServerSharedPtr* pCfgNodeSharedPtr = new CTriggerStateCfgServerSharedPtr(pCfgNode);
		InsertStringSharedPtrMap(m_mapCfg, pCfgNodeSharedPtr);
		InsertUint32SharedPtrMap(m_mapCfgById, pCfgNodeSharedPtr);
	}

	return true;
}

void CTriggerStateCfgServer::UnloadConfig()
{
	ClearMap(m_mapCfg);
}

void CTriggerStateCfgServer::UpdateCfg(const string& strName)
{
	MapTriggerStateCfg::iterator it_name = m_mapCfg.find(strName);
	if(it_name != m_mapCfg.end())
	{
		CTriggerStateCfgServerSharedPtr* pOldCfg = it_name->second;
		CTriggerStateCfgServerSharedPtr* pNewCfg = new CTriggerStateCfgServerSharedPtr(new CTriggerStateCfgServer(*(pOldCfg->get())));
		m_mapCfg.erase(it_name);
		CfgChk::InsertStringSharedPtrMap(m_mapCfg, pNewCfg);
		MapTriggerStateCfgById::iterator it_id2 = m_mapCfgById.find(pOldCfg->get()->GetId());
		if(it_id2 != m_mapCfgById.end())
		{
			m_mapCfgById.erase(it_id2);
			CfgChk::InsertUint32SharedPtrMap(m_mapCfgById, pNewCfg);
		}

		delete pOldCfg;
	}
}

CTriggerStateCfgServerSharedPtr& CTriggerStateCfgServer::Get(const string& triggerStateName)
{
	MapTriggerStateCfg::iterator mapCfgItr;
	mapCfgItr = m_mapCfg.find(triggerStateName);
	if (mapCfgItr == m_mapCfg.end())
	{
		stringstream ExpStr;
		ExpStr << "触发器魔法状态名不存在";
		CfgChk::GenExpInfo(ExpStr.str(), triggerStateName);
		return ms_NULL;
	}
	return *(mapCfgItr->second);
}

CTriggerStateCfgServer::CTriggerStateCfgServer()
:CTriggerableStateCfgServer(eSGT_TriggerState)
{
	m_pTriggerEff = new MagicEffServerStringRefer(this);
	m_pCancelableMagicEff = new MagicEffServerStringRefer(this);
}

CTriggerStateCfgServer::CTriggerStateCfgServer(const CTriggerStateCfgServer& cfg)
:CTriggerableStateCfgServer(cfg)
,m_eTargetChange(cfg.m_eTargetChange)
{
	string str;
	m_pTriggerEff = new MagicEffServerStringRefer(this);
	cfg.m_pTriggerEff->GetStringKey(str);
	m_pTriggerEff->SetStringKey(str);
	m_pCancelableMagicEff = new MagicEffServerStringRefer(this);
	cfg.m_pCancelableMagicEff->GetStringKey(str);
	m_pCancelableMagicEff->SetStringKey(str);
}

CTriggerStateCfgServer::~CTriggerStateCfgServer()
{
	SafeDelete(m_pTriggerEff);
	SafeDelete(m_pCancelableMagicEff);
}

const CMagicEffServerSharedPtr& CTriggerStateCfgServer::GetTriggerEff()const
{
	return m_pTriggerEff->Get();
}

const CMagicEffServerSharedPtr& CTriggerStateCfgServer::GetCancelableMagicEff()const
{
	return m_pCancelableMagicEff->Get();
}

//bool CTriggerStateCfgServer::InitMapCascadeType()					//建立群体叠加类型字符串到对应枚举类型的映射
//{
//	m_mapCascadeType["集中"] = eCT_Central;
//	m_mapCascadeType["分散"] = eCT_Decentral;
//
//	return true;
//}

