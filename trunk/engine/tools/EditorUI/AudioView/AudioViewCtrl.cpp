#include "StdAfx.h"
#include "AudioViewCtrl.h"
#include "AudioViewCtrl/AudioViewCtrlImp.h"
#include "AudioViewCtrl/AmbientAudioCtrlImp.h"
#include "AudioViewCtrl/CueListCtrlImp.h"

void AudioViewCtrl::Update( void )
{
	CAudioViewCtrlImp *pImp = (CAudioViewCtrlImp*)m_pCtrl;
	if( pImp )
	{
	}
}

char* AudioViewCtrl::GetImpTypeName( void )
{
	return GET_IMP_NAME(CAudioViewCtrlImp);
}

void* AudioViewCtrl::GetImpParam( void )
{
	return this->Handle.ToPointer();
}

System::Void AudioViewCtrl::AudioViewCtrl_Load( System::Object^ sender, System::EventArgs^ e )
{
	CAudioViewCtrlImp *pImp = (CAudioViewCtrlImp*)m_pCtrl;
	if( pImp )
	{
		CCueListCtrlImp* pCueListCtrlImp = (CCueListCtrlImp*)(this->cueListCtrl->GetCoreCtrl());
		if( pCueListCtrlImp )
		{
			pImp->SetCueListCtrlImp(pCueListCtrlImp);
		}

		CAmbientAudioCtrlImp* pAmbientAudioCtrlImp = (CAmbientAudioCtrlImp*)(this->ambientAudioCtrl->GetCoreCtrl());
		if( pAmbientAudioCtrlImp )
		{
			pImp->SetAmbientAudioCtrlImp(pAmbientAudioCtrlImp);
		}
	}
}

System::Void AudioViewCtrl::createAduio_button_Click( System::Object^ sender, System::EventArgs^ e )
{

}

System::Void AudioViewCtrl::on_radioButton_CheckedChanged( System::Object^ sender, System::EventArgs^ e )
{

}

System::Void AudioViewCtrl::off_radioButton_CheckedChanged( System::Object^ sender, System::EventArgs^ e )
{

}