#include "stdafx.h"
#include "TimeHelper.h"
#include "CNpcAINature.h" 
#include "Random.h"

CNpcNature::CNpcNature(const string& CNatureArg, CNpcAI* pAI)
:m_sArg(string(CNatureArg))
,m_pAI(pAI)
{
}

CNpcEventMsg* CNpcNature::DoEvent(CNpcEventMsg* pEvent)
{
	return NULL;
}

void CNpcNature::SetEvent(ENpcEvent eEvent, uint32 uTag)
{
	m_eEvent = eEvent;
	m_uTag = uTag;
}


//class Attaract
bool CNpcNatureAttract::Interest(uint32 uEventID)
{
	return (uEventID == eNpcEvent_OnObjectInSight);
}

CNpcEventMsg* CNpcNatureAttract::DoEvent(CNpcEventMsg* pEvent)
{
	if (GetEvent() == eNpcEvent_OnObjectInSight)
		return CNpcEventMsg::Create(m_pAI, eNpcEvent_OnAttract, eNECI_Zero, reinterpret_cast<void*>(GetTag()));

	return NULL;
}

//class CNpcNatureLike
bool CNpcNatureLike::Interest(uint32 uEventID)
{
	return (uEventID == eNpcEvent_OnObjectInSight);
}

CNpcEventMsg* CNpcNatureLike::DoEvent(CNpcEventMsg* pEvent)
{
	if (GetEvent() == eNpcEvent_OnObjectInSight)
		return CNpcEventMsg::Create(m_pAI, eNpcEvent_OnLike, eNECI_Zero, reinterpret_cast<void*>(GetTag()));
	return NULL;
}

//==============================================
//class CNpcNatureAbhor
bool CNpcNatureAbhor::Interest(uint32 uEventID)
{
	return (uEventID == eNpcEvent_OnObjectInSight);
}

CNpcEventMsg* CNpcNatureAbhor::DoEvent(CNpcEventMsg* pEvent)
{
	if (GetEvent() == eNpcEvent_OnObjectInSight)
		return CNpcEventMsg::Create(m_pAI, eNpcEvent_OnAbhor, eNECI_Zero, reinterpret_cast<void*>(GetTag()));
	return NULL;
}


//class CNpcNatureCoward
bool CNpcNatureCoward::Interest(uint32 uEventID)
{
	return (uEventID == eNpcEvent_OnOtherDead);
}

CNpcEventMsg* CNpcNatureCoward::DoEvent(CNpcEventMsg* pEvent)
{
	if ((uint32)Random::Rand(0,99) > this->GetExtraArg())
		return NULL;
	if (GetEvent()== eNpcEvent_OnOtherDead)
	{
		return CNpcEventMsg::Create(m_pAI, eNpcEvent_OnCoward, eNECI_Zero, reinterpret_cast<void*>(GetTag()));
	}
	return NULL;
}



//class CNpcNatureSleey
bool CNpcNatureSleepy::Interest(uint32 uEventID)
{
	return (uEventID == eNpcEvent_OnSleepyIdle);
}

CNpcEventMsg* CNpcNatureSleepy::DoEvent(CNpcEventMsg* pEvent)
{
	if (GetEvent() == eNpcEvent_OnSleepyIdle)
	{
		return CNpcEventMsg::Create(m_pAI, eNpcEvent_OnSleepy, eNECI_Zero, reinterpret_cast<void*>(GetTag()));
	}
	return NULL;
}

//class CNpcNatureGreet
bool CNpcNatureGreet::Interest(uint32 uEventID)
{
	return (uEventID == eNpcEvent_OnNpcInSight);
}

CNpcEventMsg* CNpcNatureGreet::DoEvent(CNpcEventMsg* pEvent)
{
	if (GetEvent() == eNpcEvent_OnNpcInSight)
		return CNpcEventMsg::Create(m_pAI, eNpcEvent_OnGreet, eNECI_Zero, reinterpret_cast<void*>(GetTag()));
	return NULL;
}

