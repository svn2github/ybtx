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

class CWireBox : public ISceneNode
{
	float m_fHeight;
	float m_fWidth;
	float m_fDepth;
	DWORD m_dwColor;
	
	// in line strip
	D3DXVECTOR3 m_vPos[5 * 6];
	DWORD m_dwClr[5 * 6];
	
public:
	CWireBox(ISceneNode * pParent, bool bIsVisible);
	~CWireBox() { }
	void Create(float fHeight, float fWidth, float fDepth, DWORD dwColor);
	void Render();
	void SetColor(DWORD);
	void IncreaseHeight();
	void DecreaseHeight();
	float GetHeight(); 
	void SetHeight(float fHeight);
};

}
}