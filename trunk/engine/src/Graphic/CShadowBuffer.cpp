#include "stdafx.h"
#include "CShadowBuffer.h"
#include "CGraphicExp.h"
#include "CSqrAllocPool.h"
#include "ExpHelper.h"
#include "Memory.h"

namespace sqr
{
ShadowBuffer::ShadowBuffer(size_t fSize, Usage usage)
		:CHardwareBuffer(usage,true,false)
{
	m_Size = fSize;
	m_pData = (TBYTE*)SqrNew(fSize + 16, CGraphicMallocObject::GetPool());
	size_t offset = 16 - (size_t(m_pData) & 15 );
	m_pData = m_pData + offset;
	m_pData[-1] = (TBYTE)offset;
}

ShadowBuffer::~ShadowBuffer()
{
	if (m_pData)
	{
		TBYTE* mem = (TBYTE*)m_pData;
		mem = mem - mem[-1];
		//delete [] mem;
		SqrDelete(mem, CGraphicMallocObject::GetPool());
	}
}

void ShadowBuffer::read(size_t offset, size_t length, void* pDest)
{
	Ast((offset + length) <= m_Size);
	memcpy(pDest, m_pData + offset, length);
}

void ShadowBuffer::write(size_t offset, size_t length, const void* pSource,bool discardBuffer)
{
	Ast((offset + length) <= m_Size);
	// discardBuffer ÊôÐÔÎÞÐ§
	memcpy(m_pData + offset, pSource, length);
}

void* ShadowBuffer::_lock(size_t offset, size_t length, LockOptions options)
{
	return m_pData + offset;
}

void ShadowBuffer::_unlock(void)
{
	//nothing to do...
}

}