#include "stdafx.h"
#include "CPlayerSet.h"
#include "CCharacterMediator.h"

void CPlayerSet::AddPlayer(CCharacterMediator* pPlayer)
{
	if (!pPlayer)
		return;
	CConnServer* pConn = pPlayer->GetConnection();
	if (!pConn)
		return;

	AddPipe(pConn);

	m_setPlayer.insert(pPlayer->GetEntityID());
}

void CPlayerSet::DelPlayer(CCharacterMediator* pPlayer)
{
	if (!pPlayer)
		return;
	CConnServer* pConn = pPlayer->GetConnection();
	if (!pConn)
		return;

	DelPipe(pConn);

	m_setPlayer.erase(pPlayer->GetEntityID());
}

void CPlayerSet::DoSkill(const char* szSkillName, uint32 uSkillLevel, const char* szExt)
{
	SetPlayer::iterator it = m_setPlayer.begin();
	for(;it!=m_setPlayer.end();++it)
	{
		CCharacterDictator* pCharacter = CCharacterDictator::GetCharacterByID(*it);
		if (pCharacter&&pCharacter->CastToCharacterMediator())
		{
			CCharacterMediator* pPlayer = pCharacter->CastToCharacterMediator();
			pPlayer->PlayerDoPassiveSkill(szSkillName,uSkillLevel);
		}
	}
}

