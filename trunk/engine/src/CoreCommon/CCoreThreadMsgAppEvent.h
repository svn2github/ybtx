#pragma once 

#include "CCoreThreadMsg.h"

namespace sqr
{
	class CAppThreadEvent;

	class CCoreThreadMsgAppEvent
		:public CCoreThreadMsg
	{
	public:
		CCoreThreadMsgAppEvent(CAppThreadEvent* pEvent);
	private:
		~CCoreThreadMsgAppEvent();

		CAppThreadEvent*	m_pEvent;
	};
}
