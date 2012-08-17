#include "StdAfx.h"
#include "VersionMapChunk.h"
#include "BinarySerializer.h"

namespace IO
{
VersionMapChunk* VersionMapChunk::instance = NULL;

//------------------------------------------------------------------------------
VersionMapChunk::VersionMapChunk()
		: DataChunk('CVER')
{
	// empty
}

//------------------------------------------------------------------------------
VersionMapChunk::~VersionMapChunk()
{
	// empty
}

//------------------------------------------------------------------------------
void VersionMapChunk::RegisterVersion( const Util::FourCC& id, uint ver )
{
	this->versionMap[id.AsUInt()] = ver;
	this->size = this->versionMap.size();
}

//------------------------------------------------------------------------------
void VersionMapChunk::Write()
{
	DataChunk::Write();
	for (std::map<uint, uint>::iterator it = this->versionMap.begin();
	        it != this->versionMap.end(); ++it)
	{
		this->serializer->WriteUInt(it->first);
		this->serializer->WriteUInt(it->second);
	}
}

//------------------------------------------------------------------------------
void VersionMapChunk::Read()
{
	DataChunk::Read();
	for (uint i = 0; i < this->size; i++)
	{
		uint fourcc = this->serializer->ReadUInt();
		uint version = this->serializer->ReadUInt();
		this->versionMap[fourcc] = version;
	}
}

//------------------------------------------------------------------------------
VersionMapChunk* VersionMapChunk::Instance()
{
	if (NULL == instance)
	{
		instance = new VersionMapChunk();
	}
	return instance;
}

//------------------------------------------------------------------------------
sqr::uint VersionMapChunk::FindVersion( const Util::FourCC& id )
{
	std::map<uint, uint>::iterator it = this->versionMap.find(id.AsUInt());
	if (this->versionMap.end() != it)
	{
		return it->second;
	}
	// not registered
	return 0;
}

//------------------------------------------------------------------------------
VersionMapChunk& VersionMapChunk::operator=(const VersionMapChunk& rhs)
{
	this->versionMap = rhs.versionMap;
	this->size = rhs.size;
	return *this;
}


}// namespace IO
