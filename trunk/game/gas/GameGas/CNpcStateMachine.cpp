#include "stdafx.h"
#include "CNpcStateMachine.h"

CNpcStateMachine::CNpcStateMachine(NpcStateMachineTraits::OwnerType* pOwner, const NpcStateTransitTable* pTransitTable)
:TStateMachine<NpcStateMachineTraits>(pOwner, pTransitTable)
{

}

CNpcStateMachine::~CNpcStateMachine()
{

}
