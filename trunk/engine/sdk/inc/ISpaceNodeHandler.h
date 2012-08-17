#pragma once
#include "CDynamicObject.h"
#include "CallBackX.h"

namespace sqr
{
	class ISpaceNodeHandler : public virtual CDynamicObject
	{
	public:
		virtual void OnDestroyed() { CALL_CLASS_CALLBACK() }
	};
};
