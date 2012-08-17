#pragma once
#include "TCoolDownMgr.h"
#include "CFighterDictator.h"

namespace sqr
{
	class CAppServer;
}

typedef TCoolDownMgr<CAppServer,CFighterDictator>	CCoolDownMgrServer;

