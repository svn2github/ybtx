#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace sqr_tools {

	/// <summary>
	/// Summary for MaxMinNumEditDialog
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	class NumMapEditCtrlImp;
	public ref class MaxMinNumEditDialog : public System::Windows::Forms::Form
	{
	public:
		MaxMinNumEditDialog(void)
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
		~MaxMinNumEditDialog()
		{
			if (components)
			{
				delete components;
			}
			DestroyOtherComponent();
		}
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::NumericUpDown^  MaxNumeric;
	private: System::Windows::Forms::NumericUpDown^  MinNumeric;


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
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->MaxNumeric = (gcnew System::Windows::Forms::NumericUpDown());
			this->MinNumeric = (gcnew System::Windows::Forms::NumericUpDown());
			this->OKButton = (gcnew System::Windows::Forms::Button());
			this->CancleButton = (gcnew System::Windows::Forms::Button());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->MaxNumeric))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->MinNumeric))->BeginInit();
			this->SuspendLayout();
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(63, 19);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(41, 12);
			this->label1->TabIndex = 0;
			this->label1->Text = L"最大值";
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(227, 19);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(41, 12);
			this->label2->TabIndex = 1;
			this->label2->Text = L"最大值";
			// 
			// MaxNumeric
			// 
			this->MaxNumeric->DecimalPlaces = 2;
			this->MaxNumeric->Location = System::Drawing::Point(28, 47);
			this->MaxNumeric->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {100000, 0, 0, 0});
			this->MaxNumeric->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {100000, 0, 0, System::Int32::MinValue});
			this->MaxNumeric->Name = L"MaxNumeric";
			this->MaxNumeric->Size = System::Drawing::Size(120, 21);
			this->MaxNumeric->TabIndex = 2;
			this->MaxNumeric->ValueChanged += gcnew System::EventHandler(this, &MaxMinNumEditDialog::MaxNumeric_ValueChanged);
			// 
			// MinNumeric
			// 
			this->MinNumeric->DecimalPlaces = 2;
			this->MinNumeric->Location = System::Drawing::Point(180, 47);
			this->MinNumeric->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {100000, 0, 0, 0});
			this->MinNumeric->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {100000, 0, 0, System::Int32::MinValue});
			this->MinNumeric->Name = L"MinNumeric";
			this->MinNumeric->Size = System::Drawing::Size(120, 21);
			this->MinNumeric->TabIndex = 3;
			this->MinNumeric->ValueChanged += gcnew System::EventHandler(this, &MaxMinNumEditDialog::MinNumeric_ValueChanged);
			// 
			// OKButton
			// 
			this->OKButton->Location = System::Drawing::Point(180, 83);
			this->OKButton->Name = L"OKButton";
			this->OKButton->Size = System::Drawing::Size(75, 23);
			this->OKButton->TabIndex = 4;
			this->OKButton->Text = L"确定";
			this->OKButton->UseVisualStyleBackColor = true;
			this->OKButton->Click += gcnew System::EventHandler(this, &MaxMinNumEditDialog::OKButton_Click);
			// 
			// CancleButton
			// 
			this->CancleButton->Location = System::Drawing::Point(261, 83);
			this->CancleButton->Name = L"CancleButton";
			this->CancleButton->Size = System::Drawing::Size(75, 23);
			this->CancleButton->TabIndex = 5;
			this->CancleButton->Text = L"取消";
			this->CancleButton->UseVisualStyleBackColor = true;
			this->CancleButton->Click += gcnew System::EventHandler(this, &MaxMinNumEditDialog::CancleButton_Click);
			// 
			// MaxMinNumEditDialog
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(341, 109);
			this->Controls->Add(this->CancleButton);
			this->Controls->Add(this->OKButton);
			this->Controls->Add(this->MinNumeric);
			this->Controls->Add(this->MaxNumeric);
			this->Controls->Add(this->label2);
			this->Controls->Add(this->label1);
			this->MaximizeBox = false;
			this->MinimizeBox = false;
			this->Name = L"MaxMinNumEditDialog";
			this->Text = L"MaxMinNumEditDialog";
			this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &MaxMinNumEditDialog::MaxMinNumEditDialog_FormClosing);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->MaxNumeric))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->MinNumeric))->EndInit();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
		private:
			NumMapEditCtrlImp*		m_pEditCtrl;

			float		m_fMax;
			float		m_fMin;

		private: System::Void	InitOtherComponent();
		private: System::Void	DestroyOtherComponent();
		public:	void	SetMaxMinEditData(void* ctrl);


private: System::Void MaxNumeric_ValueChanged(System::Object^  sender, System::EventArgs^  e);
private: System::Void MinNumeric_ValueChanged(System::Object^  sender, System::EventArgs^  e);
private: System::Void OKButton_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void CancleButton_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void MaxMinNumEditDialog_FormClosing(System::Object^  sender, System::Windows::Forms::FormClosingEventArgs^  e);
};
}
