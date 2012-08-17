#include "StdAfx.h"
#include "HDRSettingCtrl/CHDRSettingCtrlImp.h"
#include "CWindowTarget.h"
#include "CRenderTarget.h"

DEF_UI_IMP(CHDRSettingCtrlImp);
CHDRSettingCtrlImp::CHDRSettingCtrlImp(void* param)
{
	m_pTarget			= NULL;
	m_bTurnOnGassTarget = false;
	m_fGaussBrightPass	= 0.000f; //光通量 0~1
	m_fGaussMultiplier  = 0.35f; //泛光强度 0~2
	m_fGaussMean		= 0.292f; //泛光偏移 -1~+1
	m_fGaussStdDev		= 0.363f; //泛光融和程度  0~1
	m_fGaussExposure	= 1.00f; //最终亮度0~2
	m_fGaussSam			= 0.25f; //混合度	

	this->InitGaussParams();
}

CHDRSettingCtrlImp::~CHDRSettingCtrlImp(void)
{
}

void CHDRSettingCtrlImp::Update()
{
	CCtrlBase::Update();
}


void CHDRSettingCtrlImp::CreateBloomTarget()
{
	if( m_pTarget == NULL )
	{
		m_pTarget = CMainWindowTarget::GetInst()->GetTargetPipeline(); 
	}
}

void CHDRSettingCtrlImp::InitGaussParams()
{
	this->CreateBloomTarget();

	this->SetGaussBrightPass(m_fGaussBrightPass);
	this->SetGaussMultiplier(m_fGaussMultiplier);
	this->SetGaussMean(m_fGaussMean);
	this->SetGaussStdDev(m_fGaussStdDev);
	this->SetGaussExposure(m_fGaussExposure);
	this->SetGaussSample(m_fGaussSam);
}

bool CHDRSettingCtrlImp::HadBloomTarget()
{
	return m_pTarget != NULL;
}

void CHDRSettingCtrlImp::SetIsTurnOnGaussTarget( const bool bTurn )
{
	m_bTurnOnGassTarget = bTurn;

	if( m_pTarget )
		m_pTarget->SetTurnOn("Bloom", bTurn);
}

void CHDRSettingCtrlImp::SetIsTurnOnRefract( const bool bTurn )
{
	if( m_pTarget )
		m_pTarget->SetTurnOn("Refract", bTurn);
}

bool CHDRSettingCtrlImp::GetIsTurnOnGaussTarget() const
{
	return m_bTurnOnGassTarget;
}

void CHDRSettingCtrlImp::SetGaussBrightPass( const float fBrightPass )
{
	m_fGaussBrightPass = fBrightPass;
	if( m_pTarget )
		m_pTarget->setBrightPass(m_fGaussBrightPass);
}

float CHDRSettingCtrlImp::GetGaussBrightPass() const
{
	return m_fGaussBrightPass;
}

void CHDRSettingCtrlImp::SetGaussMultiplier( const float fGaussMultiplier )
{
	m_fGaussMultiplier = fGaussMultiplier;
	if( m_pTarget )
		m_pTarget->setGaussMultiplier(fGaussMultiplier);
}

float CHDRSettingCtrlImp::GetGaussMultiplier() const
{
	return m_fGaussMultiplier;
}

void CHDRSettingCtrlImp::SetGaussMean( const float fGaussMean )
{
	m_fGaussMean = fGaussMean;
	if( m_pTarget )
		m_pTarget->setMean(fGaussMean);
}

float CHDRSettingCtrlImp::GetGaussMean() const
{
	return m_fGaussMean;
}

void CHDRSettingCtrlImp::SetGaussStdDev( const float fGaussStdDev )
{
	m_fGaussStdDev = fGaussStdDev;
	if( m_pTarget )
		m_pTarget->setStdDev(fGaussStdDev);
}

float CHDRSettingCtrlImp::GetGaussStdDev() const
{
	return m_fGaussStdDev;
}

void CHDRSettingCtrlImp::SetGaussExposure( const float fExposure )
{
	m_fGaussExposure = fExposure;
	if( m_pTarget )
		m_pTarget->setExposure(fExposure);
}

float CHDRSettingCtrlImp::GetGaussExposure() const
{
	return m_fGaussExposure;
}

void CHDRSettingCtrlImp::SetGaussSample( const float fSam )
{
	m_fGaussSam = fSam;
	if( m_pTarget )
		m_pTarget->setSample(fSam);
}

float CHDRSettingCtrlImp::GetGaussSample() const
{
	return m_fGaussSam;
}

void CHDRSettingCtrlImp::UpdateGaussParams()
{
	if( m_pTarget )
		m_pTarget->InitParams();
}


void CHDRSettingCtrlImp::SaveGaussParamsInfo( FILE * fp )
{
	fwrite(&(this->m_fGaussBrightPass), sizeof(float), 6, fp);
}

void CHDRSettingCtrlImp::LoadGaussParamsInfo( FILE * fp )
{
	fread(&(this->m_fGaussBrightPass), sizeof(float), 6, fp);
}

bool CHDRSettingCtrlImp::_ProcMsg( const CMsg& msg )
{
	switch(msg.msgSource)
	{
	case MGS_CUSTOM:
		{
			switch(msg.msgType.custom_type)
			{
			case MGT_UPDATE_PARA:
				{
					this->UpdateGaussParams();
					break;
				}

			default:
				break;
			}
		}
	}
	return false;
}