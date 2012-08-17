#include "stdafx.h"
#include "CNpcState.h"
#include "CNpcEventMsg.h"
#include "CNpcAI.h"
#include "CNpcStateMachine.h"
#include "CNpcStateTransit.h"

CNpcStateBase::CNpcStateBase(CNpcAI* pNpc, uint32 uID) : BaseType(pNpc, uID)
{
}

CNpcEventMsg* CNpcStateBase::EnterState(CNpcEventMsg* pEvent)
{
	return pEvent;
}

CNpcEventMsg* CNpcStateBase::LeaveState(CNpcEventMsg* pEvent)
{
	return pEvent;
}

CNpcEventMsg* CNpcStateBase::Suspend(CNpcEventMsg* pEvent)
{
	return LeaveState(pEvent);
}

CNpcEventMsg* CNpcStateBase::Resume(CNpcEventMsg* pEvent)
{
	return EnterState(pEvent);
}

CNpcEventMsg* CNpcStateBase::OnEvent(CNpcEventMsg* pEvent)
{
	return pEvent;
}

CNpcEventMsg* CNpcStateBase::CheckValid(CNpcEventMsg* pEvent)
{
	GetOwner()->CheckStateMachineValid(pEvent);
	return pEvent;
}

CNpcStateCreatorBase::~CNpcStateCreatorBase()
{
}

CNpcStateBase* CNpcStateCreatorBase::operator() (CNpcAI* pNpc, uint32 uID)
{
	return Create(pNpc, uID);
}

CNpcStateCreator::CNpcStateCreator(CreateFuncPtr pCreator)
:m_pCreator(pCreator)
{
}

CNpcStateBase* CNpcStateCreator::Create(CNpcAI* pNpc, uint32 uID)
{
	return m_pCreator(pNpc, uID);
}

CNpcSubMachineCreator::CNpcSubMachineCreator(const CNpcStateTransit* pStateTransit)
{
	m_pStateTransit = pStateTransit;
}

CNpcStateBase* CNpcSubMachineCreator::Create(CNpcAI* pNpc, uint32 uID)
{
	CNpcStateMachine *pStateMachine = new CNpcStateMachine(pNpc, m_pStateTransit);
	pNpc->PushSubMachine(pStateMachine, CNpcEventMsg::Create(pNpc, eNpcEvent_OnSuspend));
	return NULL;
}

