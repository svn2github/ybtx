#include "StdAfx.h"
#include "RegionHeader.h"
#include "../IO/BinarySerializer.h"
#include "../MapEditorEngine/TerrainMesh.h"

namespace Map
{
ImplementChunk(RegionHeader, 'RHDR', 1);


//------------------------------------------------------------------------------
RegionHeader::RegionHeader()
		: DataChunk(FOURCC)
		, flag(0)
		, x(0)
		, z(0)
{
	this->size = sizeof(this->flag) + sizeof(this->x) + sizeof(this->z);
}

//------------------------------------------------------------------------------
RegionHeader::~RegionHeader()
{
	// empty
}

//------------------------------------------------------------------------------
void RegionHeader::Write()
{
	DataChunk::Write();
	this->serializer->WriteUInt(this->flag);
	this->serializer->WriteUShort(this->x);
	this->serializer->WriteUShort(this->z);
}

//------------------------------------------------------------------------------
void RegionHeader::Read()
{
	DataChunk::Read();
	this->flag = this->serializer->ReadUInt();
	this->x = this->serializer->ReadUShort();
	this->z = this->serializer->ReadUShort();
}

//------------------------------------------------------------------------------
void RegionHeader::LoadFromTerrain( CTerrainMesh* terrain, uint index )
{
	uint width = terrain->GetWidthInRegion();
	uint height = terrain->GetDepthInRegion();

	this->x = uint16(index % width);
	this->z = uint16(index / width);
}

}// namespace Map
