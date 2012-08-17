#include "stdafx.h"
#include "CActorVariantCallback.h"
#include "CCypherVariant.h"
#include "CActorFollower.h"
#include "CCharacterFollower.h"
#include "CSyncVariantClient.h"

int CMoveStateCallback::exec(CCypherVariant *pVariant)
{
	CCharacterFollower* pCharacter = m_pActor->GetCharacter();
	Ast(pCharacter);
	EMoveState eMoveState	= (EMoveState)pCharacter->GetSyncVariant(eCTI_Agile)->GetMember("Actor")->GetMember("MoveState")->GetNumber<uint8>();
	CSyncVariantClient* pVariantTemp = pCharacter->GetSyncVariant(eCTI_Agile)->GetMember("Actor")->GetMember("StateForce");
	bool bMoveStateForce	= (!pVariantTemp) ? false : pVariantTemp->GetNumber<bool>();
	if (m_pActor->GetMoveState() != eMoveState)
		m_pActor->ChangeMoveState(eMoveState, bMoveStateForce);

	return 0;
}

int CMoveDirCallback::exec(CCypherVariant *pVariant)
{
	CCharacterFollower* pCharacter = m_pActor->GetCharacter();
	Ast(pCharacter);
	uint8 uMoveDir	= pCharacter->GetSyncVariant(eCTI_Agile)->GetMember("Actor")->GetMember("MoveDir")->GetNumber<uint8>();
	m_pActor->SetMoveDir(uMoveDir);

	return 0;
}

int CActionStateCallback::exec(CCypherVariant *pVariant)
{
	CCharacterFollower* pCharacter = m_pActor->GetCharacter();
	Ast(pCharacter);
	EActionState eActionState	= (EActionState)pCharacter->GetSyncVariant(eCTI_Agile)->GetMember("Actor")->GetMember("ActionState")->GetNumber<uint8>();
	uint8 uActionIndex	= 0;//pCharacter->GetSyncVariant()->GetMember("Actor")->GetMember("ActionIndex")->GetNumber<uint8>();
	float fActionRhythm = pCharacter->GetSyncVariant(eCTI_Agile)->GetMember("Actor")->GetMember("ActionRhythm")->GetNumber<float>();
	if (eActionState >= eAS_Attack_Right && eActionState <= eAS_Attack_Fist)
	{
		CSyncVariantClient* pVariantTemp = pCharacter->GetSyncVariant(eCTI_Agile)->GetMember("Actor")->GetMember("HurtResult");
		EHurtResult eHurtResult	= (!pVariantTemp) ? eHR_Hit : EHurtResult(pVariantTemp->GetNumber<uint8>());
		m_pActor->SetHurtResult(eHurtResult);
	}

	if (fActionRhythm > 0.8f)
		m_pActor->SetNATempRhythm(fActionRhythm);

	m_pActor->DoActionState(eActionState, m_pActor->GetActionDir(), false, false, uActionIndex);

	return 0;
}

int CActionDirCallback::exec(CCypherVariant *pVariant)
{
	CCharacterFollower* pCharacter = m_pActor->GetCharacter();
	Ast(pCharacter);
	CSyncVariantClient* pVariantTemp = pCharacter->GetSyncVariant(eCTI_Agile)->GetMember("Actor")->GetMember("DirGac2Gas");
	bool bDirGac2Gas = (!pVariantTemp) ? false : pVariantTemp->GetNumber<bool>();
	if (bDirGac2Gas && pCharacter->CastToCharacterDirector())
		return 0;

	uint8 uActionDir = pCharacter->GetSyncVariant(eCTI_Agile)->GetMember("Actor")->GetMember("ActionDir")->GetNumber<uint8>();
	if (uActionDir != m_pActor->GetActionDir())
	{
		pVariantTemp = pCharacter->GetSyncVariant(eCTI_Agile)->GetMember("Actor")->GetMember("DirForce");
		bool bActionDirForce = (!pVariantTemp) ? false : pVariantTemp->GetNumber<bool>();
		pCharacter->DoActionDir(uActionDir, bActionDirForce, false);
	}
	return 0;
}

int CWalkBackRadiusCallback::exec(CCypherVariant *pVariant)
{
	CCharacterFollower* pCharacter = m_pActor->GetCharacter();
	Ast(pCharacter);
	uint8 uWalkBackRadius = pCharacter->GetSyncVariant(eCTI_Agile)->GetMember("Actor")->GetMember("WalkBackRadius")->GetNumber<uint8>();
	m_pActor->SetWalkBackRadius(uWalkBackRadius);
	return 0;
}

int CNAActionRhythmCallback::exec(CCypherVariant *pVariant)
{
	CCharacterFollower* pCharacter = m_pActor->GetCharacter();
	Ast(pCharacter);
	float fNAActionRhythm = pCharacter->GetSyncVariant(eCTI_Agile)->GetMember("Actor")->GetMember("NAActionRhythm")->GetNumber<float>();
	m_pActor->SetNAActionRhythm(fNAActionRhythm);
	return 0;
}
