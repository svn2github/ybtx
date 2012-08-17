#pragma once
#include "ICoreObjectDictatorHandler.h"

namespace sqr
{
	class ICoreObjectMediatorHandler
		:public ICoreObjectDictatorHandler
	{
	public:
		virtual void OnCaughtViewSightOf(uint32 uObjGlobalId, uint32 uDimension) {}

		virtual void OnConnectionDisbinded(){}

		virtual ICoreObjectMediatorHandler* CastToObjMedHandler()const
		{
			return const_cast<ICoreObjectMediatorHandler*>(this);
		}
	};
}


