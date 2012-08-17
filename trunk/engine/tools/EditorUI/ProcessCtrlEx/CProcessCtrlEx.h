#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

#include "CUIBase.h"

namespace sqr_tools {

	/// <summary>
	/// Summary for CProcessCtrlEx
	/// </summary>
	public ref class CProcessCtrlEx : public CUIBase
	{
	public:
		CProcessCtrlEx(void)
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
		~CProcessCtrlEx()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::ProgressBar^  progressBar1;
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
			this->progressBar1 = (gcnew System::Windows::Forms::ProgressBar());
			this->SuspendLayout();
			// 
			// progressBar1
			// 
			this->progressBar1->Location = System::Drawing::Point(3, 3);
			this->progressBar1->Name = L"progressBar1";
			this->progressBar1->Size = System::Drawing::Size(417, 33);
			this->progressBar1->TabIndex = 0;
			// 
			// CProcessCtrlEx
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->Controls->Add(this->progressBar1);
			this->Name = L"CProcessCtrlEx";
			this->Size = System::Drawing::Size(423, 39);
			this->ResumeLayout(false);

		}
#pragma endregion

		public: virtual void  Update() override;
		public: virtual char* GetImpTypeName(void)	override;
		public:  virtual void*	GetImpParam(void)		override
				 {
					 return this->Handle.ToPointer();
				 }
	};
}
