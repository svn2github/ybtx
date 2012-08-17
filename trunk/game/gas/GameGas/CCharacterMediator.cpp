#include "stdafx.h"
#include "CCharacterMediator.h"
#include "CConnServer.h"
#include "PtrlGas2GacCPPDef.h"
#include "PtrlGac2GasCPPDef.h"
#include "CCharacterDictator.inl"
#include "TCharacterAdapterServer.inl"
#include "ICharacterMediatorHandler.h"
#include "ICharacterMediatorCallbackHandler.h"
#include "CIntObjServer.h"
#include "NormalAttack.h"
#include "CNormalAttackCfg.h"
#include "CAllStateMgr.h"
#include "CBattleArrayMagicServer.h"
#include "CFighterMediator.h"
#include "CActorServer.h"
#include "IActorServerHandler.h"
#include "CEntityServer.inl"
#include "TEntityAdapterServer.inl"
#include "IFighterServerHandler.h"
#include "ErrLogHelper.h"
#include "CTriggerEvent.h"
#include "CActorIntObj.h"
#include "CMagicMgrServer.h"
#include "CServantServerMgr.h"
#include "ICharacterDictatorCallbackHandler.h"
#include "NpcInfoMgr.h"
#include "CCoreObjectMediator.h"
#include "ISend.h"
#include "CStateSync.h"
#include "FighterProperty.inl"
#include "TSqrAllocator.inl"
#include "CCoreSceneServer.h"
#include "CGameConfigServer.h"
#include "TPropertyHolder.inl"


CCharacterMediator::CCharacterMediator(CCoreObjectMediator *pCoreObj)
: m_uListeningWndRef(0)
, m_bInGatherProcess(false)
, m_pDuelTarget(NULL)
{
	Ast (pCoreObj)	
 	Init<CCharacterMediatorTraits>(pCoreObj);
	InitAoiType(eAIET_Player, eAIST_PlayerOrServant);

	InitPlaySizeForNpcSleepDim();
}

CCharacterMediator::~CCharacterMediator()
{
}

void CCharacterMediator::InitPlayerAoi()
{
	SetEyeSight(CGameConfigServer::Inst()->GetPlayerViewAoiEyeSight());
	SetSize(CGameConfigServer::Inst()->GetPlayerAoiSize());
	//SetBarrierSize(CGameConfigServer::Inst()->GetPlayerAoiSize());
	SetBottomSize(CGameConfigServer::Inst()->GetPlayerAoiSize());
	SetStealth(CGameConfigServer::Inst()->GetPlayerAoiStealth());
	SetKeenness(CGameConfigServer::Inst()->GetPlayerAoiKeenness());
	SetViewGroup(CGameConfigServer::Inst()->GetPlayerAoiViewGroup());
}

void CCharacterMediator::Release()
{
	DestoryServant(ENpcType_BattleHorse);
	CTriggerEvent::MessageEvent(eTST_Trigger, GetFighter(), NULL, eSET_Clear);

	ClearAllServant();
	DestoryQueueMgr();
	DestroyServantMgr();
	DestroyGroupMgr();

	//有时序依赖
	GetFighter()->Notify(eCE_Offline);
	GetFighter()->ClearAllStateMgr();
	GetCallbackHandler()->CastToMediatorHandler()->OnDestroyed(this);
	delete GetFighter();
	m_pFighter = NULL;
	delete this;
}

void CCharacterMediator::SetTarget(CCharacterDictator* pTarget)
{
	CFighterDictator* pFighterDictator = GetFighter();
	Ast(pFighterDictator);

	if(pTarget)
	{
		pFighterDictator->SetTarget(pTarget->GetFighter());

		CFighterMediator* pFighterMediator = pFighterDictator->CastToFighterMediator();
		ICharacterMediatorCallbackHandler* pCallbackHandler = NULL;
		
		if(pFighterMediator)
			pCallbackHandler = pFighterMediator->GetCallBackHandler();
		
		if(pCallbackHandler)
		{
			uint32 uEntityID = this->GetEntityID();
			uint32 uTargetEntityID = pTarget->GetEntityID();
			pCallbackHandler->OnSetLockObj( uEntityID, uTargetEntityID );
		}
	}
	else
	{	
		pFighterDictator->SetTarget(NULL, true);
	}
}

void CCharacterMediator::CppInit(EClass eClass, uint32 uLevel, ECamp eCamp)
{
	// 时序问题，Actor必须要在前，Fighter初始普攻时要用到Actor上的Vairant变量
	Ast(GetActor());
	GetActor()->InitActorVariant();
	Ast(GetFighter());
	CFighterMediator* pFighter=GetFighter()->CastToFighterMediator();
	Ast(pFighter);
	pFighter->CppInit(eClass,uLevel,eCamp);
}

void CCharacterMediator::ReSendToSelf()
{
	Ast(GetFighter());
	CFighterMediator* pFighter=GetFighter()->CastToFighterMediator();
	Ast(pFighter);
	pFighter->ReSendToSelf();
}

void CCharacterMediator::CppLevelUp()
{
	Ast(GetFighter());
	CFighterMediator* pFighter=GetFighter()->CastToFighterMediator();
	Ast(pFighter);
	pFighter->OnLevelUp();
}
void CCharacterMediator::CppInitHPMP(uint32 uCurHP, uint32 UCurMp)
{
	Ast(GetFighter());
	CFighterMediator* pFighter=GetFighter()->CastToFighterMediator();
	Ast(pFighter);
	pFighter->CppInitHPMP(uCurHP,UCurMp);
}

void CCharacterMediator::CppFullRage(bool bFull)
{
	Ast(GetFighter());
	CFighterMediator* pFighter=GetFighter()->CastToFighterMediator();
	Ast(pFighter);
	pFighter->CppFullRage(bFull);
}

void CCharacterMediator::CppInitByDead()
{
	Ast(GetFighter());
	CFighterMediator* pFighter = GetFighter()->CastToFighterMediator();
	Ast(pFighter);
	pFighter->m_HealthPoint.LimitSet(0, pFighter);

	CEntityServer* pEntity = pFighter->GetHandler()->GetEntity();
	pEntity->DisableDirectorMoving(true);
	pEntity->SetSizeType(eAIST_DeadPlayer);	

	pFighter->CppSetAlive(false);
	pFighter->ResetCtrlValueVariant();
	pFighter->m_HealthPoint.StopRenewHP();
	pFighter->m_ManaPoint.StopRenewMP();
	pFighter->m_RagePoint.StopReduceRP();
	pFighter->m_EnergyPoint.StopRenewEP();

	SetAndSyncMoveState(eMS_LockStop, true);
	SetAndSyncActionState(eAS_Die);
}

void CCharacterMediator::CPPInitSkillCoolDownTime(CSkillCoolDownDBDataMgr* pSkillCoolDownDBDataMgr)
{
	Ast(GetFighter());
	CFighterMediator* pFighter=GetFighter()->CastToFighterMediator();
	Ast(pFighter);
	pFighter->CPPInitSkillCoolDownTime(pSkillCoolDownDBDataMgr);
}

void CCharacterMediator::ResetAllCoolDown()
{
	Ast(GetFighter());
	GetFighter()->ResetAllCoolDown();	
}

void CCharacterMediator::RemoteInit() const
{
	Ast(GetFighter());
	CFighterMediator* pFighter=GetFighter()->CastToFighterMediator();
	Ast(pFighter);
	CGas2GacCPP_InitDirector Cmd;
	Cmd.uCamp = uint8(CppGetCamp());
	Cmd.uBirthCamp = uint8(CppGetBirthCamp());
	Cmd.uClass = uint8(CppGetClass());
	Cmd.uLevel = CppGetLevel();
	Cmd.bPKState = CppGetPKState();
	//Cmd.uPKState = uint8(CppGetPKState());
	//Cmd.uZoneType = uint8(CppGetZoneType());
	Cmd.uEntityID = GetEntityID();
	Cmd.uObjID = GetGlobalID();
	CConnServer* pConn = GetConnection();
	if (pConn)
		pConn->Send(&Cmd, sizeof(Cmd));

	pFighter->ResetCtrlValueVariant();
	
	pFighter->OnCaughtViewSightOf(pFighter, 0, false);
}

void CCharacterMediator::InitSpeed() const
{
	Ast(GetFighter());
	GetFighter()->OnMoveSpeedChange(GetFighter()->m_RunSpeed.Get(GetFighter()), "初始化", NULL);

	CConnServer* pConn = GetConnection();
	CGas2GacCPP_InitDirectorEnd endCmd;
	endCmd.uEntityID = GetEntityID();
	endCmd.uObjID = GetGlobalID();
	if (pConn)
		pConn->Send(&endCmd, sizeof(endCmd));
}

void CCharacterMediator::ReSendSpeedToSelf() const
{
	Ast(GetFighter());
	CGas2GacCPP_OnMoveSpeedChange	Cmd;
	Cmd.uEntityID=GetEntityID();
	Cmd.fSpeed=GetFighter()->m_RunSpeed.Get(GetFighter());
	GetFighter()->SendCmdToGac(&Cmd, eOnlyToDirector);
}

void CCharacterMediator::RemoteInitBattleHorse(CCharacterDictator* pBattleHorse)const
{
	uint32 uObjGlobalId = pBattleHorse->GetGlobalID();
	//cout << "	CCharacterMediator::RemoteInitBattleHorse() call InitFolBattleHorseViewInfo()\n";
	InitFolBattleHorseViewInfo(pBattleHorse, uObjGlobalId, 0, true);
	InitFolSyncInfo(pBattleHorse, uObjGlobalId, 0, true);
}

void CCharacterMediator::RemoteInitBattleHorseToSelf()const
{
	if(GetServantMgr())
	{
		uint32 uBattleHorseEntityId = GetServantMgr()->GetServantIDByType(ENpcType_BattleHorse);	//如果Mediator有多个战斗坐骑则需要用GetServantSetByType遍历
		if(uBattleHorseEntityId != 0)
		{
			CCharacterDictator* pBattleHorse = CCharacterDictator::GetCharacterByID(uBattleHorseEntityId);
			if(pBattleHorse)
			{
				uint32 uObjGlobalId = pBattleHorse->GetGlobalID();
				//cout << "	CCharacterMediator::RemoteInitBattleHorseToSelf() call InitFolBattleHorseViewInfo()\n";
				InitFolBattleHorseViewInfo(pBattleHorse, uObjGlobalId, 0, false);
				InitFolSyncInfo(pBattleHorse, uObjGlobalId, 0, false);
			}
		}
	}
}

void CCharacterMediator::RemoteUnitBattleHorse(CCharacterDictator* pBattleHorse)const
{
	Ast(GetFighter());
	CFighterMediator* pFighterObserver = GetFighter()->CastToFighterMediator();
	Ast(pFighterObserver);
	pFighterObserver->OnLostSyncSightOf(pBattleHorse->GetFighter(), 0, true);
}

void CCharacterMediator::OnCaughtViewSightOf(uint32 uObjGlobalId, uint32 uDimension)
{
	
}

void CCharacterMediator::OnLostViewSightOf(uint32 uObjGlobalId, uint32 uDimension)
{

}

CCharacterMediator* CCharacterMediator::GetCharacterMediatorByConn(CConnServer* pConn)
{
	CCharacterDictator* pChar = CCharacterDictator::GetCharacterByCoreObj(pConn->GetObjectMediator());
	if( !pChar )
		return NULL;	
	return class_cast<CCharacterMediator*>(pChar);
}

void CCharacterMediator::InitFolBattleHorseViewInfo(CCharacterDictator* pCharBattleHorse, uint32 uObjGlobalId, uint32 uDimension, bool bMultiCast) const
{
	CCoreObjectServer *pCoreObj = CCoreObjectServer::GetObjectServer(uObjGlobalId);
	Ast(pCoreObj);
	CCharacterDictator *pCharacter = CCharacterDictator::GetCharacterByCoreObj(pCoreObj);

	Ast(GetFighter());
	CFighterMediator* pFighterObserver = GetFighter()->CastToFighterMediator();
	Ast(pFighterObserver);

	Ast(pCharBattleHorse);
	CFighterDictator* pFighterBattleHorse = pCharBattleHorse->GetFighter();
	Ast(pFighterBattleHorse);

	CGas2GacCPP_InitFollower Cmd;
	Cmd.uObjID = uObjGlobalId; 
	Cmd.uEntityID = pCharBattleHorse->GetEntityID();
	Cmd.uDirectorID = GetEntityID();
	Cmd.bIsBattleHorse = true;

	if(bMultiCast)
	{
		//cout << "	InitFolBattleHorseViewInfo() send CGas2GacCPP_InitFollower to eViewSight\n";
		pFighterBattleHorse->SendCmdToGac(&Cmd, eViewSight);
	}
	else
	{
		//cout << "	InitFolBattleHorseViewInfo() send CGas2GacCPP_InitFollower to eOnlyToDirector\n";
		pFighterObserver->SendCmdToGac(&Cmd, eOnlyToDirector);
	}

	pFighterObserver->OnCaughtViewSightOf( pFighterBattleHorse, uDimension, bMultiCast);


	if (pCharacter->GetFighter())
	{
		CGas2GacCPP_OnAddServant	cmdAddServant;
		cmdAddServant.uObjID = pCharacter->GetEntityID();
		cmdAddServant.uServantObjID = pCharBattleHorse->GetEntityID();
		cmdAddServant.uType = uint8(pCharBattleHorse->GetNpcType());
		if(bMultiCast)
		{
			if(pCharBattleHorse->GetMaster() && pCharBattleHorse->GetMaster()->GetFighter())
			{
				pFighterBattleHorse->SendCmdToGac(&cmdAddServant, eViewSight);
				//pCharBattleHorse->GetMaster()->GetFighter()->SendCmdToGac(&cmdAddServant, eViewSight);
				//pCharBattleHorse->GetMaster()->GetFighter()->OnAddServant(pCharBattleHorse->GetEntityID());
			}
		}
		else
			pFighterObserver->SendCmdToGac(&cmdAddServant, eOnlyToDirector);
	}

	CGas2GacCPP_InitFollowerEnd CmdEnd;
	CmdEnd.uEntityID = pCharBattleHorse->GetEntityID();
	CmdEnd.uDirectorID = GetEntityID();
	CmdEnd.uNpcAIType = uint8(pCharBattleHorse->GetAITypeID());
	CmdEnd.bActiveNpc = NpcInfoMgr::BeActiveNpc(ENpcAIType(CmdEnd.uNpcAIType));
	CmdEnd.uNpcType = uint8(pCharBattleHorse->GetNpcType());
	CmdEnd.uNpcAttackType = uint8(pCharBattleHorse->GetNpcAttackType());

	if(bMultiCast)
	{
		pFighterBattleHorse->SendCmdToGac(&CmdEnd, eViewSight);
	}
	else
	{
		pFighterObserver->SendCmdToGac(&CmdEnd, eOnlyToDirector);
	}
}

void CCharacterMediator::OnTransfered(CCoreSceneServer* pOldScene,const CFPos& OldPos)
{
	if (pOldScene)
		GetFighter()->OnTransferEnd(pOldScene, OldPos);

	if (GetCallbackHandler())
		GetCallbackHandler()->CastToMediatorHandler()->OnTransfered(this,pOldScene,OldPos.x,OldPos.y,GetScene());

#ifndef SEND_STATE_BY_VAR
	GetFighter()->GetAllStateMgr()->SyncAllState(GetFighter());
#endif
}

void CCharacterMediator::InitFolSyncInfo(CCharacterDictator* pCharacter, uint32 uGlobalID, uint32 uDimension, bool bMultiCast) const
{
	if (pCharacter==NULL)
		return;

	CActorServer* pActor = pCharacter->GetActor();
	Ast(pActor);

	CGas2GacCPP_InitFollowerSyncInfo Cmd;
	Cmd.uObjID = uGlobalID;
	Cmd.uDirectorEntityID = GetEntityID();
	Cmd.uEntityID = pCharacter->GetEntityID();	
	if (pCharacter->GetTarget())
		Cmd.uTargetID = pCharacter->GetTarget()->GetEntityID();
	else
		Cmd.uTargetID = 0;
	
	Cmd.uWalkBackRadius = pCharacter->GetWalkBackRadius();
	Cmd.uMoveState = uint8(pActor->GetMoveState());
	Cmd.uMoveDir = pCharacter->GetMoveDir();
	Cmd.uActionState = uint8(pActor->GetActionState());
	Cmd.uActionDir = pCharacter->GetActionDir();
	//Cmd.uActionIndex = pActor->GetActionIndex();
	
	Cmd.fNADistance	= pCharacter->GetFighter()->GetNormalAttackMgr()->GetAttackDistance();
	Cmd.bCanTakeOver = pCharacter->CanBeTakeOver();
	CFighterDictator* pFighterFol = pCharacter->GetFighter();
	Ast(pFighterFol);
	if (pCharacter->CppGetCtrlState(eFCS_InWalkState))
		Cmd.fMoveSpeed = pFighterFol->m_WalkSpeed.Get(pFighterFol);
	else
		Cmd.fMoveSpeed = pFighterFol->m_RunSpeed.Get(pFighterFol);
	
	//if(bMultiCast)
	//{
	//	GetIS(0)->Send(&Cmd, sizeof(Cmd));
	//}
	//CConnServer* pConn = GetConnection();
	//if (pConn)
	//	pConn->Send(&Cmd, sizeof(Cmd)); 
	//}
	
	Ast(GetFighter());
	CFighterMediator* pFighter=GetFighter()->CastToFighterMediator();
	Ast(pFighter);

	CFighterDictator* pIncomer = pCharacter->GetFighter();

	if(bMultiCast)
		pIncomer->SendCmdToGac(&Cmd);
	else
	//	pFighter->SendCmdToGac(&Cmd, eOnlyToDirector);
		

	pFighter->OnCaughtSyncSightOf( pFighterFol, uDimension, bMultiCast);		//函数第二个参数应该为Character传进来的uDimension
}

void CCharacterMediator::ReSendSyncInfoToSelf()
{
	Ast(GetFighter());
	CFighterMediator* pFighter=GetFighter()->CastToFighterMediator();
	Ast(pFighter);
	pFighter->OnCaughtSyncSightOf( pFighter, 0, false);
}

void CCharacterMediator::ReSendSkillCoolDownTimeToSelf()
{
	Ast(GetFighter());
	CFighterMediator* pFighter=GetFighter()->CastToFighterMediator();
	pFighter->ReSendSkillCoolDownTimeToSelf();
}



void CCharacterMediator::ReSendHPMPToSelf()
{
	Ast(GetFighter());
	CFighterMediator* pFighter=GetFighter()->CastToFighterMediator();
	Ast(pFighter);
	pFighter->ReSendHPMPToSelf();
}

bool CCharacterMediator::IsSetupWeapon()
{
	CNormalAttackMgr* pAttackMgr = GetFighter()->GetNormalAttackMgr();
	Ast(pAttackMgr);
	CSingleHandNATick* pMainAttack = pAttackMgr->GetMHNormalAttack();
	Ast(pMainAttack);
	if(pMainAttack->HasWeapon() && pMainAttack->GetWeaponType() != eWT_None)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void CCharacterMediator::CppInitMHNA(const TCHAR* szMHName, const TCHAR* MHAttackType, const TCHAR* MHWeaponType, float fAttackSpeed, bool bInitPlayer)
{
	Ast(GetFighter());
	if(!bInitPlayer)
	{
		if(IsSetupWeapon())
			CTriggerEvent::MessageEvent(eTST_Trigger, GetFighter(), NULL, eSET_RemoveWeapon);
		CTriggerEvent::MessageEvent(eTST_Trigger, GetFighter(), NULL, eSET_WeaponModChange);
	}
	CFighterMediator* pFighter=GetFighter()->CastToFighterMediator();
	Ast(pFighter);
	pFighter->CppInitMHNA(szMHName, MHAttackType, MHWeaponType, fAttackSpeed);
	//if(!bInitPlayer && IsSetupWeapon())
	//	CTriggerEvent::MessageEvent(eTST_Trigger, GetFighter(), NULL, eSET_SetupWeapon);
}

void CCharacterMediator::CppInitAHNA(const TCHAR* szAHName, const TCHAR* AHAttackType, const TCHAR* AHWeaponType, float fAttackSpeed, bool bInitPlayer)
{
	//cout << "AHWeaponType " << AHWeaponType << endl;
	Ast(GetFighter());
	if(!bInitPlayer)
	{
		if(IsSetupWeapon())
			CTriggerEvent::MessageEvent(eTST_Trigger, GetFighter(), NULL, eSET_RemoveWeapon);
		CTriggerEvent::MessageEvent(eTST_Trigger, GetFighter(), NULL, eSET_WeaponModChange);
	}
	CFighterMediator* pFighter=GetFighter()->CastToFighterMediator();
	Ast(pFighter);
	pFighter->CppInitAHNA(szAHName, AHAttackType, AHWeaponType, fAttackSpeed);
	//if(!bInitPlayer && IsSetupWeapon())
	//	CTriggerEvent::MessageEvent(eTST_Trigger, GetFighter(), NULL, eSET_SetupWeapon);
}

void CCharacterMediator::CancelNormalAttack()
{
	Ast(GetFighter());
	CFighterDictator* pFighter = GetFighter();
	pFighter->CancelAttack();
}

EDoSkillResult	CCharacterMediator::PlayerDoFightSkill(const TCHAR* sName, uint32 uLevel)
{	
	Ast(GetFighter());
	CFighterMediator* pFighter=GetFighter()->CastToFighterMediator();
	Ast(pFighter);
	// 服务端 施放技能
	pFighter->SetFighterPos(GetPixelPos());
	return pFighter->DoSkill(sName, uLevel);
}

EDoSkillResult	CCharacterMediator::PlayerDoFightSkillWithoutLevel(const TCHAR* sName)
{	
	Ast(GetFighter());
	CFighterMediator* pFighter=GetFighter()->CastToFighterMediator();
	Ast(pFighter);
	// 服务端 施放技能
	pFighter->SetFighterPos(GetPixelPos());
	return pFighter->DoSkill(sName, 1);
}

EDoSkillResult	CCharacterMediator::PlayerDoPassiveSkill(const TCHAR* sName, uint32 uSkillLevel)
{
	Ast(GetFighter());
	CFighterMediator* pFighter=GetFighter()->CastToFighterMediator();
	Ast(pFighter);
	// 服务端 施放技能
	CFPos selfpos;
	pFighter->GetPixelPos(selfpos);
	pFighter->SetFighterPos(selfpos);
	pFighter->SetTargetPos(selfpos);
	return pFighter->PlayerDoPassiveSkill(sName, uSkillLevel);
}

EDoSkillResult	CCharacterMediator::PlayerDoItemSkill(const TCHAR* sName, uint32 uLevel)
{
	Ast(GetFighter());
	CFighterMediator* pFighter=GetFighter()->CastToFighterMediator();
	Ast(pFighter);
	return pFighter->DoSkill(sName,uLevel);
}

EDoSkillResult	CCharacterMediator::PlayerDoPosSkill(const TCHAR* sName, uint32 uLevel, const CFPos& Pos)
{
	Ast(GetFighter());
	CFighterMediator* pFighter=GetFighter()->CastToFighterMediator();
	Ast(pFighter);
	CFPos selfpos;
	pFighter->GetPixelPos(selfpos);
	pFighter->SetFighterPos(selfpos);
	GetFighter()->SetTargetPos(Pos);
	return pFighter->DoSkill(sName,uLevel);
}

EDoSkillResult	CCharacterMediator::PlayerDoPosSkillWithoutLevel(const TCHAR* sName, const CFPos& Pos)
{
	Ast(GetFighter());
	CFighterMediator* pFighter=GetFighter()->CastToFighterMediator();
	Ast(pFighter);
	CFPos selfpos;
	pFighter->GetPixelPos(selfpos);
	pFighter->SetFighterPos(selfpos);
	GetFighter()->SetTargetPos(Pos);
	return pFighter->DoSkill(sName,1);
}

void CCharacterMediator::InsertTalent(const TCHAR* szName, uint32 uPoint)
{
	Ast(GetFighter());
	GetFighter()->InsertTalent(szName, uPoint);
}

void CCharacterMediator::ClearAllTalent()
{
	Ast(GetFighter());
	GetFighter()->ClearAllTalent();
}

void CCharacterMediator::OnCharacterCommand(const CGac2GasCPP_LockObj* pCmd)
{
	CCharacterDictator* pTarget = CCharacterDictator::GetCharacterByID(pCmd->uLockObjId);

	if(pTarget)
		SetTarget(pTarget);
}

void CCharacterMediator::OnCharacterCommand(const CGac2GasCPP_MoveByDirection* pCmd)
{
	Ast(GetFighter());
	Ast(GetActor());
	uint8 uLastMoveDir = GetMoveDir();
	SetAndSyncMoveDir(pCmd->uMoveDir);

	if(uLastMoveDir != pCmd->uMoveDir)
		CTriggerEvent::MessageEvent(eTST_Special, GetFighter(), NULL, eSET_DirectionOrMoveSpeedChange);
		//GetFighter()->Notify(eCE_DirectionChange);
}

void CCharacterMediator::OnCharacterCommand(const CGac2GasCPP_SyncActionDir* pCmd)
{
	if (pCmd->bSyncToGac)
	{
		GetFighter()->OnTurnAround();
		SetAndSyncActionDir(pCmd->uActionDir, false, true);
	}
	else
	{
		GetActor()->SetServerDir(pCmd->uActionDir);
	}
}

void CCharacterMediator::OnCharacterCommand(const CGac2GasCPP_ChangeMoveMode* pCmd)
{
	CppSetCtrlState(eFCS_InWalkState, pCmd->bIsWalkMode);
}

void CCharacterMediator::OnCharacterCommand(const CGac2GasCPP_ChangeDrawWeaponMode* pCmd)
{
	CNormalAttackMgr* pAttackMgr = GetFighter()->GetNormalAttackMgr();
	Ast(pAttackMgr);
	CSingleHandNATick* pMainAttack = pAttackMgr->GetMHNormalAttack();
	CSingleHandNATick* pAssistAttack = pAttackMgr->GetAHNormalAttack();
	Ast(pMainAttack);
	if(pMainAttack->HasWeapon() && pMainAttack->GetWeaponType() == eWT_None && !pAssistAttack->HasWeapon())
		return;

	CppSetCtrlState(eFCS_InDrawWeaponMode, pCmd->bIsDrawWeaponMode);
	if (pCmd->bForceDraw)
		return;

	if (pCmd->bIsDrawWeaponMode)
	{
		SetAndSyncActionState(eAS_HoldWeapon);
		if (IsMoving())
		{
			Ast(GetFighter());
			if (GetFighter()->GetCtrlState(eFCS_InWalkState))
				SetAndSyncActionState(eAS_Walk_HoldWpn);
			else
				SetAndSyncActionState(eAS_Run_HoldWpn);
		}
	}
	else
	{
		SetAndSyncActionState(eAS_BackWeapon);
		if (IsMoving())
		{
			Ast(GetFighter());
			if (GetFighter()->GetCtrlState(eFCS_InWalkState))
				SetAndSyncActionState(eAS_Walk_BackWpn);
			else
				SetAndSyncActionState(eAS_Run_BackWpn);
		}
	}
}

void CCharacterMediator::OnCharacterCommand(const CGac2GasCPP_ChangeNormalAttacking* pCmd)
{
	CppSetCtrlState(eFCS_InNormalAttacking, pCmd->bIsNormalAttacking);
}

CSkillCoolDownDBDataMgr* CCharacterMediator::GetSkillCoolDownTime()
{
	Ast(GetFighter());
	return GetFighter()->ReturnSkillCoolDownTime();
}

uint32 CCharacterMediator::GetSkillCoolDownTimeByName(const TCHAR* szSkillName)
{
	Ast(GetFighter());
	return GetFighter()->GetSkillCoolDownTimeByName(szSkillName);
}
void CCharacterMediator::SaveSkillCoolDownTimeToDBEnd()
{
	Ast(GetFighter());
	GetFighter()->SaveSkillCoolDownTimeToDBEnd();
}

CAureMagicDBDataMgr* CCharacterMediator::SerializeAureMagicToDB()
{
	Ast(GetFighter());
	return GetFighter()->SerializeAureMagicToDB();
}

void CCharacterMediator::SaveAureMagicToDBEnd()
{
	Ast(GetFighter());
	GetFighter()->SaveAureMagicToDBEnd();
}

void CCharacterMediator::LoadAureMagicFromDB(const TCHAR* szAureMagicName, uint8 uSkillLevel, const TCHAR* sSkillName, EAttackType eAttackType)
{
	Ast(GetFighter());
	GetFighter()->LoadAureMagicFromDB(szAureMagicName,uSkillLevel,sSkillName,eAttackType);
}

void CCharacterMediator::AddListeningWnd()
{
	m_uListeningWndRef++;
}
void CCharacterMediator::DelListeningWnd()
{
	if (m_uListeningWndRef > 0)
		m_uListeningWndRef--;
}

void CCharacterMediator::SetInGatherProcess(bool bInProcess)
{
	m_bInGatherProcess = bInProcess;
}

bool CCharacterMediator::IsInGatherProcess()
{
	return m_bInGatherProcess;
}

void CCharacterMediator::OnMoveBegan()
{
	CCharacterDictator::OnMoveBegan();
	if (m_uListeningWndRef > 0)
	{
		Ast(GetCallbackHandler());
		ICharacterMediatorCallbackHandler* pHander=GetCallbackHandler()->CastToMediatorHandler();
		Ast(pHander);
		pHander->ListeningOpenWnd(this);
	}
}

void CCharacterMediator::AddEquipSuiteEff(const TCHAR* szEquipSuiteName, uint32 uTalentPoint)
{
	Ast(GetFighter());
	GetFighter()->InsertEquipTalent(szEquipSuiteName, uTalentPoint);
}

bool CCharacterMediator::RemoveEquipSuiteEff(const TCHAR* szEquipSuiteName, uint32 uTalentPoint)
{
	Ast(GetFighter());
	return GetFighter()->RemoveEquipTalent(szEquipSuiteName, uTalentPoint);
}

void CCharacterMediator::CreateBattleArray(const TCHAR* szArrayName,const TCHAR* szArrayPos,const TCHAR* szEntityID)
{
	Ast(GetFighter());
	CFighterMediator* pFighter=GetFighter()->CastToFighterMediator();
	Ast(pFighter);
	if (!szArrayName||!szArrayPos||!szEntityID)
	{
		return;
	}
	CMagicMgrServer* pMagicMgr = pFighter->GetMagicMgr();
	pMagicMgr->SetArrayName(szArrayName);
	CBattleArrayMagicServer::VecBattleArray_t vecArray;
	CBattleArrayMagicServer::Transfer(szArrayPos,szEntityID,vecArray);
	pMagicMgr->SetBattleArrayVector(vecArray);
	CGas2GacCPP_OnArrayCreate Cmd;
	Cmd.uObjID=GetEntityID();
	string strName(szArrayName);
	string strPos(szArrayPos);
	string strEntityID(szEntityID);
	Cmd.udbNameLen = strName.size();
	Cmd.udbPosLen = strPos.size();
	Cmd.udbEntityIDLen = strEntityID.size();
	CConnServer* pConn = GetConnection();
	if (pConn)
	{
		pConn->Send(&Cmd, sizeof(Cmd));
		pConn->Send(strName.c_str(), Cmd.udbNameLen);
		pConn->Send(strPos.c_str(), Cmd.udbPosLen);
		pConn->Send(strEntityID.c_str(), Cmd.udbEntityIDLen);
	}
}

bool CCharacterMediator::AddTempSkill(const TCHAR* szName, uint8 uSkillLevel)
{
	Ast(GetFighter());
	CFighterMediator* pFighter=GetFighter()->CastToFighterMediator();
	Ast(pFighter);
	return pFighter->AddTempSkill(szName,uSkillLevel);
}

void CCharacterMediator::RemoveTempSkill(const TCHAR* szName, uint8 uSkillLevel)
{
	Ast(GetFighter());
	CFighterMediator* pFighter=GetFighter()->CastToFighterMediator();
	Ast(pFighter);
	pFighter->RemoveTempSkill(szName,uSkillLevel);
}

void CCharacterMediator::RemoveAllTempSkill()
{
	Ast(GetFighter());
	CFighterMediator* pFighter=GetFighter()->CastToFighterMediator();
	Ast(pFighter);
	pFighter->RemoveAllTempSkill();
}

bool CCharacterMediator::IsAttacking()const
{
	Ast(GetFighter());
	return GetFighter()->IsAttacking();
}

uint32 CCharacterMediator::GetSkillCoolDownType(const TCHAR* SkillName)
{
	return GetFighter()->CastToFighterMediator()->GetSkillCoolDownType(SkillName);
}

// 暂时没用 OffBattleHorse
void CCharacterMediator::OffBattleHorse()
{
	CFighterDictator* pFighter = this->GetFighter();

	if (pFighter->GetCtrlState(eFCS_InNormalHorse))
	{
		pFighter->CppSetCtrlState(eFCS_InNormalHorse, false);
	}

	if (pFighter->GetCtrlState(eFCS_InBattleHorse))
	{
		pFighter->CppSetCtrlState(eFCS_InBattleHorse, false);

		CCharacterDictator* pCharacter = pFighter->GetCharacter();
		if (!pCharacter)
			return;
	
		CCharacterDictator* pServantHorse = pCharacter->GetServantByType(ENpcType_BattleHorse);
		if (pServantHorse)
			pCharacter->RemoveServant(pServantHorse);
	}
	pFighter->GetCharacter()->SetHorseName("");

	//同步客户端

	CFighterMediator* pFighterMediator = pFighter->CastToFighterMediator();
	if (pFighterMediator)
	{
		ICharacterMediatorCallbackHandler* pCallBackHandler = pFighterMediator->GetCallBackHandler();
		if (pCallBackHandler)
		{
			pCallBackHandler->OnRidingHorseChanged(pFighter->GetEntityID(), "", "");
		}
	}

}

// 暂时没用 CreateBattleHorse
void CCharacterMediator::CreateBattleHorse(uint32 uMasterID, const TCHAR* sBattleHorseName, const TCHAR* sNpcType, uint8 uLevel, CFPos* pPos)
{
	CFighterDictator* pFighter = this->GetFighter();

	string sHorseName = sBattleHorseName; 
	if (this->GetHorseName() != sHorseName && NULL != this->GetCallbackHandler())
	{
		this->GetCallbackHandler()->OnCreateBattleHorse(this->GetEntityID(), sBattleHorseName, sNpcType, uLevel, pPos->x, pPos->y);
		pFighter->CppSetCtrlState(eFCS_InBattleHorse, true);
	}
	SetHorseName(sHorseName);
	CFighterMediator* pFighterMediator = pFighter->CastToFighterMediator();
	if (pFighterMediator)
	{
		ICharacterMediatorCallbackHandler* pCallBackHandler = pFighterMediator->GetCallBackHandler();
		if (pCallBackHandler)
		{
			pCallBackHandler->OnRidingHorseChanged(this->GetEntityID(), sBattleHorseName, sNpcType);
		}
	}
}

void CCharacterMediator::CancelAutoTracking()
{
	CGas2GacCPP_OnCancelAutoTrack cmd;
	cmd.uObjID = GetEntityID();
	cmd.bCancelNAOnly = false;
	GetFighter()->SendCmdToGac(&cmd, eOnlyToDirector);
}

void CCharacterMediator::CancelCastingProcess()
{
	GetFighter()->CastToFighterMediator()->CancelCastingProcess();
}

bool CCharacterMediator::IsInPlayerSet(CCharacterMediator* pCharacterMediator)
{
	return m_setPlayer.find( pCharacterMediator) != m_setPlayer.end();
}

void CCharacterMediator::ClearQuestBeckonNpc()
{
	if (NULL != GetServantMgr())
	{
		uint32 uQuestBeckonNpc = GetServantMgr()->GetServantIDByType(ENpcType_QuestBeckonNpc);
		if (0 != uQuestBeckonNpc)
		{
			CCharacterDictator* pCharacter = CCharacterDictator::GetCharacterByID(uQuestBeckonNpc);
			if (NULL != pCharacter)
			{
				pCharacter->SetOnDead();
			}
		}
	}
}

void CCharacterMediator::OnEnterBattleState()
{
	GetCallbackHandler()->CastToMediatorHandler()->OnPlayerEnterBattleState(this);
}

void CCharacterMediator::OnLeaveBattleState()
{
	GetCallbackHandler()->CastToMediatorHandler()->OnPlayerLeaveBattleState(this);
}

void CCharacterMediator::InsertOtherPlayer(CCharacterMediator* pCharacter)
{
	if (pCharacter->GetGameEntityType()==eGET_CharacterMediator)
	{
		m_setPlayer.insert(pCharacter->CastToCharacterMediator());
	}
}

void CCharacterMediator::EraseOtherPlayer(CCharacterMediator* pCharacter)
{
	m_setPlayer.erase(pCharacter->CastToCharacterMediator());
}

void CCharacterMediator::OnStartDuel(CCharacterMediator* pCharacter)
{
	m_pDuelTarget = pCharacter;
}

void CCharacterMediator::OnFinishDuel(bool bResult)
{
	m_pDuelTarget = NULL;

	CAllStateMgrServer* pStateMgr = GetFighter()->GetAllStateMgr();
	Ast(pStateMgr);
	pStateMgr->EraseDecreaseState();
	GetFighter()->Notify(eCE_FinishDuel);

	CServantServerMgr* pServantMgr = GetServantMgr();
	if(pServantMgr)	
	{
		const CServantServerMgr::ServantIDMapType& ServantMap = pServantMgr->GetServantIDMapType();
		CServantServerMgr::ServantIDMapType::const_iterator it = ServantMap.begin();
		for(; it!=ServantMap.end(); ++it)
		{
			CCharacterDictator* pServant = (*it).second;
			{
				if (pServant)
				{
					CAllStateMgrServer* pStateMgr = pServant->GetFighter()->GetAllStateMgr();
					Ast(pStateMgr);
					pStateMgr->EraseDecreaseState();
					pServant->GetFighter()->Notify(eCE_FinishDuel);
				}
			}
		}
	}

	Ast (GetCallbackHandler());
	GetCallbackHandler()->CastToMediatorHandler()->OnFinishDuel(this, bResult);
}

CCharacterMediator* CCharacterMediator::GetDuelTarget() const
{
	return m_pDuelTarget;
}

void CCharacterMediator::ChangeMoveSpeedByGMCommand(float fSpeed)
{
	GetFighter()->GM_SetMoveSpeedPercent(fSpeed);
}

uint32 CCharacterMediator::GetBeAttackTimes()
{
	CFighterMediator* pFighter=GetFighter()->CastToFighterMediator();
	Ast(pFighter);
	return pFighter->GetBeAttackTimes();
}

bool CCharacterMediator::GetCloseDurabilityChange()
{
	CFighterMediator* pFighter=GetFighter()->CastToFighterMediator();
	Ast(pFighter);
	return pFighter->GetCloseDurabilityChange();
}

void CCharacterMediator::SetCloseDurabilityChange(bool bClose)
{
	CFighterMediator* pFighter=GetFighter()->CastToFighterMediator();
	Ast(pFighter);
	return pFighter->SetCloseDurabilityChange(bClose);
}

uint32 CCharacterMediator::GetBurstSoulTimes()
{
	CFighterMediator* pFighter=GetFighter()->CastToFighterMediator();
	Ast(pFighter);
	return pFighter->GetBurstSoulTimes();
}

void CCharacterMediator::SetBurstSoulTimes(uint32 uBurstSoulTimes)
{
	CFighterMediator* pFighter=GetFighter()->CastToFighterMediator();
	Ast(pFighter);
	pFighter->SetBurstSoulTimes(uBurstSoulTimes);
}

uint32 CCharacterMediator::GetContinuousBurstTimes()
{
	CFighterMediator* pFighter=GetFighter()->CastToFighterMediator();
	Ast(pFighter);
	return pFighter->GetContinuousBurstTimes();
}

void CCharacterMediator::SetArmorValueRate(float fBluntRate,float fPunctureRate,float fChopRate)
{
	CFighterMediator* pFighter=GetFighter()->CastToFighterMediator();
	Ast(pFighter);
	pFighter->SetArmorValueRate(fBluntRate,fPunctureRate,fChopRate);
}

uint32 CCharacterMediator::CreateTransactionID()
{
	CFighterMediator* pFighter=GetFighter()->CastToFighterMediator();
	Ast(pFighter);
	return pFighter->CreateTransactionID();
}
void CCharacterMediator::DestroyTransaction(uint32 uTransactionID)
{	
	CFighterMediator* pFighter=GetFighter()->CastToFighterMediator();
	Ast(pFighter);
	pFighter->DestroyTransaction(uTransactionID);
}
void CCharacterMediator::EnableTransaction(uint32 uTransactionID)
{
	CFighterMediator* pFighter=GetFighter()->CastToFighterMediator();
	Ast(pFighter);
	pFighter->EnableTransaction(uTransactionID);
}
void CCharacterMediator::DisableTransaction(uint32 uTransactionID)
{
	CFighterMediator* pFighter=GetFighter()->CastToFighterMediator();
	Ast(pFighter);
	pFighter->DisableTransaction(uTransactionID);
}
void CCharacterMediator::CommitTransaction(uint32 uTransactionID)
{
	CFighterMediator* pFighter=GetFighter()->CastToFighterMediator();
	Ast(pFighter);
	pFighter->CommitTransaction(uTransactionID);
}
void CCharacterMediator::ChangeTransactionPropertyValue(uint32 uTransactionID, const TCHAR* szPropertyName, float fValue)
{
	CFighterMediator* pFighter=GetFighter()->CastToFighterMediator();
	Ast(pFighter);
	pFighter->ChangeTransactionPropertyValue(uTransactionID,szPropertyName,fValue);
}

float CCharacterMediator::GetEquipPropertyValue(const TCHAR* szPropertyName)
{
	CFighterMediator* pFighter=GetFighter()->CastToFighterMediator();
	Ast(pFighter);
	return pFighter->GetPropertyValue(szPropertyName);
}


void CCharacterMediator::DoAlertPhaseEffect(CCharacterDictator* pAlertNpc, uint32 uLasTime)
{
	Ast (GetCallbackHandler());
	GetCallbackHandler()->CastToMediatorHandler()->DoAlertPhaseEffect(this, pAlertNpc, uLasTime);
}

void CCharacterMediator::DoAlertEndEffect(CCharacterDictator* pAlertNpc)
{
	Ast (GetCallbackHandler());
	GetCallbackHandler()->CastToMediatorHandler()->DoAlertEndEffect(this, pAlertNpc);
}

void CCharacterMediator::OnMasterDisbandTruck(CCharacterDictator* pTruck)
{
	Ast (GetCallbackHandler());
	GetCallbackHandler()->CastToMediatorHandler()->OnMasterDisbandTruck(this, pTruck);
}

void CCharacterMediator::OnMasterDestroyTruck(CCharacterDictator* pTruck)
{
	Ast (GetCallbackHandler());
	GetCallbackHandler()->CastToMediatorHandler()->OnMasterDestoryTruck(this, pTruck);
}

void CCharacterMediator::SetStealth(float fStealth)
{
	if (abs(fStealth) < 0.1)
		GetStealthVariant()->SetNumber(false);
	else 
		GetStealthVariant()->SetNumber(true);
	CEntityServer::SetStealth(fStealth);
}

bool CCharacterMediator::ChangeScene(const TCHAR* szSceneName,uint32 uNewSceneID, CCoreSceneServer *pScene,const CFPos& PixelPos)
{
	SetAndSyncMoveState(eMS_Stop, true);
	SetAndSyncActionState(eAS_Idle_BackWpn);

	bool bSuccess = GetScene()->TransferObject(GetCoreObj(),pScene, PixelPos);
	if (bSuccess)
	{
		GetFighter()->Notify(eCE_ChangeMapBegin);
		bool bClearServant = GetCallbackHandler()->CastToMediatorHandler()->BeClearServantByTransfer(szSceneName);
		if (GetServantMgr())
			GetServantMgr()->SaveAndClearAllServant(!bClearServant);

		CGas2GacCPP_BeginTransfer Cmd;
		Cmd.uEntityID	= GetEntityID();
		Cmd.bTransfer = true;
		Cmd.udbSceneNameLen = strlen(szSceneName);
		GetConnection()->Send(&Cmd,sizeof(Cmd));
		GetConnection()->Send(szSceneName, Cmd.udbSceneNameLen);
	}
	DisbindAllChildren();
	return bSuccess;
}

void CCharacterMediator::SaveServantInfo()
{
	Ast (GetCallbackHandler());
	GetCallbackHandler()->CastToMediatorHandler()->SaveServantInfo(this);
}

void CCharacterMediator::SaveServantInfoOnServantChange()
{
	Ast (GetCallbackHandler());
	GetCallbackHandler()->CastToMediatorHandler()->SaveServantInfoOnServantChange(this);
}

void CCharacterMediator::BeginStatistic(const TCHAR* sName)
{
	Ast(GetFighter());
	GetFighter()->CastToFighterMediator()->BeginStatistic(sName);
}
void CCharacterMediator::EndStatistic(const TCHAR* sName)
{
	Ast(GetFighter());
	GetFighter()->CastToFighterMediator()->EndStatistic(sName);
}

uint32 CCharacterMediator::GetStatisticFightHurt(const TCHAR* sName)
{
	Ast(GetFighter());
	return GetFighter()->CastToFighterMediator()->GetStatisticFightHurt(sName);
}

uint32 CCharacterMediator::GetStatisticFightHeal(const TCHAR* sName)
{
	Ast(GetFighter());
	return GetFighter()->CastToFighterMediator()->GetStatisticFightHeal(sName);
}

uint32 CCharacterMediator::GetStatisticBeSubHurt(const TCHAR* sName)
{
	Ast(GetFighter());
	return GetFighter()->CastToFighterMediator()->GetStatisticBeSubHurt(sName);
}

float CCharacterMediator::GetStatisticDps(const TCHAR* sName)
{
	Ast(GetFighter());
	return GetFighter()->CastToFighterMediator()->GetStatisticDps(sName);
}

float CCharacterMediator::GetStatisticHps(const TCHAR* sName)
{
	Ast(GetFighter());
	return GetFighter()->CastToFighterMediator()->GetStatisticHps(sName);
}

uint32 CCharacterMediator::GetStatisticDeadTimes(const TCHAR* sName)
{
	Ast(GetFighter());
	return GetFighter()->CastToFighterMediator()->GetStatisticDeadTimes(sName);
}

void CCharacterMediator::ClearAllStatisticData()
{
	Ast(GetFighter());
	return GetFighter()->CastToFighterMediator()->ClearAllStatisticData();
}

void CCharacterMediator::TriggerSetupWeapon()
{
	Ast(GetFighter());
	if(IsSetupWeapon())
		CTriggerEvent::MessageEvent(eTST_Trigger, GetFighter(), NULL, eSET_SetupWeapon);
}

void CCharacterMediator::TriggerChangeEquip()
{
	Ast(GetFighter());
	CTriggerEvent::MessageEvent(eTST_Trigger, GetFighter(), NULL, eSET_ChangeEquip);
}

float CCharacterMediator::CppGetPropertyValueByName(const TCHAR* szName)
{
	return GetFighter()->CppGetPropertyValue(szName);
}

void CCharacterMediator::CppSuicide()
{
	Ast(GetFighter());
	return GetFighter()->CastToFighterMediator()->CppDie(NULL);
}

uint32 CCharacterMediator::GetActivityValue()
{
	Ast(GetFighter());
	return GetFighter()->CastToFighterMediator()->GetActivityValue();
}

void CCharacterMediator::ClearActivityValue()
{
	Ast(GetFighter());
	return GetFighter()->CastToFighterMediator()->ClearActivityValue();
}

