#include "stdafx.h"
#include "CRenderInstBatch.h"
#include "CRenderPipeline.h"
#include "CMeshBatch.h"
#include "CModifyBatch.h"
#include "CVertexDeclaration.h"
#include "CGraphic.h"
#include "CDataSources.h"
#include "TSqrAllocator.inl"

CRenderInstBatch::CRenderInstBatch(const CreateFlag& fFlag, BATCH_HANDLE fHandle, CGeometryParams* pParams, CGeometryBatch* pParent)
: CGeometryBatch( fFlag, fHandle, pParams, pParent )
, m_pRenderOp(NULL)
{
	CMeshBatch* pMeshBatch = (CMeshBatch*)m_vecRenderBatch[EGBT_MESH];
	if(NULL!=pMeshBatch)
		m_pRenderOp = pMeshBatch->GetRenderOperation(fHandle);
}

CRenderInstBatch::~CRenderInstBatch()
{

}

void	CRenderInstBatch::ClearInstances(void)
{
	m_InstSet.clear();
	_Destroy();
}

void	CRenderInstBatch::AddGeometryInst( CGeometryInst* pGInst )
{
	pGInst->m_pParentBatch = this;
	m_InstSet.insert(pGInst);
}

void	CRenderInstBatch::DelGeometryInst( CGeometryInst* pGInst )
{
	m_InstSet.erase(pGInst);
	if(m_InstSet.size()==0)
		_Destroy();
}

void	CRenderInstBatch::Render(void)
{
	if(m_pRenderOp == NULL)
		return;

	GeometryInstSet::iterator it,eit = m_InstSet.end();
	CRenderPipeline::GetInst()->BeginBatch(m_pRenderOp,1);

	BufferBindingInfo Info;
	for( it = m_InstSet.begin(); it!=eit; ++it )
	{
		CRenderPipeline::GetInst()->SetWorld( *(*it)->m_pMatrix );
		if( (*it)->m_pPieceModify && (*it)->m_pPieceModify->Sorce!=0)
		{
			Info.m_pBuffer = (*it)->m_pPieceModify->pAddVB;
			Info.m_sOffset = (*it)->m_pPieceModify->Offset;
			Info.m_sLenght = (*it)->m_pPieceModify->Lenght;
			Info.m_sVertexSize	= (*it)->m_pPieceModify->pAddVB->getVertexSize();
			CRenderPipeline::GetInst()->SetBatchBuffer( (*it)->m_pPieceModify->Sorce,&Info, STREAMSOURCE_INDEXEDDATA);
		}
		CRenderPipeline::GetInst()->BatchRender();
	}
	CRenderPipeline::GetInst()->EndBatch();
}

CHWRenderInstBatch::CHWRenderInstBatch(const CreateFlag& fFlag, BATCH_HANDLE fHandle, CGeometryParams* pParams, CGeometryBatch* pParent)
: CRenderInstBatch(fFlag, fHandle, pParams, pParent)
{
	if(m_pRenderOp!=NULL)
	{
		DWORD AddDecl =  CVertexElement::GetBaseFormatID(15,VES_COLOR,2);
		CVertexDeclaration* pVD =  m_pRenderOp->vertexData->vertexDeclaration;
		CVertexDeclaration* pVDdest;
		if ( NULL == (pVDdest = CVertexDeclaration::GET_VERDECL( AddDecl | pVD->GetVertexFormatID() )) )
		{
			pVDdest = CDataSources::GetInst()->NewVertexDeclaration( AddDecl | 	pVD->GetVertexFormatID());

			const CVertexDeclaration::VertexElementList elems = pVD->GetElements();
			CVertexDeclaration::VertexElementList::const_iterator ei, eiend;
			eiend = elems.end();
			for (ei = elems.begin(); ei != eiend; ++ei)
			{
				pVDdest->AddElement(
					ei->GetSource(),
					ei->GetOffset(),
					ei->GetType(),
					ei->GetSemantic(),
					ei->GetIndex() );
			}

			pVDdest->AddElement( 1, 0,							VET_FLOAT4, VES_COLOR, 2 );
			pVDdest->AddElement( 1, pVDdest->GetVertexSize(1),	VET_FLOAT4, VES_COLOR, 3 );
			pVDdest->AddElement( 1, pVDdest->GetVertexSize(1),	VET_FLOAT4, VES_COLOR, 4 );
			pVDdest->AddElement( 1, pVDdest->GetVertexSize(1),	VET_FLOAT4, VES_COLOR, 5 );
		}
		m_pRenderOp->vertexData->vertexDeclaration = pVDdest;
	}
}

CHWRenderInstBatch::~CHWRenderInstBatch()
{

}

void	CHWRenderInstBatch::Render(void)
{
	if(m_pRenderOp == NULL)
		return;

	GeometryInstSet::iterator it,eit = m_InstSet.end();
	CRenderPipeline::GetInst()->BeginBatch(m_pRenderOp,m_InstSet.size());

	//if(m_sModify>0)
	//{
	//	char* pModifyBuffer = NULL;
	//	BufferBindingInfo InfoModify;
	//	InfoModify.m_sVertexSize = m_sModify * (m_pRenderOp->vertexData->vertexCount - m_pRenderOp->vertexData->vertexStart );
	//	InfoModify.m_sLenght = InfoModify.m_sVertexSize * m_InstSet.size();
	//	InfoModify.m_pBuffer = CGraphic::GetInst()->GetDynamicPool()->LockVertexBuffer(
	//		InfoModify.m_sLenght ,(void**)&pModifyBuffer, InfoModify.m_sOffset);

	//	for( it = m_InstSet.begin(); it!=eit; ++it )
	//	{
	//		(*m_InstSet.begin())->m_pPieceModify->pAddVB->read((*m_InstSet.begin())->m_pPieceModify->Offset,InfoModify.m_sVertexSize, pModifyBuffer);
	//		pModifyBuffer += InfoModify.m_sVertexSize;
	//	}

	//	CGraphic::GetInst()->GetDynamicPool()->UnLockVertex();
	//	InfoModify.m_sVertexSize = m_sModify;
	//	CRenderPipeline::GetInst()->SetBatchBuffer( 1, &InfoModify, STREAMSOURCE_INDEXEDDATA);
	//}

	BufferBindingInfo InfoMatrix;
	char* pMatrixBuffer = NULL;
	InfoMatrix.m_sVertexSize = sizeof(CMatrix);
	InfoMatrix.m_sLenght = InfoMatrix.m_sVertexSize*m_InstSet.size();
	InfoMatrix.m_pBuffer = CGraphic::GetInst()->GetDynamicPool()->LockVertexBuffer(
		InfoMatrix.m_sLenght ,(void**)&pMatrixBuffer, InfoMatrix.m_sOffset);
	for( it = m_InstSet.begin(); it!=eit; ++it )
	{
		memcpy( pMatrixBuffer,(*it)->m_pMatrix, InfoMatrix.m_sVertexSize );
		pMatrixBuffer += InfoMatrix.m_sVertexSize;
	}
	CGraphic::GetInst()->GetDynamicPool()->UnLockVertex();
	CRenderPipeline::GetInst()->SetBatchBuffer( 1, &InfoMatrix, STREAMSOURCE_INSTANCEDATA);
	CRenderPipeline::GetInst()->BatchRender();
	CRenderPipeline::GetInst()->EndBatch();
}
