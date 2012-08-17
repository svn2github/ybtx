#pragma once
#include "CMatrix.h"
#include "CRenderBatch.h"
#include "CRenderMatrix.h"
#include "CVertexHardwareBuffer.h"
#include "CRenderOperation.h"
#include "CSkeletalMask.h"

namespace sqr
{
	typedef DWORD	BATCH_HANDLE;

	class CGeometryParams : public CGraphicMallocObject
	{
	public:
		uint32	m_CurTime;
	};

	class INST_ANI_PARAMS : public CGraphicMallocObject
	{
	public:
		INST_ANI_PARAMS();

		BATCH_HANDLE	ANI_HANDLE;
		BATCH_HANDLE	SKE_HANDLE;
		UINT			ANI_INDEX;
		SKELETAL_MASK	SKE_MASK;
		CRenderMatrix*	pAniInst;	
	};


	//模型微调结构
	class PieceModify : public CGraphicMallocObject
	{
	public:
		PieceModify(): Sorce(0),AddFVF(0),pAddVB(NULL) {}

		uint					Sorce;
		DWORD					AddFVF;
		CVertexHardwareBuffer*	pAddVB;
		size_t					Offset;
		size_t					Lenght;
	};
		
	class CGeometryInst : public CGraphicMallocObject
	{
	public:
		CGeometryInst();
		CGeometryInst(const CGeometryInst& Inst);
		~CGeometryInst();

		BATCH_HANDLE	RS_HANDLE;
		BATCH_HANDLE	MESH_HANDLE;
				
		INST_ANI_PARAMS*	ANI_PARAMS;
		CMatrix*			m_pMatrix;	
		CRenderBatch*		m_pParentBatch;
		PieceModify*		m_pPieceModify;
		CGeometryInst*		m_pNext;
	};
}