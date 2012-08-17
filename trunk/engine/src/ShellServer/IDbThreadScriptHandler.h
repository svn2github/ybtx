#pragma once
#include "CallBackX.h"

namespace sqr
{

	class IDbThreadScriptHandler
		: public virtual CDynamicObject
	{
	public:
		virtual void Release() { delete this; }
		virtual void OnSomeQueryJobDone() { CALL_CLASS_CALLBACK(); }
	};

}

