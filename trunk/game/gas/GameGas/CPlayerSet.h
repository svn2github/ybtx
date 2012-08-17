#pragma once
#include "CMultiMsgSender.h"
#include "TFighterAllocator.h"
#include "CDynamicObject.h"

class CCharacterMediator;

class CPlayerSet :public CMultiMsgSender
{
public:
	typedef set<uint32, less<uint32>, TFighterAllocator<uint32> >	SetPlayer; 
	void AddPlayer(CCharacterMediator* pPlayer);
	void DelPlayer(CCharacterMediator* pPlayer);
	const SetPlayer& GetPlayerSet()const {return m_setPlayer;}
	void DoSkill(const char* szSkillName, uint32 uSkillLevel, const char* szExt);
private:
	set<uint32, less<uint32>, TFighterAllocator<uint32> >	 m_setPlayer;
};
