#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

#include "NewMapCtrl/CNewMapCtrl.h"

namespace sqr_tools {

	/// <summary>
	/// Summary for CNewMapDlg
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class CNewMapDlg : public System::Windows::Forms::Form
	{
	public:
		CNewMapDlg(void)
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
		~CNewMapDlg()
		{
			if (components)
			{
				delete components;
			}
		}
	private: sqr_tools::CNewMapCtrl^  m_newMapCtrl;
	private: System::Windows::Forms::Button^  btOK;
	private: System::Windows::Forms::Button^  btCancle;
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
			this->m_newMapCtrl = (gcnew sqr_tools::CNewMapCtrl());
			this->btOK = (gcnew System::Windows::Forms::Button());
			this->btCancle = (gcnew System::Windows::Forms::Button());
			this->SuspendLayout();
			// 
			// m_newMapCtrl
			// 
			this->m_newMapCtrl->Location = System::Drawing::Point(2, 3);
			this->m_newMapCtrl->Name = L"m_newMapCtrl";
			this->m_newMapCtrl->Size = System::Drawing::Size(330, 183);
			this->m_newMapCtrl->TabIndex = 0;
			// 
			// btOK
			// 
			this->btOK->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->btOK->Location = System::Drawing::Point(217, 141);
			this->btOK->Name = L"btOK";
			this->btOK->Size = System::Drawing::Size(48, 24);
			this->btOK->TabIndex = 1;
			this->btOK->Text = L"确定";
			this->btOK->UseVisualStyleBackColor = true;
			this->btOK->Click += gcnew System::EventHandler(this, &CNewMapDlg::btOK_Click);
			// 
			// btCancle
			// 
			this->btCancle->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->btCancle->Location = System::Drawing::Point(269, 141);
			this->btCancle->Name = L"btCancle";
			this->btCancle->Size = System::Drawing::Size(48, 24);
			this->btCancle->TabIndex = 1;
			this->btCancle->Text = L"取消";
			this->btCancle->UseVisualStyleBackColor = true;
			this->btCancle->Click += gcnew System::EventHandler(this, &CNewMapDlg::btCancle_Click);
			// 
			// CNewMapDlg
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(333, 186);
			this->Controls->Add(this->btCancle);
			this->Controls->Add(this->btOK);
			this->Controls->Add(this->m_newMapCtrl);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedToolWindow;
			this->Name = L"CNewMapDlg";
			this->Text = L"创建地图";
			this->ResumeLayout(false);

		}
#pragma endregion
	private: System::Void btOK_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void btCancle_Click(System::Object^  sender, System::EventArgs^  e);
	};
}
