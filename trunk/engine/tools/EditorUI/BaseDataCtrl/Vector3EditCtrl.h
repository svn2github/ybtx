#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

#include "CUIPropBase.h"
namespace sqr_tools {
	ref class Vector3EditDialog;
	/// <summary>
	/// Summary for Vector3EditCtrl
	/// </summary>
	public ref class Vector3EditCtrl : public CUIPropBase
	{
	public:
		Vector3EditCtrl(void)
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
		~Vector3EditCtrl()
		{
			if (components)
			{
				delete components;
			}
		}







	private: System::Windows::Forms::Button^  VectorEditButton;
	private: System::Windows::Forms::NumericUpDown^  VectorXUpDown;
	private: System::Windows::Forms::NumericUpDown^  VectorYUpDown;
	private: System::Windows::Forms::NumericUpDown^  VectorZUpDown;

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
			this->VectorEditButton = (gcnew System::Windows::Forms::Button());
			this->VectorXUpDown = (gcnew System::Windows::Forms::NumericUpDown());
			this->VectorYUpDown = (gcnew System::Windows::Forms::NumericUpDown());
			this->VectorZUpDown = (gcnew System::Windows::Forms::NumericUpDown());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->VectorXUpDown))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->VectorYUpDown))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->VectorZUpDown))->BeginInit();
			this->SuspendLayout();
			// 
			// VectorEditButton
			// 
			this->VectorEditButton->Location = System::Drawing::Point(224, 2);
			this->VectorEditButton->Name = L"VectorEditButton";
			this->VectorEditButton->Size = System::Drawing::Size(87, 23);
			this->VectorEditButton->TabIndex = 5;
			this->VectorEditButton->Text = L"±à¼­ÏòÁ¿";
			this->VectorEditButton->UseVisualStyleBackColor = true;
			this->VectorEditButton->Click += gcnew System::EventHandler(this, &Vector3EditCtrl::VectorEditButton_Click);
			// 
			// VectorXUpDown
			// 
			this->VectorXUpDown->DecimalPlaces = 2;
			this->VectorXUpDown->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 131072});
			this->VectorXUpDown->Location = System::Drawing::Point(3, 4);
			this->VectorXUpDown->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {100000, 0, 0, 0});
			this->VectorXUpDown->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {100000, 0, 0, System::Int32::MinValue});
			this->VectorXUpDown->Name = L"VectorXUpDown";
			this->VectorXUpDown->Size = System::Drawing::Size(69, 21);
			this->VectorXUpDown->TabIndex = 6;
			this->VectorXUpDown->ValueChanged += gcnew System::EventHandler(this, &Vector3EditCtrl::VectorXUpDown_ValueChanged);
			// 
			// VectorYUpDown
			// 
			this->VectorYUpDown->DecimalPlaces = 2;
			this->VectorYUpDown->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 131072});
			this->VectorYUpDown->Location = System::Drawing::Point(78, 4);
			this->VectorYUpDown->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {100000, 0, 0, 0});
			this->VectorYUpDown->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {100000, 0, 0, System::Int32::MinValue});
			this->VectorYUpDown->Name = L"VectorYUpDown";
			this->VectorYUpDown->Size = System::Drawing::Size(66, 21);
			this->VectorYUpDown->TabIndex = 7;
			this->VectorYUpDown->ValueChanged += gcnew System::EventHandler(this, &Vector3EditCtrl::VectorYUpDown_ValueChanged);
			// 
			// VectorZUpDown
			// 
			this->VectorZUpDown->DecimalPlaces = 2;
			this->VectorZUpDown->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 131072});
			this->VectorZUpDown->Location = System::Drawing::Point(151, 4);
			this->VectorZUpDown->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {100000, 0, 0, 0});
			this->VectorZUpDown->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {100000, 0, 0, System::Int32::MinValue});
			this->VectorZUpDown->Name = L"VectorZUpDown";
			this->VectorZUpDown->Size = System::Drawing::Size(66, 21);
			this->VectorZUpDown->TabIndex = 8;
			this->VectorZUpDown->ValueChanged += gcnew System::EventHandler(this, &Vector3EditCtrl::VectorZUpDown_ValueChanged);
			// 
			// Vector3EditCtrl
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->Controls->Add(this->VectorZUpDown);
			this->Controls->Add(this->VectorYUpDown);
			this->Controls->Add(this->VectorXUpDown);
			this->Controls->Add(this->VectorEditButton);
			this->Name = L"Vector3EditCtrl";
			this->Size = System::Drawing::Size(318, 28);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->VectorXUpDown))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->VectorYUpDown))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->VectorZUpDown))->EndInit();
			this->ResumeLayout(false);

		}
#pragma endregion
		public:  virtual char*	GetImpTypeName(void)	override;
				 virtual void	Update(void)	override;

		private: System::Void	InitOtherComponent();
		private: System::Void	DestroyOtherComponent();

private: System::Void VectorEditButton_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void VectorXUpDown_ValueChanged(System::Object^  sender, System::EventArgs^  e);
private: System::Void VectorYUpDown_ValueChanged(System::Object^  sender, System::EventArgs^  e);
private: System::Void VectorZUpDown_ValueChanged(System::Object^  sender, System::EventArgs^  e);
};
}
