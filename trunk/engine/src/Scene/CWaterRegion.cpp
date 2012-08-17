#include "stdafx.h"
#include "CWaterRegion.h"
#include "BaseHelper.h"
#include "CWaterSurface.h"
#include "CWaterEffect.h"
#include "TSqrAllocator.inl"

CWaterRegion::CWaterRegion(CElementManager* pParent , URegionID id)
: CElementNode(pParent,id)
{

}

CWaterRegion::~CWaterRegion()
{
	RegionMeshMap::iterator it,eit = m_MeshList.end();
	for(  it=m_MeshList.begin(); it!=eit; ++it )
	{
		it->first.first->RemoveRegionMesh(it->second);
		GfkSafeDelete(it->second);
	}
}

bool CWaterRegion::IsHasMesh(CWaterSurface* pSurface,uint32 RegionId)
{
	MeshKey key = make_pair(pSurface,RegionId);
	RegionMeshMap::iterator it = m_MeshList.find(key);
	return it!=m_MeshList.end();
}

CWaterRegionMesh*	CWaterRegion::GetMesh(CWaterSurface* pSurface,uint32 RegionId)
{
	MeshKey key = make_pair(pSurface,RegionId);
	RegionMeshMap::iterator it = m_MeshList.find(key);
	if(it!=m_MeshList.end())
		return it->second;

	CWaterRegionMesh* pMesh = new CWaterRegionMesh;
	m_MeshList.insert( RegionMeshMap::value_type(key,pMesh) );
	pSurface->AddRegionMesh(pMesh);
	return pMesh;
}

void CWaterRegion::ReleaseMesh(CWaterSurface* pSurface,uint32 RegionId)
{
	MeshKey key = make_pair(pSurface,RegionId);
	RegionMeshMap::iterator it = m_MeshList.find(key);
	if(it!=m_MeshList.end())
	{
		pSurface->RemoveRegionMesh(it->second);
		GfkSafeDelete(it->second);
		m_MeshList.erase(it);
	}
}