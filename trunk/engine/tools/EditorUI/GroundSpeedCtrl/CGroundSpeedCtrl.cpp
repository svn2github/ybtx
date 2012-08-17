#include "StdAfx.h"
#include "CGroundSpeedCtrl.h"
#include "GroundSpeedCtrl/CGroundSettingCtrlImp.h"

#define GroundSettingCtrlImp ((CGroundSettingCtrlImp*)GetCoreCtrl())

System::Void CGroundSpeedCtrl::set_button_Click( System::Object^ sender, System::EventArgs^ e )
{
	if (GroundSettingCtrlImp)
	{
		if (this->groundSpeed_textBox->Text->Length == 0)
			return;

		float fSpeed = (float)(System::Convert::ToDouble(this->groundSpeed_textBox->Text));
		GroundSettingCtrlImp->SetGroundSpeed(fSpeed);
		GroundSettingCtrlImp->NotifyObserver();
	}
}

void CGroundSpeedCtrl::Update( void )
{
	if( GroundSettingCtrlImp )
	{
		float fSpeend = GroundSettingCtrlImp->GetGroundSpeed();
		this->groundSpeed_textBox->Text = System::Convert::ToString(fSpeend);
	}
}


char* CGroundSpeedCtrl::GetImpTypeName( void )
{
	return GET_IMP_NAME(CGroundSettingCtrlImp);
}

void* sqr_tools::CGroundSpeedCtrl::GetImpParam( void )
{
	return this->Handle.ToPointer();
}