#pragma once

#include "VideoSetCtrl\CVideoSettingCtrl.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace sqr_tools {

	/// <summary>
	/// Summary for CVideoSetDlg
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class CVideoSetDlg : public System::Windows::Forms::Form
	{
	public:
		CVideoSetDlg(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
			//this->pVideo->HideButton();
			m_pVideo->HideButton();
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~CVideoSetDlg()
		{
			if (components)
			{
				delete components;
			}
		}
	protected: 

	protected: 
	private: System::Windows::Forms::Button^  button1;
	private: System::Windows::Forms::Button^  button2;
	private: System::Windows::Forms::Button^  button3;
	private: sqr_tools::CVideoSettingCtrl^  m_pVideo;


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
			this->button1 = (gcnew System::Windows::Forms::Button());
			this->button2 = (gcnew System::Windows::Forms::Button());
			this->button3 = (gcnew System::Windows::Forms::Button());
			this->m_pVideo = (gcnew sqr_tools::CVideoSettingCtrl());
			this->SuspendLayout();
			// 
			// button1
			// 
			this->button1->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->button1->Location = System::Drawing::Point(188, 290);
			this->button1->Name = L"button1";
			this->button1->Size = System::Drawing::Size(57, 23);
			this->button1->TabIndex = 1;
			this->button1->Text = L"确认";
			this->button1->UseVisualStyleBackColor = true;
			// 
			// button2
			// 
			this->button2->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->button2->Location = System::Drawing::Point(12, 288);
			this->button2->Name = L"button2";
			this->button2->Size = System::Drawing::Size(53, 23);
			this->button2->TabIndex = 1;
			this->button2->Text = L"重置";
			this->button2->UseVisualStyleBackColor = true;
			// 
			// button3
			// 
			this->button3->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->button3->Location = System::Drawing::Point(251, 290);
			this->button3->Name = L"button3";
			this->button3->Size = System::Drawing::Size(57, 23);
			this->button3->TabIndex = 1;
			this->button3->Text = L"取消";
			this->button3->UseVisualStyleBackColor = true;
			// 
			// m_pVideo
			// 
			this->m_pVideo->Location = System::Drawing::Point(3, 1);
			this->m_pVideo->Name = L"m_pVideo";
			this->m_pVideo->Size = System::Drawing::Size(315, 319);
			this->m_pVideo->TabIndex = 2;
			// 
			// CVideoSetDlg
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(322, 320);
			this->Controls->Add(this->button3);
			this->Controls->Add(this->button1);
			this->Controls->Add(this->button2);
			this->Controls->Add(this->m_pVideo);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedToolWindow;
			this->Name = L"CVideoSetDlg";
			this->Text = L"视频设置";
			this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &CVideoSetDlg::CVideoSetDlg_FormClosing);
			this->ResumeLayout(false);

		}
#pragma endregion
	private: System::Void CVideoSetDlg_FormClosing(System::Object^  sender, System::Windows::Forms::FormClosingEventArgs^  e);
};
}
