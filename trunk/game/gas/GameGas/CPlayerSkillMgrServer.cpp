#include "stdafx.h"
#include "CPlayerSkillMgrServer.h"
#include "CFighterMediator.h"
#include "CSkillServer.h"
#include "CCastingProcess.h"
#include "CSkillInstServer.h"
#include "ICharacterMediatorCallbackHandler.h"
#include "CDir.h"
#include "CCharacterMediator.h"
#include "CGenericTarget.h"
#include "CRelationMgrServer.h"
#include "NormalAttack.h"
#include "CoreCommon.h"
#include "TSqrAllocator.inl"

#define MAXTEMPSKILLCOUNT	6

CPlayerSkillMgrServer::CPlayerSkillMgrServer(CFighterMediator* pFighter)
:CSkillMgrServer(pFighter)
{
	m_vecTempSkill.resize(MAXTEMPSKILLCOUNT+1);
	for(size_t i = 0; i <= MAXTEMPSKILLCOUNT; ++i)
	{
		m_vecTempSkill[i] = NULL;
	}
}


CPlayerSkillMgrServer::~CPlayerSkillMgrServer(void)
{
	VecTempSkill::iterator it = m_vecTempSkill.begin();
	for (; it != m_vecTempSkill.end(); ++it)
	{
		if(*it)	(*it)->Cancel();
	}
}

EDoSkillResult CPlayerSkillMgrServer::DoPlayerSkill(const string& szName, uint8 uSkillLevel, uint8 uDir, uint8 uMagicDir)
{
	const CNormalSkillServerSharedPtr& pSkill = CNormalSkillServer::GetSkillByName(szName);	
	if(!pSkill)
		return eDSR_NoSkill;
	return DoPlayerSkill(pSkill, uSkillLevel, uDir, uMagicDir);
}


EDoSkillResult CPlayerSkillMgrServer::InitTarget(CGenericTarget& target, const CNormalSkillServerSharedPtr& pSkill, uint8 uSkillLevel)const
{
	SQR_TRY
	{
		switch(pSkill->GetSelectTargetType())
		{
		case eFSTT_EnemyObject:
		case eFSTT_FriendObject:
		case eFSTT_NeutralObject:
		case eFSTT_FriendAndEnemy:
		case eFSTT_SelectFriendObject:
			{
				CFighterDictator* pTargetObject = GetFighter()->GetTarget();
				if( pTargetObject == NULL)
					return eDSR_TargetNotExist;

				target.SetFighter(pTargetObject);
				break;
			}	
		case eFSTT_FriendandSelf:
			{
				CFighterDictator* pTargetObject = GetFighter()->GetTarget();
				if( pTargetObject == NULL)
				{
					target.SetFighter(GetFighter());
					break;
				}
				CRelationMgrServer& relationMgr = CRelationMgrServer::Inst();
				if (!relationMgr.IsFriend(GetFighter(), pTargetObject))//如果目标错误，切换目标为自身
				{
					target.SetFighter(GetFighter());
					if (relationMgr.GetRelationType(GetFighter(), pTargetObject) == eRT_FriendInRivalry)
						OnMagicCondFail((uint32)eDSR_CanNotAssist, pSkill.get()->GetName().c_str());
					break;
				}

				target.SetFighter(pTargetObject);
				break;
			}
		case eFSTT_AllObject:
		case eFSTT_NotNeutral:
		case eFSTT_NotFriendObject:
		case eFSTT_TeammatesOrEnemy:
			{
				CFighterDictator* pTargetObject = GetFighter()->GetTarget();
				if( pTargetObject == NULL)
				{
					target.SetFighter(GetFighter());
				}
				else
				{
					target.SetFighter(pTargetObject);
				}
				break;
			}
		case eFSTT_ObjectPosition:
			{			
				CFighterDictator* pTargetObject = GetFighter()->GetTarget();
				if( pTargetObject == NULL)
					return eDSR_TargetNotExist;

				CFPos posTarget;
				pTargetObject->GetPixelPos(posTarget);
				target.SetPos(posTarget);
				break;
			}		
		case eFSTT_Self:
			{
				target.SetFighter(GetFighter());
				break;
			}		
		case eFSTT_Position:
			{
				target.SetPos(GetFighter()->GetTargetPos());
				break;
			}			
		case eFSTT_SelfPosition:
			{
				CFPos posSelf;
				posSelf=GetFighterMediator()->GetFighterPos();
				target.SetPos(posSelf);
				break;
			}
		case eFSTT_SelfDirection:
			{
				CFPos pos;
				CFPos posSelf;
				GetFighter()->GetPixelPos(posSelf);
				CCharacterDictator* pCharacter = GetFighter()->GetCharacter();
				CDir dir = pCharacter?pCharacter->GetActionDir():0;
				CVector2f vector2Dir;
				dir.Get(vector2Dir);
				pos.x = vector2Dir.x * pSkill->GetDistance(GetFighter(), uSkillLevel)*eGridSpanForObj + posSelf.x;
				pos.y = vector2Dir.y * pSkill->GetDistance(GetFighter(), uSkillLevel)*eGridSpanForObj + posSelf.y;
				target.SetPos(pos);
				break;
			}
		case eFSTT_PositionOrFriend:
			{
				CFPos pos;
				CFighterDictator* pTargetObject = GetFighter()->GetTarget();
				if (pTargetObject)
				{
					CRelationMgrServer& relationMgr = CRelationMgrServer::Inst();
					if (relationMgr.IsEnemy(GetFighter(), pTargetObject))
					{
						pos=GetFighter()->GetTargetPos() ;
					}
					else
					{
						pTargetObject->GetPixelPos(pos);
					}
				}
				else
				{
					pos = GetFighter()->GetTargetPos() ;
				}
				target.SetPos(pos);
				break;
			}
		case eFSTT_PositionOrEnemy:
			{
				CFPos pos;
				CFighterDictator* pTargetObject = GetFighter()->GetTarget();
				if (pTargetObject)
				{
					if (CRelationMgrServer::IsFriend(GetFighter(), pTargetObject))
					{
						pos=GetFighter()->GetTargetPos() ;
					}
					else
					{
						pTargetObject->GetPixelPos(pos);
					}
				}
				else
				{
					pos=GetFighter()->GetTargetPos() ;
				}
				target.SetPos(pos);
				break;
			}
		case eFSTT_PositionOrLockedTarget:
			{
				CFPos pos;
				CFighterDictator* pTargetObject = GetFighter()->GetTarget();
				if (pTargetObject)
				{
					if (CRelationMgrServer::IsFriend(GetFighter(), pTargetObject))
					{
						pos=GetFighter()->GetTargetPos() ;
					}
					else
					{
						pTargetObject->GetPixelPos(pos);
					}
				}
				else
				{
					pos=GetFighter()->GetTargetPos() ;
				}
				target.SetPos(pos);
				break;
			}
		case eFSTT_None:
		default:
			{
				ostringstream strm;
				strm << pSkill->GetName();
				GenErr("技能目标类型错误", strm.str());
			}
		}
	}
	SQR_CATCH(exp)
	{
		CConnServer* pConn = GetFighter()->GetConnection();
		if (pConn)
			LogExp(exp, pConn);
		else
			LogExp(exp);
		return eDSR_TargetNotExist;
	}
	SQR_TRY_END;

	return eDSR_Success;
}

void CPlayerSkillMgrServer::AdjustSkillLevel(uint8& uOutPutSkillLevel, uint8 uSkillLevel, const CNormalSkillServerSharedPtr& pSkill)const
{
	if(pSkill->GetCoolDownType() == eSCDT_FightSkill || pSkill->GetCoolDownType() == eSCDT_NoComCDFightSkill
		|| pSkill->GetCoolDownType() == eSCDT_UnrestrainedFightSkill)
	{
		uOutPutSkillLevel = GetFighter()->GetActiveSkillLevel(pSkill->GetName());
	}
	else
	{
		uOutPutSkillLevel = pSkill->GetSkillLevelByLevel(uSkillLevel);	//由映射表中得到的等级
	}
	if(uOutPutSkillLevel == 0)
	{
		uOutPutSkillLevel	= GetFighter()->CppGetLevel();				//配置表没填映射关系,则技能等级暂时改成人物等级
	}
}

EDoSkillResult CPlayerSkillMgrServer::DoSkillCheck(const CNormalSkillServerSharedPtr& pSkill)const
{
	if(!GetFighter()->CppIsAlive() && !pSkill->GetTargetIsCorpse())
		return eDSR_CharacterHasDead;

	if(pSkill->GetHorseLimit() && !GetFighter()->GetCtrlState(eFCS_InBattleHorse))
		return eDSR_IsNotExistHorse;
	return eDSR_Success;
}

CSkillInstServer* CPlayerSkillMgrServer::CreateSkillInst(const CNormalSkillServerSharedPtr& pSkill, uint8 uSkillLevel, uint8 uDir, uint8 uMagicDir)const
{
	uint32 uAttackerID = m_pFighter->GetEntityID();
	CSkillInstServer* pInst = CSkillInstServer::CreateOrigin(uAttackerID,pSkill->GetName(),pSkill,uSkillLevel,pSkill->GetAttackType());
	pInst->SetSkillPos(GetFighterMediator()->GetFighterPos());
	pInst->SetDir(uDir);
	pInst->SetMagicDir(uMagicDir);

	CNormalAttackMgr* pAttackMgr=GetFighter()->GetNormalAttackMgr();
	if (pInst->GetAttackType() == eATT_FollowWeapon)
	{
		if(pAttackMgr)
		{
			pInst->SetAttackType(pAttackMgr->GetAttackType());
		}
		else
		{
			pInst->SetAttackType(eATT_None);
		}
	}
	pInst->SetTargetAliveCheckIsIgnored(pSkill->GetTargetIsCorpse());

	return pInst;
}

EDoSkillResult CPlayerSkillMgrServer::DoPlayerSkill(const CNormalSkillServerSharedPtr& pSkill, uint8 uSkillLevel, uint8 uDir, uint8 uMagicDir)
{	
#if defined _FightSkillCounter
	++ms_uPlayerDoSkillFailNum;
#endif

	SQR_TRY
	{
		EDoSkillResult eResult = DoSkillCheck(pSkill);
		if(eResult != eDSR_Success)
			return eResult;

		if(GetFighter()->GetDoSkillCtrlState(eDSCS_InDoingSkill) &&
			!(GetCurCasting() && GetCurCasting()->GetCastingProcessCfg()->GetCastingType() == eCPT_GradeSing
			&& strcmp(GetCurCasting()->GetCastingProcessCfg()->GetName().c_str(),pSkill->GetName().c_str()) ==0))
			return eDSR_DoingOtherSkill;

		CCharacterDictator* pCharacter = GetFighter()->GetCharacter();
		const CCastingProcessCfgServerSharedPtr& pCastingProcessCfg = pSkill->GetCastingProcess();
		if(pCastingProcessCfg && pCastingProcessCfg->GetMoveInterrupt())
		{
			if(pCharacter)
			{
				CCharacterMediator* pCharMed = pCharacter->CastToCharacterMediator();
				if(pCharMed && pCharMed->DirectorMovingIsDisabled() && pCharMed->IsMoving()) 
					return eDSR_Fail;
			}
		}
		
		//cout << "开始释放技能 Pause NA" << endl;
			//关闭自动追踪并暂停普攻
		GetFighter()->CancelNormalAttackAutoTracking();
		GetFighter()->PauseAttack(true, true);

		CGenericTarget genericTarget;
		eResult = InitTarget(genericTarget, pSkill, uSkillLevel);
		if(eResult != eDSR_Success)
			return eResult;

		uint8 uTempSkillLevel;
		AdjustSkillLevel(uTempSkillLevel, uSkillLevel, pSkill);

		CSkillInstServer* pInst = CreateSkillInst(pSkill, uTempSkillLevel, uDir, uMagicDir);
		if (pSkill->GetSelectTargetType() == eFSTT_PositionOrLockedTarget)
			pInst->SetProcessLocked(true);
		eResult = pSkill->DoPlayerSkill(pInst,static_cast<CFighterMediator*>(GetFighter()), &genericTarget, uSkillLevel);
		pInst->DestroyOrigin();

	#if defined _FightSkillCounter
		if(eResult == eDSR_Success)
		
			--ms_uPlayerDoSkillFailNum;
			++ms_uPlayerDoSkillSuccNum;
		}
	#endif

		return eResult;
	}
	SQR_CATCH(exp)
	{
		ostringstream strm;
		strm << pSkill->GetName();
		exp.AppendMsg(strm.str());
		CConnServer* pConn = GetFighter()->GetConnection();
		if (pConn)
			LogExp(exp, pConn);
		else
			LogExp(exp);
	}
	SQR_TRY_END;
	return eDSR_None;
}

EDoSkillResult CPlayerSkillMgrServer::DoPlayerPassiveSkill(const CNormalSkillServerSharedPtr& pSkill, uint8 uSkillLevel, uint8 uDir)
{
#if defined _FightSkillCounter
	++ms_uPlayerDoSkillFailNum;
#endif

	EDoSkillResult eResult = DoSkillCheck(pSkill);
	if(eResult != eDSR_Success)
		return eResult;

	//if(GetFighter()->GetCtrlState(eFCS_InDoingSkill) &&

	CGenericTarget genericTarget;
	eResult = InitTarget(genericTarget, pSkill, uSkillLevel);
	if(eResult != eDSR_Success)
		return eResult;

	uint8 uTempSkillLevel;
	AdjustSkillLevel(uTempSkillLevel, uSkillLevel, pSkill);

	CSkillInstServer* pInst = CreateSkillInst(pSkill, uTempSkillLevel, uDir);
	pInst->SetIsPassive(true);
	pInst->SetTargetAliveCheckIsIgnored(true);
	eResult = pSkill->DoPlayerPassiveSkill(pInst,static_cast<CFighterMediator*>(GetFighter()), &genericTarget, uSkillLevel);
	pInst->DestroyOrigin();

#if defined _FightSkillCounter
	if(eResult == eDSR_Success)
	
		--ms_uPlayerDoSkillFailNum;
		++ms_uPlayerDoSkillSuccNum;
	}
#endif

	return eResult;
}

EDoSkillResult CPlayerSkillMgrServer::PlayerDoPassiveFightSkill(const string& szName, uint8 uSkillLevel)
{
	// 流程中可以释放，不触发"施放技能类技能"
	const CNormalSkillServerSharedPtr& pSkill = CNormalSkillServer::GetSkillByName(szName);	
	EDoSkillResult eResult = CanPlayUseSkill(pSkill, uSkillLevel);
	if (eDSR_Success == eResult)
	{
		eResult = DoPlayerPassiveSkill( pSkill, uSkillLevel);
	}
	if (eResult != eDSR_SkillEndWithOutSing && eResult != eDSR_Success)
		GetFighter()->CastToFighterMediator()->SendFailureMsgToGac((uint32)(eResult), szName.c_str());
	return eResult;
}

EDoSkillResult CPlayerSkillMgrServer::CanPlayUseSkill(const CNormalSkillServerSharedPtr& pSkill, uint8 uSkillLevel, uint8 uPos)
{
	EDoSkillResult eResult=eDSR_Success;
	const string& szSkillName = pSkill->GetName();

	// 技能类型检查
	if (pSkill->GetCoolDownType() == eSCDT_NonFightSkill && m_pFighter->GetCtrlState(eFCS_InBattle))
		return eDSR_Fail;

	if((pSkill->GetCoolDownType() != eSCDT_UnrestrainedSkill && pSkill->GetCoolDownType() != eSCDT_UnrestrainedFightSkill)
		&& (m_pFighter->GetDoSkillCtrlState(eDSCS_ForbitUseSkill)
		|| (m_pFighter->GetDoSkillCtrlState(eDSCS_ForbitNatureMagic) && pSkill->GetAttackType() == eATT_Nature)
		|| (m_pFighter->GetDoSkillCtrlState(eDSCS_ForbitDestructionMagic) && pSkill->GetAttackType() == eATT_Destroy)
		|| (m_pFighter->GetDoSkillCtrlState(eDSCS_ForbitEvilMagic) && pSkill->GetAttackType() == eATT_Evil)
		|| (m_pFighter->GetCtrlState(eFCS_OnMission))))
	{
		eResult=eDSR_ForbitUseSkill;
	}

	if (m_pFighter->GetDoSkillCtrlState(eDSCS_ForbitUseClassSkill) && (pSkill->GetCoolDownType() == eSCDT_FightSkill
		|| pSkill->GetCoolDownType() == eSCDT_NoComCDFightSkill || pSkill->GetCoolDownType() == eSCDT_EquipSkill
		|| pSkill->GetCoolDownType() == eSCDT_UnrestrainedFightSkill))
	{
		return eDSR_ForbitUseClassSkill;
	}

	if (m_pFighter->GetDoSkillCtrlState(eDSCS_ForbitUseNonFightSkill) && (pSkill->GetCoolDownType() == eSCDT_NonFightSkill))
	{
		return eDSR_ForbitUseNonFightSkill;
	}

	switch (pSkill->GetCoolDownType())
	{
	case eSCDT_FightSkill:
	case eSCDT_NoComCDFightSkill:
	case eSCDT_UnrestrainedFightSkill:
	case eSCDT_NonFightSkill:
	case eSCDT_EquipSkill:
		if (!m_pFighter->IsSkillActive(szSkillName,uSkillLevel))
		{
			eResult=eDSR_NoSkill;
		}
		break;
	case eSCDT_TempSkill:
		if (!IsExistTempSkill(szSkillName,uSkillLevel,uPos))
		{
			eResult=eDSR_NoSkill;
		}
		break;
	case eSCDT_PublicSkill:
	case eSCDT_UnrestrainedSkill:
	case eSCDT_OtherSkill:
		break;
	default:
		eResult=eDSR_Fail;
		cout << "技能 " << szSkillName.c_str() << "存在BUG\n";
		break;
	}
	return eResult;
}


EDoSkillResult CPlayerSkillMgrServer::UseFightSkill(const CNormalSkillServerSharedPtr& pSkill, uint8 uSkillLevel, uint8 uPos, uint8 uDir, uint8 uMagicDir)
{
	EDoSkillResult eResult=CanPlayUseSkill(pSkill, uSkillLevel, uPos);
	if (eDSR_Success==eResult)
	{
		eResult = DoPlayerSkill( pSkill, uSkillLevel, uDir, uMagicDir);
	}
	// 根据技能施放结果，如失败将具体原因发送给客户端
	if ( eDSR_Success == eResult)
	{
		//cout <<  "技能 " << szSkillName << " 使用成功" << endl;
		if (uPos!=0)
		{
			ActiveTempSkill(pSkill.get()->GetName(),uSkillLevel,uPos);
		}
	}
	else
	{
		//cout <<  "技能 " << szSkillName << " 使用失败 错误ID为 " << eResult << endl;
		// 把出错MessageID发送给客户端
		OnMagicCondFail((uint32)eResult, pSkill.get()->GetName().c_str());		
	}
	return eResult;
}

bool  CPlayerSkillMgrServer::AddTempSkill(const string& szName, uint8 uSkillLevel)
{
	if (m_vecTempSkill.size()<=MAXTEMPSKILLCOUNT+1)
	{
		for (uint8 uID=1;uID<=MAXTEMPSKILLCOUNT;++uID)
		{
			if(m_vecTempSkill[uID]==NULL)
			{
				m_vecTempSkill[uID]=new CTempSkill(this,szName,uSkillLevel,uID);
				return true;
			}
		}
	}
	return false;
}

bool CPlayerSkillMgrServer::IsExistTempSkill(const string& szName, uint8 uSkillLevel, uint8 uPos)
{
	if (uPos<=MAXTEMPSKILLCOUNT&&m_vecTempSkill[uPos]!=NULL)
	{
		return m_vecTempSkill[uPos]->IsSameTempSkill(szName,uSkillLevel,uPos)&&(m_vecTempSkill[uPos]->m_uCount>0||m_vecTempSkill[uPos]->m_uCount==-1);
	}
	return false;
}

void CPlayerSkillMgrServer::RemoveTempSkill(const string& szName, uint8 uSkillLevel, uint8 uPos)
{
	if (uPos<=MAXTEMPSKILLCOUNT&&uPos<m_vecTempSkill.size()&&m_vecTempSkill[uPos]!=NULL&&m_vecTempSkill[uPos]->IsSameTempSkill(szName,uSkillLevel,uPos))
	{
			m_vecTempSkill[uPos]->Cancel(); 
			m_vecTempSkill[uPos]=NULL; 
	}
}

void CPlayerSkillMgrServer::ActiveTempSkill(const string& szName, uint8 uSkillLevel, uint8 uPos)
{
	if (uPos<=MAXTEMPSKILLCOUNT&&uPos<m_vecTempSkill.size()&&m_vecTempSkill[uPos]!=NULL&&m_vecTempSkill[uPos]->IsSameTempSkill(szName,uSkillLevel,uPos))
	{
		if ((m_vecTempSkill[uPos]->m_uCount != -1) && (--m_vecTempSkill[uPos]->m_uCount == 0) )
		{
			m_vecTempSkill[uPos]->Cancel(); 
			m_vecTempSkill[uPos]=NULL; 
		}
	}
	/*CFighterMediator* pMediator= GetFighter()->CastToFighterMediator();
	ICharacterMediatorCallbackHandler* pHandler = pMediator?pMediator->GetCallBackHandler():NULL;
	if(pHandler)
		pHandler->OnCastTempSkill(GetFighter()->GetEntityID(),uPos);*/
}

void CPlayerSkillMgrServer::RemoveTempSkill(const string& szName, uint8 uSkillLevel)
{
	for (uint8 uID=1;uID<=MAXTEMPSKILLCOUNT;++uID)
	{
		if(m_vecTempSkill[uID]!=NULL)
		{
			if (m_vecTempSkill[uID]->IsSameTempSkill(szName,uSkillLevel,uID))
			{
					m_vecTempSkill[uID]->Cancel(); 
					m_vecTempSkill[uID]=NULL; 
			}
		}
	}
}

void CPlayerSkillMgrServer::RemoveAllTempSkill()
{
	for (uint8 uID=1;uID<=MAXTEMPSKILLCOUNT;++uID)
	{
		if(m_vecTempSkill[uID]!=NULL)
		{
			m_vecTempSkill[uID]->Cancel(); 
			m_vecTempSkill[uID]=NULL; 
		}
	}
}

CFighterMediator* CPlayerSkillMgrServer::GetFighterMediator()const
{
	return GetFighter()->CastToFighterMediator();
}

//CTempSkill
CTempSkill::CTempSkill(CPlayerSkillMgrServer* pSkillMgr,const string& strSkillName,uint8 uSkillLevel,uint8 uID)
:m_strSkillName(strSkillName)
,m_pSkillMgr(pSkillMgr)
,m_uSkillLevel(uSkillLevel)
,m_uID(uID)
,m_bIsEndTime(false)
,m_uCount(0)
{
	CFighterDictator* pFighter = m_pSkillMgr->GetFighter();
	CFighterMediator* pMediator= pFighter->CastToFighterMediator();
	ICharacterMediatorCallbackHandler* pHandler = pMediator->GetCallBackHandler();
	const CNormalSkillServerSharedPtr& pSkill = CNormalSkillServer::GetSkillByName(strSkillName);
	m_uCount = pSkill->GetActiveCount()==0?(-1):pSkill->GetActiveCount();
	float fTime = pSkill->GetActiveTime();
	if (pMediator)
	{
		if(pHandler)pHandler->OnCreateTempSkill(m_pSkillMgr->GetFighter()->GetEntityID(),
			strSkillName.c_str(),m_uSkillLevel,fTime,uID);
	}
	if (fTime>0)
	{
		RegTick(uint32((fTime+0.5)*1000));
	}
}

CTempSkill::~CTempSkill()
{
}

void CTempSkill::RegTick( uint32 uTime )
{
	m_pSkillMgr->GetFighter()->RegistDistortedTick(this, uTime);
}

void CTempSkill::UnRegTick()
{
	m_pSkillMgr->GetFighter()->UnRegistDistortedTick(this);
}

void CTempSkill::OnTick()
{
	m_bIsEndTime=true;
	m_pSkillMgr->RemoveTempSkill(m_strSkillName,m_uSkillLevel,m_uID);
}

void CTempSkill::Cancel()
{
	CFighterDictator* pFighter = m_pSkillMgr->GetFighter();
	CFighterMediator* pMediator= pFighter->CastToFighterMediator();
	ICharacterMediatorCallbackHandler* pHandler = pMediator?pMediator->GetCallBackHandler():NULL;
	UnRegTick();
	if (pHandler&&!m_bIsEndTime)
	{
		pHandler->OnRemoveTempSkill(m_pSkillMgr->GetFighter()->GetEntityID(),m_uID);
	}
	delete this;
}

bool CTempSkill::IsSameTempSkill(const std::string &strNameSkil, sqr::uint8 uSkillLevel, sqr::uint8 uID)
{
	return (strNameSkil==m_strSkillName)&&(uSkillLevel==m_uSkillLevel)&&(uID==m_uID);
}
