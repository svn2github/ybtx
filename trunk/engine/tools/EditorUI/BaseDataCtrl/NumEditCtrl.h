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
	/// Summary for NumEditCtrl
	/// </summary>
	public ref class NumEditCtrl : public CUIPropBase
	{
	public:
		NumEditCtrl(void)
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
		~NumEditCtrl()
		{
			if (components)
			{
				delete components;
			}
			DestroyOtherComponent();
		}

	private: System::Windows::Forms::NumericUpDown^  NumUpDown;

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
			this->NumUpDown = (gcnew System::Windows::Forms::NumericUpDown());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->NumUpDown))->BeginInit();
			this->SuspendLayout();
			// 
			// NumUpDown
			// 
			this->NumUpDown->Location = System::Drawing::Point(3, 3);
			this->NumUpDown->Name = L"NumUpDown";
			this->NumUpDown->Size = System::Drawing::Size(120, 21);
			this->NumUpDown->TabIndex = 1;
			this->NumUpDown->ValueChanged += gcnew System::EventHandler(this, &NumEditCtrl::NumUpDown_ValueChanged);
			// 
			// NumEditCtrl
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->Controls->Add(this->NumUpDown);
			this->Name = L"NumEditCtrl";
			this->Size = System::Drawing::Size(132, 27);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->NumUpDown))->EndInit();
			this->ResumeLayout(false);

		}
#pragma endregion
		private:
			int		m_eType;
		public:  virtual char*	GetImpTypeName(void)	override;
				 virtual void	Update(void)	override;

		private: System::Void	InitOtherComponent();
		private: System::Void	DestroyOtherComponent();

	private: System::Void NumUpDown_ValueChanged(System::Object^  sender, System::EventArgs^  e);
	};
}
