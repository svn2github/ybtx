#include "stdafx.h"
#include "CAllStateMgr.h"
#include "CMagicEffServer.h"
#include "CGenericTarget.h"
#include "CQuickRand.h"
#include "CFighterDictator.h"
#include "CTempVarServer.h"
#include "CSkillInstServer.h"
#include "CCfgCalc.inl"
#include "CStateCondFunctor.h"
#include "CCumuliTriggerState.h"
#include "CCumuliTriggerStateCfg.h"
#include "CCumuliTriggerStateMgr.h"
#include "ErrLogHelper.h"
#include "BaseHelper.h"
#include "CConnServer.h"
#include "DebugHelper.h"
#include "CMagicEffDataMgrServer.h"

using namespace std;


pair<bool, bool> CCumuliTriggerStateServer::MessageEvent(uint32 uEventID, CGenericTarget * pTrigger)
{
	if(m_bEndByTime || m_bEndByCount || m_bTriggering) return make_pair(false, false);

	//避免自己被删后无法找到自己而预留的局部变量
	CCumuliTriggerStateMgrServer* pTempMgr = m_pMgr;
	CCumuliTriggerStateCfgServerSharedPtr pCfg = GetCfgSharedPtr();

	CSkillInstServer * pInst = NULL;
	SQR_TRY
	{
		if(m_iInitialValue <= 0)
		{
			GenErr("积累触发状态的m_iInitialValue必须为大于0");
		}

		CMagicEffServerSharedPtr pMagicEff;
		if (int32(m_uAccumulate) < m_iInitialValue)
		{
			//触发几率现在触发时取值，以前是安装状态时取值
			m_fProbability = float(GetCfgSharedPtr()->GetProbability()->GetDblValue(GetInstaller(), m_pSkillInst->GetLevelIndex()));

			if (CQuickRand::Rand1(0.0f, 1.0f) < m_fProbability)
			{
				int32 OldValue = GetOwner()->GetTempVarMgr()->GetVarValue(pCfg->GetTempVar());
				uint32 AbsValueChanged = abs(OldValue);// abs(NewValue - OldValue);
				m_uAccumulate += AbsValueChanged;
				uint32 uTriggerCount = m_uAccumulate / (uint32)m_iInitialValue;
				int32 iMaxNumInSingleEvent = GetCfgSharedPtr()->GetMaxNumInSingleEvent()->GetIntValue(GetInstaller(), m_pSkillInst->GetLevelIndex());

				//cout << "cu " << m_uAccumulate << " threshold " << (uint32)m_iInitialValue << endl;
				if(iMaxNumInSingleEvent > 0)
					uTriggerCount = min(uint32(iMaxNumInSingleEvent), uTriggerCount);

				m_uAccumulate %= (uint32)m_iInitialValue; 

				pMagicEff = pCfg->GetTriggerEff();
				if(pMagicEff && uTriggerCount > 0)
				{
					//CGenericTarget target; 
					//switch(pCfg->GetChangedTarget())
					//{
					//case eTC_Trigger:
					//	if(pTrigger->GetType() == eTT_Fighter)
					//		target.SetFighter(pTrigger->GetFighter());
					//	else if(pTrigger->GetType() == eTT_Position)
					//		target.SetPos(pTrigger->GetPos());				
					//	break;
					//case eTC_TargetOfTrigger:
					//	target.SetFighter(class_cast<CFighterDictator*>(pTrigger->GetFighter()->GetLockingTarget()));
					//	break;
					//case eTC_Installer:
					//	target.SetFighter(GetInstaller());
					//	break;
					//case eTC_Self:
					//	target.SetFighter(GetOwner());
					//	break;
					//case eTC_TargetOfSelf:
					//	target.SetFighter(class_cast<CFighterDictator*>(GetOwner()->GetTarget()));
					//	break;
					//case eTC_SelfCorpse:
					//	target.SetFighter(GetOwner());
					//	break;
					//default:
					//	break;
					//}

					//if(target.GetType() == eTT_Position 
					//	|| (target.GetType() == eTT_Fighter && target.GetFighter() != NULL))	// 触发者的目标有可能为NULL，所以这判断一下
					//{
#ifdef COUT_STATE_INFO
						cout << "[ << pCfg->GetName() << ]触发效果[" << pMagicEff->GetName() << "]\n";
#endif
						m_bTriggering = true;

						for(uint i = 0; i < uTriggerCount; ++i)
						{
							pInst = CSkillInstServer::CreatePartition(m_pSkillInst);
							if(pCfg->GetChangedTarget() == eTC_SelfCorpse)
								pInst->SetTargetAliveCheckIsIgnored(true);
							else
								pInst->SetTargetAliveCheckIsIgnored(false);		//暂时做成触发器都对死亡目标不可用（目标死亡能执行的操作除外）

							pInst->SetTrigger(true);
							//pMagicEff->Do(pInst, GetOwner(), &target);
							pMagicEff->Do(pInst, GetInstaller(), GetOwner());

							pInst->DestroyPartition();
							pInst = NULL;

							if(pTempMgr->m_mapState.find(pCfg.get()) == pTempMgr->m_mapState.end()) 
							{
								return make_pair(true, true);
							}
						}
						m_bTriggering = false;
					//}
				}

				return make_pair(false, true);
			}
		}
	}
	SQR_CATCH(exp)
	{
		m_bTriggering = false;
		if(pInst)
		{
			pInst->DestroyPartition();
			pInst = NULL;
		}
		stringstream sErr;
		sErr << "（在触发器状态[" << pCfg->GetName() << "]MessageEvent(" << uEventID << ")）";
		if(pTempMgr->m_mapState.find(pCfg.get()) == pTempMgr->m_mapState.end())
		{
			sErr << "（在技能[" << m_pSkillInst->GetSkillName() << "]）";
			exp.AppendMsg(sErr.str().c_str());
			//CConnServer* pConnServer = GetOwner()->GetConnection();
			//if (pConnServer)
			//	LogExp(exp, pConnServer);
			//else
			//	LogExp(exp);
		}
		else
		{
			sErr << "（状态对象已消失）";
			exp.AppendMsg(sErr.str().c_str());
			//LogExp(exp);
		}
		SQR_THROW(exp);
	}
	SQR_TRY_END;

	return make_pair(false, false);
}




//以下是CCumuliTriggerStateServer的操作
CCumuliTriggerStateServer::CCumuliTriggerStateServer(CSkillInstServer *pSkillInst, CFighterDictator* pInstaller,
	TTriggerableStateMgrServer<CCumuliTriggerStateCfgServer, CCumuliTriggerStateServer>* pMgr,
	const CCumuliTriggerStateCfgServerSharedPtr& pCfg, uint32 uAccumulate, int32 iTime, int32 iRemainTime, 
	float fProbability)
: CTriggerableStateServer(pSkillInst, pInstaller, 0, iTime, iRemainTime, fProbability)
, m_pMgr(class_cast<CCumuliTriggerStateMgrServer*>(pMgr))
, m_pStateEventBundle(NULL)
, m_pCfg(new CCumuliTriggerStateCfgServerSharedPtr(pCfg))
{
	if(!GetCfgSharedPtr()->GetModelStr().empty()) GetAllMgr()->SetModelStateId(GetCfgSharedPtr()->GetId());
	if(pInstaller)
		pInstaller->Attach(this, eCE_LevelChanged);
}


CCumuliTriggerStateServer::~CCumuliTriggerStateServer()
{
	CFighterDictator* pInstaller = GetInstaller();
	if(pInstaller)
		pInstaller->Detach(this, eCE_LevelChanged);
	//是否考虑把ClearSource()的调入语句全部移入这里？
	GetOwner()->UnRegistDistortedTick(this);

#ifdef COUT_STATE_INFO
	cout << "删除CumuliTriggerState " << GetCfgSharedPtr()->GetName() << endl;
#endif

	if(!GetCfgSharedPtr()->GetModelStr().empty()) GetAllMgr()->ClearModelStateId();
	SafeDelete(m_pCfg);
}

void CCumuliTriggerStateServer::ChangeInstaller(CFighterDictator* pInstaller)
{
	CFighterDictator* pOrinInstaller = GetInstaller();
	if(pOrinInstaller)
		pOrinInstaller->Detach(this, eCE_LevelChanged);
	if(pInstaller)
		pInstaller->Attach(this, eCE_LevelChanged);
	CTriggerableStateServer::ChangeInstaller(pInstaller);
}

void CCumuliTriggerStateServer::OnCOREvent(CPtCORSubject* pSubject, uint32 uEvent,void* pArg)
{
	switch (uEvent) 
	{
	case eCE_LevelChanged:
	{
		CFighterDictator* pInstaller = GetInstaller();
		if(pInstaller)
		{
			m_pSkillInst->InitSkillLevel(pInstaller->CppGetLevel());
			m_iInitialValue = class_cast<CCumuliTriggerStateCfgServer*>(GetCfg())->GetInitialValue()->GetIntValue(pInstaller, m_pSkillInst->GetLevelIndex());
		}
		break;
	}
	default:
		break;
	}
	CTriggerableStateServer::OnCOREvent(pSubject, uEvent, pArg);
}


void CCumuliTriggerStateServer::DeleteSelf()
{
	PrepareDeleteSelf();
	DeleteItr();
	//以下为删除自身的操作，注意这句必须放在最末尾
	delete this;
}

void CCumuliTriggerStateServer::DeleteSelfExceptItr()
{
	PrepareDeleteSelf();
	//以下为删除自身的操作，注意这句必须放在最末尾
	delete this;
}


void CCumuliTriggerStateServer::PrepareDeleteSelf()
{
	//GetOwner()->UnRegistDistortedTick(this);

	switch(GetCfgSharedPtr()->GetTriggerEvent())
	{
	case eSET_InstallerDie:
		{
			CFighterDictator* pFighter = GetInstaller();
			if(pFighter)
			{
				//因为itrPTS->second是要删除的，所以不需要再执行一次Detach，否则反而可能会因为m_pInstaller析构而报错
				pFighter->Detach(this, eCE_Die);
			}
		}
		break;
	case eSET_KillByInstaller:
		if(m_pMgr && m_pMgr->m_pOwner)
		{
			m_pMgr->m_pOwner->Detach(this, eCE_Die);
		}
		break;
	default:
		//自定义观察者模式Detach
		if (m_listStateItr != m_pStateEventBundle->m_listState.end())
			m_pStateEventBundle->m_listState.erase(m_listStateItr);
	}

	//调用取消魔法状态种类的模型和特效回调
	uint32 uId = GetCfgSharedPtr()->GetId();
	CAllStateMgrServer* pAllMgr = GetAllMgr();
	pAllMgr->OnDeleteState(uId, m_uDynamicId);
	pAllMgr->DelStateByDId(m_uDynamicId);


	//为ClearAll的中途ExistState等函数准备的（ExistState等函数要检测itr->second != NULL）
	if (m_mapStateItr != m_pMgr->m_mapState.end()) m_mapStateItr->second = NULL;

	pAllMgr->PrintFightInfo(false, GetInstallerID(), GetCfgSharedPtr().get());

	SetInstaller(NULL);
}

void CCumuliTriggerStateServer::DeleteItr()
{
	if (m_mapStateItr != m_pMgr->m_mapState.end()) m_pMgr->m_mapState.erase(m_mapStateItr);
}







CFighterDictator* CCumuliTriggerStateServer::GetOwner()
{
	return m_pMgr->m_pOwner;
}

CAllStateMgrServer* CCumuliTriggerStateServer::GetAllMgr()
{
	return m_pMgr->GetAllMgr();
}

CBaseStateCfgServer* CCumuliTriggerStateServer::GetCfg()
{
	return GetCfgSharedPtr().get();
}

CCumuliTriggerStateCfgServerSharedPtr& CCumuliTriggerStateServer::GetCfgSharedPtr()const
{
	return *m_pCfg;
}
