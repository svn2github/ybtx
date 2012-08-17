// MapEditor.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "MapEditor.h"
#include "MainFrm.h"

#include "MapEditorDoc.h"
#include "MapEditorView.h"
#include <Engine.h>
#include <SharedTexture.h>
#include <CTileEditApp.h>
#include <WireBox.h>
#include "NewMapDialog.h"
#include ".\mapeditor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMapEditorApp

BEGIN_MESSAGE_MAP(CMapEditorApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	// 基于文件的标准文档命令
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
END_MESSAGE_MAP()

// CMapEditorApp 构造

CMapEditorApp::CMapEditorApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}

CMapEditorApp::~CMapEditorApp()
{
	eg::CEngine::GetInstance()->Destroy();
}

// 唯一的一个 CMapEditorApp 对象

CMapEditorApp theApp;

// CMapEditorApp 初始化

BOOL CMapEditorApp::InitInstance()
{
	std::locale::global(std::locale(""));
	::SetCurrentDirectory("..\\..\\data");

	CWinApp::InitInstance();

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));
	LoadStdProfileSettings(4);  // 加载标准 INI 文件选项(包括 MRU)
	// 注册应用程序的文档模板。文档模板
	// 将用作文档、框架窗口和视图之间的连接
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CMapEditorDoc),
		RUNTIME_CLASS(CMainFrame),       // 主 SDI 框架窗口
		RUNTIME_CLASS(CMapEditorView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);
	// 分析标准外壳命令、DDE、打开文件操作的命令行
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
	// 调度在命令行中指定的命令。如果
	// 用 /RegServer、/Register、/Unregserver 或 /Unregister 启动应用程序，则返回 FALSE。
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;
	// 唯一的一个窗口已初始化，因此显示它并对其进行更新
	m_pMainWnd->ShowWindow(SW_SHOWMAXIMIZED);
	m_pMainWnd->UpdateWindow();
//	m_pMainWnd->SetWindowText("Tang Map Editor Version .01 Powered by Bluely Engine");
	// 仅当存在后缀时才调用 DragAcceptFiles，
	//  在 SDI 应用程序中，这应在 ProcessShellCommand  之后发生
	
	/////////////////////////////////////////////////////////////////////////
	eg::CEngine::GetInstance()->Create(((CMainFrame*)m_pMainWnd)->GetWorldView()->GetSafeHwnd(), m_pMainWnd->GetSafeHwnd(), ((CMainFrame*)m_pMainWnd)->GetBackBufferRect(),false,false);
	
	bly::CCamera * pCamera;
	bly::COperator * pOperator;
		
	// camera must be first
	pCamera = CSceneManager::GetInstance()->AddNodeCamera(STDFOV, STDPITCH, STDYAW, 50,10000,D3DXVECTOR3(2880.0f,2880.0f,0.0f), D3DXVECTOR3(0.0f,0.0f,480.0f));
	//pCamera->MoveToRTSByView(D3DXVECTOR3(REGION_SPACE/2.0f*64.0f,0.0f,REGION_SPACE/2.0f*64.0f));
	eg::CEngine::GetInstance()->SetActiveCamera(pCamera);
	
	// operator
	pOperator = CSceneManager::GetInstance()->AddNodeOperator(
		((CMainFrame*)(m_pMainWnd))->GetResSetView()->GetSafeHwnd());
	
	// XFile
	CTileEditApp::GetInstance()->SetOperator(pOperator);

	((CMainFrame*)m_pMainWnd)->InitMapEditor(1,1);
		
	((CMainFrame*)m_pMainWnd)->UpdateCamera();
	
	((CMainFrame*)m_pMainWnd)->GetResSetView()->AddEffectSetFromFile("..\\..\\data\\fx\\setting\\mapfx_local.are");
	((CMainFrame*)m_pMainWnd)->GetResSetView()->LoadLog();
			
	return TRUE;
}



// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnViewStatusBar();
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

// 用于运行对话框的应用程序命令
void CMapEditorApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CMapEditorApp 消息处理程序

void CMapEditorApp::BackToWorldView()
{
	((CMainFrame*)m_pMainWnd)->GetWorldView()->SetFocus();
}

BOOL CMapEditorApp::OnIdle(LONG lCount)
{
	if( ((CMainFrame*)m_pMainWnd)->GetWorldView() )
	{
		if ( !ut::CTimer::GetInstance()->IsStop() )
		{
			while ( ut::CTimer::GetInstance()->IsOverEF() ) 
			{
				this->Update();
				ut::CTimer::GetInstance()->Down();
			}
			ut::CFPSCounter::GetInstance()->RegisterFPS();
			this->Render();
			ut::CTimer::GetInstance()->Clear();
		}
	}
	CWinApp::OnIdle( lCount );
	return 1;
}

void CMapEditorApp::Render()
{
	eg::CEngine::GetInstance()->Render();
}

void CMapEditorApp::Update()
{
	eg::CEngine::GetInstance()->Update();
}