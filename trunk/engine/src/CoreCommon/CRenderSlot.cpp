#include "stdafx.h"
#include "CRenderSlot.h"
#include "ExpHelper.h"

CRenderSlot::CRenderSlot()
:m_uSlotIndex(uint32(-1))
,m_pRenderSlotOwner(NULL)
{
}

CRenderSlot::~CRenderSlot()
{
	if(m_pRenderSlotOwner)
		m_pRenderSlotOwner->Delete(this);
}

CRenderSlotOwner::CRenderSlotOwner()
:m_uCurUpdateIndex(uint32(-1))
{
}

CRenderSlotOwner& CRenderSlotOwner::Inst()
{
	static CRenderSlotOwner ms_Inst;
	return ms_Inst;
}

void CRenderSlotOwner::Update(uint64 uUsedTime)
{
	UpdateFromSlot(uUsedTime, 0);
}

void CRenderSlotOwner::UpdateFromSlot(uint64 uUsedTime, uint32 uSlotIndex)
{
	m_uCurUpdateIndex = uSlotIndex;

	for(; m_uCurUpdateIndex < m_vecRenderSlot.size(); ++m_uCurUpdateIndex)
	{
		CRenderSlot* pRenderSlot = m_vecRenderSlot[m_uCurUpdateIndex];
		if(pRenderSlot)
		{
			pRenderSlot->OnUpdate(uUsedTime);
		}
		else
		{
			while(m_uCurUpdateIndex < m_vecRenderSlot.size())
			{
				pRenderSlot = m_vecRenderSlot[m_vecRenderSlot.size()-1];
				m_vecRenderSlot.pop_back();

				if(pRenderSlot)
				{
					pRenderSlot->m_uSlotIndex = m_uCurUpdateIndex;
					m_vecRenderSlot[m_uCurUpdateIndex] = pRenderSlot;
					pRenderSlot->OnUpdate(uUsedTime);
					break;
				}
			}
		}
	}

	m_uCurUpdateIndex = uint32(-1);
}

void CRenderSlotOwner::PrintSlot()const
{
	uint32 uSize = m_vecRenderSlot.size();
	for(uint32 u = 0; u < uSize; ++u)
		cout << u << "(" << m_vecRenderSlot[u] << ")" << endl;
}

void CRenderSlotOwner::Defrag()
{
	int32 iIndex = m_vecRenderSlot.size()-1;
	for(; iIndex >= 0; --iIndex)
	{
		CRenderSlot* pRenderSlot = m_vecRenderSlot[iIndex];
		if(!pRenderSlot)
		{
			pRenderSlot = m_vecRenderSlot[m_vecRenderSlot.size()-1];
			m_vecRenderSlot.pop_back();
			if(pRenderSlot)
			{
				m_vecRenderSlot[iIndex] = pRenderSlot;
				pRenderSlot->m_uSlotIndex = iIndex;
			}
		}
	}
}

void CRenderSlotOwner::Insert(CRenderSlot* pRenderSlot)
{
	if(!pRenderSlot)
		return;

	if(!pRenderSlot->m_pRenderSlotOwner)
	{
		m_vecRenderSlot.push_back(pRenderSlot);
		pRenderSlot->m_uSlotIndex = m_vecRenderSlot.size() - 1;
		pRenderSlot->m_pRenderSlotOwner = this;
		return;
	}
	Ast(pRenderSlot->m_pRenderSlotOwner == this);
}

bool CRenderSlotOwner::Delete(CRenderSlot* pRenderSlot)
{
	if(!pRenderSlot)
		return false;

	CRenderSlotOwner* pOwner = pRenderSlot->m_pRenderSlotOwner;
	if(!pOwner)
		return false;

	Ast(pOwner == this);

	if(pOwner == this)
	{
		uint32 uSlotIndex = pRenderSlot->m_uSlotIndex;
		m_vecRenderSlot[uSlotIndex] = NULL;
		pRenderSlot->m_pRenderSlotOwner = NULL;
		return true;
	}

	return false;
}

uint32 CRenderSlotOwner::Size()const
{
	return uint32(m_vecRenderSlot.size());
}
