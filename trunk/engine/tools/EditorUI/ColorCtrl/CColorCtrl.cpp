#include "StdAfx.h"
#include "CColorCtrl.h"
#include "ColorCtrl/CColorCtrlImp.h"

void CColorCtrl::Update( void )
{
	CColorCtrlImp *pImp = NULL;
	pImp = (CColorCtrlImp*)m_pCtrl;
	if( pImp )
	{
		DWORD dwColor = pImp->GetColor();
		System::Drawing::Color^ clrColor = gcnew System::Drawing::Color();
		clrColor->FromArgb(dwColor);

		Int32 r = clrColor->R;
		Int32 g = clrColor->G;
		Int32 b = clrColor->B;
		this->color_R_textBox->Text = r.ToString();
		this->color_G_textBox->Text = g.ToString();
		this->color_B_textBox->Text = b.ToString();
	}
}

System::Void CColorCtrl::color_R_textBox_TextChanged( System::Object^ sender, System::EventArgs^ e )
{
	this->SetColorCtrlImp();
}

System::Void CColorCtrl::color_G_textBox_TextChanged( System::Object^ sender, System::EventArgs^ e )
{
	this->SetColorCtrlImp();
}

System::Void CColorCtrl::color_B_textBox_TextChanged( System::Object^ sender, System::EventArgs^ e )
{
	this->SetColorCtrlImp();
}

void CColorCtrl::SetColorCtrlImp()
{
	Int32 r = System::Int32::Parse(this->color_R_textBox->Text);
	Int32 g = System::Int32::Parse(this->color_G_textBox->Text);
	Int32 b = System::Int32::Parse(this->color_B_textBox->Text);

	DWORD color = 0;
	color  = r << 16 | g << 8 | b;

	CColorCtrlImp *pImp = NULL;
	pImp = (CColorCtrlImp*)m_pCtrl;
	if( pImp )
	{
		pImp->SetColor(color);
	}
}

char* CColorCtrl::GetImpTypeName()
{
	return GET_IMP_NAME(CColorCtrlImp);
}

void* CColorCtrl::GetImpParam( void )
{
	return this->Handle.ToPointer();
}