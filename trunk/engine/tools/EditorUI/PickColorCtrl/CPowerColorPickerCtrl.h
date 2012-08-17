#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


#include "CUIBase.h"

namespace sqr_tools {

	#define DEFAULT_WIDTH_OF_LUMINANCE_BAR	20
	#define DEFAULT_HEIGHT_OF_FINAL_COLOR	20
	
	/// <summary>
	/// Summary for CPowerColorPickerCtrl
	/// </summary>
	public ref class CPowerColorPickerCtrl : public CUIBase
	{
	public:
		CPowerColorPickerCtrl(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
 			this->SetStyle(System::Windows::Forms::ControlStyles::OptimizedDoubleBuffer
 				|System::Windows::Forms::ControlStyles::AllPaintingInWmPaint
 				|System::Windows::Forms::ControlStyles::UserPaint, true);
 			
 			this->UpdateStyles();

			System::Drawing::Size   sizeLable1 = this->colorPicker_label->Size;

			this->luminance_label->Location.X   = this->colorPicker_label->Location.X + this->colorPicker_label->Width;
			this->luminance_label->Location.Y   = this->colorPicker_label->Location.Y;
			this->luminance_label->Height	    = this->colorPicker_label->Height;
			this->luminance_label->Width		= DEFAULT_WIDTH_OF_LUMINANCE_BAR;
			
			this->finaleColor_label->Location.X = this->colorPicker_label->Location.X;
			this->finaleColor_label->Location.Y = this->colorPicker_label->Height;
			this->finaleColor_label->Height	    = DEFAULT_HEIGHT_OF_FINAL_COLOR;
			this->finaleColor_label->Width		= this->colorPicker_label->Width + DEFAULT_WIDTH_OF_LUMINANCE_BAR;

			this->currentPickColor  = System::Drawing::Color::Black;
			this->currentHue		= 0.0;
			this->currentLuminance  = 0.0;
			this->currentSaturation = 0.0;

			this->bPickerColorButtonDown = false;
			this->bLuminanceButtonDown	 = false;

			pBrush = gcnew System::Drawing::SolidBrush(Color::FromArgb(0xffffffff));
			pPen = gcnew System::Drawing::Pen(Color::FromArgb(0xffffffff), 1.0f);
			
			this->GenerateColorPicker();
			this->GenerateLuminanceBar();
			this->UpdateFinaleColorBar();
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~CPowerColorPickerCtrl()
		{
			if (components)
			{
				delete components;
			}
		}

	protected: 


	protected: Drawing::Color currentPickColor;
	protected: double		  currentLuminance, currentSaturation, currentHue;
	protected: Point		  curColorPickerPoint;
	protected: bool			  bPickerColorButtonDown;	
	protected: Point		  curLuminancePoint;
    protected: bool			  bLuminanceButtonDown;	

	protected: System::Drawing::SolidBrush^ pBrush;
	protected :System::Drawing::Pen^ pPen;
	private: System::Windows::Forms::Label^  finaleColor_label;
	protected: 

	private: System::Windows::Forms::Label^  luminance_label;
	protected: 

	private: System::Windows::Forms::Label^  colorPicker_label;


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
			this->finaleColor_label = (gcnew System::Windows::Forms::Label());
			this->luminance_label = (gcnew System::Windows::Forms::Label());
			this->colorPicker_label = (gcnew System::Windows::Forms::Label());
			this->SuspendLayout();
			// 
			// finaleColor_label
			// 
			this->finaleColor_label->BackColor = System::Drawing::SystemColors::Info;
			this->finaleColor_label->Location = System::Drawing::Point(4, 208);
			this->finaleColor_label->Name = L"finaleColor_label";
			this->finaleColor_label->Size = System::Drawing::Size(220, 20);
			this->finaleColor_label->TabIndex = 5;
			// 
			// luminance_label
			// 
			this->luminance_label->BackColor = System::Drawing::SystemColors::ControlLightLight;
			this->luminance_label->Cursor = System::Windows::Forms::Cursors::Cross;
			this->luminance_label->Location = System::Drawing::Point(204, 4);
			this->luminance_label->Name = L"luminance_label";
			this->luminance_label->Size = System::Drawing::Size(20, 204);
			this->luminance_label->TabIndex = 4;
			this->luminance_label->Paint += gcnew System::Windows::Forms::PaintEventHandler(this, &CPowerColorPickerCtrl::luminance_label_Paint);
			this->luminance_label->MouseMove += gcnew System::Windows::Forms::MouseEventHandler(this, &CPowerColorPickerCtrl::luminanceBar_MouseMove);
			this->luminance_label->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &CPowerColorPickerCtrl::luminanceBar_MouseDown);
			this->luminance_label->MouseUp += gcnew System::Windows::Forms::MouseEventHandler(this, &CPowerColorPickerCtrl::luminanceBar_MouseUp);

			// 
			// colorPicker_label
			// 
			this->colorPicker_label->BackColor = System::Drawing::SystemColors::ActiveCaption;
			this->colorPicker_label->Cursor = System::Windows::Forms::Cursors::NoMove2D;
			this->colorPicker_label->Location = System::Drawing::Point(4, 4);
			this->colorPicker_label->Name = L"colorPicker_label";
			this->colorPicker_label->Size = System::Drawing::Size(200, 204);
			this->colorPicker_label->TabIndex = 3;
			this->colorPicker_label->Paint += gcnew System::Windows::Forms::PaintEventHandler(this, &CPowerColorPickerCtrl::colorPicker_label_Paint);
			this->colorPicker_label->MouseMove += gcnew System::Windows::Forms::MouseEventHandler(this, &CPowerColorPickerCtrl::colorPicker_MouseMove);
			this->colorPicker_label->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &CPowerColorPickerCtrl::colorPicker_MouseDown);
			this->colorPicker_label->MouseUp += gcnew System::Windows::Forms::MouseEventHandler(this, &CPowerColorPickerCtrl::colorPicker_MouseUp);
			// 
			// CPowerColorPickerCtrl
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->Controls->Add(this->finaleColor_label);
			this->Controls->Add(this->luminance_label);
			this->Controls->Add(this->colorPicker_label);
			this->Name = L"CPowerColorPickerCtrl";
			this->Size = System::Drawing::Size(230, 233);
			this->ResumeLayout(false);

		}
#pragma endregion

		private: void GenerateColorPicker();
		private: void GenerateLuminanceBar();
		private: void UpdateFinaleColorBar();
		private: double HuetoRGB(double m1, double m2, double h );
		private: Drawing::Color HLStoRGB( double H, double L, double S );
		private: void RGBtoHSL( Drawing::Color rgb, double *H, double *S, double *L );

		private: System::Void colorPicker_MouseDown(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e);
		private: System::Void colorPicker_MouseUp(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e);
		private: System::Void colorPicker_MouseMove(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e);
		private: System::Void colorPicker_label_Paint(System::Object^  sender, System::Windows::Forms::PaintEventArgs^  e);

		private: System::Void luminanceBar_MouseDown(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e);
		private: System::Void luminanceBar_MouseUp(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e);
		private: System::Void luminanceBar_MouseMove(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e);
		private: System::Void luminance_label_Paint(System::Object^  sender, System::Windows::Forms::PaintEventArgs^  e);

		private: void SetCurPickerColorToImp();

		public:  virtual void Update(void) override;
		public:  virtual char*	GetImpTypeName(void)	override;

};
}
