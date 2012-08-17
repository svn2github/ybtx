#include "stdafx.h"
#include "CTexFontRender.h"
#include "CGraphic.h"

CTexFontRender::CTexFontRender()
: m_fOffX(0.0f)
, m_fOffY(0.0f)
, m_Width(0)
, m_Height(0)
, m_pText(NULL)
, m_bAlphaTest(TRUE)
, m_bAlphaBlend(FALSE)
, m_bStateInRender(false)
{

}

CTexFontRender::~CTexFontRender()
{

}

void CTexFontRender::SetBackTexture(ITexture* pText)
{
	m_pText = pText;
	if(m_pText)
	{
		m_Width = m_pText->GetWidth();
		m_Height = m_pText->GetHeight();
	}
}

void CTexFontRender::SetDrawRect(const CFRect& Rect)
{
	m_fRect = Rect;
}

void CTexFontRender::SetOffSet( float X, float Y )
{
	m_fOffX += X;
	m_fOffY += Y;
}

void CTexFontRender::BeginRenderer()
{	
	if(CMainWindowTarget::HasInst())
	{
		m_bStateInRender = true;
		CMainWindowTarget::GetInst()->InsertRenderer(this);
	}
}

void CTexFontRender::EndRenderer()
{
	if(CMainWindowTarget::HasInst())
	{
		m_bStateInRender = false;
		CMainWindowTarget::GetInst()->DeleteRenderer(this);
	}

}

void CTexFontRender::Render()
{
	CGraphic* pGraphic = CGraphic::GetInst();
	pGraphic->ClearStack();
	CMainWindowTarget::GetInst()->Clear();
	if(m_pText)
	{
		VerText2D* pVB;
		RenderState* pRS;
		CMatrix* Matrix = pGraphic->GetRenderStack( RS_GUI, m_pText, NULL, PT_TRIANGLESTRIP, 
			4, 2, VerText2D::Format, sizeof(VerText2D), (void**)&pVB, NULL, (void**)&pRS );
		pRS->m_Cull = 1;
		pRS->m_AlphaBlendEnable = m_bAlphaBlend;
		pRS->m_AlphaTestEnable  = m_bAlphaTest;
		pRS->m_ZTestEnable		= FALSE;
		Matrix->Identity();

		pVB[0].x = m_fOffX;  					pVB[0].y = m_fOffY;						pVB[0].z = 1.0f; 
		pVB[1].x = float(m_Width)+ m_fOffX;		pVB[1].y = m_fOffY;						pVB[1].z = 1.0f;	
		pVB[2].x = m_fOffX;						pVB[2].y = float(m_Height) + m_fOffY;	pVB[2].z = 1.0f;	
		pVB[3].x = float(m_Width)+ m_fOffX;		pVB[3].y = float(m_Height) + m_fOffY;	pVB[3].z = 1.0f;	

		pVB[0].w = 1.0;		pVB[0].tu = 0.0;	pVB[0].tv = 0.0;
		pVB[1].w = 1.0;		pVB[1].tu = 1.0;	pVB[1].tv = 0.0;
		pVB[2].w = 1.0;		pVB[2].tu = 0.0;	pVB[2].tv = 1.0; 
		pVB[3].w = 1.0;		pVB[3].tu = 1.0;	pVB[3].tv = 1.0;	

		if(m_bShowRect)
		{
			VerColor2D* pVBRenct;
			RenderState* pLRS;
			Matrix = pGraphic->GetRenderStack( RS_GUI, NULL, NULL, PT_TRIANGLESTRIP, 
				4, 2, VerColor2D::Format, sizeof(VerColor2D), (void**)&pVBRenct, NULL, (void**)&pLRS );
			Matrix->Identity();
			pLRS->m_Cull = 1;
			pLRS->m_AlpOP_S1		= TOP_SELECTARG1;
			pLRS->m_AlpArg1_S1		= TA_DIFFUSE;

			pLRS->m_TexOP_S1		= TOP_BLENDDIFFUSEALPHA;
			pLRS->m_TexArg1_S1		= TA_CURRENT;
			pLRS->m_TexArg2_S1		= TA_DIFFUSE;

			pLRS->m_AlphaBlendEnable = TRUE;
			pLRS->m_AlphaTestEnable = FALSE;
			pLRS->m_ZTestEnable		= FALSE;

			pVBRenct[0].x = m_fOffX + m_fRect.left;  	pVBRenct[0].y = m_fOffY + m_fRect.top;		pVBRenct[0].z = 1.0f;  
			pVBRenct[1].x = m_fOffX + m_fRect.right;	pVBRenct[1].y = m_fOffY + m_fRect.top;		pVBRenct[1].z = 1.0f;	
			pVBRenct[2].x = m_fOffX + m_fRect.left;		pVBRenct[2].y = m_fOffY + m_fRect.bottom;	pVBRenct[2].z = 1.0f;	
			pVBRenct[3].x = m_fOffX + m_fRect.right;	pVBRenct[3].y = m_fOffY + m_fRect.bottom;	pVBRenct[3].z = 1.0f;

			pVBRenct[0].w = 1.0f;
			pVBRenct[1].w = 1.0f;
			pVBRenct[2].w = 1.0f;
			pVBRenct[3].w = 1.0f;

			pVBRenct[0].diffuse = pVBRenct[1].diffuse = pVBRenct[2].diffuse = pVBRenct[3].diffuse = 0x800000FF;
		}
	}
}
