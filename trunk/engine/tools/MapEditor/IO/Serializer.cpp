#include "StdAfx.h"
#include "Serializer.h"

namespace sqr
{

//------------------------------------------------------------------------------
Serializer::Serializer()
		:  isOpen(false)
{
	// empty
}

//------------------------------------------------------------------------------
Serializer::~Serializer()
{
	Ast(!this->isOpen);
}

//------------------------------------------------------------------------------
bool Serializer::Open()
{
	Ast(!this->isOpen);
	Ast(NULL != this->stream);
	if (this->stream->IsOpen())
	{
		this->isOpen = true;
	}
	else
	{
		this->isOpen = this->stream->Open();
	}
	return this->isOpen;
}

//------------------------------------------------------------------------------
void Serializer::Close()
{
	Ast(this->isOpen);
	if (this->stream->IsOpen())
	{
		this->stream->Close();
	}
	this->isOpen = false;
}

}// namespace sqr
