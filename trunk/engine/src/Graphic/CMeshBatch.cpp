#include "stdafx.h"
#include "CMeshBatch.h"
#include "CAnimationBatch.h"
#include "CModifyBatch.h"
#include "CVertexDeclaration.h"
#include "TSqrAllocator.inl"

CMeshBatch::CMeshBatch(const CreateFlag& fFlag, BATCH_HANDLE fHandle, CGeometryParams* pParams, CGeometryBatch* pParent)
: CGeometryBatch(fFlag,fHandle,pParams,pParent)
, m_pMesh((CMesh*)fHandle)
{
	m_CommandPool.resize(m_pMesh->GetRenderCommandNum());
	CAnimationBatch* pAniBatch = (CAnimationBatch*)m_vecRenderBatch[EGBT_ANI];
	CModifyBatch*	pModifyBatch = (CModifyBatch*)m_vecRenderBatch[EGBT_MODIFY];
	DWORD AddFVF = NULL!=pModifyBatch?pModifyBatch->GetAddDecl():0;
	m_pRenderMatrix = NULL!=pAniBatch?&pAniBatch->GetRenderMatrix():NULL;
	for( size_t i = 0; i < m_CommandPool.size(); ++i )
	{
		m_CommandPool[i].Destroy();
		m_CommandPool[i].useIndexes = m_pRenderMatrix!=NULL && !m_pRenderMatrix->m_pBoneMatrixlist.empty();
		m_pMesh->CreateRenderCommand(m_CommandPool[i],i);
		if( AddFVF != 0)
			m_CommandPool[i].vertexData->vertexDeclaration = CVertexDeclaration::GET_VERDECL(
			m_CommandPool[i].vertexData->vertexDeclaration->GetVertexFormatID() | AddFVF );
		BATCH_HANDLE handle = BATCH_HANDLE(i);
		CGeometryBatch* pBatch = CreateGeometryBatch( m_CurCreateFlag, handle,m_pGeometryParams, this );
		if( NULL != pBatch )
		{
			m_GeometryBuffer.insert( GeometryBatchMap::value_type( handle , pBatch ) );
		}
	}
}

CMeshBatch::~CMeshBatch()
{
	for( size_t i = 0; i < m_CommandPool.size(); ++i )
	{
		m_CommandPool[i].vertexData = NULL;
		m_CommandPool[i].indexData = NULL;		
		m_CommandPool[i].param_release(m_CommandPool[i]);
	}
}

void	CMeshBatch::AddGeometryInst( CGeometryInst* pGInst )
{
	GeometryBatchMap::iterator it,eit =m_GeometryBuffer.end();
	
	for( it = m_GeometryBuffer.begin(); it!= eit; ++it )
	{
		if(m_GeometryBuffer.begin() != it)
		{
			pGInst->m_pNext = new CGeometryInst(*pGInst);
			pGInst = pGInst->m_pNext;
		}
		it->second->AddGeometryInst(pGInst);
	}
}

void	CMeshBatch::Render(void)
{
	GeometryBatchMap::iterator it,eit =m_GeometryBuffer.end();
	for( it = m_GeometryBuffer.begin(); it!= eit; ++it )
	{
		if( m_pRenderMatrix )
			m_CommandPool[it->first].param_fun(m_CommandPool[it->first],*m_pRenderMatrix);
		m_CommandPool[it->first].param_render(m_CommandPool[it->first]);
		it->second->_UpdateBatch();
		it->second->Render();
	}
}

CRenderOperation*	CMeshBatch::GetRenderOperation(BATCH_HANDLE fHandle)
{
	if(fHandle < m_CommandPool.size())
		return &m_CommandPool[fHandle];
	return NULL;
}
