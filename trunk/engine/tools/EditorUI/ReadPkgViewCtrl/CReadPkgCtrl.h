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
	/// Summary for CReadPkgCtrl
	/// </summary>
	public ref class CReadPkgCtrl : public CUIBase
	{
	public:
		CReadPkgCtrl(void)
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
		~CReadPkgCtrl()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::TextBox^  searchFilePathTextBox;
	protected: 
	private: System::Windows::Forms::Button^  fileInfoButton;
	private: System::Windows::Forms::Button^  readPkgButton;
	private: System::Windows::Forms::ListView^  fileInfosListView;
	private: System::Windows::Forms::OpenFileDialog^  openFileDialog;


	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;

		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		#pragma region Windows Form Designer generated code
		void InitializeComponent( void )
		{
			this->searchFilePathTextBox = (gcnew System::Windows::Forms::TextBox());
			this->fileInfoButton = (gcnew System::Windows::Forms::Button());
			this->readPkgButton = (gcnew System::Windows::Forms::Button());
			this->fileInfosListView = (gcnew System::Windows::Forms::ListView());
			this->openFileDialog = (gcnew System::Windows::Forms::OpenFileDialog());
			this->SuspendLayout();
			// 
			// searchFilePathTextBox
			// 
			this->searchFilePathTextBox->Location = System::Drawing::Point(5, 8);
			this->searchFilePathTextBox->Name = L"searchFilePathTextBox";
			this->searchFilePathTextBox->Size = System::Drawing::Size(396, 21);
			this->searchFilePathTextBox->TabIndex = 0;
			// 
			// fileInfoButton
			// 
			this->fileInfoButton->Location = System::Drawing::Point(411, 8);
			this->fileInfoButton->Name = L"fileInfoButton";
			this->fileInfoButton->Size = System::Drawing::Size(77, 25);
			this->fileInfoButton->TabIndex = 1;
			this->fileInfoButton->Text = L"ÏÔÊ¾Æ«ÒÆ";
			this->fileInfoButton->UseVisualStyleBackColor = true;
			this->fileInfoButton->Click += gcnew System::EventHandler(this, &CReadPkgCtrl::fileInfoButton_Click);
			// 
			// readPkgButton
			// 
			this->readPkgButton->Location = System::Drawing::Point(494, 8);
			this->readPkgButton->Name = L"readPkgButton";
			this->readPkgButton->Size = System::Drawing::Size(77, 25);
			this->readPkgButton->TabIndex = 2;
			this->readPkgButton->Text = L"¶Á°ü";
			this->readPkgButton->UseVisualStyleBackColor = true;
			this->readPkgButton->Click += gcnew System::EventHandler(this, &CReadPkgCtrl::readPkgButton_Click);
			// 
			// fileInfosListView
			// 
			this->fileInfosListView->BackColor = System::Drawing::SystemColors::GradientInactiveCaption;
			this->fileInfosListView->GridLines = true;
			this->fileInfosListView->Location = System::Drawing::Point(8, 39);
			this->fileInfosListView->Name = L"fileInfosListView";
			this->fileInfosListView->Size = System::Drawing::Size(562, 193);
			this->fileInfosListView->TabIndex = 3;
			this->fileInfosListView->UseCompatibleStateImageBehavior = false;
			this->fileInfosListView->View = System::Windows::Forms::View::Details;
			// 
			// openFileDialog
			// 
			this->openFileDialog->FileName = L"openFileDialog";
			// 
			// CReadPkgCtrl
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->Controls->Add(this->fileInfosListView);
			this->Controls->Add(this->readPkgButton);
			this->Controls->Add(this->fileInfoButton);
			this->Controls->Add(this->searchFilePathTextBox);
			this->Name = L"CReadPkgCtrl";
			this->Size = System::Drawing::Size(578, 238);
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
		void InitListViewCtrl();

		private: System::Void fileInfoButton_Click(System::Object^  sender, System::EventArgs^  e);
		private: System::Void readPkgButton_Click(System::Object^  sender, System::EventArgs^  e);

		public:  virtual char*	GetImpTypeName(void)	override;
};
}
