#include "stdafx.h"
#include "CGeometryBatch.h"
#include "CAnimationBatch.h"
#include "CMeshBatch.h"
#include "CModifyBatch.h"
#include "CRenderStateBatch.h"
#include "CRenderInstBatch.h"
#include "BaseHelper.h"
#include "TSqrAllocator.inl"

EGeoBufType	CreateFlag::GBQueue[EGBT_COUNT] = { EGBT_FILTER,	EGBT_ANI,	EGBT_MODIFY,	EGBT_RS,	EGBT_MESH,	EGBT_RENDER };
UINT		CreateFlag::GBIndex[EGBT_COUNT] = { 0,				1,			3,				2,			4,			5			};

CGeometryBatch* CreateDefaultRenderBatch(const CreateFlag& fFlag, BATCH_HANDLE fHandle, CGeometryParams* pParams, CGeometryBatch* pParent)
{
	return new CRenderInstBatch(fFlag,fHandle,pParams,pParent);
}

CreateFlag::CreateFlag()
{
	for(UINT I = 0; I < EGBT_COUNT; ++I)
		CreateBufType[I] = GBQueue[I];
	CreateIndex		= 1;
	CreateBatch		= NULL;
	CreateRenderBatch = CreateDefaultRenderBatch;
}

CGeometryBatch* sqr::CreateGeometryBatch(const CreateFlag& fFlag, BATCH_HANDLE fHandle, CGeometryParams* pParams, CGeometryBatch* pParent)
{
	if(fFlag.CreateIndex < EGBT_COUNT)
	{
		CreateFlag tflag = fFlag;
		switch(tflag.CreateBufType[tflag.CreateIndex])
		{
		case EGBT_ANI:
			++tflag.CreateIndex;
			return new CAnimationBatch(tflag,fHandle,pParams, pParent);
		case EGBT_MESH:
			++tflag.CreateIndex;
			return new CMeshBatch(tflag,fHandle,pParams, pParent);
		case EGBT_RS:
			++tflag.CreateIndex;
			return new CRenderStateBatch(tflag,fHandle,pParams, pParent);
		case EGBT_MODIFY:
			++tflag.CreateIndex;
			return new CModifyBatch(tflag,fHandle,pParams, pParent);
		case EGBT_RENDER:
			tflag.CreateIndex = EGBT_COUNT;
			return fFlag.CreateRenderBatch(tflag,fHandle,pParams, pParent);
		default:
			return NULL;
		}
	}
	return NULL;
}

BATCH_HANDLE sqr::GetBufferHandle(CGeometryInst* pGInst,EGeoBufType fGBT)
{
	switch(fGBT)
	{
	case EGBT_ANI:
		return pGInst->ANI_PARAMS?pGInst->ANI_PARAMS->ANI_HANDLE:0;
	case EGBT_MESH:
		return pGInst->MESH_HANDLE;
	case EGBT_RS:
		return pGInst->RS_HANDLE;
	case EGBT_MODIFY:
		return (BATCH_HANDLE)(pGInst->m_pPieceModify != NULL?pGInst->m_pPieceModify->AddFVF:0);
	case EGBT_RENDER:
	default:
		return 0;
	}
}

CGeometryBatch::CGeometryBatch(const CreateFlag& fFlag, BATCH_HANDLE fHandle, CGeometryParams* pParams, CGeometryBatch* pParent)
: m_CurCreateFlag(fFlag)
, m_CurHandle(fHandle)
, m_pGeometryParams(pParams)
, m_pParentBatch(pParent)
{
	if(NULL!=fFlag.CreateBatch)
	{
		for(int i = 0; i<EGBT_COUNT; ++i)
			m_vecRenderBatch[i] = fFlag.CreateBatch[i];
	}
	else
	{
		memset( m_vecRenderBatch, NULL, sizeof(m_vecRenderBatch) );
	}

	if(fFlag.CreateIndex>0)
	{
		m_vecRenderBatch[fFlag.CreateBufType[fFlag.CreateIndex - 1]] = this;
	}

	m_CurCreateFlag.CreateBatch = m_vecRenderBatch;
	AddRef();
}

CGeometryBatch::~CGeometryBatch()
{
	ClearInstances();
}

void	CGeometryBatch::AddGeometryInst( CGeometryInst* pGInst )
{
	BATCH_HANDLE handle = GetBufferHandle(pGInst, m_CurCreateFlag.CreateBufType[m_CurCreateFlag.CreateIndex]);
	GeometryBatchMap::iterator it = m_GeometryBuffer.find(handle);
	_UpdateInst(pGInst);
	if( it == m_GeometryBuffer.end() )
	{
		CGeometryBatch* pBatch = CreateGeometryBatch( m_CurCreateFlag, handle,m_pGeometryParams, this );
		if( NULL != pBatch )
		{
			m_GeometryBuffer.insert( GeometryBatchMap::value_type( handle, pBatch ) );
			pBatch->AddGeometryInst(pGInst);
		}
	}
	else
		it->second->AddGeometryInst(pGInst);
	
}

void	CGeometryBatch::Render(void)
{
	GeometryBatchMap::iterator it,eit =m_GeometryBuffer.end();
	for( it = m_GeometryBuffer.begin(); it!= eit; ++it )
	{
		it->second->_UpdateBatch();
		it->second->Render();
	}
}

void	CGeometryBatch::ClearInstances(void)
{
	GeometryBatchMap::iterator it,eit =m_GeometryBuffer.end();
	for( it = m_GeometryBuffer.begin(); it!= eit; ++it )
	{
		SafeRelease(it->second);
	}
	m_GeometryBuffer.clear();
}

void	CGeometryBatch::_DestroyBatch(BATCH_HANDLE fHandle)
{
	GeometryBatchMap::iterator it = m_GeometryBuffer.find(fHandle);
	if( it != m_GeometryBuffer.end() )
	{
		SafeRelease(it->second);
		m_GeometryBuffer.erase(it);
		if( m_GeometryBuffer.size() == 0 && m_pParentBatch )
			m_pParentBatch->_DestroyBatch(m_CurHandle);
	}
}