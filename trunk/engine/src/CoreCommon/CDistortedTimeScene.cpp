#include "stdafx.h"
#include "CDistortedTimeScene.h"
#include "CDistortedTimeObj.h"
#include "ExpHelper.h"
#include "BaseHelper.h"
#include "TSqrAllocator.inl"

CDistortedTimeScene::CDistortedTimeScene(void)
:m_fTimeDistortedRatio(1)
,m_uTimeGroup(0)
,m_bInDestroying(false)
{
}

CDistortedTimeScene::~CDistortedTimeScene()
{
}

void CDistortedTimeScene::Release()
{
	if(m_DistortedTimeObjContainer.empty())
	{
		delete this;
		return;
	}
	else
	{
		m_bInDestroying = true;
	}
}

void CDistortedTimeScene::DestroyDistortedTimeObj(CDistortedTimeObj* pObj)
{
#ifdef _WIN32
	SafeRelease(pObj);
#else
	delete pObj;
#endif
}

void CDistortedTimeScene::SetTimeGroup(uint32 uTimeGroup)
{
	if(m_uTimeGroup == uTimeGroup)
		return;

	m_uTimeGroup = uTimeGroup;

	if(m_fTimeDistortedRatio != 1)
	{
		DistortedTimeObjContainer::iterator it = m_DistortedTimeObjContainer.begin();
		for(; it != m_DistortedTimeObjContainer.end(); ++it)
		{
			if((*it)->GetTimeGroup() != GetTimeGroup())
			{
				(*it)->OnTimeRatioChanged(m_fTimeDistortedRatio);
			}
		}
	}
}

uint32 CDistortedTimeScene::GetTimeGroup()const
{
	return m_uTimeGroup;
}

void CDistortedTimeScene::SetTimeDistortedRatio(float fTimeDistortedRatio)
{
	if(m_fTimeDistortedRatio == fTimeDistortedRatio)
		return;

	float fOldTimeRatio = m_fTimeDistortedRatio;
	m_fTimeDistortedRatio = fTimeDistortedRatio;

	DistortedTimeObjContainer::iterator it = m_DistortedTimeObjContainer.begin();
	for(; it != m_DistortedTimeObjContainer.end(); ++it)
	{
		if((*it)->GetTimeGroup() != GetTimeGroup())
		{
			(*it)->OnTimeRatioChanged(fOldTimeRatio);
		}
	}
}

float CDistortedTimeScene::GetTimeDistortedRatio()const
{
	return m_fTimeDistortedRatio;
}

void CDistortedTimeScene::AddDistortedTimeObj(CDistortedTimeObj* pObj)
{
	m_DistortedTimeObjContainer.push_front(pObj);
	pObj->m_itDistortedTimeObjList = m_DistortedTimeObjContainer.begin();
}

void CDistortedTimeScene::DelDistortedTimeObj(CDistortedTimeObj* pObj)
{
	Ast(pObj->m_pDistortedTimeScene == this);
	m_DistortedTimeObjContainer.erase(pObj->m_itDistortedTimeObjList);
}

void CDistortedTimeScene::OnDistortedTimeObjDestroyed()
{
	if(!m_bInDestroying)
		return;

	if(m_DistortedTimeObjContainer.empty())
	{
		delete this;
		return;
	}
}
