#include "stdafx.h"
#include "Win32.h"

// ---- 全局变量 ------------------------------------------------------------------------------

	static HWND				g_hWnd			= NULL;
	static HINSTANCE		g_hInstance		= NULL;
	static HANDLE			g_ThreadHandle	= NULL;
	static unsigned long	g_ThreadID		= 0;

	static int				g_nWindowPosX	= 100;
	static int				g_nWindowPosY	= 100;
	static int				g_nWindowWidth	= 512;
	static int				g_nWindowHeight = 512;

// ---- 局部变量 ------------------------------------------------------------------------------

	bool					bShowWindow		= false;

// ---- 函数实现 ------------------------------------------------------------------------------

	static LRESULT WINAPI WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)
		{
		case WM_DESTROY:
			{
				PostQuitMessage(0);
				break;
			}

		default:
			{
				return DefWindowProc(hwnd, message, wParam, lParam);
				break;
			}
		}

		return 0;
	}

	HINSTANCE GetWindowInstanceWin32( void )
	{
		return g_hInstance;
	}

	bool CreateMainWindow( int x, int y, int width, int height, const char *title /*= "Gut Application"*/ )
	{
		static bool registered = false;

		WNDCLASS	window_class;

		memset(&window_class, 0, sizeof(WNDCLASS));

		window_class.style			= CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
		window_class.lpfnWndProc	= WndProc;
		window_class.hInstance		= GetModuleHandle(NULL);
		window_class.hCursor		= LoadCursor(NULL, IDC_ARROW);
		window_class.hbrBackground	= (HBRUSH)GetStockObject(BLACK_BRUSH);
		window_class.lpszClassName	= title;
		if ( !registered )
		{
			if (RegisterClass(&window_class) == 0)
				return false;
			registered = true;
		}

		DWORD window_style = WS_POPUP;

		if ( x<0 || y<0 )
		{
			// 使用默认值
			x = g_nWindowPosX;
			y = g_nWindowPosY;
		}

		if ( width<0 || height < 0 )
		{
			// 使用默认值
			width	= g_nWindowWidth;
			height	= g_nWindowHeight;
		}

		// 计算窗口的边线会占掉的点数, 必要时要增加窗口大小
		RECT window_rect;
		SetRect(&window_rect, x, y, x+width, y+height);
		AdjustWindowRect(&window_rect, window_style, FALSE);

		g_nWindowPosX = x;
		g_nWindowPosY = y;
		g_nWindowWidth = width;
		g_nWindowHeight = height;

		// 准备工作完成, 打开一个窗口
		HWND window_handle = CreateWindowEx(
			WS_EX_APPWINDOW,
			title,
			title,
			window_style,
			window_rect.left,	// x
			window_rect.top,	// y
			window_rect.right - window_rect.left,	// width
			window_rect.bottom - window_rect.top,	// height
			NULL,
			NULL,
			window_class.hInstance,
			NULL
			);

		if (window_handle == NULL)
		{
			return false;
		}

		g_hWnd		= window_handle;
		g_hInstance = window_class.hInstance;
		
		if ( bShowWindow )
		{
			ShowWindow(window_handle, SW_SHOWNORMAL);
		}
		else
		{
			ShowWindow(window_handle, SW_HIDE);
		}

		return true;
	}

	void CloseMainWindow( void )
	{
		PostQuitMessage(0);
	}

	HWND GetWindowHandleWin32( void )
	{
		return g_hWnd;
	}

	void SetWindowHandleWin32( HWND hWnd )
	{
		g_hWnd = hWnd;
	}

	void GetWindowSize( int &w, int &h )
	{
		w = g_nWindowWidth;
		h = g_nWindowHeight;
	}

	bool ProcessMessage(void)
	{
		MSG	msg;
		if ( PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) )
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			if ( msg.message==WM_QUIT )
			{
				return false;
			}
		}

		return true;
	}