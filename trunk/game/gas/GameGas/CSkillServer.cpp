#include "stdafx.h"
#include "CSkillServer.h"
#include "CSkillInstServer.h"
#include "CCoolDownMgrServer.h"
#include "CCastingProcess.h"
#include "CGenericTarget.h"
#include "CPos.h"
#include "CCoreSceneServer.h"
//#include "CMagicEffServer.h"
#include "CRelationMgrServer.h"
#include "CoreCommon.h"
#include "CTriggerEvent.h"
#include "NormalAttack.h"
#include "CFighterMediator.h"
#include "CSkillMgrServer.h"
#include "CMetaSceneServer.h"
#include "CPixelPath.h"
#include "ErrLogHelper.h"
#include "BaseHelper.h"
#include "CConnServer.h"
#include "CCfgCalcServer.inl"
#include "CAniKeyFrameMgrServer.h"
#include "CCharacterDictator.h"


CNormalSkillServerSharedPtr CNormalSkillServer::ms_NULL;

float CNormalSkillServer::GetDistance(const CFighterDictator* pFrom, uint8 uSkillLevel)const
{
	if(m_pDistance != NULL)
		return float(m_pDistance->GetDblValue(pFrom, uSkillLevel));
	else
		return 0.0f;
}

EDoSkillResult CNormalSkillServer::DoPlayerPassiveSkill(CSkillInstServer* pInst,CFighterMediator* pFrom, CGenericTarget* pTo, uint8 uSkillLevel)const
{
	EDoSkillResult eResult = PreDoSkill(pInst,pFrom,  pTo);
	CSkillMgrServer* pSkillMgr = pFrom->GetSkillMgr();

	Ast(pSkillMgr);

	if(eResult != eDSR_Success)
	{
		// 无OnSkillEnd回调
		//if (CheckDoSkillRule())
		//	pSkillMgr->OnSkillEnd(GetId(), false);

		return eResult;
	}

	DoPassiveSkillEffect(pInst,pFrom,pTo,uSkillLevel);
	
	DoCoolDown(pFrom, uSkillLevel);

	return eDSR_Success;
}

EDoSkillResult CNormalSkillServer::DoPlayerSkill(CSkillInstServer* pInst,CFighterMediator* pFrom, CGenericTarget* pTo, uint8 uSkillLevel)const
{
	//cout << "GlobalID 为 " << pFrom->GetEntityID() << " 角色" << ": 施放技能 -- " << m_sName << endl;
	EDoSkillResult eResult = PreDoSkill(pInst,pFrom,  pTo);
	CSkillMgrServer* pSkillMgr=pFrom->GetSkillMgr();

	Ast(pSkillMgr);

	CCastingProcess* pCurCast = pSkillMgr->GetCurCasting();
	if (pCurCast && pCurCast->GetCastingProcessCfg()->GetCastingType() == eCPT_GradeSing &&
		pCurCast->GetCastingProcessCfg()->GetName() == pInst->GetSkillName())
	{	//蓄力吟唱类技能第二次使用,即蓄力结束, 不需要PreDoSkill
		eResult = eDSR_Success;
	}

	if (eResult != eDSR_Success)
	{
		SQR_TRY{
			//SkillItem失败信息处理
			if (GetCoolDownType() == eSCDT_DrugItemSkill || GetCoolDownType() == eSCDT_SpecialItemSkill || GetCoolDownType() == eSCDT_OtherItemSkill)
			{
				if (eResult == eDSR_InCDTime)
				{
					eResult = eDSR_InSkillItemCDTime;
				}
				else
				{
					eResult = eDSR_UseSkillItemFail;
				}
			}
			if (eResult != eDSR_SkillEndWithOutSing)
				pFrom->SendFailureMsgToGac((uint32)(eResult));
			
			//cout << "技能释放失败后 Restart NA " << endl;
			CNormalAttackMgr* pAttackMgr=pFrom->GetNormalAttackMgr();
			if (pAttackMgr && pSkillMgr->GetCurCasting() == NULL)
			{
				switch(GetStartNormalAttack())
				{
				case eANAT_AbsolutelyStartAndAutoTrack:
				case eANAT_AbsolutelyStart:
					{
						StartOrRestartNADueToSkillFailure(pFrom);
						break;
					}
				default:
					if(pAttackMgr->IsPaused() && pAttackMgr->IsAttacking())
					{
						pFrom->RestartAttack(true);
					}
					break;
				}
			}	
		}
		SQR_CATCH(exp)
		{
			CConnServer* pConn = pFrom->GetConnection();
			if (pConn)
				LogExp(exp, pConn);
			else
				LogExp(exp);
		}
		SQR_TRY_END;

		if (CheckDoSkillRule())
			pSkillMgr->OnSkillEnd(GetId(), false);

		return eResult;
	}

	DoEffect(pInst,pFrom,pTo,uSkillLevel);
	
	if (!pFrom->CastToFighterMediator()||GetCastingProcess())
	{
		CNormalAttackMgr* pAttackMgr=pFrom->GetNormalAttackMgr();
		//cout << "技能释放成功后 Restart NA" << endl;
		if (pAttackMgr && pSkillMgr->GetCurCasting() == NULL)
		{
			CFighterDictator* pTargetObject = pFrom->GetTarget();
			switch(GetStartNormalAttack())
			{
			case eANAT_StartAndAutoTrack:
			case eANAT_Start:
			case eANAT_AbsolutelyStartAndAutoTrack:
			case eANAT_AbsolutelyStart:
				{
					if(pAttackMgr->IsAttacking())
					{
						//cout << "$$ skill restartNA" << endl;
						pFrom->RestartAttack(true);
						if(pTargetObject && (GetStartNormalAttack() == eANAT_StartAndAutoTrack || GetStartNormalAttack() == eANAT_AbsolutelyStartAndAutoTrack))
							pFrom->TellDirectorToDoNA(pTargetObject, true);
					}
					else
					{
						if(pTargetObject)
						{
							//cout << "## skill startNA" << endl;
							pFrom->TellDirectorToDoNA(pTargetObject, GetStartNormalAttack() == eANAT_StartAndAutoTrack || GetStartNormalAttack() == eANAT_AbsolutelyStartAndAutoTrack);
						}
					}
					break;
				}
			case eANAT_Cancel:
				{
					pFrom->CancelAttack(true);
					break;
				}
			case eANAT_NoChange:
				{
					if(pAttackMgr->IsAttacking())
					{
						pFrom->RestartAttack(true);
					}
					break;
				}
			default:
				{
					if(pAttackMgr->IsAttacking())
					{
						pFrom->RestartAttack(true);
					}
					break;
				}
			}
		}	
	}
	
	DoCoolDown(pFrom, uSkillLevel);

	return eDSR_Success;
}

void CNormalSkillServer::PrintFightInfo(CSkillInstServer* pInst, CFighterDictator* pFrom, CGenericTarget* pTo)const
{
	if (GetCoolDownType() != eSCDT_DrugItemSkill && GetCoolDownType() != eSCDT_SpecialItemSkill && GetCoolDownType() != eSCDT_OtherItemSkill)
	{
		uint32 uAttackerID = pFrom->GetEntityID();
		if (pTo->GetType() == eTT_Fighter)
		{
			pFrom->OnPrintFightInfo(EFII_DoSkill, uAttackerID, pTo->GetFighter()->GetEntityID(), pInst, 0, eATT_None, 0, 0);
		}
		else if(pTo->GetType() == eTT_Position)
		{
			pFrom->OnPrintFightInfo(EFII_DoPosSkill, uAttackerID, 0, pInst, 0, eATT_None, 0, 0);
		}
	}
}

void CNormalSkillServer::TriggerMsgBeforeDoingEffect(CSkillInstServer* pInst, CFighterDictator* pFrom)const
{
	// 触发器触发
	if ((pInst->GetSkillName() != "骑术" || pFrom->GetCtrlState(eFCS_FeignDeath)) &&
		pInst->GetSkillName() != "召唤术：小雪狼" && GetCoolDownType() != eSCDT_PublicSkill &&
		GetCoolDownType() != eSCDT_DrugItemSkill && GetCoolDownType() != eSCDT_SpecialItemSkill && GetCoolDownType() != eSCDT_OtherItemSkill)
	{
		CTriggerEvent::MessageEvent(eTST_Trigger, pFrom, NULL, eSET_DoSkill);
	}
	if (!GetCastingProcess() || GetCastingProcess()->GetCastingType() == eCPT_Channel)
	{
		if (!GetIsStanceSkill() && (GetCoolDownType() == eSCDT_FightSkill || GetCoolDownType() == eSCDT_NoComCDFightSkill
			|| GetCoolDownType() == eSCDT_UnrestrainedFightSkill) )
		{
			CTriggerEvent::MessageEvent(eTST_Trigger, pFrom, NULL, eSET_DoFightSkill);
		}
	}
}

void CNormalSkillServer::DoPassiveSkillEffect(CSkillInstServer* pInst, CFighterDictator* pFrom, CGenericTarget* pTo, uint8 uSkillLevel)const
{
	SQR_TRY
	{
		PrintFightInfo(pInst, pFrom, pTo);
		//TriggerMsgBeforeDoingEffect(pInst, pFrom);
		if(GetMagicEff())
		{
			CFighterDictator* pFighterTarget = pTo->GetFighter();
			GetMagicEff()->Do(pInst,pFrom, pTo);

			if(pTo->GetType() == eTT_Fighter && pFighterTarget && pFrom &&
				CRelationMgrServer::IsEnemy(pFrom, pFighterTarget) && !GetCastingProcess())
			{
				CTriggerEvent::MessageEvent(eTST_Trigger, pFrom, pFighterTarget, eSET_AttackByEnemyObjectSkill);
			}
		}
	}
	SQR_CATCH(exp)
	{
		CConnServer* pConn = pFrom->GetConnection();
		if (pConn)
			LogExp(exp, pConn);
		else
			LogExp(exp);
	}
	SQR_TRY_END;
}

void CNormalSkillServer::DoEffect(CSkillInstServer* pInst, CFighterDictator* pFrom, CGenericTarget* pTo, uint8 uSkillLevel, bool bDoSkillRule)const
{
	ETargetType eTargetType = pTo->GetType();
	CSkillMgrServer* pSkillMgr=pFrom->GetSkillMgr();
	CCastingProcess* pCurCast = pSkillMgr ? pSkillMgr->GetCurCasting() : NULL;
	CFighterDictator* pFighterTarget = pTo->GetFighter();
	Ast(pSkillMgr);

	SQR_TRY
	{
		if (pSkillMgr && pCurCast && pCurCast->GetCastingProcessCfg()->GetCastingType() == eCPT_GradeSing &&
			pCurCast->GetCastingProcessCfg()->GetName() == pInst->GetSkillName())
		{//蓄力吟唱类技能第二次使用,即蓄力结束,直接执行效果
			pCurCast->DoEffect();
		}
		else
		{
			PrintFightInfo(pInst, pFrom, pTo);
			TriggerMsgBeforeDoingEffect(pInst, pFrom);
			if(eTargetType == eTT_Fighter && CheckDoSkillRule() && bDoSkillRule)
			{
				pSkillMgr->OnSkillBegin(GetId(), uSkillLevel, eTargetType, pFighterTarget->GetEntityID());
			}
			else if(eTargetType == eTT_Position && CheckDoSkillRule() && bDoSkillRule)
			{
				pSkillMgr->OnSkillBegin(GetId(), uSkillLevel, eTargetType, 0);
			}

			if(GetMagicEff())
			{
				CFighterDictator* pFighterTarget = pTo->GetFighter();

				if (pFrom->GetCharacter()&&!GetCastingProcess())
				{
					CSkillMgrServer* pSkillMgr = pFrom->GetSkillMgr();
					pSkillMgr->GetAniKeyMgr()->RegistSkill(GetMagicEff(),pInst,pTo);
				}
				else
				{
					GetMagicEff()->Do(pInst,pFrom, pTo);
				}
				if(pTo->GetType() == eTT_Fighter && pFighterTarget && pFrom &&
					CRelationMgrServer::IsEnemy(pFrom, pFighterTarget) && !GetCastingProcess())
				{
					CTriggerEvent::MessageEvent(eTST_Trigger, pFrom, pFighterTarget, eSET_AttackByEnemyObjectSkill);
				}
			}
			// 如有施法流程，进入施法流程
			if(GetCastingProcess() && CheckDoSkillRule() && bDoSkillRule)
			{
				new CCastingProcess(GetCastingProcess(), pInst, pFrom, pTo, CNormalSkillServer::GetSkillByID(GetId()));
			}
		}
	}
	SQR_CATCH(exp)
	{
		CConnServer* pConn = pFrom->GetConnection();
		if (pConn)
			LogExp(exp, pConn);
		else
			LogExp(exp);
	}
	SQR_TRY_END;

	if(!GetCastingProcess() && CheckDoSkillRule() && bDoSkillRule)
	{
		pSkillMgr->OnSkillEnd(GetId(), true);
	}
}

EDoSkillResult CNormalSkillServer::PreDoSkill(CSkillInstServer* pInst,CFighterMediator* pFrom, CGenericTarget* pTo)const
{	
	CCoolDownMgrServer*  pCDMgr=pFrom->GetCoolDownMgr();
	CRelationMgrServer& relationMgr = CRelationMgrServer::Inst();
	CPixelPath* pPath = NULL;
	CFPos posFrom, posTo;
	CPos GridposTo;
	posFrom = pFrom->GetPixelPos();
	CFighterDictator* pToFighter = pTo->GetType() == eTT_Fighter ? pTo->GetFighter() : NULL;

	if (!(pCDMgr->IsCoolDown(m_sName.c_str(), m_eCoolDownType)))
	{
		return eDSR_InCDTime;
	}

	if(pTo->GetType() == eTT_Fighter)
	{
		if (!pToFighter->CppIsAlive() && !m_bTargetAliveCheckIsIgnored)
			return eDSR_TargetIsDead;

		if (pToFighter->CppIsAlive() && m_bTargetAliveCheckIsIgnored&&!pInst->GetIsPassive())
			return eDSR_TargetIsAlive;

		if (eFSTT_EnemyObject == GetSelectTargetType() || eFSTT_FriendAndEnemy == GetSelectTargetType())
		{
			if (pFrom == pToFighter)
				return eDSR_TargetIsSelf;

			switch(relationMgr.GetRelationType(pFrom, pToFighter))
			{
			case eRT_Friend:
				if(eFSTT_EnemyObject == GetSelectTargetType())
					return eDSR_TargetIsFriend;
				break;
			case eRT_Neutral:
				return eDSR_TargetIsNeutral;
				break;
			case eRT_SelfUnderPKLevel:
				return eDSR_SelfUnderPKLevel;
			    break;
			case eRT_TargetUnderPKLevel:
				return eDSR_TargetUnderPKLevel;
			    break;
			case eRT_SelfBeyondLv:
				return eDSR_SelfBeyondLv;
			    break;
			case eRT_TargetBeyondLv:
				return eDSR_TargetBeyondLv;
			    break;
			case eRT_SelfSafetyState:
				return eDSR_SelfSafetyState;
				break;
			case eRT_TargetSafetyState:
				return eDSR_TargetSafetyState;
				break;
			case eRT_FriendInRivalry:
				return eDSR_TargetIsFriend;
				break;
			case eRT_TargetInDuel:
				return eDSR_TargetInDuel;
				break;
			case eRT_DuelTargetWrong:
				return eDSR_DuelTargetWrong;
				break;
			default:
			    break;
			}
		}

		if (eFSTT_FriendObject == GetSelectTargetType() || eFSTT_SelectFriendObject == GetSelectTargetType())
		{
			switch(relationMgr.GetRelationType(pFrom, pToFighter))
			{
			case eRT_Enemy:
				return eDSR_TargetIsEnemy;
				break;
			case eRT_Neutral:
				return eDSR_TargetIsNeutral;
				break;
			case eRT_FriendInRivalry:
				return eDSR_CanNotAssist;
				break;
			case eRT_TargetInDuel:
				return eDSR_TargetInDuel;
				break;
			case eRT_DuelTargetWrong:
				return eDSR_DuelTargetWrong;
				break;
			default:
				break;
			}
		}

		if (eFSTT_AllObject == GetSelectTargetType() || eFSTT_NotNeutral == GetSelectTargetType() ||
			eFSTT_TeammatesOrEnemy == GetSelectTargetType())
		{
			switch(relationMgr.GetRelationType(pFrom, pToFighter))
			{
			case eRT_Neutral:
				{
					if (eFSTT_NotNeutral == GetSelectTargetType() || eFSTT_TeammatesOrEnemy == GetSelectTargetType())
						return eDSR_TargetIsNeutral;
				}
				break;
			case eRT_SelfUnderPKLevel:
				return eDSR_SelfUnderPKLevel;
				break;
			case eRT_TargetUnderPKLevel:
				return eDSR_TargetUnderPKLevel;
				break;
			case eRT_SelfBeyondLv:
				return eDSR_SelfBeyondLv;
				break;
			case eRT_TargetBeyondLv:
				return eDSR_TargetBeyondLv;
				break;
			case eRT_SelfSafetyState:
				return eDSR_SelfSafetyState;
				break;
			case eRT_TargetSafetyState:
				return eDSR_TargetSafetyState;
				break;
			case eRT_FriendInRivalry:
				return eDSR_CannotDoSkillToRivalry;
				break;
			case eRT_TargetInDuel:
				return eDSR_TargetInDuel;
				break;
			case eRT_DuelTargetWrong:
				return eDSR_DuelTargetWrong;
				break;
			case eRT_Friend:
				{
					if ( eFSTT_TeammatesOrEnemy == GetSelectTargetType() && (pFrom != pToFighter) )
					{
						CCharacterDictator* pDictator = pToFighter->GetCharacter();
						if (pDictator->GetMaster())
						{
							pDictator = pDictator->GetMaster();
						}
						if (pDictator && !relationMgr.InTheSameTeam(pFrom, pDictator->GetFighter()))
						{
							return eDSR_TargetNotTeammatesOrEnemy;
						}
					}
				}
				break;
			default:
				break;
			}
		}

		if (eFSTT_NotFriendObject == GetSelectTargetType())
		{
			if (relationMgr.GetRelationType(pFrom, pToFighter) == eRT_Friend)
			{
				return eDSR_TargetIsFriendType;
			}
		}

		if (pFrom->GetScene() != pToFighter->GetScene())
			return eDSR_OutOfDistance;

		posTo = pToFighter->GetPixelPos();
	}
	else if (pTo->GetType() == eTT_Position)
	{
		PixelPos2GridPos(GridposTo,pTo->GetPos());
		posTo = pTo->GetPos();
	}

	if(m_eSelectTargetType != eFSTT_Self && m_eSelectTargetType != eFSTT_SelfDirection && m_eSelectTargetType != eFSTT_SelfPosition)
	{
		if(!(pTo->GetType() == eTT_Fighter && pFrom == pToFighter))
		{
			CCoreSceneServer* pScene = pFrom->GetScene();
			if (pScene)pPath = pScene->CreatePath(posFrom, posTo, eFPT_Line, GetBarrierType());
			if (NULL == pPath && (GetGridByPixel(posTo.x) != GetGridByPixel(posFrom.x) || GetGridByPixel(posTo.y) != GetGridByPixel(posFrom.y)))
			{
				if ("跳跃"==m_sName)
				{
					CMetaSceneServer* pMetaScene= pScene?pScene->GetMetaScene():NULL;
					if (pMetaScene&& eBT_LowBarrier != pMetaScene->GetBarrier(GridposTo))
					{
						SafeRelease(pPath);
						return eDSR_ToTargetHaveBarrier;
					}
				}
				else
				{
					SafeRelease(pPath);
					return eDSR_ToTargetHaveBarrier;
				}
			}
			SafeRelease(pPath);
		}
	}

	if (eFSTT_Position == m_eSelectTargetType || eFSTT_EnemyObject == m_eSelectTargetType 
		|| eFSTT_FriendObject == m_eSelectTargetType || eFSTT_NeutralObject == m_eSelectTargetType 
		|| eFSTT_PositionOrEnemy == m_eSelectTargetType || eFSTT_PositionOrFriend == m_eSelectTargetType 
		|| eFSTT_FriendAndEnemy == m_eSelectTargetType || eFSTT_AllObject == m_eSelectTargetType
		|| eFSTT_NotNeutral == m_eSelectTargetType || eFSTT_SelectFriendObject == m_eSelectTargetType
		|| eFSTT_PositionOrLockedTarget == m_eSelectTargetType || eFSTT_TeammatesOrEnemy == m_eSelectTargetType)
	{
		float x = posFrom.x - posTo.x;
		float y = posFrom.y - posTo.y;
		float fDistance = 0.0f;
		if(pTo->GetType() == eTT_Position)
		{
			fDistance = sqrt(x*x + y*y) / eGridSpanForObj;
		}
		else if(pTo->GetType() == eTT_Fighter)
		{
			fDistance = pFrom->GetFighterDistInGrid(pToFighter);
		}

		if (fDistance > GetMaxCastSkillDistance()->GetDblValue(pFrom, pInst->GetSkillLevel() - 1))
		{
			return eDSR_OutOfDistance;
		}
		if (fDistance < GetMinCastSkillDistance()->GetDblValue(pFrom, pInst->GetSkillLevel() - 1))
		{
			return eDSR_NotEnoughDistance;
		}
	}

	EDoSkillResult eResult;
	if(GetMagicEff())
	{
		eResult = GetMagicEff()->DoMagicCond(pInst, pFrom, pTo);
	}
	else
	{
		eResult = eDSR_Success;
	}
	return eResult;
}

bool CNormalSkillServer::IsCastSkillDistance(CSkillInstServer* pInst, CFighterDictator* pFrom, CGenericTarget* pTo, float fExtraDistRatio)
{
	if(	m_eSelectTargetType == eFSTT_Self || 
		m_eSelectTargetType == eFSTT_SelfPosition || 
		m_eSelectTargetType == eFSTT_SelfDirection ||
		!(pFrom->CastToFighterMediator()))
	{
		return true;
	}


	return	IsInsideOfDistance(pFrom,pTo,pInst->GetSkillLevel(), fExtraDistRatio);
}

bool CNormalSkillServer::IsInsideOfDistance(CFighterDictator* pFrom, const CGenericTarget* pTo, uint32 uLevel, float fExtraDistRatio)
{
	CFPos posFrom;
	posFrom=pFrom->GetPixelPos();
	CFPos posTo;
	float fDistance = 0.0f; 
	if(pTo->GetType() == eTT_Position)
	{
		CFPos posTo = pTo->GetPos();
		float x = posFrom.x - posTo.x;
		float y = posFrom.y - posTo.y ;
		fDistance = sqrt(x*x + y*y) / eGridSpanForObj;
	}
	else if(pTo->GetType() == eTT_Fighter)
	{
		fDistance = pFrom->GetFighterDistInGrid(pTo->GetFighter());
	}
	return (fDistance<=m_pMaxCastSkillDistance->GetDblValue(pFrom,uLevel-1) * fExtraDistRatio &&
		fDistance>=m_pMinCastSkillDistance->GetDblValue(pFrom,uLevel-1) * fExtraDistRatio);
}

void CNormalSkillServer::DoCoolDown(CFighterDictator* pFrom, uint8 uSkillLevel)const
{
	float fCoolDownTime = float( GetCoolDownTime()->GetDblValue(pFrom,uSkillLevel-1) );
	CCoolDownMgrServer*  pCDMgr=pFrom->GetCoolDownMgr();
	CSkillMgrServer* pSkillMgr=pFrom->GetSkillMgr();
	if(pFrom == NULL)
		return;
	if (m_eCoolDownType == eSCDT_FightSkill)
	{
		// 如是吟唱类技能，没有公共冷却时间
		pCDMgr->IntNewCoolDown(m_sName.c_str(),m_eCoolDownType,max(uint32(fCoolDownTime*1000), CCoolDownMgrServer::ms_uFightSkillCommonCDTime));
		pSkillMgr->OnCoolDownBegin(this->m_sName.c_str(), uSkillLevel, max(uint32(fCoolDownTime*1000), CCoolDownMgrServer::ms_uFightSkillCommonCDTime));
	}
	else
	{
		pCDMgr->IntNewCoolDown(m_sName.c_str(), m_eCoolDownType, uint32(fCoolDownTime*1000));
		pSkillMgr->OnCoolDownBegin(this->m_sName.c_str(), uSkillLevel, uint32(fCoolDownTime*1000));
	}
}

ECastingProcessType CNormalSkillServer::GetCastingProcessType()
{
	if (!GetCastingProcess())
	{
		return eCPT_Wink;
	}
	else
	{
		return GetCastingProcess()->GetCastingType();
	}
}


uint32 CNormalSkillServer::GetSkillLevelByLevel(uint8 uSkillLevel)const
{
	if (m_pSkillLevel2Level->GetTestString()=="")
		return 0;
	else if (uSkillLevel > m_pSkillLevel2Level->GetStringCount())
		return uint32(atoi(m_pSkillLevel2Level->GetString(m_pSkillLevel2Level->GetStringCount()).c_str()));
	else
		return uint32(atoi(m_pSkillLevel2Level->GetString(uSkillLevel-1).c_str()));
}

void CNormalSkillServer::StartOrRestartNADueToSkillFailure(CFighterMediator* pFrom)const
{
	if(pFrom->GetNormalAttackMgr()->IsAttacking())
	{
		//cout << "$$ skill restartNA" << endl;
		pFrom->RestartAttack(true);
		CFighterDictator* pTo = pFrom->GetTarget();
		if(pTo && GetStartNormalAttack() == eANAT_AbsolutelyStartAndAutoTrack)
			pFrom->TellDirectorToDoNA(pTo, true);
	}
	else
	{
		CFighterDictator* pTo = pFrom->GetTarget();
		if(pTo)
		{
			//cout << "## skill startNA" << endl;
			pFrom->TellDirectorToDoNA(pTo, GetStartNormalAttack() == eANAT_AbsolutelyStartAndAutoTrack);
		}
	}
}
