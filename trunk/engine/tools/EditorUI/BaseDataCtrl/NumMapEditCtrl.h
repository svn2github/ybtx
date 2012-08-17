#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

#include "CUIPropBase.h"
namespace sqr_tools {

	/// <summary>
	/// Summary for NumMapEditCtrl
	/// </summary>
	public ref class NumMapEditCtrl : public CUIPropBase
	{
	public:
		NumMapEditCtrl(void)
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
		~NumMapEditCtrl()
		{
			if (components)
			{
				delete components;
			}
			DestroyOtherComponent();
		}

	private: System::Windows::Forms::Panel^  NumMapPanel;

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
			this->NumMapPanel = (gcnew System::Windows::Forms::Panel());
			this->SuspendLayout();
			// 
			// NumMapPanel
			// 
			this->NumMapPanel->BackColor = System::Drawing::SystemColors::Control;
			this->NumMapPanel->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->NumMapPanel->Location = System::Drawing::Point(6, 3);
			this->NumMapPanel->Name = L"NumMapPanel";
			this->NumMapPanel->Size = System::Drawing::Size(465, 105);
			this->NumMapPanel->TabIndex = 2;
			this->NumMapPanel->TabStop = true;
			this->NumMapPanel->MouseMove += gcnew System::Windows::Forms::MouseEventHandler(this, &NumMapEditCtrl::NumMapPanel_MouseMove);
			this->NumMapPanel->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &NumMapEditCtrl::NumMapPanel_MouseDown);
			// 
			// NumMapEditCtrl
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->Controls->Add(this->NumMapPanel);
			this->DoubleBuffered = true;
			this->Name = L"NumMapEditCtrl";
			this->Size = System::Drawing::Size(471, 114);
			this->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &NumMapEditCtrl::NumMapEditCtrl_KeyDown);
			this->ResumeLayout(false);

		}
#pragma endregion
	private:
		System::Drawing::Font^				m_pFont;
		System::Drawing::SolidBrush^		m_pBursh;
		System::Drawing::Pen^				m_pPen;

	public:  virtual char*	GetImpTypeName(void)	override;
			 virtual void	Update(void)	override;

	private: System::Void	InitOtherComponent();
	private: System::Void	DestroyOtherComponent();
	private: void			_DrawNumMapPanel();
	private: void			_DrawFloatString(Graphics^ Gra, int X, int Y, float val);
	private: void			_DrawRect(Graphics^ Gra, int X, int Y, int i);
	private: void			_GetXYPositon(int i, int& X, int& Y);

	private: float			_XToPercent(int X);
	private: float			_YToPercent(int Y);
	private: int			_PercentToX(float x);
	private: int			_PercentToY(float y);
	private: void			_SetCurXPercent(int X);
	private: void			_SetCurYPercent(int Y);


	private: System::Void NumMapPanel_MouseDown(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e);
	private: System::Void NumMapPanel_MouseMove(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e);
	private: System::Void NumMapEditCtrl_KeyDown(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e);
};
}
