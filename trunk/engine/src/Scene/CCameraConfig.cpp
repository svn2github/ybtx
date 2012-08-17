#include "stdafx.h"
#include "CCameraConfig.h"
#include "basehelper.h"
#include "CodeCvs.h"
#include "CRTSCameraCtrller.h"
#include "CTPSCameraCtrller.h"
#include "CXmlConfig.inl"
#include "CMath.h"
#include "CSceneConfig.h"

CCameraConfig::CCameraConfig(istream& strmConfig)
{
	m_DefaultMode = ECam_Mode16_9;
	m_pConfig = NULL;
	m_pConfig = new CXmlConfig( "main_config" , strmConfig );
	m_fCamMinSpeed	= m_pConfig->DftGet( 0.05f , "common","minspeed" );
	m_fCamDefDist	= m_pConfig->DftGet( 26.0f , "common","defdist" );
}

CCameraConfig::~CCameraConfig(void)
{
	delete m_pConfig;
}

void CCameraConfig::SetCameraInfo(CRenderScene* scene, ECamMode c)
{
	if (!scene)
		return;
	CRTSCameraCtrller* pRtsCam = scene->GetRTSCamCtrller();
	CTPSCameraCtrller* pTpsCam = scene->GetTPSCamCtrller();
	if ((!pRtsCam) || (!pTpsCam))
		return;
	pRtsCam->SetMaxXZ_Dist( GetCamMaxHOffset(c) * eGridSpan );
	pRtsCam->SetMinDist( GetCamMinDist(c) * eGridSpan );
	pRtsCam->SetMaxDist( GetCamMaxDist(c) * eGridSpan );
	pRtsCam->SetDist(	GetCamDefDist(c)* eGridSpan );
	pRtsCam->SetMaxRadian( GetCamMaxXRadian(c) );
	pRtsCam->SetMinRadian( GetCamMinXRadian(c) );
	pRtsCam->SetMinFov( GetCamMinFovRadian(c) );
	pRtsCam->SetMaxFov( GetCamMaxFovRadian(c) );
	pRtsCam->SetMinYOffset( GetCamMinYOffset(c) * eGridSpan );
	pRtsCam->SetMaxYOffset( GetCamMaxYOffset(c) * eGridSpan );
	pRtsCam->SetYRadian( GetCamYRadian(c) );
	pRtsCam->SetMaxNearPlan( GetCamMaxNearPlan(c) * eGridSpan );
	pRtsCam->SetMinNearPlan( GetCamMinNearPlan(c) * eGridSpan );
	pRtsCam->SetDistSpeed( GetCamSpeed(c) * eGridSpan );
	pRtsCam->ReSetPosNow();

	pTpsCam->SetMinTargetDist(GetCamMinDist(c) * eGridSpan);
	pTpsCam->SetMaxTargetDist(GetCamMaxDist(c) * eGridSpan * 2);
	pTpsCam->SetTargetDist( GetCamDefDist(c) * eGridSpan);
	pTpsCam->SetMinFov(GetCamMinFovRadian(c));
	pTpsCam->SetMaxFov(GetCamMaxFovRadian(c));
	pTpsCam->SetMaxXDegree( CMath::half_pi - 0.1f );//CSceneConfig::Inst()->GetCamMaxXRadian() );
	pTpsCam->SetMinXDegree( 0.1f );//CSceneConfig::Inst()->GetCamMinXRadian() );
	pTpsCam->SetXRotateDegree(GetCamMaxXRadian(c));
	pTpsCam->SetMaxYDegree( CMath::two_pi );
	if(c!=ECam_ModeDefault)
		m_DefaultMode = c;
	
	scene->OnRTSConfigChanged(GetCamDefDist(c) * eGridSpan);
	pRtsCam->DoQueryCtrl(0,NULL);
}

static char* aryConfigNode[]=
{
	"camera_config_16_9",
	"camera_config_16_10",
	"camera_config_4_3",
	"camera_config_5_4"
};

float	CCameraConfig::GetCamDefDist(ECamMode e)		const	
{
	if(e==ECam_ModeDefault)
		e = m_DefaultMode;
	return Get<float>( 22.0f, aryConfigNode[e] , "defdist");
}

//------------------------------------------------------------------------------
float CCameraConfig::GetCamMaxXRadian(ECamMode e)const
{
	if(e==ECam_ModeDefault)
		e = m_DefaultMode;
	return Get<float>( 45.0f, aryConfigNode[e] , "far_dist","xdegree" ) * CMath::pi/180;
}

//------------------------------------------------------------------------------
float CCameraConfig::GetCamMinXRadian(ECamMode e)const
{
	if(e==ECam_ModeDefault)
		e = m_DefaultMode;
	return Get( 23.0f , aryConfigNode[e],"near_dist","xdegree" ) * CMath::pi/180;
}

//------------------------------------------------------------------------------
float CCameraConfig::GetCamYRadian(ECamMode e)const
{
	if(e==ECam_ModeDefault)
		e = m_DefaultMode;
	return Get( 45.0f , aryConfigNode[e],"ydegree" ) * CMath::pi/180;
}

//------------------------------------------------------------------------------
float CCameraConfig::GetCamMaxFovRadian(ECamMode e)const
{
	if(e==ECam_ModeDefault)
		e = m_DefaultMode;
	return Get( 35.0f , aryConfigNode[e],"far_dist","fovdegree" ) * CMath::pi/180;
}

//------------------------------------------------------------------------------
float CCameraConfig::GetCamMinFovRadian(ECamMode e)const
{
	if(e==ECam_ModeDefault)
		e = m_DefaultMode;
	return Get( 25.0f , aryConfigNode[e],"near_dist","fovdegree" ) * CMath::pi/180;
}

//------------------------------------------------------------------------------
float CCameraConfig::GetCamMinDist(ECamMode e)const
{
	if(e==ECam_ModeDefault)
		e = m_DefaultMode;
	return Get( 26.0f , aryConfigNode[e],"near_dist","dist" );
}

//------------------------------------------------------------------------------
float CCameraConfig::GetCamMaxDist(ECamMode e)const
{
	if(e==ECam_ModeDefault)
		e = m_DefaultMode;
	return Get( 7.0f , aryConfigNode[e],"far_dist","dist" );
}


float CCameraConfig::GetCamMaxNearPlan(ECamMode e)const
{
	if(e==ECam_ModeDefault)
		e = m_DefaultMode;
	return Get( 0.1f , aryConfigNode[e],"far_dist","nearplan" );
}

float CCameraConfig::GetCamMinNearPlan(ECamMode e)const
{
	if(e==ECam_ModeDefault)
		e = m_DefaultMode;
	return Get( 0.1f , aryConfigNode[e],"near_dist","nearplan" );
}

float CCameraConfig::GetCamMaxHOffset(ECamMode e) const
{
	if(e==ECam_ModeDefault)
		e = m_DefaultMode;
	return Get( 0.4f , aryConfigNode[e],"maxhoffset" );
}


float CCameraConfig::GetCamSpeed(ECamMode e) const
{
	if(e==ECam_ModeDefault)
		e = m_DefaultMode;
	return Get( 8.0f , aryConfigNode[e],"camspeed" );
}

//------------------------------------------------------------------------------
float CCameraConfig::GetCamMinYOffset(ECamMode e)const
{
	if(e==ECam_ModeDefault)
		e = m_DefaultMode;
	return Get( -1.6f , aryConfigNode[e],"near_dist","yoffset" );
}

//------------------------------------------------------------------------------
float CCameraConfig::GetCamMaxYOffset(ECamMode e)const
{
	if(e==ECam_ModeDefault)
		e = m_DefaultMode;
	return Get( 0.7f , aryConfigNode[e],"far_dist","yoffset" );
}

// private get
template<typename ValueType>
ValueType CCameraConfig::Get(ValueType DefaultValue,const char* szElemName)const
{
	return m_pConfig->Get( DefaultValue, szElemName );
}

template<typename ValueType>
ValueType CCameraConfig::Get(ValueType DefaultValue,const char* szElemName1,const char* szElemName2)const
{
	return m_pConfig->DftGet( DefaultValue, szElemName1 , szElemName2 );
}

template<typename ValueType>
ValueType CCameraConfig::Get(ValueType DefaultValue,const char* szElemName1,const char* szElemName2,const char* szElemName3)const
{
	return m_pConfig->DftGet( DefaultValue, szElemName1 , szElemName2 , szElemName3 );
}

template<typename ValueType>
ValueType CCameraConfig::Get(const char* szElemName)const
{
	return m_pConfig->Get<ValueType>( szElemName );
}

template<typename ValueType>
ValueType CCameraConfig::Get(const char* szElemName1,const char* szElemName2)const
{
	return m_pConfig->Get<ValueType>( szElemName1 , szElemName2 );
}

template<typename ValueType>
ValueType CCameraConfig::Get(const char* szElemName1,const char* szElemName2,const char* szElemName3)const
{
	return m_pConfig->Get<ValueType>( szElemName1 , szElemName2 , szElemName3 );
}