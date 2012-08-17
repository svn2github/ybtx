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
	/// Summary for CKeyFrameCtrl
	/// </summary>
	public ref class CKeyFrameCtrl : public CUIBase
	{
	public:
		CKeyFrameCtrl(void)
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
		~CKeyFrameCtrl()
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
	private: System::Windows::Forms::Button^  setFrameString_button;
	protected: 

	private: System::Windows::Forms::TextBox^  frameString_textBox;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::Button^  SaveKeyInfoBtn;

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
			this->SaveKeyInfoBtn = (gcnew System::Windows::Forms::Button());
			this->setFrameString_button = (gcnew System::Windows::Forms::Button());
			this->frameString_textBox = (gcnew System::Windows::Forms::TextBox());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->groupBox1->SuspendLayout();
			this->SuspendLayout();
			// 
			// groupBox1
			// 
			this->groupBox1->Controls->Add(this->SaveKeyInfoBtn);
			this->groupBox1->Controls->Add(this->setFrameString_button);
			this->groupBox1->Controls->Add(this->frameString_textBox);
			this->groupBox1->Controls->Add(this->label1);
			this->groupBox1->Dock = System::Windows::Forms::DockStyle::Fill;
			this->groupBox1->Location = System::Drawing::Point(0, 0);
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(419, 56);
			this->groupBox1->TabIndex = 0;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = L"¶¯»­¹Ø¼üÖ¡";
			// 
			// SaveKeyInfoBtn
			// 
			this->SaveKeyInfoBtn->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->SaveKeyInfoBtn->Location = System::Drawing::Point(253, 18);
			this->SaveKeyInfoBtn->Name = L"SaveKeyInfoBtn";
			this->SaveKeyInfoBtn->Size = System::Drawing::Size(150, 24);
			this->SaveKeyInfoBtn->TabIndex = 3;
			this->SaveKeyInfoBtn->Text = L"±£´æ¹Ø¼üÖ¡(*.fky)ÐÅÏ¢";
			this->SaveKeyInfoBtn->UseVisualStyleBackColor = true;
			this->SaveKeyInfoBtn->Click += gcnew System::EventHandler(this, &CKeyFrameCtrl::SaveKeyInfoBtn_Click);
			// 
			// setFrameString_button
			// 
			this->setFrameString_button->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->setFrameString_button->Location = System::Drawing::Point(169, 18);
			this->setFrameString_button->Name = L"setFrameString_button";
			this->setFrameString_button->Size = System::Drawing::Size(78, 24);
			this->setFrameString_button->TabIndex = 2;
			this->setFrameString_button->Text = L"ÉèÖÃ×Ö·û´®";
			this->setFrameString_button->UseVisualStyleBackColor = true;
			this->setFrameString_button->Click += gcnew System::EventHandler(this, &CKeyFrameCtrl::setFrameString_button_Click);
			// 
			// frameString_textBox
			// 
			this->frameString_textBox->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->frameString_textBox->Location = System::Drawing::Point(82, 20);
			this->frameString_textBox->Name = L"frameString_textBox";
			this->frameString_textBox->Size = System::Drawing::Size(81, 21);
			this->frameString_textBox->TabIndex = 1;
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(11, 23);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(65, 12);
			this->label1->TabIndex = 0;
			this->label1->Text = L"ÌØÕ÷×Ö·û´®";
			// 
			// CKeyFrameCtrl
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->Controls->Add(this->groupBox1);
			this->Name = L"CKeyFrameCtrl";
			this->Size = System::Drawing::Size(419, 56);
			this->groupBox1->ResumeLayout(false);
			this->groupBox1->PerformLayout();
			this->ResumeLayout(false);

		}
#pragma endregion
			private: System::Void setFrameString_button_Click(System::Object^  sender, System::EventArgs^  e);
			private: System::Void SaveKeyInfoBtn_Click(System::Object^  sender, System::EventArgs^  e);
};
}
