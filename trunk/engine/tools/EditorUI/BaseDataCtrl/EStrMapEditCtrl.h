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
	/// Summary for EStrMapEditCtrl
	/// </summary>
	public ref class EStrMapEditCtrl : public CUIPropBase
	{
	public:
		EStrMapEditCtrl(void)
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
		~EStrMapEditCtrl()
		{
			if (components)
			{
				delete components;
			}
			DestroyOtherComponent();
		}

	protected: 
	private: System::Windows::Forms::Panel^  TextureMapPanel;
	private: System::Windows::Forms::OpenFileDialog^  OpenTexFileDialog;

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
			this->TextureMapPanel = (gcnew System::Windows::Forms::Panel());
			this->OpenTexFileDialog = (gcnew System::Windows::Forms::OpenFileDialog());
			this->SuspendLayout();
			// 
			// TextureMapPanel
			// 
			this->TextureMapPanel->BackColor = System::Drawing::Color::DarkSeaGreen;
			this->TextureMapPanel->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->TextureMapPanel->Cursor = System::Windows::Forms::Cursors::Default;
			this->TextureMapPanel->Location = System::Drawing::Point(0, 2);
			this->TextureMapPanel->Name = L"TextureMapPanel";
			this->TextureMapPanel->Size = System::Drawing::Size(468, 28);
			this->TextureMapPanel->TabIndex = 2;
			this->TextureMapPanel->MouseMove += gcnew System::Windows::Forms::MouseEventHandler(this, &EStrMapEditCtrl::TextureMapPanel_MouseMove);
			this->TextureMapPanel->MouseDoubleClick += gcnew System::Windows::Forms::MouseEventHandler(this, &EStrMapEditCtrl::TextureMapPanel_MouseDoubleClick);
			this->TextureMapPanel->MouseClick += gcnew System::Windows::Forms::MouseEventHandler(this, &EStrMapEditCtrl::TextureMapPanel_MouseClick);
			this->TextureMapPanel->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &EStrMapEditCtrl::TextureMapPanel_MouseDown);
			// 
			// OpenTexFileDialog
			// 
			this->OpenTexFileDialog->Filter = L"Texture Files (*.tex¡¢*.dds)|*.tex;*.dds||";
			this->OpenTexFileDialog->Multiselect = true;
			// 
			// EStrMapEditCtrl
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->Controls->Add(this->TextureMapPanel);
			this->Name = L"EStrMapEditCtrl";
			this->Size = System::Drawing::Size(471, 33);
			this->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &EStrMapEditCtrl::EStrMapEditCtrl_KeyDown);
			this->ResumeLayout(false);

		}
#pragma endregion
		System::Drawing::Font^				m_pFont;
		System::Drawing::SolidBrush^		m_pBursh;
		System::Drawing::Pen^				m_pPen;
		bool								m_bReSize;

	public:  virtual char*	GetImpTypeName(void)	override;
			 virtual void	Update(void)	override;

	private: System::Void	InitOtherComponent();
	private: System::Void	DestroyOtherComponent();

	private: void			_SetCurXPercent(int X);
	private: float			_GetCurSelectNode(float per);
	private: float			_XToPercent(int X);
	private: int			_PercentToX(float x);
	private: void			_DrawEStrMapPanel();
	private: void			_GetXPositon(int i, int& X );
	private: void			_DrawRectInfo(Graphics^ Gra,int X1, int X2, int n);

	private: System::Void TextureMapPanel_MouseDown(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e);
	private: System::Void TextureMapPanel_MouseMove(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e);
	private: System::Void EStrMapEditCtrl_KeyDown(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e);
	private: System::Void TextureMapPanel_MouseClick(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e);
	private: System::Void TextureMapPanel_MouseDoubleClick(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e);
	};
}
