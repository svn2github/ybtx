#include "stdafx.h"
#include "CTrMirrorQuadBuffer.h"
#include "CMirrorQuadObject.h"
#include "CMirrorBuffer.h"
#include "CMirrorQuadScene.h"

CTrMirrorQuadBuffer::CTrMirrorQuadBuffer(uint32 uDataSize, uint32 uDataNum)
: CTrMirrorBuffer(uDataSize, uDataNum)
{

}

CTrMirrorQuadBuffer::~CTrMirrorQuadBuffer()
{

}


CTrMirrorBuffer* CTrMirrorQuadBuffer::CreateBufferMgr(uint32 uDataNum)
{
	
	uint32 uDataSize = (sizeof(CMirrorQuadObject) > sizeof(CMirrorQuadScene)) ? 
		(sizeof(CMirrorQuadObject)) : (sizeof(CMirrorQuadScene));

	CTrMirrorQuadBuffer* pMgr = new CTrMirrorQuadBuffer(uDataSize, uDataNum);

	return pMgr;
}

void CTrMirrorQuadBuffer::Release()
{
	delete this;
}

