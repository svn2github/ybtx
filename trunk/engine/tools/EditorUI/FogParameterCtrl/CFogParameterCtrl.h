#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


#include "CUIBase.h"
#include "ColorCtrl/CColorCtrl.h"

namespace sqr_tools  {

	/// <summary>
	/// Summary for CFogSettingCtrl
	/// </summary>
	public ref class CFogSettingCtrl : public CUIBase//System::Windows::Forms::UserControl
	{
	public:
		CFogSettingCtrl(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
			this->dwColor = 0;
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~CFogSettingCtrl()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::GroupBox^  foggroupBox;
	protected: 
	private: System::Windows::Forms::TextBox^  end_textBox;
	private: System::Windows::Forms::TextBox^  start_textBox;
	private: System::Windows::Forms::TextBox^  desnity_textBox;
	private: System::Windows::Forms::TrackBar^  end_trackBar;
	private: System::Windows::Forms::TrackBar^  start_trackBar;
	private: System::Windows::Forms::TrackBar^  desnity_trackBar;
	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::RadioButton^  exp_radioButton;
	private: System::Windows::Forms::RadioButton^  liner_radioButton;
	private: System::Windows::Forms::ColorDialog^  fog_colorDialog;
	private: System::Windows::Forms::Button^  setColor_button;
	private: sqr_tools::CColorCtrl^  cFogColorCtrl;
	private: System::Windows::Forms::CheckBox^  enableFog_checkBox;


	public:	 virtual char* GetImpTypeName(void) override;
	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;
		DWORD								dwColor;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->foggroupBox = (gcnew System::Windows::Forms::GroupBox());
			this->enableFog_checkBox = (gcnew System::Windows::Forms::CheckBox());
			this->setColor_button = (gcnew System::Windows::Forms::Button());
			this->cFogColorCtrl = (gcnew sqr_tools::CColorCtrl());
			this->end_textBox = (gcnew System::Windows::Forms::TextBox());
			this->start_textBox = (gcnew System::Windows::Forms::TextBox());
			this->desnity_textBox = (gcnew System::Windows::Forms::TextBox());
			this->end_trackBar = (gcnew System::Windows::Forms::TrackBar());
			this->start_trackBar = (gcnew System::Windows::Forms::TrackBar());
			this->desnity_trackBar = (gcnew System::Windows::Forms::TrackBar());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->exp_radioButton = (gcnew System::Windows::Forms::RadioButton());
			this->liner_radioButton = (gcnew System::Windows::Forms::RadioButton());
			this->fog_colorDialog = (gcnew System::Windows::Forms::ColorDialog());
			this->foggroupBox->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->end_trackBar))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->start_trackBar))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->desnity_trackBar))->BeginInit();
			this->SuspendLayout();
			// 
			// foggroupBox
			// 
			this->foggroupBox->Controls->Add(this->enableFog_checkBox);
			this->foggroupBox->Controls->Add(this->setColor_button);
			this->foggroupBox->Controls->Add(this->cFogColorCtrl);
			this->foggroupBox->Controls->Add(this->end_textBox);
			this->foggroupBox->Controls->Add(this->start_textBox);
			this->foggroupBox->Controls->Add(this->desnity_textBox);
			this->foggroupBox->Controls->Add(this->end_trackBar);
			this->foggroupBox->Controls->Add(this->start_trackBar);
			this->foggroupBox->Controls->Add(this->desnity_trackBar);
			this->foggroupBox->Controls->Add(this->label3);
			this->foggroupBox->Controls->Add(this->label2);
			this->foggroupBox->Controls->Add(this->label1);
			this->foggroupBox->Controls->Add(this->exp_radioButton);
			this->foggroupBox->Controls->Add(this->liner_radioButton);
			this->foggroupBox->Location = System::Drawing::Point(6, 6);
			this->foggroupBox->Name = L"foggroupBox";
			this->foggroupBox->Size = System::Drawing::Size(255, 187);
			this->foggroupBox->TabIndex = 1;
			this->foggroupBox->TabStop = false;
			this->foggroupBox->Text = L"Fog Setup";
			// 
			// enableFog_checkBox
			// 
			this->enableFog_checkBox->AutoSize = true;
			this->enableFog_checkBox->Checked = true;
			this->enableFog_checkBox->CheckState = System::Windows::Forms::CheckState::Checked;
			this->enableFog_checkBox->Location = System::Drawing::Point(187, 25);
			this->enableFog_checkBox->Name = L"enableFog_checkBox";
			this->enableFog_checkBox->Size = System::Drawing::Size(60, 16);
			this->enableFog_checkBox->TabIndex = 13;
			this->enableFog_checkBox->Text = L"开启雾";
			this->enableFog_checkBox->UseVisualStyleBackColor = true;
			this->enableFog_checkBox->CheckedChanged += gcnew System::EventHandler(this, &CFogSettingCtrl::enableFog_checkBox_CheckedChanged);
			// 
			// setColor_button
			// 
			this->setColor_button->Location = System::Drawing::Point(199, 143);
			this->setColor_button->Name = L"setColor_button";
			this->setColor_button->Size = System::Drawing::Size(50, 42);
			this->setColor_button->TabIndex = 12;
			this->setColor_button->Text = L"设置雾颜色";
			this->setColor_button->UseVisualStyleBackColor = true;
			this->setColor_button->Click += gcnew System::EventHandler(this, &CFogSettingCtrl::setColor_button_Click);
			// 
			// cFogColorCtrl
			// 
			this->cFogColorCtrl->Location = System::Drawing::Point(4, 148);
			this->cFogColorCtrl->Name = L"cFogColorCtrl";
			this->cFogColorCtrl->Size = System::Drawing::Size(194, 31);
			this->cFogColorCtrl->TabIndex = 11;
			// 
			// end_textBox
			// 
			this->end_textBox->Location = System::Drawing::Point(199, 119);
			this->end_textBox->Name = L"end_textBox";
			this->end_textBox->Size = System::Drawing::Size(48, 21);
			this->end_textBox->TabIndex = 10;
			this->end_textBox->TextChanged += gcnew System::EventHandler(this, &CFogSettingCtrl::end_textBox_TextChanged);
			// 
			// start_textBox
			// 
			this->start_textBox->Location = System::Drawing::Point(199, 85);
			this->start_textBox->Name = L"start_textBox";
			this->start_textBox->Size = System::Drawing::Size(48, 21);
			this->start_textBox->TabIndex = 9;
			this->start_textBox->TextChanged += gcnew System::EventHandler(this, &CFogSettingCtrl::start_textBox_TextChanged);
			// 
			// desnity_textBox
			// 
			this->desnity_textBox->Location = System::Drawing::Point(199, 49);
			this->desnity_textBox->Name = L"desnity_textBox";
			this->desnity_textBox->Size = System::Drawing::Size(48, 21);
			this->desnity_textBox->TabIndex = 8;
			this->desnity_textBox->TextChanged += gcnew System::EventHandler(this, &CFogSettingCtrl::desnity_textBox_TextChanged);
			// 
			// end_trackBar
			// 
			this->end_trackBar->Location = System::Drawing::Point(36, 116);
			this->end_trackBar->Name = L"end_trackBar";
			this->end_trackBar->Size = System::Drawing::Size(166, 45);
			this->end_trackBar->TabIndex = 7;
			this->end_trackBar->Scroll += gcnew System::EventHandler(this, &CFogSettingCtrl::end_trackBar_Scroll);
			// 
			// start_trackBar
			// 
			this->start_trackBar->Location = System::Drawing::Point(36, 82);
			this->start_trackBar->Name = L"start_trackBar";
			this->start_trackBar->Size = System::Drawing::Size(166, 45);
			this->start_trackBar->TabIndex = 6;
			this->start_trackBar->Scroll += gcnew System::EventHandler(this, &CFogSettingCtrl::start_trackBar_Scroll);
			// 
			// desnity_trackBar
			// 
			this->desnity_trackBar->Location = System::Drawing::Point(36, 50);
			this->desnity_trackBar->Name = L"desnity_trackBar";
			this->desnity_trackBar->Size = System::Drawing::Size(166, 45);
			this->desnity_trackBar->TabIndex = 5;
			this->desnity_trackBar->Scroll += gcnew System::EventHandler(this, &CFogSettingCtrl::desnity_trackBar_Scroll);
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Location = System::Drawing::Point(8, 119);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(29, 12);
			this->label3->TabIndex = 4;
			this->label3->Text = L"终止";
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(8, 85);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(29, 12);
			this->label2->TabIndex = 3;
			this->label2->Text = L"起始";
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(8, 55);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(29, 12);
			this->label1->TabIndex = 2;
			this->label1->Text = L"浓度";
			// 
			// exp_radioButton
			// 
			this->exp_radioButton->AutoSize = true;
			this->exp_radioButton->Location = System::Drawing::Point(98, 25);
			this->exp_radioButton->Name = L"exp_radioButton";
			this->exp_radioButton->Size = System::Drawing::Size(83, 16);
			this->exp_radioButton->TabIndex = 1;
			this->exp_radioButton->Text = L"指数顶点雾";
			this->exp_radioButton->UseVisualStyleBackColor = true;
			this->exp_radioButton->CheckedChanged += gcnew System::EventHandler(this, &CFogSettingCtrl::exp_radioButton_CheckedChanged);
			// 
			// liner_radioButton
			// 
			this->liner_radioButton->AutoSize = true;
			this->liner_radioButton->Checked = true;
			this->liner_radioButton->Location = System::Drawing::Point(10, 25);
			this->liner_radioButton->Name = L"liner_radioButton";
			this->liner_radioButton->Size = System::Drawing::Size(83, 16);
			this->liner_radioButton->TabIndex = 0;
			this->liner_radioButton->TabStop = true;
			this->liner_radioButton->Text = L"线性顶点雾";
			this->liner_radioButton->UseVisualStyleBackColor = true;
			this->liner_radioButton->CheckedChanged += gcnew System::EventHandler(this, &CFogSettingCtrl::liner_radioButton_CheckedChanged);
			// 
			// CFogSettingCtrl
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->Controls->Add(this->foggroupBox);
			this->Name = L"CFogSettingCtrl";
			this->Size = System::Drawing::Size(267, 196);
			this->Load += gcnew System::EventHandler(this, &CFogSettingCtrl::CFogSettingCtrl_Load);
			this->foggroupBox->ResumeLayout(false);
			this->foggroupBox->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->end_trackBar))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->start_trackBar))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->desnity_trackBar))->EndInit();
			this->ResumeLayout(false);

		}
#pragma endregion

		public:  virtual void Update(void) override;
		private: System::Void liner_radioButton_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
		private: System::Void exp_radioButton_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
		private: System::Void desnity_trackBar_Scroll(System::Object^  sender, System::EventArgs^  e);
		private: System::Void start_trackBar_Scroll(System::Object^  sender, System::EventArgs^  e);
		private: System::Void end_trackBar_Scroll(System::Object^  sender, System::EventArgs^  e);
		private: System::Void desnity_textBox_TextChanged(System::Object^  sender, System::EventArgs^  e);
		private: System::Void start_textBox_TextChanged(System::Object^  sender, System::EventArgs^  e);
		private: System::Void end_textBox_TextChanged(System::Object^  sender, System::EventArgs^  e);
		private: System::Void setColor_button_Click(System::Object^  sender, System::EventArgs^  e);
		private: System::Void enableFog_checkBox_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
		
		private: void InitFogParaCtrl();
		private: void UpdateFogParaCtrl();
		private: void CollectMsg(int nId);

		private: System::Void CFogSettingCtrl_Load(System::Object^  sender, System::EventArgs^  e);
};
}
