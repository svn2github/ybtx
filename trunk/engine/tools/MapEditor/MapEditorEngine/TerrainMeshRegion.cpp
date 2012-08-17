#include "StdAfx.h"
#include "TerrainMeshRegion.h"
#include "BaseHelper.h"

namespace sqr
{
	STileRenderRegionUnit::STileRenderRegionUnit()
	{
		bIsVisibleInView	= true;//默认是没有设置
		nTileRegionIndex	= -1;
		nIndexInTileRegion	= -1;
	}

	STileRenderRegionUnit::~STileRenderRegionUnit()
	{
		vGridIndices.clear();
	}

	STileRegion::~STileRegion()
	{
		vVertexIndices.clear();
		vGridIndices.clear();
		vRegionIndices.clear();
		SafeDelCtn1(vRenderRegionUnits);
	}

	STileRegion::STileRegion()
	{
		
	}
}