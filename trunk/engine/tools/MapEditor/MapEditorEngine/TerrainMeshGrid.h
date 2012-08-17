#pragma once
#include "TRefPointer.h"
#include "OffLineBaker/CVertexColor.h"
#include "ExsertStructDB.h"

namespace sqr
{
	SmartPointer(CTObject);

	class CRiver;
	class CRiverWave;

	class SGrid
	{
	public:
		SGrid();
		~SGrid(void);

	public:
		DWORD		dwGridIndex;
		DWORD		dwVertexIndex[4];

		CVector2f	uv[2][4];
		CVector2f	uvOld[2][4];
		STextureNodeInfo nodeInfo;

		CVector3f	vCenter;
		vector< CTObjectPtr>			vecCenterObjects; // is this the center gird(in Model way)?
		int			nBlockType;
		int			nGridSeekRoadWeight;
		bool		bVisible;
		bool		bSide;
		bool		bReverse;
		CRiver	   *pRiver;
		CRiverWave *pWave;
		bool		bInHideRegion;
		bool		bEffectByVertexColor;	// 物体是否受顶点色影响
		bool		bCliffLayerOne;			// 第一层是否需要悬崖贴图
		bool		bGridHide;
		DWORD		dwBufferIndex;			// buffer索引
		
		BYTE		subGridTypes[4];

		/*			---------
					|2  |3	|
					---------
					|0	|1  |
					---------
		*/
		map<int, int> subGridIndex;			// first subgridIndex, second 0---3
		int			m_nDivideRegionIndex;	// 格子属于哪个细化区域
		int			m_nRegionIndex;			// 格子属于哪个区域
		bool		m_bSetExactBlock;

		map< ModelGroupIndex, ModelGroupColorMap > m_ObjectColorMap;
		map< ModelGroupIndex, ModelGroupColorMap > m_DepathObjColorMap;

		///地表材质索引
		int8		nMaterialIndex;
		bool		bMatNotEffectByTileSets;///强行设置该格子不受图素包设置影响，默认为受影响
			
		int8		bSubRenderUnitIndex;///在大区域中的哪个部分，暂时是分4个部分

	public:
		ModelGroupColorMap* GetModelColorMap(ModelGroupIndex modelGroupIndex);
		void ClearModelColorMap();
		void ClearGridInfo();
	};
}
