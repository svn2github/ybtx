#include "StdAfx.h"
#include "RegionBlendAlpha.h"
#include "../IO/BinarySerializer.h"
#include "../MapEditorEngine/TerrainMesh.h"

namespace Map
{
ImplementChunk(RegionBlendAlpha, 'RALP', 1);


//------------------------------------------------------------------------------
RegionBlendAlpha::RegionBlendAlpha()
		: DataChunk(FOURCC)
{
	this->size = sizeof(this->alpha1) + sizeof(this->alpha2);
}

//------------------------------------------------------------------------------
RegionBlendAlpha::~RegionBlendAlpha()
{
	// empty
}

//------------------------------------------------------------------------------
void RegionBlendAlpha::Write()
{
	DataChunk::Write();
	this->serializer->WriteData(this->alpha1, sizeof(this->alpha1));
	this->serializer->WriteData(this->alpha2, sizeof(this->alpha2));
}

//------------------------------------------------------------------------------
void RegionBlendAlpha::Read()
{
	DataChunk::Read();
	this->serializer->ReadData(this->alpha1, sizeof(this->alpha1));
	this->serializer->ReadData(this->alpha2, sizeof(this->alpha2));
}

//------------------------------------------------------------------------------
void RegionBlendAlpha::LoadFromTerrain( CTerrainMesh* terrain, uint index )
{
	SRegion& region = terrain->GetRegion(index);
	for (uint i = 0; i < REGION_VERTEX_NUM; i++)
	{
		SVertex& vertex = terrain->GetVertex(region.vVertexIndices[i]);
		this->alpha1[i] = ((byte*)(&vertex.cColor))[3];
		this->alpha2[i] = ((byte*)(&vertex.sColor))[3];
	}
}

}// namespace Map
