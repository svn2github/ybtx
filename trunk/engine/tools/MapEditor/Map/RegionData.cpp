#include "StdAfx.h"
#include "RegionData.h"
#include "../IO/BinarySerializer.h"
#include "../MapEditorEngine/TerrainMesh.h"
#include "RegionHeader.h"
#include "RegionVertexHeight.h"
#include "RegionVertexColor.h"
#include "RegionBlendAlpha.h"
#include "RegionCliffUV.h"
#include "RegionLogicalHeight.h"
#include "RegionWater.h"
#include "RegionLightInfo.h"
#include "RegionBlockInfo.h"

namespace Map
{
ImplementChunk(RegionData, 'MRGN', 1);


//------------------------------------------------------------------------------
RegionData::RegionData()
		: DataChunk(FOURCC)
{
	this->header = new RegionHeader();
	this->height = new RegionVertexHeight();
	this->color = new RegionVertexColor();
	this->alpha = new RegionBlendAlpha();
	this->cliffuv = new RegionCliffUV();
	this->logicH = new RegionLogicalHeight();
	this->water = new RegionWater();
// 	this->light = new RegionLightInfo();
	this->block = new RegionBlockInfo();
	// add to list
	this->chunkList.push_back(this->header);
	this->chunkList.push_back(this->height);
	this->chunkList.push_back(this->color);
	this->chunkList.push_back(this->alpha);
	this->chunkList.push_back(this->cliffuv);
	this->chunkList.push_back(this->logicH);
	this->chunkList.push_back(this->water);
// 	this->chunkList.push_back(this->light);
	this->chunkList.push_back(this->block);
}

//------------------------------------------------------------------------------
RegionData::~RegionData()
{
	SafeDelCtn1(this->chunkList);
}

//------------------------------------------------------------------------------
void RegionData::Write()
{
	DataChunk::Write();
	for (std::vector<DataChunk*>::iterator it = this->chunkList.begin();
	        it != this->chunkList.end(); ++it)
	{
		DataChunk* chunk = (*it);
		chunk->SetSerializer(this->serializer);
		chunk->Write();
	}
}

//------------------------------------------------------------------------------
void RegionData::Read()
{
	DataChunk::Read();
// 	for (std::vector<DataChunk*>::iterator it = this->chunkList.begin();
// 		it != this->chunkList.end(); ++it)
// 	{
// 		DataChunk* chunk = (*it);
// 		chunk->SetSerializer(this->serializer);
// 		chunk->Read();
// 	}
}

//------------------------------------------------------------------------------
void RegionData::LoadFromTerrain( CTerrainMesh* terrain, uint index )
{
	this->header->LoadFromTerrain(terrain, index);
	this->height->LoadFromTerrain(terrain, index);
	this->color->LoadFromTerrain(terrain, index);
	this->alpha->LoadFromTerrain(terrain, index);
	this->cliffuv->LoadFromTerrain(terrain, index);
	this->logicH->LoadFromTerrain(terrain, index);
	this->water->LoadFromTerrain(terrain, index);
// 	this->light->LoadFromTerrain(terrain, index);
	this->block->LoadFromTerrain(terrain, index);

	// update size
	for (std::vector<DataChunk*>::iterator it = this->chunkList.begin();
	        it != this->chunkList.end(); ++it)
	{
		DataChunk* chunk = (*it);
		this->size += sizeof(uint) * 2 + chunk->Size();
	}
}

}// namespace Map
