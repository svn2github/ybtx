#include "StdAfx.h"
#include "BinaryReader.h"

namespace sqr
{

//------------------------------------------------------------------------------
char BinaryReader::ReadChar()
{
	char c;
	this->stream->Read(&c, sizeof(c));
	return c;
}

//------------------------------------------------------------------------------
byte BinaryReader::ReadByte()
{
	byte c;
	this->stream->Read(&c, sizeof(c));
	return c;
}

//------------------------------------------------------------------------------
int16 BinaryReader::ReadInt16()
{
	short s;
	this->stream->Read(&s, sizeof(s));
	return s;
}

//------------------------------------------------------------------------------
uint16 BinaryReader::ReadUInt16()
{
	unsigned short s;
	this->stream->Read(&s, sizeof(s));
	return s;
}

//------------------------------------------------------------------------------
int32 BinaryReader::ReadInt32()
{
	int i;
	this->stream->Read(&i, sizeof(i));
	return i;
}

//------------------------------------------------------------------------------
uint32 BinaryReader::ReadUInt32()
{
	uint i;
	this->stream->Read(&i, sizeof(i));
	return i;
}

//------------------------------------------------------------------------------
float BinaryReader::ReadFloat()
{
	float f;
	this->stream->Read(&f, sizeof(f));
	return f;
}

//------------------------------------------------------------------------------
bool BinaryReader::ReadBool()
{
	bool b;
	this->stream->Read(&b, sizeof(b));
	return b;
}

//------------------------------------------------------------------------------
string BinaryReader::ReadString()
{
	uint16 length = this->ReadUInt16();
	if (length > 0)
	{
		vector<TCHAR> buffer(length + 1, 0);
		this->stream->Read((void*)&buffer.front(), length);
		return string(&buffer.front());
	}
	return "";
}

//------------------------------------------------------------------------------
void BinaryReader::ReadData( void* data, size_t size )
{
	Ast(NULL != data);
	this->stream->Read(data, size);
}

//------------------------------------------------------------------------------
void BinaryReader::AbortData( size_t size )
{
	this->stream->Seek(size, Stream::Current);
}

}// namespace sqr
