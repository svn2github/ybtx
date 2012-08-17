#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

#include "CUIBase.h"

// ²Ù×÷ÏÔÊ¾Âß¼­
#define KEY_STATE_NORMAL		0x0000
#define KEY_STATE_HIDE			0x0001
#define KEY_STATE_SELPRE		0x0010
#define KEY_STATE_SEL			0x0100
#define KEY_STATE_SELS			0x1000

namespace sqr_tools {

	/// <summary>
	/// Summary for CEfxKeyUintCtrl
	/// </summary>
	public ref class CEfxKeyUintCtrl :  public CUIBase//public System::Windows::Forms::UserControl
	{
	public:
		CEfxKeyUintCtrl(void)
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
		~CEfxKeyUintCtrl()
		{
			if (components)
			{
				delete components;
			}
			DestroyOtherComponent();
		}
	private: System::Windows::Forms::Label^  KeyNameLable;
	private: System::Windows::Forms::ToolTip^  CKeytoolTip;
	private: System::ComponentModel::IContainer^  components;
	protected: 

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>


#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			this->KeyNameLable = (gcnew System::Windows::Forms::Label());
			this->CKeytoolTip = (gcnew System::Windows::Forms::ToolTip(this->components));
			this->SuspendLayout();
			// 
			// KeyNameLable
			// 
			this->KeyNameLable->AutoSize = true;
			this->KeyNameLable->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->KeyNameLable->Location = System::Drawing::Point(4, 12);
			this->KeyNameLable->Name = L"KeyNameLable";
			this->KeyNameLable->Size = System::Drawing::Size(0, 12);
			this->KeyNameLable->TabIndex = 0;
			this->KeyNameLable->MouseDoubleClick += gcnew System::Windows::Forms::MouseEventHandler(this, &CEfxKeyUintCtrl::KeyNameLable_MouseDoubleClick);
			this->KeyNameLable->MouseClick += gcnew System::Windows::Forms::MouseEventHandler(this, &CEfxKeyUintCtrl::KeyNameLable_MouseClick);
			// 
			// CEfxKeyUintCtrl
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->BackColor = System::Drawing::Color::Chartreuse;
			this->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->Controls->Add(this->KeyNameLable);
			this->Name = L"CEfxKeyUintCtrl";
			this->Size = System::Drawing::Size(350, 42);
			this->MouseDoubleClick += gcnew System::Windows::Forms::MouseEventHandler(this, &CEfxKeyUintCtrl::CEfxKeyUintCtrl_MouseDoubleClick);
			this->MouseClick += gcnew System::Windows::Forms::MouseEventHandler(this, &CEfxKeyUintCtrl::CEfxKeyUintCtrl_MouseClick);
			this->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &CEfxKeyUintCtrl::CEfxKeyUintCtrl_KeyPress);
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	private:
		int				m_uCtrlState;

		private: System::Void	InitOtherComponent();
		private: System::Void	DestroyOtherComponent();

		public:  virtual char*	GetImpTypeName(void)	override;
		public:	 virtual void	SetKeyContent(void* content);
		public:	 virtual void	InitKeyName();

		public:	 virtual void	RefreshKeyState();
		public:  virtual void	OnKeySelected();
		public:  virtual void	OnKeySelectedLots();
		public:  virtual void	OnKeyUintEdit();


	private: System::Void CEfxKeyUintCtrl_MouseClick(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e);
	private: System::Void CEfxKeyUintCtrl_MouseDoubleClick(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e); 
	private: System::Void KeyNameLable_MouseClick(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e);
	private: System::Void KeyNameLable_MouseDoubleClick(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e);
	private: System::Void CEfxKeyUintCtrl_KeyPress(System::Object^  sender, System::Windows::Forms::KeyPressEventArgs^  e);
	};
}
