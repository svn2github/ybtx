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
	/// Summary for AmbientAudioCtrl
	/// </summary>
	public ref class AmbientAudioCtrl : CUIBase
	{
	public:
		AmbientAudioCtrl(void)
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
		~AmbientAudioCtrl()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::GroupBox^  groupBox1;
	private: System::Windows::Forms::TextBox^  randomIntervaltime_textBox;


	private: System::Windows::Forms::Label^  label6;
	private: System::Windows::Forms::TextBox^  randomPlaytime_textBox;


	private: System::Windows::Forms::Label^  label5;
	private: System::Windows::Forms::TextBox^  intervaltimeMax_textBox;

	private: System::Windows::Forms::Label^  label4;
	private: System::Windows::Forms::TextBox^  intervaltimeMin_textBox;

	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::TextBox^  playtimeMax_textBox;

	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::TextBox^  playtimeMin_textBox;
	private: System::Windows::Forms::Label^  label1;





	private: System::Windows::Forms::GroupBox^  groupBox3;
	private: System::Windows::Forms::Button^  changePro_button;

	private: System::Windows::Forms::ComboBox^  pro_comboBox;
	private: System::Windows::Forms::Label^  label7;
	private: System::Windows::Forms::Button^  clearPro_button;
	private: System::Windows::Forms::Button^  changeRange_button;

	private: System::Windows::Forms::ComboBox^  range_comboBox;
	private: System::Windows::Forms::Label^  label8;

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
			this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
			this->randomIntervaltime_textBox = (gcnew System::Windows::Forms::TextBox());
			this->label6 = (gcnew System::Windows::Forms::Label());
			this->randomPlaytime_textBox = (gcnew System::Windows::Forms::TextBox());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->intervaltimeMax_textBox = (gcnew System::Windows::Forms::TextBox());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->intervaltimeMin_textBox = (gcnew System::Windows::Forms::TextBox());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->playtimeMax_textBox = (gcnew System::Windows::Forms::TextBox());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->playtimeMin_textBox = (gcnew System::Windows::Forms::TextBox());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->groupBox3 = (gcnew System::Windows::Forms::GroupBox());
			this->changeRange_button = (gcnew System::Windows::Forms::Button());
			this->range_comboBox = (gcnew System::Windows::Forms::ComboBox());
			this->label8 = (gcnew System::Windows::Forms::Label());
			this->changePro_button = (gcnew System::Windows::Forms::Button());
			this->pro_comboBox = (gcnew System::Windows::Forms::ComboBox());
			this->label7 = (gcnew System::Windows::Forms::Label());
			this->clearPro_button = (gcnew System::Windows::Forms::Button());
			this->groupBox1->SuspendLayout();
			this->groupBox3->SuspendLayout();
			this->SuspendLayout();
			// 
			// groupBox1
			// 
			this->groupBox1->Controls->Add(this->randomIntervaltime_textBox);
			this->groupBox1->Controls->Add(this->label6);
			this->groupBox1->Controls->Add(this->randomPlaytime_textBox);
			this->groupBox1->Controls->Add(this->label5);
			this->groupBox1->Controls->Add(this->intervaltimeMax_textBox);
			this->groupBox1->Controls->Add(this->label4);
			this->groupBox1->Controls->Add(this->intervaltimeMin_textBox);
			this->groupBox1->Controls->Add(this->label3);
			this->groupBox1->Controls->Add(this->playtimeMax_textBox);
			this->groupBox1->Controls->Add(this->label2);
			this->groupBox1->Controls->Add(this->playtimeMin_textBox);
			this->groupBox1->Controls->Add(this->label1);
			this->groupBox1->Location = System::Drawing::Point(4, 8);
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(229, 171);
			this->groupBox1->TabIndex = 0;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = L"播放时间设置";
			// 
			// randomIntervaltime_textBox
			// 
			this->randomIntervaltime_textBox->Enabled = false;
			this->randomIntervaltime_textBox->Location = System::Drawing::Point(136, 145);
			this->randomIntervaltime_textBox->Name = L"randomIntervaltime_textBox";
			this->randomIntervaltime_textBox->Size = System::Drawing::Size(87, 21);
			this->randomIntervaltime_textBox->TabIndex = 11;
			// 
			// label6
			// 
			this->label6->AutoSize = true;
			this->label6->Location = System::Drawing::Point(6, 148);
			this->label6->Name = L"label6";
			this->label6->Size = System::Drawing::Size(125, 12);
			this->label6->TabIndex = 10;
			this->label6->Text = L"当前播放间隔长度(ms)";
			// 
			// randomPlaytime_textBox
			// 
			this->randomPlaytime_textBox->Enabled = false;
			this->randomPlaytime_textBox->Location = System::Drawing::Point(136, 121);
			this->randomPlaytime_textBox->Name = L"randomPlaytime_textBox";
			this->randomPlaytime_textBox->Size = System::Drawing::Size(87, 21);
			this->randomPlaytime_textBox->TabIndex = 9;
			// 
			// label5
			// 
			this->label5->AutoSize = true;
			this->label5->Location = System::Drawing::Point(6, 124);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(125, 12);
			this->label5->TabIndex = 8;
			this->label5->Text = L"当前播放随机长度(ms)";
			// 
			// intervaltimeMax_textBox
			// 
			this->intervaltimeMax_textBox->Location = System::Drawing::Point(136, 97);
			this->intervaltimeMax_textBox->Name = L"intervaltimeMax_textBox";
			this->intervaltimeMax_textBox->Size = System::Drawing::Size(87, 21);
			this->intervaltimeMax_textBox->TabIndex = 7;
			this->intervaltimeMax_textBox->TextChanged += gcnew System::EventHandler(this, &AmbientAudioCtrl::intervaltimeMax_textBox_TextChanged);
			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->Location = System::Drawing::Point(6, 100);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(125, 12);
			this->label4->TabIndex = 6;
			this->label4->Text = L"播放间隔最大长度(ms)";
			// 
			// intervaltimeMin_textBox
			// 
			this->intervaltimeMin_textBox->Location = System::Drawing::Point(136, 73);
			this->intervaltimeMin_textBox->Name = L"intervaltimeMin_textBox";
			this->intervaltimeMin_textBox->Size = System::Drawing::Size(87, 21);
			this->intervaltimeMin_textBox->TabIndex = 5;
			this->intervaltimeMin_textBox->TextChanged += gcnew System::EventHandler(this, &AmbientAudioCtrl::intervaltimeMin_textBox_TextChanged);
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Location = System::Drawing::Point(6, 76);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(125, 12);
			this->label3->TabIndex = 4;
			this->label3->Text = L"播放间隔最小长度(ms)";
			// 
			// playtimeMax_textBox
			// 
			this->playtimeMax_textBox->Location = System::Drawing::Point(136, 49);
			this->playtimeMax_textBox->Name = L"playtimeMax_textBox";
			this->playtimeMax_textBox->Size = System::Drawing::Size(87, 21);
			this->playtimeMax_textBox->TabIndex = 3;
			this->playtimeMax_textBox->TextChanged += gcnew System::EventHandler(this, &AmbientAudioCtrl::playtimeMax_textBox_TextChanged);
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(6, 52);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(125, 12);
			this->label2->TabIndex = 2;
			this->label2->Text = L"播放时间最大长度(ms)";
			// 
			// playtimeMin_textBox
			// 
			this->playtimeMin_textBox->Location = System::Drawing::Point(136, 25);
			this->playtimeMin_textBox->Name = L"playtimeMin_textBox";
			this->playtimeMin_textBox->Size = System::Drawing::Size(87, 21);
			this->playtimeMin_textBox->TabIndex = 1;
			this->playtimeMin_textBox->TextChanged += gcnew System::EventHandler(this, &AmbientAudioCtrl::playtimeMin_textBox_TextChanged);
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(6, 28);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(125, 12);
			this->label1->TabIndex = 0;
			this->label1->Text = L"播放时间最小长度(ms)";
			// 
			// groupBox3
			// 
			this->groupBox3->Controls->Add(this->changeRange_button);
			this->groupBox3->Controls->Add(this->range_comboBox);
			this->groupBox3->Controls->Add(this->label8);
			this->groupBox3->Controls->Add(this->changePro_button);
			this->groupBox3->Controls->Add(this->pro_comboBox);
			this->groupBox3->Controls->Add(this->label7);
			this->groupBox3->Controls->Add(this->clearPro_button);
			this->groupBox3->Location = System::Drawing::Point(3, 185);
			this->groupBox3->Name = L"groupBox3";
			this->groupBox3->Size = System::Drawing::Size(229, 83);
			this->groupBox3->TabIndex = 2;
			this->groupBox3->TabStop = false;
			this->groupBox3->Text = L"时间配置属性";
			// 
			// changeRange_button
			// 
			this->changeRange_button->Location = System::Drawing::Point(179, 50);
			this->changeRange_button->Name = L"changeRange_button";
			this->changeRange_button->Size = System::Drawing::Size(46, 27);
			this->changeRange_button->TabIndex = 6;
			this->changeRange_button->Text = L"替换";
			this->changeRange_button->UseVisualStyleBackColor = true;
			this->changeRange_button->Click += gcnew System::EventHandler(this, &AmbientAudioCtrl::changeRange_button_Click);
			// 
			// range_comboBox
			// 
			this->range_comboBox->FormattingEnabled = true;
			this->range_comboBox->Location = System::Drawing::Point(132, 53);
			this->range_comboBox->Name = L"range_comboBox";
			this->range_comboBox->Size = System::Drawing::Size(44, 20);
			this->range_comboBox->TabIndex = 5;
			this->range_comboBox->SelectedIndexChanged += gcnew System::EventHandler(this, &AmbientAudioCtrl::range_comboBox_SelectedIndexChanged);
			// 
			// label8
			// 
			this->label8->AutoSize = true;
			this->label8->Location = System::Drawing::Point(7, 57);
			this->label8->Name = L"label8";
			this->label8->Size = System::Drawing::Size(125, 12);
			this->label8->TabIndex = 4;
			this->label8->Text = L"是否播放范围属性选择";
			// 
			// changePro_button
			// 
			this->changePro_button->Location = System::Drawing::Point(179, 20);
			this->changePro_button->Name = L"changePro_button";
			this->changePro_button->Size = System::Drawing::Size(46, 27);
			this->changePro_button->TabIndex = 3;
			this->changePro_button->Text = L"替换";
			this->changePro_button->UseVisualStyleBackColor = true;
			this->changePro_button->Click += gcnew System::EventHandler(this, &AmbientAudioCtrl::changePro_button_Click);
			// 
			// pro_comboBox
			// 
			this->pro_comboBox->FormattingEnabled = true;
			this->pro_comboBox->Location = System::Drawing::Point(131, 24);
			this->pro_comboBox->Name = L"pro_comboBox";
			this->pro_comboBox->Size = System::Drawing::Size(44, 20);
			this->pro_comboBox->TabIndex = 2;
			this->pro_comboBox->SelectedIndexChanged += gcnew System::EventHandler(this, &AmbientAudioCtrl::pro_comboBox_SelectedIndexChanged);
			// 
			// label7
			// 
			this->label7->AutoSize = true;
			this->label7->Location = System::Drawing::Point(71, 27);
			this->label7->Name = L"label7";
			this->label7->Size = System::Drawing::Size(53, 12);
			this->label7->TabIndex = 1;
			this->label7->Text = L"配置属性";
			// 
			// clearPro_button
			// 
			this->clearPro_button->Location = System::Drawing::Point(6, 20);
			this->clearPro_button->Name = L"clearPro_button";
			this->clearPro_button->Size = System::Drawing::Size(62, 27);
			this->clearPro_button->TabIndex = 0;
			this->clearPro_button->Text = L"清空属性";
			this->clearPro_button->UseVisualStyleBackColor = true;
			this->clearPro_button->Click += gcnew System::EventHandler(this, &AmbientAudioCtrl::clearPro_button_Click);
			// 
			// AmbientAudioCtrl
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->Controls->Add(this->groupBox3);
			this->Controls->Add(this->groupBox1);
			this->Name = L"AmbientAudioCtrl";
			this->Size = System::Drawing::Size(237, 271);
			this->Load += gcnew System::EventHandler(this, &AmbientAudioCtrl::AmbientAudioCtrl_Load);
			this->groupBox1->ResumeLayout(false);
			this->groupBox1->PerformLayout();
			this->groupBox3->ResumeLayout(false);
			this->groupBox3->PerformLayout();
			this->ResumeLayout(false);

		}
#pragma endregion
		private: System::Void playtimeMin_textBox_TextChanged(System::Object^  sender, System::EventArgs^  e);
		private: System::Void playtimeMax_textBox_TextChanged(System::Object^  sender, System::EventArgs^  e);
		private: System::Void intervaltimeMin_textBox_TextChanged(System::Object^  sender, System::EventArgs^  e);
		private: System::Void intervaltimeMax_textBox_TextChanged(System::Object^  sender, System::EventArgs^  e);
		private: System::Void clearPro_button_Click(System::Object^  sender, System::EventArgs^  e);
		private: System::Void pro_comboBox_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e);
		private: System::Void changePro_button_Click(System::Object^  sender, System::EventArgs^  e);
		private: System::Void range_comboBox_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e);
		private: System::Void changeRange_button_Click(System::Object^  sender, System::EventArgs^  e);

		public:  virtual void   Update(void) override;
		public:  virtual char*	GetImpTypeName(void)	override;
		public:  virtual void*	GetImpParam(void)		override;
	    
		private: System::Void AmbientAudioCtrl_Load(System::Object^  sender, System::EventArgs^  e);
		private: void InitCueProerty();
};
}
