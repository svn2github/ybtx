#pragma once

#include "CCameraController.h"

namespace sqr
{

//控制相机位置和方向 确定Target

enum FPS_STATE
{
	STATE_MOVE_LEFT,
	STATE_MOVE_RIGHT,
	STATE_MOVE_FORWAR,
	STATE_MOVE_BACK,
	STATE_PULL,
	STATE_PUSH,
	STATE_ROT,
	STATE_STOP
};

class CFPSCameraCtrller :public CCameraController
{
public:
	CFPSCameraCtrller(void);
	~CFPSCameraCtrller(void);

public:
	//更新
	CtrlState	_DoCameraCtrl( uint32 uCurTime,CCamera* pCamera, CSceneQuery* pQuery );

protected:
	//属性
	//屏幕中心点 视口宽高
	int m_CenterX;
	int m_CenterY;
	float m_ViewWidth;
	float m_ViewHeight;
	//当前值
	CVector3f m_Pos;
	CVector3f m_Dir;
	//时间
	uint32 m_LastTime;
	//速度 每次移动米或者转动度
	float	m_MoveSpeed;
	float	m_XRate;//像素度
	float	m_YRate;
	//角度
	float	m_MaxYAngle;
	float	m_MinYAngle;
	float	m_XAngle;
	float	m_YAngle;
	float	m_XPixel;
	float	m_YPixel;
	//
	bool	m_UpdateUnit;
	FPS_STATE m_State;

public:
	//属性行为控制
	void UpdateCenterPos(int cx,int cy);
	//初始化角度
	void SetXAngle(float _xAngle);
	void SetYAngle(float _yAngle);
	float GetAngleX();
	float GetAngleY();

	//每像素多少度 控制转动速度
	void SetMoveSpeed(float _moveSpeed);
	void SetXRate(float _xRate);
	void SetYRate(float _yRate);
	void SetXPixel(float _xPixel);
	void SetYPixel(float _yPixel);
	float GetMoveSpeed();
	float GetXRate();
	float GetYRate();
	//
	void SetPostion(float x,float y,float z);
	void SetState(FPS_STATE _state);

};

inline void CFPSCameraCtrller::UpdateCenterPos(int cx,int cy)
{
	m_CenterX=cx;
	m_CenterY=cy;
}

inline void CFPSCameraCtrller::SetPostion(float x,float y,float z)
{
	m_Pos.x=x;
	m_Pos.y=y;
	m_Pos.z=z;
}

inline void CFPSCameraCtrller::SetMoveSpeed(float _moveSpeed)
{
	m_MoveSpeed=_moveSpeed;
}

inline void CFPSCameraCtrller::SetXAngle(float _xAngle)
{
	m_XAngle=_xAngle;
}

inline void CFPSCameraCtrller::SetYAngle(float _yAngle)
{
	m_YAngle=_yAngle;
}

inline void CFPSCameraCtrller::SetXRate(float _xRate)
{
	m_XRate=_xRate;
}

inline void CFPSCameraCtrller::SetYRate(float _yRate)
{
	m_YRate=_yRate;
}

inline void CFPSCameraCtrller::SetXPixel(float _xPixel)
{
	m_XPixel=_xPixel;
}

inline void CFPSCameraCtrller::SetYPixel(float _yPixel)
{
	m_YPixel=_yPixel;
}

inline float CFPSCameraCtrller::GetAngleX()
{
	return m_XAngle;
}

inline float CFPSCameraCtrller::GetAngleY()
{
	return m_YAngle;
}

inline float CFPSCameraCtrller::GetMoveSpeed()
{
	return m_MoveSpeed;
}

inline float CFPSCameraCtrller::GetXRate()
{
	return m_XRate;
}

inline float CFPSCameraCtrller::GetYRate()
{
	return m_YRate;
}


//!namespace
}
