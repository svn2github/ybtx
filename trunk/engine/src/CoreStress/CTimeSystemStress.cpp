#include "stdafx.h"
#include "CTimeSystemStress.h"
#include "TTimeSystem.inl"
#include <ctime>


template class TTimeSystem<CTimeSystemStress>;

CTimeSystemStress::CTimeSystemStress(uint32 uBaseCyc)
:Parent_t(uBaseCyc)
{
}

uint32 CTimeSystemStress::GetGlobalTime()const
{
	return uint32(time(NULL)) - m_nClientServerTimeDiff;
}

void CTimeSystemStress::SetGlobalTime(uint32 uGlobalTime)
{
	m_nClientServerTimeDiff = int32(time(NULL)) - (int32)uGlobalTime;
}
