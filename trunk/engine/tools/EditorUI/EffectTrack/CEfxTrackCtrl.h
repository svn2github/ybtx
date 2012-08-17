#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

#include "CUIBase.h"
// ²Ù×÷ÏÔÊ¾Âß¼­
#define TRACKCTRL_STATE_NORMAL		0x0000
#define TRACKCTRL_STATE_HIDE		0x0001
#define TRACKCTRL_STATE_SELPRE		0x0010
#define TRACKCTRL_STATE_SEL			0x0100
#define TRACKCTRL_STATE_SELS		0x1000


namespace sqr_tools {
	ref class CEfxKeyUintCtrl;
	/// <summary>
	/// Summary for CEfxTrackCtrl
	/// </summary>
	public ref class CEfxTrackCtrl :  public CUIBase//public System::Windows::Forms::UserControl
	{
	public:
		CEfxTrackCtrl(void)
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
		~CEfxTrackCtrl()
		{
			if (components)
			{
				delete components;
			}
			DestroyOtherComponent();
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
			this->SuspendLayout();
			// 
			// CEfxTrackCtrl
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->BackColor = System::Drawing::Color::LightSteelBlue;
			this->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->Name = L"CEfxTrackCtrl";
			this->Size = System::Drawing::Size(521, 45);
			this->MouseDoubleClick += gcnew System::Windows::Forms::MouseEventHandler(this, &CEfxTrackCtrl::CEfxTrackCtrl_MouseDoubleClick);
			this->MouseClick += gcnew System::Windows::Forms::MouseEventHandler(this, &CEfxTrackCtrl::CEfxTrackCtrl_MouseClick);
			this->ResumeLayout(false);

		}
#pragma endregion
	private:
		System::Collections::Generic::List<CEfxKeyUintCtrl^>^	m_KeyCtrls;
		int				m_uCtrlState;

	private: System::Void	InitOtherComponent();
	private: System::Void	DestroyOtherComponent();

	public:  virtual char*	GetImpTypeName(void)	override;
	public:  virtual void	UpdateTrackInfo(void);
	public:  virtual int	GetTrackState()	{ return m_uCtrlState; }; 

	public:  virtual void	SetTrackContent(void* content, void* editprop);
	public:  virtual void	ClearKeys();
	public:  virtual void	RefreshTrack();
	public:  virtual void	RefreshTrackPos();
	public:  virtual void	RefreshTrackState(bool ctrl);
	public:  virtual void	RefreshTrackColor();

	public:  virtual void	OnTrackSelected();
	public:  virtual void	OnTrackSelectedLots();
	public:  virtual void	OnKeyUintEdit(void* content);

	private: System::Void CEfxTrackCtrl_MouseClick(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e);
	private: System::Void CEfxTrackCtrl_MouseDoubleClick(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e);
	};
}
