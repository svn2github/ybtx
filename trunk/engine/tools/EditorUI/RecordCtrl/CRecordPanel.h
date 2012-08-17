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
	/// Summary for CRecordPanel
	/// </summary>
	public ref class CRecordPanel : public CUIBase
	{
	public:
		CRecordPanel(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
			m_LastSelectAnimate=-1;
			m_LastSelectKey=-1;
			m_PreView=false;
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~CRecordPanel()
		{
			if (components)
			{
				delete components;
			}
		}
	private: int m_LastSelectAnimate;
	private: int m_LastSelectKey;
	protected: bool m_PreView;

	private: System::Windows::Forms::GroupBox^  AnimateGroup;
	private: System::Windows::Forms::ListView^  AnimateList;
	private: System::Windows::Forms::ColumnHeader^  Animate_ID;
	private: System::Windows::Forms::ColumnHeader^  Animate_Num;
	private: System::Windows::Forms::Button^  AnimateDel;
	private: System::Windows::Forms::Button^  AnimateAdd;
	private: System::Windows::Forms::Panel^  AnimatePanel;

	private: System::Windows::Forms::Panel^  KeyFramePanel;
	private: System::Windows::Forms::GroupBox^  KeyFrameGroup;
	private: System::Windows::Forms::ListView^  KeyFrameList;
	private: System::Windows::Forms::ColumnHeader^  KeyFrameID;
	private: System::Windows::Forms::ColumnHeader^  KeyFrameNum;
	private: System::Windows::Forms::Button^  KeyFrameStop;
	private: System::Windows::Forms::Button^  KeyFrameDel;
	private: System::Windows::Forms::Button^  KeyFrameModify;
	private: System::Windows::Forms::Button^  KeyFrameAdd;
	private: System::Windows::Forms::CheckBox^  KeyFrameShow;

	private: System::Windows::Forms::Panel^  AnimateCtrlPanel;
	private: System::Windows::Forms::GroupBox^  AnimateCtrlGroup;
	private: System::Windows::Forms::Button^  AnimateReplay;
	private: System::Windows::Forms::Button^  AnimatePlay;
	private: System::Windows::Forms::Button^  AnimateStop;

	private: System::Windows::Forms::Panel^  RecordCamPanel;
	private: System::Windows::Forms::GroupBox^  CamCtrlGroup;
	private: System::Windows::Forms::Label^  CamCtrlLabelSpeed;
	private: System::Windows::Forms::Label^  CamCtrlLabelFov;
	private: System::Windows::Forms::RadioButton^  CamCtrltTarget;
	private: System::Windows::Forms::RadioButton^  CamCtrlPos;
	private: System::Windows::Forms::TextBox^  CamSpeedEdit;
	private: System::Windows::Forms::TextBox^  CamFovEdit;
	private: System::Windows::Forms::Label^  CamCtrlLabelTarget;
	private: System::Windows::Forms::Label^  CamCtrlLabelPos;
	private: System::Windows::Forms::Label^  label6;
	private: System::Windows::Forms::Label^  label5;
	private: System::Windows::Forms::Label^  label4;
	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::TextBox^  CamPosXEdit;
	private: System::Windows::Forms::TextBox^  CamPosYEdit;
	private: System::Windows::Forms::TextBox^  CamPosZEdit;
	private: System::Windows::Forms::TextBox^  CamDirXEdit;
	private: System::Windows::Forms::TextBox^  CamDirYEdit;
	private: System::Windows::Forms::TextBox^  CamDirZEdit;
	private: System::Windows::Forms::Button^  CamCtrModify;
	private: System::Windows::Forms::Panel^  AddKeyPanel;
	private: System::Windows::Forms::Label^  AddKeyLable;
	private: System::Windows::Forms::Button^  btKeyTimeN;
	private: System::Windows::Forms::Button^  btKeyTimeY;
	private: System::Windows::Forms::TextBox^  EditKeytime;
	private: System::Windows::Forms::Label^  CamCtrlLabelTime;
	private: System::Windows::Forms::TextBox^  CamTimeEdit;

	protected: 

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
			this->AnimateGroup = (gcnew System::Windows::Forms::GroupBox());
			this->AnimateDel = (gcnew System::Windows::Forms::Button());
			this->AnimateAdd = (gcnew System::Windows::Forms::Button());
			this->AnimateList = (gcnew System::Windows::Forms::ListView());
			this->Animate_ID = (gcnew System::Windows::Forms::ColumnHeader());
			this->Animate_Num = (gcnew System::Windows::Forms::ColumnHeader());
			this->AnimatePanel = (gcnew System::Windows::Forms::Panel());
			this->KeyFramePanel = (gcnew System::Windows::Forms::Panel());
			this->KeyFrameGroup = (gcnew System::Windows::Forms::GroupBox());
			this->AddKeyPanel = (gcnew System::Windows::Forms::Panel());
			this->btKeyTimeN = (gcnew System::Windows::Forms::Button());
			this->btKeyTimeY = (gcnew System::Windows::Forms::Button());
			this->EditKeytime = (gcnew System::Windows::Forms::TextBox());
			this->AddKeyLable = (gcnew System::Windows::Forms::Label());
			this->KeyFrameShow = (gcnew System::Windows::Forms::CheckBox());
			this->KeyFrameStop = (gcnew System::Windows::Forms::Button());
			this->KeyFrameDel = (gcnew System::Windows::Forms::Button());
			this->KeyFrameModify = (gcnew System::Windows::Forms::Button());
			this->KeyFrameAdd = (gcnew System::Windows::Forms::Button());
			this->KeyFrameList = (gcnew System::Windows::Forms::ListView());
			this->KeyFrameID = (gcnew System::Windows::Forms::ColumnHeader());
			this->KeyFrameNum = (gcnew System::Windows::Forms::ColumnHeader());
			this->AnimateCtrlPanel = (gcnew System::Windows::Forms::Panel());
			this->AnimateCtrlGroup = (gcnew System::Windows::Forms::GroupBox());
			this->AnimateReplay = (gcnew System::Windows::Forms::Button());
			this->AnimatePlay = (gcnew System::Windows::Forms::Button());
			this->AnimateStop = (gcnew System::Windows::Forms::Button());
			this->RecordCamPanel = (gcnew System::Windows::Forms::Panel());
			this->CamCtrlGroup = (gcnew System::Windows::Forms::GroupBox());
			this->CamTimeEdit = (gcnew System::Windows::Forms::TextBox());
			this->CamCtrlLabelTime = (gcnew System::Windows::Forms::Label());
			this->CamCtrModify = (gcnew System::Windows::Forms::Button());
			this->label6 = (gcnew System::Windows::Forms::Label());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->CamCtrlLabelTarget = (gcnew System::Windows::Forms::Label());
			this->CamCtrlLabelPos = (gcnew System::Windows::Forms::Label());
			this->CamDirZEdit = (gcnew System::Windows::Forms::TextBox());
			this->CamPosZEdit = (gcnew System::Windows::Forms::TextBox());
			this->CamDirYEdit = (gcnew System::Windows::Forms::TextBox());
			this->CamPosYEdit = (gcnew System::Windows::Forms::TextBox());
			this->CamDirXEdit = (gcnew System::Windows::Forms::TextBox());
			this->CamPosXEdit = (gcnew System::Windows::Forms::TextBox());
			this->CamSpeedEdit = (gcnew System::Windows::Forms::TextBox());
			this->CamFovEdit = (gcnew System::Windows::Forms::TextBox());
			this->CamCtrlLabelSpeed = (gcnew System::Windows::Forms::Label());
			this->CamCtrlLabelFov = (gcnew System::Windows::Forms::Label());
			this->CamCtrltTarget = (gcnew System::Windows::Forms::RadioButton());
			this->CamCtrlPos = (gcnew System::Windows::Forms::RadioButton());
			this->AnimateGroup->SuspendLayout();
			this->AnimatePanel->SuspendLayout();
			this->KeyFramePanel->SuspendLayout();
			this->KeyFrameGroup->SuspendLayout();
			this->AddKeyPanel->SuspendLayout();
			this->AnimateCtrlPanel->SuspendLayout();
			this->AnimateCtrlGroup->SuspendLayout();
			this->RecordCamPanel->SuspendLayout();
			this->CamCtrlGroup->SuspendLayout();
			this->SuspendLayout();
			// 
			// AnimateGroup
			// 
			this->AnimateGroup->Controls->Add(this->AnimateDel);
			this->AnimateGroup->Controls->Add(this->AnimateAdd);
			this->AnimateGroup->Controls->Add(this->AnimateList);
			this->AnimateGroup->Location = System::Drawing::Point(12, 13);
			this->AnimateGroup->Name = L"AnimateGroup";
			this->AnimateGroup->Size = System::Drawing::Size(240, 240);
			this->AnimateGroup->TabIndex = 0;
			this->AnimateGroup->TabStop = false;
			this->AnimateGroup->Text = L"动画面板";
			// 
			// AnimateDel
			// 
			this->AnimateDel->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->AnimateDel->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->AnimateDel->Location = System::Drawing::Point(168, 206);
			this->AnimateDel->Name = L"AnimateDel";
			this->AnimateDel->Size = System::Drawing::Size(50, 24);
			this->AnimateDel->TabIndex = 2;
			this->AnimateDel->Text = L"删除";
			this->AnimateDel->UseVisualStyleBackColor = true;
			this->AnimateDel->Click += gcnew System::EventHandler(this, &CRecordPanel::DelAnimate);
			// 
			// AnimateAdd
			// 
			this->AnimateAdd->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->AnimateAdd->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->AnimateAdd->Font = (gcnew System::Drawing::Font(L"SimSun", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(134)));
			this->AnimateAdd->Location = System::Drawing::Point(20, 206);
			this->AnimateAdd->Name = L"AnimateAdd";
			this->AnimateAdd->Size = System::Drawing::Size(50, 24);
			this->AnimateAdd->TabIndex = 1;
			this->AnimateAdd->Text = L"添加";
			this->AnimateAdd->UseVisualStyleBackColor = true;
			this->AnimateAdd->Click += gcnew System::EventHandler(this, &CRecordPanel::AddAnimate);
			// 
			// AnimateList
			// 
			this->AnimateList->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left));
			this->AnimateList->BorderStyle = System::Windows::Forms::BorderStyle::None;
			this->AnimateList->Columns->AddRange(gcnew cli::array< System::Windows::Forms::ColumnHeader^  >(2) {this->Animate_ID, this->Animate_Num});
			this->AnimateList->FullRowSelect = true;
			this->AnimateList->GridLines = true;
			this->AnimateList->HeaderStyle = System::Windows::Forms::ColumnHeaderStyle::Nonclickable;
			this->AnimateList->HoverSelection = true;
			this->AnimateList->ImeMode = System::Windows::Forms::ImeMode::NoControl;
			this->AnimateList->Location = System::Drawing::Point(20, 20);
			this->AnimateList->Name = L"AnimateList";
			this->AnimateList->Size = System::Drawing::Size(198, 175);
			this->AnimateList->TabIndex = 0;
			this->AnimateList->UseCompatibleStateImageBehavior = false;
			this->AnimateList->View = System::Windows::Forms::View::Details;
			this->AnimateList->Click += gcnew System::EventHandler(this, &CRecordPanel::AnimateList_Click);
			// 
			// Animate_ID
			// 
			this->Animate_ID->Text = L"动画编号";
			this->Animate_ID->Width = 74;
			// 
			// Animate_Num
			// 
			this->Animate_Num->Text = L"动画时长(s)";
			this->Animate_Num->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			this->Animate_Num->Width = 123;
			// 
			// AnimatePanel
			// 
			this->AnimatePanel->BackColor = System::Drawing::SystemColors::Control;
			this->AnimatePanel->Controls->Add(this->AnimateGroup);
			this->AnimatePanel->Dock = System::Windows::Forms::DockStyle::Top;
			this->AnimatePanel->Location = System::Drawing::Point(0, 0);
			this->AnimatePanel->Name = L"AnimatePanel";
			this->AnimatePanel->Size = System::Drawing::Size(267, 268);
			this->AnimatePanel->TabIndex = 1;
			// 
			// KeyFramePanel
			// 
			this->KeyFramePanel->Controls->Add(this->KeyFrameGroup);
			this->KeyFramePanel->Dock = System::Windows::Forms::DockStyle::Top;
			this->KeyFramePanel->Location = System::Drawing::Point(0, 268);
			this->KeyFramePanel->Name = L"KeyFramePanel";
			this->KeyFramePanel->Size = System::Drawing::Size(267, 288);
			this->KeyFramePanel->TabIndex = 2;
			// 
			// KeyFrameGroup
			// 
			this->KeyFrameGroup->Controls->Add(this->AddKeyPanel);
			this->KeyFrameGroup->Controls->Add(this->KeyFrameShow);
			this->KeyFrameGroup->Controls->Add(this->KeyFrameStop);
			this->KeyFrameGroup->Controls->Add(this->KeyFrameDel);
			this->KeyFrameGroup->Controls->Add(this->KeyFrameModify);
			this->KeyFrameGroup->Controls->Add(this->KeyFrameAdd);
			this->KeyFrameGroup->Controls->Add(this->KeyFrameList);
			this->KeyFrameGroup->Enabled = false;
			this->KeyFrameGroup->Location = System::Drawing::Point(12, 12);
			this->KeyFrameGroup->Name = L"KeyFrameGroup";
			this->KeyFrameGroup->Size = System::Drawing::Size(240, 265);
			this->KeyFrameGroup->TabIndex = 0;
			this->KeyFrameGroup->TabStop = false;
			this->KeyFrameGroup->Text = L"关键帧面板";
			// 
			// AddKeyPanel
			// 
			this->AddKeyPanel->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->AddKeyPanel->Controls->Add(this->btKeyTimeN);
			this->AddKeyPanel->Controls->Add(this->btKeyTimeY);
			this->AddKeyPanel->Controls->Add(this->EditKeytime);
			this->AddKeyPanel->Controls->Add(this->AddKeyLable);
			this->AddKeyPanel->Location = System::Drawing::Point(6, 114);
			this->AddKeyPanel->Name = L"AddKeyPanel";
			this->AddKeyPanel->Size = System::Drawing::Size(196, 82);
			this->AddKeyPanel->TabIndex = 4;
			this->AddKeyPanel->Visible = false;
			// 
			// btKeyTimeN
			// 
			this->btKeyTimeN->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->btKeyTimeN->Location = System::Drawing::Point(150, 39);
			this->btKeyTimeN->Name = L"btKeyTimeN";
			this->btKeyTimeN->Size = System::Drawing::Size(40, 21);
			this->btKeyTimeN->TabIndex = 2;
			this->btKeyTimeN->Text = L"退出";
			this->btKeyTimeN->UseVisualStyleBackColor = true;
			this->btKeyTimeN->Click += gcnew System::EventHandler(this, &CRecordPanel::btKeyTimeN_Click);
			// 
			// btKeyTimeY
			// 
			this->btKeyTimeY->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->btKeyTimeY->Location = System::Drawing::Point(99, 39);
			this->btKeyTimeY->Name = L"btKeyTimeY";
			this->btKeyTimeY->Size = System::Drawing::Size(40, 21);
			this->btKeyTimeY->TabIndex = 2;
			this->btKeyTimeY->Text = L"确认";
			this->btKeyTimeY->UseVisualStyleBackColor = true;
			this->btKeyTimeY->Click += gcnew System::EventHandler(this, &CRecordPanel::btKeyTimeY_Click);
			// 
			// EditKeytime
			// 
			this->EditKeytime->Location = System::Drawing::Point(10, 39);
			this->EditKeytime->Name = L"EditKeytime";
			this->EditKeytime->Size = System::Drawing::Size(80, 21);
			this->EditKeytime->TabIndex = 1;
			this->EditKeytime->TextChanged += gcnew System::EventHandler(this, &CRecordPanel::EditKeytime_TextChanged);
			// 
			// AddKeyLable
			// 
			this->AddKeyLable->AutoSize = true;
			this->AddKeyLable->Location = System::Drawing::Point(9, 13);
			this->AddKeyLable->Name = L"AddKeyLable";
			this->AddKeyLable->Size = System::Drawing::Size(113, 12);
			this->AddKeyLable->TabIndex = 0;
			this->AddKeyLable->Text = L"输入关键帧时间(mm)";
			// 
			// KeyFrameShow
			// 
			this->KeyFrameShow->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->KeyFrameShow->AutoSize = true;
			this->KeyFrameShow->Location = System::Drawing::Point(168, 239);
			this->KeyFrameShow->Name = L"KeyFrameShow";
			this->KeyFrameShow->Size = System::Drawing::Size(48, 16);
			this->KeyFrameShow->TabIndex = 3;
			this->KeyFrameShow->Text = L"预览";
			this->KeyFrameShow->UseVisualStyleBackColor = true;
			this->KeyFrameShow->CheckedChanged += gcnew System::EventHandler(this, &CRecordPanel::KeyFrameShow_CheckedChanged);
			// 
			// KeyFrameStop
			// 
			this->KeyFrameStop->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->KeyFrameStop->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->KeyFrameStop->Location = System::Drawing::Point(21, 234);
			this->KeyFrameStop->Name = L"KeyFrameStop";
			this->KeyFrameStop->Size = System::Drawing::Size(123, 24);
			this->KeyFrameStop->TabIndex = 2;
			this->KeyFrameStop->Text = L"设置最后悬停帧";
			this->KeyFrameStop->UseVisualStyleBackColor = true;
			this->KeyFrameStop->Click += gcnew System::EventHandler(this, &CRecordPanel::SetLastKeyFrame);
			// 
			// KeyFrameDel
			// 
			this->KeyFrameDel->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->KeyFrameDel->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->KeyFrameDel->Location = System::Drawing::Point(167, 202);
			this->KeyFrameDel->Name = L"KeyFrameDel";
			this->KeyFrameDel->Size = System::Drawing::Size(50, 24);
			this->KeyFrameDel->TabIndex = 1;
			this->KeyFrameDel->Text = L"删除";
			this->KeyFrameDel->UseVisualStyleBackColor = true;
			this->KeyFrameDel->Click += gcnew System::EventHandler(this, &CRecordPanel::DekKeyFrame);
			// 
			// KeyFrameModify
			// 
			this->KeyFrameModify->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->KeyFrameModify->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->KeyFrameModify->Location = System::Drawing::Point(94, 202);
			this->KeyFrameModify->Name = L"KeyFrameModify";
			this->KeyFrameModify->Size = System::Drawing::Size(50, 24);
			this->KeyFrameModify->TabIndex = 1;
			this->KeyFrameModify->Text = L"修改";
			this->KeyFrameModify->UseVisualStyleBackColor = true;
			this->KeyFrameModify->Visible = false;
			this->KeyFrameModify->Click += gcnew System::EventHandler(this, &CRecordPanel::ModifyKeyFrame);
			// 
			// KeyFrameAdd
			// 
			this->KeyFrameAdd->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->KeyFrameAdd->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->KeyFrameAdd->Location = System::Drawing::Point(21, 202);
			this->KeyFrameAdd->Name = L"KeyFrameAdd";
			this->KeyFrameAdd->Size = System::Drawing::Size(50, 24);
			this->KeyFrameAdd->TabIndex = 1;
			this->KeyFrameAdd->Text = L"添加";
			this->KeyFrameAdd->UseVisualStyleBackColor = true;
			this->KeyFrameAdd->Click += gcnew System::EventHandler(this, &CRecordPanel::AddKeyFrame);
			// 
			// KeyFrameList
			// 
			this->KeyFrameList->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left));
			this->KeyFrameList->Columns->AddRange(gcnew cli::array< System::Windows::Forms::ColumnHeader^  >(2) {this->KeyFrameID, this->KeyFrameNum});
			this->KeyFrameList->FullRowSelect = true;
			this->KeyFrameList->GridLines = true;
			this->KeyFrameList->HeaderStyle = System::Windows::Forms::ColumnHeaderStyle::Nonclickable;
			this->KeyFrameList->Location = System::Drawing::Point(20, 20);
			this->KeyFrameList->Name = L"KeyFrameList";
			this->KeyFrameList->Size = System::Drawing::Size(198, 175);
			this->KeyFrameList->TabIndex = 0;
			this->KeyFrameList->UseCompatibleStateImageBehavior = false;
			this->KeyFrameList->View = System::Windows::Forms::View::Details;
			this->KeyFrameList->Click += gcnew System::EventHandler(this, &CRecordPanel::KeyFrameList_Click);
			// 
			// KeyFrameID
			// 
			this->KeyFrameID->Text = L"帧编号";
			this->KeyFrameID->Width = 72;
			// 
			// KeyFrameNum
			// 
			this->KeyFrameNum->Text = L"帧时间(s)";
			this->KeyFrameNum->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			this->KeyFrameNum->Width = 122;
			// 
			// AnimateCtrlPanel
			// 
			this->AnimateCtrlPanel->BackColor = System::Drawing::SystemColors::Control;
			this->AnimateCtrlPanel->Controls->Add(this->AnimateCtrlGroup);
			this->AnimateCtrlPanel->Dock = System::Windows::Forms::DockStyle::Top;
			this->AnimateCtrlPanel->Location = System::Drawing::Point(0, 556);
			this->AnimateCtrlPanel->Name = L"AnimateCtrlPanel";
			this->AnimateCtrlPanel->Size = System::Drawing::Size(267, 78);
			this->AnimateCtrlPanel->TabIndex = 3;
			// 
			// AnimateCtrlGroup
			// 
			this->AnimateCtrlGroup->Controls->Add(this->AnimateReplay);
			this->AnimateCtrlGroup->Controls->Add(this->AnimatePlay);
			this->AnimateCtrlGroup->Controls->Add(this->AnimateStop);
			this->AnimateCtrlGroup->Enabled = false;
			this->AnimateCtrlGroup->Location = System::Drawing::Point(10, 10);
			this->AnimateCtrlGroup->Name = L"AnimateCtrlGroup";
			this->AnimateCtrlGroup->Size = System::Drawing::Size(241, 56);
			this->AnimateCtrlGroup->TabIndex = 0;
			this->AnimateCtrlGroup->TabStop = false;
			this->AnimateCtrlGroup->Text = L"动画控制";
			// 
			// AnimateReplay
			// 
			this->AnimateReplay->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->AnimateReplay->Location = System::Drawing::Point(169, 20);
			this->AnimateReplay->Name = L"AnimateReplay";
			this->AnimateReplay->Size = System::Drawing::Size(50, 24);
			this->AnimateReplay->TabIndex = 2;
			this->AnimateReplay->Text = L"重播";
			this->AnimateReplay->UseVisualStyleBackColor = true;
			this->AnimateReplay->Click += gcnew System::EventHandler(this, &CRecordPanel::ReplayAnimate);
			// 
			// AnimatePlay
			// 
			this->AnimatePlay->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->AnimatePlay->Location = System::Drawing::Point(22, 20);
			this->AnimatePlay->Name = L"AnimatePlay";
			this->AnimatePlay->Size = System::Drawing::Size(50, 24);
			this->AnimatePlay->TabIndex = 1;
			this->AnimatePlay->Text = L"播放";
			this->AnimatePlay->UseVisualStyleBackColor = true;
			this->AnimatePlay->Click += gcnew System::EventHandler(this, &CRecordPanel::PlayAnimate);
			// 
			// AnimateStop
			// 
			this->AnimateStop->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->AnimateStop->Location = System::Drawing::Point(95, 20);
			this->AnimateStop->Name = L"AnimateStop";
			this->AnimateStop->Size = System::Drawing::Size(50, 24);
			this->AnimateStop->TabIndex = 0;
			this->AnimateStop->Text = L"停止";
			this->AnimateStop->UseVisualStyleBackColor = true;
			this->AnimateStop->Click += gcnew System::EventHandler(this, &CRecordPanel::StopAnimate);
			// 
			// RecordCamPanel
			// 
			this->RecordCamPanel->Controls->Add(this->CamCtrlGroup);
			this->RecordCamPanel->Dock = System::Windows::Forms::DockStyle::Top;
			this->RecordCamPanel->Location = System::Drawing::Point(0, 634);
			this->RecordCamPanel->Name = L"RecordCamPanel";
			this->RecordCamPanel->Size = System::Drawing::Size(267, 405);
			this->RecordCamPanel->TabIndex = 4;
			// 
			// CamCtrlGroup
			// 
			this->CamCtrlGroup->Controls->Add(this->CamTimeEdit);
			this->CamCtrlGroup->Controls->Add(this->CamCtrlLabelTime);
			this->CamCtrlGroup->Controls->Add(this->CamCtrModify);
			this->CamCtrlGroup->Controls->Add(this->label6);
			this->CamCtrlGroup->Controls->Add(this->label5);
			this->CamCtrlGroup->Controls->Add(this->label4);
			this->CamCtrlGroup->Controls->Add(this->label3);
			this->CamCtrlGroup->Controls->Add(this->label2);
			this->CamCtrlGroup->Controls->Add(this->label1);
			this->CamCtrlGroup->Controls->Add(this->CamCtrlLabelTarget);
			this->CamCtrlGroup->Controls->Add(this->CamCtrlLabelPos);
			this->CamCtrlGroup->Controls->Add(this->CamDirZEdit);
			this->CamCtrlGroup->Controls->Add(this->CamPosZEdit);
			this->CamCtrlGroup->Controls->Add(this->CamDirYEdit);
			this->CamCtrlGroup->Controls->Add(this->CamPosYEdit);
			this->CamCtrlGroup->Controls->Add(this->CamDirXEdit);
			this->CamCtrlGroup->Controls->Add(this->CamPosXEdit);
			this->CamCtrlGroup->Controls->Add(this->CamSpeedEdit);
			this->CamCtrlGroup->Controls->Add(this->CamFovEdit);
			this->CamCtrlGroup->Controls->Add(this->CamCtrlLabelSpeed);
			this->CamCtrlGroup->Controls->Add(this->CamCtrlLabelFov);
			this->CamCtrlGroup->Controls->Add(this->CamCtrltTarget);
			this->CamCtrlGroup->Controls->Add(this->CamCtrlPos);
			this->CamCtrlGroup->Enabled = false;
			this->CamCtrlGroup->Location = System::Drawing::Point(10, 10);
			this->CamCtrlGroup->Name = L"CamCtrlGroup";
			this->CamCtrlGroup->Size = System::Drawing::Size(242, 369);
			this->CamCtrlGroup->TabIndex = 0;
			this->CamCtrlGroup->TabStop = false;
			this->CamCtrlGroup->Text = L"关键帧控制";
			// 
			// CamTimeEdit
			// 
			this->CamTimeEdit->Location = System::Drawing::Point(87, 279);
			this->CamTimeEdit->Name = L"CamTimeEdit";
			this->CamTimeEdit->Size = System::Drawing::Size(81, 21);
			this->CamTimeEdit->TabIndex = 8;
			this->CamTimeEdit->TextChanged += gcnew System::EventHandler(this, &CRecordPanel::CamTimeEdit_TextChanged);
			// 
			// CamCtrlLabelTime
			// 
			this->CamCtrlLabelTime->AutoSize = true;
			this->CamCtrlLabelTime->Location = System::Drawing::Point(18, 282);
			this->CamCtrlLabelTime->Name = L"CamCtrlLabelTime";
			this->CamCtrlLabelTime->Size = System::Drawing::Size(59, 12);
			this->CamCtrlLabelTime->TabIndex = 7;
			this->CamCtrlLabelTime->Text = L"时间(mm):";
			// 
			// CamCtrModify
			// 
			this->CamCtrModify->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->CamCtrModify->Location = System::Drawing::Point(175, 276);
			this->CamCtrModify->Name = L"CamCtrModify";
			this->CamCtrModify->Size = System::Drawing::Size(61, 24);
			this->CamCtrModify->TabIndex = 2;
			this->CamCtrModify->Text = L"修改帧";
			this->CamCtrModify->UseVisualStyleBackColor = true;
			this->CamCtrModify->Click += gcnew System::EventHandler(this, &CRecordPanel::CamCtrModify_Click);
			// 
			// label6
			// 
			this->label6->AutoSize = true;
			this->label6->Location = System::Drawing::Point(66, 247);
			this->label6->Name = L"label6";
			this->label6->Size = System::Drawing::Size(11, 12);
			this->label6->TabIndex = 6;
			this->label6->Text = L"Z";
			// 
			// label5
			// 
			this->label5->AutoSize = true;
			this->label5->Location = System::Drawing::Point(66, 220);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(11, 12);
			this->label5->TabIndex = 6;
			this->label5->Text = L"Y";
			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->Location = System::Drawing::Point(66, 193);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(11, 12);
			this->label4->TabIndex = 6;
			this->label4->Text = L"X";
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Location = System::Drawing::Point(66, 151);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(11, 12);
			this->label3->TabIndex = 6;
			this->label3->Text = L"Z";
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(66, 124);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(11, 12);
			this->label2->TabIndex = 6;
			this->label2->Text = L"Y";
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(66, 97);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(11, 12);
			this->label1->TabIndex = 6;
			this->label1->Text = L"X";
			// 
			// CamCtrlLabelTarget
			// 
			this->CamCtrlLabelTarget->AutoSize = true;
			this->CamCtrlLabelTarget->Location = System::Drawing::Point(15, 172);
			this->CamCtrlLabelTarget->Name = L"CamCtrlLabelTarget";
			this->CamCtrlLabelTarget->Size = System::Drawing::Size(59, 12);
			this->CamCtrlLabelTarget->TabIndex = 5;
			this->CamCtrlLabelTarget->Text = L"方向位置:";
			// 
			// CamCtrlLabelPos
			// 
			this->CamCtrlLabelPos->AutoSize = true;
			this->CamCtrlLabelPos->Location = System::Drawing::Point(15, 77);
			this->CamCtrlLabelPos->Name = L"CamCtrlLabelPos";
			this->CamCtrlLabelPos->Size = System::Drawing::Size(71, 12);
			this->CamCtrlLabelPos->TabIndex = 4;
			this->CamCtrlLabelPos->Text = L"摄像机位置:";
			// 
			// CamDirZEdit
			// 
			this->CamDirZEdit->Location = System::Drawing::Point(88, 244);
			this->CamDirZEdit->Name = L"CamDirZEdit";
			this->CamDirZEdit->Size = System::Drawing::Size(80, 21);
			this->CamDirZEdit->TabIndex = 3;
			this->CamDirZEdit->TextChanged += gcnew System::EventHandler(this, &CRecordPanel::CamDirZEdit_TextChanged);
			// 
			// CamPosZEdit
			// 
			this->CamPosZEdit->Location = System::Drawing::Point(87, 148);
			this->CamPosZEdit->Name = L"CamPosZEdit";
			this->CamPosZEdit->Size = System::Drawing::Size(80, 21);
			this->CamPosZEdit->TabIndex = 3;
			this->CamPosZEdit->TextChanged += gcnew System::EventHandler(this, &CRecordPanel::CamPosZEdit_TextChanged);
			// 
			// CamDirYEdit
			// 
			this->CamDirYEdit->Location = System::Drawing::Point(88, 217);
			this->CamDirYEdit->Name = L"CamDirYEdit";
			this->CamDirYEdit->Size = System::Drawing::Size(80, 21);
			this->CamDirYEdit->TabIndex = 3;
			this->CamDirYEdit->TextChanged += gcnew System::EventHandler(this, &CRecordPanel::CamDirYEdit_TextChanged);
			// 
			// CamPosYEdit
			// 
			this->CamPosYEdit->Location = System::Drawing::Point(87, 121);
			this->CamPosYEdit->Name = L"CamPosYEdit";
			this->CamPosYEdit->Size = System::Drawing::Size(80, 21);
			this->CamPosYEdit->TabIndex = 3;
			this->CamPosYEdit->TextChanged += gcnew System::EventHandler(this, &CRecordPanel::CamPosYEdit_TextChanged);
			// 
			// CamDirXEdit
			// 
			this->CamDirXEdit->Location = System::Drawing::Point(88, 190);
			this->CamDirXEdit->Name = L"CamDirXEdit";
			this->CamDirXEdit->Size = System::Drawing::Size(80, 21);
			this->CamDirXEdit->TabIndex = 3;
			this->CamDirXEdit->TextChanged += gcnew System::EventHandler(this, &CRecordPanel::CamDirXEdit_TextChanged);
			// 
			// CamPosXEdit
			// 
			this->CamPosXEdit->Location = System::Drawing::Point(87, 94);
			this->CamPosXEdit->Name = L"CamPosXEdit";
			this->CamPosXEdit->Size = System::Drawing::Size(80, 21);
			this->CamPosXEdit->TabIndex = 3;
			this->CamPosXEdit->TextChanged += gcnew System::EventHandler(this, &CRecordPanel::CamPosXEdit_TextChanged);
			// 
			// CamSpeedEdit
			// 
			this->CamSpeedEdit->Location = System::Drawing::Point(88, 47);
			this->CamSpeedEdit->Name = L"CamSpeedEdit";
			this->CamSpeedEdit->Size = System::Drawing::Size(80, 21);
			this->CamSpeedEdit->TabIndex = 3;
			this->CamSpeedEdit->TextChanged += gcnew System::EventHandler(this, &CRecordPanel::CamSpeedEdit_TextChanged);
			// 
			// CamFovEdit
			// 
			this->CamFovEdit->Location = System::Drawing::Point(88, 15);
			this->CamFovEdit->Name = L"CamFovEdit";
			this->CamFovEdit->Size = System::Drawing::Size(80, 21);
			this->CamFovEdit->TabIndex = 3;
			this->CamFovEdit->TextChanged += gcnew System::EventHandler(this, &CRecordPanel::CamFovEdit_TextChanged);
			// 
			// CamCtrlLabelSpeed
			// 
			this->CamCtrlLabelSpeed->AutoSize = true;
			this->CamCtrlLabelSpeed->Location = System::Drawing::Point(15, 49);
			this->CamCtrlLabelSpeed->Name = L"CamCtrlLabelSpeed";
			this->CamCtrlLabelSpeed->Size = System::Drawing::Size(71, 12);
			this->CamCtrlLabelSpeed->TabIndex = 2;
			this->CamCtrlLabelSpeed->Text = L"摄像机速度:";
			// 
			// CamCtrlLabelFov
			// 
			this->CamCtrlLabelFov->AutoSize = true;
			this->CamCtrlLabelFov->Location = System::Drawing::Point(15, 18);
			this->CamCtrlLabelFov->Name = L"CamCtrlLabelFov";
			this->CamCtrlLabelFov->Size = System::Drawing::Size(71, 12);
			this->CamCtrlLabelFov->TabIndex = 2;
			this->CamCtrlLabelFov->Text = L"摄像机视野:";
			// 
			// CamCtrltTarget
			// 
			this->CamCtrltTarget->AutoSize = true;
			this->CamCtrltTarget->Location = System::Drawing::Point(17, 345);
			this->CamCtrltTarget->Name = L"CamCtrltTarget";
			this->CamCtrltTarget->Size = System::Drawing::Size(119, 16);
			this->CamCtrltTarget->TabIndex = 1;
			this->CamCtrltTarget->TabStop = true;
			this->CamCtrltTarget->Text = L"操作摄像机观察点";
			this->CamCtrltTarget->UseVisualStyleBackColor = true;
			this->CamCtrltTarget->Visible = false;
			this->CamCtrltTarget->CheckedChanged += gcnew System::EventHandler(this, &CRecordPanel::CamCtrltTarget_CheckedChanged);
			// 
			// CamCtrlPos
			// 
			this->CamCtrlPos->AutoSize = true;
			this->CamCtrlPos->Location = System::Drawing::Point(17, 323);
			this->CamCtrlPos->Name = L"CamCtrlPos";
			this->CamCtrlPos->Size = System::Drawing::Size(107, 16);
			this->CamCtrlPos->TabIndex = 0;
			this->CamCtrlPos->TabStop = true;
			this->CamCtrlPos->Text = L"操作摄像机位置";
			this->CamCtrlPos->UseVisualStyleBackColor = true;
			this->CamCtrlPos->Visible = false;
			this->CamCtrlPos->CheckedChanged += gcnew System::EventHandler(this, &CRecordPanel::CamCtrlPos_CheckedChanged);
			// 
			// CRecordPanel
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->AutoScroll = true;
			this->Controls->Add(this->RecordCamPanel);
			this->Controls->Add(this->AnimateCtrlPanel);
			this->Controls->Add(this->KeyFramePanel);
			this->Controls->Add(this->AnimatePanel);
			this->Name = L"CRecordPanel";
			this->Size = System::Drawing::Size(267, 1042);
			this->AnimateGroup->ResumeLayout(false);
			this->AnimatePanel->ResumeLayout(false);
			this->KeyFramePanel->ResumeLayout(false);
			this->KeyFrameGroup->ResumeLayout(false);
			this->KeyFrameGroup->PerformLayout();
			this->AddKeyPanel->ResumeLayout(false);
			this->AddKeyPanel->PerformLayout();
			this->AnimateCtrlPanel->ResumeLayout(false);
			this->AnimateCtrlGroup->ResumeLayout(false);
			this->RecordCamPanel->ResumeLayout(false);
			this->CamCtrlGroup->ResumeLayout(false);
			this->CamCtrlGroup->PerformLayout();
			this->ResumeLayout(false);

		}
#pragma endregion

public:  virtual void Update(void) override;
public:  virtual char*	GetImpTypeName(void)	override;

public:	void ActivePanel();
public: void AnimateListUpdate();
public: void KeyListUpdate();
public: void CamCtrlUpdate();

private: System::Void AddAnimate(System::Object^  sender, System::EventArgs^  e); 
private: System::Void DelAnimate(System::Object^  sender, System::EventArgs^  e);	 
private: System::Void AddKeyFrame(System::Object^  sender, System::EventArgs^  e);	
private: System::Void ModifyKeyFrame(System::Object^  sender, System::EventArgs^  e);	
private: System::Void DekKeyFrame(System::Object^  sender, System::EventArgs^  e);		
private: System::Void SetLastKeyFrame(System::Object^  sender, System::EventArgs^  e);	
private: System::Void PlayAnimate(System::Object^  sender, System::EventArgs^  e);	 
private: System::Void StopAnimate(System::Object^  sender, System::EventArgs^  e);	
private: System::Void ReplayAnimate(System::Object^  sender, System::EventArgs^  e) ;	 
private: System::Void CamCtrlPos_CheckedChanged(System::Object^  sender, System::EventArgs^  e); 	 
private: System::Void CamCtrltTarget_CheckedChanged(System::Object^  sender, System::EventArgs^  e);		
private: System::Void KeyFrameShow_CheckedChanged(System::Object^  sender, System::EventArgs^  e);	
private: System::Void CamFovEdit_TextChanged(System::Object^  sender, System::EventArgs^  e);	 
private: System::Void CamSpeedEdit_TextChanged(System::Object^  sender, System::EventArgs^  e);
private: System::Void AnimateList_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void KeyFrameList_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void CamCtrModify_Click(System::Object^  sender, System::EventArgs^  e);

private: System::Void CamPosXEdit_TextChanged(System::Object^  sender, System::EventArgs^  e);
private: System::Void CamPosYEdit_TextChanged(System::Object^  sender, System::EventArgs^  e);
private: System::Void CamPosZEdit_TextChanged(System::Object^  sender, System::EventArgs^  e);
private: System::Void CamDirXEdit_TextChanged(System::Object^  sender, System::EventArgs^  e);
private: System::Void CamDirYEdit_TextChanged(System::Object^  sender, System::EventArgs^  e);
private: System::Void CamDirZEdit_TextChanged(System::Object^  sender, System::EventArgs^  e);
private: System::Void btKeyTimeN_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void btKeyTimeY_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void EditKeytime_TextChanged(System::Object^  sender, System::EventArgs^  e);
private: System::Void CamTimeEdit_TextChanged(System::Object^  sender, System::EventArgs^  e);
};

}
