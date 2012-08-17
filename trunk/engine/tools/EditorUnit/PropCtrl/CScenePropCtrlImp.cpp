#include "StdAfx.h"
#include "PropCtrl\CScenePropCtrlImp.h"
#include "CWindowTarget.h"
#include "CRenderGroup.h"
#include "CCamera.h"
#include "CCameraController.h"
#include "CEditContext.h"

CScenePropCtrlImp::CScenePropCtrlImp(void)
{
	m_PropType=PROP_SCENE;
}

CScenePropCtrlImp::~CScenePropCtrlImp(void)
{
}

string	CScenePropCtrlImp::GetSceneName()
{
	string name="default Scene";
	return name;
}
int		CScenePropCtrlImp::GetSceneWidth()
{
	if(m_pOwnContext)
	{
	}
	return 0;
}
int		CScenePropCtrlImp::GetSceneHeight()
{
	return 0;
}
int		CScenePropCtrlImp::GetSceneRegion()
{
	return 0;
}

int		CScenePropCtrlImp::GetCameraType(int& cam_type)
{
	return 0;
}
float	CScenePropCtrlImp::GetCameraPosX()
{
	float px=0;
	if(m_pOwnContext && m_pOwnContext->GetWindowTarget() && m_pOwnContext->GetWindowTarget()->GetGroup())
	{
		Camera=m_pOwnContext->GetWindowTarget()->GetGroup()->GetCamera();
		if(Camera)
			px=Camera->GetPosition().x;
	}
	return px;
}
float	CScenePropCtrlImp::GetCameraPosY()
{
	float py=0;
	if(m_pOwnContext && m_pOwnContext->GetWindowTarget() && m_pOwnContext->GetWindowTarget()->GetGroup())
	{
		Camera=m_pOwnContext->GetWindowTarget()->GetGroup()->GetCamera();
		if(Camera)
			py=Camera->GetPosition().y;
	}
	return py;
}
float	CScenePropCtrlImp::GetCameraPosZ()
{
	float pz=0;
	if(m_pOwnContext && m_pOwnContext->GetWindowTarget() && m_pOwnContext->GetWindowTarget()->GetGroup())
	{
		Camera=m_pOwnContext->GetWindowTarget()->GetGroup()->GetCamera();
		if(Camera)
			pz=Camera->GetPosition().z;
	}
	return pz;
}
float	CScenePropCtrlImp::GetCameraDirX()
{
	float dx=1.0f;
	if(m_pOwnContext && m_pOwnContext->GetWindowTarget() && m_pOwnContext->GetWindowTarget()->GetGroup())
	{
		Camera=m_pOwnContext->GetWindowTarget()->GetGroup()->GetCamera();
		if(Camera)
			dx=Camera->getDIRECTION().x;
	}
	return dx;
}
float	CScenePropCtrlImp::GetCameraDirY()
{
	float dy=1.0f;
	if(m_pOwnContext && m_pOwnContext->GetWindowTarget() && m_pOwnContext->GetWindowTarget()->GetGroup())
	{
		Camera=m_pOwnContext->GetWindowTarget()->GetGroup()->GetCamera();
		if(Camera)
			dy=Camera->getDIRECTION().y;
	}
	return dy;
}
float	CScenePropCtrlImp::GetCameraDirZ()
{
	float dz=1.0f;
	if(m_pOwnContext && m_pOwnContext->GetWindowTarget() && m_pOwnContext->GetWindowTarget()->GetGroup())
	{
		Camera=m_pOwnContext->GetWindowTarget()->GetGroup()->GetCamera();
		if(Camera)
			dz=Camera->getDIRECTION().z;
	}
	return dz;
}
float	CScenePropCtrlImp::GetCameraTargetX()
{
	float tx=0.0f;
	if(m_pOwnContext && m_pOwnContext->GetCurCamCtrller())
	{
		CCameraController*	ctrl=m_pOwnContext->GetCurCamCtrller();
		if(ctrl)
			tx=ctrl->GetTargetPos().x;
	}
	return tx;
}
float	CScenePropCtrlImp::GetCameraTargetY()
{
	float ty=0.0f;
	if(m_pOwnContext && m_pOwnContext->GetCurCamCtrller())
	{
		CCameraController*	ctrl=m_pOwnContext->GetCurCamCtrller();
		if(ctrl)
			ty=ctrl->GetTargetPos().y;
	}
	return ty;
}
float	CScenePropCtrlImp::GetCameraTargetZ()
{
	float tz=0.0f;
	if(m_pOwnContext && m_pOwnContext->GetCurCamCtrller())
	{
		CCameraController*	ctrl=m_pOwnContext->GetCurCamCtrller();
		if(ctrl)
			tz=ctrl->GetTargetPos().z;
	}
	return tz;
}
float	CScenePropCtrlImp::GetCameraFarDis()
{
	float farDis=0.0f;
	if(m_pOwnContext && m_pOwnContext->GetWindowTarget() && m_pOwnContext->GetWindowTarget()->GetGroup())
	{
		Camera=m_pOwnContext->GetWindowTarget()->GetGroup()->GetCamera();
		if(Camera)
			farDis=Camera->getFarDist();
	}
	return farDis;
}
float	CScenePropCtrlImp::GetCameraNearDis()
{
	float nearDis=0.0f;
	if(m_pOwnContext && m_pOwnContext->GetWindowTarget() && m_pOwnContext->GetWindowTarget()->GetGroup())
	{
		Camera=m_pOwnContext->GetWindowTarget()->GetGroup()->GetCamera();
		if(Camera)
			nearDis=Camera->getNearDist();
	}
	return nearDis;
}
//获取和设置全局雾效果属性
int	CScenePropCtrlImp::GetFogType()
{
	int fog_type=-1;
	if(m_pOwnContext && m_pOwnContext->GetWindowTarget() && m_pOwnContext->GetWindowTarget()->GetGroup())
	{
		CRenderGroup* m_pRenderGroup=m_pOwnContext->GetWindowTarget()->GetGroup();
		fog_type=m_pRenderGroup->GetFogType();
	}
	return fog_type;
}
unsigned int CScenePropCtrlImp::GetFogColor()
{
	unsigned int color=0xff0000ff;
	if(m_pOwnContext)
		color=m_pOwnContext->GetFogColor();
	return color;
}
float CScenePropCtrlImp::GetFogBegin()
{
	float value=0;
	if(m_pOwnContext)
		value=m_pOwnContext->GetFogStart();
	return value;
}
float CScenePropCtrlImp::GetFogEnd()
{
	float value=0;
	if(m_pOwnContext)
		value=m_pOwnContext->GetFogEnd();
	return value;
}
float	CScenePropCtrlImp::GetFogDensity()
{
	float value=0;
	if(m_pOwnContext)
		value=m_pOwnContext->GetFogEnd();
	return value;
}
//获取全局类光照(方向光 环境光)
float	CScenePropCtrlImp::GetSunLightDirX()
{
	return m_pOwnContext->GetSunLightDir().x;
}
float	CScenePropCtrlImp::GetSunLightDirY()
{
	return m_pOwnContext->GetSunLightDir().y;
}
float	CScenePropCtrlImp::GetSunLightDirZ()
{
	return m_pOwnContext->GetSunLightDir().z;
}
unsigned int	CScenePropCtrlImp::GetSunLightColor() //太阳光
{
	CColor value=m_pOwnContext->GetSunLightColor();
	unsigned int color=value.dwColor;
	return color;
}
unsigned int	CScenePropCtrlImp::GetAmbientColor() //环境光
{
	CColor value=m_pOwnContext->GetAmbientColor();
	unsigned int color=value.dwColor;
	return color;
}
unsigned int	CScenePropCtrlImp::GetPlayerSunLightColor() //角色太阳光
{
	CColor value=m_pOwnContext->GetPlayerSunColor();
	unsigned int color=value.dwColor;
	return color;
}
unsigned int	CScenePropCtrlImp::GetPlayerAmbientColor() //角色环境光 
{
	CColor value=m_pOwnContext->GetPlayerAmbientColor();
	unsigned int color=value.dwColor;
	return color;
}
//获取和设置全局相机属性
void CScenePropCtrlImp::SetCameraType(int cam_type)
{
}

//获取和设置全局雾效果属性
void CScenePropCtrlImp::SetFogType(int fog_type)
{
}
void CScenePropCtrlImp::SetFogColor(unsigned int color)
{
	m_pOwnContext->SetFogColor(color);
}
void CScenePropCtrlImp::SetFogBegin(float begin)
{
	m_pOwnContext->SetFogStart(begin);
}
void CScenePropCtrlImp::SetFogEnd(float end)
{
	m_pOwnContext->SetFogEnd(end);
}
void CScenePropCtrlImp::SetFogDensity(float destiny)
{
	//密度 错误数值
	m_pOwnContext->SetFogEnd(0.0f);
}
//获取全局类光照(方向光 环境光)
void CScenePropCtrlImp::SetSunLightDir(float dx,float dy,float dz)
{
	m_pOwnContext->SetSunLightDir(CVector3f(dx,dy,dz));
}
void CScenePropCtrlImp::SetSunLightColor(unsigned int color)
{
	m_pOwnContext->SetSunLightColor(color);
}
void CScenePropCtrlImp::SetAmbientColor(unsigned int color)
{
	m_pOwnContext->SetAmbientColor(color);
}
void CScenePropCtrlImp::SetPlayerSunLightColor(unsigned int color)
{
	m_pOwnContext->SetPlayerSunColor(color);
}
void CScenePropCtrlImp::SetPlayerAmbientColor(unsigned int color)
{
	m_pOwnContext->SetPlayerAmbientColor(color);
}
