#include "stdafx.h"
#include "CPluginIndexBuffer.h"

namespace sqr
{
	CPluginIndexBuffer::CPluginIndexBuffer( size_t numIndexes, Usage usage )
		: CIndexHardwareBuffer(numIndexes, usage, true, true)
	{

	}

	CPluginIndexBuffer::~CPluginIndexBuffer()
	{

	}
}