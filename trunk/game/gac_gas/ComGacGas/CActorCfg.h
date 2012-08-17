#pragma once
#include "ActorDef.h"
#include "CConfigMallocObject.h"
#include "TConfigAllocator.h"

class CActorCfg : public CConfigMallocObject
{
public:
	static void BuildMapString2Enum();
	static void CleanMap();
	
	static EActionState GetEnumByString(const TCHAR* strAct);

private:
	typedef map<string, EActionState, less<string>, TConfigAllocator<pair<string, EActionState > > > MapActionState;
	static MapActionState ms_mapActionState;
};
