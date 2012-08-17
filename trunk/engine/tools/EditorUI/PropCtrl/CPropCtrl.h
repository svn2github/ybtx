#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;
using namespace System::Timers;

#include "CUIBase.h"
#include "BasicPropSet\CSceneProp.h"
#include "BasicPropSet\CModelProp.h"
#include "BasicPropSet\CLightProp.h"
#include "BasicPropSet\CAudioProp.h"


namespace sqr_tools {

	/// <summary>
	/// Summary for CPropCtrl
	/// </summary>
	public ref class CPropCtrl : public CUIBase
	{
	public:
		CPropCtrl(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
			sceneProp=nullptr;
			modelProp=nullptr;
			lightProp=nullptr;
			audioProp=nullptr;	
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~CPropCtrl()
		{
			if (components)
			{
				delete components;
			}
		}

	private:
		CSceneProp^		sceneProp;
		CModelProp^		modelProp;
		CLightProp^		lightProp;
		CAudioProp^		audioProp;
	private: System::Windows::Forms::Panel^  m_bottomPanel;
	private: System::Windows::Forms::CheckBox^  m_IsEdit;
	private: System::Windows::Forms::GroupBox^  m_gpBox;

	private: System::Windows::Forms::PropertyGrid^  m_PropView;
	public:	virtual char*	GetImpTypeName(void) override;
	public:	virtual void	Update(void) override;
	public:	virtual System::Void _InitCtrl(void) override;
	public:	virtual void	UpdateTimer(void);

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
			this->m_PropView = (gcnew System::Windows::Forms::PropertyGrid());
			this->m_bottomPanel = (gcnew System::Windows::Forms::Panel());
			this->m_IsEdit = (gcnew System::Windows::Forms::CheckBox());
			this->m_gpBox = (gcnew System::Windows::Forms::GroupBox());
			this->m_bottomPanel->SuspendLayout();
			this->m_gpBox->SuspendLayout();
			this->SuspendLayout();
			// 
			// m_PropView
			// 
			this->m_PropView->Dock = System::Windows::Forms::DockStyle::Fill;
			this->m_PropView->Location = System::Drawing::Point(3, 17);
			this->m_PropView->Name = L"m_PropView";
			this->m_PropView->Size = System::Drawing::Size(221, 280);
			this->m_PropView->TabIndex = 0;
			// 
			// m_bottomPanel
			// 
			this->m_bottomPanel->Controls->Add(this->m_IsEdit);
			this->m_bottomPanel->Dock = System::Windows::Forms::DockStyle::Bottom;
			this->m_bottomPanel->Location = System::Drawing::Point(3, 297);
			this->m_bottomPanel->Name = L"m_bottomPanel";
			this->m_bottomPanel->Size = System::Drawing::Size(221, 27);
			this->m_bottomPanel->TabIndex = 1;
			// 
			// m_IsEdit
			// 
			this->m_IsEdit->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->m_IsEdit->AutoSize = true;
			this->m_IsEdit->Location = System::Drawing::Point(165, 6);
			this->m_IsEdit->Name = L"m_IsEdit";
			this->m_IsEdit->Size = System::Drawing::Size(48, 16);
			this->m_IsEdit->TabIndex = 0;
			this->m_IsEdit->Text = L"±à¼­";
			this->m_IsEdit->UseVisualStyleBackColor = true;
			// 
			// m_gpBox
			// 
			this->m_gpBox->Controls->Add(this->m_PropView);
			this->m_gpBox->Controls->Add(this->m_bottomPanel);
			this->m_gpBox->Dock = System::Windows::Forms::DockStyle::Fill;
			this->m_gpBox->Location = System::Drawing::Point(0, 0);
			this->m_gpBox->Name = L"m_gpBox";
			this->m_gpBox->Size = System::Drawing::Size(227, 327);
			this->m_gpBox->TabIndex = 2;
			this->m_gpBox->TabStop = false;
			this->m_gpBox->Text = L"ÊôÐÔ";
			// 
			// CPropCtrl
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->Controls->Add(this->m_gpBox);
			this->Name = L"CPropCtrl";
			this->Size = System::Drawing::Size(227, 327);
			this->m_bottomPanel->ResumeLayout(false);
			this->m_bottomPanel->PerformLayout();
			this->m_gpBox->ResumeLayout(false);
			this->ResumeLayout(false);

		}
#pragma endregion
	
	};
}
