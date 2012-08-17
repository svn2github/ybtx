#include "StdAfx.h"
#include "SizeControl.h"

System::Void sqr_tools::SizeControl::upBtn_Click( System::Object^ sender, System::EventArgs^ e )
{
	m_iValue+=1;
	this->_wfes->CloseDropDown();
}

System::Void sqr_tools::SizeControl::downBtn_Click( System::Object^ sender, System::EventArgs^ e )
{
	m_iValue-=1;
	this->_wfes->CloseDropDown();
}