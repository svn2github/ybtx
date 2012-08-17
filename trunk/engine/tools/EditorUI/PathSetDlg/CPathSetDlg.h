#pragma once

#include "PathSetCtrl\CPathSetCtrl.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace sqr_tools {

	/// <summary>
	/// Summary for CPathSetDlg
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class CPathSetDlg : public System::Windows::Forms::Form
	{
	public:
		CPathSetDlg(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
			this->PathCtrl->HideButton();
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~CPathSetDlg()
		{
			if (components)
			{
				delete components;
			}
		}
	
	protected: 

	private: System::Windows::Forms::Button^  PathReset;
	private: System::Windows::Forms::Button^  PathCancle;
	private: System::Windows::Forms::Button^  PathApp;
	private: System::Windows::Forms::Button^  PathOK;
	private: sqr_tools::CPathSetCtrl^  PathCtrl;
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
			this->PathReset = (gcnew System::Windows::Forms::Button());
			this->PathCancle = (gcnew System::Windows::Forms::Button());
			this->PathApp = (gcnew System::Windows::Forms::Button());
			this->PathOK = (gcnew System::Windows::Forms::Button());
			this->PathCtrl = (gcnew sqr_tools::CPathSetCtrl());
			this->SuspendLayout();
			// 
			// PathReset
			// 
			this->PathReset->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->PathReset->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->PathReset->Location = System::Drawing::Point(510, 274);
			this->PathReset->Name = L"PathReset";
			this->PathReset->Size = System::Drawing::Size(55, 25);
			this->PathReset->TabIndex = 0;
			this->PathReset->Text = L"重置";
			this->PathReset->UseVisualStyleBackColor = true;
			this->PathReset->Click += gcnew System::EventHandler(this, &CPathSetDlg::PathReset_Click);
			// 
			// PathCancle
			// 
			this->PathCancle->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->PathCancle->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->PathCancle->Location = System::Drawing::Point(570, 274);
			this->PathCancle->Name = L"PathCancle";
			this->PathCancle->Size = System::Drawing::Size(55, 25);
			this->PathCancle->TabIndex = 0;
			this->PathCancle->Text = L"取消";
			this->PathCancle->UseVisualStyleBackColor = true;
			this->PathCancle->Click += gcnew System::EventHandler(this, &CPathSetDlg::PathCancle_Click);
			// 
			// PathApp
			// 
			this->PathApp->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->PathApp->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->PathApp->Location = System::Drawing::Point(5, 274);
			this->PathApp->Name = L"PathApp";
			this->PathApp->Size = System::Drawing::Size(55, 25);
			this->PathApp->TabIndex = 0;
			this->PathApp->Text = L"应用";
			this->PathApp->UseVisualStyleBackColor = true;
			this->PathApp->Click += gcnew System::EventHandler(this, &CPathSetDlg::PathApp_Click);
			// 
			// PathOK
			// 
			this->PathOK->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->PathOK->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->PathOK->Location = System::Drawing::Point(65, 274);
			this->PathOK->Name = L"PathOK";
			this->PathOK->Size = System::Drawing::Size(55, 25);
			this->PathOK->TabIndex = 0;
			this->PathOK->Text = L"确定";
			this->PathOK->UseVisualStyleBackColor = true;
			this->PathOK->Click += gcnew System::EventHandler(this, &CPathSetDlg::PathOK_Click);
			// 
			// PathCtrl
			// 
			this->PathCtrl->BackColor = System::Drawing::SystemColors::Control;
			this->PathCtrl->Dock = System::Windows::Forms::DockStyle::Fill;
			this->PathCtrl->ForeColor = System::Drawing::SystemColors::ControlText;
			this->PathCtrl->Location = System::Drawing::Point(0, 0);
			this->PathCtrl->Name = L"PathCtrl";
			this->PathCtrl->Size = System::Drawing::Size(631, 302);
			this->PathCtrl->TabIndex = 1;
			// 
			// CPathSetDlg
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(631, 302);
			this->Controls->Add(this->PathOK);
			this->Controls->Add(this->PathCancle);
			this->Controls->Add(this->PathApp);
			this->Controls->Add(this->PathReset);
			this->Controls->Add(this->PathCtrl);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedToolWindow;
			this->Name = L"CPathSetDlg";
			this->Text = L" 路径设置";
			this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &CPathSetDlg::CPathSetDlg_FormClosing);
			this->ResumeLayout(false);

		}
#pragma endregion

	public:	
		//void SetCoreCtrl(void* ctrl);

	private: System::Void PathApp_Click(System::Object^  sender, System::EventArgs^  e);	 
	private: System::Void PathReset_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void PathCancle_Click(System::Object^  sender, System::EventArgs^  e);	 
	private: System::Void PathOK_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void CPathSetDlg_FormClosing(System::Object^  sender, System::Windows::Forms::FormClosingEventArgs^  e);
};
}
