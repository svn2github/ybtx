#pragma once
//$id$

#include "ICoreObjectServerHandler.h"


namespace sqr
{
	class ICoreObjectCalculatorHandler
		:public ICoreObjectServerHandler
	{
	public:
		virtual ICoreObjectCalculatorHandler* CastToObjCalHandler()const
		{
			return const_cast<ICoreObjectCalculatorHandler*>(this);
		}
	};
}
