#pragma once

#include "TStateMachine.h"
#include "CNpcState.h"
#include "CNpcEventMsg.h"
#include "CNpcAIMallocObject.h"

class CNpcAI;

//Npc×´Ì¬»ú
typedef TStateMachineTraits<CNpcAI, CNpcEventMsg, CNpcStateBase, CNpcStateCreatorBase*> NpcStateMachineTraits;
typedef TStateTransitTable<NpcStateMachineTraits> NpcStateTransitTable;

class CNpcStateMachine
	: public CNpcAIMallocObject
	, public TStateMachine<NpcStateMachineTraits>
{
public:
	CNpcStateMachine(NpcStateMachineTraits::OwnerType* pOwner, const NpcStateTransitTable* pTransitTable);
	~CNpcStateMachine();
};
