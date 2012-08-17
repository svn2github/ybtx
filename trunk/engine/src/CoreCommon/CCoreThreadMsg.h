#pragma once
#include "CCoreMsgMallocObject.h"

namespace sqr
{
	class CCoreThreadMsg : public CCoreMsgMallocObject
	{
	protected:
		CCoreThreadMsg(void);
	public:
		virtual ~CCoreThreadMsg(void);
	};
}

