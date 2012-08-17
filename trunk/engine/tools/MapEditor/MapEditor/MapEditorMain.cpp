#include "stdafx.h"
#include "MainForm.h"
#include "CMapEditApp.h"
#include "IApplication.h"

using namespace System;
using namespace System::Windows::Forms;

using namespace sqr;

//地编 希望 界面和引擎争取做大最大限度的分离 

// 1 UI部分  (MapEditorForm)    在脱离引擎 也能够正常显示 只不过无法实现任何功能
// 2 引擎部分(MapEditorManage)  引擎初始化 地编环境初始化 功能类创建和初始化  
// 3 UI和功能类绑定部分
// 4 消息主框架 (CEditToolApp)

#include "CTerrainMgr.h"
#include "PathManager/CPathManager.h"

[STAThreadAttribute]
int WINAPI WinMain(HINSTANCE hinst,HINSTANCE hpre, LPSTR lpCmd, int nShowCmd)
{
	CTerrainMgr::st_isEditor = true;

	IApplication::InitApp();
	CUIBase::Initialize(IApplication::GetInst()->GetCtrlImpFactory());
	//创建主窗口
	MapEditor::MainForm^ MapEditForm = gcnew MapEditor::MainForm();

	MapEditForm->Show();

	while(!CPathSetImp::GetInst()->HasSetConfigPath())
	{
		CPathSetDlg m_PathSetDlg;
		m_PathSetDlg.ShowDialog();
	}
	//初始化引擎
	if(!CMapEditApp::HasInst())
		CMapEditApp::Create();

	IApplication::GetInst()->Run();
	
	if(CMapEditApp::HasInst())
		CMapEditApp::Destroy();

	return 0;
}


