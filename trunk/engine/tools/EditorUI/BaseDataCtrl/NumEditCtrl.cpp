#include "StdAfx.h"
#include "NumEditCtrl.h"
#include "NumEditCtrlImp.h"

namespace sqr_tools
{
	// m_eType 1:int 2:float 3:byte
	char* NumEditCtrl::GetImpTypeName( void )
	{
		return GET_IMP_NAME(NumEditCtrlImp);
	}

	System::Void  NumEditCtrl::InitOtherComponent()
	{
		m_eType = 2;
		NumUpDown->Maximum = 100000;
		NumUpDown->Minimum = -100000;
	}

	System::Void  NumEditCtrl::DestroyOtherComponent()
	{
	}

	void	NumEditCtrl::Update(void)
	{
		// 根据数据类型，设置控件的属性
		NumEditCtrlImp* NumEditimp = (NumEditCtrlImp*)m_pCtrl;

		this->NumUpDown->Location = System::Drawing::Point(4, 3);
		this->Size = System::Drawing::Size(136, 27);

		// content
		ECtrlDataType tp = NumEditimp->GetDataType();
		System::String^ Textstr;
		if (tp == E_INTEGER)
		{
			m_eType = 1;
			Int32 dt = NumEditimp->GetImpIntValue();
			Textstr = dt.ToString();

			NumUpDown->DecimalPlaces = 0;
			NumUpDown->Increment = 1;
			NumUpDown->Maximum = 100000;
			NumUpDown->Minimum = -100000;
		}
		else if(tp == E_FLOAT)
		{
			m_eType = 2;
			Double dt = NumEditimp->GetImpFloatValue();
			Textstr = dt.ToString();

			NumUpDown->DecimalPlaces = 2;
			NumUpDown->Increment = System::Decimal(0.25);
			NumUpDown->Maximum = 100000;
			NumUpDown->Minimum = -100000;
		}
		else
		{
			m_eType = 3;
			Int32 dt = NumEditimp->GetImpByteValue();
			Textstr = dt.ToString();

			NumUpDown->DecimalPlaces = 0;
			NumUpDown->Increment = 1;
			NumUpDown->Maximum = 255;
			NumUpDown->Minimum = -255;
		}
		NumUpDown->Text = Textstr;
	}

	System::Void NumEditCtrl::NumUpDown_ValueChanged( System::Object^ sender, System::EventArgs^ e )
	{
		if (m_eType == 2)
		{
			float val = (float)(Convert::ToDouble(NumUpDown->Value));
			NumEditCtrlImp* NumEditimp = (NumEditCtrlImp*)m_pCtrl;
			NumEditimp->SetImpFloatValue(val);
		}
		else if(m_eType == 1)
		{
			int val = Convert::ToInt32(NumUpDown->Value);
			NumEditCtrlImp* NumEditimp = (NumEditCtrlImp*)m_pCtrl;
			NumEditimp->SetImpIntValue(val);
		}
		else
		{
			int val = Convert::ToInt32(NumUpDown->Value);
			NumEditCtrlImp* NumEditimp = (NumEditCtrlImp*)m_pCtrl;
			NumEditimp->SetImpByteValue(val);
		}
		
	}
}

