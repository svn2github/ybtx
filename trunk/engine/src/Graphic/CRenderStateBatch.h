#pragma once
#include "CGeometryBatch.h"
#include "CPieceGroup.h"

namespace sqr
{
	class CRenderStateBatch : public CGeometryBatch
	{
	public:
		CRenderStateBatch(const CreateFlag& fFlag, BATCH_HANDLE fHandle, CGeometryParams* pParams, CGeometryBatch* pParent);
		~CRenderStateBatch();
	protected:
		void				_UpdateBatch(void);
		CPieceRenderStyle*	m_pRenderStyle;
		STATIC_RS			m_RS;
	};
}