#include "stdafx.h"
#include "CAsyAoiDimObj.h"
#include "CAsyAoiViewObj.h"
#include "CAsyAoiSyncerObj.h"
#include "CAsyAoiDimScene.h"
#include "TSqrAllocator.inl"

CAsyAoiDimObj::CAsyAoiDimObj(CAsyAoiDimScene* pScene, CAsyAoiViewObj* pViewObj, const CFPos& pos)
: m_pScene(pScene), m_pViewObj(pViewObj)
, m_fSize(0),m_fEyeSight(0)
, m_fStealth(0),m_fKeenness(0),m_uViewGroup(0)
{
}

CAsyAoiDimObj::~CAsyAoiDimObj()
{
}

void CAsyAoiDimObj::Release()
{
	delete this;
}

void CAsyAoiDimObj::SetSize( float fSize )
{	
	m_fSize = fSize;
}


void CAsyAoiDimObj::SetEyeSight( float fEyeSight )
{
	m_fEyeSight = fEyeSight;
}


void CAsyAoiDimObj::SetStealth( float fStealth )
{
	m_fStealth = fStealth;
}


void CAsyAoiDimObj::SetKeenness(float fKeenness)
{
	m_fKeenness = fKeenness;
}


void CAsyAoiDimObj::SetViewGroup(uint32 uGroupID)
{
	m_uViewGroup = uGroupID;
}

float CAsyAoiDimObj::GetSize() const 
{
	return m_fSize;
}


float CAsyAoiDimObj::GetEyeSight() const
{
	return m_fEyeSight;
}


float CAsyAoiDimObj::GetStealth() const
{
	return m_fStealth;
}


float CAsyAoiDimObj::GetKeenness() const 
{
	return m_fKeenness;
}


uint32 CAsyAoiDimObj::GetViewGroup() const
{
	return m_uViewGroup;
}

