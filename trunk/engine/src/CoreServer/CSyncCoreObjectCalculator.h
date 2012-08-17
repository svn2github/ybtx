#pragma once
#include "CSyncCoreObjectServer.h"

namespace sqr
{
	class CSyncCoreObjectCalculator
		:public CSyncCoreObjectServer
	{
	public:
		CSyncCoreObjectCalculator(uint32 uID, CSyncCoreSceneServer* pScene, const CFPos& PixelPos);

		virtual EServerObjectType GetType()const;
	};
}
