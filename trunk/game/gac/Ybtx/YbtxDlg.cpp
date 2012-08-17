// YbtxDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Ybtx.h"
#include "YbtxDlg.h"
#include "ImageButton.h"
#include "ImageProgress.h"
#include "StarterUpdateHandler.h"
#include "GameSet.h"
#include "CNewHttpDownload.h"
#include "CDownloadHandler.h"
#include "CMd5Algorithm.h"
#include "VersionManager.h"
#include "PatchCommonDef.h"
#include "CPatcher.h"
#include "ShareStack.h"
#include "tinyxml/tinyxml.h"
#include <MSHTML.h>
#include <comutil.h>
#include <direct.h>
#include <process.h>
#include <tlhelp32.h>
#include "ExpMgr.h"
#include "SysAlgorithmCrc.h"
#include "CodeCvs.h"
#include "Process.h"

#include "BaseTypes.h"
#include "CMd5Algorithm.h"
#include "CPathMgr.h"
#include <string>
#include "CVerifyClient.h"
#include <Psapi.h>

using namespace sqr;


#define PATH_SIZE  4096
#define ID_SIZE    256
#define URL_INI_FILE       L"../../etc/gac/Url.ini"
#define UPDATE_INI         L"../../etc/gac/Update.ini"
#define INT_URL_INI_FILE   L"../../etc/gac/IntUrl.ini"
#define CRC32_FILES_CHECK  L"../../etc/gac/CrcCode.txt"

#define  UP_SERVER    L"UploadServer"
#define  HOST_FILE    L"uploader.php"

#define DEFAULT_SERVER_LIST_URL		L"http://10.10.43.138/release/web/ServerList.htm"
#define SERVER_LIST_SECTION			L"ServerListURL"

#define DEFAULT_NEWS_URL			L"http://10.10.43.138/release/web/ServerNews.htm"
#define NEWS_SECTION				L"NewsURL"

#define DEFAULT_PATCH_LIST_URL		L"http://10.10.43.138/release/patch"
#define	PATCH_LIST_SECTION			L"PatchListURL"

#define PATCH_LIST_NAME   L"YbtxPatch.lst"
#define PATCH_LIST_SAVE   L"download"
#define ERR_LOG           L"ybtx_err.log"
#define INPATCH_FLAG  L"../../var/gac/InPatchFlag.txt"

#define LANG_PATH					"../../lang"
#define LANG_XML_NAME				"LangConfig.xml"
#define STATIC_TEXT_FILE_NAME		"StaticText.txt"

#define BMP_QUIT1					"quit_1.bmp"
#define BMP_QUIT2					"quit_2.bmp"
#define BMP_QUIT3					"quit_3.bmp"
#define BMP_MIN1					"min_1.bmp"
#define BMP_MIN2					"min_2.bmp"
#define BMP_MIN3					"min_3.bmp"
#define BMP_SET1					"set_1.bmp"
#define BMP_SET2					"set_2.bmp"
#define BMP_SET3					"set_3.bmp"
#define BMP_START1					"start_1.bmp"
#define BMP_START2					"start_2.bmp"
#define BMP_START3					"start_3.bmp"
#define BMP_START4					"start_4.bmp"
#define BMP_START_MASK				"start_mask.bmp"
#define BMP_REGIST1					"regist_1.bmp"
#define BMP_REGIST2					"regist_2.bmp"
#define BMP_REGIST3					"regist_3.bmp"
#define BMP_CHARGE1					"charge_1.bmp"
#define BMP_CHARGE2					"charge_2.bmp"
#define BMP_CHARGE3					"charge_3.bmp"
#define BMP_BBS1					"bbs_1.bmp"
#define BMP_BBS2					"bbs_2.bmp"
#define BMP_BBS3					"bbs_3.bmp"
#define BMP_SERVER1					"server_1.bmp"
#define BMP_SERVER2					"server_2.bmp"
#define BMP_SERVER3					"server_3.bmp"
#define BMP_MANUALUPDATE1			"ManualUpdate_1.bmp"
#define BMP_MANUALUPDATE2			"ManualUpdate_2.bmp"
#define BMP_MANUALUPDATE3			"ManualUpdate_3.bmp"
#define BMP_SELECT1					"select_1.bmp"
#define BMP_SELECT2					"select_2.bmp"
#define BMP_SELECT3					"select_3.bmp"
#define BMP_PROGRESS1				"progress_1.bmp"
#define BMP_PROGRESS2				"progress_2.bmp"
#define BMP_PROGRESS3				"progress_3.bmp"
#define BMP_MAIN					"main.bmp"
#define BMP_OPENSELECT1				"openselect_1.bmp"
#define BMP_OPENSELECT2				"openselect_2.bmp"
#define BMP_OPENSELECT3				"openselect_3.bmp"
#define BMP_LOOKCHAR1				"lookchar_1.bmp"
#define BMP_LOOKCHAR2				"lookchar_2.bmp"
#define BMP_LOOKCHAR3				"lookchar_3.bmp"
#define	BMP_DUOWAN1					"duowan_1.bmp"
#define	BMP_DUOWAN2					"duowan_2.bmp"
#define	BMP_DUOWAN3					"duowan_3.bmp"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CYbtxDlg dialog
CYbtxDlg::CYbtxDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CYbtxDlg::IDD, pParent)
	, m_InUpdate(false)
	, m_InPatch(false)
	, m_InYYUpdate(false)
	, m_InYbtxUpdate(true)
	, m_eDownMode(eDM_NONE)
	, m_bNeedReload(true)
	, m_bChangeTarget(false)
	, m_strServerInfo(L"")
{
	InitExpHandler();
	InitLangPath();
	wchar_t szProcessName[MAX_PATH];	
	DWORD length = GetModuleBaseName(GetCurrentProcess(), NULL, szProcessName, MAX_PATH);	
	if ( 0 != length  )
	{
		if (0==lstrcmpi(szProcessName, L"YbtxTemp.exe"))
		{
			wstring strCommandLine(GetCommandLine());
			if ( strCommandLine.find(L"ProcessID") == wstring::npos) //判断是否为启动器更新器
			{
				::TerminateProcess(::GetCurrentProcess(), 0);
			}			
		}
	}
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_pUpdater    = new CStarterUpdateHandler();
	m_pDownHandle = new CDownloadHandler();
	m_pStaticText = new CStaticText(m_strStaticTxtPath.c_str(), m_strOriStaticTxtPath.c_str());
	m_pDownloader = new CNewHTTPDownloader(m_pDownHandle);
	m_pVersion    = new VersionManager(m_pUpdater);
	m_PatchArgs   = new PatchArgs();
	m_pVerifyArgs = new VerifyArgs();
	m_pVerifyArgs->handler = m_pUpdater;
	m_PatchArgs->handler = m_pUpdater;

	m_bNeedMd5Verify = false;
	wstring strCommandLine(GetCommandLine());
	if (strCommandLine.find(L"md5verify") != strCommandLine.npos)
	{
		m_bNeedMd5Verify = true;
	}

}

CYbtxDlg::~CYbtxDlg()
{
	delete m_pUpdater;
	delete m_pDownHandle;
	delete m_pDownloader;
	delete m_pVersion;
	delete m_PatchArgs;

	UnitExpHandler();
}

void CYbtxDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EXPLORER2, m_WebNews);
}

BEGIN_MESSAGE_MAP(CYbtxDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()

	ON_BN_CLICKED(IDC_BUTTON_SET,    &CYbtxDlg::OnBnClickedButtonSet)
	ON_BN_CLICKED(IDC_BUTTON_QUIT,   &CYbtxDlg::OnBnClickedButtonQuit)
	ON_BN_CLICKED(IDC_BUTTON_MIN,	 &CYbtxDlg::OnBnClickedButtonMin)
	ON_BN_CLICKED(IDC_BUTTON_START,  &CYbtxDlg::OnBnClickedButtonStart)
	ON_BN_CLICKED(IDC_BUTTON_SERVER, &CYbtxDlg::OnBnClickedButtonServer)
	ON_BN_CLICKED(IDC_BUTTON_BBS,    &CYbtxDlg::OnBnClickedButtonBBS)
	ON_BN_CLICKED(IDC_BUTTON_CHARGE, &CYbtxDlg::OnBnClickedButtonCharge)
	ON_BN_CLICKED(IDC_BUTTON_REGIST, &CYbtxDlg::OnBnClickedButtonRegist) 
	ON_BN_CLICKED(IDC_BUTTON_OPENSERVERITEM,	&CYbtxDlg::OnBnClickedButtonOpenServerItem)
	ON_BN_CLICKED(IDC_BUTTON_MANUALUPDATE,		&CYbtxDlg::OnBnClickedButtonManualUpdate)
	ON_BN_CLICKED(IDC_BUTTON_SELECTSERVER,		&CYbtxDlg::OnBnClickedButtonSelectServer)
	ON_BN_CLICKED(IDC_BUTTON_LOOKCHAR,			&CYbtxDlg::OnBnClickedButtonLookChar) 
	ON_BN_CLICKED(IDC_BUTTON_OPENDUOWAN,		&CYbtxDlg::OnBnClickedButtonOpenDuoWan) 

	ON_MESSAGE(WM_USER_MESSAGE_CALLBACK, &CYbtxDlg::ShowMessage)
	ON_MESSAGE(WM_USER_PROGRESS_SETUP,   &CYbtxDlg::ChangeSetupState)
	ON_MESSAGE(WM_USER_PROGRESS_DOWN,    &CYbtxDlg::ChangeDownState)
	ON_MESSAGE(WM_USER_DOWN_FINISH,      &CYbtxDlg::DownloadFinish)
	ON_MESSAGE(WM_USER_RESTART,          &CYbtxDlg::RestartProcess)
	ON_MESSAGE(WM_USER_PATCH_CONTINUE,   &CYbtxDlg::PatchContinue)
	ON_MESSAGE(WM_USER_DOWN_YY_FINISH,	 &CYbtxDlg::DownloadYYFinish)
	ON_MESSAGE(WM_USER_VERIFY_FINISH,   &CYbtxDlg::VerifyFilesFinish)
	ON_WM_LBUTTONDOWN()
	ON_WM_SYSCOMMAND()
	ON_WM_NCPAINT()
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(CYbtxDlg, CDialog)
END_EVENTSINK_MAP()

// CYbtxDlg message handlers
BOOL CYbtxDlg::OnInitDialog()
{
	// 反调试
	// 先改变window name是为了在find window中排除自身

	SetWindowText( m_pStaticText->GetText(1).c_str() );
	// 更新时反调试内部会关闭父窗口(等待5秒)
	HWND hUnique;
	int nCount = 0;
	while ( (hUnique = ::FindWindow(L"#32770", m_pStaticText->GetText(21).c_str())) != NULL )
	{
		++nCount;
		Sleep(50);
		//HANDLE hProcess;
		//DWORD pid;
		//GetWindowThreadProcessId(hUnique, &pid);
		//hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
		if ( nCount > 100 ) // hProcess != ::GetModuleHandle(NULL) &&
		{
			MessageBox(m_pStaticText->GetText(2).c_str());
			exit(0);
		}
	}

	CDialog::OnInitDialog();

	// 快速不规则窗口
	HBITMAP hBmp = (HBITMAP)::LoadImage(NULL, GetImagePath(BMP_MAIN).c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	if(!hBmp)
	{
		wstring strOriImagePath = GetImagePath(BMP_MAIN);
		replace_all_distinct(strOriImagePath, CImageManage::GetCurLan(), CImageManage::GetOriLan(), 0);
		hBmp = (HBITMAP)::LoadImage(NULL, strOriImagePath.c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	}
	HRGN hRGN = m_RegionManage.BitmapToRegion(hBmp, RGB(255, 0, 0), 0); // 获得不规则区域 64, 64, 64
	SetWindowRgn(hRGN, TRUE);
	m_BGImage.LoadImage(GetImagePath(BMP_MAIN).c_str());
	BITMAP pBitmap = m_BGImage.GetBitmapInfo();
	SetWindowPos(NULL, 0, 0, pBitmap.bmWidth, pBitmap.bmHeight, SWP_NOZORDER | SWP_NOMOVE);
	SetWindowText(m_pStaticText->GetText(21).c_str());

	// 创建控件
	m_QuitBtn.Create(NULL, WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_OWNERDRAW, CRect(794,45,794+17,45+17),
		GetDC()->GetWindow(), IDC_BUTTON_QUIT);
	m_QuitBtn.LoadButtonImage(GetImagePath(BMP_QUIT1));
	m_QuitBtn.LoadHoverImage(GetImagePath(BMP_QUIT2));
	m_QuitBtn.LoadClickImage(GetImagePath(BMP_QUIT3));


	m_MinBtn.Create(NULL, WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_OWNERDRAW, CRect(776,45,776+17,45+17),
		GetDC()->GetWindow(), IDC_BUTTON_MIN);
	m_MinBtn.LoadButtonImage(GetImagePath(BMP_MIN1));
	m_MinBtn.LoadHoverImage(GetImagePath(BMP_MIN2));
	m_MinBtn.LoadClickImage(GetImagePath(BMP_MIN3));

	m_SetBtn.Create(NULL, WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_OWNERDRAW, CRect(626,362,626+126,362+18),
		GetDC()->GetWindow(), IDC_BUTTON_SET);
	m_SetBtn.LoadButtonImage(GetImagePath(BMP_SET1));
	m_SetBtn.LoadHoverImage(GetImagePath(BMP_SET2));
	m_SetBtn.LoadClickImage(GetImagePath(BMP_SET3));

	m_StartBtn.Create(NULL, WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON | BS_OWNERDRAW, CRect(641,522,809,565),
		GetDC()->GetWindow(), IDC_BUTTON_START);
	m_StartBtn.LoadButtonImage(GetImagePath(BMP_START1));
	m_StartBtn.LoadHoverImage(GetImagePath(BMP_START2));
	m_StartBtn.LoadClickImage(GetImagePath(BMP_START3));
	m_StartBtn.LoadDisableImage(GetImagePath(BMP_START4));
	m_StartBtn.SetState(eBS_DISABLE);

	m_SelectServerBtn.Create(NULL, WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON | BS_OWNERDRAW, CRect(641,522,809,565),
		GetDC()->GetWindow(), IDC_BUTTON_SELECTSERVER);
	m_SelectServerBtn.LoadButtonImage(GetImagePath(BMP_SELECT1));
	m_SelectServerBtn.LoadHoverImage(GetImagePath(BMP_SELECT2));
	m_SelectServerBtn.LoadClickImage(GetImagePath(BMP_SELECT3));
	m_SelectServerBtn.BringWindowToTop();
	m_SelectServerBtn.ShowWindow(SW_HIDE);

	m_RegistBtn.Create(NULL, WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_OWNERDRAW, CRect(416,26,519,140),
		GetDC()->GetWindow(), IDC_BUTTON_REGIST);
	m_RegistBtn.LoadButtonImage(GetImagePath(BMP_REGIST1));
	m_RegistBtn.LoadHoverImage(GetImagePath(BMP_REGIST2));
	m_RegistBtn.LoadClickImage(GetImagePath(BMP_REGIST3));

	m_ChargeBtn.Create(NULL, WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_OWNERDRAW, CRect(58,26,161,140),
		GetDC()->GetWindow(), IDC_BUTTON_CHARGE);
	m_ChargeBtn.LoadButtonImage(GetImagePath(BMP_CHARGE1));
	m_ChargeBtn.LoadHoverImage(GetImagePath(BMP_CHARGE2));
	m_ChargeBtn.LoadClickImage(GetImagePath(BMP_CHARGE3));

	m_bbsBtn.Create(NULL, WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_OWNERDRAW, CRect(519,26,622,140),
		GetDC()->GetWindow(), IDC_BUTTON_BBS);
	m_bbsBtn.LoadButtonImage(GetImagePath(BMP_BBS1));
	m_bbsBtn.LoadHoverImage(GetImagePath(BMP_BBS2));
	m_bbsBtn.LoadClickImage(GetImagePath(BMP_BBS3));

	m_ServerBtn.Create(NULL, WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_OWNERDRAW, CRect(161,26,264,140),
		GetDC()->GetWindow(), IDC_BUTTON_SERVER);
	m_ServerBtn.LoadButtonImage(GetImagePath(BMP_SERVER1));
	m_ServerBtn.LoadHoverImage(GetImagePath(BMP_SERVER2));
	m_ServerBtn.LoadClickImage(GetImagePath(BMP_SERVER3));

	m_ManualUpdateBtn.Create(NULL,WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_OWNERDRAW,CRect(626,341,626+126,341+18),
		GetDC()->GetWindow(),IDC_BUTTON_MANUALUPDATE);
	m_ManualUpdateBtn.LoadButtonImage(GetImagePath(BMP_MANUALUPDATE1));
	m_ManualUpdateBtn.LoadHoverImage(GetImagePath(BMP_MANUALUPDATE2));
	m_ManualUpdateBtn.LoadClickImage(GetImagePath(BMP_MANUALUPDATE3));

	m_OpenServerItemBtn.Create(NULL,WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON|BS_OWNERDRAW,CRect(629,437,629+120,437+53),
		GetDC()->GetWindow(),IDC_BUTTON_OPENSERVERITEM);
	m_OpenServerItemBtn.LoadButtonImage(GetImagePath(BMP_OPENSELECT1));
	m_OpenServerItemBtn.LoadHoverImage(GetImagePath(BMP_OPENSELECT2));
	m_OpenServerItemBtn.LoadClickImage(GetImagePath(BMP_OPENSELECT3));
	
	m_DownProgress.Create(NULL, WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_OWNERDRAW, CRect(59,497,639,594),
		GetDC()->GetWindow(), IDC_PROGRESS_DOWN);
	m_DownProgress.LoadProgressImage(GetImagePath(BMP_PROGRESS1), GetImagePath(BMP_PROGRESS2));

	m_SetupProgress.Create(NULL, WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_OWNERDRAW, CRect(59,497,639,594),
		GetDC()->GetWindow(), IDC_PROGRESS_DOWN);
	m_SetupProgress.LoadProgressImage(GetImagePath(BMP_PROGRESS2), GetImagePath(BMP_PROGRESS1));
	m_SetupProgress.ShowWindow(SW_HIDE);
	m_SetupProgress.BringWindowToTop();

	m_LocalVersion.Create(NULL, WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, CRect( 178,470,278,500),
		GetDC()->GetWindow(), IDC_LABEL_LOCAL);
	m_LocalVersion.SetTextColor(0x7a7a7a);
	m_OfficialVersion.Create(NULL, WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, CRect(335,470,435,500),
		GetDC()->GetWindow(), IDC_LABEL_OFFICIAL);
	m_OfficialVersion.SetTextColor(0x7a7a7a);

	m_SelectedServerName.Create(NULL, WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,CRect(635,416,635+116,416+30),
		GetDC()->GetWindow(),IDC_LABEL_SELECTEDSERVERNAME);
	m_SelectedServerName.SetTextColor(0xf5ffaf);
	m_SelectedServerName.SetTextStyle(DT_CENTER);

	m_LookCharBtn.Create(NULL,WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON|BS_OWNERDRAW,CRect(626,384,626+126,384+21),
		GetDC()->GetWindow(),IDC_BUTTON_LOOKCHAR);
	m_LookCharBtn.LoadButtonImage(GetImagePath(BMP_LOOKCHAR1));
	m_LookCharBtn.LoadHoverImage(GetImagePath(BMP_LOOKCHAR2));
	m_LookCharBtn.LoadClickImage(GetImagePath(BMP_LOOKCHAR3));

	m_OpenDuoWanBtn.Create(NULL,WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON|BS_OWNERDRAW,CRect(626,343,626+126,343+23),
		GetDC()->GetWindow(),IDC_BUTTON_OPENDUOWAN);
	m_OpenDuoWanBtn.LoadButtonImage(GetImagePath(BMP_DUOWAN1));
	m_OpenDuoWanBtn.LoadHoverImage(GetImagePath(BMP_DUOWAN2));
	m_OpenDuoWanBtn.LoadClickImage(GetImagePath(BMP_DUOWAN3));
	m_OpenDuoWanBtn.ShowWindow(SW_HIDE);

	// 浏览器界面(浏览器上所显示的每一个超链接文字，都是一个对象)
	m_WebBrowser.Create(NULL, WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON | BS_OWNERDRAW, CRect(110,144,110+503,144+318),
		GetDC()->GetWindow(), IDC_BUTTON_START);
	m_WebNews.SetWindowPos( NULL,110,144,503,318,0);

	m_SelectedServerDlg.Create(IDD_SELECTSERVER_DIALOG,this);

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	InitData();
	InitYY();
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CYbtxDlg::OnPaint()
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
		//dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CPaintDC dc(this);
		SelectObject(m_BGImage.GetMemDC(), m_BGImage.GetBitmapHandle());
		BitBlt(dc.GetSafeHdc(), 0, 0, m_BGImage.GetBitmapInfo().bmWidth, m_BGImage.GetBitmapInfo().bmHeight,
			m_BGImage.GetMemDC(), 0, 0, SRCCOPY);
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CYbtxDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CYbtxDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	PostMessage(WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(point.x, point.y));
}

void CYbtxDlg::OnBnClickedButtonSet()
{
	MessageBox(m_pStaticText->GetText(4).c_str());
}

void CYbtxDlg::OnBnClickedButtonQuit()
{
	
	if ( m_InPatch )
		MessageBox(m_pStaticText->GetText(3).c_str());
	else
		::PostQuitMessage(0);///exit(0);
}

void CYbtxDlg::OnBnClickedButtonMin()
{
	ShowWindow(SW_MINIMIZE);
}

void CYbtxDlg::OnBnClickedButtonStart()
{
	if ( m_StartBtn.GetState() == eBS_DISABLE )
		return;
	
	if (m_strServerInfo == L"") //打开选择服务器列表
	{
		m_SelectedServerDlg.ShowWindow(SW_SHOWNORMAL);
		ShowSelectServerBtn();
		return;
	}

	// get current directory
	wchar_t CurPath[PATH_SIZE];
	memset(CurPath, 0, sizeof(CurPath));
	FixCurrentDir(CurPath);

	wstring strTempDir(CurPath);
	wstring strRunModuleName = strTempDir + L"RunModule.exe";
	wstring strNewFileName = strTempDir + L"GameGac.sso";

	wchar_t* szCommandLine = _wcsdup((L"\""+ strRunModuleName + L"\" \"" + strNewFileName+L"\" "+m_strServerInfo+L"\"").c_str());

	DWORD err_code = 0;
	if ( ! StartupProcessAndQuit(NULL, szCommandLine, strTempDir.c_str(), err_code) )
	{
		FILE* fp;
		_wfopen_s(&fp, ERR_LOG, L"w");
		fwprintf(fp, L"command line: %s\nworking dir: %s\n Error Code: %u\n", szCommandLine, strTempDir.c_str(), err_code);
		fclose(fp);
		MessageBox(m_pStaticText->GetText(5).c_str());
		exit(1);
	}

	free(szCommandLine);
}

void CYbtxDlg::OnBnClickedButtonServer()
{
	//ShellExecute(NULL, L"open", L"http://bbs.yb.linekong.com", NULL, NULL, SW_SHOWNORMAL);
	ShellExecute(NULL, L"open", L"http://kefu.linekong.com/eService/system/inputLogin.do?gameId=8", NULL, NULL, SW_SHOWNORMAL);
}

void CYbtxDlg::OnBnClickedButtonBBS()
{
	ShellExecute(NULL, L"open", L"http://bbs.yb.linekong.com", NULL, NULL, SW_SHOWNORMAL);
}

void CYbtxDlg::OnBnClickedButtonCharge()
{
	//MessageBox(m_pStaticText->GetText(6).c_str());
	ShellExecute(NULL, L"open", L"http://pay.linekong.com/charging.do?method=chargingTotal&gameid=8", NULL, NULL, SW_SHOWNORMAL);
}

void CYbtxDlg::OnBnClickedButtonRegist()
{
	ShellExecute(NULL, L"open", L"http://yb.linekong.com/page/", NULL, NULL, SW_SHOWNORMAL);
	//ShellExecute(NULL, L"open", L"http://passport.linekong.com/register/accountRegister.jsp", NULL, NULL, SW_SHOWNORMAL);
}

void CYbtxDlg::OnBnClickedButtonSelectServer()
{
	::SendMessage(m_SelectedServerDlg.GetDlgItem(IDC_BUTTON_SELECTOK)->GetSafeHwnd(),WM_LBUTTONDOWN,NULL,NULL);
	::SendMessage(m_SelectedServerDlg.GetDlgItem(IDC_BUTTON_SELECTOK)->GetSafeHwnd(),WM_LBUTTONUP,NULL,NULL);
} 

void CYbtxDlg::OnBnClickedButtonOpenDuoWan()
{
	ShellExecute(NULL, L"open", L"http://yb.duowan.com/", NULL, NULL, SW_SHOWNORMAL);
} 

void CYbtxDlg::OnBnClickedButtonLookChar()
{
	ShellExecute(NULL, L"open", L"http://yb.linekong.com/activity/query/index.php", NULL, NULL, SW_SHOWNORMAL);

} 

void CYbtxDlg::OnBnClickedButtonOpenServerItem()
{
	BOOL bIsVisible = m_SelectedServerDlg.IsWindowVisible();
	if ( bIsVisible)
		m_SelectedServerDlg.ShowWindow(SW_HIDE);
	else
		m_SelectedServerDlg.ShowWindow(SW_SHOWNORMAL);
	ShowSelectServerBtn();
}

void CYbtxDlg::ShowSelectServerBtn()
{
	BOOL bIsVisible = m_SelectedServerDlg.IsWindowVisible();
	if ( bIsVisible)
	{
		m_SelectServerBtn.ShowWindow(SW_SHOWNORMAL);
		m_StartBtn.ShowWindow(SW_HIDE);
	}
	else
	{
		m_SelectServerBtn.ShowWindow(SW_HIDE);
		m_StartBtn.ShowWindow(SW_SHOWNORMAL);
	}
}

void CYbtxDlg::OnBnClickedButtonManualUpdate()
{
	ShellExecute(NULL, L"open", L"http://yb.linekong.com/download.php", NULL, NULL, SW_SHOWNORMAL);
}

LRESULT CYbtxDlg::ShowMessage(WPARAM wparam, LPARAM lparam)
{
	MessageBox(ShareStack::GetInst()->PopMsg().c_str());
	return 0;
}


LRESULT CYbtxDlg::ChangeDownState(WPARAM wparam, LPARAM lparam)
{
	m_DownProgress.SetPos(int(wparam));
	return 0;
}

LRESULT CYbtxDlg::ChangeSetupState(WPARAM wparam, LPARAM lparam)
{
	m_SetupProgress.SetPos(int(wparam));
	return 0;
}


LRESULT CYbtxDlg::RestartProcess(WPARAM wparam, LPARAM lparam)
{
	// 更新完毕，启动更新后的Ybtx.exe
	m_bNeedMd5Verify = true;
	wchar_t buf[MAX_PATH];
	_snwprintf_s(buf, _countof(buf), ShareStack::GetInst()->PopCmd().c_str());

	if (m_bNeedMd5Verify)
	{
		wcscat_s(buf, _countof(buf), L" md5verify");
	}

	//删除安装标记文件
	DWORD dwResult;
	TryDeleteFile( INPATCH_FLAG,dwResult);

	DWORD err_code = 0;
	if ( ! StartupProcessAndQuit(m_StarterFile.c_str(), buf, m_StarterPath.c_str(), err_code) )
	{
		FILE* fp;
		_wfopen_s(&fp, ERR_LOG, L"w");
		fwprintf(fp, L"app name: %s\ncommand line: %s\nworking dir: %s\n Error Code: %u\n", m_StarterFile.c_str(), buf, m_StarterPath.c_str(), err_code);
		fclose(fp);
		MessageBox(m_pStaticText->GetText(7).c_str());
		exit(1);
	}

	return 0;
}

// 无需重启启动器继续上补丁
LRESULT CYbtxDlg::PatchContinue(WPARAM wparam, LPARAM lparam)
{
	m_bNeedMd5Verify = true;
	m_InUpdate = true;
	m_InPatch = false;
	wstring ServerListUrl( GetSetting(m_wstrUrlPath.c_str() , SERVER_LIST_SECTION, DEFAULT_SERVER_LIST_URL, L"") );

	// 更新本地版本号
	wchar_t szBuf[MAX_PATH];
	GetPrivateProfileString(L"Setting", L"Version", NULL, szBuf, MAX_PATH, UPDATE_INI);
	m_LocalVersion.SetWindowText(szBuf);
	m_OfficialVersion.SetWindowText(utf8_to_utf16(m_TargetVersion).c_str());

	m_SetupProgress.ShowWindow(SW_HIDE);

	//删除安装标记文件
	DWORD dwResult;
	TryDeleteFile( INPATCH_FLAG,dwResult);

	// 检查是否还有补丁可上
	if ( m_pVersion->GetPatchList().size() <= 0 )
	{
		m_InUpdate = false;

		// 因为Url.ini有可能被更新了，所以需要重新载入列表
		wstring NewsUrl( GetSetting(m_wstrUrlPath.c_str() , NEWS_SECTION, DEFAULT_NEWS_URL, L"") );

		m_WebNews.Navigate(NewsUrl.c_str(), NULL, NULL, NULL, NULL);

		// 已上完所有补丁，关闭YbtxTemp.exe，启动Ybtx.exe
		StartupStarter();
	}
	else
	{
		wstring strPatchUrl = utf8_to_utf16(m_pVersion->PopUrl());

		size_t flag = strPatchUrl.find_last_of(L"/");
		m_PatchUrl.assign(strPatchUrl.substr(0, flag));
		m_PatchFile.assign(strPatchUrl.substr(flag+1, strPatchUrl.size()));
		// 探测用户是否有中途改变升级目标列列表
		
		// 开始下载补丁
		m_DownType = eDT_PatchFile;
		m_pDownloader->DownloadFile(m_PatchUrl.c_str(), m_PatchFile.c_str(), m_DownloadDir);
	}

	return 0;
}

// 下载完毕回调总函数
LRESULT CYbtxDlg::DownloadFinish(WPARAM wparam, LPARAM lparam)
{
	switch ( m_DownType )
	{
		case eDT_VersionList:
			ProcessList();
			break;
		case eDT_PatchFile:
			ApplyPatch();
			break;
		case eDT_ServerList:
			AnalysisServerList();
			break;
	}
	return 0;
}

// 解析Xml服务器列表并用GDI显示
void CYbtxDlg::AnalysisServerList()
{
	m_pUpdater->SetDownState(0);
	//wstring wstrServerListPath(m_DownloadDir + L"\\" + m_ServerListFile);
	wstring wstrServerListPath(L"../../download/" + m_ServerListFile);
	m_SelectedServerDlg.InitServerGroupList(wstrServerListPath);
	UpdateSelectedServer();
}

void CYbtxDlg::ProcessList()
{
	m_pUpdater->SetDownState(0); // 将下载条恢复空状态
	// 寻找下一个可下载补丁
	if ( m_pVersion->GetVersionList(m_PatchListFile.c_str(), m_TargetVersion.c_str()) != false )
	{
		wstring strPatchUrl = utf8_to_utf16(m_pVersion->PopUrl());

		size_t flag = strPatchUrl.find_last_of(L"/");
		m_PatchUrl.assign(strPatchUrl.substr(0, flag));
		m_PatchFile.assign(strPatchUrl.substr(flag+1, strPatchUrl.size()));
		// 开始下载补丁
		m_DownType = eDT_PatchFile;
		m_InUpdate = true;
		m_DownProgress.LoadProgressImage(GetImagePath(BMP_PROGRESS3),GetImagePath(BMP_PROGRESS2));
		m_pDownloader->DownloadFile(m_PatchUrl.c_str(), m_PatchFile.c_str(), m_DownloadDir);
	}
	else 
	{
		//MessageBox(m_pStaticText->GetText(8).c_str());
		//exit(0);
	}
}

void CYbtxDlg::ApplyPatch()
{
	// 开始上补丁
	m_InUpdate = false;
	m_InPatch = true;
	// md5校验
	wstring PatchPath(m_DownloadDir);
	PatchPath += L"\\";
	PatchPath += m_PatchFile;

	FILE* fp = 0;
	_wfopen_s(&fp, PatchPath.c_str(), L"rb");
	if ( ! fp )
	{
		MessageBox(m_pStaticText->GetText(23, PatchPath.c_str()).c_str(), 0, 0);
		exit(1);
	}
	fseek(fp, 0, SEEK_END);
	uint uLength = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	PatchHead head;
	fread(&head, sizeof(PatchHead), 1, fp);
	uint32 uCompressSize = uLength - sizeof(PatchHead);
	uint8* pData = new uint8[uCompressSize]; // 不确定内存泄漏
	fread(pData, sizeof(uint8), uCompressSize, fp);
	fclose(fp);

	const int md5Size = 16;
	CMd5Algorithm digester;
	digester.Begin();
	digester.Append(pData, uCompressSize);
	uint8 szBuf[md5Size];
	digester.End(szBuf);

	if ( memcmp(szBuf, head.md5Code, md5Size) != 0 )
	{
		if ( MessageBox(m_pStaticText->GetText(9).c_str(), 0, MB_YESNO) == IDYES )
		{
			DeleteFile(PatchPath.c_str());
			m_DownType = eDT_PatchFile;
			m_pDownloader->DownloadFile(m_PatchUrl.c_str(), m_PatchFile.c_str(), m_DownloadDir);
		}
		else
		{
			exit(0);
		}
	}
	else
	{
		m_PatchArgs->PatchFile.assign(PatchPath);
		m_PatchArgs->strMsg1.assign(m_pStaticText->GetText(14).c_str());
		m_PatchArgs->strMsg2.assign(m_pStaticText->GetText(15).c_str());
		unsigned int tid; // 标识线程的ID
		uintptr_t BTThread;
		
		BTThread = _beginthreadex(NULL, 0, &CYbtxDlg::StartPatch, m_PatchArgs, 0, &tid); // 第三个参数是创建线程运行的函数
		if(!BTThread)
		{
			MessageBox(m_pStaticText->GetText(10).c_str());
			exit(1);
		}
		//CreateInPatchFlag();
		m_SetupProgress.ShowWindow(SW_SHOWNORMAL);
	}
}

void CYbtxDlg::StartupUpdater()
{
	// 如果原文件存在，先删除
	DWORD dwResult;

	if ( !TryDeleteFile( m_UpdaterFile.c_str(),dwResult) )
	{
		MessageBox(m_pStaticText->GetText(24, m_UpdaterFile.c_str(), dwResult).c_str());
		exit(1);
	}

	// 复制出更新器
	if ( !CopyFile(m_StarterFile.c_str(), m_UpdaterFile.c_str(), FALSE ) )
	{
		MessageBox(m_pStaticText->GetText(25, m_UpdaterFile.c_str()).c_str());
		exit(1);
	}

	// 启动Updater
	DWORD dwProcessID = GetCurrentProcessId();
	wchar_t szBuf[MAX_PATH];
	_snwprintf_s(szBuf, MAX_PATH, L"ProcessID:%x", dwProcessID);

	if (m_bNeedMd5Verify)
	{
		wcscat_s(szBuf, _countof(szBuf), L"md5verify");
	}

	DWORD err_code = 0;

	if ( !StartupProcessAndQuit(m_UpdaterFile.c_str(), szBuf, m_StarterPath.c_str(), err_code) )
	{
		MessageBox(m_pStaticText->GetText(26, m_StarterPath.c_str(), m_UpdaterFile.c_str(), err_code).c_str());
		exit(1);
	}
}

void CYbtxDlg::StartupStarter()
{
	// 更新完毕，启动更新后的Ybtx.exe
	wchar_t szBuf[MAX_PATH];	
	wchar_t* pCmdLine = NULL;
	if (m_bNeedMd5Verify)
	{
		_snwprintf_s(szBuf, MAX_PATH, L"md5verify");
		pCmdLine = szBuf;
	}

	DWORD err_code = 0;
	if ( ! StartupProcessAndQuit(m_StarterFile.c_str(), pCmdLine, m_StarterPath.c_str(), err_code) )
	{
		MessageBox(m_pStaticText->GetText(26, m_StarterPath.c_str(), m_StarterFile.c_str(), err_code).c_str());
		exit(1);
	}
}

void CYbtxDlg::InitData()
{
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	hFind = FindFirstFile(INT_URL_INI_FILE, &FindFileData);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		m_wstrUrlPath = INT_URL_INI_FILE;
		MessageBox(m_pStaticText->GetText(11).c_str());
	}
	else
	{
		m_wstrUrlPath = URL_INI_FILE;
	}
	wchar_t szCurPath[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, szCurPath);
	wstring strCurPath(szCurPath);

	m_StarterFile = strCurPath + L"\\Ybtx.exe";
	m_UpdaterFile = strCurPath + L"\\YbtxTemp.exe";
	m_StarterPath = strCurPath + L"\\";
	//SetFileAttributes(m_UpdaterFile.c_str(), FILE_ATTRIBUTE_NORMAL);

	// 拼合下载目录
	m_DownloadDir.assign(szCurPath);
	m_DownloadDir += L"\\";
	m_DownloadDir += L"../../";
	m_DownloadDir += PATCH_LIST_SAVE;

	m_PatchListFile.assign(m_DownloadDir);
	m_PatchListFile += L"\\";
	m_PatchListFile += PATCH_LIST_NAME;

	wchar_t szBuf[MAX_PATH];
	// 读取Url.ini文件获取下载类型
	GetPrivateProfileString(L"Download", L"DownMode", L"none", szBuf, MAX_PATH, m_wstrUrlPath.c_str() );
	if ( wcscmp(szBuf, L"bt") == 0 )
		m_eDownMode = eDM_BT;
	else if ( wcscmp(szBuf, L"http") == 0 )
		m_eDownMode = eDM_HTTP;
	else
		StartLogUpload(m_wstrUrlPath); // 如果无更新(开发测试版本)，直接上传log

	// 获得补丁下载服务器链接
	m_PatchListUrl = GetSetting(m_wstrUrlPath.c_str() , PATCH_LIST_SECTION, DEFAULT_PATCH_LIST_URL, L"");

	// 初始化目标版本为Update.ini版本
	GetPrivateProfileString(L"Setting", L"Version", NULL, szBuf, MAX_PATH, UPDATE_INI);
	m_TargetVersion.assign(utf16_to_utf8(szBuf));
	m_LocalVersion.SetWindowText(utf8_to_utf16(m_TargetVersion).c_str());

	// 初始化servernew 
	m_NewsUrl.assign( GetSetting(m_wstrUrlPath.c_str(), NEWS_SECTION, DEFAULT_NEWS_URL, L"") );
	m_WebNews.Navigate(m_NewsUrl.c_str(), NULL, NULL, NULL, NULL);

	// 每次初始化启动器时重新下载补丁列表，避免续传永久错误(目前续传没有验证机制)
	if ( ! CreateDirectory(m_DownloadDir.c_str(), NULL) && GetLastError() != ERROR_ALREADY_EXISTS )
	{
		MessageBox(m_pStaticText->GetText(27, m_DownloadDir.c_str(), GetLastError()).c_str());
	}

	FILE* fp = 0;
	_wfopen_s(&fp, m_PatchListFile.c_str(), L"wb");
	if ( !fp )
	{
		MessageBox(m_pStaticText->GetText(28, m_PatchListFile.c_str(), GetLastError()).c_str());
	}
	fclose(fp);

	//检测安装标记文件是否存在
	//FILE* fp1 = 0;
	//_wfopen_s(&fp1, INPATCH_FLAG, L"r");
	//if (fp1)
	//{
	//	MessageBox(m_pStaticText->GetText(38).c_str());
	//	fclose(fp1);
	//	exit(1);
	//}
	
	//ReadCrcCodeToMap();
	//Crc32Check();
	DownloadServerListFile();
}

void CYbtxDlg::DownloadServerListFile()
{
	//下载server list xml文件
	wstring wstrServerListLink;
	wstrServerListLink.assign( GetSetting(m_wstrUrlPath.c_str() , SERVER_LIST_SECTION, DEFAULT_SERVER_LIST_URL, L"") );

	size_t nRet = wstrServerListLink.find_last_of(L"/");
	m_ServerListUrl = wstrServerListLink.substr(0, nRet);
	m_ServerListFile = wstrServerListLink.substr(nRet+1, wstrServerListLink.size());

	wstring wstrServerListFile;
	wstrServerListFile.assign(m_DownloadDir);
	wstrServerListFile += L"\\";
	wstrServerListFile += m_ServerListFile;

	FILE* fp = 0;
	_wfopen_s(&fp, wstrServerListFile.c_str(), L"wb");
	if ( !fp )
	{
		MessageBox(m_pStaticText->GetText(28, wstrServerListFile.c_str(), GetLastError()).c_str());
	}
	fclose(fp);

	m_DownType = eDT_ServerList;
	m_pDownloader->DownloadFile( m_ServerListUrl.c_str(), m_ServerListFile.c_str(), m_DownloadDir.c_str() );
}

bool CYbtxDlg::ProcessEntry(const char* szNewTarget)
{
	wstring strCommandLine(GetCommandLine());
	if (strCommandLine.find(L"ProcessID") != strCommandLine.npos) //判断是否为启动器更新器
	{
		HWND hUnique;
		// 确认没有游戏在运行
		if ( (hUnique = ::FindWindow(L"MainWnd class of Engine", NULL)) != NULL )
		{
			MessageBox(m_pStaticText->GetText(13).c_str());
			exit(0);
		}

		// 确认没有UploadLog在运行
		while ( (hUnique = ::FindWindow(L"#32770", L"ybtx_log_uploader")) != NULL &&
			ProcessActive(L"UploadLog.exe") )
		{
			HANDLE hProcess;
			hProcess = GetModuleHandle(L"UploadLog.exe"); // 发布时可把名字改为YbtxUploadLog.exe
			if ( ! hProcess )
			{
				MessageBox(m_pStaticText->GetText(29, GetLastError()).c_str());
				exit(0);
			}

			if ( ! TerminateProcess(hProcess, 0) )
			{
				MessageBox(m_pStaticText->GetText(30, GetLastError()).c_str());
				exit(0);
			}
		}

		// 重新设置目标版本，并下载补丁列表
		m_TargetVersion.assign(szNewTarget);
		m_DownType = eDT_VersionList;
		m_pDownloader->DownloadFile( m_PatchListUrl.c_str(), PATCH_LIST_NAME, m_DownloadDir.c_str() );
		return true;
	}
	return false;
}

// http路线上patch
unsigned int __stdcall CYbtxDlg::StartPatch(void* pClass)
{
	PatchArgs* args = static_cast<PatchArgs*>(pClass);
	try
	{
		CPatcher patch;
		patch.CheckHeadInfo(args->handler, args->PatchFile.c_str());
	}
	catch(std::exception& exp)
	{
		::MessageBox(NULL, utf8_to_utf16(exp.what()).c_str(), args->strMsg1.c_str(), MB_OK);
		exit(1);
	}
	catch(...)
	{
		::MessageBox(NULL, args->strMsg2.c_str(), args->strMsg2.c_str(), MB_OK);
		exit(1);
	}
	return 0;
}

void CYbtxDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ( nID == SC_CLOSE && m_InPatch )
	{
		MessageBox(m_pStaticText->GetText(16).c_str());
		return;
	}

	CDialog::OnSysCommand(nID, lParam);
}

void CYbtxDlg::ReadCrcCodeToMap()
{
	FILE* pFile = _wfopen(CRC32_FILES_CHECK, L"rb");
	if (pFile)
	{
		char szLine[MAX_PATH];
		memset(szLine, '\0', sizeof(szLine));
		while (fgets(szLine, MAX_PATH, pFile) != NULL)
		{
			char* szCode = strchr(szLine, '\t');
			if (szCode != NULL)
			{
				int uPos = (int)(szCode - szLine);
				char* szFileName = new char[uPos + 1];
				strncpy(szFileName, szLine, uPos);
				szFileName[uPos] = '\0';
				szCode++;
				m_hmapCrcCode.insert(make_pair(szFileName, szCode));
				delete[] szFileName;
				szFileName = NULL;
			}
		}
		fclose(pFile);
	}
}

void CYbtxDlg::Crc32Check()
{
	typedef hash_map<string, string>::iterator IterHmapSS;
	for (IterHmapSS iter = m_hmapCrcCode.begin(); iter != m_hmapCrcCode.end(); ++iter)
	{
		string strFile = iter->first;
		DoCrc32Check(strFile.c_str());
	}
}

void CYbtxDlg::DoCrc32Check( const string& strFile )
{
	FILE* fp = 0;
	fopen_s(&fp, strFile.c_str(), "rb");
	if (!fp)
	{
		wstring wstrMsg = L"无法打开文件：";
		wstrMsg.append(utf8_to_utf16(strFile));
		MessageBox(wstrMsg.c_str(), 0, 0);
		exit(1);
	}
	fseek(fp, 0, SEEK_END);
	uint uLength = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	uint8* pData = new uint8[uLength];
	fread(pData, sizeof(uint8), uLength, fp);
	fclose(fp);

	unsigned __int32 uCrc32Code = CSysAlgorithmCrc::Instance()->Crc32(pData, uLength);
	
	string strFileRealName = strFile;
	size_t uPos = strFile.rfind("/");
	if (uPos != -1)
	{
		strFileRealName = strFile.substr(uPos + 1, strFile.length() - uPos - 1);
	}

	hash_map<string, string>::iterator iter = m_hmapCrcCode.find(strFile);
	__int64 uCode;
	if (iter == m_hmapCrcCode.end() || (uCode = _atoi64(iter->second.c_str())) != uCrc32Code)
	{
		wstring wstrMsg = L"文件：";
		wstrMsg.append(utf8_to_utf16(strFileRealName));
		wstrMsg.append(L" 校验码不正确");
		MessageBox(wstrMsg.c_str(), 0, 0);
		exit(1);
	}
	return;
}

void CYbtxDlg::UpdateSelectedServer()
{
	const CServerInfo* serverInfo = m_SelectedServerDlg.GetServerInfo();
	if ( serverInfo == NULL ) //找不到可以更新的目标服务器，退出更新流程
	{
		m_InUpdate = false;
		ChangeNormalState();
		return;
	}
	m_strServerInfo = utf8_to_utf16(serverInfo->server + ";" + serverInfo->name + ";" + serverInfo->param);
	string strTgtVersion = serverInfo->version;
	try
	{
		if ( m_eDownMode == eDM_NONE )
		{
			m_InUpdate = false;
			m_StartBtn.SetState(eBS_NORMAL);
			m_OfficialVersion.SetWindowText(utf8_to_utf16(strTgtVersion).c_str()); 
			m_SelectedServerName.SetWindowText(m_SelectedServerDlg.GetServerName().c_str());
			return;
		}

		if ( m_bNeedReload ) 
		{
			m_bNeedReload = false;
			m_OfficialVersion.SetWindowText(utf8_to_utf16(strTgtVersion).c_str()); // 首次载入时设置服务器目标版本
			m_SelectedServerName.SetWindowText(m_SelectedServerDlg.GetServerName().c_str());
			//判断是否为启动器(Ybtx.exe),根据传入参数
			if ( ProcessEntry(strTgtVersion.c_str()) ) //不是启动器 ,则是启动器更新器(YbtxTemp.exe)
			{
				return;
			}
			else //是启动器
			{
				//判断版本是否一致
				if ( m_TargetVersion != strTgtVersion ) //不一致，启动启动器更新器(YbtxTemp.exe)
				{
					StartupUpdater();
					return;
				}	
				else //一致，不需要更新，更新流程结束。
				{	
					//m_InUpdate = false;
					//m_InPatch = false;
					//StartLogUpload(m_wstrUrlPath); // 上传log
					//ChangeNormalState();
					VerifyFiles();
					return;
				}
			}
		}
	}
	catch(std::exception& exp)
	{
		::MessageBox(NULL, utf8_to_utf16(exp.what()).c_str(), m_pStaticText->GetText(14).c_str(), MB_OK);
		exit(1);
	}
	catch(...)
	{
		::MessageBox(NULL, m_pStaticText->GetText(15).c_str(), m_pStaticText->GetText(15).c_str(), MB_OK);
		exit(1);
	}
}

void CYbtxDlg::ChangeServer() 
{
	const CServerInfo* serverInfo = m_SelectedServerDlg.GetServerInfo();
	if ( serverInfo == NULL ) //找不到可以更新的目标服务器，退出更新流程
	{
		m_InUpdate = false;
		ChangeNormalState();
		return;
	}
	m_strServerInfo = utf8_to_utf16(serverInfo->server + ";" + serverInfo->name + ";" + serverInfo->param);
	string strTgtVersion = serverInfo->version;
	try
	{
		if ( m_eDownMode == eDM_NONE )
		{
			m_InUpdate = false;
			m_StartBtn.SetState(eBS_NORMAL);
			m_OfficialVersion.SetWindowText(utf8_to_utf16(strTgtVersion).c_str());
			m_SelectedServerName.SetWindowText(m_SelectedServerDlg.GetServerName().c_str());
			m_SelectedServerDlg.SaveSelectServer();
			return;
		}

		if ( m_TargetVersion != strTgtVersion ) // 更改服务器，判断版本是否一致。 
		{
			if ( m_InPatch )
			{
				MessageBox(m_pStaticText->GetText(17).c_str(), m_pStaticText->GetText(18).c_str(), MB_OK);
			}
			else if ( m_InUpdate )
			{
				if ( MessageBox(m_pStaticText->GetText(19).c_str(), 0, MB_YESNO) == IDYES )
				{
					HANDLE handleDownload = m_pDownloader->GetCurThreadHandle();
					unsigned int uThreadId = m_pDownloader->GetCurThreadId(); 
					BOOL bRetVal = PostThreadMessage(uThreadId, WM_USER_DOWNLOAD_SWITCH, 0, 0);
					if (!bRetVal)
					{
						MessageBox(m_pStaticText->GetText(20).c_str(), 0, MB_OK);
						return;
					}
					WaitForSingleObject(handleDownload, INFINITE);
					m_TargetVersion = strTgtVersion;
					m_pVersion->GetVersionList(m_PatchListFile.c_str(), m_TargetVersion.c_str());
					m_pUpdater->PatchContinue();
					m_OfficialVersion.SetWindowText(utf8_to_utf16(strTgtVersion).c_str());
					m_SelectedServerName.SetWindowText(m_SelectedServerDlg.GetServerName().c_str());
					m_SelectedServerDlg.SaveSelectServer();
				}
			}
			else 
			{
				m_OfficialVersion.SetWindowText(utf8_to_utf16(strTgtVersion).c_str());
				m_SelectedServerName.SetWindowText(m_SelectedServerDlg.GetServerName().c_str());
				m_SelectedServerDlg.SaveSelectServer();
				StartupUpdater();
			}
		}
		else
		{
			m_OfficialVersion.SetWindowText(utf8_to_utf16(strTgtVersion).c_str());
			m_SelectedServerName.SetWindowText(m_SelectedServerDlg.GetServerName().c_str());
			m_SelectedServerDlg.SaveSelectServer();
		}
	}
	catch(std::exception& exp)
	{
		::MessageBox(NULL, utf8_to_utf16(exp.what()).c_str(), m_pStaticText->GetText(14).c_str(), MB_OK);
		exit(1);
	}
	catch(...)
	{
		::MessageBox(NULL, m_pStaticText->GetText(15).c_str(), m_pStaticText->GetText(15).c_str(), MB_OK);
		exit(1);
	}
}

void CYbtxDlg::InitLangPath()
{
	TiXmlDocument* xmlDoc = new TiXmlDocument();
	m_strLangResPath  = LANG_PATH;
	m_strLangResPath  += "/";
	m_strOriLangResPath = m_strLangResPath;
	string strLangXmlPath = LANG_PATH;
	strLangXmlPath += "/";
	strLangXmlPath += LANG_XML_NAME;
	if (xmlDoc->LoadFile(strLangXmlPath.c_str()))
	{
		TiXmlNode* pLangCfgNode = xmlDoc->FirstChild("LanguageConfig");
		if (pLangCfgNode)
		{
			//TiXmlElement* pElement = pLangCfgNode->FirstChild("DefaultLanguage")->ToElement();
			TiXmlElement* pLangElement = pLangCfgNode->FirstChild("Language")->ToElement();
			string strStarterLangRes = pLangElement->Attribute("starter_lang_res");

			m_strLangResPath  += strStarterLangRes;
			//m_strStaticTxtPath = m_strLangResPath  + "/" + STATIC_TEXT_FILE_NAME;
			m_strStaticTxtPath = m_strLangResPath  + "__" + STATIC_TEXT_FILE_NAME;

			pLangElement = pLangCfgNode->FirstChild("OriginalLanguage")->ToElement();
			string strStarterOriLangRes = pLangElement->Attribute("starter_lang_res");

			m_strOriLangResPath  += strStarterOriLangRes;
			m_strOriStaticTxtPath = m_strOriLangResPath  + "__" + STATIC_TEXT_FILE_NAME;

			wstring wstrStarterLangRes = utf8_to_utf16(strStarterLangRes);
			wstring wstrStarterOriLangRes = utf8_to_utf16(strStarterOriLangRes);
			CImageManage::SetMultiLang(wstrStarterLangRes.c_str(), wstrStarterOriLangRes.c_str());

		}
		delete xmlDoc;
		xmlDoc = NULL;
	}
	else
	{
		delete xmlDoc;
		xmlDoc = NULL;
		exit(0);
	}
}

wstring CYbtxDlg::GetImagePath( const char* szImageFileName )
{
	//string strTempPath = m_strLangResPath + "/" + szImageFileName;
	string strTempPath = m_strLangResPath + "__" + szImageFileName;
	wstring strImageFilePath = utf8_to_utf16(strTempPath);
	return strImageFilePath;
}
//YY相关
void YYCallbackShowState( int nCurSize, int nMaxSize, int nSpeed )
{
}

void YYCallbackResult( int bUpdateRet )
{
	if( bUpdateRet == TRUE )
		theApp.GetMainWnd()->PostMessage(WM_USER_DOWN_YY_FINISH, 0, 0);
	else
		theApp.GetMainWnd()->PostMessage(WM_USER_DOWN_YY_FINISH, 0, 0);
}

void YYCallbackShowMessage( const char* szText ) 
{
}

void CYbtxDlg::InitYY()
{
	wchar_t szBuf[MAX_PATH];
	GetPrivateProfileString(L"YYUpdate", L"Update", L"false", szBuf, MAX_PATH, m_wstrUrlPath.c_str() );
	if ( wcscmp(szBuf, L"true") == 0 )
	{
		WCHAR wzYYPath[MAX_PATH];
		memset( wzYYPath, 0, sizeof(wzYYPath) );
		GetModuleFileName( NULL, wzYYPath, sizeof(wzYYPath) );
		LPWSTR lpInsertPos = wcsrchr( wzYYPath, L'\\' );
		*lpInsertPos = L'\0';
	
		WCHAR wzFileName[MAX_PATH];
		memset( wzFileName, 0, sizeof(wzFileName) );
		wsprintf( wzFileName, L"%s\\pipFactory.dll", wzYYPath );
		HMODULE hDll = LoadLibrary( wzFileName );
		if( hDll == NULL ) 
		{
			MessageBox(m_pStaticText->GetText(36).c_str());
			return;
		}

		memset( m_szYYFilePath, 0, sizeof(m_szYYFilePath) );
		GetModuleFileNameW( NULL, m_szYYFilePath, sizeof(m_szYYFilePath) );
		LPWSTR lpInsertPosFile = wcsrchr( m_szYYFilePath, L'\\' );
		*lpInsertPosFile = L'\0';
		lstrcatW( m_szYYFilePath, L"\\yy.zip" );

		pfnYYUpdate = (PFN_UPDATAYYVOICE)GetProcAddress( hDll, "YYPIP_StartUpdate" );
		if( pfnYYUpdate == NULL ) 
		{
			MessageBox(m_pStaticText->GetText(37).c_str());
			return;
		}
		pfnYYUpdate( utf16_to_utf8(m_szYYFilePath).c_str(), YYCallbackShowMessage, YYCallbackShowState,YYCallbackResult);
		m_InYYUpdate = true;
	}
}

LRESULT CYbtxDlg::DownloadYYFinish(WPARAM wparam, LPARAM lparam)
{
	YYChangeNormalState();
	return 0;
}

void CYbtxDlg::YYChangeNormalState()
{
	m_InYYUpdate = false;
	wstring strCommandLine(GetCommandLine());
	if( !m_InYbtxUpdate && strCommandLine.find(L"ProcessID") == strCommandLine.npos )
		m_StartBtn.SetState(eBS_NORMAL);
}

void CYbtxDlg::ChangeNormalState()
{
	m_InYbtxUpdate = false;
	if (!m_InYYUpdate)
		m_StartBtn.SetState(eBS_NORMAL);
}
LRESULT CYbtxDlg::VerifyFilesFinish(WPARAM wparam, LPARAM lparam)
{

	BOOL bVarifySuccess = (BOOL)wparam;
	if (!bVarifySuccess)
	{
		wstring strHint = ShareStack::GetInst()->PopMsg();
		strHint += L"\n仍继续游戏吗？选否则跳转到下载页面。";
		if (::MessageBox(NULL, strHint.c_str(), L"校验文件时发生错误", MB_YESNO) == IDNO)
		{
			ShellExecute(NULL, L"open", L"http://yb.linekong.com/download.php", NULL, NULL, SW_SHOWNORMAL);
			exit(1);
		}
	}

	m_InUpdate = false;
	m_InPatch = false;
	StartLogUpload(m_wstrUrlPath); // 上传log
	ChangeNormalState();

	m_SetupProgress.ShowWindow(SW_HIDE);
	m_DownProgress.ShowWindow(SW_SHOWNORMAL);


	//m_pUpdater->SetDownState(0);
	return 0;
}


void CYbtxDlg::VerifyFiles()
{
	if (!m_bNeedMd5Verify)
	{

		m_pVerifyArgs->handler->VerifyFilesFinish(TRUE);
		return;
	}

	m_bNeedMd5Verify = false;
	
	m_InPatch = true;
	

	wstring strDir = m_StarterPath+ L"..\\..";
	wstring strCheckFile = strDir + L"\\ver.sys";
	
	m_pVerifyArgs->strDirectory = strDir;
	m_pVerifyArgs->strCheckCodeFileName = strCheckFile;

	unsigned int tid; // 标识线程的ID
	uintptr_t BTThread;
	BTThread = _beginthreadex(NULL, 0, &CYbtxDlg::StartVerifyFiles, m_pVerifyArgs, 0, &tid); // 第三个参数是创建线程运行的函数

	m_SetupProgress.ShowWindow(SW_SHOWNORMAL);
	m_DownProgress.ShowWindow(SW_HIDE);

}

unsigned int __stdcall CYbtxDlg::StartVerifyFiles(void* pClass)// MD5校验线程 
{
	VerifyArgs* args = static_cast<VerifyArgs*>(pClass);
	try
	{

		BOOL bVerifySuccess = TRUE;
		CVerifyClient vc;
		if (vc.CheckIntegrity(args->strCheckCodeFileName))
		{

			if (!vc.VerifyDirectory(args->handler, args->strDirectory, args->strCheckCodeFileName))
			{
				wstring strErr = vc.FetchErrorMsg();
				ShareStack::GetInst()->PushMsg(strErr.c_str());
				bVerifySuccess = FALSE;
			}
		}
		else
		{

			wstring strErr = L" ver.sys文件损坏\n";
			ShareStack::GetInst()->PushMsg(strErr.c_str());
			bVerifySuccess = FALSE;
		}


		args->handler->VerifyFilesFinish(bVerifySuccess);
	}
	catch(std::exception& exp)
	{
		::MessageBox(NULL, utf8_to_utf16(exp.what()).c_str(), L"校验文件时发生异常", MB_OK);
		exit(1);
	}
	catch(...)
	{
		::MessageBox(NULL, L"游戏客户端校验文件时发生错误", L"异常", MB_OK);
		exit(1);
	}
	return 0;

}

void CYbtxDlg::CreateInPatchFlag()
{
	ofstream ofsSelectedServer(INPATCH_FLAG);
	ofsSelectedServer.close();
}

BOOL CYbtxDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if ( pMsg->message == WM_KEYDOWN)
	{
		if( int(pMsg->wParam) == VK_ESCAPE || int(pMsg->wParam) == VK_RETURN )
			return TRUE;
	}
	return CDialog::PreTranslateMessage(pMsg);
}
