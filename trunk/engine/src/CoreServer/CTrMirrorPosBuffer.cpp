#include "stdafx.h"
#include "CTrMirrorPosBuffer.h"
#include "CMirrorBuffer.h"
#include "CMirrorPosData.h"


CTrMirrorPosBuffer::CTrMirrorPosBuffer(uint32 uDataSize, uint32 uDataNum)
: CTrMirrorBuffer(uDataSize, uDataNum)
{

}

CTrMirrorPosBuffer::~CTrMirrorPosBuffer()
{

}

CTrMirrorBuffer* CTrMirrorPosBuffer::CreateBufferMgr(uint32 uDataNum)
{
	CTrMirrorPosBuffer* pMgr = new CTrMirrorPosBuffer(sizeof(CMirrorPosData), uDataNum);

	return pMgr;
}

void CTrMirrorPosBuffer::Release()
{
	delete this;
}
