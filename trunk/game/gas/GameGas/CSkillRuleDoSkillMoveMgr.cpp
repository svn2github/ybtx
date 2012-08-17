#include "stdafx.h"
#include "CSkillRuleDoSkillMoveMgr.h"
#include "PatternRef.inl"
#include "CSkillRule.h"
#include "CNpcAI.h"
#include "CCharacterDictator.h"
#include "CFighterDictator.h"
#include "CDir.h"
#include "CSkillRuleMgr.h"
#include "CSceneCommon.h"

template class TPtRefee<CSkillRuleDoSkillMoveMgr, CSkillRule>;	

CSkillRuleDoSkillMoveMgr::CSkillRuleDoSkillMoveMgr(CSkillRule* pSkillRule, CFPos targetPos, EDoSkillType eType)
:m_pSkillRule(pSkillRule)
,m_pFighter(NULL)
,m_bDoSkill(true)
,m_eDoSkillType(eType)
,m_uMoveId(0)
{
	m_pRefeeBySkillRule.SetHolder(this);
	m_pSkillRule->SkillRuleReferHandler(this);

	CNpcAI* pOwnAI = m_pSkillRule->GetOwner();
	Ast(pOwnAI);
	CCharacterDictator* pCharacter = pOwnAI->GetCharacter();
	Ast(pCharacter);

	m_pFighter = pCharacter->GetFighter();
	m_pFighter->Attach(this, eCE_Offline);
	m_pFighter->Attach(this, eCE_ChangeMapEnd);
	m_pFighter->Attach(this, eCE_MoveEnd);
	m_pFighter->Attach(this, eCE_FinishDuel);

	if (m_pFighter->GetCtrlState(eFCS_ForbitMove))
	{
		if (eType == EDoSkillType_BackDo)
			m_bDoSkill = false;
		else
			m_bDoSkill = true;
		CreateDelayTickToDoSkill();
		return;
	}
	CFPos ownDestPos;
	float fRunSpeed;
	if (eType == EDoSkillType_MoveDo)
	{
		CFPos ownPos;
		pCharacter->GetPixelPos(ownPos);
		float fDistance = ownPos.Dist(targetPos);
		float fMaxDistance		= m_pSkillRule->m_fMaxDistance;
		float fMinDistance		= m_pSkillRule->m_fMinDistance;
		Ast (fDistance>fMaxDistance || fDistance<fMinDistance)
		CVector2f vecDirf;
		if (fDistance < fMinDistance)
		{
			float fDesDistance	= fMinDistance + (float)0.5*eGridSpanForObj;
			vecDirf.x = ownPos.x - targetPos.x;
			vecDirf.y = ownPos.y - targetPos.y;
			vecDirf.Normalize();
			ownDestPos.x = targetPos.x + fDesDistance*vecDirf.x;
			ownDestPos.y = targetPos.y + fDesDistance*vecDirf.y;
		}
		else
		{
			float fDesDistance	= fMaxDistance - (float)0.5*eGridSpanForObj;
			vecDirf.x = targetPos.x - ownPos.x;
			vecDirf.y = targetPos.y - ownPos.y;
			vecDirf.Normalize();
			ownDestPos.x = ownPos.x + (fDistance - fDesDistance)*vecDirf.x;
			ownDestPos.y = ownPos.y + (fDistance - fDesDistance)*vecDirf.y;
 		}
	
		fRunSpeed = m_pFighter->m_RunSpeed.Get(m_pFighter);
		//这里直接是用Moto而不使用AI身上封装的MoveTo函数的原因的是：AI对象身上的Move不会失败，而这里需要考虑到移动失败的情况
		EMoveToResult eResult = pCharacter->MoveToInLinePath(ownDestPos, fRunSpeed, eFPT_HypoLine, eBT_LowBarrier);
		m_uMoveId = pCharacter->GetLastMoveID();
		if (eResult != eMTR_Success)	//移动失败则直接调用技能
		{
			m_bDoSkill = false;
			CreateDelayTickToDoSkill();
		}
	}
	else
	{
		CFPos ownPos;
		pCharacter->GetPixelPos(ownPos);
		pOwnAI->GetNpcBirthPos(ownDestPos);
		fRunSpeed = m_pFighter->m_RunSpeed.Get(m_pFighter);
		int32 iMaxStep = pOwnAI->GetMaxStep(ownPos, ownDestPos);
		EMoveToResult eResult = pCharacter->MoveToInAStarPath(ownDestPos, 0.7f*fRunSpeed, eFPT_AStarIgnoreEnd, eBT_LowBarrier, 0.0f, iMaxStep);
		m_uMoveId = pCharacter->GetLastMoveID();
		if (eResult != eMTR_Success)	
		{
			m_bDoSkill = false;
			CreateDelayTickToDoSkill();
		}
	}
}

CSkillRuleDoSkillMoveMgr::~CSkillRuleDoSkillMoveMgr()
{
	m_pFighter->UnRegistDistortedTick(this);
	m_pSkillRule->SkillRuleReferHandler(NULL);
	m_pFighter->Detach(this, eCE_Offline);
	m_pFighter->Detach(this, eCE_ChangeMapEnd);
	m_pFighter->Detach(this, eCE_MoveEnd);
	m_pFighter->Detach(this, eCE_FinishDuel);
}


void CSkillRuleDoSkillMoveMgr::OnCOREvent(CPtCORSubject* pSubject, uint32 uEvent,void* pArg)
{
	switch(uEvent)
	{
		case eCE_Offline:
		case eCE_ChangeMapEnd:
		case eCE_FinishDuel:
		{
			delete this;
		}
		break;
		case eCE_MoveEnd:
		{
			uint32 uMoveID = uint32(*reinterpret_cast<uint32*>(pArg));
			if (uMoveID >= m_uMoveId) //可能有位移魔法导致moveid变化
			{
				if (m_eDoSkillType == EDoSkillType_BackDo)
				{
					CNpcAI* pAI = m_pSkillRule->GetOwner();
					Ast (pAI);
					CCharacterDictator* pCharacter = pAI->GetCharacter();
					Ast (pCharacter);
					CFPos curpos, birthpos;
					pCharacter->GetPixelPos(curpos);
					pAI->GetNpcBirthPos(birthpos);
					if (curpos != birthpos)
						m_bDoSkill = false;
				}
				CreateDelayTickToDoSkill();
			}
		}
		break;

		default:
		break;
	}
}

void CSkillRuleDoSkillMoveMgr::OnTick()
{
	Ast (m_pSkillRule);
	CSkillRuleMgr* pSkillMgr = m_pSkillRule->GetSkillRuleMgr();
	SQR_TRY
	{
		if (m_bDoSkill && m_pSkillRule->SkillRuleCoolDown())
		{
			EDoSkillType eDoType = EDoSkillType_DirectDo;
			if (m_pSkillRule->m_bCheckDistanceAgain)
			{
				eDoType = m_pSkillRule->CheckSkillDistance();
			}
			if (eDoType != EDoSkillType_DirectDo || !m_pSkillRule->DoRuleSkill())
				m_pSkillRule->GetOwner()->OnSkillFailEnd();
		}
		else 
			m_pSkillRule->GetOwner()->OnSkillFailEnd();
	}
	SQR_CATCH(exp)
	{
		string strErrMsg;
		if (!m_pFighter)
		{
			strErrMsg = "m_pFighter do not exist !";
		}
		else if (!m_pSkillRule)
		{
			strErrMsg = "NpcName: " + m_pFighter->GetCharacter()->GetNpcName() + " do not exsit m_pSkillRule !";
		}
		else
		{
			strErrMsg = "NpcName: " + m_pFighter->GetCharacter()->GetNpcName() + " RuleName: " + m_pSkillRule->GetSkillRuleMgr()->GetSkillRuleName() + " exce CSkillRuleDoSkillMoveMgr::OnTick failed !";
		}
		
		exp.AppendMsg(strErrMsg);
		LogExp(exp);
	}
	SQR_TRY_END

	if (pSkillMgr->GetCurrentSkill())
	{	//可能上面DoRuleSkill施放的技能效果导致npc脱离战斗|死亡使技能规则和this析构了
		delete this;
	}
}

void CSkillRuleDoSkillMoveMgr::CreateDelayTickToDoSkill()
{
	m_pFighter->Detach(this, eCE_Offline);
	m_pFighter->Detach(this, eCE_ChangeMapEnd);
	m_pFighter->Detach(this, eCE_MoveEnd);
	m_pFighter->Detach(this, eCE_FinishDuel);
	CCharacterDictator* pCharacter = m_pFighter->GetCharacter();
	Ast (pCharacter);
	pCharacter->StopMoving();
	m_pSkillRule->GetOwner()->FaceToEnemy();
	uint32 uDelayTime = pCharacter->GetFollowerDelay() + 50;
	pCharacter->RegistDistortedTick(this, uDelayTime);
}
