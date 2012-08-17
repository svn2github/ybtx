#include "stdafx.h"
#include "CDelayMagicOpTick.h"
#include "FightDef.h"
#include "CFighterDictator.h"

CDelayMagicOpTick::CDelayMagicOpTick(CSkillInstServer* pSkillInst, CFighterDictator* pFrom)
:m_pFrom(pFrom)
,m_pSkillInst(pSkillInst)
{
	AttachEvent();
}

CDelayMagicOpTick::~CDelayMagicOpTick()
{
	m_pFrom->UnRegistDistortedTick(this);
}

void CDelayMagicOpTick::AttachEvent()
{
	m_pFrom->Attach(this, eCE_Offline);
	m_pFrom->Attach(this, eCE_Die);
	m_pFrom->Attach(this, eCE_ChangeMapEnd);
	m_pFrom->Attach(this, eCE_FinishDuel);
}

void CDelayMagicOpTick::DetachEvent()
{
	m_pFrom->Detach(this, eCE_Offline);
	m_pFrom->Detach(this, eCE_Die);
	m_pFrom->Detach(this, eCE_ChangeMapEnd);
	m_pFrom->Detach(this, eCE_FinishDuel);
}

void CDelayMagicOpTick::OnCOREvent(CPtCORSubject* pSubject, uint32 uEvent,void* pArg)
{
	if (pSubject == m_pFrom)
	{
		switch (uEvent)
		{
		case eCE_Die:
		case eCE_ChangeMapEnd:
		case eCE_Offline:
		case eCE_FinishDuel:
			delete this;
			break;
		default:
			break;
		}
	}
}
