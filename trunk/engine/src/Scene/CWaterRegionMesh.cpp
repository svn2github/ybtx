//------------------------------------------------------------------------------
//  CWaterRegionMesh.cpp
//  (C) 2008 ThreeOGCMan
//------------------------------------------------------------------------------
#include "StdAfx.h"
#include "GraphicBase.h"
#include "CWaterRegionMesh.h"
#include "CVertexDeclaration.h"
#include "CIndexHardwareBuffer.h"
#include "CVertexHardwareBuffer.h"
#include "CRenderPipeline.h"
#include "CWater.h"
//#include "CGraphic.h"

//---------------------------------
#include "TSqrAllocator.inl"
//---------------------------------

namespace sqr
{
const uint VERTEX_NUM_PER_GRID = 4;	///<每个格子的顶点数
const uint INDEX_NUM_PER_GRID = 6;	///<每个格子的顶点索引数
const uint GRID_NUM_PER_REGION = 16 * 16;
const uint VERTEX_NUM_PER_REGION = GRID_NUM_PER_REGION * VERTEX_NUM_PER_GRID;
const uint INDEX_NUM_PER_REGION = GRID_NUM_PER_REGION * INDEX_NUM_PER_GRID;
CIndexHardwareBuffer* CWaterRegionMesh::st_IndexBuffer = NULL;

//------------------------------------------------------------------------------
CWaterRegionMesh::CWaterRegionMesh()
: numGrids(0)
{
	m_pVertexBuffer = NULL;
	this->renderOperation.Create();
	this->renderOperation.useIndexes = true;
	this->renderOperation.vertexData->vertexDeclaration = CVertexDeclaration::GET_VERDECL(VerColorTex::Format);
	this->renderOperation.indexData->indexBuffer = st_IndexBuffer;

}

//------------------------------------------------------------------------------
CWaterRegionMesh::~CWaterRegionMesh()
{
	this->renderOperation.Destroy();
	CHardwareBufferMgr* bufferManager = CHardwareBufferMgr::GetInst();
	if (NULL == bufferManager)
		return;
	if (NULL != m_pVertexBuffer)
	{
		bufferManager->DestroyBuffer(m_pVertexBuffer);
		m_pVertexBuffer = NULL;
	}
}

//------------------------------------------------------------------------------
void
CWaterRegionMesh::InitRegion(CVertexHardwareBuffer* pVertex , uint num)
{
	Ast(num <= GRID_NUM_PER_REGION);
	m_pVertexBuffer = pVertex;
	this->renderOperation.vertexData->vertexBufferBinding->SetBinding(0, pVertex);
	this->numGrids = num;
	this->renderOperation.vertexData->vertexCount = num * VERTEX_NUM_PER_GRID;
	this->renderOperation.indexData->indexCount = num * INDEX_NUM_PER_GRID;
}
//------------------------------------------------------------------------------
void
CWaterRegionMesh::Render()
{
	CRenderPipeline::GetInst()->RenderEx(&this->renderOperation);
}

////------------------------------------------------------------------------------
//size_t CWaterRegionMesh::Lock(VerColorTex** pVer)
//{
//	*pVer = (VerColorTex*)m_pVertexBuffer->lock(HBL_DISCARD);
//	return m_pVertexBuffer->getSize();
//}
//
////------------------------------------------------------------------------------
//void
//CWaterRegionMesh::Unlock()
//{
//	m_pVertexBuffer->unlock();
//}

//------------------------------------------------------------------------------
void
CWaterRegionMesh::InitIndexBuffer(void)
{;
	if (NULL != st_IndexBuffer)
		return;

	SVector<int16> buffer(INDEX_NUM_PER_REGION);

	// write index data
	
	for (index_t i = 0; i < GRID_NUM_PER_REGION; ++i)
	{
		int16 index[INDEX_NUM_PER_GRID] = {0, 1, 2, 2, 1, 3};
		for (index_t j = 0; j < INDEX_NUM_PER_GRID; ++j)
		{
			buffer[i * INDEX_NUM_PER_GRID + j] = index[j] + i * VERTEX_NUM_PER_GRID;
		}
		//memcpy(&(buffer[i * INDEX_NUM_PER_GRID]), index, sizeof(index));
	}

	// create index buffer
	st_IndexBuffer = CHardwareBufferMgr::GetInst()->createIndexBuffer(INDEX_NUM_PER_REGION,
		HBU_STATIC_WRITE_ONLY, &(buffer[0]));
}

}// namespace sqr
