#pragma once

#include <map>
#include "BaseTypes.h"
#include "ExpHelper.h"
#include "TStateMachineTraits.h"
#include "TStateTransitItem.h"
#include "TStateTransitTable.h"

namespace sqr
{
	/*
	* =====================================================================================
	*        Class:  TStateMachine
	*  Description:  状态机
	* =====================================================================================
	*/
	template < class Traits >
	class TStateMachine
	{
	public:
		typedef TStateTransitTable<Traits>	TransitTableType;
		typedef TStateTransitItem<Traits>	TransitItemType;

		/* ====================  LIFECYCLE     ======================================= */
		TStateMachine (typename Traits::OwnerType* Owner, const TransitTableType* Table)
			: m_CurrentState(NULL)
			, m_Owner(Owner)
			, m_Table(Table)
			, m_bSuspended(false)
		{
		}/* constructor */

		/* ====================  OPERATIONS    ======================================= */
		typename Traits::EventType* OnEvent(typename Traits::EventType *Event)
		{
			Ast(IsRunning());
			if (IsSuspended())
				return Event;
			TransitItemType* Item = m_Table->GetTransitItem(m_CurrentState->GetID());
			Ast (Item);
			typename Traits::StateType* NextState = Item->CreateNextState(GetOwner(), Event->GetID());
			if (!NextState)		//不是跳转消息则响应，否则直接跳转
			{
				m_CurrentState->OnEvent(Event);
				return Event;
			}
			Event = m_CurrentState->LeaveState(Event);
			delete m_CurrentState;
			m_CurrentState = NextState;
			Event = m_CurrentState->CheckValid(Event);
			Event = m_CurrentState->EnterState(Event);
			return Event;
		}

		typename Traits::EventType* Start(typename Traits::EventType* Event)
		{
			Ast(!IsRunning());
			m_CurrentState = m_Table->CreateEntryState(GetOwner());
			Event = m_CurrentState->EnterState(Event);
			return Event;
		}

		typename Traits::EventType* Stop(typename Traits::EventType* Event)
		{
			Ast(IsRunning());
			if (IsSuspended())
				Event = Resume(Event);
			Event = m_CurrentState->LeaveState(Event);
			delete m_CurrentState;
			m_CurrentState = NULL;
			return Event;
		}

		typename Traits::EventType* Suspend(typename Traits::EventType* Event)
		{
			Ast(!IsSuspended());
			Event = GetCurrentState()->Suspend(Event);
			m_bSuspended = true;
			return Event;
		}

		typename Traits::EventType* Resume(typename Traits::EventType* Event)
		{
			Ast(IsSuspended());
			Event = GetCurrentState()->Resume(Event);
			m_bSuspended = false;
			return Event;
		}
		/* ====================  INQUIRY       ======================================= */
		inline bool IsRunning()
		{
			return m_CurrentState != NULL;
		}
		inline bool IsSuspended()
		{
			return m_bSuspended;
		}

		/* ====================  ACCESS        ======================================= */
		inline typename Traits::OwnerType* GetOwner() const
		{
			return m_Owner;
		}
		inline typename Traits::StateType* GetCurrentState() const
		{
			return m_CurrentState;
		}

	private:
		typename Traits::StateType* m_CurrentState;
		typename Traits::OwnerType* m_Owner;
		const TransitTableType* m_Table;
		bool m_bSuspended;
	}; /* ----------  end of template class  TStateMachine  ---------- */
}

