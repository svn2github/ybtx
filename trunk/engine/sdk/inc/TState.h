#pragma once

#include <map>
#include "BaseTypes.h"
#include "ExpHelper.h"

#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable:4290)
#endif
namespace sqr
{
	/*
	* =====================================================================================
	*        Class:  TState
	*  Description:  状态基类,所有NpcAI状态都是继承于此基类，并重载虚函数
	* =====================================================================================
	*/
	template < class OwnerTypeArg, class EventTypeArg, class IDTypeArg = uint32 >
	class TState
	{
	public:
		typedef OwnerTypeArg OwnerType;
		typedef EventTypeArg EventType;
		typedef IDTypeArg IDType;

		/* ====================  LIFECYCLE     ======================================= */
		TState(OwnerType* Owner, IDType ID)
			: m_Owner(Owner)
			, m_ID(ID)
		{
		}/* constructor */

		virtual ~TState() { }

		/* ====================  OPERATIONS    ======================================= */
		virtual EventType* OnEvent(EventType* pEvent) = 0;
		virtual EventType* EnterState(EventType* pEvent) = 0;
		virtual EventType* LeaveState(EventType* pEvent) = 0;
		virtual EventType* Suspend(EventType* pEvent) = 0;
		virtual EventType* Resume(EventType* pEvent) = 0;
		virtual EventType* CheckValid(EventType* pEvent) = 0;

		/* ====================  ACCESS        ======================================= */
		inline OwnerType* GetOwner() const
		{
			return m_Owner;
		}
		inline IDType GetID() const
		{
			return m_ID;
		}

	private:
		/* ====================  DATA MEMBERS   ======================================= */
		OwnerType*	m_Owner;
		IDType		m_ID;

	}; /* ----------  end of template class  TState  ---------- */
}

