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
	/// Summary for CLightView
	/// </summary>
	public ref class CLightView : public CUIBase
	{
	public:
		CLightView(void)
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
		~CLightView()
		{
			if (components)
			{
				delete components;
			}
		}
	public:
		virtual char*	GetImpTypeName(void)	override;
		virtual void*	GetImpParam(void)		override;

	private: System::Windows::Forms::Panel^  m_Light3DView;
	private: System::Windows::Forms::GroupBox^  gLightSetGroup;
	private: System::Windows::Forms::Button^  btColorSet;
	private: System::Windows::Forms::RadioButton^  rbtGAColor;
	private: System::Windows::Forms::RadioButton^  rbtGSColor;
	private: System::Windows::Forms::RadioButton^  rbtRAColor;
	private: System::Windows::Forms::RadioButton^  rbtRSColor;
	private: System::Windows::Forms::ColorDialog^  ColorSetDlg;
	private: System::Windows::Forms::Button^  btReset;

	private: System::Windows::Forms::Button^  btApp;

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
			this->m_Light3DView = (gcnew System::Windows::Forms::Panel());
			this->gLightSetGroup = (gcnew System::Windows::Forms::GroupBox());
			this->btReset = (gcnew System::Windows::Forms::Button());
			this->btApp = (gcnew System::Windows::Forms::Button());
			this->rbtRAColor = (gcnew System::Windows::Forms::RadioButton());
			this->rbtRSColor = (gcnew System::Windows::Forms::RadioButton());
			this->btColorSet = (gcnew System::Windows::Forms::Button());
			this->rbtGAColor = (gcnew System::Windows::Forms::RadioButton());
			this->rbtGSColor = (gcnew System::Windows::Forms::RadioButton());
			this->ColorSetDlg = (gcnew System::Windows::Forms::ColorDialog());
			this->gLightSetGroup->SuspendLayout();
			this->SuspendLayout();
			// 
			// m_Light3DView
			// 
			this->m_Light3DView->BackColor = System::Drawing::Color::Gray;
			this->m_Light3DView->Dock = System::Windows::Forms::DockStyle::Top;
			this->m_Light3DView->Location = System::Drawing::Point(3, 17);
			this->m_Light3DView->Name = L"m_Light3DView";
			this->m_Light3DView->Size = System::Drawing::Size(264, 246);
			this->m_Light3DView->TabIndex = 0;
			this->m_Light3DView->MouseLeave += gcnew System::EventHandler(this, &CLightView::m_Light3DView_MouseLeave);
			this->m_Light3DView->MouseMove += gcnew System::Windows::Forms::MouseEventHandler(this, &CLightView::m_Light3DView_MouseMove);
			this->m_Light3DView->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &CLightView::m_Light3DView_MouseDown);
			this->m_Light3DView->MouseUp += gcnew System::Windows::Forms::MouseEventHandler(this, &CLightView::m_Light3DView_MouseUp);
			this->m_Light3DView->MouseEnter += gcnew System::EventHandler(this, &CLightView::m_Light3DView_MouseEnter);
			// 
			// gLightSetGroup
			// 
			this->gLightSetGroup->Controls->Add(this->btReset);
			this->gLightSetGroup->Controls->Add(this->btApp);
			this->gLightSetGroup->Controls->Add(this->rbtRAColor);
			this->gLightSetGroup->Controls->Add(this->rbtRSColor);
			this->gLightSetGroup->Controls->Add(this->btColorSet);
			this->gLightSetGroup->Controls->Add(this->rbtGAColor);
			this->gLightSetGroup->Controls->Add(this->rbtGSColor);
			this->gLightSetGroup->Controls->Add(this->m_Light3DView);
			this->gLightSetGroup->Location = System::Drawing::Point(3, 3);
			this->gLightSetGroup->Name = L"gLightSetGroup";
			this->gLightSetGroup->Size = System::Drawing::Size(270, 356);
			this->gLightSetGroup->TabIndex = 1;
			this->gLightSetGroup->TabStop = false;
			this->gLightSetGroup->Text = L"全局光参设置";
			// 
			// btReset
			// 
			this->btReset->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->btReset->Location = System::Drawing::Point(213, 327);
			this->btReset->Name = L"btReset";
			this->btReset->Size = System::Drawing::Size(47, 23);
			this->btReset->TabIndex = 5;
			this->btReset->Text = L"重置";
			this->btReset->UseVisualStyleBackColor = true;
			this->btReset->Click += gcnew System::EventHandler(this, &CLightView::btReset_Click);
			// 
			// btApp
			// 
			this->btApp->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->btApp->Location = System::Drawing::Point(158, 327);
			this->btApp->Name = L"btApp";
			this->btApp->Size = System::Drawing::Size(47, 23);
			this->btApp->TabIndex = 5;
			this->btApp->Text = L"应用";
			this->btApp->UseVisualStyleBackColor = true;
			this->btApp->Click += gcnew System::EventHandler(this, &CLightView::btApp_Click);
			// 
			// rbtRAColor
			// 
			this->rbtRAColor->AutoSize = true;
			this->rbtRAColor->Location = System::Drawing::Point(152, 305);
			this->rbtRAColor->Name = L"rbtRAColor";
			this->rbtRAColor->Size = System::Drawing::Size(83, 16);
			this->rbtRAColor->TabIndex = 4;
			this->rbtRAColor->TabStop = true;
			this->rbtRAColor->Text = L"角色环境光";
			this->rbtRAColor->UseVisualStyleBackColor = true;
			// 
			// rbtRSColor
			// 
			this->rbtRSColor->AutoSize = true;
			this->rbtRSColor->Location = System::Drawing::Point(152, 283);
			this->rbtRSColor->Name = L"rbtRSColor";
			this->rbtRSColor->Size = System::Drawing::Size(83, 16);
			this->rbtRSColor->TabIndex = 4;
			this->rbtRSColor->TabStop = true;
			this->rbtRSColor->Text = L"角色太阳光";
			this->rbtRSColor->UseVisualStyleBackColor = true;
			// 
			// btColorSet
			// 
			this->btColorSet->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->btColorSet->Location = System::Drawing::Point(30, 327);
			this->btColorSet->Name = L"btColorSet";
			this->btColorSet->Size = System::Drawing::Size(75, 23);
			this->btColorSet->TabIndex = 3;
			this->btColorSet->Text = L"颜色设置";
			this->btColorSet->UseVisualStyleBackColor = true;
			this->btColorSet->Click += gcnew System::EventHandler(this, &CLightView::btColorSet_Click);
			// 
			// rbtGAColor
			// 
			this->rbtGAColor->AutoSize = true;
			this->rbtGAColor->Location = System::Drawing::Point(30, 305);
			this->rbtGAColor->Name = L"rbtGAColor";
			this->rbtGAColor->Size = System::Drawing::Size(83, 16);
			this->rbtGAColor->TabIndex = 2;
			this->rbtGAColor->TabStop = true;
			this->rbtGAColor->Text = L"全局环境光";
			this->rbtGAColor->UseVisualStyleBackColor = true;
			// 
			// rbtGSColor
			// 
			this->rbtGSColor->AutoSize = true;
			this->rbtGSColor->Location = System::Drawing::Point(30, 283);
			this->rbtGSColor->Name = L"rbtGSColor";
			this->rbtGSColor->Size = System::Drawing::Size(83, 16);
			this->rbtGSColor->TabIndex = 1;
			this->rbtGSColor->TabStop = true;
			this->rbtGSColor->Text = L"全局太阳光";
			this->rbtGSColor->UseVisualStyleBackColor = true;
			// 
			// CLightView
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->Controls->Add(this->gLightSetGroup);
			this->Name = L"CLightView";
			this->Size = System::Drawing::Size(283, 366);
			this->gLightSetGroup->ResumeLayout(false);
			this->gLightSetGroup->PerformLayout();
			this->ResumeLayout(false);

		}
#pragma endregion
	public:	System::Void UpdateTimer();
	private: System::Void btColorSet_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void m_Light3DView_MouseMove(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e);
	private: System::Void m_Light3DView_MouseDown(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e);
	private: System::Void m_Light3DView_MouseUp(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e);
	private: System::Void m_Light3DView_MouseEnter(System::Object^  sender, System::EventArgs^  e);
	private: System::Void m_Light3DView_MouseLeave(System::Object^  sender, System::EventArgs^  e);
	private: System::Void m_Light3DView_MouseWheel(System::Object^  sender, System::EventArgs^  e);
	private: System::Void btApp_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void btReset_Click(System::Object^  sender, System::EventArgs^  e);
};
}
