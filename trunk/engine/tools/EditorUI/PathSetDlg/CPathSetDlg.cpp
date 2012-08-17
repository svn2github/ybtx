#include "StdAfx.h"
#include "CPathSetDlg.h"
#include "CCtrlBase.h"

System::Void CPathSetDlg::PathApp_Click(System::Object^  sender, System::EventArgs^  e)
{
	if(PathCtrl)
		PathCtrl->AppPath();
}

System::Void CPathSetDlg::PathReset_Click(System::Object^  sender, System::EventArgs^  e)
{
	if(PathCtrl)
		PathCtrl->ResetPath();
}

System::Void CPathSetDlg::PathCancle_Click(System::Object^  sender, System::EventArgs^  e) 
{
	this->Hide();
}

System::Void CPathSetDlg::PathOK_Click(System::Object^  sender, System::EventArgs^  e) 
{
	if(PathCtrl)
		PathCtrl->AppPath();
	this->Hide();
}

System::Void CPathSetDlg::CPathSetDlg_FormClosing(System::Object^  sender, System::Windows::Forms::FormClosingEventArgs^  e) 
{
	if(this->Visible)
	{
		e->Cancel=true;
		this->Hide();
	}
}
