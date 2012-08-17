//////////////////////////////////////////////////////////////////////////
//  	
//	Copyright (C) 2003-2004 
//	All Rights Reserved
//  In memory of bluely
//  
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "BluelyG.h"
#include "Scene.h"
#include "Input.h"

namespace bg
{
namespace sc
{

class CSkyBox : public ISceneNode
{
	enum ESKY_TYPE { EST_FRONT = 0, EST_BACK, EST_LEFT, EST_RIGHT, EST_TOP, EST_COUNT };

	D3DXVECTOR3 m_vPositions[20];
	D3DXVECTOR2 m_vUVs[20];
	DWORD m_dwTextureHandles[EST_COUNT];
	float m_fWidth;
	float m_fHeight;
	float m_fDepth;

public:
	CSkyBox(ISceneNode * pParent);
	~CSkyBox();

	void Create(float fWidth, float fHeight, float fDepth, DWORD dwTextureHandleFront, DWORD dwTextureHandleBack, DWORD dwTextureHandleLeft, DWORD dwTextureHandleRight, DWORD dwTextureHandleTop, DWORD dwTexelCnt);
	void Render();
	void Update(ip::CInputDevice::SEvent & event);
};

}
}