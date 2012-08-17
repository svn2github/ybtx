#include "stdafx.h"
#include "CIndexHardwareBuffer.h"
#include "CShadowBuffer.h"
#include "BaseHelper.h"
#include "GraphicBase.h"

CIndexHardwareBuffer::CIndexHardwareBuffer(size_t numIndexes, Usage usage, bool useSystemMem, bool useShadowBuffer)
		:CHardwareBuffer(usage,useSystemMem,useShadowBuffer),m_sNumIndexes(numIndexes)
{
	m_Size = sizeof(WORD) * m_sNumIndexes;
	m_sLockSize = m_Size;
	if (m_UseShadowBuffer)
		m_pShadowBuffer = new ShadowBuffer(m_Size, HBU_DYNAMIC);
}

CIndexHardwareBuffer::~CIndexHardwareBuffer()
{
	GfkSafeDelete(m_pShadowBuffer);
}


void CIndexHardwareBuffer::read(size_t offset, size_t length, void* pDest)
{
	void* pSrc = this->lock(offset, length, HBL_READ_ONLY);
	if( pSrc != NULL )
		memcpy(pDest, pSrc, length);
	this->unlock();
}

void CIndexHardwareBuffer::write(size_t offset, size_t length, const void* pSource,
                                 bool discardBuffer)
{
	void* pDst = this->lock(offset, length,
	                        discardBuffer ? HBL_DISCARD : HBL_NORMAL);

	if( pDst != NULL )
		memcpy(pDst, pSource, length);
	this->unlock();
}