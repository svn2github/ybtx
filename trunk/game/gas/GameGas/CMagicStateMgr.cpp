#include "stdafx.h"
#include "CMagicStateMgr.h"
#include "CAllStateMgr.h"
#include "CStateCondFunctor.h"
#include "CFighterDictator.h"
#include "CSkillInstServer.h"
#include "CStateDBData.h"
#include "CMagicStateInstServer.h"
#include "CMagicEffServer.h"
#include "ICharacterMediatorCallbackHandler.h"
#include "ErrLogHelper.h"
#include "BaseHelper.h"
#include "CoreCommon.h"
#include "CTriggerEvent.h"
#include "CConnServer.h"
#include "DebugHelper.h"
#include "TSqrAllocator.inl"
#include "CRivalStateMgr.h"
	

//以下是CMagicStateCategoryServer操作
pair<bool, MultiMapMagicState::iterator> CMagicStateCategoryServer::AddMS(CSkillInstServer* pSkillInst, CFighterDictator* pInstaller)			//用于增加一个魔法状态元素到本魔法状态种类的map
{
#ifdef COUT_STATE_INFO
	cout << m_pMgr->m_pOwner->GetEntityID() << ": 安装魔法状态：" << GetCfg()->GetName() << endl;
#endif

	CMagicStateServer *pMS = new CMagicStateServer(pSkillInst, pInstaller, this);
	MultiMapMagicState::iterator mtmapMSItr = m_mtmapMS.insert(make_pair(GetFighterEntityID(pInstaller), pMS));
	mtmapMSItr->second->m_mtmapMSItr = mtmapMSItr;
	//m_pMgr->m_mapMSByDynamicId.insert(make_pair(mtmapMSItr->second->m_uDynamicId, mtmapMSItr->second));
	mtmapMSItr->second->m_uDynamicId = m_pMgr->GetAllMgr()->AddStateByDId(mtmapMSItr->second);
	//cout << "服务器创建DynamicID为" << m_uDynamicId << "的魔法状态\n";
	//bool selfHasBeenDeleted = mtmapMSItr->second->Start();
	return make_pair(false, mtmapMSItr);
}

pair<bool, MultiMapMagicState::iterator> CMagicStateCategoryServer::AddMSFromDB(CSkillInstServer* pSkillInst, CMagicStateCascadeDataMgr* pCancelableDataMgr,
																				CMagicStateCascadeDataMgr* pDotDataMgr, CMagicStateCascadeDataMgr* pFinalDataMgr, CFighterDictator* pInstaller,  
																				uint32 uCount, int32 iTime, int32 iRemainTime)			//用于增加一个魔法状态元素到本魔法状态种类的map
{
#ifdef COUT_STATE_INFO
	cout << m_pMgr->m_pOwner->GetEntityID() << ": 恢复魔法状态：" << GetCfg()->GetName() << endl;
#endif

	CMagicStateServer *pMS = new CMagicStateServer(pSkillInst, pCancelableDataMgr, pDotDataMgr, pFinalDataMgr, uCount, pInstaller, this, iTime, iRemainTime);
	MultiMapMagicState::iterator mtmapMSItr = m_mtmapMS.insert(make_pair(GetFighterEntityID(pInstaller), pMS));
	mtmapMSItr->second->m_mtmapMSItr = mtmapMSItr;
	//m_pMgr->m_mapMSByDynamicId.insert(make_pair(mtmapMSItr->second->m_uDynamicId, mtmapMSItr->second));
	mtmapMSItr->second->m_uDynamicId = m_pMgr->GetAllMgr()->AddStateByDId(mtmapMSItr->second);
	//cout << "服务器创建DynamicID为" << m_uDynamicId << "的魔法状态\n";
	//bool selfHasBeenDeleted = mtmapMSItr->second->Start(true);
	return make_pair(false, mtmapMSItr);
}

CMagicStateCategoryServer::CMagicStateCategoryServer(const CMagicStateCfgServerSharedPtr& pCfg, CMagicStateMgrServer* pMgr)
:m_pCfg(new CMagicStateCfgServerSharedPtr(pCfg))
,m_pMgr(pMgr)
{
}

CMagicStateCategoryServer::~CMagicStateCategoryServer()
{
	ClearMap(m_mtmapMS);
	SafeDelete(m_pCfg);
}

CMagicStateCfgServerSharedPtr& CMagicStateCategoryServer::GetCfg()const
{
	return *m_pCfg;
}










//以下是CMagicStateMgrServer操作
bool CMagicStateMgrServer::SerializeToDB(CStateDBDataSet* pRet, ICharacterDictatorCallbackHandler* pHandler, uint32 uFighterGlobalID, uint64 uNow, uint32 uGrade)
{
	int32 iLeftTime;
	for(MapMagicStateCategory::iterator itr = m_mapMSCategory.begin(); itr != m_mapMSCategory.end(); ++itr)
	{
		SQR_TRY 
		{
			CMagicStateCfgServerSharedPtr& pCfg = itr->second->GetCfg();
			if((uint32)pCfg->GetNeedSaveToDBType() < uGrade) continue;
			MultiMapMagicState& mtmapMS = itr->second->m_mtmapMS;
			for(MultiMapMagicState::iterator itrMt = mtmapMS.begin(); itrMt != mtmapMS.end(); itrMt++)
			{
				CMagicStateServer* pMS = itrMt->second;
				if(itrMt->second->m_iRemainTime > -1)
				{
					iLeftTime = itrMt->second->m_iRemainTime * 1000 - int32(uNow - itrMt->second->m_uStartTime);
				}
				else
				{
					iLeftTime = -1;
				}
				uint32 uIDBase = pMS->m_uDynamicId * 3;

				CStateDBData* pState = new CStateDBData(pCfg->GetGlobalType(), pCfg->GetName(),				//这个参数是直接复制指针的，要小心，考虑是否复制个副本
					pMS->m_iTime, iLeftTime, pMS->m_uCount, 0.0f, 
					pCfg->GetCancelableMagicEff() ? uIDBase + 1 : 0,
					pCfg->GetDotMagicEff() ? uIDBase + 2 : 0,
					pCfg->GetFinalMagicEff() ? uIDBase + 3 : 0,
					pMS->m_pSkillInst->GetSkillLevel(),
					pMS->m_pSkillInst->GetSkillName().c_str(),		//这个参数是直接复制指针的，要小心，考虑是否复制个副本
					pMS->m_pSkillInst->GetAttackType(),
					pMS->m_pSkillInst->GetInterval(),
					pMS->GetInstaller() == m_pOwner);

				//pHandler->AddStateToDB(uFighterGlobalID, eSGT_MagicState, &aState);
				pRet->m_pStateVec->PushBack(pState);

#ifdef COUT_STATEDB_INFO
				cout << "魔法状态[" << pCfg->GetName() << "]存入数据库\n";
#endif

				if(pCfg->GetCancelableMagicEff())
					pMS->m_pCancelableDataMgr->SerializeToDB(pRet->m_pStoredObjVec, pHandler, uFighterGlobalID, uIDBase + 1);

				if(pCfg->GetDotMagicEff())
					pMS->m_pDotDataMgr->SerializeToDB(pRet->m_pStoredObjVec, pHandler, uFighterGlobalID, uIDBase + 2);

				if(pCfg->GetFinalMagicEff())
					pMS->m_pFinalDataMgr->SerializeToDB(pRet->m_pStoredObjVec, pHandler, uFighterGlobalID, uIDBase + 3);
			}
		}
		SQR_CATCH(exp)
		{
			CConnServer* pConnServer = m_pOwner->GetConnection();
			if (pConnServer)
				LogExp(exp, pConnServer);
			else
				LogExp(exp);
		}
		SQR_TRY_END;
	}
	return true;
}

bool CMagicStateMgrServer::LoadFromDB(ICharacterDictatorCallbackHandler* pHandler, uint32 uFighterGlobalID)
{
	//CStateDBDataSet* pRet = GetAllMgr()->GetStateDataMgrRet();



	//TCHAR sName[256];
	//int32 iRemainTime, iCount;
	//uint32 uSkillInstID, uCancelableInstID, uDotInstID, uFinalInstID;
	for(uint32 uStateCount = 0;; uStateCount++)
	{
		CSkillInstServer* pSkillInst = NULL;
		CMagicStateCascadeDataMgr *pCancelableDataMgr = NULL, *pDotDataMgr = NULL, *pFinalDataMgr = NULL;
		bool bCatched = false;
		//CStateDBData aStateDBData;
		CStateDBData* pStateDBData = NULL;
		//bool bRet = pHandler->ReadStateFromDB(uFighterGlobalID, eSGT_MagicState, &aStateDBData);
		bool bRet = GetAllMgr()->ReadStateFromDB(pStateDBData, eSGT_MagicState);

		if(!bRet) break;

		CStateDBData& aStateDBData = *pStateDBData;

		CMagicStateCfgServerSharedPtr pCfg;

		uint32 uCascadeMax = uint32(aStateDBData.m_iCount);

		CFighterDictator* pInstaller = aStateDBData.m_bFromEqualsOwner ? m_pOwner : NULL;

		SQR_TRY
		{
			if(aStateDBData.m_iCount <= 0)
			{
				stringstream ExpStr;
				ExpStr << "魔法状态[" << aStateDBData.m_sName << "]叠加层数必须大于0\n";
				GenErr("魔法状态数据库读取错误", ExpStr.str());
				return false;
			}
			pCfg = CMagicStateCfgServer::Get(aStateDBData.m_sName);
			if(!pCfg)
			{
				stringstream ExpStr;
				ExpStr << "魔法状态[" << aStateDBData.m_sName << "]不存在于配置表\n";
				GenErr("魔法状态数据库读取错误", ExpStr.str());
				return false;
			}

			//uint8 uSkillLevel;
			//TCHAR sSkillName[256];
			//EAttackType eAttackType;
			//bool bIsDot;

			//bRet = pHandler->ReadSkillInstFromDB(uSkillInstID, uSkillLevel, sSkillName, eAttackType, bIsDot);

			//if(!bRet)
			//{
			//	ExpStr << "魔法状态读取错误：魔法状态[" << aStateDBData.m_sName << "]没有ID为" << aStateDBData.m_uSkillInstID << "的技能上下文数据\n";
			//	GenErr(ExpStr.str());
			//	return false;
			//}

			uint32 uCreatorID = 0;
			if (aStateDBData.m_bFromEqualsOwner)
			{
				uCreatorID = m_pOwner->GetEntityID();
			}
			pSkillInst = CSkillInstServer::CreateOrigin(uCreatorID,aStateDBData.m_sSkillName,aStateDBData.m_uSkillLevel, aStateDBData.m_eAttackType);
			pSkillInst->SetInterval( aStateDBData.m_bIsDot);
			//CMagicStateCascadeDataMgr *pCancelableInst = NULL, *pDotInst = NULL, *pFinalInst = NULL;
			CreateDataMgrFromDB(pHandler, uFighterGlobalID, pCancelableDataMgr, pSkillInst, aStateDBData.m_uCancelableInstID,
				"可撤销", uCascadeMax, pInstaller, pCfg->GetCancelableMagicEff(), pCfg);
			CreateDataMgrFromDB(pHandler, uFighterGlobalID, pDotDataMgr, pSkillInst, aStateDBData.m_uDotInstID, "间隔", uCascadeMax,
				pInstaller, pCfg->GetDotMagicEff(), pCfg);
			CreateDataMgrFromDB(pHandler, uFighterGlobalID, pFinalDataMgr, pSkillInst, aStateDBData.m_uFinalInstID, "最终",
				uCascadeMax, pInstaller, pCfg->GetFinalMagicEff(), pCfg);

		}
		SQR_CATCH(exp)
		{
			if(pSkillInst)
				pSkillInst->DestroyOrigin();
			delete pCancelableDataMgr;
			delete pDotDataMgr;
			delete pFinalDataMgr;

			exp.AppendMsg("（配置表被修改而数据库未清空导致，不是bug）");

			CConnServer* pConnServer = m_pOwner->GetConnection();
			if (pConnServer)
				LogExp(exp, pConnServer);
			else
				LogExp(exp);

			bCatched = true;
		}
		SQR_TRY_END;
		if(!bCatched)
		{
			int32 iLeftSecond = aStateDBData.m_iRemainTime == -1 ? -1 : aStateDBData.m_iRemainTime / 1000 + 1; //恢复要补完不足的，否则会少一次DOT
			SQR_TRY
			{
				RestoreStateFromDB(pSkillInst, pCfg, aStateDBData.m_bFromEqualsOwner ? m_pOwner : NULL,
					aStateDBData.m_iTime, iLeftSecond, uCascadeMax,
					pCancelableDataMgr, pDotDataMgr, pFinalDataMgr);
#ifdef COUT_STATEDB_INFO
				cout << "从数据库读出魔法状态[" << pCfg->GetName() << "]\n";
#endif
			}
			SQR_CATCH(exp)
			{
				CConnServer* pConnServer = m_pOwner->GetConnection();
				if (pConnServer)
					LogExp(exp, pConnServer);
				else
					LogExp(exp);
			}
			SQR_TRY_END;
			if(pSkillInst)
				pSkillInst->DestroyOrigin();
		}
		SafeDelete(pStateDBData);
	}

	return true;
}

bool CMagicStateMgrServer::CreateDataMgrFromDB(ICharacterDictatorCallbackHandler* pHandler, uint32 uFighterGlobalID,
		CMagicStateCascadeDataMgr*& pCascadeDataMgr, CSkillInstServer* pSkillInst, uint32 uInstID, const string& sEffTitle,
		uint32 uCascadeMax, CFighterDictator* pInstaller, const CMagicEffServerSharedPtr& pMagicEff, const CMagicStateCfgServerSharedPtr& pCfg)
{
	bool bRet;

	if(pMagicEff)
	{
		pCascadeDataMgr = new CMagicStateCascadeDataMgr();

		uint32 uMopMax;
		SQR_TRY 
		{
			uMopMax = pMagicEff->GetMOPCount();
		}
		SQR_CATCH(exp)
		{
			string str = exp.ErrorMsg();
			str += " uMopMax = pMagicEff->GetMOPCount()";
			GenErr("魔法状态效果的存储对象数据库读取错误", str);
			return false;
		}
		SQR_TRY_END;

		CStoredObjDBData* pData = NULL;
		for(uint32 uCascadeIndex = 0, uMopIndex = uMopMax; ;)
		{
			SQR_TRY 
			{
				bRet = GetAllMgr()->ReadStoredObjFromDB(pData, uInstID);
			}
			SQR_CATCH(exp)
			{
				string str = exp.ErrorMsg();
				str += "bRet = GetAllMgr()->ReadStoredObjFromDB(pData, uInstID);";
				GenErr("魔法状态效果的存储对象数据库读取错误", str);
				return false;
			}
			SQR_TRY_END;

			if(bRet)
			{
				if(++uMopIndex > uMopMax)
				{
					if(++uCascadeIndex > uCascadeMax)
					{
						SQR_TRY 
						{
							SafeDelete(pData);
						}
						SQR_CATCH(exp)
						{
							string str = exp.ErrorMsg();
							str += "SafeDelete(pData)[1]";
							GenErr("魔法状态效果的存储对象数据库读取错误", str);
							return false;
						}
						SQR_TRY_END;
						stringstream ExpStr;
						SQR_TRY
						{
							ExpStr << "魔法状态[" << pCfg->GetName() << "]" << sEffTitle << "效果的存储对象读取错误：当前遍历的叠加层数" << uCascadeIndex << "超出从数据库读取的最大层数" << uCascadeMax << "\n";
						}
						SQR_CATCH(exp)
						{
							string str = exp.ErrorMsg();
							str += "ExpStr << [1]";
							GenErr("魔法状态效果的存储对象数据库读取错误", str);
							return false;
						}
						SQR_TRY_END;
						GenErr("魔法状态效果的存储对象数据库读取错误", ExpStr.str());
						return false;
					}

					if(uCascadeIndex != pData->m_uCascadeID)
					{
						SQR_TRY
						{
							SafeDelete(pData);
						}
						SQR_CATCH(exp)
						{
							string str = exp.ErrorMsg();
							str += "SafeDelete(pData)[2]";
							GenErr("魔法状态效果的存储对象数据库读取错误", str);
							return false;
						}
						SQR_TRY_END;
						stringstream ExpStr;
						SQR_TRY
						{
							ExpStr << "魔法状态[" << pCfg->GetName() << "]" << sEffTitle << "效果的存储对象读取错误：从数据库读取的叠加层数" <<  pData->m_uCascadeID << "不等于当前遍历的叠加层数" << uCascadeIndex << "\n";
						}
						SQR_CATCH(exp)
						{
							string str = exp.ErrorMsg();
							str += " ExpStr << [2]";
							GenErr("魔法状态效果的存储对象数据库读取错误", str);
							return false;
						}
						SQR_TRY_END;
						GenErr("魔法状态效果的存储对象数据库读取错误", ExpStr.str());
						return false;
					}

					uMopIndex = 1;
					SQR_TRY 
					{
						pCascadeDataMgr->AddCascade(pInstaller);
					}
					SQR_CATCH(exp)
					{
						string str = exp.ErrorMsg();
						str += "pCascadeDataMgr->AddCascade(pInstaller);";
						GenErr("魔法状态效果的存储对象数据库读取错误", str);
						return false;
					}
					SQR_TRY_END;
					SQR_TRY 
					{
						pCascadeDataMgr->SetItrLast();
					}
					SQR_CATCH(exp)
					{
						string str = exp.ErrorMsg();
						str += "pCascadeDataMgr->SetItrLast();";
						GenErr("魔法状态效果的存储对象数据库读取错误", str);
						return false;
					}
					SQR_TRY_END;
				}

				if(uMopIndex != pData->m_uMOPID)
				{	
					SQR_TRY
					{
						SafeDelete(pData);
					}
					SQR_CATCH(exp)
					{
						string str = exp.ErrorMsg();
						str += "SafeDelete(pData)[3]";
						GenErr("魔法状态效果的存储对象数据库读取错误", str);
					}
					SQR_TRY_END;
					stringstream ExpStr;
					SQR_TRY 
					{
						ExpStr << "魔法状态[" << pCfg->GetName() << "]" << sEffTitle << "效果的存储对象读取错误：从数据库读取的魔法操作下标" << pData->m_uMOPID << "不等于当前遍历的魔法操作下标" << uMopIndex << "\n";
					}
					SQR_CATCH(exp)
					{
						string str = exp.ErrorMsg();
						str += "ExpStr << [3]";
						GenErr("魔法状态效果的存储对象数据库读取错误", str);
					}
					SQR_TRY_END;
					GenErr("魔法状态效果的存储对象数据库读取错误", ExpStr.str());
					return false;
				}

				CMagicEffDataMgrServer* pDataMgr = NULL;
				SQR_TRY 
				{
					pDataMgr = pCascadeDataMgr->GetLastEffDataMgr();
				}
				SQR_CATCH(exp)
				{
					string str = exp.ErrorMsg();
					str += "pDataMgr = pCascadeDataMgr->GetLastEffDataMgr();";
					GenErr("魔法状态效果的存储对象数据库读取错误", str);
					return false;
				}
				SQR_TRY_END;
				SQR_TRY 
				{
					pDataMgr->AddMopData(CMagicOpStoredObj(pData->m_iMOPArg, pData->m_uMOPRet));
				}
				SQR_CATCH(exp)
				{
					string str = exp.ErrorMsg();
					str += "pDataMgr->AddMopData(CMagicOpStoredObj(pData->m_iMOPArg, pData->m_uMOPRet));";
					GenErr("魔法状态效果的存储对象数据库读取错误", str);
					return false;
				}
				SQR_TRY_END;

#ifdef COUT_STATEDB_INFO
				cout << "从数据库读出魔法状态" << sEffTitle << "效果第" << uCascadeIndex << "层状态第" << uMopIndex << "个魔法操作的存储对象\n";
#endif
				SQR_TRY
				{
					SafeDelete(pData);
				}
				SQR_CATCH(exp)
				{
					string str = exp.ErrorMsg();
					str += "SafeDelete(pData)[4]";
					GenErr("魔法状态效果的存储对象数据库读取错误", str);
					return false;
				}
				SQR_TRY_END;
			}
			else
			{
				//当存在0魔法操作个数时这个条件要改
				if(uCascadeIndex < uCascadeMax || uMopIndex < uMopMax)
				{
					stringstream ExpStr;
					SQR_TRY
					{
						//ExpStr << "魔法状态[" << pCfg->GetName() << "]" << sEffTitle << "效果的存储对象读取错误：从数据库读取的最大叠加层数和魔法操作下标" << pData->m_uCascadeID << "," << pData->m_uMOPID << "不等于当前遍历的" << uCascadeIndex << "," << uMopIndex << "\n";
						ExpStr << "魔法状态[" << pCfg->GetName() << "]" << sEffTitle 
							<< "效果的存储对象读取错误：从数据库读取的最大叠加层数和魔法操作下标" << pData->m_uCascadeID << ","
							<< pData->m_uMOPID << "不等于当前遍历的" << uCascadeIndex << "(<" << uCascadeMax << ")," 
							<< uMopIndex << "(<" << uMopMax << ")\n";
					}
					SQR_CATCH(exp)
					{
						string str = exp.ErrorMsg();
						str += "ExpStr << [4]";
						GenErr("魔法状态效果的存储对象数据库读取错误", str);
						return false;
					}
					SQR_TRY_END;
					GenErr("魔法状态效果的存储对象数据库读取错误", ExpStr.str());
					return false;
				}
				break;
			}
		}

		//pInst->SetIsCalc(false);
	}
	return true;
}

bool CMagicStateMgrServer::RestoreStateFromDB(CSkillInstServer* pSkillInst, const CMagicStateCfgServerSharedPtr& pCfg,
												   CFighterDictator* pInstaller, int32 iTime, int32 iRemainTime, uint32 uCount,
												   CMagicStateCascadeDataMgr* pCancelableDataMgr, CMagicStateCascadeDataMgr* pDotDataMgr, CMagicStateCascadeDataMgr* pFinalDataMgr)
{
	if (!m_pOwner)
	{
		GenErr("未调用SetOwner设置指向目标本身的指针");
		//return false;
	}

	//进行纵向查找，键值是魔法状态配置表指针
	MapMagicStateCategory::iterator map2dimMSItr = m_mapMSCategory.find(pCfg.get());
	if (map2dimMSItr == m_mapMSCategory.end())
	{
		//新建一个魔法状态种类
		CMagicStateCategoryServer* pMSCategory = new CMagicStateCategoryServer(pCfg, this);

		//新建一个魔法状态对象
		pair<bool, MultiMapMagicState::iterator> pr = pMSCategory->AddMSFromDB(pSkillInst, pCancelableDataMgr, pDotDataMgr, pFinalDataMgr, pInstaller, uCount, iTime, iRemainTime);

		if(pr.first) return true;
		MultiMapMagicState::iterator itrMS = pr.second;

		m_mapMSCategory.insert(make_pair(pCfg.get(), pMSCategory));

		itrMS->second->StartTime(true);

		//调用魔法状态种类的模型和特效回调
		GetAllMgr()->OnSetState(pCfg->GetId(), itrMS->second->m_uDynamicId, itrMS->second->m_uCount,
			itrMS->second->m_iTime, itrMS->second->m_iRemainTime, itrMS->second->m_pSkillInst->GetSkillLevel(),
			itrMS->second->GetInstallerID(),itrMS->second->GetCalcValue());

		itrMS->second->StartDoFromDB();

		return true;
	}
	else
	{
		switch(pCfg->GetCascadeType())
		{
		case eCT_Unique:
		case eCT_Central:
		case eCT_Share:
			{
				//唯一和集中状态不能出现重复名字的魔法状态
				stringstream ExpStr;
				ExpStr << "叠加类型为" << pCfg->GetCascadeType() << "的魔法状态[" << pCfg->GetName() << "]不能重复恢复\n";
				GenErr("从数据库恢复安装状态错误", ExpStr.str());
				//return false;
			}
			break;
		case eCT_Decentral:
			{
				//新建一个魔法状态对象
				pair<bool, MultiMapMagicState::iterator> pr = map2dimMSItr->second->AddMSFromDB(pSkillInst, pCancelableDataMgr, pDotDataMgr, pFinalDataMgr, pInstaller, uCount, iTime, iRemainTime);
				if(pr.first) return true;
				MultiMapMagicState::iterator itrMS = pr.second;

				itrMS->second->StartTime(true);

				//调用魔法状态种类的模型和特效回调
				GetAllMgr()->OnSetState(pCfg->GetId(), itrMS->second->m_uDynamicId,  itrMS->second->m_uCount,
					itrMS->second->m_iTime, itrMS->second->m_iRemainTime, itrMS->second->m_pSkillInst->GetSkillLevel(),
					itrMS->second->GetInstallerID(),itrMS->second->GetCalcValue());

				//以上构造函数包含执行魔法状态的立即操作，并设置计时器
				itrMS->second->StartDoFromDB();
				return true;
			}
			break;
		default:
			break;
		}
	}
	return false;
}

bool CMagicStateMgrServer::IgnoreImmuneSetupState(CSkillInstServer* pSkillInst, const CMagicStateCfgServerSharedPtr& pCfg, CFighterDictator* pInstaller)
{
	CAllStateMgrServer* pAllMgr = GetAllMgr();

	//按抗性计算
	if(pAllMgr->DecreaseStateResist(pCfg.get())) return false;

	//变形状态替换处理
	if(!pAllMgr->ReplaceModelState(pCfg.get())) return false;

	//进行纵向查找，键值是魔法状态配置表指针
	MapMagicStateCategory::iterator map2dimMSItr = m_mapMSCategory.find(pCfg.get());
	if (map2dimMSItr == m_mapMSCategory.end())
	{
		//新建一个魔法状态种类
		CMagicStateCategoryServer* pMSCategory = new CMagicStateCategoryServer(pCfg, this);

		//安装一个新的魔法状态时才会打印战斗信息, 叠加或替换时不打印
		pAllMgr->PrintFightInfo(true, pInstaller ? pInstaller->GetEntityID() : 0, pCfg.get());

		//新建一个魔法状态对象
		pair<bool, MultiMapMagicState::iterator> pr = pMSCategory->AddMS(pSkillInst,pInstaller);
		if(pr.first) return true;
		MultiMapMagicState::iterator itrMS = pr.second;

		m_mapMSCategory.insert(make_pair(pCfg.get(), pMSCategory));

		itrMS->second->StartTime();

		itrMS->second->CalculateMagicOpArg(itrMS->second->GetInstaller());

		//调用魔法状态种类的模型和特效回调
		//m_pOwner->GetHandler()->OnSetState(pCfg->GetId(), itrMS->second->m_uDynamicId, 1, itrMS->second->m_iTime);
		pAllMgr->OnSetState(pCfg->GetId(), itrMS->second->m_uDynamicId, 1, itrMS->second->m_iTime,
			itrMS->second->m_iRemainTime, itrMS->second->m_pSkillInst->GetSkillLevel(),
			itrMS->second->GetInstallerID(),itrMS->second->GetCalcValue());
		itrMS->second->StartDo();

		return true;
	}
	else
	{
		switch(pCfg->GetCascadeType())
		{
		case eCT_Unique:
			{
				////等级不低于原有魔法状态的等级则进入替换操作
				//if (pSkillInst->GetSkillLevel() >= map2dimMSItr->second->m_mtmapMS.begin()->second->m_uGrade)
				//{
				//	return map2dimMSItr->second->m_mtmapMS.begin()->second->Replace(pSkillInst, pSkillInst->GetSkillLevel(), pInstaller).second;
				//}
				CMagicStateServer* pState = map2dimMSItr->second->m_mtmapMS.begin()->second;
				if(pState->m_bCancellationDone)
				{
					//如果已撤销则直接替换
					return pState->Replace(pSkillInst, pSkillInst->GetSkillLevel(), pInstaller).second;
				}
				else if(pInstaller && pState->GetInstallerID() == pInstaller->GetEntityID())
				{
					//这里需要考虑安装者始终为NULL的情况，这种情况下eCT_Unique无法叠加
					return pState->Cascade(pSkillInst, pSkillInst->GetSkillLevel(), pInstaller).second;
				}
			}
			break;
		case eCT_Central:
		case eCT_Share:
			{
				CMagicStateServer* pState = map2dimMSItr->second->m_mtmapMS.begin()->second;
				if(pState->m_bCancellationDone)
				{
					//如果已撤销则直接替换
					return pState->Replace(pSkillInst, pSkillInst->GetSkillLevel(), pInstaller).second;
				}
				else
				{
					//进入叠加操作
					return pState->Cascade(pSkillInst, pSkillInst->GetSkillLevel(), pInstaller).second;
				}
			}
			break;
		case eCT_Decentral:
			{
				//进行横向查找，键值是施法者Fighter指针
				MultiMapMagicState& curMtmapMS = map2dimMSItr->second->m_mtmapMS;
				MultiMapMagicState::iterator mtmapMSItr = curMtmapMS.end();
				if(pInstaller)
				{
					mtmapMSItr = curMtmapMS.find(pInstaller->GetEntityID());
				}
				if (mtmapMSItr == curMtmapMS.end())
				{

					//安装一个新的魔法状态时才会打印战斗信息, 叠加或替换时不打印
					pAllMgr->PrintFightInfo(true, pInstaller ? pInstaller->GetEntityID() : 0, pCfg.get());

					//新建一个魔法状态对象
					pair<bool, MultiMapMagicState::iterator> pr = map2dimMSItr->second->AddMS(pSkillInst,pInstaller);
					if(pr.first) return true;
					MultiMapMagicState::iterator itrMS = pr.second;

					itrMS->second->StartTime();

					itrMS->second->CalculateMagicOpArg(itrMS->second->GetInstaller());

					//调用魔法状态种类的模型和特效回调
					//m_pOwner->GetHandler()->OnSetState(pCfg->GetId(), itrMS->second->m_uDynamicId, 1, itrMS->second->m_iTime);
					pAllMgr->OnSetState(pCfg->GetId(), itrMS->second->m_uDynamicId, 1, itrMS->second->m_iTime,
						itrMS->second->m_iRemainTime, itrMS->second->m_pSkillInst->GetSkillLevel(), 
						itrMS->second->GetInstallerID(),itrMS->second->GetCalcValue());

					//以上构造函数包含执行魔法状态的立即操作，并设置计时器
					itrMS->second->StartDo();
					return true;
				}
				else
				{
					CMagicStateServer* pState = mtmapMSItr->second;
					if(pState->m_bCancellationDone)
					{
						//如果已撤销则直接替换
						return pState->Replace(pSkillInst, pSkillInst->GetSkillLevel(), pInstaller).second;
					}
					else
					{
						//进入叠加操作
						return pState->Cascade(pSkillInst, pSkillInst->GetSkillLevel(), pInstaller).second;
					}
				}
			}
			break;
		default:
			break;
		}
	}

	return false;
}

bool CMagicStateMgrServer::SetupState(CSkillInstServer* pSkillInst, const CMagicStateCfgServerSharedPtr& pCfg, CFighterDictator* pInstaller)
{
	if (!m_pOwner)
	{
		GenErr("未调用SetOwner设置指向目标本身的指针");
		//return false;
	}

	//免疫计算
	CAllStateMgrServer* pAllMgr = GetAllMgr();
	if(pAllMgr->Immume(pCfg.get(), pSkillInst, pInstaller))
	{
		return false;
	}
	
	return IgnoreImmuneSetupState(pSkillInst, pCfg, pInstaller);
}

bool CMagicStateMgrServer::ResetTime(const CMagicStateCfgServerSharedPtr& pCfg, CFighterDictator* pInstaller)
{
	//进行纵向查找，键值是魔法状态配置表指针
	MapMagicStateCategory::iterator map2dimMSItr = m_mapMSCategory.find(pCfg.get());
	if (map2dimMSItr == m_mapMSCategory.end())
	{
		//不存在这个状态则直接返回
		return false;
	}
	else
	{
		switch(pCfg->GetCascadeType())
		{
		case eCT_Unique:
			{
				CMagicStateServer* pState = map2dimMSItr->second->m_mtmapMS.begin()->second;
				if(pState->m_bCancellationDone)
				{
					//如果已撤销则直接返回
					return false;
				}
				else if(pInstaller && pState->GetInstallerID() == pInstaller->GetEntityID())
				{
					//这里需要考虑安装者始终为NULL的情况，这种情况下eCT_Unique无法重置时间
					return pState->ResetTime();
				}
			}
			break;
		case eCT_Decentral:
			{
				//进行横向查找，键值是施法者Fighter指针
				MultiMapMagicState& curMtmapMS = map2dimMSItr->second->m_mtmapMS;
				MultiMapMagicState::iterator mtmapMSItr = curMtmapMS.end();
				if(pInstaller)
				{
					mtmapMSItr = curMtmapMS.find(pInstaller->GetEntityID());
				}
				if (mtmapMSItr == curMtmapMS.end())
				{
					//若找不到pInstaller的状态则直接返回
					return false;
				}
				else
				{
					CMagicStateServer* pState = mtmapMSItr->second;
					if(pState->m_bCancellationDone)
					{
						//如果已撤销则直接返回
						return false;
					}
					else
					{
						//重置时间
						return pState->ResetTime();
					}
				}
			}
			break;
		default:
			{
				CMagicStateServer* pState = map2dimMSItr->second->m_mtmapMS.begin()->second;
				if(pState->m_bCancellationDone)
				{
					//如果已撤销则直接返回
					return false;
				}
				else
				{
					//重置时间
					return pState->ResetTime();
				}
			}
		}
	}
	return false;
}

bool CMagicStateMgrServer::AddTime(const CMagicStateCfgServerSharedPtr& pCfg, int32 iTime)
{
	//进行纵向查找，键值是魔法状态配置表指针
	MapMagicStateCategory::iterator map2dimMSItr = m_mapMSCategory.find(pCfg.get());
	if (map2dimMSItr == m_mapMSCategory.end())
	{
		//不存在这个状态则直接返回
		return false;
	}
	else
	{
		MultiMapMagicState& curMtmapMS = map2dimMSItr->second->m_mtmapMS;
		for(MultiMapMagicState::iterator mtmapMSItr = curMtmapMS.begin(); mtmapMSItr != curMtmapMS.end(); ++mtmapMSItr)
		{
			CMagicStateServer* pState = mtmapMSItr->second;
			if(pState->m_bCancellationDone)
			{
				//如果已撤销则直接返回
				return false;
			}
			else
			{
				//重置时间
				return pState->AddTime(iTime);
			}
		}
	}
	return false;
}




bool CMagicStateMgrServer::ExistState(const string& name)
{
	return ExistState(CMagicStateCfgServer::Get(name));
}

bool CMagicStateMgrServer::ExistState(const CMagicStateCfgServerSharedPtr& pCfg)
{
	MapMagicStateCategory::iterator itr = m_mapMSCategory.find(pCfg.get());
	if(itr != m_mapMSCategory.end())
	{
		return !itr->second->m_mtmapMS.empty();
	}
	else
	{
		return false;
	}
}

bool CMagicStateMgrServer::ExistState(const CMagicStateCfgServerSharedPtr& pCfg, const CFighterDictator* pInstaller)
{
	if(!pInstaller) return false;
	MapMagicStateCategory::iterator itr = m_mapMSCategory.find(pCfg.get());
	if(itr != m_mapMSCategory.end())
	{
		return itr->second->m_mtmapMS.find(pInstaller->GetEntityID()) != itr->second->m_mtmapMS.end();
	}
	else
	{
		return false;
	}
}

//bool CMagicStateMgrServer::EraseState(const string& name, const CFighterDictator* pInstaller)
//{
//	if(!pInstaller) return false;
//	MapMagicStateCategory::iterator itr = m_mapMSCategory.find(CMagicStateCfgServer::Get(name));
//	if(itr != m_mapMSCategory.end())
//	{
//		MultiMapMagicState iterator itrMt = itr->second->m_mtmapMS.find(pInstaller->GetEntityID());
//		if(itrMt!= itr->second->m_mtmapMS.end())
//		{
//			Erase
//		}
//	}
//	else
//	{
//		return false;
//	}
//}

uint32 CMagicStateMgrServer::MagicStateCount(const string& name)
{
	MapMagicStateCategory::iterator itr = m_mapMSCategory.find(CMagicStateCfgServer::Get(name).get());
	if(itr != m_mapMSCategory.end())
	{
		uint32 uTotalCount = 0;
		MultiMapMagicState& mtmapMS = itr->second->m_mtmapMS;
		for(MultiMapMagicState::iterator itrMt = mtmapMS.begin(); itrMt != mtmapMS.end(); ++itrMt)
		{
			uTotalCount += itrMt->second->m_uCount;
		}
		return uTotalCount;
	}
	else
	{
		return 0;
	}
}

uint32 CMagicStateMgrServer::MagicStateCountOfInstaller(const string& name, const CFighterDictator* pInstaller)
{
	MapMagicStateCategory::iterator itr = m_mapMSCategory.find(CMagicStateCfgServer::Get(name).get());
	if(itr != m_mapMSCategory.end())
	{
		MultiMapMagicState& mtmapMS = itr->second->m_mtmapMS;
		MultiMapMagicState::iterator itrMt = mtmapMS.find(pInstaller->GetEntityID());
		if(itrMt != mtmapMS.end())
		{
			return itrMt->second->m_uCount;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return 0;
	}
}

int32 CMagicStateMgrServer::StateLeftTime(const string& name, const CFighterDictator* pInstaller)
{
	MapMagicStateCategory::iterator itr = m_mapMSCategory.find(CMagicStateCfgServer::Get(name).get());
	if(itr != m_mapMSCategory.end())
	{
		MultiMapMagicState& mtmapMS = itr->second->m_mtmapMS;
		switch(itr->second->GetCfg()->GetCascadeType())
		{
		case eCT_Central:
		case eCT_Unique:
		case eCT_Share:
			if(!mtmapMS.empty())
			{
				return mtmapMS.begin()->second->GetLeftTime();
			}
			else
			{
				return 0;
			}
			break;
		case eCT_Decentral:
			{
				if(pInstaller)
				{
					MultiMapMagicState::iterator itrMt = mtmapMS.find(pInstaller->GetEntityID());
					if(itrMt != mtmapMS.end())
					{
						return itrMt->second->GetLeftTime();
					}
					else
					{
						return 0;
					}
				}
				else
				{
					stringstream str;
					str << "分散叠加魔法状态[" << name << "]的StateLeftTime()函数的pInstaller参数不能为空\n";
					GenErr("分散叠加魔法状态的StateLeftTime()函数的pInstaller参数不能为空", str.str());
					return 0;
				}
			}
			break;
		default:
			{
				stringstream str;
				str << "魔法状态[" << name << "]的StateLeftTime()函数叠加类型[" << itr->second->GetCfg()->GetCascadeType() << "错误";
				GenErr("魔法状态的StateLeftTime()函数叠加类型错误", str.str());
				return 0;
			}
		}
	}
	else
	{
		return 0;
	}
}



//void CMagicStateMgrServer::AddStateTarget(CFighterDictator* pTarget)
//{
//	m_setTarget.insert(pTarget);
//}
//
//void CMagicStateMgrServer::DelStateTarget(CFighterDictator* pTarget)
//{
//	if (pTarget) m_setTarget.erase(pTarget);
//}
//
//
//void CMagicStateMgrServer::Offline()
//{
//	for (SetStateTarget::iterator itr = m_setTarget.begin(); itr != m_setTarget.end(); itr++)
//	{
//		(*itr)->GetAllStateMgr()->GetMagicStateMgrServer().Offline(m_pOwner);
//	}
//}

void CMagicStateMgrServer::OnCOREvent(CPtCORSubject* pSubject, uint32 uEvent,void* pArg)
{
	switch(uEvent)
	{
	case eCE_Offline:
		InstallerOffline(class_cast<CFighterDictator *>(pSubject));
		break;
	case eCE_Attack:
	case eCE_MoveBegan:
		{
			CAllStateMgrServer* pStateMgr = GetAllMgr();
			Ast(pStateMgr);
			pStateMgr->EraseState(eDST_FeignDeath);
			break;
		}
	default:
		break;
	}
}

void CMagicStateMgrServer::InstallerOffline(CFighterDictator * pInstaller)
{
	uint32 uInstallerID = 0;
	if (!pInstaller)
	{
		GenErr("CMagicStateMgrServer::Offline的参数指针为空");
		return;
	}
	else
	{
		uInstallerID = pInstaller->GetEntityID();
	}

	MapMagicStateCategory::iterator map2dimMSItr;

	for (map2dimMSItr = m_mapMSCategory.begin(); map2dimMSItr != m_mapMSCategory.end(); ++map2dimMSItr)
	{
		MultiMapMagicState& curMtmapMS = map2dimMSItr->second->m_mtmapMS;
		MtMapMagicStatePairItr mtmapMSPairItr = curMtmapMS.equal_range(uInstallerID);
		if (mtmapMSPairItr.first != curMtmapMS.end())
		{
			MultiMapMagicState::const_iterator itr = mtmapMSPairItr.first; 
			if(itr != mtmapMSPairItr.second)
			{
				//施法者下线则用用NULL指针替换map中的所有该由施法者产生魔法状态的键值
				MultiMapMagicState::iterator mtmapMSItr = curMtmapMS.insert(make_pair(0, itr->second));
				mtmapMSItr->second->m_mtmapMSItr = mtmapMSItr;
				mtmapMSItr->second->SetInstaller(NULL);
				itr->second->DetachInstaller();
				curMtmapMS.erase(itr->first);

				//pInstaller->GetAllStateMgr()->GetMagicStateMgrServer().DelStateTarget(m_pOwner);

			}
		}
	}
}



void CMagicStateMgrServer::ClearState(const CMagicStateCfgServerSharedPtr& pCfg)
{
	MapMagicStateCategory::iterator itr = m_mapMSCategory.find(pCfg.get());
	if(itr != m_mapMSCategory.end())
	{
		MultiMapMagicState& mtmapMS = itr->second->m_mtmapMS;
		MultiMapMagicState::iterator itrMt = mtmapMS.begin(), itrBackup;

		if(itrMt != mtmapMS.end())
			GetAllMgr()->PrintFightInfo(false, itrMt->second->GetInstallerID(), pCfg.get());
		
		for(;itrMt != mtmapMS.end();)
		{
			itrBackup = itrMt;
			itrBackup++;
			CMagicStateServer* pMagicState = itrMt->second;
			uint32 uDynamicId = pMagicState->m_uDynamicId;
			bool selfHasBeenDeleted = pMagicState->CancelDo();
			if(selfHasBeenDeleted) return;				//这句的作用为考虑CancelDo()可能把m_pOwner弄死导致itrMt悬空
			//if(itrMt->second.DeleteSelf()) break;
			//m_mapMSByDynamicId.erase(pMagicState->m_uDynamicId);
			GetAllMgr()->DelStateByDId(uDynamicId);
			pMagicState->DetachInstaller();
			//GetOwner()->UnRegistDistortedTick(pMagicState);
			itrMt = itrBackup;
		}

#ifdef COUT_STATE_INFO
		cout << "外部调用ClearState，删除自己：" << pCfg->GetName() << endl;
#endif

		EraseMapNode(m_mapMSCategory, itr);
	}
}

void CMagicStateMgrServer::ClearAll()
{
	//状态的CancelDo()函数里不能有直接或间接调用自己状态相关的InstallerOffline操作，否则会因为结点重置导致迭代器失效而无法继续遍历下一个结点
	for(MapMagicStateCategory::iterator itr = m_mapMSCategory.begin(); itr != m_mapMSCategory.end(); ++itr)
	{
		//如果当前itr的CancelDo()会DeleteSelf()当前itr的MagicState，则有导致当前itr失效的危险，且这种危险无法用事先保存itr下一个位置的副本的办法排除，因为CancelDo()现在已经可以出现删除下一个位置的MagicState的情况，这样有可能导致事先保存的itr下一个位置的副本也失效。
		MultiMapMagicState& mtmapMS = itr->second->m_mtmapMS;
		MultiMapMagicState::iterator itrMt = mtmapMS.begin();
		for(; itrMt != mtmapMS.end(); itrMt++)
		{
			uint32 uDynamicId = itrMt->second->m_uDynamicId;
			itrMt->second->CancelDo();			
			itrMt->second->DetachInstaller();
			GetAllMgr()->DelStateByDId(uDynamicId);

			//GetOwner()->UnRegistDistortedTick(itrMt->second);


			//itrBackup = itrMt;
			//itrBackup++;
			//if(itrMt->second.DeleteSelf()) break;
			//itrMt = itrBackup;

			//这里可能需要考虑下一次CancelDo里调用的ExistState可能会用迭代器是否存在来判断至今已被删除的状态是否存在，
			//如果这种判断在当前状况下要求必须范围为false，则这里需要修改迭代器标志并重写ExistState的判断方式为判断迭代器标志或直接删除迭代器
		}
	}
	//cout << "外部调用ClearAll，删除所有MagicState。\n";
	//m_mapMSByDynamicId.clear();
	ClearMap(m_mapMSCategory);
	m_pRivalStateMgr->ClearAll();
}

void CMagicStateMgrServer::ClearAllByCond(CStateCondBase * pStateCond)
{
	//状态的CancelDo()函数里不能有直接或间接调用自己状态相关的InstallerOffline操作，否则会因为结点重置导致迭代器失效而无法继续遍历下一个结点
	MapMagicStateCategory::iterator itr = m_mapMSCategory.begin();//, itrBackup;
	for(; itr != m_mapMSCategory.end(); )
	{
		//这里有个无解的问题，如果DeleteSelf()当前itr的MagicState，则由于DeleteSelf()重新调整了容器map导致当前itr失效，
		//若CancelDo()又正好导致容器map内的下一个MagicState删除（比如下一个MagicState是由当前的MagicState的可撤销魔法效果安装的）
		//则即使用itrBackup保存itr的下一个位置的迭代器，这个迭代器也失效了，由于无法预知itr之后的连续几个MagicState会被删除，
		//所以除非将MagicState对象删除与map结点删除分离在两个循环里

		MapMagicStateCategory::iterator itrBackup = itr;
		itrBackup++;
		bool bSelfCategoryDeleted = false;

		CMagicStateCfgServerSharedPtr& uTempStateCfg = itr->second->GetCfg();

		MultiMapMagicState& mtmapMS = itr->second->m_mtmapMS;
		MultiMapMagicState::iterator itrMt = mtmapMS.begin();//, itrMtBackup;
		for(; itrMt != mtmapMS.end();)
		{
			//itrMtBackup = itrMt;
			//itrMtBackup++;
			CMagicStateServer* pState = itrMt->second;
			if((*pStateCond)(pState))
			{
				//这里的CancelDo不允许直接或间接（比如触发事件导致的）删除本Category中别人的状态，否则会导致itr迭代器失效
				MultiMapMagicState::iterator itrMtBackup;
				itrMtBackup = itrMt;
				++itrMtBackup;
				if(pState->CancelDo())
				{
					if(m_mapMSCategory.find(uTempStateCfg.get()) == m_mapMSCategory.end())
					{
						itr = itrBackup;
						bSelfCategoryDeleted = true;
						break;
					}
					itrMt = itrMtBackup;
					continue;
				}
				pState->PrepareDeleteSelf();
				if(itrMt != itr->second->m_mtmapMS.end())
				{
					itr->second->m_mtmapMS.erase(itrMt++);
				}
				delete pState;	//这句要不要考虑移到if前面？
			}
			else
			{
				++itrMt;
			}
			//itrMt = itrMtBackup;
		}
		if(!bSelfCategoryDeleted)
		{
			if(itr->second->m_mtmapMS.empty())
			{
				delete itr->second;
				m_mapMSCategory.erase(itr++);
			}
			else
			{
				++itr;
			}
		}

		//itr = itrBackup;
	}
}

bool CMagicStateMgrServer::ExistStateByCond(CStateCondBase* pStateCond)
{
	MapMagicStateCategory::iterator itr = m_mapMSCategory.begin();//, itrBackup;
	for(; itr != m_mapMSCategory.end(); ++itr)
	{
		MultiMapMagicState& mtmapMS = itr->second->m_mtmapMS;
		MultiMapMagicState::iterator itrMt = mtmapMS.begin();//, itrMtBackup;
		for(; itrMt != mtmapMS.end(); ++itrMt)
		{
			CMagicStateServer* pState = itrMt->second;
			if((*pStateCond)(pState))
			{
				return true;
			}
		}
	}
	return false;
}

void CMagicStateMgrServer::ReflectAllByCond(CStateCondBase * pStateCond)
{
	MapMagicStateCategory::iterator itr = m_mapMSCategory.begin();
	for(; itr != m_mapMSCategory.end(); ++itr)
	{
		MultiMapMagicState& mtmapMS = itr->second->m_mtmapMS;
		MultiMapMagicState::iterator itrMt = mtmapMS.begin();
		for(; itrMt != mtmapMS.end();++itrMt)
		{
			CMagicStateServer* pState = itrMt->second;
			if((*pStateCond)(pState))
			{
				CFighterDictator* pInstaller = pState->GetInstaller();
				CFighterDictator* pOwner = pState->GetOwner();
				if (pInstaller && pOwner && pInstaller->CppIsAlive() && pInstaller != pOwner)
				{
					//CFPos posOwner		= pState->GetOwner()->GetPixelPos();
					//CFPos posInstaller	= pInstaller->GetPixelPos();
					//float x = posOwner.x - posInstaller.x;
					//float y = posOwner.y - posInstaller.y;
					//float fDistance = sqrt(x*x + y*y)/eGridSpanForObj;
					
					float fDistance = pOwner->GetFighterDistInGrid(pInstaller);

					if (fDistance <= 20)	// 20格内反射
					{		
						//pState->m_pSkillInst->ChangeToSkillInst();
						pInstaller->GetAllStateMgr()->GetMagicStateMgrServer()->SetupState(pState->m_pSkillInst, pState->GetCfgSharedPtr(), pState->GetOwner());
						//pState->m_pSkillInst->RevertInstType();
					}			
				}
			}
		}
	}
}

//bool CMagicStateMgrServer::ClearStateByCondAndDynamicId(CStateCondBase* pStateCond, uint32 uDynamicId)
//{
//	MapMagicStateByDynamicId::iterator itr = m_mapMSByDynamicId.find(uDynamicId);
//	if(itr!=m_mapMSByDynamicId.end())
//	{
//		CMagicStateServer* pState = itr->second;
//		if(itr != m_mapMSByDynamicId.end())
//		{
//			if((*pStateCond)(pState))
//			{
//				if(pState->CancelDo()) return true;
//				pState->DeleteSelf();
//				//m_mapMSByDynamicId.erase(DynamicId);
//				return false;
//			}
//		}
//	}
//	return false;
//}

bool CMagicStateMgrServer::ClearStateByCond(CStateCondBase* pStateCond, uint32 uId)
{
	bool bEraseDSRSuccess = false;
	CMagicStateCfgServerSharedPtr& pCfg = CMagicStateCfgServer::GetById(uId);
	MapMagicStateCategory::iterator itr = m_mapMSCategory.find(pCfg.get());
	if(itr != m_mapMSCategory.end())
	{
		MultiMapMagicState& mtmapMS = itr->second->m_mtmapMS;
		MultiMapMagicState::iterator itrMt = mtmapMS.begin(), itrBackup;
		
		if(itrMt != mtmapMS.end())
			GetAllMgr()->PrintFightInfo(false, itrMt->second->GetInstallerID(), pCfg.get());
		
		for(;itrMt != mtmapMS.end();)
		{
			//itrBackup = itrMt;
			//itrBackup++;
			CMagicStateServer* pState = itrMt->second;
			if((*pStateCond)(pState))
			{
				bEraseDSRSuccess = true;
				uint32 uDynamicId = pState->m_uDynamicId;
				bool selfHasBeenDeleted = pState->CancelDo();
				if(selfHasBeenDeleted) return bEraseDSRSuccess;				//这句的作用为考虑CancelDo()可能把m_pOwner弄死导致itrMt悬空
				//if(pState.DeleteSelf()) break;
				//m_mapMSByDynamicId.erase(pState->m_uDynamicId);
				GetAllMgr()->DelStateByDId(uDynamicId);
				pState->DetachInstaller();
				//GetOwner()->UnRegistDistortedTick(pState);
				delete pState;
				mtmapMS.erase(itrMt++);
			}
			else
			{
				++itrMt;
			}	
			//itrMt = itrBackup;
		}
		if(mtmapMS.empty()) 
		{
#ifdef COUT_STATE_INFO
			cout << "外部调用ClearStateByCond，删除自己：" << pCfg->GetName() << endl;
#endif

			EraseMapNode(m_mapMSCategory, itr);
		}
	}
	return bEraseDSRSuccess;
}

//bool CMagicStateMgrServer::ExistState(uint32 DynamicId)
//{
//	MapMagicStateByDynamicId::iterator itr = m_mapMSByDynamicId.find(DynamicId);
//	if(itr == m_mapMSByDynamicId.end())
//	{
//		return false;
//	}
//	else
//	{
//		return true;
//	}
//}

//void CMagicStateMgrServer::SetOwner(CFighterDictator* pOwner)
//{
//	m_pOwner = pOwner;
//	//m_pOwner->Attach(this, eCE_Die);
//}

void CMagicStateMgrServer::UnloadFighter()
{
	m_pOwner = NULL;
	//m_pOwner->Detach(this, eCE_Die);
}

//void CMagicStateMgrServer::SyncAllState(CFighterDictator* pObserver, uint64 uNow)
//{
//	int32 iRemainTime;
//	//遍历整个MagicStateMap
//	CAllStateMgrServer* pAllMgr = GetAllMgr();
//	for(MapMagicStateByDynamicId::iterator itr = m_mapMSByDynamicId.begin();
//		itr != m_mapMSByDynamicId.end(); itr++)
//	{
//		CMagicStateServer* pMagicState = itr->second;
//		Ast(pMagicState);
//		iRemainTime = pMagicState->m_iRemainTime != -1 ?
//			pMagicState->m_iRemainTime - (int32(uNow - pMagicState->m_uStartTime) + 500) / 1000 : -1;
//
//		Ast(iRemainTime >= -1);
//		if(iRemainTime < -1) continue;
//		pAllMgr->OnSetState(pMagicState->m_pMSCategory->m_pCfg->GetId(), 
//			pMagicState, pMagicState->m_uCount, pMagicState->m_iTime, iRemainTime, pObserver);
//	}
//}

void CMagicStateMgrServer::SyncAllState(CFighterDictator* pObserver, uint64 uNow)
{
	int32 iRemainTime;
	CAllStateMgrServer* pAllMgr = GetAllMgr();
	for(MapMagicStateCategory::iterator itr = m_mapMSCategory.begin(); itr != m_mapMSCategory.end(); ++itr)
	{
		MultiMapMagicState& mtmapMS = itr->second->m_mtmapMS;
		MultiMapMagicState::iterator itrMt = mtmapMS.begin();
		for(; itrMt != mtmapMS.end(); itrMt++)
		{
			CMagicStateServer* pMagicState = itrMt->second;
			Ast(pMagicState);
			iRemainTime = pMagicState->m_iRemainTime != -1 ?
				pMagicState->m_iRemainTime - (int32(uNow - pMagicState->m_uStartTime) + 500) / 1000 : -1;

			pAllMgr->OnSetState(pMagicState->m_pMSCategory->GetCfg()->GetId(), 
				pMagicState->m_uDynamicId, pMagicState->m_uCount, pMagicState->m_iTime, iRemainTime,
				pMagicState->m_pSkillInst->GetSkillLevel(), pObserver);
		}
	}
}

CAllStateMgrServer* CMagicStateMgrServer::GetAllMgr()
{
	return m_pOwner->GetAllStateMgr();
}

bool CMagicStateMgrServer::CanDecStateCascade(const CMagicStateCfgServerSharedPtr& pCfg, uint32 uCascade)
{
	if(uCascade == 0) return true;
	uint32 uAllCascadeCount = 0;
	MapMagicStateCategory::iterator itr = m_mapMSCategory.find(pCfg.get());
	if(itr != m_mapMSCategory.end())
	{
		MultiMapMagicState& mtmapMS = itr->second->m_mtmapMS;
		for(MultiMapMagicState::iterator itrMt = mtmapMS.begin(); itrMt != mtmapMS.end(); itrMt++)
		{
			uAllCascadeCount += itrMt->second->m_uCount;
			if(uAllCascadeCount >= uCascade) return true;
		}
	}

	return false;
}

bool CMagicStateMgrServer::DecStateCascade(const CMagicStateCfgServerSharedPtr& pCfg, uint32 uCascade, bool bInCancel)
{
	if(uCascade == 0) return true;

	MapMagicStateCategory::iterator itr = m_mapMSCategory.find(pCfg.get());
	if(itr != m_mapMSCategory.end())
	{
		MultiMapMagicState& mtmapMS = itr->second->m_mtmapMS;
		MultiMapMagicState::iterator itrMt = mtmapMS.begin(), itrBackup;
		for(;itrMt != mtmapMS.end();)
		{
			itrBackup = itrMt;
			itrBackup++;
			pair<bool, bool> bPairStatus = itrMt->second->CancelCascade(uCascade, bInCancel);
			if(bPairStatus.first) return true;				//这句的作用为考虑CancelCascade()可能把m_pOwner弄死导致itrMt悬空
			itrMt = itrBackup;
		}
	}

	return false;
}


CMagicStateMgrServer::CMagicStateMgrServer(CFighterDictator* pOwner)
:m_pOwner(pOwner)/*, m_uMaxDynamicId(eSIC_DynamicMagicState)*/
{
	m_pRivalStateMgr = new CRivalStateMgr();
	//m_setTarget.clear();
}

CMagicStateMgrServer::~CMagicStateMgrServer()
{
	ClearAll();
	UnloadFighter();
	delete m_pRivalStateMgr;
}
