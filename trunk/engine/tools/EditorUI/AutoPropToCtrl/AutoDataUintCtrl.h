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
	/// Summary for AutoDataUintCtrl
	/// </summary>
	public ref class AutoDataUintCtrl : public System::Windows::Forms::UserControl
	{
	public:
		AutoDataUintCtrl(void)
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
		~AutoDataUintCtrl()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Label^  PropTitlelabel;
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
			this->PropTitlelabel = (gcnew System::Windows::Forms::Label());
			this->SuspendLayout();
			// 
			// PropTitlelabel
			// 
			this->PropTitlelabel->Location = System::Drawing::Point(3, 3);
			this->PropTitlelabel->Name = L"PropTitlelabel";
			this->PropTitlelabel->Size = System::Drawing::Size(120, 15);
			this->PropTitlelabel->TabIndex = 0;
			this->PropTitlelabel->Text = L"ÃèÊö";
			// 
			// AutoDataUintCtrl
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->Controls->Add(this->PropTitlelabel);
			this->Name = L"AutoDataUintCtrl";
			this->Size = System::Drawing::Size(259, 91);
			this->ResumeLayout(false);

		}
#pragma endregion
	public:	void LayoutAutoDataCtrl(bool Hover, const char* title, CUIPropBase^ ctrl);		
	};
}
