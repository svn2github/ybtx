#include "StdAfx.h"
#include "MapSoundInfo.h"
#include "../IO/BinarySerializer.h"
#include "../MapEditorEngine/Operator.h"

namespace Map
{
ImplementChunk(MapSoundInfo, 'MSND', 1);


//------------------------------------------------------------------------------
MapSoundInfo::MapSoundInfo()
		: DataChunk(FOURCC)
		, interval(0)
{
	// empty
}

//------------------------------------------------------------------------------
MapSoundInfo::~MapSoundInfo()
{
	// empty
}

//------------------------------------------------------------------------------
void MapSoundInfo::Write()
{
	DataChunk::Write();
	this->serializer->WriteString(this->backMusic);
	this->serializer->WriteInt(this->interval);
}

//------------------------------------------------------------------------------
void MapSoundInfo::Read()
{
	DataChunk::Read();
	this->backMusic = this->serializer->ReadString();
	this->interval = this->serializer->ReadInt();
}

//------------------------------------------------------------------------------
void MapSoundInfo::LoadFromOperator( COperator* o )
{
	this->backMusic = o->GetBackGroundMusic();
	this->interval = o->GetMusicInterval();
	// update size
	this->size = sizeof(uint16) + this->backMusic.length() + sizeof(this->interval);
}

}// namespace Map
