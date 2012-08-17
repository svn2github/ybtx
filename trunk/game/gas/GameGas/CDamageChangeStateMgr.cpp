#include "stdafx.h"
#include "CDamageChangeStateMgr.h"
#include "CSkillInstServer.h"
#include "CTriggerableStateMgr.inl"
#include "COtherStateMgr.inl"
#include "CStateDBData.h"

template class TOtherStateMgrServer<CDamageChangeStateCfgServer, CDamageChangeStateServer>;
template class TTriggerableStateMgrServer<CDamageChangeStateCfgServer, CDamageChangeStateServer>;

//void CTriggerStateMgrServer::ClearDamageChangeState(const string& name)
//{
//	CDamageChangeStateCfgServer* pCfg = CDamageChangeStateCfgServer::Get(name);
//	if(!pCfg) return;
//	MapDamageChangeState::iterator itrPDS = m_mapDS.find(pCfg);
//	if(itrPDS != m_mapDS.end())
//	{
//		itrPDS->second->DeleteSelf();
//	}
//}
//
//bool CTriggerStateMgrServer::ClearDamageChangeStateByCond(CStateCondBase* pStateCond, uint32 uId)
//{
//	CDamageChangeStateCfgServer::MapDamageChangeStateCfgById::iterator itr = CDamageChangeStateCfgServer::m_mapCfgById.find(uId);
//	if(itr != CDamageChangeStateCfgServer::m_mapCfgById.end())
//	{
//		MapDamageChangeState::iterator itrPDS = m_mapDS.find(itr->second);
//		if(itrPDS != m_mapDS.end())
//		{
//			if((*pStateCond)(itrPDS->second))
//				itrPDS->second->DeleteSelf();
//			else
//				return false;
//		}
//	}
//	return true;
//}
//
//bool CTriggerStateMgrServer::ExistState(const string& name)
//{
//	MapDamageChangeState::iterator itr = m_mapDS.find(CDamageChangeStateCfgServer::Get(name));
//	return  itr != m_mapDS.end();
//}
//
//bool CTriggerStateMgrServer::ExistState(const string& name, const CFighterDictator* pInstaller)
//{
//	MapDamageChangeState::iterator itr = m_mapDS.find(CDamageChangeStateCfgServer::Get(name));
//	if(itr != m_mapDS.end())
//	{
//		return itr->second->m_pInstaller == pInstaller;
//	}
//	else
//	{
//		return false;
//	}
//}
//
//
//
//
//
//
//bool CTriggerStateMgrServer::SetupDamageChangeState(CSkillInstServer* pSkillInst, const string& name, CFighterDictator* pInstaller)
//{
//	if (!m_pOwner)
//	{
//		GenErr("未调用SetOwner设置指向目标本身的指针");
//		//return false;
//	}
//	CDamageChangeStateCfgServer* pCfg = CDamageChangeStateCfgServer::Get(name);
//
//	//免疫计算
//	if(GetAllMgr()->Immume(pCfg, pSkillInst)) return false;
//
//	//按抗性计算
//	if(GetAllMgr()->DecreaseStateResist(pCfg)) return false;
//
//	//变形状态替换处理
//	if(!GetAllMgr()->ReplaceModelState(pCfg)) return false;
//
//	MapDamageChangeState::iterator mapPDSItr = m_mapDS.find(pCfg);
//	if (mapPDSItr == m_mapDS.end())
//	{
//		cout << m_pOwner->GetEntityID() << ": 安装伤害变更状态：" << pCfg->GetName() << "\n";
//
//		//创建一个新的伤害变更魔法状态
//		CDamageChangeStateServer* pTempDS = new CDamageChangeStateServer(pSkillInst, pInstaller, this, pCfg);
//
//		//状态元素插入管理对象的DamageChangeState映射
//		pair<MapDamageChangeState::iterator, bool> pr = m_mapDS.insert(make_pair(pCfg, pTempDS));
//
//		//关联DamageChangeState迭代器 
//		MapDamageChangeState::iterator mapPDSItr = pr.first;
//		mapPDSItr->second->m_mapStateItr = mapPDSItr;
//
//		if(pCfg->GetTriggerEvent() == eSET_InstallerDie)
//		{
//			pInstaller->Attach(mapPDSItr->second, eCE_Die);
//		}
//		else
//		{
//			//自定义观察者模式Attach
//			MapTSEventBandle::iterator mapStateBundleItr = m_mapBSEB.find(pCfg->GetTriggerEvent());
//			if (mapStateBundleItr == m_mapBSEB.end())
//			{
//				//创建一个新的事件类型束
//				TStateBundleByEvent tempBSEB;
//
//				//状态指针插入事件类型束的DamageChangeState列表，并关联EventBandle迭代器
//				mapPDSItr->second->m_listStateItr = tempBSEB.m_listDS.insert(tempBSEB.m_listDS.end(), mapPDSItr->second);
//
//				//事件类型束插入管理对象的EventBandle映射
//				pair<MapTSEventBandle::iterator, bool> prBSEB = m_mapBSEB.insert(make_pair(pCfg->GetTriggerEvent(), tempBSEB));
//
//				//记录刚插入的EventBandle元素在管理对象的位置
//				mapStateBundleItr = prBSEB.first;
//
//				//关联EventBandle迭代器
//				mapPDSItr->second->m_listStateItr = mapStateBundleItr->second.m_listDS.begin();
//			}
//			else
//			{
//				//这里肯定不会有mapStateBundleItr->second.m_listDS.find(mapPDSItr->second)) != second.m_listBS.end()的情况
//				//因为mapPDSItr == m_mapDS.end()已经排除了
//				//状态指针插入事件类型束的TriggerState列表，并关联EventBandle迭代器
//				mapPDSItr->second->m_listStateItr = mapStateBundleItr->second.m_listDS.insert(mapStateBundleItr->second.m_listDS.end(), mapPDSItr->second);
//			}
//			//关联EventBandle的指针
//			mapPDSItr->second->m_pStateEventBundle = &mapStateBundleItr->second;
//		}
//
//		mapPDSItr->second->Start();
//
//		//调用魔法状态种类的模型和特效回调
//		GetAllMgr()->OnSetState(pCfg->GetId(), pCfg->GetId(), 1, mapPDSItr->second->m_iTime, mapPDSItr->second->m_iRemainTime);
//		return true;
//	}
//	else
//	{
//		//等级不低于原有魔法状态的等级则进入替换操作
//		if (pSkillInst->GetSkillLevel() >= mapPDSItr->second->m_uGrade)
//		{
//			return mapPDSItr->second->Replace(pSkillInst, pInstaller);
//		}
//		else
//		{
//			return false;
//		}
//	}
//}
//
//bool CTriggerStateMgrServer::RestoreDamageChangeStateFromDB(CSkillInstServer* pSkillInst, CDamageChangeStateCfgServer* pCfg,
//															CFighterDictator* pInstaller, int32 iRemainTime, uint32 uAccumulate, float fProbability)	//从数据库恢复某个状态
//{
//	if (!m_pOwner)
//	{
//		GenErr("未调用SetOwner设置指向目标本身的指针");
//		//return false;
//	}
//
//	MapDamageChangeState::iterator mapPDSItr = m_mapDS.find(pCfg);
//	if (mapPDSItr == m_mapDS.end())
//	{
//		cout << m_pOwner->GetEntityID() << ": 安装伤害变更状态：" << pCfg->GetName() << "\n";
//
//		//创建一个新的伤害变更魔法状态
//		CDamageChangeStateServer* pTempDS = new CDamageChangeStateServer(pSkillInst, pInstaller, this, pCfg, uAccumulate, iRemainTime, fProbability);
//
//		//状态元素插入管理对象的DamageChangeState映射
//		pair<MapDamageChangeState::iterator, bool> pr = m_mapDS.insert(make_pair(pCfg, pTempDS));
//
//		//关联DamageChangeState迭代器 
//		MapDamageChangeState::iterator mapPDSItr = pr.first;
//		mapPDSItr->second->m_mapStateItr = mapPDSItr;
//
//		if(pCfg->GetTriggerEvent() == eSET_InstallerDie)
//		{
//			pInstaller->Attach(mapPDSItr->second, eCE_Die);
//		}
//		else
//		{
//			//自定义观察者模式Attach
//			MapTSEventBandle::iterator mapStateBundleItr = m_mapBSEB.find(pCfg->GetTriggerEvent());
//			if (mapStateBundleItr == m_mapBSEB.end())
//			{
//				//创建一个新的事件类型束
//				TStateBundleByEvent tempBSEB;
//
//				//状态指针插入事件类型束的DamageChangeState列表，并关联EventBandle迭代器
//				mapPDSItr->second->m_listStateItr = tempBSEB.m_listDS.insert(tempBSEB.m_listDS.end(), mapPDSItr->second);
//
//				//事件类型束插入管理对象的EventBandle映射
//				pair<MapTSEventBandle::iterator, bool> prBSEB = m_mapBSEB.insert(make_pair(pCfg->GetTriggerEvent(), tempBSEB));
//
//				//记录刚插入的EventBandle元素在管理对象的位置
//				mapStateBundleItr = prBSEB.first;
//
//				//关联EventBandle迭代器
//				mapPDSItr->second->m_listStateItr = mapStateBundleItr->second.m_listDS.begin();
//			}
//			else
//			{
//				//这里肯定不会有mapStateBundleItr->second.m_listDS.find(mapPDSItr->second)) != second.m_listBS.end()的情况
//				//因为mapPDSItr == m_mapDS.end()已经排除了
//				//状态指针插入事件类型束的TriggerState列表，并关联EventBandle迭代器
//				mapPDSItr->second->m_listStateItr = mapStateBundleItr->second.m_listDS.insert(mapStateBundleItr->second.m_listDS.end(), mapPDSItr->second);
//			}
//			//关联EventBandle的指针
//			mapPDSItr->second->m_pStateEventBundle = &mapStateBundleItr->second;
//		}
//
//		mapPDSItr->second->Start(true);
//
//		//调用魔法状态种类的模型和特效回调
//		GetAllMgr()->OnSetState(pCfg->GetId(), pCfg->GetId(), 1, mapPDSItr->second->m_iTime, mapPDSItr->second->m_iRemainTime);
//		return true;
//	}
//	else
//	{
//		//恢复伤害变更状态不能出现重复名字的状态
//		stringstream ExpStr;
//		ExpStr << "恢复伤害变更状态<" << pCfg->GetName() << ">不能出现重复名字的状态\n";
//		GenErr(ExpStr.str());
//		return false;
//	}
//}

