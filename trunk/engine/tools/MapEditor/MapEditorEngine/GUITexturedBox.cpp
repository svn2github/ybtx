#include "stdafx.h"
#include "BluelyG.h"
#include "Scene.h"
#include "Render.h"
#include "GUITexturedBox.h"

namespace bly
{

void CGUITexturedBox::Render()
{
	//CRenderDevice * pRender = CRenderDevice::GetInstance();
	//CRenderDevice::Layer3D * pLayer3D = pRender->GetLayer3D();
	//
	//LPDIRECT3DSURFACE9 pBackBuffer;
	//
	//m_pSwapChain->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer);
	//pRender->GetRawDevice()->SetRenderTarget(0, pBackBuffer);

	//pRender->Clear(0xff0000ff);
	//pRender->BeginScene();
	//
	//pRender->GetRawDevice()->SetTransform(D3DTS_WORLD,&this->GetAbsolute());
	//pLayer3D->SetShader(m_nShaderHandle);
	//pLayer3D->DrawTriangleStrip(2, m_vPos, NULL, m_dwClr, m_vUV, NULL,true);
	//
	//pRender->EndScene();
	//
	//pRender->Present(m_hWnd,m_pSwapChain);
}

void CGUITexturedBox::Create(DWORD dwWidth, DWORD dwHeight, DWORD dwTextureHandle, HWND hWnd, DWORD dwColor)
{
	m_dwTextureHandle = dwTextureHandle;
	m_dwColor = dwColor;

	IGUISceneNode::Create(dwWidth, dwHeight, hWnd);

	// create shader
	CRenderDevice::Layer3D * pLayer3D = CRenderDevice::GetInstance()->GetLayer3D();
	m_nShaderHandle = pLayer3D->CreateShader(dwTextureHandle,INVALID,EL3DST_SOLID_1_LAYER,EL3DMST_POSCOLTEX0,false,false,true,true,true,true,false,false);
	m_vPos[0] = D3DXVECTOR3(-1.0f, -1.0f, 0.0f);
	m_vPos[1] = D3DXVECTOR3(-1.0f, 1.0f, 0.0f);
	m_vPos[2] = D3DXVECTOR3(1.0f, -1.0f, 0.0f);
	m_vPos[3] = D3DXVECTOR3(1.0f, 1.0f, 0.0f);

	m_dwClr[0] = dwColor;
	m_dwClr[1] = dwColor;
	m_dwClr[2] = dwColor;
	m_dwClr[3] = dwColor;

	m_vUV[0] = D3DXVECTOR2(0.0f,1.0f);
	m_vUV[1] = D3DXVECTOR2(0.0f,0.0f);
	m_vUV[2] = D3DXVECTOR2(1.0f,1.0f);
	m_vUV[3] = D3DXVECTOR2(1.0f,0.0f);
}

//////////////////////////////////////////////////////////////////////////


void CGUICheckBox::Update(CInputDevice::SEvent & event)
{
	if ( m_bMove )
	{
		m_vUV[0].x += 0.025f;
		m_vUV[1].x += 0.025f;
		m_vUV[2].x += 0.025f;
		m_vUV[3].x += 0.025f;
		m_vUV[0].y += 0.025f;
		m_vUV[1].y += 0.025f;
		m_vUV[2].y += 0.025f;
		m_vUV[3].y += 0.025f;
	}
}

void CGUICheckBox::SetHot(bool b)
{
	m_bHot = b;
	if ( b )
	{
		m_bMove = false;
		m_dwClr[0] = 0xff202020;
		m_dwClr[1] = 0xff202020;
		m_dwClr[2] = 0xff202020;
		m_dwClr[3] = 0xff202020;
		m_vPos[0] = D3DXVECTOR3(-.9f, -.9f, 0.0f);
		m_vPos[1] = D3DXVECTOR3(-.9f, .9f, 0.0f);
		m_vPos[2] = D3DXVECTOR3(.9f, -.9f, 0.0f);
		m_vPos[3] = D3DXVECTOR3(.9f, .9f, 0.0f);
	}
	else
	{
		m_dwClr[0] = 0xff000000;
		m_dwClr[1] = 0xff000000;
		m_dwClr[2] = 0xff000000;
		m_dwClr[3] = 0xff000000;
		m_vPos[0] = D3DXVECTOR3(-1.0f, -1.0f, 0.0f);
		m_vPos[1] = D3DXVECTOR3(-1.0f, 1.0f, 0.0f);
		m_vPos[2] = D3DXVECTOR3(1.0f, -1.0f, 0.0f);
		m_vPos[3] = D3DXVECTOR3(1.0f, 1.0f, 0.0f);
	}
}

}