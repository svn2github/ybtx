#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace sqr_tools {

	/// <summary>
	/// Summary for testControl
	/// </summary>
	public ref class SizeControl : public System::Windows::Forms::UserControl
	{
	public:
		SizeControl(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
			m_iValue = 0;
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~SizeControl()
		{
			if (components)
			{
				delete components;
			}
		}

	public: System::Windows::Forms::Design::IWindowsFormsEditorService^ _wfes;
	private: System::Windows::Forms::PrintDialog^  printDialog1;
	private: System::Windows::Forms::TableLayoutPanel^  tableLayoutPanel1;
	private: System::Windows::Forms::Button^  downBtn;
	private: System::Windows::Forms::Button^  upBtn;
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
			this->printDialog1 = (gcnew System::Windows::Forms::PrintDialog());
			this->tableLayoutPanel1 = (gcnew System::Windows::Forms::TableLayoutPanel());
			this->upBtn = (gcnew System::Windows::Forms::Button());
			this->downBtn = (gcnew System::Windows::Forms::Button());
			this->tableLayoutPanel1->SuspendLayout();
			this->SuspendLayout();
			// 
			// printDialog1
			// 
			this->printDialog1->UseEXDialog = true;
			// 
			// tableLayoutPanel1
			// 
			this->tableLayoutPanel1->ColumnCount = 2;
			this->tableLayoutPanel1->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent, 
				50)));
			this->tableLayoutPanel1->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent, 
				50)));
			this->tableLayoutPanel1->Controls->Add(this->upBtn, 0, 0);
			this->tableLayoutPanel1->Controls->Add(this->downBtn, 1, 0);
			this->tableLayoutPanel1->Dock = System::Windows::Forms::DockStyle::Fill;
			this->tableLayoutPanel1->Location = System::Drawing::Point(0, 0);
			this->tableLayoutPanel1->Margin = System::Windows::Forms::Padding(0);
			this->tableLayoutPanel1->Name = L"tableLayoutPanel1";
			this->tableLayoutPanel1->RowCount = 1;
			this->tableLayoutPanel1->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 100)));
			this->tableLayoutPanel1->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 20)));
			this->tableLayoutPanel1->Size = System::Drawing::Size(95, 25);
			this->tableLayoutPanel1->TabIndex = 0;
			// 
			// upBtn
			// 
			this->upBtn->Dock = System::Windows::Forms::DockStyle::Fill;
			this->upBtn->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->upBtn->Location = System::Drawing::Point(0, 0);
			this->upBtn->Margin = System::Windows::Forms::Padding(0);
			this->upBtn->Name = L"upBtn";
			this->upBtn->Size = System::Drawing::Size(47, 25);
			this->upBtn->TabIndex = 0;
			this->upBtn->Text = L"·Å´ó";
			this->upBtn->UseVisualStyleBackColor = true;
			this->upBtn->Click += gcnew System::EventHandler(this, &SizeControl::upBtn_Click);
			// 
			// downBtn
			// 
			this->downBtn->Dock = System::Windows::Forms::DockStyle::Fill;
			this->downBtn->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->downBtn->Location = System::Drawing::Point(47, 0);
			this->downBtn->Margin = System::Windows::Forms::Padding(0);
			this->downBtn->Name = L"downBtn";
			this->downBtn->Size = System::Drawing::Size(48, 25);
			this->downBtn->TabIndex = 1;
			this->downBtn->Text = L"ËõÐ¡";
			this->downBtn->UseVisualStyleBackColor = true;
			this->downBtn->Click += gcnew System::EventHandler(this, &SizeControl::downBtn_Click);
			// 
			// SizeControl
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->Controls->Add(this->tableLayoutPanel1);
			this->Margin = System::Windows::Forms::Padding(0);
			this->Name = L"SizeControl";
			this->Size = System::Drawing::Size(95, 25);
			this->tableLayoutPanel1->ResumeLayout(false);
			this->ResumeLayout(false);

		}
#pragma endregion
	private: System::Void upBtn_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void downBtn_Click(System::Object^  sender, System::EventArgs^  e);
	public:	 int m_iValue;
};
}
