#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

#include "CUIBase.h"

namespace sqr_tools {
	ref class CueListDialog;
	enum ECtrlDataType;
	ref class AutoDataUintCtrl;
	ref class CUIPropBase;
	/// <summary>
	/// Summary for AutoPropToCtrl
	/// </summary>
	public ref class AutoPropToCtrl 
		:  public CUIBase//public System::Windows::Forms::UserControl
	{
	public:
		AutoPropToCtrl(void)
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
		~AutoPropToCtrl()
		{
			if (components)
			{
				delete components;
			}
			DestroyOtherComponent();
		}
	private: System::Windows::Forms::Button^  FourceButton;
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
			this->FourceButton = (gcnew System::Windows::Forms::Button());
			this->SuspendLayout();
			// 
			// FourceButton
			// 
			this->FourceButton->Location = System::Drawing::Point(96, 43);
			this->FourceButton->Name = L"FourceButton";
			this->FourceButton->Size = System::Drawing::Size(0, 0);
			this->FourceButton->TabIndex = 0;
			this->FourceButton->UseVisualStyleBackColor = true;
			this->FourceButton->KeyUp += gcnew System::Windows::Forms::KeyEventHandler(this, &AutoPropToCtrl::FourceButton_KeyUp);
			this->FourceButton->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &AutoPropToCtrl::FourceButton_KeyDown);
			// 
			// AutoPropToCtrl
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->AutoScroll = true;
			this->CausesValidation = false;
			this->Controls->Add(this->FourceButton);
			this->Name = L"AutoPropToCtrl";
			this->Size = System::Drawing::Size(464, 150);
			this->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &AutoPropToCtrl::AutoPropToCtrl_MouseDown);
			this->ResumeLayout(false);

		}
#pragma endregion
		private: sqr_tools::CueListDialog^ m_CueListDlg;
		private: System::Collections::Generic::List<AutoDataUintCtrl^>^	m_KeyPropUint;
		private: System::Collections::Generic::List<CUIPropBase^>^		m_PropUintBase;
		
		private: System::Void	InitOtherComponent();
		private: System::Void	DestroyOtherComponent();
		private:  void			_CreatPropUintUI(int tp, void* ct, const char* title);
		private:  void			_RefReshPropUintPos();

		public:  virtual char*	GetImpTypeName(void)		override;
		public: virtual void	Update(void)				override;
		public:  virtual void	SetPropContent(void* content);
		public:  virtual void	ClearKeyPropUint();
		public:  virtual void	UpdateKeyPropUint();
		
	private: System::Void AutoPropToCtrl_MouseDown(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e);
	private: System::Void FourceButton_KeyDown(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e);
	private: System::Void FourceButton_KeyUp(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e);
	};
}
