#include "stdafx.h"
#include "CSpecialStateMgr.h"
#include "CSpecialStateCfg.h"
#include "CAllStateMgr.h"
#include "CStateCondFunctor.h"
#include "CSkillInstServer.h"
#include "CStateDBData.h"
#include "BaseHelper.h"
#include "CFighterDictator.h"
#include "CDirMoveState.h"
#include "ErrLogHelper.h"
#include "CTriggerEvent.h"
#include "CConnServer.h"
#include "DebugHelper.h"
#include "TSqrAllocator.inl"
#include "COtherStateMgr.inl"

template class TOtherStateMgrServer<CSpecialStateCfgServer, CSpecialStateBase>;

//以下是CSpecialStateMgrServer操作
bool CSpecialStateMgrServer::SetupState(CSkillInstServer* pSkillInst, const CSpecialStateCfgServerSharedPtr& pCfg, CFighterDictator* pInstaller)
{
	if (!m_pOwner)
	{
		GenErr("未调用SetOwner设置指向目标本身的指针");
		//return false;
	}

	CAllStateMgrServer* pAllMgr = GetAllMgr();

	//免疫计算
	if(pAllMgr->Immume(pCfg.get(), pSkillInst, pInstaller))
	{
		return false;
	}

	//按抗性计算
	if(pAllMgr->DecreaseStateResist(pCfg.get())) return false;

	//变形状态替换处理
	if(!pAllMgr->ReplaceModelState(pCfg.get())) return false;

	MapSpecialState::iterator itr = m_mapState.find(pCfg.get());
	if (itr == m_mapState.end())
	{
#ifdef COUT_STATE_INFO
		cout << "安装特殊状态：" << pCfg->GetName() << endl;
#endif

		//安装一个新的魔法状态时才会打印战斗信息, 叠加或替换时不打印
		pAllMgr->PrintFightInfo(true, pInstaller ? pInstaller->GetEntityID() : 0, pCfg.get());

		//创建一个新的触发器魔法状态
		CSpecialStateBase* pSS = CreateSpecialState(pSkillInst, pInstaller, this, pCfg);
		pSS->m_uDynamicId = pAllMgr->AddStateByDId(pSS);

		//状态元素插入管理对象的SpecialState映射
		pair<MapSpecialState::iterator, bool> pr = m_mapState.insert(make_pair(pCfg.get(), pSS));

		//关联SpecialState迭代器
		MapSpecialState::iterator mapPSSItr = pr.first;
		mapPSSItr->second->m_mapSSItr = mapPSSItr;

		MapSSTypeBandle::iterator mapSSTBItr = m_mapStateTypeBundle.find(pCfg->GetSSType());
		if (mapSSTBItr == m_mapStateTypeBundle.end())
		{
			//创建一个新的功能类型束
			CSpecialStateTypeBundle tempSSTB;

			//状态指针插入功能类型束的DamageChangeState列表，并关联EventBandle迭代器
			mapPSSItr->second->m_listSSItr = tempSSTB.m_listState.insert(tempSSTB.m_listState.end(), mapPSSItr->second);

			//功能类型束插入管理对象的StateTypeBundle映射
			pair<MapSSTypeBandle::iterator, bool> prSSTB = m_mapStateTypeBundle.insert(make_pair(pCfg->GetSSType(), tempSSTB));

			//记录刚插入的StateTypeBundle元素在管理对象的位置
			mapSSTBItr = prSSTB.first;

			//关联StateTypeBundle迭代器
			mapPSSItr->second->m_listSSItr = mapSSTBItr->second.m_listState.begin();
		}
		else
		{
			//这里肯定不会有mapStateBundleItr->second.m_listDS.find(mapPDSItr->second)) != second.m_listBS.end()的情况
			//因为mapPDSItr == m_mapDS.end()已经排除了
			//状态指针插入功能类型束的TriggerState列表，并关联StateTypeBundle迭代器
			mapPSSItr->second->m_listSSItr = mapSSTBItr->second.m_listState.insert(mapSSTBItr->second.m_listState.end(), mapPSSItr->second);
		}
		//关联StateTypeBundle的指针
		mapPSSItr->second->m_pStateBundleByType = &mapSSTBItr->second;

		mapPSSItr->second->RegisterEvent();

		pSS->Start();

		//调用魔法状态种类的模型和特效回调
		pAllMgr->OnSetState(pCfg->GetId(), pSS->m_uDynamicId, 1, pSS->m_iTime, pSS->m_iRemainTime, pSS->m_pSkillInst->GetSkillLevel(), pSS->GetInstallerID());

		return true;
	}
	else
	{
		//已经插入了则不做任何操作
	}
	return false;
}

bool CSpecialStateMgrServer::RestoreStateFromDB(CSkillInstServer* pSkillInst, const CSpecialStateCfgServerSharedPtr& pCfg,
													   CFighterDictator* pInstaller, int32 iTime, int32 iRemainTime)
{
	if (!m_pOwner)
	{
		GenErr("未调用SetOwner设置指向目标本身的指针");
		//return false;
	}

	MapSpecialState::iterator itr = m_mapState.find(pCfg.get());
	if (itr == m_mapState.end())
	{
#ifdef COUT_STATE_INFO
		cout << "安装特殊状态：" << pCfg->GetName() << endl;
#endif

		//创建一个新的触发器魔法状态
		CSpecialStateBase* pSS = CreateSpecialState(pSkillInst, pInstaller, this, pCfg, iTime, iRemainTime);
		pSS->m_uDynamicId = GetAllMgr()->AddStateByDId(pSS);

		//状态元素插入管理对象的SpecialState映射
		pair<MapSpecialState::iterator, bool> pr = m_mapState.insert(make_pair(pCfg.get(), pSS));

		//关联SpecialState迭代器
		MapSpecialState::iterator mapPSSItr = pr.first;
		mapPSSItr->second->m_mapSSItr = mapPSSItr;

		MapSSTypeBandle::iterator mapSSTBItr = m_mapStateTypeBundle.find(pCfg->GetSSType());
		if (mapSSTBItr == m_mapStateTypeBundle.end())
		{
			//创建一个新的功能类型束
			CSpecialStateTypeBundle tempSSTB;

			//状态指针插入功能类型束的SpecialState列表，并关联StateTypeBundle迭代器
			mapPSSItr->second->m_listSSItr = tempSSTB.m_listState.insert(tempSSTB.m_listState.end(), mapPSSItr->second);

			//功能类型束插入管理对象的StateTypeBundle映射
			pair<MapSSTypeBandle::iterator, bool> prSSTB = m_mapStateTypeBundle.insert(make_pair(pCfg->GetSSType(), tempSSTB));

			//记录刚插入的StateTypeBundle元素在管理对象的位置
			mapSSTBItr = prSSTB.first;

			//关联StateTypeBundle迭代器
			mapPSSItr->second->m_listSSItr = mapSSTBItr->second.m_listState.begin();
		}
		else
		{
			//这里肯定不会有mapStateBundleItr->second.m_listDS.find(mapPDSItr->second)) != second.m_listBS.end()的情况
			//因为mapPDSItr == m_mapDS.end()已经排除了
			//状态指针插入功能类型束的TriggerState列表，并关联StateTypeBundle迭代器
			mapPSSItr->second->m_listSSItr = mapSSTBItr->second.m_listState.insert(mapSSTBItr->second.m_listState.end(), mapPSSItr->second);
		}
		//关联EventBandle的指针
		mapPSSItr->second->m_pStateBundleByType = &mapSSTBItr->second;

		mapPSSItr->second->RegisterEvent();

		pSS->Start(true);

		//调用魔法状态种类的模型和特效回调
		GetAllMgr()->OnSetState(pCfg->GetId(), pSS->m_uDynamicId, 1, pSS->m_iTime, pSS->m_iRemainTime, pSS->m_pSkillInst->GetSkillLevel(), pSS->GetInstallerID());

		return true;
	}
	else
	{
		//已经插入了则不做任何操作
	}
	return false;
}

CSpecialStateBase* CSpecialStateMgrServer::CreateSpecialState(CSkillInstServer* pSkillInst,
	CFighterDictator* pInstaller, CSpecialStateMgrServer* pMgr,
	const CSpecialStateCfgServerSharedPtr& pCfg, int32 iTime, int32 iRemainTime)
{
	CSpecialStateBase* pSS = NULL;
	switch(pCfg->GetSSType())
	{
	case eSST_Reflect:
		pSS = new CReflectStateServer(pSkillInst, pInstaller, pMgr, pCfg, iTime, iRemainTime);
		break;
	case eSST_DOTImmunity:
		pSS = new CSpecialStateBase(pSkillInst, pInstaller, pMgr, pCfg, iTime, iRemainTime);
		break;
	case eSST_DirMove:
		pSS = new CDirMoveStateServer(pSkillInst, pInstaller, pMgr, pCfg, iTime, iRemainTime);
		break;
	case eSST_DeadBody:
		pSS = new CDeadBodyStateServer(pSkillInst, pInstaller, pMgr, pCfg, iTime, iRemainTime);
		break;
	default:
		{
			stringstream ExpStr;
			ExpStr << "创建特殊魔法状态对象时类型错误";
			GenErr(ExpStr.str());
			//return NULL;
		}
	}
	return pSS;
}

//bool CSpecialStateMgrServer::ExistState(const string& name)
//{
//	MapSpecialState::iterator itr = m_mapState.find(CSpecialStateCfgServer::Get(name));
//	return itr != m_mapState.end();
//}
//
//bool CSpecialStateMgrServer::ExistState(const string& name, const CFighterDictator* pInstaller)
//{
//	MapSpecialState::iterator itr = m_mapState.find(CSpecialStateCfgServer::Get(name));
//	if(itr != m_mapState.end())
//	{
//		return itr->second->m_pInstaller == pInstaller;
//	}
//	else
//	{
//		return false;
//	}
//}

bool CSpecialStateMgrServer::ExistStateByType(ESpecialStateType type)
{
	MapSSTypeBandle::iterator itr = m_mapStateTypeBundle.find(type);
	if(itr != m_mapStateTypeBundle.end())
	{
		return !itr->second.m_listState.empty();
	}
	else
	{
		return false;
	}
}

//void CSpecialStateMgrServer::ClearSpecialState(const string& name)
//{
//	CSpecialStateCfgServer* pCfg = CSpecialStateCfgServer::Get(name);
//	if(!pCfg) return;
//	MapSpecialState::iterator itrPSS = m_mapState.find(pCfg);
//	if(itrPSS != m_mapState.end())
//	{
//		itrPSS->second->DeleteSelf();
//	}
//}

void CSpecialStateMgrServer::ClearStateByType(ESpecialStateType type)
{
	MapSSTypeBandle::iterator itr = m_mapStateTypeBundle.find(type);
	if(itr != m_mapStateTypeBundle.end())
	{
		ListPSpecialState::iterator itrTB, itrTBBackup;
		for(itrTB = itr->second.m_listState.begin(); itrTB != itr->second.m_listState.end();)
		{
			itrTBBackup = itrTB;
			itrTBBackup++;
			//以下这句如果特殊状态的CancelDo()有内容，并且会直接或间接触发删除当前迭代器的下一个状态，
			//则需要改变遍历方法，参考TOtherStateMgrServer<CfgType, StateType>::MessageEvent(uint32 uEventID, CGenericTarget* pTrigger)
			(*itrTB)->CancelDo();
			(*itrTB)->DeleteSelf();
			itrTB = itrTBBackup;
		}
	}
}

//bool CSpecialStateMgrServer::ClearSpecialStateByCond(CStateCondBase* pStateCond, uint32 uId)
//{
//	CSpecialStateCfgServer::MapSpecialStateCfgById::iterator itr = CSpecialStateCfgServer::m_mapCfgById.find(uId);
//	if(itr != CSpecialStateCfgServer::m_mapCfgById.end())
//	{
//		MapSpecialState::iterator itrPSS = m_mapState.find(itr->second);
//		if(itrPSS != m_mapState.end())
//		{
//			if((*pStateCond)(itrPSS->second))
//				itrPSS->second->DeleteSelf();
//			else
//				return false;
//		}
//	}
//	return true;
//}
//
void CSpecialStateMgrServer::ClearAll()
{
	m_mapStateTypeBundle.clear();
	m_mapStateBundleByEvent.clear();
	CAllStateMgrServer* pAllMgr = GetAllMgr();
	for(MapSpecialState::iterator itr = m_mapState.begin(); itr != m_mapState.end(); ++itr)
	{
		//GetOwner()->UnRegistDistortedTick(itr->second);
		itr->second->CancelDo();
		//特殊状态CancelDo()不做任何事，所以不需要判断状态是否存在
		pAllMgr->OnDeleteState(itr->second->GetCfgSharedPtr()->GetId(), itr->second->m_uDynamicId);
		pAllMgr->DelStateByDId(itr->second->m_uDynamicId);

#ifdef COUT_STATE_INFO
		cout << "外部调用ClearAll，删除所有TriggerState。\n";
#endif
	}
	ClearMap(m_mapState);
}
//
//void CSpecialStateMgrServer::ClearAllByCond(CStateCondBase* pStateCond)
//{
//	MapSpecialState::iterator itrPSS = m_mapState.begin(), itrPSSBackup;
//	for(; itrPSS != m_mapState.end();)
//	{
//		itrPSSBackup = itrPSS;
//		itrPSSBackup++;
//		if ((*pStateCond)(itrPSS->second))
//		{		
//			itrPSS->second->DeleteSelf();
//		}
//		itrPSS = itrPSSBackup;
//	}
//}
//
//
//
//
//
//
//void CSpecialStateMgrServer::SyncAllState(CFighterDictator* pObserver, uint32 uNow)
//{
//	int32 iRemainTime;
//	for(MapSpecialState::iterator itr = m_mapState.begin(); itr != m_mapState.end(); itr++)
//	{
//		iRemainTime = itr->second->m_iRemainTime != -1 ?
//			itr->second->m_iRemainTime - (int32(uNow - itr->second->m_uStartTime) + 500) / 1000 : -1;
//
//		uint32 uId = itr->second->m_pCfg->GetId();
//		//m_pOwner->GetHandler()->OnSetState(uId, uId, 1, itr->second->m_iTime);
//		GetAllMgr()->OnSetState(uId, uId, 1, itr->second->m_iTime, itr->second->m_iRemainTime, pObserver);
//	}
//}
//
//bool CSpecialStateMgrServer::SerializeToDB(CStateDBDataSet* pRet, ICharacterMediatorCallbackHandler* pHandler, uint32 uFighterGlobalID, uint32 uNow)
//{
//	int32 iLeftTime;
//	for(MapSpecialState::iterator itr = m_mapState.begin(); itr != m_mapState.end(); ++itr)
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
//			itr->second->m_pCfg->GetName(), iLeftTime, 0, 0.0f,
//			0, 0, 0, pInst->m_uSkillLevel,
//			pInst->m_strSkillName.c_str(),
//			pInst->m_eAttackType,							//这个参数是直接复制指针的，要小心，考虑是否复制个副本
//			pInst->GetInterval(),
//			itr->second->m_pInstaller == m_pOwner);
//
//		//pHandler->AddStateToDB(uFighterGlobalID, eSGT_SpecialState, &aData);	
//		pRet->m_pStateVec->PushBack(pState);
//
//		cout << "特殊状态[" << itr->second->m_pCfg->GetName() << "]存入数据库\n";
//	}
//	return true;
//}
//
//bool CSpecialStateMgrServer::LoadFromDB(ICharacterMediatorCallbackHandler* pHandler, uint32 uFighterGlobalID)
//{
//	stringstream ExpStr;
//
//	ClearAll();
//
//	for(uint32 uStateCount = 0;; uStateCount++)
//	{
//		//CStateDBData aStateDBData;
//		//bool bRet = pHandler->ReadStateFromDB(uFighterGlobalID, eSGT_SpecialState, &aStateDBData);
//		CStateDBData* pStateDBData = NULL;
//		bool bRet = GetAllMgr()->ReadStateFromDB(pStateDBData, eSGT_SpecialState);
//
//		if(!bRet) break;
//
//		CStateDBData& aStateDBData = *pStateDBData;
//
//		CSpecialStateCfgServer* pCfg = CSpecialStateCfgServer::Get(aStateDBData.m_sName);
//		if(!pCfg)
//		{
//			ExpStr << "特殊状态读取错误：特殊状态[" << aStateDBData.m_sName << "]不存在于配置表\n";
//			GenErr(ExpStr.str());
//			return false;
//		}
//
//		CSkillInstServer* pSkillInst = new CSkillInstServer(aStateDBData.m_uSkillLevel,
//			aStateDBData.m_sSkillName, aStateDBData.m_eAttackType, aStateDBData.m_bIsDot);
//
//		int32 iLeftSecond = aStateDBData.m_iRemainTime == -1 ? -1 : aStateDBData.m_iRemainTime / 1000 + 1; //恢复要补完不足的，否则会少一次DOT
//		RestoreSpecialStateFromDB(pSkillInst, pCfg, aStateDBData.m_bFromEqualsOwner ? m_pOwner : NULL,
//			iLeftSecond);
//		cout << "从数据库读出特殊状态[" << pCfg->GetName() << "]\n";
//	}
//
//	return true;
//}
//
//
//CAllStateMgrServer* CSpecialStateMgrServer::GetAllMgr()
//{
//	return m_pOwner->GetAllStateMgr();
//}


bool CSpecialStateMgrServer::RestoreStateFromDB(CSkillInstServer* pSkillInst, const CSpecialStateCfgServerSharedPtr& pCfg,
	CFighterDictator* pInstaller, int32 iTime, int32 iRemainTime, uint32 uAccumulate, float fProbability)
{
	return RestoreStateFromDB(pSkillInst, pCfg, pInstaller, iTime, iRemainTime);
}

CStateDBData* CSpecialStateMgrServer::CreateStateDBData(CSpecialStateBase* pState, int32 iRemainTime, CSkillInstServer* pInst)
{
	return new CStateDBData(pState->GetCfgSharedPtr()->GetGlobalType(),
		pState->GetCfgSharedPtr()->GetName(), pState->m_iTime, iRemainTime, 0, 0.0f,
		0, 0, 0, pInst->GetSkillLevel(),
		pInst->GetSkillName().c_str(),
		pInst->GetAttackType(),							//这个参数是直接复制指针的，要小心，考虑是否复制个副本
		pInst->GetInterval(),
		pState->GetInstaller() == m_pOwner);
}

