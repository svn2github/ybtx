#include "StdAfx.h"
#include "TSqrAllocator.inl"
#include "MainForm.h"
#include "CEditContext.h"
#include "CtrlImpHelper.h"
#include "ContextCtrl/CContextCtrl.h"
#include "CMapContext.h"
#include "IApplication.h"
#include "CEditContext.h"
#include "TextureViewCtrl/TextureViewCtrlImp.h"
#include "SetViewCtrl/SetViewCtrlImp.h"
#include "TileViewCtrl/TileViewCtrlImp.h"
#include "BlockViewCtrl/BlockViewCtrlImp.h"
#include "GridCtrl/CGridCtrlImp.h"
#include "PropCtrl\CPropCtrlImp.h"
#include "CEditorDevConfig.h"
#include "EditorExpMgr.h"

//Dlg
#include "MapSetDlg.h"
#include "CMapEditApp.h"
#include "CMapMgr.h"

using namespace MapEditor;

MainForm::~MainForm()
{
	UnitExpHandler();

	if (components)
	{
		delete components;
	}
	IApplication::GetInst()->Close();
}

System::Void MainForm::m_FormTimer_Tick(System::Object^  sender, System::EventArgs^  e)
{
	if(this->m_PropShow)
	{
		this->m_PropShow->UpdateTimer();
	}
	if(this->cLightView1)
	{
		this->cLightView1->UpdateTimer();
	}
}

System::Void MainForm::UpdateTimer(System::Object^ sender,System::Timers::ElapsedEventArgs^ e)
{
}

System::Void MainForm::MainForm_Load(System::Object^  sender, System::EventArgs^  e) 
{
	InitExp();
	if( !CMapEditApp::HasInst() )
		CMapEditApp::Create();

	CMapEditApp::GetInst()->SetRenderScene(CtrlImpPtr<CContextCtrlImp>(MapEditorDescrip->GetCoreCtrl())->GetContext());

	//所有控制面板都选择一遍
	this->TabPanel->SelectedIndex = 0;
	this->TabPanel->SelectedIndex = 1;
	this->TabPanel->SelectedIndex = 2;
	this->TabPanel->SelectedIndex = 3;
	this->TabPanel->SelectedIndex = 7;
	this->TabPanel->SelectedIndex = 8;
	this->TabPanel->SelectedIndex = 10;

	this->m_bLoadFinish = true;

	CtrlImpPtr<CTileViewCtrlImp>(this->m_TileView->GetCoreCtrl())->
		SetSetViewCtrlObserver(CtrlImpPtr<CSetViewCtrlImp>(this->m_SetView->GetCoreCtrl()));

	CtrlImpPtr<CSetViewCtrlImp>(this->m_SetView->GetCoreCtrl())->
		SetTextureViewCtrlObserver(CtrlImpPtr<CTextureViewCtrlImp>(this->m_TexView->GetCoreCtrl()));

	string strRessetPath;
	CtrlImpPtr<CTileViewCtrlImp>(this->m_TileView->GetCoreCtrl())->GetResetResPath(strRessetPath);
	CtrlImpPtr<CSetViewCtrlImp>(this->m_SetView->GetCoreCtrl())->SetResSetResPath(strRessetPath);
}

System::Void MainForm::InitExp()
{
	bool bcoreDumpIsEnabled = false;
	bool bErrorMsgShow		= true;
	if( CEditorDevConfig::HasInst() )
	{
		bcoreDumpIsEnabled = CEditorDevConfig::GetInst()->GetCoreDumpIsEnabled();
		bErrorMsgShow      = CEditorDevConfig::GetInst()->GetErrMsgIsShow();
	}

	InitExpHandler("mapeditor", bcoreDumpIsEnabled, bErrorMsgShow);
}

System::Void MainForm::MainForm_FormClosed(System::Object^  sender, System::Windows::Forms::FormClosedEventArgs^  e) 
{
	CMsg exitMsg(MGS_SYSCOM,MGT_SYS_END,0);
	CMapEditApp::GetInst()->GetRenderScene()->EventCollect(exitMsg);
}

//控制面板
System::Void MainForm::TabPanel_Selected(System::Object^  sender, System::Windows::Forms::TabControlEventArgs^  e)
{
	if( !this->m_bLoadFinish )
		return;

	int nTabSelectIndex = this->TabPanel->SelectedIndex;
	switch ( nTabSelectIndex )
	{
	case 0:
		CtrlImpPtr<CGridCtrlImp>(this->m_GridView->GetCoreCtrl())->ActiveBrush();
		break;

	case 3:
		CtrlImpPtr<CTextureViewCtrlImp>(this->m_TexView->GetCoreCtrl())->ActiveBrush();
		break;

	case 8:
		CtrlImpPtr<CSetViewCtrlImp>(this->m_SetView->GetCoreCtrl())->ActiveBrush();
		break;

	case 10:
		CtrlImpPtr<CBlockViewCtrlImp>(this->m_BlockViewCtrl->GetCoreCtrl())->ActiveBrush();
		break;

	default:
		break;
	}

	if( nTabSelectIndex == 1)
	{
		this->m_RecordView->ActivePanel();
	}
}

//菜单 
System::Void MainForm::menuFileNew_Click(System::Object^  sender, System::EventArgs^  e)
{
	MapSetDlg dlg;
	dlg.ExMapCtrl->Enabled=false;
	dlg.MapCtrl->Enabled=true;
	dlg.ShowDialog();
}

System::Void MainForm::menuFileMapEx_Click(System::Object^  sender, System::EventArgs^  e)
{
	MapSetDlg dlg;
	dlg.ExMapCtrl->Enabled=true;
	dlg.MapCtrl->Enabled=false;
	dlg.ShowDialog();
}

System::Void MainForm::menuFileOpen_Click(System::Object^  sender, System::EventArgs^  e)
{
	OpenFileDialog^ ofd=gcnew OpenFileDialog();
	ofd->Filter="ProjFile(*.project)|*.project";
	if(ofd->ShowDialog()==System::Windows::Forms::DialogResult::OK)
	{
		System::IntPtr filename=System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(ofd->FileName);
		CMapEditApp::GetInst()->GetMapMgr()->LoadOldMap((void*)filename);
	}
}

System::Void MainForm::menuFileSave_Click(System::Object^  sender, System::EventArgs^  e)
{
	SaveFileDialog^ ofd = gcnew SaveFileDialog();
	ofd->Filter="ProjFile(*.project)|*.project";
	if(ofd->ShowDialog()==System::Windows::Forms::DialogResult::OK)
	{
		System::IntPtr filename=System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(ofd->FileName);
		CMapEditApp::GetInst()->GetMapMgr()->SaveOldMap((void*)filename);
	}
}

System::Void MainForm::menuFileSaveOther_Click(System::Object^  sender, System::EventArgs^  e)
{
}

