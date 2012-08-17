// ResDll.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "ResDll.h"
#include "resource.h"

#ifdef _MANAGED
#pragma managed(push, off)
#endif

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		hInst = hModule;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
    return TRUE;
}

#ifdef _MANAGED
#pragma managed(pop)
#endif

// This is an example of an exported variable
RESDLL_API int nResDll=0;

// This is an example of an exported function.
RESDLL_API int fnResDll(void)
{
	return 42;
}

DLGPROC DialogProcFunc(HWND hWnd, UINT uInt, WPARAM wParam, LPARAM lParam)
{
	switch(uInt)
	{
	case WM_INITDIALOG:
		return 0;
	case WM_COMMAND:
		if(LOWORD(wParam)==IDOK)
		{
		}
		else if(LOWORD(wParam)==IDCANCEL)
		{
		}
		return 0;
	case WM_CLOSE:
		DestroyWindow(hWnd);
		hWnd = NULL;
		return 0;
	}
	return 0;
}

RESDLL_API HINSTANCE GetResDllHInstance(void)
{
	return hInst;
}

RESDLL_API HWND ShowDlg(DLGPROC Proc)
{
	LPTSTR lptstr = MAKEINTRESOURCE(IDD_DIALOG1);
	HWND hWnd = ::CreateDialog(hInst, lptstr, NULL, Proc);
	if (hWnd)
	{
		ShowWindow(hWnd, SW_SHOW);
		return hWnd;
	}
	return NULL;
}

RESDLL_API HWND GetListChild(HWND parentHWnd)
{
	return GetDlgItem(parentHWnd, IDC_LIST1);
}

// This is the constructor of a class that has been exported.
// see ResDll.h for the class definition
CResDll::CResDll()
{
	return;
}
