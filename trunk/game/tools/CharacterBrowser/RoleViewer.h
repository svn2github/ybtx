// RoleViewer.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "Resource.h"		// main symbols
#include "MiniCtrls/MiniCtrls.h"


// CRoleViewerApp:
// See RoleViewer.cpp for the implementation of this class
//

class CRoleViewerApp : public CWinApp
{
public:
	CRoleViewerApp();

	DWORD		m_ShowTime;
	CMsgWnd		m_AutoMsg;
	CString		m_RootPath;

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnIdle(LONG lCount);
};




extern CRoleViewerApp theApp;
extern const CString& GetRootPath();
extern const CString GetFileName(  const TCHAR* Filter);
