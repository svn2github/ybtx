#pragma once
#include "CWaterRegionMesh.h"
#include "CElementManager.h"
#include "CSceneMallocObject.h"

namespace sqr
{
	class CWaterRegion 
		: public CElementNode
	{
	public:
		CWaterRegion(CElementManager* pParent , URegionID id);
		~CWaterRegion();

		bool				IsHasMesh(CWaterSurface* pSurface,uint32 RegionId);
		CWaterRegionMesh*	GetMesh(CWaterSurface* pSurface,uint32 RegionId);
		void				ReleaseMesh(CWaterSurface* pSurface,uint32 RegionId);
	protected:
		typedef pair<CWaterSurface*,uint32> MeshKey;
		typedef SMap<MeshKey,CWaterRegionMesh*> RegionMeshMap;
		RegionMeshMap		m_MeshList;
	};
}