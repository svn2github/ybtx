#include "stdafx.h"
#include "CClientCharacterMsgDispatcher.h"
#include "CCharacterDirector.h"
#include "CIntObjClient.h"
#include "ICharacterDirectorCallbackHandler.h"
#include "IIntObjCallbackHandler.h"
#include "TCharacterAdapterClient.h"
#include "CFighterFollower.h"
#include "CCoreObjectDirector.h"
#include "ICharacterDirectorHandler.h"
#include "IActorDirectorHandler.h"
#include "IFighterDirectorHandler.h"
#include "ICharacterFollowerHandler.h"
#include "CActorIntObjClient.h"
#include "CFacingMgr.h"
#include "CNPCGazedAtMgr.h"
#include "CRenderObject.h"
#include "CTeamClientManager.h"
#include "ITeamClientManagerHandler.h"
#include "CEntityClientCreator.h"
#include "CFighterVariantCallback.h"
#include "CSyncVariantClientHolder.h"
#include "CSyncVariantClient.h"

void CClientCharacterMsgDispatcher::OnCharacterCommand(const CGas2GacCPP_InitFollower *pCmd, void* pParam)
{
	CCoreObjectFollower* pFollower = CCoreObjectFollower::GetObjectFollower(pCmd->uObjID);
	if (pFollower == NULL) 
		return; //{ GenErr("Can't find CoreObj of Follower"); }
	if (pFollower->GetHandler() != NULL && !pCmd->bIsBattleHorse) { return; }
	CCharacterFollower* pCharacterFollower = CEntityClientCreator::CreateCharacterFollower(pFollower, pCmd->uEntityID, pCmd->bIsBattleHorse);

	if (pCharacterFollower->GetCallbackHandler())
		pCharacterFollower->GetCallbackHandler()->OnCreated(pCharacterFollower, pCmd->bIsBattleHorse);
}

void CClientCharacterMsgDispatcher::OnCharacterCommand(const CGas2GacCPP_InitFollowerSyncInfo *pCmd, void* pParam)
{
	CCoreObjectFollower* pFollower = CCoreObjectFollower::GetObjectFollower(pCmd->uObjID);
	if (pFollower == NULL) 
		return ;//{ GenErr("Can't find CoreObj of Follower"); }

	CCharacterFollower* pCharacterFollower = CEntityClientManager::GetInst()->GetCharacter(pCmd->uEntityID);
	if (pCharacterFollower == NULL)
		return;

	Ast(pCharacterFollower->GetLocalID() == pFollower->GetLocalID());

	if (pCharacterFollower->CheckSyncDataReceived())
	{
		if (pCharacterFollower->GetCallbackHandler()) 
			pCharacterFollower->GetCallbackHandler()->OnLeaveSyncAoi(pCharacterFollower);
	}

	if (pCharacterFollower->GetCallbackHandler()) 
		pCharacterFollower->GetCallbackHandler()->OnIntoSyncAoi(pCharacterFollower);

	//pCharacterFollower->SetSpeed(pCmd->fMoveSpeed);
	CActorFollower* pActor = pCharacterFollower->GetActor();
	//pActor->SetWalkBackRadius(pCmd->uWalkBackRadius);
	
	//pActor->ChangeMoveState(EMoveState(pCmd->uMoveState), true);
	//pActor->SetMoveDir(pCmd->uMoveDir);
	//if(EActionState(pCmd->uActionState) != eAS_Die)	
	//	pActor->DoActionState(EActionState(pCmd->uActionState), pCmd->uActionDir, true, true, pCmd->uActionIndex);
	//else
	//	pActor->DoActionState(eAS_Dead, pCmd->uActionDir, true, true);

	//pActor->GetFacingMgr()->SetNADistance(pCmd->fNADistance);

	// 面部朝向
	if (pCmd->uTargetID)
	{
		CCharacterFollower* pCharDst = CEntityClientManager::GetInst()->GetCharacter(pCmd->uTargetID);
		if (!pCharDst)
			pCharDst->GetFacingMgr()->SetDstEntitiyID(pCmd->uTargetID);
		pCharDst->GetFacingMgr()->CreateFacingPair(pCharacterFollower, pCharDst);
	}
	CCharacterFollower* pCharacter = CEntityClientManager::GetInst()->GetCharacter(pCmd->uDirectorEntityID);
	if (pCharacter)
	{
		CCharacterDirector* pDirector = pCharacter->CastToCharacterDirector();
		if (pDirector && pCharacterFollower->GetAITypeID() == ENpcAIType_Task)
		{
			pDirector->GetNPCGazedAtMgr()->AddSyncNPC(pCharacterFollower);
		}
	}

	//CFighterFollower* pFighterFollower = pCharacterFollower->GetFighter();

	//if(!pFighterFollower->m_pSetStateVariantCallback)
	//{
	//	pFighterFollower->m_pSetStateVariantCallback = new CFighterSetStateVariantCallback(pFighterFollower);
	//	pCharacterFollower->RegSyncVarChangedCallback("zzzzzStateIsSync", pFighterFollower->m_pSetStateVariantCallback);

	//	const CSyncVariantClientHolder* pSyncVarHolder = pCharacterFollower->GetCoreObj()->GetSyncVariantHolder();
	//	pSyncVarHolder->EnumVariant("State", &CFighterFollower::OnSetStateForInit);
	//}


	//pCharacterFollower->SetSyncDataReceived(true);
	//pCharacterFollower->SetCanBeTakeOver(pCmd->bCanTakeOver);


}

void CClientCharacterMsgDispatcher::OnCharacterCommand(const CGas2GacCPP_InitFollowerEnd *pCmd, void* pParam)
{
	//cout << "CGas2GacCPP_InitFollowerEnd EntityID = " << pCmd->uEntityID << " DirectorID = " << pCmd->uDirectorID  << endl;
	CCharacterFollower* pCharacterFollower = CEntityClientManager::GetInst()->GetCharacter(pCmd->uEntityID);
	if (pCharacterFollower == NULL)
		return;


	pCharacterFollower->SetAITypeID(ENpcAIType(pCmd->uNpcAIType));
	pCharacterFollower->SetActiveNpc(pCmd->bActiveNpc);
	pCharacterFollower->SetNpcType(ENpcType(pCmd->uNpcType));
	pCharacterFollower->SetNpcAttackType(EAttackType(pCmd->uNpcAttackType));
	pCharacterFollower->InitRealName();
	pCharacterFollower->CppSetGameCamp(pCmd->iGameCamp);
	CCharacterFollower* pCharacter = CEntityClientManager::GetInst()->GetCharacter(pCmd->uDirectorID);
	CCharacterFollower* pMasterCharacter = pCharacterFollower->GetMaster();
	if(pCharacterFollower->IsBattleHorse() && pMasterCharacter)	//如果是战斗坐骑，将CoreObj的handler重新设置为主人的CharacterAdapter对象
	{
		}
	CCharacterDirector* pCharacterDirector = pCharacter->CastToCharacterDirector();
	if (pCharacterDirector != NULL)
	{
		pCharacterDirector->AddCharacterFollowerToSet(pCharacterFollower);
	}
}

void CClientCharacterMsgDispatcher::OnCharacterCommand(const CGas2GacCPP_ClearFollowerSyncInfo *pCmd, void* pParam)
{
	CCoreObjectFollower* pFollower = CCoreObjectFollower::GetObjectFollower(pCmd->uObjID);
	if (pFollower == NULL)
		return;//{ GenErr("Can't find CoreObj of Follower"); }

	if (pFollower->GetHandler() && pFollower->GetHandler()->CastToObjDirHandler())
		return;
	CCharacterFollower* pCharacterFollower = CCharacterFollower::GetCharacter(pFollower);
	if(pCharacterFollower == NULL)
		return;

	pCharacterFollower->GetFacingMgr()->RemoveFacingPair();
	CCharacterFollower* pCharacter = CEntityClientManager::GetInst()->GetCharacter(pCmd->uDirectorEntityID);
	if (pCharacter)
	{
		CCharacterDirector* pDirector = pCharacter->CastToCharacterDirector();
		if (pDirector && pCharacterFollower->GetAITypeID() == ENpcAIType_Task)
		{
			pDirector->GetNPCGazedAtMgr()->DelSyncNPC(pCharacterFollower);
		}
	}

	if (pCharacterFollower->GetCallbackHandler()) 
		pCharacterFollower->GetCallbackHandler()->OnLeaveSyncAoi(pCharacterFollower);
	
	pCharacterFollower->SetSyncDataReceived(false);
}

void CClientCharacterMsgDispatcher::OnCharacterCommand(const CGas2GacCPP_OnDead *pCmd, void *pParam)
{
	if( !VerifyCharacter(pCmd->uEntityID) ) 
		return;

	CEntityClientManager::GetInst()->GetCharacter(pCmd->uEntityID)->OnDead();
}

void CClientCharacterMsgDispatcher::OnCharacterCommand(const CGas2GacCPP_OnRemoved *pCmd, void *pParam)
{
	if( !VerifyCharacter(pCmd->uEntityID) ) 
		return;
	
	CCharacterFollower* pFollower = CEntityClientManager::GetInst()->GetCharacter(pCmd->uEntityID);
	pFollower->SetRemovedImmediately(true);
	pFollower->OnDead();
}

void CClientCharacterMsgDispatcher::OnCharacterCommand(const CGas2GacCPP_OnReborn *pCmd, void *pParam)
{
	if( !VerifyCharacter(pCmd->uEntityID) ) 
		return;

	CEntityClientManager::GetInst()->GetCharacter(pCmd->uEntityID)->OnReborn();
}

void CClientCharacterMsgDispatcher::OnCharacterCommand(const CGas2GacCPP_InitDirector* pCmd, void *pParam)
{
	CCoreObjectFollower* pFollower = CCoreObjectFollower::GetObjectFollower(pCmd->uObjID);
	if (pFollower == NULL) 
		return ;//{ GenErr("Can't find CoreObj of director"); }
	if (pFollower->GetHandler()) { GenErr("CoreObj already has handler");}
	if(pFollower->GetType() != eCOT_Director)
	{
		GenErr("Is not CoreObjectDirector");
	}
	CCharacterDirector* pCharacterDirector = CEntityClientCreator::CreateCharacterDirector(class_cast<CCoreObjectDirector*>(pFollower), pCmd->uEntityID);

	pCharacterDirector->CppSetBirthCamp(ECamp(pCmd->uBirthCamp));
	pCharacterDirector->CppSetCamp(ECamp(pCmd->uCamp));
	pCharacterDirector->CppSetClass(EClass(pCmd->uClass));
	pCharacterDirector->CppSetLevel(pCmd->uLevel);
	pCharacterDirector->CppSetPKState(pCmd->bPKState);
	//pCharacterDirector->CppSetPKState(EPKState(pCmd->uPKState));
	//pCharacterDirector->CppSetZoneType(EZoneType(pCmd->uZoneType));
	pCharacterDirector->RegistCommonVarriantCallback();

	ICharacterFollowerCallbackHandler* pFolHander = pCharacterDirector->GetCallbackHandler();
	if( pFolHander )
	{
		ICharacterDirectorCallbackHandler* pDirHandler=pFolHander->CastToDirectorHandler();
		if( pDirHandler )
			pDirHandler->OnCreated(pCharacterDirector, false);
	}

	CFighterFollower* pFighterFollower = pCharacterDirector->GetFighter();

	pFighterFollower->m_pSetStateVariantCallback = new CFighterSetStateVariantCallback(pFighterFollower);
	pCharacterDirector->RegSyncVarChangedCallback("zzzzzStateIsSync", pFighterFollower->m_pSetStateVariantCallback, eCTI_Agile);

	const CSyncVariantClientHolder* pSyncVarHolder = pCharacterDirector->GetSyncVariantHolder(eCTI_Agile);

	const CSyncVariantClient* pStateVariant = pSyncVarHolder->GetRootVariant()->GetMember("State");
	if(pStateVariant)
		pSyncVarHolder->EnumVariant("State", &CFighterFollower::OnSetStateForInit);
	const CSyncVariantClientHolder* pViewVarHolder = pCharacterDirector->GetViewVariantHolder();
	pViewVarHolder->EnumVariant("Attribute", &CFighterFollower::OnSetStateForInit);
	pViewVarHolder = pCharacterDirector->GetViewVariantHolder(eCTI_Agile);
	pViewVarHolder->EnumVariant("AgileValue", &CFighterFollower::OnSetStateForInit);
	pViewVarHolder->EnumVariant("PKSwitch", &CFighterFollower::OnSetStateForInit);
}

void CClientCharacterMsgDispatcher::OnCharacterCommand(const CGas2GacCPP_InitDirectorEnd* pCmd, void *pParam)
{
	if( !VerifyCharacter(pCmd->uEntityID) ) 
		return;
	
	CCharacterDirector* pDirector = class_cast<CCharacterDirector*>(CEntityClientManager::GetInst()->GetCharacter(pCmd->uEntityID));

	ICharacterFollowerCallbackHandler* pFolHander = pDirector->GetCallbackHandler();
	if( pFolHander )
	{
		ICharacterDirectorCallbackHandler* pDirHandler=pFolHander->CastToDirectorHandler();
		if( pDirHandler )
			pDirHandler->OnCreatedEnd(pDirector);
	}
}

void CClientCharacterMsgDispatcher::OnCharacterCommand(const CGas2GacCPP_BeginTransfer* pCmd, void *pParam)
{
	if( !VerifyCharacter(pCmd->uEntityID) ) 
		return;
	CCharacterFollower* pCharacter = CEntityClientManager::GetInst()->GetCharacter(pCmd->uEntityID);
	CCharacterDirector* pDirector = pCharacter->CastToCharacterDirector();
	Ast(pDirector);
	//需要将Character上所有和区域时间相关的tick都注销掉
	pDirector->CancelAutoTracker(false);
	pDirector->SetTransfer(pCmd->bTransfer);
	pDirector->GetActor()->ChangeActionState(eAS_Idle_BackWpn);
	string strSceneName((TCHAR*)(pCmd+1), pCmd->udbSceneNameLen);
	if (pDirector->GetCallbackHandler())
		pDirector->GetCallbackHandler()->OnBeginTransfer(pDirector,strSceneName.c_str());
}

void CClientCharacterMsgDispatcher::OnCharacterCommand(const CGas2GacCPP_OnMoveSpeedChange* pCmd, void *pParam)
{
	if( !VerifyCharacter(pCmd->uEntityID) ) return;
	CCharacterFollower* pCharacterFollower = CEntityClientManager::GetInst()->GetCharacter(pCmd->uEntityID);
	if (pCharacterFollower->GetType()==eCOT_Director)
	{
		//cout << "director's speed has been changed to " << pCmd->fSpeed << endl; 
		class_cast<CCharacterDirector*>(pCharacterFollower)->SetCoreObjSpeed(pCmd->fSpeed);
	}
	pCharacterFollower->SetSpeed(pCmd->fSpeed);
}

void CClientCharacterMsgDispatcher::OnCharacterCommand(const CGas2GacCPP_OnCancelAutoTrack* pCmd, void* pParam)
{
	if( !VerifyCharacter(pCmd->uObjID) )
		return;
	CCharacterFollower* pChar = CCharacterFollower::GetCharacterByID(pCmd->uObjID);
	if(pChar->GetGameEntityType() == eGET_CharacterDirector)
	{
		CCharacterDirector* pCharacterDirector = class_cast<CCharacterDirector*>(pChar);
		pCharacterDirector->CancelAutoTracker(pCmd->bCancelNAOnly);
	}
}


void CClientCharacterMsgDispatcher::OnCharacterCommand(const CGas2GacCPP_DestoryBattleHorse* pCmd, void* pParam)
{
	if(!VerifyCharacter(pCmd->uEntityID))
		return;
	CCharacterFollower* pChar = CCharacterFollower::GetCharacterByID(pCmd->uEntityID);
	Ast(pChar);
	//CCharacterFollower* pMaster = pChar->GetMaster();
	//Ast(pChar->GetMaster());
	//cout << "删除BattleHorse " << pCmd->uEntityID << endl;
	pChar->OnDestroy();
}

void CClientCharacterMsgDispatcher::OnCharacterCommand(const CGas2GacCPP_RemoveTeamMember* pCmd, void *pParam)
{
	CTeamClientManager::MapSyncInfo& MapSyncInfo = CTeamClientManager::GetSyncInfoMap();
	CTeamClientManager::MapSyncInfo::iterator it = MapSyncInfo.find(pCmd->uEntityID);
	if (it!=MapSyncInfo.end())
	{
		SafeDelete(it->second);
		MapSyncInfo.erase(it);
	}
}

void CClientCharacterMsgDispatcher::OnCharacterCommand(const CGas2GacCPP_TeamSyncInfo* pCmd, void *pParam)
{
	CTeamClientManager::MapSyncInfo& MapSyncInfo = CTeamClientManager::GetSyncInfoMap();
	CTeamClientManager::MapSyncInfo::iterator it = MapSyncInfo.find(pCmd->uEntityID);
	CSyncSceneInfo* pSyncScene;
	if (it!=MapSyncInfo.end())
	{
		pSyncScene =it->second;
	}
	else
	{
		pSyncScene = new CSyncSceneInfo(pCmd->uEntityID);
		MapSyncInfo.insert(make_pair(pCmd->uEntityID,pSyncScene));
	}
	pSyncScene->UpdateInfo(pCmd);
	CTeamClientManager::GetHandler()->UpdateTeamInfo();
}
