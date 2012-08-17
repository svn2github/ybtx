#include "StdAfx.h"
#include "CNewMapCtrl.h"

System::Void CNewMapCtrl::HideBt()
{
	this->m_btOK->Visible=false;
	this->m_btCancle->Visible=false;
}

System::String^ CNewMapCtrl::GetMapName()
{
	return this->m_editTitle->Text;
}

int CNewMapCtrl::GetMapWidth()
{
	return  System::Convert::ToInt32(this->m_editWidth->Text);
}

int CNewMapCtrl::GetMapHeight()
{
	return  System::Convert::ToInt32(this->m_editLength->Text);
}

System::String^ CNewMapCtrl::GetHMapName()
{
	return this->m_editHighMap->Text;
}

System::Void CNewMapCtrl::CreatMap()
{
	if(System::String::IsNullOrEmpty(this->m_editWidth->Text))
		return ;
	if(System::String::IsNullOrEmpty(this->m_editLength->Text))
		return ;

	int w=System::Convert::ToInt32(this->m_editWidth->Text);
	int h=System::Convert::ToInt32(this->m_editLength->Text);
	if(w!=0 && h!=0 && w%4==0 && h%4==0)
	{
		bool UseHighMap=this->m_checkHighMap->Checked;
	}
	else
		System::Windows::Forms::MessageBox::Show("ERROR 宽或者高为0 或者不是4的倍数");

}

System::Void CNewMapCtrl::m_btOK_Click(System::Object^  sender, System::EventArgs^  e)
{
	CreatMap();
}

System::Void CNewMapCtrl::m_btCancle_Click(System::Object^  sender, System::EventArgs^  e)
{
}

System::Void CNewMapCtrl::hmapPathClick(System::Object^  sender, System::EventArgs^  e)
{
	int a=0;
}

System::Void CNewMapCtrl::m_checkHighMap_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
{
}

System::Void CNewMapCtrl::m_editWidth_TextChanged(System::Object^  sender, System::EventArgs^  e)
{
	System::Text::RegularExpressions::Regex^ reg=gcnew System::Text::RegularExpressions::Regex("^(\\d)+$");
	if(!reg->IsMatch(this->m_editWidth->Text))
		this->m_editWidth->Text=L"4";
}

System::Void CNewMapCtrl::m_editLength_TextChanged(System::Object^  sender, System::EventArgs^  e)
{
	System::Text::RegularExpressions::Regex^ reg=gcnew System::Text::RegularExpressions::Regex("^(\\d)+$");
	if(!reg->IsMatch(this->m_editLength->Text))
		this->m_editLength->Text=L"4";
}


