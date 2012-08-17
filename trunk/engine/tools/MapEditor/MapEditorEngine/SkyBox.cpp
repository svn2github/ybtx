#include "stdafx.h"
#include "SkyBox.h"
#include "Render.h"

namespace sqr
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
		m_vPositions[0] = CVector3f(-fHalfWidth, -fHalfHeight, fHalfDepth);
		m_vPositions[1] = CVector3f(-fHalfWidth, fHalfHeight, fHalfDepth);
		m_vPositions[2] = CVector3f( fHalfWidth,-fHalfHeight, fHalfDepth);
		m_vPositions[3] = CVector3f( fHalfWidth, fHalfHeight, fHalfDepth);
		
		m_vUVs[0] = CVector2f(s,e);
		m_vUVs[1] = CVector2f(s,s);
		m_vUVs[2] = CVector2f(e,e);
		m_vUVs[3] = CVector2f(e,s);

		// back
		m_vPositions[4] = CVector3f(fHalfWidth, -fHalfHeight, -fHalfDepth);
		m_vPositions[5] = CVector3f(fHalfWidth, fHalfHeight, -fHalfDepth);
		m_vPositions[6] = CVector3f(-fHalfWidth,-fHalfHeight, -fHalfDepth);
		m_vPositions[7] = CVector3f(-fHalfWidth, fHalfHeight, -fHalfDepth);
		
		m_vUVs[4] = CVector2f(s,e);
		m_vUVs[5] = CVector2f(s,s);
		m_vUVs[6] = CVector2f(e,e);
		m_vUVs[7] = CVector2f(e,s);

		// left
		m_vPositions[8] = CVector3f(-fHalfWidth, -fHalfHeight, -fHalfDepth);
		m_vPositions[9] = CVector3f(-fHalfWidth, fHalfHeight, -fHalfDepth);
		m_vPositions[10] = CVector3f(-fHalfWidth,-fHalfHeight, fHalfDepth);
		m_vPositions[11] = CVector3f(-fHalfWidth, fHalfHeight, fHalfDepth);

		m_vUVs[8] = CVector2f(s,e);
		m_vUVs[9] = CVector2f(s,s);
		m_vUVs[10] = CVector2f(e,e);
		m_vUVs[11] = CVector2f(e,s);

		// right
		m_vPositions[12] = CVector3f(fHalfWidth, -fHalfHeight, fHalfDepth);
		m_vPositions[13] = CVector3f(fHalfWidth, fHalfHeight, fHalfDepth);
		m_vPositions[14] = CVector3f(fHalfWidth,-fHalfHeight, -fHalfDepth);
		m_vPositions[15] = CVector3f(fHalfWidth, fHalfHeight, -fHalfDepth);

		m_vUVs[12] = CVector2f(s,e);
		m_vUVs[13] = CVector2f(s,s);
		m_vUVs[14] = CVector2f(e,e);
		m_vUVs[15] = CVector2f(e,s);

		// top
		m_vPositions[16] = CVector3f(-fHalfWidth, fHalfHeight, fHalfDepth);
		m_vPositions[17] = CVector3f(-fHalfWidth, fHalfHeight, -fHalfDepth);
		m_vPositions[18] = CVector3f(fHalfWidth, fHalfHeight, fHalfDepth);
		m_vPositions[19] = CVector3f(fHalfWidth, fHalfHeight, -fHalfDepth);

		m_vUVs[16] = CVector2f(s,e);
		m_vUVs[17] = CVector2f(s,s);
		m_vUVs[18] = CVector2f(e,e);
		m_vUVs[19] = CVector2f(e,s);
	}

	void CSkyBox::Render()
	{
		CMatrix mat, matOld;

		//pRender->GetRawDevice()->GetTransform(D3DTS_VIEW,&matOld);
		//mat = matOld;

		//mat._41 = 0.0f;
		//mat._42 = 0.0f;
		//mat._43 = 0.0f;
		//
		//pRender->GetRawDevice()->SetTransform(D3DTS_VIEW,&mat);

		//D3DCOLOR    color[6] = { 0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff};

		//pRender->GetRawDevice()->SetTransform(D3DTS_VIEW,&matOld);
	}

	void CSkyBox::Update(InputEvent & event)
	{
		//for ( int i = 0; i < 20; i++)
		//{
		//	//m_vUVs[i] += CVector2f(0.02,0.0); 
		//}
	}
}