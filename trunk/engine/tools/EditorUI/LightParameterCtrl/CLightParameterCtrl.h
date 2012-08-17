#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

#include "CUIBase.h"
#include "LightViewCtrl\CLightView.h"

namespace sqr_tools {

	/// <summary>
	/// Summary for CLightParameterCtrl
	/// </summary>
	public ref class CLightParameterCtrl : public CUIBase
	{
	public:
		CLightParameterCtrl(void)
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
		~CLightParameterCtrl()
		{
			if (components)
			{
				delete components;
			}
		}

	private: System::Windows::Forms::PropertyGrid^  m_LightProp;
	private: System::Windows::Forms::GroupBox^  lightPropGroup;

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
			this->m_LightProp = (gcnew System::Windows::Forms::PropertyGrid());
			this->lightPropGroup = (gcnew System::Windows::Forms::GroupBox());
			this->lightPropGroup->SuspendLayout();
			this->SuspendLayout();
			// 
			// m_LightProp
			// 
			this->m_LightProp->Dock = System::Windows::Forms::DockStyle::Fill;
			this->m_LightProp->Location = System::Drawing::Point(3, 17);
			this->m_LightProp->Name = L"m_LightProp";
			this->m_LightProp->Size = System::Drawing::Size(263, 251);
			this->m_LightProp->TabIndex = 1;
			// 
			// lightPropGroup
			// 
			this->lightPropGroup->Controls->Add(this->m_LightProp);
			this->lightPropGroup->Location = System::Drawing::Point(3, 12);
			this->lightPropGroup->Name = L"lightPropGroup";
			this->lightPropGroup->Size = System::Drawing::Size(269, 271);
			this->lightPropGroup->TabIndex = 2;
			this->lightPropGroup->TabStop = false;
			this->lightPropGroup->Text = L"指定灯光属性";
			// 
			// CLightParameterCtrl
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->Controls->Add(this->lightPropGroup);
			this->Name = L"CLightParameterCtrl";
			this->Size = System::Drawing::Size(284, 300);
			this->lightPropGroup->ResumeLayout(false);
			this->ResumeLayout(false);

		}
#pragma endregion
	public:	 virtual char* GetImpTypeName(void) override;
	public:  virtual void Update(void) override;
	//public:  virtual char*	GetImpTypeName(void)	override;
	};
}
