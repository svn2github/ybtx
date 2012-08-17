#pragma once
#include "CTriggerableStateMgr.h"
#include "CTriggerStateCfg.h"
#include "CCumuliTriggerStateCfg.h"
#include "CFighterDictator.h"
#include "CSkillInstServer.h"
#include "ErrLogHelper.h"
#include "CTriggerEvent.h"
#include "CAllStateMgr.h"
#include "CConnServer.h"
#include "DebugHelper.h"
#include "BaseHelper.h"

template<typename CfgType, typename StateType>
bool TTriggerableStateMgrServer<CfgType, StateType>::SetupState(CSkillInstServer* pSkillInst, const SharedPtrType& pCfg, CFighterDictator* pInstaller)
{
	if (!this->m_pOwner)
	{
		GenErr("未调用SetOwner设置指向目标本身的指针");
		//return false;
	}
	//typename CfgType::SharedPtrType& pCfg = CfgType::Get(name);

	//免疫计算
	CAllStateMgrServer* pAllMgr = this->GetAllMgr();
	if(pAllMgr->Immume(pCfg.get(), pSkillInst, pInstaller))
	{
		return false;
	}

	//按抗性计算
	if(pAllMgr->DecreaseStateResist(pCfg.get())) return false;

	//变形状态替换处理
	if(!pAllMgr->ReplaceModelState(pCfg.get())) return false;

	typename MapState::iterator mapStateItr = this->m_mapState.find(pCfg.get());

	if (mapStateItr == this->m_mapState.end())
	{
#ifdef COUT_STATE_INFO
		cout << this->m_pOwner->GetEntityID() << " 安装" << pCfg->GetTypeName() << "：" << pCfg->GetName() << endl;
#endif

		//安装一个新的魔法状态时才会打印战斗信息, 叠加或替换时不打印
		pAllMgr->PrintFightInfo(true, pInstaller ? pInstaller->GetEntityID() : 0, pCfg.get());

		//创建一个新的触发器魔法状态
		StateType* pNewState = new StateType(pSkillInst, pInstaller, this, pCfg);
		pNewState->m_uDynamicId = pAllMgr->AddStateByDId(pNewState);

		//状态元素插入管理对象的TriggerState映射
		pair<typename MapState::iterator, bool> pr = this->m_mapState.insert(make_pair(pCfg.get(), pNewState));

		//关联TriggerState迭代器
		typename MapState::iterator mapStateItr = pr.first;
		StateType* pState = mapStateItr->second; 
		pState->m_mapStateItr = mapStateItr;

		switch(pCfg->GetTriggerEvent())
		{
		case eSET_InstallerDie:
			pInstaller->Attach(pState, eCE_Die);
			break;
		case eSET_KillByInstaller:
			this->m_pOwner->Attach(pState, eCE_Die);
			break;
		default:
			{
				this->RegisterEvent(pState, pCfg->GetTriggerEvent());
			}
		}

		//pState->Start();
		pState->StartTime();

		//调用魔法状态种类的模型和特效回调
		uint32 uId = pCfg->GetId();
		pAllMgr->OnSetState(uId, pState->m_uDynamicId, 1, pState->m_iTime,
			pState->m_iRemainTime, pState->m_pSkillInst->GetSkillLevel(), pState->GetInstallerID());

		pState->StartDo();

		return true;
	}
	else
	{
		//等级不低于原有魔法状态的等级则进入替换操作
		StateType* pState = mapStateItr->second;
		if (pSkillInst->GetSkillLevel() >= pState->m_uGrade)
		{
			return pState->Replace(pSkillInst, pInstaller);
		}
		else
		{
			return false;
		}
	}
}

template<typename CfgType, typename StateType>
bool TTriggerableStateMgrServer<CfgType, StateType>::RestoreStateFromDB(CSkillInstServer* pSkillInst, const SharedPtrType& pCfg,
	CFighterDictator* pInstaller, int32 iTime, int32 iRemainTime, uint32 uAccumulate, float fProbability)
{
	if (!this->m_pOwner)
	{
		GenErr("未调用SetOwner设置指向目标本身的指针");
		//return false;
	}

	typename MapState::iterator mapStateItr = this->m_mapState.find(pCfg.get());
	if (mapStateItr == this->m_mapState.end())
	{
#ifdef COUT_STATE_INFO
		cout << "安装触发器状态：" << pCfg->GetName() << endl;
#endif

		//创建一个新的触发器魔法状态
#ifdef COUT_STATE_INFO
		cout << this->m_pOwner->GetEntityID() << ": 恢复：" << pCfg->GetTypeName() << " : " << pCfg->GetName() << endl;
#endif

		StateType* pNewState = new StateType(pSkillInst, pInstaller, this, pCfg, uAccumulate, iTime, iRemainTime, fProbability);
		pNewState->m_uDynamicId = this->GetAllMgr()->AddStateByDId(pNewState);

		//状态元素插入管理对象的TriggerState映射
		pair<typename MapState::iterator, bool> pr = this->m_mapState.insert(make_pair(pCfg.get(), pNewState));

		//关联TriggerState迭代器
		typename MapState::iterator mapStateItr = pr.first;
		StateType* pState = mapStateItr->second;
		pState->m_mapStateItr = mapStateItr;

		switch(pCfg->GetTriggerEvent())
		{
		case eSET_InstallerDie:
			Ast(pInstaller);
			pInstaller->Attach(pState, eCE_Die);
			break;
		case eSET_KillByInstaller:
			this->m_pOwner->Attach(pState, eCE_Die);
			break;
		default:
			{
				this->RegisterEvent(pState, pCfg->GetTriggerEvent());
			}
		}

		//pState->Start(true);
		pState->StartTime(true);

		//调用魔法状态种类的模型和特效回调
		uint32 uId = pCfg->GetId();
		this->GetAllMgr()->OnSetState(uId, pState->m_uDynamicId, 1, pState->m_iTime, pState->m_iRemainTime,
			pState->m_pSkillInst->GetSkillLevel(), pState->GetInstallerID());

		pState->StartDo(true);
		return true;
	}
	else
	{
		//恢复触发器状态不能出现重复名字的状态
		stringstream ExpStr;
		ExpStr << "恢复触发器状态[" << pCfg->GetName() << "]不能出现重复名字的状态\n";
		GenErr("恢复触发器状态不能出现重复名字的状态", ExpStr.str());
		return false;
	}
}

template<typename CfgType, typename StateType>
void TTriggerableStateMgrServer<CfgType, StateType>::ClearAll()
{
	this->m_mapStateBundleByEvent.clear();
	CAllStateMgrServer* pAllMgr = this->GetAllMgr();
	for(typename MapState::iterator itrState = this->m_mapState.begin(); itrState != this->m_mapState.end(); ++itrState)
	{
		StateType* pState = itrState->second;
		SharedPtrType& pCfg = pState->GetCfgSharedPtr();
		uint32 uDynamicId = pState->m_uDynamicId;
		pState->CancelDo();
		//这里要考虑是否检测pState的有效性
		this->GetOwner()->UnRegistDistortedTick(pState);

		if(pCfg->GetTriggerEvent() == eSET_InstallerDie)
		{
			CFighterDictator* pFighter = pState->GetInstaller();
			if(pFighter)
			{
				//因为itrState->second是要删除的，所以不需要再执行一次Detach，否则反而可能会因为m_pInstaller析构而报错
				pFighter->Detach(pState, eCE_Die);
			}
		}

		uint32 uId = pCfg->GetId();
		pAllMgr->OnDeleteState(uId, uDynamicId);
		pAllMgr->DelStateByDId(uDynamicId);

		////为ClearAll的中途CancelDo里调用的ExistState等函数准备的（ExistState等函数要检测itr->second != NULL）
		//if (pState->m_mapStateItr != m_mapState.end()) pState->m_mapStateItr->second = NULL;

#ifdef COUT_STATE_INFO
		cout << "外部调用ClearAll，删除所有" << pCfg->GetTypeName() << endl;
#endif
	}
	ClearMap(this->m_mapState);
}

template<typename CfgType, typename StateType>
CStateDBData* TTriggerableStateMgrServer<CfgType, StateType>::CreateStateDBData(StateType* pState, int32 iRemainTime, CSkillInstServer* pInst)
{
	return new CStateDBData(pState->GetCfgSharedPtr()->GetGlobalType(),
		pState->GetCfgSharedPtr()->GetName(),			//这个参数是直接复制指针的，要小心，考虑是否复制个副本
		pState->m_iTime, iRemainTime, pState->m_uAccumulate, pState->m_fProbability, 
		0, 0, 0, pInst->GetSkillLevel(),
		pInst->GetSkillName().c_str(),			//这个参数是直接复制指针的，要小心，考虑是否复制个副本
		pInst->GetAttackType(),
		pInst->GetInterval(),
		pState->GetInstaller() == this->m_pOwner);
}

template<typename CfgType, typename StateType>
uint32 TTriggerableStateMgrServer<CfgType, StateType>::TriggerCount(const string& name)
{
	typename MapState::iterator itr = this->m_mapState.find(CfgType::Get(name).get());
	if(itr != this->m_mapState.end())
	{
		return itr->second->GetValue();
	}
	else
	{
		return 0;
	}
}

template<typename CfgType, typename StateType>
bool TTriggerableStateMgrServer<CfgType, StateType>::SetAccumulate(const string& name, uint32 uValue)
{
	typename MapState::iterator itr = this->m_mapState.find(CfgType::Get(name).get());
	if(itr != this->m_mapState.end())
	{
		itr->second->SetAccumulate(uValue);
		return true;
	}
	else
	{
		return false;
	}
}
