#include "StdAfx.h"
#include "CFogParameterCtrl.h"
#include "FogParameterCtrl/CFogParameterCtrlImp.h"
#include "ColorCtrl/CColorCtrlImp.h"

//DWORD gColor = 0;
void CFogSettingCtrl::Update( void )
{
	CFogParameterCtrlImp *pImp = NULL;
	pImp = (CFogParameterCtrlImp*)m_pCtrl;
	if( pImp )
	{
		static bool bFirst = true;
		if( bFirst )
		{
			this->InitFogParaCtrl();
			bFirst = false;
		}
	}
}

System::Void CFogSettingCtrl::liner_radioButton_CheckedChanged( System::Object^ sender, System::EventArgs^ e )
{
	this->UpdateFogParaCtrl();
}

System::Void CFogSettingCtrl::exp_radioButton_CheckedChanged( System::Object^ sender, System::EventArgs^ e )
{
	this->UpdateFogParaCtrl();
}

System::Void CFogSettingCtrl::desnity_trackBar_Scroll( System::Object^ sender, System::EventArgs^ e )
{
	this->desnity_textBox->Text = System::Convert::ToString(this->desnity_trackBar->Value);
	this->desnity_textBox->Refresh();
}

System::Void CFogSettingCtrl::start_trackBar_Scroll( System::Object^ sender, System::EventArgs^ e )
{
	this->start_textBox->Text = System::Convert::ToString(this->start_trackBar->Value);
	this->start_textBox->Refresh();
}

System::Void CFogSettingCtrl::end_trackBar_Scroll( System::Object^ sender, System::EventArgs^ e )
{
	this->end_textBox->Text = System::Convert::ToString(this->end_trackBar->Value);
	this->end_textBox->Refresh();
}

System::Void CFogSettingCtrl::desnity_textBox_TextChanged( System::Object^ sender, System::EventArgs^ e )
{
	this->start_trackBar->Value = System::Convert::ToInt32(this->start_textBox->Text); 
	this->start_trackBar->Refresh();

	this->UpdateFogParaCtrl();
}

System::Void CFogSettingCtrl::start_textBox_TextChanged( System::Object^ sender, System::EventArgs^ e )
{
	this->desnity_trackBar->Value = System::Convert::ToInt32(this->desnity_textBox->Text); 
	this->desnity_trackBar->Refresh();

	this->UpdateFogParaCtrl();
}

System::Void CFogSettingCtrl::end_textBox_TextChanged( System::Object^ sender, System::EventArgs^ e )
{
	this->end_trackBar->Value = System::Convert::ToInt32(this->end_textBox->Text); 
	this->end_trackBar->Refresh();

	this->UpdateFogParaCtrl();
}

System::Void CFogSettingCtrl::setColor_button_Click( System::Object^ sender, System::EventArgs^ e )
{
	this->fog_colorDialog->FullOpen = true;
	if( this->fog_colorDialog->ShowDialog() == System::Windows::Forms::DialogResult::OK )
	{
		System::Drawing::Color clrcolor = this->fog_colorDialog->Color;
		this->dwColor = clrcolor.R << 16 | clrcolor.G << 8 | clrcolor.B;

		///传到imp去让那边更新color的ctrl
		this->CollectMsg(MGT_UPDATE_FOGCOLOR);
	}
}

System::Void CFogSettingCtrl::enableFog_checkBox_CheckedChanged( System::Object^ sender, System::EventArgs^ e )
{
	this->UpdateFogParaCtrl();
}

void CFogSettingCtrl::InitFogParaCtrl()
{
	this->desnity_trackBar->Minimum = 0;
	this->desnity_trackBar->Maximum = (int)(FOGDENSITY * 80000);
	this->desnity_trackBar->LargeChange = 1;
	this->desnity_trackBar->SmallChange = 1;
	this->desnity_trackBar->TickFrequency = 1;//每次移动的刻度
	this->desnity_trackBar->Value = 4; //初始位置

	this->start_trackBar->Minimum = 0;
	this->start_trackBar->Maximum = FOGMAXSTART;
	//this->start_trackBar->LargeChange = 1;
	//this->start_trackBar->SmallChange = 60;
	this->start_trackBar->TickFrequency = 60;//每次移动的刻度
	this->start_trackBar->Value = FOGSTART; //初始位置

	this->end_trackBar->Minimum = 0;
	this->end_trackBar->Maximum = FOGMAXEND;
	this->end_trackBar->TickFrequency = 60;//每次移动的刻度
	this->end_trackBar->Value = FOGEND; //初始位置
}

void CFogSettingCtrl::UpdateFogParaCtrl()
{
	CFogParameterCtrlImp *pImp = NULL;
	pImp = (CFogParameterCtrlImp*)m_pCtrl;
	if( pImp )
	{
		if( this->liner_radioButton->Checked )
			pImp->SetFogType(FMT_LINEAR);
		else
		{
			if( this->exp_radioButton->Checked )
				pImp->SetFogType(FMT_EXP);
		}

		pImp->SetFogIsEnable(this->enableFog_checkBox->Checked);

		float fFogStart = (float)(System::Convert::ToDouble(this->start_textBox->Text));

		float fFogEnd   = (float)(System::Convert::ToDouble(this->end_textBox->Text));

		float fFogDensity = (float)(System::Convert::ToDouble(this->desnity_textBox->Text));

		this->CollectMsg(MGT_UPDATE_FOGPARA);
	}
}	

void CFogSettingCtrl::CollectMsg( int nId )
{
	///传到imp去让那边更新color的ctrl
	CFogParameterCtrlImp *pImp = NULL;
	pImp = (CFogParameterCtrlImp*)m_pCtrl;
	if( pImp )
	{
		MSG_SOURCE source;
		MSG_TYPE type;
		MSG_INFO info;
		source = MGS_CUSTOM;

		switch (nId)
		{
		case MGT_UPDATE_COLOR:
			{
				type.custom_type	= MGT_UPDATE_COLOR;
				info._iinfo			= this->dwColor;
			}
			break;

		default:
			return;
		}
		
		sqr::CMsg msg(source,type,info);
		m_pCtrl->Collect(msg);
	}
}

System::Void sqr_tools::CFogSettingCtrl::CFogSettingCtrl_Load( System::Object^ sender, System::EventArgs^ e )
{
	CFogParameterCtrlImp *pImp = NULL;
	pImp = (CFogParameterCtrlImp*)m_pCtrl;
	if( pImp )
	{
		CColorCtrlImp* pColorImp = NULL;
		pColorImp = (CColorCtrlImp*)(this->cFogColorCtrl->GetCoreCtrl());
		if( pColorImp )
		{
			string strTypeName = pColorImp->GetTypeName();
			pImp->SetFogColorImp(pColorImp);
		}
	}
}

char* CFogSettingCtrl::GetImpTypeName( void )
{
	return NULL;
}