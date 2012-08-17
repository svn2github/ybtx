#include "StdAfx.h"
#include "AmbientAudioCtrl.h"
#include "AudioViewCtrl/AmbientAudioCtrlImp.h"

System::Void AmbientAudioCtrl::playtimeMin_textBox_TextChanged( System::Object^ sender, System::EventArgs^ e )
{
	CAmbientAudioCtrlImp *pImp = (CAmbientAudioCtrlImp*)m_pCtrl;
	if( pImp )
	{
		String^ strPlayTimeMin = this->playtimeMin_textBox->Text;
		if( strPlayTimeMin == "" )
			return;

		int playTimeMin = Convert::ToInt32(strPlayTimeMin);

		int nProIndex = this->pro_comboBox->SelectedIndex;
		pImp->SetCuePlayTimeLenMin(nProIndex, playTimeMin);
	}
}

System::Void AmbientAudioCtrl::playtimeMax_textBox_TextChanged( System::Object^ sender, System::EventArgs^ e )
{
	CAmbientAudioCtrlImp *pImp = (CAmbientAudioCtrlImp*)m_pCtrl;
	if( pImp )
	{
		String^ strPlayTimeMax = this->playtimeMax_textBox->Text;
		if( strPlayTimeMax == "" )
			return;

		int playTimeMax = Convert::ToInt32(strPlayTimeMax);

		int nProIndex = this->pro_comboBox->SelectedIndex;
		pImp->SetCuePlayTimeLenMax(nProIndex, playTimeMax);
	}
}

System::Void AmbientAudioCtrl::intervaltimeMin_textBox_TextChanged( System::Object^ sender, System::EventArgs^ e )
{
	CAmbientAudioCtrlImp *pImp = (CAmbientAudioCtrlImp*)m_pCtrl;
	if( pImp )
	{
		String^ strIntervalTimeMin = this->intervaltimeMin_textBox->Text;
		if( strIntervalTimeMin == "" )
			return;

		int intervalTimeMin = Convert::ToInt32(strIntervalTimeMin);

		int nProIndex = this->pro_comboBox->SelectedIndex;
		pImp->SetCueIntervalTimeLenMin(nProIndex, intervalTimeMin);
	}
}

System::Void AmbientAudioCtrl::intervaltimeMax_textBox_TextChanged( System::Object^ sender, System::EventArgs^ e )
{
	CAmbientAudioCtrlImp *pImp = (CAmbientAudioCtrlImp*)m_pCtrl;
	if( pImp )
	{
		String^ strIntervalTimeMax = this->intervaltimeMax_textBox->Text;
		if( strIntervalTimeMax == "" )
			return;

		int intervalTimeMax = Convert::ToInt32(strIntervalTimeMax);

		int nProIndex = this->pro_comboBox->SelectedIndex;
		pImp->SetCueIntervalTimeLenMax(nProIndex, intervalTimeMax);
	}
}

System::Void AmbientAudioCtrl::clearPro_button_Click( System::Object^ sender, System::EventArgs^ e )
{
	this->playtimeMax_textBox->Text = "";
	this->playtimeMin_textBox->Text = "";
	this->intervaltimeMax_textBox->Text = "";
	this->intervaltimeMin_textBox->Text = "";
}

System::Void AmbientAudioCtrl::pro_comboBox_SelectedIndexChanged( System::Object^ sender, System::EventArgs^ e )
{
	int nProIndex = this->pro_comboBox->SelectedIndex;
	CAmbientAudioCtrlImp *pImp = (CAmbientAudioCtrlImp*)m_pCtrl;
	if( pImp )
	{
		Int32 nPlayTimeMax = pImp->GetCuePlayTimeLenMax(nProIndex);
		this->playtimeMax_textBox->Text = nPlayTimeMax.ToString();

		Int32 nPlayTimeMin = pImp->GetCuePlayTimeLenMin(nProIndex);
		this->playtimeMin_textBox->Text = nPlayTimeMin.ToString();

		Int32 nIntervalTimeMax = pImp->GetCueIntervalTimeLenMax(nProIndex);
		this->intervaltimeMax_textBox->Text = nIntervalTimeMax.ToString();

		Int32 nIntervalTimeMin = pImp->GetCueIntervalTimeLenMin(nProIndex);
		this->intervaltimeMin_textBox->Text = nIntervalTimeMin.ToString();
	}
}

System::Void AmbientAudioCtrl::changePro_button_Click( System::Object^ sender, System::EventArgs^ e )
{

}

System::Void AmbientAudioCtrl::range_comboBox_SelectedIndexChanged( System::Object^ sender, System::EventArgs^ e )
{

}

System::Void AmbientAudioCtrl::changeRange_button_Click( System::Object^ sender, System::EventArgs^ e )
{

}

void AmbientAudioCtrl::Update( void )
{
	CAmbientAudioCtrlImp *pImp = (CAmbientAudioCtrlImp*)m_pCtrl;
	if( pImp )
	{
	}
}

char* AmbientAudioCtrl::GetImpTypeName( void )
{
	return GET_IMP_NAME(CAmbientAudioCtrlImp);
}

void* AmbientAudioCtrl::GetImpParam( void )
{
	return this->Handle.ToPointer();
}

System::Void AmbientAudioCtrl::AmbientAudioCtrl_Load( System::Object^ sender, System::EventArgs^ e )
{
	InitCueProerty();
}

void AmbientAudioCtrl::InitCueProerty()
{
	CAmbientAudioCtrlImp *pImp = (CAmbientAudioCtrlImp*)m_pCtrl;
	if( pImp )
	{
		pImp->LoadCueTimeConfig();

		size_t cueAttributeCount = pImp->GetCueAttributeCount();
		if( cueAttributeCount != 0 )
		{
			for ( size_t m = 0; m < cueAttributeCount; ++m )
			{
				Int32 index = m;
				this->pro_comboBox->Items->Add(index.ToString());
			}
			this->pro_comboBox->SelectedIndex = 0;
		}

		size_t rangeCount = pImp->GetAudioRangeCount();
		if( rangeCount != 0 )
		{
			for ( size_t m = 0; m < rangeCount; ++m )
			{
				int range = pImp->GetAudioRange(m);
				Int32 index = range;
				this->range_comboBox->Items->Add(index.ToString());
			}
			this->range_comboBox->SelectedIndex = 0;
		}
	}
}