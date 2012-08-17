#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

#include "CUIBase.h"
#include "BasicPropSet\CLightProp.h"
#include "BasicPropSet\CSceneProp.h"

namespace sqr_tools {

	/// <summary>
	/// Summary for CTestUI
	/// </summary>
	public ref class CTestUI : public CUIBase
	{
	public:
		CTestUI(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
			Init();
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~CTestUI()
		{
			if (components)
			{
				delete components;
			}
		}
	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;
	private: System::Windows::Forms::PropertyGrid^  propertyGrid1;

		CLightProp^		lightProp;
		CSceneProp^		sceneProp;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->propertyGrid1 = (gcnew System::Windows::Forms::PropertyGrid());
			this->SuspendLayout();
			// 
			// propertyGrid1
			// 
			this->propertyGrid1->Location = System::Drawing::Point(17, 15);
			this->propertyGrid1->Name = L"propertyGrid1";
			this->propertyGrid1->Size = System::Drawing::Size(216, 341);
			this->propertyGrid1->TabIndex = 0;
			// 
			// CTestUI
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->Controls->Add(this->propertyGrid1);
			this->Name = L"CTestUI";
			this->Size = System::Drawing::Size(258, 386);
			this->ResumeLayout(false);

		}
#pragma endregion

		void Init();
	};
}
