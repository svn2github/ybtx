#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace ModelEditor {

	/// <summary>
	/// Summary for ModelEditorForm
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class ModelEditorForm : public System::Windows::Forms::Form
	{
	public:
		ModelEditorForm(void)
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
		~ModelEditorForm()
		{
			if (components)
			{
				delete components;
			}
		}
	private: sqr_tools::CContextComponent^  cContextComponent;
	private: sqr_tools::CRTSCameraCom^  crtsCameraCom;
	private: System::Windows::Forms::TableLayoutPanel^  tableLayoutPanel1;
	private: System::Windows::Forms::TableLayoutPanel^  tableLayoutPanel2;
	private: System::Windows::Forms::TableLayoutPanel^  tableLayoutPanel3;
	private: sqr_tools::CViewCtrl^  cViewCtrl;
	private: System::Windows::Forms::MenuStrip^  menuStrip;
	private: System::Windows::Forms::ToolStripMenuItem^  文件ToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  打开模型ToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  保存模型ToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  保存模型为ToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  保存全部ToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  关闭所有模型文件ToolStripMenuItem;

	private: sqr_tools::CBoundingBoxPropCom^  cBoundingBoxPropCom;
	private: sqr_tools::CSkeletonPropCom^  cSkeletonPropCom;
	private: System::Windows::Forms::TableLayoutPanel^  tableLayoutPanel9;
	private: sqr_tools::CFrameScaleCtrl^  cFrameScaleCtrl;


	private: System::Windows::Forms::TabControl^  tabControl1;
	private: System::Windows::Forms::TabPage^  tabPage1;
	private: System::Windows::Forms::TableLayoutPanel^  tableLayoutPanel10;
	private: System::Windows::Forms::TableLayoutPanel^  tableLayoutPanel11;
	private: System::Windows::Forms::TableLayoutPanel^  tableLayoutPanel12;
	private: System::Windows::Forms::TableLayoutPanel^  tableLayoutPanel13;
	private: System::Windows::Forms::TableLayoutPanel^  tableLayoutPanel14;
	private: System::Windows::Forms::TabPage^  tabPage2;
	private: System::Windows::Forms::TableLayoutPanel^  tableLayoutPanel4;
	private: System::Windows::Forms::TableLayoutPanel^  tableLayoutPanel5;
	private: System::Windows::Forms::TableLayoutPanel^  tableLayoutPanel6;
	private: System::Windows::Forms::TableLayoutPanel^  tableLayoutPanel7;
	private: System::Windows::Forms::TableLayoutPanel^  tableLayoutPanel8;
	private: sqr_tools::CPieceGroupEditCtrl^  cPieceGroupEditCtrl;
	private: sqr_tools::CPieceUnitEditCtrl^  cPieceUnitEditCtrl;
	private: System::Windows::Forms::PropertyGrid^  cBoundingBoxPropertyGrid;
	private: sqr_tools::CAnimationGroupEditCtrl^  cAnimationGroupEditCtrl;
	private: System::Windows::Forms::PropertyGrid^  cSkeletonPropertyGrid;
	private: System::Windows::Forms::TableLayoutPanel^  tableLayoutPanel15;
	private: sqr_tools::CKeyFrameCtrl^  cKeyFrameCtrl;
	private: sqr_tools::CGroundSpeedCtrl^ cGroundSpeedCtrl;
	private: System::Windows::Forms::TabPage^  tabPage3;
	private: System::Windows::Forms::TableLayoutPanel^  tableLayoutPanel16;
	private: System::Windows::Forms::TableLayoutPanel^  tableLayoutPanel17;
	private: sqr_tools::CBoneListCtrl^  cBoneListCtrl;
	private: sqr_tools::CBoneBoxListCtrl^  cBoneBoxListCtrl;
	private: System::Windows::Forms::TableLayoutPanel^  tableLayoutPanel18;
	private: sqr_tools::CBoneBoxEditCtrl^  cBoneBoxEditCtrl;
	private: System::Windows::Forms::ToolStripMenuItem^  设置ToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  工作路径ToolStripMenuItem;


	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->tableLayoutPanel1 = (gcnew System::Windows::Forms::TableLayoutPanel());
			this->tableLayoutPanel2 = (gcnew System::Windows::Forms::TableLayoutPanel());
			this->tableLayoutPanel3 = (gcnew System::Windows::Forms::TableLayoutPanel());
			this->cViewCtrl = (gcnew sqr_tools::CViewCtrl());
			this->cContextComponent = (gcnew sqr_tools::CContextComponent());
			this->tabControl1 = (gcnew System::Windows::Forms::TabControl());
			this->tabPage1 = (gcnew System::Windows::Forms::TabPage());
			this->tableLayoutPanel10 = (gcnew System::Windows::Forms::TableLayoutPanel());
			this->tableLayoutPanel11 = (gcnew System::Windows::Forms::TableLayoutPanel());
			this->tableLayoutPanel12 = (gcnew System::Windows::Forms::TableLayoutPanel());
			this->cPieceUnitEditCtrl = (gcnew sqr_tools::CPieceUnitEditCtrl());
			this->cBoundingBoxPropertyGrid = (gcnew System::Windows::Forms::PropertyGrid());
			this->cBoundingBoxPropCom = (gcnew sqr_tools::CBoundingBoxPropCom());
			this->cPieceGroupEditCtrl = (gcnew sqr_tools::CPieceGroupEditCtrl());
			this->tableLayoutPanel13 = (gcnew System::Windows::Forms::TableLayoutPanel());
			this->tableLayoutPanel14 = (gcnew System::Windows::Forms::TableLayoutPanel());
			this->cSkeletonPropertyGrid = (gcnew System::Windows::Forms::PropertyGrid());
			this->cSkeletonPropCom = (gcnew sqr_tools::CSkeletonPropCom());
			this->cAnimationGroupEditCtrl = (gcnew sqr_tools::CAnimationGroupEditCtrl());
			this->tabPage2 = (gcnew System::Windows::Forms::TabPage());
			this->tabPage3 = (gcnew System::Windows::Forms::TabPage());
			this->tableLayoutPanel16 = (gcnew System::Windows::Forms::TableLayoutPanel());
			this->tableLayoutPanel17 = (gcnew System::Windows::Forms::TableLayoutPanel());
			this->cBoneListCtrl = (gcnew sqr_tools::CBoneListCtrl());
			this->cBoneBoxListCtrl = (gcnew sqr_tools::CBoneBoxListCtrl());
			this->tableLayoutPanel18 = (gcnew System::Windows::Forms::TableLayoutPanel());
			this->cBoneBoxEditCtrl = (gcnew sqr_tools::CBoneBoxEditCtrl());
			this->menuStrip = (gcnew System::Windows::Forms::MenuStrip());
			this->文件ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->打开模型ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->保存模型ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->保存模型为ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->保存全部ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->关闭所有模型文件ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->tableLayoutPanel9 = (gcnew System::Windows::Forms::TableLayoutPanel());
			this->cFrameScaleCtrl = (gcnew sqr_tools::CFrameScaleCtrl());
			this->tableLayoutPanel15 = (gcnew System::Windows::Forms::TableLayoutPanel());
			this->cKeyFrameCtrl = (gcnew sqr_tools::CKeyFrameCtrl());
			this->cGroundSpeedCtrl = (gcnew sqr_tools::CGroundSpeedCtrl());
			this->tableLayoutPanel4 = (gcnew System::Windows::Forms::TableLayoutPanel());
			this->tableLayoutPanel5 = (gcnew System::Windows::Forms::TableLayoutPanel());
			this->tableLayoutPanel6 = (gcnew System::Windows::Forms::TableLayoutPanel());
			this->tableLayoutPanel7 = (gcnew System::Windows::Forms::TableLayoutPanel());
			this->tableLayoutPanel8 = (gcnew System::Windows::Forms::TableLayoutPanel());
			this->crtsCameraCom = (gcnew sqr_tools::CRTSCameraCom());
			this->设置ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->工作路径ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->tableLayoutPanel1->SuspendLayout();
			this->tableLayoutPanel2->SuspendLayout();
			this->tableLayoutPanel3->SuspendLayout();
			this->tabControl1->SuspendLayout();
			this->tabPage1->SuspendLayout();
			this->tableLayoutPanel10->SuspendLayout();
			this->tableLayoutPanel11->SuspendLayout();
			this->tableLayoutPanel12->SuspendLayout();
			this->tableLayoutPanel13->SuspendLayout();
			this->tableLayoutPanel14->SuspendLayout();
			this->tabPage3->SuspendLayout();
			this->tableLayoutPanel16->SuspendLayout();
			this->tableLayoutPanel17->SuspendLayout();
			this->tableLayoutPanel18->SuspendLayout();
			this->menuStrip->SuspendLayout();
			this->tableLayoutPanel9->SuspendLayout();
			this->tableLayoutPanel15->SuspendLayout();
			this->tableLayoutPanel4->SuspendLayout();
			this->tableLayoutPanel5->SuspendLayout();
			this->tableLayoutPanel7->SuspendLayout();
			this->SuspendLayout();
			// 
			// tableLayoutPanel1
			// 
			this->tableLayoutPanel1->ColumnCount = 1;
			this->tableLayoutPanel1->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent, 
				100)));
			this->tableLayoutPanel1->Controls->Add(this->tableLayoutPanel2, 0, 0);
			this->tableLayoutPanel1->Controls->Add(this->tableLayoutPanel9, 0, 1);
			this->tableLayoutPanel1->Dock = System::Windows::Forms::DockStyle::Fill;
			this->tableLayoutPanel1->Location = System::Drawing::Point(0, 0);
			this->tableLayoutPanel1->Margin = System::Windows::Forms::Padding(0);
			this->tableLayoutPanel1->Name = L"tableLayoutPanel1";
			this->tableLayoutPanel1->RowCount = 2;
			this->tableLayoutPanel1->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 100)));
			this->tableLayoutPanel1->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 96)));
			this->tableLayoutPanel1->Size = System::Drawing::Size(991, 620);
			this->tableLayoutPanel1->TabIndex = 0;
			// 
			// tableLayoutPanel2
			// 
			this->tableLayoutPanel2->ColumnCount = 1;
			this->tableLayoutPanel2->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent, 
				100)));
			this->tableLayoutPanel2->Controls->Add(this->tableLayoutPanel3, 0, 1);
			this->tableLayoutPanel2->Controls->Add(this->menuStrip, 0, 0);
			this->tableLayoutPanel2->Dock = System::Windows::Forms::DockStyle::Fill;
			this->tableLayoutPanel2->Location = System::Drawing::Point(0, 0);
			this->tableLayoutPanel2->Margin = System::Windows::Forms::Padding(0);
			this->tableLayoutPanel2->Name = L"tableLayoutPanel2";
			this->tableLayoutPanel2->RowCount = 2;
			this->tableLayoutPanel2->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 20)));
			this->tableLayoutPanel2->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 100)));
			this->tableLayoutPanel2->Size = System::Drawing::Size(991, 524);
			this->tableLayoutPanel2->TabIndex = 0;
			// 
			// tableLayoutPanel3
			// 
			this->tableLayoutPanel3->ColumnCount = 2;
			this->tableLayoutPanel3->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent, 
				46.41776F)));
			this->tableLayoutPanel3->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent, 
				53.58224F)));
			this->tableLayoutPanel3->Controls->Add(this->cViewCtrl, 0, 0);
			this->tableLayoutPanel3->Controls->Add(this->tabControl1, 1, 0);
			this->tableLayoutPanel3->Dock = System::Windows::Forms::DockStyle::Fill;
			this->tableLayoutPanel3->Location = System::Drawing::Point(0, 20);
			this->tableLayoutPanel3->Margin = System::Windows::Forms::Padding(0);
			this->tableLayoutPanel3->Name = L"tableLayoutPanel3";
			this->tableLayoutPanel3->RowCount = 1;
			this->tableLayoutPanel3->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 100)));
			this->tableLayoutPanel3->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 504)));
			this->tableLayoutPanel3->Size = System::Drawing::Size(991, 504);
			this->tableLayoutPanel3->TabIndex = 0;
			// 
			// cViewCtrl
			// 
			this->cViewCtrl->BackColor = System::Drawing::Color::Gray;
			this->cViewCtrl->ContextComponent = this->cContextComponent;
			this->cViewCtrl->Dock = System::Windows::Forms::DockStyle::Fill;
			this->cViewCtrl->Location = System::Drawing::Point(0, 0);
			this->cViewCtrl->Margin = System::Windows::Forms::Padding(0);
			this->cViewCtrl->Name = L"cViewCtrl";
			this->cViewCtrl->Size = System::Drawing::Size(459, 504);
			this->cViewCtrl->TabIndex = 0;
			// 
			// cContextComponent
			// 
			this->cContextComponent->AmbientColor = System::Drawing::SystemColors::ActiveCaptionText;
			this->cContextComponent->SceneName = nullptr;
			this->cContextComponent->ShowBlock = false;
			this->cContextComponent->ShowGrid = false;
			this->cContextComponent->SunColor = System::Drawing::Color::GreenYellow;
			// 
			// tabControl1
			// 
			this->tabControl1->Controls->Add(this->tabPage1);
			this->tabControl1->Controls->Add(this->tabPage2);
			this->tabControl1->Controls->Add(this->tabPage3);
			this->tabControl1->Dock = System::Windows::Forms::DockStyle::Fill;
			this->tabControl1->Location = System::Drawing::Point(459, 0);
			this->tabControl1->Margin = System::Windows::Forms::Padding(0);
			this->tabControl1->Name = L"tabControl1";
			this->tabControl1->SelectedIndex = 0;
			this->tabControl1->Size = System::Drawing::Size(532, 504);
			this->tabControl1->TabIndex = 1;
			// 
			// tabPage1
			// 
			this->tabPage1->Controls->Add(this->tableLayoutPanel10);
			this->tabPage1->Location = System::Drawing::Point(4, 21);
			this->tabPage1->Name = L"tabPage1";
			this->tabPage1->Padding = System::Windows::Forms::Padding(3);
			this->tabPage1->Size = System::Drawing::Size(524, 479);
			this->tabPage1->TabIndex = 0;
			this->tabPage1->Text = L"Mesh和动画";
			this->tabPage1->UseVisualStyleBackColor = true;
			// 
			// tableLayoutPanel10
			// 
			this->tableLayoutPanel10->ColumnCount = 1;
			this->tableLayoutPanel10->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent, 
				100)));
			this->tableLayoutPanel10->Controls->Add(this->tableLayoutPanel11, 0, 0);
			this->tableLayoutPanel10->Controls->Add(this->tableLayoutPanel13, 0, 1);
			this->tableLayoutPanel10->Dock = System::Windows::Forms::DockStyle::Fill;
			this->tableLayoutPanel10->Location = System::Drawing::Point(3, 3);
			this->tableLayoutPanel10->Margin = System::Windows::Forms::Padding(2, 0, 0, 0);
			this->tableLayoutPanel10->Name = L"tableLayoutPanel10";
			this->tableLayoutPanel10->RowCount = 2;
			this->tableLayoutPanel10->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 50)));
			this->tableLayoutPanel10->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 50)));
			this->tableLayoutPanel10->Size = System::Drawing::Size(518, 473);
			this->tableLayoutPanel10->TabIndex = 2;
			// 
			// tableLayoutPanel11
			// 
			this->tableLayoutPanel11->ColumnCount = 2;
			this->tableLayoutPanel11->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent, 
				32.89474F)));
			this->tableLayoutPanel11->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent, 
				67.10526F)));
			this->tableLayoutPanel11->Controls->Add(this->tableLayoutPanel12, 1, 0);
			this->tableLayoutPanel11->Controls->Add(this->cPieceGroupEditCtrl, 0, 0);
			this->tableLayoutPanel11->Dock = System::Windows::Forms::DockStyle::Fill;
			this->tableLayoutPanel11->Location = System::Drawing::Point(1, 0);
			this->tableLayoutPanel11->Margin = System::Windows::Forms::Padding(1, 0, 0, 0);
			this->tableLayoutPanel11->Name = L"tableLayoutPanel11";
			this->tableLayoutPanel11->RowCount = 1;
			this->tableLayoutPanel11->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 100)));
			this->tableLayoutPanel11->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 
				236)));
			this->tableLayoutPanel11->Size = System::Drawing::Size(517, 236);
			this->tableLayoutPanel11->TabIndex = 0;
			// 
			// tableLayoutPanel12
			// 
			this->tableLayoutPanel12->ColumnCount = 2;
			this->tableLayoutPanel12->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent, 
				50)));
			this->tableLayoutPanel12->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent, 
				50)));
			this->tableLayoutPanel12->Controls->Add(this->cPieceUnitEditCtrl, 0, 0);
			this->tableLayoutPanel12->Controls->Add(this->cBoundingBoxPropertyGrid, 1, 0);
			this->tableLayoutPanel12->Dock = System::Windows::Forms::DockStyle::Fill;
			this->tableLayoutPanel12->Location = System::Drawing::Point(170, 0);
			this->tableLayoutPanel12->Margin = System::Windows::Forms::Padding(0);
			this->tableLayoutPanel12->Name = L"tableLayoutPanel12";
			this->tableLayoutPanel12->RowCount = 1;
			this->tableLayoutPanel12->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 100)));
			this->tableLayoutPanel12->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 
				236)));
			this->tableLayoutPanel12->Size = System::Drawing::Size(347, 236);
			this->tableLayoutPanel12->TabIndex = 1;
			// 
			// cPieceUnitEditCtrl
			// 
			this->cPieceUnitEditCtrl->ContextComponent = this->cContextComponent;
			this->cPieceUnitEditCtrl->Dock = System::Windows::Forms::DockStyle::Fill;
			this->cPieceUnitEditCtrl->Location = System::Drawing::Point(0, 0);
			this->cPieceUnitEditCtrl->Margin = System::Windows::Forms::Padding(0);
			this->cPieceUnitEditCtrl->Name = L"cPieceUnitEditCtrl";
			this->cPieceUnitEditCtrl->Size = System::Drawing::Size(173, 236);
			this->cPieceUnitEditCtrl->TabIndex = 0;
			// 
			// cBoundingBoxPropertyGrid
			// 
			this->cBoundingBoxPropertyGrid->Dock = System::Windows::Forms::DockStyle::Fill;
			this->cBoundingBoxPropertyGrid->Location = System::Drawing::Point(173, 0);
			this->cBoundingBoxPropertyGrid->Margin = System::Windows::Forms::Padding(0);
			this->cBoundingBoxPropertyGrid->Name = L"cBoundingBoxPropertyGrid";
			this->cBoundingBoxPropertyGrid->SelectedObject = this->cBoundingBoxPropCom;
			this->cBoundingBoxPropertyGrid->Size = System::Drawing::Size(174, 236);
			this->cBoundingBoxPropertyGrid->TabIndex = 1;
			// 
			// cBoundingBoxPropCom
			// 
			this->cBoundingBoxPropCom->ContextComponent = this->cContextComponent;
			this->cBoundingBoxPropCom->ControlParent = this->cBoundingBoxPropertyGrid;
			this->cBoundingBoxPropCom->showBigBox = false;
			this->cBoundingBoxPropCom->showSmaBox = false;
			this->cBoundingBoxPropCom->xDelta = 0;
			this->cBoundingBoxPropCom->xSize = 0;
			this->cBoundingBoxPropCom->yDelta = 0;
			this->cBoundingBoxPropCom->ySize = 0;
			this->cBoundingBoxPropCom->zDelta = 0;
			this->cBoundingBoxPropCom->zSize = 0;
			// 
			// cPieceGroupEditCtrl
			// 
			this->cPieceGroupEditCtrl->ContextComponent = this->cContextComponent;
			this->cPieceGroupEditCtrl->Dock = System::Windows::Forms::DockStyle::Fill;
			this->cPieceGroupEditCtrl->Location = System::Drawing::Point(0, 0);
			this->cPieceGroupEditCtrl->Margin = System::Windows::Forms::Padding(0);
			this->cPieceGroupEditCtrl->Name = L"cPieceGroupEditCtrl";
			this->cPieceGroupEditCtrl->Size = System::Drawing::Size(170, 236);
			this->cPieceGroupEditCtrl->TabIndex = 2;
			// 
			// tableLayoutPanel13
			// 
			this->tableLayoutPanel13->ColumnCount = 2;
			this->tableLayoutPanel13->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent, 
				32.89F)));
			this->tableLayoutPanel13->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent, 
				67.11F)));
			this->tableLayoutPanel13->Controls->Add(this->tableLayoutPanel14, 1, 0);
			this->tableLayoutPanel13->Controls->Add(this->cAnimationGroupEditCtrl, 0, 0);
			this->tableLayoutPanel13->Dock = System::Windows::Forms::DockStyle::Fill;
			this->tableLayoutPanel13->Location = System::Drawing::Point(1, 236);
			this->tableLayoutPanel13->Margin = System::Windows::Forms::Padding(1, 0, 0, 0);
			this->tableLayoutPanel13->Name = L"tableLayoutPanel13";
			this->tableLayoutPanel13->RowCount = 1;
			this->tableLayoutPanel13->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 100)));
			this->tableLayoutPanel13->Size = System::Drawing::Size(517, 237);
			this->tableLayoutPanel13->TabIndex = 1;
			// 
			// tableLayoutPanel14
			// 
			this->tableLayoutPanel14->ColumnCount = 2;
			this->tableLayoutPanel14->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent, 
				50)));
			this->tableLayoutPanel14->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent, 
				50)));
			this->tableLayoutPanel14->Controls->Add(this->cSkeletonPropertyGrid, 0, 0);
			this->tableLayoutPanel14->Dock = System::Windows::Forms::DockStyle::Fill;
			this->tableLayoutPanel14->Location = System::Drawing::Point(170, 0);
			this->tableLayoutPanel14->Margin = System::Windows::Forms::Padding(0);
			this->tableLayoutPanel14->Name = L"tableLayoutPanel14";
			this->tableLayoutPanel14->RowCount = 1;
			this->tableLayoutPanel14->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 100)));
			this->tableLayoutPanel14->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 
				237)));
			this->tableLayoutPanel14->Size = System::Drawing::Size(347, 237);
			this->tableLayoutPanel14->TabIndex = 1;
			// 
			// cSkeletonPropertyGrid
			// 
			this->cSkeletonPropertyGrid->Dock = System::Windows::Forms::DockStyle::Fill;
			this->cSkeletonPropertyGrid->Location = System::Drawing::Point(0, 0);
			this->cSkeletonPropertyGrid->Margin = System::Windows::Forms::Padding(0);
			this->cSkeletonPropertyGrid->Name = L"cSkeletonPropertyGrid";
			this->cSkeletonPropertyGrid->SelectedObject = this->cSkeletonPropCom;
			this->cSkeletonPropertyGrid->Size = System::Drawing::Size(173, 237);
			this->cSkeletonPropertyGrid->TabIndex = 0;
			// 
			// cSkeletonPropCom
			// 
			this->cSkeletonPropCom->ContextComponent = this->cContextComponent;
			this->cSkeletonPropCom->ControlParent = this->cSkeletonPropertyGrid;
			this->cSkeletonPropCom->保存骨骼文件 = nullptr;
			this->cSkeletonPropCom->当前选中骨骼 = nullptr;
			this->cSkeletonPropCom->显示骨骼 = false;
			this->cSkeletonPropCom->调整骨骼大小 = 0;
			// 
			// cAnimationGroupEditCtrl
			// 
			this->cAnimationGroupEditCtrl->ContextComponent = this->cContextComponent;
			this->cAnimationGroupEditCtrl->Dock = System::Windows::Forms::DockStyle::Fill;
			this->cAnimationGroupEditCtrl->Location = System::Drawing::Point(0, 0);
			this->cAnimationGroupEditCtrl->Margin = System::Windows::Forms::Padding(0);
			this->cAnimationGroupEditCtrl->Name = L"cAnimationGroupEditCtrl";
			this->cAnimationGroupEditCtrl->Size = System::Drawing::Size(170, 237);
			this->cAnimationGroupEditCtrl->TabIndex = 2;
			// 
			// tabPage2
			// 
			this->tabPage2->Location = System::Drawing::Point(4, 21);
			this->tabPage2->Name = L"tabPage2";
			this->tabPage2->Padding = System::Windows::Forms::Padding(3);
			this->tabPage2->Size = System::Drawing::Size(524, 479);
			this->tabPage2->TabIndex = 1;
			this->tabPage2->Text = L"渲染类型";
			this->tabPage2->UseVisualStyleBackColor = true;
			// 
			// tabPage3
			// 
			this->tabPage3->Controls->Add(this->tableLayoutPanel16);
			this->tabPage3->Location = System::Drawing::Point(4, 21);
			this->tabPage3->Name = L"tabPage3";
			this->tabPage3->Size = System::Drawing::Size(524, 479);
			this->tabPage3->TabIndex = 0;
			this->tabPage3->Text = L"骨骼包围盒";
			this->tabPage3->UseVisualStyleBackColor = true;
			// 
			// tableLayoutPanel16
			// 
			this->tableLayoutPanel16->ColumnCount = 1;
			this->tableLayoutPanel16->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent, 
				100)));
			this->tableLayoutPanel16->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Absolute, 
				20)));
			this->tableLayoutPanel16->Controls->Add(this->tableLayoutPanel17, 0, 0);
			this->tableLayoutPanel16->Controls->Add(this->tableLayoutPanel18, 0, 1);
			this->tableLayoutPanel16->Dock = System::Windows::Forms::DockStyle::Fill;
			this->tableLayoutPanel16->Location = System::Drawing::Point(0, 0);
			this->tableLayoutPanel16->Margin = System::Windows::Forms::Padding(2, 0, 0, 0);
			this->tableLayoutPanel16->Name = L"tableLayoutPanel16";
			this->tableLayoutPanel16->RowCount = 2;
			this->tableLayoutPanel16->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 50)));
			this->tableLayoutPanel16->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 50)));
			this->tableLayoutPanel16->Size = System::Drawing::Size(524, 479);
			this->tableLayoutPanel16->TabIndex = 0;
			// 
			// tableLayoutPanel17
			// 
			this->tableLayoutPanel17->ColumnCount = 2;
			this->tableLayoutPanel17->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent, 
				50)));
			this->tableLayoutPanel17->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent, 
				50)));
			this->tableLayoutPanel17->Controls->Add(this->cBoneListCtrl, 0, 0);
			this->tableLayoutPanel17->Controls->Add(this->cBoneBoxListCtrl, 1, 0);
			this->tableLayoutPanel17->Dock = System::Windows::Forms::DockStyle::Fill;
			this->tableLayoutPanel17->Location = System::Drawing::Point(0, 0);
			this->tableLayoutPanel17->Margin = System::Windows::Forms::Padding(0);
			this->tableLayoutPanel17->Name = L"tableLayoutPanel17";
			this->tableLayoutPanel17->RowCount = 1;
			this->tableLayoutPanel17->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 100)));
			this->tableLayoutPanel17->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 
				239)));
			this->tableLayoutPanel17->Size = System::Drawing::Size(524, 239);
			this->tableLayoutPanel17->TabIndex = 0;
			// 
			// cBoneListCtrl
			// 
			this->cBoneListCtrl->ContextComponent = this->cContextComponent;
			this->cBoneListCtrl->Dock = System::Windows::Forms::DockStyle::Fill;
			this->cBoneListCtrl->Location = System::Drawing::Point(0, 0);
			this->cBoneListCtrl->Margin = System::Windows::Forms::Padding(0);
			this->cBoneListCtrl->Name = L"cBoneListCtrl";
			this->cBoneListCtrl->Size = System::Drawing::Size(262, 239);
			this->cBoneListCtrl->TabIndex = 0;
			// 
			// cBoneBoxListCtrl
			// 
			this->cBoneBoxListCtrl->ContextComponent = this->cContextComponent;
			this->cBoneBoxListCtrl->Dock = System::Windows::Forms::DockStyle::Fill;
			this->cBoneBoxListCtrl->Location = System::Drawing::Point(262, 0);
			this->cBoneBoxListCtrl->Margin = System::Windows::Forms::Padding(0);
			this->cBoneBoxListCtrl->Name = L"cBoneBoxListCtrl";
			this->cBoneBoxListCtrl->Size = System::Drawing::Size(262, 239);
			this->cBoneBoxListCtrl->TabIndex = 1;
			// 
			// tableLayoutPanel18
			// 
			this->tableLayoutPanel18->ColumnCount = 2;
			this->tableLayoutPanel18->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent, 
				50)));
			this->tableLayoutPanel18->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent, 
				50)));
			this->tableLayoutPanel18->Controls->Add(this->cBoneBoxEditCtrl, 0, 0);
			this->tableLayoutPanel18->Dock = System::Windows::Forms::DockStyle::Fill;
			this->tableLayoutPanel18->Location = System::Drawing::Point(3, 242);
			this->tableLayoutPanel18->Name = L"tableLayoutPanel18";
			this->tableLayoutPanel18->RowCount = 1;
			this->tableLayoutPanel18->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 100)));
			this->tableLayoutPanel18->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 
				234)));
			this->tableLayoutPanel18->Size = System::Drawing::Size(518, 234);
			this->tableLayoutPanel18->TabIndex = 1;
			// 
			// cBoneBoxEditCtrl
			// 
			this->cBoneBoxEditCtrl->ContextComponent = this->cContextComponent;
			this->cBoneBoxEditCtrl->Dock = System::Windows::Forms::DockStyle::Fill;
			this->cBoneBoxEditCtrl->Location = System::Drawing::Point(0, 0);
			this->cBoneBoxEditCtrl->Margin = System::Windows::Forms::Padding(0);
			this->cBoneBoxEditCtrl->Name = L"cBoneBoxEditCtrl";
			this->cBoneBoxEditCtrl->Size = System::Drawing::Size(259, 234);
			this->cBoneBoxEditCtrl->TabIndex = 0;
			// 
			// menuStrip
			// 
			this->menuStrip->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(2) {this->文件ToolStripMenuItem, 
				this->设置ToolStripMenuItem});
			this->menuStrip->Location = System::Drawing::Point(0, 0);
			this->menuStrip->Name = L"menuStrip";
			this->menuStrip->Size = System::Drawing::Size(991, 20);
			this->menuStrip->TabIndex = 1;
			this->menuStrip->Text = L"menuStrip";
			// 
			// 文件ToolStripMenuItem
			// 
			this->文件ToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(5) {this->打开模型ToolStripMenuItem, 
				this->保存模型ToolStripMenuItem, this->保存模型为ToolStripMenuItem, this->保存全部ToolStripMenuItem, this->关闭所有模型文件ToolStripMenuItem});
			this->文件ToolStripMenuItem->Name = L"文件ToolStripMenuItem";
			this->文件ToolStripMenuItem->Size = System::Drawing::Size(43, 16);
			this->文件ToolStripMenuItem->Text = L"文件";
			// 
			// 打开模型ToolStripMenuItem
			// 
			this->打开模型ToolStripMenuItem->Name = L"打开模型ToolStripMenuItem";
			this->打开模型ToolStripMenuItem->Size = System::Drawing::Size(170, 22);
			this->打开模型ToolStripMenuItem->Text = L"打开模型";
			this->打开模型ToolStripMenuItem->Click += gcnew System::EventHandler(this, &ModelEditorForm::打开模型ToolStripMenuItem_Click);
			// 
			// 保存模型ToolStripMenuItem
			// 
			this->保存模型ToolStripMenuItem->Name = L"保存模型ToolStripMenuItem";
			this->保存模型ToolStripMenuItem->Size = System::Drawing::Size(170, 22);
			this->保存模型ToolStripMenuItem->Text = L"保存模型";
			this->保存模型ToolStripMenuItem->Click += gcnew System::EventHandler(this, &ModelEditorForm::保存模型ToolStripMenuItem_Click);
			// 
			// 保存模型为ToolStripMenuItem
			// 
			this->保存模型为ToolStripMenuItem->Name = L"保存模型为ToolStripMenuItem";
			this->保存模型为ToolStripMenuItem->Size = System::Drawing::Size(170, 22);
			this->保存模型为ToolStripMenuItem->Text = L"保存模型为";
			this->保存模型为ToolStripMenuItem->Click += gcnew System::EventHandler(this, &ModelEditorForm::保存模型为ToolStripMenuItem_Click);
			// 
			// 保存全部ToolStripMenuItem
			// 
			this->保存全部ToolStripMenuItem->Name = L"保存全部ToolStripMenuItem";
			this->保存全部ToolStripMenuItem->Size = System::Drawing::Size(170, 22);
			this->保存全部ToolStripMenuItem->Text = L"保存全部";
			this->保存全部ToolStripMenuItem->Click += gcnew System::EventHandler(this, &ModelEditorForm::保存全部ToolStripMenuItem_Click);
			// 
			// 关闭所有模型文件ToolStripMenuItem
			// 
			this->关闭所有模型文件ToolStripMenuItem->Name = L"关闭所有模型文件ToolStripMenuItem";
			this->关闭所有模型文件ToolStripMenuItem->Size = System::Drawing::Size(170, 22);
			this->关闭所有模型文件ToolStripMenuItem->Text = L"关闭所有模型文件";
			this->关闭所有模型文件ToolStripMenuItem->Click += gcnew System::EventHandler(this, &ModelEditorForm::关闭所有模型文件ToolStripMenuItem_Click);
			// 
			// tableLayoutPanel9
			// 
			this->tableLayoutPanel9->ColumnCount = 1;
			this->tableLayoutPanel9->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent, 
				100)));
			this->tableLayoutPanel9->Controls->Add(this->cFrameScaleCtrl, 0, 0);
			this->tableLayoutPanel9->Controls->Add(this->tableLayoutPanel15, 0, 1);
			this->tableLayoutPanel9->Dock = System::Windows::Forms::DockStyle::Fill;
			this->tableLayoutPanel9->Location = System::Drawing::Point(0, 524);
			this->tableLayoutPanel9->Margin = System::Windows::Forms::Padding(0);
			this->tableLayoutPanel9->Name = L"tableLayoutPanel9";
			this->tableLayoutPanel9->RowCount = 2;
			this->tableLayoutPanel9->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 40)));
			this->tableLayoutPanel9->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 100)));
			this->tableLayoutPanel9->Size = System::Drawing::Size(991, 96);
			this->tableLayoutPanel9->TabIndex = 1;
			// 
			// cFrameScaleCtrl
			// 
			this->cFrameScaleCtrl->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->cFrameScaleCtrl->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->cFrameScaleCtrl->ContextComponent = this->cContextComponent;
			this->cFrameScaleCtrl->Location = System::Drawing::Point(0, 0);
			this->cFrameScaleCtrl->Margin = System::Windows::Forms::Padding(0);
			this->cFrameScaleCtrl->Name = L"cFrameScaleCtrl";
			this->cFrameScaleCtrl->Size = System::Drawing::Size(991, 40);
			this->cFrameScaleCtrl->TabIndex = 0;
			// 
			// tableLayoutPanel15
			// 
			this->tableLayoutPanel15->ColumnCount = 2;
			this->tableLayoutPanel15->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Absolute, 
				419)));
			this->tableLayoutPanel15->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent, 
				100)));
			this->tableLayoutPanel15->Controls->Add(this->cKeyFrameCtrl, 0, 0);
			this->tableLayoutPanel15->Controls->Add(this->cGroundSpeedCtrl, 1, 0);
			this->tableLayoutPanel15->Dock = System::Windows::Forms::DockStyle::Fill;
			this->tableLayoutPanel15->Location = System::Drawing::Point(3, 43);
			this->tableLayoutPanel15->Name = L"tableLayoutPanel15";
			this->tableLayoutPanel15->RowCount = 1;
			this->tableLayoutPanel15->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 100)));
			this->tableLayoutPanel15->Size = System::Drawing::Size(985, 50);
			this->tableLayoutPanel15->TabIndex = 1;
			// 
			// cKeyFrameCtrl
			// 
			this->cKeyFrameCtrl->ContextComponent = this->cContextComponent;
			this->cKeyFrameCtrl->Dock = System::Windows::Forms::DockStyle::Fill;
			this->cKeyFrameCtrl->Location = System::Drawing::Point(0, 0);
			this->cKeyFrameCtrl->Margin = System::Windows::Forms::Padding(0);
			this->cKeyFrameCtrl->Name = L"cKeyFrameCtrl";
			this->cKeyFrameCtrl->Size = System::Drawing::Size(419, 50);
			this->cKeyFrameCtrl->TabIndex = 0;
			// 
			// cGroundSpeedCtrl
			// 
			this->cGroundSpeedCtrl->ContextComponent = this->cContextComponent;
			this->cGroundSpeedCtrl->Location = System::Drawing::Point(419, 0);
			this->cGroundSpeedCtrl->Margin = System::Windows::Forms::Padding(0);
			this->cGroundSpeedCtrl->Name = L"cGroundSpeedCtrl";
			this->cGroundSpeedCtrl->Size = System::Drawing::Size(244, 50);
			this->cGroundSpeedCtrl->TabIndex = 1;
			// 
			// tableLayoutPanel4
			// 
			this->tableLayoutPanel4->ColumnCount = 1;
			this->tableLayoutPanel4->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent, 
				100)));
			this->tableLayoutPanel4->Controls->Add(this->tableLayoutPanel5, 0, 0);
			this->tableLayoutPanel4->Dock = System::Windows::Forms::DockStyle::Fill;
			this->tableLayoutPanel4->Location = System::Drawing::Point(0, 0);
			this->tableLayoutPanel4->Margin = System::Windows::Forms::Padding(2, 0, 0, 0);
			this->tableLayoutPanel4->Name = L"tableLayoutPanel4";
			this->tableLayoutPanel4->RowCount = 2;
			this->tableLayoutPanel4->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 20)));
			this->tableLayoutPanel4->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 20)));
			this->tableLayoutPanel4->Size = System::Drawing::Size(200, 100);
			this->tableLayoutPanel4->TabIndex = 0;
			// 
			// tableLayoutPanel5
			// 
			this->tableLayoutPanel5->ColumnCount = 2;
			this->tableLayoutPanel5->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent, 
				32.89474F)));
			this->tableLayoutPanel5->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent, 
				67.10526F)));
			this->tableLayoutPanel5->Controls->Add(this->tableLayoutPanel6, 1, 0);
			this->tableLayoutPanel5->Dock = System::Windows::Forms::DockStyle::Fill;
			this->tableLayoutPanel5->Location = System::Drawing::Point(1, 0);
			this->tableLayoutPanel5->Margin = System::Windows::Forms::Padding(1, 0, 0, 0);
			this->tableLayoutPanel5->Name = L"tableLayoutPanel5";
			this->tableLayoutPanel5->RowCount = 1;
			this->tableLayoutPanel5->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 100)));
			this->tableLayoutPanel5->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 233)));
			this->tableLayoutPanel5->Size = System::Drawing::Size(199, 20);
			this->tableLayoutPanel5->TabIndex = 0;
			// 
			// tableLayoutPanel6
			// 
			this->tableLayoutPanel6->ColumnCount = 2;
			this->tableLayoutPanel6->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent, 
				50)));
			this->tableLayoutPanel6->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent, 
				50)));
			this->tableLayoutPanel6->Dock = System::Windows::Forms::DockStyle::Fill;
			this->tableLayoutPanel6->Location = System::Drawing::Point(65, 0);
			this->tableLayoutPanel6->Margin = System::Windows::Forms::Padding(0);
			this->tableLayoutPanel6->Name = L"tableLayoutPanel6";
			this->tableLayoutPanel6->RowCount = 1;
			this->tableLayoutPanel6->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 100)));
			this->tableLayoutPanel6->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 233)));
			this->tableLayoutPanel6->Size = System::Drawing::Size(134, 20);
			this->tableLayoutPanel6->TabIndex = 1;
			// 
			// tableLayoutPanel7
			// 
			this->tableLayoutPanel7->ColumnCount = 2;
			this->tableLayoutPanel7->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent, 
				32.89F)));
			this->tableLayoutPanel7->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent, 
				67.11F)));
			this->tableLayoutPanel7->Controls->Add(this->tableLayoutPanel8, 1, 0);
			this->tableLayoutPanel7->Dock = System::Windows::Forms::DockStyle::Fill;
			this->tableLayoutPanel7->Location = System::Drawing::Point(1, 216);
			this->tableLayoutPanel7->Margin = System::Windows::Forms::Padding(1, 0, 0, 0);
			this->tableLayoutPanel7->Name = L"tableLayoutPanel7";
			this->tableLayoutPanel7->RowCount = 1;
			this->tableLayoutPanel7->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 100)));
			this->tableLayoutPanel7->Size = System::Drawing::Size(514, 217);
			this->tableLayoutPanel7->TabIndex = 1;
			// 
			// tableLayoutPanel8
			// 
			this->tableLayoutPanel8->ColumnCount = 2;
			this->tableLayoutPanel8->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent, 
				50)));
			this->tableLayoutPanel8->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent, 
				50)));
			this->tableLayoutPanel8->Dock = System::Windows::Forms::DockStyle::Fill;
			this->tableLayoutPanel8->Location = System::Drawing::Point(169, 0);
			this->tableLayoutPanel8->Margin = System::Windows::Forms::Padding(0);
			this->tableLayoutPanel8->Name = L"tableLayoutPanel8";
			this->tableLayoutPanel8->RowCount = 1;
			this->tableLayoutPanel8->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 100)));
			this->tableLayoutPanel8->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 234)));
			this->tableLayoutPanel8->Size = System::Drawing::Size(345, 217);
			this->tableLayoutPanel8->TabIndex = 1;
			// 
			// crtsCameraCom
			// 
			this->crtsCameraCom->ContextComponent = this->cContextComponent;
			// 
			// 设置ToolStripMenuItem
			// 
			this->设置ToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) {this->工作路径ToolStripMenuItem});
			this->设置ToolStripMenuItem->Name = L"设置ToolStripMenuItem";
			this->设置ToolStripMenuItem->Size = System::Drawing::Size(43, 16);
			this->设置ToolStripMenuItem->Text = L"设置";
			// 
			// 工作路径ToolStripMenuItem
			// 
			this->工作路径ToolStripMenuItem->Name = L"工作路径ToolStripMenuItem";
			this->工作路径ToolStripMenuItem->Size = System::Drawing::Size(152, 22);
			this->工作路径ToolStripMenuItem->Text = L"工作路径";
			this->工作路径ToolStripMenuItem->Click += gcnew System::EventHandler(this, &ModelEditorForm::工作路径ToolStripMenuItem_Click);
			// 
			// ModelEditorForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(991, 620);
			this->Controls->Add(this->tableLayoutPanel1);
			this->MainMenuStrip = this->menuStrip;
			this->Name = L"ModelEditorForm";
			this->SizeGripStyle = System::Windows::Forms::SizeGripStyle::Hide;
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = L"ModelEditorForm";
			this->FormClosed += gcnew System::Windows::Forms::FormClosedEventHandler(this, &ModelEditorForm::ModelEditorForm_FormClosed);
			this->Load += gcnew System::EventHandler(this, &ModelEditorForm::ModelEditorForm_Load);
			this->tableLayoutPanel1->ResumeLayout(false);
			this->tableLayoutPanel2->ResumeLayout(false);
			this->tableLayoutPanel2->PerformLayout();
			this->tableLayoutPanel3->ResumeLayout(false);
			this->tabControl1->ResumeLayout(false);
			this->tabPage1->ResumeLayout(false);
			this->tableLayoutPanel10->ResumeLayout(false);
			this->tableLayoutPanel11->ResumeLayout(false);
			this->tableLayoutPanel12->ResumeLayout(false);
			this->tableLayoutPanel13->ResumeLayout(false);
			this->tableLayoutPanel14->ResumeLayout(false);
			this->tabPage3->ResumeLayout(false);
			this->tableLayoutPanel16->ResumeLayout(false);
			this->tableLayoutPanel17->ResumeLayout(false);
			this->tableLayoutPanel18->ResumeLayout(false);
			this->menuStrip->ResumeLayout(false);
			this->menuStrip->PerformLayout();
			this->tableLayoutPanel9->ResumeLayout(false);
			this->tableLayoutPanel15->ResumeLayout(false);
			this->tableLayoutPanel4->ResumeLayout(false);
			this->tableLayoutPanel5->ResumeLayout(false);
			this->tableLayoutPanel7->ResumeLayout(false);
			this->ResumeLayout(false);

		}
#pragma endregion
		private: System::Void ModelEditorForm_FormClosed(System::Object^  sender, System::Windows::Forms::FormClosedEventArgs^  e);
		private: System::Void ModelEditorForm_Load(System::Object^  sender, System::EventArgs^  e);
		private: System::Void 打开模型ToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
		private: System::Void 保存模型ToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
		private: System::Void 保存模型为ToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
		private: System::Void 保存全部ToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
		private: System::Void 关闭所有模型文件ToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
		private: System::Void 工作路径ToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
};
}
