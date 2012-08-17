#pragma once
#include "CDistortedTimeScene.h"

namespace sqr
{
	class CDistortedTimeObjClient;

	class CDistortedTimeSceneClient : public CDistortedTimeScene
	{
	public:
		CDistortedTimeSceneClient(void);
		~CDistortedTimeSceneClient(void);

		CDistortedTimeObjClient* CreateDistortedTimeObj();
	};
}
