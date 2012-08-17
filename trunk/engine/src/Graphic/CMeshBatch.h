#pragma once
#include "CGeometryBatch.h"
#include "CMesh.h"

namespace sqr
{
	class CMeshBatch : public CGeometryBatch
	{
	public:
		CMeshBatch(const CreateFlag& fFlag, BATCH_HANDLE fHandle, CGeometryParams* pParams, CGeometryBatch* pParent);
		~CMeshBatch();

		void	AddGeometryInst( CGeometryInst* pGInst );
		void	Render(void);
		CRenderOperation*	GetRenderOperation(BATCH_HANDLE fHandle);
	protected:
		typedef GVector<CRenderCommand> CommandPool;			
		CMesh*			m_pMesh;
		CommandPool		m_CommandPool;
		CRenderMatrix*	m_pRenderMatrix;
	};
}