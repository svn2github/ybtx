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
	/// Summary for CPathSetCtrl
	/// </summary>
	public ref class CPathSetCtrl : public CUIBase//System::Windows::Forms::UserControl//
	{
	public:
		CPathSetCtrl(void)
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
		~CPathSetCtrl()
		{
			if (components)
			{
				delete components;
			}
		}

	public:	virtual char*	GetImpTypeName(void)	override;

	protected: 


	private: System::Windows::Forms::Button^  PathAdd;
	private: System::Windows::Forms::Button^  PathDel;
	private: System::Windows::Forms::Button^  PathUp;
	private: System::Windows::Forms::Button^  PathDown;
	private: System::Windows::Forms::Button^  PathApp;
	private: System::Windows::Forms::Button^  PathReset;
	private: System::Windows::Forms::Panel^  ButtonPanel1;
	private: System::Windows::Forms::ListBox^  PList;
	private: System::Windows::Forms::Button^  FileBrowser;
	private: System::Windows::Forms::TextBox^  PathEdit;
	private: System::Windows::Forms::Panel^  EditPanel;
	private: System::Windows::Forms::Panel^  ButtonPanel2;
	private: Int32	m_ActiveIndex;

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
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(CPathSetCtrl::typeid));
			this->PathAdd = (gcnew System::Windows::Forms::Button());
			this->PathDel = (gcnew System::Windows::Forms::Button());
			this->PathUp = (gcnew System::Windows::Forms::Button());
			this->PathDown = (gcnew System::Windows::Forms::Button());
			this->PathApp = (gcnew System::Windows::Forms::Button());
			this->ButtonPanel1 = (gcnew System::Windows::Forms::Panel());
			this->PList = (gcnew System::Windows::Forms::ListBox());
			this->FileBrowser = (gcnew System::Windows::Forms::Button());
			this->PathEdit = (gcnew System::Windows::Forms::TextBox());
			this->EditPanel = (gcnew System::Windows::Forms::Panel());
			this->ButtonPanel2 = (gcnew System::Windows::Forms::Panel());
			this->PathReset = (gcnew System::Windows::Forms::Button());
			this->ButtonPanel1->SuspendLayout();
			this->EditPanel->SuspendLayout();
			this->ButtonPanel2->SuspendLayout();
			this->SuspendLayout();
			// 
			// PathAdd
			// 
			this->PathAdd->AutoSize = true;
			this->PathAdd->BackgroundImage = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"PathAdd.BackgroundImage")));
			this->PathAdd->BackgroundImageLayout = System::Windows::Forms::ImageLayout::Zoom;
			this->PathAdd->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->PathAdd->ImageAlign = System::Drawing::ContentAlignment::MiddleLeft;
			this->PathAdd->Location = System::Drawing::Point(3, 3);
			this->PathAdd->Name = L"PathAdd";
			this->PathAdd->Size = System::Drawing::Size(38, 38);
			this->PathAdd->TabIndex = 0;
			this->PathAdd->UseVisualStyleBackColor = true;
			this->PathAdd->Click += gcnew System::EventHandler(this, &CPathSetCtrl::PathAdd_Click);
			// 
			// PathDel
			// 
			this->PathDel->AutoSize = true;
			this->PathDel->BackgroundImage = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"PathDel.BackgroundImage")));
			this->PathDel->BackgroundImageLayout = System::Windows::Forms::ImageLayout::Zoom;
			this->PathDel->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->PathDel->Location = System::Drawing::Point(43, 3);
			this->PathDel->Name = L"PathDel";
			this->PathDel->Size = System::Drawing::Size(38, 38);
			this->PathDel->TabIndex = 1;
			this->PathDel->UseVisualStyleBackColor = true;
			this->PathDel->Click += gcnew System::EventHandler(this, &CPathSetCtrl::PathDel_Click);
			// 
			// PathUp
			// 
			this->PathUp->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->PathUp->AutoSize = true;
			this->PathUp->BackgroundImage = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"PathUp.BackgroundImage")));
			this->PathUp->BackgroundImageLayout = System::Windows::Forms::ImageLayout::Zoom;
			this->PathUp->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->PathUp->Location = System::Drawing::Point(407, 3);
			this->PathUp->Name = L"PathUp";
			this->PathUp->Size = System::Drawing::Size(38, 38);
			this->PathUp->TabIndex = 2;
			this->PathUp->UseVisualStyleBackColor = true;
			this->PathUp->Click += gcnew System::EventHandler(this, &CPathSetCtrl::PathUp_Click);
			// 
			// PathDown
			// 
			this->PathDown->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->PathDown->AutoSize = true;
			this->PathDown->BackgroundImage = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"PathDown.BackgroundImage")));
			this->PathDown->BackgroundImageLayout = System::Windows::Forms::ImageLayout::Zoom;
			this->PathDown->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->PathDown->Location = System::Drawing::Point(447, 3);
			this->PathDown->Name = L"PathDown";
			this->PathDown->Size = System::Drawing::Size(38, 38);
			this->PathDown->TabIndex = 3;
			this->PathDown->UseVisualStyleBackColor = true;
			this->PathDown->Click += gcnew System::EventHandler(this, &CPathSetCtrl::PathDown_Click);
			// 
			// PathApp
			// 
			this->PathApp->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->PathApp->AutoSize = true;
			this->PathApp->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->PathApp->Location = System::Drawing::Point(367, 5);
			this->PathApp->Name = L"PathApp";
			this->PathApp->Size = System::Drawing::Size(55, 25);
			this->PathApp->TabIndex = 5;
			this->PathApp->Text = L"Ó¦ÓÃ";
			this->PathApp->UseVisualStyleBackColor = true;
			this->PathApp->Click += gcnew System::EventHandler(this, &CPathSetCtrl::PathApp_Click);
			// 
			// ButtonPanel1
			// 
			this->ButtonPanel1->Controls->Add(this->PathDown);
			this->ButtonPanel1->Controls->Add(this->PathUp);
			this->ButtonPanel1->Controls->Add(this->PathDel);
			this->ButtonPanel1->Controls->Add(this->PathAdd);
			this->ButtonPanel1->Dock = System::Windows::Forms::DockStyle::Top;
			this->ButtonPanel1->Location = System::Drawing::Point(0, 0);
			this->ButtonPanel1->Name = L"ButtonPanel1";
			this->ButtonPanel1->Size = System::Drawing::Size(488, 44);
			this->ButtonPanel1->TabIndex = 8;
			// 
			// PList
			// 
			this->PList->Dock = System::Windows::Forms::DockStyle::Fill;
			this->PList->Font = (gcnew System::Drawing::Font(L"SimSun", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(134)));
			this->PList->Location = System::Drawing::Point(0, 44);
			this->PList->Margin = System::Windows::Forms::Padding(5);
			this->PList->Name = L"PList";
			this->PList->Size = System::Drawing::Size(488, 212);
			this->PList->TabIndex = 9;
			this->PList->SelectedIndexChanged += gcnew System::EventHandler(this, &CPathSetCtrl::PList_SelectedIndexChanged);
			this->PList->DoubleClick += gcnew System::EventHandler(this, &CPathSetCtrl::OnSetPath);
			// 
			// FileBrowser
			// 
			this->FileBrowser->AutoSize = true;
			this->FileBrowser->Cursor = System::Windows::Forms::Cursors::Default;
			this->FileBrowser->Dock = System::Windows::Forms::DockStyle::Right;
			this->FileBrowser->FlatAppearance->BorderColor = System::Drawing::SystemColors::Control;
			this->FileBrowser->FlatAppearance->BorderSize = 0;
			this->FileBrowser->FlatAppearance->MouseDownBackColor = System::Drawing::SystemColors::ControlDarkDark;
			this->FileBrowser->FlatAppearance->MouseOverBackColor = System::Drawing::SystemColors::ControlDark;
			this->FileBrowser->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->FileBrowser->Font = (gcnew System::Drawing::Font(L"SimSun", 9, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(134)));
			this->FileBrowser->ImageAlign = System::Drawing::ContentAlignment::TopCenter;
			this->FileBrowser->Location = System::Drawing::Point(260, 0);
			this->FileBrowser->Margin = System::Windows::Forms::Padding(1);
			this->FileBrowser->Name = L"FileBrowser";
			this->FileBrowser->Size = System::Drawing::Size(40, 17);
			this->FileBrowser->TabIndex = 10;
			this->FileBrowser->Text = L"...\r\n";
			this->FileBrowser->TextAlign = System::Drawing::ContentAlignment::TopCenter;
			this->FileBrowser->UseVisualStyleBackColor = true;
			this->FileBrowser->Click += gcnew System::EventHandler(this, &CPathSetCtrl::OnFileBrowserClick);
			// 
			// PathEdit
			// 
			this->PathEdit->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->PathEdit->Dock = System::Windows::Forms::DockStyle::Fill;
			this->PathEdit->Font = (gcnew System::Drawing::Font(L"SimSun", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(134)));
			this->PathEdit->Location = System::Drawing::Point(0, 0);
			this->PathEdit->Margin = System::Windows::Forms::Padding(0);
			this->PathEdit->Name = L"PathEdit";
			this->PathEdit->Size = System::Drawing::Size(260, 22);
			this->PathEdit->TabIndex = 11;
			this->PathEdit->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &CPathSetCtrl::PathEdit_KeyDown);
			// 
			// EditPanel
			// 
			this->EditPanel->Controls->Add(this->PathEdit);
			this->EditPanel->Controls->Add(this->FileBrowser);
			this->EditPanel->Location = System::Drawing::Point(22, 11);
			this->EditPanel->Name = L"EditPanel";
			this->EditPanel->Size = System::Drawing::Size(300, 17);
			this->EditPanel->TabIndex = 12;
			this->EditPanel->Visible = false;
			// 
			// ButtonPanel2
			// 
			this->ButtonPanel2->Controls->Add(this->PathReset);
			this->ButtonPanel2->Controls->Add(this->EditPanel);
			this->ButtonPanel2->Controls->Add(this->PathApp);
			this->ButtonPanel2->Dock = System::Windows::Forms::DockStyle::Bottom;
			this->ButtonPanel2->Location = System::Drawing::Point(0, 234);
			this->ButtonPanel2->Name = L"ButtonPanel2";
			this->ButtonPanel2->Size = System::Drawing::Size(488, 33);
			this->ButtonPanel2->TabIndex = 13;
			// 
			// PathReset
			// 
			this->PathReset->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->PathReset->AutoSize = true;
			this->PathReset->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->PathReset->Location = System::Drawing::Point(427, 5);
			this->PathReset->Name = L"PathReset";
			this->PathReset->Size = System::Drawing::Size(55, 25);
			this->PathReset->TabIndex = 5;
			this->PathReset->Text = L"ÖØÖÃ";
			this->PathReset->UseVisualStyleBackColor = true;
			this->PathReset->Click += gcnew System::EventHandler(this, &CPathSetCtrl::PathApp_Click);
			// 
			// CPathSetCtrl
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->BackColor = System::Drawing::SystemColors::Control;
			this->Controls->Add(this->ButtonPanel2);
			this->Controls->Add(this->PList);
			this->Controls->Add(this->ButtonPanel1);
			this->ForeColor = System::Drawing::SystemColors::ControlText;
			this->Location = System::Drawing::Point(40, 40);
			this->Name = L"CPathSetCtrl";
			this->Size = System::Drawing::Size(488, 267);
			this->ButtonPanel1->ResumeLayout(false);
			this->ButtonPanel1->PerformLayout();
			this->EditPanel->ResumeLayout(false);
			this->EditPanel->PerformLayout();
			this->ButtonPanel2->ResumeLayout(false);
			this->ButtonPanel2->PerformLayout();
			this->ResumeLayout(false);

		}
#pragma endregion
		private: System::Void PathAdd_Click(System::Object^  sender, System::EventArgs^  e);
		private: System::Void PathDel_Click(System::Object^  sender, System::EventArgs^  e);
		private: System::Void PathUp_Click(System::Object^  sender, System::EventArgs^  e);
		private: System::Void PathDown_Click(System::Object^  sender, System::EventArgs^  e);
		private: System::Void PathApp_Click(System::Object^  sender, System::EventArgs^  e);
		private: System::Void OnSetPath(System::Object^  sender, System::EventArgs^  e);	 
		private: System::Void PathEdit_KeyDown(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e) ;
		private: System::Void OnFileBrowserClick(System::Object^  sender, System::EventArgs^  e);
		
		public:  virtual void Update(void) override;
		public:  virtual void ResetPath();
				 virtual void AppPath();
				 virtual void HideButton();
		protected:	virtual System::Void _InitCtrl(void) override;
		private:	System::Void PList_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e);
		private:	System::Void _ApplyEdit(void);
};
}
