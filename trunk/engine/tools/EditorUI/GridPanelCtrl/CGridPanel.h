#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

#include "CUIBase.h"

namespace sqr_tools {

	/// <summary>
	/// Summary for CGridPanel
	/// </summary>
	public ref class CGridPanel : public CUIBase
	{
	public:
		CGridPanel(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
			this->trackBarBrush->Minimum = 1;
			this->trackBarBrush->Maximum = 32;
			this->trackBarBrush->LargeChange = 1;
			this->trackBarBrush->SmallChange = 1;
			this->trackBarBrush->TickFrequency = 1;//每次移动的刻度
			this->trackBarBrush->Value = 4; //初始位置
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~CGridPanel()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::GroupBox^  OpLevelGroup;
	private: System::Windows::Forms::GroupBox^  OpHeightGroup;
	private: System::Windows::Forms::GroupBox^  BrushTypeGroup;
	private: System::Windows::Forms::GroupBox^  LogicHeightGroup;
	private: System::Windows::Forms::CheckBox^  checkWave;
	private: System::Windows::Forms::RadioButton^  rbWater;
	private: System::Windows::Forms::RadioButton^  rbTerrain;
	private: System::Windows::Forms::GroupBox^  MeshTmpGroup;
	private: System::Windows::Forms::RadioButton^  rbLogicHeight;
	private: System::Windows::Forms::RadioButton^  rbPhysicHeight;
	private: System::Windows::Forms::GroupBox^  groupBox3;
	private: System::Windows::Forms::RadioButton^  rbDirBoth;
	private: System::Windows::Forms::RadioButton^  rbDirZ;


	private: System::Windows::Forms::RadioButton^  rbDirX;
	private: System::Windows::Forms::GroupBox^  groupBox2;
	private: System::Windows::Forms::RadioButton^  rbGaosi;
	private: System::Windows::Forms::RadioButton^  rbLine;
	private: System::Windows::Forms::GroupBox^  groupBox1;

	private: System::Windows::Forms::RadioButton^  rbSmooth;

	private: System::Windows::Forms::RadioButton^  rbAlign;
	private: System::Windows::Forms::RadioButton^  rbPush;
	private: System::Windows::Forms::RadioButton^  rbPull;
	private: System::Windows::Forms::TrackBar^  trackBarBrush;
	private: System::Windows::Forms::Button^  btLogicHeight;
	private: System::Windows::Forms::TextBox^  textLogicHeight;
	private: System::Windows::Forms::Label^  labelLogicHeight;
	private: System::Windows::Forms::RadioButton^  rbVolumnHead;
	private: System::Windows::Forms::RadioButton^  rbGaosiHead;
	private: System::Windows::Forms::Label^  labelStrength;
	private: System::Windows::Forms::Label^  labelCutPos;
	private: System::Windows::Forms::TrackBar^  trackBarStrength;
	private: System::Windows::Forms::TrackBar^  trackBarCutPos;
	private: System::Windows::Forms::Label^  labelBrush;

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
			this->OpLevelGroup = (gcnew System::Windows::Forms::GroupBox());
			this->checkWave = (gcnew System::Windows::Forms::CheckBox());
			this->rbWater = (gcnew System::Windows::Forms::RadioButton());
			this->rbTerrain = (gcnew System::Windows::Forms::RadioButton());
			this->OpHeightGroup = (gcnew System::Windows::Forms::GroupBox());
			this->rbLogicHeight = (gcnew System::Windows::Forms::RadioButton());
			this->rbPhysicHeight = (gcnew System::Windows::Forms::RadioButton());
			this->BrushTypeGroup = (gcnew System::Windows::Forms::GroupBox());
			this->trackBarBrush = (gcnew System::Windows::Forms::TrackBar());
			this->labelBrush = (gcnew System::Windows::Forms::Label());
			this->groupBox3 = (gcnew System::Windows::Forms::GroupBox());
			this->rbDirBoth = (gcnew System::Windows::Forms::RadioButton());
			this->rbDirZ = (gcnew System::Windows::Forms::RadioButton());
			this->rbDirX = (gcnew System::Windows::Forms::RadioButton());
			this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
			this->rbPush = (gcnew System::Windows::Forms::RadioButton());
			this->rbPull = (gcnew System::Windows::Forms::RadioButton());
			this->groupBox2 = (gcnew System::Windows::Forms::GroupBox());
			this->rbAlign = (gcnew System::Windows::Forms::RadioButton());
			this->rbSmooth = (gcnew System::Windows::Forms::RadioButton());
			this->rbGaosi = (gcnew System::Windows::Forms::RadioButton());
			this->rbLine = (gcnew System::Windows::Forms::RadioButton());
			this->LogicHeightGroup = (gcnew System::Windows::Forms::GroupBox());
			this->btLogicHeight = (gcnew System::Windows::Forms::Button());
			this->textLogicHeight = (gcnew System::Windows::Forms::TextBox());
			this->labelLogicHeight = (gcnew System::Windows::Forms::Label());
			this->MeshTmpGroup = (gcnew System::Windows::Forms::GroupBox());
			this->labelStrength = (gcnew System::Windows::Forms::Label());
			this->labelCutPos = (gcnew System::Windows::Forms::Label());
			this->trackBarStrength = (gcnew System::Windows::Forms::TrackBar());
			this->trackBarCutPos = (gcnew System::Windows::Forms::TrackBar());
			this->rbVolumnHead = (gcnew System::Windows::Forms::RadioButton());
			this->rbGaosiHead = (gcnew System::Windows::Forms::RadioButton());
			this->OpLevelGroup->SuspendLayout();
			this->OpHeightGroup->SuspendLayout();
			this->BrushTypeGroup->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->trackBarBrush))->BeginInit();
			this->groupBox3->SuspendLayout();
			this->groupBox1->SuspendLayout();
			this->groupBox2->SuspendLayout();
			this->LogicHeightGroup->SuspendLayout();
			this->MeshTmpGroup->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->trackBarStrength))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->trackBarCutPos))->BeginInit();
			this->SuspendLayout();
			// 
			// OpLevelGroup
			// 
			this->OpLevelGroup->Controls->Add(this->checkWave);
			this->OpLevelGroup->Controls->Add(this->rbWater);
			this->OpLevelGroup->Controls->Add(this->rbTerrain);
			this->OpLevelGroup->Location = System::Drawing::Point(10, 10);
			this->OpLevelGroup->Name = L"OpLevelGroup";
			this->OpLevelGroup->Size = System::Drawing::Size(238, 45);
			this->OpLevelGroup->TabIndex = 0;
			this->OpLevelGroup->TabStop = false;
			this->OpLevelGroup->Text = L"操作层";
			// 
			// checkWave
			// 
			this->checkWave->AutoSize = true;
			this->checkWave->Checked = true;
			this->checkWave->CheckState = System::Windows::Forms::CheckState::Checked;
			this->checkWave->Location = System::Drawing::Point(172, 21);
			this->checkWave->Name = L"checkWave";
			this->checkWave->Size = System::Drawing::Size(48, 16);
			this->checkWave->TabIndex = 1;
			this->checkWave->Text = L"浪花";
			this->checkWave->UseVisualStyleBackColor = true;
			this->checkWave->CheckedChanged += gcnew System::EventHandler(this, &CGridPanel::checkWave_CheckedChanged);
			// 
			// rbWater
			// 
			this->rbWater->AutoSize = true;
			this->rbWater->Location = System::Drawing::Point(95, 20);
			this->rbWater->Name = L"rbWater";
			this->rbWater->Size = System::Drawing::Size(59, 16);
			this->rbWater->TabIndex = 0;
			this->rbWater->Text = L"水面层";
			this->rbWater->UseVisualStyleBackColor = true;
			this->rbWater->CheckedChanged += gcnew System::EventHandler(this, &CGridPanel::rbWater_CheckedChanged);
			// 
			// rbTerrain
			// 
			this->rbTerrain->AutoSize = true;
			this->rbTerrain->Location = System::Drawing::Point(15, 20);
			this->rbTerrain->Name = L"rbTerrain";
			this->rbTerrain->Size = System::Drawing::Size(59, 16);
			this->rbTerrain->TabIndex = 0;
			this->rbTerrain->Text = L"地表层";
			this->rbTerrain->UseVisualStyleBackColor = true;
			this->rbTerrain->CheckedChanged += gcnew System::EventHandler(this, &CGridPanel::rbTerrain_CheckedChanged);
			// 
			// OpHeightGroup
			// 
			this->OpHeightGroup->Controls->Add(this->rbLogicHeight);
			this->OpHeightGroup->Controls->Add(this->rbPhysicHeight);
			this->OpHeightGroup->Location = System::Drawing::Point(10, 63);
			this->OpHeightGroup->Name = L"OpHeightGroup";
			this->OpHeightGroup->Size = System::Drawing::Size(238, 45);
			this->OpHeightGroup->TabIndex = 0;
			this->OpHeightGroup->TabStop = false;
			this->OpHeightGroup->Text = L"操作高度";
			// 
			// rbLogicHeight
			// 
			this->rbLogicHeight->AutoSize = true;
			this->rbLogicHeight->Location = System::Drawing::Point(95, 20);
			this->rbLogicHeight->Name = L"rbLogicHeight";
			this->rbLogicHeight->Size = System::Drawing::Size(71, 16);
			this->rbLogicHeight->TabIndex = 0;
			this->rbLogicHeight->TabStop = true;
			this->rbLogicHeight->Text = L"逻辑高度";
			this->rbLogicHeight->UseVisualStyleBackColor = true;
			this->rbLogicHeight->CheckedChanged += gcnew System::EventHandler(this, &CGridPanel::rbLogicHeight_CheckedChanged);
			// 
			// rbPhysicHeight
			// 
			this->rbPhysicHeight->AutoSize = true;
			this->rbPhysicHeight->Location = System::Drawing::Point(15, 20);
			this->rbPhysicHeight->Name = L"rbPhysicHeight";
			this->rbPhysicHeight->Size = System::Drawing::Size(71, 16);
			this->rbPhysicHeight->TabIndex = 0;
			this->rbPhysicHeight->Text = L"物理高度";
			this->rbPhysicHeight->UseVisualStyleBackColor = true;
			this->rbPhysicHeight->CheckedChanged += gcnew System::EventHandler(this, &CGridPanel::rbPhysicHeight_CheckedChanged);
			// 
			// BrushTypeGroup
			// 
			this->BrushTypeGroup->Controls->Add(this->trackBarBrush);
			this->BrushTypeGroup->Controls->Add(this->labelBrush);
			this->BrushTypeGroup->Controls->Add(this->groupBox3);
			this->BrushTypeGroup->Controls->Add(this->groupBox1);
			this->BrushTypeGroup->Controls->Add(this->groupBox2);
			this->BrushTypeGroup->Location = System::Drawing::Point(10, 123);
			this->BrushTypeGroup->Name = L"BrushTypeGroup";
			this->BrushTypeGroup->Size = System::Drawing::Size(238, 234);
			this->BrushTypeGroup->TabIndex = 0;
			this->BrushTypeGroup->TabStop = false;
			this->BrushTypeGroup->Text = L"画刷";
			// 
			// trackBarBrush
			// 
			this->trackBarBrush->Location = System::Drawing::Point(15, 176);
			this->trackBarBrush->Maximum = 32;
			this->trackBarBrush->Name = L"trackBarBrush";
			this->trackBarBrush->Size = System::Drawing::Size(205, 45);
			this->trackBarBrush->TabIndex = 3;
			this->trackBarBrush->Value = 16;
			this->trackBarBrush->Scroll += gcnew System::EventHandler(this, &CGridPanel::trackBarBrush_Scroll);
			// 
			// labelBrush
			// 
			this->labelBrush->AutoSize = true;
			this->labelBrush->Location = System::Drawing::Point(95, 161);
			this->labelBrush->Name = L"labelBrush";
			this->labelBrush->Size = System::Drawing::Size(53, 12);
			this->labelBrush->TabIndex = 4;
			this->labelBrush->Text = L"画刷强度";
			// 
			// groupBox3
			// 
			this->groupBox3->Controls->Add(this->rbDirBoth);
			this->groupBox3->Controls->Add(this->rbDirZ);
			this->groupBox3->Controls->Add(this->rbDirX);
			this->groupBox3->Location = System::Drawing::Point(15, 108);
			this->groupBox3->Name = L"groupBox3";
			this->groupBox3->Size = System::Drawing::Size(205, 42);
			this->groupBox3->TabIndex = 2;
			this->groupBox3->TabStop = false;
			// 
			// rbDirBoth
			// 
			this->rbDirBoth->AutoSize = true;
			this->rbDirBoth->Enabled = false;
			this->rbDirBoth->Location = System::Drawing::Point(140, 16);
			this->rbDirBoth->Name = L"rbDirBoth";
			this->rbDirBoth->Size = System::Drawing::Size(47, 16);
			this->rbDirBoth->TabIndex = 0;
			this->rbDirBoth->Text = L"双向";
			this->rbDirBoth->UseVisualStyleBackColor = true;
			this->rbDirBoth->CheckedChanged += gcnew System::EventHandler(this, &CGridPanel::rbDirBoth_CheckedChanged);
			// 
			// rbDirZ
			// 
			this->rbDirZ->AutoSize = true;
			this->rbDirZ->Enabled = false;
			this->rbDirZ->Location = System::Drawing::Point(75, 16);
			this->rbDirZ->Name = L"rbDirZ";
			this->rbDirZ->Size = System::Drawing::Size(53, 16);
			this->rbDirZ->TabIndex = 0;
			this->rbDirZ->Text = L"Z方向";
			this->rbDirZ->UseVisualStyleBackColor = true;
			this->rbDirZ->CheckedChanged += gcnew System::EventHandler(this, &CGridPanel::rbDirZ_CheckedChanged);
			// 
			// rbDirX
			// 
			this->rbDirX->AutoSize = true;
			this->rbDirX->Enabled = false;
			this->rbDirX->Location = System::Drawing::Point(10, 16);
			this->rbDirX->Name = L"rbDirX";
			this->rbDirX->Size = System::Drawing::Size(53, 16);
			this->rbDirX->TabIndex = 0;
			this->rbDirX->Text = L"X方向";
			this->rbDirX->UseVisualStyleBackColor = true;
			this->rbDirX->CheckedChanged += gcnew System::EventHandler(this, &CGridPanel::rbDirX_CheckedChanged);
			// 
			// groupBox1
			// 
			this->groupBox1->Controls->Add(this->rbPush);
			this->groupBox1->Controls->Add(this->rbPull);
			this->groupBox1->Location = System::Drawing::Point(15, 62);
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(205, 42);
			this->groupBox1->TabIndex = 0;
			this->groupBox1->TabStop = false;
			// 
			// rbPush
			// 
			this->rbPush->AutoSize = true;
			this->rbPush->Location = System::Drawing::Point(125, 20);
			this->rbPush->Name = L"rbPush";
			this->rbPush->Size = System::Drawing::Size(47, 16);
			this->rbPush->TabIndex = 0;
			this->rbPush->Text = L"降低";
			this->rbPush->UseVisualStyleBackColor = true;
			this->rbPush->CheckedChanged += gcnew System::EventHandler(this, &CGridPanel::rbPush_CheckedChanged);
			// 
			// rbPull
			// 
			this->rbPull->AutoSize = true;
			this->rbPull->Location = System::Drawing::Point(33, 20);
			this->rbPull->Name = L"rbPull";
			this->rbPull->Size = System::Drawing::Size(47, 16);
			this->rbPull->TabIndex = 0;
			this->rbPull->Text = L"增加";
			this->rbPull->UseVisualStyleBackColor = true;
			this->rbPull->CheckedChanged += gcnew System::EventHandler(this, &CGridPanel::rbPull_CheckedChanged);
			// 
			// groupBox2
			// 
			this->groupBox2->Controls->Add(this->rbAlign);
			this->groupBox2->Controls->Add(this->rbSmooth);
			this->groupBox2->Controls->Add(this->rbGaosi);
			this->groupBox2->Controls->Add(this->rbLine);
			this->groupBox2->Location = System::Drawing::Point(15, 16);
			this->groupBox2->Name = L"groupBox2";
			this->groupBox2->Size = System::Drawing::Size(205, 42);
			this->groupBox2->TabIndex = 1;
			this->groupBox2->TabStop = false;
			// 
			// rbAlign
			// 
			this->rbAlign->AutoSize = true;
			this->rbAlign->Location = System::Drawing::Point(152, 18);
			this->rbAlign->Name = L"rbAlign";
			this->rbAlign->Size = System::Drawing::Size(47, 16);
			this->rbAlign->TabIndex = 0;
			this->rbAlign->Text = L"对齐";
			this->rbAlign->UseVisualStyleBackColor = true;
			this->rbAlign->CheckedChanged += gcnew System::EventHandler(this, &CGridPanel::rbAlign_CheckedChanged);
			// 
			// rbSmooth
			// 
			this->rbSmooth->AutoSize = true;
			this->rbSmooth->Location = System::Drawing::Point(104, 18);
			this->rbSmooth->Name = L"rbSmooth";
			this->rbSmooth->Size = System::Drawing::Size(47, 16);
			this->rbSmooth->TabIndex = 0;
			this->rbSmooth->Text = L"平滑";
			this->rbSmooth->UseVisualStyleBackColor = true;
			this->rbSmooth->CheckedChanged += gcnew System::EventHandler(this, &CGridPanel::rbSmooth_CheckedChanged);
			// 
			// rbGaosi
			// 
			this->rbGaosi->AutoSize = true;
			this->rbGaosi->Location = System::Drawing::Point(54, 18);
			this->rbGaosi->Name = L"rbGaosi";
			this->rbGaosi->Size = System::Drawing::Size(47, 16);
			this->rbGaosi->TabIndex = 0;
			this->rbGaosi->Text = L"高斯";
			this->rbGaosi->UseVisualStyleBackColor = true;
			this->rbGaosi->CheckedChanged += gcnew System::EventHandler(this, &CGridPanel::rbGaosi_CheckedChanged);
			this->rbGaosi->EnabledChanged += gcnew System::EventHandler(this, &CGridPanel::rbGaosi_EnabledChanged);
			// 
			// rbLine
			// 
			this->rbLine->AutoSize = true;
			this->rbLine->Location = System::Drawing::Point(6, 18);
			this->rbLine->Name = L"rbLine";
			this->rbLine->Size = System::Drawing::Size(47, 16);
			this->rbLine->TabIndex = 0;
			this->rbLine->Text = L"线性";
			this->rbLine->UseVisualStyleBackColor = true;
			this->rbLine->CheckedChanged += gcnew System::EventHandler(this, &CGridPanel::rbLine_CheckedChanged);
			this->rbLine->EnabledChanged += gcnew System::EventHandler(this, &CGridPanel::rbLine_EnabledChanged);
			// 
			// LogicHeightGroup
			// 
			this->LogicHeightGroup->Controls->Add(this->btLogicHeight);
			this->LogicHeightGroup->Controls->Add(this->textLogicHeight);
			this->LogicHeightGroup->Controls->Add(this->labelLogicHeight);
			this->LogicHeightGroup->Location = System::Drawing::Point(10, 363);
			this->LogicHeightGroup->Name = L"LogicHeightGroup";
			this->LogicHeightGroup->Size = System::Drawing::Size(238, 60);
			this->LogicHeightGroup->TabIndex = 0;
			this->LogicHeightGroup->TabStop = false;
			this->LogicHeightGroup->Text = L"物体逻辑高设置";
			// 
			// btLogicHeight
			// 
			this->btLogicHeight->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->btLogicHeight->Location = System::Drawing::Point(168, 23);
			this->btLogicHeight->Name = L"btLogicHeight";
			this->btLogicHeight->Size = System::Drawing::Size(55, 24);
			this->btLogicHeight->TabIndex = 2;
			this->btLogicHeight->Text = L"确定";
			this->btLogicHeight->UseVisualStyleBackColor = true;
			this->btLogicHeight->Click += gcnew System::EventHandler(this, &CGridPanel::btLogicHeight_Click);
			// 
			// textLogicHeight
			// 
			this->textLogicHeight->Location = System::Drawing::Point(66, 24);
			this->textLogicHeight->Name = L"textLogicHeight";
			this->textLogicHeight->Size = System::Drawing::Size(88, 21);
			this->textLogicHeight->TabIndex = 1;
			// 
			// labelLogicHeight
			// 
			this->labelLogicHeight->AutoSize = true;
			this->labelLogicHeight->Location = System::Drawing::Point(15, 29);
			this->labelLogicHeight->Name = L"labelLogicHeight";
			this->labelLogicHeight->Size = System::Drawing::Size(47, 12);
			this->labelLogicHeight->TabIndex = 0;
			this->labelLogicHeight->Text = L"逻辑高:";
			// 
			// MeshTmpGroup
			// 
			this->MeshTmpGroup->Controls->Add(this->labelStrength);
			this->MeshTmpGroup->Controls->Add(this->labelCutPos);
			this->MeshTmpGroup->Controls->Add(this->trackBarStrength);
			this->MeshTmpGroup->Controls->Add(this->trackBarCutPos);
			this->MeshTmpGroup->Controls->Add(this->rbVolumnHead);
			this->MeshTmpGroup->Controls->Add(this->rbGaosiHead);
			this->MeshTmpGroup->Location = System::Drawing::Point(10, 429);
			this->MeshTmpGroup->Name = L"MeshTmpGroup";
			this->MeshTmpGroup->Size = System::Drawing::Size(238, 203);
			this->MeshTmpGroup->TabIndex = 0;
			this->MeshTmpGroup->TabStop = false;
			this->MeshTmpGroup->Text = L"MeshTemplate";
			// 
			// labelStrength
			// 
			this->labelStrength->AutoSize = true;
			this->labelStrength->Location = System::Drawing::Point(95, 128);
			this->labelStrength->Name = L"labelStrength";
			this->labelStrength->Size = System::Drawing::Size(53, 12);
			this->labelStrength->TabIndex = 4;
			this->labelStrength->Text = L"拉伸强度";
			// 
			// labelCutPos
			// 
			this->labelCutPos->AutoSize = true;
			this->labelCutPos->Location = System::Drawing::Point(95, 64);
			this->labelCutPos->Name = L"labelCutPos";
			this->labelCutPos->Size = System::Drawing::Size(53, 12);
			this->labelCutPos->TabIndex = 4;
			this->labelCutPos->Text = L"截取位置";
			// 
			// trackBarStrength
			// 
			this->trackBarStrength->Location = System::Drawing::Point(10, 153);
			this->trackBarStrength->Name = L"trackBarStrength";
			this->trackBarStrength->Size = System::Drawing::Size(205, 45);
			this->trackBarStrength->TabIndex = 3;
			// 
			// trackBarCutPos
			// 
			this->trackBarCutPos->Location = System::Drawing::Point(10, 80);
			this->trackBarCutPos->Name = L"trackBarCutPos";
			this->trackBarCutPos->Size = System::Drawing::Size(205, 45);
			this->trackBarCutPos->TabIndex = 3;
			// 
			// rbVolumnHead
			// 
			this->rbVolumnHead->AutoSize = true;
			this->rbVolumnHead->Location = System::Drawing::Point(95, 29);
			this->rbVolumnHead->Name = L"rbVolumnHead";
			this->rbVolumnHead->Size = System::Drawing::Size(83, 16);
			this->rbVolumnHead->TabIndex = 1;
			this->rbVolumnHead->Text = L"四棱锥截头";
			this->rbVolumnHead->UseVisualStyleBackColor = true;
			this->rbVolumnHead->CheckedChanged += gcnew System::EventHandler(this, &CGridPanel::rbVolumnHead_CheckedChanged);
			// 
			// rbGaosiHead
			// 
			this->rbGaosiHead->AutoSize = true;
			this->rbGaosiHead->Checked = true;
			this->rbGaosiHead->Location = System::Drawing::Point(10, 29);
			this->rbGaosiHead->Name = L"rbGaosiHead";
			this->rbGaosiHead->Size = System::Drawing::Size(71, 16);
			this->rbGaosiHead->TabIndex = 0;
			this->rbGaosiHead->TabStop = true;
			this->rbGaosiHead->Text = L"高斯截头";
			this->rbGaosiHead->UseVisualStyleBackColor = true;
			this->rbGaosiHead->CheckedChanged += gcnew System::EventHandler(this, &CGridPanel::rbGaosiHead_CheckedChanged);
			// 
			// CGridPanel
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->AutoScroll = true;
			this->Controls->Add(this->MeshTmpGroup);
			this->Controls->Add(this->LogicHeightGroup);
			this->Controls->Add(this->BrushTypeGroup);
			this->Controls->Add(this->OpHeightGroup);
			this->Controls->Add(this->OpLevelGroup);
			this->Name = L"CGridPanel";
			this->Size = System::Drawing::Size(258, 668);
			this->OpLevelGroup->ResumeLayout(false);
			this->OpLevelGroup->PerformLayout();
			this->OpHeightGroup->ResumeLayout(false);
			this->OpHeightGroup->PerformLayout();
			this->BrushTypeGroup->ResumeLayout(false);
			this->BrushTypeGroup->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->trackBarBrush))->EndInit();
			this->groupBox3->ResumeLayout(false);
			this->groupBox3->PerformLayout();
			this->groupBox1->ResumeLayout(false);
			this->groupBox1->PerformLayout();
			this->groupBox2->ResumeLayout(false);
			this->groupBox2->PerformLayout();
			this->LogicHeightGroup->ResumeLayout(false);
			this->LogicHeightGroup->PerformLayout();
			this->MeshTmpGroup->ResumeLayout(false);
			this->MeshTmpGroup->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->trackBarStrength))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->trackBarCutPos))->EndInit();
			this->ResumeLayout(false);

		}
#pragma endregion
public:  virtual void Update(void) override;
public:  virtual char*	GetImpTypeName(void)	override;

private: System::Void rbTerrain_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
private: System::Void rbWater_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
private: System::Void checkWave_CheckedChanged(System::Object^  sender, System::EventArgs^  e);

private: System::Void rbPhysicHeight_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
private: System::Void rbLogicHeight_CheckedChanged(System::Object^  sender, System::EventArgs^  e);

private: System::Void rbSmooth_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
private: System::Void rbAlign_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
private: System::Void rbLine_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
private: System::Void rbGaosi_CheckedChanged(System::Object^  sender, System::EventArgs^  e);

private: System::Void rbPull_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
private: System::Void rbPush_CheckedChanged(System::Object^  sender, System::EventArgs^  e);

private: System::Void rbDirX_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
private: System::Void rbDirZ_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
private: System::Void rbDirBoth_CheckedChanged(System::Object^  sender, System::EventArgs^  e);

private: System::Void btLogicHeight_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void rbGaosiHead_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
private: System::Void rbVolumnHead_CheckedChanged(System::Object^  sender, System::EventArgs^  e);

private: System::Void rbLine_EnabledChanged(System::Object^  sender, System::EventArgs^  e) ;
private: System::Void rbGaosi_EnabledChanged(System::Object^  sender, System::EventArgs^  e) ;
private: System::Void trackBarBrush_Scroll(System::Object^  sender, System::EventArgs^  e);
};
}
