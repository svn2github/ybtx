#include "StdAfx.h"
#include "CVideoSetDlg.h"

System::Void CVideoSetDlg::CVideoSetDlg_FormClosing(System::Object^  sender, System::Windows::Forms::FormClosingEventArgs^  e)
{
	if(this->Visible)
	{
		e->Cancel=true;
		this->Hide();
	}
}

