#include "StdAfx.h"
#include "RegionVertexHeight.h"
#include "../IO/BinarySerializer.h"
#include "../MapEditorEngine/TerrainMesh.h"

namespace Map
{
ImplementChunk(RegionVertexHeight, 'RVTX', 1);


//------------------------------------------------------------------------------
RegionVertexHeight::RegionVertexHeight()
		: DataChunk(FOURCC)
{
	this->size = sizeof(this->height);
}

//------------------------------------------------------------------------------
RegionVertexHeight::~RegionVertexHeight()
{
	// empty
}

//------------------------------------------------------------------------------
void RegionVertexHeight::Write()
{
	DataChunk::Write();
	this->serializer->WriteData(this->height, this->size);
}

//------------------------------------------------------------------------------
void RegionVertexHeight::Read()
{
	DataChunk::Read();
	this->serializer->ReadData(this->height, this->size);
}

//------------------------------------------------------------------------------
void RegionVertexHeight::LoadFromTerrain( CTerrainMesh* terrain, uint index )
{
	SRegion& region = terrain->GetRegion(index);
	for (uint i = 0; i < REGION_VERTEX_NUM; i++)
	{
		SVertex& vertex = terrain->GetVertex(region.vVertexIndices[i]);
		this->height[i] = short(vertex.vPosition.y);
	}
}

}// namespace Map
