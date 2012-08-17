#include "StdAfx.h"
#include "RegionFile.h"
#include "../IO/VersionMapChunk.h"
#include "../IO/BinarySerializer.h"
#include "RegionIndex.h"
#include "RegionData.h"
#include "../MapEditorEngine/CMapEditApp.h"
#include "../MapEditorEngine/TerrainMesh.h"

#define MAP_VERSION	0x10

namespace Map
{

//------------------------------------------------------------------------------
RegionFile::RegionFile()
		: BinFileBase(MAP_VERSION)
{
	this->indices = new RegionIndex();
}

//------------------------------------------------------------------------------
RegionFile::~RegionFile()
{
	SafeDelCtn1(this->regionList);
}

//------------------------------------------------------------------------------
void RegionFile::OnInit()
{
	BinFileBase::OnInit();

	CTerrainMesh* terrain = CMapEditApp::GetInstance()->GetTerrainMesh();
	uint regionCount = terrain->GetRegionCount();
	this->indices->SetSerializer(this->serializer);
	this->indices->Resize(regionCount);
	this->regionList.resize(regionCount);
	for (uint i = 0; i < regionCount; i++)
	{
		this->regionList[i] = new RegionData();
		this->regionList[i]->SetSerializer(this->serializer);
		this->regionList[i]->LoadFromTerrain(terrain, i);
	}
}

//------------------------------------------------------------------------------
void RegionFile::Load()
{
	//@todo
}

//------------------------------------------------------------------------------
void RegionFile::Save()
{
	BinFileBase::Save();
	const IO::StreamPtr& stream = this->serializer->GetStream();
	uint offset = stream->GetPosition();
	offset += sizeof(uint) * 2 + this->indices->Size();
	// calculate the region offsets
	for (uint i = 0; i < this->regionList.size(); i++)
	{
		this->indices->SetOffset(i, offset);
		offset += sizeof(uint) * 2 + this->regionList[i]->Size();
	}
	// save
	this->indices->Write();
	for (uint i = 0; i < this->regionList.size(); i++)
	{
		this->regionList[i]->Write();
	}
}

}// namespace Map
