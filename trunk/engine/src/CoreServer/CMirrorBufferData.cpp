#include "stdafx.h"
#include "CMirrorBufferData.h"
#include "CMirrorBuffer.h"


void* CMirrorBufferData::operator new(size_t stSize, CMirrorBuffer* pBuffer, uint32 uSlot)
{
	return pBuffer->AllocData(uSlot);
}

void CMirrorBufferData::operator delete(void* pMem, CMirrorBuffer* pBuffer, uint32 uSlot)
{

}

void CMirrorBufferData::operator delete(void* pMem)
{

}
