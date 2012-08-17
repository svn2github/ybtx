#include "stdafx.h"
#include "CFighterViewInfo.h"
#include "TPropertyHolder.inl"
#include "CCharacterFollower.h"
#include "CEntityClientManager.h"

CFighterViewInfo::CFighterViewInfo(void)
{
}

CFighterViewInfo::~CFighterViewInfo(void)
{
}

void CFighterViewInfo::CppInit(uint32 uGlobalID)
{
	CCharacterFollower* pChar = CEntityClientManager::GetInst()->GetCharacter(uGlobalID);
	SetClass(pChar->CppGetClass());
	SetLevel(pChar->CppGetLevel());
}