#include "StdAfx.h"
#include "CExMapCtrl.h"

void CExMapCtrl::HideBt()
{
	this->btOK->Visible=false;
	this->btCancle->Visible=false;
}

void CExMapCtrl::SetMapName(void* name)
{
}

void CExMapCtrl::SetCurMapWidth(int w)
{
	System::Int32^ tmpWidth=w;
	this->labelCurWidth->Text=tmpWidth->ToString();
}

void CExMapCtrl::SetCurMapLength(int l)
{
	System::Int32^ tmpLength=l;
	this->labelCurLength->Text=tmpLength->ToString();
}

int CExMapCtrl::GetExMapWidth()
{
	int ResultWidth=System::Convert::ToInt32(this->EditExWidth->Text);
	return ResultWidth;
}

int CExMapCtrl::GetExMapLength()
{
	int ResultLength=System::Convert::ToInt32(this->EditExLength->Text);
	return ResultLength;
}

int CExMapCtrl::GetExMapWidthDir()
{
	if(this->cBoxWDouble->Checked)
		return 0;
	if(this->cBoxWRight->Checked)
		return 1;
	if(this->cBoxWLeft->Checked)
		return 2;
	return 0;
}

int CExMapCtrl::GetExMapLengthDir()
{
	if(this->cBoxLDouble->Checked)
		return 0;
	if(this->cBoxLFront->Checked)
		return 1;
	if(this->cBoxLBack->Checked)
		return 2;
	return 0;
}

System::Void CExMapCtrl::EditExWidth_TextChanged(System::Object^  sender, System::EventArgs^  e)
{
	System::Text::RegularExpressions::Regex^ reg=gcnew System::Text::RegularExpressions::Regex("^(\\d)+$");
	if(!reg->IsMatch(this->EditExWidth->Text))
		this->EditExWidth->Text=L"0";
}

System::Void CExMapCtrl::EditExLength_TextChanged(System::Object^  sender, System::EventArgs^  e)
{
	System::Text::RegularExpressions::Regex^ reg=gcnew System::Text::RegularExpressions::Regex("^(\\d)+$");
	if(!reg->IsMatch(this->EditExLength->Text))
		this->EditExLength->Text=L"0";
}

System::Void CExMapCtrl::cBoxWDouble_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
{
	if(this->cBoxWDouble->Checked)
	{
		this->cBoxWLeft->Checked=false;
		this->cBoxWRight->Checked=false;
	}
}

System::Void CExMapCtrl::cBoxWRight_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
{
	if(this->cBoxWRight->Checked)
	{
		this->cBoxWLeft->Checked=false;
		this->cBoxWDouble->Checked=false;
	}
}

System::Void CExMapCtrl::cBoxWLeft_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
{
	if(this->cBoxWLeft->Checked)
	{
		this->cBoxWDouble->Checked=false;
		this->cBoxWRight->Checked=false;
	}
}

System::Void CExMapCtrl::cBoxLDouble_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
{
	if(this->cBoxLDouble->Checked)
	{
		this->cBoxLBack->Checked=false;
		this->cBoxLFront->Checked=false;
	}
}

System::Void CExMapCtrl::cBoxLFront_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
{
	if(this->cBoxLFront->Checked)
	{
		this->cBoxLBack->Checked=false;
		this->cBoxLDouble->Checked=false;
	}
}

System::Void CExMapCtrl::cBoxLBack_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
{
	if(this->cBoxLBack->Checked)
	{
		this->cBoxLDouble->Checked=false;
		this->cBoxLFront->Checked=false;
	}
}

System::Void CExMapCtrl::btOK_Click(System::Object^  sender, System::EventArgs^  e)
{
}

System::Void CExMapCtrl::btCancle_Click(System::Object^  sender, System::EventArgs^  e)
{
}