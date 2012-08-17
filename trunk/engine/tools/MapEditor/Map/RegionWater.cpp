#include "StdAfx.h"
#include "RegionWater.h"
#include "../IO/BinarySerializer.h"
#include "../MapEditorEngine/TerrainMesh.h"

namespace Map
{
ImplementChunk(RegionWater, 'RWTR', 1);


//------------------------------------------------------------------------------
RegionWater::RegionWater()
		: DataChunk(FOURCC)
{
	// empty
}

//------------------------------------------------------------------------------
RegionWater::~RegionWater()
{
	// empty
}

//------------------------------------------------------------------------------
void RegionWater::Write()
{
	DataChunk::Write();
	if (this->size > 0)
	{
		this->serializer->WriteData(&this->water.front(), this->size);
	}
}

//------------------------------------------------------------------------------
void RegionWater::Read()
{
	DataChunk::Read();
	if (this->size > 0)
	{
		this->water.resize(this->size / sizeof(WaterGrid));
		this->serializer->ReadData(&this->water.front(), this->size);
	}
}

//------------------------------------------------------------------------------
void RegionWater::LoadFromTerrain( CTerrainMesh* terrain, uint index )
{
	SRegion& region = terrain->GetRegion(index);
	std::vector<int>& gridSet = region.vGridIndices;

	CTerrainMesh* waterMesh = terrain->GetWater();
	std::set<int>& waterGrids = waterMesh->m_setGridIndicesVisibleWater;
	for (std::set<int>::iterator it = waterGrids.begin(); it != waterGrids.end(); ++it)
	{
		SGrid& grid = waterMesh->GetGrid(*it);
		// check whether it is water grid
		if (!grid.bVisible || !grid.bWater || NULL == grid.pRiver)
			continue;
		// check whether it is in current region
		std::vector<int>::iterator itGrid = std::find(gridSet.begin(), gridSet.end(), grid.dwGridIndex);
		if (gridSet.end() == itGrid)
			continue;
		WaterGrid waterGrid;
		// convert to region's local grid index
		waterGrid.index = itGrid - gridSet.begin();
		// grid vertex
		for (int i = 0; i < GRID_VERTEX_NUM; i++)
		{
			SVertex& vertex = waterMesh->GetVertex(grid.dwVertexIndex[i]);
			waterGrid.vertex[i].height = short(vertex.vPosition.y);
			waterGrid.vertex[i].color = short(vertex.cColor);
		};
	}
	// update size
	this->size = this->water.size() * sizeof(WaterGrid);
}

}// namespace Map
