//////////////////////////////////////////////////////////////////////////
//  	
//	Copyright (C) 2003-2004
//	All Rights Reserved
//  In memory of bluely
//  
//////////////////////////////////////////////////////////////////////////

#include "SkyBox.h"
#include "Render.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
namespace bg
{
namespace sc
{

CSkyBox::CSkyBox(ISceneNode * pParent) : ISceneNode(pParent){ }

CSkyBox::~CSkyBox()
{
}

void CSkyBox::Create(float fWidth, float fHeight, float fDepth, DWORD dwTextureHandleFront, DWORD dwTextureHandleBack, DWORD dwTextureHandleLeft, DWORD dwTextureHandleRight, DWORD dwTextureHandleTop, DWORD dwTexelCnt)
{
	m_fWidth = fWidth;
	m_fHeight = fHeight;
	m_fDepth = fDepth;
	m_dwTextureHandles[EST_FRONT] = dwTextureHandleFront;
	m_dwTextureHandles[EST_BACK] = dwTextureHandleBack;
	m_dwTextureHandles[EST_LEFT] = dwTextureHandleLeft;
	m_dwTextureHandles[EST_RIGHT] = dwTextureHandleRight;
	m_dwTextureHandles[EST_TOP] = dwTextureHandleTop;

	float fHalfWidth = fWidth  * 0.5f;
	float fHalfHeight= fHeight * 0.5f;
	float fHalfDepth = fDepth  * 0.5f;

	float delta = 1.0f / dwTexelCnt*1.5f; 

	delta = 0.0f;


	float e = 1.0f - delta;
	float s = 0.0f + delta;

	// front
	m_vPositions[0] = D3DXVECTOR3(-fHalfWidth, -fHalfHeight, fHalfDepth);
	m_vPositions[1] = D3DXVECTOR3(-fHalfWidth, fHalfHeight, fHalfDepth);
	m_vPositions[2] = D3DXVECTOR3( fHalfWidth,-fHalfHeight, fHalfDepth);
	m_vPositions[3] = D3DXVECTOR3( fHalfWidth, fHalfHeight, fHalfDepth);
	
	m_vUVs[0] = D3DXVECTOR2(s,e);
	m_vUVs[1] = D3DXVECTOR2(s,s);
	m_vUVs[2] = D3DXVECTOR2(e,e);
	m_vUVs[3] = D3DXVECTOR2(e,s);

	// back
	m_vPositions[4] = D3DXVECTOR3(fHalfWidth, -fHalfHeight, -fHalfDepth);
	m_vPositions[5] = D3DXVECTOR3(fHalfWidth, fHalfHeight, -fHalfDepth);
	m_vPositions[6] = D3DXVECTOR3(-fHalfWidth,-fHalfHeight, -fHalfDepth);
	m_vPositions[7] = D3DXVECTOR3(-fHalfWidth, fHalfHeight, -fHalfDepth);
	
	m_vUVs[4] = D3DXVECTOR2(s,e);
	m_vUVs[5] = D3DXVECTOR2(s,s);
	m_vUVs[6] = D3DXVECTOR2(e,e);
	m_vUVs[7] = D3DXVECTOR2(e,s);

	// left
	m_vPositions[8] = D3DXVECTOR3(-fHalfWidth, -fHalfHeight, -fHalfDepth);
	m_vPositions[9] = D3DXVECTOR3(-fHalfWidth, fHalfHeight, -fHalfDepth);
	m_vPositions[10] = D3DXVECTOR3(-fHalfWidth,-fHalfHeight, fHalfDepth);
	m_vPositions[11] = D3DXVECTOR3(-fHalfWidth, fHalfHeight, fHalfDepth);

	m_vUVs[8] = D3DXVECTOR2(s,e);
	m_vUVs[9] = D3DXVECTOR2(s,s);
	m_vUVs[10] = D3DXVECTOR2(e,e);
	m_vUVs[11] = D3DXVECTOR2(e,s);

	// right
	m_vPositions[12] = D3DXVECTOR3(fHalfWidth, -fHalfHeight, fHalfDepth);
	m_vPositions[13] = D3DXVECTOR3(fHalfWidth, fHalfHeight, fHalfDepth);
	m_vPositions[14] = D3DXVECTOR3(fHalfWidth,-fHalfHeight, -fHalfDepth);
	m_vPositions[15] = D3DXVECTOR3(fHalfWidth, fHalfHeight, -fHalfDepth);

	m_vUVs[12] = D3DXVECTOR2(s,e);
	m_vUVs[13] = D3DXVECTOR2(s,s);
	m_vUVs[14] = D3DXVECTOR2(e,e);
	m_vUVs[15] = D3DXVECTOR2(e,s);

	// top
	m_vPositions[16] = D3DXVECTOR3(-fHalfWidth, fHalfHeight, fHalfDepth);
	m_vPositions[17] = D3DXVECTOR3(-fHalfWidth, fHalfHeight, -fHalfDepth);
	m_vPositions[18] = D3DXVECTOR3(fHalfWidth, fHalfHeight, fHalfDepth);
	m_vPositions[19] = D3DXVECTOR3(fHalfWidth, fHalfHeight, -fHalfDepth);

	m_vUVs[16] = D3DXVECTOR2(s,e);
	m_vUVs[17] = D3DXVECTOR2(s,s);
	m_vUVs[18] = D3DXVECTOR2(e,e);
	m_vUVs[19] = D3DXVECTOR2(e,s);

	rd::CRenderDevice * pRender = rd::CRenderDevice::GetInstance();
	rd::CRenderDevice::Layer3D * pLayer3d = pRender->GetLayer3D();

	// create shader here...
	m_dwShaderHandle = pLayer3d->CreateShader(INVALID,INVALID,rd::EL3DST_SOLID_1_LAYER,rd::EL3DMST_POSCOLTEX0,false,false,true,false,true,true,false,false);
}

void CSkyBox::Render()
{	
	rd::CRenderDevice * pRender = rd::CRenderDevice::GetInstance();
	rd::CRenderDevice::Layer3D * pLayer3d = pRender->GetLayer3D();

	D3DXMATRIX mat, matOld;

	pRender->GetRawDevice()->GetTransform(D3DTS_VIEW,&matOld);
	mat = matOld;

	mat._41 = 0.0f;
	mat._42 = 0.0f;
	mat._43 = 0.0f;
	
	pRender->GetRawDevice()->SetTransform(D3DTS_VIEW,&mat);

	pLayer3d->SetShader(m_dwShaderHandle);

	D3DCOLOR    color[6] = { 0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff};

	D3DXVECTOR3 * pPos = m_vPositions;
	D3DXVECTOR2 * pUV = m_vUVs;

	pLayer3d->GetShader(m_dwShaderHandle)->iTexHandle0 = m_dwTextureHandles[EST_FRONT];
	pLayer3d->DrawTriangleStrip(2,pPos,NULL,color,pUV,NULL);
	
	pPos += 4;pUV += 4;
	pLayer3d->GetShader(m_dwShaderHandle)->iTexHandle0 = m_dwTextureHandles[EST_BACK];
	pLayer3d->DrawTriangleStrip(2,pPos,NULL,color,pUV,NULL);
		
	pPos += 4;pUV += 4;
	pLayer3d->GetShader(m_dwShaderHandle)->iTexHandle0 = m_dwTextureHandles[EST_LEFT];
	pLayer3d->DrawTriangleStrip(2,pPos,NULL,color,pUV,NULL);
	
	pPos += 4;pUV += 4;
	pLayer3d->GetShader(m_dwShaderHandle)->iTexHandle0 = m_dwTextureHandles[EST_RIGHT];
	pLayer3d->DrawTriangleStrip(2,pPos,NULL,color,pUV,NULL);
	
	pPos += 4;pUV += 4;
	pLayer3d->GetShader(m_dwShaderHandle)->iTexHandle0 = m_dwTextureHandles[EST_TOP];
	pLayer3d->DrawTriangleStrip(2,pPos,NULL,color,pUV,NULL);

	pRender->GetRawDevice()->SetTransform(D3DTS_VIEW,&matOld);
}

void CSkyBox::Update(ip::CInputDevice::SEvent & event)
{
	//for ( int i = 0; i < 20; i++)
	//{
	//	//m_vUVs[i] += D3DXVECTOR2(0.02,0.0); 
	//}
}


}
}