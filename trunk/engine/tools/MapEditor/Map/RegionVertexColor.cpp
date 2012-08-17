#include "StdAfx.h"
#include "RegionVertexColor.h"
#include "../IO/BinarySerializer.h"
#include "../MapEditorEngine/TerrainMesh.h"

namespace Map
{
ImplementChunk(RegionVertexColor, 'RCLR', 1);


//------------------------------------------------------------------------------
RegionVertexColor::RegionVertexColor()
		: DataChunk(FOURCC)
{
	this->size = sizeof(this->color);
}

//------------------------------------------------------------------------------
RegionVertexColor::~RegionVertexColor()
{
	// empty
}

//------------------------------------------------------------------------------
void RegionVertexColor::Write()
{
	DataChunk::Write();
	this->serializer->WriteData(this->color, this->size);
}

//------------------------------------------------------------------------------
void RegionVertexColor::Read()
{
	DataChunk::Read();
	this->serializer->ReadData(this->color, this->size);
}

//------------------------------------------------------------------------------
inline uint16 RegionVertexColor::X888To565( uint32 c )
{
	byte* by = (byte*)&c;
	return uint16((float(by[2])/0xff)*((1<<5)-1)) << 11
	       | uint16((float(by[1])/0xff)*((1<<6)-1)) << 5
	       | uint16((float(by[0])/0xff)*((1<<5)-1));
}

//------------------------------------------------------------------------------
void RegionVertexColor::LoadFromTerrain( CTerrainMesh* terrain, uint index )
{
	SRegion& region = terrain->GetRegion(index);
	for (uint i = 0; i < REGION_VERTEX_NUM; i++)
	{
		SVertex& vertex = terrain->GetVertex(region.vVertexIndices[i]);
		this->color[i] = X888To565(vertex.cColor);
	}
}

}// namespace Map
