//////////////////////////////////////////////////////////////////////////
//	
//	Copyright (C) 2003-2004
//  All Rights Reserved
//  In memory of bluely
//  
//////////////////////////////////////////////////////////////////////////

#include "Square.h"
#include "Render.h"
#include "Picker.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace bg
{
namespace sc
{

CSquare::CSquare(ISceneNode * pParent, DWORD dwTextureHandle, float fSize, DWORD dwColor) : 
	ISceneNode(pParent), m_dwTextureHandle(dwTextureHandle), m_fSize(fSize),m_dwColor(dwColor)
{
	// create shader
	rd::CRenderDevice::Layer3D * pLayer3D = rd::CRenderDevice::GetInstance()->GetLayer3D();
	m_dwShaderHandle = pLayer3D->CreateShader(dwTextureHandle,INVALID,rd::EL3DST_SOLID_1_LAYER,rd::EL3DMST_POSCOLTEX0,false,false,true,true,true,true,false,false);
	m_vPos[0] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_vPos[1] = D3DXVECTOR3(0.0f, 0.0f, fSize);
	m_vPos[2] = D3DXVECTOR3(fSize, 0.0f, 0.0f);
	m_vPos[3] = D3DXVECTOR3(fSize, 0.0f,fSize);

	m_dwClr[0] = dwColor;
	m_dwClr[1] = dwColor;
	m_dwClr[2] = dwColor;
	m_dwClr[3] = dwColor;
	
	m_vUV[0] = D3DXVECTOR2(0.0f,fSize);
	m_vUV[1] = D3DXVECTOR2(0.0f,0.0f);
	m_vUV[2] = D3DXVECTOR2(fSize,fSize);
	m_vUV[3] = D3DXVECTOR2(fSize,0.0f);
}

void CSquare::Render()
{
	rd::CRenderDevice * pRender = rd::CRenderDevice::GetInstance();
	rd::CRenderDevice::Layer3D * pLayer3D = rd::CRenderDevice::GetInstance()->GetLayer3D();
	pRender->GetRawDevice()->SetTransform(D3DTS_WORLD,&this->GetAbsolute());
	pLayer3D->SetShader(m_dwShaderHandle);
	pLayer3D->DrawTriangleStrip(2, m_vPos, NULL, m_dwClr, m_vUV, NULL);
}

void CSquare::OnEvent(ip::CInputDevice::SEvent & event)
{
	//switch ( event.eType )
	//{
	//case ip::EIET_MS_MOVE :
	//	{
	//		D3DXVECTOR3 v = rd::CPicker::GetInstance()->GetPickPos();
	//		v.x = floor(v.x);
	//		v.z = floor(v.z);
	//		v.y = 0.01f;
	//		this->SetRelativePos(v);
	//		this->UpdateAbsolute();
	//	}
	//break;
	//}
}


}
}