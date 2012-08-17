#pragma once
//$id$

#include "ICoreObjectClientHandler.h"

namespace sqr
{
	class ICoreObjectPerformerHandler
		:public ICoreObjectClientHandler
	{
	public:
		virtual ICoreObjectPerformerHandler* CastToObjPerHandler()const
		{
			return const_cast<ICoreObjectPerformerHandler*>(this);
		}
	};
}