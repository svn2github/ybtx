#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

#include "CUIPropBase.h"
namespace sqr_tools {

	/// <summary>
	/// Summary for BoolEditCtrl
	/// </summary>
	public ref class BoolEditCtrl : public CUIPropBase
	{
	public:
		BoolEditCtrl(void)
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
		~BoolEditCtrl()
		{
			if (components)
			{
				delete components;
			}
			DestroyOtherComponent();
		}

	private: System::Windows::Forms::CheckBox^  DataCheckBox;

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
			this->DataCheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->SuspendLayout();
			// 
			// DataCheckBox
			// 
			this->DataCheckBox->Location = System::Drawing::Point(3, 4);
			this->DataCheckBox->Name = L"DataCheckBox";
			this->DataCheckBox->Size = System::Drawing::Size(49, 19);
			this->DataCheckBox->TabIndex = 2;
			this->DataCheckBox->Text = L"·ñ";
			this->DataCheckBox->UseVisualStyleBackColor = true;
			this->DataCheckBox->CheckStateChanged += gcnew System::EventHandler(this, &BoolEditCtrl::DataCheckBox_CheckStateChanged);
			this->DataCheckBox->KeyUp += gcnew System::Windows::Forms::KeyEventHandler(this, &BoolEditCtrl::DataCheckBox_KeyUp);
			this->DataCheckBox->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &BoolEditCtrl::DataCheckBox_KeyDown);
			// 
			// BoolEditCtrl
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->Controls->Add(this->DataCheckBox);
			this->Name = L"BoolEditCtrl";
			this->Size = System::Drawing::Size(61, 26);
			this->ResumeLayout(false);

		}
#pragma endregion

		public:  virtual char*	GetImpTypeName(void)	override;
				 virtual void	Update(void)	override;

		private: System::Void	InitOtherComponent();
		private: System::Void	DestroyOtherComponent();
	private: System::Void DataCheckBox_CheckStateChanged(System::Object^  sender, System::EventArgs^  e);
	private: System::Void DataCheckBox_KeyDown(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e);
	private: System::Void DataCheckBox_KeyUp(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e);
	};
}
