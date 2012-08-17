#include "StdAfx.h"
#include "RegionBlockInfo.h"
#include "../IO/BinarySerializer.h"
#include "../MapEditorEngine/TerrainMesh.h"
#include <bitset>

namespace Map
{
ImplementChunk(RegionBlockInfo, 'RBLK', 1);


//------------------------------------------------------------------------------
RegionBlockInfo::RegionBlockInfo()
		: DataChunk(FOURCC)
{
	this->size = sizeof(this->block);
}

//------------------------------------------------------------------------------
RegionBlockInfo::~RegionBlockInfo()
{
	// empty
}

//------------------------------------------------------------------------------
void RegionBlockInfo::Write()
{
	DataChunk::Write();
	this->serializer->WriteData(this->block, this->size);
}

//------------------------------------------------------------------------------
void RegionBlockInfo::Read()
{
	DataChunk::Read();
	this->serializer->ReadData(this->block, this->size);
}

//------------------------------------------------------------------------------
void RegionBlockInfo::LoadFromTerrain( CTerrainMesh* terrain, uint index )
{
	SRegion& region = terrain->GetRegion(index);
	for (int i = 0; i < REGION_GRID_NUM; i++)
	{
		SGrid& grid = terrain->GetGrid(region.vGridIndices[i]);
		std::bitset<8> attributes;
		attributes.reset();
		byte blockType;
		if (!grid.vecBlockTypes.empty())
		{
			blockType = grid.vecBlockTypes[0].first;
		}
		else
		{
			blockType = grid.nBlockType;
		}
		if (blockType & 0x01)
		{
			attributes.set(0, 1);
		}
		if (blockType & 0x02)
		{
			attributes.set(1, 1);
		}

		// influence on ground objects 2
		attributes.set(2, grid.bInfluence ? 1 : 0);

		this->block[i] = (BYTE)attributes.to_ulong();
	}
}

}// namespace Map
