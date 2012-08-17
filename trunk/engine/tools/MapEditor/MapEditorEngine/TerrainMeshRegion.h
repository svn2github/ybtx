#pragma once
#include "TerrainMeshObject.h"
#include "TRefPointer.h"
#include "CColor.h"

namespace sqr
{
	SmartPointer(CTObject);
	class ITexture;

	struct SDivideRegion
	{
		bool		m_bSetRegionEnv;
		bool		bNeedCalRegion;
		string		m_strRegionName;///所有已经设置的区域的名字
		CVector3f   m_vCenter;
		vector<int> vGridIndices;

		SDivideRegion()
		{
			bNeedCalRegion	= false;
			m_bSetRegionEnv	= false;
			m_strRegionName = "default";
			m_vCenter.x		= m_vCenter.y = m_vCenter.z = 0.0f;
		}

		~SDivideRegion()
		{
			vGridIndices.clear();
		}
	};

	struct SRegion
	{
		vector<int> vVertexIndices;
		vector<int> vGridIndices;
		int			divideRgnIndices[REGION_DIVIDE];
		CVector3f	vCenter;

		SRegion()
		{
			divideRgnIndices[0] = divideRgnIndices[1] = divideRgnIndices[2] = divideRgnIndices[3] = 0;
		}
		~SRegion()
		{
			vVertexIndices.clear();
			vGridIndices.clear();
		}
	};

	struct STileRenderRegionUnit
	{
		vector<int>	vGridIndices;
		int			nTileRegionIndex;
		int			nIndexInTileRegion;
		bool		bIsVisibleInView;

		STileRenderRegionUnit();
		~STileRenderRegionUnit();
	};

	struct STileRegion
	{
		vector<int>						vVertexIndices;
		vector<int>						vGridIndices;
		vector<int>						vRegionIndices;
		vector<STileRenderRegionUnit*>	vRenderRegionUnits;

		STileRegion();
		~STileRegion();
	};
}
