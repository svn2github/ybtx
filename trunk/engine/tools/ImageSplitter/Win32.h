#pragma once
#include <Windows.h>

#pragma comment(lib, "winmm.lib")

// ---- ½Ó¿ÚÉùÃ÷ ------------------------------------------------------------------------------

	HINSTANCE	GetWindowInstanceWin32(void);
	bool		CreateMainWindow(int x, int y, int width, int height, 
								 const char *title = "Main window");
	void		CloseMainWindow(void);
	HWND		GetWindowHandleWin32(void);
	void		SetWindowHandleWin32(HWND hWnd);
	void		GetWindowSize(int &w, int &h);
	bool		ProcessMessage(void);