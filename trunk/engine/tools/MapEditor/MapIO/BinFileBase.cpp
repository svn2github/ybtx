#include "StdAfx.h"
#include "BinFileBase.h"
#include "BinarySerializer.h"
#include "VersionChunk.h"
#include "VersionMapChunk.h"
#include "BaseHelper.h"

namespace IO
{

//------------------------------------------------------------------------------
BinFileBase::BinFileBase( uint version )
		: serializer(new BinarySerializer())
{
	VersionChunk* chunk = new VersionChunk();
	chunk->SetVersion(version);
	this->chunkList.push_back(chunk);
}

//------------------------------------------------------------------------------
BinFileBase::~BinFileBase()
{
	SafeDelCtn1(this->chunkList);
}

//------------------------------------------------------------------------------
bool BinFileBase::Open( const StreamPtr& stream )
{
	this->OnInit();
	this->serializer->SetStream(stream);
	if (this->serializer->Open())
	{
		this->OnOpen();
		return true;
	}
	return false;
}

//------------------------------------------------------------------------------
void BinFileBase::Close()
{
	this->serializer->Close();
	this->OnClose();
}

//------------------------------------------------------------------------------
void BinFileBase::OnInit()
{
	// overrided by sub-class
}

//------------------------------------------------------------------------------
void BinFileBase::OnOpen()
{
	for (ChunkList::iterator it = this->chunkList.begin(); it != this->chunkList.end(); ++it)
	{
		(*it)->SetSerializer(this->serializer);
	}
}

//------------------------------------------------------------------------------
void BinFileBase::OnClose()
{
	// overrided by sub-class
}

//------------------------------------------------------------------------------
void BinFileBase::Load()
{
	while (!this->serializer->Eof())
	{
		uint fourcc = this->serializer->ReadUInt();
		DataChunk* chunk = this->FindChunk(fourcc);
		if (NULL != chunk)
		{
			chunk->Read();
		}
		else
		{
			assert(false);
			uint size = this->serializer->ReadUInt();
			this->serializer->AbortData(size);
		}
	}
}

//------------------------------------------------------------------------------
void BinFileBase::Save()
{
	for (ChunkList::iterator it = this->chunkList.begin(); it != this->chunkList.end(); ++it)
	{
		(*it)->Write();
	}
}

//------------------------------------------------------------------------------
DataChunk* BinFileBase::FindChunk( const Util::FourCC& fourcc )
{
	for (ChunkList::iterator it = this->chunkList.begin(); it != this->chunkList.end(); ++it)
	{
		DataChunk* chunk = (*it);
		if (chunk->GetID() == fourcc)
			return chunk;
	}
	// not exist
	return NULL;
}

//------------------------------------------------------------------------------
void BinFileBase::AddChunk( DataChunk* chunk )
{
	assert(NULL != chunk);
	this->chunkList.push_back(chunk);
}

}// namespace IO
