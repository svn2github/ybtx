#include "stdafx.h"
#include "CNpcChaosCoolDownTick.h"
#include "CNpcEnmityMember.h"
#include "CCharacterDictator.h"

CNpcChaosCoolDownTick::CNpcChaosCoolDownTick(CNpcEnmityMember *pMember)
:m_pMember(pMember)
{
}

CNpcChaosCoolDownTick::~CNpcChaosCoolDownTick()
{
}

void CNpcChaosCoolDownTick::OnTick()
{
	Ast (m_pMember);
	m_pMember->m_pChaosCoolDownTick = NULL;
	m_pMember->m_pEntitySelf->UnRegistDistortedTick(this);
	delete this;
}
