#include "StdAfx.h"
#include "MapHDRInfo.h"
#include "../IO/BinarySerializer.h"
#include "../MapEditorEngine/Operator.h"

namespace Map
{
ImplementChunk(MapHDRInfo, 'MHDR', 1);


//------------------------------------------------------------------------------
MapHDRInfo::MapHDRInfo()
		: DataChunk(FOURCC)
		, fBrightPass(0.0f)
		, fGaussMultiplier(0.0f)
		, fGaussMean(0.0f)
		, fGaussStdDev(0.0f)
		, fExposure(0.0f)
		, fSample(0.0f)
{
	this->size = sizeof(float) * 6;
}

//------------------------------------------------------------------------------
MapHDRInfo::~MapHDRInfo()
{
	// empty
}

//------------------------------------------------------------------------------
void MapHDRInfo::Write()
{
	DataChunk::Write();
	this->serializer->WriteFloat(this->fBrightPass);
	this->serializer->WriteFloat(this->fGaussMultiplier);
	this->serializer->WriteFloat(this->fGaussMean);
	this->serializer->WriteFloat(this->fGaussStdDev);
	this->serializer->WriteFloat(this->fExposure);
	this->serializer->WriteFloat(this->fSample);
}

//------------------------------------------------------------------------------
void MapHDRInfo::Read()
{
	DataChunk::Read();
	this->fBrightPass = this->serializer->ReadFloat();
	this->fGaussMultiplier = this->serializer->ReadFloat();
	this->fGaussMean = this->serializer->ReadFloat();
	this->fGaussStdDev = this->serializer->ReadFloat();
	this->fExposure = this->serializer->ReadFloat();
	this->fSample = this->serializer->ReadFloat();
}

//------------------------------------------------------------------------------
void MapHDRInfo::LoadFromOperator( COperator* o )
{
	this->fBrightPass = o->m_fBrightPass;
	this->fGaussMultiplier = o->m_fGaussMultiplier;
	this->fGaussMean = o->m_fGaussMean;
	this->fGaussStdDev = o->m_fGaussStdDev;
	this->fExposure = o->m_fExposure;
	this->fSample = o->m_fSam;
}

}// namespace Map
