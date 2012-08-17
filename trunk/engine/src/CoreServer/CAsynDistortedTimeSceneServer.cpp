#include "stdafx.h"
#include "CAsynDistortedTimeSceneServer.h"
#include "CAsynDistortedTimeObjServer.h"

CAsynDistortedTimeSceneServer::CAsynDistortedTimeSceneServer(void)
{
}

CAsynDistortedTimeSceneServer::~CAsynDistortedTimeSceneServer(void)
{
}

CAsynDistortedTimeObjServer* CAsynDistortedTimeSceneServer::CreateDistortedTimeObj()
{
	return new CAsynDistortedTimeObjServer(this);
}

