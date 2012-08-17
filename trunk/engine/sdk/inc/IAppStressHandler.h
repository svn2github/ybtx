#pragma once

#include "TAppHandler.h"
#include "CTraitsStress.h"
#include "CallBackX.h"

namespace sqr
{
	class IAppStressHandler
		:public TAppHandler<CTraitsStress>
	{
	public:
		virtual void OnClose() { CALL_CLASS_CALLBACK() };
		virtual ~IAppStressHandler(){};
	};
}

