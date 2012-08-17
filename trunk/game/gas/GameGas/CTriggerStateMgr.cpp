#include "stdafx.h"
#include "CTriggerStateMgr.h"
#include "CSkillInstServer.h"
#include "CTriggerableStateMgr.inl"
#include "COtherStateMgr.inl"
#include "CStateDBData.h"


template class TOtherStateMgrServer<CTriggerStateCfgServer, CTriggerStateServer>;
template class TTriggerableStateMgrServer<CTriggerStateCfgServer, CTriggerStateServer>;

//bool CTriggerStateMgrServer::SetupTriggerState(CSkillInstServer* pSkillInst, const string& name, CFighterDictator* pInstaller)
//{
//	if (!m_pOwner)
//	{
//		GenErr("未调用SetOwner设置指向目标本身的指针");
//		//return false;
//	}
//	CTriggerStateCfgServer* pCfg = CTriggerStateCfgServer::Get(name);
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
//	MapTriggerState::iterator mapPTSItr = m_mapTS.find(pCfg);
//	if (mapPTSItr == m_mapTS.end())
//	{
//		cout << m_pOwner->GetEntityID()  << " 安装触发器状态：" << pCfg->GetName() << "\n";
//
//		//创建一个新的触发器魔法状态
//		CTriggerStateServer* pTempTS = new CTriggerStateServer(pSkillInst, pInstaller, this, pCfg);
//
//		//状态元素插入管理对象的TriggerState映射
//		pair<MapTriggerState::iterator, bool> pr = m_mapTS.insert(make_pair(pCfg, pTempTS));
//
//		//关联TriggerState迭代器
//		MapTriggerState::iterator mapPTSItr = pr.first;
//		mapPTSItr->second->m_mapStateItr = mapPTSItr;
//
//		if(pCfg->GetTriggerEvent() == eSET_InstallerDie)
//		{
//			pInstaller->Attach(mapPTSItr->second, eCE_Die);
//		}
//		else
//		{
//			//自定义观察者模式Attach
//			MapTSEventBandle::iterator mapStateBundleItr = m_mapBSEB.find(pCfg->GetTriggerEvent());
//			if (mapStateBundleItr == m_mapBSEB.end())
//			{
//				//创建一个新的事件类型束
//				TStateBundleByEvent<StateType> tempBSEB;
//
//				//状态指针插入事件类型束的TriggerState列表，并关联EventBandle迭代器
//				mapPTSItr->second->m_listStateItr = tempBSEB.m_listTS.insert(tempBSEB.m_listTS.end(), mapPTSItr->second);
//
//				//事件类型束插入管理对象的EventBandle映射
//				pair<MapTSEventBandle::iterator, bool> prBSEB = m_mapBSEB.insert(make_pair(pCfg->GetTriggerEvent(), tempBSEB));
//
//				//记录刚插入的EventBandle元素在管理对象的位置
//				mapStateBundleItr = prBSEB.first;
//
//				//关联EventBandle迭代器
//				mapPTSItr->second->m_listStateItr = mapStateBundleItr->second.m_listTS.begin();
//			}
//			else
//			{
//				//这里肯定不会有mapStateBundleItr->second.m_listTS.find(mapPTSItr->second) != second.m_listTS.end()的情况
//				//因为mapPTSItr == m_mapTS.end()已经排除了
//				//状态指针插入事件类型束的TriggerState列表，并关联EventBandle迭代器
//				mapPTSItr->second->m_listStateItr = mapStateBundleItr->second.m_listTS.insert(mapStateBundleItr->second.m_listTS.end(), mapPTSItr->second);
//			}
//			//关联EventBandle的指针
//			mapPTSItr->second->m_pStateEventBundle = &mapStateBundleItr->second;
//		}
//
//		mapPTSItr->second->Start();
//
//		//调用魔法状态种类的模型和特效回调
//		GetAllMgr()->OnSetState(pCfg->GetId(), pCfg->GetId(), 1, mapPTSItr->second->m_iTime, mapPTSItr->second->m_iRemainTime);
//		return true;
//	}
//	else
//	{
//		//等级不低于原有魔法状态的等级则进入替换操作
//		if (pSkillInst->GetSkillLevel() >= mapPTSItr->second->m_uGrade)
//		{
//			return mapPTSItr->second->Replace(pSkillInst, pInstaller);
//		}
//		else
//		{
//			return false;
//		}
//	}
//}
//
//bool CTriggerStateMgrServer::RestoreTriggerStateFromDB(CSkillInstServer* pSkillInst, CTriggerStateCfgServer* pCfg,
//													   CFighterDictator* pInstaller, int32 iRemainTime, uint32 uAccumulate, float fProbability)
//{
//	stringstream ExpStr;
//
//	ClearAll();
//
//	if (!m_pOwner)
//	{
//		GenErr("未调用SetOwner设置指向目标本身的指针");
//		//return false;
//	}
//
//	MapTriggerState::iterator mapPTSItr = m_mapTS.find(pCfg);
//	if (mapPTSItr == m_mapTS.end())
//	{
//		cout << "安装触发器状态：" << pCfg->GetName() << "\n";
//
//		//创建一个新的触发器魔法状态
//		cout << m_pOwner->GetEntityID() << ": 安装触发器状态：" << pCfg->GetName() << "\n";
//		CTriggerStateServer* pTempTS = new CTriggerStateServer(pSkillInst, pInstaller, this, pCfg, uAccumulate, iRemainTime, fProbability);
//
//		//状态元素插入管理对象的TriggerState映射
//		pair<MapTriggerState::iterator, bool> pr = m_mapTS.insert(make_pair(pCfg, pTempTS));
//
//		//关联TriggerState迭代器
//		MapTriggerState::iterator mapPTSItr = pr.first;
//		mapPTSItr->second->m_mapStateItr = mapPTSItr;
//
//		if(pCfg->GetTriggerEvent() == eSET_InstallerDie)
//		{
//			pInstaller->Attach(mapPTSItr->second, eCE_Die);
//		}
//		else
//		{
//			//自定义观察者模式Attach
//			MapTSEventBandle::iterator mapStateBundleItr = m_mapBSEB.find(pCfg->GetTriggerEvent());
//			if (mapStateBundleItr == m_mapBSEB.end())
//			{
//				//创建一个新的事件类型束
//				TStateBundleByEvent<StateType> tempBSEB;
//
//				//状态指针插入事件类型束的TriggerState列表，并关联EventBandle迭代器
//				mapPTSItr->second->m_listStateItr = tempBSEB.m_listTS.insert(tempBSEB.m_listTS.end(), mapPTSItr->second);
//
//				//事件类型束插入管理对象的EventBandle映射
//				pair<MapTSEventBandle::iterator, bool> prBSEB = m_mapBSEB.insert(make_pair(pCfg->GetTriggerEvent(), tempBSEB));
//
//				//记录刚插入的EventBandle元素在管理对象的位置
//				mapStateBundleItr = prBSEB.first;
//
//				//关联EventBandle迭代器
//				mapPTSItr->second->m_listStateItr = mapStateBundleItr->second.m_listTS.begin();
//			}
//			else
//			{
//				//这里肯定不会有mapStateBundleItr->second.m_listTS.find(mapPTSItr->second) != second.m_listTS.end()的情况
//				//因为mapPTSItr == m_mapTS.end()已经排除了
//				//状态指针插入事件类型束的TriggerState列表，并关联EventBandle迭代器
//				mapPTSItr->second->m_listStateItr = mapStateBundleItr->second.m_listTS.insert(mapStateBundleItr->second.m_listTS.end(), mapPTSItr->second);
//			}
//			//关联EventBandle的指针
//			mapPTSItr->second->m_pStateEventBundle = &mapStateBundleItr->second;
//		}
//
//		mapPTSItr->second->Start(true);
//
//		//调用魔法状态种类的模型和特效回调
//		GetAllMgr()->OnSetState(pCfg->GetId(), pCfg->GetId(), 1, mapPTSItr->second->m_iTime, mapPTSItr->second->m_iRemainTime);
//		return true;
//	}
//	else
//	{
//		//恢复触发器状态不能出现重复名字的状态
//		stringstream ExpStr;
//		ExpStr << "恢复触发器状态<" << pCfg->GetName() << ">不能出现重复名字的状态\n";
//		GenErr(ExpStr.str());
//		return false;
//	}
//}
//
//CTriggerStateServer* CTriggerStateMgrServer::FindTriggerState(const string& name)
//{
//	MapTriggerState::iterator itr = m_mapTS.find(CTriggerStateCfgServer::Get(name));
//	if(itr != m_mapTS.end())
//		return itr->second;
//	else
//		return NULL;
//}
//
//
//
//void CTriggerStateMgrServer::ClearTriggerState(const string& name)
//{
//	CTriggerStateCfgServer* pCfg = CTriggerStateCfgServer::Get(name);
//	if(!pCfg) return;
//	MapTriggerState::iterator itrPTS = m_mapTS.find(pCfg);
//	if(itrPTS != m_mapTS.end())
//	{
//		itrPTS->second->DeleteSelf();
//	}
//}
//
//bool CTriggerStateMgrServer::ClearTriggerStateByCond(CStateCondBase* pStateCond, uint32 uId)
//{
//	CTriggerStateCfgServer::MapTriggerStateCfgById::iterator itr = CTriggerStateCfgServer::m_mapCfgById.find(uId);
//	if(itr != CTriggerStateCfgServer::m_mapCfgById.end())
//	{
//		MapTriggerState::iterator itrPTS = m_mapTS.find(itr->second);
//		if(itrPTS != m_mapTS.end())
//		{
//			if((*pStateCond)(itrPTS->second))
//				itrPTS->second->DeleteSelf();
//			else
//				return false;
//		}
//	}
//	return true;
//}
//
//
//
//
//bool CTriggerStateMgrServer::ExistState(const string& name)
//{
//	MapTriggerState::iterator itr = m_mapTS.find(CTriggerStateCfgServer::Get(name));
//	return itr != m_mapTS.end();
//}
//
//bool CTriggerStateMgrServer::ExistState(const string& name, const CFighterDictator* pInstaller)
//{
//	MapTriggerState::iterator itr = m_mapTS.find(CTriggerStateCfgServer::Get(name));
//	if(itr != m_mapTS.end())
//	{
//		return itr->second->m_pInstaller == pInstaller;
//	}
//	else
//	{
//		return false;
//	}
//}








////以下是CTriggerStateMgrServer关于CTriggerStateServer和CDamageStateServer公共的部分
//bool CTriggerStateMgrServer::MessageEvent(uint32 uEventID, CFighterDictator* pTrigger, ETriggerStateType eType)
//{
//
//	CGenericTarget GenTrigger(pTrigger);
//	return MessageEvent(uEventID, &GenTrigger, eType);
//}
//
//bool CTriggerStateMgrServer::MessageEvent(uint32 uEventID, const CPos& pTrigger, ETriggerStateType eType)
//{
//
//	CGenericTarget GenTrigger(pTrigger);
//	return MessageEvent(uEventID, &GenTrigger, eType);
//}
//
//bool CTriggerStateMgrServer::MessageEvent(uint32 uEventID, CGenericTarget* pTrigger, ETriggerStateType eType)
//{
//	//cout << "触发器状态管理器响应触发事件：" << eEventType << "\n";
//	MapTSEventBandle::iterator mapBSEBIter = m_mapBSEB.find(uEventID);
//	if (mapBSEBIter == m_mapBSEB.end())
//	{
//		return false;
//	}
//	else
//	{
//		if(eType != eTST_Trigger)
//		{
//			//先触发伤害变更状态再触发触发器状态
//			ListPDamageChangeState& listDS = mapBSEBIter->second.m_listDS;
//			ListPDamageChangeState::iterator itrDSBackup;
//			for (ListPDamageChangeState::iterator itr = listDS.begin(); itr != listDS.end();)
//			{
//				itrDSBackup = itr;
//				++itrDSBackup;
//				(*itr)->MessageEvent(uEventID, pTrigger);
//				itr = itrDSBackup;
//			}
//		}
//
//		if(eType != eTST_DamageChange)
//		{
//			ListPTriggerState& listTS = mapBSEBIter->second.m_listTS;
//			ListPTriggerState::iterator itrTSBackup;
//			for (ListPTriggerState::iterator itr = listTS.begin(); itr != listTS.end();)
//			{
//				itrTSBackup = itr;
//				++itrTSBackup;
//				(*itr)->MessageEvent(uEventID, pTrigger);
//				itr = itrTSBackup;
//			}
//
//		}
//
//		return true;
//	}
//}
//
//void CTriggerStateMgrServer::ClearAll()
//{
//	m_mapBSEB.clear();
//	for(MapTriggerState::iterator itrPTS = m_mapTS.begin(); itrPTS != m_mapTS.end(); ++itrPTS)
//	{
//		CTriggerStateCfgServer* pCfg = class_cast<CTriggerStateCfgServer*>(itrPTS->second->m_pCfg);
//		GetOwner()->UnRegistDistortedTick(itrPTS->second);
//		itrPTS->second->CancelDo();
//
//		if(pCfg->GetTriggerEvent() == eSET_InstallerDie)
//		{
//			if(itrPTS->second->m_pInstaller)
//			{
//				//因为itrPTS->second是要删除的，所以不需要再执行一次Detach，否则反而可能会因为m_pInstaller析构而报错
//				itrPTS->second->m_pInstaller->Detach(itrPTS->second, eCE_Die);
//			}
//		}
//
//		GetAllMgr()->OnDeleteState(itrPTS->second->m_pCfg->GetId(), itrPTS->second->m_pCfg->GetId());
//		cout << "外部调用ClearAll，删除所有TriggerState。\n";
//	}
//	ClearMap(m_mapTS);
//
//	for(MapDamageChangeState::iterator itrPDS = m_mapDS.begin(); itrPDS != m_mapDS.end(); ++itrPDS)
//	{
//		GetOwner()->UnRegistDistortedTick(itrPDS->second);
//		itrPDS->second->CancelDo();
//
//		if(itrPDS->second->m_pCfg->GetTriggerEvent() == eSET_InstallerDie)
//		{
//			if(itrPDS->second->m_pInstaller)
//			{
//				//因为itrPDS->second是要删除的，所以不需要再执行一次Detach，否则反而可能会因为m_pInstaller析构而报错
//				itrPDS->second->m_pInstaller->Detach(itrPDS->second, eCE_Die);
//			}
//		}
//
//		GetAllMgr()->OnDeleteState(itrPDS->second->m_pCfg->GetId(), itrPDS->second->m_pCfg->GetId());
//		cout << "外部调用ClearAll，删除所有DamageChangeState。\n";
//	}
//	ClearMap(m_mapDS);
//}
//
//
//
//
//void CTriggerStateMgrServer::ClearAllByCond(CStateCondBase* pStateCond)
//{
//	MapTriggerState::iterator itrPTS = m_mapTS.begin(), itrPTSBackup;
//	for(; itrPTS != m_mapTS.end();)
//	{
//		itrPTSBackup = itrPTS;
//		itrPTSBackup++;
//		if ((*pStateCond)(itrPTS->second))
//		{		
//			itrPTS->second->DeleteSelf();
//		}
//		itrPTS = itrPTSBackup;
//	}
//
//	MapDamageChangeState::iterator itrPDS = m_mapDS.begin(), itrPDSBackup;
//	for(; itrPDS != m_mapDS.end();)
//	{
//		itrPDSBackup = itrPDS;
//		itrPDSBackup++;
//		if ((*pStateCond)(itrPDS->second))
//		{	
//			itrPDS->second->DeleteSelf();
//		}
//		itrPDS = itrPDSBackup;
//	}
//}
//
//
//bool CTriggerStateMgrServer::CountSub(const string& name)
//{
//	CTriggerStateCfgServer* pCfg = CTriggerStateCfgServer::Get(name);
//	MapTriggerState::iterator mapPTSItr = m_mapTS.find(pCfg);
//	if(mapPTSItr == m_mapTS.end())
//	{
//		stringstream ExpStr;
//		ExpStr << "外部调用减少触发作用次数时出错，触发器状态 " << name << " 不存在\n";
//		GenErr(ExpStr.str());
//		//return false;
//	}
//
//	CTriggerStateServer* pTS = mapPTSItr->second;
//	pTS->m_uAccumulate++;
//	if ((int32)pTS->m_uAccumulate >= pTS->m_iInitialValue && pTS->m_iInitialValue >= 0)
//	{
//		cout << "因触发次数到了而删除自己\n";
//		pTS->DeleteSelf();
//	}
//
//	return true;
//}
//
//void CTriggerStateMgrServer::SyncAllState(CFighterDictator* pObserver, uint32 uNow)
//{
//	int32 iRemainTime;
//
//	for(MapTriggerState::iterator itr = m_mapTS.begin(); itr != m_mapTS.end(); itr++)
//	{
//		iRemainTime = itr->second->m_iRemainTime != -1 ?
//			itr->second->m_iRemainTime - (int32(uNow - itr->second->m_uStartTime) + 500) / 1000 : -1;
//
//		uint32 uId = itr->second->m_pCfg->GetId();
//		GetAllMgr()->OnSetState(uId, uId, 1, itr->second->m_iTime, iRemainTime, pObserver);
//	}
//	for(MapDamageChangeState::iterator itr = m_mapDS.begin(); itr != m_mapDS.end(); itr++)
//	{
//		iRemainTime = itr->second->m_iRemainTime != -1 ?
//			itr->second->m_iRemainTime - (int32(uNow - itr->second->m_uStartTime) + 500) / 1000 : -1;
//
//		uint32 uId = itr->second->m_pCfg->GetId();
//		GetAllMgr()->OnSetState(uId, uId, 1, itr->second->m_iTime, iRemainTime, pObserver);
//	}
//}
//
//bool CTriggerStateMgrServer::SerializeToDB(CStateDBDataSet* pRet, ICharacterMediatorCallbackHandler* pHandler, uint32 uFighterGlobalID, uint32 uNow)
//{
//	int32 iLeftTime;
//	for(MapTriggerState::iterator itr = m_mapTS.begin(); itr != m_mapTS.end(); ++itr)
//	{
//		if(!itr->second->m_pCfg->GetNeedSaveToDB()) continue;
//
//		if(itr->second->m_iRemainTime > -1)
//		{
//			iLeftTime = itr->second->m_iRemainTime * 1000 - int32(uNow - itr->second->m_uStartTime);
//		}
//		else
//		{
//			iLeftTime = -1;
//		}
//
//		CSkillInstServer*& pInst = itr->second->m_pSkillInst;
//
//		CStateDBData* pState = new CStateDBData(itr->second->m_pCfg->GetGlobalType(),
//			itr->second->m_pCfg->GetName(),			//这个参数是直接复制指针的，要小心，考虑是否复制个副本
//			iLeftTime, itr->second->m_uAccumulate, itr->second->m_fProbability, 
//			0, 0, 0, pInst->m_uSkillLevel,
//			pInst->m_strSkillName.c_str(),			//这个参数是直接复制指针的，要小心，考虑是否复制个副本
//			pInst->m_eAttackType,
//			pInst->GetInterval(),
//			itr->second->m_pInstaller == m_pOwner);
//
//		//pHandler->AddStateToDB(uFighterGlobalID, eSGT_TriggerState, &aData);	
//		pRet->m_pStateVec->PushBack(pState);
//
//		cout << "触发器状态[" << itr->second->m_pCfg->GetName() << "]存入数据库\n";
//	}
//
//	for(MapDamageChangeState::iterator itr = m_mapDS.begin(); itr != m_mapDS.end(); ++itr)
//	{
//		if(!itr->second->m_pCfg->GetNeedSaveToDB()) continue;
//
//		if(itr->second->m_iRemainTime > -1)
//		{
//			iLeftTime = itr->second->m_iRemainTime * 1000 - int32(uNow - itr->second->m_uStartTime);
//		}
//		else
//		{
//			iLeftTime = -1;
//		}
//
//		CSkillInstServer*& pInst = itr->second->m_pSkillInst;
//
//		CStateDBData* pState = new CStateDBData(itr->second->m_pCfg->GetGlobalType(), itr->second->m_pCfg->GetName(),			//这个参数是直接复制指针的，要小心，考虑是否复制个副本
//			iLeftTime, itr->second->m_uAccumulate, itr->second->m_fProbability, 
//			0, 0, 0, pInst->m_uSkillLevel,
//			pInst->m_strSkillName.c_str(),				//这个参数是直接复制指针的，要小心，考虑是否复制个副本
//			pInst->m_eAttackType,
//			pInst->GetInterval(),
//			itr->second->m_pInstaller == m_pOwner);
//
//
//		//pHandler->AddStateToDB(uFighterGlobalID, eSGT_DamageChangeState, &aData);	
//		pRet->m_pStateVec->PushBack(pState);
//
//		cout << "伤害变更状态[" << itr->second->m_pCfg->GetName() << "]存入数据库\n";
//	}
//	return true;
//}
//
//bool CTriggerStateMgrServer::LoadFromDB(ICharacterMediatorCallbackHandler* pHandler, uint32 uFighterGlobalID)
//{
//	stringstream ExpStr;
//
//	ClearAll();
//
//	for(uint32 uStateCount = 0;; uStateCount++)
//	{
//		//CStateDBData aStateDBData;
//		//bool bRet = pHandler->ReadStateFromDB(uFighterGlobalID, eSGT_TriggerState, &aStateDBData);
//		CStateDBData* pStateDBData = NULL;
//		bool bRet = GetAllMgr()->ReadStateFromDB(pStateDBData, eSGT_TriggerState);
//
//		if(!bRet) break;
//
//		CStateDBData& aStateDBData = *pStateDBData;
//
//		CTriggerStateCfgServer* pCfg = CTriggerStateCfgServer::Get(aStateDBData.m_sName);
//		if(!pCfg)
//		{
//			ExpStr << "触发器状态读取错误：触发器状态[" << aStateDBData.m_sName << "]不存在于配置表\n";
//			GenErr(ExpStr.str());
//			return false;
//		}
//
//		CSkillInstServer* pSkillInst = new CSkillInstServer(aStateDBData.m_uSkillLevel,
//			aStateDBData.m_sSkillName, aStateDBData.m_eAttackType, aStateDBData.m_bIsDot);
//
//		int32 iLeftSecond = aStateDBData.m_iRemainTime == -1 ? -1 : aStateDBData.m_iRemainTime / 1000 + 1; //恢复要补完不足的，否则会少一次DOT
//		RestoreTriggerStateFromDB(pSkillInst, pCfg, aStateDBData.m_bFromEqualsOwner ? m_pOwner : NULL,
//			iLeftSecond, aStateDBData.m_iCount,
//			aStateDBData.m_fProbability);
//		cout << "从数据库读出触发器状态[" << pCfg->GetName() << "]\n";
//	}
//
//	for(uint32 uStateCount = 0;; uStateCount++)
//	{
//		//CStateDBData aStateDBData;
//		//bool bRet = pHandler->ReadStateFromDB(uFighterGlobalID, eSGT_DamageChangeState, &aStateDBData);
//		CStateDBData* pStateDBData = NULL;
//		bool bRet = GetAllMgr()->ReadStateFromDB(pStateDBData, eSGT_DamageChangeState);
//
//		if(!bRet) break;
//
//		CStateDBData& aStateDBData = *pStateDBData;
//
//		CDamageChangeStateCfgServer* pCfg = CDamageChangeStateCfgServer::Get(aStateDBData.m_sName);
//		if(!pCfg)
//		{
//			ExpStr << "伤害变更状态读取错误：伤害变更状态[" << aStateDBData.m_sName << "]不存在于配置表\n";
//			GenErr(ExpStr.str());
//			return false;
//		}
//
//		CSkillInstServer* pSkillInst = new CSkillInstServer(aStateDBData.m_uSkillLevel,
//			aStateDBData.m_sSkillName, aStateDBData.m_eAttackType, aStateDBData.m_bIsDot);
//
//		int32 iLeftSecond = aStateDBData.m_iRemainTime == -1 ? -1 : aStateDBData.m_iRemainTime / 1000 + 1; //恢复要补完不足的，否则会少一次DOT
//		RestoreDamageChangeStateFromDB(pSkillInst, pCfg, aStateDBData.m_bFromEqualsOwner ? m_pOwner : NULL,
//			iLeftSecond, aStateDBData.m_iCount,
//			aStateDBData.m_fProbability);
//		cout << "从数据库读出伤害变更状态[" << pCfg->GetName() << "]\n";
//	}
//
//	return true;
//}
