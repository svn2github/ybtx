#include "stdafx.h"
#include "CAllStateMgr.h"
#include "CTriggerState.h"
#include "CTriggerStateCfg.h"
#include "CMagicEffServer.h"
#include "CSkillInstServer.h"
#include "CQuickRand.h"
#include "CFighterDictator.h"
#include "CTriggerEvent.h"
#include "CGenericTarget.h"
#include "CCfgCalc.inl"
#include "CTriggerStateMgr.h"
#include "ErrLogHelper.h"
#include "BaseHelper.h"
#include "CConnServer.h"
#include "DebugHelper.h"
#include "CMagicEffDataMgrServer.h"

using namespace std;














bool CTriggerStateServer::DoCancelableMagicEff(bool bFromDB)
{
	CTriggerStateMgrServer* pTempMgr = m_pMgr;
	CTriggerStateCfgServerSharedPtr pCfg = GetCfgSharedPtr();
	const CMagicEffServerSharedPtr& pMagicEff = pCfg->GetCancelableMagicEff();
	if (pMagicEff)
	{
#ifdef COUT_STATE_INFO
		cout << "执行可撤销魔法效果一次：" << pMagicEff->GetName() << endl;
#endif

		if(bFromDB) m_pSkillInst->SetForbitSetupSavedState(true);
		m_pSkillInst->SetStateId(GetCfgSharedPtr()->GetId());
		m_pCancelEffData = new CMagicEffDataMgrServer(GetInstaller());
		pMagicEff->Do(m_pSkillInst, GetInstaller(), GetOwner(), m_pCancelEffData);
		//以下这句在以上的Do可能导致人物死亡时要取消异常变为逐层函数返回
		if(!pTempMgr->ExistState(pCfg->GetName())) 
		{
			//stringstream str;
			//str << "触发器魔法状态 " << pCfg->GetName() << " 的拥有者在执行可撤销魔法效果 " << pMagicEff->GetName() << " 中死亡";
			//GenErr("触发器魔法状态的拥有者在执行可撤销魔法效果中死亡", str.str());
			return true;
		}
		if(bFromDB) m_pSkillInst->SetForbitSetupSavedState(false);
		m_pSkillInst->SetStateId(0);
	}
	return false; 
}

void CTriggerStateServer::CancelCancelableMagicEff()
{
	//避免自己被删后无法找到自己而预留的局部变量
	CTriggerStateMgrServer* pTempMgr = m_pMgr;
	CTriggerStateCfgServerSharedPtr pCfg = GetCfgSharedPtr();

	const CMagicEffServerSharedPtr& pMagicEff = pCfg->GetCancelableMagicEff();
	if (pMagicEff)				//可撤销魔法效果
	{
#ifdef COUT_STATE_INFO
		cout << "取消可撤销魔法效果一次：" << pMagicEff->GetName() << endl;
#endif

		m_pSkillInst->SetStateId(GetCfgSharedPtr()->GetId());
		pMagicEff->Cancel(m_pSkillInst, GetOwner(), m_pCancelEffData);
		//以下这句在以上的Do可能导致人物死亡时要取消异常变为逐层函数返回
		if(!pTempMgr->ExistState(pCfg->GetName())) 
		{
			stringstream str;
			str << "触发器魔法状态 " << pCfg->GetName() << " 的拥有者在撤销可撤销魔法效果 " << pMagicEff->GetName() << " 中死亡";
			GenErr("触发器魔法状态的拥有者在撤销可撤销魔法效果中死亡", str.str());
		}
		m_pSkillInst->SetStateId(0);
	}
}









//以下是CTriggerStateServer的操作
CTriggerStateServer::CTriggerStateServer(CSkillInstServer *pSkillInst, CFighterDictator* pInstaller,
	TTriggerableStateMgrServer<CTriggerStateCfgServer, CTriggerStateServer>* pMgr,
	const CTriggerStateCfgServerSharedPtr&	pCfg, uint32 uAccumulate, int32 iTime, int32 iRemainTime, float fProbability)
: CTriggerableStateServer(pSkillInst, pInstaller, uAccumulate, iTime, iRemainTime, fProbability)
, m_pMgr(class_cast<CTriggerStateMgrServer*>(pMgr))
, m_pStateEventBundle(NULL)
, m_pCfg(new CTriggerStateCfgServerSharedPtr(pCfg))
{
	if(!GetCfgSharedPtr()->GetModelStr().empty()) GetAllMgr()->SetModelStateId(GetCfgSharedPtr()->GetId());
}

CTriggerStateServer::~CTriggerStateServer()
{
	//是否考虑把ClearSource()的调入语句全部移入这里？

	GetOwner()->UnRegistDistortedTick(this);

#ifdef COUT_STATE_INFO
	cout << "删除TriggerState " << GetCfgSharedPtr()->GetName() << endl;
#endif

	if(!GetCfgSharedPtr()->GetModelStr().empty()) GetAllMgr()->ClearModelStateId();
	SafeDelete(m_pCfg);
}

pair<bool, bool> CTriggerStateServer::MessageEvent(uint32 uEventID, CGenericTarget * pTrigger)
{
	if(m_bEndByTime || m_bEndByCount || m_bTriggering) return make_pair(false, false);

	//避免自己被删后无法找到自己而预留的局部变量
	CTriggerStateMgrServer* pTempMgr = m_pMgr;
	CTriggerStateCfgServerSharedPtr pCfg = GetCfgSharedPtr();

	CSkillInstServer * pInst = NULL;
	SQR_TRY
	{
		CMagicEffServerSharedPtr pMagicEff;

		if (int32(m_uAccumulate) < m_iInitialValue || m_iInitialValue < 0)
		{
			//触发几率现在触发时取值，以前是安装状态时取值
			m_fProbability = float(GetCfgSharedPtr()->GetProbability()->GetDblValue(GetInstaller(), m_pSkillInst->GetLevelIndex()));

			if (CQuickRand::Rand1(0.0f, 1.0f) < m_fProbability)
			{
				pMagicEff = pCfg->GetTriggerEff();

				if(pMagicEff)
				{
					CGenericTarget target; 
					switch(pCfg->GetChangedTarget())
					{
					case eTC_Trigger:
						if(pTrigger->GetType() == eTT_Fighter)
							target.SetFighter(pTrigger->GetFighter());
						else if(pTrigger->GetType() == eTT_Position)
							target.SetPos(pTrigger->GetPos());				
						break;
					case eTC_TargetOfTrigger:
						target.SetFighter(class_cast<CFighterDictator*>(pTrigger->GetFighter()->GetLockingTarget()));
						break;
					case eTC_Installer:
						target.SetFighter(GetInstaller());
						break;
					case eTC_Self:
						target.SetFighter(GetOwner());
						break;
					case eTC_TargetOfSelf:
						target.SetFighter(class_cast<CFighterDictator*>(GetOwner()->GetTarget()));
						break;
					case eTC_SelfCorpse:
						target.SetFighter(GetOwner());
						break;
					default:
						break;
					}

					if(target.GetType() == eTT_Position 
						|| (target.GetType() == eTT_Fighter && target.GetFighter() != NULL))	// 触发者的目标有可能为NULL，所以这判断一下
					{
#ifdef COUT_STATE_INFO
						cout << "[ << pCfg->GetName() << ]触发效果[" << pMagicEff->GetName() << "]\n";
#endif
						pInst = CSkillInstServer::CreatePartition(m_pSkillInst);
						if(pCfg->GetChangedTarget() == eTC_SelfCorpse)
							pInst->SetTargetAliveCheckIsIgnored(true);
						else
							pInst->SetTargetAliveCheckIsIgnored(false);		//暂时做成触发器都对死亡目标不可用（目标死亡能执行的操作除外）

						pInst->SetTrigger(true);
						m_bTriggering = true;
						pMagicEff->Do(pInst, GetOwner(), &target);

						pInst->DestroyPartition();
						pInst = NULL;

						if(pTempMgr->m_mapState.find(pCfg.get()) == pTempMgr->m_mapState.end()) 
						{
							return make_pair(true, true);
						}
						m_bTriggering = false;
					}
				}

				++m_uAccumulate;
				if ((int32)m_uAccumulate >= m_iInitialValue && m_iInitialValue >= 0)
				{
#ifdef COUT_STATE_INFO
				cout << "因触发次数到了而准备删除自己\n";
#endif

					//这里改成为实现最后触发的触发器或伤害效果而使用的异步撤销
					//DeleteSelf();
					//return make_pair(true, true);
					m_bEndByCount = true;
					GetOwner()->RegistDistortedTick(this, 0);
					return make_pair(false, true);
				}
				else
				{
					return make_pair(false, true);
				}
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

void CTriggerStateServer::DeleteSelf()
{
	PrepareDeleteSelf();
	DeleteItr();
	//以下为删除自身的操作，注意这句必须放在最末尾
	delete this;
}

void CTriggerStateServer::DeleteSelfExceptItr()
{
	PrepareDeleteSelf();
	//以下为删除自身的操作，注意这句必须放在最末尾
	delete this;
}


void CTriggerStateServer::PrepareDeleteSelf()
{
	//CancelDo();

	//GetOwner()->UnRegistDistortedTick(this);

	if(GetCfgSharedPtr()->GetTriggerEvent() == eSET_InstallerDie)
	{
		CFighterDictator* pFighter = GetInstaller();
		if(pFighter)
		{
			//因为itrPTS->second是要删除的，所以不需要再执行一次Detach，否则反而可能会因为m_pInstaller析构而报错
			pFighter->Detach(this, eCE_Die);
		}
	}
	else if(GetCfgSharedPtr()->GetTriggerEvent() == eSET_KillByInstaller)
	{
		if(m_pMgr && m_pMgr->m_pOwner)
		{
			m_pMgr->m_pOwner->Detach(this, eCE_Die);
		}
	}
	else
	{
		//自定义观察者模式Detach
		SQR_TRY
		{
			if (m_listStateItr != m_pStateEventBundle->m_listState.end()) m_pStateEventBundle->m_listState.erase(m_listStateItr);
		}
		SQR_CATCH(exp)
		{
			stringstream sErr;
			sErr << "（m_pStateEventBundle->m_listState.erase(m_listStateItr)失败：m_pStateEventBundle->m_listState.size() = " << m_pStateEventBundle->m_listState.size()
				<< "。在触发器状态[" << GetCfgSharedPtr()->GetName() << "]）";
				exp.AppendMsg(sErr.str().c_str());
			SQR_THROW(exp);
		}
		SQR_TRY_END;
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

void CTriggerStateServer::DeleteItr()
{
	if (m_mapStateItr != m_pMgr->m_mapState.end()) m_pMgr->m_mapState.erase(m_mapStateItr);
}







CFighterDictator* CTriggerStateServer::GetOwner()
{
	return m_pMgr->m_pOwner;
}

CAllStateMgrServer* CTriggerStateServer::GetAllMgr()
{
	return m_pMgr->GetAllMgr();
}

CBaseStateCfgServer* CTriggerStateServer::GetCfg()
{
	return GetCfgSharedPtr().get();
}

CTriggerStateCfgServerSharedPtr& CTriggerStateServer::GetCfgSharedPtr()const
{
	return *m_pCfg;
}

