#pragma once
#include "CHardwareBuffer.h"
namespace sqr
{
class CVertexHardwareBuffer : public CHardwareBuffer
{
public:
	CVertexHardwareBuffer(size_t vertexSize, size_t numVertices,
	                      Usage usage, bool useSystemMem = false, bool useShadowBuffer = true);
	virtual ~CVertexHardwareBuffer();

	void read(size_t offset, size_t length, void* pDest);
	void write(size_t offset, size_t length, const void* pSource,
	           bool discardBuffer = false);

	size_t getVertexSize(void) const
	{
		return m_VertexSize;
	}
	size_t getNumVertices(void) const
	{
		return m_NumVertices;
	}

protected:
	size_t m_NumVertices;
	size_t m_VertexSize;
};

struct BufferBindingInfo
{
	BufferBindingInfo();
	BufferBindingInfo(CVertexHardwareBuffer* pBuffer,size_t VerSize,size_t offset = 0,size_t lenght = 0);

	CVertexHardwareBuffer*	m_pBuffer;
	size_t					m_sOffset;
	size_t					m_sLenght;
	size_t					m_sVertexSize;
};

}