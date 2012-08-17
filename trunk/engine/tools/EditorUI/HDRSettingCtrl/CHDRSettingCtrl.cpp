#include "StdAfx.h"
#include "CHDRSettingCtrl.h"
#include "HDRSettingCtrl/CHDRSettingCtrlImp.h"

void CHDRSettingCtrl::Update(void)
{
	CHDRSettingCtrlImp *pImp = NULL;
	pImp = (CHDRSettingCtrlImp*)m_pCtrl;
	if( pImp )
	{
		static bool bFirst = true;
		if( bFirst )
		{
			this->InitHDRParaCtrl();
			bFirst = false;
		}
	}
}

char* CHDRSettingCtrl::GetImpTypeName()
{
	return GET_IMP_NAME(CHDRSettingCtrlImp);
}

System::Void CHDRSettingCtrl::checkBox1_CheckedChanged( System::Object^ sender, System::EventArgs^ e )
{
	bool bTurnOnHDR = this->checkBox1->Checked;
	CHDRSettingCtrlImp *pImp = (CHDRSettingCtrlImp*)m_pCtrl;
	if( pImp )
		pImp->SetIsTurnOnGaussTarget(bTurnOnHDR);
}

System::Void CHDRSettingCtrl::checkBox2_CheckedChanged( System::Object^ sender, System::EventArgs^ e )
{
	bool bTurnOnRefract = this->checkBox2->Checked;
	CHDRSettingCtrlImp *pImp = (CHDRSettingCtrlImp*)m_pCtrl;
	if( pImp )
		pImp->SetIsTurnOnRefract(bTurnOnRefract);
}

System::Void CHDRSettingCtrl::HighPass_trackBar_Scroll( System::Object^ sender, System::EventArgs^ e )
{
	this->HighPass_textBox->Text = System::Convert::ToString(this->HighPass_trackBar->Value);
	this->HighPass_textBox->Refresh();
}

System::Void CHDRSettingCtrl::HighPass_textBox_TextChanged( System::Object^ sender, System::EventArgs^ e )
{
	this->HighPass_trackBar->Value = System::Convert::ToInt32(this->HighPass_textBox->Text); 
	this->HighPass_trackBar->Refresh();

	this->CollectMsg();
}

System::Void CHDRSettingCtrl::Mul_trackBar_Scroll( System::Object^ sender, System::EventArgs^ e )
{
	this->Mul_textBox->Text = System::Convert::ToString(this->Mul_trackBar->Value);
	this->Mul_textBox->Refresh();
}

System::Void CHDRSettingCtrl::Mul_textBox_TextChanged( System::Object^ sender, System::EventArgs^ e )
{
	this->Mul_trackBar->Value = System::Convert::ToInt32(this->Mul_textBox->Text); 
	this->Mul_trackBar->Refresh();

	this->CollectMsg();
}

System::Void CHDRSettingCtrl::FMean_trackBar_Scroll( System::Object^ sender, System::EventArgs^ e )
{
	this->FMean_textBox->Text = System::Convert::ToString(this->FMean_trackBar->Value);
	this->FMean_textBox->Refresh();
}

System::Void CHDRSettingCtrl::FMean_textBox_TextChanged( System::Object^ sender, System::EventArgs^ e )
{
	this->FMean_trackBar->Value = System::Convert::ToInt32(this->FMean_textBox->Text); 
	this->FMean_trackBar->Refresh();

	this->CollectMsg();
}

System::Void CHDRSettingCtrl::StdDev_trackBar_Scroll( System::Object^ sender, System::EventArgs^ e )
{
	this->StdDev_textBox->Text = System::Convert::ToString(this->StdDev_trackBar->Value);
	this->StdDev_textBox->Refresh();
}

System::Void CHDRSettingCtrl::StdDev_textBox_TextChanged( System::Object^ sender, System::EventArgs^ e )
{
	this->StdDev_trackBar->Value = System::Convert::ToInt32(this->StdDev_textBox->Text); 
	this->StdDev_trackBar->Refresh();

	this->CollectMsg();
}

System::Void CHDRSettingCtrl::Exp_trackBar_Scroll( System::Object^ sender, System::EventArgs^ e )
{
	this->Exp_textBox->Text = System::Convert::ToString(this->Exp_trackBar->Value);
	this->Exp_textBox->Refresh();
}

System::Void CHDRSettingCtrl::Exp_textBox_TextChanged( System::Object^ sender, System::EventArgs^ e )
{
	this->Exp_trackBar->Value = System::Convert::ToInt32(this->Exp_textBox->Text); 
	this->Exp_trackBar->Refresh();

	this->CollectMsg();
}

System::Void CHDRSettingCtrl::SAM_trackBar_Scroll( System::Object^ sender, System::EventArgs^ e )
{
	this->SAM_textBox->Text = System::Convert::ToString(this->SAM_trackBar->Value);
	this->SAM_textBox->Refresh();
}

System::Void CHDRSettingCtrl::SAM_textBox_TextChanged( System::Object^ sender, System::EventArgs^ e )
{
	this->SAM_trackBar->Value = System::Convert::ToInt32(this->SAM_textBox->Text); 
	this->SAM_trackBar->Refresh();

	this->CollectMsg();
}

void CHDRSettingCtrl::CollectMsg()
{
	///传到imp去让那边更新color的ctrl
	CHDRSettingCtrlImp *pImp = NULL;
	pImp = (CHDRSettingCtrlImp*)m_pCtrl;
	if( pImp )
	{
		MSG_SOURCE source;
		MSG_TYPE type;
		MSG_INFO info;
		source = MGS_CUSTOM;
		type.custom_type = MGT_UPDATE_PARA;

		float fHighPass			= (float)(System::Convert::ToDouble(this->HighPass_textBox->Text)) * 0.001f;
 		float fGaussMultiplier	= (float)(System::Convert::ToDouble(this->Mul_textBox->Text)) * 0.001f;
 		float fGaussMean		= (float)(System::Convert::ToDouble(this->FMean_textBox->Text)) * 0.001f;
 		float fGaussStdDev		= (float)(System::Convert::ToDouble(this->StdDev_textBox->Text)) * 0.001f;
 		float fGaussExposure	= (float)(System::Convert::ToDouble(this->Exp_textBox->Text)) * 0.001f;
 		float fGaussSam			= (float)(System::Convert::ToDouble(this->SAM_textBox->Text)) * 0.001f;

		sqr::CMsg msg(source,type,info);
		m_pCtrl->Collect(msg);
	}
}

void CHDRSettingCtrl::InitHDRParaCtrl()
{
	CHDRSettingCtrlImp *pImp = (CHDRSettingCtrlImp*)m_pCtrl;
	float fHighPass			= pImp->GetGaussBrightPass();
	float fGaussMultiplier	= pImp->GetGaussMultiplier(); 
	float fGaussMean		= pImp->GetGaussMean();
	float fGaussStdDev		= pImp->GetGaussStdDev();
	float fGaussExposure	= pImp->GetGaussExposure();
	float fGaussSam			= pImp->GetGaussSample();

	this->HighPass_trackBar->Minimum = 0;
	this->HighPass_trackBar->Maximum = 1000;
	this->HighPass_trackBar->LargeChange = 1;
	this->HighPass_trackBar->SmallChange = 1;
	this->HighPass_trackBar->TickFrequency = 1;//每次移动的刻度
	this->HighPass_trackBar->Value = (int)(fHighPass*1000); //初始位置

	this->Mul_trackBar->Minimum = 0;
	this->Mul_trackBar->Maximum = 2000;
	this->Mul_trackBar->LargeChange = 1;
	this->Mul_trackBar->SmallChange = 1;
	this->Mul_trackBar->TickFrequency = 1;//每次移动的刻度
	this->Mul_trackBar->Value = (int)(fGaussMultiplier*1000); //初始位置

	this->FMean_trackBar->Minimum = -1000;
	this->FMean_trackBar->Maximum = 1000;
	this->FMean_trackBar->LargeChange = 1;
	this->FMean_trackBar->SmallChange = 1;
	this->FMean_trackBar->TickFrequency = 1;//每次移动的刻度
	this->FMean_trackBar->Value = (int)(fGaussMean*1000); //初始位置

	this->StdDev_trackBar->Minimum = 0;
	this->StdDev_trackBar->Maximum = 1000;
	this->StdDev_trackBar->LargeChange = 1;
	this->StdDev_trackBar->SmallChange = 1;
	this->StdDev_trackBar->TickFrequency = 1;//每次移动的刻度
	this->StdDev_trackBar->Value = (int)(fGaussStdDev*1000); //初始位置

	this->Exp_trackBar->Minimum = 0;
	this->Exp_trackBar->Maximum = 2000;
	this->Exp_trackBar->LargeChange = 1;
	this->Exp_trackBar->SmallChange = 1;
	this->Exp_trackBar->TickFrequency = 1;//每次移动的刻度
	this->Exp_trackBar->Value = (int)(fGaussExposure*1000); //初始位置

	this->SAM_trackBar->Minimum = 0;
	this->SAM_trackBar->Maximum = 1000;
	this->SAM_trackBar->LargeChange = 1;
	this->SAM_trackBar->SmallChange = 1;
	this->SAM_trackBar->TickFrequency = 1;//每次移动的刻度
	this->SAM_trackBar->Value = (int)(fGaussSam*1000); //初始位置

	this->HighPass_textBox->Text	= System::Convert::ToString(fHighPass);
	this->Mul_textBox->Text			= System::Convert::ToString(fGaussMultiplier);
	this->FMean_textBox->Text		= System::Convert::ToString(fGaussMean);
	this->StdDev_textBox->Text		= System::Convert::ToString(fGaussStdDev);
	this->Exp_textBox->Text			= System::Convert::ToString(fGaussExposure);
	this->SAM_textBox->Text			= System::Convert::ToString(fGaussSam);
}