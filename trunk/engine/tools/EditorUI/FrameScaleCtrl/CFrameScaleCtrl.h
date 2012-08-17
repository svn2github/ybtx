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
	/// Summary for CFrameScaleCtrl
	/// </summary>
	public ref class CFrameScaleCtrl : public CUIBase
	{
	public:
		CFrameScaleCtrl(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//

			this->SetStyle(System::Windows::Forms::ControlStyles::OptimizedDoubleBuffer
				|System::Windows::Forms::ControlStyles::AllPaintingInWmPaint
				|System::Windows::Forms::ControlStyles::UserPaint, true);
			this->UpdateStyles();

			this->InitDrawEnv();
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~CFrameScaleCtrl()
		{
			if (components)
			{
				this->ReleaseRes();

				delete components;
			}
		}

	protected: 


	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^		components;
		System::Drawing::SolidBrush^			pBursh;
		System::Drawing::Pen^					pPen;
		System::Drawing::Font^					pFont;





			 System::Drawing::Bitmap^			pScaleBitMap;
	private: System::Windows::Forms::Button^  move_button;
	private: System::Windows::Forms::Label^  scale_label;
	private: System::Windows::Forms::Button^  StopBtn;

	private: System::Windows::Forms::Button^  PlayBtn;


			 bool								bLButtonDown;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->move_button = (gcnew System::Windows::Forms::Button());
			this->scale_label = (gcnew System::Windows::Forms::Label());
			this->StopBtn = (gcnew System::Windows::Forms::Button());
			this->PlayBtn = (gcnew System::Windows::Forms::Button());
			this->SuspendLayout();
			// 
			// move_button
			// 
			this->move_button->BackColor = System::Drawing::SystemColors::ControlDark;
			this->move_button->Enabled = false;
			this->move_button->Font = (gcnew System::Drawing::Font(L"SimSun", 8));
			this->move_button->Location = System::Drawing::Point(1, 1);
			this->move_button->Name = L"move_button";
			this->move_button->Size = System::Drawing::Size(100, 16);
			this->move_button->TabIndex = 2;
			this->move_button->UseVisualStyleBackColor = false;
			// 
			// scale_label
			// 
			this->scale_label->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->scale_label->Location = System::Drawing::Point(0, 1);
			this->scale_label->Name = L"scale_label";
			this->scale_label->Size = System::Drawing::Size(475, 35);
			this->scale_label->TabIndex = 0;
			this->scale_label->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &CFrameScaleCtrl::scale_label_MouseDown);
			this->scale_label->MouseMove += gcnew System::Windows::Forms::MouseEventHandler(this, &CFrameScaleCtrl::scale_label_MouseMove);
			this->scale_label->MouseUp += gcnew System::Windows::Forms::MouseEventHandler(this, &CFrameScaleCtrl::scale_label_MouseUp);
			// 
			// StopBtn
			// 
			this->StopBtn->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->StopBtn->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->StopBtn->Location = System::Drawing::Point(514, 1);
			this->StopBtn->Name = L"StopBtn";
			this->StopBtn->Size = System::Drawing::Size(42, 35);
			this->StopBtn->TabIndex = 3;
			this->StopBtn->Text = L"Í£Ö¹";
			this->StopBtn->UseVisualStyleBackColor = true;
			this->StopBtn->Click += gcnew System::EventHandler(this, &CFrameScaleCtrl::StopBtn_Click);
			// 
			// PlayBtn
			// 
			this->PlayBtn->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->PlayBtn->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->PlayBtn->Font = (gcnew System::Drawing::Font(L"SimSun", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(134)));
			this->PlayBtn->ImageAlign = System::Drawing::ContentAlignment::TopCenter;
			this->PlayBtn->Location = System::Drawing::Point(473, 1);
			this->PlayBtn->Name = L"PlayBtn";
			this->PlayBtn->Size = System::Drawing::Size(42, 35);
			this->PlayBtn->TabIndex = 3;
			this->PlayBtn->Text = L"²¥·Å";
			this->PlayBtn->UseVisualStyleBackColor = true;
			this->PlayBtn->Click += gcnew System::EventHandler(this, &CFrameScaleCtrl::PlayBtn_Click);
			// 
			// CFrameScaleCtrl
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->Controls->Add(this->PlayBtn);
			this->Controls->Add(this->StopBtn);
			this->Controls->Add(this->move_button);
			this->Controls->Add(this->scale_label);
			this->Margin = System::Windows::Forms::Padding(0);
			this->Name = L"CFrameScaleCtrl";
			this->Size = System::Drawing::Size(561, 40);
			this->Load += gcnew System::EventHandler(this, &CFrameScaleCtrl::CFrameScaleCtrl_Load);
			this->Resize += gcnew System::EventHandler(this, &CFrameScaleCtrl::CFrameScaleCtrl_Resize);
			this->ResumeLayout(false);

		}
#pragma endregion

		System::Void scale_label_MouseDown(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e);
		System::Void scale_label_MouseUp(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e);
		System::Void scale_label_MouseMove(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e);

		void InitDrawEnv();
		void ReleaseRes();
		void DrawFrameScale();
	
		public:  virtual void Update(void) override;
		public:  virtual char*	GetImpTypeName(void)	override;
		private: System::Void CFrameScaleCtrl_Load(System::Object^  sender, System::EventArgs^  e);
		private: System::Void PlayBtn_Click(System::Object^  sender, System::EventArgs^  e);
		private: System::Void StopBtn_Click(System::Object^  sender, System::EventArgs^  e);
		private: System::Void CFrameScaleCtrl_Resize(System::Object^  sender, System::EventArgs^  e);
};
}
