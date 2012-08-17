#pragma once
#include "TBaseCOEvent.h"
#include "CTraitsClient.h"

namespace sqr
{
	class CCOEventWayPointChanged
		:public TBaseCOEvent<CTraitsClient>
	{
	public:
		CCOEventWayPointChanged(CCoreObjectClient* pObj);
	private:
		~CCOEventWayPointChanged();
	};

}

