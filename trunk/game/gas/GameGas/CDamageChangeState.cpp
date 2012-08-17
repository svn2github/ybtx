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
#include "CDamageChangeState.h"
#include "CDamageChangeStateCfg.h"
#include "CDamageChangeStateMgr.h"
#include "ErrLogHelper.h"
#include "BaseHelper.h"
#include "CConnServer.h"
#include "DebugHelper.h"
#include "CMagicEffDataMgrServer.h"

using namespace std;

//inline int32 sgn(int32 value)
//{
//	if(value > 0)
//		return 1;
//	else if(value < 0)
//		return -1;
//	else
//		return 0;
//}









//以下是CDamageChangeStateServer的操作
CDamageChangeStateServer::CDamageChangeStateServer(CSkillInstServer *pSkillInst, CFighterDictator* pInstaller,
	TTriggerableStateMgrServer<CDamageChangeStateCfgServer, CDamageChangeStateServer>* pMgr,
	const CDamageChangeStateCfgServerSharedPtr& pCfg, uint32 uAccumulate, int32 iTime, int32 iRemainTime, 
	float fProbability)
: CTriggerableStateServer(pSkillInst, pInstaller, 0, iTime, iRemainTime, fProbability)
, m_pMgr(class_cast<CDamageChangeStateMgrServer*>(pMgr))
, m_pStateEventBundle(NULL)
, m_pCfg(new CDamageChangeStateCfgServerSharedPtr(pCfg))
{
	if(!GetCfgSharedPtr()->GetModelStr().empty()) GetAllMgr()->SetModelStateId(GetCfgSharedPtr()->GetId());
}


CDamageChangeStateServer::~CDamageChangeStateServer()
{
	//是否考虑把ClearSource()的调入语句全部移入这里？
	GetOwner()->UnRegistDistortedTick(this);

#ifdef COUT_STATE_INFO
	cout << "删除DamageChangeState " << GetCfgSharedPtr()->GetName() << endl;
#endif

	if(!GetCfgSharedPtr()->GetModelStr().empty()) GetAllMgr()->ClearModelStateId();
	SafeDelete(m_pCfg);
}





pair<bool, bool> CDamageChangeStateServer::MessageEvent(uint32 uEventID, CGenericTarget * pTrigger)
{
	if(m_bEndByTime || m_bEndByCount) return make_pair(false, false);

	//cout << "伤害变更状态触发事件：" << uEventID << "\n";
	//伤害吸收的触发事件只能为被伤害（14）
	//减蓝吸收的触发事件只能为被减蓝，其余依此类推
	CDamageChangeStateCfgServerSharedPtr& pCfg = GetCfgSharedPtr(); 

	SQR_TRY
	{
		if (int32(m_uAccumulate) < m_iInitialValue || m_iInitialValue < 0)
		{
			//触发几率现在触发时取值，以前是安装状态时取值
			m_fProbability = float(GetCfgSharedPtr()->GetProbability()->GetDblValue(GetInstaller(), GetOwner(), m_pSkillInst->GetLevelIndex()));

			if (CQuickRand::Rand1(0.0f, 1.0f) < m_fProbability)
			{
				//AddValue(pSkillInst->GetDamaSetSetupge());

				//uint32 beDamaged = GetOwner()->GetTempVarMgr()->GetBeDamaged();
				int32 OldValue = GetOwner()->GetTempVarMgr()->GetVarValue(pCfg->GetTempVar());

				if(pTrigger->GetType() != eTT_Fighter)
				{
					stringstream str;
					str << "攻击者不是一个人！他不是一个人！\n";
					GenErr(str.str());
					//return make_pair(false, false);
				}
				else if(pTrigger->GetFighter() && 
					pTrigger->GetFighter()->GetTempVarMgr()
					->GetVarValue(CTempVarMgrServer::GetPassVarName(pCfg->GetTempVar()))
					!= OldValue)
				{
					//如果这里抛异常，则需要考虑严格管理临时变量的生存期，比如引入技能流程线回溯机制以及多临时变量管理
					stringstream str;
					str << "临时变量[" << pCfg->GetTempVar() << "]主动值" << (pTrigger->GetFighter()->GetTempVarMgr()
						->GetVarValue(CTempVarMgrServer::GetPassVarName(pCfg->GetTempVar()))
						) << "与被动值" << OldValue << "数值不一致\n";
					GenErr("临时变量主动值与被动值数值不一致", str.str());
					//return make_pair(false, false);
				}

				int32 NewValue = pCfg->GetTempValue()->GetIntValue(GetInstaller(), GetOwner(), m_pSkillInst->GetLevelIndex());
				uint32 AbsValueChanged = abs(OldValue);// abs(NewValue - OldValue);

				AddValue(AbsValueChanged);

				//注意以下这句在超过初始值后只能使用一次
				//pSkillInst->SetDamage(((int32)m_uAccumulate > m_iInitialValue && m_iInitialValue >= 0) ? m_uAccumulate - m_iInitialValue : 0);
				//int32 ProcessedNewValue = ((int32)m_uAccumulate > m_iInitialValue && m_iInitialValue >= 0) ? NewValue + sgn(OldValue/* - NewValue*/) * (m_uAccumulate - m_iInitialValue) : NewValue;
				int32 ProcessedNewValue;
				if((int32)m_uAccumulate > m_iInitialValue && m_iInitialValue >= 0)
				{
					if(AbsValueChanged == 0)
					{
						//因为m_iInitialValue的值在本函数以上过程中不会发生改变，
						//从满足if (int32(m_uAccumulate) < m_iInitialValue || m_iInitialValue < 0)
						//到满足if ((int32)m_uAccumulate > m_iInitialValue && m_iInitialValue >= 0)
						//表示m_uAccumulate的值在两个if判断之间发生了改变，则唯一导致m_uAccumulate改变的AbsValueChanged不可能为0
						GenErr("恭喜您成功穿越到第10次元与第11次元宇宙的间隙\n");
						return make_pair(false, false);
					}
					ProcessedNewValue = NewValue + int32(int64(OldValue - NewValue) * int64(m_uAccumulate - m_iInitialValue) / int64(AbsValueChanged));
				}
				else
				{
					ProcessedNewValue = NewValue;
				}
				//if(pCfg->GetApplyTempValue())
				//{
				GetOwner()->GetTempVarMgr()->SetVarValue(pCfg->GetTempVar(), ProcessedNewValue);
				if (pCfg->GetTempVar() == "被伤害")
				{
					if (GetOwner()->GetTempVarMgr()->GetDamageChangeStateName() == "")
					{
						GetOwner()->GetTempVarMgr()->SetDamageChangeStateName(pCfg->GetName());
					}
					else
					{
						GetOwner()->GetTempVarMgr()->SetDamageChangeStateName("*$");
					}
				}
				
				if(pTrigger->GetFighter()) 
					pTrigger->GetFighter()->GetTempVarMgr()
					->SetVarValue(CTempVarMgrServer::GetPassVarName(pCfg->GetTempVar()), ProcessedNewValue);
				//}

#ifdef COUT_STATE_INFO
			cout << "[" << pCfg->GetName() << "]将[ " << pCfg->GetTempVar().c_str() << " ]由" << OldValue << "变更为" << ProcessedNewValue << endl;
#endif

				//if (pSkillInst->GetDamage() > 0)
				if (m_iInitialValue >= 0 && (int32)m_uAccumulate >= m_iInitialValue)
				{
#ifdef COUT_STATE_INFO
				cout << "因伤害吸收数值到了而准备删除自己\n";
#endif

					//这里的 DoFinalMagicEff() == true 时表示最终效果把目标杀死了，则（非永久）状态也消失了
					//注意：永久状态（即m_pCfg->GetPersistent() == true）因为变更上限为无限而根本不可能进这里来，因此不需要做判断
					if (GetCfgSharedPtr()->GetCancelableMagicEff())
						Ast(!GetCfgSharedPtr()->GetPersistent());
					//if(!DoFinalMagicEff())
					//{
					//	//这里重写一遍是避免DoFinalMagicEff里的GetTempVarMgr()->SetVarValue覆盖掉伤害临时变量的值
					//	//若将触发最终效果改成异步，则这段代码可能需要重新考虑放在!bRet的判断之外
					//	GetOwner()->GetTempVarMgr()->SetVarValue(pCfg->GetTempVar(), ProcessedNewValue);
					//	if(pTrigger->GetFighter()) 
					//		pTrigger->GetFighter()->GetTempVarMgr()
					//		->SetVarValue(CTempVarMgrServer::GetPassVarName(pCfg->GetTempVar()), ProcessedNewValue);

					//	//这里改成为实现最后触发的触发器或伤害效果而使用的异步撤销
					//	//DeleteSelf();
					//	//return make_pair(true, true);
					m_bEndByCount = true;
					GetOwner()->UnRegistDistortedTick(this);
					GetOwner()->RegistDistortedTick(this, 0);
					return make_pair(false, true);
					//}
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
		stringstream sErr;
		sErr << "（在伤害变更状态[" << pCfg->GetName() << "]MessageEvent(" << uEventID << ")）"
			<< "（在技能[" << m_pSkillInst->GetSkillName() << "]）";
		exp.AppendMsg(sErr.str().c_str());
		//CConnServer* pConnServer = GetOwner()->GetConnection();
		//if (pConnServer)
		//	LogExp(exp, pConnServer);
		//else
		//	LogExp(exp);
		SQR_THROW(exp);
	}
	SQR_TRY_END;

	return make_pair(false, false);
}

void CDamageChangeStateServer::DeleteSelf()
{
	PrepareDeleteSelf();
	DeleteItr();
	//以下为删除自身的操作，注意这句必须放在最末尾
	delete this;
}

void CDamageChangeStateServer::DeleteSelfExceptItr()
{
	PrepareDeleteSelf();
	//以下为删除自身的操作，注意这句必须放在最末尾
	delete this;
}


void CDamageChangeStateServer::PrepareDeleteSelf()
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

void CDamageChangeStateServer::DeleteItr()
{
	if (m_mapStateItr != m_pMgr->m_mapState.end()) m_pMgr->m_mapState.erase(m_mapStateItr);
}




bool CDamageChangeStateServer::DoCancelableMagicEff(bool bFromDB)
{
	CDamageChangeStateMgrServer* pTempMgr = m_pMgr;
	CDamageChangeStateCfgServerSharedPtr pCfg = GetCfgSharedPtr();
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
			//str << "伤害变更状态 " << pCfg->GetName() << " 的拥有者在执行可撤销魔法效果 " << pMagicEff->GetName() << " 中死亡";
			//GenErr("伤害变更状态的拥有者在执行可撤销魔法效果中死亡", str.str());
			return true;
		}
		if(bFromDB) m_pSkillInst->SetForbitSetupSavedState(false);
		m_pSkillInst->SetStateId(0);
	}
	return false;
}

void CDamageChangeStateServer::CancelCancelableMagicEff()
{
	//避免自己被删后无法找到自己而预留的局部变量
	CDamageChangeStateMgrServer* pTempMgr = m_pMgr;
	CDamageChangeStateCfgServerSharedPtr pCfg = GetCfgSharedPtr();

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
			str << "伤害变更状态 " << pCfg->GetName() << " 的拥有者在撤销可撤销魔法效果 " << pMagicEff->GetName() << " 中死亡";
			GenErr("伤害变更状态的拥有者在撤销可撤销魔法效果中死亡", str.str());
		}
		m_pSkillInst->SetStateId(0);
	}
}

bool CDamageChangeStateServer::DoFinalMagicEff()
{
	CDamageChangeStateMgrServer* pTempMgr = m_pMgr;
	CDamageChangeStateCfgServerSharedPtr pCfg = GetCfgSharedPtr();
	const CMagicEffServerSharedPtr& pMagicEff = pCfg->GetFinalMagicEff();
	if (pMagicEff)
	{
#ifdef COUT_STATE_INFO
		cout << "执行最终魔法效果一次：" << pMagicEff->GetName() << endl;
#endif

		pMagicEff->Do(m_pSkillInst, GetInstaller(), GetOwner());
		//以下这句在以上的Do可能导致人物死亡时要取消异常变为逐层函数返回
		if(!pTempMgr->ExistState(pCfg->GetName())) 
		{
			return true;
		}
	}
	return false;
}



CFighterDictator* CDamageChangeStateServer::GetOwner()
{
	return m_pMgr->m_pOwner;
}

CAllStateMgrServer* CDamageChangeStateServer::GetAllMgr()
{
	return m_pMgr->GetAllMgr();
}

CBaseStateCfgServer* CDamageChangeStateServer::GetCfg()
{
	return GetCfgSharedPtr().get();
}

CDamageChangeStateCfgServerSharedPtr& CDamageChangeStateServer::GetCfgSharedPtr()const
{
	return *m_pCfg;
}
