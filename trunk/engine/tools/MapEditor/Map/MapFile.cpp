#include "StdAfx.h"
#include "MapFile.h"
#include "../IO/VersionMapChunk.h"
#include "MapHeader.h"
#include "MapSceneInfo.h"
#include "MapSoundInfo.h"
#include "MapAmbientEffect.h"
#include "MapHDRInfo.h"
#include "../MapEditorEngine/CMapEditApp.h"

#define MAP_VERSION	0x10

namespace Map
{

//------------------------------------------------------------------------------
MapFile::MapFile()
		: BinFileBase(MAP_VERSION)
{
	this->versions = new IO::VersionMapChunk();
	this->mapHeader = new MapHeader();
	this->sceneInfo = new MapSceneInfo();
	this->soundInfo = new MapSoundInfo();
	this->ambientFx = new MapAmbientEffect();
	this->hdrInfo = new MapHDRInfo();
}

//------------------------------------------------------------------------------
MapFile::~MapFile()
{
	// empty
}

//------------------------------------------------------------------------------
void MapFile::OnInit()
{
	BinFileBase::OnInit();

	this->AddChunk(this->versions);
	this->AddChunk(this->mapHeader);
	this->AddChunk(this->sceneInfo);
	this->AddChunk(this->soundInfo);
	this->AddChunk(this->ambientFx);
	this->AddChunk(this->hdrInfo);

	*this->versions = *IO::VersionMapChunk::Instance();
	CTerrainMesh* terrain = CMapEditApp::GetInstance()->GetTerrainMesh();
	COperator* operate = CMapEditApp::GetInstance()->GetOperator();
	this->mapHeader->LoadFromTerrainMesh(terrain);
	this->sceneInfo->LoadFromOperator(operate);
	this->soundInfo->LoadFromOperator(operate);
	this->hdrInfo->LoadFromOperator(operate);
}

}// namespace Map
