#pragma once

#include "TState.h"
#include "CNpcAIMallocObject.h"

class CNpcAI;
class CNpcEventMsg;
class CNpcStateTransit;

//Npc×´Ì¬»ùÀà
class CNpcStateBase 
	: public TState<CNpcAI, CNpcEventMsg>
	, public CNpcAIMallocObject
{
	typedef TState<CNpcAI, CNpcEventMsg> BaseType;
public:
	CNpcStateBase(CNpcAI *pNpc, uint32 uID);

	virtual CNpcEventMsg* EnterState(CNpcEventMsg* pEvent);
	virtual CNpcEventMsg* LeaveState(CNpcEventMsg* pEvent);
	virtual CNpcEventMsg* Suspend(CNpcEventMsg* pEvent);
	virtual CNpcEventMsg* Resume(CNpcEventMsg* pEvent);
	virtual CNpcEventMsg* OnEvent(CNpcEventMsg* pEvent);
	virtual CNpcEventMsg* CheckValid(CNpcEventMsg* pEvent);
};

class CNpcStateCreatorBase
	: public CNpcAIMallocObject
{
public:
	virtual ~CNpcStateCreatorBase();
	CNpcStateBase* operator() (CNpcAI* pNpc, uint32 uID);
private:
	virtual CNpcStateBase* Create(CNpcAI* pNpc, uint32 uID) = 0;
};

class CNpcStateCreator : public CNpcStateCreatorBase
{
	typedef CNpcStateBase* (*CreateFuncPtr) (CNpcAI *pNpc, uint32 uID);
public:
	CNpcStateCreator(CreateFuncPtr pCreator);
private:
	virtual CNpcStateBase* Create(CNpcAI* pNpc, uint32 uID);
	CreateFuncPtr m_pCreator;
};

class CNpcSubMachineCreator : public CNpcStateCreatorBase
{
public:
	CNpcSubMachineCreator(const CNpcStateTransit* pStateTransit);
private:
	virtual CNpcStateBase* Create(CNpcAI* pNpc, uint32 uID);
	const CNpcStateTransit* m_pStateTransit;
};
