#include "stdafx.h"
#include "CMainWnd.h"
#include "CAppClient.h"
#include "IMainWndHandler.h"
#include "CIme.h"
#include "ExpHelper.h"
#include "ErrLogHelper.h"

#define MinWidth	800
namespace sqr
{
const wchar_t* gs_szMainWndClassName=L"MainWnd class of Engine";
static stdext::hash_map<DWORD,CMainWnd*> gs_mapWnd;
}
#include "SQRYYCtrl.h"
LRESULT CALLBACK CMainWnd::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	stdext::hash_map<DWORD,CMainWnd*>::iterator it=gs_mapWnd.find(reinterpret_cast<DWORD>(hWnd));
	CMainWnd* pGacMainWnd;
	if (it==gs_mapWnd.end())
	{
		if ( message != WM_CREATE )
			return ::DefWindowProcW( hWnd, message, wParam, lParam );
		pGacMainWnd=reinterpret_cast<CMainWnd*>(reinterpret_cast<CREATESTRUCT*>(lParam)->lpCreateParams);
		pGacMainWnd->m_hMainWnd=hWnd;
		gs_mapWnd[reinterpret_cast<DWORD>(hWnd)]=pGacMainWnd;
	}
	else
	{
		pGacMainWnd=it->second;
	}
	if( message == WM_MOUSEMOVE && pGacMainWnd->m_bNeedTrackMouseEvent)
	{
		TRACKMOUSEEVENT trmouse;
		trmouse.cbSize = sizeof(TRACKMOUSEEVENT);
		trmouse.dwFlags = TME_LEAVE;
		trmouse.dwHoverTime = 10;
		trmouse.hwndTrack = pGacMainWnd->m_hMainWnd;
		::TrackMouseEvent(&trmouse);
		pGacMainWnd->m_bNeedTrackMouseEvent = false;
	}

	if(CYYManager::HasInst() && CYYManager::GetInst()->GetYYCtrl())
		if( 0 == CYYManager::GetInst()->FilterAndProcYYMsg(message,wParam,lParam) )
			return 0;

	if (!pGacMainWnd)
		GenErr("Invalid pGacMainWnd pointer.");
	return pGacMainWnd->OnWndMessage(message,wParam,lParam);
}


ATOM CMainWnd::RegisterClass(HINSTANCE hInstance,HICON hBigIcon,HICON hSmallIcon)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= hBigIcon;
	wcex.hCursor		= LoadCursorW(NULL, MAKEINTRESOURCEW(32512));
	wcex.hbrBackground	= NULL;
	wcex.lpszMenuName	= (LPCWSTR)NULL;
	wcex.lpszClassName	= gs_szMainWndClassName;
	wcex.hIconSm		= hSmallIcon;

	return RegisterClassExW(&wcex);
}


CMainWnd::CMainWnd(IMainWndHandler* pHandler, HINSTANCE hInstance
                   , int32 nWidth, int32 nHeight, const wchar_t* szTitle
                   , const wchar_t* szBigIconFileName, const wchar_t* szSmallIconFileName)
		: m_pHandler(pHandler)
		, m_bActive( false )
		, m_bNeedTrackMouseEvent( true )
{
	wstring sTitle = szTitle;

	m_hIconBig=szBigIconFileName?static_cast<HICON>
	           (LoadImageW(NULL,szBigIconFileName,IMAGE_ICON,0,0,LR_LOADFROMFILE)):NULL;

	m_hIconSmall=szSmallIconFileName?static_cast<HICON>
	             (LoadImageW(NULL,szSmallIconFileName,IMAGE_ICON,0,0,LR_LOADFROMFILE)):NULL;

	DWORD dwStyle	= WS_OVERLAPPEDWINDOW;

	RECT ClientRect = { 0, 0, nWidth, nHeight };
	m_fWndRatio = float(nHeight) / float(nWidth);
	AdjustWindowRect(&ClientRect,dwStyle,false);
	RECT workArea;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &workArea, 0);
	nWidth = min(ClientRect.right - ClientRect.left, workArea.right - workArea.left);
	nHeight = min(ClientRect.bottom - ClientRect.top, workArea.bottom - workArea.top);
	int32 nPosX = workArea.left + (workArea.right - workArea.left - nWidth) / 2;
	int32 nPosY = workArea.top + (workArea.bottom - workArea.top - nHeight) / 2;

	if (gs_mapWnd.empty())
		RegisterClass(hInstance,m_hIconBig,m_hIconSmall);

	SystemParametersInfoW(SPI_GETDRAGFULLWINDOWS, 0, &m_bFullWindow, 0);

	m_hMainWnd = CreateWindowW( gs_szMainWndClassName, sTitle.c_str(), dwStyle,
		nPosX,nPosY,nWidth,nHeight,NULL, NULL, hInstance, this);
	SetWindowPos(m_hMainWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);

	if (!m_hMainWnd)
		GenErr("Can't create GameClient main window.");

	UpdateWindow( m_hMainWnd );
}


CMainWnd::~CMainWnd()
{
	Ast(m_hMainWnd);
	DestroyWindow(m_hMainWnd);

	gs_mapWnd.erase(reinterpret_cast<DWORD>(m_hMainWnd));
	m_hMainWnd=NULL;

	DestroyIcon(m_hIconBig);
	DestroyIcon(m_hIconSmall);

	m_hIconBig=NULL;
	m_hIconSmall=NULL;
}

void CMainWnd::Show(bool bShow)
{
	::ShowWindow(m_hMainWnd,bShow?SW_SHOW:SW_HIDE);
}

inline LRESULT CMainWnd::OnWndMessage(UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	
	SQR_TRY
	{
		if ( !CIme::GetIme()->CorrectCharMsg( uMsg, wParam ) )
			return 0;
		if (uMsg == WM_MOUSELEAVE && m_pHandler)
		{
			m_bNeedTrackMouseEvent = true;
			return m_pHandler->OnInputMessage(uMsg,wParam,lParam);
		}
		if ( uMsg == WM_ACTIVATE )
		{
			m_bActive = !m_bActive;
			if ( !m_bActive && m_pHandler )
			{
				return m_pHandler->OnInputMessage(uMsg,wParam,lParam);
			}
		}
		if ( (( WM_MOUSEFIRST<=uMsg && uMsg<=WM_MOUSELAST ) || ( WM_KEYFIRST<=uMsg && uMsg<=WM_KEYLAST )) && m_pHandler )
		{
			return m_pHandler->OnInputMessage(uMsg,wParam,lParam);
		}
		else if ( WM_USER<=uMsg && uMsg<=0x7FFF && m_pHandler )
		{
			return m_pHandler->OnUserMessage(uMsg,wParam,lParam);
		}
		else
		{
			switch (uMsg)
			{
			case WM_SETCURSOR:
				if ( LOWORD( lParam ) != HTCLIENT )
					return ::DefWindowProcW( m_hMainWnd, uMsg, wParam, lParam );
			case WM_IME_CHAR:
				return m_pHandler?m_pHandler->OnInputMessage(uMsg,wParam,lParam):0;
			case WM_INPUTLANGCHANGE:
			case WM_IME_NOTIFY:
				return (NULL != CIme::GetIme()) ? CIme::GetIme()->ImeProc(uMsg,wParam,lParam) : 0;
			case WM_SIZE:
				{
					if (GetKeyState(VK_LBUTTON) >= 0)
						SystemParametersInfo(SPI_SETDRAGFULLWINDOWS, m_bFullWindow, NULL, 0);
					switch (wParam)
					{
					case SIZE_MINIMIZED:
						if ( m_pHandler )
							m_pHandler->OnMinimize();
						m_bActive = !m_bActive;
						if ( !m_bActive && m_pHandler )
						{
							return m_pHandler->OnInputMessage(WM_ACTIVATE,wParam,lParam);
						}
						break;
					case SIZE_MAXIMIZED:
					case SIZE_RESTORED:
						//普通的改变窗口大小也会调用到这里
						if ( m_pHandler )
							m_pHandler->OnRestore();
						break;
					}
				}
				break;
			case WM_SIZING:
				{
					SystemParametersInfo(SPI_SETDRAGFULLWINDOWS, FALSE, NULL, 0);
					// 保持窗口比例
					RECT rcClient, rcWnd;
					GetClientRect(m_hMainWnd, &rcClient);
					GetWindowRect(m_hMainWnd, &rcWnd);
					int deltaWidth = rcWnd.right - rcWnd.left - rcClient.right;
					int deltaHeight = rcWnd.bottom - rcWnd.top - rcClient.bottom;
					RECT& rect = *((RECT*)(lParam));
					int width = rect.right - rect.left - deltaWidth;
					int height = rect.bottom - rect.top - deltaHeight;
					switch (wParam)
					{
					case WMSZ_LEFT:
					case WMSZ_TOPLEFT:
						if (width < MinWidth)
						{
							rect.left = rect.right - MinWidth - deltaWidth;
							width = MinWidth;
						}
						rect.top = rect.bottom - deltaHeight - int(width * m_fWndRatio + 0.5f);
						
						break;
					
					case WMSZ_TOP:
					case WMSZ_TOPRIGHT:
						rect.right = rect.left + deltaWidth + int(height / m_fWndRatio + 0.5f);
						if (rect.right - rect.left - deltaWidth < MinWidth)
						{
							rect.right = rect.left + MinWidth + deltaWidth;
							rect.top = rect.bottom - deltaHeight - int(MinWidth * m_fWndRatio + 0.5f);
						}
						break;
					case WMSZ_RIGHT:
					case WMSZ_BOTTOMRIGHT:
						if (width < MinWidth)
						{
							rect.right = rect.left + MinWidth + deltaWidth;
							width = MinWidth;
						}
						rect.bottom = rect.top + deltaHeight + int(width * m_fWndRatio + 0.5f);
						break;
					case WMSZ_BOTTOM:
					case WMSZ_BOTTOMLEFT:
						rect.left = rect.right - deltaWidth - int(height / m_fWndRatio + 0.5f);
						if (rect.right - rect.left - deltaWidth < MinWidth)
						{
							rect.left = rect.right - MinWidth - deltaWidth;
							rect.bottom = rect.top + deltaHeight + int(MinWidth * m_fWndRatio + 0.5f);
						}
						break;
					}
				}
				break;
			case WM_ACTIVATEAPP:
				{
					if ( m_pHandler )
						m_pHandler->OnActive( wParam!=0 );
				}
				return 0;
			case WM_CLOSE:
				{
					if (m_pHandler)
						m_pHandler->OnClose();
					m_pHandler = NULL;
				}
				return 0;
			}
		}
	}
	SQR_CATCH(exp)
	{
		LogExp(exp);
	}
	SQR_TRY_END;
	
	return DefWindowProcW(m_hMainWnd,uMsg,wParam,lParam);
}


HWND CMainWnd::GetHwnd()const
{
	return m_hMainWnd;
}


bool CMainWnd::IsActive()const
{
	return m_hMainWnd == GetForegroundWindow();
}
