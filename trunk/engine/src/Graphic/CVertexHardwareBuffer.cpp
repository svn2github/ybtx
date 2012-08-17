#include "stdafx.h"
#include "CVertexHardwareBuffer.h"
#include "BaseHelper.h"
#include "CShadowBuffer.h"
#include "GraphicBase.h"

CVertexHardwareBuffer::CVertexHardwareBuffer(size_t vertexSize, size_t numVertices,
        Usage usage, bool useSystemMem, bool useShadowBuffer)
		:CHardwareBuffer(usage,useSystemMem,useShadowBuffer),m_VertexSize(vertexSize),m_NumVertices(numVertices)
{
	m_Size = m_VertexSize * m_NumVertices;
	m_sLockSize = m_Size;
	if (m_UseShadowBuffer)
		m_pShadowBuffer = new ShadowBuffer(m_Size, HBU_DYNAMIC);
}

CVertexHardwareBuffer::~CVertexHardwareBuffer()
{
	GfkSafeDelete(m_pShadowBuffer);
}

void CVertexHardwareBuffer::read(size_t offset, size_t length, void* pDest)
{
	void* pSrc = this->lock(offset, length, HBL_READ_ONLY);
	if( pSrc == 0 )
		this->unlock();
	memcpy(pDest, pSrc, length);
	this->unlock();
}

void CVertexHardwareBuffer::write(size_t offset, size_t length, const void* pSource, bool discardBuffer)
{
	void* pDst = this->lock(offset, length,
	                        discardBuffer ? HBL_DISCARD : HBL_NORMAL);
	if( pDst != NULL )
		memcpy(pDst, pSource, length);
	this->unlock();
}

BufferBindingInfo::BufferBindingInfo()
: m_pBuffer(NULL)
, m_sOffset(0)
, m_sLenght(0)
, m_sVertexSize(0)
{
}

BufferBindingInfo::BufferBindingInfo(CVertexHardwareBuffer* pBuffer,size_t VerSize,size_t offset,size_t lenght)
: m_pBuffer(pBuffer)
, m_sOffset(offset)
, m_sLenght(lenght)
, m_sVertexSize(VerSize)
{

}
