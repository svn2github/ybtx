#include "stdafx.h"
#include "CTimeSystemClient.h"
#include "TTimeSystem.inl"


template class TTimeSystem<CTimeSystemClient>;

CTimeSystemClient::CTimeSystemClient(uint32 uBaseCyc)
:Parent_t(uBaseCyc)
{
}

