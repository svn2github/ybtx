//////////////////////////////////////////////////////////////////////////
//	
//	Copyright (C) 2003-2004
//  All Rights Reserved
//  In memory of bluely
//  
//////////////////////////////////////////////////////////////////////////

#include "basetypes.h"
#include "Engine.h"
#include "Render.h"
#include "BluelyGUI.h"
#include "Input.h"
#include "../Bluely Common/memfile.h"
#include "../Bluely Common/Console.h"
#include "Scene.h"
#include "CTileEditApp.h"
#include "ICoreGraphic.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace bg 
{
namespace eg
{

CEngine * CEngine::one = NULL;

CEngine::CEngine()
{ 
	m_pCamera = NULL;
}


CEngine::~CEngine() { }

bool CEngine::Create(HWND hWndD3D, HWND hWndInput, RECT rc, bool bMyGUI, bool bMultipleView)
{
	m_hWnd = hWndD3D;
	m_bMyGUI = bMyGUI;
	m_bMultipleView = bMultipleView;
	ip::CInputDevice::GetInstance()->Create(hWndInput,hWndD3D);
	ui::CUIManager::GetInstance()->Create();
	rd::CRenderDevice::GetInstance()->Create(hWndD3D, rc, NULL); // temporarily null
	return true;
}

HWND CEngine::GetD3DWindow()
{
	return m_hWnd;
}

void CEngine::Destroy()
{
	ip::CInputDevice::GetInstance()->Destroy();
	ui::CUIManager::GetInstance()->Destroy();
	rd::CRenderDevice::GetInstance()->Destroy();
	sc::CSceneManager::GetInstance()->Drop();
}

bool CEngine::WndProc(UINT uMessage, WPARAM wParam, LPARAM lParam)
{
	switch (uMessage) 
	{
	case WM_ACTIVATE:
			m_bWndActive = LOWORD(wParam);  
	break;
	
	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:		
	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
	case 0x020a:
	case WM_CHAR:		
		{
			ip::CInputDevice * pInput = ip::CInputDevice::GetInstance();
			if ( pInput )
			{
				pInput->ParseMsg(uMessage,wParam,lParam);
			}
		}
	break;
	
	default:
		return false; // no message handled
	}

	return true;
//	return false;
}

void CEngine::Update()
{
	ip::CInputDevice::SEvent event;
	ip::CInputDevice * pInput = ip::CInputDevice::GetInstance();
	
	// update user input
	pInput->UpdateInput();
		
	// magage user input	
	pInput->PopEvent(&event);

	// pass to gui and update...
	if ( m_bMyGUI ) ui::CUIManager::GetInstance()->Update(event);
	
	// pass to scene and update...
	sc::CSceneManager * pScene = sc::CSceneManager::GetInstance();
	pScene->UpdateWorld(event);
	if ( m_bMultipleView ) pScene->UpdateWin32GUI(event);

	// for extra input msg
	while ( pInput->PopEvent(&event) )
	{
		// pass to gui and update...
		if ( m_bMyGUI ) ui::CUIManager::GetInstance()->Update(event);

		// pass to scene and update...
		sc::CSceneManager * pScene = sc::CSceneManager::GetInstance();
		pScene->UpdateWorld(event);
		if ( m_bMultipleView ) pScene->UpdateWin32GUI(event);
	}
	
	// manage AI
}

void CEngine::Render()
{
	static DWORD LastUpdateTime = ::GetTickCount();
	
	if( ::GetTickCount() - LastUpdateTime > 32)
	{
		rd::CRenderDevice * pRender = rd::CRenderDevice::GetInstance();
		sc::CSceneManager * pScene = sc::CSceneManager::GetInstance();
		
		pRender->Clear(0xff000000);
		
		pRender->BeginScene();
		
		// render 3d
		pScene->DrawWorld();
		
		// prepare my style gui to cache if necessary
		if ( m_bMyGUI )
		{
			ui::CUIManager * pUI = ui::CUIManager::GetInstance();
			pUI->Render();  
		}

		// render 2d 
		pRender->Render();

		//D3DMATRIX fixMatrix1, fixMatrix2, fixMatrix3;
		D3DLIGHT9 tl[8];
		BOOL tlb[8];
		for(int i=0;i<8;++i) {
			pRender->GetRawDevice()->GetLight( i, tl+i);
			pRender->GetRawDevice()->GetLightEnable( i, tlb+i);
		}
		//pRender->GetRawDevice()->GetTransform( D3DTS_VIEW,			&fixMatrix1 );
		//pRender->GetRawDevice()->GetTransform( D3DTS_PROJECTION,	&fixMatrix2 );
		//pRender->GetRawDevice()->GetTransform( D3DTS_WORLD,			&fixMatrix3 );
		
		pRender->GetRawDevice()->SetRenderState( D3DRS_AMBIENT, 0xffffffff);
		
		bg::sc::CTileEditApp::GetInstance()->GetOperator()->GetCoreGraphic()->RenderEndForMapEditor(false);
		pRender->GetRawDevice()->SetRenderState( D3DRS_AMBIENT, 0xffffffff);
		
		//pRender->GetRawDevice()->SetTransform( D3DTS_VIEW,			&fixMatrix1 );
		//pRender->GetRawDevice()->SetTransform( D3DTS_PROJECTION,	&fixMatrix2 );
		//pRender->GetRawDevice()->SetTransform( D3DTS_WORLD,			&fixMatrix3 );
		//
		//for(int i=0;i<8;++i) {
		//	pRender->GetRawDevice()->SetLight( i, tl+i );
		//	pRender->GetRawDevice()->LightEnable( i, tlb[i]);
		//}

		pRender->EndScene();
		
		pRender->Present();
		
		// render win32 gui if necessary using swapchains 
		if ( m_bMultipleView )
		{
			pScene->DrawWin32GUI();
		}
		
		LastUpdateTime = ::GetTickCount();
	}
}

void CEngine::SetWnd(void * pWnd)
{
	ui::CUIManager::GetInstance()->SetMainWnd((ui::CWnd*)pWnd);
}

void CEngine::SetActiveCamera(sc::CCamera * pCamera)
{
	m_pCamera = pCamera; 
}

sc::CCamera * CEngine::GetActiveCamera()
{
	return m_pCamera;
}

}
}