#include "stdafx.h"
#include "CSkillCondMgrClient.h"
#include "CFighterDirector.h"
#include "CAppClient.h"

CSkillCondMgrClient::CSkillCondMgrClient(CFighterDirector* pFighter)
:m_pFighter(pFighter)
{
	CAppClient::Inst()->RegisterTick(this, 330);
}

CSkillCondMgrClient::~CSkillCondMgrClient()
{
	CAppClient::Inst()->UnRegisterTick(this);
}

void CSkillCondMgrClient::OnTick()
{
	m_pFighter->CondTestOnTick();
}