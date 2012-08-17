#include "stdafx.h"
#include "CCastingProcess.h"
#include "CCastingProcessCfg.h"
#include "CTxtTableFile.h"
#include "LoadSkillCommon.h"
#include "CSkillInstServer.h"
#include "CGenericTarget.h"
#include "CSkillServer.h"
#include "TimeHelper.h"
#include "CTempVarServer.h"
#include "CSkillMgrServer.h"
#include "CFighterMediator.h"
#include "ICharacterDictatorCallbackHandler.h"
#include "NormalAttack.h"
#include "CTriggerEvent.h"
#include "CMagicEffDataMgrServer.h"
#include "TCoolDownMgr.inl"
#include "ErrLogHelper.h"
#include "PatternRef.inl"
#include "CRelationMgrServer.h"
#include "BaseHelper.h"
#include "CConnServer.h"
#include "FighterProperty.inl"
#include "TSqrAllocator.inl"
#include "CSyncVariantServer.h"
#include "CCharacterMediator.h"
#include "PtrlGas2GacCPPDef.h"

template class TPtRefer<CCastingProcess,CFighterDictator>;

CCastingProcess::CCastingProcess( const CCastingProcessCfgServerSharedPtr& pCfg, CSkillInstServer* pInst, CFighterDictator* pFrom, CGenericTarget* pTo, const CNormalSkillServerSharedPtr& pSkill)
: m_uCount(1)
, m_pCfg(new CCastingProcessCfgServerSharedPtr(pCfg))
, m_pSkillInst(NULL)
, m_uBeAttackedCount(2)
, m_pNormalSkill(new CNormalSkillServerSharedPtr(pSkill))
, m_pSelfCancelEffData(NULL)
, m_pObjCancelEffData(NULL)
, m_bNoStop(false)
, m_bIsFinish(false)
, m_uTcpExpiredTime(0)
{
	m_From.SetHolder(this);
	Ast(pFrom);
	SetFighter(pFrom);
	m_pSkillInst = CSkillInstServer::CreateTransfer(pInst);
	m_pTo = new CGenericTarget(*pTo);
	CCharacterDictator* pCharacter = pFrom->GetCharacter();
	CCharacterMediator* pMediator = pCharacter?pCharacter->CastToCharacterMediator():NULL;
	InitCastTime();
	if (pFrom->IsMoving() && pCfg->GetMoveInterrupt() && pMediator && m_fCastTime != 0.0f)
	{
		pMediator->DisableDirectorMoving(true);
		pMediator->DisableDirectorMoving(false);
	}
	RegistEvent();
	pFrom->Attach(this, eCE_BeforeDie);
	Ast(GetFighter()->GetSkillMgr());
	GetFighter()->GetSkillMgr()->SetCurCasting(this);
	StartCasting();
}

CCastingProcess::~CCastingProcess()
{
	//CFighterDictator* pFrom = GetFighter();
	//if(pFrom)
	//{
	//	pFrom->DelGPCNFlagRef();
	//}

	m_pSkillInst->DestroyTransfer();
	delete m_pTo;
	SafeDelete(m_pNormalSkill);
	SafeDelete(m_pCfg);
}

void CCastingProcess::SetFighter(CFighterDictator* pFighter)
{
	pFighter ? m_From.Attach(pFighter->GetRefFighterByCaset()) : m_From.Detach();
}

void CCastingProcess::RegistEvent()
{
	CFighterDictator* pFighter = GetFighter();
	Ast(pFighter);

	if(m_pTo->GetType() == eTT_Fighter)
	{	
		CFighterDictator* pTo = m_pTo->GetFighter();
		pTo->Attach(this, eCE_BeforeDie);
		pTo->Attach(this, eCE_Offline);
		pTo->Attach(this, eCE_ChangeMapEnd);
	}
	//pFighter->AddGPCNFlagRef();
	pFighter->Attach(this, eCE_BeAttackedFromNotDot);
	pFighter->Attach(this, eCE_BeInterrupted);

	pFighter->Attach(this, eCE_Offline);
	pFighter->Attach(this, eCE_ChangeMapEnd);
	pFighter->Attach(this, eCE_FinishDuel);
	if(GetCastingProcessCfg()->GetMoveInterrupt())
	{
		pFighter->Attach(this, eCE_MoveBegan);
	}
}

void CCastingProcess::UnRegistEvent()
{
	CFighterDictator* pFighter = GetFighter();
	Ast(pFighter);

	if(m_pTo->GetType() == eTT_Fighter)
	{	
		CFighterDictator* pTo = m_pTo->GetFighter();
		pTo->Detach(this, eCE_BeforeDie);
		pTo->Detach(this, eCE_Offline);
		pTo->Detach(this, eCE_ChangeMapEnd);
	}
	pFighter->Detach(this, eCE_BeAttackedFromNotDot);
	pFighter->Detach(this, eCE_BeInterrupted);
	pFighter->Detach(this, eCE_Offline);
	pFighter->Detach(this, eCE_ChangeMapEnd);
	if(GetCastingProcessCfg()->GetMoveInterrupt())
	{
		pFighter->Detach(this, eCE_MoveBegan);
	}
}

void CCastingProcess::InitCastTime()
{
	m_eCastType=GetCastingProcessCfg()->GetCastingType();
	float fCastTimeRatio = IsSingType() ? GetFighter()->m_CastingProcessTimeRatio.Get(GetFighter()) : 1.0f;
	m_fCastTime = float(this->GetCastingProcessCfg()->GetCastingTime()->GetDblValue(GetFighter(),m_pSkillInst->GetSkillLevel()-1)) * fCastTimeRatio;
	if(m_fCastTime < 0.0f) m_fCastTime = 0.0f;

	if ( eCPT_Channel == m_eCastType )
		return;

	CFighterDictator* pFighter = GetFighter();
	Ast(pFighter);
	CFighterDictator* pFighterTarget = m_pTo->GetType() == eTT_Fighter ? m_pTo->GetFighter() : NULL;
	CTempVarMgrServer* pTempMgr=pFighter->GetTempVarMgr();
	Ast(pTempMgr);
	if ( pTempMgr->GetNoSingTime() )
	{
		pTempMgr->CountNoSingTime();
		m_fCastTime = 0;
		m_bNoStop = true;
		if ( !pTempMgr->GetNoSingTime() )
		{
			if(m_pTo->GetType() == eTT_Fighter)
			{
				CTriggerEvent::MessageEvent(eTST_Trigger, pFighter, pFighterTarget, eSET_CancelAnyNoSingTime);
			}
			else if(m_pTo->GetType() == eTT_Position)
			{
				CTriggerEvent::MessageEvent(eTST_Trigger, pFighter, m_pTo->GetPos(), eSET_CancelAnyNoSingTime);
			}
		}
	}
	if (pTempMgr->GetNoSingTimeSkill("战斗技能"))
	{
		const CNormalSkillServerSharedPtr& pSkill = m_pSkillInst->GetSkillCfg();
		if(pSkill && (pSkill->GetCoolDownType() == eSCDT_FightSkill ||
			pSkill->GetCoolDownType() == eSCDT_UnrestrainedFightSkill || pSkill->GetCoolDownType() == eSCDT_NoComCDFightSkill))
		{
			pTempMgr->CancelNoSingTimeSkill(m_pSkillInst->GetSkillName());
			m_fCastTime = 0;
			m_bNoStop = true;
			if(m_pTo->GetType() == eTT_Fighter)
			{
				CTriggerEvent::MessageEvent(eTST_Trigger, pFighter, pFighterTarget, eSET_CancelAnyNoSingTime);
			}
			else if(m_pTo->GetType() == eTT_Position)
			{
				CTriggerEvent::MessageEvent(eTST_Trigger, pFighter, m_pTo->GetPos(), eSET_CancelAnyNoSingTime);
			}
		}
	}
	if ( pTempMgr->GetNoSingTimeSkill(m_pSkillInst->GetSkillName()) )
	{
		pTempMgr->CancelNoSingTimeSkill(m_pSkillInst->GetSkillName());
		m_fCastTime = 0;
		m_bNoStop = true;
		if(m_pTo->GetType() == eTT_Fighter)
		{
			CTriggerEvent::MessageEvent(eTST_Trigger, pFighter, pFighterTarget, eSET_CancelSpecifiedNoSingTime);
		}
		else if(m_pTo->GetType() == eTT_Position)
		{
			CTriggerEvent::MessageEvent(eTST_Trigger, pFighter, m_pTo->GetPos(), eSET_CancelSpecifiedNoSingTime);
		}
	}
}

void CCastingProcess::StartCasting()
{
	CFighterDictator* pFighter = GetFighter();
	Ast(pFighter);

	m_uTcpExpiredTime = pFighter->GetDistortedFrameTime();

	CGenericTarget tempTarget = CGenericTarget(m_pTo->GetPos());
	CGenericTarget* pGenericTarget = &tempTarget;

	if (GetNormalSkill()->GetSelectTargetType() != eFSTT_ObjectPosition)
		pGenericTarget = m_pTo;
	
	CSkillMgrServer* pSkillMgr=pFighter->GetSkillMgr();
	CFighterDictator* pFighterTarget = m_pTo->GetType() == eTT_Fighter ? m_pTo->GetFighter() : NULL;
	uint32 uTargetID = pFighterTarget ? pFighterTarget->GetEntityID() : 0;
	Ast(pSkillMgr);

	if(IsSingType())
	{
		m_uStartTime = pFighter->GetDistortedFrameTime();
		pFighter->RegistDistortedTick(this,uint32(m_fCastTime*1000));
		pSkillMgr->OnCastBegin(GetCastingProcessCfg()->GetId(), m_fCastTime, m_uStartTime, uTargetID);
	}
	else 
	{
		m_fChannelTime=m_fCastTime;
		m_uStartTime = pFighter->GetDistortedFrameTime();
		pFighter->RegistDistortedTick(this, uint32(GetCastingProcessCfg()->GetChannelInterval() * 1000));
		pSkillMgr->OnCastBegin(GetCastingProcessCfg()->GetId(), m_fCastTime, m_uStartTime, uTargetID);
	}

	if(GetCastingProcessCfg()->GetSelfCancelableMagicEff())
	{
		m_pSelfCancelEffData = new CMagicEffDataMgrServer(pFighter);
		GetCastingProcessCfg()->GetSelfCancelableMagicEff()->Do(m_pSkillInst, pFighter, pFighter, m_pSelfCancelEffData);
	}
	if(GetCastingProcessCfg()->GetObjCancelableMagicEff())
	{
		m_pObjCancelEffData = new CMagicEffDataMgrServer(pFighter);
		GetCastingProcessCfg()->GetObjCancelableMagicEff()->Do(m_pSkillInst, pFighter, m_pTo, m_pObjCancelEffData);
	}
}

void CCastingProcess::CancelCasting()
{
	CFighterDictator* pFrom = GetFighter();
	if(pFrom == NULL)
	{
		SafeDelete(m_pSelfCancelEffData);
		SafeDelete(m_pObjCancelEffData);
		pFrom->UnRegistDistortedTick(this);
		delete this;
		return;
	}
	SQR_TRY
	{
		CSkillMgrServer* pSkillMgr = pFrom->GetSkillMgr();
		Ast(pSkillMgr);
		if (pSkillMgr->GetCurCasting() == NULL)
			return;
		pFrom->UnRegistDistortedTick(this);
		pSkillMgr->OnCastEnd(this, GetNormalSkill()->GetId(), GetCastingProcessCfg()->GetId(), m_bIsFinish);

		if(GetCastingProcessCfg()->GetSelfCancelableMagicEff())
		{
			GetCastingProcessCfg()->GetSelfCancelableMagicEff()->Cancel(m_pSkillInst, pFrom, m_pSelfCancelEffData);
			SafeDelete(m_pSelfCancelEffData);
		}
		if(GetCastingProcessCfg()->GetObjCancelableMagicEff())
		{
			GetCastingProcessCfg()->GetObjCancelableMagicEff()->Cancel(m_pSkillInst, m_pTo->GetFighter(), m_pObjCancelEffData);
			SafeDelete(m_pObjCancelEffData);
		}

		CNormalAttackMgr* pAttackMgr = pFrom->GetNormalAttackMgr();
		CFighterDictator* pTargetObject = pFrom->GetTarget();
		uint64 uExpiredTime = pFrom->GetDistortedFrameTime() - m_uTcpExpiredTime;
		if (pAttackMgr && pFrom->CastToFighterMediator())
		{
			if(!m_bIsFinish)
			{
				if(pAttackMgr->IsPaused() && pAttackMgr->IsAttacking())
				{
					pAttackMgr->AddNoneCDTime(uExpiredTime);
					pFrom->RestartAttack(true);
				}
			}
			else
			{
				switch(GetNormalSkill()->GetStartNormalAttack())
				{
				case eANAT_StartAndAutoTrack:
				case eANAT_Start:
				case eANAT_AbsolutelyStartAndAutoTrack:
				case eANAT_AbsolutelyStart:
					{
						if(pAttackMgr->IsAttacking())
						{
							//cout << "$$ CastProcess restartNA" << endl;
							pAttackMgr->AddNoneCDTime(uExpiredTime);
							pFrom->RestartAttack(true);
							if(pTargetObject && (GetNormalSkill()->GetStartNormalAttack() == eANAT_StartAndAutoTrack || GetNormalSkill()->GetStartNormalAttack() == eANAT_AbsolutelyStartAndAutoTrack))
								pFrom->CastToFighterMediator()->TellDirectorToDoNA(pTargetObject, true);
						}
						else
						{
							if(pTargetObject)
							{
								//cout << "## CastProcess startNA" << endl;
								pAttackMgr->AddNoneCDTime(uExpiredTime);
								pFrom->CastToFighterMediator()->TellDirectorToDoNA(pTargetObject, GetNormalSkill()->GetStartNormalAttack() == eANAT_StartAndAutoTrack || GetNormalSkill()->GetStartNormalAttack() == eANAT_AbsolutelyStartAndAutoTrack);
							}
						}
						break;
					}
				case eANAT_Cancel:
					{
						if(pAttackMgr->IsAttacking())
							pAttackMgr->AddNoneCDTime(uExpiredTime);
						pFrom->CancelAttack(true);
						break;
					}
				case eANAT_NoChange:
					{
						if(pAttackMgr->IsAttacking())
						{
							pAttackMgr->AddNoneCDTime(uExpiredTime);
							pFrom->RestartAttack(true);
						}
						break;
					}
				default:
					{
						if(pAttackMgr->IsAttacking())
						{
							pAttackMgr->AddNoneCDTime(uExpiredTime);
							pFrom->RestartAttack(true);
						}
						break;
					}
				}
			}
		}
		if (GetNormalSkill()->CheckDoSkillRule())
			pSkillMgr->OnSkillEnd(GetNormalSkill()->GetId(), m_bIsFinish);
	}
	SQR_CATCH(exp)
	{
		ostringstream strm;		
		strm << " CCastingProcess::CancelCasting " << GetCastingProcessCfg()->GetName();
		CNormalAttackMgr* pAttackMgr = pFrom->GetNormalAttackMgr();
		if(pAttackMgr)
		{
			strm << " NA can be restarted " << pAttackMgr->GetCanBeRestarted();
		}
		exp.AppendMsg(strm.str().c_str());
		CConnServer* pConnServer = pFrom->GetConnection();
		if (pConnServer)
			LogExp(exp, pConnServer);
		else
			LogExp(exp);
	}
	SQR_TRY_END;
	
	delete this;
}

void CCastingProcess::SyncCasting(CFighterDictator* pFighter)
{	
	CFighterDictator* pFrom = GetFighter();
	Ast(pFrom);
	CSkillMgrServer* pSkillMgr=pFrom->GetSkillMgr();
	Ast(pSkillMgr);

	CGas2GacCPP_OnSyncCastingProcess Cmd;
	Cmd.uObjID = pFighter?pFighter->GetEntityID():0;
	Cmd.uFromID = pFrom->GetEntityID();
	Cmd.uCastID = GetCastingProcessCfg()->GetId();
	Cmd.fCastTime = m_fCastTime;
	Cmd.uStartTime = m_uStartTime;

	pFighter->SendCmdToGac(&Cmd, eOnlyToDirector);
}

void CCastingProcess::OnBeCancel(CFighterDictator* pAttacker)
{
	CFighterDictator* pFrom = GetFighter();
	Ast(pFrom);
	if(m_bIsFinish == false)
	{
		CSkillMgrServer* pSkillMgr=pFrom->GetSkillMgr();
		CCoolDownMgrServer*  pCDMgr=pFrom->GetCoolDownMgr();
		Ast(pSkillMgr);
		Ast(pCDMgr);
	
		if (IsSingType())
		{
			pCDMgr->IntNewCoolDown(GetNormalSkill()->GetName().c_str(), GetNormalSkill()->GetCoolDownType(), 0);
			pCDMgr->ClearCommonCD();
			pSkillMgr->OnCoolDownEnd(GetNormalSkill()->GetName().c_str());
			pSkillMgr->OnClearCommonCD();
		}
		CFighterMediator* pMed = pFrom->CastToFighterMediator();
		if (pMed)
		{
			pMed->SendFailureMsgToGac((uint32)(eDSR_SkillBeInterrupted));
		}
	}
	if (!m_bIsFinish)
	{
		pFrom->OnPrintFightInfo(EFII_InterruptCasting, pAttacker?pAttacker->GetEntityID():0, pFrom->GetEntityID(), m_pSkillInst, 0, eATT_None, 0, 0);
	}
	CancelCasting();
}

void CCastingProcess::OnBeAttacked(CFighterDictator* pAttacker)
{
	CFighterDictator* pFrom = GetFighter();
	Ast(pFrom);

	const CNormalSkillServerSharedPtr& pSkill = m_pSkillInst->GetSkillCfg();
	if (pSkill && eSCDT_NonFightSkill == pSkill->GetCoolDownType())
	{
		OnBeCancel(pAttacker);
		return;
	}

	if (m_uBeAttackedCount == 0)
	{
		return;
	}
	CSkillMgrServer* pSkillMgr=pFrom->GetSkillMgr();
	Ast(pSkillMgr);
	m_uBeAttackedCount--;
	float fDelayRate = ((float)rand())/RAND_MAX;
	float fInterruptRate = float(GetCastingProcessCfg()->GetInterruptPercent()->GetDblValue(pFrom,m_pSkillInst->GetSkillLevel()-1));
	float fResistInterruptRate = 1.0f - fInterruptRate + pFrom->m_ResistCastingInterruptionRate.Get(pFrom);
	
	//cout << "fDelayRate = " << fDelayRate << " fResistInterruptRate = " << fResistInterruptRate << endl;
	if(fDelayRate >= fResistInterruptRate)
	{
		CFighterDictator* pTo = m_pTo->GetFighter();
		uint32 uTargetID = pTo ? pTo->GetEntityID() : 0;
		if(IsSingType())
		{
			uint64 uInterTime = pFrom->GetDistortedFrameTime();
			float fRestTime = m_fCastTime - uInterTime / 1000.0f +	m_uStartTime / 1000.0f + m_fCastTime*0.2f;
			if (fRestTime <= 0.0000001f)
			{
				fRestTime = 0.0f;
			}
			if (fRestTime >= m_fCastTime || m_fCastTime <= 1)//吟唱时间<1s的,打断则从头吟唱,>1s的打退20%
			{
				m_uStartTime = pFrom->GetDistortedFrameTime();
				fRestTime = m_fCastTime;
			}
			else
			{
				m_uStartTime = m_uStartTime + uint64(m_fCastTime*0.2*1000);
			}
			//m_fExpiredTime = m_fCastTime - fRestTime;
			pFrom->RegistDistortedTick(this,uint32(fRestTime*1000));
			pSkillMgr->OnCastBegin(GetCastingProcessCfg()->GetId(), m_fCastTime, m_uStartTime, uTargetID);
		}
		else if(m_eCastType==eCPT_Channel)
		{
			float fDelTime = float(m_fCastTime*0.2);
			if (m_fChannelTime >= fDelTime)
			{
				m_fChannelTime -=fDelTime;
				m_uStartTime -= uint64(fDelTime * 1000);
			}
			else
			{
				m_fChannelTime = 0;
				m_uStartTime -= uint64(m_fChannelTime * 1000);
			}
			//m_fExpiredTime = m_fCastTime-m_fChannelTime+m_uCount;
			pSkillMgr->OnCastBegin(GetCastingProcessCfg()->GetId(), m_fCastTime, m_uStartTime, uTargetID);
		}
	}
}

void CCastingProcess::DoEffect()
{
	CMagicEffServerSharedPtr pMagicEff;
	if (m_eCastType == eCPT_GradeSing)
	{
		m_bIsFinish = true;
		pMagicEff = GetCastingProcessCfg()->GetMagicEff(m_uCount);
	}
	else
	{
		pMagicEff = GetCastingProcessCfg()->GetMagicEff();
	}
	CFighterDictator* pFrom = GetFighter();
	CFighterDictator* pTo = m_pTo->GetFighter();
	Ast(pFrom);

	bool bNeedMsgEventDoFightSkill = false;
	if (m_eCastType != eCPT_Channel)
	{
		if (GetNormalSkill()->GetCoolDownType() == eSCDT_FightSkill || GetNormalSkill()->GetCoolDownType() == eSCDT_NoComCDFightSkill
			|| GetNormalSkill()->GetCoolDownType() == eSCDT_UnrestrainedFightSkill)
		{
			bNeedMsgEventDoFightSkill = true;
		}
	}

	string sSkillName = m_pSkillInst->GetSkillName();
	bool bIsFinish = m_bIsFinish;
	if (pMagicEff)
	{
		/*if(m_pTo->GetType() == eTT_Fighter && pTo && pFrom &&
			CRelationMgrServer::IsEnemy(pFrom, pTo) && pTo->GetCtrlState(eFCS_ReflectMagic) && m_pSkillInst->GetSkillType() == eST_Magic)
		{
			pMagicEff->Do(m_pSkillInst, pFrom, pFrom);
			switch(m_pSkillInst->GetAttackType())
			{
			case eATT_Nature:
				CTriggerEvent::MessageEvent(eTST_Trigger, pFrom, pTo, eSET_DoNatureMagic);
				break;
			case eATT_Destroy:
				CTriggerEvent::MessageEvent(eTST_Trigger, pFrom, pTo, eSET_DoDestructionMagic);
				break;
			case eATT_Evil:
				CTriggerEvent::MessageEvent(eTST_Trigger, pFrom, pTo, eSET_DoEvilMagic);
				break;
			default:
				break;
			}
		}*/
		if(m_pTo->GetType() == eTT_Fighter && pTo && pFrom &&
			CRelationMgrServer::IsEnemy(pFrom, pTo))
		{
			pMagicEff->Do(m_pSkillInst, pFrom, m_pTo);
			CTriggerEvent::MessageEvent(eTST_Trigger, pFrom, pTo, eSET_AttackByEnemyObjectSkill);
		}
		else
		{
			if (m_pSkillInst->GetProcessLocked() && m_pTo->GetType() == eTT_Position)
			{		
				CFighterDictator* pTargetObject = GetFighter()->GetTarget();
				if (pTargetObject)
				{
					CFPos pos;
					pTargetObject->GetPixelPos(pos);
					m_pTo->SetPos(pos);
					CCharacterDictator* pCharacter = GetFighter()->GetCharacter();
					pCharacter->SetAndSyncActionDirByPos(pos);
				}
			}

			pMagicEff->Do(m_pSkillInst, pFrom, m_pTo);
		}
	}
	if (bIsFinish)
	{
		pFrom->OnPrintFightInfo(EFII_SuccessDoCasting, pFrom->GetEntityID(), 0, NULL, 0, eATT_None, 0, 0, false, 0, sSkillName.c_str(), (uint16)sSkillName.size());
	}
	//蓄力吟唱, 执行一次效果后就立刻取消流程,但有可能因为pTo的死亡导致已经Cancel,需要下面的
	CCastingProcess* pCurCastingProcess = pFrom->GetSkillMgr()->GetCurCasting();
	if (pCurCastingProcess && pCurCastingProcess->GetCastingProcessCfg()->GetCastingType() == eCPT_GradeSing) 
		CancelCasting();
	
	if (bNeedMsgEventDoFightSkill)
		CTriggerEvent::MessageEvent(eTST_Trigger, pFrom, NULL, eSET_DoFightSkill);
}

void CCastingProcess::OnTick()
{
	CFighterDictator* pFrom = GetFighter();
	CFighterDictator* pTo = m_pTo->GetFighter();
	if(!pFrom || (!pTo && m_pTo->GetType() == eTT_Fighter))
	{
		CancelCasting();
		return;
	}

	SQR_TRY{
		//UnRegistEvent();
		if ( !pFrom->CppIsAlive() || !GetNormalSkill()->IsCastSkillDistance(m_pSkillInst,pFrom,m_pTo,1.3f) || (pTo && !pTo->CppIsAlive() && !GetNormalSkill()->GetTargetIsCorpse()) )
		{
			OnBeCancel();	
			return;
		}

		if (IsSingType())
		{
			m_bIsFinish = true;
			DoEffect();
			if (pFrom->GetSkillMgr()->GetCurCasting() == NULL)
				return;
			CancelCasting();
			return;
		}
		else//引导 or 蓄力吟唱
		{
			if (m_eCastType==eCPT_Channel)
				DoEffect();
			if (pFrom->GetSkillMgr()->GetCurCasting() == NULL)
				return;

			m_uCount++;
			if(m_uCount * GetCastingProcessCfg()->GetChannelInterval() > m_fChannelTime)
			{
				m_bIsFinish = true;
				if(GetCastingProcessCfg()->GetFinalMagicEff())
					GetCastingProcessCfg()->GetFinalMagicEff()->Do(m_pSkillInst, pFrom, m_pTo);
				if (pFrom->GetSkillMgr()->GetCurCasting() == NULL)
					return;

				if (m_eCastType==eCPT_GradeSing)
				{
					DoEffect();
					if (pFrom->GetSkillMgr()->GetCurCasting() == NULL)
						return;
				}
				CancelCasting();
			}

			//RegistEvent();
		}
	}
	SQR_CATCH(exp)
	{
		CConnServer* pConnServer = pFrom->GetConnection();
		if (pConnServer)
			LogExp(exp, pConnServer);
		else
			LogExp(exp);
	}
	SQR_TRY_END;
}

void CCastingProcess::OnCOREvent(CPtCORSubject *pSubject, uint32 uEvent,void* pArg)
{
	CFighterDictator* pFrom = GetFighter();
	Ast(pFrom);

	if( m_pTo&& pSubject == m_pTo->GetFighter() )
	{
		switch(uEvent)
		{
		case eCE_BeforeDie:
		case eCE_Offline:
		case eCE_ChangeMapEnd:
			OnBeCancel();
			return;
		default:
			break;
		}	
	}
	if(pSubject == pFrom)
	{
		CFighterDictator* pAttacker = NULL;
		switch(uEvent)
		{
		case eCE_BeAttackedFromNotDot:
			{
				pAttacker = reinterpret_cast<CFighterDictator*>(pArg);
				OnBeAttacked(pAttacker);
				break;
			}
		case eCE_BeInterrupted:
			{
				pAttacker = reinterpret_cast<CFighterDictator*>(pArg);
			}
		case eCE_MoveBegan:
			if (m_eCastType == eCPT_GradeSing)	//蓄力吟唱,移动或被打断时直接执行效果
			{
				DoEffect();
				if (pFrom->GetSkillMgr()->GetCurCasting() == NULL)
					return;
				break;
			}
		case eCE_BeforeDie:
		case eCE_Offline:
		case eCE_ChangeMapEnd:
		case eCE_FinishDuel:
			OnBeCancel(pAttacker);
			return;
		}
	}
}

bool CCastingProcess::CanBeInterrupted()const
{
	if(abs(float(GetCastingProcessCfg()->GetInterruptPercent()->GetDblValue(GetFighter(),m_pSkillInst->GetSkillLevel()-1)) - 0.0f < 0.0001f))
	{
		return false;
	}
	return true;
}

CCastingProcessCfgServer* CCastingProcess::GetCastingProcessCfg()const
{
	return m_pCfg->get();
}

CNormalSkillServer* CCastingProcess::GetNormalSkill()
{
	return m_pNormalSkill->get();
}

bool CCastingProcess::IsSingType()
{
	return m_eCastType == eCPT_Sing || m_eCastType == eCPT_ImmovableSing;
}
