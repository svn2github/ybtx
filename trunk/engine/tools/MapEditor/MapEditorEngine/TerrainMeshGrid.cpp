#include "StdAfx.h"
#include "TerrainMeshGrid.h"
#include "TerrainMeshObject.h"
#include "ModelGroup.h"
#include "EditStateOpertiaonMgr.h"

namespace sqr
{
	SGrid::SGrid(void)
	{
		bReverse				= false;
		bCliffLayerOne			= false;
		pRiver					= NULL;
		pWave					= NULL;
		bGridHide				= false;
		dwBufferIndex			= 0;
		m_bSetExactBlock		= false;
		m_nDivideRegionIndex	= 0;
		nMaterialIndex			= -1;
		bMatNotEffectByTileSets = false;
		subGridTypes[0]			= subGridTypes[1] = subGridTypes[2] = subGridTypes[3] = 0;

		m_nRegionIndex			= -1;

#ifdef USE_OLD_LIGHT_MODEL
		bEffectByVertexColor	= false;
#else
		bEffectByVertexColor	= true;
#endif

		bSubRenderUnitIndex		= 0;
	}

	SGrid::~SGrid(void)
	{
		pRiver = NULL;
		pWave = NULL;
		vecCenterObjects.clear();
		subGridIndex.clear();
		this->ClearModelColorMap();
	}

	//------------------------------------------------------------------------------
	/**
		没有的话会返回一个空的map
	 */
	ModelGroupColorMap*
	SGrid::GetModelColorMap( ModelGroupIndex modelGroupIndex )
	{
		Ast(modelGroupIndex < this->vecCenterObjects.size());

		CEditModelGroup* modelGroup = this->vecCenterObjects[modelGroupIndex]->m_pModelGroup;
		Ast(NULL != modelGroup);
		if (modelGroup->GetIsEffectBased())
			return NULL;
		if (modelGroup->GetIsSubUnit())
			return &m_DepathObjColorMap[modelGroupIndex];
		else
			return &m_ObjectColorMap[modelGroupIndex];
	}

	//------------------------------------------------------------------------------
	void
	SGrid::ClearModelColorMap()
	{
		m_ObjectColorMap.clear();
		m_DepathObjColorMap.clear();
	}

	void SGrid::ClearGridInfo()
	{
		for ( vector< CTObjectPtr >::iterator iter = this->vecCenterObjects.begin(); iter != this->vecCenterObjects.end(); ++iter )
		{
			CEditModelGroup *pModelGroup = NULL;
			pModelGroup = (*iter)->GetModelGroup();
			pModelGroup->SetEditState(sqr::EES_DELETE);
		}

		pRiver = NULL;
		pWave = NULL;
		vecCenterObjects.clear();
		subGridIndex.clear();
		this->ClearModelColorMap();

		bReverse			 = false;
		bEffectByVertexColor = false;
		bCliffLayerOne		 = false;
		bGridHide			 = false;
		dwBufferIndex		 = 0;
		nGridSeekRoadWeight  = 0;
		m_bSetExactBlock	 = false;
		m_nDivideRegionIndex = 0;
		nMaterialIndex		 = -1;
		nBlockType			 = 0;
		subGridTypes[0] = subGridTypes[1] = subGridTypes[2] = subGridTypes[3] = 0;

		nodeInfo.strTextureNames[0] = "";
		nodeInfo.strTextureNames[1] = "";
	}
}