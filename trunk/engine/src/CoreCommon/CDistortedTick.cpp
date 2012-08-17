#include "stdafx.h"
#include "CDistortedTick.h"
#include "CTick.h"
#include "CDistortedTimeObj.h"
#include "CallBackX.h"
#include "TSqrAllocator.inl"

namespace sqr
{
	class CDistortedTickImp 
		: public CTick
		, public CDistortedTimeMallocObject
	{
	public:
		CDistortedTickImp(CDistortedTick* pDistortedTick):m_pDistortedTick(pDistortedTick){}
		~CDistortedTickImp(void){}
		virtual void OnTick()
		{
			IDistortedTime* pDistortedTime = m_pDistortedTick->m_pDistortedTime;
			if(pDistortedTime)
			{
				if(m_pDistortedTick->m_bIntervalNeedToBeRefreshed)
					static_cast<CDistortedTimeObj*>(pDistortedTime)->RefreshTickInterval(m_pDistortedTick);
			}
			m_pDistortedTick->OnTick();
		}

		const char* GetClassName()const
		{
			return typeid(*m_pDistortedTick).name();
		}

	private:
		CDistortedTick*	m_pDistortedTick;
	};
}

CDistortedTick::CDistortedTick()
:m_pTick(NULL)
,m_pDistortedTime(NULL)
,m_uKeyOfDistortedTickMap(uint32(-1))
,m_uTickCycInRealTime(uint32(-1))
,m_bIntervalNeedToBeRefreshed(false)
{
	m_pTick = new CDistortedTickImp(this);
}

CDistortedTick::~CDistortedTick(void)
{
	if(m_pDistortedTime)
	{
		m_pDistortedTime->UnregistDistortedTick(this);	
	}

	delete m_pTick;
	m_pTick = NULL;
}

bool CDistortedTick::Registered() const
{
	return m_pTick->Registered();
}

void CDistortedTick::SetDistortedTime(IDistortedTime* pDistortedTime)
{
	m_pDistortedTime = pDistortedTime;
}

void CDistortedTick::SetTickName(const char* szName)
{
	m_pTick->SetTickName(szName);
}

void CDistortedTick::OnTick()
{
	CALL_CLASS_CALLBACK()
}

uint32 CDistortedTick::GetInterval()const
{
	if(m_pDistortedTime)
	{
		float fTimeRatio = static_cast<CDistortedTimeObj*>(m_pDistortedTime)->GetTimeRatio();
		return uint32(fTimeRatio * m_pTick->GetInterval());
	}
	return m_pTick->GetInterval();
}

uint32 CDistortedTick::GetLeftTime()const
{
	if(m_pDistortedTime)
	{
		float fTimeRatio = static_cast<CDistortedTimeObj*>(m_pDistortedTime)->GetTimeRatio();
		return uint32(fTimeRatio * m_pTick->GetLeftTime());
	}
	return m_pTick->GetLeftTime();
}
