#pragma once
#include "TBaseCOEvent.h"
#include "CTraitsClient.h"

namespace sqr
{
	class CCoreObjectFollower;

	class CCOEventPositionSet
		:public TBaseCOEvent<CTraitsClient>
	{
	public:
		CCOEventPositionSet(CCoreObjectFollower* pObj);
	private:
		~CCOEventPositionSet();
	};

}

