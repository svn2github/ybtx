//------------------------------------------------------------------------------
//  CWaterSurface.cpp
//  (C) 2008 ThreeOGCMan
//------------------------------------------------------------------------------
#include "StdAfx.h"
#include "CWaterSurface.h"
#include "CWaterEffect.h"
#include "CWaterRegionMesh.h"
#include "CCamera.h"
#include "TSqrAllocator.inl"
#include "CWater.h"

namespace sqr
{

//------------------------------------------------------------------------------
CWaterSurface::CWaterSurface()
		: isHide(false)
{
	// empty
}

//------------------------------------------------------------------------------
CWaterSurface::~CWaterSurface()
{
	this->regionMeshSet.clear();
	this->waterEffect = NULL;
}
//------------------------------------------------------------------------------

void CWaterSurface::AddRegionMesh(CWaterRegionMesh* pRegion)
{
	this->regionMeshSet.insert(pRegion);
}

//------------------------------------------------------------------------------
void
CWaterSurface::RemoveRegionMesh(CWaterRegionMesh* pRegion)
{
	this->regionMeshSet.erase(pRegion);
}

//------------------------------------------------------------------------------
void
CWaterSurface::Render(CCamera* pCamera)
{
	if ( this->waterEffect == 0 )
		return;

	if (this->IsHide())
		return;
	
	this->waterEffect->OnFrameUpdate();
	this->waterEffect->OnRenderBegin();
	
	for (RegionMeshSet::iterator it = this->regionMeshSet.begin();
	        it != this->regionMeshSet.end(); ++it)
	{
		if (pCamera->IsVisible((*it)->GetBoundingBox()))
			(*it)->Render();
	}
	this->waterEffect->OnRenderEnd();
}

void CWaterSurface::RenderWave(CCamera* pCamera,ITexture* pTexture ,uint32 uCurTime)
{
	for (RegionMeshSet::iterator it = this->regionMeshSet.begin();
		it != this->regionMeshSet.end(); ++it)
	{
		if (pCamera->IsVisible((*it)->GetBoundingBox()))
		{
			CWaterRegionMesh::WaterPool::iterator wit = (*it)->m_vecWater.begin();
			CWaterRegionMesh::WaterPool::iterator ewit = (*it)->m_vecWater.end();

			for( ;wit!=ewit; ++wit)
			{
				(*wit)->RenderWave( pTexture, uCurTime );
				// dimple
				(*wit)->RenderDimple();
			}
		}
	}
}

CWater* CWaterSurface::GetWater(uint32 OrgId , int32 nGrid)
{
	for (RegionMeshSet::iterator it = this->regionMeshSet.begin();
		it != this->regionMeshSet.end(); ++it)
	{
		CWaterRegionMesh::WaterPool::iterator wit = (*it)->m_vecWater.begin();
		CWaterRegionMesh::WaterPool::iterator ewit = (*it)->m_vecWater.end();

		for( ;wit!=ewit; ++wit)
		{
			if((*wit)->GetOrgID()!=OrgId)
				continue;

			if( (*wit)->IsWater( nGrid ) )
				return (*wit);
		}
	}
	return NULL;
}

}// namespace sqr
