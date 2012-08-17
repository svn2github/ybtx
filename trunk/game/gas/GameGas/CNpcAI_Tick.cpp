#include "stdafx.h"
#include "CNpcAI.h"
#include "CCharacterDictator.h"
#include "Random.h"
#include "CSceneCommon.h"

void CNpcAI::PatrolTick::OnTick()
{
	if (m_pAI->m_bAlertState)
		return;
	m_pAI->OnMoveToPathPoint(m_pAI->m_uCurrentPoint);
	m_pAI->GetCharacter()->UnRegistDistortedTick(this);
}

void CNpcAI::SpecialActionTick::OnTick()
{
	Ast(m_pAI);
	CCharacterDictator* pCharacterDictator = m_pAI->GetCharacter();
	Ast(pCharacterDictator);
	pCharacterDictator->SetAndSyncActionState(eAS_Idle_Special);
	m_pAI->GetCharacter()->UnRegistDistortedTick(this);
}

void CNpcAI::ExistTick::OnTick()
{
	m_pAI->GetCharacter()->UnRegistDistortedTick(this);
	CCharacterDictator* pCharacter = m_pAI->GetCharacter();
	Ast (pCharacter);
	if (m_pAI->BeInBattle())
	{
		if (m_pAI->CanDelInBattle())
			pCharacter->KillNpcByLifeTimeEnd();
		else
		{
			m_pAI->SetLifeTimeEnd();
		}
	}
	else
		pCharacter->KillNpcByLifeTimeEnd();

	m_pAI->m_pExistTick = NULL;
	delete this;
}

void CNpcAI::DoActionOrRandomMoveTick::OnTick()
{
	Ast (m_pAI);
	Ast (m_pAI->GetCharacter());

	if (m_pAI->m_bAlertState)
		return;

	uint32 uRandomMoveInterval = m_pAI->GetRandMoveTime()*1000;
	m_pAI->GetCharacter()->UnRegistDistortedTick(this);

	if (m_pAI->m_eMoveType == ENpcMove_Random)
	{
		if (Random::Rand(0,100) > 50)
		{
			CFPos pos = m_pAI->m_RandomMoveBeginPos;
			CFPos CurPos;
			m_pAI->GetCharacter()->GetPixelPos(CurPos);
			int32 MoveRange = m_pAI->m_uMoveRange;
			pos.x = pos.x + Random::Rand(-MoveRange*eGridSpanForObj, MoveRange*eGridSpanForObj);		
			pos.y =	pos.y + Random::Rand(-MoveRange*eGridSpanForObj, MoveRange*eGridSpanForObj);
			if (pos != CurPos)
				m_pAI->MoveToPixelInLinePath(pos, m_pAI->m_bDefaultWalkState);

			m_pAI->GetCharacter()->RegistDistortedTick(this, uRandomMoveInterval);

			return;
		}
	}

	if (m_pAI->m_eMoveType == ENpcMove_None && m_pAI->m_uSpecialActionProbability == 100)
	{
		m_pAI->GetCharacter()->SetAndSyncActionState(eAS_Idle_Special_Loop);
		return;
	}
	else if ( (uint32)Random::Rand(0,100) < m_pAI->m_uSpecialActionProbability)
	{
		m_pAI->GetCharacter()->SetAndSyncActionState(eAS_Idle_Special);

		uint32 uActionFrame = m_pAI->GetCharacter()->GetSpecialActionAniCnt();
		m_pAI->GetCharacter()->RegistDistortedTick(this, uRandomMoveInterval + uActionFrame*33);
		return;
	}

	m_pAI->GetCharacter()->RegistDistortedTick(this, uRandomMoveInterval);
}

void CNpcAI::CNpcAlertDelayAttackTick::OnTick()
{
	m_pAI->BeginAttack(m_uTargetID);
	m_pAI->m_pNpcAlertDelayAttackTick = NULL;
	delete this;
}

void CNpcAI::DelaySetEyeSizeAndNatureTick::OnTick()
{
	Ast (m_pAI);
	m_pAI->m_pSetNpcAoiTick = NULL;
	CCharacterDictator* pCharacter = m_pAI->GetCharacter();
	Ast (pCharacter);
	if (!m_pAI->m_bInSleepState)
		pCharacter->SetEyeSight(m_pAI->GetAIBaseData()->m_uEyeSize);
	
	if (!pCharacter->IsMoving())
		pCharacter->SetAndSyncActionState(eAS_Still_1);

	delete this;
}
