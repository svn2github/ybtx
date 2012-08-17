#include "StdAfx.h"
#include "AudioViewCtrl/AudioViewCtrlImp.h"
#include "AudioViewCtrl/AmbientAudioCtrlImp.h"
#include "AudioViewCtrl/CueListCtrlImp.h"
#include "IAudioModule.h"

DEF_UI_IMP(CAudioViewCtrlImp);
CAudioViewCtrlImp::CAudioViewCtrlImp(void* param)
{
	m_pCueListCtrlImp		= NULL;
	m_pAmbientAudioCtrlImp	= NULL;
}

CAudioViewCtrlImp::~CAudioViewCtrlImp(void)
{
}

void CAudioViewCtrlImp::Update()
{
	CCtrlBase::Update();
}


bool CAudioViewCtrlImp::_ProcMsg( const CMsg& msg )
{
	return false;
}

void CAudioViewCtrlImp::SetContext( CEditContext* pContext )
{
	m_pOwnContext = pContext;
}

void CAudioViewCtrlImp::SetCueListCtrlImp( CCueListCtrlImp* pImp )
{
	m_pCueListCtrlImp = pImp;
}

void CAudioViewCtrlImp::SetAmbientAudioCtrlImp( CAmbientAudioCtrlImp* pImp )
{
	m_pAmbientAudioCtrlImp = pImp;
}

void CAudioViewCtrlImp::PlayCurrentCue()
{
	string strCurCueName;
	m_pCueListCtrlImp->GetCurSelectCueName(strCurCueName);
	if( strCurCueName != "" )
		PlayCue(strCurCueName.c_str());
}

void CAudioViewCtrlImp::StopCurrentCue()
{
	string strCurCueName;
	m_pCueListCtrlImp->GetCurSelectCueName(strCurCueName);
	if( strCurCueName != "" )
		PlayCue(strCurCueName.c_str());
}

void CAudioViewCtrlImp::CreateAmbientAudio()
{

}

