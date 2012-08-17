#include "stdafx.h"
#include "CDistortedTimeSceneClient.h"
#include "CDistortedTimeObjClient.h"

CDistortedTimeSceneClient::CDistortedTimeSceneClient(void)
{
}

CDistortedTimeSceneClient::~CDistortedTimeSceneClient(void)
{
}

CDistortedTimeObjClient* CDistortedTimeSceneClient::CreateDistortedTimeObj()
{
	return new CDistortedTimeObjClient(this);
}