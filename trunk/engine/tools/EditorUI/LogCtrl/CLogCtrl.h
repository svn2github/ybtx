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
	/// Summary for CLogCtrl
	/// </summary>
	public enum struct ELogType
	{
		PUBLIC_LOG	= 0,
		PRIVATE_LOG = 1,
	};
	public ref class CLogCtrl : public CUIBase
	{
	public:
		CLogCtrl(void)
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

		~CLogCtrl()
		{
			if (components)
			{
				delete components;
			}
		}


	public:	 virtual char*	GetImpTypeName(void) override;
	public:	 virtual void	Update(void) override;
	private: System::Windows::Forms::RichTextBox^  LogCtrlText;
	private: ELogType m_LogType;
		
	public:
		[Category("扩展属性"),Description("选择Log控件是全局或私有"),TypeConverter(EnumConverter::typeid)]
	property ELogType LogType
	{
		ELogType get()			{ return m_LogType;  }
		void set(ELogType val)	{ m_LogType = val; }
	}

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
			this->LogCtrlText = (gcnew System::Windows::Forms::RichTextBox());
			this->SuspendLayout();
			// 
			// LogCtrlText
			// 
			this->LogCtrlText->Dock = System::Windows::Forms::DockStyle::Fill;
			this->LogCtrlText->Location = System::Drawing::Point(0, 0);
			this->LogCtrlText->Name = L"LogCtrlText";
			this->LogCtrlText->ReadOnly = true;
			this->LogCtrlText->ScrollBars = System::Windows::Forms::RichTextBoxScrollBars::ForcedBoth;
			this->LogCtrlText->Size = System::Drawing::Size(848, 135);
			this->LogCtrlText->TabIndex = 0;
			this->LogCtrlText->Text = L"";
			// 
			// CLogCtrl
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->Controls->Add(this->LogCtrlText);
			this->Name = L"CLogCtrl";
			this->Size = System::Drawing::Size(848, 135);
			this->ResumeLayout(false);

		}
#pragma endregion
	};
}
