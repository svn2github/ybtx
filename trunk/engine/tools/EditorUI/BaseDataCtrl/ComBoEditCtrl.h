#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

#include "CUIPropBase.h"
namespace sqr_tools {

	class CCombDataHelper;

	/// <summary>
	/// Summary for ComBoEditCtrl
	/// </summary>
	public ref class ComBoEditCtrl : public CUIPropBase
	{
	public:
		ComBoEditCtrl(void)
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
		~ComBoEditCtrl()
		{
			if (components)
			{
				delete components;
			}
			DestroyOtherComponent();
		}
	private: System::Windows::Forms::ComboBox^  EditComboBox;
	protected: 

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
			this->EditComboBox = (gcnew System::Windows::Forms::ComboBox());
			this->SuspendLayout();
			// 
			// EditComboBox
			// 
			this->EditComboBox->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->EditComboBox->FormattingEnabled = true;
			this->EditComboBox->Location = System::Drawing::Point(0, 3);
			this->EditComboBox->Name = L"EditComboBox";
			this->EditComboBox->Size = System::Drawing::Size(152, 20);
			this->EditComboBox->TabIndex = 2;
			this->EditComboBox->SelectedValueChanged += gcnew System::EventHandler(this, &ComBoEditCtrl::EditComboBox_SelectedValueChanged);
			// 
			// ComBoEditCtrl
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->Controls->Add(this->EditComboBox);
			this->Name = L"ComBoEditCtrl";
			this->Size = System::Drawing::Size(158, 28);
			this->ResumeLayout(false);

		}
#pragma endregion
		public:  virtual char*	GetImpTypeName(void)	override;
				 virtual void	Update(void)	override;

		private: System::Void	InitOtherComponent();
		private: System::Void	DestroyOtherComponent();
		private: void			_ResetItem( int Value, CCombDataHelper* pDataHelper );
	private: System::Void EditComboBox_SelectedValueChanged(System::Object^  sender, System::EventArgs^  e);
	};
}
