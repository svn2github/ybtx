//////////////////////////////////////////////////////////////////////////
//	
//	Copyright (C) 2003-2004
//  All Rights Reserved
//  In memory of bluely
//  
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "BluelyG.h"
#include "../Bluely Common/Console.h"
#include "Input.h"

#define GRID_SPACE 64.0f
#define REGION_SPACE 128
#define FIX_GRID (REGION_SPACE*REGION_SPACE/2+REGION_SPACE/2)

namespace bg
{
namespace sc
{

class CTerrainMesh;
class CCamera;
class CMouse;
class CSquare;
class CSkyDome;
class CGUICheckBox;
class CSkyBox;
class COperator;
class CTileEditorModel;
class CWireBox;

//////////////////////////////////////////////////////////////////////////
// scene node interface

class ISceneNode
{
protected:
	DWORD m_nCount; // life count
	std::list< ISceneNode* > m_children;
	bool m_bIsVisible; 
	
	ISceneNode * m_pParent;
	D3DXMATRIX m_matRelative;
	D3DXMATRIX m_matAbsolute;
	
	D3DXVECTOR3 m_vecRelativeTranslation;
	D3DXVECTOR3 m_vecRelativeRotation;
	D3DXVECTOR3 m_vecRelativeScale;
	
	DWORD m_dwShaderHandle;

public:
	//////////////////////////////////////////////////////////////////////////
	// resource life counting
	
	void Grab() { ++m_nCount; }
	bool Drop()
	{ 
		--m_nCount; 
		if ( m_nCount == 0 ) 
		{ 
			delete this;
			return true;
		}
		return false;
	}
	
	//////////////////////////////////////////////////////////////////////////
	
	//////////////////////////////////////////////////////////////////////////
	// node management
	
	ISceneNode(ISceneNode * pParent,bool bIsVisible = true,
		D3DXVECTOR3 & vecTranslation = D3DXVECTOR3(0.0f,0.0f,0.0f), 
		D3DXVECTOR3 & vecRotation = D3DXVECTOR3(0.0f,0.0f,0.0f),
		D3DXVECTOR3 & vecScale = D3DXVECTOR3(1.0f,1.0f,1.0f))
		: m_nCount(0),m_bIsVisible(bIsVisible), m_pParent(pParent), m_vecRelativeTranslation(vecTranslation),m_vecRelativeRotation(vecRotation),m_vecRelativeScale(vecScale),m_dwShaderHandle(INVALID) 
	{
		if ( m_pParent )
			m_pParent->AddChild(this);
		else
			m_nCount++; // only root flows here 

		UpdateAbsolute();			
	}

	virtual ~ISceneNode() // destructor will remove all children
	{
		Clear();
	}

	void SetVisible(bool b) 
	{
		m_bIsVisible = b;
	}

	bool GetVisible()
	{
		return m_bIsVisible;
	}

	void Clear()
	{
		LogConsole("info : ISceneNode::Clear : delete all nodes...");
		while( !m_children.empty() )
		{
			ISceneNode * p = m_children.front();
			p->Drop();
			m_children.pop_front();
		}
		m_children.clear();
	}

	void AddChild(ISceneNode * p)
	{
		p->Grab();
		assert(p);
		m_children.push_back(p);
	}

    void RemoveChild(ISceneNode * p)
	{
		p->Drop();
		m_children.remove(p); 
	}
		
    void Remove()
	{
		if ( m_pParent )
			m_pParent->RemoveChild(this);
	}	

	// updates the absolute transformation based on the relative and the parent 
	void UpdateAbsolute()
	{
		if ( m_pParent )
			m_matAbsolute = m_pParent->GetAbsolute() * GetRelative();
		else
			m_matAbsolute = GetRelative();
		
		std::list<ISceneNode*>::iterator it = m_children.begin();
		for (; it != m_children.end(); ++it)
			(*it)->UpdateAbsolute();
	}

	const D3DXMATRIX & GetRelative() 
	{
		D3DXMATRIX matTrans,matRot,matScale;
		D3DXMatrixTranslation(&matTrans,m_vecRelativeTranslation.x,m_vecRelativeTranslation.y,m_vecRelativeTranslation.z);
		D3DXMatrixRotationYawPitchRoll(&matRot,m_vecRelativeRotation.y,m_vecRelativeRotation.x,m_vecRelativeRotation.z);
		D3DXMatrixScaling(&matScale,m_vecRelativeScale.x,m_vecRelativeScale.y,m_vecRelativeScale.z);
		return m_matRelative = matRot*matTrans*matScale;
	}
	
	const D3DXMATRIX & GetAbsolute()
	{ 
		return m_matAbsolute;
	}

	void SetRelativePos(D3DXVECTOR3 v) 
	{
		m_vecRelativeTranslation = v;
	}

	void SetRelativeRotation(D3DXVECTOR3 v)
	{
		m_vecRelativeRotation = v;
	}

	virtual void Render()
	{
		std::list<ISceneNode*>::iterator it = m_children.begin();
		for (; it != m_children.end(); ++it)
		{
			if ( (*it)->GetVisible() )
				(*it)->Render();
		}
	}

	virtual void Update(ip::CInputDevice::SEvent & event)
	{
		OnEvent(event);
		
		if ( m_bIsVisible )
		{
			std::list<ISceneNode*>::iterator it = m_children.begin();
			for (; it != m_children.end(); ++it)
				(*it)->Update(event);
		}
	}

private:
	virtual void OnEvent(ip::CInputDevice::SEvent & event) { }
};

//////////////////////////////////////////////////////////////////////////
// GUI node

// each ui scene node has a swapchain for rendering
class IGUISceneNode : public ISceneNode
{	
protected:
	LPDIRECT3DSWAPCHAIN9 m_pSwapChain;
	HWND m_hWnd;

public:
	IGUISceneNode(ISceneNode * pParent = NULL) : ISceneNode(pParent),m_pSwapChain(NULL) { }
	~IGUISceneNode() { SAFE_RELEASE(m_pSwapChain); }
	void Create(DWORD dwWidth, DWORD dwHeight, HWND hWnd);
	void Render();
};	

//////////////////////////////////////////////////////////////////////////
// scene manager

class CSceneManager : public ISceneNode
{
public:
	static CSceneManager * GetInstance() { if ( !one ) one = new CSceneManager; return one;} 

private:
	CSceneManager(ISceneNode * pParent = NULL) : ISceneNode(pParent) 
	{ 
		m_pGUIRoot = new IGUISceneNode; 
	}
	
	~CSceneManager() { m_pGUIRoot->Drop(); }
	static CSceneManager * one;
	
private:
	IGUISceneNode * m_pGUIRoot;
	
public:
	void DrawWorld() { this->Render(); }
	void DrawWin32GUI() {	m_pGUIRoot->Render(); }
	
	void UpdateWorld(ip::CInputDevice::SEvent & event) { this->Update(event); }
	void UpdateWin32GUI(ip::CInputDevice::SEvent & event) { m_pGUIRoot->Update(event); }
	
	//////////////////////////////////////////////////////////////////////////
	// world scene node
	
	CTerrainMesh * AddNodeTerrainMesh(DWORD dwWidth, DWORD dwHeight, DWORD dwTextureHandleDefault, ISceneNode * pParent = NULL);
	CCamera * AddNodeCamera(float fFOV, float fPitch, float fYaw, float fZNear, float fZfar, D3DXVECTOR3 vecPos, D3DXVECTOR3 vecView, ISceneNode * pParent = NULL);
	CMouse * AddNodeMouse(DWORD dwTextureHandleIlde, DWORD dwTextureHandleClick, ISceneNode * pParent = NULL);
	CSquare * AddNodeSquare(float fSize = 1.0f, DWORD dwColor = cl::GRIDHOT, DWORD dwTextureHandle = INVALID, ISceneNode * pParent = NULL);
	CSkyDome * AddNodeSkyDome(ISceneNode * pParent = NULL);
	CSkyBox * AddNodeSkyBox(float fWidth, float fHeight, float fDepth, DWORD dwTextureHandleFront, DWORD dwTextureHandleBack, DWORD dwTextureHandleLeft, DWORD dwTextureHandleRight, DWORD dwTextureHandleTop, DWORD dwTexelCnt, ISceneNode * pParent = NULL);
	COperator * AddNodeOperator(HWND hResSetView, ISceneNode * pParent = NULL);
	CTileEditorModel * AddNodeModel(std::string strFileName, bool bSubUnit, float fLift, float fShiftX, float fShiftZ, ISceneNode * pParent = NULL);
	CWireBox * AddNodeWireBox(float fHeight, float fWidth, float fDepth, DWORD dwColor, bool bIsVisible = true, ISceneNode * pParent = NULL);
	
	//////////////////////////////////////////////////////////////////////////
	// GUI scene node ( only for window GUI mode... if you use my windows style simply ignore this...
	
	CGUICheckBox * AddNodeGUICheckBox(DWORD dwWidth, DWORD dwHeight, DWORD dwTextureHandle, HWND hWnd, DWORD dwColor = 0xff000000, ISceneNode * pParent = NULL);
};


}
}