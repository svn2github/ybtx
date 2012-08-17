//////////////////////////////////////////////////////////////////////////
//	
//	Copyright (C) 2003-2004
//  All Rights Reserved
//  In memory of bluely
//  
//////////////////////////////////////////////////////////////////////////

#include "WireBox.h"
#include "Render.h"
#include "Picker.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace bg
{
namespace sc
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
	
	// create shader
	rd::CRenderDevice::Layer3D * pLayer3D = rd::CRenderDevice::GetInstance()->GetLayer3D();
	
	m_dwShaderHandle = pLayer3D->CreateShader(INVALID,INVALID,rd::EL3DST_WIREFRAME_0_LAYER,rd::EL3DMST_POSCOL,false,false,true,true,true,true,false,false);
	
	// up down left right front rear clockwise
	m_vPos[0] = D3DXVECTOR3(-fWidth/2, fHeight, -fDepth/2);
	m_vPos[1] = m_vPos[0] + D3DXVECTOR3(0.0f,0.0f,fDepth); 
	m_vPos[2] = m_vPos[1] + D3DXVECTOR3(fWidth,0.0f,0.0f); 
	m_vPos[3] = m_vPos[0] + D3DXVECTOR3(fWidth,0.0f,0.0f); 
	m_vPos[4] = m_vPos[0];
	
	m_vPos[5] = m_vPos[0] + D3DXVECTOR3(0.0f,-fHeight,0.0f);
	m_vPos[6] = m_vPos[1] + D3DXVECTOR3(0.0f,-fHeight,0.0f);
	m_vPos[7] = m_vPos[2] + D3DXVECTOR3(0.0f,-fHeight,0.0f);
	m_vPos[8] = m_vPos[3] + D3DXVECTOR3(0.0f,-fHeight,0.0f);
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

void CWireBox::Render()
{
	rd::CRenderDevice * pRender = rd::CRenderDevice::GetInstance();
	rd::CRenderDevice::Layer3D * pLayer3D = rd::CRenderDevice::GetInstance()->GetLayer3D();
	pRender->GetRawDevice()->SetTransform(D3DTS_WORLD,&this->GetAbsolute());
	pLayer3D->SetShader(m_dwShaderHandle);
	
	//// Use depth bias
	//pRender->GetRawDevice()->SetRenderState( D3DRS_SLOPESCALEDEPTHBIAS, F2DW(0.0f) );
	//pRender->GetRawDevice()->SetRenderState( D3DRS_DEPTHBIAS, F2DW(-.1f) );
	
	pRender->GetRawDevice()->SetRenderState(D3DRS_ZFUNC,D3DCMP_ALWAYS);
	
	D3DXVECTOR3 * pvPos = m_vPos;
	DWORD * pdwClr = m_dwClr;
	
	pLayer3D->DrawLineStrip(4, pvPos, pdwClr);
	
	pvPos += 5;
	pdwClr += 5;
	
	pLayer3D->DrawLineStrip(4, pvPos, pdwClr);
	
	pvPos += 5;
	pdwClr += 5;
	pLayer3D->DrawLineStrip(4, pvPos, pdwClr);

	pvPos += 5;
	pdwClr += 5;
	pLayer3D->DrawLineStrip(4, pvPos, pdwClr);

	pvPos += 5;
	pdwClr += 5;
	pLayer3D->DrawLineStrip(4, pvPos, pdwClr);

	pvPos += 5;
	pdwClr += 5;
	pLayer3D->DrawLineStrip(4, pvPos, pdwClr);

	//pRender->GetRawDevice()->SetRenderState( D3DRS_SLOPESCALEDEPTHBIAS, F2DW(0.0f) );
	//pRender->GetRawDevice()->SetRenderState( D3DRS_DEPTHBIAS, F2DW(0.0f) );
	
	pRender->GetRawDevice()->SetRenderState(D3DRS_ZFUNC,D3DCMP_LESSEQUAL);
}

void CWireBox::IncreaseHeight()
{
	m_fHeight += SPACE_GRANULARITY;

	// up down left right front rear clockwise
	m_vPos[0] = D3DXVECTOR3(-m_fWidth/2, m_fHeight, -m_fDepth/2);
	m_vPos[1] = m_vPos[0] + D3DXVECTOR3(0.0f,0.0f,m_fDepth); 
	m_vPos[2] = m_vPos[1] + D3DXVECTOR3(m_fWidth,0.0f,0.0f); 
	m_vPos[3] = m_vPos[0] + D3DXVECTOR3(m_fWidth,0.0f,0.0f); 
	m_vPos[4] = m_vPos[0];

	m_vPos[5] = m_vPos[0] + D3DXVECTOR3(0.0f,-m_fHeight,0.0f);
	m_vPos[6] = m_vPos[1] + D3DXVECTOR3(0.0f,-m_fHeight,0.0f);
	m_vPos[7] = m_vPos[2] + D3DXVECTOR3(0.0f,-m_fHeight,0.0f);
	m_vPos[8] = m_vPos[3] + D3DXVECTOR3(0.0f,-m_fHeight,0.0f);
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

void CWireBox::DecreaseHeight()
{
	m_fHeight -= SPACE_GRANULARITY;

	// up down left right front rear clockwise
	m_vPos[0] = D3DXVECTOR3(-m_fWidth/2, m_fHeight, -m_fDepth/2);
	m_vPos[1] = m_vPos[0] + D3DXVECTOR3(0.0f,0.0f,m_fDepth); 
	m_vPos[2] = m_vPos[1] + D3DXVECTOR3(m_fWidth,0.0f,0.0f); 
	m_vPos[3] = m_vPos[0] + D3DXVECTOR3(m_fWidth,0.0f,0.0f); 
	m_vPos[4] = m_vPos[0];

	m_vPos[5] = m_vPos[0] + D3DXVECTOR3(0.0f,-m_fHeight,0.0f);
	m_vPos[6] = m_vPos[1] + D3DXVECTOR3(0.0f,-m_fHeight,0.0f);
	m_vPos[7] = m_vPos[2] + D3DXVECTOR3(0.0f,-m_fHeight,0.0f);
	m_vPos[8] = m_vPos[3] + D3DXVECTOR3(0.0f,-m_fHeight,0.0f);
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

float CWireBox::GetHeight() 
{
	return m_fHeight;
}

void CWireBox::SetHeight(float fHeight)
{
	m_fHeight = fHeight;

	// up down left right front rear clockwise
	m_vPos[0] = D3DXVECTOR3(-m_fWidth/2, m_fHeight, -m_fDepth/2);
	m_vPos[1] = m_vPos[0] + D3DXVECTOR3(0.0f,0.0f,m_fDepth); 
	m_vPos[2] = m_vPos[1] + D3DXVECTOR3(m_fWidth,0.0f,0.0f); 
	m_vPos[3] = m_vPos[0] + D3DXVECTOR3(m_fWidth,0.0f,0.0f); 
	m_vPos[4] = m_vPos[0];

	m_vPos[5] = m_vPos[0] + D3DXVECTOR3(0.0f,-m_fHeight,0.0f);
	m_vPos[6] = m_vPos[1] + D3DXVECTOR3(0.0f,-m_fHeight,0.0f);
	m_vPos[7] = m_vPos[2] + D3DXVECTOR3(0.0f,-m_fHeight,0.0f);
	m_vPos[8] = m_vPos[3] + D3DXVECTOR3(0.0f,-m_fHeight,0.0f);
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
}