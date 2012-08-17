#include "stdafx.h"
#include "CSyncTimeSystemServer.h"
#include "TTimeSystem.inl"


template class TTimeSystem<CSyncTimeSystemServer>;

CSyncTimeSystemServer::CSyncTimeSystemServer(uint32 uBaseCyc)
:Parent_t(uBaseCyc)
{
}
