#pragma once
#include "CTick.h"
#include "CFighterMallocObject.h"

class CFighterDirector;

class CSkillCondMgrClient
	: public CTick
	, public CFighterMallocObject
{
public:
	CSkillCondMgrClient(CFighterDirector* mFighter);
	~CSkillCondMgrClient();
private:
	void OnTick();
	CFighterDirector* m_pFighter;;
};