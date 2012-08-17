//////////////////////////////////////////////////////////////////////////
//	
//	Copyright (C) 2003-2004
//  All Rights Reserved
//  In memory of bluely
//  
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "BluelyG.h"
#include "Render.h"
#include "../TileEditorEngine/Console.h"
#include "Camera.h"

namespace bg
{

class CCamera;

namespace eg
{

class CEngine
{
public:
	static CEngine * GetInstance() { if ( !one ) one = new CEngine; return one;} 

private:
	CEngine();
	virtual ~CEngine();
	static CEngine * one;

private:
	bool m_bWndActive;
	bool m_bMyGUI;
	HWND m_hWnd;
	bool m_bMultipleView;
	CCamera * m_pCamera;

public:

public:
	bool Create(HWND hWndD3D,HWND hWndInput, RECT rc, bool bMyGUI,bool bMultipleView);										
	void Destroy();
	bool WndProc(UINT uMessage, WPARAM wParam, LPARAM lParam);
	void Update(void);
	void Render(void);
	void SetWnd(void * pWnd);
	void SetActiveCamera(CCamera * pCamera);
	CCamera * GetActiveCamera();
	HWND GetD3DWindow();
};

};
};