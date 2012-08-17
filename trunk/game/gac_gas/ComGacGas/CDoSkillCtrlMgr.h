#pragma once
#include "CDynamicObject.h"
#include "FightDef.h"
#include "CFighterMallocObject.h"

class CDoSkillCtrlMgr
	: public virtual CDynamicObject
	, public CFighterMallocObject
{
public:
	typedef vector<uint16> VecSkillCtrlState;
	CDoSkillCtrlMgr();
	~CDoSkillCtrlMgr();

	void SetDoSkillCtrlState(EDoSkillCtrlState eSkillCtrlState, bool bSet);
	bool GetDoSkillCtrlState(EDoSkillCtrlState eSkillCtrlState);
	void RevertSkillCtrlState();
protected:
	VecSkillCtrlState m_vecSkillCtrlState;
};

