#include "stdafx.h"
#include "CFighterMediator.h"
#include "IFighterServerHandler.h"
#include "TPropertyHolder.inl"
#include "CAllStateMgr.h"
#include "CPlayerSkillMgrServer.h"
#include "CTalentEffServer.h"
#include "PtrlGac2GasCPPDef.h"
#include "PtrlGas2GacCPPDef.h"
#include "CCharacterDictator.h"
#include "ICharacterMediatorCallbackHandler.h"
#include "NormalAttack.h"
#include "CNormalAttackCfg.h"
#include "CPropertyCfg.h"
#include "CEntityServerManager.h"
#include "CBattleArrayMagicServer.h"
#include "CBattleStateManager.h"
#include "CMagicMgrServer.h"
#include "ErrLogHelper.h"
#include "CSkillMgrServer.h"
#include "CSkillServer.h"
#include "CCoolDownMgrServer.h"
#include "CoreCommon.h"
#include "CStateSync.h"
#include "CBurstSoulMgrServer.h"
#include "CPropertyTransaction.h"
#include "MagicOps_ChangeValue.h"
#include "CSkillReplaceServer.h"
#include "CFightStatisticMgr.h"
#include "CActivityValueManager.h"

CFighterMediator::CFighterMediator()
:m_pEquipPropertyMgr(NULL)
,m_pBurstSoulMgrServer(NULL)
,m_pSkillReplaceServer(NULL)
,m_pFightStatisticMgr(NULL)
,m_pActivityValueMgr(NULL)
,m_BeAttackTimes(0)
,m_bCloseDurabilityChange(false)
,m_bSpeedIsSuspicious(false)
{
}

CFighterMediator::~CFighterMediator()
{
	delete GetTalentHolder()->GetHandler();
	delete m_pEquipPropertyMgr;
	SafeDelete(m_pBurstSoulMgrServer);
	SafeDelete(m_pSkillReplaceServer);
	SafeDelete(m_pFightStatisticMgr);
	SafeDelete(m_pActivityValueMgr);
	GetTalentHolder()->SetHandler(NULL);
}

void CFighterMediator::Init()
{
	m_pSkilllMgr = new CPlayerSkillMgrServer(this);
	m_pEquipPropertyMgr = new CPropertyTransactionMgr(this);
	m_pBurstSoulMgrServer	= new CBurstSoulMgrServer(this);
	m_pSkillReplaceServer	= new CSkillReplaceServer(this);
	m_pFightStatisticMgr		= new CFightStatisticMgr();
	m_pActivityValueMgr = new CActivityValueManager();
	GetTalentHolder()->SetHandler(new CTalentEffServer(this));
}


void CFighterMediator::CppInit(EClass eClass, uint32 uLevel, ECamp eCamp)
{
	CppSetClass(eClass);
	OnClassChanged(eClass);
	CppSetLevel(uLevel);
	OnLevelChanged(uLevel);
	CppSetCamp(eCamp);
	OnCampChanged(eCamp);
	class_cast<CTalentEffServer*>(GetTalentHolder()->GetHandler())->InitClassTalentEff(eClass);
	InitAgileValue();
	//初始化天赋效果 一定要在设置完eClass 之后 在初始化易变值之前完成
	SetMainHandSpeed(this,2.0);
	CppInitNormalAttack((CPropertyCfg::GetClassNameString(CppGetClass())+"普通攻击").c_str(),eATT_None,eWT_None,NULL,eATT_None,eWT_None);
}

void CFighterMediator::ReSendToSelf()
{
	//OnClassChanged(CppGetClass());
	//OnLevelChanged(CppGetLevel());
	//OnCampChanged(CppGetCamp());

	if( !GetHandler() )
		return;

	CGas2GacCPP_OnClassChanged	Cmd1;
	Cmd1.uObjID = GetEntityID();
	Cmd1.uClass = uint8(CppGetClass());
	SendCmdToGac(&Cmd1,eOnlyToDirector);	

	CGas2GacCPP_OnLevelChanged	Cmd2;
	Cmd2.uObjID=GetEntityID();
	Cmd2.uLevel=CppGetLevel();
	SendCmdToGac(&Cmd2,eOnlyToDirector);	

	CGas2GacCPP_OnCampChanged	Cmd3;
	Cmd3.uObjID	= GetEntityID();
	Cmd3.uCamp	= uint8(CppGetCamp());
	SendCmdToGac(&Cmd3,eOnlyToDirector);	
}

void CFighterMediator::OnLevelUp()
{
	CppSetLevel(CppGetLevel()+1);
	OnLevelChanged(CppGetLevel());
	m_HealthPoint.FullFill(this);
	if (CppGetClass() != eCL_Warrior && CppGetClass() != eCL_OrcWarrior && CppGetClass() != eCL_DwarfPaladin)
	{
		m_ManaPoint.FullFill(this);
	}
}

void CFighterMediator::CppInitHPMP(uint32 uCurHP, uint32 uCurMp)
{
	m_HealthPoint.LimitSet(uCurHP, this);
	if (CppGetClass() == eCL_Warrior || CppGetClass() == eCL_OrcWarrior)
	{
		m_RagePoint.LimitSet(0, this);
	}
	else if (CppGetClass() == eCL_DwarfPaladin)
	{
		m_EnergyPoint.LimitSet(100, this);
	}
	else
	{
		m_ManaPoint.LimitSet(uCurMp, this);
	}
}

void CFighterMediator::CppFullRage(bool bFull)
{
	if (bFull)
	{
		SetRPConsumeRate(this, 0.0f);
		m_RagePoint.LimitSet(100, this);
		m_RagePoint.StopReduceRP();
	}
	else
	{
		SetRPConsumeRate(this, 1.0f);
		m_RagePoint.LimitSet(0, this);
		m_RagePoint.ResetReduceRP(this);
	}
}

void CFighterMediator::ReSendHPMPToSelf()
{
	//ReSendAgileValueToSelf(ePID_HealthPoint * ePFT_Count + ePFT_Agile, float(m_HealthPoint.LimitGet()));
	//ReSendAgileValueToSelf(ePID_ManaPoint * ePFT_Count + ePFT_Agile, float(m_ManaPoint.LimitGet()));
	//ReSendAgileValueToSelf(ePID_RagePoint * ePFT_Count + ePFT_Agile, float(m_RagePoint.LimitGet()));
	//ReSendAgileValueToSelf(ePID_EnergyPoint * ePFT_Count + ePFT_Agile, float(m_EnergyPoint.LimitGet()));
}

ICharacterMediatorCallbackHandler*	CFighterMediator::GetCallBackHandler()const	
{
	CCharacterDictator* pCharacter = GetCharacter();
	return pCharacter ? pCharacter->GetCallbackHandler()->CastToMediatorHandler():NULL;
}

void CFighterMediator::CppDie(CFighterDictator* pAttacker)
{
	if(!CppIsAlive())
		return;
	GetBattleStateMgr()->LeaveBattleStateByForce();

	CFighterDictator::CppDie(pAttacker,NULL);
	CEntityServer* pEntity = GetHandler()->GetEntity();
	if (!CppIsAlive())
		pEntity->DisableDirectorMoving(true);

	m_pFightStatisticMgr->OnDead();
	ICharacterMediatorCallbackHandler* pCallbackHandler = GetCallBackHandler();
	if(pCallbackHandler)
		pCallbackHandler->OnDead(pAttacker?pAttacker->GetCharacter():NULL, GetCharacter());
	pEntity->SetSizeType(eAIST_DeadPlayer);	
}

void CFighterMediator::CppReborn()
{
	CFighterDictator::CppReborn();
	CEntityServer* pEntity = GetHandler()->GetEntity();
	pEntity->DisableDirectorMoving(false);
	pEntity->SetSizeType(eAIST_PlayerOrServant);	
}

void CFighterMediator::OnCaughtViewSightOf(CFighterDictator* pFighter, uint32 uDimension, bool bMultiCast)
{
	if( !GetHandler() )
		return;

	uint32 uFighterID = pFighter->GetEntityID();

	//cout << GetEntityID() << " CFighterMediator::OnCaughtViewSightOf " << uFighterID << endl; 
	CGas2GacCPP_OnLevelChanged	CmdOnLevelChanged;
	CmdOnLevelChanged.uObjID=uFighterID;
	CmdOnLevelChanged.uLevel=pFighter->CppGetLevel();
	CGas2GacCPP_OnClassChanged	CmdOnClassChanged;
	CmdOnClassChanged.uObjID=uFighterID;
	CmdOnClassChanged.uClass=uint8(pFighter->CppGetClass());
	CGas2GacCPP_OnCampChanged	CmdOnCampChanged;
	CmdOnCampChanged.uObjID=uFighterID;
	CmdOnCampChanged.uCamp=uint8(pFighter->CppGetCamp());

	pFighter->OnPKStateChanged(pFighter->CppGetPKState());
	//pFighter->OnZoneTypeChanged(pFighter->CppGetZoneType());

	if(bMultiCast)
	{
		pFighter->SendCmdToGac(&CmdOnLevelChanged,eViewSight);
		pFighter->SendCmdToGac(&CmdOnClassChanged,eViewSight);	
		pFighter->SendCmdToGac(&CmdOnCampChanged,eViewSight);	
	}
	else
	{
		SendCmdToGac(&CmdOnLevelChanged,eOnlyToDirector);
		SendCmdToGac(&CmdOnClassChanged,eOnlyToDirector);	
		SendCmdToGac(&CmdOnCampChanged,eOnlyToDirector);	
	}

	////同步CtrlValue
	//CGas2GacCPP_OnCtrlValueChanged cmdCtrlValue;
	//cmdCtrlValue.uObjID			= uFighterID;
	//cmdCtrlValue.uValue			= pFighter->GetAllCtrlState();
	//cmdCtrlValue.uValueTypeID	= 0;
	//cmdCtrlValue.bSet			= false;
	//cmdCtrlValue.bIsAlive		= pFighter->CppIsAlive();

	//if(bMultiCast)
	//	pFighter->SendCmdToGac(&cmdCtrlValue, eViewSight);
	//else
	//	SendCmdToGac(&cmdCtrlValue, eOnlyToDirector);
}

void CFighterMediator::OnLostViewSightOf(CFighterDictator* pFighter, uint32 uDimension, bool bMultiCast)
{
}

void CFighterMediator::OnCaughtSyncSightOf(CFighterDictator* pFighter, uint32 uDimension, bool bMultiCast)
{
	if (!pFighter) return;
#ifndef SEND_STATE_BY_VAR
	CFighterMediator* pObserver = bMultiCast ? NULL : this;
	//pFighter->GetSkillMgr()->SyncCastingProcess(pObserver);
	pFighter->GetAllStateMgr()->SyncAllState(pObserver);
#endif
}

void CFighterMediator::OnLostSyncSightOf(CFighterDictator* pFighter, uint32 uDimension, bool bMultiCast)
{
	if (!pFighter) return;
#ifndef SEND_STATE_BY_VAR
	CFighterMediator* pObserver = bMultiCast ? NULL : this;
	pFighter->GetAllStateMgr()->SyncClearState(pObserver);
#endif
}

void CFighterMediator::ReSendSkillCoolDownTimeToSelf()
{
	for (MapActiveSkill::iterator it=m_mapActiveSkill.begin(); it != m_mapActiveSkill.end(); ++it)
	{
		const CNormalSkillServerSharedPtr& pSkill = CNormalSkillServer::GetSkillByName(it->first);	
		if (!pSkill)
		{
			continue;
		}
		ESkillCoolDownType eCoolDownType = pSkill->GetCoolDownType();
		uint32 uLeftTime = m_pCoolDownMgr->GetCoolDownTime(it->first.c_str(), eCoolDownType);
		if (uLeftTime >= 3000)
		{
			this->GetSkillMgr()->OnCoolDownBegin(it->first.c_str(), it->second, uLeftTime);
		}
	}
}

void CFighterMediator::CancelCastingProcess()
{
	GetSkillMgr()->CancelCastingProcess();
}

void CFighterMediator::OnBeAttacked()
{
	if (m_bCloseDurabilityChange)
	{
		return;//关闭了耐久计算
	}
	m_BeAttackTimes = m_BeAttackTimes + 1;
	if (m_BeAttackTimes > 100)
	{
		ICharacterMediatorCallbackHandler* pCallbackHandler = GetCallBackHandler();
		if(pCallbackHandler)
		{
			pCallbackHandler->OnAttackTimesChanged(GetEntityID(), m_BeAttackTimes);
			m_BeAttackTimes = 0;
		}
	}
}

uint32 CFighterMediator::GetBeAttackTimes()
{
	return m_BeAttackTimes;
}

void CFighterMediator::SetCloseDurabilityChange(bool bClose)
{
	m_bCloseDurabilityChange = bClose;
}

bool CFighterMediator::GetCloseDurabilityChange()
{
	return m_bCloseDurabilityChange;
}

bool CFighterMediator::SendToGac(const void* pData,uint32 uSize,uint32 uRange)const
{
	CConnServer* pConn= GetConnection();
	if( pConn && (!DirectorMovingIsDisabled() || uRange == eOnlyToDirector) )
		pConn->Send(pData,uSize);
	if(uRange != eOnlyToDirector)
		GetIS(uRange)->Send(pData,uSize);
	return true;
}

void CFighterMediator::SetFighterPos(const CFPos& pos)
{
	m_FighterPos = pos;
}

const CFPos& CFighterMediator::GetFighterPos()const
{
	return m_FighterPos;
}

// 接受 客户端"施放技能(DoSkill)" 并响应
void CFighterMediator::OnClientCommand(const CGac2GasCPP_DoSkill* pCmd)
{
	// 服务端 施放技能
	CFPos mPos;
	GetPixelPos(mPos);
	if (abs(pCmd->fPosFrom.x - mPos.x)<eGridSpanForObj && abs(pCmd->fPosFrom.y - mPos.y)<eGridSpanForObj)
	{
		m_FighterPos = pCmd->fPosFrom;
	}
	else
	{
		GetPixelPos(m_FighterPos);	
	}
	const CNormalSkillServerSharedPtr& pSkill = CNormalSkillServer::GetSkillByID(pCmd->uqbSkillID);	
	//cout << "skill " << pSkill->GetName() << " " << pCmd->uqbSkillID << endl;
	switch (pSkill->GetCoolDownType())
	{
	case eSCDT_FightSkill:
	case eSCDT_NoComCDFightSkill:
	case eSCDT_NonFightSkill:
	case eSCDT_TempSkill:
	case eSCDT_EquipSkill:
	case eSCDT_UnrestrainedFightSkill:
	//case eSCDT_OtherSkill:// 客户端发上来的 "其他技能"类技能,目前没办法检测是否有权限释放
		{
			static_cast<CPlayerSkillMgrServer*>(GetSkillMgr())->UseFightSkill(pSkill,pCmd->uSkillLevel,pCmd->uPos, pCmd->uDir, pCmd->uMagicDir);
		}
		break;
	default:
		{
			ostringstream strm;
			strm<< "技能ID " << pCmd->uqbSkillID <<endl;
			GenErr("客户端请求的DoSkill不允许这个类型的技能", strm.str());
		}
	}

}

void CFighterMediator::OnClientCommand(const CGac2GasCPP_StopNormalAttack* pCmd)
{
	CancelAttack();
}

void CFighterMediator::OnClientCommand(const CGac2GasCPP_DoNormalAttack* pCmd)
{
	CCharacterDictator* pCharacterDictator= CEntityServerManager::GetInst()->GetCharacter(pCmd->uTargetID);
	if(pCharacterDictator == NULL) return;
	CFighterDictator* pTarget = pCharacterDictator->GetFighter();
	if(pTarget != GetLockingTarget())
	{
		//cout << "切换普攻目标" << endl;
	}

	if (pTarget == NULL)
	{
		return;
	}

	//cout << "gac2gas donormalattack " << pCmd->bToBeStarted << endl;
	if(pCmd->bToBeStarted)
	{
		DoNormalAttack(pTarget);
	}
	else
	{
		CancelAttack(true);
	}
	
}

void CFighterMediator::OnClientCommand(const CGac2GasCPP_StartOrRestartNADueToSkillFailure* pCmd)
{
	//cout << "skill failure and startorrestartna " << strSkillName << endl;
	const CNormalSkillServerSharedPtr& pSkill = CNormalSkillServer::GetSkillByID(pCmd->uqbSkillID);	
	if(pSkill && m_pSkilllMgr->GetCurCasting() == NULL)
	{
		pSkill->StartOrRestartNADueToSkillFailure(this);
	}
}

void CFighterMediator::OnClientCommand(const CGac2GasCPP_CancelState* pCmd)
{
	GetAllStateMgr()->CancelStateByDynamicId(pCmd->uID);
}

void CFighterMediator::OnClientCommand(const CGac2GasCPP_CancelCoerceMove* pCmd)
{
	CCharacterDictator* pCharacterDictator= CEntityServerManager::GetInst()->GetCharacter(pCmd->uID);
	if(pCharacterDictator == NULL)
		return;
	CFighterDictator* pFighter = pCharacterDictator->GetFighter();

	pFighter->Notify(eCE_CancelCoerceMove);
}

void CFighterMediator::OnClientCommand(const CGac2GasCPP_SetTargetPos* pCmd)
{
	SetTargetPos(pCmd->Pos);
}

void CFighterMediator::OnClientCommand(const CGac2GasCPP_OpenObjPanel* pCmd)
{
	CCharacterDictator* pCharacterDictator= CEntityServerManager::GetInst()->GetCharacter(pCmd->uObjID);
	if(pCharacterDictator == NULL)
		return ;
	CFighterDictator* pFighter = pCharacterDictator->GetFighter();
	pFighter->OpenObjPanelByID(GetEntityID(), pCmd->IsOpenpanel, pCmd->bIsTargetPanel);
}

void CFighterMediator::OnClientCommand(const CGac2GasCPP_DestroyServant* pCmd)
{
	CCharacterDictator* pCharacterDictator= CEntityServerManager::GetInst()->GetCharacter(pCmd->uObjID);
	if(pCharacterDictator == NULL)
		return ;

	CCharacterDictator* pServant_Pet = pCharacterDictator->GetServantByType(ENpcType_Summon);
	if (pServant_Pet)
		pCharacterDictator->RemoveServant(pServant_Pet);
	CCharacterDictator* pServant_BattleHorse = pCharacterDictator->GetServantByType(ENpcType_BattleHorse); 
	if (pServant_BattleHorse)
	{
		pCharacterDictator->RemoveServant(pServant_BattleHorse);
		pCharacterDictator->CppSetCtrlState(eFCS_InBattleHorse, false);
	}
}

void CFighterMediator::OnClientCommand(const CGac2GasCPP_SetAutoNormalAttack* pCmd)
{
	//CCharacterDictator* pCharacterDictator= CEntityServerManager::GetInst()->GetCharacter(pCmd->uObjID);
	//if(pCharacterDictator == NULL)
	//	return ;
	//CFighterDictator* pFighter = pCharacterDictator->GetFighter();
	//CNormalAttackMgr* pAttackMgr= pFighter->GetNormalAttackMgr();
	//pAttackMgr->SetAutoNormalAttack(pCmd->bIsAuto);
}

void CFighterMediator::OnClientCommand(const CGac2GasCPP_CancelCastingProcess* pCmd)
{
	CancelCastingProcess();
}

void CFighterMediator::OnClientCommand(const CGac2GasCPP_GetTargetCastingProcess* pCmd)
{
	CCharacterDictator* pCharacterDictator= CEntityServerManager::GetInst()->GetCharacter(pCmd->uTargetID);
	if(pCharacterDictator == NULL)
		return ;
	CFighterDictator* pFighter = pCharacterDictator->GetFighter();
	pFighter->GetSkillMgr()->SyncCastingProcess(this);
}

void CFighterMediator::OnClientCommand(const CGac2GasCPP_GetTargetOfTarget* pCmd)
{
	CCharacterDictator* pCharacterDictator= CEntityServerManager::GetInst()->GetCharacter(pCmd->uTargetID);
	if(pCharacterDictator == NULL)
		return ;
	CFighterDictator* pFighter = pCharacterDictator->GetFighter();
	if(pFighter&&pFighter->GetTarget())
	{
		pFighter->SyncTarget(pFighter->GetTarget()->GetEntityID());
	}
}

void CFighterMediator::OnClientCommand(const CGac2GasCPP_NotifyBurstSoulArrived* pCmd)
{
	GetBurstSoulMgrServer()->NotifyBurstSoulArrived((EBurstSoulType)pCmd->uBurstSoulType);
}

void CFighterMediator::ReturnArray()
{
	CMagicMgrServer* pMagicMgr = GetMagicMgr();
	if (pMagicMgr->GetArrayStation())
	{
		pMagicMgr->GetArrayStation()->ChangeArrayState(eBAS_ReturnArray);
	}	
}

bool CFighterMediator::AddTempSkill(const string& strName, uint8 uSkillLevel)
{
	return static_cast<CPlayerSkillMgrServer*>(m_pSkilllMgr)->AddTempSkill(strName,uSkillLevel);
}

void CFighterMediator::RemoveTempSkill(const string& strName, uint8 uSkillLevel)
{
	static_cast<CPlayerSkillMgrServer*>(m_pSkilllMgr)->RemoveTempSkill(strName,uSkillLevel);
}

void CFighterMediator::RemoveAllTempSkill()
{
	static_cast<CPlayerSkillMgrServer*>(m_pSkilllMgr)->RemoveAllTempSkill();
}

void CFighterMediator::TellDirectorToDoNA(const CFighterDictator* pTarget, bool bAutoTrack)
{
	CGas2GacCPP_OnDoNormalAttack cmd;
	cmd.uObjID = GetEntityID();
	cmd.uTargetID = pTarget->GetEntityID();
	cmd.bAutoTrack = bAutoTrack && pTarget->CppIsAlive();
	SendCmdToGac(&cmd, eOnlyToDirector);
	//cout << "TellDirToDoNa " << cmd.uTargetID << endl;
}

void CFighterMediator::CancelAttack(bool bCalDamageImmediately)
{
	if(NULL==GetNormalAttackMgr())
	{
		return;
	}
	//if (!IsAttacking())
	//{
	//	return;
	//}

	GetNormalAttackMgr()->Cancel(bCalDamageImmediately, IsAttacking());
	SetLockingTarget(NULL);

	CancelNormalAttackAutoTracking();

	CGas2GacCPP_OnCancelNormalAttack cmd;
	cmd.uObjID = GetEntityID();
	SendCmdToGac(&cmd, eOnlyToDirector);
	GetCharacter()->CppSetCtrlState(eFCS_InNormalAttacking, false);
}

void CFighterMediator::CancelNormalAttackAutoTracking()
{
	CGas2GacCPP_OnCancelAutoTrack cmd;
	cmd.uObjID = GetEntityID();
	cmd.bCancelNAOnly = true;
	SendCmdToGac(&cmd, eOnlyToDirector);
}

EDoSkillResult CFighterMediator::UseFightSkill(const string& szName, uint8 uSkillLevel, uint8 uPos, uint8 uDir, uint8 uMagicDir)
{
	CPlayerSkillMgrServer* pSkillMgr = static_cast<CPlayerSkillMgrServer*>(GetSkillMgr());
	Ast(pSkillMgr);
	const CNormalSkillServerSharedPtr& pSkill = CNormalSkillServer::GetSkillByName(szName);	
	return pSkillMgr->UseFightSkill(pSkill, uSkillLevel, uPos, uDir, uMagicDir);
}

EDoSkillResult CFighterMediator::DoSkill(const string& szName, uint8 uSkillLevel, uint8 uDir, uint8 uMagicDir)
{
	CPlayerSkillMgrServer* pSkillMgr = static_cast<CPlayerSkillMgrServer*>(GetSkillMgr());
	Ast(pSkillMgr);
	return pSkillMgr->DoPlayerSkill(szName, uSkillLevel, uDir, uMagicDir);
}

EDoSkillResult CFighterMediator::DoPassiveSkill(const string& szName, uint8 uSkillLevel, uint8 uDir)
{
	CPlayerSkillMgrServer* pSkillMgr = static_cast<CPlayerSkillMgrServer*>(GetSkillMgr());
	Ast(pSkillMgr);
	const CNormalSkillServerSharedPtr& pSkill = CNormalSkillServer::GetSkillByName(szName);	
	return pSkillMgr->DoPlayerPassiveSkill(pSkill, uSkillLevel, uDir);
}

EDoSkillResult CFighterMediator::PlayerDoPassiveSkill(const TCHAR* sName, uint32 uSkillLevel)
{
	CPlayerSkillMgrServer* pSkillMgr = static_cast<CPlayerSkillMgrServer*>(GetSkillMgr());
	Ast(pSkillMgr);
	return pSkillMgr->PlayerDoPassiveFightSkill(sName, uSkillLevel);
}

void CFighterMediator::SendFailureMsgToGac(uint32 uMsg, const TCHAR* szName)
{
	CGas2GacCPP_OnMagicCondFail cmd;
	cmd.uObjID = GetEntityID();
	cmd.uMessageID = uMsg;
	string strSkillName(szName);
	cmd.udbSkillNameLen = strSkillName.size();
	SendCmdToGac(&cmd, eOnlyToDirector);
	SendToGac(strSkillName.c_str(), cmd.udbSkillNameLen, eOnlyToDirector);
}

void CFighterMediator::OnMoveSpeedChange(float fSpeed, const char* szSpeedChangeReason, const char* szSkillName)const
{
	//cout << "主角设置最大移动速度 " << fSpeed << endl;
	CCharacterDictator* pChar = GetCharacter();
	if(fSpeed <= 0)
	{
		GenErr("主角设置最大移动速度不能小于0");
	}
	pChar->SetDirectorMaxSpeed(fSpeed);
	CFighterDictator::OnMoveSpeedChange(fSpeed, szSpeedChangeReason, szSkillName);

	const float fThreshold = (pChar->CppGetCtrlState(eFCS_InNormalHorse) || pChar->CppGetCtrlState(eFCS_InBattleHorse)) ? 8.0f : 4.0f;
	if((!m_bSpeedIsSuspicious && fSpeed > fThreshold) || (m_bSpeedIsSuspicious && fSpeed <= fThreshold))
	{
		m_bSpeedIsSuspicious = m_bSpeedIsSuspicious ? false : true;
		ICharacterMediatorCallbackHandler* pCallback = GetCallBackHandler();
		if(pCallback)
		{
			if(szSkillName)
			{
				ostringstream strm;
				strm << szSpeedChangeReason << "\t" << szSkillName;
				pCallback->OnLogSpeedChangeReason(pChar->CastToCharacterMediator(), fSpeed, strm.str().c_str());
			}
			else
			{
				pCallback->OnLogSpeedChangeReason(pChar->CastToCharacterMediator(), fSpeed, szSpeedChangeReason);
			}
		}
	}
}

void CFighterMediator::PauseAttack(bool bLockRestart, bool bCalDamageImmediately)
{
	//cout << GetEntityID() << " CFighterMediator::PauseAttack " << endl;
	if(NULL==GetNormalAttackMgr())
	{
		return;
	}
	if(bLockRestart)
		m_pNormalAttackMgr->SetCanBeRestarted(false);
	m_pNormalAttackMgr->Pause(bCalDamageImmediately);
}
bool CFighterMediator::RestartAttack(bool bUnlockRestart)
{
	CNormalAttackMgr* pNormalAttackMgr = GetNormalAttackMgr();
	if(!pNormalAttackMgr)
		return false;

	if(bUnlockRestart)
		m_pNormalAttackMgr->SetCanBeRestarted(true);

	if(pNormalAttackMgr->IsTargetInAttackScope() && !pNormalAttackMgr->IsTargetUnreachableInLine(GetLockingTarget()))
	{
		//cout << GetEntityID() << " CFighterMediator::RestartAttack " << endl;
		return m_pNormalAttackMgr->Restart();
	}
	return false;
}

uint32 CFighterMediator::GetSkillCoolDownType(const TCHAR* SkillName)
{
	const CNormalSkillServerSharedPtr& pSkill = CNormalSkillServer::GetSkillByName(SkillName);	
	if (!pSkill)
	{
		GenErr("CFighterMediator::GetSkillCoolDownType:技能不存在");
	}
	return pSkill->GetCoolDownType();
}

void CFighterMediator::SetArmorValueRate(float fBluntRate,float fPunctureRate,float fChopRate)
{
	SetArmorValueRateMultiplier(this,fBluntRate,fPunctureRate,fChopRate);
}


uint32 CFighterMediator::CreateTransactionID()
{
	return m_pEquipPropertyMgr->CreateTransactionID();
}
void CFighterMediator::DestroyTransaction(uint32 uTransactionID)
{	
	m_pEquipPropertyMgr->DestroyTransaction(uTransactionID);
}
void CFighterMediator::EnableTransaction(uint32 uTransactionID)
{
	m_pEquipPropertyMgr->EnableTransaction(uTransactionID);
}
void CFighterMediator::DisableTransaction(uint32 uTransactionID)
{
	m_pEquipPropertyMgr->DisableTransaction(uTransactionID);
}
void CFighterMediator::CommitTransaction(uint32 uTransactionID)
{
	m_pEquipPropertyMgr->CommitTransaction(uTransactionID);
}
void CFighterMediator::ChangeTransactionPropertyValue(uint32 uTransactionID, const string& strPropertyName, float fValue)
{
	m_pEquipPropertyMgr->ChangeTransactionPropertyValue(uTransactionID,strPropertyName,fValue);
}

float CFighterMediator::GetPropertyValue(const string& strPropertyName)
{
	return m_pEquipPropertyMgr->GetPropertyValue(strPropertyName);
}

uint32 CFighterMediator::GetBurstSoulTimes()
{
	return m_pBurstSoulMgrServer->GetBurstSoulTimes();
}

void CFighterMediator::SetBurstSoulTimes(uint32 uBurstSoulTimes)
{
	m_pBurstSoulMgrServer->AddBurstSoulTimes(uBurstSoulTimes);
}

uint32 CFighterMediator::GetContinuousBurstTimes()
{
	return m_pBurstSoulMgrServer->GetContinuousBurstTimes();
}

void CFighterMediator::BeginStatistic(const TCHAR* sName)
{
	m_pFightStatisticMgr->BeginStatistic(sName);
}

void CFighterMediator::EndStatistic(const TCHAR* sName)
{
	m_pFightStatisticMgr->EndStatistic(sName);
}

void CFighterMediator::OnFightHurt(EFightInfoIndex eFightInfoIndex, uint32 uHurt)
{
	m_pFightStatisticMgr->OnFightHurt(eFightInfoIndex, uHurt);
}

void CFighterMediator::OnBeSubHp(EFightInfoIndex eFightInfoIndex, uint32 uHurt)
{
	m_pFightStatisticMgr->OnBeSubHp(eFightInfoIndex, uHurt);
}

void CFighterMediator::OnBattleStateChanged(bool bEnterBattle)
{
	m_pFightStatisticMgr->OnBattleStateChanged(bEnterBattle);
}

uint32 CFighterMediator::GetStatisticFightHurt(const TCHAR* sName)
{
	return m_pFightStatisticMgr->GetStatisticFightHurt(sName);
}

uint32 CFighterMediator::GetStatisticFightHeal(const TCHAR* sName)
{
	return m_pFightStatisticMgr->GetStatisticFightHeal(sName);
}

uint32 CFighterMediator::GetStatisticBeSubHurt(const TCHAR* sName)
{
	return m_pFightStatisticMgr->GetStatisticBeSubHurt(sName);
}

float CFighterMediator::GetStatisticDps(const TCHAR* sName)
{
	return m_pFightStatisticMgr->GetStatisticDps(sName);
}

float CFighterMediator::GetStatisticHps(const TCHAR* sName)
{
	return m_pFightStatisticMgr->GetStatisticHps(sName);
}

uint32 CFighterMediator::GetStatisticDeadTimes(const TCHAR* sName)
{
	return m_pFightStatisticMgr->GetStatisticDeadTimes(sName);
}

void CFighterMediator::ClearAllStatisticData()
{
	m_pFightStatisticMgr->ClearAllStatisticData();
}

void CFighterMediator::ChangeTargetHP(int32 uHP)
{
	m_pActivityValueMgr->ChangeTargetHP(uHP);
}

uint32 CFighterMediator::GetActivityValue()
{
	return m_pActivityValueMgr->GetActivityValue();
}

void CFighterMediator::ClearActivityValue()
{
	m_pActivityValueMgr->ClearActivityValue();	
}

