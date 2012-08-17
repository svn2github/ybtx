#pragma once

#include "PatternRef.h"
#include "CEnmityMgr.h"

class CCharacterDictator;
class CNpcAI;
class CNpcEnmityMember;
class CMemberAI;
class CNpcEventMsg;

class CDummyEnmityMgr : public CEnmityMgr
{
public:

	CDummyEnmityMgr(CNpcAI* pOwner);
	~CDummyEnmityMgr();
	virtual bool JudgeIsEnemy(CCharacterDictator* pTarget) {return true;}
	virtual void ClearAllEnmity();
	virtual void RemoveFarawayEnemy(CCharacterDictator* pEnemy);
	virtual void RemoveEnemy(CCharacterDictator* pEnemy);

	virtual bool AddEnemy(CCharacterDictator* pEnemy, bool bHurtEffect);
	void CopyEnmityListToMember(CMemberAI* pMemAI, CNpcEventMsg* pEvent);

private:

}; 
