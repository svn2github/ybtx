#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

#include "CUIPropBase.h"
#include "Vector3EditDialog.h"

namespace sqr_tools {
	/// <summary>
	/// Summary for Vector3MapCtrl
	/// </summary>
	public ref class Vector3MapCtrl : public CUIPropBase
	{
	public:
		Vector3MapCtrl(void)
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
		~Vector3MapCtrl()
		{
			if (components)
			{
				delete components;
			}
			DestroyOtherComponent();
		}

	private: System::Windows::Forms::Panel^  Vector3Mappanel;
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
			this->Vector3Mappanel = (gcnew System::Windows::Forms::Panel());
			this->SuspendLayout();
			// 
			// Vector3Mappanel
			// 
			this->Vector3Mappanel->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->Vector3Mappanel->Location = System::Drawing::Point(4, 3);
			this->Vector3Mappanel->Name = L"Vector3Mappanel";
			this->Vector3Mappanel->Size = System::Drawing::Size(464, 56);
			this->Vector3Mappanel->TabIndex = 3;
			this->Vector3Mappanel->MouseMove += gcnew System::Windows::Forms::MouseEventHandler(this, &Vector3MapCtrl::Vector3Mappanel_MouseMove);
			this->Vector3Mappanel->MouseDoubleClick += gcnew System::Windows::Forms::MouseEventHandler(this, &Vector3MapCtrl::Vector3Mappanel_MouseDoubleClick);
			this->Vector3Mappanel->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &Vector3MapCtrl::Vector3Mappanel_MouseDown);
			// 
			// Vector3MapCtrl
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->Controls->Add(this->Vector3Mappanel);
			this->Name = L"Vector3MapCtrl";
			this->Size = System::Drawing::Size(471, 64);
			this->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &Vector3MapCtrl::Vector3MapCtrl_KeyDown);
			this->ResumeLayout(false);

		}
#pragma endregion
	private:
		System::Drawing::Font^				m_pFont;
		System::Drawing::SolidBrush^		m_pBursh;
		System::Drawing::Pen^				m_pPen;
		Vector3EditDialog					m_Vec3Dlg;

	public:  virtual char*	GetImpTypeName(void)	override;
			 virtual void	Update(void)	override;

	private: System::Void	InitOtherComponent();
	private: System::Void	DestroyOtherComponent();

	private: void			_DrawVecMapPanel();
	private: void			_SetCurXPercent(int X);
	private: float			_XToPercent(int X);
	private: int			_PercentToX(float x);
	private: void			_GetXPositon(int i, int& X );
	private: void			_DrawRectInfo(Graphics^ Gra,int X, int n);
	private: bool			_GetCurPosNode(float per, float exper, float& findper);

	private: System::Void Vector3MapCtrl_KeyDown(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e);
	private: System::Void Vector3Mappanel_MouseDown(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e);
	private: System::Void Vector3Mappanel_MouseMove(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e);
	private: System::Void Vector3Mappanel_MouseDoubleClick(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e);
	};
}
