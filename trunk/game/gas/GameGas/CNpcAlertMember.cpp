#include "stdafx.h"
#include "CNpcAlertMember.h"
#include "CNpcAlertMgr.h"
#include "CCharacterDictator.h"
#include "CoreCommon.h"
#include "CNpcEventMsg.h"

CNpcAlertMember::CNpcAlertMember(CNpcAlertMgr* pAlertMgr, uint32 uAlertTarget)
:m_pAlertMgr(pAlertMgr)
,m_uAlertTarget(uAlertTarget)
{
	CNpcAI* pOwnAI = m_pAlertMgr->GetOwner();
	Ast (pOwnAI);
	CCharacterDictator* pOwnCharacter = pOwnAI->GetCharacter();
	Ast (pOwnCharacter);
	CCharacterDictator* pAlertTarget = 	CCharacterDictator::GetCharacterByID(m_uAlertTarget);
	Ast (pAlertTarget);
	pOwnCharacter->Watch(this, pAlertTarget, (float)pOwnAI->GetAlertDist() * eGridSpan);
	m_pAlertMgr->AddAlertTargetToMap(m_uAlertTarget, this);
}

CNpcAlertMember::~CNpcAlertMember()
{
	CNpcAI* pOwnAI = m_pAlertMgr->GetOwner();
	Ast (pOwnAI);
	CCharacterDictator* pOwnCharacter = pOwnAI->GetCharacter();
	Ast (pOwnCharacter);
	pOwnCharacter->StopWatching(this);
	m_pAlertMgr->DelAlertTargetToMap(m_uAlertTarget, this);
}

uint32 CNpcAlertMember::GetAlertTargetID()
{
	return m_uAlertTarget;
}

void CNpcAlertMember::OnObserveeEntered()
{

}

void CNpcAlertMember::OnObserveeLeft()
{
	CNpcAI* pOwnAI = m_pAlertMgr->GetOwner();
	Ast (pOwnAI);
	CCharacterDictator* pOwnCharacter = pOwnAI->GetCharacter();
	Ast (pOwnCharacter);
	pOwnCharacter->StopWatching(this);
	CNpcEventMsg::Create(pOwnAI, eNpcEvent_OnLeaveAlertTarget, eNECI_Zero, reinterpret_cast<void*>(m_uAlertTarget));
}

void CNpcAlertMember::OnWatched(EWatchResult eResult)
{
	if (eResult == eWR_NotInScope)
		OnObserveeLeft();
}
