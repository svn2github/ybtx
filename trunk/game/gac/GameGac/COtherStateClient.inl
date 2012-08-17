#pragma once
#include "LoadSkillCommon.h"
#include "CTxtTableFile.h"
#include "COtherStateClient.h"
#include "CTplStateCfgClient.inl"
#include "CAllStateMgrClient.h"
#include "ICharacterFollowerCallbackHandler.h"
#include "ErrLogHelper.h"

template <class StateType>
bool COtherStateMgrClient<StateType>::SetupState(uint32 uCategoryId, uint32 uDynamicId, int32 iTime, int32 iRemainTime, uint32 uSkillLevel, uint32 uInstallerID)
{
	//uint32 uDynamicId = uCategoryId;
	const CTplStateCfgClient<StateType>::SharedPtrType& pCfg = CTplStateCfgClient<StateType>::GetById(uCategoryId);
	CAllStateMgrClient* pAllStateMgr = m_pOwner->GetAllStateMgr();
	typename MapTplStateClient::iterator itr = m_mapState.find(uCategoryId);
	if(itr == m_mapState.end())
	{
		COtherStateClient<StateType>* pState =
			new COtherStateClient<StateType>(uDynamicId, iTime, iRemainTime, uSkillLevel, pCfg, uInstallerID, this, pAllStateMgr);
		pAllStateMgr->m_mapSetupOrder[pState->GetSetupOrder()] = pState;
		pState->UpdateRemainTime(iRemainTime);
		m_mapState.insert(make_pair(uCategoryId, pState));
		m_pOwner->GetAllStateMgr()->OnBuffUpdate(
			uDynamicId, pCfg->GetName(), 1, iTime, (float)iRemainTime, pCfg->GetDecrease(), uSkillLevel, pCfg->GetDotInterval(), uInstallerID);
		m_pOwner->GetAllStateMgr()->OnStateCategoryBegin(uCategoryId, uDynamicId, 1);
		return true;
	}
	else
	{
		itr->second->UpdateTime(iTime);
		itr->second->UpdateRemainTime(iRemainTime);
		pAllStateMgr->OnBuffUpdate(
			uDynamicId, pCfg->GetName(), 1, iTime, (float)iRemainTime, pCfg->GetDecrease(), uSkillLevel, pCfg->GetDotInterval(), uInstallerID);
		return false;
	}
}

template <class StateType>
void COtherStateMgrClient<StateType>::DeleteState(uint32 uCategoryId, uint32 uDynamicId)
{
	//uint32 uDynamicId = uCategoryId;
	typename MapTplStateClient::iterator itr = m_mapState.find(uCategoryId);
	if(itr != m_mapState.end())
	{
		//去掉状态的特效和图标
		m_pOwner->GetAllStateMgr()->OnBuffUpdate(
			uDynamicId, itr->second->m_pCfg->GetName(), 0, 0, 0.0f, itr->second->m_pCfg->GetDecrease(), 1, 1, itr->second->GetInstallerID());
		
		m_pOwner->GetAllStateMgr()->OnStateCategoryEnd(uCategoryId, uDynamicId, 1);

		//因为其它状态一个种类ID只有一个状态，对应于一个动态ID，所以可以直接删除这个种类ID的状态，否则应该做成类似魔法状态的删除方式
		EraseMapNode(m_mapState, itr);
	}
}

template <class StateType>
bool COtherStateMgrClient<StateType>::ExistState(const string& name)
{
	typename MapTplStateClient::iterator itr = 
		m_mapState.find(CTplStateCfgClient<StateType>::GetByName(name.c_str())->GetId());
	return itr != m_mapState.end();
}

template <class StateType>
int32 COtherStateMgrClient<StateType>::StateLeftTime(const string& name)
{
	typename MapTplStateClient::iterator itr = 
		m_mapState.find(CTplStateCfgClient<StateType>::GetByName(name.c_str())->GetId());
	if(itr != m_mapState.end())
	{
		return itr->second->GetRemainTime();
	}
	else
	{
		return 0;
	}
}

template <class StateType>
float COtherStateMgrClient<StateType>::GetRemainTime(const TCHAR* sName, uint32 uDynamicId)
{
	//因为其它状态一个种类ID只有一个状态，对应于一个动态ID，所以可以直接获取这个种类ID的状态，否则应该做成类似魔法状态的获取方式
	typename MapTplStateClient::iterator itr = m_mapState.find(CTplStateCfgClient<StateType>::GetByName(sName)->GetId());
	if(itr != m_mapState.end())
	{
		return itr->second->GetRemainTime();
	}
	else
	{
		return 0;
	}
}


template <class StateType>
void COtherStateMgrClient<StateType>::SetTargetBuff()
{
	//添加所有状态的特效和图标
	for(typename MapTplStateClient::iterator itr = m_mapState.begin(); itr != m_mapState.end(); itr++)
	{
		m_pOwner->GetAllStateMgr()->OnBuffUpdate(
			itr->second->GetDynamicID(), itr->second->m_pCfg->GetName(), 1, 
			itr->second->GetTime() ,itr->second->GetRemainTime(), 
			itr->second->m_pCfg->GetDecrease(), itr->second->GetSkillLevel(), itr->second->m_pCfg->GetDotInterval(), itr->second->GetInstallerID());
	}
}

template <class StateType>
void COtherStateMgrClient<StateType>::ClearAll()
{
	//去掉所有状态的特效和图标
	for(typename MapTplStateClient::iterator itr = m_mapState.begin(); itr != m_mapState.end(); itr++)
	{
		//pMagicIcon->Update(itr->second->GetId(), itr->second->GetIcon(), 0, 0);
		m_pOwner->GetAllStateMgr()->OnBuffUpdate(
			itr->second->GetDynamicID(), itr->second->m_pCfg->GetName(), 0, 0, 0.0f,
			itr->second->m_pCfg->GetDecrease(), 1, 1, itr->second->GetInstallerID());
		m_pOwner->GetAllStateMgr()->OnStateCategoryEnd(itr->second->GetID(), itr->second->GetID(), 1);
	}
	ClearMap(m_mapState);
}
