#include "stdafx.h"
#include "CAniKeyFrameMgrServer.h"
#include "CGenericTarget.h"
#include "CSkillInstServer.h"
#include "CFighterDictator.h"
#include "CSkillServer.h"
#include "CEntityServer.h"
#include "CoreCommon.h"
#include "CSkillMgrServer.h"
#include "NormalAttack.h"
#include "CFighterMediator.h"
#include "CCharacterDictator.h"
#include "CActorServer.h"
#include "CAniKeyFrameCfg.h"

struct  SkillInfo : public CFighterMallocObject
{
	SkillInfo(const CMagicEffServerSharedPtr& MagicEff, CSkillInstServer* pInst, CGenericTarget* pTarget)
		:m_MagicEff(MagicEff),m_pInst(pInst),m_pTo(new CGenericTarget(*pTarget))
	{
		pInst->AddRef();
	}
	~SkillInfo()
	{
		m_pInst->DelRef();
		delete m_pTo;
	}
	const CMagicEffServerSharedPtr m_MagicEff;
	CSkillInstServer* m_pInst;
	CGenericTarget* m_pTo;
};

CAniKeyFrameMgrServer::CAniKeyFrameMgrServer(CFighterDictator* pFrom)
:m_pFrom(pFrom)
,m_pSkillInfo(NULL)
{
}

CAniKeyFrameMgrServer::~CAniKeyFrameMgrServer()
{
	FailEffect();
}

void CAniKeyFrameMgrServer::RegistSkill(const CMagicEffServerSharedPtr& MagicEff,CSkillInstServer* pInst, CGenericTarget* pTarget)
{
	if (m_pSkillInfo)
	{
		UnregistEvent();
		if (m_pSkillInfo->m_MagicEff)
		{
			m_pSkillInfo->m_MagicEff->Do(m_pSkillInfo->m_pInst, m_pFrom, m_pSkillInfo->m_pTo);
		}
		else
		{

			LogErr("SkillInfo->m_MagicEff ΪNULL");
		}
		delete m_pSkillInfo;
		m_pSkillInfo =NULL;
	}

	const CNormalSkillServerSharedPtr& pSkill = pInst->GetSkillCfg();	
	uint8 uFrame;
	if(m_pFrom->CastToFighterMediator())
		uFrame= m_pFrom->GetCharacter()->GetActor()->GetCastKeyFrameByAniName(pSkill->GetCastAction());
	else 
		uFrame = CAniKeyFrameCfg::GetNPCAniKeyFrame(m_pFrom->GetCharacter()->GetNpcName().c_str(), pSkill->GetCastAction().c_str(), "k");
	if (uFrame !=0)
	{
		m_pSkillInfo= new SkillInfo(MagicEff,pInst,pTarget);
		m_pFrom->Attach(this,eCE_Die);
		m_pFrom->Attach(this,eCE_ChangeMapEnd);
		m_pFrom->Attach(this, eCE_FinishDuel);
		m_pFrom->Attach(this, eCE_BeInterrupted);

		if (pTarget->GetFighter()&&pTarget->GetFighter()!=m_pFrom)
		{
			CFighterDictator* pTo = pTarget->GetFighter();
			pTo->Attach(this,eCE_Die);
			pTo->Attach(this, eCE_Offline);
			pTo->Attach(this, eCE_ChangeMapEnd);
			if(m_pFrom->CastToFighterMediator())
			{
				float fMaxDis = (float)pSkill->GetMaxCastSkillDistance()->GetDblValue(m_pFrom, pInst->GetSkillLevel() - 1);
				m_pFrom->GetEntity()->Watch(this,pTo->GetEntity(),fMaxDis*2*eGridSpanForObj);
			}
		}
		m_pFrom->RegistDistortedTick(this, uFrame*33);
	}
	else
	{
		MagicEff->Do(pInst, m_pFrom, pTarget);
		ResumeAttack(pInst);
	}
}

void CAniKeyFrameMgrServer::UnregistEvent()
{
	m_pFrom->UnRegistDistortedTick(this);
	if (m_pSkillInfo&&m_pSkillInfo->m_pTo->GetFighter())
	{
		CFighterDictator* pTarget = m_pSkillInfo->m_pTo->GetFighter();
		if (pTarget)
		{
			pTarget->Detach(this,eCE_Die);
			pTarget->Detach(this, eCE_Offline);
			pTarget->Detach(this, eCE_ChangeMapEnd);
		}
		m_pFrom->GetEntity()->StopWatching(this);
	}
	m_pFrom->Detach(this,eCE_Die);
	m_pFrom->Detach(this,eCE_ChangeMapEnd);
	m_pFrom->Detach(this, eCE_FinishDuel);
	m_pFrom->Detach(this, eCE_BeInterrupted);
}

void CAniKeyFrameMgrServer::DoEffect()
{
	UnregistEvent();
	if (m_pSkillInfo)
	{
		if (m_pSkillInfo->m_MagicEff)
		{
			m_pSkillInfo->m_MagicEff->Do(m_pSkillInfo->m_pInst, m_pFrom, m_pSkillInfo->m_pTo);
		}
		else
		{

			LogErr("SkillInfo->m_MagicEff ΪNULL");
		}
		if (m_pSkillInfo)
		{
			ResumeAttack(m_pSkillInfo->m_pInst);
		}
		delete m_pSkillInfo;
		m_pSkillInfo =  NULL;
	}
}

void CAniKeyFrameMgrServer::FailEffect()
{
	UnregistEvent();
	if (m_pSkillInfo)
	{
		ResumeAttack(m_pSkillInfo->m_pInst);
		delete m_pSkillInfo;
		m_pSkillInfo =  NULL;
	}
}


void CAniKeyFrameMgrServer::OnCOREvent(CPtCORSubject* pSubject,uint32 uEvent,void* pArg)
{
	CFighterDictator* pTo = m_pSkillInfo?m_pSkillInfo->m_pTo->GetFighter():NULL ;
	if( pSubject == pTo)
	{
		switch(uEvent)
		{
		case eCE_Die:
		case eCE_Offline:
		case eCE_ChangeMapEnd:
			FailEffect();
		default:
			break;
		}
	}
	else if (pSubject == m_pFrom)
	{
		switch(uEvent)
		{
		case eCE_FinishDuel:
		case eCE_ChangeMapEnd:
			FailEffect();
			break;
		case eCE_Die:
		case eCE_BeInterrupted:
			DoEffect();
			break;
		default:
			break;
		}
	}
}

void CAniKeyFrameMgrServer::OnObserveeLeft()
{
	DoEffect();
}

void CAniKeyFrameMgrServer::OnTick()
{
	DoEffect();
}

void CAniKeyFrameMgrServer::ResumeAttack(const CSkillInstServer* pInst)
{
	CNormalAttackMgr* pAttackMgr=m_pFrom->GetNormalAttackMgr();
	CSkillMgrServer* pSkillMgr = m_pFrom ->GetSkillMgr();
	if (pAttackMgr && pSkillMgr->GetCurCasting() == NULL)
	{
		const CNormalSkillServerSharedPtr& pSkill = pInst->GetSkillCfg();
		CFighterDictator* pTargetObject = m_pFrom->GetTarget();
		switch(pSkill->GetStartNormalAttack())
		{
		case eANAT_StartAndAutoTrack:
		case eANAT_Start:
		case eANAT_AbsolutelyStartAndAutoTrack:
		case eANAT_AbsolutelyStart:
			{
				if(pAttackMgr->IsAttacking())
				{
					//cout << "$$ skill restartNA" << endl;
					m_pFrom->RestartAttack(true);
					if(pTargetObject && (pSkill->GetStartNormalAttack() == eANAT_StartAndAutoTrack || pSkill->GetStartNormalAttack() == eANAT_AbsolutelyStartAndAutoTrack))
						m_pFrom->CastToFighterMediator()->TellDirectorToDoNA(pTargetObject, true);
				}
				else
				{
					if(pTargetObject)
					{
						//cout << "## skill startNA" << endl;
						m_pFrom->CastToFighterMediator()->TellDirectorToDoNA(pTargetObject, (pSkill->GetStartNormalAttack()) == eANAT_StartAndAutoTrack || (pSkill->GetStartNormalAttack()) == eANAT_AbsolutelyStartAndAutoTrack);
					}
				}
				break;
			}
		case eANAT_Cancel:
			{
				m_pFrom->CancelAttack(true);
				break;
			}
		case eANAT_NoChange:
			{
				if(pAttackMgr->IsAttacking())
				{
					m_pFrom->RestartAttack(true);
				}
				break;
			}
		default:
			{
				if(pAttackMgr->IsAttacking())
				{
					m_pFrom->RestartAttack(true);
				}
				break;
			}
		}
	}	
}

