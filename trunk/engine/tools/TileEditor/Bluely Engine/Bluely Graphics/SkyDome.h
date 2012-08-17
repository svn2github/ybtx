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

class CSkyDome : public ISceneNode
{
	DWORD m_dwPlaneVertexCnt;
	D3DXVECTOR3 * m_pPosBuffer;
	D3DXVECTOR2 * m_pUVBuffer;
	D3DCOLOR    * m_pColBuffer;	
	DWORD * m_pdwIndices;
	DWORD m_dwIndexCnt;
	float m_fRadius; 
	float m_fOffset;
	
public:
	CSkyDome(ISceneNode * pParent);
	~CSkyDome();

	void Create(DWORD dwTextureHandle,int iDivisions, float fPlanetRadius, float fAtmosphereRadius, float fHTile, float fVTile, float fOffset);
	void Render();
	void Update(ip::CInputDevice::SEvent & event);
};

}
}