#include "StdAfx.h"
#include "RegionLightInfo.h"
#include "../IO/BinarySerializer.h"

namespace Map
{
ImplementChunk(RegionLightInfo, 'RLIT', 1);


//------------------------------------------------------------------------------
RegionLightInfo::RegionLightInfo()
		: DataChunk(FOURCC)
{

}

//------------------------------------------------------------------------------
RegionLightInfo::~RegionLightInfo()
{
	// empty
}

//------------------------------------------------------------------------------
void RegionLightInfo::Write()
{
	DataChunk::Write();
}

//------------------------------------------------------------------------------
void RegionLightInfo::Read()
{
	DataChunk::Read();
}

}// namespace Map
