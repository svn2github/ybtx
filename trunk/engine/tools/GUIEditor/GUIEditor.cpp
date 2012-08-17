// GUIEditor.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "GUIEditor.h"
#include "GUIEditorDlg.h"
#include ".\guieditor.h"
#include "PathBrowser\CEditorConfig.h"
//#include "../src/gui/SQRGUIManager.h"

const wstring PATH_ALIAS_GUIEDITOR = L"";

BEGIN_MESSAGE_MAP(CGUIEditorApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// CGUIEditorApp 构造

CGUIEditorApp::CGUIEditorApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
	EnableCatchError(true);
}


// 唯一的一个 CGUIEditorApp 对象

CGUIEditorApp theApp;


// CGUIEditorApp 初始化

BOOL CGUIEditorApp::InitInstance()
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControls()。否则，将无法创建窗口。
	InitCommonControls();

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));

	//m_MainDlg.Create(CGUIEditorDlg::GetDialogID(), CWnd::GetDesktopWindow() );
	CGUIEditorDlg* pDlg = new CGUIEditorDlg();
	pDlg->Create( IDD_GUIEDITOR_DIALOG_1024, ::CWnd::GetDesktopWindow() );//屏蔽掉选取1024或800对话框
	m_pMainWnd = pDlg;

	::SetCursor( ::LoadCursor( NULL, IDC_ARROW ) );
	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	// 而不是启动应用程序的消息泵。
	return TRUE;
}

//------------------------------------------------------------------------------
CGUIEditorApp::~CGUIEditorApp()
{
	SafeDelete(m_pMainWnd);
}