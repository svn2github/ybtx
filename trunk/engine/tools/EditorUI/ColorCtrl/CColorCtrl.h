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
	/// Summary for CColorCtrl
	/// </summary>
	public ref class CColorCtrl : public CUIBase
	{
	public:
		CColorCtrl(void)
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
		~CColorCtrl()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Label^  label1;
	protected: 
	private: System::Windows::Forms::TextBox^  color_R_textBox;
	private: System::Windows::Forms::TextBox^  color_G_textBox;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::TextBox^  color_B_textBox;
	private: System::Windows::Forms::Label^  label3;

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
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->color_R_textBox = (gcnew System::Windows::Forms::TextBox());
			this->color_G_textBox = (gcnew System::Windows::Forms::TextBox());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->color_B_textBox = (gcnew System::Windows::Forms::TextBox());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->SuspendLayout();
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(3, 9);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(11, 12);
			this->label1->TabIndex = 0;
			this->label1->Text = L"R";
			// 
			// color_R_textBox
			// 
			this->color_R_textBox->Location = System::Drawing::Point(19, 6);
			this->color_R_textBox->Name = L"color_R_textBox";
			this->color_R_textBox->Size = System::Drawing::Size(39, 21);
			this->color_R_textBox->TabIndex = 1;
			this->color_R_textBox->TextChanged += gcnew System::EventHandler(this, &CColorCtrl::color_R_textBox_TextChanged);
			// 
			// color_G_textBox
			// 
			this->color_G_textBox->Location = System::Drawing::Point(84, 6);
			this->color_G_textBox->Name = L"color_G_textBox";
			this->color_G_textBox->Size = System::Drawing::Size(39, 21);
			this->color_G_textBox->TabIndex = 3;
			this->color_G_textBox->TextChanged += gcnew System::EventHandler(this, &CColorCtrl::color_G_textBox_TextChanged);
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(68, 9);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(11, 12);
			this->label2->TabIndex = 2;
			this->label2->Text = L"G";
			// 
			// color_B_textBox
			// 
			this->color_B_textBox->Location = System::Drawing::Point(148, 6);
			this->color_B_textBox->Name = L"color_B_textBox";
			this->color_B_textBox->Size = System::Drawing::Size(39, 21);
			this->color_B_textBox->TabIndex = 5;
			this->color_B_textBox->TextChanged += gcnew System::EventHandler(this, &CColorCtrl::color_B_textBox_TextChanged);
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Location = System::Drawing::Point(132, 9);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(11, 12);
			this->label3->TabIndex = 4;
			this->label3->Text = L"B";
			// 
			// CColorCtrl
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->Controls->Add(this->color_B_textBox);
			this->Controls->Add(this->label3);
			this->Controls->Add(this->color_G_textBox);
			this->Controls->Add(this->label2);
			this->Controls->Add(this->color_R_textBox);
			this->Controls->Add(this->label1);
			this->Name = L"CColorCtrl";
			this->Size = System::Drawing::Size(197, 33);
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion

		public:  virtual void   Update(void) override;
		public:  virtual char*	GetImpTypeName(void)	override;
		public:  virtual void*	GetImpParam(void)		override;
		private: System::Void color_R_textBox_TextChanged(System::Object^  sender, System::EventArgs^  e);
		private: System::Void color_G_textBox_TextChanged(System::Object^  sender, System::EventArgs^  e);
		private: System::Void color_B_textBox_TextChanged(System::Object^  sender, System::EventArgs^  e);
		private: void SetColorCtrlImp();
};
}
