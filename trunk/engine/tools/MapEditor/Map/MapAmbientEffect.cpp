#include "StdAfx.h"
#include "MapAmbientEffect.h"
#include "../IO/BinarySerializer.h"

namespace Map
{
ImplementChunk(MapAmbientEffect, 'MAFX', 1);


//------------------------------------------------------------------------------
MapAmbientEffect::MapAmbientEffect()
		: DataChunk(FOURCC)
{

}

//------------------------------------------------------------------------------
MapAmbientEffect::~MapAmbientEffect()
{
	// empty
}

//------------------------------------------------------------------------------
void MapAmbientEffect::Write()
{
// 	DataChunk::Write();
}

//------------------------------------------------------------------------------
void MapAmbientEffect::Read()
{
// 	DataChunk::Read();
}

}// namespace Map
