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
	/// Summary for CSceneStateCtrl
	/// </summary>
	public ref class CSceneStateCtrl : public CUIBase//System::Windows::Forms::UserControl
	{
	public:
		CSceneStateCtrl(void)
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
		~CSceneStateCtrl()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Button^  bt_pnum;
	private: System::Windows::Forms::Button^  bt_fnum;
	private: System::Windows::Forms::Button^  bt_texnum;
	private: System::Windows::Forms::Button^  bt_dpnum;
	private: System::Windows::Forms::Label^  l_pnum;
	private: System::Windows::Forms::Label^  l_fnum;
	private: System::Windows::Forms::Label^  l_texnum;
	private: System::Windows::Forms::Label^  l_dpnum;
	private: System::Windows::Forms::GroupBox^  pCount;
	private: System::Windows::Forms::Label^  data_dpnum;
	private: System::Windows::Forms::Label^  data_texnum;
	private: System::Windows::Forms::Label^  data_fnum;
	private: System::Windows::Forms::Label^  data_pum;
	private: System::Windows::Forms::GroupBox^  pState;
	private: System::Windows::Forms::Button^  bt_white;
	private: System::Windows::Forms::Button^  bt_red;
	private: System::Windows::Forms::Button^  bt_yellow;
	private: System::Windows::Forms::Button^  bt_green;
	private: System::Windows::Forms::Button^  bt_blue;
	private: System::Windows::Forms::Button^  bt_black;
	private: System::Windows::Forms::Label^  label6;
	private: System::Windows::Forms::Label^  label5;
	private: System::Windows::Forms::Label^  label4;
	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::RadioButton^  radioFull;
	private: System::Windows::Forms::RadioButton^  radioAlpha;
	private: System::Windows::Forms::CheckBox^  pCountSwitch;
	private: System::Windows::Forms::CheckBox^  pStateSwitch;

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
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(CSceneStateCtrl::typeid));
			this->bt_pnum = (gcnew System::Windows::Forms::Button());
			this->bt_fnum = (gcnew System::Windows::Forms::Button());
			this->bt_texnum = (gcnew System::Windows::Forms::Button());
			this->bt_dpnum = (gcnew System::Windows::Forms::Button());
			this->l_pnum = (gcnew System::Windows::Forms::Label());
			this->l_fnum = (gcnew System::Windows::Forms::Label());
			this->l_texnum = (gcnew System::Windows::Forms::Label());
			this->l_dpnum = (gcnew System::Windows::Forms::Label());
			this->pCount = (gcnew System::Windows::Forms::GroupBox());
			this->pCountSwitch = (gcnew System::Windows::Forms::CheckBox());
			this->data_dpnum = (gcnew System::Windows::Forms::Label());
			this->data_texnum = (gcnew System::Windows::Forms::Label());
			this->data_fnum = (gcnew System::Windows::Forms::Label());
			this->data_pum = (gcnew System::Windows::Forms::Label());
			this->pState = (gcnew System::Windows::Forms::GroupBox());
			this->pStateSwitch = (gcnew System::Windows::Forms::CheckBox());
			this->radioFull = (gcnew System::Windows::Forms::RadioButton());
			this->radioAlpha = (gcnew System::Windows::Forms::RadioButton());
			this->label6 = (gcnew System::Windows::Forms::Label());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->bt_white = (gcnew System::Windows::Forms::Button());
			this->bt_red = (gcnew System::Windows::Forms::Button());
			this->bt_yellow = (gcnew System::Windows::Forms::Button());
			this->bt_green = (gcnew System::Windows::Forms::Button());
			this->bt_blue = (gcnew System::Windows::Forms::Button());
			this->bt_black = (gcnew System::Windows::Forms::Button());
			this->pCount->SuspendLayout();
			this->pState->SuspendLayout();
			this->SuspendLayout();
			// 
			// bt_pnum
			// 
			this->bt_pnum->BackgroundImage = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"bt_pnum.BackgroundImage")));
			this->bt_pnum->BackgroundImageLayout = System::Windows::Forms::ImageLayout::Zoom;
			this->bt_pnum->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->bt_pnum->Location = System::Drawing::Point(10, 17);
			this->bt_pnum->Name = L"bt_pnum";
			this->bt_pnum->Size = System::Drawing::Size(16, 16);
			this->bt_pnum->TabIndex = 0;
			this->bt_pnum->UseVisualStyleBackColor = true;
			// 
			// bt_fnum
			// 
			this->bt_fnum->BackgroundImage = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"bt_fnum.BackgroundImage")));
			this->bt_fnum->BackgroundImageLayout = System::Windows::Forms::ImageLayout::Zoom;
			this->bt_fnum->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->bt_fnum->Location = System::Drawing::Point(10, 39);
			this->bt_fnum->Name = L"bt_fnum";
			this->bt_fnum->Size = System::Drawing::Size(16, 16);
			this->bt_fnum->TabIndex = 0;
			this->bt_fnum->UseVisualStyleBackColor = true;
			// 
			// bt_texnum
			// 
			this->bt_texnum->BackgroundImage = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"bt_texnum.BackgroundImage")));
			this->bt_texnum->BackgroundImageLayout = System::Windows::Forms::ImageLayout::Zoom;
			this->bt_texnum->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->bt_texnum->Location = System::Drawing::Point(10, 61);
			this->bt_texnum->Name = L"bt_texnum";
			this->bt_texnum->Size = System::Drawing::Size(16, 16);
			this->bt_texnum->TabIndex = 0;
			this->bt_texnum->UseVisualStyleBackColor = true;
			// 
			// bt_dpnum
			// 
			this->bt_dpnum->BackgroundImage = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"bt_dpnum.BackgroundImage")));
			this->bt_dpnum->BackgroundImageLayout = System::Windows::Forms::ImageLayout::Zoom;
			this->bt_dpnum->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->bt_dpnum->Location = System::Drawing::Point(10, 83);
			this->bt_dpnum->Name = L"bt_dpnum";
			this->bt_dpnum->Size = System::Drawing::Size(16, 16);
			this->bt_dpnum->TabIndex = 0;
			this->bt_dpnum->UseVisualStyleBackColor = true;
			// 
			// l_pnum
			// 
			this->l_pnum->AutoSize = true;
			this->l_pnum->Location = System::Drawing::Point(35, 20);
			this->l_pnum->Name = L"l_pnum";
			this->l_pnum->Size = System::Drawing::Size(41, 12);
			this->l_pnum->TabIndex = 1;
			this->l_pnum->Text = L"点  数";
			// 
			// l_fnum
			// 
			this->l_fnum->AutoSize = true;
			this->l_fnum->Location = System::Drawing::Point(35, 42);
			this->l_fnum->Name = L"l_fnum";
			this->l_fnum->Size = System::Drawing::Size(41, 12);
			this->l_fnum->TabIndex = 1;
			this->l_fnum->Text = L"面  数";
			// 
			// l_texnum
			// 
			this->l_texnum->AutoSize = true;
			this->l_texnum->Location = System::Drawing::Point(35, 64);
			this->l_texnum->Name = L"l_texnum";
			this->l_texnum->Size = System::Drawing::Size(41, 12);
			this->l_texnum->TabIndex = 1;
			this->l_texnum->Text = L"纹理数";
			// 
			// l_dpnum
			// 
			this->l_dpnum->AutoSize = true;
			this->l_dpnum->Location = System::Drawing::Point(35, 86);
			this->l_dpnum->Name = L"l_dpnum";
			this->l_dpnum->Size = System::Drawing::Size(41, 12);
			this->l_dpnum->TabIndex = 1;
			this->l_dpnum->Text = L"DP  数";
			// 
			// pCount
			// 
			this->pCount->Controls->Add(this->pCountSwitch);
			this->pCount->Controls->Add(this->data_dpnum);
			this->pCount->Controls->Add(this->data_texnum);
			this->pCount->Controls->Add(this->data_fnum);
			this->pCount->Controls->Add(this->data_pum);
			this->pCount->Controls->Add(this->l_texnum);
			this->pCount->Controls->Add(this->l_dpnum);
			this->pCount->Controls->Add(this->bt_pnum);
			this->pCount->Controls->Add(this->bt_fnum);
			this->pCount->Controls->Add(this->l_fnum);
			this->pCount->Controls->Add(this->bt_texnum);
			this->pCount->Controls->Add(this->l_pnum);
			this->pCount->Controls->Add(this->bt_dpnum);
			this->pCount->Location = System::Drawing::Point(7, 4);
			this->pCount->Name = L"pCount";
			this->pCount->Size = System::Drawing::Size(161, 134);
			this->pCount->TabIndex = 2;
			this->pCount->TabStop = false;
			this->pCount->Text = L"场景统计";
			// 
			// pCountSwitch
			// 
			this->pCountSwitch->AutoSize = true;
			this->pCountSwitch->Location = System::Drawing::Point(86, 113);
			this->pCountSwitch->Name = L"pCountSwitch";
			this->pCountSwitch->Size = System::Drawing::Size(72, 16);
			this->pCountSwitch->TabIndex = 4;
			this->pCountSwitch->Text = L"是否开启";
			this->pCountSwitch->UseVisualStyleBackColor = true;
			this->pCountSwitch->CheckedChanged += gcnew System::EventHandler(this, &CSceneStateCtrl::CountSwitch_CheckedChanged);
			// 
			// data_dpnum
			// 
			this->data_dpnum->AutoSize = true;
			this->data_dpnum->Location = System::Drawing::Point(87, 85);
			this->data_dpnum->Name = L"data_dpnum";
			this->data_dpnum->Size = System::Drawing::Size(53, 12);
			this->data_dpnum->TabIndex = 3;
			this->data_dpnum->Text = L"00000000";
			// 
			// data_texnum
			// 
			this->data_texnum->AutoSize = true;
			this->data_texnum->Location = System::Drawing::Point(87, 63);
			this->data_texnum->Name = L"data_texnum";
			this->data_texnum->Size = System::Drawing::Size(53, 12);
			this->data_texnum->TabIndex = 3;
			this->data_texnum->Text = L"00000000";
			// 
			// data_fnum
			// 
			this->data_fnum->AutoSize = true;
			this->data_fnum->Location = System::Drawing::Point(87, 41);
			this->data_fnum->Name = L"data_fnum";
			this->data_fnum->Size = System::Drawing::Size(53, 12);
			this->data_fnum->TabIndex = 3;
			this->data_fnum->Text = L"00000000";
			// 
			// data_pum
			// 
			this->data_pum->AutoSize = true;
			this->data_pum->Location = System::Drawing::Point(87, 19);
			this->data_pum->Name = L"data_pum";
			this->data_pum->Size = System::Drawing::Size(53, 12);
			this->data_pum->TabIndex = 3;
			this->data_pum->Text = L"00000000";
			// 
			// pState
			// 
			this->pState->Controls->Add(this->pStateSwitch);
			this->pState->Controls->Add(this->radioFull);
			this->pState->Controls->Add(this->radioAlpha);
			this->pState->Controls->Add(this->label6);
			this->pState->Controls->Add(this->label5);
			this->pState->Controls->Add(this->label4);
			this->pState->Controls->Add(this->label3);
			this->pState->Controls->Add(this->label2);
			this->pState->Controls->Add(this->label1);
			this->pState->Controls->Add(this->bt_white);
			this->pState->Controls->Add(this->bt_red);
			this->pState->Controls->Add(this->bt_yellow);
			this->pState->Controls->Add(this->bt_green);
			this->pState->Controls->Add(this->bt_blue);
			this->pState->Controls->Add(this->bt_black);
			this->pState->Location = System::Drawing::Point(7, 144);
			this->pState->Name = L"pState";
			this->pState->Size = System::Drawing::Size(161, 181);
			this->pState->TabIndex = 3;
			this->pState->TabStop = false;
			this->pState->Text = L"场景状态";
			// 
			// pStateSwitch
			// 
			this->pStateSwitch->AutoSize = true;
			this->pStateSwitch->Location = System::Drawing::Point(83, 159);
			this->pStateSwitch->Name = L"pStateSwitch";
			this->pStateSwitch->Size = System::Drawing::Size(72, 16);
			this->pStateSwitch->TabIndex = 4;
			this->pStateSwitch->Text = L"是否开启";
			this->pStateSwitch->UseVisualStyleBackColor = true;
			this->pStateSwitch->CheckedChanged += gcnew System::EventHandler(this, &CSceneStateCtrl::StateSwitch_CheckedChanged);
			// 
			// radioFull
			// 
			this->radioFull->AutoSize = true;
			this->radioFull->Location = System::Drawing::Point(105, 108);
			this->radioFull->Name = L"radioFull";
			this->radioFull->Size = System::Drawing::Size(47, 16);
			this->radioFull->TabIndex = 1;
			this->radioFull->TabStop = true;
			this->radioFull->Text = L"全色";
			this->radioFull->UseVisualStyleBackColor = true;
			this->radioFull->CheckedChanged += gcnew System::EventHandler(this, &CSceneStateCtrl::radioFull_CheckedChanged);
			// 
			// radioAlpha
			// 
			this->radioAlpha->AutoSize = true;
			this->radioAlpha->Location = System::Drawing::Point(105, 130);
			this->radioAlpha->Name = L"radioAlpha";
			this->radioAlpha->Size = System::Drawing::Size(47, 16);
			this->radioAlpha->TabIndex = 0;
			this->radioAlpha->TabStop = true;
			this->radioAlpha->Text = L"透明";
			this->radioAlpha->UseVisualStyleBackColor = true;
			this->radioAlpha->CheckedChanged += gcnew System::EventHandler(this, &CSceneStateCtrl::radioAlpha_CheckedChanged);
			// 
			// label6
			// 
			this->label6->AutoSize = true;
			this->label6->Location = System::Drawing::Point(40, 133);
			this->label6->Name = L"label6";
			this->label6->Size = System::Drawing::Size(53, 12);
			this->label6->TabIndex = 3;
			this->label6->Text = L"00000000";
			// 
			// label5
			// 
			this->label5->AutoSize = true;
			this->label5->Location = System::Drawing::Point(40, 111);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(53, 12);
			this->label5->TabIndex = 3;
			this->label5->Text = L"00000000";
			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->Location = System::Drawing::Point(40, 89);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(53, 12);
			this->label4->TabIndex = 3;
			this->label4->Text = L"00000000";
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Location = System::Drawing::Point(40, 67);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(53, 12);
			this->label3->TabIndex = 3;
			this->label3->Text = L"00000000";
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(40, 45);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(53, 12);
			this->label2->TabIndex = 3;
			this->label2->Text = L"00000000";
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(40, 23);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(53, 12);
			this->label1->TabIndex = 3;
			this->label1->Text = L"00000000";
			// 
			// bt_white
			// 
			this->bt_white->BackColor = System::Drawing::Color::White;
			this->bt_white->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->bt_white->Location = System::Drawing::Point(10, 131);
			this->bt_white->Name = L"bt_white";
			this->bt_white->Size = System::Drawing::Size(16, 16);
			this->bt_white->TabIndex = 0;
			this->bt_white->UseVisualStyleBackColor = false;
			// 
			// bt_red
			// 
			this->bt_red->BackColor = System::Drawing::Color::Red;
			this->bt_red->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->bt_red->Location = System::Drawing::Point(10, 109);
			this->bt_red->Name = L"bt_red";
			this->bt_red->Size = System::Drawing::Size(16, 16);
			this->bt_red->TabIndex = 0;
			this->bt_red->UseVisualStyleBackColor = false;
			// 
			// bt_yellow
			// 
			this->bt_yellow->BackColor = System::Drawing::Color::Yellow;
			this->bt_yellow->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->bt_yellow->Location = System::Drawing::Point(10, 87);
			this->bt_yellow->Name = L"bt_yellow";
			this->bt_yellow->Size = System::Drawing::Size(16, 16);
			this->bt_yellow->TabIndex = 0;
			this->bt_yellow->UseVisualStyleBackColor = false;
			// 
			// bt_green
			// 
			this->bt_green->BackColor = System::Drawing::Color::Lime;
			this->bt_green->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->bt_green->Location = System::Drawing::Point(10, 65);
			this->bt_green->Name = L"bt_green";
			this->bt_green->Size = System::Drawing::Size(16, 16);
			this->bt_green->TabIndex = 0;
			this->bt_green->UseVisualStyleBackColor = false;
			// 
			// bt_blue
			// 
			this->bt_blue->BackColor = System::Drawing::SystemColors::ActiveCaption;
			this->bt_blue->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->bt_blue->Location = System::Drawing::Point(10, 43);
			this->bt_blue->Name = L"bt_blue";
			this->bt_blue->Size = System::Drawing::Size(16, 16);
			this->bt_blue->TabIndex = 0;
			this->bt_blue->UseVisualStyleBackColor = false;
			// 
			// bt_black
			// 
			this->bt_black->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(64)), static_cast<System::Int32>(static_cast<System::Byte>(0)), 
				static_cast<System::Int32>(static_cast<System::Byte>(64)));
			this->bt_black->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->bt_black->Location = System::Drawing::Point(10, 21);
			this->bt_black->Name = L"bt_black";
			this->bt_black->Size = System::Drawing::Size(16, 16);
			this->bt_black->TabIndex = 0;
			this->bt_black->UseVisualStyleBackColor = false;
			// 
			// CSceneStateCtrl
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->Controls->Add(this->pState);
			this->Controls->Add(this->pCount);
			this->Name = L"CSceneStateCtrl";
			this->Size = System::Drawing::Size(182, 334);
			this->pCount->ResumeLayout(false);
			this->pCount->PerformLayout();
			this->pState->ResumeLayout(false);
			this->pState->PerformLayout();
			this->ResumeLayout(false);

		}
#pragma endregion
	private: System::Void StateSwitch_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	private: System::Void CountSwitch_CheckedChanged(System::Object^  sender, System::EventArgs^  e) ;
	private: System::Void radioFull_CheckedChanged(System::Object^  sender, System::EventArgs^  e) ;
	private: System::Void radioAlpha_CheckedChanged(System::Object^  sender, System::EventArgs^  e) ;
};
}
