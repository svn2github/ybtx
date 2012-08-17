#include "stdafx.h"
#include "CSkillInstServer.h"
#include "CValueMagicOpServer.h"
#include "CFighterMediator.h"
#include "CTriggerEvent.h"
#include "CAllStateMgr.h"
#include "CTempVarServer.h"
#include "NormalAttack.h"
#include "CNormalAttackCfg.h"
#include "ICharacterMediatorCallbackHandler.h"
#include "CCfgCalcFunction.h"
#include "CActorServer.h"
#include "CSpecialStateMgr.h"
#include "CBattleStateManager.h"
#include "ErrLogHelper.h"
#include "CRelationMgrServer.h"
#include "TypeLimit.h"
#include "CCharacterMediator.h"
#include "NpcInfoMgr.h"
#include "CSkillMgrServer.h"
#include "CCfgArg.h"
#include "CEntityServerManager.h"

uint32 CValueMagicOpServer::Do(CSkillInstServer* pSkillInst,const CCfgArg* Arg,CFighterDictator* pFrom,CFighterDictator* pTo,int32 *MajorMopValue)
{
	if (!Arg)
	{
		return Do(pSkillInst,*ms_pMagicOpCalcArg->m_pArg,pFrom,pTo,MajorMopValue);
	}
	CMagicOpCalcArg* pMagicOpCalcArg = static_cast<CMagicOpCalcArg*>(const_cast<CCfgArg*>(Arg));
	return Do(pSkillInst,*pMagicOpCalcArg->m_pArg,pFrom,pTo,MajorMopValue);
}

int32 CValueMagicOpServer::MagicOpCalc(CSkillInstServer* pSkillInst,const CCfgArg* Arg,CFighterDictator* pFrom,CFighterDictator* pTo,CValueData *pValue,uint32* pExtraValue)
{
	CMagicOpCalcArg* pMagicOpCalcArg = static_cast<CMagicOpCalcArg*>(const_cast<CCfgArg*>(Arg));
	return MagicOpCalc(pSkillInst,*pMagicOpCalcArg->m_pArg,pFrom,pTo,pValue,pExtraValue);
}

void CValueCancelableMagicOp::Cancel(CSkillInstServer* pSkillInst,const CCfgArg* Arg,CFighterDictator* pFrom,CFighterDictator* pTo)
{
	CMagicOpCalcArg* pMagicOpCalcArg = static_cast<CMagicOpCalcArg*>(const_cast<CCfgArg*>(Arg));
	return Cancel(pSkillInst,*pMagicOpCalcArg->m_pArg,pFrom,pTo);
}


//	这个函数里面所有对CFighterDictator* pFrom的操作都要加判断，因为pFrom很有可能为NULL
void CValueMagicOpServer::MagicOpExec(CSkillInstServer* pSkillInst,CFighterDictator* pFrom,CFighterDictator* pTo,const CValueData& Value,EHurtResult eResult,uint32 uExtraValue)
{
	int32 iValue = Value.GetInt32();
	GetImmuneResultAtExec(pTo, eResult, pSkillInst, iValue);

	if(eResult == eHR_Immune)
	{
		iValue = 0;
		uExtraValue = 0;
	}

	int32 iOriginalValue = iValue;
	int32 iFinalValue= 0;  //最终的修改量（伤害或加血）
	CTempVarMgrServer* pTemeVarMgrFrom = pFrom?pFrom->GetTempVarMgr():NULL;
	CTempVarMgrServer* pTemeVarMgrTo = pTo->GetTempVarMgr();
	Ast(pTemeVarMgrTo);
	bool bPTOIsFirstEnterBattle = false;
	bool bFinishDuel(false);
	if (pSkillInst->GetValueScale()!=0)
	{
		iValue=int32(iValue*pSkillInst->GetValueScale());
	}
	if(iValue < 0)
	{
		if( pTo->GetAllStateMgr()->GetSpecialStateMgrServer()->ExistStateByType(eSST_DOTImmunity) &&
			pSkillInst->GetInterval()||
			pTemeVarMgrTo->m_bLockDecreaseHP)
		{
			iValue = 0;
			uExtraValue = 0;
			eResult = eHR_Immune;
		}

		if(pTemeVarMgrFrom)pTemeVarMgrFrom->SetDamage(-iValue);
		pTemeVarMgrTo->SetBeDamaged(-iValue);

		if (pTo->GetPet()&&pTo->GetTempVarMgr()->m_pLinkFighter)
		{
			int32 iDamage=pTemeVarMgrTo->GetBeDamaged();
			float fDamageShareProportion=pTemeVarMgrTo->m_fDamageShareProportion;
			int32 iDamageToMaster = int32(iDamage*fDamageShareProportion);
			CValueData DamageToPet = -(int32)(iDamage*max(0.0f,(1-fDamageShareProportion)));
			this->MagicOpExec(pSkillInst, pFrom, pTo->GetPet(), DamageToPet, eResult, 0);
			pTemeVarMgrTo->SetBeDamaged(iDamageToMaster);
			if(pTemeVarMgrFrom)pTemeVarMgrFrom->SetDamage(iDamageToMaster);
		}

		pTemeVarMgrTo->SetDamageChangeStateName("");
		if(!pSkillInst->GetInterval())
		{
			CTriggerEvent::MessageEvent(eTST_DamageChange,pFrom, pTo, eHR_HurtExptDOT, false, GetSkillType(pSkillInst), GetAttackType(pSkillInst));
		}

		CTriggerEvent::MessageEvent(eTST_DamageChange,pFrom, pTo, eHR_Hurt, false, GetSkillType(pSkillInst), GetAttackType(pSkillInst));

		//iFinalValue= (int32)pTo->m_HealthPoint.LimitGet() - pTemeVarMgrTo->GetBeDamaged() < 0 ? 
		//	-(int32)pTo->m_HealthPoint.LimitGet() : -pTemeVarMgrTo->GetBeDamaged();
		iFinalValue = -pTemeVarMgrTo->GetBeDamaged();

		if(iFinalValue < 0 && pSkillInst->GetType() == eIT_NormalAttackInst && pFrom)
		{
			CNormalAttackMgr* pAttackMgr = pFrom->GetNormalAttackMgr();
			CSingleHandNATick* pMainAttack = pAttackMgr?pAttackMgr ->GetMHNormalAttack():NULL;
			if(pMainAttack&&pMainAttack->IsShortNormalAttack())
			{
				CTriggerEvent::MessageEvent(eTST_DamageChange, pFrom, pTo, eSET_ShortNormalAttackDamage);

				//iFinalValue= (int32)pTo->m_HealthPoint.LimitGet() - pTemeVarMgrTo->GetBeDamaged() < 0 ? 
				//	-(int32)pTo->m_HealthPoint.LimitGet() : -	pTemeVarMgrTo->GetBeDamaged();
				iFinalValue = -pTemeVarMgrTo->GetBeDamaged();
				if(!pSkillInst->GetIsScopeTargetMop())
				{
					CTriggerEvent::MessageEvent(eTST_DamageChange, pFrom, pTo, eSET_ShortNormalAttackSingletonDamage);
					iFinalValue = -pTemeVarMgrTo->GetBeDamaged();
				}
			}
			else
			{
				CTriggerEvent::MessageEvent(eTST_DamageChange, pFrom, pTo, eSET_LongNormalAttackDamage);

				//iFinalValue= (int32)pTo->m_HealthPoint.LimitGet() - pTemeVarMgrTo->GetBeDamaged() < 0 ? 
				//	-(int32)pTo->m_HealthPoint.LimitGet() : -	pTemeVarMgrTo->GetBeDamaged();
				iFinalValue = -pTemeVarMgrTo->GetBeDamaged();
			}
		}

		if((int32)pTo->m_HealthPoint.LimitGet() + iFinalValue <= 0)
		{
			CTriggerEvent::MessageEvent(eTST_DamageChange, pFrom, pTo, eSET_DeadlyHurt);

			//这个eTST_Trigger很特殊，因为需要依赖于前一个eTST_DamageChange的未变更伤害数值，因此中间不能插入其它伤害
			CTriggerEvent::MessageEvent(eTST_Trigger, pFrom, pTo, eSET_DeadlyHurt);

			//iFinalValue= (int32)pTo->m_HealthPoint.LimitGet() - pTemeVarMgrTo->GetBeDamaged() < 0 ? 
			//	-(int32)pTo->m_HealthPoint.LimitGet() : -	pTemeVarMgrTo->GetBeDamaged();
			iFinalValue = -pTemeVarMgrTo->GetBeDamaged();

			if((int32)pTo->m_HealthPoint.LimitGet() + iFinalValue <= 0)
			{
				CCharacterMediator* pCharTo = pTo->GetCharacter()->CastToCharacterMediator();
				if (pTo->GetCtrlState(eFCS_InDuel) && pCharTo)
				{
					CCharacterMediator* pCharFrom = pFrom->GetCharacter()->CastToCharacterMediator();

					if (pCharFrom)
					{
						if (pCharFrom != pCharTo)
						{
							pCharTo->OnFinishDuel(false);
							pTo->GetBattleStateMgr()->LeaveBattleStateByForce();

							pCharFrom->OnFinishDuel(true);
							pFrom->GetBattleStateMgr()->LeaveBattleStateByForce();
						}
						else
						{
							CCharacterMediator* pDuelFrom = pCharTo->GetDuelTarget();
							pDuelFrom->OnFinishDuel(true);
							pDuelFrom->GetFighter()->GetBattleStateMgr()->LeaveBattleStateByForce();

							pCharTo->OnFinishDuel(false);
							pTo->GetBattleStateMgr()->LeaveBattleStateByForce();
						}
					}
					else
					{
						pCharTo->OnFinishDuel(false);
						pTo->GetBattleStateMgr()->LeaveBattleStateByForce();

						CCharacterDictator* pMaster = pFrom->GetCharacter()->GetMaster();
						pCharFrom = pMaster ? pMaster->CastToCharacterMediator() : NULL;
						if (pCharFrom)
						{
							pCharFrom->OnFinishDuel(true);
							pCharFrom->GetFighter()->GetBattleStateMgr()->LeaveBattleStateByForce();
						}
					}

					iFinalValue = 1 - pTo->m_HealthPoint.LimitGet();
					bFinishDuel = true;
				}
			}
		}

		uint32 uCheckValue = iValue*(-1);
		CheckUnsignedUnderFlow(uCheckValue);
	}
	else if(iValue > 0)
	{
		if (pTemeVarMgrTo->m_bLockIncreaseHP)
		{
			iValue=0;
		}
		if(pTemeVarMgrFrom)
		{
			pTemeVarMgrFrom->SetVarValue("治疗",iValue);
			pTemeVarMgrFrom->SetVarValue("未变更治疗",iValue);
		}
		pTemeVarMgrTo->SetVarValue("被治疗",iValue);
		pTemeVarMgrTo->SetVarValue("被未变更治疗",iValue);

		if (eResult == eHR_SuckBlood)		// 其他代码也应该区分治疗和吸血
		{
			CTriggerEvent::MessageEvent(eTST_DamageChange, pFrom, pTo, eSET_SuckBlood);
			eResult = eHR_Hit;
		}
		else
			CTriggerEvent::MessageEvent(eTST_DamageChange, pFrom, pTo, eSET_Heal);
		iFinalValue = pTemeVarMgrTo->GetVarValue("被治疗");

		CFighterMediator* pFromMediator = pFrom ? pFrom->CastToFighterMediator() : NULL;
		CFighterMediator* pToMediator	= pTo->CastToFighterMediator();
		bool bIsDOT = pSkillInst->GetInterval();
		if (!bIsDOT && pFrom && pFromMediator)
		{
			if (pTo->GetCtrlState(eFCS_InBattle))
				pTo->Notify(eCE_EntryEnmity,reinterpret_cast<void*>(pFrom));

			if (pToMediator)
			{
				if (pFrom->GetCtrlState(eFCS_InBattle) && !pTo->GetCtrlState(eFCS_InBattle))
					bPTOIsFirstEnterBattle = pTo->GetBattleStateMgr()->EnterBattleStateByPlayer();

				if (!pFrom->GetCtrlState(eFCS_InBattle) && pTo->GetCtrlState(eFCS_InBattle))
					pFrom->GetBattleStateMgr()->EnterBattleStateByPlayer();
			}
		}
		uint32 uCheckValue = iValue;
		CheckUnsignedUnderFlow(uCheckValue);
		//iFinalValue = pTo->m_HealthPoint.LimitGet() + pTemeVarMgrTo->GetVarValue("被治疗") > pTo->m_HealthPoint.Get(pTo) ? 
		//	pTo->m_HealthPoint.Get(pTo) - pTo->m_HealthPoint.LimitGet() : pTemeVarMgrTo->GetVarValue("被治疗");
	}
	else		// iValue == 0
	{
		if(pTemeVarMgrFrom)pTemeVarMgrFrom->SetDamage(-iValue);
		pTemeVarMgrTo->SetBeDamaged(-iValue);
	}

	if (!bFinishDuel)
	{
		CRelationMgrServer& relationMgr = CRelationMgrServer::Inst();
		if (iValue < 0 || (iValue == 0 && pFrom && relationMgr.IsEnemy(pFrom, pTo)))
		{
			CFighterMediator* pFromMediator = pFrom ? pFrom->CastToFighterMediator() : NULL;
			CFighterMediator* pToMediator	= pTo->CastToFighterMediator();
			if (!pSkillInst->GetInterval() && pToMediator && pFromMediator)
			{	
				if(pFrom->GetEntityID() != pTo->GetEntityID())
				{
					bPTOIsFirstEnterBattle = pTo->GetBattleStateMgr()->EnterBattleStateByPlayer();
					pFrom->GetBattleStateMgr()->EnterBattleStateByPlayer();
				}
				else if(pFrom == NULL)
				{
					bPTOIsFirstEnterBattle = pTo->GetBattleStateMgr()->EnterBattleStateByPlayer();
				}
			}
			if(pFrom && pFrom != pTo)
			{
				if (!pFrom->GetCtrlState(eFCS_FeignDeath))
				{
					pTo->Notify(eCE_BeAttacked,reinterpret_cast<void*>(pFrom));
					pFrom->Notify(eCE_Attack, reinterpret_cast<void*>(pTo));
					if (!pSkillInst->GetInterval())
						pTo->Notify(eCE_BeAttackedFromNotDot,reinterpret_cast<void*>(pFrom));
				}
				if (pToMediator && pFrom->GetCharacter() && pFrom->GetCharacter()->CanBeSelected())
					pTo->SetEnmityToTarget(pFrom);
			}
			if (!pSkillInst->GetInterval())
			{
				if (pToMediator && !pFromMediator)
					pToMediator->OnBeAttacked();
			}
		}

		if (iFinalValue < 0)
		{
			if (pFrom)
				pTo->Notify(eCE_BeHurtFirstTime, reinterpret_cast<void*>(pFrom));

			CFighterMediator* pMediator = pTo->CastToFighterMediator();
			if (pMediator && !pSkillInst->GetInterval())
			{
				CCharacterMediator* pCharTo = pTo->GetCharacter()->CastToCharacterMediator();
				if (pCharTo && pCharTo->IsInGatherProcess() && pMediator->GetCallBackHandler())
					pMediator->GetCallBackHandler()->OnBreakGatherProcess(pMediator->GetEntityID());
			}
		}
	}

	int32 CurHP = pTo->m_HealthPoint.LimitGet();
	int32 nMaxHP = int32(pTo->m_HealthPoint.Get(pTo));
	uint32 uRealChangeHP = iFinalValue>0?iFinalValue:(0-iFinalValue);
	if (iFinalValue < 0 && (CurHP+iFinalValue)<0 )
	{
		uRealChangeHP = CurHP;
	}
	else if (iFinalValue > 0 && (CurHP+iFinalValue) > nMaxHP)
	{
		uRealChangeHP = nMaxHP - CurHP;
	}

	if (pSkillInst && pSkillInst->GetIsArea())
		pSkillInst->AddEffectHPChangeValue(uRealChangeHP);
	if(!pTo->PrintInfoIsOff())
	{
		PrintFightInfo( pSkillInst, pFrom, pTo, iFinalValue, iValue, eResult, uExtraValue, uRealChangeHP );
	}

	uint32 uPreHP = pTo->m_HealthPoint.LimitGet();
	pTo->m_HealthPoint.LimitSet(pTo->m_HealthPoint.LimitGet() + iFinalValue, pTo);
	uint32 uCurHP = pTo->m_HealthPoint.LimitGet();
	if (pTo->m_HealthPoint.LimitGet() <= 0 && CurHP != 0)
	{
		if (pTo->CastToFighterMediator())
			pTo->CastToFighterMediator()->CppDie(pFrom);
		else
			pTo->CppDie(pFrom,pSkillInst->GetSkillName().c_str());
	}

	HurtResultToActionState( pSkillInst, pFrom, pTo, iValue, eResult, bPTOIsFirstEnterBattle);
	// 如是决斗中最后伤害，不要触发伤害事件，有可能会又安装DOT伤害，以致造成死亡
	if (!bFinishDuel)
		NotifyMessageEvent( pSkillInst, pFrom, pTo, iOriginalValue, iFinalValue, eResult, uExtraValue );

	if(pFrom && pFrom->CastToFighterMediator()&&uPreHP!=uCurHP)
		pFrom->CastToFighterMediator()->ChangeTargetHP(iFinalValue);
}

float CValueMagicOpServer::GetLevelPressRate(CFighterDictator* pFrom, CFighterDictator* pTo)
{
	if (!pTo)
		return 1.0f;
	if (!pFrom->GetLevelPress() || !pTo->GetLevelPress())
		return 1.0f;
	if (!pFrom->GetCharacter())
		return 1.0f;
	int32 iLevelDist = pFrom->CppGetLevel() - pTo->CppGetLevel();
	if (abs(iLevelDist) > 10)
	{
		bool bFromIsMonster = !pFrom->CastToFighterMediator() && !NpcInfoMgr::BeServantType(pFrom->GetCharacter()->GetNpcType());
		bool bToIsMonster = !pTo->CastToFighterMediator() && !NpcInfoMgr::BeServantType(pTo->GetCharacter()->GetNpcType());
		if (bFromIsMonster && !bToIsMonster)
		{
			if (iLevelDist <= 10)
				return 1.0f;
			else if (10 < iLevelDist && iLevelDist <= 20)
				return 2.0f;
			else if (20 < iLevelDist && iLevelDist <=30)
				return 6.0f;	
			else if (30 < iLevelDist)
				return 10.0f;
		}
		else if (!bFromIsMonster && bToIsMonster)
		{
			iLevelDist = pTo->CppGetLevel() - pFrom->CppGetLevel();
			if (iLevelDist <= 10)
				return 1.0f;
			else if (10 < iLevelDist && iLevelDist <= 20)
				return 0.8f;
			else if (20 < iLevelDist && iLevelDist <=30)
				return 0.6f;	
			else if (30 < iLevelDist)
				return 0.4f;
		}
	}
	return 1.0f;
}

void CValueMagicOpServer::PrintFightInfo(CSkillInstServer* pSkillInst,CFighterDictator* pFrom,CFighterDictator* pTo,int32 iFinalValue,int32 iValue,EHurtResult eResult,uint32 uExtraValue,uint32 uRealChangeValue)
{
	CCharacterDictator* pCreatorChar = CCharacterDictator::GetCharacterByID(pSkillInst->GetCreatorID());
	CFighterDictator* pCreatorFighter = pCreatorChar?pCreatorChar->GetFighter():NULL;
	bool bIsSkill = pSkillInst->GetType()==eIT_NormalAttackInst ? false: true;
	bool bIsMagic = pSkillInst->GetIsMagic();
	bool bIsArea = pSkillInst->GetIsArea();
	if (bIsMagic)
	{
			pTo->GetSkillMgr()->DisplayFightInfo(pCreatorFighter,pFrom,iFinalValue,eResult,bIsSkill);
	}
	else
	{
		pTo->GetSkillMgr()->DisplayFightInfo(pCreatorFighter,pFrom,iFinalValue,eResult,pSkillInst->GetInterval(),bIsSkill,bIsArea);
	}
	switch(eResult)
	{
	case eHR_Hit:
		{
			if (iFinalValue != iValue && iFinalValue <= 0 && iValue <= iFinalValue)
			{
				CTempVarMgrServer* pTempVarMgrTo = pTo?pTo->GetTempVarMgr():NULL;
				const TCHAR* szStateName = pTempVarMgrTo?pTempVarMgrTo->GetDamageChangeStateName().c_str():"";
				uint16 uStateNameLen = pTempVarMgrTo?pTempVarMgrTo->GetDamageChangeStateName().size():0;
				if (strcmp("*$",szStateName) == 0)
				{
					szStateName = "";
				}
				OnPrintFightInfo(EFII_StateEffectHit, pSkillInst, pFrom, pTo, iFinalValue, -iValue+iFinalValue, uRealChangeValue, szStateName, uStateNameLen);
			}
			else
			{
				if (iFinalValue>0)
				{
					OnPrintFightInfo(EFII_Heal, pSkillInst, pFrom, pTo, iFinalValue, uExtraValue, uRealChangeValue);
				}
				else if(iFinalValue<0)
				{
					EFightInfoIndex eFightInfoIndex = IsNonTypeAttack() ? EFII_NonTypeAttackHit : EFII_Hurt;
					OnPrintFightInfo(eFightInfoIndex, pSkillInst, pFrom, pTo, iFinalValue, uExtraValue, uRealChangeValue);
				}
				else
				{
					if(iValue>0)
					{
						OnPrintFightInfo(EFII_Heal, pSkillInst, pFrom, pTo, iFinalValue, uExtraValue, uRealChangeValue);
					}
					else
					{
						EFightInfoIndex eFightInfoIndex = IsNonTypeAttack() ? EFII_NonTypeAttackHit : EFII_Hurt;
						OnPrintFightInfo(eFightInfoIndex, pSkillInst, pFrom, pTo, iFinalValue, uExtraValue, uRealChangeValue);
					}
				}
			}
		}
		break;
	case eHR_Strike:
		{
			if (iFinalValue != iValue && iFinalValue <= 0 && iValue <= iFinalValue)
			{
				CTempVarMgrServer* pTempVarMgrTo = pTo?pTo->GetTempVarMgr():NULL;
				const TCHAR* szStateName = pTempVarMgrTo?pTempVarMgrTo->GetDamageChangeStateName().c_str():"";
				uint16 uStateNameLen = pTempVarMgrTo?pTempVarMgrTo->GetDamageChangeStateName().size():0;
				if (strcmp("*$",szStateName) == 0)
				{
					szStateName = "";
				}
				OnPrintFightInfo(EFII_StateEffectStrikeHit, pSkillInst, pFrom, pTo, iFinalValue, -iValue+iFinalValue, uRealChangeValue, szStateName, uStateNameLen);
			}
			else
			{
				if (iFinalValue>0)
				{
					OnPrintFightInfo(EFII_StrikeHeal, pSkillInst, pFrom, pTo, iFinalValue, uExtraValue, uRealChangeValue);
				}
				else if(iFinalValue<0)
				{
					OnPrintFightInfo(EFII_StrikeHurt, pSkillInst, pFrom, pTo, iFinalValue, uExtraValue, uRealChangeValue);
				}
				else
				{
					if(iValue>0)
					{
						OnPrintFightInfo(EFII_Heal, pSkillInst, pFrom, pTo, iFinalValue, uExtraValue, uRealChangeValue);
					}
					else
					{
						OnPrintFightInfo(EFII_Hurt, pSkillInst, pFrom, pTo, iFinalValue, uExtraValue, uRealChangeValue);
					}
				}
			}
		}
		break;
	case eHR_Miss:
		{
			OnPrintFightInfo(EFII_Miss, pSkillInst, pFrom, pTo, iFinalValue, uExtraValue, uRealChangeValue);
		}
		break;
	case eHR_PhyDodge:
	case eHR_MagDodge:
		{
			OnPrintFightInfo(EFII_Dodge, pSkillInst, pFrom, pTo, iFinalValue, uExtraValue, uRealChangeValue);
		}
		break;
	case eHR_Immune:
		{
			OnPrintFightInfo(EFII_Immune, pSkillInst, pFrom, pTo, iFinalValue, uExtraValue, uRealChangeValue);
		}
		break;
	case eHR_Parry:
		{
			OnPrintFightInfo(EFII_Parry, pSkillInst, pFrom, pTo, iFinalValue, uExtraValue, uRealChangeValue);
		}
		break;
	case eHR_Block:
		{
			if (iFinalValue != iValue && iFinalValue <= 0 && iValue <= iFinalValue)
			{
				CTempVarMgrServer* pTempVarMgrTo = pTo?pTo->GetTempVarMgr():NULL;
				const TCHAR* szStateName = pTempVarMgrTo?pTempVarMgrTo->GetDamageChangeStateName().c_str():"";
				uint16 uStateNameLen = pTempVarMgrTo?pTempVarMgrTo->GetDamageChangeStateName().size():0;
				if (strcmp("*$",szStateName) == 0)
				{
					szStateName = "";
				}
				OnPrintFightInfo(EFII_MultiAbsorb, pSkillInst, pFrom, pTo, iFinalValue, uExtraValue-iValue+iFinalValue, uRealChangeValue, szStateName, uStateNameLen);
			}
			else
			{
				OnPrintFightInfo(EFII_Block, pSkillInst, pFrom, pTo, iFinalValue, uExtraValue, uRealChangeValue);
			}
		}
		break;			
	case eHR_ComResist:
		{
			OnPrintFightInfo(EFII_ComResist, pSkillInst, pFrom, pTo, iFinalValue, uExtraValue, uRealChangeValue);
		}
		break;
	case eHR_Resist:
		{
			if (iFinalValue != iValue && iFinalValue <= 0 && iValue <= iFinalValue)
			{
				CTempVarMgrServer* pTempVarMgrTo = pTo?pTo->GetTempVarMgr():NULL;
				const TCHAR* szStateName = pTempVarMgrTo?pTempVarMgrTo->GetDamageChangeStateName().c_str():"";
				uint16 uStateNameLen = pTempVarMgrTo?pTempVarMgrTo->GetDamageChangeStateName().size():0;
				if (strcmp("*$",szStateName) == 0)
				{
					szStateName = "";
				}
				OnPrintFightInfo(EFII_MultiAbsorb, pSkillInst, pFrom, pTo, iFinalValue, uExtraValue-iValue+iFinalValue, uRealChangeValue, szStateName, uStateNameLen);
			}
			else
			{
				OnPrintFightInfo(EFII_Resist, pSkillInst, pFrom, pTo, iFinalValue, uExtraValue, uRealChangeValue);
			}
		}
		break;
	default:
		{
		}
		break;
	}
}

void CValueMagicOpServer::HurtResultToActionState( CSkillInstServer* pSkillInst, CFighterDictator* pFrom, CFighterDictator* pTo, 
												  int32 iValue, EHurtResult eResult, bool bPTOIsFirstEnterBattle)
{
	CCharacterDictator* pCharacter = pTo->GetCharacter();
	if(!pCharacter)
		return;

	if (!pTo->CppIsAlive() || iValue >= 0)
		return;

	EActionState eActionStateTo;
	switch(eResult)
	{
	case eHR_Hit:
	case eHR_Strike:
	case eHR_Resist:
		eActionStateTo = eAS_Suffer_Back;
		break;
	case eHR_Miss:
	case eHR_PhyDodge:
	case eHR_MagDodge:
	case eHR_Immune:
	case eHR_ComResist:
		eActionStateTo = eAS_Dodge;
		break;
	case eHR_Parry:
		eActionStateTo = eAS_Parry;
		break;
	case eHR_Block:
		eActionStateTo = eAS_Block;
		break;
	default:
		eActionStateTo = eAS_Suffer_Back;
		break;
	}

	bool bPFromIsIntFighterObject = false;
	if (pFrom)
	{
		CEntityServer* pEntity = pFrom->GetEntity();
		if ( pEntity && pEntity->GetGameEntityType() == eGET_IntObject)
			bPFromIsIntFighterObject = true;
	}

	// 暂时注释掉，不区分suffer01~03只播01
	//if (pSkillInst->GetSkillType() == eST_Physical)
	//{
	//	if (eActionStateTo == eAS_Suffer_Back)
	//	{
	//		//计算pFrom在pTo的方位
	//		bool bPToTurnDir = false;
	//		if (pFrom)
	//		{
	//			CFPos posFrom, posTo;
	//			posFrom = pFrom->GetPixelPos();
	//			posTo = pTo->GetPixelPos();	
	//			CDir dirTo;
	//			CVector2f vecDir;
	//			vecDir.x = posFrom.x - posTo.x;
	//			vecDir.y = posFrom.y - posTo.y;
	//			dirTo.Set(vecDir);
	//			uint8 uToCurDir = pToActor->GetActionDir();
	//			if (((abs(dirTo.uDir - uToCurDir)) <= 64) || (abs(dirTo.uDir - uToCurDir)) >= 192)
	//				bPToTurnDir = true;
	//		}
	//		//当pFrom在pTo面朝方向90°夹角内，受击动作对应三种攻击动作			
	//		uint32 uIndex;
	//		if(pSkillInst->GetType() == eIT_NormalAttackInst && bPToTurnDir)
	//		{
	//			uIndex = class_cast<CNAInstServer*>(pSkillInst)->GetAniIndex();
	//			eActionStateTo = EActionState(uIndex + (eAS_Suffer_Back - eAS_Attack_All));
	//		}
	//		else	//其余都播向后
	//			eActionStateTo = eAS_Suffer_Back;
	//	}
	//}
	if (pSkillInst->GetInterval())
		return;

	// 临时：因JT动作包动作不全
	if (pTo->GetCtrlState(eFCS_InNormalHorse) && (eActionStateTo == eAS_Dodge || eActionStateTo == eAS_Parry || eActionStateTo == eAS_Block))
		return; 

	pCharacter->SetAndSyncActionState(eActionStateTo);
}

void CValueMagicOpServer::OnPrintFightInfo(EFightInfoIndex eFightInfoIndex, CSkillInstServer* pSkillInst, CFighterDictator* pFrom,CFighterDictator* pTo,int32 iValue,uint32 uExtraValue,uint32 uRealChangeValue,const TCHAR* szArgName,uint16 uArgLen)
{
	EValueMagicOpType eValueMagicOpType = this->GetValueMagicOpType();
	EAttackType eAttackType = eATT_None;
	switch(eValueMagicOpType)
	{
	case eVMOT_Nature:
		eAttackType= eATT_Nature;
		break;
	case eVMOT_Destruction:
		eAttackType= eATT_Destroy;
		break;
	case eVMOT_Evil:
		eAttackType= eATT_Evil;
		break;
	case eVMOT_Normal:
	case eVMOT_Physical:
		eAttackType= pSkillInst->GetAttackType();
		break;
	case eVMOT_TaskNonBattle:
		eAttackType= eATT_TaskNonBattle;
		break;
	case eVMOT_TaskBomb:
		eAttackType= eATT_TaskBomb;
		break;
	case eVMOT_TaskSpecial:
		eAttackType= eATT_TaskSpecial;
		break;
	default:
		break;
	}
	uint32 uStateId = 0;
	if (pSkillInst->GetType() == eIT_MagicStateInst || pSkillInst->GetType() == eIT_StateInst)
	{
		uStateId = pSkillInst->GetStateId();
	}
	pTo->OnPrintFightInfo(eFightInfoIndex, GetFighterEntityID(pFrom), pTo->GetEntityID(), pSkillInst, iValue, eAttackType, uExtraValue, uRealChangeValue, false, uStateId, szArgName, uArgLen);
	StatisticFightHPChange(eFightInfoIndex,pSkillInst,pFrom,pTo,uRealChangeValue);
}

void CValueMagicOpServer::StatisticFightHPChange(EFightInfoIndex eFightInfoIndex, CSkillInstServer* pSkillInst, CFighterDictator* pFrom, CFighterDictator* pTo, uint32 uRealChangeValue)
{
	uint32 uObjID = GetFighterEntityID(pFrom);
	if (pSkillInst)
	{
		if (pSkillInst->GetTrigger() && pSkillInst->GetCreatorID() != uObjID)
		{
			uObjID = 0;
		}
		else
		{
			uObjID = pSkillInst->GetCreatorID();
		}
	}
	CCharacterDictator* pCharacter = NULL;
	CEntityServer* pFormEntity = CEntityServerManager::GetEntityByID(uObjID);
	if (pFormEntity)
	{
		switch(pFormEntity->GetGameEntityType())
		{
		case eGET_CharacterMediator:
			{
				pCharacter=class_cast<CCharacterMediator*>(pFormEntity);
				if (pCharacter->GetFighter()->CastToFighterMediator())
				{
					pCharacter->GetFighter()->CastToFighterMediator()->OnFightHurt(eFightInfoIndex, uRealChangeValue);
				}
				break;
			}
		case eGET_CharacterDictator:
			{
				CCharacterDictator* pCharacter=class_cast<CCharacterDictator*>(pFormEntity);
				pCharacter = pCharacter?pCharacter->GetMaster():NULL;
				if (pCharacter && pCharacter->GetFighter() && pCharacter->GetFighter()->CastToFighterMediator())
				{
					pCharacter->GetFighter()->CastToFighterMediator()->OnFightHurt(eFightInfoIndex, uRealChangeValue);
				}
				break;
			}
		default:
			break;
		}
	}

	CEntityServer* pToEntity = pTo->GetEntity();
	if (pToEntity)
	{
		switch(pToEntity->GetGameEntityType())
		{
		case eGET_CharacterMediator:
			{
				pCharacter=class_cast<CCharacterMediator*>(pToEntity);
				if (pCharacter->GetFighter()->CastToFighterMediator())
				{
					pCharacter->GetFighter()->CastToFighterMediator()->OnBeSubHp(eFightInfoIndex, uRealChangeValue);
				}
				break;
			}
		default:
			break;
		}
	}
}

void CValueMagicOpServer::NotifyMessageEvent(CSkillInstServer* pSkillInst,CFighterDictator* pFrom,CFighterDictator* pTo,int32 iValue,int32 iFinalValue,EHurtResult eResult,uint32 uExtraValue)
{
	CNormalAttackMgr* pAttackMgr = pFrom? pFrom->GetNormalAttackMgr():NULL;

	if (iValue < 0)
	{
		CTriggerEvent::MessageEvent(eTST_Trigger, pFrom, pTo, eSET_BeforeChangeHurt, false, GetSkillType(pSkillInst), GetAttackType(pSkillInst));
	}
	else if (iValue > 0)
	{
		CTriggerEvent::MessageEvent(eTST_Trigger, pFrom, pTo, eSET_BeforeChangeHeal);
	}
	if(iFinalValue < 0)
	{
		if(!pSkillInst->GetInterval())
		{
			CTriggerEvent::MessageEvent(eTST_Trigger,pFrom, pTo, eHR_HurtExptDOT, false, GetSkillType(pSkillInst), GetAttackType(pSkillInst));
		}
		CTriggerEvent::MessageEvent(eTST_Trigger,pFrom, pTo, eHR_Hurt, false, GetSkillType(pSkillInst), GetAttackType(pSkillInst));
		if(pSkillInst->GetType() == eIT_NormalAttackInst && pFrom && pAttackMgr)
		{
			CSingleHandNATick* pMainAttack= pAttackMgr->GetMHNormalAttack();
			if(pMainAttack&&pMainAttack->IsShortNormalAttack())
			{
				CTriggerEvent::MessageEvent(eTST_Trigger, pFrom, pTo, eSET_ShortNormalAttackDamage);
				if(!pSkillInst->GetIsScopeTargetMop())
				{
					CTriggerEvent::MessageEvent(eTST_Trigger, pFrom, pTo, eSET_ShortNormalAttackSingletonDamage);
				}
			}
			else
			{
				CTriggerEvent::MessageEvent(eTST_Trigger, pFrom, pTo, eSET_LongNormalAttackDamage);
			}
		}
	}
	else if(iFinalValue > 0)
	{
		CTriggerEvent::MessageEvent(eTST_Trigger, pFrom, pTo, eSET_Heal);
	}
	else if(iFinalValue == 0 && iValue < 0) // 伤害被全部吸收
	{
		if(!pSkillInst->GetInterval())
		{
			CTriggerEvent::MessageEvent(eTST_Trigger,pFrom, pTo, eHR_HurtExptDOT, false, GetSkillType(pSkillInst), GetAttackType(pSkillInst));
		}
		return;
	}
	if(pFrom &&pAttackMgr)
	{
		if(iValue < 0)
		{
			CSingleHandNATick* pMainAttack= pAttackMgr->GetMHNormalAttack();
			if(pMainAttack && pSkillInst->GetType() == eIT_NormalAttackInst)
			{
				if(pMainAttack->GetWeaponType() <eWT_Bow)
				{
					CTriggerEvent::MessageEvent(eTST_Trigger, pFrom, pTo, eSET_ShortNormalAttack);
				}
				if(eHR_Strike == eResult)
				{
					CTriggerEvent::MessageEvent(eTST_Trigger, pFrom, pTo, eSET_NormalAttackStrike);
				}
			}
			CTriggerEvent::MessageEvent(eTST_Trigger, pFrom, pTo, eSET_Attack, false, GetSkillType(pSkillInst), GetAttackType(pSkillInst));
		}
		else if(iValue == 0 && (eResult >= eHR_Hit || eResult <= eHR_Hurt) && eResult != eHR_Immune)
		{
			CTriggerEvent::MessageEvent(eTST_Trigger, pFrom, pTo, eSET_Attack, false, GetSkillType(pSkillInst), GetAttackType(pSkillInst));
		}
	}
	bool bArg2 = (eResult >= eHR_PhyDodge && eResult <= eHR_Block)? true: false;
	CTriggerEvent::MessageEvent(eTST_Trigger, pFrom, pTo, eResult, bArg2, GetSkillType(pSkillInst), GetAttackType(pSkillInst));
	if(pTo->m_HealthPoint.LimitGet() == 0)
	{
		uint32 uFromID = pFrom ? pFrom->GetEntityID() : 0;
		uint32 uToID = pTo ? pTo->GetEntityID() : 0;
		CTriggerEvent::MessageEvent(eTST_Trigger, uFromID, uToID, eSET_Killed);
		if (pTo->CastToFighterMediator())
		{
			CTriggerEvent::MessageEvent(eTST_Trigger, uFromID, uToID, eSET_KillPlayer);
		}
		else
		{
			CTriggerEvent::MessageEvent(eTST_Trigger, uFromID, uToID, eSET_KillNPC);
		}
	}	
}

bool CValueMagicOpServer::CalcNeedTarget(CCfgArg* pArg) const
{
	CMagicOpCalcArg* pMagicOpCalcArg = static_cast<CMagicOpCalcArg*>(const_cast<CCfgArg*>(pArg));
	return pMagicOpCalcArg->m_pArg->CalcNeedTarget();
}

EAttackType CValueMagicOpServer::GetAttackType(CSkillInstServer* pSkillInst)
{
	return pSkillInst->GetAttackType();
}

ESkillType CValueMagicOpServer::GetSkillType(CSkillInstServer* pSkillInst)
{
	return pSkillInst->GetSkillType();
}
