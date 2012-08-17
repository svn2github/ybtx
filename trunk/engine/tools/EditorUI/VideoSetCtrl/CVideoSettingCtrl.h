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
	/// Summary for CVideoSettingCtrl
	/// </summary>
	public ref class CVideoSettingCtrl : public CUIBase
	{
	public:
		CVideoSettingCtrl(void)
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
		~CVideoSettingCtrl()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::GroupBox^  AliasSetting;
	private: System::Windows::Forms::RadioButton^  Alias_High;
	private: System::Windows::Forms::RadioButton^  Alias_Low;
	private: System::Windows::Forms::RadioButton^  Alias_Shut;
	private: System::Windows::Forms::GroupBox^  ShadowSetting;
	private: System::Windows::Forms::GroupBox^  TerrainSetting;
	private: System::Windows::Forms::GroupBox^  WaterSetting;
	private: System::Windows::Forms::GroupBox^  TexSetting;
	private: System::Windows::Forms::RadioButton^  Shadow_High;
	private: System::Windows::Forms::RadioButton^  Shadow_Low;
	private: System::Windows::Forms::RadioButton^  Shadow_Shut;
	private: System::Windows::Forms::RadioButton^  Terrain_Low;
	private: System::Windows::Forms::RadioButton^  Terrain_High;
	private: System::Windows::Forms::RadioButton^  Water_Shut;
	private: System::Windows::Forms::RadioButton^  Water_Default;
	private: System::Windows::Forms::RadioButton^  Tex_Mid;
	private: System::Windows::Forms::RadioButton^  Tex_High;
	private: System::Windows::Forms::RadioButton^  Tex_Low;
	private: System::Windows::Forms::RadioButton^  Rate16_9;
	private: System::Windows::Forms::RadioButton^  Rate16_10;
	private: System::Windows::Forms::RadioButton^  Rate5_4;
	private: System::Windows::Forms::GroupBox^  RateSetting;
	private: System::Windows::Forms::RadioButton^  Rate4_3;
	private: System::Windows::Forms::GroupBox^  OtherSetting;
	private: System::Windows::Forms::CheckBox^  SynCheck;
	private: System::Windows::Forms::CheckBox^  WinmodeCheck;
	private: System::Windows::Forms::CheckBox^  CamCheck;
	private: System::Windows::Forms::CheckBox^  RefractCheck;
	private: System::Windows::Forms::CheckBox^  BloomCheck;
	private: System::Windows::Forms::GroupBox^  FPSSetting;
	private: System::Windows::Forms::RadioButton^  FPS_60;
	private: System::Windows::Forms::RadioButton^  FPS_45;
	private: System::Windows::Forms::RadioButton^  FPS_30;
	private: System::Windows::Forms::RadioButton^  FPS_15;

	private: System::Windows::Forms::GroupBox^  EffectSetting;
	private: System::Windows::Forms::RadioButton^  Effect_High;
	private: System::Windows::Forms::RadioButton^  Effect_Mid;
	private: System::Windows::Forms::RadioButton^  Effect_Low;
	private: System::Windows::Forms::RadioButton^  radioButton1;
	private: System::Windows::Forms::RadioButton^  radioButton2;
	private: System::Windows::Forms::Button^  bt_app;
	private: System::Windows::Forms::Button^  bt_reset;
	protected: 
	public:	 virtual char* GetImpTypeName(void) override;
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
			this->AliasSetting = (gcnew System::Windows::Forms::GroupBox());
			this->Alias_High = (gcnew System::Windows::Forms::RadioButton());
			this->Alias_Low = (gcnew System::Windows::Forms::RadioButton());
			this->Alias_Shut = (gcnew System::Windows::Forms::RadioButton());
			this->ShadowSetting = (gcnew System::Windows::Forms::GroupBox());
			this->Shadow_High = (gcnew System::Windows::Forms::RadioButton());
			this->Shadow_Low = (gcnew System::Windows::Forms::RadioButton());
			this->Shadow_Shut = (gcnew System::Windows::Forms::RadioButton());
			this->TerrainSetting = (gcnew System::Windows::Forms::GroupBox());
			this->Terrain_Low = (gcnew System::Windows::Forms::RadioButton());
			this->Terrain_High = (gcnew System::Windows::Forms::RadioButton());
			this->WaterSetting = (gcnew System::Windows::Forms::GroupBox());
			this->Water_Shut = (gcnew System::Windows::Forms::RadioButton());
			this->Water_Default = (gcnew System::Windows::Forms::RadioButton());
			this->TexSetting = (gcnew System::Windows::Forms::GroupBox());
			this->Tex_Mid = (gcnew System::Windows::Forms::RadioButton());
			this->Tex_High = (gcnew System::Windows::Forms::RadioButton());
			this->Tex_Low = (gcnew System::Windows::Forms::RadioButton());
			this->Rate16_9 = (gcnew System::Windows::Forms::RadioButton());
			this->Rate16_10 = (gcnew System::Windows::Forms::RadioButton());
			this->Rate5_4 = (gcnew System::Windows::Forms::RadioButton());
			this->RateSetting = (gcnew System::Windows::Forms::GroupBox());
			this->radioButton1 = (gcnew System::Windows::Forms::RadioButton());
			this->Rate4_3 = (gcnew System::Windows::Forms::RadioButton());
			this->OtherSetting = (gcnew System::Windows::Forms::GroupBox());
			this->CamCheck = (gcnew System::Windows::Forms::CheckBox());
			this->RefractCheck = (gcnew System::Windows::Forms::CheckBox());
			this->BloomCheck = (gcnew System::Windows::Forms::CheckBox());
			this->SynCheck = (gcnew System::Windows::Forms::CheckBox());
			this->WinmodeCheck = (gcnew System::Windows::Forms::CheckBox());
			this->FPSSetting = (gcnew System::Windows::Forms::GroupBox());
			this->radioButton2 = (gcnew System::Windows::Forms::RadioButton());
			this->FPS_60 = (gcnew System::Windows::Forms::RadioButton());
			this->FPS_45 = (gcnew System::Windows::Forms::RadioButton());
			this->FPS_30 = (gcnew System::Windows::Forms::RadioButton());
			this->FPS_15 = (gcnew System::Windows::Forms::RadioButton());
			this->EffectSetting = (gcnew System::Windows::Forms::GroupBox());
			this->Effect_High = (gcnew System::Windows::Forms::RadioButton());
			this->Effect_Mid = (gcnew System::Windows::Forms::RadioButton());
			this->Effect_Low = (gcnew System::Windows::Forms::RadioButton());
			this->bt_app = (gcnew System::Windows::Forms::Button());
			this->bt_reset = (gcnew System::Windows::Forms::Button());
			this->AliasSetting->SuspendLayout();
			this->ShadowSetting->SuspendLayout();
			this->TerrainSetting->SuspendLayout();
			this->WaterSetting->SuspendLayout();
			this->TexSetting->SuspendLayout();
			this->RateSetting->SuspendLayout();
			this->OtherSetting->SuspendLayout();
			this->FPSSetting->SuspendLayout();
			this->EffectSetting->SuspendLayout();
			this->SuspendLayout();
			// 
			// AliasSetting
			// 
			this->AliasSetting->Controls->Add(this->Alias_High);
			this->AliasSetting->Controls->Add(this->Alias_Low);
			this->AliasSetting->Controls->Add(this->Alias_Shut);
			this->AliasSetting->Location = System::Drawing::Point(239, 111);
			this->AliasSetting->Name = L"AliasSetting";
			this->AliasSetting->Size = System::Drawing::Size(67, 96);
			this->AliasSetting->TabIndex = 0;
			this->AliasSetting->TabStop = false;
			this->AliasSetting->Text = L"反锯齿";
			// 
			// Alias_High
			// 
			this->Alias_High->AutoSize = true;
			this->Alias_High->Location = System::Drawing::Point(12, 66);
			this->Alias_High->Name = L"Alias_High";
			this->Alias_High->Size = System::Drawing::Size(35, 16);
			this->Alias_High->TabIndex = 1;
			this->Alias_High->TabStop = true;
			this->Alias_High->Text = L"高";
			this->Alias_High->UseVisualStyleBackColor = true;
			this->Alias_High->CheckedChanged += gcnew System::EventHandler(this, &CVideoSettingCtrl::Alias_High_CheckedChanged);
			// 
			// Alias_Low
			// 
			this->Alias_Low->AutoSize = true;
			this->Alias_Low->Location = System::Drawing::Point(12, 44);
			this->Alias_Low->Name = L"Alias_Low";
			this->Alias_Low->Size = System::Drawing::Size(35, 16);
			this->Alias_Low->TabIndex = 1;
			this->Alias_Low->TabStop = true;
			this->Alias_Low->Text = L"低";
			this->Alias_Low->UseVisualStyleBackColor = true;
			this->Alias_Low->CheckedChanged += gcnew System::EventHandler(this, &CVideoSettingCtrl::Alias_Low_CheckedChanged);
			// 
			// Alias_Shut
			// 
			this->Alias_Shut->AutoSize = true;
			this->Alias_Shut->Location = System::Drawing::Point(12, 23);
			this->Alias_Shut->Name = L"Alias_Shut";
			this->Alias_Shut->Size = System::Drawing::Size(35, 16);
			this->Alias_Shut->TabIndex = 0;
			this->Alias_Shut->TabStop = true;
			this->Alias_Shut->Text = L"关";
			this->Alias_Shut->UseVisualStyleBackColor = true;
			this->Alias_Shut->CheckedChanged += gcnew System::EventHandler(this, &CVideoSettingCtrl::Alias_Shut_CheckedChanged);
			// 
			// ShadowSetting
			// 
			this->ShadowSetting->Controls->Add(this->Shadow_High);
			this->ShadowSetting->Controls->Add(this->Shadow_Low);
			this->ShadowSetting->Controls->Add(this->Shadow_Shut);
			this->ShadowSetting->Location = System::Drawing::Point(240, 3);
			this->ShadowSetting->Name = L"ShadowSetting";
			this->ShadowSetting->Size = System::Drawing::Size(67, 102);
			this->ShadowSetting->TabIndex = 3;
			this->ShadowSetting->TabStop = false;
			this->ShadowSetting->Text = L"阴影质量";
			// 
			// Shadow_High
			// 
			this->Shadow_High->AutoSize = true;
			this->Shadow_High->Location = System::Drawing::Point(11, 62);
			this->Shadow_High->Name = L"Shadow_High";
			this->Shadow_High->Size = System::Drawing::Size(35, 16);
			this->Shadow_High->TabIndex = 1;
			this->Shadow_High->TabStop = true;
			this->Shadow_High->Text = L"高";
			this->Shadow_High->UseVisualStyleBackColor = true;
			this->Shadow_High->CheckedChanged += gcnew System::EventHandler(this, &CVideoSettingCtrl::Shadow_High_CheckedChanged);
			// 
			// Shadow_Low
			// 
			this->Shadow_Low->AutoSize = true;
			this->Shadow_Low->Location = System::Drawing::Point(11, 40);
			this->Shadow_Low->Name = L"Shadow_Low";
			this->Shadow_Low->Size = System::Drawing::Size(35, 16);
			this->Shadow_Low->TabIndex = 1;
			this->Shadow_Low->TabStop = true;
			this->Shadow_Low->Text = L"中";
			this->Shadow_Low->UseVisualStyleBackColor = true;
			this->Shadow_Low->CheckedChanged += gcnew System::EventHandler(this, &CVideoSettingCtrl::Shadow_Low_CheckedChanged);
			// 
			// Shadow_Shut
			// 
			this->Shadow_Shut->AutoSize = true;
			this->Shadow_Shut->Location = System::Drawing::Point(11, 19);
			this->Shadow_Shut->Name = L"Shadow_Shut";
			this->Shadow_Shut->Size = System::Drawing::Size(35, 16);
			this->Shadow_Shut->TabIndex = 0;
			this->Shadow_Shut->TabStop = true;
			this->Shadow_Shut->Text = L"低";
			this->Shadow_Shut->UseVisualStyleBackColor = true;
			this->Shadow_Shut->CheckedChanged += gcnew System::EventHandler(this, &CVideoSettingCtrl::Shadow_Shut_CheckedChanged);
			// 
			// TerrainSetting
			// 
			this->TerrainSetting->Controls->Add(this->Terrain_Low);
			this->TerrainSetting->Controls->Add(this->Terrain_High);
			this->TerrainSetting->Location = System::Drawing::Point(7, 141);
			this->TerrainSetting->Name = L"TerrainSetting";
			this->TerrainSetting->Size = System::Drawing::Size(73, 67);
			this->TerrainSetting->TabIndex = 4;
			this->TerrainSetting->TabStop = false;
			this->TerrainSetting->Text = L"地表质量";
			// 
			// Terrain_Low
			// 
			this->Terrain_Low->AutoSize = true;
			this->Terrain_Low->Location = System::Drawing::Point(10, 20);
			this->Terrain_Low->Name = L"Terrain_Low";
			this->Terrain_Low->Size = System::Drawing::Size(35, 16);
			this->Terrain_Low->TabIndex = 0;
			this->Terrain_Low->TabStop = true;
			this->Terrain_Low->Text = L"底";
			this->Terrain_Low->UseVisualStyleBackColor = true;
			this->Terrain_Low->CheckedChanged += gcnew System::EventHandler(this, &CVideoSettingCtrl::Terrain_Low_CheckedChanged);
			// 
			// Terrain_High
			// 
			this->Terrain_High->AutoSize = true;
			this->Terrain_High->Location = System::Drawing::Point(10, 41);
			this->Terrain_High->Name = L"Terrain_High";
			this->Terrain_High->Size = System::Drawing::Size(35, 16);
			this->Terrain_High->TabIndex = 1;
			this->Terrain_High->TabStop = true;
			this->Terrain_High->Text = L"高";
			this->Terrain_High->UseVisualStyleBackColor = true;
			this->Terrain_High->CheckedChanged += gcnew System::EventHandler(this, &CVideoSettingCtrl::Terrain_High_CheckedChanged);
			// 
			// WaterSetting
			// 
			this->WaterSetting->Controls->Add(this->Water_Shut);
			this->WaterSetting->Controls->Add(this->Water_Default);
			this->WaterSetting->Location = System::Drawing::Point(85, 141);
			this->WaterSetting->Name = L"WaterSetting";
			this->WaterSetting->Size = System::Drawing::Size(74, 67);
			this->WaterSetting->TabIndex = 5;
			this->WaterSetting->TabStop = false;
			this->WaterSetting->Text = L"水面质量";
			// 
			// Water_Shut
			// 
			this->Water_Shut->AutoSize = true;
			this->Water_Shut->Location = System::Drawing::Point(10, 19);
			this->Water_Shut->Name = L"Water_Shut";
			this->Water_Shut->Size = System::Drawing::Size(47, 16);
			this->Water_Shut->TabIndex = 0;
			this->Water_Shut->TabStop = true;
			this->Water_Shut->Text = L"关闭";
			this->Water_Shut->UseVisualStyleBackColor = true;
			this->Water_Shut->CheckedChanged += gcnew System::EventHandler(this, &CVideoSettingCtrl::Water_Shut_CheckedChanged);
			// 
			// Water_Default
			// 
			this->Water_Default->AutoSize = true;
			this->Water_Default->Location = System::Drawing::Point(10, 41);
			this->Water_Default->Name = L"Water_Default";
			this->Water_Default->Size = System::Drawing::Size(47, 16);
			this->Water_Default->TabIndex = 1;
			this->Water_Default->TabStop = true;
			this->Water_Default->Text = L"默认";
			this->Water_Default->UseVisualStyleBackColor = true;
			this->Water_Default->CheckedChanged += gcnew System::EventHandler(this, &CVideoSettingCtrl::Water_Default_CheckedChanged);
			// 
			// TexSetting
			// 
			this->TexSetting->Controls->Add(this->Tex_Mid);
			this->TexSetting->Controls->Add(this->Tex_High);
			this->TexSetting->Controls->Add(this->Tex_Low);
			this->TexSetting->Location = System::Drawing::Point(166, 112);
			this->TexSetting->Name = L"TexSetting";
			this->TexSetting->Size = System::Drawing::Size(67, 96);
			this->TexSetting->TabIndex = 6;
			this->TexSetting->TabStop = false;
			this->TexSetting->Text = L"纹理质量";
			// 
			// Tex_Mid
			// 
			this->Tex_Mid->AutoSize = true;
			this->Tex_Mid->Location = System::Drawing::Point(11, 45);
			this->Tex_Mid->Name = L"Tex_Mid";
			this->Tex_Mid->Size = System::Drawing::Size(35, 16);
			this->Tex_Mid->TabIndex = 1;
			this->Tex_Mid->TabStop = true;
			this->Tex_Mid->Text = L"中";
			this->Tex_Mid->UseVisualStyleBackColor = true;
			this->Tex_Mid->CheckedChanged += gcnew System::EventHandler(this, &CVideoSettingCtrl::Tex_Mid_CheckedChanged);
			// 
			// Tex_High
			// 
			this->Tex_High->AutoSize = true;
			this->Tex_High->Location = System::Drawing::Point(11, 67);
			this->Tex_High->Name = L"Tex_High";
			this->Tex_High->Size = System::Drawing::Size(35, 16);
			this->Tex_High->TabIndex = 1;
			this->Tex_High->TabStop = true;
			this->Tex_High->Text = L"高";
			this->Tex_High->UseVisualStyleBackColor = true;
			this->Tex_High->CheckedChanged += gcnew System::EventHandler(this, &CVideoSettingCtrl::Tex_High_CheckedChanged);
			// 
			// Tex_Low
			// 
			this->Tex_Low->AutoSize = true;
			this->Tex_Low->Location = System::Drawing::Point(11, 24);
			this->Tex_Low->Name = L"Tex_Low";
			this->Tex_Low->Size = System::Drawing::Size(35, 16);
			this->Tex_Low->TabIndex = 0;
			this->Tex_Low->TabStop = true;
			this->Tex_Low->Text = L"低";
			this->Tex_Low->UseVisualStyleBackColor = true;
			this->Tex_Low->CheckedChanged += gcnew System::EventHandler(this, &CVideoSettingCtrl::Tex_Low_CheckedChanged);
			// 
			// Rate16_9
			// 
			this->Rate16_9->AutoSize = true;
			this->Rate16_9->Location = System::Drawing::Point(11, 23);
			this->Rate16_9->Name = L"Rate16_9";
			this->Rate16_9->Size = System::Drawing::Size(47, 16);
			this->Rate16_9->TabIndex = 0;
			this->Rate16_9->TabStop = true;
			this->Rate16_9->Text = L"16:9";
			this->Rate16_9->UseVisualStyleBackColor = true;
			this->Rate16_9->CheckedChanged += gcnew System::EventHandler(this, &CVideoSettingCtrl::Rate16_9_CheckedChanged);
			// 
			// Rate16_10
			// 
			this->Rate16_10->AutoSize = true;
			this->Rate16_10->Location = System::Drawing::Point(11, 44);
			this->Rate16_10->Name = L"Rate16_10";
			this->Rate16_10->Size = System::Drawing::Size(53, 16);
			this->Rate16_10->TabIndex = 1;
			this->Rate16_10->TabStop = true;
			this->Rate16_10->Text = L"16:10";
			this->Rate16_10->UseVisualStyleBackColor = true;
			this->Rate16_10->CheckedChanged += gcnew System::EventHandler(this, &CVideoSettingCtrl::Rate16_10_CheckedChanged);
			// 
			// Rate5_4
			// 
			this->Rate5_4->AutoSize = true;
			this->Rate5_4->Location = System::Drawing::Point(11, 66);
			this->Rate5_4->Name = L"Rate5_4";
			this->Rate5_4->Size = System::Drawing::Size(41, 16);
			this->Rate5_4->TabIndex = 1;
			this->Rate5_4->TabStop = true;
			this->Rate5_4->Text = L"5:4";
			this->Rate5_4->UseVisualStyleBackColor = true;
			this->Rate5_4->CheckedChanged += gcnew System::EventHandler(this, &CVideoSettingCtrl::Rate5_4_CheckedChanged);
			// 
			// RateSetting
			// 
			this->RateSetting->Controls->Add(this->radioButton1);
			this->RateSetting->Controls->Add(this->Rate4_3);
			this->RateSetting->Controls->Add(this->Rate5_4);
			this->RateSetting->Controls->Add(this->Rate16_10);
			this->RateSetting->Controls->Add(this->Rate16_9);
			this->RateSetting->Location = System::Drawing::Point(7, 3);
			this->RateSetting->Name = L"RateSetting";
			this->RateSetting->Size = System::Drawing::Size(73, 132);
			this->RateSetting->TabIndex = 7;
			this->RateSetting->TabStop = false;
			this->RateSetting->Text = L"画面比例";
			// 
			// radioButton1
			// 
			this->radioButton1->AutoSize = true;
			this->radioButton1->Location = System::Drawing::Point(11, 110);
			this->radioButton1->Name = L"radioButton1";
			this->radioButton1->Size = System::Drawing::Size(59, 16);
			this->radioButton1->TabIndex = 2;
			this->radioButton1->TabStop = true;
			this->radioButton1->Text = L"自适应";
			this->radioButton1->UseVisualStyleBackColor = true;
			// 
			// Rate4_3
			// 
			this->Rate4_3->AutoSize = true;
			this->Rate4_3->Location = System::Drawing::Point(11, 88);
			this->Rate4_3->Name = L"Rate4_3";
			this->Rate4_3->Size = System::Drawing::Size(41, 16);
			this->Rate4_3->TabIndex = 1;
			this->Rate4_3->TabStop = true;
			this->Rate4_3->Text = L"4:3";
			this->Rate4_3->UseVisualStyleBackColor = true;
			this->Rate4_3->CheckedChanged += gcnew System::EventHandler(this, &CVideoSettingCtrl::Rate4_3_CheckedChanged);
			// 
			// OtherSetting
			// 
			this->OtherSetting->Controls->Add(this->CamCheck);
			this->OtherSetting->Controls->Add(this->RefractCheck);
			this->OtherSetting->Controls->Add(this->BloomCheck);
			this->OtherSetting->Controls->Add(this->SynCheck);
			this->OtherSetting->Controls->Add(this->WinmodeCheck);
			this->OtherSetting->Location = System::Drawing::Point(4, 211);
			this->OtherSetting->Name = L"OtherSetting";
			this->OtherSetting->Size = System::Drawing::Size(302, 72);
			this->OtherSetting->TabIndex = 8;
			this->OtherSetting->TabStop = false;
			this->OtherSetting->Text = L"其他设置";
			// 
			// CamCheck
			// 
			this->CamCheck->AutoSize = true;
			this->CamCheck->Location = System::Drawing::Point(97, 50);
			this->CamCheck->Name = L"CamCheck";
			this->CamCheck->Size = System::Drawing::Size(72, 16);
			this->CamCheck->TabIndex = 4;
			this->CamCheck->Text = L"镜头震动";
			this->CamCheck->UseVisualStyleBackColor = true;
			this->CamCheck->CheckedChanged += gcnew System::EventHandler(this, &CVideoSettingCtrl::CamCheck_CheckedChanged);
			// 
			// RefractCheck
			// 
			this->RefractCheck->AutoSize = true;
			this->RefractCheck->Location = System::Drawing::Point(14, 50);
			this->RefractCheck->Name = L"RefractCheck";
			this->RefractCheck->Size = System::Drawing::Size(72, 16);
			this->RefractCheck->TabIndex = 3;
			this->RefractCheck->Text = L"空气折射";
			this->RefractCheck->UseVisualStyleBackColor = true;
			this->RefractCheck->CheckedChanged += gcnew System::EventHandler(this, &CVideoSettingCtrl::RefractCheck_CheckedChanged);
			// 
			// BloomCheck
			// 
			this->BloomCheck->AutoSize = true;
			this->BloomCheck->Location = System::Drawing::Point(181, 22);
			this->BloomCheck->Name = L"BloomCheck";
			this->BloomCheck->Size = System::Drawing::Size(72, 16);
			this->BloomCheck->TabIndex = 2;
			this->BloomCheck->Text = L"全屏泛光";
			this->BloomCheck->UseVisualStyleBackColor = true;
			this->BloomCheck->CheckedChanged += gcnew System::EventHandler(this, &CVideoSettingCtrl::BloomCheck_CheckedChanged);
			// 
			// SynCheck
			// 
			this->SynCheck->AutoSize = true;
			this->SynCheck->Location = System::Drawing::Point(97, 22);
			this->SynCheck->Name = L"SynCheck";
			this->SynCheck->Size = System::Drawing::Size(72, 16);
			this->SynCheck->TabIndex = 1;
			this->SynCheck->Text = L"垂直同步";
			this->SynCheck->UseVisualStyleBackColor = true;
			this->SynCheck->CheckedChanged += gcnew System::EventHandler(this, &CVideoSettingCtrl::SynCheck_CheckedChanged);
			// 
			// WinmodeCheck
			// 
			this->WinmodeCheck->AutoSize = true;
			this->WinmodeCheck->Location = System::Drawing::Point(13, 22);
			this->WinmodeCheck->Name = L"WinmodeCheck";
			this->WinmodeCheck->Size = System::Drawing::Size(72, 16);
			this->WinmodeCheck->TabIndex = 0;
			this->WinmodeCheck->Text = L"窗口模式";
			this->WinmodeCheck->UseVisualStyleBackColor = true;
			this->WinmodeCheck->CheckedChanged += gcnew System::EventHandler(this, &CVideoSettingCtrl::WinmodeCheck_CheckedChanged);
			// 
			// FPSSetting
			// 
			this->FPSSetting->Controls->Add(this->radioButton2);
			this->FPSSetting->Controls->Add(this->FPS_60);
			this->FPSSetting->Controls->Add(this->FPS_45);
			this->FPSSetting->Controls->Add(this->FPS_30);
			this->FPSSetting->Controls->Add(this->FPS_15);
			this->FPSSetting->Location = System::Drawing::Point(85, 3);
			this->FPSSetting->Name = L"FPSSetting";
			this->FPSSetting->Size = System::Drawing::Size(74, 132);
			this->FPSSetting->TabIndex = 9;
			this->FPSSetting->TabStop = false;
			this->FPSSetting->Text = L"FPS控制";
			// 
			// radioButton2
			// 
			this->radioButton2->AutoSize = true;
			this->radioButton2->Location = System::Drawing::Point(12, 109);
			this->radioButton2->Name = L"radioButton2";
			this->radioButton2->Size = System::Drawing::Size(59, 16);
			this->radioButton2->TabIndex = 4;
			this->radioButton2->TabStop = true;
			this->radioButton2->Text = L"无限制";
			this->radioButton2->UseVisualStyleBackColor = true;
			// 
			// FPS_60
			// 
			this->FPS_60->AutoSize = true;
			this->FPS_60->Location = System::Drawing::Point(12, 86);
			this->FPS_60->Name = L"FPS_60";
			this->FPS_60->Size = System::Drawing::Size(53, 16);
			this->FPS_60->TabIndex = 3;
			this->FPS_60->TabStop = true;
			this->FPS_60->Text = L"60fps";
			this->FPS_60->UseVisualStyleBackColor = true;
			this->FPS_60->CheckedChanged += gcnew System::EventHandler(this, &CVideoSettingCtrl::FPS_60_CheckedChanged);
			// 
			// FPS_45
			// 
			this->FPS_45->AutoSize = true;
			this->FPS_45->Location = System::Drawing::Point(12, 63);
			this->FPS_45->Name = L"FPS_45";
			this->FPS_45->Size = System::Drawing::Size(53, 16);
			this->FPS_45->TabIndex = 2;
			this->FPS_45->TabStop = true;
			this->FPS_45->Text = L"45fps";
			this->FPS_45->UseVisualStyleBackColor = true;
			this->FPS_45->CheckedChanged += gcnew System::EventHandler(this, &CVideoSettingCtrl::FPS_45_CheckedChanged);
			// 
			// FPS_30
			// 
			this->FPS_30->AutoSize = true;
			this->FPS_30->Location = System::Drawing::Point(12, 41);
			this->FPS_30->Name = L"FPS_30";
			this->FPS_30->Size = System::Drawing::Size(53, 16);
			this->FPS_30->TabIndex = 1;
			this->FPS_30->TabStop = true;
			this->FPS_30->Text = L"30fps";
			this->FPS_30->UseVisualStyleBackColor = true;
			this->FPS_30->CheckedChanged += gcnew System::EventHandler(this, &CVideoSettingCtrl::FPS_30_CheckedChanged);
			// 
			// FPS_15
			// 
			this->FPS_15->AutoSize = true;
			this->FPS_15->Location = System::Drawing::Point(12, 20);
			this->FPS_15->Name = L"FPS_15";
			this->FPS_15->Size = System::Drawing::Size(53, 16);
			this->FPS_15->TabIndex = 0;
			this->FPS_15->TabStop = true;
			this->FPS_15->Text = L"15fps";
			this->FPS_15->UseVisualStyleBackColor = true;
			this->FPS_15->CheckedChanged += gcnew System::EventHandler(this, &CVideoSettingCtrl::FPS_15_CheckedChanged);
			// 
			// EffectSetting
			// 
			this->EffectSetting->Controls->Add(this->Effect_High);
			this->EffectSetting->Controls->Add(this->Effect_Mid);
			this->EffectSetting->Controls->Add(this->Effect_Low);
			this->EffectSetting->Location = System::Drawing::Point(166, 3);
			this->EffectSetting->Name = L"EffectSetting";
			this->EffectSetting->Size = System::Drawing::Size(67, 102);
			this->EffectSetting->TabIndex = 10;
			this->EffectSetting->TabStop = false;
			this->EffectSetting->Text = L"特效等级";
			// 
			// Effect_High
			// 
			this->Effect_High->AutoSize = true;
			this->Effect_High->Location = System::Drawing::Point(10, 65);
			this->Effect_High->Name = L"Effect_High";
			this->Effect_High->Size = System::Drawing::Size(35, 16);
			this->Effect_High->TabIndex = 2;
			this->Effect_High->TabStop = true;
			this->Effect_High->Text = L"高";
			this->Effect_High->UseVisualStyleBackColor = true;
			this->Effect_High->CheckedChanged += gcnew System::EventHandler(this, &CVideoSettingCtrl::Effect_High_CheckedChanged);
			// 
			// Effect_Mid
			// 
			this->Effect_Mid->AutoSize = true;
			this->Effect_Mid->Location = System::Drawing::Point(10, 43);
			this->Effect_Mid->Name = L"Effect_Mid";
			this->Effect_Mid->Size = System::Drawing::Size(35, 16);
			this->Effect_Mid->TabIndex = 1;
			this->Effect_Mid->TabStop = true;
			this->Effect_Mid->Text = L"中";
			this->Effect_Mid->UseVisualStyleBackColor = true;
			this->Effect_Mid->CheckedChanged += gcnew System::EventHandler(this, &CVideoSettingCtrl::Effect_Mid_CheckedChanged);
			// 
			// Effect_Low
			// 
			this->Effect_Low->AutoSize = true;
			this->Effect_Low->Location = System::Drawing::Point(10, 21);
			this->Effect_Low->Name = L"Effect_Low";
			this->Effect_Low->Size = System::Drawing::Size(35, 16);
			this->Effect_Low->TabIndex = 0;
			this->Effect_Low->TabStop = true;
			this->Effect_Low->Text = L"低";
			this->Effect_Low->UseVisualStyleBackColor = true;
			this->Effect_Low->CheckedChanged += gcnew System::EventHandler(this, &CVideoSettingCtrl::Effect_Low_CheckedChanged);
			// 
			// bt_app
			// 
			this->bt_app->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->bt_app->Location = System::Drawing::Point(185, 289);
			this->bt_app->Name = L"bt_app";
			this->bt_app->Size = System::Drawing::Size(57, 23);
			this->bt_app->TabIndex = 11;
			this->bt_app->Text = L"应用";
			this->bt_app->UseVisualStyleBackColor = true;
			this->bt_app->Click += gcnew System::EventHandler(this, &CVideoSettingCtrl::bt_app_Click);
			// 
			// bt_reset
			// 
			this->bt_reset->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->bt_reset->Location = System::Drawing::Point(248, 289);
			this->bt_reset->Name = L"bt_reset";
			this->bt_reset->Size = System::Drawing::Size(57, 23);
			this->bt_reset->TabIndex = 11;
			this->bt_reset->Text = L"重置";
			this->bt_reset->UseVisualStyleBackColor = true;
			this->bt_reset->Click += gcnew System::EventHandler(this, &CVideoSettingCtrl::bt_reset_Click);
			// 
			// CVideoSettingCtrl
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->Controls->Add(this->bt_reset);
			this->Controls->Add(this->bt_app);
			this->Controls->Add(this->EffectSetting);
			this->Controls->Add(this->FPSSetting);
			this->Controls->Add(this->OtherSetting);
			this->Controls->Add(this->RateSetting);
			this->Controls->Add(this->TexSetting);
			this->Controls->Add(this->WaterSetting);
			this->Controls->Add(this->TerrainSetting);
			this->Controls->Add(this->ShadowSetting);
			this->Controls->Add(this->AliasSetting);
			this->Name = L"CVideoSettingCtrl";
			this->Size = System::Drawing::Size(315, 319);
			this->AliasSetting->ResumeLayout(false);
			this->AliasSetting->PerformLayout();
			this->ShadowSetting->ResumeLayout(false);
			this->ShadowSetting->PerformLayout();
			this->TerrainSetting->ResumeLayout(false);
			this->TerrainSetting->PerformLayout();
			this->WaterSetting->ResumeLayout(false);
			this->WaterSetting->PerformLayout();
			this->TexSetting->ResumeLayout(false);
			this->TexSetting->PerformLayout();
			this->RateSetting->ResumeLayout(false);
			this->RateSetting->PerformLayout();
			this->OtherSetting->ResumeLayout(false);
			this->OtherSetting->PerformLayout();
			this->FPSSetting->ResumeLayout(false);
			this->FPSSetting->PerformLayout();
			this->EffectSetting->ResumeLayout(false);
			this->EffectSetting->PerformLayout();
			this->ResumeLayout(false);

		}
#pragma endregion

	private: System::Void Rate16_9_CheckedChanged(System::Object^  sender, System::EventArgs^  e) ;
	private: System::Void Rate16_10_CheckedChanged(System::Object^  sender, System::EventArgs^  e) ;
	private: System::Void Rate5_4_CheckedChanged(System::Object^  sender, System::EventArgs^  e) ;
	private: System::Void Rate4_3_CheckedChanged(System::Object^  sender, System::EventArgs^  e) ;
	private: System::Void Terrain_Low_CheckedChanged(System::Object^  sender, System::EventArgs^  e) ;
	private: System::Void Terrain_High_CheckedChanged(System::Object^  sender, System::EventArgs^  e) ;
	private: System::Void FPS_15_CheckedChanged(System::Object^  sender, System::EventArgs^  e) ;
	private: System::Void FPS_30_CheckedChanged(System::Object^  sender, System::EventArgs^  e) ;
	private: System::Void FPS_45_CheckedChanged(System::Object^  sender, System::EventArgs^  e) ;
	private: System::Void FPS_60_CheckedChanged(System::Object^  sender, System::EventArgs^  e) ;
	private: System::Void Water_Shut_CheckedChanged(System::Object^  sender, System::EventArgs^  e) ;
	private: System::Void Water_Default_CheckedChanged(System::Object^  sender, System::EventArgs^  e) ;
	private: System::Void Effect_Low_CheckedChanged(System::Object^  sender, System::EventArgs^  e) ;
	private: System::Void Effect_Mid_CheckedChanged(System::Object^  sender, System::EventArgs^  e) ;
	private: System::Void Effect_High_CheckedChanged(System::Object^  sender, System::EventArgs^  e) ;
	private: System::Void Tex_Low_CheckedChanged(System::Object^  sender, System::EventArgs^  e) ;
	private: System::Void Tex_Mid_CheckedChanged(System::Object^  sender, System::EventArgs^  e) ;
	private: System::Void Tex_High_CheckedChanged(System::Object^  sender, System::EventArgs^  e) ;
	private: System::Void Shadow_Shut_CheckedChanged(System::Object^  sender, System::EventArgs^  e) ;
	private: System::Void Shadow_Low_CheckedChanged(System::Object^  sender, System::EventArgs^  e) ;
	private: System::Void Shadow_High_CheckedChanged(System::Object^  sender, System::EventArgs^  e) ;
	private: System::Void Alias_Shut_CheckedChanged(System::Object^  sender, System::EventArgs^  e) ;
	private: System::Void Alias_Low_CheckedChanged(System::Object^  sender, System::EventArgs^  e) ;
	private: System::Void Alias_High_CheckedChanged(System::Object^  sender, System::EventArgs^  e) ;
	private: System::Void WinmodeCheck_CheckedChanged(System::Object^  sender, System::EventArgs^  e) ;
	private: System::Void SynCheck_CheckedChanged(System::Object^  sender, System::EventArgs^  e) ;
	private: System::Void BloomCheck_CheckedChanged(System::Object^  sender, System::EventArgs^  e) ;
	private: System::Void RefractCheck_CheckedChanged(System::Object^  sender, System::EventArgs^  e) ;
	private: System::Void CamCheck_CheckedChanged(System::Object^  sender, System::EventArgs^  e) ;
	private: System::Void bt_app_Click(System::Object^  sender, System::EventArgs^  e) ;
	private: System::Void bt_reset_Click(System::Object^  sender, System::EventArgs^  e) ;

	public: virtual void Update(void) override;
	public: System::Void HideButton();

	public: System::Void SendUpdate();
	public: System::Void SendReset();
	public: System::Void SendEnsure();



};

}
