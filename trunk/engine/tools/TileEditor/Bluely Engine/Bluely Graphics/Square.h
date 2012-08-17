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

class CSquare : public ISceneNode
{
	DWORD m_dwTextureHandle;
	DWORD m_fSize;
	DWORD m_dwColor;

	// in strip
	D3DXVECTOR3 m_vPos[4];
	D3DXVECTOR2 m_vUV[4];
	DWORD m_dwClr[4];

public:
	CSquare(ISceneNode * pParent, DWORD dwTextureHandle, float fSize, DWORD dwColor);
	~CSquare() { }
	void OnEvent(ip::CInputDevice::SEvent & event);
	void Render();
};

}
}