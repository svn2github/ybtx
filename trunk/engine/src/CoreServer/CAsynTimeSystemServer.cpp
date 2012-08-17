#include "stdafx.h"
#include "CAsynTimeSystemServer.h"
#include "TTimeSystem.inl"


template class TTimeSystem<CAsynTimeSystemServer>;

CAsynTimeSystemServer::CAsynTimeSystemServer(uint32 uBaseCyc)
:Parent_t(uBaseCyc)
{
}
