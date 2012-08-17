#include "StdAfx.h"
#include "CDefineScaleDlg.h"


System::Void CharacterBrowser::CDefineScaleDlg::ok_button_Click( System::Object^ sender, System::EventArgs^ e )
{
	this->Hide();
}

double CharacterBrowser::CDefineScaleDlg::GetScale()
{
	return System::Convert::ToDouble(this->scale_textBox->Text);  
}