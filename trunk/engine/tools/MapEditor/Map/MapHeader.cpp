#include "StdAfx.h"
#include "MapHeader.h"
#include "../IO/BinarySerializer.h"
#include "TerrainMesh.h"

namespace Map
{
ImplementChunk(MapHeader, 'MHDR', 1);

//------------------------------------------------------------------------------
MapHeader::MapHeader()
		: DataChunk(FOURCC)
{
	this->flag = 0;//reserved
	// update size
	this->size = sizeof(this->flag) + sizeof(this->width) + sizeof(this->height);
}

//------------------------------------------------------------------------------
MapHeader::~MapHeader()
{
	// empty
}

//------------------------------------------------------------------------------
void MapHeader::Write()
{
	DataChunk::Write();
	this->serializer->WriteUInt(this->flag);
	this->serializer->WriteUShort(this->width);
	this->serializer->WriteUShort(this->height);
}

//------------------------------------------------------------------------------
void MapHeader::Read()
{
	DataChunk::Read();
	this->flag = this->serializer->ReadUInt();
	this->width = this->serializer->ReadUShort();
	this->height = this->serializer->ReadUShort();
}

//------------------------------------------------------------------------------
void MapHeader::LoadFromTerrainMesh( CTerrainMesh* terrain )
{
	this->width = terrain->GetWidthInRegion();
	this->height = terrain->GetDepthInRegion();
}
}// namespace Map
