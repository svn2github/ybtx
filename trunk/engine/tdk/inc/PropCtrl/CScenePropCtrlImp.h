#pragma once

#include "PropCtrl\CBasePropCtrlImp.h"

namespace sqr
{
	class CCamera;
}

namespace sqr_tools
{
	class CEditContext;
	class CScenePropCtrlImp :public CBasePropCtrlImp
	{
	public:
		CScenePropCtrlImp(void);
		~CScenePropCtrlImp(void);

	public:
		virtual string	GetSceneName();
		virtual int		GetSceneWidth();
		virtual int		GetSceneHeight();
		virtual int		GetSceneRegion();
		//获取和设置全局相机属性
		virtual int		GetCameraType(int& cam_type);
		virtual float	GetCameraPosX();
		virtual float	GetCameraPosY();
		virtual float	GetCameraPosZ();
		virtual float	GetCameraDirX();
		virtual float	GetCameraDirY();
		virtual float	GetCameraDirZ();
		virtual float	GetCameraTargetX();
		virtual float	GetCameraTargetY();
		virtual float	GetCameraTargetZ();
		virtual float	GetCameraFarDis();
		virtual float	GetCameraNearDis();
		//获取和设置全局雾效果属性
		virtual int		GetFogType();
		virtual float	GetFogBegin();
		virtual float	GetFogEnd();
		virtual float	GetFogDensity();
		virtual unsigned int	GetFogColor();
		//获取全局类光照(方向光 环境光)
		virtual float	GetSunLightDirX();
		virtual float	GetSunLightDirY();
		virtual float	GetSunLightDirZ();
		virtual unsigned int	GetSunLightColor();
		virtual unsigned int	GetAmbientColor();
		virtual unsigned int	GetPlayerSunLightColor();
		virtual unsigned int	GetPlayerAmbientColor();
		//设置全局相机属性
		virtual void SetCameraType(int cam_type);
		//设置全局雾效果属性
		virtual void SetFogType(int fog_type);
		virtual void SetFogBegin(float begin);
		virtual void SetFogEnd(float end);
		virtual void SetFogDensity(float destiny);
		virtual void SetFogColor(unsigned int color);
		//设置全局类光照(方向光 环境光)
		virtual void SetSunLightDir(float dx,float dy,float dz);
		virtual void SetSunLightColor(unsigned int color);
		virtual void SetAmbientColor(unsigned int color);
		virtual void SetPlayerSunLightColor(unsigned int color);
		virtual void SetPlayerAmbientColor(unsigned int color);
	protected:
		CCamera*	Camera;
	};

//!namespace
}