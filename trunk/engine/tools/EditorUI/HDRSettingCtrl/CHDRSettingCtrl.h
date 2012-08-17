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
	/// Summary for CHDRSettingCtrl
	/// </summary>
	public ref class CHDRSettingCtrl : public CUIBase
	{
	public:
		CHDRSettingCtrl(void)
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
		~CHDRSettingCtrl()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::CheckBox^  checkBox1;
	protected: 
	private: System::Windows::Forms::CheckBox^  checkBox2;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::TrackBar^  HighPass_trackBar;
	private: System::Windows::Forms::TextBox^  HighPass_textBox;
	private: System::Windows::Forms::TextBox^  Mul_textBox;
	private: System::Windows::Forms::TrackBar^  Mul_trackBar;
	private: System::Windows::Forms::TextBox^  FMean_textBox;
	private: System::Windows::Forms::TrackBar^  FMean_trackBar;
	private: System::Windows::Forms::TextBox^  SAM_textBox;

	private: System::Windows::Forms::TrackBar^  SAM_trackBar;

	private: System::Windows::Forms::TextBox^  Exp_textBox;

	private: System::Windows::Forms::TrackBar^  Exp_trackBar;

	private: System::Windows::Forms::TextBox^  StdDev_textBox;


	private: System::Windows::Forms::TrackBar^  StdDev_trackBar;

	private: System::Windows::Forms::Label^  label4;
	private: System::Windows::Forms::Label^  label5;
	private: System::Windows::Forms::Label^  label6;

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
			this->checkBox1 = (gcnew System::Windows::Forms::CheckBox());
			this->checkBox2 = (gcnew System::Windows::Forms::CheckBox());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->HighPass_trackBar = (gcnew System::Windows::Forms::TrackBar());
			this->HighPass_textBox = (gcnew System::Windows::Forms::TextBox());
			this->Mul_textBox = (gcnew System::Windows::Forms::TextBox());
			this->Mul_trackBar = (gcnew System::Windows::Forms::TrackBar());
			this->FMean_textBox = (gcnew System::Windows::Forms::TextBox());
			this->FMean_trackBar = (gcnew System::Windows::Forms::TrackBar());
			this->SAM_textBox = (gcnew System::Windows::Forms::TextBox());
			this->SAM_trackBar = (gcnew System::Windows::Forms::TrackBar());
			this->Exp_textBox = (gcnew System::Windows::Forms::TextBox());
			this->Exp_trackBar = (gcnew System::Windows::Forms::TrackBar());
			this->StdDev_textBox = (gcnew System::Windows::Forms::TextBox());
			this->StdDev_trackBar = (gcnew System::Windows::Forms::TrackBar());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->label6 = (gcnew System::Windows::Forms::Label());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->HighPass_trackBar))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->Mul_trackBar))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->FMean_trackBar))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->SAM_trackBar))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->Exp_trackBar))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->StdDev_trackBar))->BeginInit();
			this->SuspendLayout();
			// 
			// checkBox1
			// 
			this->checkBox1->AutoSize = true;
			this->checkBox1->Location = System::Drawing::Point(8, 12);
			this->checkBox1->Name = L"checkBox1";
			this->checkBox1->Size = System::Drawing::Size(72, 16);
			this->checkBox1->TabIndex = 0;
			this->checkBox1->Text = L"全屏泛光";
			this->checkBox1->UseVisualStyleBackColor = true;
			this->checkBox1->CheckedChanged += gcnew System::EventHandler(this, &CHDRSettingCtrl::checkBox1_CheckedChanged);
			// 
			// checkBox2
			// 
			this->checkBox2->AutoSize = true;
			this->checkBox2->Location = System::Drawing::Point(136, 12);
			this->checkBox2->Name = L"checkBox2";
			this->checkBox2->Size = System::Drawing::Size(72, 16);
			this->checkBox2->TabIndex = 1;
			this->checkBox2->Text = L"空气折射";
			this->checkBox2->UseVisualStyleBackColor = true;
			this->checkBox2->CheckedChanged += gcnew System::EventHandler(this, &CHDRSettingCtrl::checkBox2_CheckedChanged);
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(6, 39);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(41, 12);
			this->label1->TabIndex = 2;
			this->label1->Text = L"光通量";
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(6, 69);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(41, 12);
			this->label2->TabIndex = 3;
			this->label2->Text = L"光强度";
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Location = System::Drawing::Point(6, 99);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(41, 12);
			this->label3->TabIndex = 4;
			this->label3->Text = L"光偏移";
			// 
			// HighPass_trackBar
			// 
			this->HighPass_trackBar->Location = System::Drawing::Point(44, 36);
			this->HighPass_trackBar->Name = L"HighPass_trackBar";
			this->HighPass_trackBar->Size = System::Drawing::Size(146, 45);
			this->HighPass_trackBar->TabIndex = 5;
			this->HighPass_trackBar->Scroll += gcnew System::EventHandler(this, &CHDRSettingCtrl::HighPass_trackBar_Scroll);
			// 
			// HighPass_textBox
			// 
			this->HighPass_textBox->Location = System::Drawing::Point(192, 37);
			this->HighPass_textBox->Name = L"HighPass_textBox";
			this->HighPass_textBox->Size = System::Drawing::Size(49, 21);
			this->HighPass_textBox->TabIndex = 6;
			this->HighPass_textBox->TextChanged += gcnew System::EventHandler(this, &CHDRSettingCtrl::HighPass_textBox_TextChanged);
			// 
			// Mul_textBox
			// 
			this->Mul_textBox->Location = System::Drawing::Point(192, 65);
			this->Mul_textBox->Name = L"Mul_textBox";
			this->Mul_textBox->Size = System::Drawing::Size(49, 21);
			this->Mul_textBox->TabIndex = 8;
			this->Mul_textBox->TextChanged += gcnew System::EventHandler(this, &CHDRSettingCtrl::Mul_textBox_TextChanged);
			// 
			// Mul_trackBar
			// 
			this->Mul_trackBar->Location = System::Drawing::Point(44, 64);
			this->Mul_trackBar->Name = L"Mul_trackBar";
			this->Mul_trackBar->Size = System::Drawing::Size(146, 45);
			this->Mul_trackBar->TabIndex = 7;
			this->Mul_trackBar->Scroll += gcnew System::EventHandler(this, &CHDRSettingCtrl::Mul_trackBar_Scroll);
			// 
			// FMean_textBox
			// 
			this->FMean_textBox->Location = System::Drawing::Point(192, 96);
			this->FMean_textBox->Name = L"FMean_textBox";
			this->FMean_textBox->Size = System::Drawing::Size(49, 21);
			this->FMean_textBox->TabIndex = 10;
			this->FMean_textBox->TextChanged += gcnew System::EventHandler(this, &CHDRSettingCtrl::FMean_textBox_TextChanged);
			// 
			// FMean_trackBar
			// 
			this->FMean_trackBar->Location = System::Drawing::Point(44, 93);
			this->FMean_trackBar->Name = L"FMean_trackBar";
			this->FMean_trackBar->Size = System::Drawing::Size(146, 45);
			this->FMean_trackBar->TabIndex = 9;
			this->FMean_trackBar->Scroll += gcnew System::EventHandler(this, &CHDRSettingCtrl::FMean_trackBar_Scroll);
			// 
			// SAM_textBox
			// 
			this->SAM_textBox->Location = System::Drawing::Point(192, 186);
			this->SAM_textBox->Name = L"SAM_textBox";
			this->SAM_textBox->Size = System::Drawing::Size(49, 21);
			this->SAM_textBox->TabIndex = 19;
			this->SAM_textBox->TextChanged += gcnew System::EventHandler(this, &CHDRSettingCtrl::SAM_textBox_TextChanged);
			// 
			// SAM_trackBar
			// 
			this->SAM_trackBar->Location = System::Drawing::Point(44, 185);
			this->SAM_trackBar->Name = L"SAM_trackBar";
			this->SAM_trackBar->Size = System::Drawing::Size(146, 45);
			this->SAM_trackBar->TabIndex = 18;
			this->SAM_trackBar->Scroll += gcnew System::EventHandler(this, &CHDRSettingCtrl::SAM_trackBar_Scroll);
			// 
			// Exp_textBox
			// 
			this->Exp_textBox->Location = System::Drawing::Point(192, 155);
			this->Exp_textBox->Name = L"Exp_textBox";
			this->Exp_textBox->Size = System::Drawing::Size(49, 21);
			this->Exp_textBox->TabIndex = 17;
			this->Exp_textBox->TextChanged += gcnew System::EventHandler(this, &CHDRSettingCtrl::Exp_textBox_TextChanged);
			// 
			// Exp_trackBar
			// 
			this->Exp_trackBar->Location = System::Drawing::Point(44, 154);
			this->Exp_trackBar->Name = L"Exp_trackBar";
			this->Exp_trackBar->Size = System::Drawing::Size(146, 45);
			this->Exp_trackBar->TabIndex = 16;
			this->Exp_trackBar->Scroll += gcnew System::EventHandler(this, &CHDRSettingCtrl::Exp_trackBar_Scroll);
			// 
			// StdDev_textBox
			// 
			this->StdDev_textBox->Location = System::Drawing::Point(192, 127);
			this->StdDev_textBox->Name = L"StdDev_textBox";
			this->StdDev_textBox->Size = System::Drawing::Size(49, 21);
			this->StdDev_textBox->TabIndex = 15;
			this->StdDev_textBox->TextChanged += gcnew System::EventHandler(this, &CHDRSettingCtrl::StdDev_textBox_TextChanged);
			// 
			// StdDev_trackBar
			// 
			this->StdDev_trackBar->Location = System::Drawing::Point(44, 126);
			this->StdDev_trackBar->Name = L"StdDev_trackBar";
			this->StdDev_trackBar->Size = System::Drawing::Size(146, 45);
			this->StdDev_trackBar->TabIndex = 14;
			this->StdDev_trackBar->Scroll += gcnew System::EventHandler(this, &CHDRSettingCtrl::StdDev_trackBar_Scroll);
			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->Location = System::Drawing::Point(6, 189);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(41, 12);
			this->label4->TabIndex = 13;
			this->label4->Text = L"模糊比";
			// 
			// label5
			// 
			this->label5->AutoSize = true;
			this->label5->Location = System::Drawing::Point(6, 159);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(41, 12);
			this->label5->TabIndex = 12;
			this->label5->Text = L"白平衡";
			// 
			// label6
			// 
			this->label6->AutoSize = true;
			this->label6->Location = System::Drawing::Point(6, 129);
			this->label6->Name = L"label6";
			this->label6->Size = System::Drawing::Size(41, 12);
			this->label6->TabIndex = 11;
			this->label6->Text = L"融合度";
			// 
			// CHDRSettingCtrl
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->Controls->Add(this->SAM_textBox);
			this->Controls->Add(this->SAM_trackBar);
			this->Controls->Add(this->Exp_textBox);
			this->Controls->Add(this->Exp_trackBar);
			this->Controls->Add(this->StdDev_textBox);
			this->Controls->Add(this->StdDev_trackBar);
			this->Controls->Add(this->label4);
			this->Controls->Add(this->label5);
			this->Controls->Add(this->label6);
			this->Controls->Add(this->FMean_textBox);
			this->Controls->Add(this->FMean_trackBar);
			this->Controls->Add(this->Mul_textBox);
			this->Controls->Add(this->Mul_trackBar);
			this->Controls->Add(this->HighPass_textBox);
			this->Controls->Add(this->HighPass_trackBar);
			this->Controls->Add(this->label3);
			this->Controls->Add(this->label2);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->checkBox2);
			this->Controls->Add(this->checkBox1);
			this->Name = L"CHDRSettingCtrl";
			this->Size = System::Drawing::Size(245, 226);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->HighPass_trackBar))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->Mul_trackBar))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->FMean_trackBar))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->SAM_trackBar))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->Exp_trackBar))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->StdDev_trackBar))->EndInit();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion

		private: void InitHDRParaCtrl();
		private: void CollectMsg();
		public:  virtual void Update(void) override;
		public:  virtual char*	GetImpTypeName(void) override;
		private: System::Void checkBox1_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
		private: System::Void checkBox2_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
		private: System::Void HighPass_trackBar_Scroll(System::Object^  sender, System::EventArgs^  e);
		private: System::Void HighPass_textBox_TextChanged(System::Object^  sender, System::EventArgs^  e);
		private: System::Void Mul_trackBar_Scroll(System::Object^  sender, System::EventArgs^  e);
		private: System::Void Mul_textBox_TextChanged(System::Object^  sender, System::EventArgs^  e);
		private: System::Void FMean_trackBar_Scroll(System::Object^  sender, System::EventArgs^  e);
		private: System::Void FMean_textBox_TextChanged(System::Object^  sender, System::EventArgs^  e);
		private: System::Void StdDev_trackBar_Scroll(System::Object^  sender, System::EventArgs^  e);
		private: System::Void StdDev_textBox_TextChanged(System::Object^  sender, System::EventArgs^  e);
		private: System::Void Exp_trackBar_Scroll(System::Object^  sender, System::EventArgs^  e);
		private: System::Void Exp_textBox_TextChanged(System::Object^  sender, System::EventArgs^  e);
		private: System::Void SAM_trackBar_Scroll(System::Object^  sender, System::EventArgs^  e);
		private: System::Void SAM_textBox_TextChanged(System::Object^  sender, System::EventArgs^  e);
};
}
