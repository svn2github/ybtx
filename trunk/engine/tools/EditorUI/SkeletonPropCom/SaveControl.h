#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace sqr_tools {

	/// <summary>
	/// Summary for SaveControl
	/// </summary>
	public ref class SaveControl : public System::Windows::Forms::UserControl
	{
	public:
		SaveControl(void)
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
		~SaveControl()
		{
			if (components)
			{
				delete components;
			}
		}
	public: System::Windows::Forms::Design::IWindowsFormsEditorService^ _wfes;
	private: System::Windows::Forms::Button^  SaveBtn;

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
			this->SaveBtn = (gcnew System::Windows::Forms::Button());
			this->SuspendLayout();
			// 
			// SaveBtn
			// 
			this->SaveBtn->Dock = System::Windows::Forms::DockStyle::Fill;
			this->SaveBtn->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->SaveBtn->Location = System::Drawing::Point(0, 0);
			this->SaveBtn->Name = L"SaveBtn";
			this->SaveBtn->Size = System::Drawing::Size(80, 36);
			this->SaveBtn->TabIndex = 0;
			this->SaveBtn->Text = L"±£´æ";
			this->SaveBtn->UseVisualStyleBackColor = true;
			this->SaveBtn->Click += gcnew System::EventHandler(this, &SaveControl::SaveBtn_Click);
			// 
			// SaveControl
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->Controls->Add(this->SaveBtn);
			this->Name = L"SaveControl";
			this->Size = System::Drawing::Size(80, 36);
			this->ResumeLayout(false);

		}
#pragma endregion
	private: System::Void SaveBtn_Click(System::Object^  sender, System::EventArgs^  e);
	};
}
