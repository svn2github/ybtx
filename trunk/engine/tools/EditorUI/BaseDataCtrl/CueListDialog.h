#pragma once
#include "AudioView/CueListCtrl.h"
using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace sqr_tools {
	/// <summary>
	/// Summary for CueListDialog
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class CueListDialog : public System::Windows::Forms::Form
	{
	public:
		CueListDialog(void)
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
		~CueListDialog()
		{
			if (components)
			{
				delete components;
			}
			DestroyOtherComponent();
		}
	private: sqr_tools::CueListCtrl^  DlgCueListCtrl;
	protected: 

	protected: 

	private: System::Windows::Forms::Button^  OKButton;
	private: System::Windows::Forms::Button^  CancleButton;
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
			this->DlgCueListCtrl = (gcnew sqr_tools::CueListCtrl());
			this->OKButton = (gcnew System::Windows::Forms::Button());
			this->CancleButton = (gcnew System::Windows::Forms::Button());
			this->SuspendLayout();
			// 
			// DlgCueListCtrl
			// 
			this->DlgCueListCtrl->ContextComponent = nullptr;
			this->DlgCueListCtrl->Location = System::Drawing::Point(0, 1);
			this->DlgCueListCtrl->Name = L"DlgCueListCtrl";
			this->DlgCueListCtrl->Size = System::Drawing::Size(214, 307);
			this->DlgCueListCtrl->TabIndex = 0;
			// 
			// OKButton
			// 
			this->OKButton->Location = System::Drawing::Point(13, 309);
			this->OKButton->Name = L"OKButton";
			this->OKButton->Size = System::Drawing::Size(75, 23);
			this->OKButton->TabIndex = 1;
			this->OKButton->Text = L"确定";
			this->OKButton->UseVisualStyleBackColor = true;
			this->OKButton->Click += gcnew System::EventHandler(this, &CueListDialog::OKButton_Click);
			// 
			// CancleButton
			// 
			this->CancleButton->Location = System::Drawing::Point(117, 309);
			this->CancleButton->Name = L"CancleButton";
			this->CancleButton->Size = System::Drawing::Size(75, 23);
			this->CancleButton->TabIndex = 2;
			this->CancleButton->Text = L"取消";
			this->CancleButton->UseVisualStyleBackColor = true;
			this->CancleButton->Click += gcnew System::EventHandler(this, &CueListDialog::CancleButton_Click);
			// 
			// CueListDialog
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(214, 337);
			this->Controls->Add(this->CancleButton);
			this->Controls->Add(this->OKButton);
			this->Controls->Add(this->DlgCueListCtrl);
			this->MaximizeBox = false;
			this->MinimizeBox = false;
			this->Name = L"CueListDialog";
			this->Text = L"CueListDialog";
			this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &CueListDialog::CueListDialog_FormClosing);
			this->ResumeLayout(false);

		}
#pragma endregion
	private:
		void*				m_pEditCtrl;
		System::String^		m_OndString;

	private: System::Void	InitOtherComponent();
	private: System::Void	DestroyOtherComponent();
	public:	void			SetCueEditData(void* ctrl, const char* str);

	private: System::Void OKButton_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void CancleButton_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void CueListDialog_FormClosing(System::Object^  sender, System::Windows::Forms::FormClosingEventArgs^  e);
	};
}
