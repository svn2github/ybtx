#pragma once 

#include "CCoreMsg.h"

namespace sqr
{
	class CAppEvent;

	class CCoreMsgAppEvent
		: public CCoreMsg
	{
	public:
		CCoreMsgAppEvent(CAppEvent* pEvent);
		
	private:
		~CCoreMsgAppEvent();
		
		CAppEvent*	m_pEvent;
	};
}

