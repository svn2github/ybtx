#include "StdAfx.h"
#include "CNewMapDlg.h"

System::Void CNewMapDlg::btOK_Click(System::Object^  sender, System::EventArgs^  e)
{
	this->m_newMapCtrl->CreatMap();
	this->Close();
}

System::Void CNewMapDlg::btCancle_Click(System::Object^  sender, System::EventArgs^  e)
{
	this->Close();
}