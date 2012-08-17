#pragma once
#include "CNpcEnmityMgr.h"

class CCharacterDictator;
class CNpcAI;
class CNpcEnmityMember;

class CMemberEnmityMgr : public CNpcEnmityMgr
{
public:

	CMemberEnmityMgr(CNpcAI* pOwner);             
	~CMemberEnmityMgr(); 
	virtual bool BeTargetOutOfLockEnemyDis(CCharacterDictator* pCharacter);
	virtual bool AddEnemy(CCharacterDictator* pEnemy, bool bHurtEffect);
}; 

