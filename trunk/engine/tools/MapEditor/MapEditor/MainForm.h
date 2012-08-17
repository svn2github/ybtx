#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;
using namespace System::Timers;

namespace MapEditor {

	/// <summary>
	/// Summary for MainForm
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class MainForm : public System::Windows::Forms::Form
	{
	public:
		MainForm(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
			m_bLoadFinish = false;
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~MainForm();
	private: sqr_tools::CContextComponent^  MapEditorDescrip;
	private: System::Timers::Timer^ m_Timer;
	protected: 
	private: sqr_tools::CViewCtrl^  MainView;
	private: sqr_tools::CRTSCameraCom^  RTSCameraCom;

	private: System::Windows::Forms::ToolStripContainer^  FrameContainer;
	private: System::Windows::Forms::MenuStrip^  menuStrip1;
	private: System::Windows::Forms::ToolStripMenuItem^  menuFileNew;
	private: System::Windows::Forms::ToolStripMenuItem^  MenuFile;
	private: System::Windows::Forms::ToolStripMenuItem^  menuFileOpen;
	private: System::Windows::Forms::ToolStripMenuItem^  menuFileSave;
	private: System::Windows::Forms::ToolStripMenuItem^  menuFileSaveOther;
	private: System::Windows::Forms::TabControl^  TabPanel;
	private: System::Windows::Forms::TabPage^	TabGrid;
	private: System::Windows::Forms::TabPage^	TabRecord;
	private: System::Windows::Forms::TabPage^	TabAduio;
	private: System::Windows::Forms::TabPage^	TabTex;
	private: System::Windows::Forms::TabPage^	TabLight;
	private: System::Windows::Forms::TabPage^	TabEffect;	
	private: System::Windows::Forms::TabPage^	TabBasic;
	private: System::Windows::Forms::TabPage^	TabPacket;
	private: System::Windows::Forms::TabPage^	TabRegion;
	private: System::Windows::Forms::TabPage^	TabBlock;
	private: System::Windows::Forms::TabPage^	TabObject;
	private: System::Windows::Forms::TabPage^  TabProp;	
	private: sqr_tools::CGridPanel^					m_GridView;
	private: sqr_tools::CRecordPanel^				m_RecordView;
	private: sqr_tools::CTextureViewCltr^			m_TexView;
	private: sqr_tools::CMapEffectCtrl^				m_EffectView;
	private: sqr_tools::CBasicPanel^				m_BasicView;
	private: sqr_tools::CSetViewCtrl^				m_SetView;
	private: sqr_tools::CTileViewCtrl^				m_TileView;
	private: sqr_tools::CBlockViewCtrl^				m_BlockViewCtrl;
	private: sqr_tools::AudioViewCtrl^  audioViewCtrl;
	private: System::Windows::Forms::ToolStripMenuItem^  menuFileMapEx;
	private: System::Windows::Forms::ToolStripMenuItem^  MenuEdit;
	protected: System::Windows::Forms::Timer^  m_FormTimer;
	private: System::ComponentModel::IContainer^  components;
	private: sqr_tools::CPropCtrl^  m_PropShow;
	private: sqr_tools::CLightView^  cLightView1;
	private: System::Windows::Forms::ToolStripMenuItem^  纹理ToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  第1层全铺CtrlFToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  隐藏第1层F1ToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  隐藏第2层F2ToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  删除纹理DeleteToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  设置地表材质音效ToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  强行F5ToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  强行不设置F6ToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  打开纹理图片Ctrl1ToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  阻挡ToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  清除所有阻挡ToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  低障ToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  中障ToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  高障ToolStripMenuItem;
	private: sqr_tools::CFREECameraCom^  FREECameraCom;
	private: System::Windows::Forms::ToolStripMenuItem^  物件ToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  缩放ToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  还原6ToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  放大7ToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  缩小8ToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  旋转ToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  x轴正向RToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  x轴负向FToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  y轴正向IToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  y轴负向KToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  z轴正向YToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  z轴负向HToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  旋转90度TToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  移动ToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  向左LeftToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  向右RightToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  向里ToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  向外ToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  向上PageUpToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  向下PageDownToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  快速向上HomeToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  快速向下EndToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  设置阴影ToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  切换阴影F6ToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  关掉所选物件阴影ToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  开启所选物件阴影ToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  接收阴影ToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  切换物体ToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  全部接收阴影ToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  全部不接收阴影ToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  下一个渲染状态F9ToolStripMenuItem;
private: System::Windows::Forms::ToolStripMenuItem^  镜像MToolStripMenuItem;
private: System::Windows::Forms::ToolStripMenuItem^  删除DeleteToolStripMenuItem;
private: System::Windows::Forms::ToolStripMenuItem^  复制CtrlCToolStripMenuItem;
private: System::Windows::Forms::ToolStripMenuItem^  粘贴CtrlVToolStripMenuItem;
private: System::Windows::Forms::ToolStripMenuItem^  反选CtrlRToolStripMenuItem;
private: System::Windows::Forms::ToolStripMenuItem^  选中所有相同物体CtrlAToolStripMenuItem;
private: System::Windows::Forms::ToolStripMenuItem^  物体使用摄像机碰撞CtrlF6ToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  toolStripMenuItem1;








	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>

	bool										m_bLoadFinish;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			this->FrameContainer = (gcnew System::Windows::Forms::ToolStripContainer());
			this->MainView = (gcnew sqr_tools::CViewCtrl());
			this->MapEditorDescrip = (gcnew sqr_tools::CContextComponent());
			this->TabPanel = (gcnew System::Windows::Forms::TabControl());
			this->TabGrid = (gcnew System::Windows::Forms::TabPage());
			this->m_GridView = (gcnew sqr_tools::CGridPanel());
			this->TabRecord = (gcnew System::Windows::Forms::TabPage());
			this->m_RecordView = (gcnew sqr_tools::CRecordPanel());
			this->TabAduio = (gcnew System::Windows::Forms::TabPage());
			this->audioViewCtrl = (gcnew sqr_tools::AudioViewCtrl());
			this->TabTex = (gcnew System::Windows::Forms::TabPage());
			this->m_TexView = (gcnew sqr_tools::CTextureViewCltr());
			this->TabLight = (gcnew System::Windows::Forms::TabPage());
			this->cLightView1 = (gcnew sqr_tools::CLightView());
			this->TabEffect = (gcnew System::Windows::Forms::TabPage());
			this->m_EffectView = (gcnew sqr_tools::CMapEffectCtrl());
			this->TabBasic = (gcnew System::Windows::Forms::TabPage());
			this->m_BasicView = (gcnew sqr_tools::CBasicPanel());
			this->TabPacket = (gcnew System::Windows::Forms::TabPage());
			this->m_TileView = (gcnew sqr_tools::CTileViewCtrl());
			this->TabObject = (gcnew System::Windows::Forms::TabPage());
			this->m_SetView = (gcnew sqr_tools::CSetViewCtrl());
			this->TabRegion = (gcnew System::Windows::Forms::TabPage());
			this->TabBlock = (gcnew System::Windows::Forms::TabPage());
			this->m_BlockViewCtrl = (gcnew sqr_tools::CBlockViewCtrl());
			this->TabProp = (gcnew System::Windows::Forms::TabPage());
			this->m_PropShow = (gcnew sqr_tools::CPropCtrl());
			this->menuStrip1 = (gcnew System::Windows::Forms::MenuStrip());
			this->MenuFile = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->menuFileNew = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->menuFileOpen = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->menuFileSave = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->menuFileSaveOther = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->menuFileMapEx = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->MenuEdit = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->纹理ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->第1层全铺CtrlFToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->隐藏第1层F1ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->隐藏第2层F2ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->删除纹理DeleteToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->设置地表材质音效ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->强行F5ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->强行不设置F6ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->打开纹理图片Ctrl1ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->物件ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->移动ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->向左LeftToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->向右RightToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->向里ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->向外ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->向上PageUpToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->向下PageDownToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->快速向上HomeToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->快速向下EndToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->缩放ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->还原6ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->放大7ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->缩小8ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->旋转ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->x轴正向RToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->x轴负向FToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->y轴正向IToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->y轴负向KToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->z轴正向YToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->z轴负向HToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->旋转90度TToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->设置阴影ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->切换阴影F6ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->关掉所选物件阴影ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->开启所选物件阴影ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->接收阴影ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->切换物体ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->全部接收阴影ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->全部不接收阴影ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->下一个渲染状态F9ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->镜像MToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->删除DeleteToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->复制CtrlCToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->粘贴CtrlVToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->反选CtrlRToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->选中所有相同物体CtrlAToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->物体使用摄像机碰撞CtrlF6ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->阻挡ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->清除所有阻挡ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->低障ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->中障ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->高障ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->RTSCameraCom = (gcnew sqr_tools::CRTSCameraCom());
			this->m_FormTimer = (gcnew System::Windows::Forms::Timer(this->components));
			this->FREECameraCom = (gcnew sqr_tools::CFREECameraCom());
			this->FrameContainer->ContentPanel->SuspendLayout();
			this->FrameContainer->TopToolStripPanel->SuspendLayout();
			this->FrameContainer->SuspendLayout();
			this->TabPanel->SuspendLayout();
			this->TabGrid->SuspendLayout();
			this->TabRecord->SuspendLayout();
			this->TabAduio->SuspendLayout();
			this->TabTex->SuspendLayout();
			this->TabLight->SuspendLayout();
			this->TabEffect->SuspendLayout();
			this->TabBasic->SuspendLayout();
			this->TabPacket->SuspendLayout();
			this->TabObject->SuspendLayout();
			this->TabBlock->SuspendLayout();
			this->TabProp->SuspendLayout();
			this->menuStrip1->SuspendLayout();
			this->SuspendLayout();
			// 
			// FrameContainer
			// 
			// 
			// FrameContainer.ContentPanel
			// 
			this->FrameContainer->ContentPanel->Controls->Add(this->MainView);
			this->FrameContainer->ContentPanel->Controls->Add(this->TabPanel);
			this->FrameContainer->ContentPanel->Size = System::Drawing::Size(841, 578);
			this->FrameContainer->Dock = System::Windows::Forms::DockStyle::Fill;
			this->FrameContainer->Location = System::Drawing::Point(0, 0);
			this->FrameContainer->Name = L"FrameContainer";
			this->FrameContainer->Size = System::Drawing::Size(841, 603);
			this->FrameContainer->TabIndex = 2;
			this->FrameContainer->Text = L"FrameIn";
			// 
			// FrameContainer.TopToolStripPanel
			// 
			this->FrameContainer->TopToolStripPanel->Controls->Add(this->menuStrip1);
			// 
			// MainView
			// 
			this->MainView->BackColor = System::Drawing::Color::Gray;
			this->MainView->ContextComponent = this->MapEditorDescrip;
			this->MainView->Dock = System::Windows::Forms::DockStyle::Fill;
			this->MainView->Location = System::Drawing::Point(0, 0);
			this->MainView->Name = L"MainView";
			this->MainView->Size = System::Drawing::Size(499, 578);
			this->MainView->TabIndex = 0;
			// 
			// MapEditorDescrip
			// 
			this->MapEditorDescrip->AmbientColor = System::Drawing::Color::Empty;
			this->MapEditorDescrip->SceneName = nullptr;
			this->MapEditorDescrip->ShowBlock = false;
			this->MapEditorDescrip->ShowGrid = false;
			this->MapEditorDescrip->SunColor = System::Drawing::Color::Empty;
			// 
			// TabPanel
			// 
			this->TabPanel->Controls->Add(this->TabGrid);
			this->TabPanel->Controls->Add(this->TabRecord);
			this->TabPanel->Controls->Add(this->TabAduio);
			this->TabPanel->Controls->Add(this->TabTex);
			this->TabPanel->Controls->Add(this->TabLight);
			this->TabPanel->Controls->Add(this->TabEffect);
			this->TabPanel->Controls->Add(this->TabBasic);
			this->TabPanel->Controls->Add(this->TabPacket);
			this->TabPanel->Controls->Add(this->TabObject);
			this->TabPanel->Controls->Add(this->TabRegion);
			this->TabPanel->Controls->Add(this->TabBlock);
			this->TabPanel->Controls->Add(this->TabProp);
			this->TabPanel->Dock = System::Windows::Forms::DockStyle::Right;
			this->TabPanel->Location = System::Drawing::Point(499, 0);
			this->TabPanel->Multiline = true;
			this->TabPanel->Name = L"TabPanel";
			this->TabPanel->SelectedIndex = 0;
			this->TabPanel->Size = System::Drawing::Size(342, 578);
			this->TabPanel->TabIndex = 2;
			this->TabPanel->Selected += gcnew System::Windows::Forms::TabControlEventHandler(this, &MainForm::TabPanel_Selected);
			// 
			// TabGrid
			// 
			this->TabGrid->Controls->Add(this->m_GridView);
			this->TabGrid->Location = System::Drawing::Point(4, 40);
			this->TabGrid->Name = L"TabGrid";
			this->TabGrid->Padding = System::Windows::Forms::Padding(3);
			this->TabGrid->Size = System::Drawing::Size(334, 534);
			this->TabGrid->TabIndex = 0;
			this->TabGrid->Text = L"网格";
			this->TabGrid->UseVisualStyleBackColor = true;
			// 
			// m_GridView
			// 
			this->m_GridView->AutoScroll = true;
			this->m_GridView->ContextComponent = this->MapEditorDescrip;
			this->m_GridView->Dock = System::Windows::Forms::DockStyle::Fill;
			this->m_GridView->Location = System::Drawing::Point(3, 3);
			this->m_GridView->Name = L"m_GridView";
			this->m_GridView->Size = System::Drawing::Size(328, 528);
			this->m_GridView->TabIndex = 0;
			// 
			// TabRecord
			// 
			this->TabRecord->Controls->Add(this->m_RecordView);
			this->TabRecord->Location = System::Drawing::Point(4, 40);
			this->TabRecord->Name = L"TabRecord";
			this->TabRecord->Padding = System::Windows::Forms::Padding(3);
			this->TabRecord->Size = System::Drawing::Size(334, 534);
			this->TabRecord->TabIndex = 1;
			this->TabRecord->Text = L"录像";
			this->TabRecord->UseVisualStyleBackColor = true;
			// 
			// m_RecordView
			// 
			this->m_RecordView->AutoScroll = true;
			this->m_RecordView->ContextComponent = this->MapEditorDescrip;
			this->m_RecordView->Dock = System::Windows::Forms::DockStyle::Fill;
			this->m_RecordView->Location = System::Drawing::Point(3, 3);
			this->m_RecordView->Name = L"m_RecordView";
			this->m_RecordView->Size = System::Drawing::Size(328, 528);
			this->m_RecordView->TabIndex = 1;
			// 
			// TabAduio
			// 
			this->TabAduio->Controls->Add(this->audioViewCtrl);
			this->TabAduio->Location = System::Drawing::Point(4, 40);
			this->TabAduio->Name = L"TabAduio";
			this->TabAduio->Padding = System::Windows::Forms::Padding(3);
			this->TabAduio->Size = System::Drawing::Size(334, 534);
			this->TabAduio->TabIndex = 2;
			this->TabAduio->Text = L"音效";
			this->TabAduio->UseVisualStyleBackColor = true;
			// 
			// audioViewCtrl
			// 
			this->audioViewCtrl->AutoScroll = true;
			this->audioViewCtrl->ContextComponent = nullptr;
			this->audioViewCtrl->Dock = System::Windows::Forms::DockStyle::Fill;
			this->audioViewCtrl->Location = System::Drawing::Point(3, 3);
			this->audioViewCtrl->Name = L"audioViewCtrl";
			this->audioViewCtrl->Size = System::Drawing::Size(328, 528);
			this->audioViewCtrl->TabIndex = 0;
			// 
			// TabTex
			// 
			this->TabTex->Controls->Add(this->m_TexView);
			this->TabTex->Location = System::Drawing::Point(4, 40);
			this->TabTex->Name = L"TabTex";
			this->TabTex->Padding = System::Windows::Forms::Padding(3);
			this->TabTex->Size = System::Drawing::Size(334, 534);
			this->TabTex->TabIndex = 3;
			this->TabTex->Text = L"纹理";
			this->TabTex->UseVisualStyleBackColor = true;
			// 
			// m_TexView
			// 
			this->m_TexView->AutoScroll = true;
			this->m_TexView->ContextComponent = this->MapEditorDescrip;
			this->m_TexView->Dock = System::Windows::Forms::DockStyle::Fill;
			this->m_TexView->Location = System::Drawing::Point(3, 3);
			this->m_TexView->Name = L"m_TexView";
			this->m_TexView->Size = System::Drawing::Size(328, 528);
			this->m_TexView->TabIndex = 0;
			// 
			// TabLight
			// 
			this->TabLight->Controls->Add(this->cLightView1);
			this->TabLight->Location = System::Drawing::Point(4, 40);
			this->TabLight->Name = L"TabLight";
			this->TabLight->Padding = System::Windows::Forms::Padding(3);
			this->TabLight->Size = System::Drawing::Size(334, 534);
			this->TabLight->TabIndex = 4;
			this->TabLight->Text = L"光效";
			this->TabLight->UseVisualStyleBackColor = true;
			// 
			// cLightView1
			// 
			this->cLightView1->ContextComponent = this->MapEditorDescrip;
			this->cLightView1->Location = System::Drawing::Point(29, 22);
			this->cLightView1->Name = L"cLightView1";
			this->cLightView1->Size = System::Drawing::Size(278, 364);
			this->cLightView1->TabIndex = 0;
			// 
			// TabEffect
			// 
			this->TabEffect->Controls->Add(this->m_EffectView);
			this->TabEffect->Location = System::Drawing::Point(4, 40);
			this->TabEffect->Name = L"TabEffect";
			this->TabEffect->Padding = System::Windows::Forms::Padding(3);
			this->TabEffect->Size = System::Drawing::Size(334, 534);
			this->TabEffect->TabIndex = 5;
			this->TabEffect->Text = L"特效";
			this->TabEffect->UseVisualStyleBackColor = true;
			// 
			// m_EffectView
			// 
			this->m_EffectView->ContextComponent = nullptr;
			this->m_EffectView->Dock = System::Windows::Forms::DockStyle::Fill;
			this->m_EffectView->Location = System::Drawing::Point(3, 3);
			this->m_EffectView->Name = L"m_EffectView";
			this->m_EffectView->Size = System::Drawing::Size(328, 528);
			this->m_EffectView->TabIndex = 0;
			// 
			// TabBasic
			// 
			this->TabBasic->Controls->Add(this->m_BasicView);
			this->TabBasic->Location = System::Drawing::Point(4, 40);
			this->TabBasic->Name = L"TabBasic";
			this->TabBasic->Padding = System::Windows::Forms::Padding(3);
			this->TabBasic->Size = System::Drawing::Size(334, 534);
			this->TabBasic->TabIndex = 6;
			this->TabBasic->Text = L"基础";
			this->TabBasic->UseVisualStyleBackColor = true;
			// 
			// m_BasicView
			// 
			this->m_BasicView->AutoScroll = true;
			this->m_BasicView->ContextComponent = nullptr;
			this->m_BasicView->Dock = System::Windows::Forms::DockStyle::Fill;
			this->m_BasicView->Location = System::Drawing::Point(3, 3);
			this->m_BasicView->Name = L"m_BasicView";
			this->m_BasicView->Size = System::Drawing::Size(328, 528);
			this->m_BasicView->TabIndex = 0;
			// 
			// TabPacket
			// 
			this->TabPacket->Controls->Add(this->m_TileView);
			this->TabPacket->Location = System::Drawing::Point(4, 40);
			this->TabPacket->Name = L"TabPacket";
			this->TabPacket->Padding = System::Windows::Forms::Padding(3);
			this->TabPacket->Size = System::Drawing::Size(334, 534);
			this->TabPacket->TabIndex = 7;
			this->TabPacket->Text = L"图素";
			this->TabPacket->UseVisualStyleBackColor = true;
			// 
			// m_TileView
			// 
			this->m_TileView->AutoScroll = true;
			this->m_TileView->ContextComponent = this->MapEditorDescrip;
			this->m_TileView->Dock = System::Windows::Forms::DockStyle::Fill;
			this->m_TileView->Location = System::Drawing::Point(3, 3);
			this->m_TileView->Name = L"m_TileView";
			this->m_TileView->Size = System::Drawing::Size(328, 528);
			this->m_TileView->TabIndex = 0;
			// 
			// TabObject
			// 
			this->TabObject->Controls->Add(this->m_SetView);
			this->TabObject->Location = System::Drawing::Point(4, 40);
			this->TabObject->Name = L"TabObject";
			this->TabObject->Padding = System::Windows::Forms::Padding(3);
			this->TabObject->Size = System::Drawing::Size(334, 534);
			this->TabObject->TabIndex = 8;
			this->TabObject->Text = L"物件";
			this->TabObject->UseVisualStyleBackColor = true;
			// 
			// m_SetView
			// 
			this->m_SetView->AutoScroll = true;
			this->m_SetView->ContextComponent = this->MapEditorDescrip;
			this->m_SetView->Dock = System::Windows::Forms::DockStyle::Fill;
			this->m_SetView->Location = System::Drawing::Point(3, 3);
			this->m_SetView->Name = L"m_SetView";
			this->m_SetView->Size = System::Drawing::Size(328, 528);
			this->m_SetView->TabIndex = 0;
			// 
			// TabRegion
			// 
			this->TabRegion->Location = System::Drawing::Point(4, 40);
			this->TabRegion->Name = L"TabRegion";
			this->TabRegion->Padding = System::Windows::Forms::Padding(3);
			this->TabRegion->Size = System::Drawing::Size(334, 534);
			this->TabRegion->TabIndex = 9;
			this->TabRegion->Text = L"区域";
			this->TabRegion->UseVisualStyleBackColor = true;
			// 
			// TabBlock
			// 
			this->TabBlock->Controls->Add(this->m_BlockViewCtrl);
			this->TabBlock->Location = System::Drawing::Point(4, 40);
			this->TabBlock->Name = L"TabBlock";
			this->TabBlock->Padding = System::Windows::Forms::Padding(3);
			this->TabBlock->Size = System::Drawing::Size(334, 534);
			this->TabBlock->TabIndex = 10;
			this->TabBlock->Text = L"阻挡";
			this->TabBlock->UseVisualStyleBackColor = true;
			// 
			// m_BlockViewCtrl
			// 
			this->m_BlockViewCtrl->ContextComponent = this->MapEditorDescrip;
			this->m_BlockViewCtrl->Location = System::Drawing::Point(29, 0);
			this->m_BlockViewCtrl->Name = L"m_BlockViewCtrl";
			this->m_BlockViewCtrl->Size = System::Drawing::Size(272, 554);
			this->m_BlockViewCtrl->TabIndex = 0;
			// 
			// TabProp
			// 
			this->TabProp->AutoScroll = true;
			this->TabProp->Controls->Add(this->m_PropShow);
			this->TabProp->Location = System::Drawing::Point(4, 40);
			this->TabProp->Name = L"TabProp";
			this->TabProp->Padding = System::Windows::Forms::Padding(3);
			this->TabProp->Size = System::Drawing::Size(334, 534);
			this->TabProp->TabIndex = 11;
			this->TabProp->Text = L"属性";
			this->TabProp->UseVisualStyleBackColor = true;
			// 
			// m_PropShow
			// 
			this->m_PropShow->ContextComponent = this->MapEditorDescrip;
			this->m_PropShow->Dock = System::Windows::Forms::DockStyle::Top;
			this->m_PropShow->Location = System::Drawing::Point(3, 3);
			this->m_PropShow->Name = L"m_PropShow";
			this->m_PropShow->Size = System::Drawing::Size(328, 469);
			this->m_PropShow->TabIndex = 1;
			// 
			// menuStrip1
			// 
			this->menuStrip1->Dock = System::Windows::Forms::DockStyle::None;
			this->menuStrip1->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(5) {this->MenuFile, this->MenuEdit, 
				this->纹理ToolStripMenuItem, this->物件ToolStripMenuItem, this->阻挡ToolStripMenuItem});
			this->menuStrip1->Location = System::Drawing::Point(0, 0);
			this->menuStrip1->Name = L"menuStrip1";
			this->menuStrip1->Size = System::Drawing::Size(841, 25);
			this->menuStrip1->TabIndex = 0;
			this->menuStrip1->Text = L"menuStrip1";
			// 
			// MenuFile
			// 
			this->MenuFile->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(5) {this->menuFileNew, 
				this->menuFileOpen, this->menuFileSave, this->menuFileSaveOther, this->menuFileMapEx});
			this->MenuFile->Name = L"MenuFile";
			this->MenuFile->Size = System::Drawing::Size(44, 21);
			this->MenuFile->Text = L"文件";
			// 
			// menuFileNew
			// 
			this->menuFileNew->Name = L"menuFileNew";
			this->menuFileNew->Size = System::Drawing::Size(124, 22);
			this->menuFileNew->Text = L"新建";
			this->menuFileNew->Click += gcnew System::EventHandler(this, &MainForm::menuFileNew_Click);
			// 
			// menuFileOpen
			// 
			this->menuFileOpen->Name = L"menuFileOpen";
			this->menuFileOpen->Size = System::Drawing::Size(124, 22);
			this->menuFileOpen->Text = L"打开";
			this->menuFileOpen->Click += gcnew System::EventHandler(this, &MainForm::menuFileOpen_Click);
			// 
			// menuFileSave
			// 
			this->menuFileSave->Name = L"menuFileSave";
			this->menuFileSave->Size = System::Drawing::Size(124, 22);
			this->menuFileSave->Text = L"保存";
			this->menuFileSave->Click += gcnew System::EventHandler(this, &MainForm::menuFileSave_Click);
			// 
			// menuFileSaveOther
			// 
			this->menuFileSaveOther->Name = L"menuFileSaveOther";
			this->menuFileSaveOther->Size = System::Drawing::Size(124, 22);
			this->menuFileSaveOther->Text = L"另存为";
			this->menuFileSaveOther->Click += gcnew System::EventHandler(this, &MainForm::menuFileSaveOther_Click);
			// 
			// menuFileMapEx
			// 
			this->menuFileMapEx->Name = L"menuFileMapEx";
			this->menuFileMapEx->Size = System::Drawing::Size(124, 22);
			this->menuFileMapEx->Text = L"地图扩展";
			this->menuFileMapEx->Click += gcnew System::EventHandler(this, &MainForm::menuFileMapEx_Click);
			// 
			// MenuEdit
			// 
			this->MenuEdit->Name = L"MenuEdit";
			this->MenuEdit->Size = System::Drawing::Size(44, 21);
			this->MenuEdit->Text = L"编辑";
			// 
			// 纹理ToolStripMenuItem
			// 
			this->纹理ToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(6) {this->第1层全铺CtrlFToolStripMenuItem, 
				this->隐藏第1层F1ToolStripMenuItem, this->隐藏第2层F2ToolStripMenuItem, this->删除纹理DeleteToolStripMenuItem, this->设置地表材质音效ToolStripMenuItem, 
				this->打开纹理图片Ctrl1ToolStripMenuItem});
			this->纹理ToolStripMenuItem->Name = L"纹理ToolStripMenuItem";
			this->纹理ToolStripMenuItem->Size = System::Drawing::Size(44, 21);
			this->纹理ToolStripMenuItem->Text = L"纹理";
			// 
			// 第1层全铺CtrlFToolStripMenuItem
			// 
			this->第1层全铺CtrlFToolStripMenuItem->Name = L"第1层全铺CtrlFToolStripMenuItem";
			this->第1层全铺CtrlFToolStripMenuItem->Size = System::Drawing::Size(192, 22);
			this->第1层全铺CtrlFToolStripMenuItem->Text = L"第1层全铺(&Ctrl+F)";
			// 
			// 隐藏第1层F1ToolStripMenuItem
			// 
			this->隐藏第1层F1ToolStripMenuItem->Name = L"隐藏第1层F1ToolStripMenuItem";
			this->隐藏第1层F1ToolStripMenuItem->Size = System::Drawing::Size(192, 22);
			this->隐藏第1层F1ToolStripMenuItem->Text = L"隐藏第1层(&F1)";
			// 
			// 隐藏第2层F2ToolStripMenuItem
			// 
			this->隐藏第2层F2ToolStripMenuItem->Name = L"隐藏第2层F2ToolStripMenuItem";
			this->隐藏第2层F2ToolStripMenuItem->Size = System::Drawing::Size(192, 22);
			this->隐藏第2层F2ToolStripMenuItem->Text = L"隐藏第2层(&F2)";
			// 
			// 删除纹理DeleteToolStripMenuItem
			// 
			this->删除纹理DeleteToolStripMenuItem->Name = L"删除纹理DeleteToolStripMenuItem";
			this->删除纹理DeleteToolStripMenuItem->Size = System::Drawing::Size(192, 22);
			this->删除纹理DeleteToolStripMenuItem->Text = L"删除纹理(&Delete)";
			// 
			// 设置地表材质音效ToolStripMenuItem
			// 
			this->设置地表材质音效ToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(2) {this->强行F5ToolStripMenuItem, 
				this->强行不设置F6ToolStripMenuItem});
			this->设置地表材质音效ToolStripMenuItem->Name = L"设置地表材质音效ToolStripMenuItem";
			this->设置地表材质音效ToolStripMenuItem->Size = System::Drawing::Size(192, 22);
			this->设置地表材质音效ToolStripMenuItem->Text = L"地表材质音效";
			// 
			// 强行F5ToolStripMenuItem
			// 
			this->强行F5ToolStripMenuItem->Name = L"强行F5ToolStripMenuItem";
			this->强行F5ToolStripMenuItem->Size = System::Drawing::Size(157, 22);
			this->强行F5ToolStripMenuItem->Text = L"强行设置(&F5)";
			// 
			// 强行不设置F6ToolStripMenuItem
			// 
			this->强行不设置F6ToolStripMenuItem->Name = L"强行不设置F6ToolStripMenuItem";
			this->强行不设置F6ToolStripMenuItem->Size = System::Drawing::Size(157, 22);
			this->强行不设置F6ToolStripMenuItem->Text = L"强行不设置(&F6)";
			// 
			// 打开纹理图片Ctrl1ToolStripMenuItem
			// 
			this->打开纹理图片Ctrl1ToolStripMenuItem->Name = L"打开纹理图片Ctrl1ToolStripMenuItem";
			this->打开纹理图片Ctrl1ToolStripMenuItem->Size = System::Drawing::Size(192, 22);
			this->打开纹理图片Ctrl1ToolStripMenuItem->Text = L"打开纹理图片(&Ctrl+1)";
			// 
			// 物件ToolStripMenuItem
			// 
			this->物件ToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(13) {this->移动ToolStripMenuItem, 
				this->缩放ToolStripMenuItem, this->旋转ToolStripMenuItem, this->设置阴影ToolStripMenuItem, this->接收阴影ToolStripMenuItem, this->下一个渲染状态F9ToolStripMenuItem, 
				this->镜像MToolStripMenuItem, this->删除DeleteToolStripMenuItem, this->复制CtrlCToolStripMenuItem, this->粘贴CtrlVToolStripMenuItem, 
				this->反选CtrlRToolStripMenuItem, this->选中所有相同物体CtrlAToolStripMenuItem, this->物体使用摄像机碰撞CtrlF6ToolStripMenuItem});
			this->物件ToolStripMenuItem->Name = L"物件ToolStripMenuItem";
			this->物件ToolStripMenuItem->Size = System::Drawing::Size(44, 21);
			this->物件ToolStripMenuItem->Text = L"物件";
			// 
			// 移动ToolStripMenuItem
			// 
			this->移动ToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(8) {this->向左LeftToolStripMenuItem, 
				this->向右RightToolStripMenuItem, this->向里ToolStripMenuItem, this->向外ToolStripMenuItem, this->向上PageUpToolStripMenuItem, this->向下PageDownToolStripMenuItem, 
				this->快速向上HomeToolStripMenuItem, this->快速向下EndToolStripMenuItem});
			this->移动ToolStripMenuItem->Name = L"移动ToolStripMenuItem";
			this->移动ToolStripMenuItem->Size = System::Drawing::Size(234, 22);
			this->移动ToolStripMenuItem->Text = L"移动";
			// 
			// 向左LeftToolStripMenuItem
			// 
			this->向左LeftToolStripMenuItem->Name = L"向左LeftToolStripMenuItem";
			this->向左LeftToolStripMenuItem->Size = System::Drawing::Size(170, 22);
			this->向左LeftToolStripMenuItem->Text = L"向左(&Left)";
			// 
			// 向右RightToolStripMenuItem
			// 
			this->向右RightToolStripMenuItem->Name = L"向右RightToolStripMenuItem";
			this->向右RightToolStripMenuItem->Size = System::Drawing::Size(170, 22);
			this->向右RightToolStripMenuItem->Text = L"向右(&Right)";
			// 
			// 向里ToolStripMenuItem
			// 
			this->向里ToolStripMenuItem->Name = L"向里ToolStripMenuItem";
			this->向里ToolStripMenuItem->Size = System::Drawing::Size(170, 22);
			this->向里ToolStripMenuItem->Text = L"向里(&↑)";
			// 
			// 向外ToolStripMenuItem
			// 
			this->向外ToolStripMenuItem->Name = L"向外ToolStripMenuItem";
			this->向外ToolStripMenuItem->Size = System::Drawing::Size(170, 22);
			this->向外ToolStripMenuItem->Text = L"向外(&↓)";
			// 
			// 向上PageUpToolStripMenuItem
			// 
			this->向上PageUpToolStripMenuItem->Name = L"向上PageUpToolStripMenuItem";
			this->向上PageUpToolStripMenuItem->Size = System::Drawing::Size(170, 22);
			this->向上PageUpToolStripMenuItem->Text = L"向上(&PageUp)";
			// 
			// 向下PageDownToolStripMenuItem
			// 
			this->向下PageDownToolStripMenuItem->Name = L"向下PageDownToolStripMenuItem";
			this->向下PageDownToolStripMenuItem->Size = System::Drawing::Size(170, 22);
			this->向下PageDownToolStripMenuItem->Text = L"向下(&PageDown)";
			// 
			// 快速向上HomeToolStripMenuItem
			// 
			this->快速向上HomeToolStripMenuItem->Name = L"快速向上HomeToolStripMenuItem";
			this->快速向上HomeToolStripMenuItem->Size = System::Drawing::Size(170, 22);
			this->快速向上HomeToolStripMenuItem->Text = L"快速向上(&Home)";
			// 
			// 快速向下EndToolStripMenuItem
			// 
			this->快速向下EndToolStripMenuItem->Name = L"快速向下EndToolStripMenuItem";
			this->快速向下EndToolStripMenuItem->Size = System::Drawing::Size(170, 22);
			this->快速向下EndToolStripMenuItem->Text = L"快速向下(&End)";
			// 
			// 缩放ToolStripMenuItem
			// 
			this->缩放ToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(3) {this->还原6ToolStripMenuItem, 
				this->放大7ToolStripMenuItem, this->缩小8ToolStripMenuItem});
			this->缩放ToolStripMenuItem->Name = L"缩放ToolStripMenuItem";
			this->缩放ToolStripMenuItem->Size = System::Drawing::Size(234, 22);
			this->缩放ToolStripMenuItem->Text = L"缩放";
			// 
			// 还原6ToolStripMenuItem
			// 
			this->还原6ToolStripMenuItem->Name = L"还原6ToolStripMenuItem";
			this->还原6ToolStripMenuItem->Size = System::Drawing::Size(115, 22);
			this->还原6ToolStripMenuItem->Text = L"还原(&6)";
			// 
			// 放大7ToolStripMenuItem
			// 
			this->放大7ToolStripMenuItem->Name = L"放大7ToolStripMenuItem";
			this->放大7ToolStripMenuItem->Size = System::Drawing::Size(115, 22);
			this->放大7ToolStripMenuItem->Text = L"放大(&7)";
			// 
			// 缩小8ToolStripMenuItem
			// 
			this->缩小8ToolStripMenuItem->Name = L"缩小8ToolStripMenuItem";
			this->缩小8ToolStripMenuItem->Size = System::Drawing::Size(115, 22);
			this->缩小8ToolStripMenuItem->Text = L"缩小(&8)";
			// 
			// 旋转ToolStripMenuItem
			// 
			this->旋转ToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(7) {this->x轴正向RToolStripMenuItem, 
				this->x轴负向FToolStripMenuItem, this->y轴正向IToolStripMenuItem, this->y轴负向KToolStripMenuItem, this->z轴正向YToolStripMenuItem, this->z轴负向HToolStripMenuItem, 
				this->旋转90度TToolStripMenuItem});
			this->旋转ToolStripMenuItem->Name = L"旋转ToolStripMenuItem";
			this->旋转ToolStripMenuItem->Size = System::Drawing::Size(234, 22);
			this->旋转ToolStripMenuItem->Text = L"旋转";
			// 
			// x轴正向RToolStripMenuItem
			// 
			this->x轴正向RToolStripMenuItem->Name = L"x轴正向RToolStripMenuItem";
			this->x轴正向RToolStripMenuItem->Size = System::Drawing::Size(141, 22);
			this->x轴正向RToolStripMenuItem->Text = L"X轴正向(&R)";
			// 
			// x轴负向FToolStripMenuItem
			// 
			this->x轴负向FToolStripMenuItem->Name = L"x轴负向FToolStripMenuItem";
			this->x轴负向FToolStripMenuItem->Size = System::Drawing::Size(141, 22);
			this->x轴负向FToolStripMenuItem->Text = L"X轴负向(&F)";
			// 
			// y轴正向IToolStripMenuItem
			// 
			this->y轴正向IToolStripMenuItem->Name = L"y轴正向IToolStripMenuItem";
			this->y轴正向IToolStripMenuItem->Size = System::Drawing::Size(141, 22);
			this->y轴正向IToolStripMenuItem->Text = L"Y轴正向(&I)";
			// 
			// y轴负向KToolStripMenuItem
			// 
			this->y轴负向KToolStripMenuItem->Name = L"y轴负向KToolStripMenuItem";
			this->y轴负向KToolStripMenuItem->Size = System::Drawing::Size(141, 22);
			this->y轴负向KToolStripMenuItem->Text = L"Y轴负向(&K)";
			// 
			// z轴正向YToolStripMenuItem
			// 
			this->z轴正向YToolStripMenuItem->Name = L"z轴正向YToolStripMenuItem";
			this->z轴正向YToolStripMenuItem->Size = System::Drawing::Size(141, 22);
			this->z轴正向YToolStripMenuItem->Text = L"Z轴正向(&Y)";
			// 
			// z轴负向HToolStripMenuItem
			// 
			this->z轴负向HToolStripMenuItem->Name = L"z轴负向HToolStripMenuItem";
			this->z轴负向HToolStripMenuItem->Size = System::Drawing::Size(141, 22);
			this->z轴负向HToolStripMenuItem->Text = L"Z轴负向(&H)";
			// 
			// 旋转90度TToolStripMenuItem
			// 
			this->旋转90度TToolStripMenuItem->Name = L"旋转90度TToolStripMenuItem";
			this->旋转90度TToolStripMenuItem->Size = System::Drawing::Size(141, 22);
			this->旋转90度TToolStripMenuItem->Text = L"旋转90度(&T)";
			// 
			// 设置阴影ToolStripMenuItem
			// 
			this->设置阴影ToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(3) {this->切换阴影F6ToolStripMenuItem, 
				this->关掉所选物件阴影ToolStripMenuItem, this->开启所选物件阴影ToolStripMenuItem});
			this->设置阴影ToolStripMenuItem->Name = L"设置阴影ToolStripMenuItem";
			this->设置阴影ToolStripMenuItem->Size = System::Drawing::Size(234, 22);
			this->设置阴影ToolStripMenuItem->Text = L"设置阴影";
			// 
			// 切换阴影F6ToolStripMenuItem
			// 
			this->切换阴影F6ToolStripMenuItem->Name = L"切换阴影F6ToolStripMenuItem";
			this->切换阴影F6ToolStripMenuItem->Size = System::Drawing::Size(172, 22);
			this->切换阴影F6ToolStripMenuItem->Text = L"切换阴影(&F6)";
			// 
			// 关掉所选物件阴影ToolStripMenuItem
			// 
			this->关掉所选物件阴影ToolStripMenuItem->Name = L"关掉所选物件阴影ToolStripMenuItem";
			this->关掉所选物件阴影ToolStripMenuItem->Size = System::Drawing::Size(172, 22);
			this->关掉所选物件阴影ToolStripMenuItem->Text = L"关掉所选物件阴影";
			// 
			// 开启所选物件阴影ToolStripMenuItem
			// 
			this->开启所选物件阴影ToolStripMenuItem->Name = L"开启所选物件阴影ToolStripMenuItem";
			this->开启所选物件阴影ToolStripMenuItem->Size = System::Drawing::Size(172, 22);
			this->开启所选物件阴影ToolStripMenuItem->Text = L"开启所选物件阴影";
			// 
			// 接收阴影ToolStripMenuItem
			// 
			this->接收阴影ToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(3) {this->切换物体ToolStripMenuItem, 
				this->全部接收阴影ToolStripMenuItem, this->全部不接收阴影ToolStripMenuItem});
			this->接收阴影ToolStripMenuItem->Name = L"接收阴影ToolStripMenuItem";
			this->接收阴影ToolStripMenuItem->Size = System::Drawing::Size(234, 22);
			this->接收阴影ToolStripMenuItem->Text = L"接收阴影";
			// 
			// 切换物体ToolStripMenuItem
			// 
			this->切换物体ToolStripMenuItem->Name = L"切换物体ToolStripMenuItem";
			this->切换物体ToolStripMenuItem->Size = System::Drawing::Size(193, 22);
			this->切换物体ToolStripMenuItem->Text = L"切换接收阴影状态(&F5)";
			// 
			// 全部接收阴影ToolStripMenuItem
			// 
			this->全部接收阴影ToolStripMenuItem->Name = L"全部接收阴影ToolStripMenuItem";
			this->全部接收阴影ToolStripMenuItem->Size = System::Drawing::Size(193, 22);
			this->全部接收阴影ToolStripMenuItem->Text = L"全部接收阴影";
			// 
			// 全部不接收阴影ToolStripMenuItem
			// 
			this->全部不接收阴影ToolStripMenuItem->Name = L"全部不接收阴影ToolStripMenuItem";
			this->全部不接收阴影ToolStripMenuItem->Size = System::Drawing::Size(193, 22);
			this->全部不接收阴影ToolStripMenuItem->Text = L"全部不接收阴影";
			// 
			// 下一个渲染状态F9ToolStripMenuItem
			// 
			this->下一个渲染状态F9ToolStripMenuItem->Name = L"下一个渲染状态F9ToolStripMenuItem";
			this->下一个渲染状态F9ToolStripMenuItem->Size = System::Drawing::Size(234, 22);
			this->下一个渲染状态F9ToolStripMenuItem->Text = L"下一个渲染状态(&F9)";
			// 
			// 镜像MToolStripMenuItem
			// 
			this->镜像MToolStripMenuItem->Name = L"镜像MToolStripMenuItem";
			this->镜像MToolStripMenuItem->Size = System::Drawing::Size(234, 22);
			this->镜像MToolStripMenuItem->Text = L"镜像(M)";
			// 
			// 删除DeleteToolStripMenuItem
			// 
			this->删除DeleteToolStripMenuItem->Name = L"删除DeleteToolStripMenuItem";
			this->删除DeleteToolStripMenuItem->Size = System::Drawing::Size(234, 22);
			this->删除DeleteToolStripMenuItem->Text = L"删除(&Delete)";
			// 
			// 复制CtrlCToolStripMenuItem
			// 
			this->复制CtrlCToolStripMenuItem->Name = L"复制CtrlCToolStripMenuItem";
			this->复制CtrlCToolStripMenuItem->Size = System::Drawing::Size(234, 22);
			this->复制CtrlCToolStripMenuItem->Text = L"复制(&Ctrl+C)";
			// 
			// 粘贴CtrlVToolStripMenuItem
			// 
			this->粘贴CtrlVToolStripMenuItem->Name = L"粘贴CtrlVToolStripMenuItem";
			this->粘贴CtrlVToolStripMenuItem->Size = System::Drawing::Size(234, 22);
			this->粘贴CtrlVToolStripMenuItem->Text = L"粘贴(&Ctrl+V)";
			// 
			// 反选CtrlRToolStripMenuItem
			// 
			this->反选CtrlRToolStripMenuItem->Name = L"反选CtrlRToolStripMenuItem";
			this->反选CtrlRToolStripMenuItem->Size = System::Drawing::Size(234, 22);
			this->反选CtrlRToolStripMenuItem->Text = L"反选(&Ctrl+R)";
			// 
			// 选中所有相同物体CtrlAToolStripMenuItem
			// 
			this->选中所有相同物体CtrlAToolStripMenuItem->Name = L"选中所有相同物体CtrlAToolStripMenuItem";
			this->选中所有相同物体CtrlAToolStripMenuItem->Size = System::Drawing::Size(234, 22);
			this->选中所有相同物体CtrlAToolStripMenuItem->Text = L"选中所有相同物体(&Ctrl+A)";
			// 
			// 物体使用摄像机碰撞CtrlF6ToolStripMenuItem
			// 
			this->物体使用摄像机碰撞CtrlF6ToolStripMenuItem->Name = L"物体使用摄像机碰撞CtrlF6ToolStripMenuItem";
			this->物体使用摄像机碰撞CtrlF6ToolStripMenuItem->Size = System::Drawing::Size(234, 22);
			this->物体使用摄像机碰撞CtrlF6ToolStripMenuItem->Text = L"物体使用摄像机碰撞(&Ctrl+F6)";
			// 
			// 阻挡ToolStripMenuItem
			// 
			this->阻挡ToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) {this->清除所有阻挡ToolStripMenuItem});
			this->阻挡ToolStripMenuItem->Name = L"阻挡ToolStripMenuItem";
			this->阻挡ToolStripMenuItem->Size = System::Drawing::Size(44, 21);
			this->阻挡ToolStripMenuItem->Text = L"阻挡";
			// 
			// 清除所有阻挡ToolStripMenuItem
			// 
			this->清除所有阻挡ToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(3) {this->低障ToolStripMenuItem, 
				this->中障ToolStripMenuItem, this->高障ToolStripMenuItem});
			this->清除所有阻挡ToolStripMenuItem->Name = L"清除所有阻挡ToolStripMenuItem";
			this->清除所有阻挡ToolStripMenuItem->Size = System::Drawing::Size(148, 22);
			this->清除所有阻挡ToolStripMenuItem->Text = L"清除所有阻挡";
			// 
			// 低障ToolStripMenuItem
			// 
			this->低障ToolStripMenuItem->Name = L"低障ToolStripMenuItem";
			this->低障ToolStripMenuItem->Size = System::Drawing::Size(100, 22);
			this->低障ToolStripMenuItem->Text = L"低障";
			// 
			// 中障ToolStripMenuItem
			// 
			this->中障ToolStripMenuItem->Name = L"中障ToolStripMenuItem";
			this->中障ToolStripMenuItem->Size = System::Drawing::Size(100, 22);
			this->中障ToolStripMenuItem->Text = L"中障";
			// 
			// 高障ToolStripMenuItem
			// 
			this->高障ToolStripMenuItem->Name = L"高障ToolStripMenuItem";
			this->高障ToolStripMenuItem->Size = System::Drawing::Size(100, 22);
			this->高障ToolStripMenuItem->Text = L"高障";
			// 
			// RTSCameraCom
			// 
			this->RTSCameraCom->ContextComponent = this->MapEditorDescrip;
			// 
			// m_FormTimer
			// 
			this->m_FormTimer->Enabled = true;
			this->m_FormTimer->Tick += gcnew System::EventHandler(this, &MainForm::m_FormTimer_Tick);
			// 
			// FREECameraCom
			// 
			this->FREECameraCom->ContextComponent = this->MapEditorDescrip;
			// 
			// MainForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(841, 603);
			this->Controls->Add(this->FrameContainer);
			this->MainMenuStrip = this->menuStrip1;
			this->Name = L"MainForm";
			this->Text = L"MainForm";
			this->Load += gcnew System::EventHandler(this, &MainForm::MainForm_Load);
			this->FormClosed += gcnew System::Windows::Forms::FormClosedEventHandler(this, &MainForm::MainForm_FormClosed);
			this->FrameContainer->ContentPanel->ResumeLayout(false);
			this->FrameContainer->TopToolStripPanel->ResumeLayout(false);
			this->FrameContainer->TopToolStripPanel->PerformLayout();
			this->FrameContainer->ResumeLayout(false);
			this->FrameContainer->PerformLayout();
			this->TabPanel->ResumeLayout(false);
			this->TabGrid->ResumeLayout(false);
			this->TabRecord->ResumeLayout(false);
			this->TabAduio->ResumeLayout(false);
			this->TabTex->ResumeLayout(false);
			this->TabLight->ResumeLayout(false);
			this->TabEffect->ResumeLayout(false);
			this->TabBasic->ResumeLayout(false);
			this->TabPacket->ResumeLayout(false);
			this->TabObject->ResumeLayout(false);
			this->TabBlock->ResumeLayout(false);
			this->TabProp->ResumeLayout(false);
			this->menuStrip1->ResumeLayout(false);
			this->menuStrip1->PerformLayout();
			this->ResumeLayout(false);

		}
#pragma endregion
	private: System::Void InitExp();
	//定时器处理函数
	private: System::Void m_FormTimer_Tick(System::Object^  sender, System::EventArgs^  e);
	private: System::Void UpdateTimer(System::Object^ sender,System::Timers::ElapsedEventArgs^ e);
	private: System::Void MainForm_Load(System::Object^  sender, System::EventArgs^  e);
	private: System::Void MainForm_FormClosed(System::Object^  sender, System::Windows::Forms::FormClosedEventArgs^  e) ;
	private: System::Void TabPanel_Selected(System::Object^  sender, System::Windows::Forms::TabControlEventArgs^  e);
	private: System::Void menuFileNew_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void menuFileOpen_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void menuFileSave_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void menuFileSaveOther_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void menuFileMapEx_Click(System::Object^  sender, System::EventArgs^  e);
	
};
}
