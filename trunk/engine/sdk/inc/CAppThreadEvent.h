#pragma once

namespace sqr
{
	class CAppThreadEvent
	{
		friend class CCoreThreadMsgAppEvent;
	protected:
		CAppThreadEvent();
		virtual ~CAppThreadEvent();
	};
}
