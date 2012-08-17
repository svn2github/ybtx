#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

#include "CUIBase.h"

namespace sqr_tools  {

	/// <summary>
	/// Summary for CGroundSpeedCtrl
	/// </summary>
	public ref class CGroundSpeedCtrl : public CUIBase
	{
	public:
		CGroundSpeedCtrl(void)
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
		~CGroundSpeedCtrl()
		{
			if (components)
			{
				delete components;
			}
		}

	public:  virtual void Update(void) override;
	public:	 virtual void*	GetImpParam(void)		override;
	public:  virtual char*	GetImpTypeName(void)	override;

	private: System::Windows::Forms::GroupBox^  groupBox1;
	protected: 
	private: System::Windows::Forms::Button^  set_button;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::TextBox^  groundSpeed_textBox;
	private: System::Windows::Forms::Label^  label1;

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
			this->set_button = (gcnew System::Windows::Forms::Button());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->groundSpeed_textBox = (gcnew System::Windows::Forms::TextBox());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->groupBox1->SuspendLayout();
			this->SuspendLayout();
			// 
			// groupBox1
			// 
			this->groupBox1->Controls->Add(this->set_button);
			this->groupBox1->Controls->Add(this->label2);
			this->groupBox1->Controls->Add(this->groundSpeed_textBox);
			this->groupBox1->Controls->Add(this->label1);
			this->groupBox1->Dock = System::Windows::Forms::DockStyle::Fill;
			this->groupBox1->Location = System::Drawing::Point(0, 0);
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(244, 56);
			this->groupBox1->TabIndex = 0;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = L"地面速度";
			// 
			// set_button
			// 
			this->set_button->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->set_button->Location = System::Drawing::Point(188, 20);
			this->set_button->Name = L"set_button";
			this->set_button->Size = System::Drawing::Size(39, 24);
			this->set_button->TabIndex = 3;
			this->set_button->Text = L"设置";
			this->set_button->UseVisualStyleBackColor = true;
			this->set_button->Click += gcnew System::EventHandler(this, &CGroundSpeedCtrl::set_button_Click);
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(147, 25);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(35, 12);
			this->label2->TabIndex = 2;
			this->label2->Text = L"米/秒";
			// 
			// groundSpeed_textBox
			// 
			this->groundSpeed_textBox->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->groundSpeed_textBox->Location = System::Drawing::Point(88, 21);
			this->groundSpeed_textBox->Name = L"groundSpeed_textBox";
			this->groundSpeed_textBox->Size = System::Drawing::Size(56, 21);
			this->groundSpeed_textBox->TabIndex = 1;
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(8, 24);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(77, 12);
			this->label1->TabIndex = 0;
			this->label1->Text = L"地面移动速度";
			// 
			// CGroundSpeedCtrl
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->Controls->Add(this->groupBox1);
			this->Name = L"CGroundSpeedCtrl";
			this->Size = System::Drawing::Size(244, 56);
			this->groupBox1->ResumeLayout(false);
			this->groupBox1->PerformLayout();
			this->ResumeLayout(false);

		}
#pragma endregion
	private: System::Void set_button_Click(System::Object^  sender, System::EventArgs^  e);
};
}
