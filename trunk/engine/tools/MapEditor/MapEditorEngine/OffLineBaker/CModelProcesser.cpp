#include "stdafx.h"
#include "CModelProcesser.h"
#include "CMapEditApp.h"
#include "TerrainMesh.h"
#include "ModelGroup.h"

namespace sqr
{
	CModelProcesser::CModelProcesser()
	{

	}

	CModelProcesser::~CModelProcesser()
	{

	}

	void CModelProcesser::ProcessTile( uint32 x, uint32 z )
	{
		CTerrainMesh*	pTerrainMesh		= CMapEditApp::GetInst()->GetTerrain();
		size_t			nRegionsPerTileRow	= pTerrainMesh->GetWidthInRegion() * TILE_WIDTH_IN_REGION;
		index_t			iRegionIndex		= z * nRegionsPerTileRow + x * TILE_WIDTH_IN_REGION;
		
		for ( index_t j = 0; j < TILE_WIDTH_IN_REGION; ++j )
		{
			for ( index_t i = 0; i < TILE_WIDTH_IN_REGION; ++i )
			{
				int regionIndex = iRegionIndex + i;
				SRegion& region = pTerrainMesh->GetRegion(regionIndex);
				
				// for each grid
				for ( vector<int>::iterator it = region.vGridIndices.begin();
					  it != region.vGridIndices.end(); ++it )
				{
					int gridIndex = *it;
					SGrid& grid = pTerrainMesh->GetGrid(gridIndex);
					processGrid(grid);
				}
			}
			
			iRegionIndex += pTerrainMesh->GetWidthInRegion();	
		}
	}

	void CModelProcesser::processGrid( const SGrid& grid )
	{
		typedef vector<CMapEditObject*>			MapEditObjVector;
		typedef MapEditObjVector::iterator		MapEditObjIter;	

		for ( uint32 iModelGroup = 0; iModelGroup < grid.vecCenterObjects.size(); ++iModelGroup )
		{
			CEditModelGroup* pModelGroup = grid.vecCenterObjects[iModelGroup]->m_pModelGroup;

			for ( MapEditObjIter it = pModelGroup->m_vecSelfPresentation.begin(); 
				  it!= pModelGroup->m_vecSelfPresentation.end(); ++it )
			{
				CMapEditObject* meModel = *it;
				ProcessModel( iModelGroup, *meModel );
			}
		}
	}
}