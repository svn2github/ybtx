#include "stdafx.h"
#include "CRenderStateBatch.h"
#include "CRenderPipeline.h"
#include "CAnimationBatch.h"
#include "CAnimation.h"

CRenderStateBatch::CRenderStateBatch(const CreateFlag& fFlag, BATCH_HANDLE fHandle, CGeometryParams* pParams, CGeometryBatch* pParent)
: CGeometryBatch( fFlag, fHandle, pParams, pParent )
{
	m_pRenderStyle = (CPieceRenderStyle*)fHandle;
}

CRenderStateBatch::~CRenderStateBatch()
{

}

void CRenderStateBatch::_UpdateBatch(void)
{
	if ( m_pRenderStyle == NULL )
		return;

	CAnimationBatch* pAniBatch = (CAnimationBatch*)m_vecRenderBatch[EGBT_ANI];
	uint32	FrameTime = (pAniBatch!=NULL &&  pAniBatch->GetAnimate()!=NULL)?
		(uint32)(pAniBatch->GetCurFrame() * pAniBatch->GetAnimate()->GetFrameRatio()):m_pGeometryParams->m_CurTime;
	m_pRenderStyle->GetCurRS( m_RS,  FrameTime ,m_pGeometryParams->m_CurTime, this);
	

	//Material* pMat = (Material*)pRORS->m_nParam2;
	//m_RS.m_Material.Diffuse.r *= pMat->Diffuse.r;
	//m_RS.m_Material.Diffuse.g *= pMat->Diffuse.g;
	//m_RS.m_Material.Diffuse.b *= pMat->Diffuse.b;
	//m_RS.m_Material.Ambient.r *= pMat->Ambient.r;
	//m_RS.m_Material.Ambient.g *= pMat->Ambient.g;
	//m_RS.m_Material.Ambient.b *= pMat->Ambient.b;	

	//if ( pRenderPiece->m_RS.m_nMask&PIECERS_DIFFUSE )
	//{
	//	m_RS.m_Material.Diffuse.a *= float(pRenderPiece->m_RS.m_Diffuse.A) / 255.0f;
	//	m_RS.m_Material.Diffuse.r *= float(pRenderPiece->m_RS.m_Diffuse.R) / 255.0f;
	//	m_RS.m_Material.Diffuse.g *= float(pRenderPiece->m_RS.m_Diffuse.G) / 255.0f;
	//	m_RS.m_Material.Diffuse.b *= float(pRenderPiece->m_RS.m_Diffuse.B) / 255.0f;

	//	if ( m_RS.m_Material.Diffuse.a < 1.0f )
	//	{
	//		m_RS.m_AlphaBlendEnable	= TRUE;
	//		m_RS.m_SrcBlend			= BLEND_SRCALPHA;
	//		m_RS.m_DestBlend 		= BLEND_INVSRCALPHA;
	//		m_RS.m_AlpOP_S0			= TOP_MODULATE;
	//	}
	//}
	//if ( pRenderPiece->m_RS.m_nMask&PIECERS_AMBIENT )
	//{
	//	m_RS.m_Material.Ambient.r *= float(pRenderPiece->m_RS.m_Ambient.R) / 255.0f;
	//	m_RS.m_Material.Ambient.g *= float(pRenderPiece->m_RS.m_Ambient.G) / 255.0f;
	//	m_RS.m_Material.Ambient.b *= float(pRenderPiece->m_RS.m_Ambient.B) / 255.0f;
	//}
	//if ( pRenderPiece->m_RS.m_nMask&PIECERS_SPECULAR )
	//{
	//	m_RS.m_Material.Specular.r = max( float(pRenderPiece->m_RS.m_Specular.R) / 255.0f, m_RS.m_Material.Specular.r );
	//	m_RS.m_Material.Specular.g = max( float(pRenderPiece->m_RS.m_Specular.G) / 255.0f, m_RS.m_Material.Specular.g );
	//	m_RS.m_Material.Specular.b = max( float(pRenderPiece->m_RS.m_Specular.B) / 255.0f, m_RS.m_Material.Specular.b );
	//	m_RS.m_Material.Power	    = pRenderPiece->m_RS.m_fPower;
	//	m_RS.m_SpecularEnable		= TRUE;
	//}
	//if ( pRenderPiece->m_RS.m_nMask&PIECERS_SECONDTEX )
	//{
	//	m_RS.m_Texture_S1		= pRenderPiece->m_RS.m_pTexture;
	//	m_RS.m_Texturefactor	= pRenderPiece->m_RS.m_BlendAlpha;
	//	m_RS.m_TexOP_S1		= TOP_BLENDFACTORALPHA;
	//	m_RS.m_TexArg1_S1	= TA_TEXTURE;
	//	m_RS.m_TexArg2_S1	= TA_CURRENT;
	//	m_RS.m_AlpOP_S1		= TOP_SELECTARG1;
	//	m_RS.m_AlpArg1_S1	= TA_CURRENT;
	//}
	//if (pRenderPiece->m_RS.m_nMask &PIECERS_CLIPPLANE)
	//{
	//	m_RS.m_ClipPlane = pRenderPiece->m_RS.m_ClipPlane;
	//}
	//else
	//{
	//	ZeroMemory(&m_RS.m_ClipPlane, sizeof(m_RS.m_ClipPlane));
	//}
	for(UINT I = 0; I < EGBT_COUNT; ++I)
		if(m_vecRenderBatch[I] != NULL)
			m_vecRenderBatch[I]->ModifyRS(m_RS);
	CRenderPipeline::GetInst()->SetRenderStyle(m_RS);
}