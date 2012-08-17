#pragma once
#include "IPipeModule.h"
#include "CNetworkMallocObject.h"

class CPipeModule
	:public IPipeModule
	,public CNetworkMallocObject
{
public:
	CPipeModule(void);
	~CPipeModule(void);

	IPipeReactor* GetSyncIPipeReactor();
	IPipeReactor* GetAsynIPipeReactor(uint32 uThreadNum);
};
