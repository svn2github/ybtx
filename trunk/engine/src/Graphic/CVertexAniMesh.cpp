#include "stdafx.h"
#include "CVertexAniMesh.h"
#include "CMeshLoader.h"
#include "CRenderPiece.h"
#include "CRenderOperation.h"
#include "CVertexDeclaration.h"
#include "CRenderPipeline.h"
#include "BaseHelper.h"

CVertexAniMesh::CVertexAniMesh()
{
	m_VertexType = VT_VERTEXANI;	
};

void VerAniRender(FilterNode& fNode)
{	
	CRenderPipeline::GetInst()->SetVertexShaderF(ACT_CUSTOM_PARAMS_LIST,(CVertexAniMesh::VertexAniParams*)(fNode.param_ptr),1);
}

void VerAniRelease(FilterNode& fNode)
{
	GfkSafeDelete(fNode.param_ptr);
	fNode.param_release = NullRelease;
}

void VerAniUpdate(FilterNode& fNode,CRenderMatrix& AniInst)
{
	uint StartFrame,EndFrame;
	CVertexAniMesh::VertexAniParams* Param = (CVertexAniMesh::VertexAniParams*)(fNode.param_ptr);
	CRenderPiece* pRenderPiece = Param->pRenderPiece;
	WORD AniNo = pRenderPiece->m_Pieces[Param->iPiece].m_CurPiecesAniNo;
	CVertexAniMesh::AniVector* pMeshFrame = Param->pAniVector;
	float PreFrameKeyNum = float( AniInst.m_CurFrame )/(*pMeshFrame)[AniNo].m_FramePerKeyFrame;
	StartFrame = (uint)PreFrameKeyNum;
	EndFrame = StartFrame + 1;
	Param->InvWeight = PreFrameKeyNum - StartFrame;
	Param->Weight = 1 - Param->InvWeight;
	fNode.vertexData->vertexBufferBinding->SetBinding(0,(*pMeshFrame)[AniNo].m_FrameBuffer[StartFrame]);
	fNode.vertexData->vertexBufferBinding->SetBinding(1,(*pMeshFrame)[AniNo].m_FrameBuffer[EndFrame]);
}
