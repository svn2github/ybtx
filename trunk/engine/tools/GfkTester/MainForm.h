#pragma once
namespace sqr_tools
{
	class CBaseUnit;
}
class TiXmlDocument;

namespace GraphicTester {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace sqr_tools;

	/// <summary>
	/// Summary for Form1
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	class MainFormImp;
	public ref class MainForm : public System::Windows::Forms::Form
	{
	public:
		MainForm(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~MainForm()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::MenuStrip^  MainMenu;
	private: System::Windows::Forms::ToolStripMenuItem^  TestUnit;
	protected: 
	private: System::Windows::Forms::StatusStrip^  StatusBar;
	private: System::Windows::Forms::ToolStrip^  toolStrip1;
	private: System::Windows::Forms::ToolStripMenuItem^  Exit;
	private: System::Windows::Forms::SplitContainer^  splitContainer1;
	private: System::Windows::Forms::SplitContainer^  splitContainer2;

	private: System::Windows::Forms::TabControl^  TabCtrl;
	private: System::Windows::Forms::TabPage^  SystemPad;

	private: System::Windows::Forms::ToolStripMenuItem^  SettingItem;
	private: System::Windows::Forms::ToolStripMenuItem^  SetPathBrowser;


	private: System::Windows::Forms::TabControl^  SceneTab;
	private: System::Windows::Forms::TabPage^  TestPad;
	private: System::Windows::Forms::TabPage^  CtrlTab;

	private: MainFormImp*							m_Imp;
	private: System::Windows::Forms::ToolStrip^  toolStrip2;
	private: System::Windows::Forms::ToolStripComboBox^  TestUnitSelector;
	private: System::Windows::Forms::ToolStripButton^  AddNewTestUnit;
	private: System::Windows::Forms::CheckedListBox^  TestUnitList;
	private: int m_UnitCont;



	private: System::ComponentModel::IContainer^  components;
	private: System::Windows::Forms::ToolStripSeparator^  toolStripSeparator1;
	private: System::Windows::Forms::ToolStripButton^  UnitDelete;
	private: System::Windows::Forms::ToolStripSeparator^  toolStripSeparator2;
	private: System::Windows::Forms::ToolStripButton^  SaveProj;

	private: sqr_tools::CViewCtrl^  MainView;
	private: System::Windows::Forms::PropertyGrid^  propertyGrid1;
	private: System::Windows::Forms::TabPage^  UnitEditTab;
	private: System::Windows::Forms::GroupBox^  TestUnitTitle;

	private: sqr_tools::AutoPropToCtrl^  AutoUnitEditor;
	private: sqr_tools::CContextComponent^  TestMainContext;
	private: sqr_tools::CRTSCameraCom^  RTSCamera;
	private: sqr_tools::CLogCtrl^  SystemLog;
	private: System::Windows::Forms::TableLayoutPanel^  tableLayoutPanel1;
	private: System::Windows::Forms::Button^  UnitSave;
	private: System::Windows::Forms::Button^  UnitLoad;
	private: System::Windows::Forms::OpenFileDialog^  openTestUnit;
	private: System::Windows::Forms::SaveFileDialog^  saveTestUnit;
	private: System::Windows::Forms::ToolStripSeparator^  toolStripSeparator3;
	private: System::Windows::Forms::ToolStripButton^  LoadProj;
	private: System::Windows::Forms::ToolStripSeparator^  toolStripSeparator4;
	private: System::Windows::Forms::ToolStripButton^  TestUnitClear;
	private: System::Windows::Forms::ToolStripMenuItem^  NewTestProj;
	private: System::Windows::Forms::ToolStripMenuItem^  SaveTestProj;


	private: System::Windows::Forms::ToolStripSeparator^  toolStripSeparator6;
	private: System::Windows::Forms::ToolStripMenuItem^  OpenTestProj;
	private: System::Windows::Forms::ToolStripMenuItem^  ImportTestProj;


	private: System::Windows::Forms::ToolStripSeparator^  toolStripSeparator5;

	private: MainFormImp*							m_pImp;


			 /// <summary>
		/// Required designer variable.
		/// </summary>


#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(MainForm::typeid));
			this->MainMenu = (gcnew System::Windows::Forms::MenuStrip());
			this->TestUnit = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->NewTestProj = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->SaveTestProj = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripSeparator6 = (gcnew System::Windows::Forms::ToolStripSeparator());
			this->OpenTestProj = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->ImportTestProj = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripSeparator5 = (gcnew System::Windows::Forms::ToolStripSeparator());
			this->Exit = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->SettingItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->SetPathBrowser = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->StatusBar = (gcnew System::Windows::Forms::StatusStrip());
			this->toolStrip1 = (gcnew System::Windows::Forms::ToolStrip());
			this->splitContainer1 = (gcnew System::Windows::Forms::SplitContainer());
			this->splitContainer2 = (gcnew System::Windows::Forms::SplitContainer());
			this->MainView = (gcnew sqr_tools::CViewCtrl());
			this->TestMainContext = (gcnew sqr_tools::CContextComponent());
			this->SceneTab = (gcnew System::Windows::Forms::TabControl());
			this->CtrlTab = (gcnew System::Windows::Forms::TabPage());
			this->TestUnitList = (gcnew System::Windows::Forms::CheckedListBox());
			this->toolStrip2 = (gcnew System::Windows::Forms::ToolStrip());
			this->TestUnitSelector = (gcnew System::Windows::Forms::ToolStripComboBox());
			this->AddNewTestUnit = (gcnew System::Windows::Forms::ToolStripButton());
			this->toolStripSeparator1 = (gcnew System::Windows::Forms::ToolStripSeparator());
			this->UnitDelete = (gcnew System::Windows::Forms::ToolStripButton());
			this->toolStripSeparator2 = (gcnew System::Windows::Forms::ToolStripSeparator());
			this->LoadProj = (gcnew System::Windows::Forms::ToolStripButton());
			this->toolStripSeparator3 = (gcnew System::Windows::Forms::ToolStripSeparator());
			this->SaveProj = (gcnew System::Windows::Forms::ToolStripButton());
			this->toolStripSeparator4 = (gcnew System::Windows::Forms::ToolStripSeparator());
			this->TestUnitClear = (gcnew System::Windows::Forms::ToolStripButton());
			this->UnitEditTab = (gcnew System::Windows::Forms::TabPage());
			this->TestUnitTitle = (gcnew System::Windows::Forms::GroupBox());
			this->tableLayoutPanel1 = (gcnew System::Windows::Forms::TableLayoutPanel());
			this->UnitSave = (gcnew System::Windows::Forms::Button());
			this->UnitLoad = (gcnew System::Windows::Forms::Button());
			this->AutoUnitEditor = (gcnew sqr_tools::AutoPropToCtrl());
			this->TestPad = (gcnew System::Windows::Forms::TabPage());
			this->propertyGrid1 = (gcnew System::Windows::Forms::PropertyGrid());
			this->TabCtrl = (gcnew System::Windows::Forms::TabControl());
			this->SystemPad = (gcnew System::Windows::Forms::TabPage());
			this->SystemLog = (gcnew sqr_tools::CLogCtrl());
			this->openTestUnit = (gcnew System::Windows::Forms::OpenFileDialog());
			this->saveTestUnit = (gcnew System::Windows::Forms::SaveFileDialog());
			this->RTSCamera = (gcnew sqr_tools::CRTSCameraCom());
			this->MainMenu->SuspendLayout();
			this->splitContainer1->Panel1->SuspendLayout();
			this->splitContainer1->Panel2->SuspendLayout();
			this->splitContainer1->SuspendLayout();
			this->splitContainer2->Panel1->SuspendLayout();
			this->splitContainer2->Panel2->SuspendLayout();
			this->splitContainer2->SuspendLayout();
			this->SceneTab->SuspendLayout();
			this->CtrlTab->SuspendLayout();
			this->toolStrip2->SuspendLayout();
			this->UnitEditTab->SuspendLayout();
			this->TestUnitTitle->SuspendLayout();
			this->tableLayoutPanel1->SuspendLayout();
			this->TestPad->SuspendLayout();
			this->TabCtrl->SuspendLayout();
			this->SystemPad->SuspendLayout();
			this->SuspendLayout();
			// 
			// MainMenu
			// 
			this->MainMenu->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(2) {this->TestUnit, this->SettingItem});
			this->MainMenu->Location = System::Drawing::Point(0, 0);
			this->MainMenu->Name = L"MainMenu";
			this->MainMenu->Size = System::Drawing::Size(917, 25);
			this->MainMenu->TabIndex = 1;
			this->MainMenu->Text = L"menuStrip1";
			// 
			// TestUnit
			// 
			this->TestUnit->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(7) {this->NewTestProj, 
				this->SaveTestProj, this->toolStripSeparator6, this->OpenTestProj, this->ImportTestProj, this->toolStripSeparator5, this->Exit});
			this->TestUnit->Name = L"TestUnit";
			this->TestUnit->Size = System::Drawing::Size(68, 21);
			this->TestUnit->Text = L"测试用例";
			// 
			// NewTestProj
			// 
			this->NewTestProj->Name = L"NewTestProj";
			this->NewTestProj->Size = System::Drawing::Size(100, 22);
			this->NewTestProj->Text = L"新建";
			this->NewTestProj->Click += gcnew System::EventHandler(this, &MainForm::TestUnitClear_Click);
			// 
			// SaveTestProj
			// 
			this->SaveTestProj->Name = L"SaveTestProj";
			this->SaveTestProj->Size = System::Drawing::Size(100, 22);
			this->SaveTestProj->Text = L"保存";
			this->SaveTestProj->Click += gcnew System::EventHandler(this, &MainForm::SaveProj_Click);
			// 
			// toolStripSeparator6
			// 
			this->toolStripSeparator6->Name = L"toolStripSeparator6";
			this->toolStripSeparator6->Size = System::Drawing::Size(97, 6);
			// 
			// OpenTestProj
			// 
			this->OpenTestProj->Name = L"OpenTestProj";
			this->OpenTestProj->Size = System::Drawing::Size(100, 22);
			this->OpenTestProj->Text = L"打开";
			this->OpenTestProj->Click += gcnew System::EventHandler(this, &MainForm::LoadProj_Click);
			// 
			// ImportTestProj
			// 
			this->ImportTestProj->Name = L"ImportTestProj";
			this->ImportTestProj->Size = System::Drawing::Size(100, 22);
			this->ImportTestProj->Text = L"导入";
			this->ImportTestProj->Click += gcnew System::EventHandler(this, &MainForm::ImportTestProj_Click);
			// 
			// toolStripSeparator5
			// 
			this->toolStripSeparator5->Name = L"toolStripSeparator5";
			this->toolStripSeparator5->Size = System::Drawing::Size(97, 6);
			// 
			// Exit
			// 
			this->Exit->Name = L"Exit";
			this->Exit->Size = System::Drawing::Size(100, 22);
			this->Exit->Text = L"退出";
			this->Exit->Click += gcnew System::EventHandler(this, &MainForm::ExitMenuItem_Click);
			// 
			// SettingItem
			// 
			this->SettingItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) {this->SetPathBrowser});
			this->SettingItem->Name = L"SettingItem";
			this->SettingItem->Size = System::Drawing::Size(44, 21);
			this->SettingItem->Text = L"设置";
			// 
			// SetPathBrowser
			// 
			this->SetPathBrowser->Name = L"SetPathBrowser";
			this->SetPathBrowser->Size = System::Drawing::Size(148, 22);
			this->SetPathBrowser->Text = L"设置工作路径";
			this->SetPathBrowser->Click += gcnew System::EventHandler(this, &MainForm::SetPathBrowser_Click);
			// 
			// StatusBar
			// 
			this->StatusBar->Location = System::Drawing::Point(0, 491);
			this->StatusBar->Name = L"StatusBar";
			this->StatusBar->Size = System::Drawing::Size(917, 22);
			this->StatusBar->TabIndex = 2;
			this->StatusBar->Text = L"statusStrip1";
			// 
			// toolStrip1
			// 
			this->toolStrip1->Location = System::Drawing::Point(0, 25);
			this->toolStrip1->Name = L"toolStrip1";
			this->toolStrip1->Size = System::Drawing::Size(917, 25);
			this->toolStrip1->TabIndex = 3;
			this->toolStrip1->Text = L"toolStrip1";
			// 
			// splitContainer1
			// 
			this->splitContainer1->Dock = System::Windows::Forms::DockStyle::Fill;
			this->splitContainer1->FixedPanel = System::Windows::Forms::FixedPanel::Panel2;
			this->splitContainer1->Location = System::Drawing::Point(0, 50);
			this->splitContainer1->Name = L"splitContainer1";
			this->splitContainer1->Orientation = System::Windows::Forms::Orientation::Horizontal;
			// 
			// splitContainer1.Panel1
			// 
			this->splitContainer1->Panel1->Controls->Add(this->splitContainer2);
			// 
			// splitContainer1.Panel2
			// 
			this->splitContainer1->Panel2->Controls->Add(this->TabCtrl);
			this->splitContainer1->Size = System::Drawing::Size(917, 441);
			this->splitContainer1->SplitterDistance = 282;
			this->splitContainer1->TabIndex = 4;
			// 
			// splitContainer2
			// 
			this->splitContainer2->Dock = System::Windows::Forms::DockStyle::Fill;
			this->splitContainer2->FixedPanel = System::Windows::Forms::FixedPanel::Panel2;
			this->splitContainer2->Location = System::Drawing::Point(0, 0);
			this->splitContainer2->Name = L"splitContainer2";
			// 
			// splitContainer2.Panel1
			// 
			this->splitContainer2->Panel1->Controls->Add(this->MainView);
			// 
			// splitContainer2.Panel2
			// 
			this->splitContainer2->Panel2->Controls->Add(this->SceneTab);
			this->splitContainer2->Size = System::Drawing::Size(917, 282);
			this->splitContainer2->SplitterDistance = 597;
			this->splitContainer2->TabIndex = 0;
			// 
			// MainView
			// 
			this->MainView->BackColor = System::Drawing::Color::Gray;
			this->MainView->ContextComponent = this->TestMainContext;
			this->MainView->Dock = System::Windows::Forms::DockStyle::Fill;
			this->MainView->Location = System::Drawing::Point(0, 0);
			this->MainView->Name = L"MainView";
			this->MainView->Size = System::Drawing::Size(597, 282);
			this->MainView->TabIndex = 1;
			// 
			// TestMainContext
			// 
			this->TestMainContext->AmbientColor = System::Drawing::Color::Empty;
			this->TestMainContext->SceneName = L"H:\\ybtx\\artist\\resbin\\map\\bsdt04_c.arg";
			this->TestMainContext->ShowBlock = false;
			this->TestMainContext->ShowGrid = false;
			this->TestMainContext->SunColor = System::Drawing::Color::Empty;
			// 
			// SceneTab
			// 
			this->SceneTab->Alignment = System::Windows::Forms::TabAlignment::Bottom;
			this->SceneTab->Controls->Add(this->CtrlTab);
			this->SceneTab->Controls->Add(this->UnitEditTab);
			this->SceneTab->Controls->Add(this->TestPad);
			this->SceneTab->Dock = System::Windows::Forms::DockStyle::Fill;
			this->SceneTab->Location = System::Drawing::Point(0, 0);
			this->SceneTab->Name = L"SceneTab";
			this->SceneTab->SelectedIndex = 0;
			this->SceneTab->Size = System::Drawing::Size(316, 282);
			this->SceneTab->TabIndex = 0;
			// 
			// CtrlTab
			// 
			this->CtrlTab->Controls->Add(this->TestUnitList);
			this->CtrlTab->Controls->Add(this->toolStrip2);
			this->CtrlTab->Location = System::Drawing::Point(4, 4);
			this->CtrlTab->Name = L"CtrlTab";
			this->CtrlTab->Padding = System::Windows::Forms::Padding(3);
			this->CtrlTab->Size = System::Drawing::Size(308, 256);
			this->CtrlTab->TabIndex = 1;
			this->CtrlTab->Text = L"测试用例";
			this->CtrlTab->UseVisualStyleBackColor = true;
			this->CtrlTab->Validated += gcnew System::EventHandler(this, &MainForm::CtrlTab_Validated);
			this->CtrlTab->Enter += gcnew System::EventHandler(this, &MainForm::CtrlTab_Enter);
			// 
			// TestUnitList
			// 
			this->TestUnitList->Dock = System::Windows::Forms::DockStyle::Fill;
			this->TestUnitList->FormattingEnabled = true;
			this->TestUnitList->Location = System::Drawing::Point(3, 28);
			this->TestUnitList->Name = L"TestUnitList";
			this->TestUnitList->ScrollAlwaysVisible = true;
			this->TestUnitList->Size = System::Drawing::Size(302, 212);
			this->TestUnitList->TabIndex = 1;
			this->TestUnitList->ItemCheck += gcnew System::Windows::Forms::ItemCheckEventHandler(this, &MainForm::TestUnitList_ItemCheck);
			// 
			// toolStrip2
			// 
			this->toolStrip2->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(10) {this->TestUnitSelector, 
				this->AddNewTestUnit, this->toolStripSeparator1, this->UnitDelete, this->toolStripSeparator2, this->LoadProj, this->toolStripSeparator3, 
				this->SaveProj, this->toolStripSeparator4, this->TestUnitClear});
			this->toolStrip2->LayoutStyle = System::Windows::Forms::ToolStripLayoutStyle::HorizontalStackWithOverflow;
			this->toolStrip2->Location = System::Drawing::Point(3, 3);
			this->toolStrip2->Name = L"toolStrip2";
			this->toolStrip2->Size = System::Drawing::Size(302, 25);
			this->toolStrip2->TabIndex = 0;
			this->toolStrip2->Text = L"toolStrip2";
			// 
			// TestUnitSelector
			// 
			this->TestUnitSelector->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->TestUnitSelector->MergeAction = System::Windows::Forms::MergeAction::MatchOnly;
			this->TestUnitSelector->MergeIndex = 0;
			this->TestUnitSelector->Name = L"TestUnitSelector";
			this->TestUnitSelector->Size = System::Drawing::Size(121, 25);
			// 
			// AddNewTestUnit
			// 
			this->AddNewTestUnit->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Text;
			this->AddNewTestUnit->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"AddNewTestUnit.Image")));
			this->AddNewTestUnit->ImageAlign = System::Drawing::ContentAlignment::TopRight;
			this->AddNewTestUnit->ImageTransparentColor = System::Drawing::Color::Magenta;
			this->AddNewTestUnit->Name = L"AddNewTestUnit";
			this->AddNewTestUnit->Overflow = System::Windows::Forms::ToolStripItemOverflow::Never;
			this->AddNewTestUnit->Size = System::Drawing::Size(36, 22);
			this->AddNewTestUnit->Text = L"新建";
			this->AddNewTestUnit->Click += gcnew System::EventHandler(this, &MainForm::AddNewTestUnit_Click);
			// 
			// toolStripSeparator1
			// 
			this->toolStripSeparator1->Name = L"toolStripSeparator1";
			this->toolStripSeparator1->Size = System::Drawing::Size(6, 25);
			// 
			// UnitDelete
			// 
			this->UnitDelete->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Text;
			this->UnitDelete->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"UnitDelete.Image")));
			this->UnitDelete->ImageTransparentColor = System::Drawing::Color::Magenta;
			this->UnitDelete->Name = L"UnitDelete";
			this->UnitDelete->Size = System::Drawing::Size(36, 22);
			this->UnitDelete->Text = L"删除";
			this->UnitDelete->Click += gcnew System::EventHandler(this, &MainForm::UnitDelete_Click);
			// 
			// toolStripSeparator2
			// 
			this->toolStripSeparator2->Name = L"toolStripSeparator2";
			this->toolStripSeparator2->Size = System::Drawing::Size(6, 25);
			// 
			// LoadProj
			// 
			this->LoadProj->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Text;
			this->LoadProj->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"LoadProj.Image")));
			this->LoadProj->ImageTransparentColor = System::Drawing::Color::Magenta;
			this->LoadProj->Name = L"LoadProj";
			this->LoadProj->Size = System::Drawing::Size(36, 22);
			this->LoadProj->Text = L"载入";
			this->LoadProj->Click += gcnew System::EventHandler(this, &MainForm::LoadProj_Click);
			// 
			// toolStripSeparator3
			// 
			this->toolStripSeparator3->Name = L"toolStripSeparator3";
			this->toolStripSeparator3->Size = System::Drawing::Size(6, 25);
			// 
			// SaveProj
			// 
			this->SaveProj->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Text;
			this->SaveProj->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"SaveProj.Image")));
			this->SaveProj->ImageTransparentColor = System::Drawing::Color::Magenta;
			this->SaveProj->Name = L"SaveProj";
			this->SaveProj->Size = System::Drawing::Size(36, 21);
			this->SaveProj->Text = L"保存";
			this->SaveProj->Click += gcnew System::EventHandler(this, &MainForm::SaveProj_Click);
			// 
			// toolStripSeparator4
			// 
			this->toolStripSeparator4->Name = L"toolStripSeparator4";
			this->toolStripSeparator4->Size = System::Drawing::Size(6, 25);
			// 
			// TestUnitClear
			// 
			this->TestUnitClear->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Text;
			this->TestUnitClear->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"TestUnitClear.Image")));
			this->TestUnitClear->ImageTransparentColor = System::Drawing::Color::Magenta;
			this->TestUnitClear->Name = L"TestUnitClear";
			this->TestUnitClear->Size = System::Drawing::Size(36, 21);
			this->TestUnitClear->Text = L"清空";
			this->TestUnitClear->Click += gcnew System::EventHandler(this, &MainForm::TestUnitClear_Click);
			// 
			// UnitEditTab
			// 
			this->UnitEditTab->Controls->Add(this->TestUnitTitle);
			this->UnitEditTab->Location = System::Drawing::Point(4, 4);
			this->UnitEditTab->Name = L"UnitEditTab";
			this->UnitEditTab->Padding = System::Windows::Forms::Padding(3);
			this->UnitEditTab->Size = System::Drawing::Size(308, 256);
			this->UnitEditTab->TabIndex = 2;
			this->UnitEditTab->Text = L"编辑用例";
			this->UnitEditTab->UseVisualStyleBackColor = true;
			this->UnitEditTab->Layout += gcnew System::Windows::Forms::LayoutEventHandler(this, &MainForm::UnitEditTab_Layout);
			this->UnitEditTab->Enter += gcnew System::EventHandler(this, &MainForm::UnitEditTab_Enter);
			// 
			// TestUnitTitle
			// 
			this->TestUnitTitle->Controls->Add(this->tableLayoutPanel1);
			this->TestUnitTitle->Controls->Add(this->AutoUnitEditor);
			this->TestUnitTitle->Dock = System::Windows::Forms::DockStyle::Fill;
			this->TestUnitTitle->Location = System::Drawing::Point(3, 3);
			this->TestUnitTitle->Name = L"TestUnitTitle";
			this->TestUnitTitle->Size = System::Drawing::Size(302, 250);
			this->TestUnitTitle->TabIndex = 0;
			this->TestUnitTitle->TabStop = false;
			this->TestUnitTitle->Text = L"UnitEidt";
			// 
			// tableLayoutPanel1
			// 
			this->tableLayoutPanel1->ColumnCount = 2;
			this->tableLayoutPanel1->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent, 
				50)));
			this->tableLayoutPanel1->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent, 
				50)));
			this->tableLayoutPanel1->Controls->Add(this->UnitSave, 0, 0);
			this->tableLayoutPanel1->Controls->Add(this->UnitLoad, 1, 0);
			this->tableLayoutPanel1->Dock = System::Windows::Forms::DockStyle::Bottom;
			this->tableLayoutPanel1->Location = System::Drawing::Point(3, 211);
			this->tableLayoutPanel1->Name = L"tableLayoutPanel1";
			this->tableLayoutPanel1->RowCount = 1;
			this->tableLayoutPanel1->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 50)));
			this->tableLayoutPanel1->Size = System::Drawing::Size(296, 36);
			this->tableLayoutPanel1->TabIndex = 2;
			// 
			// UnitSave
			// 
			this->UnitSave->Dock = System::Windows::Forms::DockStyle::Fill;
			this->UnitSave->Location = System::Drawing::Point(3, 3);
			this->UnitSave->Name = L"UnitSave";
			this->UnitSave->Size = System::Drawing::Size(142, 30);
			this->UnitSave->TabIndex = 2;
			this->UnitSave->Text = L"保存成文件";
			this->UnitSave->UseVisualStyleBackColor = true;
			this->UnitSave->Click += gcnew System::EventHandler(this, &MainForm::UnitSave_Click);
			// 
			// UnitLoad
			// 
			this->UnitLoad->Dock = System::Windows::Forms::DockStyle::Fill;
			this->UnitLoad->Location = System::Drawing::Point(151, 3);
			this->UnitLoad->Name = L"UnitLoad";
			this->UnitLoad->Size = System::Drawing::Size(142, 30);
			this->UnitLoad->TabIndex = 3;
			this->UnitLoad->Text = L"从文件载入";
			this->UnitLoad->UseVisualStyleBackColor = true;
			this->UnitLoad->Click += gcnew System::EventHandler(this, &MainForm::UnitLoad_Click);
			// 
			// AutoUnitEditor
			// 
			this->AutoUnitEditor->AutoScroll = true;
			this->AutoUnitEditor->BackColor = System::Drawing::SystemColors::Control;
			this->AutoUnitEditor->CausesValidation = false;
			this->AutoUnitEditor->ContextComponent = nullptr;
			this->AutoUnitEditor->Dock = System::Windows::Forms::DockStyle::Fill;
			this->AutoUnitEditor->Location = System::Drawing::Point(3, 17);
			this->AutoUnitEditor->Name = L"AutoUnitEditor";
			this->AutoUnitEditor->Size = System::Drawing::Size(296, 230);
			this->AutoUnitEditor->TabIndex = 0;
			// 
			// TestPad
			// 
			this->TestPad->Controls->Add(this->propertyGrid1);
			this->TestPad->Location = System::Drawing::Point(4, 4);
			this->TestPad->Name = L"TestPad";
			this->TestPad->Size = System::Drawing::Size(308, 256);
			this->TestPad->TabIndex = 0;
			this->TestPad->Text = L"场景属性";
			this->TestPad->UseVisualStyleBackColor = true;
			// 
			// propertyGrid1
			// 
			this->propertyGrid1->Dock = System::Windows::Forms::DockStyle::Fill;
			this->propertyGrid1->Location = System::Drawing::Point(0, 0);
			this->propertyGrid1->Name = L"propertyGrid1";
			this->propertyGrid1->SelectedObject = this->TestMainContext;
			this->propertyGrid1->Size = System::Drawing::Size(308, 256);
			this->propertyGrid1->TabIndex = 1;
			// 
			// TabCtrl
			// 
			this->TabCtrl->Alignment = System::Windows::Forms::TabAlignment::Bottom;
			this->TabCtrl->Controls->Add(this->SystemPad);
			this->TabCtrl->Dock = System::Windows::Forms::DockStyle::Fill;
			this->TabCtrl->Location = System::Drawing::Point(0, 0);
			this->TabCtrl->Name = L"TabCtrl";
			this->TabCtrl->SelectedIndex = 0;
			this->TabCtrl->Size = System::Drawing::Size(917, 155);
			this->TabCtrl->TabIndex = 0;
			// 
			// SystemPad
			// 
			this->SystemPad->Controls->Add(this->SystemLog);
			this->SystemPad->Location = System::Drawing::Point(4, 4);
			this->SystemPad->Name = L"SystemPad";
			this->SystemPad->Padding = System::Windows::Forms::Padding(3);
			this->SystemPad->Size = System::Drawing::Size(909, 129);
			this->SystemPad->TabIndex = 0;
			this->SystemPad->Text = L"系统消息";
			this->SystemPad->UseVisualStyleBackColor = true;
			// 
			// SystemLog
			// 
			this->SystemLog->ContextComponent = this->TestMainContext;
			this->SystemLog->Dock = System::Windows::Forms::DockStyle::Fill;
			this->SystemLog->Location = System::Drawing::Point(3, 3);
			this->SystemLog->LogType = sqr_tools::ELogType::PUBLIC_LOG;
			this->SystemLog->Name = L"SystemLog";
			this->SystemLog->Size = System::Drawing::Size(903, 123);
			this->SystemLog->TabIndex = 0;
			// 
			// openTestUnit
			// 
			this->openTestUnit->FileName = L"openFileDialog1";
			// 
			// RTSCamera
			// 
			this->RTSCamera->ContextComponent = this->TestMainContext;
			// 
			// MainForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(917, 513);
			this->Controls->Add(this->splitContainer1);
			this->Controls->Add(this->toolStrip1);
			this->Controls->Add(this->StatusBar);
			this->Controls->Add(this->MainMenu);
			this->MainMenuStrip = this->MainMenu;
			this->Name = L"MainForm";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = L"压力测试工具";
			this->WindowState = System::Windows::Forms::FormWindowState::Maximized;
			this->Load += gcnew System::EventHandler(this, &MainForm::MainForm_Load);
			this->FormClosed += gcnew System::Windows::Forms::FormClosedEventHandler(this, &MainForm::MainForm_FormClosed);
			this->MainMenu->ResumeLayout(false);
			this->MainMenu->PerformLayout();
			this->splitContainer1->Panel1->ResumeLayout(false);
			this->splitContainer1->Panel2->ResumeLayout(false);
			this->splitContainer1->ResumeLayout(false);
			this->splitContainer2->Panel1->ResumeLayout(false);
			this->splitContainer2->Panel2->ResumeLayout(false);
			this->splitContainer2->ResumeLayout(false);
			this->SceneTab->ResumeLayout(false);
			this->CtrlTab->ResumeLayout(false);
			this->CtrlTab->PerformLayout();
			this->toolStrip2->ResumeLayout(false);
			this->toolStrip2->PerformLayout();
			this->UnitEditTab->ResumeLayout(false);
			this->TestUnitTitle->ResumeLayout(false);
			this->tableLayoutPanel1->ResumeLayout(false);
			this->TestPad->ResumeLayout(false);
			this->TabCtrl->ResumeLayout(false);
			this->SystemPad->ResumeLayout(false);
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	private: System::Void ExitMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void MainForm_FormClosed(System::Object^  sender, System::Windows::Forms::FormClosedEventArgs^  e);
	private: System::Void MainForm_Load(System::Object^  sender, System::EventArgs^  e);
	private: System::Void SetPathBrowser_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void CtrlTab_Validated(System::Object^  sender, System::EventArgs^  e);
	private: System::Void CtrlTab_Enter(System::Object^  sender, System::EventArgs^  e);
	private: System::Void AddNewTestUnit_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void TestUnitList_ItemCheck(System::Object^  sender, System::Windows::Forms::ItemCheckEventArgs^  e);
	private: System::Void UnitDelete_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void UnitEdit_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void UnitEditTab_Enter(System::Object^  sender, System::EventArgs^  e);
	private: System::Void UnitEditTab_Layout(System::Object^  sender, System::Windows::Forms::LayoutEventArgs^  e);
	private: System::Void UnitSave_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void UnitLoad_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void LoadProj_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void SaveProj_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void TestUnitClear_Click(System::Object^  sender, System::EventArgs^  e);
	private: CBaseUnit*		CreateTestUnit(const char* name); 
	private: void			FillProjForXml(TiXmlDocument& XmlDoc);
	private: System::Void ImportTestProj_Click(System::Object^  sender, System::EventArgs^  e);
};
}

