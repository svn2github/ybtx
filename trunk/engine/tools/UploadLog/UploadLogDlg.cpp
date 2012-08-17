// UploadLogDlg.cpp : implementation file
//

#include "stdafx.h"
#include "UploadLog.h"
#include "UploadLogDlg.h"
#include "CHttpUploader.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CUploadLogDlg dialog
#define  BUFFER_SIZE  4096


typedef void (*cbFunc) (void*);

typedef struct {
	const char* szLogHost;
	const char* szFile;
	const char* szCurrDir;
	const char* szSearchDir;
} sParameter;

// 遍历文件夹
static void RecursionDir(cbFunc func, void* args)
{
	HANDLE hFind;
	WIN32_FIND_DATA ffd;
	sParameter* pPara = static_cast<sParameter*>(args);

	hFind = FindFirstFile( pPara->szSearchDir, &ffd );
	if ( hFind == INVALID_HANDLE_VALUE )
	{
		printf("Invalid File Handle %s. GetLastError reports %d\n", pPara->szCurrDir, GetLastError());
		return;
	}

	do {
		TCHAR szLogFile[BUFFER_SIZE];
		strcpy(szLogFile, pPara->szCurrDir);
		strcat(szLogFile, "/");
		strcat(szLogFile, ffd.cFileName);

		pPara->szFile = szLogFile;
		func(pPara);
		if ( DeleteFile(pPara->szFile)  )
			printf("Can't delete: %s %d\n", pPara->szFile, GetLastError());
	} while( FindNextFile( hFind, &ffd ) );

	FindClose( hFind );
}

static void UploadLog(void* args)
{
	sParameter* pPara = static_cast<sParameter*>(args);
	CHttpUploader::UploadFile(pPara->szLogHost, pPara->szFile);
	//if ( ! CHttpUploader::UploadFile(pPara->szLogHost, pPara->szFile) )
	//	MessageBox(NULL, "upload error!!", 0, 0);
}

// 还没考虑转义及unicode
static void AnalysisArgs(const TCHAR* Args, vector<string>& vec)
{
	int nQuote = 0;
	const TCHAR* ptr = Args;
	string ArgStr;
	while ( *ptr != '\0' )
	{
		if ( *ptr == '"' )
		{
			++nQuote;
			if ( nQuote >= 2 )
			{
				++ptr; // 跳过之后的空白
				vec.push_back(ArgStr);
				ArgStr = "";
				nQuote = 0;
			}

			++ptr; // 跳过"
			continue;
		}
		else if ( *ptr == ' ' && nQuote == 0 )
		{
			++ptr; // 跳过之后的空白
			vec.push_back(ArgStr);
			ArgStr = "";
			continue;
		}

		ArgStr.append(1, *ptr);
		++ptr;
	}

	vec.push_back(ArgStr);
}


CUploadLogDlg::CUploadLogDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CUploadLogDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CUploadLogDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CUploadLogDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


// CUploadLogDlg message handlers

BOOL CUploadLogDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	SetWindowText( "ybtx_log_uploader" );

	vector<string> vec;
	AnalysisArgs(GetCommandLine(), vec);

	TCHAR szSearchDir[BUFFER_SIZE];
	strcpy(szSearchDir, vec.at(1).c_str());
	TCHAR* ptr = szSearchDir + strlen(szSearchDir);
	strcat(szSearchDir, "/*.log");

	sParameter para  = {0};
	string strCurrDir =vec.at(1);
	para.szLogHost   = vec.at(2).c_str();
	para.szCurrDir   = strCurrDir.c_str();
	para.szSearchDir = szSearchDir;
	RecursionDir(UploadLog, &para);

	strcpy(ptr, "/*.dmp");
	RecursionDir(UploadLog, &para);

	strCurrDir = vec.at(1)+"/gas";
	para.szCurrDir   = strCurrDir.c_str();	
	strcpy(ptr, "/gas/gas_err*.info");
	RecursionDir(UploadLog, &para);

	strcpy(ptr, "/gas/gas_err*.comm");
	RecursionDir(UploadLog, &para);

	strCurrDir = vec.at(1)+"/gac";
	para.szCurrDir   = strCurrDir.c_str();	
	strcpy(ptr, "/gac/gac_err*.info");
	RecursionDir(UploadLog, &para);

	strcpy(ptr, "/gac/gac_err*.comm");
	RecursionDir(UploadLog, &para);

	strcpy(ptr, "/gac/Conn*.log");
	RecursionDir(UploadLog, &para);

	strcpy(ptr, "/gac/*.dmp");
	RecursionDir(UploadLog, &para);

	exit(0);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CUploadLogDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CUploadLogDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

