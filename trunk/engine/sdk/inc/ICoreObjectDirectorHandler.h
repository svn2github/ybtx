#pragma once
//$id$

#include "ICoreObjectFollowerHandler.h"

namespace sqr
{
	class ICoreObjectDirectorHandler
		:public ICoreObjectFollowerHandler
	{
	public:
		virtual ICoreObjectDirectorHandler* CastToObjDirHandler()const
		{
			return const_cast<ICoreObjectDirectorHandler*>(this);
		}
	};
}