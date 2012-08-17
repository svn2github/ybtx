#pragma once
#include "CDistortedTimeScene.h"

namespace sqr
{
	class CSyncDistortedTimeObjServer;

	class CSyncDistortedTimeSceneServer : public CDistortedTimeScene
	{
	public:
		CSyncDistortedTimeSceneServer(void);
		~CSyncDistortedTimeSceneServer(void);

		CSyncDistortedTimeObjServer* CreateDistortedTimeObj();
	};
}

