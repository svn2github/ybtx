#pragma once
#include "CGeometryBatch.h"
#include "CRenderMatrix.h"
#include "CSkeletalFrame.h"

namespace sqr
{
	class CAnimationBatch : public CGeometryBatch
	{
	public:
		CAnimationBatch(const CreateFlag& fFlag, BATCH_HANDLE fHandle, CGeometryParams* pParams, CGeometryBatch* pParent);
		~CAnimationBatch();
		
		CAnimate*		GetAnimate(void);
		float			GetCurFrame(void);
		CRenderMatrix&	GetRenderMatrix(void);
		void			ModifyRS(STATIC_RS&	RS);
	protected:
		void			_UpdateBatch(void);
		void			_UpdateInst( CGeometryInst* pGInst );
		CAnimate*		m_pBatchAnimate;
		CSkeletalFrame*	m_pSkeletalFrame;
		CRenderMatrix	m_RenderMatrix;
		UINT			m_CurTime;
	};		
}