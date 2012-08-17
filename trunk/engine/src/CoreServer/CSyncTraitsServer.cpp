#include "stdafx.h"
#include "CSyncTraitsServer.h"
#include "CoreObjectServerCommon.h"

namespace sqr
{
	uint32 CSyncTraitsServer::GetDefaultFollowerDelay()
	{
		return 200;
	}

	uint32 CSyncTraitsServer::GetObjectMoveCyc()
	{
		return CSyncTraitsServer::GetDefaultFollowerDelay();
	}
}
