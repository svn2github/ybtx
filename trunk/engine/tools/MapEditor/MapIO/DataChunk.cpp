#include "StdAfx.h"
#include "DataChunk.h"
#include "BinarySerializer.h"

namespace IO
{


//------------------------------------------------------------------------------
DataChunk::DataChunk( const Util::FourCC& id )
		: fourcc(id)
		, size(0)
{
	// empty
}

//------------------------------------------------------------------------------
DataChunk::~DataChunk()
{
	// empty
}

//------------------------------------------------------------------------------
uint DataChunk::Size()
{
	return this->size;
}

//------------------------------------------------------------------------------
void DataChunk::Write()
{
	assert(this->serializer.isvalid());
	this->serializer->WriteUInt(this->fourcc.AsUInt());
	this->serializer->WriteUInt(this->size);
}

//------------------------------------------------------------------------------
void DataChunk::Read()
{
	assert(this->serializer.isvalid());
	this->size = this->serializer->ReadUInt();
}

//------------------------------------------------------------------------------
void DataChunk::SetSerializer( const BinarySerializerPtr& s )
{
	this->serializer = s;
}

//------------------------------------------------------------------------------
const BinarySerializerPtr& DataChunk::GetSerializer() const
{
	return this->serializer;
}

//------------------------------------------------------------------------------
const Util::FourCC& DataChunk::GetID() const
{
	return this->fourcc;
}

}// namespace IO