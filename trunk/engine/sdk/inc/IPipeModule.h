#pragma once

#include "IPipe.h"
#include "IPipeReactor.h"
#include "NetworkDefs.h"

namespace sqr
{
	enum EPipeType
	{
		ePT_Sync = 1,
		ePT_Asyn
	};

	class IPipeModule
	{
	public:
		virtual IPipeReactor* GetSyncIPipeReactor()=0;
		virtual IPipeReactor* GetAsynIPipeReactor(uint32 uThreadNum)=0;
		//virtual void ReleaseIPipeReactor(IPipeReactor*)=0;
		virtual ~IPipeModule(){}
	};

	NETWORK_API IPipeModule* GetIPipeModule();
	NETWORK_API void ReleaseIPipeModule(IPipeModule* pModule);

};
