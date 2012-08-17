// RoleViewer.cpp : Defines the class behaviors for the application.
//
#include "stdafx.h"
#include "BaseTypes.h"
#include "RoleViewer.h"
#include "RoleViewerDlg.h"
#include "ScriptMgr.h"
#include "Resource.h"
#include "ExpMgr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CRoleViewerApp

BEGIN_MESSAGE_MAP(CRoleViewerApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()

// CRoleViewerApp construction

CRoleViewerApp::CRoleViewerApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CRoleViewerApp object

CRoleViewerApp theApp;


// CRoleViewerApp initialization

BOOL CRoleViewerApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	AfxEnableControlContainer();
	//InitExpHandler();
	CRoleViewerDlg dlg;
	m_pMainWnd = &dlg;
	Init();
	SQR_TRY
	{
		INT_PTR nResponse = dlg.DoModal();
		if (nResponse == IDOK)
		{
			// TODO: Place code here to handle when the dialog is
			//  dismissed with OK
		}
		else if (nResponse == IDCANCEL)
		{
			// TODO: Place code here to handle when the dialog is
			//  dismissed with Cancel
		}
	}
	SQR_CATCH(error)
	{
		MessageBox(NULL,error.ErrorMsg(),"",MB_OK);
	}
	SQR_TRY_END;

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

BOOL CRoleViewerApp::OnIdle(LONG lCount)
{
	DWORD Time = GetTickCount();
	float Alpha = 1.0f - ( Time - m_ShowTime )/1000.0f;
	if( m_ShowTime && Alpha < 0 )
	{
		if( m_AutoMsg.GetSafeHwnd() )
			m_AutoMsg.ShowWindow( SW_HIDE );
		m_ShowTime = 0;
	}
	else if( m_ShowTime )
	{
		if( m_AutoMsg.GetSafeHwnd() )
			m_AutoMsg.SetLayeredWindowAttributes( 0, (BYTE)( Alpha*255.0f ), LWA_ALPHA );
	}

#ifdef _FOR_ART_
	return CWinApp::OnIdle( lCount );
#else
	if( m_pMainWnd )
		((CRoleViewerDlg*)m_pMainWnd)->OnDraw();
	CWinApp::OnIdle( lCount );

	return 1;
#endif
}

inline const CString& GetRootPath()
{
	return theApp.m_RootPath;
}
const CString GetFileName( const TCHAR* Filter )
{
	CString RootPath = GetRootPath();
	CFileDialog Dlg( TRUE, NULL, "³¡¾°ÎÄ¼þ", OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_NOCHANGEDIR, Filter );
	Dlg.m_ofn.lpstrInitialDir = RootPath;
	Dlg.DoModal();

	CString Filename = Dlg.GetPathName();
	Filename.MakeLower();

	return Filename;
}