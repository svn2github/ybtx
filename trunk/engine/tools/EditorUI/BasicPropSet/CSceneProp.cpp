#include "stdafx.h"
#include "CSceneProp.h"
#include "PropCtrl\CPropCtrlImp.h"
#include "PropCtrl\CScenePropCtrlImp.h"
#include "dthelp.h"

#define PropImp ((CPropCtrlImp*)this->GetCoreCtrl())

CSceneProp::CSceneProp(void* ctrl)
:CBaseProp(ctrl)
{
	m_LastSceneRegion=-1;
	m_SceneRegion=0;
}

CSceneProp::~CSceneProp()
{
}

void CSceneProp::UpdateTimerProp()
{
	if(PropImp && PropImp->GetScenePropImp())
	{
		CScenePropCtrlImp* sceneImp=PropImp->GetScenePropImp();
		//场景
		m_SceneRegion=sceneImp->GetSceneRegion();
		
		m_CamPosX=ShowModify( sceneImp->GetCameraPosX() );
		m_CamPosY=ShowModify( sceneImp->GetCameraPosY() );
		m_CamPosZ=ShowModify( sceneImp->GetCameraPosZ() );
		m_CamDirX=ShowModify( sceneImp->GetCameraDirX() );
		m_CamDirY=ShowModify( sceneImp->GetCameraDirY() );
		m_CamDirZ=ShowModify( sceneImp->GetCameraDirZ() );
		m_CamTargetX=ShowModify( sceneImp->GetCameraTargetX() );
		m_CamTargetY=ShowModify( sceneImp->GetCameraTargetY() );
		m_CamTargetZ=ShowModify( sceneImp->GetCameraTargetZ() );
		m_CamFarDis=ShowModify( sceneImp->GetCameraFarDis() );
		m_CamNearDis=ShowModify( sceneImp->GetCameraNearDis() );
		//if(m_LastSceneRegion!=m_SceneRegion)
		{
			m_LastSceneRegion=m_SceneRegion;
			std::string tmpName=sceneImp->GetSceneName();
			m_SceneName=UnmagStringToMagString(tmpName.c_str());
			m_Width=sceneImp->GetSceneWidth();
			m_Height=sceneImp->GetSceneHeight();
			//太阳光
			m_AmbientColor=m_AmbientColor.FromArgb(sceneImp->GetAmbientColor());
			m_SunColor=m_SunColor.FromArgb(sceneImp->GetSunLightColor());
			m_PlayerAmbientColor=m_PlayerAmbientColor.FromArgb(sceneImp->GetPlayerAmbientColor());
			m_PlayerSunColor=m_PlayerSunColor.FromArgb(sceneImp->GetPlayerSunLightColor());

			m_SunDirX=ShowModify( sceneImp->GetSunLightDirX() );
			m_SunDirY=ShowModify( sceneImp->GetSunLightDirY() );
			m_SunDirZ=ShowModify( sceneImp->GetSunLightDirZ() );
			//雾效果
			int tmpType=sceneImp->GetFogType();
			if(tmpType==0)
				m_FogType=CSceneProp::FOGMODE::FOG_LINEAR;
			else
				m_FogType=CSceneProp::FOGMODE::FOG_EXPONENT;
			m_FogColor=m_FogColor.FromArgb(sceneImp->GetFogColor());
			m_Begin=ShowModify( sceneImp->GetFogBegin() );
			m_End=ShowModify( sceneImp->GetFogEnd() );
			m_Density=ShowModify( sceneImp->GetFogDensity() );
		}
	}
}

void CSceneProp::UpdateProp()
{
		if(PropImp && PropImp->GetScenePropImp())
	{
		CScenePropCtrlImp* sceneImp=PropImp->GetScenePropImp();
		//场景
		m_SceneRegion=sceneImp->GetSceneRegion();
		std::string tmpName=sceneImp->GetSceneName();
		m_SceneName=UnmagStringToMagString(tmpName.c_str());
		m_Width=sceneImp->GetSceneWidth();
		m_Height=sceneImp->GetSceneHeight();
		//相机
		m_CamPosX=ShowModify( sceneImp->GetCameraPosX() );
		m_CamPosY=ShowModify( sceneImp->GetCameraPosY() );
		m_CamPosZ=ShowModify( sceneImp->GetCameraPosZ() );
		m_CamDirX=ShowModify( sceneImp->GetCameraDirX() );
		m_CamDirY=ShowModify( sceneImp->GetCameraDirY() );
		m_CamDirZ=ShowModify( sceneImp->GetCameraDirZ() );
		m_CamTargetX=ShowModify( sceneImp->GetCameraTargetX() );
		m_CamTargetY=ShowModify( sceneImp->GetCameraTargetY() );
		m_CamTargetZ=ShowModify( sceneImp->GetCameraTargetZ() );
		m_CamFarDis=ShowModify( sceneImp->GetCameraFarDis() );
		m_CamNearDis=ShowModify( sceneImp->GetCameraNearDis() );
		//太阳光
		m_AmbientColor.FromArgb(sceneImp->GetAmbientColor());
		m_SunColor.FromArgb(sceneImp->GetSunLightColor());
		m_PlayerAmbientColor.FromArgb(sceneImp->GetPlayerAmbientColor());
		m_PlayerSunColor.FromArgb(sceneImp->GetPlayerSunLightColor());
		m_SunDirX=ShowModify( sceneImp->GetSunLightDirX() );
		m_SunDirY=ShowModify( sceneImp->GetSunLightDirY() );
		m_SunDirZ=ShowModify( sceneImp->GetSunLightDirZ() );
		//雾效果
		int tmpType=sceneImp->GetFogType();
		if(tmpType==0)
			m_FogType=CSceneProp::FOGMODE::FOG_LINEAR;
		else
			m_FogType=CSceneProp::FOGMODE::FOG_EXPONENT;
		m_FogColor.FromArgb(sceneImp->GetFogColor());
		m_Begin=ShowModify( sceneImp->GetFogBegin() );
		m_End=ShowModify( sceneImp->GetFogEnd() );
		m_Density=ShowModify( sceneImp->GetFogDensity() );

	}
}

//场景
System::String^  CSceneProp::SceneName::get()
{
	return m_SceneName;
}
void CSceneProp::SceneName::set(System::String^  value)
{
	m_SceneName=value;
}
int  CSceneProp::SceneWidth::get()
{
	return m_Width;
}
void CSceneProp::SceneWidth::set(int value)
{
	m_Width=value;
}
int  CSceneProp::SceneHeight::get()
{
	return m_Height;
}
void CSceneProp::SceneHeight::set(int value)
{
	m_Height=value;
}
int  CSceneProp::SceneRegion::get()
{
	return m_SceneRegion;
}
void CSceneProp::SceneRegion::set(int value)
{
	m_SceneRegion=value;
}
//相机
int CSceneProp::CamType::get()
{
	return m_CamType;
}
void CSceneProp::CamType::set(int value)
{
	m_CamType=value;
}
//CamPos
float  CSceneProp::CamPosX::get()
{
	return m_CamPosX;
}
void CSceneProp::CamPosX::set(float value)
{
	m_CamPosX=value;
}

float  CSceneProp::CamPosY::get()
{
	return m_CamPosY;
}
void CSceneProp::CamPosY::set(float value)
{
	m_CamPosY=value;
}

float  CSceneProp::CamPosZ::get()
{
	return m_CamPosZ;
}
void CSceneProp::CamPosZ::set(float value)
{
	m_CamPosZ=value;
}
//CamDir
float  CSceneProp::CamDirX::get()
{
	return m_CamDirX;
}
void CSceneProp::CamDirX::set(float value)
{
	m_CamDirX=value;
}

float  CSceneProp::CamDirY::get()
{
	return m_CamDirY;
}
void CSceneProp::CamDirY::set(float value)
{
	m_CamDirY=value;
}

float  CSceneProp::CamDirZ::get()
{
	return m_CamDirZ;
}
void CSceneProp::CamDirZ::set(float value)
{
	m_CamDirZ=value;
}
//CamTarget
float  CSceneProp::CamTargetX::get()
{
	return m_CamTargetX;
}
void CSceneProp::CamTargetX::set(float value)
{
	m_CamTargetX=value;
}

float  CSceneProp::CamTargetY::get()
{
	return m_CamTargetY;
}
void CSceneProp::CamTargetY::set(float value)
{
	m_CamTargetY=value;
}

float  CSceneProp::CamTargetZ::get()
{
	return m_CamTargetZ;
}
void CSceneProp::CamTargetZ::set(float value)
{
	m_CamTargetZ=value;
}
//Cam Far Near
float  CSceneProp::CamFarDis::get()
{
	return m_CamFarDis;
}
void CSceneProp::CamFarDis::set(float value)
{
	m_CamFarDis=value;
}
float  CSceneProp::CamNearDis::get()
{
	return m_CamNearDis;
}
void CSceneProp::CamNearDis::set(float value)
{
	m_CamNearDis=value;
}
//Light
System::Drawing::Color  CSceneProp::AmbientColor::get()
{
	return m_AmbientColor;
}
void CSceneProp::AmbientColor::set(System::Drawing::Color value)
{
	m_AmbientColor=value;
	if(PropImp && PropImp->GetScenePropImp())
	{
		CScenePropCtrlImp* sceneImp=PropImp->GetScenePropImp();
		unsigned int tmpColor=m_AmbientColor.ToArgb();
		sceneImp->SetAmbientColor( tmpColor );
	}
}

System::Drawing::Color  CSceneProp::PlayerAmbientColor::get()
{
	return m_PlayerAmbientColor;
}
void CSceneProp::PlayerAmbientColor::set(System::Drawing::Color value)
{
	m_PlayerAmbientColor=value;
	if(PropImp && PropImp->GetScenePropImp())
	{
		CScenePropCtrlImp* sceneImp=PropImp->GetScenePropImp();
		unsigned int tmpColor=m_PlayerAmbientColor.ToArgb();
		sceneImp->SetPlayerAmbientColor( tmpColor );
	}
}

System::Drawing::Color  CSceneProp::PlayerSunColor::get()
{
	return m_PlayerSunColor;
}
void CSceneProp::PlayerSunColor::set(System::Drawing::Color value)
{
	m_PlayerSunColor=value;
	if(PropImp && PropImp->GetScenePropImp())
	{
		CScenePropCtrlImp* sceneImp=PropImp->GetScenePropImp();
		unsigned int tmpColor=m_PlayerSunColor.ToArgb();
		sceneImp->SetPlayerSunLightColor( tmpColor );
	}
}
System::Drawing::Color  CSceneProp::SunColor::get()
{
	return m_SunColor;
}
void CSceneProp::SunColor::set(System::Drawing::Color value)
{
	m_SunColor=value;
	if(PropImp && PropImp->GetScenePropImp())
	{
		CScenePropCtrlImp* sceneImp=PropImp->GetScenePropImp();
		unsigned int tmpColor=m_SunColor.ToArgb();
		sceneImp->SetSunLightColor( tmpColor );
	}
}
float  CSceneProp::SunDirX::get()
{
	return m_SunDirX;
}
void CSceneProp::SunDirX::set(float value)
{
	m_SunDirX=value;
	if(PropImp && PropImp->GetScenePropImp())
	{
		CScenePropCtrlImp* sceneImp=PropImp->GetScenePropImp();
		m_SunDirY=sceneImp->GetSunLightDirY();
		m_SunDirZ=sceneImp->GetSunLightDirZ();
		sceneImp->SetSunLightDir(m_SunDirX,m_SunDirY,m_SunDirZ);
	}
}
float  CSceneProp::SunDirY::get()
{
	return m_SunDirY;
}
void CSceneProp::SunDirY::set(float value)
{
	m_SunDirY=value;
	if(PropImp && PropImp->GetScenePropImp())
	{
		CScenePropCtrlImp* sceneImp=PropImp->GetScenePropImp();
		m_SunDirX=sceneImp->GetSunLightDirX();
		m_SunDirZ=sceneImp->GetSunLightDirZ();
		sceneImp->SetSunLightDir(m_SunDirX,m_SunDirY,m_SunDirZ);
	}
}
float  CSceneProp::SunDirZ::get()
{
	return m_SunDirZ;
}
void CSceneProp::SunDirZ::set(float value)
{
	m_SunDirZ=value;
	if(PropImp && PropImp->GetScenePropImp())
	{
		CScenePropCtrlImp* sceneImp=PropImp->GetScenePropImp();
		m_SunDirX=sceneImp->GetSunLightDirX();
		m_SunDirY=sceneImp->GetSunLightDirY();
		sceneImp->SetSunLightDir(m_SunDirX,m_SunDirY,m_SunDirZ);
	}
}

//Fog
//类型
CSceneProp::FOGMODE  CSceneProp::FogType::get()
{
	return m_FogType;
}
void CSceneProp::FogType::set(FOGMODE value)
{
	m_FogType=value;
}
//颜色
System::Drawing::Color  CSceneProp::FogColor::get()
{
	return m_FogColor;
}
void CSceneProp::FogColor::set(System::Drawing::Color value)
{
	m_FogColor=value;
	if(PropImp && PropImp->GetScenePropImp())
	{
		CScenePropCtrlImp* sceneImp=PropImp->GetScenePropImp();
		unsigned int tmpColor=m_FogColor.ToArgb();
		sceneImp->SetFogColor( tmpColor );
	}
}
//开始
float  CSceneProp::FogBegin::get()
{
	return m_Begin;
}
void CSceneProp::FogBegin::set(float value)
{
	m_Begin=value;
}
//结束
float  CSceneProp::FogEnd::get()
{
	return m_End;
}
void CSceneProp::FogEnd::set(float value)
{
	m_End=value;
}
//密度
float  CSceneProp::FogDensity::get()
{
	return m_Density;
}
void CSceneProp::FogDensity::set(float value)
{
	m_Density=value;
}




