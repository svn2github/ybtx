//$Id: CTickImp.cpp 54290 2007-04-02 08:19:54Z shhuang $
#include "stdafx.h"
#include "CTickImp.h"
#include "CTickSlot.h"
#include "CTickMgr.h"
#include "ErrLogHelper.h"
#include "ExpHelper.h"
#include "TSqrAllocator.inl"

CTickImp::CTickImp()
:m_uKeyOfTickMap(uint32(-1))
{
}

CTickImp::~CTickImp()
{
	UnRegister();
}

void CTickImp::UnRegister()
{
	m_pTickMgr->UnRegister( m_pTick );
}

CTickSlot*	CTickImp::GetSlot()const
{
	return m_pTickMgr->m_vecSlot[m_uSlotPos];
}

void CTickImp::AddToSlot(uint16 uSlotPos,uint32 uRing)
{
	m_uSlotPos = uSlotPos;
	m_uRing = uRing;
	m_pTickMgr->m_vecSlot[uSlotPos]->AddTick( this );
}

void CTickImp::DelFrSlot()
{
	m_pTickMgr->m_vecSlot[m_uSlotPos]->DelTick(this);
}

uint32 CTickImp::GetInterval()const
{
	return m_uInterval;
}

uint32 CTickImp::GetLeftTime()const
{
	const size_t uSlotNumInRing = m_pTickMgr->m_vecSlot.size();
	
	const uint32 uSlotDist = (m_uRing - m_pTickMgr->m_uRing) * uSlotNumInRing + m_uSlotPos - m_pTickMgr->m_uCurPos;

	return uSlotDist * m_pTickMgr->m_uInterval;
}

void CTickImp::SetTickName(const char* szName)
{
	m_sTickName = szName;
}

const char* CTickImp::GetTickName()const
{
	if(m_sTickName.empty())
		return m_szClassName;
	return m_sTickName.c_str();
}

