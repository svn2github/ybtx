#pragma once

#include "BaseTypes.h"

namespace sqr
{
	/*
	* =====================================================================================
	*        Class:  TStateEvent
	*  Description:  事件基类
	* =====================================================================================
	*/
	template < class EventIDType = uint32 >
	class TStateEvent
	{
	public:
		typedef EventIDType IDType;

		/* ====================  LIFECYCLE     ======================================= */
		TStateEvent(EventIDType ID)
			:m_ID(ID)
		{
		}/* constructor */

		virtual ~TStateEvent() { }

		/* ====================  ACCESS        ======================================= */
		inline EventIDType GetID() const
		{
			return m_ID;
		}

	private:
		EventIDType m_ID;

	}; /* ----------  end of template class  TStateEvent  ---------- */
}
