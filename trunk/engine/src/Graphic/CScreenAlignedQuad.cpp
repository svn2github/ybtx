#include "StdAfx.h"
#include "CScreenAlignedQuad.h"
#include "CVertexDeclaration.h"
#include "CRenderPipeline.h"
#include "CGraphic.h"

namespace sqr
{
VerTex quad[4] =
{
	VerTex(-1,  1, 0, 0, 0),
	VerTex(-1, -1, 0, 0, 1),
	VerTex( 1,  1, 0, 1, 0),
	VerTex( 1, -1, 0, 1, 1)
};

//------------------------------------------------------------------------------
CScreenAlignedQuad::CScreenAlignedQuad()
{
	m_Op.Create();
	m_Op.vertexData->vertexDeclaration = CVertexDeclaration::GET_VERDECL(VerTex::Format);
	m_Op.useIndexes = false;
	m_Op.vertexData->vertexStart = 0;
	m_Op.vertexData->vertexCount = 4;
	m_Op.operationType = PT_TRIANGLESTRIP;
	CVertexHardwareBuffer* pBuf = CHardwareBufferMgr::GetInst()->createVertexBuffer( sizeof(VerTex),4,HBU_STATIC_WRITE_ONLY,quad);
	m_Op.vertexData->vertexBufferBinding->SetBinding(0,pBuf);
}

//------------------------------------------------------------------------------
CScreenAlignedQuad::~CScreenAlignedQuad()
{
	m_Op.vertexData->Destroy();
	m_Op.Destroy();
}

//------------------------------------------------------------------------------
void
CScreenAlignedQuad::Render()
{
	CRenderPipeline::GetInst()->RenderEx(&m_Op);
}
}// namespace sqr
