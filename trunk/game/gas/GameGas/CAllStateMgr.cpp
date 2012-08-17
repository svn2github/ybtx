#include "stdafx.h"
#include "CAllStateMgr.h"
#include "PtrlGas2GacCPPDef.h"
#include "CQuickRand.h"
#include "CSkillInstServer.h"
#include "BaseHelper.h"
#include "ICharacterMediatorCallbackHandler.h"
//#include "CAppServer.h"
#include "CMagicStateMgr.h"
#include "CTriggerStateCfg.h"
#include "CTriggerStateMgr.h"
#include "CDamageChangeStateCfg.h"
#include "CDamageChangeStateMgr.h"
#include "CCumuliTriggerStateCfg.h"
#include "CCumuliTriggerStateMgr.h"
#include "CSpecialStateMgr.h"
#include "COtherStateMgr.inl"
#include "CTriggerableStateMgr.inl"
#include "LoadSkillCommon.h"
#include "CEntityServerManager.h"
#include "CSizeChangeMgr.h"
#include "CCharacterDictator.h"
#include "CSyncVariantServerHolder.h"
#include "CStateSync.h"
#include "CSyncVariantServer.h"
#include "TIDPtrMap.inl"
#include "TSqrAllocator.inl"
#include "CGenericTarget.h"
#include "CSkillMgrServer.h"

CAllStateMgrServer::CAllStateMgrServer(CFighterDictator* pOwner)
: m_pOwner(pOwner),
m_pRet(NULL)
{
	m_pMagicStateMgrServer = new CMagicStateMgrServer(pOwner);
	m_pTriggerStateMgrServer = new CTriggerStateMgrServer(pOwner);
	m_pDamageChangeStateMgrServer = new CDamageChangeStateMgrServer(pOwner);
	m_pCumuliTriggerStateMgrServer = new CCumuliTriggerStateMgrServer(pOwner);
	m_pSpecialStateMgrServer = new CSpecialStateMgrServer(pOwner);
	m_pSizeChangeMgr = new CSizeChangeMgr(m_pOwner);
	ClearModelStateId();
}

CAllStateMgrServer::~CAllStateMgrServer()
{
	SafeDelete(m_pMagicStateMgrServer);
	SafeDelete(m_pTriggerStateMgrServer);
	SafeDelete(m_pDamageChangeStateMgrServer);
	SafeDelete(m_pCumuliTriggerStateMgrServer);
	SafeDelete(m_pSpecialStateMgrServer);
	SafeDelete(m_pSizeChangeMgr);
}

void CAllStateMgrServer::SyncClearState(CFighterDictator* pObserver)
{
	OnClearState(pObserver);
}

void CAllStateMgrServer::SyncAllState(CFighterDictator* pObserver)
{
	uint64 uNow = pObserver->GetDistortedFrameTime();

	//SyncClearState(pObserver);
	GetMagicStateMgrServer()->SyncAllState(pObserver, uNow);
	GetTriggerStateMgrServer()->SyncAllState(pObserver, uNow);
	GetDamageChangeStateMgrServer()->SyncAllState(pObserver, uNow);
	GetSpecialStateMgrServer()->SyncAllState(pObserver, uNow);
}

bool CAllStateMgrServer::ReplaceModelRuler(CBaseStateCfgServer* pCfgNew,CBaseStateCfgServer* pCfgOld)
{
	if(!pCfgOld->GetDecrease() && pCfgNew->GetDecrease())		//原来的是正面，现在的是负面
		return false;
	else
		return true;
}

bool CAllStateMgrServer::ReplaceModelState(CBaseStateCfgServer* pCfg)
{
	if(!pCfg->GetModelStr().empty())				//是否是模型状态
	{
		if(m_uModelStateId != 0)				//标志位有模型
		{
			CBaseStateCfgServer* pOldCfg = class_cast<CBaseStateCfgServer*>(CBaseStateCfgServer::GetByGlobalId(m_uModelStateId));
			if(ReplaceModelRuler(pCfg, pOldCfg))	//符合替换规则
			{
				if(!CancelState(m_uModelStateId))	//能撤销（则撤销并继续安装）
				{
					return true;
				}
				else											//不能撤销（则不继续安装）
				{
					return false;
				}
			}
			else										//不符合替换规则（则不继续安装）
			{
				return false;
			}
		}
	}
	return true;
}

void CAllStateMgrServer::SetModelStateId(uint32 uId)
{
	if(m_uModelStateId != 0)
		GenErr("变形状态ID不为0");
	m_uModelStateId = uId;
	m_uModelStateCascde = 1;
}

void CAllStateMgrServer::ClearModelStateId()
{
	m_uModelStateId = 0;
	m_uModelStateCascde = 0;
}

void CAllStateMgrServer::AddModelStateId(uint32 uId)
{
	if(m_uModelStateId != 0 && m_uModelStateId != uId)
		GenErr("变形状态ID为0和uId以外的值");
	m_uModelStateId = uId;
	++m_uModelStateCascde;
}

void CAllStateMgrServer::SubModelStateId()
{
	if(m_uModelStateCascde == 0)
		GenErr("变形状态模型层数已经为0了");
	--m_uModelStateCascde;
	if(m_uModelStateCascde == 0)
		m_uModelStateId = 0;
}

//uint32 uCmdOrder = 0;


//状态同步

void CAllStateMgrServer::OnClearState(CFighterDictator* pObserver)
{
#ifdef SEND_STATE_BY_VAR
	//CCharacterDictator* pCharacter = m_pOwner->GetCharacter();
	//CSyncVariantServerHolder* SyncVariantHolder = pCharacter->GetSyncVariantHolder();
	//SyncVariantHolder->SetNumber("BaseState/",uId);
	//SyncVariantHolder->SetNumber("BaseState/CastType",(uint32)(eCastType));
	//SyncVariantHolder->SetNumber("BaseState/CastTime",fCastTime);
	//SyncVariantHolder->SetNumber("BaseState/ExpiredTime",fExpiredTime);
	//SyncVariantHolder->SetNumber("BaseState/TargetID",uTargetID);
	//SyncVariantHolder->SetNumber("BaseState/IsSync",bIsSync);

#else
	CGas2GacCPP_OnClearState Cmd;
	Cmd.uObjID = m_pOwner->GetEntityID();

	//++uCmdOrder;
	//Cmd.uCmdOrder = uCmdOrder;

	//cout << "SendCmdToGac[" << Cmd.uCmdOrder << "](CGas2GacCPP_OnClearState) by ";

	//cout << "pObserver = " << Cmd.uSendBy << "\n";
	if(!pObserver)
	{
		//Cmd.uSendBy = m_pOwner->GetEntityID();
		m_pOwner->SendCmdToGac(&Cmd);
	}
	else
	{
		//Cmd.uSendBy = pObserver->GetEntityID();
		pObserver->SendCmdToGac(&Cmd, eOnlyToDirector);
	}
#endif
}

void CAllStateMgrServer::OnDeleteState( uint32 uCategoryId, uint32 uDynamicId )
{
	CBaseStateServer* pState;
	if(!m_idmapStateByDId.Get(pState, uDynamicId))
	{
		stringstream sErr;
		sErr << "OnDeleteState(" << uCategoryId << ", " << uDynamicId << ")编号为" << uDynamicId << "的["
			<< CBaseStateCfgServer::GetByGlobalId(uCategoryId)->GetName() << "]状态不存在于IDMAP中（不一定是bug）\n";
		CConnServer* pConnServer = m_pOwner->GetConnection();
		if (pConnServer)
			LogErr("OnDeleteState状态不存在于IDMAP中（不一定是bug）", sErr.str().c_str(), pConnServer);
		else
			LogErr("OnDeleteState状态不存在于IDMAP中（不一定是bug）", sErr.str().c_str());
		return;
	}

	if(!pState)
	{
		stringstream sErr;
		sErr << "OnDeleteState(" << uCategoryId << ", " << uDynamicId << ")编号为" << uDynamicId << "的["
			<< CBaseStateCfgServer::GetByGlobalId(uCategoryId)->GetName() << "]状态指针为空（不一定是bug）\n";
		CConnServer* pConnServer = m_pOwner->GetConnection();
		if (pConnServer)
			LogErr("OnDeleteState状态在IDMAP中的指针为空（不一定是bug）", sErr.str().c_str(), pConnServer);
		else
			LogErr("OnDeleteState状态在IDMAP中的指针为空（不一定是bug）", sErr.str().c_str());
		return;
	}

		
	if(!pState->GetCfg()->GetNeedSync())
		return;

#ifdef SEND_STATE_BY_VAR
	CStateVaraintPointerHolder* pVarPtrHolder = pState->GetVaraintPointerHolder();
	Ast(pVarPtrHolder);
	if(!pVarPtrHolder)
		return;

	//cout << "[" << CAppServer::Inst()->GetFrameTime() << "]OnDeleteState(" << uCategoryId << ", " << uDynamicId << ")\n";

	//用VariantHolder路径设置结点
	//CCharacterDictator* pCharacter = m_pOwner->GetCharacter();
	//CSyncVariantServerHolder* SyncVariantHolder = pCharacter->GetSyncVariantHolder();

	//stringstream sStateKey, sStateId, sStateIsFinish;
	//string sStateRootKey = "State";
	//sStateId << "s" << uDynamicId;
	//sStateKey << sStateRootKey << "/" << sStateId.str();
	//sStateIsFinish << sStateKey.str() << "/zzzzzStateIsFinish";

	//SyncVariantHolder->AddSubTree(sStateRootKey.c_str(), "AllState", sStateId.str().c_str());
	//pVarPtrHolder->m_pCategoryId->SetNumber(uCategoryId);	//不会变，可以考虑不设
	//pVarPtrHolder->m_pDynamicId->SetNumber(uDynamicId);		//不会变，可以考虑不设
	//pVarPtrHolder->m_pStateIsFinish = SyncVariantHolder->SetNumber(sStateIsFinish.str().c_str(), true);

	//SyncVariantHolder->DelMember(sStateKey.str().c_str());

	uint16 uCategoryId16 = uint16(uCategoryId);
	if(uint32(uCategoryId16) != uCategoryId)
	{
		ostringstream strm;
		strm << uCategoryId;
		LogErr("state CategoryId is great than 65535", strm.str());
	}
	//用Variant指针设置结点
	pVarPtrHolder->m_pCategoryId->SetNumber(uCategoryId16);	//不会变，可以考虑不设
	pVarPtrHolder->m_pDynamicId->SetNumber(uDynamicId);		//不会变，可以考虑不设
	Ast(pVarPtrHolder->m_pStateSubTree);
	pVarPtrHolder->m_pStateIsFinish = pVarPtrHolder->m_pStateSubTree->SetMember("zzzzzStateIsFinish");
	pVarPtrHolder->m_pStateIsFinish->SetNumber(true);

	static TCHAR chrStateId[24] = "s";
	//_itoa(int32(uDynamicId), chrStateId + 1, 10);
	sprintf(chrStateId + 1, "%u", uDynamicId);

	CSyncVariantServer* pStateRoot = pVarPtrHolder->m_pStateSubTree->GetParent();
	pStateRoot->DelMember(chrStateId);
	
	pState->DeleteVaraintPointerHolder();

#else
	CGas2GacCPP_OnDeleteState Cmd;
	Cmd.uObjID=m_pOwner->GetEntityID();
	Cmd.uCategoryId = uCategoryId;
	Cmd.uDynamicId = uDynamicId;

	//++uCmdOrder;
	//Cmd.uCmdOrder = uCmdOrder;
	//Cmd.uSendBy = m_pOwner->GetEntityID();

	//cout << "SendCmdToGac[" << Cmd.uCmdOrder << "](CGas2GacCPP_OnDeleteState[" << uCategoryId << "," << uDynamicId << "]) by " << Cmd.uSendBy << "\n";

	m_pOwner->SendCmdToGac(&Cmd);

	//CCoreObjectServer* pObj = m_pOwner->GetCoreObj();

	//CPos myPos;
	//pObj->GetGridPos(myPos);

	//vector<CCoreObjectServer*> vecObject;
	//pObj->GetScene()->QueryObject(vecObject, myPos, pObj->GetEyeSight());
	//for(vector<CCoreObjectServer* >::iterator it = vecObject.begin(); it != vecObject.end(); ++it)
	//{
	//	if ( *it == NULL ) 
	//		continue;
	//	ICoreObjectServerHandler* pHandler = (*it)->GetHandler();
	//	if ( pHandler == NULL )
	//		continue;
	//	int32 eType = (int32)(pHandler->GetTag());
	//	switch(eType)
	//	{
	//	case eCOHT_Character:
	//		CCharacterDictator *pCharacter = CCharacterDictator::GetCharacterByCoreObj(*it);
	//		if (pCharacter->GetNpcAIHandler())
	//			pCharacter->GetNpcAIHandler()->OnDeleteState(pObj->GetGlobalID());
	//	}
	//	//if (GetName() == it->Get)
	//}
#endif
}


void CAllStateMgrServer::OnSetState( uint32 uCategoryId, uint32 uDynamicId, uint32 uCount, int32 iTime,
									int32 iRemainTime, uint32 uSkillLevel,uint32 uInstallerID, int32 iCalcValue)
{
	CBaseStateServer* pState;
	if(!m_idmapStateByDId.Get(pState, uDynamicId))
	{
		stringstream sErr;
		sErr << "OnSetState(" << uCategoryId << ", " << uDynamicId << ", " << uCount << ", " << iTime
			<< ", " << iRemainTime << ")编号为" << uDynamicId << "的["
			<< CBaseStateCfgServer::GetByGlobalId(uCategoryId)->GetName() << "]状态不存在于IDMAP中（不一定是bug）\n";
		CConnServer* pConnServer = m_pOwner->GetConnection();
		if (pConnServer)
			LogErr("OnSetState状态不存在于IDMAP中（不一定是bug）", sErr.str().c_str(), pConnServer);
		else
			LogErr("OnSetState状态不存在于IDMAP中（不一定是bug）", sErr.str().c_str());
		return;
	}

	if(!pState)
	{
		stringstream sErr;
		sErr << "OnSetState(" << uCategoryId << ", " << uDynamicId << ", " << uCount << ", " << iTime
			<< ", " << iRemainTime << ")编号为" << uDynamicId << "的["
			<< CBaseStateCfgServer::GetByGlobalId(uCategoryId)->GetName() << "]状态指针为空（不一定是bug）\n";
		CConnServer* pConnServer = m_pOwner->GetConnection();
		if (pConnServer)
			LogErr("OnDeleteState状态在IDMAP中的指针为空（不一定是bug）", sErr.str().c_str(), pConnServer);
		else
			LogErr("OnDeleteState状态在IDMAP中的指针为空（不一定是bug）", sErr.str().c_str());
		return;
	}

	if(!pState->GetCfg()->GetNeedSync())
		return;

	int64 iGuessStartTime;
	//iRemainTime有可能为0的情况，不能加异常
	//Ast(iRemainTime != 0);

#ifdef TICK_BY_REMAINTIME
	iGuessStartTime = int64(iRemainTime);
#else
	if(iRemainTime < 0)
	{
		if(iRemainTime == -1)
		{
			//iGuessStartTime = int64(iRemainTime);
			iGuessStartTime = STATE_TIME_INT64_MIN_VALUE + 1;	//-9223372036854775807
		}
		else
		{
			iGuessStartTime = STATE_TIME_INT64_MIN_VALUE;		//-9223372036854775808
		}
	}
	else
	{
		Ast(iTime >= 0);
		iGuessStartTime = int64(m_pOwner->GetDistortedFrameTime()) - int64(iTime - iRemainTime) * 1000;
	}
#endif

	int64 iTime64 = int64(iTime);

	//cout << "[" << CAppServer::Inst()->GetFrameTime() << "]OnSetState(" << uCategoryId << ", " << uDynamicId << ", " 
	//	<< uCount << ", " << iTime64 << ", " << iRemainTime << ", " << (!pObserver ? "True" : "False") << ") Guess=" 
	//	<< iGuessStartTime << "\n";
	uint16 uCategoryId16 = uint16(uCategoryId);
	if(uint32(uCategoryId16) != uCategoryId)
	{
		ostringstream strm;
		strm << uCategoryId;
		LogErr("state CategoryId is great than 65535", strm.str());
	}
	uint8 uSkillLevel8 = uint8(uSkillLevel);
	if(uint32(uSkillLevel8) != uSkillLevel)
	{
		ostringstream strm;
		strm << uSkillLevel;
		LogErr("state uSkillLevel is great than 255", strm.str());
	}
	uint8 uCount8 = uint8(uCount);
	if(uint32(uCount8) != uCount)
	{
		ostringstream strm;
		strm << uCount;
		LogErr("state uCount is great than 255", strm.str());
	}

	CStateVaraintPointerHolder* pVarPtrHolder = pState->GetVaraintPointerHolder();
	if(!pVarPtrHolder)
	{
		pVarPtrHolder = new CStateVaraintPointerHolder();
		pState->SetVaraintPointerHolder(pVarPtrHolder);

		CCharacterDictator* pCharacter = m_pOwner->GetCharacter();
		CSyncVariantServerHolder* SyncVariantHolder = pCharacter->GetSyncVariantHolder(eCTI_Agile);

		//用VariantHolder路径设置结点
		//stringstream sStateKey, sStateId, sDynamicId, sCategoryId, sCount, sTime, sGuessStartTime, sStateIsSync;
		//string sStateRootKey = "State";
		//sStateId << "s" << uDynamicId;
		//sStateKey << sStateRootKey << "/" << sStateId.str();
		//sCategoryId << sStateKey.str() << "/CategoryId";
		//sDynamicId << sStateKey.str() << "/DynamicId";
		//sCount << sStateKey.str() << "/Count";
		//sTime << sStateKey.str() << "/Time";
		//sGuessStartTime << sStateKey.str() << "/GuessStartTime";
		//sStateIsSync << sStateKey.str() << "/zzzzzStateIsSync";

		//SyncVariantHolder->AddSubTree(sStateRootKey.c_str(), "AllState", sStateId.str().c_str());
		//pVarPtrHolder->m_pCategoryId = SyncVariantHolder->SetNumber(sCategoryId.str().c_str(), uCategoryId);
		//pVarPtrHolder->m_pDynamicId = SyncVariantHolder->SetNumber(sDynamicId.str().c_str(), uDynamicId);
		//pVarPtrHolder->m_pCount = SyncVariantHolder->SetNumber(sCount.str().c_str(), uCount);
		//pVarPtrHolder->m_pTime = SyncVariantHolder->SetNumber(sTime.str().c_str(), iTime64);
		//pVarPtrHolder->m_pGuessStartTime = SyncVariantHolder->SetNumber(sGuessStartTime.str().c_str(), iGuessStartTime);
		//pVarPtrHolder->m_pStateIsSync = SyncVariantHolder->SetNumber(sStateIsSync.str().c_str(), true);

		//用Variant指针设置结点
		static TCHAR chrStateId[24] = "s";
		//_itoa(int32(uDynamicId), chrStateId + 1, 10);
		sprintf(chrStateId + 1, "%u", uDynamicId);

		CSyncVariantServer* pStateVar = SyncVariantHolder->AddSubTree("State", "AllState", chrStateId);
		Ast(pStateVar);
		pVarPtrHolder->m_pStateSubTree = pStateVar;			

		pVarPtrHolder->m_pCategoryId = pStateVar->SetMember("CategoryId");

		pVarPtrHolder->m_pCategoryId->SetNumber(uCategoryId16);

		pVarPtrHolder->m_pDynamicId = pStateVar->SetMember("DynamicId");
		pVarPtrHolder->m_pDynamicId->SetNumber(uDynamicId);
		pVarPtrHolder->m_pCount = pStateVar->SetMember("Count");

		pVarPtrHolder->m_pCount->SetNumber(uCount8);
		pVarPtrHolder->m_pTime = pStateVar->SetMember("Time");
		pVarPtrHolder->m_pTime->SetNumber(iTime64);
		pVarPtrHolder->m_pGuessStartTime = pStateVar->SetMember("GuessStartTime");
		if(iGuessStartTime == STATE_TIME_INT64_MIN_VALUE)
		{
			iGuessStartTime = int64(m_pOwner->GetDistortedFrameTime()) - int64(iTime - pState->GetLeftTime()) * 1000;
		}
		pVarPtrHolder->m_pGuessStartTime->SetNumber(iGuessStartTime);
		pVarPtrHolder->m_pSkillLevel = pStateVar->SetMember("SkillLevel");

		pVarPtrHolder->m_pSkillLevel->SetNumber(uSkillLevel8);
		pVarPtrHolder->m_pInstallerID = pStateVar->SetMember("InstallerID");
		pVarPtrHolder->m_pInstallerID->SetNumber(uInstallerID);
		pVarPtrHolder->m_pCalcValue = pStateVar->SetMember("CalcValue");
		pVarPtrHolder->m_pCalcValue->SetNumber(iCalcValue);
		pVarPtrHolder->m_pStateIsSync = pStateVar->SetMember("zzzzzStateIsSync");
		pVarPtrHolder->m_pStateIsSync->SetNumber(true);
		pVarPtrHolder->m_pStateIsFinish = pStateVar->SetMember("zzzzzStateIsFinish");
		pVarPtrHolder->m_pStateIsFinish->SetNumber(false);
	}
	else
	{
		pVarPtrHolder->m_pCategoryId->SetNumber(uCategoryId16);		//不会变，可以考虑不设
		pVarPtrHolder->m_pDynamicId->SetNumber(uDynamicId);		//不会变，可以考虑不设
		pVarPtrHolder->m_pCount->SetNumber(uCount8);
		pVarPtrHolder->m_pTime->SetNumber(iTime64);
		if(iGuessStartTime != STATE_TIME_INT64_MIN_VALUE)
		{
			pVarPtrHolder->m_pGuessStartTime->SetNumber(iGuessStartTime);
		}
		pVarPtrHolder->m_pSkillLevel->SetNumber(uSkillLevel8);
		pVarPtrHolder->m_pInstallerID->SetNumber(uInstallerID);
		pVarPtrHolder->m_pCalcValue->SetNumber(iCalcValue);
		pVarPtrHolder->m_pStateIsSync->SetNumber(true);
		pVarPtrHolder->m_pStateIsFinish->SetNumber(false);
	}
}

void CAllStateMgrServer::OnSetState( uint32 uCategoryId, uint32 uDynamicId, uint32 uCount, int32 iTime,
									int32 iRemainTime, uint32 uSkillLevel, CFighterDictator* pObserver)
{
	CBaseStateServer* pState;
	if(!m_idmapStateByDId.Get(pState, uDynamicId))
	{
		stringstream sErr;
		sErr << "OnSetState(" << uCategoryId << ", " << uDynamicId << ", " << uCount << ", " << iTime
			<< ", " << iRemainTime << (uint64)pObserver << ")编号为" << uDynamicId << "的["
			<< CBaseStateCfgServer::GetByGlobalId(uCategoryId)->GetName() << "]状态不存在于IDMAP中（不一定是bug）\n";
		CConnServer* pConnServer = m_pOwner->GetConnection();
		if (pConnServer)
			LogErr("OnSetState状态不存在于IDMAP中（不一定是bug）", sErr.str().c_str(), pConnServer);
		else
			LogErr("OnSetState状态不存在于IDMAP中（不一定是bug）", sErr.str().c_str());
		return;
	}

	if(!pState)
	{
		stringstream sErr;
		sErr << "OnSetState(" << uCategoryId << ", " << uDynamicId << ", " << uCount << ", " << iTime
			<< ", " << iRemainTime << (uint64)pObserver << ")编号为" << uDynamicId << "的["
			<< CBaseStateCfgServer::GetByGlobalId(uCategoryId)->GetName() << "]状态指针为空（不一定是bug）\n";
		CConnServer* pConnServer = m_pOwner->GetConnection();
		if (pConnServer)
			LogErr("OnDeleteState状态在IDMAP中的指针为空（不一定是bug）", sErr.str().c_str(), pConnServer);
		else
			LogErr("OnDeleteState状态在IDMAP中的指针为空（不一定是bug）", sErr.str().c_str());
		return;
	}
		
	if(!pState->GetCfg()->GetNeedSync())
		return;

	int64 iGuessStartTime;
	//iRemainTime有可能为0的情况，不能加异常
	//Ast(iRemainTime != 0);

#ifdef TICK_BY_REMAINTIME
	iGuessStartTime = int64(iRemainTime);
#else
	if(iRemainTime < 0)
	{
		if(iRemainTime == -1)
		{
			//iGuessStartTime = int64(iRemainTime);
			iGuessStartTime = STATE_TIME_INT64_MIN_VALUE + 1;	//-9223372036854775807
		}
		else
		{
			iGuessStartTime = STATE_TIME_INT64_MIN_VALUE;		//-9223372036854775808
		}
	}
	else
	{
		Ast(iTime >= 0);
		iGuessStartTime = int64(m_pOwner->GetDistortedFrameTime()) - int64(iTime - iRemainTime) * 1000;
	}
#endif

	int64 iTime64 = int64(iTime);

	//cout << "[" << CAppServer::Inst()->GetFrameTime() << "]OnSetState(" << uCategoryId << ", " << uDynamicId << ", " 
	//	<< uCount << ", " << iTime64 << ", " << iRemainTime << ", " << (!pObserver ? "True" : "False") << ") Guess=" 
	//	<< iGuessStartTime << "\n";

#ifdef SEND_STATE_BY_VAR
	uint16 uCategoryId16 = uint16(uCategoryId);
	if(uint32(uCategoryId16) != uCategoryId)
	{
		ostringstream strm;
		strm << uCategoryId;
		LogErr("state CategoryId is great than 65535", strm.str());
	}
	uint8 uSkillLevel8 = uint8(uSkillLevel);
	if(uint32(uSkillLevel8) != uSkillLevel)
	{
		ostringstream strm;
		strm << uSkillLevel;
		LogErr("state uSkillLevel is great than 255", strm.str());
	}
	uint8 uCount8 = uint8(uCount);
	if(uint32(uCount8) != uCount)
	{
		ostringstream strm;
		strm << uCount;
		LogErr("state uCount is great than 255", strm.str());
	}

	if (!pObserver)
	{
		CStateVaraintPointerHolder* pVarPtrHolder = pState->GetVaraintPointerHolder();
		if(!pVarPtrHolder)
		{
			pVarPtrHolder = new CStateVaraintPointerHolder();
			pState->SetVaraintPointerHolder(pVarPtrHolder);

			CCharacterDictator* pCharacter = m_pOwner->GetCharacter();
			CSyncVariantServerHolder* SyncVariantHolder = pCharacter->GetSyncVariantHolder(eCTI_Agile);

			//用VariantHolder路径设置结点
			//stringstream sStateKey, sStateId, sDynamicId, sCategoryId, sCount, sTime, sGuessStartTime, sStateIsSync;
			//string sStateRootKey = "State";
			//sStateId << "s" << uDynamicId;
			//sStateKey << sStateRootKey << "/" << sStateId.str();
			//sCategoryId << sStateKey.str() << "/CategoryId";
			//sDynamicId << sStateKey.str() << "/DynamicId";
			//sCount << sStateKey.str() << "/Count";
			//sTime << sStateKey.str() << "/Time";
			//sGuessStartTime << sStateKey.str() << "/GuessStartTime";
			//sStateIsSync << sStateKey.str() << "/zzzzzStateIsSync";

			//SyncVariantHolder->AddSubTree(sStateRootKey.c_str(), "AllState", sStateId.str().c_str());
			//pVarPtrHolder->m_pCategoryId = SyncVariantHolder->SetNumber(sCategoryId.str().c_str(), uCategoryId);
			//pVarPtrHolder->m_pDynamicId = SyncVariantHolder->SetNumber(sDynamicId.str().c_str(), uDynamicId);
			//pVarPtrHolder->m_pCount = SyncVariantHolder->SetNumber(sCount.str().c_str(), uCount);
			//pVarPtrHolder->m_pTime = SyncVariantHolder->SetNumber(sTime.str().c_str(), iTime64);
			//pVarPtrHolder->m_pGuessStartTime = SyncVariantHolder->SetNumber(sGuessStartTime.str().c_str(), iGuessStartTime);
			//pVarPtrHolder->m_pStateIsSync = SyncVariantHolder->SetNumber(sStateIsSync.str().c_str(), true);

			//用Variant指针设置结点
			static TCHAR chrStateId[24] = "s";
			//_itoa(int32(uDynamicId), chrStateId + 1, 10);
			sprintf(chrStateId + 1, "%u", uDynamicId);

			CSyncVariantServer* pStateVar = SyncVariantHolder->AddSubTree("State", "AllState", chrStateId);
			Ast(pStateVar);
			pVarPtrHolder->m_pStateSubTree = pStateVar;			
			
			pVarPtrHolder->m_pCategoryId = pStateVar->SetMember("CategoryId");
			pVarPtrHolder->m_pCategoryId->SetNumber(uCategoryId16);

			pVarPtrHolder->m_pDynamicId = pStateVar->SetMember("DynamicId");
			pVarPtrHolder->m_pDynamicId->SetNumber(uDynamicId);
			pVarPtrHolder->m_pCount = pStateVar->SetMember("Count");
			pVarPtrHolder->m_pCount->SetNumber(uCount8);
			pVarPtrHolder->m_pTime = pStateVar->SetMember("Time");
			pVarPtrHolder->m_pTime->SetNumber(iTime64);
			pVarPtrHolder->m_pGuessStartTime = pStateVar->SetMember("GuessStartTime");
			if(iGuessStartTime == STATE_TIME_INT64_MIN_VALUE)
			{
				iGuessStartTime = int64(m_pOwner->GetDistortedFrameTime()) - int64(iTime - pState->GetLeftTime()) * 1000;
			}
			pVarPtrHolder->m_pGuessStartTime->SetNumber(iGuessStartTime);
			pVarPtrHolder->m_pSkillLevel = pStateVar->SetMember("SkillLevel");
			pVarPtrHolder->m_pSkillLevel->SetNumber(uSkillLevel8);
			pVarPtrHolder->m_pStateIsSync = pStateVar->SetMember("zzzzzStateIsSync");
			pVarPtrHolder->m_pStateIsSync->SetNumber(true);
			pVarPtrHolder->m_pStateIsFinish = pStateVar->SetMember("zzzzzStateIsFinish");
			pVarPtrHolder->m_pStateIsFinish->SetNumber(false);
		}
		else
		{
			pVarPtrHolder->m_pCategoryId->SetNumber(uCategoryId16);		//不会变，可以考虑不设
			pVarPtrHolder->m_pDynamicId->SetNumber(uDynamicId);		//不会变，可以考虑不设
			pVarPtrHolder->m_pCount->SetNumber(uCount8);
			pVarPtrHolder->m_pTime->SetNumber(iTime64);
			if(iGuessStartTime != STATE_TIME_INT64_MIN_VALUE)
			{
				pVarPtrHolder->m_pGuessStartTime->SetNumber(iGuessStartTime);
			}
			pVarPtrHolder->m_pSkillLevel->SetNumber(uSkillLevel8);
			pVarPtrHolder->m_pStateIsSync->SetNumber(true);
			pVarPtrHolder->m_pStateIsFinish->SetNumber(false);
		}
	}


#else
	CGas2GacCPP_OnSetState Cmd;
	Cmd.uObjID=m_pOwner->GetEntityID();
	Cmd.uCategoryId = uCategoryId;
	Cmd.uDynamicId = uDynamicId;
	Cmd.uCount = uCount;
	Cmd.iTime = iTime64;
	Cmd.iGuessStartTime = iGuessStartTime;
	Cmd.uInstallerID = 0;
	Cmd.iCalcValue = 0;

	//++uCmdOrder;
	//Cmd.uCmdOrder = uCmdOrder;

	//cout << "SendCmdToGac[" << Cmd.uCmdOrder << "](CGas2GacCPP_OnSetState[" << uCategoryId << "," << uDynamicId << "]) by ";

	if (!pObserver)
	{
		//Cmd.uSendBy = m_pOwner->GetEntityID();
		//cout << "m_pOwner = " << Cmd.uSendBy << "\n";
		m_pOwner->SendCmdToGac(&Cmd);
	}
	else
	{
		//Cmd.uSendBy = pObserver->GetEntityID();
		//cout << "pObserver = " << Cmd.uSendBy << "\n";
		pObserver->SendCmdToGac(&Cmd, eOnlyToDirector);
	}
#endif

//	CCoreObjectServer* pObj = m_pOwner->GetCoreObj();
//	
//	CPos myPos;	
//	pObj->GetGridPos(myPos);
//
//#define NPC_DEFAULT_EYESIGHT 10
//
//	vector<CCoreObjectServer*> vecObject;
//	pObj->GetScene()->QueryObject(vecObject, myPos, NPC_DEFAULT_EYESIGHT);
//	for(vector<CCoreObjectServer* >::iterator it = vecObject.begin(); it != vecObject.end(); ++it)
//	{
//		if ( *it == NULL ) 
//			continue;
//		ICoreObjectServerHandler* pHandler = (*it)->GetHandler();
//		if ( pHandler == NULL )
//			continue;
//		int32 eType = (int32)(pHandler->GetTag());
//		switch(eType)
//		{
//		case eCOHT_Character:
//			CCharacterDictator *pCharacter = CCharacterDictator::GetCharacterByCoreObj(*it);
//			if (pCharacter->GetNpcAIHandler())
//				pCharacter->GetNpcAIHandler()->OnSetState(pObj->GetGlobalID());
//		}
//		//if (GetName() == it->Get)
//	}
}


void CAllStateMgrServer::ClearAll()
{
	GetMagicStateMgrServer()->ClearAll();
	GetTriggerStateMgrServer()->ClearAll();
	GetDamageChangeStateMgrServer()->ClearAll();
	GetSpecialStateMgrServer()->ClearAll();
	//以防万一再删一次
	//m_idmapStateByDId.clear();
}

void CAllStateMgrServer::ClearAllByCond(CStateCondBase* pCond)
{
	GetMagicStateMgrServer()->ClearAllByCond(pCond);
	GetTriggerStateMgrServer()->ClearAllByCond(pCond);
	GetDamageChangeStateMgrServer()->ClearAllByCond(pCond);
	GetSpecialStateMgrServer()->ClearAllByCond(pCond);
}

void CAllStateMgrServer::ReflectStateByCond(CStateCondBase* pCond)
{
	GetMagicStateMgrServer()->ReflectAllByCond(pCond);
}


bool CAllStateMgrServer::ExistDecreaseState()
{
	CStateCondForAllDecrease StateCond;
	return ExistStateByCond(&StateCond);
}

bool CAllStateMgrServer::ExistStateByDecreaseType(EDecreaseStateType eDecreaseType)
{
	CStateCondForDecreaseType StateCond(eDecreaseType);
	return ExistStateByCond(&StateCond);
}

bool CAllStateMgrServer::ExistState(const string& strName)
{
	CBaseStateCfg* pCfg = CBaseStateCfg::GetByGlobalName(strName);
	if(pCfg)
		switch(pCfg->GetGlobalType())
	{
		case eSGT_MagicState:
			return GetMagicStateMgrServer()->ExistState(strName);
		case eSGT_TriggerState:
			return GetTriggerStateMgrServer()->ExistState(strName);
		case eSGT_DamageChangeState:
			return GetDamageChangeStateMgrServer()->ExistState(strName);
		case eSGT_CumuliTriggerState:
			return GetCumuliTriggerStateMgrServer()->ExistState(strName);
		case eSGT_SpecialState:
			return GetSpecialStateMgrServer()->ExistState(strName);
		default:
			{
				stringstream ExpStr;
				ExpStr << "状态[" << strName << "]种类类型不正确";
				GenErr("状态种类类型不正确", ExpStr.str());
			}
	}
	else
	{
		stringstream ExpStr;
		ExpStr << "[" << strName << "]状态不存在";
		GenErr("状态不存在", ExpStr.str());
	}
	return false;
}

bool CAllStateMgrServer::ExistState(uint32 uDynamicId)
{
	CBaseStateServer* pState;
	return m_idmapStateByDId.Get(pState, uDynamicId);
}

bool CAllStateMgrServer::ExistStateByCond(CStateCondBase* pStateCond)
{
	if(GetMagicStateMgrServer()->ExistStateByCond(pStateCond) || 
		GetTriggerStateMgrServer()->ExistStateByCond(pStateCond) || 
		GetDamageChangeStateMgrServer()->ExistStateByCond(pStateCond) || 
		GetSpecialStateMgrServer()->ExistStateByCond(pStateCond))
		return true;
	else
		return false;
}

uint32 CAllStateMgrServer::GetStateCascade(const string& strName)
{
	CBaseStateCfg* pCfg = CBaseStateCfg::GetByGlobalName(strName);
	if(pCfg)
		switch(pCfg->GetGlobalType())
	{
		case eSGT_MagicState:
			return GetMagicStateMgrServer()->MagicStateCount(strName);
		default:
			{
				stringstream ExpStr;
				ExpStr << "状态[" << strName << "]种类类型不正确";
				GenErr("GetStateCascade:状态种类类型不正确", ExpStr.str());
			}
	}
	else
	{
		stringstream ExpStr;
		ExpStr << "[" << strName << "]状态不存在";
		GenErr("状态不存在", ExpStr.str());
	}
	return 0;
}


void CAllStateMgrServer::ClearAllNonpersistentState()
{
	CStateCondForNonPersistent StateCond;
	ClearAllByCond(&StateCond);
}

void CAllStateMgrServer::EraseAllState()
{
	CStateCondForNonPersistent StateCond;
	ClearAllByCond(&StateCond);
}

void CAllStateMgrServer::EraseDecreaseState(int32 iCount)
{
	CStateCondForAllDecrease StateCond(iCount);
	ClearAllByCond(&StateCond);
}

void CAllStateMgrServer::EraseDispellableDecreaseState(int32 iCount)
{
	CStateCondForDispellableAllDecrease StateCond(iCount);
	ClearAllByCond(&StateCond);
}

void CAllStateMgrServer::EraseDispellableDecreaseStateRand(int32 iCount)
{
	CStateCondForCountDispellableAllDecrease CountStateCond(iCount);
	ExistStateByCond(&CountStateCond);
	CStateCondForRandDispellableAllDecrease StateCond(CountStateCond.GetAllCount(), iCount);
	ClearAllByCond(&StateCond);
}

void CAllStateMgrServer::EraseIncreaseState(int32 iCount)
{
	CStateCondForDecreaseType StateCond(eDST_Increase, iCount);
	ClearAllByCond(&StateCond);
}

void CAllStateMgrServer::EraseDispellableIncreaseState(int32 iCount)
{
	CStateCondForDispellableDecreaseType StateCond(eDST_Increase, iCount);
	ClearAllByCond(&StateCond);
}

void CAllStateMgrServer::EraseDispellableIncreaseStateRand(int32 iCount)
{
	CStateCondForCountDispellableDecreaseType CountStateCond(eDST_Increase, iCount);
	ExistStateByCond(&CountStateCond);
	CStateCondForRandDispellableDecreaseType StateCond(CountStateCond.GetAllCount(), eDST_Increase, iCount);
	ClearAllByCond(&StateCond);
}


void CAllStateMgrServer::EraseState(EDecreaseStateType eDecreaseType, int32 iCount, bool bDelEqual)
{
	CStateCondForDecreaseType StateCond(eDecreaseType, iCount, bDelEqual);
	ClearAllByCond(&StateCond);
}

void CAllStateMgrServer::EraseDispellableState(EDecreaseStateType eDecreaseType, int32 iCount, bool bDelEqual)
{
	CStateCondForDispellableDecreaseType StateCond(eDecreaseType, iCount, bDelEqual);
	ClearAllByCond(&StateCond);
}

bool CAllStateMgrServer::EraseErasableState(const string& strName)
{
	CBaseStateCfg* pCfg = CBaseStateCfg::GetByGlobalName(strName);
	uint32 uId=pCfg->GetId();
	CStateCondForNonPersistent StateCond;
	return ClearStateByCond(&StateCond, uId);
}

void CAllStateMgrServer::EraseState(const string& strName)
{
	CBaseStateCfg* pCfg = CBaseStateCfg::GetByGlobalName(strName);
	EraseState(pCfg);
}

void CAllStateMgrServer::EraseState(CBaseStateCfg* pCfg)
{
	uint32 uId=pCfg->GetId();
	CStateCondForAll StateCond;
	ClearStateByCond(&StateCond, uId);
}

void CAllStateMgrServer::EraseSelfState()
{
	CStateCondForSelfState StateCond(m_pOwner);
	ClearAllByCond(&StateCond);
}

void CAllStateMgrServer::EraseState(CBaseStateCfg* pCfg, CFighterDictator* pInstaller)
{
	uint32 uId=pCfg->GetId();
	CStateCondForSelfState StateCond(pInstaller);
	ClearStateByCond(&StateCond, uId);
}


void CAllStateMgrServer::ReflectState(EDecreaseStateType eDecreaseType)
{
	CStateCondForDecreaseType StateCond(eDecreaseType);
	ReflectStateByCond(&StateCond);
}

bool CAllStateMgrServer::ClearStateByCondAndDynamicId(CStateCondBase* pStateCond, uint32 uDynamicId)
{
		CBaseStateServer* pState;
		if(m_idmapStateByDId.Get(pState, uDynamicId))
		{
			if(pState && (*pStateCond)(pState))
			{
				CBaseStateCfg* pCfg = pState->GetCfg();
				if(pCfg)
				{
					switch(pCfg->GetGlobalType())
					{
					case eSGT_MagicState:
						{
							CMagicStateServer* pMagicState = class_cast<CMagicStateServer*>(pState);
							if(pMagicState->CancelDo()) return true;
							pMagicState->DeleteSelf();
							return true;
						}
						break;
					case eSGT_TriggerState:
					case eSGT_DamageChangeState:
					case eSGT_CumuliTriggerState:
						{
							CTriggerableStateServer * pTState = class_cast<CTriggerableStateServer*>(pState);
							pTState->CancelDo();
							pTState->DeleteSelf();
							return true;
						}
						break;
					case eSGT_SpecialState:
						{
							CSpecialStateBase* pSState = class_cast<CSpecialStateBase*>(pState);
							pSState->CancelDo();		//这句可以不写
							pSState->DeleteSelf();
							return true;
						}
						break;
					default:
						{
							stringstream ExpStr;
							ExpStr << "DynamicId = " << uDynamicId;
							GenErr("状态种类类型不正确", ExpStr.str());
						}
					}
				}
				//m_mapMSByDynamicId.erase(DynamicId);
			}
		}
		return false;		
}

bool CAllStateMgrServer::ClearStateByCond(CStateCondBase* pStateCond, uint32 uId)
{
	//	if(uId >= eSIC_DynamicMagicState)
	//	{
	//		return GetMagicStateMgrServer()->ClearStateByCondAndDynamicId(pStateCond, uId);
	//	}
	//else
	//{
		CBaseStateCfg* pCfg = CBaseStateCfg::GetByGlobalId(uId);
		if(pCfg)
			switch(pCfg->GetGlobalType())
		{
			case eSGT_MagicState:
				return GetMagicStateMgrServer()->ClearStateByCond(pStateCond, uId);
			case eSGT_TriggerState:
				return GetTriggerStateMgrServer()->ClearStateByCond(pStateCond, uId);
			case eSGT_DamageChangeState:
				return GetDamageChangeStateMgrServer()->ClearStateByCond(pStateCond, uId);
			case eSGT_CumuliTriggerState:
				return GetCumuliTriggerStateMgrServer()->ClearStateByCond(pStateCond, uId);
			case eSGT_SpecialState:
				return GetSpecialStateMgrServer()->ClearStateByCond(pStateCond, uId);
			default:
				{
					stringstream ExpStr;
					ExpStr << "ID = " << uId;
					GenErr("状态种类类型不正确", ExpStr.str());
				}
		}
	//}
	return false;
}


bool CAllStateMgrServer::CancelState(uint32 uId)
{
	CStateCondForIcon StateCond;
	return ClearStateByCond(&StateCond, uId);
}

bool CAllStateMgrServer::CancelStateByDynamicId(uint32 uDynamicId)
{
	CStateCondForIcon StateCond;
	return ClearStateByCondAndDynamicId(&StateCond, uDynamicId);
}

bool CAllStateMgrServer::DecreaseStateResist(CBaseStateCfgServer* pCfg)
{
	switch(pCfg->GetDecreaseType())
	{
	case eDST_Control:
		{
			if(CQuickRand::Rand1(0.0f, 1.0f) < m_pOwner->m_ControlDecreaseResistRate.Get())
			{
				return true;
			}
		}
		break;
	case eDST_Pause:
		{
			if(CQuickRand::Rand1(0.0f, 1.0f) < m_pOwner->m_PauseDecreaseResistRate.Get())
			{
				return true;
			}
		}
		break;
	case eDST_Crippling:
		{
			if(CQuickRand::Rand1(0.0f, 1.0f) < m_pOwner->m_CripplingDecreaseResistRate.Get())
			{
				return true;
			}
		}
		break;
	case eDST_Debuff:
		{
			if(CQuickRand::Rand1(0.0f, 1.0f) < m_pOwner->m_DebuffDecreaseResistRate.Get())
			{
				return true;
			}
		}
		break;
	case eDST_DOT:
		{
			if(CQuickRand::Rand1(0.0f, 1.0f) < m_pOwner->m_DOTDecreaseResistRate.Get())
			{
				return true;
			}
		}
		break;
	case eDST_Special:
		{
			if(CQuickRand::Rand1(0.0f, 1.0f) < m_pOwner->m_SpecialDecreaseResistRate.Get())
			{
				return true;
			}
		}
		break;
	default:
		break;
	}
	
	return false;
}

bool CAllStateMgrServer::Immume(CBaseStateCfgServer* pCfg, CSkillInstServer* pSkillInst, CFighterDictator* pInstaller)
{
	//if(pCfg->GetDecrease())
	//{
	//	ESkillType pSkillType = pSkillInst->GetSkillType();
	//	if(pSkillType != eST_DOT && pCfg->GetGlobalType() == eSGT_MagicState)
	//	{
	//		CMagicStateCfgServer* pMSCfg = (CMagicStateCfgServer*)pCfg;
	//		if(pMSCfg->GetDotMagicEff()) pSkillType = eST_DOT;
	//	}

	//	if (pSkillType == eST_DOT && GetSpecialStateMgrServer()->ExistStateByType(eSST_DOTImmunity))
	//		return true;
	//}

	//return false;

	bool bResult = false;

	if(pCfg->GetDecrease())
	{
		if(m_pOwner->Get(&CFighterCalInfo::m_ImmuneRate) >= 1.0f)
			bResult = true;
		else
		{
			switch(pCfg->GetDecreaseType())
			{
			case eDST_Control:
				if (m_pOwner->m_ControlDecreaseImmuneRate.Get() >= 1.0f) 
					bResult = true;
				break;
			case eDST_Pause:
				if (m_pOwner->m_PauseDecreaseImmuneRate.Get() >= 1.0f) 
					bResult = true;
				break;
			case eDST_Crippling:
				if (m_pOwner->m_CripplingDecreaseImmuneRate.Get() >= 1.0f) 
					bResult = true;
				break;
			case eDST_Debuff:
				if (m_pOwner->m_DebuffDecreaseImmuneRate.Get() >= 1.0f) 
					bResult = true;
				break;
			case eDST_DOT:
				if (m_pOwner->m_DOTDecreaseImmuneRate.Get() >= 1.0f) 
					bResult = true;
				break;
			case eDST_Special:
				if (m_pOwner->m_SpecialDecreaseImmuneRate.Get() >= 1.0f) 
					bResult = true;
				break;
			default: 
				break;
			}
		}
	}
	if(bResult)
	{
		if(!m_pOwner->PrintInfoIsOff())
		{
			CCharacterDictator* pCreatorChar = CCharacterDictator::GetCharacterByID(pSkillInst->GetCreatorID());
			CFighterDictator* pCreatorFighter = pCreatorChar?pCreatorChar->GetFighter():NULL;
			m_pOwner->GetSkillMgr()->DisplayFightInfo(pCreatorFighter,pInstaller,0,eAT_HP,eHR_Immune);
		}
		CTriggerEvent::MessageEvent(eTST_Trigger, pInstaller, m_pOwner, eHR_Immune, true);
	}
	return bResult;
}

CStateDBDataSet* CAllStateMgrServer::SerializeToDB(uint32 uGrade)
{
	ICharacterDictatorCallbackHandler* pHandler = m_pOwner->GetCallBackHandler();
	uint32 uFighterGlobalID = m_pOwner->GetEntityID();
	uint64 uNow = m_pOwner->GetDistortedFrameTime();
	m_pRet = new CStateDBDataSet;
	if(!GetMagicStateMgrServer()->SerializeToDB(m_pRet, pHandler, uFighterGlobalID, uNow, uGrade)
		|| !GetTriggerStateMgrServer()->SerializeToDB(m_pRet, pHandler, uFighterGlobalID, uNow, uGrade)
		|| !GetDamageChangeStateMgrServer()->SerializeToDB(m_pRet, pHandler, uFighterGlobalID, uNow, uGrade)
		|| !GetSpecialStateMgrServer()->SerializeToDB(m_pRet, pHandler, uFighterGlobalID, uNow, uGrade))
	{
		SafeDelete(m_pRet);
		return NULL;
	}
	//pHandler->AddToDBEnd(uFighterGlobalID);
	return m_pRet;
}

void CAllStateMgrServer::SaveStateToDBEnd()
{
	SafeDelete(m_pRet);
}

bool CAllStateMgrServer::LoadFromDB(CStateDBDataSet* pRet)
{
	ICharacterDictatorCallbackHandler* pHandler = m_pOwner->GetCallBackHandler();
	//因为现在登录时天赋效果可能会安装状态（魔剑士的火焰积累天赋），而天赋效果在状态从数据库读入之前执行，
	//所以这句的ClearAll()会导致天赋效果安装的状态消失，因此去掉
	//但这句ClearAll()是46062版本为实现魔剑士新功能而加的，具体原因忘了，删掉这句可能会导致魔剑士的那个新功能出问题，
	//等发现的时候再查。
	//ClearAll();
	m_pRet = pRet;
	uint32 uFighterGlobalID = m_pOwner->GetEntityID();
	if(!GetMagicStateMgrServer()->LoadFromDB(pHandler, uFighterGlobalID)) return false;
	if(!GetTriggerStateMgrServer()->LoadFromDB(pHandler, uFighterGlobalID)) return false;
	if(!GetDamageChangeStateMgrServer()->LoadFromDB(pHandler, uFighterGlobalID)) return false;
	if(!GetSpecialStateMgrServer()->LoadFromDB(pHandler, uFighterGlobalID)) return false;
	//pHandler->ReadFromDBEnd(uFighterGlobalID);
	return true;
}

bool CAllStateMgrServer::ReadStateFromDB(CStateDBData*& pStateDBData, EStateGlobalType eType)
{
	CStateDBDataVec* pStateVec = m_pRet->m_pStateVec;
	if(pStateVec->Empty())
	{
		return false;
	}
	else
	{
		pStateDBData = pStateVec->GetFront();
		if(pStateDBData->m_eType != eType)
		{
			return false;
		}
		else
		{
			pStateVec->PopFront();
			return true;
		}
	}
	//SafeDelete(m_pRet);
}

bool CAllStateMgrServer::ReadStoredObjFromDB(CStoredObjDBData*& pStored, uint32 uInstID)
{
	CStoredObjDBDataVec* pStoredObjVec = m_pRet->m_pStoredObjVec;
	if(pStoredObjVec->Empty())
	{
		return false;
	}
	else
	{
		pStored = pStoredObjVec->GetFront();
		if(pStored->m_uInstID != uInstID)
		{
			return false;
		}
		else
		{
			pStoredObjVec->PopFront();
			return true;
		}
	}
}






bool CAllStateMgrServer::MessageEvent(uint32 uEventID, CFighterDictator* pTrigger, ETriggerStateType eType)
{
	CGenericTarget GenTrigger(pTrigger);
	return MessageEvent(uEventID, &GenTrigger, eType);
}

bool CAllStateMgrServer::MessageEvent(uint32 uEventID, const CFPos& pTrigger, ETriggerStateType eType)
{

	CGenericTarget GenTrigger(pTrigger);
	return MessageEvent(uEventID, &GenTrigger, eType);
}

bool CAllStateMgrServer::MessageEvent(uint32 uEventID, CGenericTarget* pTrigger, ETriggerStateType eType)
{
	bool bRet = false;
	//先触发伤害变更状态再触发触发器状态
	switch(eType)
	{
	case eTST_All:
		if(m_pDamageChangeStateMgrServer->MessageEvent(uEventID, pTrigger))
			bRet = true;
		if(m_pCumuliTriggerStateMgrServer->MessageEvent(uEventID, pTrigger))
			bRet = true;
		if(m_pTriggerStateMgrServer->MessageEvent(uEventID, pTrigger))
			bRet = true;
		break;
	case eTST_DamageChange:
		if(m_pDamageChangeStateMgrServer->MessageEvent(uEventID, pTrigger))
			bRet = true;
		break;
	case eTST_Trigger:
		if(m_pCumuliTriggerStateMgrServer->MessageEvent(uEventID, pTrigger))
			bRet = true;
		if(m_pTriggerStateMgrServer->MessageEvent(uEventID, pTrigger))
			bRet = true;
		break;
	case eTST_Special:
		if(m_pSpecialStateMgrServer->MessageEvent(uEventID, pTrigger))
			bRet = true;
		break;
	}

	return bRet;
}






void CAllStateMgrServer::PrintFightInfo(bool bSetupOrDelete, uint32 uObjInstallerID, CBaseStateCfgServer* pCfg)
{
	if(m_pOwner->PrintInfoIsOff())
		return;

	if (!pCfg->GetNeedSync())
	{
		return;
	}

	Ast(m_pOwner);

	CEntityServer* pInstaller = CEntityServerManager::GetEntityByID(uObjInstallerID);
	uint32 uObjOwnerID = m_pOwner->GetEntityID();


	if(!pInstaller || uObjInstallerID == uObjOwnerID)
	{
		m_pOwner->OnPrintFightInfo(bSetupOrDelete ? EFII_SetUpState : EFII_ClearState, uObjOwnerID, 0, NULL, 0, eATT_None, 0, 0, false, 0, pCfg->GetName(), strlen(pCfg->GetName()));
	}
	else
	{
		//m_pOwner->OnPrintFightInfo(bSetupOrDelete ? 14 : 15, uObjInstallerID, uObjOwnerID, NULL, pCfg->GetName(), 0, eATT_None, 0);
	}
}

void CAllStateMgrServer::ChangeSizeRate(CBaseStateCfgServer* pCfg, int32 iChangeCount)
{
	m_pSizeChangeMgr->ChangeSizeRate(pCfg, iChangeCount);
}

double CAllStateMgrServer::GetStateZoomRate()
{
	return m_pSizeChangeMgr->GetStateZoomRate();
}



bool CAllStateMgrServer::GetStateByDId(const CBaseStateServer*& pValue,uint32 uID) const
{
	return m_idmapStateByDId.Get(pValue, uID);
}

uint32 CAllStateMgrServer::AddStateByDId(CBaseStateServer* const & Value)
{
	return m_idmapStateByDId.Add(Value);
}

bool CAllStateMgrServer::DelStateByDId(uint32 uID)
{
	return m_idmapStateByDId.Del(uID);
}

void CAllStateMgrServer::CountCtrlState(EFighterCtrlState eState, bool bSet, uint32 uStateId, sqr::CError* pExp)
{
	if (eState >= eFCS_CountCtrlValueBegin)
	{
		MapStateCtrlCount::iterator itr = m_mapStateCtrlCount.find(eState);
		if(bSet)
		{
			if(itr != m_mapStateCtrlCount.end())
			{
				if(uStateId != 0)
				{
					++m_mapStateCtrlCount[eState].m_mapStateIdCount[uStateId];
					//cout << "SetState(" << eState << ", true) 状态[" << uStateId << "]Count = " << m_mapStateCtrlCount[eState].m_mapStateIdCount[uStateId] << "\n";
				}
				else
				{
					++m_mapStateCtrlCount[eState].m_iNonStateCount;
					//cout << "SetState(" << eState << ", true) 非状态Count = " << m_mapStateCtrlCount[eState].m_iNonStateCount << "\n";
				}
			}
			else
			{
				m_mapStateCtrlCount[eState] = CStateCtrlCount();
				CStateCtrlCount& aStateCtrlCount = m_mapStateCtrlCount[eState];
				if(uStateId != 0)
				{
					aStateCtrlCount.m_mapStateIdCount[uStateId] = 1;
					//cout << "SetState(" << eState << ", true) 状态[" << uStateId << "]Count = " << m_mapStateCtrlCount[eState].m_mapStateIdCount[uStateId] << "\n";
				}
				else
				{
					aStateCtrlCount.m_iNonStateCount = 1;
					//cout << "SetState(" << eState << ", true) 非状态Count = " << m_mapStateCtrlCount[eState].m_iNonStateCount << "\n";
				}
			}
		}
		else
		{
			if(itr != m_mapStateCtrlCount.end())
			{
				stringstream sStateInfo;
				if(uStateId != 0)
				{
					CStateCtrlCount::MapStateIdCount& mapStateIdCount = m_mapStateCtrlCount[eState].m_mapStateIdCount;
					CStateCtrlCount::MapStateIdCount::iterator itStateIdCount = mapStateIdCount.find(uStateId);
					CBaseStateCfg* pCfg = CBaseStateCfg::GetByGlobalId(uStateId);
					if(itStateIdCount != mapStateIdCount.end())
					{
						sStateInfo << "状态[" << pCfg->GetName() << "]控制值计数 = " << itStateIdCount->second;
						--itStateIdCount->second;
						//cout << "SetState(" << eState << ", false) 状态[" << uStateId << "]Count = " << --itStateIdCount->second << "\n";
						if(itStateIdCount->second <= 0)
						{
							mapStateIdCount.erase(itStateIdCount);
						}
					}
					else
					{
						sStateInfo << "状态[" << pCfg->GetName() << "]控制值计数不存在";
					}
				}
				else
				{
					int32& iNonStateCount = m_mapStateCtrlCount[eState].m_iNonStateCount;
					--iNonStateCount;
					//cout << "SetState(" << eState << ", false) 非状态Count = " << iNonStateCount << "\n";
				}

				if(pExp)
				{
					if(uStateId == 0)
					{
						//GenErr("CFighterCtrlInfo::SetState错误\n");
					}
					else
					{
						//为查状态修改控制量异常而加的代码
						stringstream sErr;
						int32& iNonStateCount = m_mapStateCtrlCount[eState].m_iNonStateCount;
						sErr << " " << sStateInfo.str() << " 非状态控制值计数 = " << iNonStateCount << "\n";
						if(iNonStateCount < 0)
							iNonStateCount = 0;

						pExp->AppendMsg(sErr.str().c_str());
					}
				}
			}
		}
	}

}

void CAllStateMgrServer::ClearCtrlStateCount()
{
	m_mapStateCtrlCount.clear();
}




