// Ybtx.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "Ybtx.h"
#include "YbtxDlg.h"
#include <Psapi.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CYbtxApp

BEGIN_MESSAGE_MAP(CYbtxApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CYbtxApp construction


typedef struct _PROCESS_DEBUG_PORT_INFO 
{ 
	HANDLE DebugPort; 
}PROCESS_DEBUG_PORT_INFO;

typedef struct _PROCESS_BASIC_INFO
{
	DWORD ExitStatus;
	DWORD PebBaseAddress;
	DWORD AffinityMask;
	DWORD BasePriority;
	ULONG UniqueProcessId;
	ULONG InheritedFromUniqueProcessId;
} PROCESS_BASIC_INFO;

typedef long              NTSTATUS; 

enum PROCESS_INFO_CLASS
{
	ProcessBasicInfo = 0,
	ProcessDebugPort = 7
};

typedef NTSTATUS  (NTAPI *ZW_QUERY_INFORMATION_PROCESS)(IN HANDLE ProcessHandle, IN PROCESS_INFO_CLASS ProcessInformationClass, OUT PVOID ProcessInformation, IN ULONG ProcessInformationLength, OUT PULONG ReturnLength);
#define STATUS_SUCCESS    ((NTSTATUS)0L) 


CYbtxApp::CYbtxApp()
{
	HMODULE hModule = GetModuleHandle(L"ntdll.dll"); 
	ZW_QUERY_INFORMATION_PROCESS ZwQueryInformationProcess; 
	ZwQueryInformationProcess = (ZW_QUERY_INFORMATION_PROCESS)GetProcAddress(hModule, "ZwQueryInformationProcess"); 
	if (ZwQueryInformationProcess == NULL) 
		return;

	PROCESS_BASIC_INFO BasicInfo;
	if (STATUS_SUCCESS != ZwQueryInformationProcess(GetCurrentProcess(),
		ProcessBasicInfo, &BasicInfo, sizeof(BasicInfo), NULL))
		return;

	DWORD dwParentID = BasicInfo.InheritedFromUniqueProcessId;

	HANDLE hParent = OpenProcess(PROCESS_ALL_ACCESS, TRUE, dwParentID);
	if( !hParent )
		return;

	wchar_t szProcessName[MAX_PATH];

	DWORD length = GetModuleBaseName(hParent, NULL, szProcessName, MAX_PATH);
	if ( 0 == length )
	{
		CloseHandle(hParent);
		return;
	}

	if ( ( 0==lstrcmpi(szProcessName, L"Ybtx.exe") ) || ( 0==lstrcmpi(szProcessName, L"YbtxTemp.exe") ) )
	{
		TerminateProcess(hParent, 0);
	}
	
	CloseHandle(hParent);
}


// The one and only CYbtxApp object

CYbtxApp theApp;


// CYbtxApp initialization

BOOL CYbtxApp::InitInstance()
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

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	CYbtxDlg dlg;
	m_pMainWnd = &dlg;
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

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
