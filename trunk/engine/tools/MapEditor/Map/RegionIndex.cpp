#include "StdAfx.h"
#include "RegionIndex.h"
#include "../IO/BinarySerializer.h"

namespace Map
{
ImplementChunk(RegionIndex, 'RIND', 1);


//------------------------------------------------------------------------------
RegionIndex::RegionIndex()
		: DataChunk(FOURCC)
{

}

//------------------------------------------------------------------------------
RegionIndex::~RegionIndex()
{
	// empty
}

//------------------------------------------------------------------------------
void RegionIndex::Write()
{
	DataChunk::Write();
	for (std::vector<uint>::iterator it = this->indexList.begin();
	        it != this->indexList.end(); ++it)
	{
		uint& index = (*it);
		this->serializer->WriteUInt(index);
	}
}

//------------------------------------------------------------------------------
void RegionIndex::Read()
{
	DataChunk::Read();
	this->indexList.resize(this->size / sizeof(uint));
	for (std::vector<uint>::iterator it = this->indexList.begin();
	        it != this->indexList.end(); ++it)
	{
		(*it) = this->serializer->ReadUInt();
	}
}

//------------------------------------------------------------------------------
void RegionIndex::Resize( uint regionNum )
{
	this->indexList.resize(regionNum);
	// update size
	this->size = this->indexList.size() * sizeof(uint);
}

//------------------------------------------------------------------------------
void RegionIndex::SetOffset( uint index, uint offset )
{
	assert(index < this->indexList.size());
	this->indexList[index] = offset;
}

}// namespace Map
