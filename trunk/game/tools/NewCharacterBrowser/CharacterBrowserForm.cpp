#include "StdAfx.h"
#include "CharacterBrowserForm.h"
#include "ScriptMgr.h"
#include "CDataCache.h"
#include "CharacterAttributes.h"
#include "CharacterBrowserBriage.h"
#include "GTRenderMgr.h"
#include "CodeCvs.h"
#include "CCharacter.h"
#include "CCharacter.inl"
#include "TSqrAllocator.inl"
#include "IApplication.h"
#include "CtrlImpHelper.h"
#include "ContextCtrl/CContextCtrl.h"
#include "CEditContext.h"
#include "CDefineScaleDlg.h"

namespace CharacterBrowser {

	#define	DEFAULT_ANI_PATH "character\\ani\\zhujue\\"

	vector<string> gPlayerEquipInfos;

	//String convert to string
	void MarshalString( System::String^ s, std::string& os)
	{
		using namespace System::Runtime::InteropServices;
		const char* chars = (const char*)(Marshal::StringToHGlobalAnsi(s)).ToPointer();
		os = chars;
		Marshal::FreeHGlobal(IntPtr((void*)chars));
	}

	//String convert to string
	void MarshalWString( System::String^ s, std::wstring& os)
	{
		using namespace System::Runtime::InteropServices;
		const wchar_t* chars = (const wchar_t*)(Marshal::StringToHGlobalUni(s)).ToPointer();
		os = chars;
		Marshal::FreeHGlobal(IntPtr((void*)chars));
	}

	CharacterBrowserForm::CharacterBrowserForm( void )
	{
		InitializeComponent();
		//
		//TODO: Add the constructor code here
		//
		gPlayerEquipInfos.resize(ITEM_COUNT);

		pCharacterAttributes = NULL;
	}

	CharacterBrowserForm::~CharacterBrowserForm()
	{
		if( pCharacterAttributes )
		{
			delete pCharacterAttributes;
			pCharacterAttributes = NULL;
		}

		if (components)
		{
			delete components;
		}

		IApplication::GetInst()->Close();
	}

#pragma region Windows Form Designer generated code
	/// <summary>
	/// Required method for Designer support - do not modify
	/// the contents of this method with the code editor.
	/// </summary>
	void CharacterBrowserForm::InitializeComponent( void )
	{
		this->cContextComponent = (gcnew sqr_tools::CContextComponent());
		this->crtsCameraCom = (gcnew sqr_tools::CRTSCameraCom());
		this->TOOL_ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
		this->CHECK_ANIMATE_ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
		this->SHOW_BRUSH_ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
		this->ONEPOINTFIVE_ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
		this->TWOPOINT_ToolStripMenuItem1 = (gcnew System::Windows::Forms::ToolStripMenuItem());
		this->SELFDEFINE_ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
		this->HIDEAPERTURE_ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
		this->SET_ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
		this->PATH_SET_ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
		this->menuStrip = (gcnew System::Windows::Forms::MenuStrip());
		this->tabPage2 = (gcnew System::Windows::Forms::TabPage());
		this->Equip_Info_textBox = (gcnew System::Windows::Forms::TextBox());
		this->groupBox3 = (gcnew System::Windows::Forms::GroupBox());
		this->Speed_textBox = (gcnew System::Windows::Forms::TextBox());
		this->Ani_Speed_trackBar = (gcnew System::Windows::Forms::TrackBar());
		this->label16 = (gcnew System::Windows::Forms::Label());
		this->Player_ANI_comboBox = (gcnew System::Windows::Forms::ComboBox());
		this->label15 = (gcnew System::Windows::Forms::Label());
		this->Player_AGP_comboBox = (gcnew System::Windows::Forms::ComboBox());
		this->label12 = (gcnew System::Windows::Forms::Label());
		this->groupBox2 = (gcnew System::Windows::Forms::GroupBox());
		this->SubWeapon_Adv_comboBox = (gcnew System::Windows::Forms::ComboBox());
		this->Player_SubWeapon_comboBox = (gcnew System::Windows::Forms::ComboBox());
		this->label13 = (gcnew System::Windows::Forms::Label());
		this->MainWeapon_Adv_comboBox = (gcnew System::Windows::Forms::ComboBox());
		this->Player_MainWeapon_comboBox = (gcnew System::Windows::Forms::ComboBox());
		this->label14 = (gcnew System::Windows::Forms::Label());
		this->Boot_Adv_comboBox = (gcnew System::Windows::Forms::ComboBox());
		this->Player_Boot_comboBox = (gcnew System::Windows::Forms::ComboBox());
		this->label9 = (gcnew System::Windows::Forms::Label());
		this->Cuff_Adv_comboBox = (gcnew System::Windows::Forms::ComboBox());
		this->Player_Cuff_comboBox = (gcnew System::Windows::Forms::ComboBox());
		this->label10 = (gcnew System::Windows::Forms::Label());
		this->Mantle_Adv_comboBox = (gcnew System::Windows::Forms::ComboBox());
		this->Player_Mantle_comboBox = (gcnew System::Windows::Forms::ComboBox());
		this->label11 = (gcnew System::Windows::Forms::Label());
		this->Body_Adv_comboBox = (gcnew System::Windows::Forms::ComboBox());
		this->Player_Body_comboBox = (gcnew System::Windows::Forms::ComboBox());
		this->label8 = (gcnew System::Windows::Forms::Label());
		this->Shoulder_Adv_comboBox = (gcnew System::Windows::Forms::ComboBox());
		this->Player_Shoulder_comboBox = (gcnew System::Windows::Forms::ComboBox());
		this->label7 = (gcnew System::Windows::Forms::Label());
		this->Equip_Group_comboBox = (gcnew System::Windows::Forms::ComboBox());
		this->label6 = (gcnew System::Windows::Forms::Label());
		this->Head_Adv_comboBox = (gcnew System::Windows::Forms::ComboBox());
		this->Player_Head_comboBox = (gcnew System::Windows::Forms::ComboBox());
		this->label5 = (gcnew System::Windows::Forms::Label());
		this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
		this->comboBox1 = (gcnew System::Windows::Forms::ComboBox());
		this->label4 = (gcnew System::Windows::Forms::Label());
		this->Player_Hair_comboBox = (gcnew System::Windows::Forms::ComboBox());
		this->label3 = (gcnew System::Windows::Forms::Label());
		this->Player_Face_comboBox = (gcnew System::Windows::Forms::ComboBox());
		this->label2 = (gcnew System::Windows::Forms::Label());
		this->PlayType_comboBox = (gcnew System::Windows::Forms::ComboBox());
		this->label1 = (gcnew System::Windows::Forms::Label());
		this->tabPage1 = (gcnew System::Windows::Forms::TabPage());
		this->OBJ_treeView = (gcnew System::Windows::Forms::TreeView());
		this->NPC_treeView = (gcnew System::Windows::Forms::TreeView());
		this->Search_Button = (gcnew System::Windows::Forms::Button());
		this->Search_textBox = (gcnew System::Windows::Forms::TextBox());
		this->npcobj_groupBox = (gcnew System::Windows::Forms::GroupBox());
		this->OBJ_radioButton = (gcnew System::Windows::Forms::RadioButton());
		this->NPC_radioButton = (gcnew System::Windows::Forms::RadioButton());
		this->tabControl1 = (gcnew System::Windows::Forms::TabControl());
		this->cViewCtrl = (gcnew sqr_tools::CViewCtrl());
		this->menuStrip->SuspendLayout();
		this->tabPage2->SuspendLayout();
		this->groupBox3->SuspendLayout();
		(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->Ani_Speed_trackBar))->BeginInit();
		this->groupBox2->SuspendLayout();
		this->groupBox1->SuspendLayout();
		this->tabPage1->SuspendLayout();
		this->npcobj_groupBox->SuspendLayout();
		this->tabControl1->SuspendLayout();
		this->SuspendLayout();
		// 
		// cContextComponent
		// 
		this->cContextComponent->AmbientColor = System::Drawing::Color::White;
		this->cContextComponent->SceneName = nullptr;
		this->cContextComponent->ShowBlock = false;
		this->cContextComponent->ShowGrid = true;
		this->cContextComponent->SunColor = System::Drawing::Color::White;
		// 
		// crtsCameraCom
		// 
		this->crtsCameraCom->ContextComponent = this->cContextComponent;
		// 
		// TOOL_ToolStripMenuItem
		// 
		this->TOOL_ToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) {this->CHECK_ANIMATE_ToolStripMenuItem});
		this->TOOL_ToolStripMenuItem->Name = L"TOOL_ToolStripMenuItem";
		this->TOOL_ToolStripMenuItem->Size = System::Drawing::Size(44, 21);
		this->TOOL_ToolStripMenuItem->Text = L"工具";
		// 
		// CHECK_ANIMATE_ToolStripMenuItem
		// 
		this->CHECK_ANIMATE_ToolStripMenuItem->Name = L"CHECK_ANIMATE_ToolStripMenuItem";
		this->CHECK_ANIMATE_ToolStripMenuItem->Size = System::Drawing::Size(160, 22);
		this->CHECK_ANIMATE_ToolStripMenuItem->Text = L"检测动作完整性";
		this->CHECK_ANIMATE_ToolStripMenuItem->Click += gcnew System::EventHandler(this, &CharacterBrowserForm::CHECK_ANIMATE_ToolStripMenuItem_Click);
		// 
		// SHOW_BRUSH_ToolStripMenuItem
		// 
		this->SHOW_BRUSH_ToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(4) {this->ONEPOINTFIVE_ToolStripMenuItem, 
			this->TWOPOINT_ToolStripMenuItem1, this->SELFDEFINE_ToolStripMenuItem, this->HIDEAPERTURE_ToolStripMenuItem});
		this->SHOW_BRUSH_ToolStripMenuItem->Name = L"SHOW_BRUSH_ToolStripMenuItem";
		this->SHOW_BRUSH_ToolStripMenuItem->Size = System::Drawing::Size(68, 21);
		this->SHOW_BRUSH_ToolStripMenuItem->Text = L"显示光圈";
		// 
		// ONEPOINTFIVE_ToolStripMenuItem
		// 
		this->ONEPOINTFIVE_ToolStripMenuItem->Name = L"ONEPOINTFIVE_ToolStripMenuItem";
		this->ONEPOINTFIVE_ToolStripMenuItem->Size = System::Drawing::Size(160, 22);
		this->ONEPOINTFIVE_ToolStripMenuItem->Text = L"1.5X";
		this->ONEPOINTFIVE_ToolStripMenuItem->Click += gcnew System::EventHandler(this, &CharacterBrowserForm::ONEPOINTFIVE_ToolStripMenuItem_Click);
		// 
		// TWOPOINT_ToolStripMenuItem1
		// 
		this->TWOPOINT_ToolStripMenuItem1->Name = L"TWOPOINT_ToolStripMenuItem1";
		this->TWOPOINT_ToolStripMenuItem1->Size = System::Drawing::Size(160, 22);
		this->TWOPOINT_ToolStripMenuItem1->Text = L"2X";
		this->TWOPOINT_ToolStripMenuItem1->Click += gcnew System::EventHandler(this, &CharacterBrowserForm::TWOPOINT_ToolStripMenuItem1_Click);
		// 
		// SELFDEFINE_ToolStripMenuItem
		// 
		this->SELFDEFINE_ToolStripMenuItem->Name = L"SELFDEFINE_ToolStripMenuItem";
		this->SELFDEFINE_ToolStripMenuItem->Size = System::Drawing::Size(160, 22);
		this->SELFDEFINE_ToolStripMenuItem->Text = L"自定义大小";
		this->SELFDEFINE_ToolStripMenuItem->Click += gcnew System::EventHandler(this, &CharacterBrowserForm::SELFDEFINE_ToolStripMenuItem_Click);
		// 
		// HIDEAPERTURE_ToolStripMenuItem
		// 
		this->HIDEAPERTURE_ToolStripMenuItem->Name = L"HIDEAPERTURE_ToolStripMenuItem";
		this->HIDEAPERTURE_ToolStripMenuItem->Size = System::Drawing::Size(160, 22);
		this->HIDEAPERTURE_ToolStripMenuItem->Text = L"光圈显示与隐藏";
		this->HIDEAPERTURE_ToolStripMenuItem->Click += gcnew System::EventHandler(this, &CharacterBrowserForm::HIDEAPERTURE_ToolStripMenuItem_Click);
		// 
		// SET_ToolStripMenuItem
		// 
		this->SET_ToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) {this->PATH_SET_ToolStripMenuItem});
		this->SET_ToolStripMenuItem->Name = L"SET_ToolStripMenuItem";
		this->SET_ToolStripMenuItem->Size = System::Drawing::Size(44, 21);
		this->SET_ToolStripMenuItem->Text = L"设置";
		// 
		// PATH_SET_ToolStripMenuItem
		// 
		this->PATH_SET_ToolStripMenuItem->Name = L"PATH_SET_ToolStripMenuItem";
		this->PATH_SET_ToolStripMenuItem->Size = System::Drawing::Size(148, 22);
		this->PATH_SET_ToolStripMenuItem->Text = L"工作路径设置";
		this->PATH_SET_ToolStripMenuItem->Click += gcnew System::EventHandler(this, &CharacterBrowserForm::PATH_SET_ToolStripMenuItem_Click);
		// 
		// menuStrip
		// 
		this->menuStrip->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(3) {this->TOOL_ToolStripMenuItem, 
			this->SHOW_BRUSH_ToolStripMenuItem, this->SET_ToolStripMenuItem});
		this->menuStrip->Location = System::Drawing::Point(0, 0);
		this->menuStrip->Name = L"menuStrip";
		this->menuStrip->Size = System::Drawing::Size(854, 25);
		this->menuStrip->TabIndex = 0;
		this->menuStrip->Text = L"menuStrip1";
		// 
		// tabPage2
		// 
		this->tabPage2->Controls->Add(this->Equip_Info_textBox);
		this->tabPage2->Controls->Add(this->groupBox3);
		this->tabPage2->Controls->Add(this->groupBox2);
		this->tabPage2->Controls->Add(this->groupBox1);
		this->tabPage2->Location = System::Drawing::Point(4, 22);
		this->tabPage2->Name = L"tabPage2";
		this->tabPage2->Padding = System::Windows::Forms::Padding(3);
		this->tabPage2->Size = System::Drawing::Size(321, 441);
		this->tabPage2->TabIndex = 1;
		this->tabPage2->Text = L"装备查看";
		this->tabPage2->UseVisualStyleBackColor = true;
		// 
		// Equip_Info_textBox
		// 
		this->Equip_Info_textBox->Location = System::Drawing::Point(13, 432);
		this->Equip_Info_textBox->Multiline = true;
		this->Equip_Info_textBox->Name = L"Equip_Info_textBox";
		this->Equip_Info_textBox->ReadOnly = true;
		this->Equip_Info_textBox->ScrollBars = System::Windows::Forms::ScrollBars::Both;
		this->Equip_Info_textBox->Size = System::Drawing::Size(296, 93);
		this->Equip_Info_textBox->TabIndex = 5;
		// 
		// groupBox3
		// 
		this->groupBox3->Controls->Add(this->Speed_textBox);
		this->groupBox3->Controls->Add(this->Ani_Speed_trackBar);
		this->groupBox3->Controls->Add(this->label16);
		this->groupBox3->Controls->Add(this->Player_ANI_comboBox);
		this->groupBox3->Controls->Add(this->label15);
		this->groupBox3->Controls->Add(this->Player_AGP_comboBox);
		this->groupBox3->Controls->Add(this->label12);
		this->groupBox3->Location = System::Drawing::Point(11, 321);
		this->groupBox3->Name = L"groupBox3";
		this->groupBox3->Size = System::Drawing::Size(299, 99);
		this->groupBox3->TabIndex = 4;
		this->groupBox3->TabStop = false;
		this->groupBox3->Text = L"动作控制";
		// 
		// Speed_textBox
		// 
		this->Speed_textBox->Location = System::Drawing::Point(255, 65);
		this->Speed_textBox->Name = L"Speed_textBox";
		this->Speed_textBox->Size = System::Drawing::Size(31, 21);
		this->Speed_textBox->TabIndex = 6;
		this->Speed_textBox->TextChanged += gcnew System::EventHandler(this, &CharacterBrowserForm::Speed_textBox_TextChanged);
		// 
		// Ani_Speed_trackBar
		// 
		this->Ani_Speed_trackBar->AutoSize = false;
		this->Ani_Speed_trackBar->BackColor = System::Drawing::SystemColors::Window;
		this->Ani_Speed_trackBar->Location = System::Drawing::Point(74, 63);
		this->Ani_Speed_trackBar->Name = L"Ani_Speed_trackBar";
		this->Ani_Speed_trackBar->Size = System::Drawing::Size(184, 31);
		this->Ani_Speed_trackBar->TabIndex = 5;
		this->Ani_Speed_trackBar->Scroll += gcnew System::EventHandler(this, &CharacterBrowserForm::Ani_Speed_trackBar_Scroll);
		// 
		// label16
		// 
		this->label16->AutoSize = true;
		this->label16->Location = System::Drawing::Point(15, 72);
		this->label16->Name = L"label16";
		this->label16->Size = System::Drawing::Size(53, 12);
		this->label16->TabIndex = 4;
		this->label16->Text = L"播放速度";
		// 
		// Player_ANI_comboBox
		// 
		this->Player_ANI_comboBox->FormattingEnabled = true;
		this->Player_ANI_comboBox->Location = System::Drawing::Point(86, 40);
		this->Player_ANI_comboBox->Name = L"Player_ANI_comboBox";
		this->Player_ANI_comboBox->Size = System::Drawing::Size(200, 20);
		this->Player_ANI_comboBox->TabIndex = 3;
		this->Player_ANI_comboBox->SelectedIndexChanged += gcnew System::EventHandler(this, &CharacterBrowserForm::Player_ANI_comboBox_SelectedIndexChanged);
		// 
		// label15
		// 
		this->label15->AutoSize = true;
		this->label15->Location = System::Drawing::Point(10, 43);
		this->label15->Name = L"label15";
		this->label15->Size = System::Drawing::Size(65, 12);
		this->label15->TabIndex = 2;
		this->label15->Text = L"主角动作名";
		// 
		// Player_AGP_comboBox
		// 
		this->Player_AGP_comboBox->FormattingEnabled = true;
		this->Player_AGP_comboBox->Location = System::Drawing::Point(86, 17);
		this->Player_AGP_comboBox->Name = L"Player_AGP_comboBox";
		this->Player_AGP_comboBox->Size = System::Drawing::Size(200, 20);
		this->Player_AGP_comboBox->TabIndex = 1;
		this->Player_AGP_comboBox->SelectedIndexChanged += gcnew System::EventHandler(this, &CharacterBrowserForm::Player_AGP_comboBox_SelectedIndexChanged);
		// 
		// label12
		// 
		this->label12->AutoSize = true;
		this->label12->Location = System::Drawing::Point(11, 21);
		this->label12->Name = L"label12";
		this->label12->Size = System::Drawing::Size(65, 12);
		this->label12->TabIndex = 0;
		this->label12->Text = L"主角动作包";
		// 
		// groupBox2
		// 
		this->groupBox2->Controls->Add(this->SubWeapon_Adv_comboBox);
		this->groupBox2->Controls->Add(this->Player_SubWeapon_comboBox);
		this->groupBox2->Controls->Add(this->label13);
		this->groupBox2->Controls->Add(this->MainWeapon_Adv_comboBox);
		this->groupBox2->Controls->Add(this->Player_MainWeapon_comboBox);
		this->groupBox2->Controls->Add(this->label14);
		this->groupBox2->Controls->Add(this->Boot_Adv_comboBox);
		this->groupBox2->Controls->Add(this->Player_Boot_comboBox);
		this->groupBox2->Controls->Add(this->label9);
		this->groupBox2->Controls->Add(this->Cuff_Adv_comboBox);
		this->groupBox2->Controls->Add(this->Player_Cuff_comboBox);
		this->groupBox2->Controls->Add(this->label10);
		this->groupBox2->Controls->Add(this->Mantle_Adv_comboBox);
		this->groupBox2->Controls->Add(this->Player_Mantle_comboBox);
		this->groupBox2->Controls->Add(this->label11);
		this->groupBox2->Controls->Add(this->Body_Adv_comboBox);
		this->groupBox2->Controls->Add(this->Player_Body_comboBox);
		this->groupBox2->Controls->Add(this->label8);
		this->groupBox2->Controls->Add(this->Shoulder_Adv_comboBox);
		this->groupBox2->Controls->Add(this->Player_Shoulder_comboBox);
		this->groupBox2->Controls->Add(this->label7);
		this->groupBox2->Controls->Add(this->Equip_Group_comboBox);
		this->groupBox2->Controls->Add(this->label6);
		this->groupBox2->Controls->Add(this->Head_Adv_comboBox);
		this->groupBox2->Controls->Add(this->Player_Head_comboBox);
		this->groupBox2->Controls->Add(this->label5);
		this->groupBox2->Location = System::Drawing::Point(10, 80);
		this->groupBox2->Name = L"groupBox2";
		this->groupBox2->Size = System::Drawing::Size(301, 235);
		this->groupBox2->TabIndex = 3;
		this->groupBox2->TabStop = false;
		this->groupBox2->Text = L"装备选择";
		// 
		// SubWeapon_Adv_comboBox
		// 
		this->SubWeapon_Adv_comboBox->FormattingEnabled = true;
		this->SubWeapon_Adv_comboBox->Location = System::Drawing::Point(233, 204);
		this->SubWeapon_Adv_comboBox->Name = L"SubWeapon_Adv_comboBox";
		this->SubWeapon_Adv_comboBox->Size = System::Drawing::Size(55, 20);
		this->SubWeapon_Adv_comboBox->TabIndex = 25;
		this->SubWeapon_Adv_comboBox->SelectedIndexChanged += gcnew System::EventHandler(this, &CharacterBrowserForm::SubWeapon_Adv_comboBox_SelectedIndexChanged);
		// 
		// Player_SubWeapon_comboBox
		// 
		this->Player_SubWeapon_comboBox->FormattingEnabled = true;
		this->Player_SubWeapon_comboBox->Location = System::Drawing::Point(48, 204);
		this->Player_SubWeapon_comboBox->Name = L"Player_SubWeapon_comboBox";
		this->Player_SubWeapon_comboBox->Size = System::Drawing::Size(174, 20);
		this->Player_SubWeapon_comboBox->TabIndex = 24;
		this->Player_SubWeapon_comboBox->SelectedIndexChanged += gcnew System::EventHandler(this, &CharacterBrowserForm::Sub_Weapon_comboBox_SelectedIndexChanged);
		// 
		// label13
		// 
		this->label13->AutoSize = true;
		this->label13->Location = System::Drawing::Point(13, 208);
		this->label13->Name = L"label13";
		this->label13->Size = System::Drawing::Size(29, 12);
		this->label13->TabIndex = 23;
		this->label13->Text = L"副手";
		// 
		// MainWeapon_Adv_comboBox
		// 
		this->MainWeapon_Adv_comboBox->FormattingEnabled = true;
		this->MainWeapon_Adv_comboBox->Location = System::Drawing::Point(233, 181);
		this->MainWeapon_Adv_comboBox->Name = L"MainWeapon_Adv_comboBox";
		this->MainWeapon_Adv_comboBox->Size = System::Drawing::Size(55, 20);
		this->MainWeapon_Adv_comboBox->TabIndex = 22;
		this->MainWeapon_Adv_comboBox->SelectedIndexChanged += gcnew System::EventHandler(this, &CharacterBrowserForm::MainWeapon_Adv_comboBox_SelectedIndexChanged);
		// 
		// Player_MainWeapon_comboBox
		// 
		this->Player_MainWeapon_comboBox->FormattingEnabled = true;
		this->Player_MainWeapon_comboBox->Location = System::Drawing::Point(47, 181);
		this->Player_MainWeapon_comboBox->Name = L"Player_MainWeapon_comboBox";
		this->Player_MainWeapon_comboBox->Size = System::Drawing::Size(174, 20);
		this->Player_MainWeapon_comboBox->TabIndex = 21;
		this->Player_MainWeapon_comboBox->SelectedIndexChanged += gcnew System::EventHandler(this, &CharacterBrowserForm::Main_Weapon_comboBox_SelectedIndexChanged);
		// 
		// label14
		// 
		this->label14->AutoSize = true;
		this->label14->Location = System::Drawing::Point(12, 185);
		this->label14->Name = L"label14";
		this->label14->Size = System::Drawing::Size(29, 12);
		this->label14->TabIndex = 20;
		this->label14->Text = L"主手";
		// 
		// Boot_Adv_comboBox
		// 
		this->Boot_Adv_comboBox->FormattingEnabled = true;
		this->Boot_Adv_comboBox->Location = System::Drawing::Point(233, 158);
		this->Boot_Adv_comboBox->Name = L"Boot_Adv_comboBox";
		this->Boot_Adv_comboBox->Size = System::Drawing::Size(55, 20);
		this->Boot_Adv_comboBox->TabIndex = 19;
		this->Boot_Adv_comboBox->SelectedIndexChanged += gcnew System::EventHandler(this, &CharacterBrowserForm::Boot_Adv_comboBox_SelectedIndexChanged);
		// 
		// Player_Boot_comboBox
		// 
		this->Player_Boot_comboBox->FormattingEnabled = true;
		this->Player_Boot_comboBox->Location = System::Drawing::Point(48, 158);
		this->Player_Boot_comboBox->Name = L"Player_Boot_comboBox";
		this->Player_Boot_comboBox->Size = System::Drawing::Size(174, 20);
		this->Player_Boot_comboBox->TabIndex = 18;
		this->Player_Boot_comboBox->SelectedIndexChanged += gcnew System::EventHandler(this, &CharacterBrowserForm::Player_Boot_comboBox_SelectedIndexChanged);
		// 
		// label9
		// 
		this->label9->AutoSize = true;
		this->label9->Location = System::Drawing::Point(11, 161);
		this->label9->Name = L"label9";
		this->label9->Size = System::Drawing::Size(29, 12);
		this->label9->TabIndex = 17;
		this->label9->Text = L"靴子";
		// 
		// Cuff_Adv_comboBox
		// 
		this->Cuff_Adv_comboBox->FormattingEnabled = true;
		this->Cuff_Adv_comboBox->Location = System::Drawing::Point(233, 135);
		this->Cuff_Adv_comboBox->Name = L"Cuff_Adv_comboBox";
		this->Cuff_Adv_comboBox->Size = System::Drawing::Size(55, 20);
		this->Cuff_Adv_comboBox->TabIndex = 16;
		this->Cuff_Adv_comboBox->SelectedIndexChanged += gcnew System::EventHandler(this, &CharacterBrowserForm::Cuff_Adv_comboBox_SelectedIndexChanged);
		// 
		// Player_Cuff_comboBox
		// 
		this->Player_Cuff_comboBox->FormattingEnabled = true;
		this->Player_Cuff_comboBox->Location = System::Drawing::Point(48, 135);
		this->Player_Cuff_comboBox->Name = L"Player_Cuff_comboBox";
		this->Player_Cuff_comboBox->Size = System::Drawing::Size(174, 20);
		this->Player_Cuff_comboBox->TabIndex = 15;
		this->Player_Cuff_comboBox->SelectedIndexChanged += gcnew System::EventHandler(this, &CharacterBrowserForm::Player_Cuff_comboBox_SelectedIndexChanged);
		// 
		// label10
		// 
		this->label10->AutoSize = true;
		this->label10->Location = System::Drawing::Point(13, 139);
		this->label10->Name = L"label10";
		this->label10->Size = System::Drawing::Size(29, 12);
		this->label10->TabIndex = 14;
		this->label10->Text = L"护腕";
		// 
		// Mantle_Adv_comboBox
		// 
		this->Mantle_Adv_comboBox->FormattingEnabled = true;
		this->Mantle_Adv_comboBox->Location = System::Drawing::Point(234, 112);
		this->Mantle_Adv_comboBox->Name = L"Mantle_Adv_comboBox";
		this->Mantle_Adv_comboBox->Size = System::Drawing::Size(54, 20);
		this->Mantle_Adv_comboBox->TabIndex = 13;
		this->Mantle_Adv_comboBox->SelectedIndexChanged += gcnew System::EventHandler(this, &CharacterBrowserForm::Mantle_Adv_comboBox_SelectedIndexChanged);
		// 
		// Player_Mantle_comboBox
		// 
		this->Player_Mantle_comboBox->FormattingEnabled = true;
		this->Player_Mantle_comboBox->Location = System::Drawing::Point(47, 112);
		this->Player_Mantle_comboBox->Name = L"Player_Mantle_comboBox";
		this->Player_Mantle_comboBox->Size = System::Drawing::Size(174, 20);
		this->Player_Mantle_comboBox->TabIndex = 12;
		this->Player_Mantle_comboBox->SelectedIndexChanged += gcnew System::EventHandler(this, &CharacterBrowserForm::Player_Mantle_comboBox_SelectedIndexChanged);
		// 
		// label11
		// 
		this->label11->AutoSize = true;
		this->label11->Location = System::Drawing::Point(12, 116);
		this->label11->Name = L"label11";
		this->label11->Size = System::Drawing::Size(29, 12);
		this->label11->TabIndex = 11;
		this->label11->Text = L"披风";
		// 
		// Body_Adv_comboBox
		// 
		this->Body_Adv_comboBox->FormattingEnabled = true;
		this->Body_Adv_comboBox->Location = System::Drawing::Point(234, 89);
		this->Body_Adv_comboBox->Name = L"Body_Adv_comboBox";
		this->Body_Adv_comboBox->Size = System::Drawing::Size(54, 20);
		this->Body_Adv_comboBox->TabIndex = 10;
		this->Body_Adv_comboBox->SelectedIndexChanged += gcnew System::EventHandler(this, &CharacterBrowserForm::Body_Adv_comboBox_SelectedIndexChanged);
		// 
		// Player_Body_comboBox
		// 
		this->Player_Body_comboBox->FormattingEnabled = true;
		this->Player_Body_comboBox->Location = System::Drawing::Point(47, 89);
		this->Player_Body_comboBox->Name = L"Player_Body_comboBox";
		this->Player_Body_comboBox->Size = System::Drawing::Size(174, 20);
		this->Player_Body_comboBox->TabIndex = 9;
		this->Player_Body_comboBox->SelectedIndexChanged += gcnew System::EventHandler(this, &CharacterBrowserForm::Player_Body_comboBox_SelectedIndexChanged);
		// 
		// label8
		// 
		this->label8->AutoSize = true;
		this->label8->Location = System::Drawing::Point(12, 93);
		this->label8->Name = L"label8";
		this->label8->Size = System::Drawing::Size(29, 12);
		this->label8->TabIndex = 8;
		this->label8->Text = L"身体";
		// 
		// Shoulder_Adv_comboBox
		// 
		this->Shoulder_Adv_comboBox->FormattingEnabled = true;
		this->Shoulder_Adv_comboBox->Location = System::Drawing::Point(234, 66);
		this->Shoulder_Adv_comboBox->Name = L"Shoulder_Adv_comboBox";
		this->Shoulder_Adv_comboBox->Size = System::Drawing::Size(54, 20);
		this->Shoulder_Adv_comboBox->TabIndex = 7;
		this->Shoulder_Adv_comboBox->SelectedIndexChanged += gcnew System::EventHandler(this, &CharacterBrowserForm::Shoulder_Adv_comboBox_SelectedIndexChanged);
		// 
		// Player_Shoulder_comboBox
		// 
		this->Player_Shoulder_comboBox->FormattingEnabled = true;
		this->Player_Shoulder_comboBox->Location = System::Drawing::Point(47, 66);
		this->Player_Shoulder_comboBox->Name = L"Player_Shoulder_comboBox";
		this->Player_Shoulder_comboBox->Size = System::Drawing::Size(174, 20);
		this->Player_Shoulder_comboBox->TabIndex = 6;
		this->Player_Shoulder_comboBox->SelectedIndexChanged += gcnew System::EventHandler(this, &CharacterBrowserForm::Player_Shoulder_comboBox_SelectedIndexChanged);
		// 
		// label7
		// 
		this->label7->AutoSize = true;
		this->label7->Location = System::Drawing::Point(12, 70);
		this->label7->Name = L"label7";
		this->label7->Size = System::Drawing::Size(29, 12);
		this->label7->TabIndex = 5;
		this->label7->Text = L"披肩";
		// 
		// Equip_Group_comboBox
		// 
		this->Equip_Group_comboBox->FormattingEnabled = true;
		this->Equip_Group_comboBox->Location = System::Drawing::Point(46, 20);
		this->Equip_Group_comboBox->Name = L"Equip_Group_comboBox";
		this->Equip_Group_comboBox->Size = System::Drawing::Size(174, 20);
		this->Equip_Group_comboBox->TabIndex = 1;
		this->Equip_Group_comboBox->SelectedIndexChanged += gcnew System::EventHandler(this, &CharacterBrowserForm::Equip_Group_comboBox_SelectedIndexChanged);
		// 
		// label6
		// 
		this->label6->AutoSize = true;
		this->label6->Location = System::Drawing::Point(12, 23);
		this->label6->Name = L"label6";
		this->label6->Size = System::Drawing::Size(29, 12);
		this->label6->TabIndex = 0;
		this->label6->Text = L"套装";
		// 
		// Head_Adv_comboBox
		// 
		this->Head_Adv_comboBox->FormattingEnabled = true;
		this->Head_Adv_comboBox->Location = System::Drawing::Point(233, 43);
		this->Head_Adv_comboBox->Name = L"Head_Adv_comboBox";
		this->Head_Adv_comboBox->Size = System::Drawing::Size(55, 20);
		this->Head_Adv_comboBox->TabIndex = 4;
		this->Head_Adv_comboBox->SelectedIndexChanged += gcnew System::EventHandler(this, &CharacterBrowserForm::Head_Adv_comboBox_SelectedIndexChanged);
		// 
		// Player_Head_comboBox
		// 
		this->Player_Head_comboBox->FormattingEnabled = true;
		this->Player_Head_comboBox->Location = System::Drawing::Point(46, 43);
		this->Player_Head_comboBox->Name = L"Player_Head_comboBox";
		this->Player_Head_comboBox->Size = System::Drawing::Size(174, 20);
		this->Player_Head_comboBox->TabIndex = 3;
		this->Player_Head_comboBox->SelectedIndexChanged += gcnew System::EventHandler(this, &CharacterBrowserForm::Player_Head_comboBox_SelectedIndexChanged);
		// 
		// label5
		// 
		this->label5->AutoSize = true;
		this->label5->Location = System::Drawing::Point(11, 47);
		this->label5->Name = L"label5";
		this->label5->Size = System::Drawing::Size(29, 12);
		this->label5->TabIndex = 2;
		this->label5->Text = L"头盔";
		// 
		// groupBox1
		// 
		this->groupBox1->Controls->Add(this->comboBox1);
		this->groupBox1->Controls->Add(this->label4);
		this->groupBox1->Controls->Add(this->Player_Hair_comboBox);
		this->groupBox1->Controls->Add(this->label3);
		this->groupBox1->Controls->Add(this->Player_Face_comboBox);
		this->groupBox1->Controls->Add(this->label2);
		this->groupBox1->Controls->Add(this->PlayType_comboBox);
		this->groupBox1->Controls->Add(this->label1);
		this->groupBox1->Location = System::Drawing::Point(12, 6);
		this->groupBox1->Name = L"groupBox1";
		this->groupBox1->Size = System::Drawing::Size(299, 73);
		this->groupBox1->TabIndex = 2;
		this->groupBox1->TabStop = false;
		this->groupBox1->Text = L"类型选择";
		// 
		// comboBox1
		// 
		this->comboBox1->FormattingEnabled = true;
		this->comboBox1->Location = System::Drawing::Point(62, 106);
		this->comboBox1->Name = L"comboBox1";
		this->comboBox1->Size = System::Drawing::Size(84, 20);
		this->comboBox1->TabIndex = 7;
		// 
		// label4
		// 
		this->label4->AutoSize = true;
		this->label4->Location = System::Drawing::Point(18, 109);
		this->label4->Name = L"label4";
		this->label4->Size = System::Drawing::Size(29, 12);
		this->label4->TabIndex = 6;
		this->label4->Text = L"相貌";
		// 
		// Player_Hair_comboBox
		// 
		this->Player_Hair_comboBox->FormattingEnabled = true;
		this->Player_Hair_comboBox->Location = System::Drawing::Point(191, 45);
		this->Player_Hair_comboBox->Name = L"Player_Hair_comboBox";
		this->Player_Hair_comboBox->Size = System::Drawing::Size(95, 20);
		this->Player_Hair_comboBox->TabIndex = 5;
		this->Player_Hair_comboBox->SelectedIndexChanged += gcnew System::EventHandler(this, &CharacterBrowserForm::Player_Hair_comboBox_SelectedIndexChanged);
		// 
		// label3
		// 
		this->label3->AutoSize = true;
		this->label3->Location = System::Drawing::Point(158, 48);
		this->label3->Name = L"label3";
		this->label3->Size = System::Drawing::Size(29, 12);
		this->label3->TabIndex = 4;
		this->label3->Text = L"头发";
		// 
		// Player_Face_comboBox
		// 
		this->Player_Face_comboBox->FormattingEnabled = true;
		this->Player_Face_comboBox->Location = System::Drawing::Point(41, 44);
		this->Player_Face_comboBox->Name = L"Player_Face_comboBox";
		this->Player_Face_comboBox->RightToLeft = System::Windows::Forms::RightToLeft::No;
		this->Player_Face_comboBox->Size = System::Drawing::Size(108, 20);
		this->Player_Face_comboBox->TabIndex = 3;
		this->Player_Face_comboBox->SelectedIndexChanged += gcnew System::EventHandler(this, &CharacterBrowserForm::Player_Face_comboBox_SelectedIndexChanged);
		// 
		// label2
		// 
		this->label2->AutoSize = true;
		this->label2->Location = System::Drawing::Point(9, 47);
		this->label2->Name = L"label2";
		this->label2->Size = System::Drawing::Size(29, 12);
		this->label2->TabIndex = 2;
		this->label2->Text = L"相貌";
		// 
		// PlayType_comboBox
		// 
		this->PlayType_comboBox->FormattingEnabled = true;
		this->PlayType_comboBox->Location = System::Drawing::Point(82, 15);
		this->PlayType_comboBox->Name = L"PlayType_comboBox";
		this->PlayType_comboBox->Size = System::Drawing::Size(137, 20);
		this->PlayType_comboBox->TabIndex = 1;
		this->PlayType_comboBox->SelectedIndexChanged += gcnew System::EventHandler(this, &CharacterBrowserForm::PlayType_comboBox_SelectedIndexChanged);
		// 
		// label1
		// 
		this->label1->AutoSize = true;
		this->label1->Location = System::Drawing::Point(25, 19);
		this->label1->Name = L"label1";
		this->label1->Size = System::Drawing::Size(53, 12);
		this->label1->TabIndex = 0;
		this->label1->Text = L"主角类型";
		// 
		// tabPage1
		// 
		this->tabPage1->Controls->Add(this->OBJ_treeView);
		this->tabPage1->Controls->Add(this->NPC_treeView);
		this->tabPage1->Controls->Add(this->Search_Button);
		this->tabPage1->Controls->Add(this->Search_textBox);
		this->tabPage1->Controls->Add(this->npcobj_groupBox);
		this->tabPage1->Location = System::Drawing::Point(4, 22);
		this->tabPage1->Name = L"tabPage1";
		this->tabPage1->Padding = System::Windows::Forms::Padding(3);
		this->tabPage1->Size = System::Drawing::Size(321, 441);
		this->tabPage1->TabIndex = 0;
		this->tabPage1->Text = L"角色查看";
		this->tabPage1->UseVisualStyleBackColor = true;
		// 
		// OBJ_treeView
		// 
		this->OBJ_treeView->Location = System::Drawing::Point(5, 79);
		this->OBJ_treeView->Name = L"OBJ_treeView";
		this->OBJ_treeView->Size = System::Drawing::Size(307, 451);
		this->OBJ_treeView->TabIndex = 5;
		this->OBJ_treeView->NodeMouseClick += gcnew System::Windows::Forms::TreeNodeMouseClickEventHandler(this, &CharacterBrowserForm::OBJ_treeView_NodeMouseClick);
		// 
		// NPC_treeView
		// 
		this->NPC_treeView->Location = System::Drawing::Point(6, 81);
		this->NPC_treeView->Name = L"NPC_treeView";
		this->NPC_treeView->Size = System::Drawing::Size(307, 449);
		this->NPC_treeView->TabIndex = 4;
		this->NPC_treeView->NodeMouseClick += gcnew System::Windows::Forms::TreeNodeMouseClickEventHandler(this, &CharacterBrowserForm::NPC_treeView_NodeMouseClick);

		// 
		// Search_Button
		// 
		this->Search_Button->Location = System::Drawing::Point(10, 45);
		this->Search_Button->Name = L"Search_Button";
		this->Search_Button->Size = System::Drawing::Size(44, 27);
		this->Search_Button->TabIndex = 3;
		this->Search_Button->Text = L"搜索";
		this->Search_Button->UseVisualStyleBackColor = true;
		this->Search_Button->Click += gcnew System::EventHandler(this, &CharacterBrowserForm::Search_Button_Click);
		// 
		// Search_textBox
		// 
		this->Search_textBox->ImeMode = System::Windows::Forms::ImeMode::NoControl;
		this->Search_textBox->Location = System::Drawing::Point(55, 49);
		this->Search_textBox->Name = L"Search_textBox";
		this->Search_textBox->Size = System::Drawing::Size(115, 21);
		this->Search_textBox->TabIndex = 2;
		this->Search_textBox->TextChanged += gcnew System::EventHandler(this, &CharacterBrowserForm::Search_textBox_TextChanged);
		// 
		// npcobj_groupBox
		// 
		this->npcobj_groupBox->Controls->Add(this->OBJ_radioButton);
		this->npcobj_groupBox->Controls->Add(this->NPC_radioButton);
		this->npcobj_groupBox->Location = System::Drawing::Point(6, 5);
		this->npcobj_groupBox->Name = L"npcobj_groupBox";
		this->npcobj_groupBox->Size = System::Drawing::Size(309, 37);
		this->npcobj_groupBox->TabIndex = 0;
		this->npcobj_groupBox->TabStop = false;
		this->npcobj_groupBox->Text = L"列表选项";
		// 
		// OBJ_radioButton
		// 
		this->OBJ_radioButton->AutoSize = true;
		this->OBJ_radioButton->Location = System::Drawing::Point(134, 14);
		this->OBJ_radioButton->Name = L"OBJ_radioButton";
		this->OBJ_radioButton->Size = System::Drawing::Size(65, 16);
		this->OBJ_radioButton->TabIndex = 1;
		this->OBJ_radioButton->Text = L"OBJ列表";
		this->OBJ_radioButton->UseVisualStyleBackColor = true;
		this->OBJ_radioButton->CheckedChanged += gcnew System::EventHandler(this, &CharacterBrowserForm::OBJ_radioButton_CheckedChanged);
		// 
		// NPC_radioButton
		// 
		this->NPC_radioButton->AutoSize = true;
		this->NPC_radioButton->Location = System::Drawing::Point(12, 14);
		this->NPC_radioButton->Name = L"NPC_radioButton";
		this->NPC_radioButton->Size = System::Drawing::Size(65, 16);
		this->NPC_radioButton->TabIndex = 0;
		this->NPC_radioButton->TabStop = true;
		this->NPC_radioButton->Text = L"NPC列表";
		this->NPC_radioButton->UseVisualStyleBackColor = true;
		this->NPC_radioButton->CheckedChanged += gcnew System::EventHandler(this, &CharacterBrowserForm::NPC_radioButton_CheckedChanged);
		// 
		// tabControl1
		// 
		this->tabControl1->Controls->Add(this->tabPage1);
		this->tabControl1->Controls->Add(this->tabPage2);
		this->tabControl1->Dock = System::Windows::Forms::DockStyle::Right;
		this->tabControl1->Location = System::Drawing::Point(525, 25);
		this->tabControl1->Name = L"tabControl1";
		this->tabControl1->SelectedIndex = 0;
		this->tabControl1->Size = System::Drawing::Size(329, 467);
		this->tabControl1->TabIndex = 2;
		this->tabControl1->SelectedIndexChanged += gcnew System::EventHandler(this, &CharacterBrowserForm::tabControl1_SelectedIndexChanged);
		// 
		// cViewCtrl
		// 
		this->cViewCtrl->BackColor = System::Drawing::Color::LightGray;
		this->cViewCtrl->ContextComponent = this->cContextComponent;
		this->cViewCtrl->Dock = System::Windows::Forms::DockStyle::Fill;
		this->cViewCtrl->ForeColor = System::Drawing::SystemColors::ActiveCaption;
		this->cViewCtrl->Location = System::Drawing::Point(0, 25);
		this->cViewCtrl->Name = L"cViewCtrl";
		this->cViewCtrl->Size = System::Drawing::Size(525, 467);
		this->cViewCtrl->TabIndex = 3;
		// 
		// CharacterBrowserForm
		// 
		this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
		this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
		this->ClientSize = System::Drawing::Size(854, 492);
		this->Controls->Add(this->cViewCtrl);
		this->Controls->Add(this->tabControl1);
		this->Controls->Add(this->menuStrip);
		this->MainMenuStrip = this->menuStrip;
		this->Name = L"CharacterBrowserForm";
		this->Text = L"CharacterBrowserForm";
		this->Load += gcnew System::EventHandler(this, &CharacterBrowserForm::CharacterBrowserForm_Load);
		this->SizeChanged += gcnew System::EventHandler(this, &CharacterBrowserForm::CharacterBrowserForm_SizeChanged);
		this->menuStrip->ResumeLayout(false);
		this->menuStrip->PerformLayout();
		this->tabPage2->ResumeLayout(false);
		this->tabPage2->PerformLayout();
		this->groupBox3->ResumeLayout(false);
		this->groupBox3->PerformLayout();
		(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->Ani_Speed_trackBar))->EndInit();
		this->groupBox2->ResumeLayout(false);
		this->groupBox2->PerformLayout();
		this->groupBox1->ResumeLayout(false);
		this->groupBox1->PerformLayout();
		this->tabPage1->ResumeLayout(false);
		this->tabPage1->PerformLayout();
		this->npcobj_groupBox->ResumeLayout(false);
		this->npcobj_groupBox->PerformLayout();
		this->tabControl1->ResumeLayout(false);
		this->ResumeLayout(false);
		this->PerformLayout();

	}
#pragma endregion

	void CharacterBrowserForm::InitComponentValue()
	{
		//tab1
		NPC_radioButton->Checked = true;

		//tab2 test
		this->Speed_textBox->Text = "4";
		this->lastModelInfo		  = "";

		this->InitCtrlFormLua();

		pCharacterAttributes = new CCharacterAttributes;
		pCharacterAttributes->Init();
	}

#pragma region LuaValue
	void CharacterBrowserForm::InitCtrlFormLua()
	{
		//tab1
 		vector<string> names;
 		gs_pCahce->ShowIntNpcName(names);

 		size_t size = names.size();
 		for ( size_t i = 0; i < size; ++i )
 		{
  			string name = names[i];
  			String^ clrName = gcnew String(name.c_str());
  			
			TreeNode ^parentNode = gcnew TreeNode;
			parentNode->Text = clrName;
			parentNode->ImageIndex = 0;
			this->NPC_treeView->Nodes->Add(parentNode);
 			
			delete clrName;
 		}
		this->treeNodeBackColor	= this->NPC_treeView->BackColor;
 
 		names.clear();
 		gs_pCahce->ShowIntObjName(names);
 		size = names.size();
 		for ( size_t i = 0; i < size; ++i )
 		{
			string name = names[i];
			String^ clrName = gcnew String(name.c_str());

			TreeNode ^parentNode = gcnew TreeNode;
			parentNode->Text = clrName;
			parentNode->ImageIndex = 0;
			this->OBJ_treeView->Nodes->Add(parentNode);

			delete clrName;
 		}
 
 		this->NPC_treeView->Visible = true;
 		this->OBJ_treeView->Visible = false;

		//tab2
		this->Ani_Speed_trackBar->Minimum = 0;
		this->Ani_Speed_trackBar->Maximum = 8;
		this->Ani_Speed_trackBar->LargeChange = 1;
		this->Ani_Speed_trackBar->SmallChange = 1;
		this->Ani_Speed_trackBar->TickFrequency = 1;//每次移动的刻度
		this->Ani_Speed_trackBar->Value = 4; //初始位置

	}
#pragma endregion

	System::Void CharacterBrowserForm::CharacterBrowserForm_Load( System::Object^ sender, System::EventArgs^ e )
	{
		this->InitComponentValue();

		CGTRenderMgr::Create();
		CGTRenderMgr::GetInst()->SetRenderSceneContext(CtrlImpPtr<CContextCtrlImp>(cContextComponent->GetCoreCtrl())->GetContext());
	}

	System::Void CharacterBrowserForm::CHECK_ANIMATE_ToolStripMenuItem_Click( System::Object^ sender, System::EventArgs^ e )
	{
	}

	System::Void CharacterBrowserForm::ONEPOINTFIVE_ToolStripMenuItem_Click( System::Object^ sender, System::EventArgs^ e )
	{
		CGTRenderMgr::GetInst()->DisplayCharacterAperture(1.5f);
	}

	System::Void CharacterBrowserForm::TWOPOINT_ToolStripMenuItem1_Click( System::Object^ sender, System::EventArgs^ e )
	{
		CGTRenderMgr::GetInst()->DisplayCharacterAperture(2.0f);
	}

	System::Void CharacterBrowserForm::SELFDEFINE_ToolStripMenuItem_Click( System::Object^ sender, System::EventArgs^ e )
	{
		CDefineScaleDlg dlg;
 		dlg.ShowDialog();
 		float fScale = (float)(dlg.GetScale());
 		CGTRenderMgr::GetInst()->DisplayCharacterAperture(fScale);
	}

	System::Void CharacterBrowserForm::HIDEAPERTURE_ToolStripMenuItem_Click( System::Object^ sender, System::EventArgs^ e )
	{
		CGTRenderMgr::GetInst()->HideApertureAndRefer();
	}

	System::Void CharacterBrowserForm::NPC_treeView_NodeMouseClick( System::Object^  sender, System::Windows::Forms::TreeNodeMouseClickEventArgs^  e )
	{
		TreeNode ^seleceNode = this->NPC_treeView->SelectedNode;
		if( seleceNode )
		{
			string  strname = "";
			int nSelectImageInex = seleceNode->ImageIndex;
			if( nSelectImageInex == 0 )
			{
				String^ clrname = seleceNode->Text;
				MarshalString(clrname, strname);	
				strname = CDataCache::GetNpc_ObjRealName(strname);
				if( lastModelInfo != clrname )
				{
					seleceNode->Nodes->Clear();

					vector<string> agpNames;
					gs_pCahce->GetAnimateNames(CGTRenderMgr::GetInst()->CreateCharacter(strname, eMT_Character), agpNames);
					size_t size = agpNames.size();
					for ( size_t i = 0; i < size; ++i )
					{
						string  strAniName = agpNames[i];
						String^ clrAniName = gcnew String(strAniName.c_str());

						TreeNode ^childNode = gcnew TreeNode();
						childNode->Text = clrAniName;
						childNode->ImageIndex = 2;
						childNode->SelectedImageIndex = 3;
						seleceNode->Nodes->Add(childNode);
					}
				}
				
				seleceNode->Expand();

				if( lastModelInfo != clrname )
				{
					if( this->lastNpcSeleceNode )
					{
						this->lastNpcSeleceNode->Collapse();
						this->lastNpcSeleceNode->BackColor = System::Drawing::SystemColors::Window;
					}

					lastModelInfo = clrname;
					this->lastNpcSeleceNode = seleceNode;
				}
			}
			else
			{
				String^ clrname = seleceNode->Text;
				MarshalString(clrname, strname);	

				CGTRenderMgr::GetInst()->DoAnimate(strname);
			}
		}
	}

	System::Void CharacterBrowserForm::OBJ_treeView_NodeMouseClick(System::Object^  sender, System::Windows::Forms::TreeNodeMouseClickEventArgs^  e)
	{
		TreeNode ^seleceNode = this->OBJ_treeView->SelectedNode;
		if( seleceNode )
		{
			string  strname = "";
			int nSelectImageInex = seleceNode->ImageIndex;
			if( nSelectImageInex == 0 )
			{
				String^ clrname = seleceNode->Text;
				MarshalString(clrname, strname);	
				strname = CDataCache::GetNpc_ObjRealName(strname);
				if( lastModelInfo != clrname )
				{
					seleceNode->Nodes->Clear();

					vector<string> agpNames;
					gs_pCahce->GetAnimateNames(CGTRenderMgr::GetInst()->CreateCharacter(strname, eMT_IntObj), agpNames);
					size_t size = agpNames.size();
					for ( size_t i = 0; i < size; ++i )
					{
						string  strAniName = agpNames[i];
						String^ clrAniName = gcnew String(strAniName.c_str());

						TreeNode ^childNode = gcnew TreeNode();
						childNode->Text = clrAniName;
						childNode->ImageIndex = 2;
						childNode->SelectedImageIndex = 3;
						seleceNode->Nodes->Add(childNode);
					}
				}

				seleceNode->Expand();

				if( lastModelInfo != clrname )
				{
					if( this->lastObjSeleceNode )
					{
						this->lastObjSeleceNode->Collapse();
						this->lastObjSeleceNode->BackColor = System::Drawing::SystemColors::Window;
					}

					lastModelInfo = clrname;
					this->lastObjSeleceNode = seleceNode;
				}
			}
			else
			{
				String^ clrname = seleceNode->Text;
				MarshalString(clrname, strname);	

				CGTRenderMgr::GetInst()->DoAnimate(strname);
			}
		}
	}

	System::Void CharacterBrowserForm::PATH_SET_ToolStripMenuItem_Click( System::Object^ sender, System::EventArgs^ e )
	{
		CPathSetDlg dlg;
		dlg.ShowDialog();
	}

	System::Void CharacterBrowserForm::NPC_radioButton_CheckedChanged( System::Object^ sender, System::EventArgs^ e )
	{
		if( this->NPC_radioButton->Checked )
		{
 			this->NPC_treeView->Visible = true;
 			this->OBJ_treeView->Visible = false;
			//MessageBox::Show(NPC_radioButton->Name);
		}
	}

	System::Void CharacterBrowserForm::OBJ_radioButton_CheckedChanged( System::Object^ sender, System::EventArgs^ e )
	{
		if( this->OBJ_radioButton->Checked )
		{
 			this->NPC_treeView->Visible = false;
 			this->OBJ_treeView->Visible = true;
		}
	}

	System::Void CharacterBrowserForm::Search_Button_Click( System::Object^ sender, System::EventArgs^ e )
	{
		System::String^ searchName =  this->Search_textBox->Text->ToString();
		if( searchName == "" )
			return;

		static int nLastCharacterFindIndex = 0;
		static int nLastIntObjFindIndex    = 0;
		int        nCurFindeStartIndex	   = 0;
		TreeView^  selecttreeView;
		bool       bNpcRadioIsCheck = this->NPC_radioButton->Checked;
		if( this->NPC_radioButton->Checked )
		{
			//npc
			selecttreeView = this->NPC_treeView;
			nCurFindeStartIndex = nLastCharacterFindIndex;
		}
		else
		{
			//obj
			selecttreeView = this->OBJ_treeView;
			nCurFindeStartIndex = nLastIntObjFindIndex;
		}

		TreeNode ^node;
		System::String^ treeText;
		int nCount = selecttreeView->Nodes->Count;
		for ( int i = nCurFindeStartIndex; i < nCount; ++i )
		{
			node = selecttreeView->Nodes[i];
			treeText = node->Text;

			if( treeText->Contains(searchName) )
			{
				selecttreeView->SelectedNode = node;
				selecttreeView->SelectedNode->BackColor = System::Drawing::Color::Blue;
				node->Collapse();
				if( bNpcRadioIsCheck )
				{
					//npc
					nLastCharacterFindIndex = i + 1;

				}
				else
				{
					//obj
					nLastIntObjFindIndex = i + 1;
				}

				break;
			}
		}
	}

	System::Void CharacterBrowserForm::PlayType_comboBox_SelectedIndexChanged( System::Object^ sender, System::EventArgs^ e )
	{
		int index = this->PlayType_comboBox->SelectedIndex;
		if( index < 0 )
			return;

		this->UpdateSelchangePlayerType(index);
	}

	//相貌
	System::Void CharacterBrowserForm::Player_Face_comboBox_SelectedIndexChanged( System::Object^ sender, System::EventArgs^ e )
	{
		int nCurSelectFaceIndex = this->Player_Face_comboBox->SelectedIndex;
		if( nCurSelectFaceIndex < 0 )
			return;


		vector<string> hidePieceNames;
		this->UpdatePlayerModel(nCurSelectFaceIndex, ITEM_FACE, hidePieceNames);
	}

	//头发
	System::Void CharacterBrowserForm::Player_Hair_comboBox_SelectedIndexChanged( System::Object^ sender, System::EventArgs^ e )
	{
		int nCurSelectHairIndex = this->Player_Hair_comboBox->SelectedIndex;
		if( nCurSelectHairIndex < 0 )
			return;

		vector<string> hidePieceNames;
		this->UpdatePlayerModel(nCurSelectHairIndex, ITEM_HAIR, hidePieceNames);
	}

	//套装
	System::Void CharacterBrowserForm::Equip_Group_comboBox_SelectedIndexChanged( System::Object^ sender, System::EventArgs^ e )
	{
		int nSelectEquipIndex = this->Equip_Group_comboBox->SelectedIndex;
		if( nSelectEquipIndex < 0 )
			return;

		String^ clrEquipName = this->Equip_Group_comboBox->Text;
		string  strEquipName = "";
		MarshalString(clrEquipName, strEquipName);
		
		UpdateEquipGroup(strEquipName);
	}

	void CharacterBrowserForm::UpdateEquipGroup(const string& strEquipName)
	{
		int nCurPlayTypeIndex = this->PlayType_comboBox->SelectedIndex;
		if( nCurPlayTypeIndex < 0 )
			return;

		static vector<int> vecItemType;
		vecItemType.push_back(ITEM_HEAD);
		vecItemType.push_back(ITEM_SHOULDER);
		vecItemType.push_back(ITEM_BODY);
		vecItemType.push_back(ITEM_MANTLE);
		vecItemType.push_back(ITEM_CUFF);
		vecItemType.push_back(ITEM_BOOT);

		vector<int> pieceItemIndex;
		this->pCharacterAttributes->GetPieceItemIndexByName(nCurPlayTypeIndex, vecItemType, strEquipName, pieceItemIndex);

		this->Player_Head_comboBox->SelectedIndex = pieceItemIndex[0];
		this->Player_Shoulder_comboBox->SelectedIndex = pieceItemIndex[1];
		this->Player_Body_comboBox->SelectedIndex = pieceItemIndex[2];
		this->Player_Mantle_comboBox->SelectedIndex = pieceItemIndex[3];
		this->Player_Cuff_comboBox->SelectedIndex = pieceItemIndex[4];
		this->Player_Boot_comboBox->SelectedIndex = pieceItemIndex[5];

		this->UpdatePlayerAllAttribute();
	}

	//头盔
	System::Void CharacterBrowserForm::Player_Head_comboBox_SelectedIndexChanged( System::Object^ sender, System::EventArgs^ e )
	{
		int nCurSelectHairIndex = this->Player_Hair_comboBox->SelectedIndex;
		if( nCurSelectHairIndex < 0 )
			return;

		vector<string> hidePieceNames;
		this->UpdatePlayerModel(nCurSelectHairIndex, ITEM_HEAD, hidePieceNames);
		this->UpdateAdvComboBox(this->Head_Adv_comboBox, ITEM_HEAD, nCurSelectHairIndex);
	}

	System::Void CharacterBrowserForm::Head_Adv_comboBox_SelectedIndexChanged( System::Object^ sender, System::EventArgs^ e )
	{
		int nCurSelectHairIndex = this->Player_Hair_comboBox->SelectedIndex;
		if( nCurSelectHairIndex < 0 )
			return;

		this->UpdatePlayerEffect(this->Head_Adv_comboBox, ITEM_HEAD, nCurSelectHairIndex);
	}

	//护肩
	System::Void CharacterBrowserForm::Player_Shoulder_comboBox_SelectedIndexChanged( System::Object^ sender, System::EventArgs^ e )
	{
		int nCurSelectShoulderIndex = this->Player_Shoulder_comboBox->SelectedIndex;
		if( nCurSelectShoulderIndex < 0 )
			return;

		vector<string> hidePieceNames;
		this->UpdatePlayerModel(nCurSelectShoulderIndex, ITEM_SHOULDER, hidePieceNames);
	}

	System::Void CharacterBrowserForm::Shoulder_Adv_comboBox_SelectedIndexChanged( System::Object^ sender, System::EventArgs^ e )
	{

	}

	//身体
	System::Void CharacterBrowserForm::Player_Body_comboBox_SelectedIndexChanged( System::Object^ sender, System::EventArgs^ e )
	{
		int nCurSelectBodyIndex = this->Player_Body_comboBox->SelectedIndex;
		if( nCurSelectBodyIndex < 0 )
			return;

		vector<string> hidePieceNames;
		this->UpdatePlayerModel(nCurSelectBodyIndex, ITEM_BODY, hidePieceNames);
	}

	System::Void CharacterBrowserForm::Body_Adv_comboBox_SelectedIndexChanged( System::Object^ sender, System::EventArgs^ e )
	{

	}

	//披风
	System::Void CharacterBrowserForm::Player_Mantle_comboBox_SelectedIndexChanged( System::Object^ sender, System::EventArgs^ e )
	{
		int nCurSelectMantleIndex = this->Player_Mantle_comboBox->SelectedIndex;
		if( nCurSelectMantleIndex < 0 )
			return;

		vector<string> hidePieceNames;
		this->UpdatePlayerModel(nCurSelectMantleIndex, ITEM_MANTLE, hidePieceNames);
	}

	System::Void CharacterBrowserForm::Mantle_Adv_comboBox_SelectedIndexChanged( System::Object^ sender, System::EventArgs^ e )
	{

	}

	//护腕
	System::Void CharacterBrowserForm::Player_Cuff_comboBox_SelectedIndexChanged( System::Object^ sender, System::EventArgs^ e )
	{
		int nCurSelectCuffIndex = this->Player_Cuff_comboBox->SelectedIndex;
		if( nCurSelectCuffIndex < 0 )
			return;

		vector<string> hidePieceNames;
		this->UpdatePlayerModel(nCurSelectCuffIndex, ITEM_CUFF, hidePieceNames);
	}

	System::Void CharacterBrowserForm::Cuff_Adv_comboBox_SelectedIndexChanged( System::Object^ sender, System::EventArgs^ e )
	{

	}

	//靴子
	System::Void CharacterBrowserForm::Player_Boot_comboBox_SelectedIndexChanged( System::Object^ sender, System::EventArgs^ e )
	{
		int nCurBootCuffIndex = this->Player_Boot_comboBox->SelectedIndex;
		if( nCurBootCuffIndex < 0 )
			return;

		vector<string> hidePieceNames;
		this->UpdatePlayerModel(nCurBootCuffIndex, ITEM_BOOT, hidePieceNames);
	}

	System::Void CharacterBrowserForm::Boot_Adv_comboBox_SelectedIndexChanged( System::Object^ sender, System::EventArgs^ e )
	{

	}

	//主手
	System::Void CharacterBrowserForm::Main_Weapon_comboBox_SelectedIndexChanged( System::Object^ sender, System::EventArgs^ e )
	{
		int nCurSelectMainWeaponIndex = this->Player_MainWeapon_comboBox->SelectedIndex;
		if( nCurSelectMainWeaponIndex < 0 )
			return;

		
		this->UpdatePlayerAllAttribute();
	}

	System::Void CharacterBrowserForm::MainWeapon_Adv_comboBox_SelectedIndexChanged( System::Object^ sender, System::EventArgs^ e )
	{

	}

	//副手
	System::Void CharacterBrowserForm::Sub_Weapon_comboBox_SelectedIndexChanged( System::Object^ sender, System::EventArgs^ e )
	{
		int nCurSelectSubWeaponIndex = this->Player_SubWeapon_comboBox->SelectedIndex;
		if( nCurSelectSubWeaponIndex < 0 )
			return;

		
		this->UpdatePlayerAllAttribute();
	}

	System::Void CharacterBrowserForm::SubWeapon_Adv_comboBox_SelectedIndexChanged( System::Object^ sender, System::EventArgs^ e )
	{

	}

	System::Void CharacterBrowserForm::Player_AGP_comboBox_SelectedIndexChanged( System::Object^ sender, System::EventArgs^ e )
	{
		int nCurSelectAgpIndex = this->Player_AGP_comboBox->SelectedIndex;
		if( nCurSelectAgpIndex < 0 )
			return;

		String^ clrAgpName = this->Player_AGP_comboBox->Text;
		string  strAgpName = "";
		MarshalString(clrAgpName, strAgpName);
		string tPath =  DEFAULT_ANI_PATH;
		tPath += strAgpName;
		tPath += ".agp";

		vector<string> aniNames;
		CGTRenderMgr::GetInst()->GetAnimationGroupAniNames(tPath, aniNames);
		if( !aniNames.empty() )
		{
			this->Player_ANI_comboBox->Items->Clear();
			string strAniName = "";
			size_t size = aniNames.size();
			for ( size_t i = 0; i < size; ++i )
			{
				strAniName = aniNames[i];
				String^ clrAniName = gcnew String(strAniName.c_str());
				this->Player_ANI_comboBox->Items->Add(clrAniName);
			}
		}
		
		this->UpdatePlayerAnimationGroup();
		int nAniCount = this->Player_ANI_comboBox->Items->Count;
		if( nAniCount > 0 )
			this->Player_ANI_comboBox->SelectedIndex = 0;
	}

	System::Void CharacterBrowserForm::Player_ANI_comboBox_SelectedIndexChanged( System::Object^ sender, System::EventArgs^ e )
	{
		this->UpdateMainPlayerAni();
	}

	System::Void CharacterBrowserForm::Ani_Speed_trackBar_Scroll( System::Object^ sender, System::EventArgs^ e )
	{
		this->Speed_textBox->Text = this->Ani_Speed_trackBar->Value.ToString();
		this->UpdateMainPlayerAni();
	}

	System::Void CharacterBrowserForm::Speed_textBox_TextChanged( System::Object^ sender, System::EventArgs^ e )
	{
		this->UpdateMainPlayerAni();
	}

	System::Void CharacterBrowserForm::CharacterBrowserForm_SizeChanged( System::Object^ sender, System::EventArgs^ e )
	{
	}

	System::Void CharacterBrowserForm::OnIdle( System::Object^ sender,System::EventArgs^ e )
	{
		int tt = 0;
	}

	System::Void CharacterBrowserForm::tabControl1_SelectedIndexChanged( System::Object^ sender, System::EventArgs^ e )
	{
		int nselectIndex = this->tabControl1->SelectedIndex;
		if( nselectIndex == 1 )
		{
			static bool bFirst = true;

			CGTRenderMgr::GetInst()->DestroyObj();
			CGTRenderMgr::GetInst()->CreateMainPlayer();

			if( bFirst )
			{
				this->InitEquipForm();
				this->UpdatePlayerAllAttribute();
				this->UpdatePlayerAnimationGroup();
				this->UpdateMainPlayerAni();

				bFirst = false;
			}
			else
				CGTRenderMgr::GetInst()->SetMainPlayerIsShow(true);
		}
		else
		{
			lastModelInfo = "";
			if( this->lastNpcSeleceNode )
			{
				this->lastNpcSeleceNode->Collapse();
			}

			CGTRenderMgr::GetInst()->SetMainPlayerIsShow(false);
		}
	}

	void CharacterBrowserForm::InitEquipForm()
	{
		if( pCharacterAttributes == NULL )
			return;

		vector<string> texts;

		//主角类型combobox
		pCharacterAttributes->GetItemNames(texts);
		size_t size = texts.size();
		for ( size_t i = 0; i < size; ++i )
		{
			string  strAniName = texts[i];
			String^ clrAniName = gcnew String(strAniName.c_str());
			this->PlayType_comboBox->Items->Add(clrAniName);
		}


		this->CreateEquipGroup();

		this->PlayType_comboBox->SelectedIndex = 0;
		texts.clear();
	}

	void CharacterBrowserForm::CreateEquipGroup()
	{
		int nCurSelectTypeIndex = this->PlayType_comboBox->SelectedIndex;
		if( nCurSelectTypeIndex < 0 )
			nCurSelectTypeIndex = 0;

		vector<string> texts;

		//主角相貌combobox
		pCharacterAttributes->GetItemListInfos(nCurSelectTypeIndex, ITEM_FACE, texts);
		this->UpdateComboBox(this->Player_Face_comboBox, texts);
		texts.clear();

		//主角头发combobox
		pCharacterAttributes->GetItemListInfos(nCurSelectTypeIndex, ITEM_HAIR, texts);
		this->UpdateComboBox(this->Player_Hair_comboBox, texts);
		texts.clear();

		//主角头盔combobox
		pCharacterAttributes->GetItemListInfos(nCurSelectTypeIndex, ITEM_HEAD, texts);
		this->UpdateComboBox(this->Player_Head_comboBox, texts);
		texts.clear();

		//主角护肩combobox
		pCharacterAttributes->GetItemListInfos(nCurSelectTypeIndex, ITEM_SHOULDER, texts);
		this->UpdateComboBox(this->Player_Shoulder_comboBox, texts);
		texts.clear();


		//主角身体combobox
		pCharacterAttributes->GetItemListInfos(nCurSelectTypeIndex, ITEM_BODY, texts);
		this->UpdateComboBox(this->Player_Body_comboBox, texts);
		texts.clear();


		//主角披风combobox
		pCharacterAttributes->GetItemListInfos(nCurSelectTypeIndex, ITEM_MANTLE, texts);
		this->UpdateComboBox(this->Player_Mantle_comboBox, texts);
		texts.clear();


		//主角护腕combobox
		pCharacterAttributes->GetItemListInfos(nCurSelectTypeIndex, ITEM_CUFF, texts);
		this->UpdateComboBox(this->Player_Cuff_comboBox, texts);
		texts.clear();


		//主角靴子combobox
		pCharacterAttributes->GetItemListInfos(nCurSelectTypeIndex, ITEM_BOOT, texts);
		this->UpdateComboBox(this->Player_Boot_comboBox, texts);
		texts.clear();


		//主角主手combobox
		pCharacterAttributes->GetItemListInfos(nCurSelectTypeIndex, ITEM_MAINWEAPON, texts);
		this->UpdateComboBox(this->Player_MainWeapon_comboBox, texts);
		texts.clear();

		//主角副手combobox
		pCharacterAttributes->GetItemListInfos(nCurSelectTypeIndex, ITEM_SUBWEAPON, texts);
		this->UpdateComboBox(this->Player_SubWeapon_comboBox, texts);
		texts.clear();

		//主角相貌combobox
		pCharacterAttributes->GetEquipGroupNames(nCurSelectTypeIndex, texts);
		this->UpdateComboBox(this->Equip_Group_comboBox, texts);
		texts.clear();
	}

	void CharacterBrowserForm::UpdateComboBox( ComboBox^ cb, const vector<string>& texts )
	{
		size_t size = texts.size();
		for ( size_t i = 0; i < size; ++i )
		{
			string  strAniName = texts[i];
			String^ clrAniName = gcnew String(strAniName.c_str());
			cb->Items->Add(clrAniName);
		}

		cb->SelectedIndex = 0;
	}

	void CharacterBrowserForm::UpdatePlayerEffect(ComboBox^ cb, const int itemIndex, const int pieceItemIndex)
	{
		int nCurSelectTypeIndex = this->PlayType_comboBox->SelectedIndex;
		if( nCurSelectTypeIndex < 0 )
			nCurSelectTypeIndex = 0;

		string effectPath;

		pCharacterAttributes->GetItemEffectInfo(nCurSelectTypeIndex, itemIndex, pieceItemIndex, effectPath);
		String ^clrEffectName = cb->Text;
		string  strEffectName = "";
		MarshalString(clrEffectName, strEffectName);
		if( strEffectName != "无" )
		{
			CGTRenderMgr::GetInst()->AttachPlayerEffect(effectPath, strEffectName);
		}
	}

	void CharacterBrowserForm::UpdateAdvComboBox(ComboBox^ cb, const int itemIndex, const int pieceItemIndex)
	{
		int nCurSelectTypeIndex = this->PlayType_comboBox->SelectedIndex;
		if( nCurSelectTypeIndex < 0 )
			nCurSelectTypeIndex = 0;

		string effectPath;

		cb->Items->Clear();
		cb->Items->Add("无");

		pCharacterAttributes->GetItemEffectInfo(nCurSelectTypeIndex, itemIndex, pieceItemIndex, effectPath);
		vector<string> strEffectList;
		CGTRenderMgr::GetInst()->GetPlayerEffectNames(effectPath, strEffectList);
		size_t size = strEffectList.size();
		for ( size_t i = 0; i < size; ++i )
		{
			string  strEffectName = strEffectList[i];
			String^ clrEffectName = gcnew String(strEffectName.c_str());
			cb->Items->Add(clrEffectName);
		}
		if( size > 0 )
			cb->SelectedIndex = 1;
	}

	System::Void CharacterBrowserForm::Search_textBox_TextChanged( System::Object^ sender, System::EventArgs^ e )
	{
	}

	void CharacterBrowserForm::UpdateSelchangePlayerType( const int nPlayerType )
	{
		//清空动作组合框
		this->Player_AGP_comboBox->Items->Clear();
		this->Player_ANI_comboBox->Items->Clear();
		//更新动作包
		static const uint NumPart1 = 3;
		static const uint NumPart2 = 3;

		static string Part1[NumPart1] = { "_js","_qs", "_fs" };
		static string Part2[NumPart2] = { "_ss","_ds", "_ks" };

		string strTypePath = "";
		this->pCharacterAttributes->GetLoadInfTypePath(nPlayerType, strTypePath);

 		for( uint i = 0; i < NumPart1; ++i )
 		{
  			for( uint j = 0; j < NumPart2; ++j )
  			{
  				string tAniName = strTypePath + Part1[i] + Part2[j];
  				string tGroupPath =  DEFAULT_ANI_PATH + tAniName + ".ske";
  				//CAnimationsRes* pAniRes = NULL;
  				string sAgpfile = tGroupPath.substr(0,tGroupPath.size()-4)+".agp";
  
 				vector<string> aniNames;
 				if( CGTRenderMgr::GetInst()->GetAnimationGroupAniNames(sAgpfile, aniNames) )
  				{
  					String^ clrAniName = gcnew String(tAniName.c_str());
 					this->Player_AGP_comboBox->Items->Add(clrAniName);
  				}
  			}
 		}

		this->UpdatePlayerAllAttribute();
		this->UpdatePlayerAnimationGroup();

		int nAgpCount = this->Player_AGP_comboBox->Items->Count;
		if( nAgpCount > 0 )
			this->Player_AGP_comboBox->SelectedIndex = 0;

		int nAniCount = this->Player_ANI_comboBox->Items->Count;
		if( nAniCount > 0 )
			this->Player_ANI_comboBox->SelectedIndex = 0;
	}

	//更新player的各个装备
	void CharacterBrowserForm::UpdateMainPlayer()
	{
		int nCurSelectTypeIndex = this->PlayType_comboBox->SelectedIndex;
		if( nCurSelectTypeIndex < 0 )
			return;
	}

	//更新所有属性，和特效
	void CharacterBrowserForm::UpdatePlayerAllAttribute()
	{
		vector<string> hidePieceNames;

		CGTRenderMgr::GetInst()->ReleaseMainPlayer();
		
		//更新装备
		int nCurSelectHeadIndex = this->Player_Head_comboBox->SelectedIndex;
		this->UpdatePlayerModel(nCurSelectHeadIndex, ITEM_HEAD, hidePieceNames);

		int nCurSelectFaceIndex = this->Player_Face_comboBox->SelectedIndex;
		this->UpdatePlayerModel(nCurSelectFaceIndex, ITEM_FACE, hidePieceNames);
		
		int nCurSelectHairIndex = this->Player_Hair_comboBox->SelectedIndex;
		this->UpdatePlayerModel(nCurSelectHairIndex, ITEM_HAIR, hidePieceNames);
		
		int nCurSelectShoulderIndex = this->Player_Shoulder_comboBox->SelectedIndex;
		this->UpdatePlayerModel(nCurSelectShoulderIndex, ITEM_SHOULDER, hidePieceNames);
		
		int nCurSelectBodyIndex = this->Player_Body_comboBox->SelectedIndex;
		this->UpdatePlayerModel(nCurSelectBodyIndex, ITEM_BODY, hidePieceNames);

		int nCurSelectMantleIndex = this->Player_Mantle_comboBox->SelectedIndex;
		this->UpdatePlayerModel(nCurSelectMantleIndex, ITEM_MANTLE, hidePieceNames);
		
		int nCurSelectCuffIndex = this->Player_Cuff_comboBox->SelectedIndex;
		this->UpdatePlayerModel(nCurSelectCuffIndex, ITEM_CUFF, hidePieceNames);
		
		int nCurSelectBootIndex = this->Player_Boot_comboBox->SelectedIndex;
		this->UpdatePlayerModel(nCurSelectBootIndex, ITEM_BOOT, hidePieceNames);
		
		int nCurSelectMainWeaponIndex = this->Player_MainWeapon_comboBox->SelectedIndex;
		this->UpdatePlayerModel(nCurSelectMainWeaponIndex, ITEM_MAINWEAPON, hidePieceNames);
		
		int nCurSelectSubWeaponIndex = this->Player_SubWeapon_comboBox->SelectedIndex;
		this->UpdatePlayerModel(nCurSelectSubWeaponIndex, ITEM_SUBWEAPON, hidePieceNames);

		CGTRenderMgr::GetInst()->HideMainPlayerPiece(hidePieceNames);
	}

	//根据选中的相貌等属性，修改piece
	void CharacterBrowserForm::UpdatePlayerModel( const int nSelectItemIndex, const int ItemTypeIndex, vector<string>& hidePieceNames )
	{
		int nCurPlayTypeIndex = this->PlayType_comboBox->SelectedIndex;
		if( nCurPlayTypeIndex < 0 )
			return;
		if( ItemTypeIndex < 0 )
			return;

		CCharacterAttributes::PieceItem Item = pCharacterAttributes->GetPieceItem(nCurPlayTypeIndex, ItemTypeIndex, nSelectItemIndex);
		CGTRenderMgr::GetInst()->MainPlayerAddPiece(Item.m_PiecePath, Item.m_pPartInfo->GetPieceName(), Item.m_pPartInfo->GetRenderStyle());

		gPlayerEquipInfos[ItemTypeIndex] = Item.m_PiecePath;

		string HideString;
		switch(nCurPlayTypeIndex)
		{
		case eRLM:	//人类男
			HideString = Item.m_pPartInfo->GetHumanMaleHidePiece();
			break;
		case eRLW:	//人类女
			HideString = Item.m_pPartInfo->GetHumanFeMaleHidePiece();
			break;
		case eSRM:	//兽人男
			HideString = Item.m_pPartInfo->GetOrcHidePiece();
			break;
		case eSRW:	//兽人女
			break;
		case eJLM:	//精灵男
			HideString = Item.m_pPartInfo->GetElfHidePiece();
			break;
		case eJLW:	//精灵女
			break;
		case eARM:	//矮人男
			HideString = Item.m_pPartInfo->GetDwarfHidePiece();
			break;
		case eARW:	//矮人女
			break;
		case eOTHER://其他
			break;
		default:
			break;
		}

		size_t off = 0;
		if( HideString.size() > 0 )
		{
			while( off != size_t(-1) )
			{
				size_t tOff;
				tOff = HideString.find(",",off);
				string temp;
				if( tOff == size_t(-1) )
					temp = HideString.substr(off ,HideString.size()-off);
				else
				{
					temp = HideString.substr(off,tOff-off);
					++tOff;
				}
				if( temp.size() > 0 )
					hidePieceNames.push_back(temp);
				off = tOff;
			}
		}

		this->UpdatePlayerEquipInfo();
	}

	void CharacterBrowserForm::UpdatePlayerAnimationGroup()
	{
		int nCurSelectAgpIndex = this->Player_AGP_comboBox->SelectedIndex;
		if( nCurSelectAgpIndex < 0 )
			return;

		String^ clrAgpName = this->Player_AGP_comboBox->Text;
		string  strAgpName = "";
		MarshalString(clrAgpName, strAgpName);

		string tPath =  DEFAULT_ANI_PATH;
		tPath += strAgpName;
		tPath += ".ske";
		CGTRenderMgr::GetInst()->UpdatePlayerAnimationGroup(tPath);
	}


	void CharacterBrowserForm::UpdateMainPlayerAni()
	{
		int nCurSelectAniIndex = this->Player_ANI_comboBox->SelectedIndex;
		if( nCurSelectAniIndex < 0 )
			return;

		String^ clrAniName = this->Player_ANI_comboBox->Text;
		string  strAniName = "";
		MarshalString(clrAniName, strAniName);

		int   nSpeed = Convert::ToInt16(this->Speed_textBox->Text);;
		float fSpeed = nSpeed * 0.25f;
		CGTRenderMgr::GetInst()->SetMainPlayerAni(strAniName, fSpeed);
	}


	void CharacterBrowserForm::UpdatePlayerEquipInfo()
	{
		string headName		= "头盔:" + gPlayerEquipInfos[ITEM_HEAD];
		string faceName		= "相貌:" + gPlayerEquipInfos[ITEM_FACE];
		string hairName		= "头发:" + gPlayerEquipInfos[ITEM_HAIR];
		string shoulderName = "护肩:" + gPlayerEquipInfos[ITEM_SHOULDER];
		string bodyName		= "身体:" + gPlayerEquipInfos[ITEM_BODY];
		string mantleName	= "披风:" + gPlayerEquipInfos[ITEM_MANTLE];
		string cuffName		= "护腕:" + gPlayerEquipInfos[ITEM_CUFF];
		string bootName		= "靴子:" + gPlayerEquipInfos[ITEM_BOOT];
		string weaponName	= "主手:" + gPlayerEquipInfos[ITEM_MAINWEAPON];
		string sWeaponName	= "副手:" + gPlayerEquipInfos[ITEM_SUBWEAPON];
		string strResult	= headName + "\r\n" + faceName + "\r\n" + hairName + "\r\n"+ shoulderName + "\r\n"
			+ bodyName + "\r\n" + mantleName + "\r\n" + cuffName + "\r\n" + bootName + "\r\n" + weaponName + "\r\n"
			+ sWeaponName;

		String ^clrResult   = gcnew String(strResult.c_str());
		this->Equip_Info_textBox->Text = clrResult;
	}
}
