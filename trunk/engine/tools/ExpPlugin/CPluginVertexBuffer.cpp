#include "stdafx.h"
#include "CPluginVertexBuffer.h"

namespace sqr
{
	CPluginVertexBuffer::CPluginVertexBuffer( size_t vertexSize, size_t numVertices, Usage usage )
		: CVertexHardwareBuffer(vertexSize, numVertices, usage, true, true )
	{

	}

	CPluginVertexBuffer::~CPluginVertexBuffer()
	{

	}
}