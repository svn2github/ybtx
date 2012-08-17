#include "StdAfx.h"
#include "MaxMinNumEditDialog.h"
#include "CCtrlPropBase.h"
#include "NumMapEditCtrlImp.h"

namespace sqr_tools
{
	System::Void  MaxMinNumEditDialog::InitOtherComponent()
	{
		m_pEditCtrl = NULL;
	}

	System::Void  MaxMinNumEditDialog::DestroyOtherComponent()
	{
	}

	void	MaxMinNumEditDialog::SetMaxMinEditData(void* ctrl)
	{
		m_pEditCtrl = (NumMapEditCtrlImp*)ctrl;

		m_fMax = m_pEditCtrl->GetMaxVal();
		m_fMin = m_pEditCtrl->GetMinVal();

		Double dt = m_fMax;
		System::String^ Textstr = dt.ToString();
		MaxNumeric->Text = Textstr;
		dt = m_fMin;
		Textstr = dt.ToString();
		MinNumeric->Text = Textstr;
	}

	System::Void MaxMinNumEditDialog::MaxNumeric_ValueChanged( System::Object^ sender, System::EventArgs^ e )
	{
		m_fMax = (float)(Convert::ToDouble(MaxNumeric->Value));
		if (m_fMax <= m_fMin)
		{
			m_fMax = m_fMin + 1.0f;
			Double dt = m_fMax;
			System::String^ Textstr = dt.ToString();
			MaxNumeric->Text = Textstr;
		}
	}

	System::Void MaxMinNumEditDialog::MinNumeric_ValueChanged( System::Object^ sender, System::EventArgs^ e )
	{
		m_fMin = (float)(Convert::ToDouble(MinNumeric->Value));
		if (m_fMax <= m_fMin)
		{
			m_fMin = m_fMax - 1.0f;
			Double dt = m_fMin;
			System::String^ Textstr = dt.ToString();
			MinNumeric->Text = Textstr;
		}
	}

	System::Void MaxMinNumEditDialog::OKButton_Click( System::Object^ sender, System::EventArgs^ e )
	{
		m_pEditCtrl->SetMaxVal(m_fMax);
		m_pEditCtrl->SetMinVal(m_fMin);
		if(this->Visible)
			this->Hide();
	}

	System::Void MaxMinNumEditDialog::CancleButton_Click( System::Object^ sender, System::EventArgs^ e )
	{
		if(this->Visible)
			this->Hide();
	}

	System::Void MaxMinNumEditDialog::MaxMinNumEditDialog_FormClosing( System::Object^ sender, System::Windows::Forms::FormClosingEventArgs^ e )
	{
		if(this->Visible)
		{
			e->Cancel=true;
			this->Hide();
		}
	}
}