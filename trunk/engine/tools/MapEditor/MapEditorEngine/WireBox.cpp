#include "stdafx.h"
#include "WireBox.h"
#include "Render.h"
#include "Picker.h"
#include "CGraphic.h"
#include "CRenderPipeline.h"

namespace sqr
{
	CWireBox::CWireBox(ISceneNode * pParent, bool bIsVisible) : ISceneNode(pParent,bIsVisible)
	{
	}

	void CWireBox::Create(float fHeight, float fWidth, float fDepth, DWORD dwColor)
	{
		m_dwColor = dwColor;
		m_fHeight = fHeight;
		m_fWidth = fWidth;
		m_fDepth = fDepth;
		
		// up down left right front rear clockwise
		m_vPos[0] = CVector3f(-fWidth/2, fHeight, -fDepth/2);
		m_vPos[1] = m_vPos[0] + CVector3f(0.0f,0.0f,fDepth); 
		m_vPos[2] = m_vPos[1] + CVector3f(fWidth,0.0f,0.0f); 
		m_vPos[3] = m_vPos[0] + CVector3f(fWidth,0.0f,0.0f); 
		m_vPos[4] = m_vPos[0];
		
		m_vPos[5] = m_vPos[0] + CVector3f(0.0f,-fHeight,0.0f);
		m_vPos[6] = m_vPos[1] + CVector3f(0.0f,-fHeight,0.0f);
		m_vPos[7] = m_vPos[2] + CVector3f(0.0f,-fHeight,0.0f);
		m_vPos[8] = m_vPos[3] + CVector3f(0.0f,-fHeight,0.0f);
		m_vPos[9] = m_vPos[5];
		
		m_vPos[10] = m_vPos[5];
		m_vPos[11] = m_vPos[0];
		m_vPos[12] = m_vPos[1];
		m_vPos[13] = m_vPos[6];
		m_vPos[14] = m_vPos[5];
		
		m_vPos[15] = m_vPos[8];
		m_vPos[16] = m_vPos[3];
		m_vPos[17] = m_vPos[2];
		m_vPos[18] = m_vPos[7];
		m_vPos[19] = m_vPos[8];

		m_vPos[20] = m_vPos[5];
		m_vPos[21] = m_vPos[0];
		m_vPos[22] = m_vPos[3];
		m_vPos[23] = m_vPos[8];
		m_vPos[24] = m_vPos[5];

		m_vPos[25] = m_vPos[6];
		m_vPos[26] = m_vPos[1];
		m_vPos[27] = m_vPos[2];
		m_vPos[28] = m_vPos[7];
		m_vPos[29] = m_vPos[6];

		for ( int i = 0; i < sizeof(m_dwClr)/sizeof(m_dwClr[0]); i++ )
		{
			m_dwClr[i] = dwColor;
		}
	}

	void CWireBox::SetColor(DWORD dwClr)
	{
		for ( int i = 0; i < sizeof(m_dwClr)/sizeof(m_dwClr[0]); i++ )
		{
			m_dwClr[i] = dwClr;
		}
	}

	void CWireBox::Render( CGraphic * pCoreGraphic, bool bBefore, bool bRenderLine )
	{
		if( bRenderLine )
		{
			this->RenderBoxLine();
			return;
		}

		CVector3f * pvPos = m_vPos;
		DWORD * pdwClr = m_dwClr;

		CMatrix *pcm;
		VerColor3D* pVB;
		WORD*		pIB;
		WORD		ibBuf[] = { 0,1,1,2,2,3,3,0,
								4,5,5,6,6,7,7,4,
								0,4,1,5,2,6,3,7};

		RenderState * pRS;

		CRenderPipeline::GetInst()->SetZDelta( -1.0f );
		float r,g,b;
		b = (pdwClr[0] & 0xff) / 255.0f;
		g = ((pdwClr[0]>>8) & 0xff) / 255.0f;
		r = ((pdwClr[0]>>16) & 0xff) / 255.0f;
	    
		if( bBefore )
			pcm = pCoreGraphic->GetRenderStack( RS_DEFAULT, NULL, NULL, PT_LINELIST, 8, 12, VerColor3D::Format, sizeof(VerColor3D),(void**)&pVB, (void**)&pIB, (void**)&pRS);
		else
			pcm = pCoreGraphic->GetRenderStack( RS_DEFAULT, ROS_LIGHTMAT1, NULL, NULL, PT_LINELIST, 8, 12, VerColor3D::Format, sizeof(VerColor3D),(void**)&pVB, (void**)&pIB, (void**)&pRS);

		pVB[0].p = *(CVector3f*)&pvPos[0]; pVB[0].diffuse = pdwClr[0];
		pVB[1].p = *(CVector3f*)&pvPos[1]; pVB[1].diffuse = pdwClr[1];
		pVB[2].p = *(CVector3f*)&pvPos[2]; pVB[2].diffuse = pdwClr[2];
		pVB[3].p = *(CVector3f*)&pvPos[3]; pVB[3].diffuse = pdwClr[3];
		
		pvPos += 5;
		pVB[4].p = *(CVector3f*)&pvPos[0]; pVB[4].diffuse = pdwClr[0];
		pVB[5].p = *(CVector3f*)&pvPos[1]; pVB[5].diffuse = pdwClr[1];
		pVB[6].p = *(CVector3f*)&pvPos[2]; pVB[6].diffuse = pdwClr[2];
		pVB[7].p = *(CVector3f*)&pvPos[3]; pVB[7].diffuse = pdwClr[3];

		for( int i = 0; i < 24 ; i += 4)
		{
			pIB[i]		= ibBuf[i];
			pIB[i + 1]	= ibBuf[i + 1];
			pIB[i + 2]	= ibBuf[i + 2];
			pIB[i + 3]	= ibBuf[i + 3];
		}

		if( bBefore )
			pRS->m_ZTestEnable = false;
		else
			pRS->m_ZTestEnable = true;

		pRS->m_LightEnable = false;
		pRS->m_ZWrite = true;
		pRS->m_Cull = CULLT_NONE;
		pRS->m_Material.Emissive.r = r;    pRS->m_Material.Emissive.g = g;    pRS->m_Material.Emissive.b = b;
		memcpy ( (void*)pcm, (void*)&this->GetAbsolute(), sizeof(CMatrix));
		CRenderPipeline::GetInst()->SetZDelta( 0.0f );
	}

	void CWireBox::RenderBoxLine()
	{
		CRenderDevice * pRender = CRenderDevice::GetInst();

		CVector3f start = m_vecRelativeTranslation;
		CVector3f end   = m_vecRelativeTranslation;
		end.y			= end.y + m_fHeight;

		if( start != end )
			pRender->Draw3DLines(start, end, 0xff0000ff);
	}

	void CWireBox::IncreaseHeight()
	{
		m_fHeight += SPACE_GRANULARITY;

		UpdateWireBox();
	}

	void CWireBox::DecreaseHeight()
	{
		m_fHeight -= SPACE_GRANULARITY;

		UpdateWireBox();
	}

	float CWireBox::GetHeight() 
	{
		return m_fHeight;
	}

	float CWireBox::GetWidth() 
	{
		return m_fWidth;
	}

	float CWireBox::GetDepth()
	{
		return m_fDepth;
	}

	void CWireBox::SetHeight(float fHeight)
	{
		m_fHeight = fHeight;
		UpdateWireBox();
	}

	void CWireBox::SetWireBoxRange( float fHeight, float fWidth, float fDepth )
	{
		m_fHeight = fHeight;
		m_fWidth  = fWidth;
		m_fDepth  = fDepth;

		UpdateWireBox();
	}

	void CWireBox::UpdateWireBox()
	{
		// up down left right front rear clockwise
		m_vPos[0] = CVector3f(-m_fWidth/2, m_fHeight, -m_fDepth/2);
		m_vPos[1] = m_vPos[0] + CVector3f(0.0f,0.0f,m_fDepth); 
		m_vPos[2] = m_vPos[1] + CVector3f(m_fWidth,0.0f,0.0f); 
		m_vPos[3] = m_vPos[0] + CVector3f(m_fWidth,0.0f,0.0f); 
		m_vPos[4] = m_vPos[0];

		m_vPos[5] = m_vPos[0] + CVector3f(0.0f,-m_fHeight,0.0f);
		m_vPos[6] = m_vPos[1] + CVector3f(0.0f,-m_fHeight,0.0f);
		m_vPos[7] = m_vPos[2] + CVector3f(0.0f,-m_fHeight,0.0f);
		m_vPos[8] = m_vPos[3] + CVector3f(0.0f,-m_fHeight,0.0f);
		m_vPos[9] = m_vPos[5];

		m_vPos[10] = m_vPos[5];
		m_vPos[11] = m_vPos[0];
		m_vPos[12] = m_vPos[1];
		m_vPos[13] = m_vPos[6];
		m_vPos[14] = m_vPos[5];

		m_vPos[15] = m_vPos[8];
		m_vPos[16] = m_vPos[3];
		m_vPos[17] = m_vPos[2];
		m_vPos[18] = m_vPos[7];
		m_vPos[19] = m_vPos[8];

		m_vPos[20] = m_vPos[5];
		m_vPos[21] = m_vPos[0];
		m_vPos[22] = m_vPos[3];
		m_vPos[23] = m_vPos[8];
		m_vPos[24] = m_vPos[5];

		m_vPos[25] = m_vPos[6];
		m_vPos[26] = m_vPos[1];
		m_vPos[27] = m_vPos[2];
		m_vPos[28] = m_vPos[7];
		m_vPos[29] = m_vPos[6];
	}

}
