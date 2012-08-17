//////////////////////////////////////////////////////////////////////////
//	
//	Copyright (C) 2003-2004
//  All Rights Reserved
//  In memory of bluely
//  
//////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "extradefine.h"
#include "Scene.h"
#include "TerrainMesh.h"
#include "Camera.h"
#include "Mouse.h"
#include "Square.h"
#include "SkyDome.h"
#include "GUITexturedBox.h"
#include "Render.h"
#include "SkyBox.h"
#include "Operator.h"
#include "WireBox.h"

#if defined(_DEBUG) && defined(_WIN32)
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif

namespace bg
{
namespace sc
{

//////////////////////////////////////////////////////////////////////////
// scene manager

CSceneManager * CSceneManager::one = NULL;

CTerrainMesh * CSceneManager::AddNodeTerrainMesh(DWORD dwWidth, DWORD dwHeight, DWORD dwTextureHandleDefault, ISceneNode * pParent)
{
	if ( !pParent )
		pParent = this;
	
	CTerrainMesh * pNode = new CTerrainMesh(pParent);
	pNode->Create(dwWidth,dwHeight,dwTextureHandleDefault);
	return pNode;
}

CCamera * CSceneManager::AddNodeCamera(float fFOV, float fPitch, float fYaw, float fZNear, float fZfar, D3DXVECTOR3 vecPos, D3DXVECTOR3 vecView, ISceneNode * pParent)
{
	if ( !pParent )
		pParent = this;
	
	CCamera * pNode = new CCamera(pParent);
	
	pNode->SetNearClipPlane(fZNear);
	pNode->SetFarClipPlane(fZfar);
	pNode->SetPosition(vecPos);
	pNode->SetView(vecView);
	pNode->CalculateAxis();
	pNode->SetFOVY(fFOV);
	pNode->SetPitchAndYaw(fPitch,fYaw);
	return pNode;
}

CMouse * CSceneManager::AddNodeMouse(DWORD dwTextureHandleIlde, DWORD dwTextureHandleClick, ISceneNode * pParent)
{
	if ( !pParent )
		pParent = this;
	
	CMouse * pNode = new CMouse(pParent,dwTextureHandleIlde,dwTextureHandleClick);
	return pNode;
}

CSquare * CSceneManager::AddNodeSquare(float fSize, DWORD dwColor, DWORD dwTextureHandle, ISceneNode * pParent)
{
	if ( !pParent )
		pParent = this;
	
	CSquare * pNode = new CSquare(pParent, dwTextureHandle, fSize, dwColor);
	return pNode;
}

CSkyDome * CSceneManager::AddNodeSkyDome(ISceneNode * pParent) 
{
	if ( !pParent )
		pParent = this;

	CSkyDome * pNode = new CSkyDome(pParent);
	return pNode;
}


CSkyBox * CSceneManager::AddNodeSkyBox(float fWidth, float fHeight, float fDepth, DWORD dwTextureHandleFront, DWORD dwTextureHandleBack, DWORD dwTextureHandleLeft, DWORD dwTextureHandleRight, DWORD dwTextureHandleTop, DWORD dwTexelCnt, ISceneNode * pParent)
{
	if ( !pParent )
		pParent = this;

	CSkyBox * pNode = new CSkyBox(pParent);
	pNode->Create(fWidth,fHeight,fDepth,dwTextureHandleFront,dwTextureHandleBack,dwTextureHandleLeft,dwTextureHandleRight,dwTextureHandleTop,dwTexelCnt);
	return pNode;
}

COperator * CSceneManager::AddNodeOperator(HWND hResSetView, ISceneNode * pParent)
{
	if ( !pParent )
		pParent = this;

	COperator * pNode = new COperator(pParent);
	pNode->Create(hResSetView);
	return pNode;
}

CTileEditorModel * CSceneManager::AddNodeModel(std::string strFileName, bool bSubUnit, float fLift, float fShiftX, float fShiftZ, int &BlockInfoNum, int &LogicHeightInfoNum, ISceneNode * pParent)
{
	if ( !pParent )
		pParent = this;
	
	CTileEditorModel * pNode = new CTileEditorModel(pParent);
	pNode->Create(strFileName, bSubUnit, fLift, fShiftX, fShiftZ, BlockInfoNum, LogicHeightInfoNum);
	return pNode;
}

CWireBox * CSceneManager::AddNodeWireBox(float fHeight, float fWidth, float fDepth, DWORD dwColor,  bool bIsVisible, ISceneNode * pParent)
{
	if ( !pParent )
		pParent = this;

	CWireBox * pNode = new CWireBox(pParent,bIsVisible);
	pNode->Create(fHeight,fWidth,fDepth,dwColor);
	return pNode;
}

//////////////////////////////////////////////////////////////////////////
// gui nodes
CGUICheckBox * CSceneManager::AddNodeGUICheckBox(DWORD dwWidth, DWORD dwHeight, DWORD dwTextureHandle, HWND hWnd, DWORD dwColor, ISceneNode * pParent)
{
	if ( !pParent )
		pParent = m_pGUIRoot;
	
	CGUICheckBox * pNode = new CGUICheckBox(pParent);
	pNode->Create(dwWidth, dwHeight, dwTextureHandle, hWnd, dwColor);
	return pNode;
}


//////////////////////////////////////////////////////////////////////////
// gui scene node

// each ui scene node has a swapchain for rendering
void IGUISceneNode::Create(DWORD dwWidth, DWORD dwHeight, HWND hWnd)
{ 
	m_hWnd = hWnd;
	rd::CRenderDevice * pRender = rd::CRenderDevice::GetInstance();
	pRender->AddSwapChain(dwWidth,dwHeight,&m_pSwapChain);
}

void IGUISceneNode::Render()
{
	rd::CRenderDevice * pRender = rd::CRenderDevice::GetInstance();
	
	LPDIRECT3DSURFACE9 pOldBackBuffer;
	
	pRender->GetRawDevice()->GetRenderTarget(0, &pOldBackBuffer);
	
	ISceneNode::Render();
	
	pRender->GetRawDevice()->SetRenderTarget(0, pOldBackBuffer);
}

}
}