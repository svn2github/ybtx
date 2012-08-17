#include "StdAfx.h"
#include "RegionCliffUV.h"
#include "../IO/BinarySerializer.h"
#include "../MapEditorEngine/TerrainMesh.h"

namespace Map
{
ImplementChunk(RegionCliffUV, 'RCUV', 1);


//------------------------------------------------------------------------------
RegionCliffUV::RegionCliffUV()
		: DataChunk(FOURCC)
{
	this->size = sizeof(this->cliffuv);
}

//------------------------------------------------------------------------------
RegionCliffUV::~RegionCliffUV()
{
	// empty
}

//------------------------------------------------------------------------------
void RegionCliffUV::Write()
{
	DataChunk::Write();
	this->serializer->WriteData(this->cliffuv, this->size);
}

//------------------------------------------------------------------------------
void RegionCliffUV::Read()
{
	DataChunk::Read();
	this->serializer->ReadData(this->cliffuv, this->size);
}

//------------------------------------------------------------------------------
void RegionCliffUV::LoadFromTerrain( CTerrainMesh* terrain, uint index )
{
	SRegion& region = terrain->GetRegion(index);
	for (int i = 0; i < REGION_GRID_NUM; i++)
	{
		SGrid& grid = terrain->GetGrid(region.vGridIndices[i]);
		for (int j = 0; j < GRID_VERTEX_NUM; j++)
		{
			int index = i * GRID_VERTEX_NUM + j;
			this->cliffuv[index] = grid.uv[1][i];
		}
	}
}

}// namespace Map
