#include "StdAfx.h"
#include "CueListDialog.h"
#include "CCtrlPropBase.h"
#include "SingleStringEditCtrlImp.h"

namespace sqr_tools
{
	System::Void  CueListDialog::InitOtherComponent()
	{
		m_pEditCtrl = NULL;
	}

	System::Void  CueListDialog::DestroyOtherComponent()
	{
	}

	void	CueListDialog::SetCueEditData(void* ctrl, const char* str)
	{
		m_pEditCtrl = ctrl;
		if (str)
			m_OndString = gcnew System::String(str);
	}

	System::Void CueListDialog::OKButton_Click( System::Object^ sender, System::EventArgs^ e )
	{
		// 设置当前音效
		const char* name = this->DlgCueListCtrl->GetImpCueName();
		SingleStringEditCtrlImp* StrEditimp = (SingleStringEditCtrlImp*)m_pEditCtrl;
		ECtrlDataType tp = StrEditimp->GetDataType();
		if (tp == E_CUEESTRING)
		{
			StrEditimp->SetImpSingleStrValue(name);
			StrEditimp->Update();
		}
		if(this->Visible)
			this->Hide();
		
	}

	System::Void CueListDialog::CancleButton_Click( System::Object^ sender, System::EventArgs^ e )
	{
		this->DlgCueListCtrl->SetImpCueName(m_OndString);
		if(this->Visible)
			this->Hide();
	}

	System::Void CueListDialog::CueListDialog_FormClosing( System::Object^ sender, System::Windows::Forms::FormClosingEventArgs^ e )
	{
		this->DlgCueListCtrl->SetImpCueName(m_OndString);
		if(this->Visible)
			this->Hide();
	}
}

