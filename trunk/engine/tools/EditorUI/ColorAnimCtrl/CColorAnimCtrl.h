#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

#include "CUIPropBase.h"

namespace sqr_tools 
{
	struct ColorAnimNode;

	public ref class CColorAnimCtrl : public CUIPropBase
	{
	public:
		CColorAnimCtrl(void)
		{
			InitializeComponent();
		}

		virtual char*	
				GetImpTypeName(void) override;
		
		virtual void*	
				GetImpParam(void) override;

		virtual void	
				Update(void) override;

	protected:
		~CColorAnimCtrl()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Label^  Titlelabel;

	private:
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		void InitializeComponent(void)
		{
			this->Titlelabel = (gcnew System::Windows::Forms::Label());
			this->SuspendLayout();
			// 
			// Titlelabel
			// 
			this->Titlelabel->Location = System::Drawing::Point(3, 3);
			this->Titlelabel->Name = L"Titlelabel";
			this->Titlelabel->Size = System::Drawing::Size(120, 13);
			this->Titlelabel->TabIndex = 3;
			this->Titlelabel->Text = L"ÃèÊöÎÄ×Ö";
			this->Titlelabel->Visible = false;
			// 
			// CColorAnimCtrl
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->BackColor = System::Drawing::SystemColors::Control;
			this->Controls->Add(this->Titlelabel);
			this->Name = L"CColorAnimCtrl";
			this->Paint += gcnew System::Windows::Forms::PaintEventHandler(this, &CColorAnimCtrl::OnPaint);
			this->MouseDoubleClick += gcnew System::Windows::Forms::MouseEventHandler(this, &CColorAnimCtrl::OnDoubleClick);
			this->ResumeLayout(false);

		}
#pragma endregion
	private: 
		System::Void OnPaint( System::Object^  sender, System::Windows::Forms::PaintEventArgs^ e );
		System::Void OnDoubleClick( System::Object^  sender, System::Windows::Forms::MouseEventArgs^ e );
		System::Void OnDraw();
	};
}
