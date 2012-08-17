#pragma once
#include "CGeometryBatch.h"

namespace sqr
{
	class CRenderInstBatch : public CGeometryBatch
	{
	public:
		CRenderInstBatch(const CreateFlag& fFlag, BATCH_HANDLE fHandle, CGeometryParams* pParams, CGeometryBatch* pParent);
		virtual ~CRenderInstBatch();

		void	ClearInstances(void);
		void	AddGeometryInst( CGeometryInst* pGInst );
		void	DelGeometryInst( CGeometryInst* pGInst );

		virtual  void	Render(void);
	protected:
		typedef GSet<CGeometryInst*> GeometryInstSet;
		GeometryInstSet		m_InstSet;
		CRenderOperation*	m_pRenderOp;
	};

	class CHWRenderInstBatch : public CRenderInstBatch
	{
	public:
		CHWRenderInstBatch(const CreateFlag& fFlag, BATCH_HANDLE fHandle, CGeometryParams* pParams, CGeometryBatch* pParent);
		~CHWRenderInstBatch();

		void	Render(void);
	};

}