#pragma once
#include "CDynamicObject.h"
#include "CRefObject.h"

namespace sqr
{
	class IRenderTime
		:public CRefObject
		,public virtual CDynamicObject
	{
	public:
		virtual ~IRenderTime(){};
		virtual uint32	GetCurTime(void) = 0;
	};
}