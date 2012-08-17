#include "stdafx.h"
#include "CTriggerableState.h"
#include "CSkillInstServer.h"
#include "CAllStateMgr.h"
#include "CFighterDictator.h"
#include "CCfgCalc.inl"
#include "CTriggerEvent.h"
#include "COtherStateMgr.inl"
#include "BaseHelper.h"
#include "FighterProperty.inl"
#include "DebugHelper.h"
#include "CGenericTarget.h"
#include "CMagicEffDataMgrServer.h"









CTriggerableStateServer::CTriggerableStateServer(CSkillInstServer *pSkillInst, CFighterDictator* pInstaller,
	uint32 uAccumulate, int32 iTime, int32 iRemainTime, float fProbability)
:CBaseStateServer(pSkillInst, pInstaller, iTime, iRemainTime)
, m_uGrade(pSkillInst->GetSkillLevel())
, m_uAccumulate(uAccumulate)
, m_fProbability(fProbability)
, m_bEndByCount(false)
, m_bEndByTime(false)
, m_bCancellationDone(false)
, m_bTriggering(false)
, m_pCancelEffData(NULL)
{
	CreateInst(pSkillInst);
	m_pSkillInst->AddRef();			//可触发状态语义上不保存改变的值，因此这里不需要CreateTransfer()；否则则需要
	SetInstaller(pInstaller);
}




bool CTriggerableStateServer::Replace(CSkillInstServer *pSkillInst, CFighterDictator* pInstaller)
{
	m_uAccumulate = 0;						//uAccumulate的更新

#ifdef COUT_STATE_INFO
	cout << "状态替换\n";
#endif

	CancelDo();

	if(class_cast<CTriggerableStateCfgServer*>(GetCfg())->GetTriggerEvent() == eSET_InstallerDie)
	{
		//因为itrPTS->second是要删除的，所以不需要再执行一次Detach，否则反而可能会因为m_pInstaller析构而报错
		CFighterDictator* pFighter = GetInstaller();
		if(pFighter)
		{
			pFighter->Detach(this, eCE_Die);
		}
		pInstaller->Attach(this, eCE_Die);
	}

	m_pSkillInst->DelRef();
	DeleteInst();							//更新m_pCancelableInst


	//原则上这里需要执行构造函数里的一切功能，除了静态不变的部分（比如换模型、配置表）以及一些可以简化的步骤（比如删除再安装特效可以简化成一次改变特效）


	m_pSkillInst = pSkillInst;
	CreateInst(pSkillInst);					//更新m_pCancelableInst

	m_pSkillInst->AddRef();					//m_pSkillInst的更新
	ChangeInstaller(pInstaller);			//m_pInstaller的更新
	m_uGrade = pSkillInst->GetSkillLevel();	//m_uGrade的更新
	m_bEndByCount = false;					//m_bEndByCount的更新
	m_bEndByTime = false;					//m_bEndByTime的更新
	m_bCancellationDone = false;			//m_bCancellationDone的更新
	m_bTriggering = false;


	//StartDo();								//m_fProbability的更新
	RefreshTime();							//m_iTime、m_iRemainTime的更新
	GetAllMgr()->OnSetState(class_cast<CTriggerableStateCfgServer*>(GetCfg())->GetId(), m_uDynamicId, 1, m_iTime,
		m_iRemainTime, m_pSkillInst->GetSkillLevel(), GetInstallerID());

	StartDo();								//m_fProbability的更新
	return true;
}

//void CTriggerableStateServer::Start(bool bFromDB)			//只在新建魔法状态对象时执行一次
//{
//#ifdef COUT_STATE_INFO
//	cout << "新建魔法状态并开始计时\n";
//#endif
//
//	StartTime(bFromDB);
//	StartDo(bFromDB);
//}

void CTriggerableStateServer::StartDo(bool bFromDB)
{
	GetAllMgr()->ChangeSizeRate(GetCfg(), 1);
	if(!bFromDB)
	{
		m_fProbability = float(class_cast<CTriggerableStateCfgServer*>(GetCfg())->GetProbability()->GetDblValue(GetInstaller(), m_pSkillInst->GetLevelIndex()));
	}
	DoCancelableMagicEff(bFromDB);
}

void CTriggerableStateServer::CancelDo()
{
	if(m_bCancellationDone) return;
	m_bCancellationDone = true;

	GetAllMgr()->ChangeSizeRate(GetCfg(), -1);
	CancelCancelableMagicEff();
}

void CTriggerableStateServer::StartTime(bool bFromDB)
{
	if(!bFromDB)
	{
		m_iTime = class_cast<CTriggerableStateCfgServer*>(GetCfg())->GetTime()->GetIntValue(GetInstaller(), GetOwner(), m_pSkillInst->GetLevelIndex());

		if (GetInstaller() != GetOwner() && (m_iTime != -1))
		{
			switch(class_cast<CTriggerableStateCfgServer*>(GetCfg())->GetDecreaseType())
			{
			case eDST_Control:
				m_iTime = uint32(m_iTime * GetOwner()->m_ControlDecreaseTimeRate.Get(GetOwner()) + 0.5f);
				break;
			case eDST_Pause:
				m_iTime = uint32(m_iTime * GetOwner()->m_PauseDecreaseTimeRate.Get(GetOwner()) + 0.5f);
				break;
			case eDST_Crippling:
				m_iTime = uint32(m_iTime * GetOwner()->m_CripplingDecreaseTimeRate.Get(GetOwner()) + 0.5f);
				break;
			case eDST_Special:
				m_iTime = uint32(m_iTime * GetOwner()->m_SpecialDecreaseTimeRate.Get(GetOwner()) + 0.5f);
				break;
			default:
				break;
			}
		}
	}

	if(m_iTime < -1)
	{
		stringstream ExpStr;
		ExpStr << "触发器状态的作用时间不能小于-1：" << m_iTime << endl;
		GenErr("触发器状态的作用时间不能小于-1", ExpStr.str());
		return;
	}
	if(!bFromDB)
	{
		m_iRemainTime = m_iTime;
	}

	m_uStartTime = GetOwner()->GetDistortedFrameTime();

	m_iInitialValue = class_cast<CTriggerableStateCfgServer*>(GetCfg())->GetInitialValue()->GetIntValue(GetInstaller(), m_pSkillInst->GetLevelIndex());
	if(m_iInitialValue < -1)
	{
		stringstream ExpStr;
		ExpStr << "触发器状态的作用次数不能小于-1：" << m_iInitialValue << endl;
		GenErr("触发器状态的作用时间不能小于-1", ExpStr.str());
	}

	if (m_iRemainTime >= 0)
	{
#ifdef COUT_STATE_INFO
		cout << "创建时间为" << m_iRemainTime << "秒的计时器\n";
#endif 

		GetOwner()->RegistDistortedTick(this, m_iRemainTime * 1000);
	}
};



void CTriggerableStateServer::RefreshTime()
{
#ifdef COUT_STATE_INFO
	cout << "刷新时间\n";
#endif

	GetOwner()->UnRegistDistortedTick(this);
	StartTime();
}

void CTriggerableStateServer::OnTick()
{
	CancelDo();

	if(m_bEndByCount)
	{
#ifdef COUT_STATE_INFO
		cout << "因触发上限到了而删除自己\n";
#endif
	}
	else
	{
#ifdef COUT_STATE_INFO
		cout << "因时间到了而删除自己\n";
#endif

		m_bEndByTime = true;
	}


	//这里的DoFinalMagicEff() == true 时表示最终效果把目标杀死了，则（非永久）状态也消失了
	//注意，永久状态（即class_cast<CTriggerableStateCfgServer*>(GetCfg())->GetPersistent() == true）因为时间为无限而根本不可能进这里来，因此不需要做判断
	//CTriggerableStateCfgServer* pCfg = class_cast<CTriggerableStateCfgServer*>(GetCfg());
	//if (pCfg->GetCancelableMagicEff())
	//	Ast(!pCfg->GetPersistent());
	if(DoFinalMagicEff()) return;

	//m_bEndByCount的值和m_bEndByTime的值在DoFinalMagicEff()中可能发生变化，所以需要再判断一遍
	if(m_bEndByCount || m_bEndByTime)
	{
		DeleteSelf();
	}
}

pair<bool, bool> CTriggerableStateServer::MessageEvent(uint32 uEventID, CFighterDictator * pTrigger)
{
	CGenericTarget GenTrigger(pTrigger);
	return MessageEvent(uEventID, &GenTrigger);
}

CTriggerableStateServer::~CTriggerableStateServer()
{
	SafeDelete(m_pCancelEffData);

	DeleteInst();
	m_pSkillInst->DelRef();
	//需要检测这里是否能进入
}

void CTriggerableStateServer::OnCOREvent(CPtCORSubject* pSubject, uint32 uEvent,void* pArg)
{
	switch (uEvent) 
	{
	case eCE_Die:
		{
			CFighterDictator* pTrigger = class_cast<CFighterDictator *>(pSubject);
			if(pTrigger == GetInstaller())
			{
				CGenericTarget GenTrigger(pTrigger);
				MessageEvent(CTriggerEvent::GetID(eSET_InstallerDie, false), &GenTrigger);
			}
			else if(pTrigger == GetOwner())
			{
				if(static_cast<CFighterDictator *>(pArg) == GetInstaller())
				{
					CGenericTarget GenTrigger(pTrigger);
					MessageEvent(CTriggerEvent::GetID(eSET_KillByInstaller, false), &GenTrigger);
				}
			}
			break;
		}
	case eCE_Offline:
		//ClearSource();
		//m_pInstaller = NULL;
		SetInstaller(NULL);
		break;
	}
}

void CTriggerableStateServer::SetInstaller(CFighterDictator* pInstaller)
{
	//m_pInstaller = pInstaller;
	if(pInstaller)
	{
		m_uInstallerID = pInstaller->GetEntityID();
		pInstaller->Attach(this, eCE_Offline);
	}
	else
	{
		m_uInstallerID = 0;
	}
}

void CTriggerableStateServer::ChangeInstaller(CFighterDictator* pInstaller)
{
	if (pInstaller != GetInstaller())
	{
		ClearInstaller();
		SetInstaller(pInstaller);
	}
}

void CTriggerableStateServer::ClearInstaller()
{
	CFighterDictator* pInstaller = GetInstaller();
	if(pInstaller)
	{
		pInstaller->Detach(this, eCE_Offline);
		SetInstaller(NULL);
	}
}



void CTriggerableStateServer::CreateInst(CSkillInstServer* pSkillInst)
{
	//if (class_cast<CTriggerableStateCfgServer*>(GetCfg())->GetCancelableMagicEff())
	//{
	//	m_pCancelableInst = new CMagicStateCascadeDataMgr(pSkillInst, this->GetCfg(), eMSET_TriggerableCancellable);
	//	m_pCancelableInst->SetIsCalc(false);
	//}
}

void CTriggerableStateServer::DeleteInst()
{
	//if(m_pCancelableInst)
	//{
	//	m_pCancelableInst->DelRef();
	//	m_pCancelableInst = NULL;
	//}
}

//uint32 CTriggerableStateServer::GetDynamicId()
//{
//	return class_cast<CTriggerableStateCfgServer*>(GetCfg())->GetId();
//}

void CTriggerableStateServer::SetAccumulate(uint32 uValue)
{
	m_uAccumulate = uValue;
}



