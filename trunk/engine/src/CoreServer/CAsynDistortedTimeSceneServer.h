#pragma once
#include "CDistortedTimeScene.h"

namespace sqr
{
	class CAsynDistortedTimeObjServer;

	class CAsynDistortedTimeSceneServer : public CDistortedTimeScene
	{
	public:
		CAsynDistortedTimeSceneServer(void);
		~CAsynDistortedTimeSceneServer(void);

		CAsynDistortedTimeObjServer* CreateDistortedTimeObj();
	};
}

