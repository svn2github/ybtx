#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

#include "CUIBase.h"

namespace sqr_tools {

	ref class CEfxTrackCtrl;
	ref class AutoPropToCtrl;
	/// <summary>
	/// Summary for CEfxEditProp
	/// </summary>
	public ref class CEfxEditProp :  public CUIBase//public System::Windows::Forms::UserControl
	{
	public:
		CEfxEditProp(void)
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
		~CEfxEditProp()
		{
			if (components)
			{
				delete components;
			}
			DestroyOtherComponent();
		}
	private: System::Windows::Forms::Panel^  EfxEditPanel;
	private: System::Windows::Forms::Panel^  EfxInfoPanel;
	private: System::Windows::Forms::Label^  labelInfo;
	private: System::Windows::Forms::Label^  KeyNumInfo;
	private: System::Windows::Forms::Label^  KeySizeInfo;
	private: System::Windows::Forms::Panel^  KeyEditPanel;
	private: System::Windows::Forms::Label^  KeyEditLable;
	private: System::Windows::Forms::TrackBar^  ScaletrackBar;

	private: System::Windows::Forms::Label^  TimeSetLabel;
	private: System::Windows::Forms::Label^  ScaleLabel;
	private: System::Windows::Forms::TextBox^  TimeSetTextBox;
	private: System::Windows::Forms::Button^  SetTimeButton;

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
			this->EfxEditPanel = (gcnew System::Windows::Forms::Panel());
			this->EfxInfoPanel = (gcnew System::Windows::Forms::Panel());
			this->labelInfo = (gcnew System::Windows::Forms::Label());
			this->KeyNumInfo = (gcnew System::Windows::Forms::Label());
			this->KeySizeInfo = (gcnew System::Windows::Forms::Label());
			this->KeyEditPanel = (gcnew System::Windows::Forms::Panel());
			this->KeyEditLable = (gcnew System::Windows::Forms::Label());
			this->ScaleLabel = (gcnew System::Windows::Forms::Label());
			this->TimeSetLabel = (gcnew System::Windows::Forms::Label());
			this->ScaletrackBar = (gcnew System::Windows::Forms::TrackBar());
			this->TimeSetTextBox = (gcnew System::Windows::Forms::TextBox());
			this->SetTimeButton = (gcnew System::Windows::Forms::Button());
			this->EfxInfoPanel->SuspendLayout();
			this->KeyEditPanel->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->ScaletrackBar))->BeginInit();
			this->SuspendLayout();
			// 
			// EfxEditPanel
			// 
			this->EfxEditPanel->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->EfxEditPanel->AutoScroll = true;
			this->EfxEditPanel->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->EfxEditPanel->Location = System::Drawing::Point(0, 0);
			this->EfxEditPanel->Name = L"EfxEditPanel";
			this->EfxEditPanel->Size = System::Drawing::Size(500, 651);
			this->EfxEditPanel->TabIndex = 0;
			// 
			// EfxInfoPanel
			// 
			this->EfxInfoPanel->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->EfxInfoPanel->BackColor = System::Drawing::Color::LightGray;
			this->EfxInfoPanel->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->EfxInfoPanel->Controls->Add(this->KeySizeInfo);
			this->EfxInfoPanel->Controls->Add(this->KeyNumInfo);
			this->EfxInfoPanel->Controls->Add(this->labelInfo);
			this->EfxInfoPanel->Location = System::Drawing::Point(0, 653);
			this->EfxInfoPanel->Name = L"EfxInfoPanel";
			this->EfxInfoPanel->Size = System::Drawing::Size(500, 25);
			this->EfxInfoPanel->TabIndex = 1;
			// 
			// labelInfo
			// 
			this->labelInfo->AutoSize = true;
			this->labelInfo->ForeColor = System::Drawing::Color::DarkSlateBlue;
			this->labelInfo->Location = System::Drawing::Point(3, 6);
			this->labelInfo->Name = L"labelInfo";
			this->labelInfo->Size = System::Drawing::Size(77, 12);
			this->labelInfo->TabIndex = 0;
			this->labelInfo->Text = L"关键帧信息：";
			// 
			// KeyNumInfo
			// 
			this->KeyNumInfo->AutoSize = true;
			this->KeyNumInfo->Location = System::Drawing::Point(86, 6);
			this->KeyNumInfo->Name = L"KeyNumInfo";
			this->KeyNumInfo->Size = System::Drawing::Size(41, 12);
			this->KeyNumInfo->TabIndex = 1;
			this->KeyNumInfo->Text = L"帧 0~0";
			// 
			// KeySizeInfo
			// 
			this->KeySizeInfo->AutoSize = true;
			this->KeySizeInfo->Location = System::Drawing::Point(228, 6);
			this->KeySizeInfo->Name = L"KeySizeInfo";
			this->KeySizeInfo->Size = System::Drawing::Size(47, 12);
			this->KeySizeInfo->TabIndex = 2;
			this->KeySizeInfo->Text = L"每格0帧";
			// 
			// KeyEditPanel
			// 
			this->KeyEditPanel->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->KeyEditPanel->BackColor = System::Drawing::Color::LightGray;
			this->KeyEditPanel->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->KeyEditPanel->Controls->Add(this->SetTimeButton);
			this->KeyEditPanel->Controls->Add(this->TimeSetTextBox);
			this->KeyEditPanel->Controls->Add(this->ScaletrackBar);
			this->KeyEditPanel->Controls->Add(this->TimeSetLabel);
			this->KeyEditPanel->Controls->Add(this->ScaleLabel);
			this->KeyEditPanel->Controls->Add(this->KeyEditLable);
			this->KeyEditPanel->Location = System::Drawing::Point(0, 679);
			this->KeyEditPanel->Name = L"KeyEditPanel";
			this->KeyEditPanel->Size = System::Drawing::Size(500, 45);
			this->KeyEditPanel->TabIndex = 2;
			// 
			// KeyEditLable
			// 
			this->KeyEditLable->AutoSize = true;
			this->KeyEditLable->ForeColor = System::Drawing::Color::DarkSlateBlue;
			this->KeyEditLable->Location = System::Drawing::Point(3, 9);
			this->KeyEditLable->Name = L"KeyEditLable";
			this->KeyEditLable->Size = System::Drawing::Size(65, 12);
			this->KeyEditLable->TabIndex = 0;
			this->KeyEditLable->Text = L"轨道设置：";
			// 
			// ScaleLabel
			// 
			this->ScaleLabel->AutoSize = true;
			this->ScaleLabel->ForeColor = System::Drawing::Color::Blue;
			this->ScaleLabel->Location = System::Drawing::Point(86, 10);
			this->ScaleLabel->Name = L"ScaleLabel";
			this->ScaleLabel->Size = System::Drawing::Size(41, 12);
			this->ScaleLabel->TabIndex = 1;
			this->ScaleLabel->Text = L"缩放：";
			// 
			// TimeSetLabel
			// 
			this->TimeSetLabel->AutoSize = true;
			this->TimeSetLabel->ForeColor = System::Drawing::Color::Blue;
			this->TimeSetLabel->Location = System::Drawing::Point(269, 12);
			this->TimeSetLabel->Name = L"TimeSetLabel";
			this->TimeSetLabel->Size = System::Drawing::Size(41, 12);
			this->TimeSetLabel->TabIndex = 2;
			this->TimeSetLabel->Text = L"时间：";
			// 
			// ScaletrackBar
			// 
			this->ScaletrackBar->AutoSize = false;
			this->ScaletrackBar->LargeChange = 10;
			this->ScaletrackBar->Location = System::Drawing::Point(129, 4);
			this->ScaletrackBar->Minimum = -10;
			this->ScaletrackBar->Name = L"ScaletrackBar";
			this->ScaletrackBar->Size = System::Drawing::Size(137, 25);
			this->ScaletrackBar->TabIndex = 3;
			// 
			// TimeSetTextBox
			// 
			this->TimeSetTextBox->Location = System::Drawing::Point(313, 7);
			this->TimeSetTextBox->Name = L"TimeSetTextBox";
			this->TimeSetTextBox->Size = System::Drawing::Size(100, 21);
			this->TimeSetTextBox->TabIndex = 4;
			this->TimeSetTextBox->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &CEfxEditProp::TimeSetTextBox_KeyPress);
			// 
			// SetTimeButton
			// 
			this->SetTimeButton->Location = System::Drawing::Point(420, 6);
			this->SetTimeButton->Name = L"SetTimeButton";
			this->SetTimeButton->Size = System::Drawing::Size(52, 23);
			this->SetTimeButton->TabIndex = 5;
			this->SetTimeButton->Text = L"设 置";
			this->SetTimeButton->UseVisualStyleBackColor = true;
			// 
			// CEfxEditProp
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->Controls->Add(this->KeyEditPanel);
			this->Controls->Add(this->EfxInfoPanel);
			this->Controls->Add(this->EfxEditPanel);
			this->Name = L"CEfxEditProp";
			this->Size = System::Drawing::Size(500, 802);
			this->EfxInfoPanel->ResumeLayout(false);
			this->EfxInfoPanel->PerformLayout();
			this->KeyEditPanel->ResumeLayout(false);
			this->KeyEditPanel->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->ScaletrackBar))->EndInit();
			this->ResumeLayout(false);

		}
#pragma endregion
	private: 
		System::Collections::Generic::List<CEfxTrackCtrl^>^	m_TrackCtrls;
		AutoPropToCtrl^		m_AutoPropToCtrl;
		float				m_fScale;

	private:	System::Void	InitOtherComponent();
	private:	System::Void	DestroyOtherComponent();

	public:		virtual char*	GetImpTypeName(void)	override;
				virtual void	Update(void)	override;

	public:		virtual float	GetCurScale();
	public:		virtual void	UpdateTracksBG(bool ctrl);
	public:		virtual void	UpdateTrackContent(void);
	public:		virtual void	UpdateTrackPosAndScale(void);
	public:		virtual void	ClearTracks(void);
	public:		virtual void	ReSetTracks(void);
	public:		virtual void	OnKeyUintEdit(void* content);
	public:		virtual void	OnBackTrack();


	private:	System::Void TimeSetTextBox_KeyPress(System::Object^  sender, System::Windows::Forms::KeyPressEventArgs^  e);
};
}
