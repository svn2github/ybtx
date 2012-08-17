#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace CharacterBrowser {

	/// <summary>
	/// Summary for CDefineScaleDlg
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class CDefineScaleDlg : public System::Windows::Forms::Form
	{
	public:
		CDefineScaleDlg(void)
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
		~CDefineScaleDlg()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Label^  label;
	protected: 
	private: System::Windows::Forms::TextBox^  scale_textBox;
	private: System::Windows::Forms::Button^  ok_button;


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
			this->label = (gcnew System::Windows::Forms::Label());
			this->scale_textBox = (gcnew System::Windows::Forms::TextBox());
			this->ok_button = (gcnew System::Windows::Forms::Button());
			this->SuspendLayout();
			// 
			// label
			// 
			this->label->AutoSize = true;
			this->label->Location = System::Drawing::Point(46, 15);
			this->label->Name = L"label";
			this->label->Size = System::Drawing::Size(41, 12);
			this->label->TabIndex = 0;
			this->label->Text = L"大小：";
			// 
			// scale_textBox
			// 
			this->scale_textBox->Location = System::Drawing::Point(87, 12);
			this->scale_textBox->Name = L"scale_textBox";
			this->scale_textBox->Size = System::Drawing::Size(70, 21);
			this->scale_textBox->TabIndex = 1;
			// 
			// ok_button
			// 
			this->ok_button->Location = System::Drawing::Point(70, 39);
			this->ok_button->Name = L"ok_button";
			this->ok_button->Size = System::Drawing::Size(71, 30);
			this->ok_button->TabIndex = 2;
			this->ok_button->Text = L"确定";
			this->ok_button->UseVisualStyleBackColor = true;
			this->ok_button->Click += gcnew System::EventHandler(this, &CDefineScaleDlg::ok_button_Click);
			// 
			// CDefineScaleDlg
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(216, 88);
			this->Controls->Add(this->ok_button);
			this->Controls->Add(this->scale_textBox);
			this->Controls->Add(this->label);
			this->Name = L"CDefineScaleDlg";
			this->Text = L"自定义光圈大小";
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion

		private: System::Void ok_button_Click(System::Object^  sender, System::EventArgs^  e);

		public:  double GetScale();
	};
}
