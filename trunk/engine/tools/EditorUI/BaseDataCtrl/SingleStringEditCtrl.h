#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

#include "CUIPropBase.h"
namespace sqr_tools {
	ref class CueListDialog;
	/// <summary>
	/// Summary for SingleStringEditCtrl
	/// </summary>
	public ref class SingleStringEditCtrl : public CUIPropBase
	{
	public:
		SingleStringEditCtrl(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
			InitOtherComponent();
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~SingleStringEditCtrl()
		{
			if (components)
			{
				delete components;
			}
			DestroyOtherComponent();
		}

	private: System::Windows::Forms::TextBox^  EditStringtextBox;
	private: System::Windows::Forms::Button^  ReadButton;
	private: System::Windows::Forms::OpenFileDialog^  StrOpenFileDialog;

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
			this->EditStringtextBox = (gcnew System::Windows::Forms::TextBox());
			this->ReadButton = (gcnew System::Windows::Forms::Button());
			this->StrOpenFileDialog = (gcnew System::Windows::Forms::OpenFileDialog());
			this->SuspendLayout();
			// 
			// EditStringtextBox
			// 
			this->EditStringtextBox->Location = System::Drawing::Point(3, 3);
			this->EditStringtextBox->Name = L"EditStringtextBox";
			this->EditStringtextBox->Size = System::Drawing::Size(259, 21);
			this->EditStringtextBox->TabIndex = 2;
			this->EditStringtextBox->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &SingleStringEditCtrl::EditStringtextBox_KeyDown);
			this->EditStringtextBox->Leave += gcnew System::EventHandler(this, &SingleStringEditCtrl::EditStringtextBox_Leave);
			// 
			// ReadButton
			// 
			this->ReadButton->Location = System::Drawing::Point(268, 1);
			this->ReadButton->Name = L"ReadButton";
			this->ReadButton->Size = System::Drawing::Size(67, 23);
			this->ReadButton->TabIndex = 3;
			this->ReadButton->Text = L"...";
			this->ReadButton->UseVisualStyleBackColor = true;
			this->ReadButton->Click += gcnew System::EventHandler(this, &SingleStringEditCtrl::ReadButton_Click);
			// 
			// StrOpenFileDialog
			// 
			this->StrOpenFileDialog->CheckFileExists = false;
			this->StrOpenFileDialog->FileName = L"选择打开文件";
			this->StrOpenFileDialog->ValidateNames = false;
			// 
			// SingleStringEditCtrl
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->Controls->Add(this->ReadButton);
			this->Controls->Add(this->EditStringtextBox);
			this->Name = L"SingleStringEditCtrl";
			this->Size = System::Drawing::Size(339, 30);
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
		public:  sqr_tools::CueListDialog^ m_CueListDlg;
		public:  virtual char*	GetImpTypeName(void)	override;
				 virtual void	Update(void)	override;
				 virtual void	SetCueListDlg(CueListDialog^ cueListDlg);

		private: System::Void	InitOtherComponent();
		private: System::Void	DestroyOtherComponent();
		private: void			_SetImpData();
				 
	private: System::Void EditStringtextBox_Leave(System::Object^  sender, System::EventArgs^  e);
	private: System::Void EditStringtextBox_KeyDown(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e);
	private: System::Void ReadButton_Click(System::Object^  sender, System::EventArgs^  e);
	};
}
