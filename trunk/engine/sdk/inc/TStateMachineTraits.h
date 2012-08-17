#pragma once

#include "BaseTypes.h"

namespace sqr
{
	/*
	* =====================================================================================
	*        Class:  TStateMachineTraits
	*  Description:  ×´Ì¬»úTraits
	* =====================================================================================
	*/
	template < class OwnerTypeArg,
	class EventTypeArg,
	class StateTypeArg,
		typename StateCreatorTypeArg = StateTypeArg* (*)(OwnerTypeArg*, typename StateTypeArg::IDType) >
	class TStateMachineTraits
	{
	public:
		/* ====================  TypeDefs     ======================================= */
		typedef OwnerTypeArg                    OwnerType;
		typedef EventTypeArg                    EventType;
		typedef StateTypeArg                    StateType;
		typedef typename EventTypeArg::IDType   EventIDType;
		typedef typename StateTypeArg::IDType   StateIDType;
		typedef StateCreatorTypeArg             StateCreatorType;

	}; /* ----------  end of template class  TStateMachineTraits  ---------- */
}

