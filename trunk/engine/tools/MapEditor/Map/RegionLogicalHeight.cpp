#include "StdAfx.h"
#include "RegionLogicalHeight.h"
#include "../IO/BinarySerializer.h"
#include "../MapEditorEngine/TerrainMesh.h"

namespace Map
{
ImplementChunk(RegionLogicalHeight, 'RLHT', 1);


//------------------------------------------------------------------------------
RegionLogicalHeight::RegionLogicalHeight()
		: DataChunk(FOURCC)
{
	this->size = sizeof(this->height);
}

//------------------------------------------------------------------------------
RegionLogicalHeight::~RegionLogicalHeight()
{
	// empty
}

//------------------------------------------------------------------------------
void RegionLogicalHeight::Write()
{
	DataChunk::Write();
	this->serializer->WriteData(this->height, this->size);
}

//------------------------------------------------------------------------------
void RegionLogicalHeight::Read()
{
	DataChunk::Read();
	this->serializer->ReadData(this->height, this->size);
}

//------------------------------------------------------------------------------
void RegionLogicalHeight::LoadFromTerrain( CTerrainMesh* terrain, uint index )
{
	SRegion& region = terrain->GetRegion(index);
	for (int i = 0; i < REGION_VERTEX_NUM; i++)
	{
		SVertex& vertex = terrain->GetVertex(region.vVertexIndices[i]);
		this->height[i] = short(vertex.fLogicHeight + vertex.fLogicHeightEx);
	}
}
}// namespace Map
