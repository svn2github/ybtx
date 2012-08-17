#pragma once 

#include "CNpcAI.h"
#include "PatternRef.h"
#include "CNpcEventMsg.h"
#include "CNpcAIMallocObject.h"

class StateMachineCheckValid
	: public CNpcAIMallocObject
{
friend class TPtRefee<StateMachineCheckValid, CNpcAI>;
public:
	StateMachineCheckValid(CNpcAI* pAI);
	~StateMachineCheckValid();
	TPtRefee<StateMachineCheckValid, CNpcAI>& GetRefByNpcAI();
	void CheckValid(CNpcEventMsg* pEvent);
	void DoAction();
private:
	TPtRefee<StateMachineCheckValid, CNpcAI>	m_RefOfAI;
	CNpcAI*										m_pAI;
	uint64										m_uLasteTime;
	uint32										m_uCycNum;
	bool										m_bMayDeadCycState;
	string										m_sErrLog;
};

