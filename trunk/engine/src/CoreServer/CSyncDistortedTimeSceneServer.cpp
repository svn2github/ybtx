#include "stdafx.h"
#include "CSyncDistortedTimeSceneServer.h"
#include "CSyncDistortedTimeObjServer.h"

CSyncDistortedTimeSceneServer::CSyncDistortedTimeSceneServer(void)
{
}

CSyncDistortedTimeSceneServer::~CSyncDistortedTimeSceneServer(void)
{
}

CSyncDistortedTimeObjServer* CSyncDistortedTimeSceneServer::CreateDistortedTimeObj()
{
	return new CSyncDistortedTimeObjServer(this);
}

