#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;
#include "FProcessCtrl\CFProcessCtrl.h"

namespace sqr_tools {

	/// <summary>
	/// Summary for CFProcessDlg
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class CFProcessDlg : public System::Windows::Forms::Form
	{
	public:
		CFProcessDlg(void)
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
		~CFProcessDlg()
		{
			if (components)
			{
				delete components;
			}
		}
	private: sqr_tools::CFProcessCtrl^  cFileProcessCtrl1;

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
			this->cFileProcessCtrl1 = (gcnew sqr_tools::CFProcessCtrl());
			this->SuspendLayout();
			// 
			// cFileProcessCtrl1
			// 
			this->cFileProcessCtrl1->Location = System::Drawing::Point(9, 7);
			this->cFileProcessCtrl1->Name = L"cFileProcessCtrl1";
			this->cFileProcessCtrl1->Size = System::Drawing::Size(503, 508);
			this->cFileProcessCtrl1->TabIndex = 0;
			// 
			// CFProcessDlg
			// 
			this->ClientSize = System::Drawing::Size(512, 518);
			this->Controls->Add(this->cFileProcessCtrl1);
			this->Name = L"CFProcessDlg";
			this->Text = L"批量处理";
			this->FormClosed += gcnew System::Windows::Forms::FormClosedEventHandler(this, &CFProcessDlg::CFProcessDlg_FormClosed);
			this->ResumeLayout(false);

		}
#pragma endregion

		public:void BindPathBrowserCtrl();
		public:void UpdateCtrl();

	private: System::Void CFProcessDlg_FormClosed(System::Object^  sender, System::Windows::Forms::FormClosedEventArgs^  e);
	};
}
