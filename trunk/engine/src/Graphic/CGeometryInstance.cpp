#include "stdafx.h"
#include "CGeometryInstance.h"
#include "CRenderInstBatch.h"
#include "TSqrAllocator.inl"

INST_ANI_PARAMS::INST_ANI_PARAMS()
{
	ANI_HANDLE = 0;
	SKE_HANDLE = 0;
	ANI_INDEX = 0;
	pAniInst = NULL;	
	
}

CGeometryInst::CGeometryInst()
: RS_HANDLE(0)
, MESH_HANDLE(0)
, ANI_PARAMS(NULL)
, m_pMatrix(NULL)
, m_pParentBatch(NULL)
, m_pPieceModify(NULL)
, m_pNext(NULL)
{
}

CGeometryInst::CGeometryInst(const CGeometryInst& Inst)
: RS_HANDLE(Inst.RS_HANDLE)
, MESH_HANDLE(Inst.MESH_HANDLE)
, ANI_PARAMS(Inst.ANI_PARAMS)
, m_pMatrix(Inst.m_pMatrix)
, m_pParentBatch(Inst.m_pParentBatch)
, m_pPieceModify(Inst.m_pPieceModify)
, m_pNext(Inst.m_pNext)
{
}

CGeometryInst::~CGeometryInst()
{
	CGeometryInst* tCur = this;
	CGeometryInst* tNext = NULL;

	while(tCur)
	{
		tNext = tCur->m_pNext;
		if(tCur->m_pParentBatch)
			((CRenderInstBatch*)tCur->m_pParentBatch)->DelGeometryInst(tCur);
		tCur = tNext;
	}
}