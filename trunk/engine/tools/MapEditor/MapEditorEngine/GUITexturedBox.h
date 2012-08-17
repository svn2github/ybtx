//////////////////////////////////////////////////////////////////////////
//
//	Copyright (C) 2003-2004
//	All Rights Reserved
//  In memory of Millennium 2000
//
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "BluelyG.h"
#include "Scene.h"
#include "CMapEditApp.h"

namespace bly
{

class CGUITexturedBox : public IGUISceneNode
{
protected:
	DWORD m_dwTextureHandle;
	DWORD m_dwColor;

	// in strip
	D3DXVECTOR3 m_vPos[4];
	D3DXVECTOR2 m_vUV[4];
	DWORD m_dwClr[4];

public:
	CGUITexturedBox(ISceneNode * pParent) : IGUISceneNode(pParent),m_dwTextureHandle(INVALID) { }
	~CGUITexturedBox() { }
	void Render();
	void Create(DWORD dwWidth, DWORD dwHeight, DWORD dwTextureHandle, HWND hWnd, DWORD dwColor);
	DWORD GetTextureHandle()
	{
		return m_dwTextureHandle;
	}
	void SetTextureHandle(DWORD dw)
	{
		CRenderDevice * pRender = CRenderDevice::GetInstance();
		CRenderDevice::Layer3D * pLayer3d = pRender->GetLayer3D();
		pLayer3d->GetShader(m_nShaderHandle)->iTexHandle0 = dw;
		m_dwTextureHandle = dw;
		//CMapEditApp::GetInstance()->GetOperator()->SetTextureHandleHot(dw); //×¢ÊÍby baozi
	}
};

//////////////////////////////////////////////////////////////////////////

class CGUICheckBox : public CGUITexturedBox
{
	bool m_bHot;
	bool m_bMove;

public:
	CGUICheckBox(ISceneNode * pParent) : CGUITexturedBox(pParent),m_bHot(false),m_bMove(false) { }
	~CGUICheckBox() { }
	void Update(CInputDevice::SEvent & event);
	void SetMove(bool b)
	{
		if ( !m_bHot )
			m_bMove = b;
	}
	void SetHot(bool b);
};


}