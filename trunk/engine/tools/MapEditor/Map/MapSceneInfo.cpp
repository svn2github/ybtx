#include "StdAfx.h"
#include "MapSceneInfo.h"
#include "../IO/BinarySerializer.h"
#include "../MapEditorEngine/Operator.h"

namespace Map
{
ImplementChunk(MapSceneInfo, 'MINF', 1);


//------------------------------------------------------------------------------
MapSceneInfo::MapSceneInfo()
		: DataChunk(FOURCC)
{
	this->size = sizeof(this->sceneInfo);
}

//------------------------------------------------------------------------------
MapSceneInfo::~MapSceneInfo()
{
	// empty
}

//------------------------------------------------------------------------------
void MapSceneInfo::Write()
{
	DataChunk::Write();
	this->serializer->WriteData(this->sceneInfo, this->size);
}

//------------------------------------------------------------------------------
void MapSceneInfo::Read()
{
	DataChunk::Read();
	this->serializer->ReadData(this->sceneInfo, this->size);
}

//------------------------------------------------------------------------------
void MapSceneInfo::LoadFromOperator( COperator* o )
{
	for (int i = 0; i < DAYTIME_NUM; i++)
	{
		this->sceneInfo[i].ambientColor = o->GetAmbientColor(COperator::EEnvState(i));
		this->sceneInfo[i].playerColor = o->GetMainPlayerAmbientColor(COperator::EEnvState(i));
		this->sceneInfo[i].sunColor = o->GetDirColor(COperator::EEnvState(i));
		this->sceneInfo[i].sunDir = o->GetDirDir(COperator::EEnvState(i));
		this->sceneInfo[i].playerSunColor = o->GetPlayerDirColor(COperator::EEnvState(i));
		this->sceneInfo[i].playerAmbientColor = o->GetPlayerAmbientColor(COperator::EEnvState(i));
		this->sceneInfo[i].playerLightColor = o->GetDotColor(COperator::EEnvState(i));
		this->sceneInfo[i].attenuation = o->GetDotRatio(COperator::EEnvState(i));
		this->sceneInfo[i].rangeOfLight = o->GetDotRange(COperator::EEnvState(i));
		this->sceneInfo[i].fogColor = o->GetFogColor(COperator::EEnvState(i));
		this->sceneInfo[i].fogStart = o->GetFogStart(COperator::EEnvState(i));
		this->sceneInfo[i].fogEnd = o->GetFogEnd(COperator::EEnvState(i));
		std::string sky = o->GetSkyTextureName(COperator::EEnvState(i));
		lstrcpyn(this->sceneInfo[i].skyTexture, sky.c_str(), 64);
	}
}

}// namespace Map
