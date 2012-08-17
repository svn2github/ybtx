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

class CMouse : public ISceneNode
{
	D3DXVECTOR2 m_vPt;
	DWORD m_dwTextureHandleIdle;
	DWORD m_dwTextureHandleClicked;
	bool m_bClicked;

public:
	CMouse(ISceneNode * pParent, DWORD dwTextureHandleIdle, DWORD dwTextureHandleClicked): ISceneNode(pParent),m_vPt(400,300),m_bClicked(false),
		m_dwTextureHandleIdle(dwTextureHandleIdle),m_dwTextureHandleClicked(dwTextureHandleClicked) { }

	~CMouse() { }
	void SetMouseTexture(DWORD dwTextureHandleIdle,DWORD dwTextureHandleClicked);
	void OnEvent(ip::CInputDevice::SEvent & event);
	void Render();
};

}
}